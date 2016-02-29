
#ifndef __CAN_H__
#define __CAN_H__

#include "chip.h"

#define SERIAL_BUFFER_SIZE 64

void CAN_Init();
unsigned int CAN_Write(unsigned int id, unsigned short int length, volatile uint8_t * value);
unsigned int CAN_Read(unsigned int id, unsigned short int length, volatile uint8_t * value);
unsigned int CAN_ReadChar(unsigned char * buffer);
void CAN_Config(int baudrate);
void CAN_Handler (unsigned int cycle);

#endif
