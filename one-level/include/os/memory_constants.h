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

//Note:
// * Had to convert decimal values to hex due to error: nondigits in number and not hexadecimal in memory.c

//--------------------------------------------------------
#define MEM_L1FIELD_FIRST_BITNUM 0xC    //least sig bit in vadd
#define MEM_MAX_VIRTUAL_ADDRESS 0xFFFFF //maximum virtual memory size (1024KB - 1 = 1048575 Bytes = 0xFFFFF).
#define MEM_MAX_SIZE 0x200000           //maximum physical memory size (2MB = 2097152 Bytes = 0x200000).

#define MEM_PTE_READONLY 0x4//0b100
#define MEM_PTE_DIRTY    0x2//0b010
#define MEM_PTE_VALID    0x1//0b001
#define MEM_PTE_MASK4PAGE (~(MEM_PTE_READONLY | MEM_PTE_DIRTY | MEM_PTE_VALID)) //Attempting to get 0b000

#define MEM_L1PTSIZE (MEM_MAX_VIRTUAL_ADDRESS + 1) >> MEM_L1FIELD_FIRST_BITNUM //Page Size is 4KB = 4096
#define MEM_PAGESIZE (0x1 << MEM_L1FIELD_FIRST_BITNUM)  //Used in Memory.c
#define MEM_ADDRESS_OFFSET_MASK (MEM_PAGESIZE - 1)      //Used in memory.c
#define MEM_SIZE_OF_ONE_PAGE 0x20 //TODO - convert to summ of above
#define MEM_FREEMAP_SIZE (MEM_MAX_SIZE >> MEM_L1FIELD_FIRST_BITNUM) / MEM_SIZE_OF_ONE_PAGE //TODO - double chekc
#endif	// _memory_constants_h_
