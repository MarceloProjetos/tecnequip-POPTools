#include "expansionPort.h"

#define XP_CMD_WRITE 0X00UL
#define XP_CMD_READ  0x80UL

extern volatile unsigned int I2CCount;
extern volatile unsigned char I2CMasterBuffer[BUFSIZE];
extern volatile unsigned int I2CCmd, I2CMasterState;
extern volatile unsigned int I2CReadLength, I2CWriteLength;

static unsigned int xpAddress;

void XP_Init(void)
{
	XP_SetAddress(0);
}

static int timeout = 0;

int XP_Busy(void)
{
	return timeout++ < 100;
}

void XP_SetAddress(unsigned int address)
{
	xpAddress = address << 1UL;
}

unsigned int XP_Write(unsigned int data)
{
	timeout = 0;
	memset((void *)I2CMasterBuffer, 0, BUFSIZE);

	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = xpAddress | XP_CMD_WRITE;
	I2CMasterBuffer[1] = data & 0xff;

	return I2C_Engine(2);
}

unsigned int XP_Read(void)
{
	unsigned int ret;
	timeout = 0;
	memset((void *)I2CMasterBuffer, 0, BUFSIZE);

	I2CWriteLength = 1;
	I2CReadLength  = 2;
	I2CMasterBuffer[0] = xpAddress | XP_CMD_READ;
	ret = I2C_Engine(2);

	if(ret) {
		ret = atoi((char *)&I2CMasterBuffer[3]);
	} else {
		ret = 0;
	}

	return ret;
}
