//AVR Dynamic Memory and Allocation Management
//By Kenneth Vorseth

#ifndef AVR_HEAPMEMORY_H
#define AVR_HEAPMEMORY_H
#include "AVR_Core.h"

/*
	Memory Management and Structure in SRAM
	[- 32 Memory Block Heap (288 Total Bytes, 256 Data Bytes) -]

	Virtual Address: [xxx][xxxxxx] : [Byte# (3 Bit) | Block# (5 Bit)]

	|-Memory Block (9 Bytes)--------------------------------------------------------------------|
	|-Ctrl (1B)|-Data Block (8 Bytes)-----------------------------------------------------------|
	|Ctrl_Reg--|-Byte0-----Byte1-----Byte2-----Byte3-----Byte4-----Byte5-----Byte6-----Byte7----|
 	|----------|-0---------0---------0---------0---------0---------0---------0---------0--------|
	|[xxxxxxxx]|[xxxxxxxx][xxxxxxxx][xxxxxxxx][xxxxxxxx][xxxxxxxx][xxxxxxxx][xxxxxxxx][xxxxxxxx]|

	Key:[bk = block#, bt = byte#, vA = virtualAddress, val = value]
*/

//Bit-wise Manipulation Macros                
#define MACRO_MakeVirtual(bk, bt) (bk | (bt << 5))			//Macro for Forming Virtual Address
#define MACRO_SplitBlock(vA)      ((vA & 0x1F))			    //Macro for Splitting Block# from Virtual Address
#define MACRO_SplitByte(vA)       ((vA & 0xE0) >> 5)		//Macro for Splitting Byte # from Virtual Address
#define MACRO_OffsetCtrl(bk)      (bk * 9)				    //Macro for Calculating Offset from Heap for Ctrl
#define MACRO_OffsetData(bk, bt)  ((bk * 9) + bt + 1)	    //Macro for Calculating Offset from Heap for Data
#define MACRO_CheckBit(val, bit)  ((val & (1 << bit)) != 0) //Macro for Checking Bit in Integer of N-Length
//Defined Heap Offset Constants
#define HEAP_START	0x000
#define HEAP_END	0x11F
#define BLOCK_SIZE  0x009
//Defined Block Control Register Constants
#define BLOCK_USED  0x00
#define BLOCK_FREE  0xFF
#define BYTE_FREE   0x01
#define BYTE_USED   0x00

//<Allocates a New Heap, By Setting All Control Registers to Free>
void MEM_AllocHeap(byte* heapPtr)
{
	//For Each Control Register in the Heap...
	for (int b = HEAP_START; b <= HEAP_END; b += BLOCK_SIZE)
	{
		//Mark 8-Bytes Represented in Control Register as Free
		heapPtr[b] = 0xFF;
	}
}

//<Allocates a Single Byte into the Heap, Returns Virtual Address>
byte MEM_Alloc(byte* heapPtr)
{
	//[Utilizes First-Found Method for Allocation]

	//For Each Control Register in the Heap...
	byte blockNum = 0;
	for (int block = HEAP_START; block <= HEAP_END; block += BLOCK_SIZE)
	{
		//Retrieve Value of Control Register
		byte ctrlReg = heapPtr[block];
		//Skip Iterating Over Bits if All Bits Used (Performance Increase)
		if (ctrlReg != BLOCK_USED)
		{
			//Check for First Free Bit in Control Register...
			for (int byte = 0; byte <= 7; byte++)
			{
				//If Bit Free in Control Register, Allocate Byte
				if (MACRO_CheckBit(ctrlReg, byte)) 
				{ 
					//Mark Bit in Control Register as Used
					heapPtr[block] &= ~(1<<byte); 
					//Initialize Allocated Byte to 0xEE (User-Uninitialized)
					heapPtr[MACRO_OffsetData(blockNum, byte)] = 0xEE;
					//Concatenate and Return Virtual Address for Allocated Byte
					return MACRO_MakeVirtual(blockNum, byte);
				}
				//Update Value of Control Register for Next Loop
				ctrlReg = heapPtr[block];
			}
		}
		//Increment Block# (For Determining Virtual Address)
		blockNum++;
	}
	//Failure in Allocation: ErrorCode: 0x00
	return 0x00;
}
//<Frees Allocation of Byte Specified by Virtual Address in Heap>
void MEM_Free(byte *heapPtr, byte virtualAddr)
{
	//Set Bit in Block Control Register to Mark as Free
	heapPtr[MACRO_SplitBlock(virtualAddr) * 9] |= (1<<MACRO_SplitByte(virtualAddr)); 
}
//<Frees and Overwrites Allocated Byte to Zero>
void MEM_Erase(byte *heapPtr, byte virtualAddr)
{
	//Decipher Virtual Address, Split into Block#/Byte# Subparts
	byte blockNum = MACRO_SplitBlock(virtualAddr);
	byte byteNum  = MACRO_SplitByte(virtualAddr);
	//Overwrite Byte to 0x00 (Clear/Erase)
	heapPtr[MACRO_OffsetData(blockNum, byteNum)] = 0x00;
	//Set Bit in Block Control Register to Mark as Free
	heapPtr[MACRO_SplitBlock(virtualAddr) * 9] |= (1<<MACRO_SplitByte(virtualAddr));
}

//<Performs an Checked Write Operation using Virtual Address, Returns True if Successful>
byte MEM_SafeWrite(byte* heapPtr, byte virtualAddr, byte writeVal)
{
	//Decipher Virtual Address, Split into Block#/Byte# Subparts
	byte blockNum = MACRO_SplitBlock(virtualAddr);
	byte byteNum  = MACRO_SplitByte(virtualAddr);
	//Determine Whether Virtual Address is Valid- Memory is Used
	if (!MACRO_CheckBit(blockNum * 9, byteNum))
	{
		//Write to Virtual Address in Designated Heap
		heapPtr[MACRO_OffsetData(blockNum, byteNum)] = writeVal;
		//Return Op Successful
		return true;
	}
	//Return Op Failure
	return false;
}
//<Performs an Checked Write Operation using Virtual Address, Returns True if Successful>
byte MEM_SafeRead(byte* heapPtr, byte virtualAddr)
{
	//Decipher Virtual Address, Split into Block#/Byte# Subparts
	byte blockNum = MACRO_SplitBlock(virtualAddr);
	byte byteNum  = MACRO_SplitByte(virtualAddr);
	//Determine Whether Virtual Address is Valid- Memory is Used
	if (!MACRO_CheckBit(blockNum * 9, byteNum))
	{
		//Write to Virtual Address in Designated Heap
		return heapPtr[MACRO_OffsetData(blockNum, byteNum)];
	}
	//Return Op Failure
	return false;
}

//<Performs an Unchecked Fast Write Operation using Virtual Address>
void MEM_FastWrite(byte* heapPtr, byte virtualAddr, byte writeVal)
{
	//Decipher Virtual Address, Split into Block#/Byte# Subparts
	byte blockNum = MACRO_SplitBlock(virtualAddr);
	byte byteNum  = MACRO_SplitByte(virtualAddr);
	//Write to Virtual Address in Designated Heap
	heapPtr[MACRO_OffsetData(blockNum, byteNum)] = writeVal;
}
//<Performs an Unchecked Fast Read Operation using Virtual Address>
byte MEM_FastRead(byte* heapPtr, byte virtualAddr)
{
	//Decipher Virtual Address, Split into Block#/Byte# Subparts
	byte blockNum = MACRO_SplitBlock(virtualAddr);
	byte byteNum  = MACRO_SplitByte(virtualAddr);
	//Write to Virtual Address in Designated Heap
	return heapPtr[MACRO_OffsetData(blockNum, byteNum)];
}

#endif