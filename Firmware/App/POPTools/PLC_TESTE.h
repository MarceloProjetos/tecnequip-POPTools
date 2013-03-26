/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "ld.h"


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
volatile unsigned char GPIO_INPUT_PORT1 = 0;
volatile unsigned char GPIO_INPUT_PORT4 = 0;
volatile int VAR_NULL = 0;
volatile unsigned char GPIO_INPUT_PORT18 = 0;
volatile unsigned char GPIO_INPUT_PORT5 = 0;
volatile unsigned char GPIO_INPUT_PORT3 = 0;
volatile unsigned char GPIO_INPUT_PORT8 = 0;
volatile unsigned char GPIO_OUTPUT_PORT11 = 0;
volatile unsigned char GPIO_INPUT_PORT6 = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char GPIO_INPUT_PORT13 = 0;
volatile unsigned char GPIO_INPUT_PORT2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT9 = 0;
volatile unsigned char GPIO_OUTPUT_PORT10 = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;
volatile unsigned char GPIO_OUTPUT_PORT5 = 0;
volatile unsigned char GPIO_OUTPUT_PORT6 = 0;
volatile unsigned char GPIO_OUTPUT_PORT7 = 0;
volatile unsigned char GPIO_INPUT_PORT7 = 0;
volatile unsigned char GPIO_INPUT_PORT9 = 0;
volatile unsigned char GPIO_INPUT_PORT10 = 0;
volatile unsigned char GPIO_INPUT_PORT12 = 0;
volatile unsigned char GPIO_INPUT_PORT11 = 0;
volatile unsigned char GPIO_OUTPUT_PORT14 = 0;
volatile unsigned char GPIO_OUTPUT_PORT13 = 0;
volatile unsigned char GPIO_INPUT_PORT14 = 0;
volatile unsigned char GPIO_INPUT_PORT17 = 0;
volatile unsigned char GPIO_OUTPUT_PORT16 = 0;
volatile unsigned char GPIO_INPUT_PORT15 = 0;
volatile unsigned char GPIO_INPUT_PORT16 = 0;
volatile unsigned char GPIO_OUTPUT_PORT12 = 0;
volatile unsigned char GPIO_OUTPUT_PORT8 = 0;
volatile unsigned char GPIO_INPUT_PORT19 = 0;
volatile unsigned char GPIO_OUTPUT_PORT15 = 0;
volatile int ArrayBitUser_Count = 3;
volatile int ArrayBitUser[3];
volatile int ArrayBitSystem_Count = 5;
volatile int ArrayBitSystem[5];
volatile int ArrayIntUser_Count = 62;
volatile int ArrayIntUser[62];

void PLC_Run(void)
{
    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* start rung 52 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 0) & 1)) {  // YLedUser
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[0] < 99) {
            ArrayIntUser[0]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[0] = 0; // TEsperaLedON
    }

    if (!((ArrayBitSystem[0] >> 2) & 1)) {  // $TEsperaLedOFF_antiglitch
        ArrayIntUser[1] = 99; // TEsperaLedOFF
    }
    ArrayBitSystem[0] |= 1UL << 2; // $TEsperaLedOFF_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[1] < 99) {
            ArrayIntUser[1]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[1] = 0; // TEsperaLedOFF
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 0; else ArrayBitUser[0] &= ~(1UL << 0); // YLedUser = $rung_top

    /* ] finish series */

    /* start rung 54 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (GPIO_INPUT_PORT1) {  // XEmergencia
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (GPIO_INPUT_PORT4) {  // XFaltaFase
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // RErroGeral = $parThis_0001

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0001
    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $oneShot_0000
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $oneShot_0000 = $scratch

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 2); // RMaqInitOK = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 55 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgMaqLib
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 1);  // YMbMaqLib = 1
    }

    /* ] finish series */

    /* start rung 56 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 5))) {  // XDbgValErro48
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[2] = 48; // PrsValErro
    }

    /* ] finish series */

    /* start rung 57 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 4))) {  // XDbgValErro0
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[2] = 0; // PrsValErro
    }

    /* ] finish series */

    /* start rung 58 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 3) & 1)) {  // RPrsPausaSrvOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 6) & 1)) {  // RMaqSerialBusy
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[3] < 199) {
            ArrayIntUser[3]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[3] = 0; // TEspChecarSrvOK
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $oneShot_0001
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $oneShot_0001 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 7; // RPrsChecarSrvOK = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 6; // RMaqSerialBusy = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 59 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 7) & 1)) {  // RPrsChecarSrvOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 10) & 1)) {  // $oneShot_0002
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "ERR", &VAR_NULL);
                if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $oneShot_0002 = $rung_top
            }
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 11; else ArrayBitSystem[0] &= ~(1UL << 11); // $oneShot_0003 = $rung_top
        }
        if (!((ArrayBitSystem[0] >> 11) & 1)) {  // $oneShot_0003
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 11; // $oneShot_0003 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $oneShot_0002 = $rung_top
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[4] < 4) {
            ArrayIntUser[4]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[4] = 0; // TEspRespChecar
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 12) & 1)) {  // $oneShot_0004
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Read("0", "ERR NONE", &ArrayIntUser[2]);
                if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $oneShot_0004 = $rung_top
            }
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 13; else ArrayBitSystem[0] &= ~(1UL << 13); // $oneShot_0005 = $rung_top
        }
        if (!((ArrayBitSystem[0] >> 13) & 1)) {  // $oneShot_0005
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 13; // $oneShot_0005 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $oneShot_0004 = $rung_top
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 8); // RServoOK = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 6); // RMaqSerialBusy = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 7); // RPrsChecarSrvOK = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[5] = 0; // $scratch2
    if (ArrayIntUser[2] == ArrayIntUser[5]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 8; // RServoOK = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 60 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 9) & 1)) {  // RCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 14) & 1)) {  // $oneShot_0006
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 14; else ArrayBitSystem[0] &= ~(1UL << 14); // $oneShot_0006 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 6); // RMaqSerialBusy = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 10); // RSerialReservada = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 11); // RMaqLerOKSerial = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 12); // RStartDesabSrv = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 13); // RStartZerarEnc = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 14); // RStartCorrigeEnc = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 15); // RStartCfgSrvJog = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 16); // RStartCfgSrv = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 61 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT18) {  // XPressaoArOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitSystem[0] >> 15) & 1)) {  // $TEspPressaoAr_antiglitch
        ArrayIntUser[6] = 49; // TEspPressaoAr
    }
    ArrayBitSystem[0] |= 1UL << 15; // $TEspPressaoAr_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[6] < 49) {
            ArrayIntUser[6]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[6] = 0; // TEspPressaoAr
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 17; else ArrayBitUser[0] &= ~(1UL << 17); // RPressaoArOK = $rung_top

    /* ] finish series */

    /* start rung 62 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 1) & 1)) {  // RErroGeral
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!GPIO_INPUT_PORT5) {  // XInversorOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RErroBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!GPIO_INPUT_PORT3) {  // XTermMartelo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 17) & 1)) {  // RPressaoArOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_INPUT_PORT8) {  // XDesbobOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 8) & 1)) {  // RServoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // YMbMaqLib
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 19; else ArrayBitUser[0] &= ~(1UL << 19); // RMaqOK = $rung_top

    /* ] finish series */

    /* start rung 63 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[0] >> 16) & 1)) {  // $oneShot_0007
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 16; else ArrayBitSystem[0] &= ~(1UL << 16); // $oneShot_0007 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 1);  // YMbMaqLib = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 20); // RAplanLigarMotor = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 21; // RMaqDesabSrv = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 22); // RMaqCfgSrvOK = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        GPIO_OUTPUT_PORT11 = 0; // YPrsEmbreagem = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 23); // PrsAtivarLub = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 64 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 17) & 1)) {  // $oneShot_0008
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 17; else ArrayBitSystem[0] &= ~(1UL << 17); // $oneShot_0008 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] |= 1UL << 21; // RMaqDesabSrv = 1
    }

    /* ] finish series */

    /* start rung 65 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 24; else ArrayBitUser[0] &= ~(1UL << 24); // YLedErro = $rung_top

    /* ] finish series */

    /* start rung 66 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 22) & 1)) {  // RMaqCfgSrvOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 25; else ArrayBitUser[0] &= ~(1UL << 25); // RMaqConfigOK = $rung_top

    /* ] finish series */

    /* start rung 67 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 25) & 1)) {  // RMaqConfigOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 26; else ArrayBitUser[0] &= ~(1UL << 26); // RMaqPronta = $rung_top

    /* ] finish series */

    /* start rung 68 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 26) & 1)) {  // RMaqPronta
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 27) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 29; else ArrayBitUser[0] &= ~(1UL << 29); // RMaqManualOK = $rung_top

    /* ] finish series */

    /* start rung 71 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (MODBUS_REGISTER[3] & (1 << 1)) {  // XDbgDeslReadEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[7] = ENC_Read(0);
    }

    /* ] finish series */

    /* start rung 72 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 2))) {  // XDbgSimulaEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 25; // $scratch2
        ArrayIntUser[7] = ArrayIntUser[7] + ArrayIntUser[5]; // EMaqPosAtual = EMaqPosAtual + $scratch2
    }

    /* ] finish series */

    /* start rung 73 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgSimulaEncNeg
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 20; // $scratch2
        ArrayIntUser[7] = ArrayIntUser[7] - ArrayIntUser[5]; // EMaqPosAtual = EMaqPosAtual - $scratch2
    }

    /* ] finish series */

    /* start rung 78 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[8] = ArrayIntUser[7] * MODBUS_REGISTER[22]; // MaqPosAtual = EMaqPosAtual * YMbEncPerim
    }

    /* ] finish series */

    /* start rung 79 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 100; // $scratch2
        ArrayIntUser[8] = ArrayIntUser[8] * ArrayIntUser[5]; // MaqPosAtual = MaqPosAtual * $scratch2
    }

    /* ] finish series */

    /* start rung 80 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 4; // $scratch2
        if(ArrayIntUser[5]) ArrayIntUser[8] = ArrayIntUser[8] / ArrayIntUser[5]; // MaqPosAtual = MaqPosAtual / $scratch2
    }

    /* ] finish series */

    /* start rung 81 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[5] = 0; // $scratch2
    if (MODBUS_REGISTER[21] > ArrayIntUser[5]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if(MODBUS_REGISTER[21]) ArrayIntUser[8] = ArrayIntUser[8] / MODBUS_REGISTER[21]; // MaqPosAtual = MaqPosAtual / YMbEncResol
    }

    /* ] finish series */

    /* start rung 82 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[8] = ArrayIntUser[8] * MODBUS_REGISTER[20]; // MaqPosAtual = MaqPosAtual * YMbEncFator
    }

    /* ] finish series */

    /* start rung 83 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 10000; // $scratch2
        if(ArrayIntUser[5]) ArrayIntUser[8] = ArrayIntUser[8] / ArrayIntUser[5]; // MaqPosAtual = MaqPosAtual / $scratch2
    }

    /* ] finish series */

    /* start rung 84 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[5] = ArrayIntUser[8]; // YMbPosAtual = MaqPosAtual
    }

    /* ] finish series */

    /* start rung 86 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT6) {  // XAplanFechada
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[9] < 19) {
            ArrayIntUser[9]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[9] = 0; // TEspAplanFechar
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitUser[0] |= 1UL << 30; else ArrayBitUser[0] &= ~(1UL << 30); // RAplanFechada = $parThis_0000

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 18) & 1)) {  // $oneShot_0009
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 18; else ArrayBitSystem[0] &= ~(1UL << 18); // $oneShot_0009 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[4] &= ~(1 << 0);  // YAplanManAvcIHM = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[4] &= ~(1 << 1);  // YAplanManRecIHM = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 87 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 30) & 1)) {  // RAplanFechada
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 31) & 1)) {  // RPrsAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 0; else ArrayBitUser[1] &= ~(1UL << 0); // RAplanAvancoOK = $rung_top

    /* ] finish series */

    /* start rung 88 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[4] & (1 << 2))) {  // YMbAplanAbre
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 1; else ArrayBitUser[1] &= ~(1UL << 1); // RAplanAbre = $rung_top

    /* ] finish series */

    /* start rung 89 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[4] & (1 << 3))) {  // YMbAplanFecha
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 2; else ArrayBitUser[1] &= ~(1UL << 2); // RAplanFecha = $rung_top

    /* ] finish series */

    /* start rung 90 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[4] & (1 << 4))) {  // YMbAplanSobe
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 3; else ArrayBitUser[1] &= ~(1UL << 3); // RAplanSobe = $rung_top

    /* ] finish series */

    /* start rung 91 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[4] & (1 << 5))) {  // YMbAplanDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 4; else ArrayBitUser[1] &= ~(1UL << 4); // RAplanDesce = $rung_top

    /* ] finish series */

    /* start rung 92 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[4] & (1 << 6))) {  // YMbAplanExtSubir
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 19) & 1)) {  // $oneShot_000a
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 19; else ArrayBitSystem[0] &= ~(1UL << 19); // $oneShot_000a = $scratch

    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 5; // RAplanExtSubir = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[0] >> 20) & 1)) {  // $oneShot_000b
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 20; else ArrayBitSystem[0] &= ~(1UL << 20); // $oneShot_000b = $scratch

    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 5); // RAplanExtSubir = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 93 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[4] & (1 << 7))) {  // YMbAplanExpandir
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 21) & 1)) {  // $oneShot_000c
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 21; else ArrayBitSystem[0] &= ~(1UL << 21); // $oneShot_000c = $scratch

    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 6; // RAplanExpandir = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[0] >> 22) & 1)) {  // $oneShot_000d
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 22; else ArrayBitSystem[0] &= ~(1UL << 22); // $oneShot_000d = $scratch

    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 6); // RAplanExpandir = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 95 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[10] < 4) {
            ArrayIntUser[10]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[10] = 0; // TEspRetSerial
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 23) & 1)) {  // $oneShot_000e
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Read("0", "OK", &ArrayIntUser[11]);
                if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 23; else ArrayBitSystem[0] &= ~(1UL << 23); // $oneShot_000e = $rung_top
            }
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 24; else ArrayBitSystem[0] &= ~(1UL << 24); // $oneShot_000f = $rung_top
        }
        if (!((ArrayBitSystem[0] >> 24) & 1)) {  // $oneShot_000f
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 24; // $oneShot_000f = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 23; else ArrayBitSystem[0] &= ~(1UL << 23); // $oneShot_000e = $rung_top
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 11); // RMaqLerOKSerial = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[5] = 0; // $scratch2
    if (ArrayIntUser[5] > ArrayIntUser[11]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 4; // RMaqCfgSrvErro = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 97 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 7) & 1)) {  // RAplanCorrigeEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 6) & 1)) {  // RMaqSerialBusy
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 14; // RStartCorrigeEnc = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 6; // RMaqSerialBusy = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 98 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[12] = ArrayIntUser[8] * ArrayIntUser[13]; // MaqPosAtualCorr = MaqPosAtual * FatorCorrServo
    }

    /* ] finish series */

    /* start rung 99 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 10000; // $scratch2
        if(ArrayIntUser[5]) ArrayIntUser[12] = ArrayIntUser[12] / ArrayIntUser[5]; // MaqPosAtualCorr = MaqPosAtualCorr / $scratch2
    }

    /* ] finish series */

    /* start rung 100 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 14) & 1)) {  // RStartCorrigeEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 25) & 1)) {  // $oneShot_0010
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "ZSET%d", &ArrayIntUser[12]);
                if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 25; else ArrayBitSystem[0] &= ~(1UL << 25); // $oneShot_0010 = $rung_top
            }
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 26; else ArrayBitSystem[0] &= ~(1UL << 26); // $oneShot_0011 = $rung_top
        }
        if (!((ArrayBitSystem[0] >> 26) & 1)) {  // $oneShot_0011
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 26; // $oneShot_0011 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 25; else ArrayBitSystem[0] &= ~(1UL << 25); // $oneShot_0010 = $rung_top
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 27) & 1)) {  // $oneShot_0012
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 27; else ArrayBitSystem[0] &= ~(1UL << 27); // $oneShot_0012 = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 14); // RStartCorrigeEnc = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 7); // RAplanCorrigeEnc = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 6); // RMaqSerialBusy = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 102 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 21) & 1)) {  // RMaqDesabSrv
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 6) & 1)) {  // RMaqSerialBusy
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 12; // RStartDesabSrv = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 21); // RMaqDesabSrv = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 8); // RStartDesabSVOFF = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 6; // RMaqSerialBusy = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 103 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RStartDesabSrv
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[14] < 199) {
            ArrayIntUser[14]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[14] = 0; // TEspFimDesabSrv
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 4; // RMaqCfgSrvErro = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 12); // RStartDesabSrv = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[1] >> 8) & 1)) {  // RStartDesabSVOFF
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[0] >> 28) & 1)) {  // $oneShot_0013
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "SKIP", &VAR_NULL);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 28; else ArrayBitSystem[0] &= ~(1UL << 28); // $oneShot_0013 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 29; else ArrayBitSystem[0] &= ~(1UL << 29); // $oneShot_0014 = $parThis_0000
        }
        if (!((ArrayBitSystem[0] >> 29) & 1)) {  // $oneShot_0014
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 29; // $oneShot_0014 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 28; else ArrayBitSystem[0] &= ~(1UL << 28); // $oneShot_0013 = $parThis_0000
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 31) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0002
    if (((ArrayBitSystem[1] >> 0) & 1)) {  // $oneShot_0015
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 0; else ArrayBitSystem[1] &= ~(1UL << 0); // $oneShot_0015 = $scratch

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[1] |= 1UL << 8; // RStartDesabSVOFF = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 8) & 1)) {  // RStartDesabSVOFF
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 1) & 1)) {  // $oneShot_0016
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "SVOFF", &VAR_NULL);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 1; else ArrayBitSystem[1] &= ~(1UL << 1); // $oneShot_0016 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 2; else ArrayBitSystem[1] &= ~(1UL << 2); // $oneShot_0017 = $parThis_0000
        }
        if (!((ArrayBitSystem[1] >> 2) & 1)) {  // $oneShot_0017
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 2; // $oneShot_0017 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 1; else ArrayBitSystem[1] &= ~(1UL << 1); // $oneShot_0016 = $parThis_0000
    }

    /* start parallel [ */
    ArrayBitSystem[1] &= ~(1UL << 3); // $parOut_0003 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[1] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0003
    if (((ArrayBitSystem[1] >> 5) & 1)) {  // $oneShot_0018
        ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 5; else ArrayBitSystem[1] &= ~(1UL << 5); // $oneShot_0018 = $scratch

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[1] &= ~(1UL << 6); // $parOut_0004 = 0
    if(((ArrayBitSystem[1] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 7; else ArrayBitSystem[1] &= ~(1UL << 7); // $parThis_0004 = $parThis_0003
    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitUser[0] &= ~(1UL << 12); // RStartDesabSrv = 0
    }

    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitSystem[1] |= 1UL << 6; // $parOut_0004 = 1
    }
    if(((ArrayBitSystem[1] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 7; else ArrayBitSystem[1] &= ~(1UL << 7); // $parThis_0004 = $parThis_0003
    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitUser[0] &= ~(1UL << 22); // RMaqCfgSrvOK = 0
    }

    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitSystem[1] |= 1UL << 6; // $parOut_0004 = 1
    }
    if(((ArrayBitSystem[1] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 7; else ArrayBitSystem[1] &= ~(1UL << 7); // $parThis_0004 = $parThis_0003
    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitUser[0] &= ~(1UL << 6); // RMaqSerialBusy = 0
    }

    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitSystem[1] |= 1UL << 6; // $parOut_0004 = 1
    }
    if(((ArrayBitSystem[1] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $parOut_0004
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[1] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0003
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 105 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 9) & 1)) {  // RAplanZerarEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 10) & 1)) {  // RSrvMotorParado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 6) & 1)) {  // RMaqSerialBusy
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 13; // RStartZerarEnc = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 6; // RMaqSerialBusy = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[15] = 0; // EncPosZero
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 106 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 13) & 1)) {  // RStartZerarEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[1] >> 8) & 1)) {  // $oneShot_0019
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 8; else ArrayBitSystem[1] &= ~(1UL << 8); // $oneShot_0019 = $scratch

    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 11) & 1)) {  // RMaqForaPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[15] = ArrayIntUser[16]; // EncPosZero = DifPosic
    }

    /* ] finish series */

    /* start rung 107 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 13) & 1)) {  // RStartZerarEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[1] >> 9) & 1)) {  // $oneShot_001a
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "ZSET%d", &ArrayIntUser[15]);
                if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[1] |= 1UL << 9; else ArrayBitSystem[1] &= ~(1UL << 9); // $oneShot_001a = $rung_top
            }
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[1] |= 1UL << 10; else ArrayBitSystem[1] &= ~(1UL << 10); // $oneShot_001b = $rung_top
        }
        if (!((ArrayBitSystem[1] >> 10) & 1)) {  // $oneShot_001b
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 10; // $oneShot_001b = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[1] |= 1UL << 9; else ArrayBitSystem[1] &= ~(1UL << 9); // $oneShot_001a = $rung_top
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 11) & 1)) {  // $oneShot_001c
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 11; else ArrayBitSystem[1] &= ~(1UL << 11); // $oneShot_001c = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 9); // RAplanZerarEnc = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 13); // RStartZerarEnc = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 6); // RMaqSerialBusy = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ENC_Reset(0, ArrayIntUser[17]);
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 1))) {  // XDbgDeslReadEnc
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayIntUser[7] = 0; // EMaqPosAtual
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 109 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 1) & 1)) {  // RAplanAbre
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 2) & 1)) {  // RAplanFecha
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 3) & 1)) {  // RAplanSobe
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 4) & 1)) {  // RAplanDesce
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 1; // YLigarBombaHidr = 1
    }

    /* ] finish series */

    /* start rung 110 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (GPIO_INPUT_PORT13) {  // XBombaHidrLigada
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[18] < 499) {
            ArrayIntUser[18]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[18] = 0; // TBombaHidrTO
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (GPIO_INPUT_PORT2) {  // XTermBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 18; else ArrayBitUser[0] &= ~(1UL << 18); // RErroBombaHidr = $rung_top

    /* ] finish series */

    /* start rung 111 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[19] < 299) {
            ArrayIntUser[19]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[19] = 0; // TBombaHidrEstab
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 12; else ArrayBitUser[1] &= ~(1UL << 12); // RBombaHidrOK = $rung_top

    /* ] finish series */

    /* start rung 112 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 12) & 1)) {  // RBombaHidrOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[1] >> 3) & 1)) {  // RAplanSobe
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[1] >> 4) & 1)) {  // RAplanDesce
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[1] >> 1) & 1)) {  // RAplanAbre
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[1] >> 2) & 1)) {  // RAplanFecha
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[20] < 29999) {
            ArrayIntUser[20]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[20] = 0; // TBombaHidrInat
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 0; // YLigarBombaHidr = 0
    }

    /* ] finish series */

    /* start rung 114 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 12) & 1)) {  // RBombaHidrOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 1) & 1)) {  // RAplanAbre
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT3) {  // YHidrFecha
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT2 = ((ArrayBitSystem[0] >> 1) & 1); // YHidrAbre = $rung_top

    /* ] finish series */

    /* start rung 115 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 12) & 1)) {  // RBombaHidrOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 2) & 1)) {  // RAplanFecha
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 30) & 1)) {  // RAplanFechada
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    GPIO_OUTPUT_PORT3 = ((ArrayBitSystem[0] >> 4) & 1); // YHidrFecha = $parThis_0000

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 30) & 1)) {  // RAplanFechada
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[4] &= ~(1 << 3);  // YMbAplanFecha = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 116 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 12) & 1)) {  // RBombaHidrOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 3) & 1)) {  // RAplanSobe
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT9) {  // YHidrDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 13; else ArrayBitUser[1] &= ~(1UL << 13); // YHidrSobe = $rung_top

    /* ] finish series */

    /* start rung 117 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 12) & 1)) {  // RBombaHidrOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 4) & 1)) {  // RAplanDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT9 = ((ArrayBitSystem[0] >> 1) & 1); // YHidrDesce = $rung_top

    /* ] finish series */

    /* start rung 118 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT2) {  // YHidrAbre
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT3) {  // YHidrFecha
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT9) {  // YHidrDesce
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 13) & 1)) {  // YHidrSobe
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    GPIO_OUTPUT_PORT10 = ((ArrayBitSystem[0] >> 1) & 1); // YHidrVentagem = $rung_top

    /* ] finish series */

    /* start rung 119 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 5) & 1)) {  // RAplanExtSubir
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 12) & 1)) {  // $oneShot_001d
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 12; else ArrayBitSystem[1] &= ~(1UL << 12); // $oneShot_001d = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT4 = 1; // YAplanExtSobe = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT5 = 0; // YAplanExtDesce = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[1] >> 13) & 1)) {  // $oneShot_001e
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 13; else ArrayBitSystem[1] &= ~(1UL << 13); // $oneShot_001e = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        GPIO_OUTPUT_PORT4 = 0; // YAplanExtSobe = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        GPIO_OUTPUT_PORT5 = 1; // YAplanExtDesce = 1
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 120 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT4) {  // YAplanExtSobe
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT5) {  // YAplanExtDesce
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[21] < 99) {
            ArrayIntUser[21]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[21] = 0; // TEspAtuarExt
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT4 = 0; // YAplanExtSobe = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT5 = 0; // YAplanExtDesce = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 121 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 6) & 1)) {  // RAplanExpandir
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 14) & 1)) {  // $oneShot_001f
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 14; else ArrayBitSystem[1] &= ~(1UL << 14); // $oneShot_001f = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT6 = 1; // YAplanAvancaMesa = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT7 = 0; // YAplanRecuaMesa = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[1] >> 15) & 1)) {  // $oneShot_0020
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 15; else ArrayBitSystem[1] &= ~(1UL << 15); // $oneShot_0020 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        GPIO_OUTPUT_PORT6 = 0; // YAplanAvancaMesa = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        GPIO_OUTPUT_PORT7 = 1; // YAplanRecuaMesa = 1
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 122 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT7) {  // XAplanExtRecuada
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 16) & 1)) {  // $oneShot_0021
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 16; else ArrayBitSystem[1] &= ~(1UL << 16); // $oneShot_0021 = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[1] >> 17) & 1)) {  // $oneShot_0022
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 17; else ArrayBitSystem[1] &= ~(1UL << 17); // $oneShot_0022 = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT6 = 0; // YAplanAvancaMesa = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT7 = 0; // YAplanRecuaMesa = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 124 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RMaqManualOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RStartCfgSrvJog
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (!((ArrayBitSystem[1] >> 18) & 1)) {  // $TEspFimCfgJog_antiglitch
        ArrayIntUser[22] = 4; // TEspFimCfgJog
    }
    ArrayBitSystem[1] |= 1UL << 18; // $TEspFimCfgJog_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[22] < 4) {
            ArrayIntUser[22]++;
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayIntUser[22] = 0; // TEspFimCfgJog
    }

    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RAplanAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 14; else ArrayBitUser[1] &= ~(1UL << 14); // RAplanManualOK = $rung_top

    /* ] finish series */

    /* start rung 125 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 14) & 1)) {  // RAplanManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_INPUT_PORT9) {  // XAplanManAvanca
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!(MODBUS_REGISTER[4] & (1 << 0))) {  // YAplanManAvcIHM
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (GPIO_INPUT_PORT10) {  // XAplanManRecua
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (MODBUS_REGISTER[4] & (1 << 1)) {  // YAplanManRecIHM
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[1] |= 1UL << 15; else ArrayBitUser[1] &= ~(1UL << 15); // RAplanAvancarMan = $parThis_0001

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0001
    if (((ArrayBitSystem[1] >> 19) & 1)) {  // $oneShot_0023
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 19; else ArrayBitSystem[1] &= ~(1UL << 19); // $oneShot_0023 = $scratch

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 16; // RMaqCfgSrvJog = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 126 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 14) & 1)) {  // RAplanManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_INPUT_PORT9) {  // XAplanManAvanca
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (MODBUS_REGISTER[4] & (1 << 0)) {  // YAplanManAvcIHM
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_INPUT_PORT10) {  // XAplanManRecua
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!(MODBUS_REGISTER[4] & (1 << 1))) {  // YAplanManRecIHM
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[1] |= 1UL << 17; else ArrayBitUser[1] &= ~(1UL << 17); // RAplanRecuarMan = $parThis_0001

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0001
    if (((ArrayBitSystem[1] >> 20) & 1)) {  // $oneShot_0024
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 20; else ArrayBitSystem[1] &= ~(1UL << 20); // $oneShot_0024 = $scratch

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 16; // RMaqCfgSrvJog = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 127 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 15) & 1)) {  // RAplanAvancarMan
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[1] >> 21) & 1)) {  // $oneShot_0025
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 21; else ArrayBitSystem[1] &= ~(1UL << 21); // $oneShot_0025 = $scratch

    if (((ArrayBitUser[1] >> 17) & 1)) {  // RAplanRecuarMan
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 17) & 1)) {  // RAplanRecuarMan
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[1] >> 22) & 1)) {  // $oneShot_0026
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 22; else ArrayBitSystem[1] &= ~(1UL << 22); // $oneShot_0026 = $scratch

    if (((ArrayBitUser[1] >> 18) & 1)) {  // RAplanAvancaMan
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[1] |= 1UL << 16; // RMaqCfgSrvJog = 1
    }

    /* ] finish series */

    /* start rung 128 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 16) & 1)) {  // RMaqCfgSrvJog
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 6) & 1)) {  // RMaqSerialBusy
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 15) & 1)) {  // RStartCfgSrvJog
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 15; // RStartCfgSrvJog = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 16); // RMaqCfgSrvJog = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 6; // RMaqSerialBusy = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 19); // RCfgJogFrente = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 20); // RCfgJogRecua = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 21); // RCfgJogParar = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 15) & 1)) {  // RAplanAvancarMan
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[1] >> 17) & 1)) {  // RAplanRecuarMan
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 19; // RCfgJogFrente = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 17) & 1)) {  // RAplanRecuarMan
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[1] >> 15) & 1)) {  // RAplanAvancarMan
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 20; // RCfgJogRecua = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[1] >> 19) & 1)) {  // RCfgJogFrente
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[1] >> 20) & 1)) {  // RCfgJogRecua
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 21; // RCfgJogParar = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 129 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RStartCfgSrvJog
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 19) & 1)) {  // RCfgJogFrente
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 23) & 1)) {  // $oneShot_0027
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 23; else ArrayBitSystem[1] &= ~(1UL << 23); // $oneShot_0027 = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 22); // RMaqCfgSrvOK = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[23] < 99) {
            ArrayIntUser[23]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[23] = 0; // TCfgSrvJogAvcTO
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 4; // RMaqCfgSrvErro = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 24) & 1)) {  // $oneShot_0028
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "JOGP%d", &ArrayIntUser[24]);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 24; else ArrayBitSystem[1] &= ~(1UL << 24); // $oneShot_0028 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 25; else ArrayBitSystem[1] &= ~(1UL << 25); // $oneShot_0029 = $parThis_0000
        }
        if (!((ArrayBitSystem[1] >> 25) & 1)) {  // $oneShot_0029
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 25; // $oneShot_0029 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 24; else ArrayBitSystem[1] &= ~(1UL << 24); // $oneShot_0028 = $parThis_0000
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0001
    if (((ArrayBitSystem[1] >> 26) & 1)) {  // $oneShot_002a
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 26; else ArrayBitSystem[1] &= ~(1UL << 26); // $oneShot_002a = $scratch

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] &= ~(1UL << 15); // RStartCfgSrvJog = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] |= 1UL << 22; // RMaqCfgSrvOK = 1
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 130 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RStartCfgSrvJog
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 20) & 1)) {  // RCfgJogRecua
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 27) & 1)) {  // $oneShot_002b
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 27; else ArrayBitSystem[1] &= ~(1UL << 27); // $oneShot_002b = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 22); // RMaqCfgSrvOK = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[25] < 99) {
            ArrayIntUser[25]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[25] = 0; // TCfgSrvJogRecTO
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 4; // RMaqCfgSrvErro = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 28) & 1)) {  // $oneShot_002c
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "JOGN%d", &ArrayIntUser[24]);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 28; else ArrayBitSystem[1] &= ~(1UL << 28); // $oneShot_002c = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 29; else ArrayBitSystem[1] &= ~(1UL << 29); // $oneShot_002d = $parThis_0000
        }
        if (!((ArrayBitSystem[1] >> 29) & 1)) {  // $oneShot_002d
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 29; // $oneShot_002d = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 28; else ArrayBitSystem[1] &= ~(1UL << 28); // $oneShot_002c = $parThis_0000
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0001
    if (((ArrayBitSystem[1] >> 30) & 1)) {  // $oneShot_002e
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 30; else ArrayBitSystem[1] &= ~(1UL << 30); // $oneShot_002e = $scratch

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] &= ~(1UL << 15); // RStartCfgSrvJog = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] |= 1UL << 22; // RMaqCfgSrvOK = 1
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 131 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RStartCfgSrvJog
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 21) & 1)) {  // RCfgJogParar
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 31) & 1)) {  // $oneShot_002f
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[1] |= 1UL << 31; else ArrayBitSystem[1] &= ~(1UL << 31); // $oneShot_002f = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 22); // RMaqCfgSrvOK = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[26] < 99) {
            ArrayIntUser[26]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[26] = 0; // TCfgSrvJogDesTO
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 4; // RMaqCfgSrvErro = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[2] >> 0) & 1)) {  // $oneShot_0030
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "SKIP", &VAR_NULL);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 0; else ArrayBitSystem[2] &= ~(1UL << 0); // $oneShot_0030 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 1; else ArrayBitSystem[2] &= ~(1UL << 1); // $oneShot_0031 = $parThis_0000
        }
        if (!((ArrayBitSystem[2] >> 1) & 1)) {  // $oneShot_0031
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[2] |= 1UL << 1; // $oneShot_0031 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 0; else ArrayBitSystem[2] &= ~(1UL << 0); // $oneShot_0030 = $parThis_0000
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0001
    if (((ArrayBitSystem[2] >> 2) & 1)) {  // $oneShot_0032
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 2; else ArrayBitSystem[2] &= ~(1UL << 2); // $oneShot_0032 = $scratch

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] &= ~(1UL << 15); // RStartCfgSrvJog = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] |= 1UL << 22; // RMaqCfgSrvOK = 1
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 132 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RStartCfgSrvJog
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[2] >> 3) & 1)) {  // $oneShot_0033
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 3; else ArrayBitSystem[2] &= ~(1UL << 3); // $oneShot_0033 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 15); // RStartCfgSrvJog = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 6); // RMaqSerialBusy = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 136 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT12) {  // XPrsCameParar
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[27] < 4) {
            ArrayIntUser[27]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[27] = 0; // TEspCameON
    }

    if (!((ArrayBitSystem[2] >> 4) & 1)) {  // $TEspCameOFF_antiglitch
        ArrayIntUser[28] = 4; // TEspCameOFF
    }
    ArrayBitSystem[2] |= 1UL << 4; // $TEspCameOFF_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[28] < 4) {
            ArrayIntUser[28]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[28] = 0; // TEspCameOFF
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 22; else ArrayBitUser[1] &= ~(1UL << 22); // RPrsCameParar = $rung_top

    /* ] finish series */

    /* start rung 137 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT11) {  // XPrsAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[29] < 4) {
            ArrayIntUser[29]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[29] = 0; // TEspAvancoON
    }

    if (!((ArrayBitSystem[2] >> 5) & 1)) {  // $TEspAvancoOFF_antiglitch
        ArrayIntUser[30] = 4; // TEspAvancoOFF
    }
    ArrayBitSystem[2] |= 1UL << 5; // $TEspAvancoOFF_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[30] < 4) {
            ArrayIntUser[30]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[30] = 0; // TEspAvancoOFF
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 31; else ArrayBitUser[0] &= ~(1UL << 31); // RPrsAvancoOK = $rung_top

    /* ] finish series */

    /* start rung 139 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 3))) {  // XDbgPrsLigaMotor
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 6) & 1)) {  // $oneShot_0034
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 6; else ArrayBitSystem[2] &= ~(1UL << 6); // $oneShot_0034 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[4] |= (1 << 8);  // YMbPrsLigaMotor = 1
    }

    /* ] finish series */

    /* start rung 140 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 1) & 1)) {  // RErroGeral
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (GPIO_INPUT_PORT5) {  // XInversorOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[4] &= ~(1 << 8);  // YMbPrsLigaMotor = 0
    }

    /* ] finish series */

    /* start rung 141 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[1] >> 23) & 1)) {  // RPrsMotorLigado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[31] < 11999) {
            ArrayIntUser[31]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[31] = 0; // TEspMotorParar
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 24; else ArrayBitUser[1] &= ~(1UL << 24); // RPrsMotorParado = $rung_top

    /* ] finish series */

    /* start rung 142 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 4))) {  // YMbPrsSentidoInv
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 7) & 1)) {  // $oneShot_0035
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 7; else ArrayBitSystem[2] &= ~(1UL << 7); // $oneShot_0035 = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 8) & 1)) {  // $oneShot_0036
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 8; else ArrayBitSystem[2] &= ~(1UL << 8); // $oneShot_0036 = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[1] |= 1UL << 25; // RPrsNovoSentido = 1
    }

    /* ] finish series */

    /* start rung 143 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 26) & 1)) {  // RMaqPronta
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 23) & 1)) {  // RPrsMotorLigado
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!(MODBUS_REGISTER[4] & (1 << 8))) {  // YMbPrsLigaMotor
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (!((ArrayBitUser[1] >> 24) & 1)) {  // RPrsMotorParado
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitUser[1] >> 25) & 1)) {  // RPrsNovoSentido
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $rung_top
    /* iniciando serie [ */
    if (MODBUS_REGISTER[2] & (1 << 4)) {  // YMbPrsSentidoInv
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    GPIO_OUTPUT_PORT14 = ((ArrayBitSystem[0] >> 31) & 1); // YPrsLigaNormal = $parThis_0002

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $rung_top
    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 4))) {  // YMbPrsSentidoInv
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    GPIO_OUTPUT_PORT13 = ((ArrayBitSystem[0] >> 31) & 1); // YPrsLigaInv = $parThis_0002

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $rung_top
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[1] &= ~(1UL << 25); // RPrsNovoSentido = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 144 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT14) {  // YPrsLigaNormal
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT13) {  // YPrsLigaInv
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[1] |= 1UL << 23; else ArrayBitUser[1] &= ~(1UL << 23); // RPrsMotorLigado = $parThis_0001

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0001
    if (((ArrayBitSystem[2] >> 9) & 1)) {  // $oneShot_0037
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 9; else ArrayBitSystem[2] &= ~(1UL << 9); // $oneShot_0037 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        MODBUS_REGISTER[4] &= ~(1 << 9);  // YMbPrsIniciar = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[1] &= ~(1UL << 26); // RPrsIniciarCiclo = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 145 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 23) & 1)) {  // RPrsMotorLigado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[32] < 1499) {
            ArrayIntUser[32]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[32] = 0; // TEspPrsMotorOK
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 27; else ArrayBitUser[1] &= ~(1UL << 27); // RPrsMotorOK = $rung_top

    /* ] finish series */

    /* start rung 147 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 26) & 1)) {  // RMaqPronta
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 27) & 1)) {  // RPrsMotorOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_OUTPUT_PORT14) {  // YPrsLigaNormal
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (!((ArrayBitUser[1] >> 26) & 1)) {  // RPrsIniciarCiclo
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT14) {  // XPrsCmdAjuste
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[4] &= ~(1 << 10);  // YMbPrsParar = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 26); // RPrsIniciarCiclo = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT17) {  // XPrsDeslContinuo
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (((ArrayBitUser[1] >> 28) & 1)) {  // RBloquearPrensa
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[1] |= 1UL << 29; // RPrsPararMartelo = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        GPIO_OUTPUT_PORT11 = 1; // YPrsEmbreagem = 1
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 148 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 22) & 1)) {  // RPrsCameParar
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 10) & 1)) {  // $oneShot_0038
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 10; else ArrayBitSystem[2] &= ~(1UL << 10); // $oneShot_0038 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (!((ArrayBitUser[1] >> 29) & 1)) {  // RPrsPararMartelo
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 20) & 1)) {  // RAplanLigarMotor
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (GPIO_OUTPUT_PORT16) {  // YSrvHabilita
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    if (GPIO_INPUT_PORT14) {  // XPrsCmdAjuste
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT14) {  // XPrsCmdAjuste
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 11) & 1)) {  // $oneShot_0039
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 11; else ArrayBitSystem[2] &= ~(1UL << 11); // $oneShot_0039 = $scratch

    if (((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[1] &= ~(1UL << 3); // $parOut_0003 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $rung_top
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        GPIO_OUTPUT_PORT11 = 0; // YPrsEmbreagem = 0
    }

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $rung_top
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitUser[1] &= ~(1UL << 29); // RPrsPararMartelo = 0
    }

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $rung_top
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitUser[1] &= ~(1UL << 26); // RPrsIniciarCiclo = 0
    }

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[1] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0003
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 149 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 23) & 1)) {  // RPrsMotorLigado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[2] >> 12) & 1)) {  // $oneShot_003a
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 12; else ArrayBitSystem[2] &= ~(1UL << 12); // $oneShot_003a = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        GPIO_OUTPUT_PORT11 = 0; // YPrsEmbreagem = 0
    }

    /* ] finish series */

    /* start rung 151 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 5))) {  // YMbPrsCiclosNovo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 5);  // YMbPrsCiclosNovo = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[5] = 1000; // $scratch2
        ArrayIntUser[33] = MODBUS_REGISTER[18] * ArrayIntUser[5]; // PrsCiclos = YMbNovoCiclosMil * $scratch2
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[33] = ArrayIntUser[33] + MODBUS_REGISTER[17]; // PrsCiclos = PrsCiclos + YMbNovoCiclosUnd
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 152 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 22) & 1)) {  // RPrsCameParar
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 13) & 1)) {  // $oneShot_003b
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 13; else ArrayBitSystem[2] &= ~(1UL << 13); // $oneShot_003b = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 1; // $scratch2
        ArrayIntUser[33] = ArrayIntUser[33] + ArrayIntUser[5]; // PrsCiclos = PrsCiclos + $scratch2
    }

    /* ] finish series */

    /* start rung 153 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 1000; // $scratch2
        if(ArrayIntUser[5]) MODBUS_REGISTER[16] = ArrayIntUser[33] / ArrayIntUser[5]; // YMbPrsCiclosMil = PrsCiclos / $scratch2
    }

    /* ] finish series */

    /* start rung 154 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 1000; // $scratch2
        ArrayIntUser[34] = MODBUS_REGISTER[16] * ArrayIntUser[5]; // PrsCiclosMil = YMbPrsCiclosMil * $scratch2
    }

    /* ] finish series */

    /* start rung 155 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[15] = ArrayIntUser[33] - ArrayIntUser[34]; // YMbPrsCiclosUnid = PrsCiclos - PrsCiclosMil
    }

    /* ] finish series */

    /* start rung 157 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT15) {  // XPrsCmdBM1
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitSystem[2] >> 14) & 1)) {  // $TEspCmdBM1_antiglitch
        ArrayIntUser[35] = 49; // TEspCmdBM1
    }
    ArrayBitSystem[2] |= 1UL << 14; // $TEspCmdBM1_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[35] < 49) {
            ArrayIntUser[35]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[35] = 0; // TEspCmdBM1
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[1] >> 30) & 1)) {  // RPrsCmdBM1Blq
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitUser[1] |= 1UL << 31; else ArrayBitUser[1] &= ~(1UL << 31); // RPrsCmdBM1OK = $parThis_0000

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[36] < 49) {
            ArrayIntUser[36]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[36] = 0; // TTOCmdBM1
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitUser[1] |= 1UL << 30; else ArrayBitUser[1] &= ~(1UL << 30); // RPrsCmdBM1Blq = $parThis_0000

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 158 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT16) {  // XPrsCmdBM2
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitSystem[2] >> 15) & 1)) {  // $TEspCmdBM2_antiglitch
        ArrayIntUser[37] = 49; // TEspCmdBM2
    }
    ArrayBitSystem[2] |= 1UL << 15; // $TEspCmdBM2_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[37] < 49) {
            ArrayIntUser[37]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[37] = 0; // TEspCmdBM2
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[2] >> 0) & 1)) {  // RPrsCmdBM2Blq
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitUser[2] |= 1UL << 1; else ArrayBitUser[2] &= ~(1UL << 1); // RPrsCmdBM2OK = $parThis_0000

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[38] < 49) {
            ArrayIntUser[38]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[38] = 0; // TTOCmdBM2
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitUser[2] |= 1UL << 0; else ArrayBitUser[2] &= ~(1UL << 0); // RPrsCmdBM2Blq = $parThis_0000

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 159 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 31) & 1)) {  // RPrsCmdBM1OK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[2] >> 1) & 1)) {  // RPrsCmdBM2OK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!GPIO_INPUT_PORT17) {  // XPrsDeslContinuo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[2] |= 1UL << 2; else ArrayBitUser[2] &= ~(1UL << 2); // RPrsCmdBMOK = $rung_top

    /* ] finish series */

    /* start rung 161 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 26) & 1)) {  // RMaqPronta
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 27) & 1)) {  // RPrsMotorOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!GPIO_OUTPUT_PORT14) {  // YPrsLigaNormal
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[4] & (1 << 9))) {  // YMbPrsIniciar
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[2] >> 2) & 1)) {  // RPrsCmdBMOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 26; // RPrsIniciarCiclo = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[4] &= ~(1 << 9);  // YMbPrsIniciar = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 28); // RBloquearPrensa = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 163 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_OUTPUT_PORT11) {  // YPrsEmbreagem
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (GPIO_INPUT_PORT17) {  // XPrsDeslContinuo
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!(MODBUS_REGISTER[4] & (1 << 10))) {  // YMbPrsParar
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 16) & 1)) {  // $oneShot_003c
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 16; else ArrayBitSystem[2] &= ~(1UL << 16); // $oneShot_003c = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 29; // RPrsPararMartelo = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[4] &= ~(1 << 10);  // YMbPrsParar = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 164 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT17) {  // XPrsDeslContinuo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[2] >> 17) & 1)) {  // $oneShot_003d
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 17; else ArrayBitSystem[2] &= ~(1UL << 17); // $oneShot_003d = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[1] |= 1UL << 28; // RBloquearPrensa = 1
    }

    /* ] finish series */

    /* start rung 166 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT11) {  // YPrsEmbreagem
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT12 = ((ArrayBitSystem[0] >> 1) & 1); // YPrsAjMartelo = $rung_top

    /* ] finish series */

    /* start rung 168 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (ArrayIntUser[39] > ArrayIntUser[33]) {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 18) & 1)) {  // $oneShot_003e
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 18; else ArrayBitSystem[2] &= ~(1UL << 18); // $oneShot_003e = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 23; // PrsAtivarLub = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[40] = ArrayIntUser[41]; // PrsContPulsosLub = PrsQtdPulsosLub
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 169 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 23) & 1)) {  // PrsAtivarLub
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT8) {  // PrsLigaLubrif
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[42] < 49) {
            ArrayIntUser[42]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[42] = 0; // EspLubON
    }

    if (!((ArrayBitSystem[2] >> 19) & 1)) {  // $EspLubOFF_antiglitch
        ArrayIntUser[43] = 49; // EspLubOFF
    }
    ArrayBitSystem[2] |= 1UL << 19; // $EspLubOFF_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[43] < 49) {
            ArrayIntUser[43]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[43] = 0; // EspLubOFF
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    GPIO_OUTPUT_PORT8 = ((ArrayBitSystem[0] >> 4) & 1); // PrsLigaLubrif = $parThis_0000

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[2] >> 20) & 1)) {  // $oneShot_003f
            ArrayBitSystem[0] |= 1UL << 7; // $scratch
            ArrayIntUser[40] = ArrayIntUser[40] - ((ArrayBitSystem[0] >> 7) & 1); // PrsContPulsosLub = PrsContPulsosLub - $scratch
        }
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 20; else ArrayBitSystem[2] &= ~(1UL << 20); // $oneShot_003f = $parThis_0000
    if (ArrayIntUser[40] > VAR_NULL) {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    } else {
        ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 21) & 1)) {  // $oneShot_0040
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 21; else ArrayBitSystem[2] &= ~(1UL << 21); // $oneShot_0040 = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 23); // PrsAtivarLub = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 170 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 23) & 1)) {  // PrsAtivarLub
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[2] >> 22) & 1)) {  // $oneShot_0041
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 22; else ArrayBitSystem[2] &= ~(1UL << 22); // $oneShot_0041 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[39] = ArrayIntUser[33] + ArrayIntUser[44]; // PrsCiclosProxLub = PrsCiclos + PrsIntervaloLub
    }

    /* ] finish series */

    /* start rung 173 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 6))) {  // XDbgParamSync
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 23) & 1)) {  // $oneShot_0042
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 23; else ArrayBitSystem[2] &= ~(1UL << 23); // $oneShot_0042 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 3);  // YMbParamSync = 1
    }

    /* ] finish series */

    /* start rung 174 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!(MODBUS_REGISTER[2] & (1 << 3))) {  // YMbParamSync
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 24) & 1)) {  // $oneShot_0043
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 24; else ArrayBitSystem[2] &= ~(1UL << 24); // $oneShot_0043 = $scratch

    if (((ArrayBitUser[0] >> 22) & 1)) {  // RMaqCfgSrvOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] |= 1UL << 3; // RMaqCfgSrv = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[2] &= ~(1 << 3);  // YMbParamSync = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 175 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 3) & 1)) {  // RMaqCfgSrv
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 6) & 1)) {  // RMaqSerialBusy
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 16; // RStartCfgSrv = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[2] &= ~(1UL << 3); // RMaqCfgSrv = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 6; // RMaqSerialBusy = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 176 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // RStartCfgSrv
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 25) & 1)) {  // $oneShot_0044
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 25; else ArrayBitSystem[2] &= ~(1UL << 25); // $oneShot_0044 = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 22); // RMaqCfgSrvOK = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 26) & 1)) {  // $oneShot_0045
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 26; else ArrayBitSystem[2] &= ~(1UL << 26); // $oneShot_0045 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] &= ~(1UL << 4); // RCfgSrvParP1OK = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] &= ~(1UL << 5); // RCfgSrvParP2OK = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] &= ~(1UL << 6); // RCfgSrvParP3OK = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] &= ~(1UL << 7); // RCfgSrvParP4OK = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] &= ~(1UL << 8); // RCfgSrvParP5OK = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] &= ~(1UL << 9); // RCfgSrvParP6OK = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] &= ~(1UL << 10); // RCfgSrvParP7OK = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 6); // RMaqSerialBusy = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[45] < 1499) {
            ArrayIntUser[45]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[45] = 0; // TCfgSrvIniTO
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 4; // RMaqCfgSrvErro = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[2] >> 4) & 1)) {  // RCfgSrvParP1OK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[2] >> 27) & 1)) {  // $oneShot_0046
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "RES", &VAR_NULL);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 27; else ArrayBitSystem[2] &= ~(1UL << 27); // $oneShot_0046 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 28; else ArrayBitSystem[2] &= ~(1UL << 28); // $oneShot_0047 = $parThis_0000
        }
        if (!((ArrayBitSystem[2] >> 28) & 1)) {  // $oneShot_0047
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[2] |= 1UL << 28; // $oneShot_0047 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 27; else ArrayBitSystem[2] &= ~(1UL << 27); // $oneShot_0046 = $parThis_0000
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[46] < 499) {
            ArrayIntUser[46]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[46] = 0; // TCfgSrvParP1Fim
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 31) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0002
    if (((ArrayBitSystem[2] >> 29) & 1)) {  // $oneShot_0048
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[2] |= 1UL << 29; else ArrayBitSystem[2] &= ~(1UL << 29); // $oneShot_0048 = $scratch

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[2] |= 1UL << 4; // RCfgSrvParP1OK = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 4) & 1)) {  // RCfgSrvParP1OK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[2] >> 30) & 1)) {  // $oneShot_0049
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "ARES", &VAR_NULL);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 30; else ArrayBitSystem[2] &= ~(1UL << 30); // $oneShot_0049 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 31; else ArrayBitSystem[2] &= ~(1UL << 31); // $oneShot_004a = $parThis_0000
        }
        if (!((ArrayBitSystem[2] >> 31) & 1)) {  // $oneShot_004a
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[2] |= 1UL << 31; // $oneShot_004a = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[2] |= 1UL << 30; else ArrayBitSystem[2] &= ~(1UL << 30); // $oneShot_0049 = $parThis_0000
    }

    /* start parallel [ */
    ArrayBitSystem[1] &= ~(1UL << 3); // $parOut_0003 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[1] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0003
    if (((ArrayBitSystem[3] >> 0) & 1)) {  // $oneShot_004b
        ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[3] |= 1UL << 0; else ArrayBitSystem[3] &= ~(1UL << 0); // $oneShot_004b = $scratch

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = 0
    }

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitUser[2] |= 1UL << 5; // RCfgSrvParP2OK = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[1] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0003
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 5) & 1)) {  // RCfgSrvParP2OK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[3] >> 1) & 1)) {  // $oneShot_004c
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "SPD%d", &ArrayIntUser[47]);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 1; else ArrayBitSystem[3] &= ~(1UL << 1); // $oneShot_004c = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 2; else ArrayBitSystem[3] &= ~(1UL << 2); // $oneShot_004d = $parThis_0000
        }
        if (!((ArrayBitSystem[3] >> 2) & 1)) {  // $oneShot_004d
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[3] |= 1UL << 2; // $oneShot_004d = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 1; else ArrayBitSystem[3] &= ~(1UL << 1); // $oneShot_004c = $parThis_0000
    }

    /* start parallel [ */
    ArrayBitSystem[1] &= ~(1UL << 6); // $parOut_0004 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 7; else ArrayBitSystem[1] &= ~(1UL << 7); // $parThis_0004 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[1] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0004
    if (((ArrayBitSystem[3] >> 3) & 1)) {  // $oneShot_004e
        ArrayBitSystem[1] &= ~(1UL << 7); // $parThis_0004 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[3] |= 1UL << 3; else ArrayBitSystem[3] &= ~(1UL << 3); // $oneShot_004e = $scratch

    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitSystem[1] |= 1UL << 6; // $parOut_0004 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[1] |= 1UL << 7; else ArrayBitSystem[1] &= ~(1UL << 7); // $parThis_0004 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[1] &= ~(1UL << 7); // $parThis_0004 = 0
    }

    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitUser[2] |= 1UL << 6; // RCfgSrvParP3OK = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[1] >> 7) & 1)) {  // $parThis_0004
        ArrayBitSystem[1] |= 1UL << 6; // $parOut_0004 = 1
    }
    if(((ArrayBitSystem[1] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0004
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 6) & 1)) {  // RCfgSrvParP3OK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[3] >> 4) & 1)) {  // $oneShot_004f
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "ACC%d", &ArrayIntUser[48]);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 4; else ArrayBitSystem[3] &= ~(1UL << 4); // $oneShot_004f = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 5; else ArrayBitSystem[3] &= ~(1UL << 5); // $oneShot_0050 = $parThis_0000
        }
        if (!((ArrayBitSystem[3] >> 5) & 1)) {  // $oneShot_0050
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[3] |= 1UL << 5; // $oneShot_0050 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 4; else ArrayBitSystem[3] &= ~(1UL << 4); // $oneShot_004f = $parThis_0000
    }

    /* start parallel [ */
    ArrayBitSystem[3] &= ~(1UL << 6); // $parOut_0005 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 7; else ArrayBitSystem[3] &= ~(1UL << 7); // $parThis_0005 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[3] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0005
    if (((ArrayBitSystem[3] >> 8) & 1)) {  // $oneShot_0051
        ArrayBitSystem[3] &= ~(1UL << 7); // $parThis_0005 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[3] |= 1UL << 8; else ArrayBitSystem[3] &= ~(1UL << 8); // $oneShot_0051 = $scratch

    if (((ArrayBitSystem[3] >> 7) & 1)) {  // $parThis_0005
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 7) & 1)) {  // $parThis_0005
        ArrayBitSystem[3] |= 1UL << 6; // $parOut_0005 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 7; else ArrayBitSystem[3] &= ~(1UL << 7); // $parThis_0005 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[3] &= ~(1UL << 7); // $parThis_0005 = 0
    }

    if (((ArrayBitSystem[3] >> 7) & 1)) {  // $parThis_0005
        ArrayBitUser[2] |= 1UL << 7; // RCfgSrvParP4OK = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 7) & 1)) {  // $parThis_0005
        ArrayBitSystem[3] |= 1UL << 6; // $parOut_0005 = 1
    }
    if(((ArrayBitSystem[3] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0005
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 7) & 1)) {  // RCfgSrvParP4OK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[3] >> 9) & 1)) {  // $oneShot_0052
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "DEC%d", &ArrayIntUser[48]);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 9; else ArrayBitSystem[3] &= ~(1UL << 9); // $oneShot_0052 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 10; else ArrayBitSystem[3] &= ~(1UL << 10); // $oneShot_0053 = $parThis_0000
        }
        if (!((ArrayBitSystem[3] >> 10) & 1)) {  // $oneShot_0053
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[3] |= 1UL << 10; // $oneShot_0053 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 9; else ArrayBitSystem[3] &= ~(1UL << 9); // $oneShot_0052 = $parThis_0000
    }

    /* start parallel [ */
    ArrayBitSystem[3] &= ~(1UL << 11); // $parOut_0006 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 12; else ArrayBitSystem[3] &= ~(1UL << 12); // $parThis_0006 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[3] >> 12) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0006
    if (((ArrayBitSystem[3] >> 13) & 1)) {  // $oneShot_0054
        ArrayBitSystem[3] &= ~(1UL << 12); // $parThis_0006 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[3] |= 1UL << 13; else ArrayBitSystem[3] &= ~(1UL << 13); // $oneShot_0054 = $scratch

    if (((ArrayBitSystem[3] >> 12) & 1)) {  // $parThis_0006
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 12) & 1)) {  // $parThis_0006
        ArrayBitSystem[3] |= 1UL << 11; // $parOut_0006 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 12; else ArrayBitSystem[3] &= ~(1UL << 12); // $parThis_0006 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[3] &= ~(1UL << 12); // $parThis_0006 = 0
    }

    if (((ArrayBitSystem[3] >> 12) & 1)) {  // $parThis_0006
        ArrayBitUser[2] |= 1UL << 8; // RCfgSrvParP5OK = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 12) & 1)) {  // $parThis_0006
        ArrayBitSystem[3] |= 1UL << 11; // $parOut_0006 = 1
    }
    if(((ArrayBitSystem[3] >> 11) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0006
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 8) & 1)) {  // RCfgSrvParP5OK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[3] >> 14) & 1)) {  // $oneShot_0055
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "POST000=%d", &ArrayIntUser[49]);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 14; else ArrayBitSystem[3] &= ~(1UL << 14); // $oneShot_0055 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 15; else ArrayBitSystem[3] &= ~(1UL << 15); // $oneShot_0056 = $parThis_0000
        }
        if (!((ArrayBitSystem[3] >> 15) & 1)) {  // $oneShot_0056
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[3] |= 1UL << 15; // $oneShot_0056 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 14; else ArrayBitSystem[3] &= ~(1UL << 14); // $oneShot_0055 = $parThis_0000
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[50] < 19) {
            ArrayIntUser[50]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[50] = 0; // TEspCfgSrvPar5
    }

    /* start parallel [ */
    ArrayBitSystem[3] &= ~(1UL << 16); // $parOut_0007 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 17; else ArrayBitSystem[3] &= ~(1UL << 17); // $parThis_0007 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[3] >> 17) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0007
    if (((ArrayBitSystem[3] >> 18) & 1)) {  // $oneShot_0057
        ArrayBitSystem[3] &= ~(1UL << 17); // $parThis_0007 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[3] |= 1UL << 18; else ArrayBitSystem[3] &= ~(1UL << 18); // $oneShot_0057 = $scratch

    if (((ArrayBitSystem[3] >> 17) & 1)) {  // $parThis_0007
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 17) & 1)) {  // $parThis_0007
        ArrayBitSystem[3] |= 1UL << 16; // $parOut_0007 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 17; else ArrayBitSystem[3] &= ~(1UL << 17); // $parThis_0007 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[3] &= ~(1UL << 17); // $parThis_0007 = 0
    }

    if (((ArrayBitSystem[3] >> 17) & 1)) {  // $parThis_0007
        ArrayBitUser[2] |= 1UL << 9; // RCfgSrvParP6OK = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 17) & 1)) {  // $parThis_0007
        ArrayBitSystem[3] |= 1UL << 16; // $parOut_0007 = 1
    }
    if(((ArrayBitSystem[3] >> 16) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0007
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 9) & 1)) {  // RCfgSrvParP6OK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[3] >> 19) & 1)) {  // $oneShot_0058
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "SPDT000=%d", &ArrayIntUser[51]);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 19; else ArrayBitSystem[3] &= ~(1UL << 19); // $oneShot_0058 = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 20; else ArrayBitSystem[3] &= ~(1UL << 20); // $oneShot_0059 = $parThis_0000
        }
        if (!((ArrayBitSystem[3] >> 20) & 1)) {  // $oneShot_0059
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[3] |= 1UL << 20; // $oneShot_0059 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 19; else ArrayBitSystem[3] &= ~(1UL << 19); // $oneShot_0058 = $parThis_0000
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[52] < 19) {
            ArrayIntUser[52]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[52] = 0; // TEspCfgSrvPar6
    }

    /* start parallel [ */
    ArrayBitSystem[3] &= ~(1UL << 21); // $parOut_0008 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 22; else ArrayBitSystem[3] &= ~(1UL << 22); // $parThis_0008 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[3] >> 22) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0008
    if (((ArrayBitSystem[3] >> 23) & 1)) {  // $oneShot_005a
        ArrayBitSystem[3] &= ~(1UL << 22); // $parThis_0008 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[3] |= 1UL << 23; else ArrayBitSystem[3] &= ~(1UL << 23); // $oneShot_005a = $scratch

    if (((ArrayBitSystem[3] >> 22) & 1)) {  // $parThis_0008
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 22) & 1)) {  // $parThis_0008
        ArrayBitSystem[3] |= 1UL << 21; // $parOut_0008 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 22; else ArrayBitSystem[3] &= ~(1UL << 22); // $parThis_0008 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[3] &= ~(1UL << 22); // $parThis_0008 = 0
    }

    if (((ArrayBitSystem[3] >> 22) & 1)) {  // $parThis_0008
        ArrayBitUser[2] |= 1UL << 10; // RCfgSrvParP7OK = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 22) & 1)) {  // $parThis_0008
        ArrayBitSystem[3] |= 1UL << 21; // $parOut_0008 = 1
    }
    if(((ArrayBitSystem[3] >> 21) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0008
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 10) & 1)) {  // RCfgSrvParP7OK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[3] >> 24) & 1)) {  // $oneShot_005b
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "SVON", &VAR_NULL);
                if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 24; else ArrayBitSystem[3] &= ~(1UL << 24); // $oneShot_005b = $parThis_0000
            }
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 25; else ArrayBitSystem[3] &= ~(1UL << 25); // $oneShot_005c = $parThis_0000
        }
        if (!((ArrayBitSystem[3] >> 25) & 1)) {  // $oneShot_005c
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[3] |= 1UL << 25; // $oneShot_005c = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
            }
        }
    } else {
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 24; else ArrayBitSystem[3] &= ~(1UL << 24); // $oneShot_005b = $parThis_0000
    }

    /* start parallel [ */
    ArrayBitSystem[3] &= ~(1UL << 26); // $parOut_0009 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 27; else ArrayBitSystem[3] &= ~(1UL << 27); // $parThis_0009 = $parThis_0000
    /* iniciando serie [ */
    if(((ArrayBitSystem[3] >> 27) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0009
    if (((ArrayBitSystem[3] >> 28) & 1)) {  // $oneShot_005d
        ArrayBitSystem[3] &= ~(1UL << 27); // $parThis_0009 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[3] |= 1UL << 28; else ArrayBitSystem[3] &= ~(1UL << 28); // $oneShot_005d = $scratch

    if (((ArrayBitSystem[3] >> 27) & 1)) {  // $parThis_0009
        ArrayBitUser[0] |= 1UL << 11; // RMaqLerOKSerial = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[3] >> 27) & 1)) {  // $parThis_0009
        ArrayBitSystem[3] |= 1UL << 26; // $parOut_0009 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[3] |= 1UL << 27; else ArrayBitSystem[3] &= ~(1UL << 27); // $parThis_0009 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 11) & 1)) {  // RMaqLerOKSerial
        ArrayBitSystem[3] &= ~(1UL << 27); // $parThis_0009 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[3] &= ~(1UL << 29); // $parOut_000a = 0
    if(((ArrayBitSystem[3] >> 27) & 1)) ArrayBitSystem[3] |= 1UL << 30; else ArrayBitSystem[3] &= ~(1UL << 30); // $parThis_000a = $parThis_0009
    if (((ArrayBitSystem[3] >> 30) & 1)) {  // $parThis_000a
        ArrayBitUser[0] &= ~(1UL << 16); // RStartCfgSrv = 0
    }

    if (((ArrayBitSystem[3] >> 30) & 1)) {  // $parThis_000a
        ArrayBitSystem[3] |= 1UL << 29; // $parOut_000a = 1
    }
    if(((ArrayBitSystem[3] >> 27) & 1)) ArrayBitSystem[3] |= 1UL << 30; else ArrayBitSystem[3] &= ~(1UL << 30); // $parThis_000a = $parThis_0009
    if (((ArrayBitSystem[3] >> 30) & 1)) {  // $parThis_000a
        ArrayBitUser[0] |= 1UL << 22; // RMaqCfgSrvOK = 1
    }

    if (((ArrayBitSystem[3] >> 30) & 1)) {  // $parThis_000a
        ArrayBitSystem[3] |= 1UL << 29; // $parOut_000a = 1
    }
    if(((ArrayBitSystem[3] >> 27) & 1)) ArrayBitSystem[3] |= 1UL << 30; else ArrayBitSystem[3] &= ~(1UL << 30); // $parThis_000a = $parThis_0009
    if (((ArrayBitSystem[3] >> 30) & 1)) {  // $parThis_000a
        ArrayBitUser[1] |= 1UL << 9; // RAplanZerarEnc = 1
    }

    if (((ArrayBitSystem[3] >> 30) & 1)) {  // $parThis_000a
        ArrayBitSystem[3] |= 1UL << 29; // $parOut_000a = 1
    }
    if(((ArrayBitSystem[3] >> 29) & 1)) ArrayBitSystem[3] |= 1UL << 27; else ArrayBitSystem[3] &= ~(1UL << 27); // $parThis_0009 = $parOut_000a
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[3] >> 27) & 1)) {  // $parThis_0009
        ArrayBitSystem[3] |= 1UL << 26; // $parOut_0009 = 1
    }
    if(((ArrayBitSystem[3] >> 26) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0009
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 178 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 27) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (MODBUS_REGISTER[2] & (1 << 4)) {  // YMbPrsSentidoInv
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[2] |= 1UL << 11; else ArrayBitUser[2] &= ~(1UL << 11); // RModoAutoOK = $rung_top

    /* ] finish series */

    /* start rung 179 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_OUTPUT_PORT16) {  // YSrvHabilita
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[53] < 299) {
            ArrayIntUser[53]++;
            ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[53] = 0; // TEspDeslSrvInPos
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (GPIO_INPUT_PORT6) {  // XAplanFechada
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $rung_top
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] &= ~(1UL << 5); // RMaqProduzindo = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $rung_top
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] &= ~(1UL << 28); // RMaqStartProd = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $rung_top
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_OUTPUT_PORT16) {  // YSrvHabilita
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[2] |= 1UL << 12; // RMaqErroPosic = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 180 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 27) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YMbModoAutoAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo = 0
    }

    /* ] finish series */

    /* start rung 181 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[3] >> 31) & 1)) {  // $oneShot_005e
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[3] |= 1UL << 31; else ArrayBitSystem[3] &= ~(1UL << 31); // $oneShot_005e = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 29; // RPrsPararMartelo = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 20); // RAplanLigarMotor = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[2] &= ~(1UL << 13); // RMaqInicioAuto = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[2] &= ~(1UL << 14); // RMaqPosicionando = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 183 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 27) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[4] >> 0) & 1)) {  // $oneShot_005f
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 0; else ArrayBitSystem[4] &= ~(1UL << 0); // $oneShot_005f = $scratch

    if (((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[2] >> 12) & 1)) {  // RMaqErroPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 25) & 1)) {  // RMaqConfigOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (MODBUS_REGISTER[2] & (1 << 4)) {  // YMbPrsSentidoInv
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 28; // RMaqStartProd = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[4] &= ~(1 << 6);  // YMbAplanExtSubir = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[4] &= ~(1 << 7);  // YMbAplanExpandir = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 9; // RAplanZerarEnc = 1
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 184 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 23) & 1)) {  // RPrsMotorLigado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[4] |= (1 << 8);  // YMbPrsLigaMotor = 1
    }

    /* ] finish series */

    /* start rung 185 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_INPUT_PORT6) {  // XAplanFechada
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[4] |= (1 << 3);  // YMbAplanFecha = 1
    }

    /* ] finish series */

    /* start rung 186 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[54] < 299) {
            ArrayIntUser[54]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[54] = 0; // TEspExtRepouso
    }

    if (!((ArrayBitUser[1] >> 27) & 1)) {  // RPrsMotorOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!GPIO_INPUT_PORT6) {  // XAplanFechada
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[4] >> 1) & 1)) {  // $oneShot_0060
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 1; else ArrayBitSystem[4] &= ~(1UL << 1); // $oneShot_0060 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 28); // RMaqStartProd = 0
    }

    /* ] finish series */

    /* start rung 187 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[4] >> 2) & 1)) {  // $oneShot_0061
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 2; else ArrayBitSystem[4] &= ~(1UL << 2); // $oneShot_0061 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 11) & 1)) {  // RModoAutoOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 5; // RMaqProduzindo = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 26; // RPrsIniciarCiclo = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] |= 1UL << 13; // RMaqInicioAuto = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 28); // RBloquearPrensa = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[2] >> 11) & 1)) {  // RModoAutoOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 189 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[16] = ArrayIntUser[8] - ArrayIntUser[55]; // DifPosic = MaqPosAtual - AplanPasso
    }

    /* ] finish series */

    /* start rung 190 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    ArrayIntUser[5] = -75; // $scratch2
    if (ArrayIntUser[5] > ArrayIntUser[16]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    ArrayIntUser[5] = 75; // $scratch2
    if (ArrayIntUser[16] > ArrayIntUser[5]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 11; else ArrayBitUser[1] &= ~(1UL << 11); // RMaqForaPosic = $rung_top

    /* ] finish series */

    /* start rung 191 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[5] = 30; // $scratch2
    if (ArrayIntUser[5] > ArrayIntUser[16]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayIntUser[5] = -30; // $scratch2
    if (ArrayIntUser[16] > ArrayIntUser[5]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[2] |= 1UL << 15; else ArrayBitUser[2] &= ~(1UL << 15); // RMaqPosicOK = $rung_top

    /* ] finish series */

    /* start rung 192 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 31) & 1)) {  // RPrsAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[4] >> 3) & 1)) {  // $oneShot_0062
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 3; else ArrayBitSystem[4] &= ~(1UL << 3); // $oneShot_0062 = $scratch

    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[2] >> 12) & 1)) {  // RMaqErroPosic
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 20; // RAplanLigarMotor = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[2] &= ~(1UL << 13); // RMaqInicioAuto = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[4] >> 4) & 1)) {  // $oneShot_0063
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 4; else ArrayBitSystem[4] &= ~(1UL << 4); // $oneShot_0063 = $scratch

    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 30); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 11) & 1)) {  // RMaqForaPosic
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitUser[2] >> 13) & 1)) {  // RMaqInicioAuto
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[1] &= ~(1UL << 3); // $parOut_0003 = 0
    if(((ArrayBitSystem[0] >> 31) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $parThis_0002
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitUser[2] |= 1UL << 12; // RMaqErroPosic = 1
    }

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 31) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $parThis_0002
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitUser[1] |= 1UL << 29; // RPrsPararMartelo = 1
    }

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 31) & 1)) ArrayBitSystem[1] |= 1UL << 4; else ArrayBitSystem[1] &= ~(1UL << 4); // $parThis_0003 = $parThis_0002
    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayIntUser[5] = 10; // $scratch2
        if(ArrayIntUser[5]) MODBUS_REGISTER[14] = ArrayIntUser[16] / ArrayIntUser[5]; // YMbMaqErroPos = DifPosic / $scratch2
    }

    if (((ArrayBitSystem[1] >> 4) & 1)) {  // $parThis_0003
        ArrayBitSystem[1] |= 1UL << 3; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[1] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parOut_0003
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[2] >> 12) & 1)) {  // RMaqErroPosic
        ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[1] |= 1UL << 9; // RAplanZerarEnc = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[0] &= ~(1UL << 20); // RAplanLigarMotor = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitUser[2] &= ~(1UL << 14); // RMaqPosicionando = 0
    }

    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 30; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 193 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 12) & 1)) {  // RMaqErroPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[4] >> 5) & 1)) {  // $oneShot_0064
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 5; else ArrayBitSystem[4] &= ~(1UL << 5); // $oneShot_0064 = $scratch

    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RAplanAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] |= 1UL << 20; // RAplanLigarMotor = 1
    }

    /* ] finish series */

    /* start rung 194 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[4] >> 6) & 1)) {  // $oneShot_0065
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 6; else ArrayBitSystem[4] &= ~(1UL << 6); // $oneShot_0065 = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 15) & 1)) {  // RMaqPosicOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[4] >> 7) & 1)) {  // $oneShot_0066
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 7; else ArrayBitSystem[4] &= ~(1UL << 7); // $oneShot_0066 = $scratch

    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if (!((ArrayBitUser[2] >> 14) & 1)) {  // RMaqPosicionando
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 20); // RAplanLigarMotor = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 15) & 1)) {  // RMaqPosicOK
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (GPIO_OUTPUT_PORT11) {  // YPrsEmbreagem
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 26; // RPrsIniciarCiclo = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[2] >> 15) & 1)) {  // RMaqPosicOK
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RAplanAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 7; // RAplanCorrigeEnc = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 195 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 7) & 1)) {  // RAplanCorrigeEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[4] >> 8) & 1)) {  // $oneShot_0067
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 8; else ArrayBitSystem[4] &= ~(1UL << 8); // $oneShot_0067 = $scratch

    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RAplanAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] |= 1UL << 20; // RAplanLigarMotor = 1
    }

    /* ] finish series */

    /* start rung 197 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[4] >> 9) & 1)) {  // $oneShot_0068
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 9; else ArrayBitSystem[4] &= ~(1UL << 9); // $oneShot_0068 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[2] |= 1UL << 16; // SrvMotorPartiu = 1
    }

    /* ] finish series */

    /* start rung 198 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 20) & 1)) {  // RAplanLigarMotor
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[4] >> 10) & 1)) {  // $oneShot_0069
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 10; else ArrayBitSystem[4] &= ~(1UL << 10); // $oneShot_0069 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[2] &= ~(1UL << 16); // SrvMotorPartiu = 0
    }

    /* ] finish series */

    /* start rung 199 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 20) & 1)) {  // RAplanLigarMotor
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[2] >> 17) & 1)) {  // SrvReligarMotor
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[56] < 4) {
            ArrayIntUser[56]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[56] = 0; // EspSrvPartir
    }

    if (((ArrayBitUser[2] >> 16) & 1)) {  // SrvMotorPartiu
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitSystem[4] >> 11) & 1)) {  // $EspSrvReligar_antiglitch
        ArrayIntUser[57] = 4; // EspSrvReligar
    }
    ArrayBitSystem[4] |= 1UL << 11; // $EspSrvReligar_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[57] < 4) {
            ArrayIntUser[57]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[57] = 0; // EspSrvReligar
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[2] |= 1UL << 17; else ArrayBitUser[2] &= ~(1UL << 17); // SrvReligarMotor = $rung_top

    /* ] finish series */

    /* start rung 200 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 20) & 1)) {  // RAplanLigarMotor
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 7) & 1)) {  // RAplanCorrigeEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 9) & 1)) {  // RAplanZerarEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[2] >> 17) & 1)) {  // SrvReligarMotor
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    GPIO_OUTPUT_PORT16 = ((ArrayBitSystem[0] >> 4) & 1); // YSrvHabilita = $parThis_0000

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[4] >> 12) & 1)) {  // $oneShot_006a
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 12; else ArrayBitSystem[4] &= ~(1UL << 12); // $oneShot_006a = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[2] |= 1UL << 14; // RMaqPosicionando = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitSystem[4] >> 13) & 1)) {  // $TEspSrvParar_antiglitch
        ArrayIntUser[58] = 9; // TEspSrvParar
    }
    ArrayBitSystem[4] |= 1UL << 13; // $TEspSrvParar_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[58] < 9) {
            ArrayIntUser[58]++;
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayIntUser[58] = 0; // TEspSrvParar
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 10); // RSrvMotorParado = 0
    } else {
        ArrayBitUser[1] |= 1UL << 10; // RSrvMotorParado = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 202 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 31) & 1)) {  // RPrsAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[4] >> 14) & 1)) {  // $oneShot_006b
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 14; else ArrayBitSystem[4] &= ~(1UL << 14); // $oneShot_006b = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 27) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[4] >> 15) & 1)) {  // $oneShot_006c
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 15; else ArrayBitSystem[4] &= ~(1UL << 15); // $oneShot_006c = $scratch

    if (GPIO_OUTPUT_PORT11) {  // YPrsEmbreagem
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 27) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 5); // RMaqProduzindo = 0
    }

    /* ] finish series */

    /* start rung 204 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // XMbModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitUser[0] |= 1UL << 27; else ArrayBitUser[0] &= ~(1UL << 27); // RModoAuto = $parThis_0000

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $parThis_0000
    if (((ArrayBitSystem[4] >> 16) & 1)) {  // $oneShot_006d
        ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 16; else ArrayBitSystem[4] &= ~(1UL << 16); // $oneShot_006d = $scratch

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[1] |= (1 << 0);  // YMbModoAutoAtivo = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 205 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgLimpaErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[4] >> 17) & 1)) {  // $oneShot_006e
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 17; else ArrayBitSystem[4] &= ~(1UL << 17); // $oneShot_006e = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 2);  // YMbLimpaErro = 1
    }

    /* ] finish series */

    /* start rung 206 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 2))) {  // YMbLimpaErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 4); // RMaqCfgSrvErro = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitUser[2] &= ~(1UL << 12); // RMaqErroPosic = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitSystem[4] >> 18) & 1)) {  // $TEspInvLimpaErr_antiglitch
        ArrayIntUser[59] = 9; // TEspInvLimpaErr
    }
    ArrayBitSystem[4] |= 1UL << 18; // $TEspInvLimpaErr_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        if (ArrayIntUser[59] < 9) {
            ArrayIntUser[59]++;
            ArrayBitSystem[0] |= 1UL << 4; // $parThis_0000 = 1
        }
    } else {
        ArrayIntUser[59] = 0; // TEspInvLimpaErr
    }

    GPIO_OUTPUT_PORT15 = ((ArrayBitSystem[0] >> 4) & 1); // YInvLimpaErro = $parThis_0000

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 2);  // YMbLimpaErro = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 208 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT1) {  // XEmergencia
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 0); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 0;  // YMbErrEmergencia = $rung_top

    /* ] finish series */

    /* start rung 209 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT4) {  // XFaltaFase
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 1); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 1;  // YMbErrFaltaFase = $rung_top

    /* ] finish series */

    /* start rung 210 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RErroBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 2); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 2;  // YMbErrBombaHidr = $rung_top

    /* ] finish series */

    /* start rung 211 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT3) {  // XTermMartelo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 3); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 3;  // YMbErrMartelo = $rung_top

    /* ] finish series */

    /* start rung 212 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 17) & 1)) {  // RPressaoArOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 4); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 4;  // YMbErrPressaoAr = $rung_top

    /* ] finish series */

    /* start rung 213 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT5) {  // XInversorOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 5); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 5;  // YMbErrInversor = $rung_top

    /* ] finish series */

    /* start rung 214 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 8) & 1)) {  // RServoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 6); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 6;  // YMbErrServo = $rung_top

    /* ] finish series */

    /* start rung 215 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT8) {  // XDesbobOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0

    MODBUS_REGISTER[0] &= ~(1 << 7); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 7;  // YMbErrDesbob = $rung_top

    /* ] finish series */

    /* start rung 216 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 4) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 8); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 8;  // YMbErrSrvComunic = $rung_top

    /* ] finish series */

    /* start rung 217 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 12) & 1)) {  // RMaqErroPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 9); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 9;  // YMbErrPosic = $rung_top

    /* ] finish series */

    /* start rung 219 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 1); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 1;  // YMbMaqOK = $rung_top

    /* ] finish series */

    /* start rung 220 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 26) & 1)) {  // RMaqPronta
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 2); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 2;  // YMbMaqPronta = $rung_top

    /* ] finish series */

    /* start rung 221 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 22) & 1)) {  // RMaqCfgSrvOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 3); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 3;  // YMbMaqCfgSrvOK = $rung_top

    /* ] finish series */

    /* start rung 222 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RAplanAvancoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 4); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 4;  // YMbAplanAvancoOK = $rung_top

    /* ] finish series */

    /* start rung 223 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 24) & 1)) {  // RPrsMotorParado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 5); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 5;  // YMbPrsParado = $rung_top

    /* ] finish series */

    /* start rung 224 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 23) & 1)) {  // RPrsMotorLigado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 6); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 6;  // YMbPrsLigado = $rung_top

    /* ] finish series */

    /* start rung 225 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 5) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 7); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 7;  // YMbMaqProduzindo = $rung_top

    /* ] finish series */

    /* start rung 226 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RMaqStartProd
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 8); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 8;  // YMbMaqStartProd = $rung_top

    /* ] finish series */

    /* start rung 227 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 11) & 1)) {  // RMaqForaPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 9); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 9;  // YMbMaqForaPosic = $rung_top

    /* ] finish series */

    /* start rung 228 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT6) {  // XAplanFechada
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 12); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 12;  // YMbAplanAberta = $rung_top

    /* ] finish series */

    /* start rung 229 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 5) & 1)) {  // RAplanExtSubir
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 11); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 11;  // YMbAplanExtCima = $rung_top

    /* ] finish series */

    /* start rung 230 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT7) {  // XAplanExtRecuada
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 10); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 10;  // YMbAplanExtAvanc = $rung_top

    /* ] finish series */

    /* start rung 231 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 13); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 13;  // YMbSrvInPos = $rung_top

    /* ] finish series */

    /* start rung 232 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[2] >> 15) & 1)) {  // RMaqPosicOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 14); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 14;  // YMbPosicOK = $rung_top

    /* ] finish series */

    /* start rung 234 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[4] >> 19) & 1)) {  // $oneShot_006f
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 19; else ArrayBitSystem[4] &= ~(1UL << 19); // $oneShot_006f = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[17] = 0; // ValZero
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ENC_Reset(0, ArrayIntUser[17]);
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[11] &= ~(1 << 0);  // YMbAplanAcelMS = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[12] &= ~(1 << 0);  // YMbAplanVelAuto = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[13] &= ~(1 << 0);  // YMbAplanVelMan = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[10] &= ~(1 << 0);  // YMbAplanPasso = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[20] &= ~(1 << 0);  // YMbEncFator = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[21] &= ~(1 << 0);  // YMbEncResol = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[22] &= ~(1 << 0);  // YMbEncPerim = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[18] &= ~(1 << 0);  // YMbNovoCiclosMil = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[17] &= ~(1 << 0);  // YMbNovoCiclosUnd = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[16] &= ~(1 << 0);  // YMbPrsCiclosMil = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[15] &= ~(1 << 0);  // YMbPrsCiclosUnid = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[14] &= ~(1 << 0);  // YMbMaqErroPos = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[5] &= ~(1 << 0);  // YMbPosAtual = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 235 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $scratch = $rung_top
    if (((ArrayBitSystem[4] >> 20) & 1)) {  // $oneShot_0070
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[4] |= 1UL << 20; else ArrayBitSystem[4] &= ~(1UL << 20); // $oneShot_0070 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[47] = 5934; // AplanVelMax
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[13] = 10875; // FatorCorrServo
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[12] = 100; // YMbAplanVelAuto
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[13] = 30; // YMbAplanVelMan
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[11] = 100; // YMbAplanAcelMS
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[10] = 320; // YMbAplanPasso
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[20] = 10000; // YMbEncFator
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[21] = 2500; // YMbEncResol
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[22] = 400; // YMbEncPerim
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[33] = 0; // PrsCiclos
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[41] = 3; // PrsQtdPulsosLub
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[44] = 1000; // PrsIntervaloLub
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[39] = ArrayIntUser[33] + ArrayIntUser[44]; // PrsCiclosProxLub = PrsCiclos + PrsIntervaloLub
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[18] = 0; // YMbNovoCiclosMil
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[17] = 0; // YMbNovoCiclosUnd
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayIntUser[60] = ArrayIntUser[61]; // dest = src
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 238 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[51] = ArrayIntUser[47] * MODBUS_REGISTER[12]; // AplanVelAuto = AplanVelMax * YMbAplanVelAuto
    }

    /* ] finish series */

    /* start rung 239 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 100; // $scratch2
        if(ArrayIntUser[5]) ArrayIntUser[51] = ArrayIntUser[51] / ArrayIntUser[5]; // AplanVelAuto = AplanVelAuto / $scratch2
    }

    /* ] finish series */

    /* start rung 240 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[24] = ArrayIntUser[47] * MODBUS_REGISTER[13]; // AplanVelManual = AplanVelMax * YMbAplanVelMan
    }

    /* ] finish series */

    /* start rung 241 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 100; // $scratch2
        if(ArrayIntUser[5]) ArrayIntUser[24] = ArrayIntUser[24] / ArrayIntUser[5]; // AplanVelManual = AplanVelManual / $scratch2
    }

    /* ] finish series */

    /* start rung 242 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[5] = 0; // $scratch2
    if (MODBUS_REGISTER[11] > ArrayIntUser[5]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if(MODBUS_REGISTER[11]) ArrayIntUser[48] = ArrayIntUser[47] / MODBUS_REGISTER[11]; // AplanRampa = AplanVelMax / YMbAplanAcelMS
    }

    /* ] finish series */

    /* start rung 243 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[5] = 0; // $scratch2
    if (ArrayIntUser[48] == ArrayIntUser[5]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[48] = 1; // AplanRampa
    }

    /* ] finish series */

    /* start rung 244 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 100; // $scratch2
        ArrayIntUser[55] = MODBUS_REGISTER[10] * ArrayIntUser[5]; // AplanPasso = YMbAplanPasso * $scratch2
    }

    /* ] finish series */

    /* start rung 245 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[49] = ArrayIntUser[55] * ArrayIntUser[13]; // AplanPassoCorr = AplanPasso * FatorCorrServo
    }

    /* ] finish series */

    /* start rung 246 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr
    
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[5] = 10000; // $scratch2
        if(ArrayIntUser[5]) ArrayIntUser[49] = ArrayIntUser[49] / ArrayIntUser[5]; // AplanPassoCorr = AplanPassoCorr / $scratch2
    }

    /* ] finish series */
}

void PLC_Init(void)
{
	I_SerialReady = 1;
	MODBUS_RS485_MASTER = 0;
	MODBUS_TCP_MASTER = 0;
	ModBUS_SetID(0);

	RS485_Config(19200, 7, 2, 1);

	IP4_ADDR(&IP_ADDRESS, 192,168,0,235);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,10);
	IP4_ADDR(&IP_DNS, 192,168,0,1);

    QEIConfig.CaptureMode = QEI_CAPMODE_4X;
    ADC_SetMask(0);
    SSI_Init(24, 0);

//    ENC_Config(0, 0, 400, 1.000000, 4096);
//    ENC_Config(1, 0, 400, 1.000000, 12);

    memset((void*)ArrayBitUser, 0, sizeof(ArrayBitUser));
    memset((void*)ArrayBitSystem, 0, sizeof(ArrayBitSystem));
    memset((void*)ArrayIntUser, 0, sizeof(ArrayIntUser));
}
