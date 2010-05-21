/*****************************************************************************
 *   timer.h:  Header file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H

extern void delayMs(unsigned char timer_num, unsigned int delayInMs);
extern unsigned int init_timer( unsigned char timer_num, unsigned int timerInterval );
extern void enable_timer( unsigned char timer_num );
extern void disable_timer( unsigned char timer_num );
extern void reset_timer( unsigned char timer_num );

#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/

