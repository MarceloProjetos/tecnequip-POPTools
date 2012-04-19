/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "ld.h"
#include <coocox.h>
#include "lpc17xx.h"

//const volatile unsigned int 	CYCLE_TIME = 10;
volatile unsigned int 			PLC_ERROR = 0;

extern volatile unsigned char 	MODBUS_MASTER; // 0 = Slave, 1 = Master
extern volatile int 			MODBUS_REGISTER[32];
extern struct MB_Device 		modbus_master;
extern volatile unsigned int 	GPIO_OUTPUT;
extern volatile unsigned int 	GPIO_INPUT;
extern RTC_Time 				RTC_Now;
extern volatile unsigned int 	I_SerialReady;

extern struct ip_addr 			IP_ADDRESS;
extern struct ip_addr 			IP_NETMASK;
extern struct ip_addr 			IP_GATEWAY;
extern struct ip_addr			IP_DNS;

//char 							SNTP_SERVER_ADDRESS[] = "servidor.local";
//char 							SNTP_SERVER_ADDRESS[] = "br.pool.ntp.org";
char 							SNTP_SERVER_ADDRESS[] = "192.168.0.5";

int								SNTP_GMT = -3;
int								SNTP_DAILY_SAVE = 0;

unsigned int					PLC_CycleStack[PLC_CYCLE_THREAD_STACKSIZE];

#define PLC_ENCODER

#if defined(PLC_NULL)
#include "POPTools/PLC_NULL.h"
#elif defined(PLC_ERR_LED)
#include "POPTools/PLC_ERR_LED.h"
#elif defined(PLC_RTC_BUG)
#include "POPTools/PLC_RTC_BUG.h"
#elif defined(PLC_RS485)
#include "POPTools/PLC_RS485.h"
#elif defined(PLC_TESTE_RS485)
#include "POPTools/PLC_TESTE_RS485.h"
#elif defined(PLC_RS232)
#include "POPTools/PLC_RS232.h"
#elif defined(PLC_MODBUS_MASTER)
#include "POPTools/PLC_MODBUS_MASTER.h"
#elif defined(PLC_MODBUS_SLAVE)
#include "POPTools/PLC_MODBUS_SLAVE.h"
#elif defined(PLC_COLUNAN)
#include "POPTools/PLC_COLUNAN.h"
#elif defined(PLC_BOBINADOR_MASTER)
#include "POPTools/PLC_BOBINADOR_MASTER.h"
#elif defined(PLC_ADC)
#include "POPTools/PLC_ADC.h"
#elif defined(PLC_ADC_LED)
#include "POPTools/PLC_ADC_LED.h"
#elif defined(PLC_LED_BLINK)	// Pisca led
#include "POPTools/PLC_LED_BLINK.h"
#elif defined(PLC_CONTACT)
#include "POPTools/PLC_CONTACT.h"
#elif defined(PLC_DA)
#include "POPTools/PLC_DA.h"
#elif defined(PLC_CPU_LED)
#include "POPTools/PLC_CPU_LED.h"
#elif defined(PLC_DAC_RAMPA_LINEAR)
#include "POPTools/PLC_DAC_RAMPA_LINEAR.h"
#elif defined(PLC_DAC_RAMPA_LINEAR_RECUAR)
#include "POPTools/PLC_DAC_RAMPA_LINEAR_RECUAR.h"
#elif defined(PLC_DAC_RAMPA_CURVA)
#include "POPTools/PLC_DAC_RAMPA_CURVA.h"
#elif defined(PLC_DAC_RAMPA_CURVA_RECUAR)
#include "POPTools/PLC_DAC_RAMPA_CURVA_RECUAR.h"
#elif defined(PLC_ENCODER)
#include "POPTools/PLC_ENCODER.h"
#else
// Variaveis PLC
volatile int I_mcr = 0;

void PLC_Run(void)
{
    I_mcr = 1;
}
#endif

/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PLC_Cycle(void *pdata)
{
	U64 start_tick, diff_tick;
	StatusType s;

	for (;;)
	{
		start_tick = OSTickCnt;

		GPIO_Output(GPIO_OUTPUT);
		GPIO_INPUT = GPIO_Input();

		RS232_Console();
		RS485_Handler();

		PLC_Run();

		//CoTickDelay(10);

		diff_tick = 10 - (OSTickCnt - start_tick);
		s = CoTickDelay(diff_tick > 10 ? 10 : diff_tick);
		//s = CoTimeDelay(0, 0, 0, CYCLE_TIME);

		if (s != E_OK)
			PLC_ERROR |= 1 << 20;
		else
			PLC_ERROR &= ~(1 << 20);

	}
}

void PLC_Init(void)
{
	I_SerialReady = 1;
	MODBUS_MASTER = 0;

	RS485_Config(19200, 8, 2, 1);
	//RS485_Config(9600, 8, 0, 1);

	IP4_ADDR(&IP_ADDRESS, 192,168,0,235);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,10);
	IP4_ADDR(&IP_DNS, 192,168,0,10);

	/*IP4_ADDR(&IP_ADDRESS, 192,168,0,254);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,10);
	IP4_ADDR(&IP_DNS, 192,168,0,10);*/
}

