#ifndef __INTREG_H__
#define __INTREG_H__

// Faixas de Registradores Internos
#define INTREG_START          0
#define INTREG_MODBUS_START   (INTREG_START)
#define INTREG_MODBUS_SIZE    32
#define INTREG_DATETIME_START (INTREG_MODBUS_START + INTREG_MODBUS_SIZE)
#define INTREG_DATETIME_SIZE  5
#define INTREG_VARCOUNT_START (INTREG_DATETIME_START + INTREG_DATETIME_SIZE)
#define INTREG_VARCOUNT_SIZE  2
#define INTREG_RESERVED_START (INTREG_VARCOUNT_START + INTREG_VARCOUNT_SIZE)
#define INTREG_RESERVED_SIZE  (100 - INTREG_MODBUS_SIZE - INTREG_DATETIME_SIZE - INTREG_VARCOUNT_SIZE)
#define INTREG_SIZE           (INTREG_MODBUS_SIZE + INTREG_DATETIME_SIZE + INTREG_VARCOUNT_SIZE + INTREG_RESERVED_SIZE)

#define INTREG_USERBIT_START  INTREG_SIZE
#define INTREG_USERBIT_SIZE   50
#define INTREG_USERINT_START  (INTREG_SIZE + INTREG_USERBIT_SIZE)
#define INTREG_USERINT_SIZE   500

// Checa se a faixa de registradores eh valida
int IntReg_CheckBounds(int start, int quant);

// Le o valor do registrador
int IntReg_Read(int reg);

// Escreve o valor no registrador
void IntReg_Write(int reg, int val);

#endif
