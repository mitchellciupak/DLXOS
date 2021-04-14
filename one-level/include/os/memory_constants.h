#ifndef	_memory_constants_h_
#define	_memory_constants_h_

//------------------------------------------------
// #define's that you are given:
//------------------------------------------------

// We can read this address in I/O space to figure out how much memory
// is available on the system.
#define	DLX_MEMSIZE_ADDRESS	0xffff0000

// Return values for success and failure of functions
#define MEM_SUCCESS 1
#define MEM_FAIL -1

//--------------------------------------------------------
// Put your constant definitions related to memory here.
// Be sure to prepend any constant names with "MEM_" so
// that the grader knows they are defined in this file.

//--------------------------------------------------------
#define MEM_L1FIELD_FIRST_BITNUM 12     //least sig bit in vadd
#define MEM_MAX_VIRTUAL_ADDRESS 1048575 //maximum virtual memory size (1024KB - 1 = 1048575 Bytes).
#define MEM_MAX_SIZE 2097152            //maximum physical memory size (2MB = 2097152 Bytes).

#define MEM_PTE_READONLY 0b100
#define MEM_PTE_DIRTY    0b010
#define MEM_PTE_VALID    0b001
#define MEM_PTE_MASK4PAGE (~(MEM_PTE_READONLY | MEM_PTE_DIRTY | MEM_PTE_VALID)) //Attempting to get 0b000

#define MEM_L1PTSIZE 1048576 >> 12                      //TODO - Convert to Calculation of Above, Page Size is 4KB = 4096
#define MEM_PAGESIZE (0x1 << MEM_L1FIELD_FIRST_BITNUM)  //Used in Memory.c
#define MEM_ADDRESS_OFFSET_MASK (MEM_PAGESIZE - 1)      //Used in memory.c
#define MEM_SIZE_OF_ONE_PAGE 32 //TODO - convert to summ of above
#define MEM_FREEMAP_SIZE (MEM_MAX_SIZE >> MEM_L1FIELD_FIRST_BITNUM) / MEM_SIZE_OF_ONE_PAGE //TODO - double chekc
#endif	// _memory_constants_h_
