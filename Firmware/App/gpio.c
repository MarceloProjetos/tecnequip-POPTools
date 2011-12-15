/******************************************************************************
* Rotinas de atualização das Entradas/Saidas
******************************************************************************/
#include "lpc17xx.h"
#include "gpio.h"
#include "ssp.h"

volatile unsigned int GPIO_OUTPUT = 0;
volatile unsigned int GPIO_INPUT = 0;

volatile int GPIO_INPUT_PORT1 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT2 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT3 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT4 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT5 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT6 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT7 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT8 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT9 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT10 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT11 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT12 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT13 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT14 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT15 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT16 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT17 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT18 __attribute__((weak)) = 0;
volatile int GPIO_INPUT_PORT19 __attribute__((weak)) = 0;

volatile int GPIO_OUTPUT_PORT1 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT2 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT3 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT4 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT5 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT6 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT7 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT8 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT9 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT10 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT11 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT12 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT13 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT14 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT15 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT16 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT17 __attribute__((weak)) = 0;
volatile int GPIO_OUTPUT_PORT18 __attribute__((weak)) = 0;

void GPIO_Init()
{

	// Entradas
	PINCON->PINSEL0 &= ~0xF00;
	PINCON->PINSEL1 &= ~0x3CC0;
	PINCON->PINSEL4 &= ~0xFCFFFFF;
	GPIO2->FIODIR &= ~0x3BFF;
	GPIO0->FIODIR &= ~0x680030;

	// Saidas Led Erro/CPU
	PINCON->PINSEL3 &= ~0x3c00;
	GPIO1->FIODIR |= 0x3 << 21;

	// Saidas (SSP)
	SC->PCONP |= 1 << 21;     // ssp on
	PINCON->PINSEL0 &= ~0xC0000000;
	PINCON->PINSEL0 |= 0x80000000;
	PINCON->PINSEL1 &= ~0x3F;
	PINCON->PINSEL1 |= 0x28; //0x2A; ChipSelect as GPIO
	GPIO0->FIODIR |= 0x10000; // P0.16 as output
	SSP0->CR1 = 0;        // disable, set to master
	SSP0->CR0 = 0x1F87;   // 8 bits, cpol=0, cpha=1, scr=32
	SSP0->CPSR = 6;       // prescaler=6
	SSP0->CR1 |= 0x2;     // enable

	unsigned char cmd = OUTPUT_CMD_CONTROL;
	SSP_Write((unsigned char*)&cmd, 1);
}

unsigned int GPIO_Output(unsigned int OUTPUT)
{
  unsigned int i = 0;
  unsigned int status = 0;
  unsigned char cmd = OUTPUT_CMD_CONTROL;

  i |= GPIO_OUTPUT_PORT1;
  i |= GPIO_OUTPUT_PORT2 << 1;
  i |= GPIO_OUTPUT_PORT3 << 2;
  i |= GPIO_OUTPUT_PORT4 << 3;
  i |= GPIO_OUTPUT_PORT5 << 4;
  i |= GPIO_OUTPUT_PORT6 << 5;
  i |= GPIO_OUTPUT_PORT7 << 6;
  i |= GPIO_OUTPUT_PORT8 << 7;
  i |= GPIO_OUTPUT_PORT9 << 8;
  i |= GPIO_OUTPUT_PORT10 << 9;
  i |= GPIO_OUTPUT_PORT11 << 10;
  i |= GPIO_OUTPUT_PORT12 << 11;
  i |= GPIO_OUTPUT_PORT13 << 12;
  i |= GPIO_OUTPUT_PORT14 << 13;
  i |= GPIO_OUTPUT_PORT15 << 14;
  i |= GPIO_OUTPUT_PORT16 << 15;

  /*i |= OUTPUT & 0x1;
  i |= OUTPUT & (1 << 1);
  i |= OUTPUT & (1 << 2);
  i |= OUTPUT & (1 << 3);
  i |= OUTPUT & (1 << 4);
  i |= OUTPUT & (1 << 5);
  i |= OUTPUT & (1 << 6);
  i |= OUTPUT & (1 << 7);
  i |= OUTPUT & (1 << 8);
  i |= OUTPUT & (1 << 9);
  i |= OUTPUT & (1 << 10);
  i |= OUTPUT & (1 << 11);
  i |= OUTPUT & (1 << 12);
  i |= OUTPUT & (1 << 13);
  i |= OUTPUT & (1 << 14);
  i |= OUTPUT & (1 << 15);*/

  i = HTONS(i);

  i &= 0xFFFF;

  GPIO0->FIOCLR = 0x10000;
  SSP_Write((unsigned char*)&cmd, 1);
  SSP_Write((unsigned char*)&i, 2);
  GPIO0->FIOSET = 0x10000;

  SSP_Read((unsigned char*)&status, 3);

  // U17 - ERRO
  // U18 - CPU
  GPIO1->FIOPIN |= 0x3 << 21;
  GPIO1->FIOPIN &= ~(((GPIO_OUTPUT_PORT17 & 0x1) << 21) | ((GPIO_OUTPUT_PORT18 & 0x1) << 22));

  return i;
}

unsigned int GPIO_Input(void)
{
  unsigned int i = 0;

  i |= 0x3FF & GPIO2->FIOPIN;              // P2.0 ~ P2.9    [00..10]
  i |= (0x3800 & GPIO2->FIOPIN) >> 1;      // P2.11 ~ P2.13  [11..13]
  i |= (0x30 & GPIO0->FIOPIN) << 9;        // P0.4 ~ P0.5    [14..15]
  i |= (0x80000 & GPIO0->FIOPIN) >> 4;     // P0.19 ~ P0.19  [16..16]
  i |= (0x600000 & GPIO0->FIOPIN) >> 5;    // P0.21 ~ P0.22  [17..18]
  i |= (0x20000000 & GPIO0->FIOPIN) >> 11; // P0.29          [19]

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

  return i;
}
