/****************************************************************************
*  Copyright (c) 2010 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
****************************************************************************
*  History:
*
*  18.02.10  mifi   First Version
****************************************************************************/
#define __MAIN_C__

/*
 * I use the include only, to show
 * how to setup a include dir in the makefile
 */
#include "typedefs.h"
#include "lpc17xx.h"
#include "timer.h"

/*=========================================================================*/
/*  DEFINE: All Structures and Common Constants                            */
/*=========================================================================*/

//SSP Status register
#define SSPSR_TNF     1 << 1
#define SSPSR_RNE     1 << 2
#define SSPSR_BSY     1 << 4

#define OUTPUT_CMD_CONTROL  (0 << 2)

#define RS485_ENABLE    (1 << 20)

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/
volatile unsigned int I_E1 __attribute__((weak)) = 0;
volatile unsigned int I_E2 __attribute__((weak)) = 0;
volatile unsigned int I_E3 __attribute__((weak)) = 0;
volatile unsigned int I_E4 __attribute__((weak)) = 0;
volatile unsigned int I_E5 __attribute__((weak)) = 0;
volatile unsigned int I_E6 __attribute__((weak)) = 0;
volatile unsigned int I_E7 __attribute__((weak)) = 0;
volatile unsigned int I_E8 __attribute__((weak)) = 0;
volatile unsigned int I_E9 __attribute__((weak)) = 0;
volatile unsigned int I_E10 __attribute__((weak)) = 0;
volatile unsigned int I_E11 __attribute__((weak)) = 0;
volatile unsigned int I_E12 __attribute__((weak)) = 0;
volatile unsigned int I_E13 __attribute__((weak)) = 0;
volatile unsigned int I_E14 __attribute__((weak)) = 0;
volatile unsigned int I_E15 __attribute__((weak)) = 0;
volatile unsigned int I_E16 __attribute__((weak)) = 0;
volatile unsigned int I_E17 __attribute__((weak)) = 0;
volatile unsigned int I_E18 __attribute__((weak)) = 0;
volatile unsigned int I_E19 __attribute__((weak)) = 0;

volatile unsigned int U_S1 __attribute__((weak)) = 0;
volatile unsigned int U_S2 __attribute__((weak)) = 0;
volatile unsigned int U_S3 __attribute__((weak)) = 0;
volatile unsigned int U_S4 __attribute__((weak)) = 0;
volatile unsigned int U_S5 __attribute__((weak)) = 0;
volatile unsigned int U_S6 __attribute__((weak)) = 0;
volatile unsigned int U_S7 __attribute__((weak)) = 0;
volatile unsigned int U_S8 __attribute__((weak)) = 0;
volatile unsigned int U_S9 __attribute__((weak)) = 0;
volatile unsigned int U_S10 __attribute__((weak)) = 0;
volatile unsigned int U_S11 __attribute__((weak)) = 0;
volatile unsigned int U_S12 __attribute__((weak)) = 0;
volatile unsigned int U_S13 __attribute__((weak)) = 0;
volatile unsigned int U_S14 __attribute__((weak)) = 0;
volatile unsigned int U_S15 __attribute__((weak)) = 0;
volatile unsigned int U_S16 __attribute__((weak)) = 0;

extern void PlcCycle(void);
extern volatile unsigned int TIME_INTERVAL;

/*=========================================================================*/
/*  DEFINE: Prototypes                                                     */
/*=========================================================================*/

/*=========================================================================*/
/*  DEFINE: Definition of all local Data                                   */
/*=========================================================================*/

/*=========================================================================*/
/*  DEFINE: Definition of all local Procedures                             */
/*=========================================================================*/
void TIMER0_IRQHandler (void)
{
  TIM0->IR = 1;                       /* clear interrupt flag */

  PlcCycle();

  GPIO0->FIOPIN = ~GPIO0->FIOPIN;
}

/******************************************************************************
* Inicializacao do Hardware
******************************************************************************/
void HardwareInit(void)
{
  // Led CPU
  GPIO0->FIODIR = (1 << 4);       /* CPU LED */
  GPIO0->FIOCLR = (1 << 4);       /* turn on the LED */
  GPIO0->FIOMASK = ~(1 << 4);

  // Inicializacao 485
  PINCON->PINSEL0 &= ~(0xF << 4);
  PINCON->PINSEL0 |= 5 << 4;

  // WEG SoftStarter 9600 (8, N, 1)
  UART1->LCR = 0x83;          /* 8 bits, no Parity, 1 Stop bit */
  UART1->DLM = 1;
  UART1->DLL = 119;
  UART1->FDR = (4 << 4) | 1;
  UART1->LCR = 0x03;          /* DLAB = 0 */
  UART1->FCR = 0x07;          /* Enable and reset TX and RX FIFO. */

  PINCON->PINSEL1 &= ~(0x3 << 8);       // P0.20
  GPIO0->FIODIR |= RS485_ENABLE;        // P0.20 output

  // Entradas
  PINCON->PINSEL0 &= ~0xF00;
  PINCON->PINSEL1 &= ~0x3CC0;
  PINCON->PINSEL4 &= ~0xFCFFFFF;
  GPIO2->FIODIR &= ~0x3BFF;
  GPIO0->FIODIR &= ~0x680030;

  // Saidas (SSP)
  PINCON->PINSEL0 &= ~0xC0000000;
  PINCON->PINSEL0 |= ~0x20000000;
  PINCON->PINSEL1 &= ~0x3F;
  PINCON->PINSEL1 |= 0x2A;
  SSP0->CR1 &= ~0x6;    // disable, set to master
  SSP0->CR0 = 0x1F87;   // 8 bits, cpol=0, cpha=1, scr=32
  SSP0->CPSR = 5;       // prescaler=2
  SSP0->CR1 |= 0x2;     // enable

}

unsigned char SSPWrite(unsigned char * buffer, unsigned int size)
{
  unsigned int i;

  for (i = 0; i < size; i++ )
  {
    while ( !(SSP0->SR & SSPSR_TNF) );
    SSP0->DR = *buffer;
    buffer++;
    while ( SSP0->SR & SSPSR_BSY );
  }
  return i;
}

unsigned char SSPRead(uint8_t * buffer, uint32_t size)
{
  unsigned int i;

  for (i = 0; i < size; i++ )
  {
    while ( !(SSP0->SR & SSPSR_RNE) );
    *buffer = SSP0->DR;
    buffer++;
  }
  return i;
}

void AtualizaSaidas(void)
{
  unsigned int i = 0;
  unsigned char cmd = OUTPUT_CMD_CONTROL;

  i |= U_S1;
  i |= U_S2 << 1;
  i |= U_S3 << 2;
  i |= U_S4 << 3;
  i |= U_S5 << 4;
  i |= U_S6 << 5;
  i |= U_S7 << 6;
  i |= U_S8 << 7;
  i |= U_S9 << 8;
  i |= U_S10 << 9;
  i |= U_S11 << 10;
  i |= U_S12 << 11;
  i |= U_S13 << 12;
  i |= U_S14 << 13;
  i |= U_S15 << 14;
  i |= U_S16 << 15;

  SSPWrite((unsigned char*)&cmd, 1);
  SSPWrite((unsigned char*)&i, 2);

  SSPRead((unsigned char*)&i, 3);
}

void AtualizaEntradas(void)
{
  unsigned int i = 0;

  i |= 0x3FF & GPIO2->FIOPIN;            // P2.0 ~ P2.9    [00..10]
  i |= (0x3800 & GPIO2->FIOPIN) >> 1;    // P2.11 ~ P2.13  [11..13]
  i |= (0x30 & GPIO0->FIOPIN) << 9;      // P0.4 ~ P0.5    [14..15]
  i |= (0x80000 & GPIO0->FIOPIN) >> 4;   // P0.19 ~ P0.19  [16..16]
  i |= (0x600000 & GPIO0->FIOPIN) >> 5;  // P0.21 ~ P0.22  [17..18]

  I_E1 = i & 0x1;
  I_E2 = (i & (1 << 1)) >> 1;
  I_E3 = (i & (1 << 2)) >> 2;
  I_E4 = (i & (1 << 3)) >> 3;
  I_E5 = (i & (1 << 4)) >> 4;
  I_E6 = (i & (1 << 5)) >> 5;
  I_E7 = (i & (1 << 6)) >> 6;
  I_E8 = (i & (1 << 7)) >> 7;
  I_E9 = (i & (1 << 8)) >> 8;
  I_E10 = (i & (1 << 9)) >> 9;
  I_E11 = (i & (1 << 10)) >> 10;
  I_E12 = (i & (1 << 11)) >> 11;
  I_E13 = (i & (1 << 12)) >> 12;
  I_E14 = (i & (1 << 13)) >> 13;
  I_E15 = (i & (1 << 14)) >> 14;
  I_E16 = (i & (1 << 15)) >> 15;
  I_E17 = (i & (1 << 16)) >> 16;
  I_E18 = (i & (1 << 17)) >> 17;
  I_E19 = (i & (1 << 18)) >> 18;

}

/***************************************************************************/
/*  main                                                                   */
/***************************************************************************/
int main (void)
{
  /**************************************************************************
   * Inicializa System
   *************************************************************************/
  SystemInit();
  HardwareInit();

  /**************************************************************************
   * Inicializa timer
   *************************************************************************/
  /*TIM0 -> MR0 = TIME_INTERVAL;
  TIM0 -> MCR = 3;*/				/* Interrupt and Reset on MR0 */

  /*NVIC_EnableIRQ(TIMER0_IRQn);
  TIM0 -> TCR = 1;*/
  init_timer( 0, TIME_INTERVAL ); // 5ms
  enable_timer( 0 );

  while(1)
  {
    AtualizaEntradas();
    AtualizaSaidas();
  }

  return(0);
}

/*** EOF ***/
