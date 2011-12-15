/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include <string.h>
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "coos.h"

#include "rtc.h"
#include "gpio.h"
#include "rtc.h"
#include "dac.h"
#include "adc.h"
#include "qei.h"
#include "rs485.h"
#include "rs232.h"
#include "modbus.h"
#include "uss.h"
#include "yaskawa.h"
#include "format_str.h"

const volatile unsigned int 	CYCLE_TIME = 10;

extern volatile unsigned char 	MODBUS_MASTER; // 0 = Slave, 1 = Master
extern volatile int 			MODBUS_REGISTER[32];
extern struct 					MB_Device modbus_master;
extern volatile unsigned int 	GPIO_OUTPUT;
extern volatile unsigned int 	GPIO_INPUT;
extern RTC_Time 				RTC_Now;
extern volatile unsigned int 	I_SerialReady;

extern struct ip_addr 			IP_ADDRESS;
extern struct ip_addr 			IP_NETMASK;
extern struct ip_addr 			IP_GATEWAY;
extern struct ip_addr			IP_DNS;

char 							SNTP_SERVER_ADDRESS[] = "servidor.local";
int								SNTP_GMT = -3;
int								SNTP_DAILY_SAVE = 0;

OS_STK							PLC_CycleStack[PLC_CYCLE_THREAD_STACKSIZE];

// Variaveis PLC
//volatile int I_mcr = 0;

//#include "PLC_Null.inc"
#include "PLC_Comm_Test.inc"
//#include "PLC_ColunaN.inc"

/*void PLC_Run(void)
{
    I_mcr = 1;
}*/


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PLC_Cycle(void *pdata)
{
	for (;;)
	{
		GPIO_Output(GPIO_OUTPUT);
		GPIO_INPUT = GPIO_Input();

		RS485_Handler();

		PLC_Run();

		CoTickDelay(CYCLE_TIME);
	}
}

void PLC_Init(void)
{
	I_SerialReady = 1;
	MODBUS_MASTER = 0;

	RS485_Config(9600, 8, 0, 1);

	IP4_ADDR(&IP_ADDRESS, 10,0,0,6);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 10,0,0,1);
	IP4_ADDR(&IP_DNS, 10,0,0,2);
}

