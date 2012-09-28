/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "ld.h"
#include "lpc17xx.h"

extern volatile unsigned char 	MODBUS_MASTER; // 0 = Slave, 1 = Master
extern volatile int 			MODBUS_REGISTER[32];
extern struct MB_Device 		modbus_master;
extern RTC_Time 				RTC_Now;
extern volatile unsigned int 	I_SerialReady;
extern volatile unsigned int 	I_SerialTimeout;
extern volatile unsigned int 	I_SerialAborted;

extern struct ip_addr 			IP_ADDRESS;
extern struct ip_addr 			IP_NETMASK;
extern struct ip_addr 			IP_GATEWAY;
extern struct ip_addr			IP_DNS;

//char 							SNTP_SERVER_ADDRESS[] = "servidor.local";
//char 							SNTP_SERVER_ADDRESS[] = "br.pool.ntp.org";
char 							SNTP_SERVER_ADDRESS[] = "192.168.0.5";

int								SNTP_GMT = -3;
int								SNTP_DAILY_SAVE = 0;

#define PLC_DAC_RAMPA_VALIDACAO

#if defined(PLC_NULL)
#include "POPTools/PLC_NULL.h"
#elif defined(PLC_TESTE)
#include "POPTools/PLC_TESTE.h"
#elif defined(PLC_ERR_LED)
#include "POPTools/PLC_ERR_LED.h"
#elif defined(PLC_RTC_BUG)
#include "POPTools/PLC_RTC_BUG.h"
#elif defined(PLC_RS485)
#include "POPTools/PLC_RS485.h"
#elif defined(PLC_USS)
#include "POPTools/PLC_USS.h"
#elif defined(PLC_TESTE_RS485)
#include "POPTools/PLC_TESTE_RS485.h"
#elif defined(PLC_NS600)
#include "POPTools/PLC_NS600.h"
#elif defined(PLC_APLANPP)
#include "POPTools/PLC_APLANPP.h"
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
#elif defined(PLC_DAC_RAMPA_VALIDACAO)
#include "POPTools/PLC_DAC_RAMPA_VALIDACAO.h"
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
#elif defined(PLC_SCADA)
#include "POPTools/PLC_SCADA.h"
#else
// Variaveis PLC
volatile int I_mcr = 0;

void PLC_Run(void)
{
    I_mcr = 1;
}
#endif

void PLC_Init(void)
{
	I_SerialReady = 1;
	MODBUS_MASTER = 0;

	RS485_Config(19200, 8, 2, 1);
	//RS485_Config(9600, 8, 0, 1);

	IP4_ADDR(&IP_ADDRESS, 192,168,0,254);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,10);
	IP4_ADDR(&IP_DNS, 192,168,0,10);

	/*IP4_ADDR(&IP_ADDRESS, 192,168,0,254);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,10);
	IP4_ADDR(&IP_DNS, 192,168,0,10);*/

	ADC_SetMask(0x3F);
}

