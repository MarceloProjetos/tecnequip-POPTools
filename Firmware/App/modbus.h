#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "lpc_types.h"

#include "rs485.h"
#include "rs232.h"
#include "modbus_rtu.h"
#include "modbus_tcp.h"

volatile int MODBUS_REGISTER[32] __attribute__((weak));

extern volatile unsigned char MODBUS_MASTER; // __attribute__((weak)) = 0;

unsigned int Modbus_ReadCoils(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_ReadDiscreteInputs(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_ReadHoldingRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_ReadInputRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_WriteSingleCoil(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_WriteSingleRegister(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_WriteMultipleCoils(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_WriteMultipleRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_MaskWriteRegister(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_RWMultipleRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus_ReadExceptionStatus(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);

unsigned int Modbus232_ReadHoldingRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus232_WriteSingleRegister(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
unsigned int Modbus232_WriteMultipleRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);

/******************************************************************************
* Prototipos de Funções
******************************************************************************/

void 		 Modbus_Init(void);
unsigned int Modbus_Request(unsigned char * buffer, unsigned int sz);
void Modbus_Send(unsigned char id,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value);


#endif

