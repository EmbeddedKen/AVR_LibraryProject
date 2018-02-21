//AVR Microcontroller Library Project [Test Suite]
//By Kenneth Vorseth, December 2017
#include "AVR_Core.h"
#include "AVR_I2C.h"
#include "AVR_ShiftReg.h"
#include "AVR_HeapMemory.h"  //See this file for Data Structs/Algorithms!

//<Program Entry for Simulator/Chip>
int main(void)
{
	//Initialize a New Heap of Dynamic Memory
	//Place A Heap Block at 0x0200 (Address: 512)
	byte* heap = (byte*)0x0200;						//[UNIT TESTING FOR HEAP MEMORY MODULE- NOTES FOR EXPECTED IRAM VALUES]
	MEM_AllocHeap(heap);							//Checks AllocHeap(), at 0x0200, Should See Ctrl Registers Set to 0xFF
	//Initialize an Array of New Bytes in Heap
	byte data[12];
	for (int i = 0; i < 12; i++) 
	{
		data[i] = MEM_Alloc(heap);					//Checks Alloc(), Should See Twelve New Pieces of Allocated Memory (0xEE)
	}
	//Change First Two Entries
	MEM_FastWrite(heap, data[0], 0xAA);				//Checks FastWrite(), Should See First Two Elements Set to 0xAA
	MEM_FastWrite(heap, data[1], 0xAA);				//...
	//Read Entries (ReadVal Used for Debugging)
	byte readVal = MEM_FastRead(heap, data[1]);		//Checks FastRead(), Should See Return Value of 0xAA
	readVal = MEM_FastRead(heap, 0x03);				//Checks FastRead(), Should See Return Value of (Free Data)	
	//Read Entries (Safely)
	readVal = MEM_SafeRead(heap, data[1]);			//Checks SafeRead(), Should See Return Value of 0xAA
	readVal = MEM_SafeRead(heap, 0x03);				//Checks SafeRead(), Should See Failure Return Value (Reading Freed Data)
	//Free First Two Entries
	MEM_Free(heap, data[0]);						//Checks MEM_Free(), Should See Ctrl Registers Change for Element 0-1
	MEM_Free(heap, data[1]);						//... However, the Memory Values Themselves Should Remain UNCHANGED
	//Free Array of Bytes from Heap
	for (int i = 2; i < 12; i++) 
	{ 
		MEM_Erase(heap, data[i]);					//Checks MEM_Erase(), Should See Ctrl Registers Change for Element 2-12 
	}												//... However, the Memory Values should be OVERWRITTEN with 0x00

	//Create a Second Heap, at 0x400
	byte* heapB = (byte*)0x0400;
	MEM_AllocHeap(heapB);							//Checks AllocHeap(), Should See Second Heap Created at 0x400

	//<Program Loop>
    while (1) 
    {
		//Best for the AVR Platform to Stay in a Loop For Program Termination (No HALT Instruction on CPU)
		//<Program Complete>
    }
}

