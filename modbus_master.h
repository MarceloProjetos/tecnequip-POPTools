#ifndef MODBUS_MASTER_H
#define MODBUS_MASTER_H

#include <modbus_rtu.h>

extern struct MODBUS_Device MBDev_TCP;
extern struct MODBUS_Device MBDev_Serial;

bool OpenCOMPort (unsigned int iPort, DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits);
void CloseCOMPort(void);

void Init_MBDev(void);

#endif
