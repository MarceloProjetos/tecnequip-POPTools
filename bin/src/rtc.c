/*********** ********* ********* ********* ********* ********* ********* ********* ***
 *   rtc.c:  Realtime clock C file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.26  ver 1.00    Prelimnary version, first Release
 *
************ ********* ********* ********* ********* ********* ********* ********* **/

/*********** ********* ********* ********* ********* ********* ********* ********* ********* *******
 Includes
************ ********* ********* ********* ********* ********* ********* ********* ********* ******/
#include "lpc17xx.h"
#include "type.h"
#include "stdio.h"
#include "rtc.h"

RTCTime relogio_atual;

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTC_IRQHandler
**
** Descriptions:  RTC interrupt handler
**
** parameters:   None
** Returned value:  None
**
************ ********* ********* ********* ********* ********* ********* ********* **/
void RTC_IRQHandler (void)
{
 RTC->ILR |= ILR_RTCCIF; //reset pending int flag 
 relogio_atual = RTCGetTime() ; //read RTC;
}

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTCInit
**
** Descriptions:  Initialize RTC timer
**
** parameters:   None
** Returned value:  None
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
void RTCInit( void )
{
  // Enable CLOCK into RTC
  SC->PCONP |= (1 << 9);

  //If RTC is stopped, clear STOP bit.
  if ( RTC->RTC_AUX & (0x1<<4) )
  {
 RTC->RTC_AUX |= (0x1<<4);
  }

  //--- Initialize registers ---
  RTC->AMR = 0;
  RTC->CIIR = 1; //INT every 1 sec;
  RTC->CCR = 0;
  return;
}

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTCStart
**
** Descriptions:  Start RTC timer
**
** parameters:   None
** Returned value:  None
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
void RTCStart( void ) 
{
 /*--- Start RTC counters ---*/
 RTC->CCR |= CCR_CLKEN;
 RTC->ILR = ILR_RTCCIF;
}

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTCStop
**
** Descriptions:  Stop RTC timer
**
** parameters:   None
** Returned value:  None
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
void RTCStop( void )
{   
 /*--- Stop RTC counters ---*/
 RTC->CCR &= ~CCR_CLKEN;
} 

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTC_CTCReset
**
** Descriptions:  Reset RTC clock tick counter
**
** parameters:   None
** Returned value:  None
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
void RTC_CTCReset( void )
{   
 RTC->CCR |= CCR_CTCRST;
}

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTCSetTime
**
** Descriptions:  Setup RTC timer value
**
** parameters:   None
** Returned value:  None
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
unsigned char RTCSetTime( RTCTime Time ) 
{
	if (Time.Sec > 59 ||
		Time.Min > 59 ||
		Time.Hour > 23 ||
		Time.Wday > 6 ||
		Time.Mday == 0 || Time.Mday > 31 ||
		Time.Yday == 0 || Time.Yday > 366 ||
		Time.Mon == 0 || Time.Mon > 12 ||
		Time.Year > 4095)
		return 0;

	RTC->SEC = Time.Sec;
	RTC->MIN = Time.Min;
	RTC->HOUR = Time.Hour;
	RTC->DOM = Time.Mday;
	RTC->DOW = Time.Wday;
	RTC->DOY = Time.Yday;
	RTC->MONTH = Time.Mon;
	RTC->YEAR = Time.Year;    

	return 1;
}

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTCSetAlarm
**
** Descriptions:  Initialize RTC timer
**
** parameters:   None
** Returned value:  None
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
void RTCSetAlarm( RTCTime Alarm ) 
{   
 RTC->ALSEC = Alarm.Sec;
 RTC->ALMIN = Alarm.Min;
 RTC->ALHOUR = Alarm.Hour;
 RTC->ALDOM = Alarm.Mday;
 RTC->ALDOW = Alarm.Wday;
 RTC->ALDOY = Alarm.Yday;
 RTC->ALMON = Alarm.Mon;
 RTC->ALYEAR = Alarm.Year;    
}

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTCGetTime
**
** Descriptions:  Get RTC timer value
**
** parameters:   None
** Returned value:  The data structure of the RTC time table
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
RTCTime RTCGetTime( void ) 
{
 RTCTime LocalTime;
 
 LocalTime.Sec = RTC->SEC;
 LocalTime.Min = RTC->MIN;
 LocalTime.Hour = RTC->HOUR;
 LocalTime.Mday = RTC->DOM;
 LocalTime.Wday = RTC->DOW;
 LocalTime.Yday = RTC->DOY;
 LocalTime.Mon = RTC->MONTH;
 LocalTime.Year = RTC->YEAR;
 return ( LocalTime );    
}

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTCSetAlarmMask
**
** Descriptions:  Set RTC timer alarm mask
**
** parameters:   Alarm mask setting
** Returned value:  None
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
void RTCSetAlarmMask( unsigned int AlarmMask ) 
{
 /*--- Set alarm mask ---*/    
 RTC->AMR = AlarmMask;
}