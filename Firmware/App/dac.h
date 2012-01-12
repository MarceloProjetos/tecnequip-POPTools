/*****************************************************************************
 *   da.h:  Header file for Digital to Analog functions
 *
 *   Copyright(C) 2010, Tecnequip Tecnologia Ltda.
 *   All rights reserved.
 *
 *   History
 *   2010.04.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __DAC_H
#define __DAC_H

#define 		DAC_CYCLE_INTERVAL	10
#define 		DAC_RESOLUTION		(4096 / 2)

void			DAC_Init (void);
unsigned int	DAC_Read (void);
void			DAC_Write(unsigned int val);

unsigned int	DAC_CalcCurveUp(int time, int value, int i);
unsigned int	DAC_CalcCurveDown(int time, int value, int i);

unsigned int 	DAC_CalcLinearUp(int time, int value, int i);
unsigned int 	DAC_CalcLinearDown(int time, int value, int i);
unsigned int 	DAC_CalcGainDown(	int calc_time,
									int calc_initval,
									unsigned char calc_gaint,
									unsigned char calc_gainr,
									int i);

void 			DAC_CycleUp(void * pdata);
void 			DAC_CycleDown(void * pdata);

void 			DAC_StartUp(unsigned char linear, int time, int value);
void 			DAC_StartDown(unsigned char linear, unsigned char gaint, unsigned char gainr, int time, int value);

typedef struct DA_SetTag
{
	unsigned char	linear;		// 1 = linear, 0 = curve
	unsigned char 	gaint;		// time gain (%)
	unsigned char 	gainr;		// resolution gain (%)
	unsigned int 	time;
	int 		 	value;
} DA_Set;

#endif /* end __DAC_H */

/******************************************************************************
**                            End Of File
******************************************************************************/
