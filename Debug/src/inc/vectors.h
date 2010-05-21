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
*	Copyright 2005 Anglia Design
*	This demo code and associated components are provided as is and has no warranty,
*	implied or otherwise.  You are free to use/modify any of the provided
*	code at your own risk in your applications with the expressed limitation
*	of liability (see below)
* 
*	LIMITATION OF LIABILITY:   ANGLIA OR ANGLIA DESIGNS SHALL NOT BE LIABLE FOR ANY
*	LOSS OF PROFITS, LOSS OF USE, LOSS OF DATA, INTERRUPTION OF BUSINESS, NOR FOR
*	INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND WHETHER UNDER
*	THIS AGREEMENT OR OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
*	Author			: Spencer Oliver
*	Web     		: www.anglia-designs.com
*
***********************************************************************************/

#ifndef __VECTORS_H
#define __VECTORS_H

#ifdef __cplusplus
extern "C" {
#endif

#define WEAK __attribute__ ((weak))

WEAK void NMIException(void) __attribute__((__interrupt__));
WEAK void HardFaultException(void) __attribute__((__interrupt__));
WEAK void MemManageException(void) __attribute__((__interrupt__));
WEAK void BusFaultException(void) __attribute__((__interrupt__));
WEAK void UsageFaultException(void) __attribute__((__interrupt__));
WEAK void DebugMonitor(void) __attribute__((__interrupt__));
WEAK void SVCHandler(void) __attribute__((__interrupt__));
WEAK void PendSVC(void) __attribute__((__interrupt__));
WEAK void SysTickHandler(void) __attribute__((__interrupt__));

WEAK void WDT_IRQHandler(void) __attribute__((__interrupt__));
WEAK void TIMER0_IRQHandler(void) __attribute__((__interrupt__));
WEAK void TIMER1_IRQHandler(void) __attribute__((__interrupt__));
WEAK void TIMER2_IRQHandler(void) __attribute__((__interrupt__));
WEAK void TIMER3_IRQHandler(void) __attribute__((__interrupt__));
WEAK void UART0_IRQHandler(void) __attribute__((__interrupt__));
WEAK void UART1_IRQHandler(void) __attribute__((__interrupt__));
WEAK void UART2_IRQHandler(void) __attribute__((__interrupt__));
WEAK void UART3_IRQHandler(void) __attribute__((__interrupt__));
WEAK void PWM1_IRQHandler(void) __attribute__((__interrupt__));
WEAK void I2C0_IRQHandler(void) __attribute__((__interrupt__));
WEAK void I2C1_IRQHandler(void) __attribute__((__interrupt__));
WEAK void I2C2_IRQHandler(void) __attribute__((__interrupt__));
WEAK void SPI_IRQHandler(void) __attribute__((__interrupt__));
WEAK void SSP0_IRQHandler(void) __attribute__((__interrupt__));
WEAK void SSP1_IRQHandler(void) __attribute__((__interrupt__));
WEAK void PLL0_IRQHandler(void) __attribute__((__interrupt__));
WEAK void RTC_IRQHandler(void) __attribute__((__interrupt__));
WEAK void EINT0_IRQHandler(void) __attribute__((__interrupt__));
WEAK void EINT1_IRQHandler(void) __attribute__((__interrupt__));
WEAK void EINT2_IRQHandler(void) __attribute__((__interrupt__));
WEAK void EINT3_IRQHandler(void) __attribute__((__interrupt__));
WEAK void ADC_IRQHandler(void) __attribute__((__interrupt__));
WEAK void BOD_IRQHandler(void) __attribute__((__interrupt__));
WEAK void USB_IRQHandler(void) __attribute__((__interrupt__));
WEAK void CAN_IRQHandler(void) __attribute__((__interrupt__));
WEAK void GPDMA_IRQHandler(void) __attribute__((__interrupt__));
WEAK void I2S_IRQHandler(void) __attribute__((__interrupt__));
WEAK void ETHERNET_IRQHandler(void) __attribute__((__interrupt__));
WEAK void RIT_IRQHandler(void) __attribute__((__interrupt__));
WEAK void MCPWM_IRQHandler(void) __attribute__((__interrupt__));
WEAK void QE_IRQHandler(void) __attribute__((__interrupt__));
WEAK void PLL1_IRQHandler(void) __attribute__((__interrupt__));
WEAK void USBActivity_IRQHandler(void) __attribute__((__interrupt__));
WEAK void CANActivity_IRQHandler(void) __attribute__((__interrupt__));

#ifdef __cplusplus
}
#endif

#endif /* VECTORS */
