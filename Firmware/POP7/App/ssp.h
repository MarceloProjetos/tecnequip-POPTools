#ifndef __SSP_H__
#define __SSP_H__

#include "lpc17xx.h"

//SSP Status register
#define SSPSR_TNF     1 << 1
#define SSPSR_RNE     1 << 2
#define SSPSR_BSY     1 << 4

#define OUTPUT_CMD_CONTROL  (0 << 2)

#define LSR_RDR         0x01
#define LSR_OE          0x02
#define LSR_PE          0x04
#define LSR_FE          0x08
#define LSR_BI          0x10
#define LSR_THRE        0x20
#define LSR_TEMT        0x40
#define LSR_RXFE        0x80

void SSP_Init();
unsigned char SSP_Write(unsigned char * buffer, unsigned int size);
unsigned char SSP_Read(unsigned char * buffer, unsigned int size);

#endif
