/*****************************************************************************
 *   i2c.h:  Header file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __I2C_H
#define __I2C_H

#define BUFSIZE			0x20

#define E2PROM_ADDR			0xA0
#define E2PROM_CMD_READ		0x1
#define E2PROM_CMD_WRITE	0x0

extern unsigned int I2C_Init  ( void );
extern unsigned int I2C_Engine( void );

#endif /* end __I2C_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
