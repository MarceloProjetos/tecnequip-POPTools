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

unsigned int	DAC_CalcCurveUp(int time, int desloc, int i);
unsigned int	DAC_CalcCurveDown(int time, int desloc, int i);

unsigned int 	DAC_CalcLinearUp(int time, int desloc, int i);
unsigned int 	DAC_CalcLinearDown(int time, int desloc, int i);

void 			DAC_CycleUp(void * pdata);
void 			DAC_CycleDown(void * pdata);

void 			DAC_StartUp(int time, int desloc);
void 			DAC_StartDown(int time, int desloc);

typedef struct DA_SetTag
{
	unsigned int time;
	int 		 desloc;
} DA_Set;

#endif /* end __DAC_H */

/******************************************************************************
**                            End Of File
******************************************************************************/
