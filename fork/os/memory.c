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
static uint32 freemap[NPAGES / 32];
static uint32 pagestart;
static int refCtr[NPAGES]; // reference counter for each physical page (addr/4KB)
// static int nfreepages;
// static int freemapmax;

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
    // printf("%d, ", freemap[i]);
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
  int i = 0;
  if(attempt < pcb->sysStackPtr) ProcessKill();
  while(pcb->pagetable[i] != 0 && i > 0){
    i--;
  }
  if(i==0x0) ProcessKill();
  pcb->pagetable[i] = MemoryAllocUserPage();
  dbprintf("m", "Page table %d allocated\n", pcb->pagetable[i]);
  return MEM_SUCCESS;
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever an illegal memory
// access is made
//---------------------------------------------------------------------
int MemoryROPHandler(PCB *pcb) {
  uint32 page = pcb->currentSavedFrame[PROCESS_STACK_FAULT];// && MEM_ADDRESS_OFFSET_MASK;
  int idx = page>>MEM_L1FIELD_FIRST_BITNUM;

  printf("Rop called with page 0x%x\n", page);
  printf("Old page was 0x%x ", pcb->pagetable[idx]);
  pcb->pagetable[idx] = MemoryAllocUserPage();
  printf("and new page for process %d is 0x%x\n", GetCurrentPid(), pcb->pagetable[idx]);
  bcopy((char *)page, (char *)((pcb->pagetable[idx])), MEM_PAGESIZE);
  pcb->pagetable[idx] &= invert(MEM_PTE_READONLY);

  // Update ref counter for old page
  refCtr[page/MEM_PAGESIZE]--;

  return MEM_SUCCESS;
}


//---------------------------------------------------------------------
// You may need to implement the following functions and access them from process.c
// Feel free to edit/remove them
//---------------------------------------------------------------------
int findFreePage(void) {
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
  dbprintf('m',"findFreePage: No free page found!\n");
  return MEM_FAIL;
}

int MemoryAllocUserPage(void) {
  int page = findFreePage();
  int addr = page*MEM_PAGESIZE;
  addr &= invert(MEM_ADDRESS_PAGE_MASK);
  refCtr[page] = 1;
  return addr | MEM_PTE_VALID;
}

int MemoryAllocSysPage(void) {
  int page = findFreePage();
  int addr = page*MEM_PAGESIZE;
  refCtr[page] = 1;
  return addr;
}

uint32 MemorySetupPte (uint32 page) {
  return -1;
}

// Just increments counter for when pages are copied in RealFork
int MemoryUpdateReference(uint32 page){
  refCtr[page/MEM_PAGESIZE]++;
}

void MemoryFreePage(uint32 page) {
  int idx;
  int bit;

  page /= MEM_PAGESIZE;
  // printf("Freeing page 0x%x, %d {%d}!\n", page, refCtr[page], GetCurrentPid());
  if(refCtr[page] > 0){
    refCtr[page]--;
  }
  else{
    // printf("Free\n");
    idx = (int)(page / 32);
    bit = page % 32;
    freemap[idx] &= invert(1<<bit);
  }
}
