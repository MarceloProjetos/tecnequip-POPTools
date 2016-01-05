
#include "system_lpc17xx.h"
#include <coocox.h>

#include "net.h"
#include "rtc.h"
#include "rs232.h"
#include "rs485.h"
#include "i2c.h"
#include "ssp.h"
#include "gpio.h"
#include "XP_lcd.h"

#include <time.h>
#include <stdlib.h>

void Devices_Init(void)
{
	RTC_Init();
	RTC_Start();

	I2C_Init( (unsigned int)I2CMASTER );

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

volatile unsigned int 			PLC_ERROR = 0;

struct tm RTC_NowTM;

#define CYCLE_TIME 10

extern struct MODBUS_Device modbus_tcp;
extern volatile unsigned int I_TcpTimeout;

extern volatile unsigned char WAITING_FOR_FMTSTR;

/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PLC_Cycle(void *pdata)
{
	U64 end_tick, cycle = CoGetOSTime()/CYCLE_TIME;

	for (;;)
	{
		end_tick = (cycle+1)*CYCLE_TIME;

		// Etapa de Aquisicao
		RTC_NowTM = RTC_GetTM();
		GPIO_Input();

		// Etapa de Execucao

		// Sempre limpamos a flag e, se houver alguma leitura de formatted string ativa,
		// a flag sera ligada novamente. Assim garantimos que se nenhum objeto estiver
		// aguardando por uma string a serial vai usar os outros protocolos
		WAITING_FOR_FMTSTR = 0;

		PLC_Run();

		// Etapa de Atualizacao
		GPIO_Output();

		do{
			RS232_Console(cycle);
			RS485_Handler(cycle);

			if(MBTCP_Transfer.status == MBTCP_STATUS_DONE) {
				if(!I_TcpTimeout) {
					Modbus_Request(&modbus_tcp, MBTCP_Transfer.buf, MBTCP_Transfer.bufsize);
				}
				MBTCP_Transfer.status = MBTCP_STATUS_READY;
			}

			CoTickDelay(1);
		} while(CoGetOSTime() < end_tick);

		cycle++;
	}
}

int main()
{
	Hardware_Init();

	CoInitOS();

	Devices_Init();

	PLC_Init();

	XP_lcd_Init(XP_LCD_TYPE_SC2004A);

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
