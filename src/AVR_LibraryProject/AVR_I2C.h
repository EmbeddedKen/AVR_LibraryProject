//I2C Communication Protocol Using AVR TWI (Two-Wire)
//By Kenneth Vorseth, December 2017

#ifndef AVR_I2C_H
#define AVR_I2C_H
#include "AVR_Core.h"

//AVR I2C/TWI Definitions
#define FREQ_SCL      100000L //100Khz (Normal-Mode)
#define I2C_Prescaler 0	      //No Prescaler to Apply
#define I2C_START     0xA4
#define I2C_STOP      0x94
#define I2C_ACK	      0xC4
#define I2C_NACK      0x84
#define I2C_SEND      0x84
#define I2C_READY     (TWCR & 0x80)
#define I2C_STATUS    (TWSR & 0xF8)

//<Initialize I2C Master>
void I2C_MasterInit()
{
	//Set TWSR: I2C Prescaler
	TWSR = I2C_Prescaler;
	//Set TWBR: Baud Rate for I2C (SCL Line)
	TWBR = ((FREQ_CPU/FREQ_SCL)-16)/2;
}

//<Looks for I2C Device, Returns Boolean>
byte I2C_Detect(byte address)
{
	//Set TWCR: Send Start Condition
	TWCR = I2C_START;
	//Wait Until I2C (TWI) Unit is Ready
	while (!I2C_READY) {}
	//Set TWDR: Load Device's Address
	TWDR = address;
	//Set TWCR: Flag for Transmission of Address
	TWCR = I2C_SEND;
	//Wait Until I2C (TWI) Unit is Ready
	while (!I2C_READY) {}
	//Return True if Device Was Found on I2C Bus
	return (I2C_STATUS == 0x18);
}

//<Terminations Communication to Device>
void I2C_Terminate()
{
	//Set TWCR: Set to Stop Communication
	TWCR = I2C_STOP;
}

//<Writes a Single Byte to I2C Bus>
byte I2C_Write (byte data)
{
	//Set TWDR: Assign Data Buffer to Be Sent
	TWDR = data;
	//Set TWCR: Flag for Transmission of Byte
	TWCR = I2C_SEND;
	//Wait Until I2C (TWI) Unit is Ready
	while (!I2C_READY) {}
	//Return True if ACKNOWLEDGE Has Been Received
	return (I2C_STATUS == 0x28);
}

//<Reads from I2C Bus, With Expectation of Reading More>
byte I2C_ReadACK()
{
	//Set TWCR: Flag for Reading More Data
	TWCR = I2C_ACK;
	//Wait Until I2C (TWI) Unit is Ready
	while (!I2C_READY) {}
	//Return Read Data from I2C Bus
	return TWDR;
}

//<Reads from I2C Bus, With Expectation of Ending Transmissions>
byte I2C_ReadNACK()
{
	//Set TWCR: Flag for End of Reading Data
	TWCR = I2C_NACK;
	//Wait Until I2C (TWI) Unit is Ready
	while (!I2C_READY) {}
	//Return Final Data from Transmission
	return TWDR;
}

//<Writes a Single Byte to I2C Device>
void I2C_WriteByte(byte address, byte data)
{
	//Begin Communication with an I2C Device
	if (I2C_Detect(address))
	{
		//If Device Responded, Write Byte
		I2C_Write(data);
		I2C_Terminate();
	}
}

//<Writes to an I2C Device's Register>
void I2C_WriteRegister(byte address, byte deviceReg, byte data)
{
	//Begin Communication with an I2C Device
	if (I2C_Detect(address))
	{
		//If Device Responded, Write to Register
		I2C_Write(deviceReg);
		I2C_Write(data);
		I2C_Terminate();
	}
}

byte I2C_ReadRegister(byte address, byte deviceRegister)
{
	//Begin Communication with an I2C Device
	if (I2C_Detect(address))
	{
		//If Device Responded, Write Register to Read
		I2C_Write(deviceRegister);
		//Restart as a Read Operation, Read Register
		I2C_Detect(address + 1);
		byte recData = I2C_ReadNACK();
		I2C_Terminate();
		//Return Register Data from Transmission
		return recData;
	} else { return -1; }
}

#endif