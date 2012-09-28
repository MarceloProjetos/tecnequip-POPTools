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
#include <stdlib.h>
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

OS_STK	DACCycleStack[DAC_CYCLE_THREAD_STACKSIZE];

OS_MutexID		DAC_Mutex;
unsigned int 	DAC_Value = 0;

DA_Set GlobalDA;

#define DAC_MODE_RAW 0
#define DAC_MODE_MV  1
#define DAC_MODE_PCT 2
extern int MODBUS_REGISTER[32];
unsigned int DAC_Conv(int val, unsigned int mode)
{
	int ret;
	switch(mode) {
	case DAC_MODE_RAW:
		ret = val + DAC_RESOLUTION;
		break;

	case DAC_MODE_MV:
		ret = ((val * DAC_RESOLUTION) / DAC_VOLTAGE) + DAC_RESOLUTION;
		break;

	case DAC_MODE_PCT:
		ret = ((val * DAC_RESOLUTION) / 100) + DAC_RESOLUTION;
		break;

	default:
		ret = DAC_RESOLUTION;
	}

	if(ret < 0)
		ret = 0;
	else if (ret >= 2*DAC_RESOLUTION)
		ret = 2*DAC_RESOLUTION - 1;

	return (unsigned int)ret;
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

#ifdef RAMP_USING_FLOAT
unsigned int DAC_CalcLinearUp(int time, int value, int tm)
{
	if (value > DAC_RESOLUTION ||
			value < (DAC_RESOLUTION * -1))
		return 0;

	float t = (float)(time);
	float d = (float)(value);

	float a = d / t; // coeficiente angular
	float l = 0.0f; // coeficiente linear

	if (tm < time)
	{
		return DAC_RESOLUTION + ((tm * a) + l); // y = a.x + b
	}

	return DAC_RESOLUTION;
}

unsigned int DAC_CalcLinearDown(int time, int value, int tm)
{
	if (value > DAC_RESOLUTION ||
		value < (DAC_RESOLUTION * -1))
		return 0;

	float t = (float)(time);
	float d = (float)(value);

	float a = d / t; // abs(d) / t; // coeficiente angular
	float l = (float)(value); // coeficiente linear

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

unsigned int DAC_CalcGainDown(int calc_time, int calc_initval, unsigned char calc_gaint, unsigned char calc_gainr, int tm)
{
	if (calc_initval > DAC_RESOLUTION - 1 ||
		calc_initval < (DAC_RESOLUTION * -1))
		return 0;

	float t = (float)(calc_time);
	float d = (float)(calc_initval);

	// calcula os pontos da linha da curva de ganho proporcional
	float gd = d * ((float)(calc_gainr) / 100.0f);	// % altura (y) da curva de ganho
	float gt = t * ((float)(calc_gaint) / 100.0f);	// % largura (x) do tempo
	float ga;	// coeficiente angular (a = DeltaY / DeltaX)
	float gb;					// coeficiente linear (b = y - ax)

	if (tm < gt) {
		ga = - gd/gt;	// coeficiente angular (a = DeltaY / DeltaX)
		gb = d;					// coeficiente linear (b = y - ax)
	} else if(tm < (t - gt)) {
		// calcula os pontos da linha de aceleração/desaceleração
		ga = (2*gd - d) / (t - 2*gt);											// coeficiente angular (a = DeltaY / DeltaX)
		gb = (d - gd) - (ga * gt);													// coeficiente linear (b = y - ax)
	} else {
		// calcula os pontos da linha da curva de ganho
		ga = - gd/gt;			// coeficiente angular (a = DeltaY / DeltaX)
		gb = gd - (ga * (t - gt));		// coeficiente linear (b = y - ax)
	}

	return ((tm * ga) + gb) + 2048; // y = a.x + b
}


unsigned int DAC_CalcGainUp(int calc_time, int calc_initval, unsigned char calc_gaint, unsigned char calc_gainr, int tm)
{
	return DAC_CalcLinearUp(calc_time, calc_initval, tm);
}
#else
unsigned int DAC_CalcGain(unsigned char up, int calc_time, int calc_initval, unsigned char calc_gaint, unsigned char calc_gainr, int tm)
{
	if (abs(calc_initval) >  2*DAC_RESOLUTION - 1)
		return 0;

	int VarDA;

	int DeltaXGanho  = (calc_time    * calc_gaint) / 100;
	int DeltaYGanho  = (calc_initval * calc_gainr) / 100;

	int DeltaXLinear = calc_time    - 2*DeltaXGanho;
	int DeltaYLinear = calc_initval - 2*DeltaYGanho;

	if (tm < DeltaXGanho) {
		VarDA = (DeltaYGanho * tm) / DeltaXGanho;
	} else if(tm < (calc_time - DeltaXGanho)) {
		VarDA = DeltaYGanho + (DeltaYLinear * (tm - DeltaXGanho)) / DeltaXLinear;
	} else {
		VarDA = calc_initval - (DeltaYGanho * (calc_time - tm)) / DeltaXGanho;
	}

	if(up)
		return                VarDA + DAC_RESOLUTION;
	else
		return calc_initval - VarDA + DAC_RESOLUTION;
}

#define DAC_CalcGainUp(calc_time,calc_initval,calc_gaint,calc_gainr,tm)   DAC_CalcGain(1,calc_time,calc_initval,calc_gaint,calc_gainr,tm)
#define DAC_CalcGainDown(calc_time,calc_initval,calc_gaint,calc_gainr,tm) DAC_CalcGain(0,calc_time,calc_initval,calc_gaint,calc_gainr,tm)
#endif

unsigned int DAC_CalcCurveUp(int time, int value, int tm)
{
	float factor = 0;

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

	return DAC_RESOLUTION + (value * 2 * factor) / 1000.0f;
}

unsigned int DAC_CalcCurveDown(int time, int value, int tm)
{
	float factor = 0;

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

	return DAC_RESOLUTION + (value * 2 * factor) / 1000.0f;
}

void DAC_Cycle(void * pdata)
{
	char ramp_ON   = 0;
	unsigned int i = 0;
	int start = 0, offset = 0;

	DA_Set local;
	local.ID = 0;

	while(1) {
		if(GlobalDA.ID != local.ID) {
			local = GlobalDA;

			if((local.time >= DAC_CYCLE_INTERVAL * 3 && local.time <= 10000) &&
			   (local.value >= -DAC_RESOLUTION && local.value <= DAC_RESOLUTION-1)) {
				i       = 0;
				start   = 0;
				offset  = 0;
				ramp_ON = 1;

				if(local.up == 2) { // Modo coincidente
					local.up = 1;
					start  = DAC_Read() - DAC_RESOLUTION;
					offset = local.value - start;
				}
			}
		}

		if(ramp_ON) {
			if(local.up) {
				if (local.linear)
					DAC_Write(DAC_CalcGainUp   (local.time, offset ? offset : local.value, local.gaint, local.gainr, i) + start);
				else
					DAC_Write(DAC_CalcCurveUp  (local.time, offset ? offset : local.value,                           i) + start);
			} else {
				if (local.linear)
					DAC_Write(DAC_CalcGainDown (local.time, offset ? offset : local.value, local.gaint, local.gainr, i) + start);
				else
					DAC_Write(DAC_CalcCurveDown(local.time, offset ? offset : local.value,                           i) + start);
			}

			i += DAC_CYCLE_INTERVAL;

			if(i >= local.time) {
				ramp_ON = 0;
				DAC_Write((local.up ? local.value : 0) + DAC_RESOLUTION);
			}
		}

		CoTickDelay(DAC_CYCLE_INTERVAL);
	}

	CoExitTask();
}

void DAC_Start(unsigned char up, unsigned char linear, unsigned char gaint, unsigned char gainr, int time, int value)
{
	GlobalDA.up     = up;
	GlobalDA.linear = linear;

	GlobalDA.gaint  = gaint;
	GlobalDA.gainr  = gainr;

	GlobalDA.time   = time;
	GlobalDA.value  = value - DAC_RESOLUTION;

	GlobalDA.ID++;
}

void DAC_Init(void)
{
  //if ( I2CInitFast( (unsigned int)I2CMASTER ) == FALSE )  /* initialize I2c */
  if ( I2C_InitFast( (unsigned int)I2CMASTER ) == FALSE )  /* initialize I2c */
    return;

  DAC_Mutex = CoCreateMutex();

  // Inicializa o D/A para estar em zero volts ao inves de -10V
  DAC_Write(DAC_RESOLUTION);

  // Configure XTR300 I/Os
  PINCON->PINSEL3 &= ~0x00FC0000;  /* set PIO1.25  -  PIO1.27 to GPIO */
  PINCON->PINSEL7 &= ~0x003C0000;  /* set PIO3.25 and PIO3.26 to GPIO */

  GPIO1->FIODIR   |=  0x08000000;  /* set PIO1.27 to output */
  GPIO1->FIOCLR   |=  0x08000000;  /* set PIO1.27 to low  */

  GPIO3->FIODIR   |=  0x02000000;  /* set PIO3.25 to output */
  GPIO3->FIOSET   |=  0x02000000;  /* set PIO3.25 to high */

  GlobalDA.ID = 0;

  CoCreateTask(DAC_Cycle, NULL,
		  	   DAC_CYCLE_THREAD_PRIO,
		  	   &DACCycleStack[DAC_CYCLE_THREAD_STACKSIZE-1],
		  	   DAC_CYCLE_THREAD_STACKSIZE);
}

