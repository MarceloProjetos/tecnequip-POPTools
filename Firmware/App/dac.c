/*****************************************************************************
 *   da.c:  Digital to Analog functions
 *
 *   Copyright(C) 2010, Tecnequip Tecnologia Ltda.
 *   All rights reserved.
 *
 *   History
 *   2010.04.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include <string.h>
#include <math.h>
#include "lpc17xx.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "coos.h"
#include "i2c.h"
#include "dac.h"

#define TRUE	1
#define FALSE	0

extern volatile unsigned int timer0_counter;

extern volatile unsigned int I2CCount;
extern volatile unsigned char I2CMasterBuffer[BUFSIZE];
extern volatile unsigned int I2CCmd, I2CMasterState;
extern volatile unsigned int I2CReadLength, I2CWriteLength;

OS_STK	DACCycleStackUp[DAC_CYCLE_THREAD_STACKSIZE];
OS_STK	DACCycleStackDown[DAC_CYCLE_THREAD_STACKSIZE];

OS_MutexID		DAC_Mutex;
unsigned int 	DAC_Value = 0;

void DAC_Init(void)
{
  //if ( I2CInitFast( (unsigned int)I2CMASTER ) == FALSE )  /* initialize I2c */
  if ( I2C_InitFast( (unsigned int)I2CMASTER ) == FALSE )  /* initialize I2c */
    return;

  // Configure XTR300 I/Os
  PINCON->PINSEL3 &= ~0x00FC0000;  /* set PIO1.25  -  PIO1.27 to GPIO */
  PINCON->PINSEL7 &= ~0x003C0000;  /* set PIO3.25 and PIO3.26 to GPIO */

  GPIO1->FIODIR   |=  0x08000000;  /* set PIO1.27 to output */
  GPIO1->FIOCLR   |=  0x08000000;  /* set PIO1.27 to low  */

  GPIO3->FIODIR   |=  0x02000000;  /* set PIO3.25 to output */
  GPIO3->FIOSET   |=  0x02000000;  /* set PIO3.25 to high */

  DAC_Mutex = CoCreateMutex();

}

void DAC_Write(unsigned int val)
{
	// Checa estouro do D/A
	DAC_Value = val >= (1 << 12) ? (1 << 12) - 1 : val;

	memset((void *)I2CMasterBuffer, 0, BUFSIZE);

	CoEnterMutexSection(DAC_Mutex);

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = 0x98;
	I2CMasterBuffer[1] = (DAC_Value>>8) & 0x0F;
	I2CMasterBuffer[2] = (DAC_Value>>0) & 0xFF;
	I2C_Engine();

	CoLeaveMutexSection(DAC_Mutex);
}

unsigned int DAC_Read(void)
{
  return DAC_Value;
}
unsigned int DAC_CalcLinearUp(int time, int desloc, int i)
{
	int tm = 0;

	if (desloc > DAC_RESOLUTION ||
		desloc < (DAC_RESOLUTION * -1))
		return 0;

	float t = (float)(time);
	float d = (float)(desloc);

	float a = d / t; // coeficiente angular
	float l = 0.0f; // coeficiente linear

	tm = i * DAC_CYCLE_INTERVAL;
	if (tm < time)
	{
		return DAC_RESOLUTION + ((tm * a) + l); // y = a.x + b
	}

	return DAC_RESOLUTION;
}

unsigned int DAC_CalcLinearDown(int time, int desloc, int i)
{
	int tm = 0;

	if (desloc > DAC_RESOLUTION ||
		desloc < (DAC_RESOLUTION * -1))
		return 0;

	float t = (float)(time);
	float d = (float)(desloc);

	float a = d / t; // abs(d) / t; // coeficiente angular
	float l = (float)(desloc); // coeficiente linear

	tm = i * DAC_CYCLE_INTERVAL;
	if (tm < time)
	{
		if (d < 0)
		{
			return ((tm * a) + l); // y = a.x + b
		}
		else
		{
			a *= -1.0f;
			return ((tm * a) + l); // y = a.x + b
		}
	}
	return 0;
}

unsigned int DAC_CalcCurveUp(int time, int desloc, int i)
{
	int tm = 0;
	float factor = 0;

	tm = i * DAC_CYCLE_INTERVAL;
	if (tm < time)
	{
		if (tm < time / 2)
		{
			factor = pow((tm * 1000.0f) / time, 2);
		}
		else
		{
			factor = 500000.0f - pow(1000.0f - (tm * 1000.0f) / time, 2);
		}
		factor /= 1000.0f;
	}
	else
	{
		factor = 500.0f;
	}

	return DAC_RESOLUTION + (desloc * 2 * factor) / 1000.0f;
}

unsigned int DAC_CalcCurveDown(int time, int desloc, int i)
{
	int tm = 0;
	float factor = 0;

	tm = i * DAC_CYCLE_INTERVAL;
	if (tm < time)
	{
		if (tm < time / 2)
		{
			factor = 500000.0f - pow((tm * 1000.0f) / time, 2);
		}
		else
		{
			factor = pow(1000.0f - (tm * 1000.0f) / time, 2);
		}
		factor /= 1000.0f;
	}
	else
	{
		factor = 0.0f;
	}

	return DAC_RESOLUTION + (desloc * 2 * factor) / 1000.0f;
}

void DAC_CycleUp(void * pdata)
{
	unsigned int i;
	int count;

	unsigned int time = 0;
	int desloc = 0;

	time = ((DA_Set*)pdata)->time;
	desloc = ((DA_Set*)pdata)->desloc;

	if (time < DAC_CYCLE_INTERVAL * 2 || time > 3000)
		CoExitTask();

	if (desloc < -2048 || desloc > 2047)
		CoExitTask();

	count = (unsigned int)(time / DAC_CYCLE_INTERVAL);

	for (i = 0; i < count; i++)
	{
		DAC_Write(DAC_CalcCurveUp(time, desloc, i));
		//DAC_Write(DAC_CalcLinearUp(time, desloc, i));
		CoTickDelay(DAC_CYCLE_INTERVAL);
	}

	CoExitTask();
}

void DAC_CycleDown(void * pdata)
{
	unsigned int i;
	int count;

	unsigned int time = 0;
	int desloc = 0;

	time = ((DA_Set*)pdata)->time;
	desloc = ((DA_Set*)pdata)->desloc;

	if (time < DAC_CYCLE_INTERVAL * 2 || time > 3000)
		CoExitTask();

	if (desloc < -2048 || desloc > 2047)
		CoExitTask();

	count = (unsigned int)(time / DAC_CYCLE_INTERVAL);

	for (i = 0; i < count; i++)
	{
		DAC_Write(DAC_CalcCurveDown(time, desloc, i));
		//DAC_Write(DAC_CalcLinearDown(time, desloc, i));
		CoTickDelay(DAC_CYCLE_INTERVAL);
	}

	CoExitTask();
}

void DAC_StartUp(int time, int desloc)
{
	DA_Set da;

	da.time = time;
	da.desloc = desloc;

	CoCreateTask(DAC_CycleUp, &da,
				  DAC_CYCLE_THREAD_PRIO,
				  &DACCycleStackUp[DAC_CYCLE_THREAD_STACKSIZE-1],
				  DAC_CYCLE_THREAD_STACKSIZE);
}

void DAC_StartDown(int time, int desloc)
{
	DA_Set da;

	da.time = time;
	da.desloc = desloc;

	CoCreateTask(DAC_CycleDown, &da,
				  DAC_CYCLE_THREAD_PRIO,
				  &DACCycleStackDown[DAC_CYCLE_THREAD_STACKSIZE-1],
				  DAC_CYCLE_THREAD_STACKSIZE);
}

