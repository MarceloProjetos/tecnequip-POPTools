#include "eeprom_24AA02.h"

void E2P_Init(void)
{

}

int E2P_Busy(void)
{
	return E2P_24AA02_Busy();
}

void E2P_Read(void *data, unsigned char address, int size)
{
	E2P_24AA02_Read(data, address, size);
}

void E2P_Write(void *data, unsigned char address, int size)
{
	E2P_24AA02_Write(data, address, size);
}
