/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "ld.h"


extern volatile unsigned char 	MODBUS_MASTER; // 0 = Slave, 1 = Master
extern volatile int 			MODBUS_REGISTER[32];
extern struct 					MB_Device modbus_master;
extern struct tm 				RTC_NowTM;
struct tm 						RTC_StartTM;
struct tm 						RTC_EndTM;
extern volatile unsigned int 	I_SerialReady;
extern volatile unsigned int 	I_SerialTimeout;
extern volatile unsigned int 	I_SerialAborted;
extern volatile unsigned int 	I_RampActive;

extern struct ip_addr 			IP_ADDRESS;
extern struct ip_addr 			IP_NETMASK;
extern struct ip_addr 			IP_GATEWAY;
extern struct ip_addr			IP_DNS;

char 							SNTP_SERVER_ADDRESS[] = "br.pool.ntp.org";
int								SNTP_GMT = -3;
int								SNTP_DAILY_SAVE = 0;

// Variaveis PLC
volatile int ArrayBitUser_Count = 0;
volatile int ArrayBitUser[0];
volatile int ArrayBitSystem_Count = 1;
volatile int ArrayBitSystem[1];
volatile int ArrayIntUser_Count = 1;
volatile int ArrayIntUser[1];

void PLC_Run(void)
{
    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* start rung 1 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 2; else ArrayBitSystem[0] &= ~(1UL << 2); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $oneShot_0000
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $oneShot_0000 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[0] = 10; // pwm
    }

    /* ] finish series */

    /* start rung 2 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[0] = MODBUS_REGISTER[0]; // duty_cycle = pwm
    }

    /* ] finish series */

    /* start rung 3 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr
    
    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // ativar
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitSystem[0] |= 1UL << 4; // $oneShot_0001 = 1
        PWM_Set(ArrayIntUser[0], 1);
    } else if (((ArrayBitSystem[0] >> 4) & 1)) {  // $oneShot_0001
        ArrayBitSystem[0] &= ~(1UL << 4); // $oneShot_0001 = 0
        PWM_Set(0, 0);
    }

    /* ] finish series */
}

void PLC_Init(void)
{
	I_SerialReady = 1;
	MODBUS_MASTER = 0;
	ModBUS_SetID(0);

	RS485_Config(9600, 8, 0, 1);

	IP4_ADDR(&IP_ADDRESS, 192,168,0,254);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,1);
	IP4_ADDR(&IP_DNS, 192,168,0,1);

    QEIConfig.CaptureMode = QEI_CAPMODE_4X;
    ADC_SetMask(0);
    SSI_Init(24, 0);
    PWM_Init();
}
