/******************************************************************************
* Rotinas de atualização das Entradas/Saidas
******************************************************************************/
#include "chip.h"
#include "gpio.h"
#include "ssp.h"

volatile unsigned int GPIO_OUTPUT = 0;
volatile unsigned int GPIO_INPUT = 0;

unsigned int OUTPUT_FORCED_MASK  = ~0UL; // Saidas forcadas ficam em zero na mascara
unsigned int OUTPUT_FORCED_STATE =  0UL;

volatile char GPIO_INPUT_PORT1 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT2 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT3 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT4 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT5 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT6 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT7 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT8 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT9 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT10 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT11 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT12 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT13 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT14 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT15 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT16 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT17 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT18 __attribute__((weak)) = 0;
volatile char GPIO_INPUT_PORT19 __attribute__((weak)) = 0;

volatile char GPIO_OUTPUT_PORT1 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT2 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT3 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT4 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT5 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT6 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT7 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT8 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT9 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT10 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT11 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT12 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT13 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT14 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT15 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT16 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT17 __attribute__((weak)) = 0;
volatile char GPIO_OUTPUT_PORT18 __attribute__((weak)) = 0;

volatile unsigned char GPIO_PWM_ENABLED = 0;

void GPIO_Init()
{
	int i;

	// Valor de configuracao dos GPIOs: funcao 0, com pull-up e histerese
	uint32_t gpio_mode = IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_HYS_EN;

	// Inicializa IOCON e GPIO
	Chip_IOCON_Init(LPC_IOCON);
	Chip_GPIO_Init(LPC_GPIO);

	struct {
		uint8_t port;
		uint8_t pin;
		uint8_t isOutput;
	} pins[] = {
			// Pinos das entradas
			{ 2,  0, 0 }, // E1
			{ 2,  1, 0 }, // E2
			{ 2,  2, 0 }, // E3
			{ 2,  3, 0 }, // E4
			{ 2,  4, 0 }, // E5
			{ 2,  5, 0 }, // E6
			{ 2,  6, 0 }, // E7
			{ 2,  7, 0 }, // E8
			{ 2,  8, 0 }, // E9
			{ 2,  9, 0 }, // E10
			{ 2, 11, 0 }, // E11
			{ 2, 12, 0 }, // E12
			{ 2, 13, 0 }, // E13
			{ 0,  4, 0 }, // E14
			{ 0,  5, 0 }, // E15
			{ 0, 19, 0 }, // E16
			{ 0, 21, 0 }, // E17
			{ 1, 19, 0 }, // E18
			{ 0, 29, 0 }, // E19

			// Pino que controla o led CPU
			{ 1, 22, 1 },

			// Pino que controla o led ERRO
			{ 1, 21, 1 }
	};

	// Configura os GPIOs conforme definido na listagem acima
	for(i = (sizeof(pins) / sizeof(pins[0])) - 1; i >= 0; i--) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, pins[i].port, pins[i].pin, gpio_mode);
		Chip_GPIO_SetDir(LPC_GPIO, pins[i].port, pins[i].pin, pins[i].isOutput);
	}

	// Inicializacao das Saidas (SSP)
	SSP_Init();

	unsigned char cmd = OUTPUT_CMD_CONTROL;
	SSP_Write((unsigned char*)&cmd, 1);

	if(GPIO_PWM_ENABLED) {
		unsigned int mask = HTONS(0x8000);

		cmd = 0x4 << 2;
		SSP_Enable();
		SSP_Write((unsigned char*)&cmd, 1);
		SSP_Write((unsigned char*)&mask, 2);
		SSP_Disable();
	}

	// U17 - ERRO. Inicia desligado, ou seja, sinal alto!
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 21, 1);
}

unsigned int GPIO_Output(void)
{
  unsigned int i = 0;
  unsigned char cmd = OUTPUT_CMD_CONTROL;

  i |= ((unsigned int)GPIO_OUTPUT_PORT1);
  i |= ((unsigned int)GPIO_OUTPUT_PORT2) << 1;
  i |= ((unsigned int)GPIO_OUTPUT_PORT3) << 2;
  i |= ((unsigned int)GPIO_OUTPUT_PORT4) << 3;
  i |= ((unsigned int)GPIO_OUTPUT_PORT5) << 4;
  i |= ((unsigned int)GPIO_OUTPUT_PORT6) << 5;
  i |= ((unsigned int)GPIO_OUTPUT_PORT7) << 6;
  i |= ((unsigned int)GPIO_OUTPUT_PORT8) << 7;
  i |= ((unsigned int)GPIO_OUTPUT_PORT9) << 8;
  i |= ((unsigned int)GPIO_OUTPUT_PORT10) << 9;
  i |= ((unsigned int)GPIO_OUTPUT_PORT11) << 10;
  i |= ((unsigned int)GPIO_OUTPUT_PORT12) << 11;
  i |= ((unsigned int)GPIO_OUTPUT_PORT13) << 12;
  i |= ((unsigned int)GPIO_OUTPUT_PORT14) << 13;
  i |= ((unsigned int)GPIO_OUTPUT_PORT15) << 14;
  i |= ((unsigned int)GPIO_OUTPUT_PORT16) << 15;

  // Aqui ligamos ou desligamos as saidas forcadas pelo ModBUS
  i = (i & OUTPUT_FORCED_MASK) | OUTPUT_FORCED_STATE;

  GPIO_OUTPUT = i;

  i = HTONS(i);

  i &= 0xFFFF;

  SSP_Enable();
  SSP_Write((unsigned char*)&cmd, 1);
  SSP_Write((unsigned char*)&i, 2);
  SSP_Disable();

//  SSP_Read((unsigned char*)&status, 3);

  // U17 - ERRO
  // U18 - CPU
  //GPIO1->FIOPIN |= 0x3 << 21;
  //GPIO1->FIOPIN &= ~(((GPIO_OUTPUT_PORT17 & 0x1) << 21) | ((GPIO_OUTPUT_PORT18 & 0x1) << 22));

  return i;
}

unsigned int GPIO_Input(void)
{
  unsigned int i = 0;
  uint32_t port0, port2;

  port0 = Chip_GPIO_GetPortValue(LPC_GPIO, 0);
  port2 = Chip_GPIO_GetPortValue(LPC_GPIO, 2);

  i |=  0x000003FF & port2;        // P2.0 ~ P2.9    [00..10]
  i |= (0x00003800 & port2) >>  1; // P2.11 ~ P2.13  [11..13]
  i |= (0x00000030 & port0) <<  9; // P0.4 ~ P0.5    [14..15]
  i |= (0x00080000 & port0) >>  4; // P0.19 ~ P0.19  [16..16]
  i |= (0x00600000 & port0) >>  5; // P0.21 ~ P0.22  [17..18]
  i |= (0x20000000 & port0) >> 11; // P0.29          [19]

  i = ~i;

  GPIO_INPUT_PORT1 = i & 0x1;
  GPIO_INPUT_PORT2 = (i & (1 << 1)) >> 1;
  GPIO_INPUT_PORT3 = (i & (1 << 2)) >> 2;
  GPIO_INPUT_PORT4 = (i & (1 << 3)) >> 3;
  GPIO_INPUT_PORT5 = (i & (1 << 4)) >> 4;
  GPIO_INPUT_PORT6 = (i & (1 << 5)) >> 5;
  GPIO_INPUT_PORT7 = (i & (1 << 6)) >> 6;
  GPIO_INPUT_PORT8 = (i & (1 << 7)) >> 7;
  GPIO_INPUT_PORT9 = (i & (1 << 8)) >> 8;
  GPIO_INPUT_PORT10 = (i & (1 << 9)) >> 9;
  GPIO_INPUT_PORT11 = (i & (1 << 10)) >> 10;
  GPIO_INPUT_PORT12 = (i & (1 << 11)) >> 11;
  GPIO_INPUT_PORT13 = (i & (1 << 12)) >> 12;
  GPIO_INPUT_PORT14 = (i & (1 << 13)) >> 13;
  GPIO_INPUT_PORT15 = (i & (1 << 14)) >> 14;
  GPIO_INPUT_PORT16 = (i & (1 << 15)) >> 15;
  GPIO_INPUT_PORT17 = (i & (1 << 16)) >> 16;
  GPIO_INPUT_PORT18 = (i & (1 << 17)) >> 17;
  GPIO_INPUT_PORT19 = (i & (1 << 18)) >> 18;

  GPIO_INPUT = i;

  return i;
}
