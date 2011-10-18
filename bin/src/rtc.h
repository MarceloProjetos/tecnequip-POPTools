/*********** ********* ********* ********* ********* ********* ********* ********* ***
 *   rtc.h:  Header file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
************ ********* ********* ********* ********* ********* ********* ********* ***/

typedef struct {
    unsigned int Sec;     /* Second value - [0,59] */
    unsigned int Min;     /* Minute value - [0,59] */
    unsigned int Hour;    /* Hour value - [0,23] */
    unsigned int Mday;    /* Day of the month value - [1,31] */
    unsigned int Mon;     /* Month value - [1,12] */
    unsigned int Year;    /* Year value - [0,4095] */
    unsigned int Wday;    /* Day of week value - [0,6] */
    unsigned int Yday;    /* Day of year value - [1,365] */
} RTCTime;

#define IMSEC  0x00000001
#define IMMIN  0x00000002
#define IMHOUR  0x00000004
#define IMDOM  0x00000008
#define IMDOW  0x00000010
#define IMDOY  0x00000020
#define IMMON  0x00000040
#define IMYEAR  0x00000080

#define AMRSEC  0x00000001  /* Alarm mask for Seconds */
#define AMRMIN  0x00000002  /* Alarm mask for Minutes */
#define AMRHOUR  0x00000004  /* Alarm mask for Hours */
#define AMRDOM  0x00000008  /* Alarm mask for Day of Month */
#define AMRDOW  0x00000010  /* Alarm mask for Day of Week */
#define AMRDOY  0x00000020  /* Alarm mask for Day of Year */
#define AMRMON  0x00000040  /* Alarm mask for Month */
#define AMRYEAR  0x00000080  /* Alarm mask for Year */

#define PREINT_RTC 0x000001C8  /* Prescaler value, integer portion, 
        PCLK = 15Mhz */
#define PREFRAC_RTC 0x000061C0  /* Prescaler value, fraction portion, 
        PCLK = 15Mhz */

#define ILR_RTCCIF 0x01
#define ILR_RTCALF 0x02

#define CCR_CLKEN 0x01
#define CCR_CTCRST 0x02
#define CCR_CLKSRC 0x10


#define IDEAL_RTC_FREQ  32768000 //in mHz (milli Hz)
#define MAX_DELTA_FREQUENCY _VALUE (IDEAL_RTC_FREQ / ((1 << CALIBRATION_ VALUE_SIZE) - 1))
#define CALIBRATION_ VALUE_SIZE 17
#define CALDIR(n) (n << 17) //1 = Backward, 0 = Forward
#define CCALEN (1<<4)


void RTC_IRQHandler (void);
void RTCInit( void );
void RTCStart( void );
void RTCStop( void );
void RTC_CTCReset( void );
unsigned char RTCSetTime( RTCTime );
RTCTime RTCGetTime( void );
void RTCSetAlarm( RTCTime );
void RTCSetAlarmMask( unsigned int AlarmMask );
