#include "ssp.h"

/******************************************************************************
* Comunicação SSP
******************************************************************************/
void SSP_Init()
{
	LPC_SSP_T *pSSP = LPC_SSP1;
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SSP1);

	// Valor de configuracao dos GPIOs: com pull-up e histerese
	uint32_t gpio_mode = IOCON_MODE_PULLUP | IOCON_HYS_EN;

	Chip_SSP_Set_Mode(pSSP, SSP_MODE_MASTER);
	Chip_SSP_SetFormat(pSSP, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA1_CPOL0);
	Chip_SSP_SetBitRate(pSSP, 100000);

	// Configuracao dos I/Os
	Chip_IOCON_PinMux(LPC_IOCON, 0, 7, gpio_mode, IOCON_FUNC2); // P0.7: SCK
	Chip_IOCON_PinMux(LPC_IOCON, 0, 8, gpio_mode, IOCON_FUNC2); // P0.8: MISO
	Chip_IOCON_PinMux(LPC_IOCON, 0, 9, gpio_mode, IOCON_FUNC2); // P0.9: MOSI

	// P0.16: SSEL, gerado por software
	Chip_IOCON_PinMux(LPC_IOCON, 0, 6, gpio_mode, IOCON_FUNC0);
	Chip_GPIO_SetDir (LPC_GPIO , 0, 6, 1); // Define pino como Saida

	Chip_SSP_Enable(pSSP);
}

void SSP_Enable()
{
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 6, false);
}

void SSP_Disable()
{
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 6, true);
}

unsigned char SSP_Write(unsigned char * buffer, unsigned int size)
{
  unsigned int i;

  for (i = 0; i < size; i++ )
  {
    while ( !(LPC_SSP1->SR & SSP_STAT_TNF) );
    LPC_SSP1->DR = *buffer;
    buffer++;
    while ( LPC_SSP1->SR & SSP_STAT_BSY );
  }
  return i;
}

unsigned char SSP_Read(uint8_t * buffer, unsigned int size)
{
  unsigned int i;

  for (i = 0; i < size; i++ )
  {
    while ( !(LPC_SSP1->SR & SSP_STAT_RNE) );
    *buffer = LPC_SSP1->DR;
    buffer++;
  }
  return i;
}
