#include <string.h>
#include "i2c.h"

extern volatile unsigned int I2CCount;
extern volatile unsigned char I2CMasterBuffer[BUFSIZE];
extern volatile unsigned int I2CCmd, I2CMasterState;
extern volatile unsigned int I2CReadLength, I2CWriteLength;

void E2P_24AA02_Init(void)
{

}

int timeout = 0;

int E2P_24AA02_Busy(void)
{
	return timeout++ < 100;
}

unsigned int E2P_24AA02_Write(void *data, unsigned char address, int size)
{
	timeout = 0;
	memset((void *)I2CMasterBuffer, 0, BUFSIZE);

	I2CWriteLength = 2+size;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = E2PROM_ADDR | E2PROM_CMD_WRITE;
	I2CMasterBuffer[1] = address;

	memcpy((void *)&I2CMasterBuffer[2], data, size);

	return I2C_Engine();
}

unsigned int E2P_24AA02_Read(void *data, unsigned char address, int size)
{
	unsigned int ret;
	timeout = 0;
	memset((void *)I2CMasterBuffer, 0, BUFSIZE);

	I2CWriteLength = 2;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = E2PROM_ADDR | E2PROM_CMD_WRITE;
	I2CMasterBuffer[1] = address;
	ret = I2C_Engine();

	if(ret) {
		I2CWriteLength = 1;
		I2CReadLength = size;
		I2CMasterBuffer[0] = E2PROM_ADDR | E2PROM_CMD_READ;
		ret = I2C_Engine();

		memcpy(data, (void*)&I2CMasterBuffer[3], size);
	} else {
		memset(data, 0, size);
	}

	return ret;
}
