//Atmega AVR Core (Needed by Other Setting-Dependent Includes)
//By Kenneth Vorseth, December 2017

#ifndef AVR_CORE_H
#define AVR_CORE_H
#include <avr/io.h>

//Universal Redefined Types
typedef uint8_t byte;
typedef uint16_t word;
#define false	0
#define true	1

//AVR Core Definitions
long FREQ_CPU = 8000000L; //1Mhz
#define OSC_Internal_1Mhz 0 //Enum Replacement
#define OSC_Internal_8Mhz 1

#define F_CPU 8000000L;

void CORE_SetOscillator(byte oscType)
{
	switch (oscType)
	{
		case OSC_Internal_1Mhz:
			asm ("CLI \n");	//Disable Interrupts
			CLKPR = 0b10000000; //Enable CLKPR Change
			CLKPR = 0b00000011; //Set ScaleDivisor to 8
			asm ("SEI \n"); //Enable Interrupts
			FREQ_CPU = 1000000L;
			break;
		case OSC_Internal_8Mhz:
			asm ("CLI \n");	//Disable Interrupts
			CLKPR = 0b10000000; //Enable CLKPR Change
			CLKPR = 0b00000000; //Set ScaleDivisor to 1
			asm ("SEI \n"); //Enable Interrupts
			FREQ_CPU = 8000000L;
			break;
	}
}

void CORE_SyncDelay(const int div)
{
	//Perform a Redundant Delay (Assignment needed to prevent Compiler Optimization from removing it!!!)
	for (long i = 0; i < 2000000 / div; i++) { PORTD = PORTD; }
}


#endif 