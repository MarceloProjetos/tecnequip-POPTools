/*****************************************************************************
 *   da.c:  Digital to Analog functions
 *
 *   Copyright(C) 2010, Tecnequip Tecnologia Ltda.
 *   All rights reserved.
 *
 *   History
 *   2010.04.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "lpc17xx.h"
#include <string.h>
#include "i2c.h"

#define TRUE	1
#define FALSE	0

extern volatile unsigned int timer0_counter;

extern volatile unsigned int I2CCount;
extern volatile unsigned char I2CMasterBuffer[BUFSIZE];
extern volatile unsigned int I2CCmd, I2CMasterState;
extern volatile unsigned int I2CReadLength, I2CWriteLength;

void DAC_Init(void)
{
  //if ( I2CInitFast( (unsigned int)I2CMASTER ) == FALSE )  /* initialize I2c */
  if ( I2CInitFast( (unsigned int)I2CMASTER ) == FALSE )  /* initialize I2c */
    return;

  // Configure XTR300 I/Os
  PINCON->PINSEL3 &= ~0x00FC0000;  /* set PIO1.25  -  PIO1.27 to GPIO */
  PINCON->PINSEL7 &= ~0x003C0000;  /* set PIO3.25 and PIO3.26 to GPIO */

  GPIO1->FIODIR   |=  0x08000000;  /* set PIO1.27 to output */
  GPIO1->FIOCLR   |=  0x08000000;  /* set PIO1.27 to low  */

  GPIO3->FIODIR   |=  0x02000000;  /* set PIO3.25 to output */
  GPIO3->FIOSET   |=  0x02000000;  /* set PIO3.25 to high */
}

unsigned int DAC_current_value = 0;

void DAC_Write(unsigned int val)
{
  // Checa estouro do D/A
  DAC_current_value = val >= (1 << 12) ? (1 << 12) - 1 : val;

  memset((void *)I2CMasterBuffer, 0, BUFSIZE);

  I2CWriteLength = 3;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = 0x98;
  I2CMasterBuffer[1] = (DAC_current_value>>8) & 0x0F;
  I2CMasterBuffer[2] = (DAC_current_value>>0) & 0xFF;
  I2CEngine();
}

unsigned int DAC_Read(void)
{
  return DAC_current_value;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
