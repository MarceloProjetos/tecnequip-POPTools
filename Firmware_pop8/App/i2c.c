/*****************************************************************************
 *   i2c.c:  I2C C file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "chip.h"
#include "i2c.h"

#include <string.h>

#define TRUE	1
#define FALSE	0

unsigned char I2CMasterBuffer[BUFSIZE];
unsigned int I2CReadLength;
unsigned int I2CWriteLength;

/*****************************************************************************
** Function name:		I2CInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**				interrupt handler was not installed correctly
**
*****************************************************************************/
unsigned int I2C_Init()
{
	uint32_t freq = 100000;

	Chip_I2C_Init(0);

	// Valor de configuracao dos GPIOs: com pull-up e histerese
	uint32_t gpio_mode = IOCON_MODE_PULLUP | IOCON_HYS_EN;

	// Configuracao dos I/Os
	Chip_IOCON_PinMux(LPC_IOCON, 0, 27, gpio_mode, IOCON_FUNC1); // P0.27: SDA
	Chip_IOCON_PinMux(LPC_IOCON, 0, 28, gpio_mode, IOCON_FUNC1); // P0.28: SCL

	Chip_I2C_SetClockRate(0, freq); // 100 kHz

	return( TRUE );
}

/*****************************************************************************
** Function name:		I2CEngine
**
** Descriptions:		The routine to complete a I2C transaction
**				from start to stop. All the intermitten
**				steps are handled in the interrupt handler.
**				Before this routine is called, the read
**				length, write length, I2C master buffer,
**				and I2C command fields need to be filled.
**				see i2cmst.c for more details.
**
** parameters:			None
** Returned value:		true or false, return false only if the
**				start condition can never be generated and
**				timed out.
**
*****************************************************************************/
unsigned int I2C_Engine( void )
{
	uint8_t rxbuf[BUFSIZE], txbuf[BUFSIZE];

	I2C_XFER_T xfer;

	xfer.slaveAddr = I2CMasterBuffer[0] >> 1;

	xfer.txBuff = txbuf;
	xfer.txSz   = I2CWriteLength - 1;

	xfer.rxBuff = rxbuf;
	xfer.rxSz   = I2CReadLength;

	memcpy(txbuf, I2CMasterBuffer + 1, xfer.txSz);

	Chip_I2C_MasterTransfer(0, &xfer);

	memcpy(I2CMasterBuffer, rxbuf, I2CReadLength);

	return ( TRUE );
}

/******************************************************************************
**                            End Of File
******************************************************************************/

