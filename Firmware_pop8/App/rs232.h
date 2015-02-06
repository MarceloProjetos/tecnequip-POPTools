#ifndef __RS232_H__
#define __RS232_H__

#include "chip.h"

void RS232_Init();
unsigned int RS232_Write(char * buffer, unsigned int size);
unsigned int RS232_Read(char * buffer, unsigned int size);
void RS232_Console(unsigned int cycle);
unsigned char RS232_CRC(char * buffer, unsigned int size);

#endif
