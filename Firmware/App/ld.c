/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "ld.h"

const volatile unsigned int 	CYCLE_TIME = 10;

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

char 							SNTP_SERVER_ADDRESS[] = "servidor.local";
int								SNTP_GMT = -3;
int								SNTP_DAILY_SAVE = 0;

unsigned int					PLC_CycleStack[PLC_CYCLE_THREAD_STACKSIZE];

#define PLC_BLINK

#if defined(PLC_NULL)
#include "POPTools/PLC_Null.h"
#elif defined(PLC_RS485)
#include "POPTools/PLC_RS485.h"
#elif defined(PLC_RS232)
#include "POPTools/PLC_RS232.h"
#elif defined(PLC_COLUNAN)
#include "POPTools/PLC_ColunaN.h"
#elif defined(PLC_ADC)
#include "POPTools/PLC_ADC.h"
#elif defined(PLC_ADC_LED)
#include "POPTools/PLC_ADC_Led.h"
#elif defined(PLC_BLINK)	// Pisca led
#include "POPTools/PLC_Blink.h"
#elif defined(PLC_CONTACT)
#include "POPTools/PLC_Contact.h"
#elif defined(PLC_DA)
#include "POPTools/PLC_DA.h"
#elif defined(PLC_CPU_LED)
#include "POPTools/PLC_CPU_LED.h"
#elif defined(PLC_DAC_RAMPA_CURVA)
#include "POPTools/PLC_DAC_RAMPA_CURVA.h"
#elif defined(PLC_DAC_RAMPA_CURVA_RECUAR)
#include "POPTools/PLC_DAC_RAMPA_CURVA_RECUAR.h"
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
	for (;;)
	{
		GPIO_Output(GPIO_OUTPUT);
		GPIO_INPUT = GPIO_Input();

		RS232_Console();
		RS485_Handler();

		PLC_Run();

		//CoTickDelay(CYCLE_TIME);
		CoTimeDelay(0, 0, 0, CYCLE_TIME);
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

