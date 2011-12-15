#ifndef __RS232_H__
#define __RS232_H__

#include "serial.h"

void RS232_Init();
unsigned int RS232_Write(unsigned char * buffer, unsigned int size);
unsigned int RS232_Read(unsigned char * buffer, unsigned int size);

#endif
