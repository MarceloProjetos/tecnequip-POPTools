
#include "system_lpc17xx.h"
#include <coocox.h>

#include "net.h"
#include "rtc.h"
#include "rs232.h"
#include "rs485.h"
#include "i2c.h"
#include "ssp.h"
#include "gpio.h"
#include "adc.h"
#include "dac.h"
#include "qei.h"
#include "ld.h"

#include <time.h>
#include <stdlib.h>

void Devices_Init(void)
{
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

void Hardware_Init(void)
{
	SystemInit();

	RS232_Init();
	RS485_Init();
}

// Task list
// See lwip-1.4.0/port/CoOS/lwipopts.h for stack size and priority
//
// TaskID - Prio - Task Name
//   0    -  64  - CoOS Idle task
//   1    -  25  - tcpip_thread task
//   2    -  26  - ethernet_if_input task
//   3    -  54  - HTTP_Connect task
//   4    -  59  - SNTP_Thread task
//   5    -  30  - Modbus_TCP_Connect
//   6    -  15  - PLC_Cycle
//   7    -  10  - DAC_StartUp/DAC_StartDown task

unsigned int					PLC_CycleStack[PLC_CYCLE_THREAD_STACKSIZE];

extern void PLC_Run   (void);
extern void PLC_Init  (void);
extern void ADC_Update(void);

volatile unsigned int 			PLC_ERROR = 0;

struct tm RTC_NowTM;

#define CYCLE_TIME 10

/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PLC_Cycle(void *pdata)
{
	U64 end_tick, cycle = CoGetOSTime()/CYCLE_TIME;

	for (;;)
	{
		end_tick = (cycle+1)*CYCLE_TIME;

		// Etapa de Aquisicao
		ADC_Update();
		RTC_NowTM = RTC_GetTM();
		GPIO_Input();

		// Etapa de Execucao
		PLC_Run();

		// Etapa de Atualizacao
		GPIO_Output();

		do{
			RS232_Console(cycle);
			RS485_Handler(cycle);

			CoTickDelay(1);
		} while(CoGetOSTime() < end_tick);

		cycle++;
	}
}

int main()
{
	Hardware_Init();

	CoInitOS();

	PLC_Init();

	Devices_Init();

	Modbus_Init();

	Net_Init();

	CoCreateTask(PLC_Cycle, 0,
			  PLC_CYCLE_THREAD_PRIO,
			  &PLC_CycleStack[PLC_CYCLE_THREAD_STACKSIZE-1],
			  PLC_CYCLE_THREAD_STACKSIZE);

	srand(time(NULL));

	CoStartOS();

	while(1);
}
