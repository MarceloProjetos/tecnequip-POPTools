
#ifndef __RS485_H__
#define __RS485_H__

#include "serial.h"

void RS485_Init();
unsigned int RS485_Write(unsigned char * buffer, unsigned int size);
unsigned int RS485_Read(unsigned char * buffer, unsigned int size);
unsigned int RS485_ReadChar(unsigned char * buffer);
void RS485_Config(int baudrate, int bits, int parity, int stopbit);
void RS485_Handler (unsigned int cycle);

#endif
