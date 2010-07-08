/*
 * 32bitmicro startup_LPC17xx.S  Startup File for the NXP LPC17xx Device Series

 * $URL$
 * $ID$

Copyright (c) 2009, 32bitmicro
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
Neither the name of the 32bitmicro nor the names of its contributors may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


	.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb

/* interrupt vectors table */

	.global	__isr_vectors
	.weak	__isr_vectors

 	.section ".isr_vector","a",%progbits
	.type	__isr_vectors, %object
	.size	__isr_vectors, .-__isr_vectors

__isr_vectors:
	.long	_estack                   /*  0: Top of Stack                 */
/* CM3 system interrupts */
	.long	Reset_Handler             /*  1: Reset Handler                */
	.long	NMI_Handler               /*  2: NMI Handler                  */
	.long	HardFault_Handler         /*  3: Hard Fault Handler           */
	.long	MemManage_Handler         /*  4: MPU Fault Handler            */
	.long	BusFault_Handler          /*  5: Bus Fault Handler            */
	.long	UsageFault_Handler        /*  6: Usage Fault Handler          */
	 .word 0xefff7b22
	/*.long	0                         /*  7: Reserved                     */
	.long	0                         /*  8: Reserved                     */
	.long	0                         /*  9: Reserved                     */
	.long	0                         /* 10: Reserved                     */
	.long	SVC_Handler               /* 11: SVCall Handler               */
	.long	DebugMon_Handler          /* 12: Debug Monitor Handler        */
	.long	0                         /* 13: Reserved                     */
	.long	PendSV_Handler            /* 14: PendSV Handler               */
	.long	SysTick_Handler           /* 15: SysTick Handler              */

/* LPC17xx interrupts */
	.long	WDT_IRQHandler            /* 16: Watchdog Timer               */
	.long	TIMER0_IRQHandler         /* 17: Timer0                       */
	.long	TIMER1_IRQHandler         /* 18: Timer1                       */
	.long	TIMER2_IRQHandler         /* 19: Timer2                       */
	.long	TIMER3_IRQHandler         /* 20: Timer3                       */
	.long	UART0_IRQHandler          /* 21: UART0                        */
	.long	UART1_IRQHandler          /* 22: UART1                        */
	.long	UART2_IRQHandler          /* 23: UART2                        */
	.long	UART3_IRQHandler          /* 24: UART3                        */
	.long	PWM1_IRQHandler           /* 25: PWM1                         */
	.long	I2C0_IRQHandler           /* 26: I2C0                         */
	.long	I2C1_IRQHandler           /* 27: I2C1                         */
	.long	I2C2_IRQHandler           /* 28: I2C2                         */
	.long	SPI_IRQHandler            /* 29: SPI                          */
	.long	SSP0_IRQHandler           /* 30: SSP0                         */
	.long	SSP1_IRQHandler           /* 31: SSP1                         */
	.long	PLL0_IRQHandler           /* 32: PLL0 Lock (Main PLL)         */
	.long	RTC_IRQHandler            /* 33: Real Time Clock              */
	.long	EINT0_IRQHandler          /* 34: External Interrupt 0         */
	.long	EINT1_IRQHandler          /* 35: External Interrupt 1         */
	.long	EINT2_IRQHandler          /* 36: External Interrupt 2         */
	.long	EINT3_IRQHandler          /* 37: External Interrupt 3         */
	.long	ADC_IRQHandler            /* 38: A/D Converter                */
	.long	BOD_IRQHandler            /* 39: Brown-Out Detect             */
	.long	USB_IRQHandler            /* 40: USB                          */
	.long	CAN_IRQHandler            /* 41: CAN                          */
	.long	DMA_IRQHandler            /* 42: General Purpose DMA          */
	.long	I2S_IRQHandler            /* 43: I2S                          */
	.long	ENET_IRQHandler           /* 44: Ethernet                     */
	.long	RIT_IRQHandler            /* 45: Repetitive Interrupt Timer   */
	.long	MCPWM_IRQHandler          /* 46: Motor Control PWM            */
	.long	QEI_IRQHandler            /* 47: Quadrature Encoder Interface */
	.long	PLL1_IRQHandler           /* 48: PLL1 Lock (USB PLL)          */



/* Reset handler */
	.section ".text.handler.reset","x",%progbits

Reset_Handler:
	.thumb_func
	.globl  Reset_Handler
	.type   Reset_Handler, %function
	ldr     r0, =SystemInit
	blx     r0
	/* branch to entry point */
	ldr     r0, =_start+1
	bx      r0
	.size	Reset_Handler, .-Reset_Handler


/* default handlers */
NMI_Handler:
	.globl  NMI_Handler
	.weak   NMI_Handler
	.type   NMI_Handler, %function
        b .

HardFault_Handler:
	.globl  HardFault_Handler
	.weak   HardFault_Handler
	.type   HardFault_Handler, %function
        b .

MemManage_Handler:
	.globl  MemManage_Handler
	.weak   MemManage_Handler
	.type   MemManage_Handler, %function
        b .

BusFault_Handler:
	.globl  BusFault_Handler
	.weak   BusFault_Handler
	.type   BusFault_Handler, %function
        b .

UsageFault_Handler:
	.globl  UsageFault_Handler
	.weak   UsageFault_Handler
	.type   UsageFault_Handler, %function
        b .

SVC_Handler:
	.globl  SVC_Handler
	.weak   SVC_Handler
	.type   SVC_Handler, %function
        b .

DebugMon_Handler:
	.globl  DebugMon_Handler
	.weak   DebugMon_Handler
	.type   DebugMon_Handler, %function
        b .

PendSV_Handler:
	.globl  PendSV_Handler
	.weak   PendSV_Handler
	.type   PendSV_Handler, %function
        b .

SysTick_Handler:
	.globl  SysTick_Handler
	.weak   SysTick_Handler
	.type   SysTick_Handler, %function
        b .

/* Default handler */

	.global	Default_Handler
	.weak	Default_Handler

	.section	.text.Default_Handler,"ax",%progbits
Default_Handler:
	.globl  WDT_IRQHandler
	.weak   WDT_IRQHandler
	.globl  TIMER0_IRQHandler
	.weak   TIMER0_IRQHandler
	.globl  TIMER1_IRQHandler
	.weak   TIMER1_IRQHandler
	.globl  TIMER2_IRQHandler
	.weak   TIMER2_IRQHandler
	.globl  TIMER3_IRQHandler
	.weak   TIMER3_IRQHandler
	.globl  UART0_IRQHandler
	.weak   UART0_IRQHandler
	.globl  UART1_IRQHandler
	.weak   UART1_IRQHandler
	.globl  UART2_IRQHandler
	.weak   UART2_IRQHandler
	.globl  UART3_IRQHandler
	.weak   UART3_IRQHandler
	.globl  PWM1_IRQHandler
	.weak   PWM1_IRQHandler
	.globl  I2C0_IRQHandler
	.weak   I2C0_IRQHandler
	.globl  I2C1_IRQHandler
	.weak   I2C1_IRQHandler
	.globl  I2C2_IRQHandler
	.weak   I2C2_IRQHandler
	.globl  SPI_IRQHandler
	.weak   SPI_IRQHandler
	.globl  SSP0_IRQHandler
	.weak   SSP0_IRQHandler
	.globl  SSP1_IRQHandler
	.weak   SSP1_IRQHandler
	.globl  PLL0_IRQHandler
	.weak   PLL0_IRQHandler
	.globl  RTC_IRQHandler
	.weak   RTC_IRQHandler
	.globl  EINT0_IRQHandler
	.weak   EINT0_IRQHandler
	.globl  EINT1_IRQHandler
	.weak   EINT1_IRQHandler
	.globl  EINT2_IRQHandler
	.weak   EINT2_IRQHandler
	.globl  EINT3_IRQHandler
	.weak   EINT3_IRQHandler
	.globl  ADC_IRQHandler
	.weak   ADC_IRQHandler
	.globl  BOD_IRQHandler
	.weak   BOD_IRQHandler
	.globl  USB_IRQHandler
	.weak   USB_IRQHandler
	.globl  CAN_IRQHandler
	.weak   CAN_IRQHandler
	.globl  DMA_IRQHandler
	.weak   DMA_IRQHandler
	.globl  I2S_IRQHandler
	.weak   I2S_IRQHandler
	.globl  ENET_IRQHandler
	.weak   ENET_IRQHandler
	.globl  RIT_IRQHandler
	.weak   RIT_IRQHandler
	.globl  MCPWM_IRQHandler
	.weak   MCPWM_IRQHandler
	.globl  QEI_IRQHandler
	.weak   QEI_IRQHandler
	.globl  PLL1_IRQHandler
	.weak   PLL1_IRQHandler

WDT_IRQHandler:
TIMER0_IRQHandler:
TIMER1_IRQHandler:
TIMER2_IRQHandler:
TIMER3_IRQHandler:
UART0_IRQHandler:
UART1_IRQHandler:
UART2_IRQHandler:
UART3_IRQHandler:
PWM1_IRQHandler:
I2C0_IRQHandler:
I2C1_IRQHandler:
I2C2_IRQHandler:
SPI_IRQHandler:
SSP0_IRQHandler:
SSP1_IRQHandler:
PLL0_IRQHandler:
RTC_IRQHandler:
EINT0_IRQHandler:
EINT1_IRQHandler:
EINT2_IRQHandler:
EINT3_IRQHandler:
ADC_IRQHandler:
BOD_IRQHandler:
USB_IRQHandler:
CAN_IRQHandler:
DMA_IRQHandler:
I2S_IRQHandler:
ENET_IRQHandler:
RIT_IRQHandler:
MCPWM_IRQHandler:
QEI_IRQHandler:
PLL1_IRQHandler:

/* catch it here */
        b       .
        .size	Default_Handler, .-Default_Handler

/* Default _start */

	.section ".text"
	.thumb
	.global	_start
	.weak	_start
	.type   _start, %function
_start:

/* fill .bss with 0 using byte stores */
	ldr     r0, =_bss
	ldr     r1, =_ebss
        mov     r2, #0
.L0:
	strb	r2, [r0], #1
	cmp     r0, r1
  	bcc.n   .L0

/* copy initialized .data from rom to ram */
	ldr     r0, =_data
	ldr     r1, =_edata
	ldr     r2, =_ldata
.L1:
	ldrb	r3, [r2], #1
	strb	r3, [r0], #1
	cmp     r0, r1
	bcc.n   .L1

/* branch to main */
	ldr     r0, =main+1
	bx      r0
	.size	_start, .-_start

	.section ".CRP._0x02FC","a",%progbits
CRP_Key:
	.word     0xFFFFFFFF

	.end

