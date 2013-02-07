#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "lpc_types.h"

#include "rs485.h"
#include "rs232.h"
#include "modbus_rtu.h"
#include "modbus_tcp.h"

volatile int MODBUS_REGISTER[32] __attribute__((weak));

extern volatile unsigned char MODBUS_MASTER; // __attribute__((weak)) = 0;

/******************************************************************************
* Prototipos de Funções
******************************************************************************/

void 		 Modbus_Init(void);
unsigned int Modbus_Request(struct MODBUS_Device *dev, unsigned char * buffer, unsigned int sz);
void Modbus_Send(unsigned char id,
                  unsigned long ip,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value);


#endif

