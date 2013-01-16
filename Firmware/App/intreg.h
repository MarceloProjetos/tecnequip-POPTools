#ifndef __INTREG_H__
#define __INTREG_H__

// Faixas de Registradores Internos
#define INTREG_START          0
#define INTREG_MODBUS_START   (INTREG_START)
#define INTREG_MODBUS_SIZE    32
#define INTREG_DATETIME_START (INTREG_MODBUS_START + INTREG_MODBUS_SIZE)
#define INTREG_DATETIME_SIZE  5
#define INTREG_SIZE           (INTREG_MODBUS_SIZE + INTREG_DATETIME_SIZE)

// Checa se a faixa de registradores eh valida
int IntReg_CheckBounds(int start, int quant);

// Le o valor do registrador
int IntReg_Read(int reg);

// Escreve o valor no registrador
void IntReg_Write(int reg, int val);

#endif
