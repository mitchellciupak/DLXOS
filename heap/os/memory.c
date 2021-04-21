//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"

// num_pages = size_of_memory / size_of_one_page
#define NPAGES (MEM_SIZE / MEM_PAGESIZE)
static uint32 freemap[NPAGES / 32];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;

//----------------------------------------------------------------------
//
//	This silliness is required because the compiler believes that
//	it can invert a number by subtracting it from zero and subtracting
//	an additional 1.  This works unless you try to negate 0x80000000,
//	which causes an overflow when subtracted from 0.  Simply
//	trying to do an XOR with 0xffffffff results in the same code
//	being emitted.
//
//----------------------------------------------------------------------
static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
int MemoryGetSize() {
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}


//----------------------------------------------------------------------
//
//	MemoryModuleInit
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {
  int i = 0;
  int j = 0;
  int lastos_page = (int)(lastosaddress / MEM_PAGESIZE);
  int lastos_idx = (int)(lastos_page / 32);
  int lastos_bit = lastos_page % 32 + 1;
  int freemap_size = (int)(NPAGES / 32);

  for(i=0; i < freemap_size; i++){
    if(i < lastos_idx) freemap[i] = negativeone;
    else if (i > lastos_idx) freemap[i] = 0;
    else{
      for(j=0; j < lastos_bit; j++){
        freemap[i] |= (1<<j);
      }
    }
  }

  pagestart = lastos_page + 1;
  
  printf("Freemap size = %d\nLastosaddress = %d\n", freemap_size, lastosaddress);
  for(i=0; i < freemap_size; i++){
    printf("%d, ", freemap[i]);
  }
  printf("\n");

}


//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr) {
  int offset = addr & MEM_ADDRESS_OFFSET_MASK;
  int page_num = (addr >> MEM_L1FIELD_FIRST_BITNUM) & MEM_ADDRESS_PAGE_MASK;
  int phys_page = pcb->pagetable[page_num] & MEM_PTE_MASK;

  if(PROCESS_TYPE_SYSTEM & pcb->flags){
    printf("System address 0x%x found\n", addr);
    return addr;
  }
  //printf("Translating user address 0x%x to physical 0x%x\n",addr, phys_page | offset);
  return phys_page | offset;
}


//----------------------------------------------------------------------
//
//	MemoryMoveBetweenSpaces
//
//	Copy data between user and system spaces.  This is done page by
//	page by:
//	* Translating the user address into system space.
//	* Copying all of the data in that page
//	* Repeating until all of the data is copied.
//	A positive direction means the copy goes from system to user
//	space; negative direction means the copy goes from user to system
//	space.
//
//	This routine returns the number of bytes copied.  Note that this
//	may be less than the number requested if there were unmapped pages
//	in the user range.  If this happens, the copy stops at the
//	first unmapped address.
//
//----------------------------------------------------------------------
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir) {
  unsigned char *curUser;         // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;  // Running counter
  int		bytesToCopy;      // Used to compute number of bytes left in page to be copied

  while (n > 0) {
    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char *)MemoryTranslateUserToSystem (pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char *)0) break;

    // Calculate the number of bytes to copy this time.  If we have more bytes
    // to copy than there are left in the current page, we'll have to just copy to the
    // end of the page and then go through the loop again with the next page.
    // In other words, "bytesToCopy" is the minimum of the bytes left on this page 
    // and the total number of bytes left to copy ("n").

    // First, compute number of bytes left in this page.  This is just
    // the total size of a page minus the current offset part of the physical
    // address.  MEM_PAGESIZE should be the size (in bytes) of 1 page of memory.
    // MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the
    // "offset" portion of an address.
    bytesToCopy = MEM_PAGESIZE - ((uint32)curUser & MEM_ADDRESS_OFFSET_MASK);
    
    // Now find minimum of bytes in this page vs. total bytes left to copy
    if (bytesToCopy > n) {
      bytesToCopy = n;
    }

    // Perform the copy.
    if (dir >= 0) {
      bcopy (system, curUser, bytesToCopy);
    } else {
      bcopy (curUser, system, bytesToCopy);
    }

    // Keep track of bytes copied and adjust addresses appropriately.
    n -= bytesToCopy;           // Total number of bytes left to copy
    bytesCopied += bytesToCopy; // Total number of bytes copied thus far
    system += bytesToCopy;      // Current address in system space to copy next bytes from/into
    user += bytesToCopy;        // Current virtual address in user space to copy next bytes from/into
  }
  return (bytesCopied);
}

//----------------------------------------------------------------------
//
//	These two routines copy data between user and system spaces.
//	They call a common routine to do the copying; the only difference
//	between the calls is the actual call to do the copying.  Everything
//	else is identical.
//
//----------------------------------------------------------------------
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever a page fault 
// (better known as a "seg fault" occurs.  If the address that was
// being accessed is on the stack, we need to allocate a new page 
// for the stack.  If it is not on the stack, then this is a legitimate
// seg fault and we should kill the process.  Returns MEM_SUCCESS
// on success, and kills the current process on failure.  Note that
// fault_address is the beginning of the page of the virtual address that 
// caused the page fault, i.e. it is the vaddr with the offset zero-ed
// out.
//
// Note: The existing code is incomplete and only for reference. 
// Feel free to edit.
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB *pcb) {
  int attempt = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  int i = 0xFE;
  if(attempt < pcb->sysStackPtr) ProcessKill();
  while(pcb->pagetable[i] != 0 && i > 0){
    i--;
  }
  if(i==0) ProcessKill();
  pcb->pagetable[i] = MemoryAllocUserPage();
  dbprintf("m", "Page table %d allocated\n", pcb->pagetable[i]);
  return MEM_SUCCESS;
}


//---------------------------------------------------------------------
// You may need to implement the following functions and access them from process.c
// Feel free to edit/remove them
//---------------------------------------------------------------------
int findFreePage(void){
  int i;
  int j;
  for(i=0; i<(NPAGES/32);i++){
    if(freemap[i] != negativeone){
      for(j=0;j<32;j++){
        if(((freemap[i]>>j) & 1) == 0){
          freemap[i] = freemap[i] | (1<<j); // mark as used
          return (j + i*32);
        }
      }
    }
  }
  printf("No free page found!\n");
}

int MemoryAllocUserPage(void) {
  int addr = findFreePage()*MEM_PAGESIZE;
  addr &= ~MEM_ADDRESS_PAGE_MASK;
  return addr | MEM_PTE_VALID;
}

int MemoryAllocSysPage(void) {
  int page = findFreePage();
  int addr = page*MEM_PAGESIZE;
  return addr;
}

uint32 MemorySetupPte (uint32 page) {
  return -1;
}


void MemoryFreePage(uint32 page) {
  int idx;
  int bit;
  page /= MEM_PAGESIZE;
  idx = (int)(page / 32);
  bit = page % 32;
  freemap[idx] &= invert(1<<bit);
}
//---------------------------------------------------------------------
// All heap management work goes here
//---------------------------------------------------------------------

// 2^a
// Only for numbers 0-15
int pow2(int a){
  int i;
  int b = 1;
  a &= 0xF;
  for(i=0; i<a;i++){
    b *= 2;
  }
  return b;
}

// log(a) base 2
int log2(int a){
  int i = 0; 
  int b = 1;
  while(a > b){
    b *= 2;
    i++;
  }
  return i;
}

int is_used(int a){
  return (a>>4 & 1);
}

/*int findIndex(int order, int* heap){
  int idx = 0;
  int left = 1;
  int heap_max = MEM_PAGESIZE / MEM_ORDER0;

  // traverse and look for index
  while(idx < heap_max){
    if(heap[idx] == order){
      if(is_unused(heap[idx])) return idx;
    }
    idx += (pow2(heap[idx]) / MEM_ORDER0);
  }
  return -1;
}*/

void printHeap(int* heap);
int makeIndex(int o, int* heap){
  int idx = 0;
  int heap_max = MEM_PAGESIZE / MEM_ORDER0;
  int tiniest_idx = heap_max;
  int buddy_idx;

  // First look to see if we can find an unused one that's the right size
  // Keep track of the smallest unused idx that's greater order than o
  while(idx < heap_max){
    if(heap[idx] == o && !is_used(heap[idx])){
      return idx;
    }
    if(heap[idx] > o && idx < tiniest_idx && !is_used(heap[idx])) {
    tiniest_idx = idx;
    }
    idx += (pow2(heap[idx]) / 1);
  }

  // Either start at the optimal location, or you're going to have to look all the way
  idx = tiniest_idx < heap_max ? tiniest_idx : 0;

  // Now iterate through and find one to split into a usable one
  while(idx < heap_max && heap[idx] != o){
    if(!is_used(heap[idx]) && heap[idx] > o){
      heap[idx] -= 1;
      buddy_idx = pow2(heap[idx]) + idx;
      heap[buddy_idx] = heap[idx];
    }
    else idx += (pow2(heap[idx]) / 1);
  }
  if(idx == heap_max) return -1;
  return idx;
}

void printHeap(int* heap){
  int heap_max = MEM_PAGESIZE / MEM_ORDER0;
  int i;
  int j;
  int num_cols=8;
  for(i=0;i<num_cols;i++){
    for(j=0;j<heap_max/num_cols;j++){
      printf("%d\t", heap[i*(heap_max/num_cols) + j] & 0xF);
    }
    printf("\n");
  }
}

void fancyPrint(int* heap){
  int heap_max = MEM_PAGESIZE / MEM_ORDER0;
  int i=0;
  int j=0;
  int inuse = 0;

  while(i<heap_max){
    inuse = is_used(heap[i]);
    printf("%d |\t", heap[i] & 0xF);
    for(j=0;j<(pow2(heap[i]));j++){
      printf("%d  ",inuse);
      if(j!=0 && !(j%12)) printf("\n\t");
    }
    printf("\n");
    i += j;
  }
}

// use a binary tree, but use the actual memory space to store information
void* malloc(PCB* pcb, int memsize){
  int order;
  int idx;
  if(memsize > MEM_PAGESIZE || memsize < 0) return NULL;

  order = log2(memsize/MEM_ORDER0);
  idx = makeIndex(order, &pcb->heapNodes);
  if(idx == -1){
    printf("Process (%d) Heap full\n");
    return NULL;
  }
  // mark as inuse
  pcb->heapNodes[idx] |= (1<<4);

  // do printing
  printf("Allocated the block: ");
  printf("order = %d, ", order);
  printf("addr = %d, ", idx*32);
  printf("requested mem size = %d, ", memsize);
  printf("block size = %d\n", pow2(order)*MEM_ORDER0);
  return (pcb->pagetable[pcb->heap_idx] & MEM_PTE_MASK) | (idx*MEM_ORDER0);
}

void shrink(int* heap, int idx){
  
  int order = heap[idx];
  int left = 0;
  int right = 0;
  int buddy;
  int idx_copy = 0;

  // first look left
  while(idx_copy < idx){
    if(heap[idx_copy] == order) left++;
    else left = 0;
    idx_copy += pow2(heap[idx_copy]);
  }
  // If there is an odd number to the left, merge left
  if(left % 2){
    heap[idx] = 0;
    buddy = idx - pow2(order);
    heap[buddy] = order + 1;
    printf("Coalesced buddy nodes ");
    printf("(order = %d, addr = %d, size = %d) & ", order, idx*32, pow2(idx)*32);
    printf("(order = %d, addr = %d, size = %d)\n", order, buddy*32, pow2(order)*32);
    printf("into the parent node ");
    printf("order = %d, addr = %d, size = %d)\n", heap[buddy], buddy*32, pow2(heap[buddy])*32);
    shrink(heap, buddy);
    return;
  }

  // now see if there's one to the right
  buddy = idx + pow2(order);
  if(heap[buddy] == order && !is_used(heap[buddy])){
    heap[buddy] = 0;
    heap[idx] = order + 1;
    printf("Coalesced buddy nodes ");
    printf("(order = %d, addr = %d, size = %d) & ", order, buddy*32, pow2(buddy)*32);
    printf("(order = %d, addr = %d, size = %d)\n", order, idx*32, pow2(order)*32);
    printf("into the parent node ");
    printf("order = %d, addr = %d, size = %d)\n", heap[idx], idx*32, pow2(heap[idx])*32);
        shrink(heap, idx);
  }
  return;
}

int mfree(PCB* pcb, void *ptr){
  int ptr_page = ((int)ptr >> MEM_L1FIELD_FIRST_BITNUM) & 0xFF;
  int offset = (int)ptr & 0xFFF;
  int idx = offset / MEM_ORDER0;
  int order = pcb->heapNodes[idx]& 0xF;
  int heap_loc = (pcb->pagetable[pcb->heap_idx] >> MEM_L1FIELD_FIRST_BITNUM) & 0xFF;
  
  if(!is_used(pcb->heapNodes[idx]) || ptr_page != heap_loc){
    printf("Not a valid heap address\n");
    return -1;
  }
  pcb->heapNodes[idx] &= ~(1<<4);
  shrink(&pcb->heapNodes, idx);
  fancyPrint(&pcb->heapNodes);
  printf("Freed the block: order = %d, addr = %d, size = %d)\n", order, offset, pow2(order)*32);

  return NULL;
}