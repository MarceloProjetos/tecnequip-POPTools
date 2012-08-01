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

#define 		DAC_CYCLE_INTERVAL	2
#define 		DAC_RESOLUTION		(4096 / 2)
#define         DAC_VOLTAGE         10000

void			DAC_Init (void);
unsigned int	DAC_Read (void);
void			DAC_Write(unsigned int val);

void 			DAC_Start(unsigned char up, unsigned char linear, unsigned char gaint, unsigned char gainr, int time, int value);

typedef struct DA_SetTag
{
	unsigned char	up;			// 1 = up, 0 = down
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
