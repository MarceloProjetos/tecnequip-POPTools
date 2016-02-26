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
#include "chip.h"
//#include "type.h"
#include "stdio.h"
#include "rtc.h"

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTCInit
**
** Descriptions:  Initialize RTC timer
**
** parameters:   None
** Returned value:  None
** 
************ ********* ********* ********* ********* ********* ********* ********* **/
void RTC_Init( void )
{
  // Enable CLOCK into RTC
  Chip_RTC_Init(LPC_RTC);
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
void RTC_Start( void )
{
	/*--- Start RTC counters ---*/
	Chip_RTC_Enable(LPC_RTC, ENABLE);
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
void RTC_Stop( void )
{   
	/*--- Stop RTC counters ---*/
	Chip_RTC_Enable(LPC_RTC, DISABLE);
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
	Chip_RTC_ResetClockTickCounter(LPC_RTC);
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
unsigned char RTC_SetTime( RTC_Time Time )
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

	RTC_TIME_T fulltime;

	fulltime.time[0] = Time.Sec;
	fulltime.time[1] = Time.Min;
	fulltime.time[2] = Time.Hour;
	fulltime.time[3] = Time.Mday;
	fulltime.time[4] = Time.Wday;
	fulltime.time[5] = Time.Yday;
	fulltime.time[6] = Time.Mon;
	fulltime.time[7] = Time.Year;

	Chip_RTC_SetFullTime(LPC_RTC, &fulltime);

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
void RTC_SetAlarm( RTC_Time Alarm )
{   
/* RTC->ALSEC = Alarm.Sec;
 RTC->ALMIN = Alarm.Min;
 RTC->ALHOUR = Alarm.Hour;
 RTC->ALDOM = Alarm.Mday;
 RTC->ALDOW = Alarm.Wday;
 RTC->ALDOY = Alarm.Yday;
 RTC->ALMON = Alarm.Mon;
 RTC->ALYEAR = Alarm.Year;*/
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
RTC_Time RTC_GetTime( void )
{
	RTC_TIME_T fulltime;
	RTC_Time LocalTime;

	Chip_RTC_GetFullTime(LPC_RTC, &fulltime);

	LocalTime.Sec  = fulltime.time[0];
	LocalTime.Min  = fulltime.time[1];
	LocalTime.Hour = fulltime.time[2];
	LocalTime.Mday = fulltime.time[3];
	LocalTime.Wday = fulltime.time[4];
	LocalTime.Yday = fulltime.time[5];
	LocalTime.Mon  = fulltime.time[6];
	LocalTime.Year = fulltime.time[7];

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
void RTC_SetAlarmMask( unsigned int AlarmMask )
{
 /*--- Set alarm mask ---*/    
// RTC->AMR = AlarmMask;
}

struct tm * AdjustDate(struct tm timeinfo, int mode)
{
	int last_day;
	time_t rawtime = 0;
	struct tm tm_tmp, *ptr;

	// recebe referencia a estrutura tm do sistema
	ptr = localtime(&rawtime);

	// Ajuste de mes e ano para o caso de ter sido escolhido "Todos". No caso, digitado zero.
	if(!timeinfo.tm_mon ) timeinfo.tm_mon  = (mode == RTC_GETDATE_MODE_START ?    1 :   12);
	if(!timeinfo.tm_year) timeinfo.tm_year = (mode == RTC_GETDATE_MODE_START ? 1970 : 2037);

	// Atualizacao da estrutura tm com os valores esperados
	timeinfo.tm_year -= 1900;
	timeinfo.tm_mon  -= 1;

	// Ajusta de dia para o caso de ter sido escolhido "Todos". No caso, digitado zero.
	if(!timeinfo.tm_mday) {
		if(timeinfo.tm_mon == 11) { // Dezembro
			last_day = 31;
		} else {
			tm_tmp = timeinfo;
			tm_tmp.tm_mon++;
			tm_tmp.tm_mday = 1;
			tm_tmp.tm_hour = 0;
			rawtime = mktime(&tm_tmp) - 3600; // Hora definida - 1 hora, volta para o mes anterior
			tm_tmp = *localtime(&rawtime);
			last_day = tm_tmp.tm_mday;
		}

		timeinfo.tm_mday = (mode == RTC_GETDATE_MODE_START ? 1 : last_day);
	}

	// Retorna estrutura com a data
	*ptr = timeinfo;
	return ptr;
}

// Tempo em segundos no dia
static int GetHourOffset(struct tm timeinfo)
{
	time_t now;

	// Primeiro configura o dia para uma data conhecida. O que nos importa aqui eh o tempo em segundos desde o inicio do dia
	timeinfo.tm_mon  =  0;
	timeinfo.tm_yday =  0;
	timeinfo.tm_year = 70;
	timeinfo.tm_mday =  1;
	timeinfo.tm_wday =  4;

	now = mktime(&timeinfo);

	timeinfo.tm_hour = timeinfo.tm_min = timeinfo.tm_sec = 0;

	return now - mktime(&timeinfo);
}

/*********** ********* ********* ********* ********* ********* ********* ********* ***
** Function name:  RTC_GetTM
**
** Descriptions:  Get RTC timer value
**
** parameters:   None
** Returned value:  The standard data structure tm containing current date/hour
**
************ ********* ********* ********* ********* ********* ********* ********* **/
struct tm RTC_GetTM( void )
{
	struct tm now;
	RTC_Time LocalTime;

	LocalTime = RTC_GetTime();

	now.tm_sec  = LocalTime.Sec;
	now.tm_min  = LocalTime.Min;
	now.tm_hour = LocalTime.Hour;
	now.tm_mday = LocalTime.Mday;
	now.tm_wday = LocalTime.Wday;
	now.tm_yday = LocalTime.Yday;
	now.tm_mon  = LocalTime.Mon;
	now.tm_year = LocalTime.Year;

	return *AdjustDate(now, RTC_GETDATE_MODE_START);
}

int RTC_OutputState(struct tm start, struct tm end, struct tm now, int mode, int mask_wday)
{
	int previous_wday_on;
	time_t tstart, tend, tnow;
	int ret=0, seg = -1, seg_previous = -1, i, offset_start, offset_end, offset_now, first_wday[4], last_wday[4], segs_wday = 0, first_found;

	offset_start = GetHourOffset(start);
	offset_end   = GetHourOffset(end  );
	offset_now   = GetHourOffset(now  );

	tstart = mktime(&start);
	tend   = mktime(&end);
	tnow   = mktime(&now);

	if(mode == RTC_MODE_DATE_CONTINUOUS) {
		if(tnow >= tstart && tnow < tend) {
			ret = 1;
		}
	} else if(mode == RTC_MODE_DATE_INTERMITTENT) {
		if(tnow >= tstart && tnow < tend &&
			(offset_start > offset_end ? (offset_now >= offset_start || offset_now < offset_end) :
			(offset_now >= offset_start && offset_now < offset_end))) {
			ret = 1;
		}
	} else {
		first_found = 0;
		for(i=0; i<7; i++) { // Loop entre todos os dias da semana
			if(!first_found && ((1 << i) & mask_wday)) {
				first_found = 1;
				first_wday[segs_wday] = i;
			}
			if(first_found && !((1 << (i+1)) & mask_wday)) {
				first_found = 0;
				last_wday[segs_wday++] = i;
			}
		}

		// Se o ultimo segmento termina no sabado e o primeiro comeca no domingo, eles fazem parte do mesmo segmento
		// Isso apenas se houver mais de 1 segmento senao indica que a semana inteira foi selecionada
		if(segs_wday > 1 && last_wday[segs_wday-1] == 6 && first_wday[0] == 0) {
			segs_wday--;
			first_wday[0] = first_wday[segs_wday];
		}

		for(i=0; i < segs_wday; i++) {
			if(first_wday[i] > last_wday[i]) {
				if(now.tm_wday >= first_wday[i] || now.tm_wday <= last_wday[i]) {
					seg = i;
					break;
				}
			} else  if(now.tm_wday >= first_wday[i] && now.tm_wday <= last_wday[i]) {
					seg = i;
					break;
			}
		}

		if(mode == RTC_MODE_WEEKDAY_CONTINUOUS) {
			if(seg >= 0 && (offset_now >= offset_start || now.tm_wday != first_wday[seg]) &&
				(offset_now < offset_end || now.tm_wday != last_wday[seg])) {
				ret = 1;
			}
		} else if(mode == RTC_MODE_WEEKDAY_INTERMITTENT) {
			previous_wday_on = now.tm_wday - 1;
			if(previous_wday_on < 0) previous_wday_on = 6;
			if(seg < 0 && offset_start > offset_end) {
				for(i=0; i < segs_wday; i++) {
					if(first_wday[i] > last_wday[i]) {
						if(previous_wday_on >= first_wday[i] || previous_wday_on <= last_wday[i]) {
							seg_previous = i;
							break;
						}
					} else  if(previous_wday_on >= first_wday[i] && previous_wday_on <= last_wday[i]) {
							seg_previous = i;
							break;
					}
				}
			}
			previous_wday_on = (1 << previous_wday_on) & mask_wday;

			if((seg >= 0 || seg_previous >= 0) && (offset_start > offset_end ? ((offset_now >= offset_start && seg>=0) || (offset_now < offset_end && previous_wday_on)) :
				(offset_now >= offset_start && offset_now < offset_end))) {
				ret = 1;
			}
		}
	}

	return ret;
}

#include <sys/time.h>

int _gettimeofday(struct timeval *tp, void *tzp)
{
	struct tm now;

	if(tp != NULL) {
		now = RTC_GetTM();

		tp->tv_sec  = mktime(&now);
		tp->tv_usec = 0;
	}

	return 0;
}
