//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "os/memory_constants.h"
#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"
#include "queue.h"

static uint32 freemap[MEM_FREEMAP_SIZE];
static int nfreepages;
static int whichmap_memalloc;
static int whichpage_memalloc;

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
int MemoryGetSize() {
  // return 1 << 20; //TODO - adviced by RAVI
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}

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
static inline uint32 invert(uint32 n) {
  return (n ^ 0xFFFFFFFF);
}

//----------------------------------------------------------------------
//
//	MemoryModuleInit (Completed)
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {
  int i;

  dbprintf('m', "MemoryModuleInit: Entering\n");
  dbprintf('m', "MemoryModuleInit: PAGE_MAX_MEM_SIZE: %d\n",MemoryGetSize()/MEM_PAGESIZE);
  dbprintf('m', "MemoryModuleInit: PAGE_START: 0x%x\n",(lastosaddress / MEM_PAGESIZE) + 1);
  dbprintf('m', "MemoryModuleInit: NUM_MAP_ENTRIES: %d\n",MemoryGetSize() / MEM_PAGESIZE);

  //setup a freemap for pages
  for (i = 0; i < MEM_FREEMAP_SIZE;i++){
    // dbprintf('m', "MemoryModuleInit: clearing freemap of index %d\n", i);
    freemap[i] = 0;
  }
  nfreepages = 0;

  //mark the ones in use by the operating system as "VALID" or 1
  for (i = (lastosaddress / MEM_PAGESIZE) + 1;i < (MemoryGetSize() / MEM_PAGESIZE);i++){
    freemap[i / 32] |= (1 << i % 32);
    // dbprintf('m', "MemoryModuleInit: setting freemap VALID for freemap[%d] as %d\n", i / 32,freemap[i / 32]);
    nfreepages += 1;
  }

  dbprintf('m', "MemoryModuleInit: Exiting with %d free pages created\n",nfreepages);
}


//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem(PCB* pcb, uint32 addr) { //TODO - Assure Correct Inputs

  int page_num = ((addr & 0xFF000) >> MEM_L1FIELD_FIRST_BITNUM); //Addr is fff-ff-ff0

  // dbprintf('m', "MemoryTranslateUserToSystem: Starting for page of %d\n", page_num);

  //is virtual address grater than possible?
  if (addr > MEM_MAX_VIRTUAL_ADDRESS){ ProcessKill(); }

  // dbprintf('m', "MemoryTranslateUserToSystem: Addr 0x%x, Page: %d,\n", page_num, addr);

  if (pcb->pagetable[page_num] & MEM_PTE_VALID) {

    // dbprintf('m', "MemoryTranslateUserToSystem: Page is valid\n");

    //  //Check for page fault
    if ((pcb->pagetable[page_num] & MEM_PTE_VALID) != MEM_PTE_VALID){

      pcb->currentSavedFrame[PROCESS_STACK_FAULT] = addr;
      if (MemoryPageFaultHandler(pcb) != MEM_SUCCESS) {
        dbprintf('m', "MemoryTranslateUserToSystem: Failing due to Page Fault\n");
        return MEM_FAIL;
      }
    }

    //Return Physical Address
    // dbprintf('m', "MemoryTranslateUserToSystem: Returning 0x%x\n", ((pcb->pagetable[page_num] & MEM_PTE_MASK4PAGE) | (addr & MEM_ADDRESS_OFFSET_MASK)));
    return ((pcb->pagetable[page_num] & MEM_PTE_MASK4PAGE) | (addr & MEM_ADDRESS_OFFSET_MASK));
  }

  //Address exceeds max available
  dbprintf('m', "MemoryTranslateUserToSystem: Failing due to invalid page of %d\n", pcb->pagetable[page_num]);
  ProcessKill();
  return MEM_FAIL;
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
int MemoryMoveBetweenSpaces(PCB* pcb, unsigned char* system, unsigned char* user, int n, int dir) {


  unsigned char* curUser;         // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;          // Running counter
  int		bytesToCopy;              // Used to compute number of bytes left in page to be copied

  // dbprintf('m',"MemoryMoveBetweenSpaces: STARTING\n");

  while (n > 0) {

    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char*)MemoryTranslateUserToSystem(pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char*)0) break;

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
    if (bytesToCopy > n) { bytesToCopy = n; }

    // Perform the copy.
    if (dir >= 0) {
      bcopy(system, curUser, bytesToCopy);
    }
    else { bcopy(curUser, system, bytesToCopy); }

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
int MemoryCopySystemToUser(PCB* pcb, unsigned char* from, unsigned char* to, int n) {
  // dbprintf('m',"MemoryCopySystemToUser: STARTING\n");
  return (MemoryMoveBetweenSpaces(pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem(PCB* pcb, unsigned char* from, unsigned char* to, int n) {
  return (MemoryMoveBetweenSpaces(pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// STUDENT
//---------------------------------------------------------------------

void MemoryPreAlloc(void) {
  whichmap_memalloc = 0;
  whichpage_memalloc = 0;
}

int MemoryAllocPage(void) {

  //Corner Case Check
  if (nfreepages == 0){
    dbprintf('m',"MemoryAllocPage: No More Free Pages: %d\n",whichmap_memalloc);
    return MEM_FAIL;
  }

  dbprintf('m',"MemoryAllocPage: Allocating Starting At: %d\n",whichmap_memalloc);

  //Finding Map (int)
  while(freemap[whichmap_memalloc] == 0){
    // dbprintf('m',"MemoryAllocPage: Searching through freemap %d\n",whichmap_memalloc); //added for bedbuging infinite loop
    whichmap_memalloc++;
    if (whichmap_memalloc >= MEM_FREEMAP_SIZE) {
      whichmap_memalloc = 0;
    }
  }

  //Finding Page (bit)
  for (whichpage_memalloc = 0; (freemap[whichmap_memalloc] & (1 << whichpage_memalloc)) == 0; whichpage_memalloc++) {}

  //Mark Allocated
  freemap[whichmap_memalloc] &= invert(1<<whichpage_memalloc);
  nfreepages -= 1;

  return (uint32 *)(whichmap_memalloc * 32) + whichpage_memalloc;
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
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB* pcb) {
  uint32 createdPage;
  uint32 faddr = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  uint32 addr = pcb->currentSavedFrame[PROCESS_STACK_FAULT] >> MEM_L1FIELD_FIRST_BITNUM;


  dbprintf('m',"MemoryPageFaultHandler: STARTING\n");
  dbprintf('m',"MemoryPageFaultHandler: Has an adress of %d and Stack Pointer address of %d\n", pcb->currentSavedFrame[PROCESS_STACK_FAULT], pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER]);

  //Check for undersized address
  if (faddr < pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER]) { ProcessKill();return MEM_FAIL; }

  //Check for oversided address
  if (addr > (MEM_L1PTSIZE + 1)) {
    ProcessKill();return MEM_FAIL; }

  //Allocate a new page
  createdPage = MemoryAllocPage();

  //Checl for allocation faulure
  if (createdPage == 0){
    ProcessKill();
  }

  pcb->pagetable[faddr >> MEM_L1FIELD_FIRST_BITNUM] = MemorySetupPte(createdPage);
  pcb->npages += 1;
  return MEM_SUCCESS;

}

uint32 MemorySetupPte(uint32 page) {
  //Make Page Valid
  dbprintf('m',"MemorySetupPte: Returing %d\n",((page<<MEM_L1FIELD_FIRST_BITNUM) | MEM_PTE_VALID));
  return ((page<<MEM_L1FIELD_FIRST_BITNUM) | MEM_PTE_VALID);
}

void MemoryFreePage(uint32 page) {
  //Set Freemap Bit to Available or 1
  uint32 bit = page % 32;
  freemap[page / 32] |= (1 << bit);

  //Incrament Number of Free Pages
  nfreepages += 1;
}