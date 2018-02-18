//8-Bit Serial-In Shift Register Protocol (SN74LS164N Verified)
//By Kenneth Vorseth, December 2017

#ifndef AVR_SHIFTREG_H
#define AVR_SHIFTREG_H
#include "AVR_Core.h"

//NOTE: On SN74LS164N, there are A and B Serial Data Lines. Only one is needed, pull the other one up to 5v!

//<Write to Shift Register, PortD-Bound>
void SR_SerialWrite(byte clkPinNum, byte dataPinNum, byte data)
{
	//Write Each Bit on the Rising Edge of CLK
	for (int b = 0; b < 8; b++)
	{
		//Determine Whether Current Bit is Set/Clear
		byte dataBit = (data >> b) & 0x01;
		//Output Data Line State for this Pulse Cycle
		if (dataBit) { PORTD |= (1 << dataPinNum); }
		else { PORTD &= !(1 << dataPinNum); }
		//Bring CLK Line High
		PORTD |= (1 << clkPinNum);
		//Bring CLK Line Low
		PORTD &= !(1 << clkPinNum);
	}
}

#endif