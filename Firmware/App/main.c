
#include "system_lpc17xx.h"
#include "coos.h"

#include "net.h"
#include "rtc.h"
#include "rs232.h"
#include "rs485.h"
#include "i2c.h"
#include "ssp.h"
#include "gpio.h"
#include "adc.h"
#include "dac.h"
#include "ld.h"
#include "qei.h"
#include "ld.h"

extern OS_STK PLC_CycleStack[PLC_CYCLE_THREAD_STACKSIZE];

void Hardware_Init(void)
{
	SystemInit();

	RS232_Init();
	RS485_Init();

	RTC_Init();
	RTC_Start();

	//RTC_Time t;
	//memset(&t, 0, sizeof(t));
	//RTC_SetTime(t);

	I2C_Init( (unsigned int)I2CMASTER );

	ADC_Init();
	DAC_Init();
	QEI_Init();
	GPIO_Init();

}

int main()
{
	Hardware_Init();

	CoInitOS();

	PLC_Init();

	Net_Init();

	CoCreateTask(PLC_Cycle, 0,
			  PLC_CYCLE_THREAD_PRIO,
			  &PLC_CycleStack[PLC_CYCLE_THREAD_STACKSIZE-1],
			  PLC_CYCLE_THREAD_STACKSIZE);

	CoStartOS();

	while(1);
}
