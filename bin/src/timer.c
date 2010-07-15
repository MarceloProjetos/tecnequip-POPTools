/*****************************************************************************
 *   timer.c:  Timer C file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "lpc17xx.h"
#include "timer.h"

volatile unsigned int timer0_counter = 0;
volatile unsigned int timer1_counter = 0;

/*****************************************************************************
** Function name:		delayMs
**
** Descriptions:		Start the timer delay in milo seconds
**						until elapsed
**
** parameters:			timer number, Delay value in milo second			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delayMs(unsigned char timer_num, unsigned int delayInMs)
{
  if ( timer_num == 0 )
  {
	/*
	* setup timer #0 for delay
	*/
	TIM0 -> TCR = 0x02;		/* reset timer */
	TIM0 -> PR  = 0x00;		/* set prescaler to zero */
	TIM0 -> MR0 = delayInMs * (9000000 / 1000-1);
	TIM0 -> IR  = 0xff;		/* reset all interrrupts */
	TIM0 -> MCR = 0x04;		/* stop timer on match */
	TIM0 -> TCR = 0x01;		/* start timer */

	/* wait until delay time has elapsed */
	while (TIM0 -> TCR & 0x01);
  }
  else if ( timer_num == 1 )
  {
	/*
	* setup timer #1 for delay
	*/
	TIM1 -> TCR = 0x02;		/* reset timer */
	TIM1 -> PR  = 0x00;		/* set prescaler to zero */
	TIM1 -> MR0 = delayInMs * (9000000 / 1000-1);
	TIM1 -> IR  = 0xff;		/* reset all interrrupts */
	TIM1 -> MCR = 0x04;		/* stop timer on match */
	TIM1 -> TCR = 0x01;		/* start timer */

	/* wait until delay time has elapsed */
	while (TIM1 -> TCR & 0x01);
  }
  return;
}


/******************************************************************************
** Function name:		Timer0Handler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
/*void TIMER0_IRQHandler (void)
{  
  TIM0 -> IR = 1;

  timer0_counter++;

}*/

/******************************************************************************
** Function name:		Timer1Handler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
/*void TIMER1_IRQHandler (void)
{  
  TIM1 -> IR = 1;

  timer1_counter++;

}*/

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void enable_timer( unsigned char timer_num )
{
  if ( timer_num == 0 )
  {
	TIM0 -> TCR = 1;
  }
  else
  {
	TIM1 -> TCR = 1;
  }
  return;
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void disable_timer( unsigned char timer_num )
{
  if ( timer_num == 0 )
  {
	TIM0 -> TCR = 0;
  }
  else
  {
	TIM1 -> TCR = 0;
  }
  return;
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void reset_timer( unsigned char timer_num )
{
  unsigned int regVal;

  if ( timer_num == 0 )
  {
	regVal = TIM0 -> TCR;
	regVal |= 0x02;
	TIM0 -> TCR = regVal;
  }
  else
  {
	regVal = TIM1 -> TCR;
	regVal |= 0x02;
	TIM1 -> TCR = regVal;
  }
  return;
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		true or false, if the interrupt handler can't be
**						installed, return false.
** 
******************************************************************************/
unsigned int init_timer ( unsigned char timer_num, unsigned int TimerInterval )
{

  if ( timer_num == 0 )
  {
	timer0_counter = 0;
	TIM0 -> MR0 = TimerInterval;
	TIM0 -> MCR = 3;				/* Interrupt and Reset on MR0 */

	NVIC_EnableIRQ(TIMER0_IRQn);

	return (1);
  }
  else if ( timer_num == 1 )
  {
	timer1_counter = 0;
	TIM1 -> MR0 = TimerInterval;
	TIM1 -> MCR = 3;				/* Interrupt and Reset on MR1 */

	NVIC_EnableIRQ(TIMER1_IRQn);

	return (1);
  }
  return (0);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
