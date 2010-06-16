/***********************************************************************************
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
************************************************************************************
*  History:
*
*  18.02.10  mifi   First Version, this file based on an anglia example 
*                   and was modified for the LPC1768.
***********************************************************************************/

/***********************************************************************************
*  Copyright 2005 Anglia Design
*  This demo code and associated components are provided as is and has no warranty,
*  implied or otherwise.  You are free to use/modify any of the provided
*  code at your own risk in your applications with the expressed limitation
*  of liability (see below)
* 
*  LIMITATION OF LIABILITY:   ANGLIA OR ANGLIA DESIGNS SHALL NOT BE LIABLE FOR ANY
*  LOSS OF PROFITS, LOSS OF USE, LOSS OF DATA, INTERRUPTION OF BUSINESS, NOR FOR
*  INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND WHETHER UNDER
*  THIS AGREEMENT OR OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
*  Author         : Spencer Oliver
*  Web         : www.anglia-designs.com
*
***********************************************************************************/

#include "vectors.h"

void ResetISR(void) __attribute__((__interrupt__));

extern int main(void);

extern unsigned long _etext;
extern unsigned long __data_start;
extern unsigned long _edata;
extern unsigned long __bss_start__;
extern unsigned long __bss_end__;
extern unsigned long __stack_end__;
extern unsigned long __ctors_start__;
extern unsigned long __ctors_end__;

/* 
 * LPC1768 vectors 
 */
__attribute__ ((section(".vectors"), used))
void (* const g_pfnVectors[])(void) = {
   (void (*)(void))((unsigned long)&__stack_end__),
   ResetISR,
   NMIException,
   HardFaultException,
   MemManageException,
   BusFaultException,
   UsageFaultException,
(void (*)(void))((unsigned long)0x8ffe70aa),
   0,                   /* Reserved */
   0,                   /* Reserved */
   0,                   /* Reserved */
   SVCHandler,
   DebugMonitor,
   0,                   /* Reserved */
   PendSVC,
   SysTickHandler,
   
   /*
    * For the following interrupts take a look in the 
    * LPC17xx user manual, UM10360, Rev. 01-4 January 2010.
    * Chapter 6: LPC17xx Nested Vectored Interrupt Controller (NVIC)
    */
   WDT_IRQHandler,
   TIMER0_IRQHandler,
   TIMER1_IRQHandler,
   TIMER2_IRQHandler,
   TIMER3_IRQHandler,
   UART0_IRQHandler,
   UART1_IRQHandler,
   UART2_IRQHandler,
   UART3_IRQHandler,
   PWM1_IRQHandler,
   I2C0_IRQHandler,
   I2C1_IRQHandler,
   I2C2_IRQHandler,
   SPI_IRQHandler,
   SSP0_IRQHandler,
   SSP1_IRQHandler,
   PLL0_IRQHandler,
   RTC_IRQHandler,
   EINT0_IRQHandler,
   EINT1_IRQHandler,
   EINT2_IRQHandler,
   EINT3_IRQHandler,
   ADC_IRQHandler,
   BOD_IRQHandler,
   USB_IRQHandler,
   CAN_IRQHandler,
   GPDMA_IRQHandler,
   I2S_IRQHandler,
   ETHERNET_IRQHandler,
   RIT_IRQHandler,
   MCPWM_IRQHandler,
   QE_IRQHandler,
   PLL1_IRQHandler,
   USBActivity_IRQHandler,
   CANActivity_IRQHandler
};

/* 
 * Reset code 
 */
void ResetISR(void)
{
   unsigned long *pSrc, *pDest;
   
   /* 
    * Copy the data segment initializers from flash to SRAM 
    */
   pSrc = &_etext;
   for(pDest = &__data_start; pDest < &_edata; )
   {
      *pDest++ = *pSrc++;
   }

   /* 
    * Zero fill the bss segment 
    */
   for(pDest = &__bss_start__; pDest < &__bss_end__; ) 
   {
      *pDest++ = 0;
   }

   /* 
    * Call the application entry point 
    */
   main();
}

/*
 * WEAK Exception and IRQHandler
 */
WEAK void NMIException(void) { while(1); }
WEAK void HardFaultException(void) { while(1); }
WEAK void MemManageException(void) { while(1); }
WEAK void BusFaultException(void) { while(1); }
WEAK void UsageFaultException(void) { while(1); }
WEAK void SVCHandler(void) { while(1); }
WEAK void DebugMonitor(void) { while(1); }
WEAK void PendSVC(void) { while(1); }
WEAK void SysTickHandler(void) { while(1); }

WEAK void WDT_IRQHandler(void) { while(1); }
WEAK void TIMER0_IRQHandler(void) { while(1); }
WEAK void TIMER1_IRQHandler(void) { while(1); }
WEAK void TIMER2_IRQHandler(void) { while(1); }
WEAK void TIMER3_IRQHandler(void) { while(1); }
WEAK void UART0_IRQHandler(void) { while(1); }
WEAK void UART1_IRQHandler(void) { while(1); }
WEAK void UART2_IRQHandler(void) { while(1); }
WEAK void UART3_IRQHandler(void) { while(1); }
WEAK void PWM1_IRQHandler(void) { while(1); }
WEAK void I2C0_IRQHandler(void) { while(1); }
WEAK void I2C1_IRQHandler(void) { while(1); }
WEAK void I2C2_IRQHandler(void) { while(1); }
WEAK void SPI_IRQHandler(void) { while(1); }
WEAK void SSP0_IRQHandler(void) { while(1); }
WEAK void SSP1_IRQHandler(void) { while(1); }
WEAK void PLL0_IRQHandler(void) { while(1); }
WEAK void RTC_IRQHandler(void) { while(1); }
WEAK void EINT0_IRQHandler(void) { while(1); }
WEAK void EINT1_IRQHandler(void) { while(1); }
WEAK void EINT2_IRQHandler(void) { while(1); }
WEAK void EINT3_IRQHandler(void) { while(1); }
WEAK void ADC_IRQHandler(void) { while(1); }
WEAK void BOD_IRQHandler(void) { while(1); }
WEAK void USB_IRQHandler(void) { while(1); }
WEAK void CAN_IRQHandler(void) { while(1); }
WEAK void GPDMA_IRQHandler(void) { while(1); }
WEAK void I2S_IRQHandler(void) { while(1); }
WEAK void ETHERNET_IRQHandler(void) { while(1); }
WEAK void RIT_IRQHandler(void) { while(1); }
WEAK void MCPWM_IRQHandler(void) { while(1); }
WEAK void QE_IRQHandler(void) { while(1); }
WEAK void PLL1_IRQHandler(void) { while(1); }
WEAK void USBActivity_IRQHandler(void) { while(1); }
WEAK void CANActivity_IRQHandler(void) { while(1); }

/*** EOF ***/

