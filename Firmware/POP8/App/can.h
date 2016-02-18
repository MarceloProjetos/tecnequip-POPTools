
#ifndef __CAN_H__
#define __CAN_H__

#include "chip.h"

#define SERIAL_BUFFER_SIZE 64

void CAN_Init();
unsigned int CAN_Write(CAN_MSG_T);
unsigned int CAN_Read(CAN_MSG_T);
unsigned int CAN_ReadChar(unsigned char * buffer);
void CAN_Config(int baudrate);
void CAN_Handler (unsigned int cycle);

#endif
