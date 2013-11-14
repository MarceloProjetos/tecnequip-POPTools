/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "ld.h"

void Cycle_Init(void);

extern volatile unsigned char 	MODBUS_RS485_MASTER; // 0 = Slave, 1 = Master
extern volatile unsigned char 	MODBUS_TCP_MASTER; // 0 = Slave, 1 = Master
extern volatile int 			MODBUS_REGISTER[32];
extern struct tm 				RTC_NowTM;
struct tm 						RTC_StartTM;
struct tm 						RTC_EndTM;
extern volatile unsigned int 	I_SerialReady;
extern volatile unsigned int 	I_SerialTimeout;
extern volatile unsigned int 	I_SerialAborted;
extern volatile unsigned int 	I_RampActive;
extern volatile unsigned int 	I_TcpReady;
extern volatile unsigned int 	I_TcpTimeout;

extern struct ip_addr 			IP_ADDRESS;
extern struct ip_addr 			IP_NETMASK;
extern struct ip_addr 			IP_GATEWAY;
extern struct ip_addr			IP_DNS;

char 							SNTP_SERVER_ADDRESS[] = "br.pool.ntp.org";
int								SNTP_GMT = -3;
int								SNTP_DAILY_SAVE = 0;

// Variaveis PLC
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile int ArrayBitUser_Count = 1;
volatile int ArrayBitUser[1];
volatile int ArrayBitSystem_Count = 1;
volatile int ArrayBitSystem[1];
volatile int ArrayIntUser_Count = 4;
volatile int ArrayIntUser[4];

void PLC_Run(void)
{
    Cycle_Init();

    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* start rung 1 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $oneShot_0
            if (I_SerialReady) {  // $SerialReady
                ArrayIntUser[1] = Format_String_Read("SAIDA %d ON\r\n", &ArrayIntUser[0]);
                ArrayIntUser[2] = 0; // $scratchZero
                if (ArrayIntUser[1] > ArrayIntUser[2]) {
                    ArrayBitSystem[0] |= 1UL << 4; // $oneShot_0 = 1
                }
            }
        }
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $oneShot_0
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $oneShot_0 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 5) & 1)) {  // $oneShot_1
            if (I_SerialReady) {  // $SerialReady
                ArrayIntUser[1] = Format_String_Read("SAIDA %d ON\n", &ArrayIntUser[0]);
                ArrayIntUser[2] = 0; // $scratchZero
                if (ArrayIntUser[1] > ArrayIntUser[2]) {
                    ArrayBitSystem[0] |= 1UL << 5; // $oneShot_1 = 1
                }
            }
        }
        if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $oneShot_1
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $oneShot_1 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* finish parallel [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 0; else ArrayBitUser[0] &= ~(1UL << 0); // ligarSaida = $rung_top

    /* finish series [ */

    /* start rung 2 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 6) & 1)) {  // $oneShot_2
            if (I_SerialReady) {  // $SerialReady
                ArrayIntUser[1] = Format_String_Read("SAIDA %d OFF\r\n", &ArrayIntUser[0]);
                ArrayIntUser[2] = 0; // $scratchZero
                if (ArrayIntUser[1] > ArrayIntUser[2]) {
                    ArrayBitSystem[0] |= 1UL << 6; // $oneShot_2 = 1
                }
            }
        }
        if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $oneShot_2
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $oneShot_2 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 7) & 1)) {  // $oneShot_3
            if (I_SerialReady) {  // $SerialReady
                ArrayIntUser[1] = Format_String_Read("SAIDA %d OFF\n", &ArrayIntUser[0]);
                ArrayIntUser[2] = 0; // $scratchZero
                if (ArrayIntUser[1] > ArrayIntUser[2]) {
                    ArrayBitSystem[0] |= 1UL << 7; // $oneShot_3 = 1
                }
            }
        }
        if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $oneShot_3
    } else {
        ArrayBitSystem[0] &= ~(1UL << 7); // $oneShot_3 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* finish parallel [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // desligarSaida = $rung_top

    /* finish series [ */

    /* start rung 3 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // ligarSaida
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 1; // $scratch2
    if (ArrayIntUser[0] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        GPIO_OUTPUT_PORT1 = 1; // S1 = 1
    }

    /* finish series [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 2; // $scratch2
    if (ArrayIntUser[0] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        GPIO_OUTPUT_PORT2 = 1; // S2 = 1
    }

    /* finish series [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* finish parallel [ */
    /* finish series [ */

    /* start rung 4 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 1) & 1)) {  // desligarSaida
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 1; // $scratch2
    if (ArrayIntUser[0] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        GPIO_OUTPUT_PORT1 = 0; // S1 = 0
    }

    /* finish series [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 2; // $scratch2
    if (ArrayIntUser[0] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        GPIO_OUTPUT_PORT2 = 0; // S2 = 0
    }
    
    /* finish series [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* finish parallel [ */
    /* finish series [ */
}

void PLC_Net_Init(void)
{
}

void PLC_Init(void)
{
	MODBUS_RS485_MASTER = 0;
	MODBUS_TCP_MASTER = 0;
	ModBUS_SetID(0);
	ModBUS_SetAppName("POPTools");

	RS485_Config(9600, 8, 0, 1);

	IP4_ADDR(&IP_ADDRESS, 192,168,0,254);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,1);
	IP4_ADDR(&IP_DNS, 192,168,0,1);

    memset((void*)ArrayBitUser, 0, sizeof(ArrayBitUser));
    memset((void*)ArrayBitSystem, 0, sizeof(ArrayBitSystem));
    memset((void*)ArrayIntUser, 0, sizeof(ArrayIntUser));
}

void Cycle_Init(void)
{
}
