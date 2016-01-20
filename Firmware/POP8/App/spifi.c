#include "spifi.h"

void SPIFI_Init(void)
{
	Chip_SPIFI_Init();

	// Valor de configuracao dos GPIOs: com pull-up e histerese
	uint32_t gpio_mode = IOCON_MODE_PULLUP | IOCON_FASTSLEW_EN;

	// Configuracao dos I/Os
	Chip_IOCON_PinMux(LPC_IOCON, 2,  7, gpio_mode, IOCON_FUNC5); // P2.7 : CS
	Chip_IOCON_PinMux(LPC_IOCON, 0, 22, gpio_mode, IOCON_FUNC5); // P0.22: SCK
	Chip_IOCON_PinMux(LPC_IOCON, 0, 18, gpio_mode, IOCON_FUNC5); // P0.18: MOSI
	Chip_IOCON_PinMux(LPC_IOCON, 0, 17, gpio_mode, IOCON_FUNC5); // P0.17: MISO

	// Configuração do pino WP
	Chip_IOCON_PinMux(LPC_IOCON, 0, 15, gpio_mode, IOCON_FUNC0); // P0.15: WP   - Configurado como GPIO
	Chip_GPIO_SetDir (LPC_GPIO , 0, 15, 1); // Define pino como Saida
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 15, 0); // Configura nivel baixo para desativar Write Protection

	// Configuração do pino HOLD
	Chip_IOCON_PinMux(LPC_IOCON, 0, 16, gpio_mode, IOCON_FUNC0); // P0.16: HOLD - Configurado como GPIO
	Chip_GPIO_SetDir (LPC_GPIO , 0, 16, 1); // Define pino como Saida
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 16, 1); // Configura nivel alto para estar sempre desativado o HOLD

	Chip_Clock_SetSPIFIClockSource(SYSCTL_SPIFICLKSRC_SYSCLK);
	Chip_Clock_SetSPIFIClockDiv(10);

	spifiInit(LPC_SPIFI_BASE, TRUE);

	spifiRegisterFamily(spifi_REG_FAMILY_CommonCommandSet);
	uint32_t memSize = spifiGetHandleMemSize(LPC_SPIFI_BASE);
	if (memSize == 0) {
	/* No device detected, error */
		return;
	}

	uint32_t lmem[64];
	SPIFI_HANDLE_T *pSpifiHandle = spifiInitDevice(lmem, sizeof(lmem), LPC_SPIFI_BASE,	SPIFLASH_BASE_ADDRESS);
	if (pSpifiHandle == NULL) {
		return;
	}

	// Codigo para gravar um dado na memoria.
	uint32_t data_X = 153;
	spifiDevUnlockDevice(pSpifiHandle);
	spifiErase(pSpifiHandle, 0, 1);
	spifiProgram(pSpifiHandle,SPIFLASH_BASE_ADDRESS,&data_X,sizeof(data_X));

	/* turn on memmode on for default */
	// IMPORTANTE!!! Apos ativar o Memory Mode, nenhuma operacao (escrita, erase...) pode ser executada na flash ou o Memory Mode sera desativado
	// e acessos posteriores a regiao de memoria mapeada vao gerar execao na cpu!
	spifiDevSetMemMode(pSpifiHandle, true);

	uint32_t data_R = *(uint32_t *)(SPIFLASH_BASE_ADDRESS);
}
