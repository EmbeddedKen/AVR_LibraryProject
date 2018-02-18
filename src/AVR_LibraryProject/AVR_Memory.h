//AVR Dynamic Memory and Allocation Management
//By Kenneth Vorseth
#ifndef AVR_MEMORY_H
#define AVR_MEMORY_H
#include "AVR_Core.h"

/*Dimensional Matrix for Memory Allocation			  
   (C)[A]0  1  2  3  4  5  6  7
   [B]
	0    0  1  2  3  4  5  6  7  }
	1    8  9  10 11 12 13 14 15 } Total Allocatable Bytes Per Heap Block:
	2    16 17 18 19 20 21 22 23 } [256B]
	3    24 25 26 27 28 29 30 31 }
	4    32 33 34 35 36 37 38 39 } Block Hierarchy:
	5    40 41 42 43 44 45 46 47 } [Block Size 259B]
	6    48 49 50 51 52 53 54 55 } [A-Use 1B|B-Use 1B|C-Use 1B] [Block Data... 256B]
	7    56 57 58 59 60 61 62 63 }

	3-Dimensions: [A] [B] [C] = 8 x 8 x 4

	Byte Address
	|-A----B----C--|  Bit Clear (0) = Free Memory
	|[000][000][00]|  Bit Set   (1) = Used Memory
*/

//Renamed Types for Convenience
typedef uint16_t block; //Typically a Pointer to Beginning of Block
typedef uint8_t  mem;	//Never a Pointer, Block-Memory Address Format
//Offset of Named Memory Locations Per Block
#define CTRL_A		0x000
#define CTRL_B		0x001
#define CTRL_C		0x002
#define BLOCK_END   0x102

//<Initializes a 259-Byte [256-Free] Block Starting at Specified Address>
void MEM_BlockAlloc(block* blockAddress)
{
	//Initializes 3-Byte Control Sector
	blockAddress[CTRL_A] = 0x00; // [xxxx.xxxx]
	blockAddress[CTRL_B] = 0x00; // [xxxx.xxxx]
	blockAddress[CTRL_C] = 0xF0; // [1111.xxxx] Last Four Bits Mark Valid Block
}

//<Allocates 1 Byte in Specified Block>
mem MEM_Alloc(block* bAddress, byte initVal)
{
	//Check to Ensure Block Address Points to a Valid Block
	if ((bAddress[CTRL_C] >> 4) == 0x0F)
	{
		//Algorithm for Searching for Unused Memory Here...
		//.................................................
	} else { return nullptr; } //ERROR: NOT A HEAP BLOCK!!!
}

//<Retrieves 1 Byte Using Block-Memory Address Format>
byte MEM_Retrieve(block* bAddress, mem mAddress) //[00][000][000]
{
	//Check to Ensure Block Address Points to a Valid Block
	if ((bAddress[CTRL_C] >> 4) == 0x0F)
	{
		//Calculate Respective BitMask Identities
		mem mA = !(1 << (mAddress & 0x07));
		mem mB = !(1 << ((mAddress & 0x36) >> 3));
		mem mC = !(1 << ((mAddress & 0xC0) >> 6));
		//Check to Ensure Memory Has Been Marked 'Used'
		if (((bAddress[CTRL_A] & mA) == mA) && ((bAddress[CTRL_B] & mB) == mB) && ((bAddress[CTRL_C] & mC) == mC))
		{
			//Algorithm for Returning for Memory Here...
			//..........................................
		} else { return nullptr; } //ERROR: DATA IS FREE!!!
	}
}

#endif