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
volatile unsigned char GPIO_INPUT_PORT6 = 0;
volatile unsigned char GPIO_INPUT_PORT4 = 0;
volatile unsigned char GPIO_INPUT_PORT2 = 0;
volatile int A6 = 0;
volatile unsigned char GPIO_OUTPUT_PORT14 = 0;
volatile unsigned char GPIO_INPUT_PORT11 = 0;
volatile unsigned char GPIO_INPUT_PORT12 = 0;
volatile unsigned char GPIO_INPUT_PORT13 = 0;
volatile unsigned char GPIO_INPUT_PORT14 = 0;
volatile unsigned char GPIO_INPUT_PORT15 = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char GPIO_OUTPUT_PORT11 = 0;
volatile unsigned char GPIO_OUTPUT_PORT12 = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;
volatile unsigned char GPIO_OUTPUT_PORT6 = 0;
volatile unsigned char GPIO_OUTPUT_PORT7 = 0;
volatile unsigned char GPIO_OUTPUT_PORT5 = 0;
volatile unsigned char GPIO_INPUT_PORT16 = 0;
volatile unsigned char GPIO_INPUT_PORT17 = 0;
volatile unsigned char GPIO_INPUT_PORT18 = 0;
volatile int VAR_NULL = 0;
volatile int ArrayBitUser_Count = 2;
volatile int ArrayBitUser[2];
volatile int ArrayBitSystem_Count = 3;
volatile int ArrayBitSystem[3];
volatile int ArrayIntUser_Count = 90;
volatile int ArrayIntUser[90];

void PLC_Run(void)
{
    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* start rung 61 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (GPIO_INPUT_PORT1) {  // XEmergencia
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (GPIO_INPUT_PORT6) {  // XFaltaFase
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitUser[0] |= 1UL << 0; else ArrayBitUser[0] &= ~(1UL << 0); // RErroGeral = $parThis_0001

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0001
    if (!((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        if (((ArrayBitSystem[0] >> 7) & 1)) {  // $oneShot_0000
            ArrayBitSystem[0] |= 1UL << 5; // $parThis_0001 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $oneShot_0000 = $scratch

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 1; // ZerarEncServo = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 62 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgMaqLib
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $oneShot_0001
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $oneShot_0001 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 6);  // YMbMaqLib = 1
    }

    /* ] finish series */

    /* start rung 63 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 0) & 1)) {  // RErroGeral
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!GPIO_INPUT_PORT4) {  // ServoOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!GPIO_INPUT_PORT2) {  // XTermBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[2] & (1 << 6))) {  // YMbMaqLib
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitUser[0] |= 1UL << 2; else ArrayBitUser[0] &= ~(1UL << 2); // RMaqOK = $parThis_0001

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0001
    if(((ArrayBitSystem[0] >> 10) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0002
    if (((ArrayBitSystem[0] >> 11) & 1)) {  // $oneShot_0002
        ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 11; else ArrayBitSystem[0] &= ~(1UL << 11); // $oneShot_0002 = $scratch

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0001
    if(((ArrayBitSystem[0] >> 10) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0002
    if (!((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        if (((ArrayBitSystem[0] >> 12) & 1)) {  // $oneShot_0003
            ArrayBitSystem[0] |= 1UL << 10; // $parThis_0002 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $oneShot_0003 = $scratch

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 3; // ConfigServo = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 64 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[0] >> 13) & 1)) {  // $oneShot_0004
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 13; else ArrayBitSystem[0] &= ~(1UL << 13); // $oneShot_0004 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[2] &= ~(1 << 6);  // YMbMaqLib = 0
    }

    /* ] finish series */

    /* start rung 65 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[0] < 499) {
            ArrayIntUser[0]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[0] = 0; // EspServoInit
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 14) & 1)) {  // $oneShot_0005
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 14; else ArrayBitSystem[0] &= ~(1UL << 14); // $oneShot_0005 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 3; // ConfigServo = 1
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 1; // ZerarEncServo = 1
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 66 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 3) & 1)) {  // ConfigServo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[1] = 0; // EstadoServo
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[1] = 1; // EstadoServo
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[0] >> 15) & 1)) {  // $oneShot_0006
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 809, 1, &ArrayIntUser[1]);
                ArrayBitSystem[0] |= 1UL << 15; // $oneShot_0006 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[0] &= ~(1UL << 16); // $oneShot_0007 = 0
        }
        if (!((ArrayBitSystem[0] >> 16) & 1)) {  // $oneShot_0007
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 16; // $oneShot_0007 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 809, 1, &ArrayIntUser[1]);
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 15); // $oneShot_0006 = 0
        ArrayBitSystem[0] &= ~(1UL << 16); // $oneShot_0007 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 3); // ConfigServo = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 68 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        A6 = ADC_Read(6);
    }

    /* ] finish series */

    /* start rung 69 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[4] = A6; // MbTemp = Temperatura
    }

    /* ] finish series */

    /* start rung 71 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (MODBUS_REGISTER[3] & (1 << 2)) {  // XDbgDeslReadEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[2] = ENC_Read(0);
    }

    /* ] finish series */

    /* start rung 72 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 3))) {  // XDbgSimulaEnc
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 25; // $scratch2
        ArrayIntUser[2] = ArrayIntUser[2] + ArrayIntUser[3]; // EMaqPosAtual = EMaqPosAtual + $scratch2
    }

    /* ] finish series */

    /* start rung 73 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 4))) {  // XDbgSimulaEncNeg
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 20; // $scratch2
        ArrayIntUser[2] = ArrayIntUser[2] - ArrayIntUser[3]; // EMaqPosAtual = EMaqPosAtual - $scratch2
    }

    /* ] finish series */

    /* start rung 78 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[4] = ArrayIntUser[2] * MODBUS_REGISTER[22]; // MaqPosAtual = EMaqPosAtual * YMbEncPerim
    }

    /* ] finish series */

    /* start rung 79 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 100; // $scratch2
        ArrayIntUser[4] = ArrayIntUser[4] * ArrayIntUser[3]; // MaqPosAtual = MaqPosAtual * $scratch2
    }

    /* ] finish series */

    /* start rung 80 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 4; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[4] = ArrayIntUser[4] / ArrayIntUser[3]; // MaqPosAtual = MaqPosAtual / $scratch2
    }

    /* ] finish series */

    /* start rung 81 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[3] = 0; // $scratch2
    if (MODBUS_REGISTER[21] > ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if(MODBUS_REGISTER[21]) ArrayIntUser[4] = ArrayIntUser[4] / MODBUS_REGISTER[21]; // MaqPosAtual = MaqPosAtual / YMbEncResol
    }

    /* ] finish series */

    /* start rung 82 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[4] = ArrayIntUser[4] * MODBUS_REGISTER[20]; // MaqPosAtual = MaqPosAtual * YMbEncFator
    }

    /* ] finish series */

    /* start rung 83 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 1000; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[4] = ArrayIntUser[4] / ArrayIntUser[3]; // MaqPosAtual = MaqPosAtual / $scratch2
    }

    /* ] finish series */

    /* start rung 84 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 10; // $scratch2
        if(ArrayIntUser[3]) MODBUS_REGISTER[29] = ArrayIntUser[4] / ArrayIntUser[3]; // YMbPosAtual = MaqPosAtual / $scratch2
    }

    /* ] finish series */

    /* start rung 86 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 1) & 1)) {  // ZerarEncServo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 4) & 1)) {  // RSrvMotorParado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 5) & 1)) {  // RMaqCfgSrvErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 6) & 1)) {  // ZerarEncPerfil
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[5] < 9) {
            ArrayIntUser[5]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[5] = 0; // EspZerarEncServo
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[6] < 499) {
            ArrayIntUser[6]++;
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[6] = 0; // ZerarEncServoTO
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 5; // RMaqCfgSrvErro = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT14 = 0; // YInvResetPos = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 17) & 1)) {  // $oneShot_0008
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 17; else ArrayBitSystem[0] &= ~(1UL << 17); // $oneShot_0008 = $scratch

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        GPIO_OUTPUT_PORT14 = 1; // YInvResetPos = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[0] >> 18) & 1)) {  // $oneShot_0009
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 800, 1, &ArrayIntUser[7]);
                ArrayBitSystem[0] |= 1UL << 18; // $oneShot_0009 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[0] &= ~(1UL << 19); // $oneShot_000a = 0
        }
        if (!((ArrayBitSystem[0] >> 19) & 1)) {  // $oneShot_000a
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 19; // $oneShot_000a = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 800, 1, &ArrayIntUser[7]);
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 18); // $oneShot_0009 = 0
        ArrayBitSystem[0] &= ~(1UL << 19); // $oneShot_000a = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[0] >> 20) & 1)) {  // $oneShot_000b
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 801, 1, &ArrayIntUser[8]);
                ArrayBitSystem[0] |= 1UL << 20; // $oneShot_000b = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[0] &= ~(1UL << 21); // $oneShot_000c = 0
        }
        if (!((ArrayBitSystem[0] >> 21) & 1)) {  // $oneShot_000c
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 21; // $oneShot_000c = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 801, 1, &ArrayIntUser[8]);
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 20); // $oneShot_000b = 0
        ArrayBitSystem[0] &= ~(1UL << 21); // $oneShot_000c = 0
    }

    if (GPIO_INPUT_PORT11) {  // XInvFimPosic
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitUser[0] &= ~(1UL << 1); // ZerarEncServo = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        GPIO_OUTPUT_PORT14 = 0; // YInvResetPos = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 88 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 6) & 1)) {  // ZerarEncPerfil
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 4) & 1)) {  // RSrvMotorParado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 6); // ZerarEncPerfil = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ENC_Reset(0, ArrayIntUser[9]);
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 2))) {  // XDbgDeslReadEnc
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[2] = 0; // EMaqPosAtual
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 90 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT12) {  // XSnsSupHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 7; else ArrayBitUser[0] &= ~(1UL << 7); // RSnsSupCortePerf = $rung_top

    /* ] finish series */

    /* start rung 91 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT13) {  // XSnsInfHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 8; else ArrayBitUser[0] &= ~(1UL << 8); // RSnsInfCortePerf = $rung_top

    /* ] finish series */

    /* start rung 92 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT14) {  // SnsSupPrensa
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 9; else ArrayBitUser[0] &= ~(1UL << 9); // SnsSupPrensaPerf = $rung_top

    /* ] finish series */

    /* start rung 93 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT15) {  // SnsInfPrensa
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 10; else ArrayBitUser[0] &= ~(1UL << 10); // SnsInfPrensaPerf = $rung_top

    /* ] finish series */

    /* start rung 94 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 11) & 1)) {  // RBombaHidrOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 12; else ArrayBitUser[0] &= ~(1UL << 12); // RCortePronto = $rung_top

    /* ] finish series */

    /* start rung 95 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 7) & 1)) {  // RSnsSupCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 8) & 1)) {  // RSnsInfCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 13; else ArrayBitUser[0] &= ~(1UL << 13); // RFerramCorteCima = $rung_top

    /* ] finish series */

    /* start rung 96 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 9) & 1)) {  // SnsSupPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 10) & 1)) {  // SnsInfPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 14; else ArrayBitUser[0] &= ~(1UL << 14); // FerramPrensaCima = $rung_top

    /* ] finish series */

    /* start rung 98 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 17) & 1)) {  // RForcaRecuoCorte
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 22) & 1)) {  // $oneShot_000d
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 22; else ArrayBitSystem[0] &= ~(1UL << 22); // $oneShot_000d = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 1; // YLigarBombaHidr = 1
    }

    /* ] finish series */

    /* start rung 99 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 11) & 1)) {  // RBombaHidrOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[10] < 29999) {
            ArrayIntUser[10]++;
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[10] = 0; // TBombaHidrInat
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 0; // YLigarBombaHidr = 0
    }

    /* ] finish series */

    /* start rung 100 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[11] < 499) {
            ArrayIntUser[11]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[11] = 0; // TBombaHidrEstab
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 11; else ArrayBitUser[0] &= ~(1UL << 11); // RBombaHidrOK = $rung_top

    /* ] finish series */

    /* start rung 102 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 19) & 1)) {  // ErroPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 20) & 1)) {  // PrensarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT11) {  // YAvancarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT12) {  // YRecuarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 16; // PrensaPerfAtiva = 1
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 20); // PrensarPerf = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 8);  // MbPrensaIHM = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 103 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[12] < 499) {
            ArrayIntUser[12]++;
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[12] = 0; // MaxPrensaPerf
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 16); // PrensaPerfAtiva = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 19; // ErroPrensaPerf = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 21); // PrensaPerfDesce = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 104 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 23) & 1)) {  // $oneShot_000e
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 23; else ArrayBitSystem[0] &= ~(1UL << 23); // $oneShot_000e = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] |= 1UL << 21; // PrensaPerfDesce = 1
    }

    /* ] finish series */

    /* start rung 105 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 21) & 1)) {  // PrensaPerfDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 10) & 1)) {  // SnsInfPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 21); // PrensaPerfDesce = 0
    }

    /* ] finish series */

    /* start rung 106 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 21) & 1)) {  // PrensaPerfDesce
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // ErroPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 9) & 1)) {  // SnsSupPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 24) & 1)) {  // $oneShot_000f
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 24; else ArrayBitSystem[0] &= ~(1UL << 24); // $oneShot_000f = $scratch

    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 16); // PrensaPerfAtiva = 0
    }

    /* ] finish series */

    /* start rung 107 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[0] >> 25) & 1)) {  // $oneShot_0010
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 25; else ArrayBitSystem[0] &= ~(1UL << 25); // $oneShot_0010 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 21); // PrensaPerfDesce = 0
    }

    /* ] finish series */

    /* start rung 109 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 22) & 1)) {  // RErroCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 23) & 1)) {  // RCortarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT11) {  // YAvancarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT12) {  // YRecuarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 15; // RCortePerfAtivo = 1
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 23); // RCortarPerf = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 1);  // YMbCorteIHM = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 110 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[13] < 499) {
            ArrayIntUser[13]++;
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[13] = 0; // TMaxCortePerf
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 15); // RCortePerfAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 22; // RErroCortePerf = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 24); // RCortePerfDesce = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 111 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 26) & 1)) {  // $oneShot_0011
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 26; else ArrayBitSystem[0] &= ~(1UL << 26); // $oneShot_0011 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] |= 1UL << 24; // RCortePerfDesce = 1
    }

    /* ] finish series */

    /* start rung 112 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 24) & 1)) {  // RCortePerfDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 8) & 1)) {  // RSnsInfCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 24); // RCortePerfDesce = 0
    }

    /* ] finish series */

    /* start rung 113 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 24) & 1)) {  // RCortePerfDesce
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 22) & 1)) {  // RErroCortePerf
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 7) & 1)) {  // RSnsSupCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 27) & 1)) {  // $oneShot_0012
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 27; else ArrayBitSystem[0] &= ~(1UL << 27); // $oneShot_0012 = $scratch

    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 15); // RCortePerfAtivo = 0
    }

    /* ] finish series */

    /* start rung 114 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[0] >> 28) & 1)) {  // $oneShot_0013
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 28; else ArrayBitSystem[0] &= ~(1UL << 28); // $oneShot_0013 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 24); // RCortePerfDesce = 0
    }

    /* ] finish series */

    /* start rung 116 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 24) & 1)) {  // RCortePerfDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT3 = ((ArrayBitSystem[0] >> 1) & 1); // YHidrDesce = $rung_top

    /* ] finish series */

    /* start rung 117 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 17) & 1)) {  // RForcaRecuoCorte
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 24) & 1)) {  // RCortePerfDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 7) & 1)) {  // RSnsSupCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT4 = ((ArrayBitSystem[0] >> 1) & 1); // YHidrSobe = $rung_top

    /* ] finish series */

    /* start rung 118 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 21) & 1)) {  // PrensaPerfDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT6 = ((ArrayBitSystem[0] >> 1) & 1); // PrensaDesce = $rung_top

    /* ] finish series */

    /* start rung 119 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 17) & 1)) {  // RForcaRecuoCorte
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 21) & 1)) {  // PrensaPerfDesce
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 9) & 1)) {  // SnsSupPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT7 = ((ArrayBitSystem[0] >> 1) & 1); // PrensaSobe = $rung_top

    /* ] finish series */

    /* start rung 120 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT3) {  // YHidrDesce
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT4) {  // YHidrSobe
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT6) {  // PrensaDesce
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!GPIO_OUTPUT_PORT7) {  // PrensaSobe
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    GPIO_OUTPUT_PORT5 = ((ArrayBitSystem[0] >> 1) & 1); // HidrVentagem = $rung_top

    /* ] finish series */

    /* start rung 121 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 22) & 1)) {  // RErroCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 19) & 1)) {  // ErroPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 7) & 1)) {  // RSnsSupCortePerf
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 9) & 1)) {  // SnsSupPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 17; else ArrayBitUser[0] &= ~(1UL << 17); // RForcaRecuoCorte = $rung_top

    /* ] finish series */

    /* start rung 122 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 22) & 1)) {  // RErroCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 23) & 1)) {  // RCortarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 22); // RErroCortePerf = 0
    }

    /* ] finish series */

    /* start rung 123 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (GPIO_INPUT_PORT16) {  // XPerfManAvanca
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (GPIO_INPUT_PORT17) {  // XPerfManRecua
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT18) {  // XCortePerfManual
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0001
    if (((ArrayBitSystem[0] >> 29) & 1)) {  // $oneShot_0014
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 29; else ArrayBitSystem[0] &= ~(1UL << 29); // $oneShot_0014 = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // YMbCorteIHM
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (!((ArrayBitUser[0] >> 25) & 1)) {  // RCortarPerfAuto
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 10) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0002
    if (((ArrayBitSystem[0] >> 30) & 1)) {  // $oneShot_0015
        ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 30; else ArrayBitSystem[0] &= ~(1UL << 30); // $oneShot_0015 = $scratch

    if (((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitUser[0] |= 1UL << 23; // RCortarPerf = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $rung_top
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        MODBUS_REGISTER[2] &= ~(1 << 1);  // YMbCorteIHM = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 124 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // ErroPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 20) & 1)) {  // PrensarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 19); // ErroPrensaPerf = 0
    }

    /* ] finish series */

    /* start rung 125 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (GPIO_INPUT_PORT16) {  // XPerfManAvanca
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (GPIO_INPUT_PORT17) {  // XPerfManRecua
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (!(MODBUS_REGISTER[2] & (1 << 8))) {  // MbPrensaIHM
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (!((ArrayBitUser[0] >> 26) & 1)) {  // PrensaPerfAuto
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0001
    if (((ArrayBitSystem[0] >> 31) & 1)) {  // $oneShot_0016
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 31; else ArrayBitSystem[0] &= ~(1UL << 31); // $oneShot_0016 = $scratch

    if (((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 20; // PrensarPerf = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[2] &= ~(1 << 8);  // MbPrensaIHM = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 127 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 27) & 1)) {  // RMaqIniciarPosic
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RAvancarPerfMan
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RRecuarPerfMan
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    if (!((ArrayBitSystem[1] >> 0) & 1)) {  // $TMantemFreioLib_antiglitch
        ArrayIntUser[14] = 199; // TMantemFreioLib
    }
    ArrayBitSystem[1] |= 1UL << 0; // $TMantemFreioLib_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[14] < 199) {
            ArrayIntUser[14]++;
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0000 = 1
        }
    } else {
        ArrayIntUser[14] = 0; // TMantemFreioLib
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 30; else ArrayBitUser[0] &= ~(1UL << 30); // RInvFreioLib = $rung_top

    /* ] finish series */

    /* start rung 128 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 30) & 1)) {  // RInvFreioLib
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 28) & 1)) {  // RAvancarPerfMan
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 27) & 1)) {  // RMaqIniciarPosic
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (!((ArrayBitUser[0] >> 31) & 1)) {  // SentidoAvancar
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 14) & 1)) {  // FerramPrensaCima
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 13) & 1)) {  // RFerramCorteCima
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT11 = ((ArrayBitSystem[0] >> 1) & 1); // YAvancarPerf = $rung_top

    /* ] finish series */

    /* start rung 129 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 30) & 1)) {  // RInvFreioLib
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 29) & 1)) {  // RRecuarPerfMan
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 27) & 1)) {  // RMaqIniciarPosic
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[0] >> 31) & 1)) {  // SentidoAvancar
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 14) & 1)) {  // FerramPrensaCima
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT11) {  // YAvancarPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT12 = ((ArrayBitSystem[0] >> 1) & 1); // YRecuarPerf = $rung_top

    /* ] finish series */

    /* start rung 131 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 13) & 1)) {  // RFerramCorteCima
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 14) & 1)) {  // FerramPrensaCima
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RMaqConfigOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 1; else ArrayBitUser[1] &= ~(1UL << 1); // RPerfManualOK = $rung_top

    /* ] finish series */

    /* start rung 132 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 1) & 1)) {  // RPerfManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!GPIO_INPUT_PORT16) {  // XPerfManAvanca
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!(MODBUS_REGISTER[2] & (1 << 3))) {  // YPerfManAvcIHM
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (GPIO_INPUT_PORT17) {  // XPerfManRecua
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (MODBUS_REGISTER[2] & (1 << 4)) {  // YPerfManRecIHM
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 28; else ArrayBitUser[0] &= ~(1UL << 28); // RAvancarPerfMan = $rung_top

    /* ] finish series */

    /* start rung 133 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 1) & 1)) {  // RPerfManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_INPUT_PORT16) {  // XPerfManAvanca
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (MODBUS_REGISTER[2] & (1 << 3)) {  // YPerfManAvcIHM
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!GPIO_INPUT_PORT17) {  // XPerfManRecua
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!(MODBUS_REGISTER[2] & (1 << 4))) {  // YPerfManRecIHM
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[1] >> 2) & 1)) {  // RBloqueiaRecuo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitUser[0] |= 1UL << 29; else ArrayBitUser[0] &= ~(1UL << 29); // RRecuarPerfMan = $parThis_0001

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0001
    if (!((ArrayBitUser[0] >> 30) & 1)) {  // RInvFreioLib
        ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0001
    if (!((ArrayBitUser[1] >> 2) & 1)) {  // RBloqueiaRecuo
        ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        if (ArrayIntUser[15] < 299) {
            ArrayIntUser[15]++;
            ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
        }
    } else {
        ArrayIntUser[15] = 0; // TMaxPerfRecua
    }

    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitUser[1] |= 1UL << 2; else ArrayBitUser[1] &= ~(1UL << 2); // RBloqueiaRecuo = $parThis_0001

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 135 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr
    
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 3) & 1)) {  // RStartConfigAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 4) & 1)) {  // RMaqConfigErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 1) & 1)) {  // $oneShot_0017
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 1; else ArrayBitSystem[1] &= ~(1UL << 1); // $oneShot_0017 = $scratch

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 0); // RMaqConfigOK = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[16] < 499) {
            ArrayIntUser[16]++;
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[16] = 0; // TConfigAutoTO
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 4; // RMaqConfigErro = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 2) & 1)) {  // $oneShot_0018
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 808, 1, &ArrayIntUser[17]);
                ArrayBitSystem[1] |= 1UL << 2; // $oneShot_0018 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 3); // $oneShot_0019 = 0
        }
        if (!((ArrayBitSystem[1] >> 3) & 1)) {  // $oneShot_0019
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 3; // $oneShot_0019 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 808, 1, &ArrayIntUser[17]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 2); // $oneShot_0018 = 0
        ArrayBitSystem[1] &= ~(1UL << 3); // $oneShot_0019 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 4) & 1)) {  // $oneShot_001a
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 804, 1, &ArrayIntUser[18]);
                ArrayBitSystem[1] |= 1UL << 4; // $oneShot_001a = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 5); // $oneShot_001b = 0
        }
        if (!((ArrayBitSystem[1] >> 5) & 1)) {  // $oneShot_001b
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 5; // $oneShot_001b = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 804, 1, &ArrayIntUser[18]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 4); // $oneShot_001a = 0
        ArrayBitSystem[1] &= ~(1UL << 5); // $oneShot_001b = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 6) & 1)) {  // $oneShot_001c
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 805, 1, &ArrayIntUser[19]);
                ArrayBitSystem[1] |= 1UL << 6; // $oneShot_001c = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 7); // $oneShot_001d = 0
        }
        if (!((ArrayBitSystem[1] >> 7) & 1)) {  // $oneShot_001d
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 7; // $oneShot_001d = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 805, 1, &ArrayIntUser[19]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 6); // $oneShot_001c = 0
        ArrayBitSystem[1] &= ~(1UL << 7); // $oneShot_001d = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 3); // RStartConfigAuto = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 0; // RMaqConfigOK = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 5; // MaqConfigDesl = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 136 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 6) & 1)) {  // RStartConfigMan
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 4) & 1)) {  // RMaqConfigErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 8) & 1)) {  // $oneShot_001e
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 8; else ArrayBitSystem[1] &= ~(1UL << 8); // $oneShot_001e = $scratch

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 0); // RMaqConfigOK = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[20] < 499) {
            ArrayIntUser[20]++;
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[20] = 0; // TConfigManTO
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 4; // RMaqConfigErro = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 9) & 1)) {  // $oneShot_001f
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 808, 1, &ArrayIntUser[21]);
                ArrayBitSystem[1] |= 1UL << 9; // $oneShot_001f = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 10); // $oneShot_0020 = 0
        }
        if (!((ArrayBitSystem[1] >> 10) & 1)) {  // $oneShot_0020
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 10; // $oneShot_0020 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 808, 1, &ArrayIntUser[21]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 9); // $oneShot_001f = 0
        ArrayBitSystem[1] &= ~(1UL << 10); // $oneShot_0020 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 11) & 1)) {  // $oneShot_0021
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 804, 1, &ArrayIntUser[22]);
                ArrayBitSystem[1] |= 1UL << 11; // $oneShot_0021 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 12); // $oneShot_0022 = 0
        }
        if (!((ArrayBitSystem[1] >> 12) & 1)) {  // $oneShot_0022
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 12; // $oneShot_0022 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 804, 1, &ArrayIntUser[22]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 11); // $oneShot_0021 = 0
        ArrayBitSystem[1] &= ~(1UL << 12); // $oneShot_0022 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 13) & 1)) {  // $oneShot_0023
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 805, 1, &ArrayIntUser[23]);
                ArrayBitSystem[1] |= 1UL << 13; // $oneShot_0023 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 14); // $oneShot_0024 = 0
        }
        if (!((ArrayBitSystem[1] >> 14) & 1)) {  // $oneShot_0024
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 14; // $oneShot_0024 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 805, 1, &ArrayIntUser[23]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 13); // $oneShot_0023 = 0
        ArrayBitSystem[1] &= ~(1UL << 14); // $oneShot_0024 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 6); // RStartConfigMan = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 0; // RMaqConfigOK = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 138 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 7) & 1)) {  // InvSyncAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[24] < 1) {
            ArrayIntUser[24]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[24] = 0; // EspInvSyncAtivo
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!I_SerialAborted) {  // $SerialAborted
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 4; // RMaqConfigErro = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 7); // InvSyncAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 15) & 1)) {  // $oneShot_0025
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_READ_HOLDING_REGISTERS, 4, 1, &ArrayIntUser[25]);
                ArrayBitSystem[1] |= 1UL << 15; // $oneShot_0025 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 16); // $oneShot_0026 = 0
        }
        if (!((ArrayBitSystem[1] >> 16) & 1)) {  // $oneShot_0026
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 16; // $oneShot_0026 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_READ_HOLDING_REGISTERS, 4, 1, &ArrayIntUser[25]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 15); // $oneShot_0025 = 0
        ArrayBitSystem[1] &= ~(1UL << 16); // $oneShot_0026 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 17) & 1)) {  // $oneShot_0027
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_READ_HOLDING_REGISTERS, 3, 1, &ArrayIntUser[26]);
                ArrayBitSystem[1] |= 1UL << 17; // $oneShot_0027 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 18); // $oneShot_0028 = 0
        }
        if (!((ArrayBitSystem[1] >> 18) & 1)) {  // $oneShot_0028
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 18; // $oneShot_0028 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_READ_HOLDING_REGISTERS, 3, 1, &ArrayIntUser[26]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 17); // $oneShot_0027 = 0
        ArrayBitSystem[1] &= ~(1UL << 18); // $oneShot_0028 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 19) & 1)) {  // $oneShot_0029
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_READ_HOLDING_REGISTERS, 12, 1, &ArrayIntUser[27]);
                ArrayBitSystem[1] |= 1UL << 19; // $oneShot_0029 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 20); // $oneShot_002a = 0
        }
        if (!((ArrayBitSystem[1] >> 20) & 1)) {  // $oneShot_002a
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 20; // $oneShot_002a = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_READ_HOLDING_REGISTERS, 12, 1, &ArrayIntUser[27]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 19); // $oneShot_0029 = 0
        ArrayBitSystem[1] &= ~(1UL << 20); // $oneShot_002a = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 21) & 1)) {  // $oneShot_002b
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, 0UL, MODBUS_FC_READ_HOLDING_REGISTERS, 13, 1, &ArrayIntUser[28]);
                ArrayBitSystem[1] |= 1UL << 21; // $oneShot_002b = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            ArrayBitSystem[1] &= ~(1UL << 22); // $oneShot_002c = 0
        }
        if (!((ArrayBitSystem[1] >> 22) & 1)) {  // $oneShot_002c
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[1] |= 1UL << 22; // $oneShot_002c = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                Modbus_Send(1, 0UL, MODBUS_FC_READ_HOLDING_REGISTERS, 13, 1, &ArrayIntUser[28]);
            }
        }
    } else {
        ArrayBitSystem[1] &= ~(1UL << 21); // $oneShot_002b = 0
        ArrayBitSystem[1] &= ~(1UL << 22); // $oneShot_002c = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 7); // InvSyncAtivo = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 139 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 7) & 1)) {  // InvSyncAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[1] >> 23) & 1)) {  // $oneShot_002d
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 23; else ArrayBitSystem[1] &= ~(1UL << 23); // $oneShot_002d = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[2] &= ~(1 << 7);  // InvSync = 0
    }

    /* ] finish series */

    /* start rung 142 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[1] >> 24) & 1)) {  // $oneShot_002e
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 24; else ArrayBitSystem[1] &= ~(1UL << 24); // $oneShot_002e = $scratch

    if (((ArrayBitUser[0] >> 19) & 1)) {  // ErroPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 1; // $scratch2
        ArrayIntUser[29] = ArrayIntUser[29] + ArrayIntUser[3]; // QtdPecasFila = QtdPecasFila + $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[1] >> 25) & 1)) {  // $oneShot_002f
            ArrayIntUser[30] = ArrayIntUser[31]; // PecasFila39 = PecasFila38
            ArrayIntUser[31] = ArrayIntUser[32]; // PecasFila38 = PecasFila37
            ArrayIntUser[32] = ArrayIntUser[33]; // PecasFila37 = PecasFila36
            ArrayIntUser[33] = ArrayIntUser[34]; // PecasFila36 = PecasFila35
            ArrayIntUser[34] = ArrayIntUser[35]; // PecasFila35 = PecasFila34
            ArrayIntUser[35] = ArrayIntUser[36]; // PecasFila34 = PecasFila33
            ArrayIntUser[36] = ArrayIntUser[37]; // PecasFila33 = PecasFila32
            ArrayIntUser[37] = ArrayIntUser[38]; // PecasFila32 = PecasFila31
            ArrayIntUser[38] = ArrayIntUser[39]; // PecasFila31 = PecasFila30
            ArrayIntUser[39] = ArrayIntUser[40]; // PecasFila30 = PecasFila29
            ArrayIntUser[40] = ArrayIntUser[41]; // PecasFila29 = PecasFila28
            ArrayIntUser[41] = ArrayIntUser[42]; // PecasFila28 = PecasFila27
            ArrayIntUser[42] = ArrayIntUser[43]; // PecasFila27 = PecasFila26
            ArrayIntUser[43] = ArrayIntUser[44]; // PecasFila26 = PecasFila25
            ArrayIntUser[44] = ArrayIntUser[45]; // PecasFila25 = PecasFila24
            ArrayIntUser[45] = ArrayIntUser[46]; // PecasFila24 = PecasFila23
            ArrayIntUser[46] = ArrayIntUser[47]; // PecasFila23 = PecasFila22
            ArrayIntUser[47] = ArrayIntUser[48]; // PecasFila22 = PecasFila21
            ArrayIntUser[48] = ArrayIntUser[49]; // PecasFila21 = PecasFila20
            ArrayIntUser[49] = ArrayIntUser[50]; // PecasFila20 = PecasFila19
            ArrayIntUser[50] = ArrayIntUser[51]; // PecasFila19 = PecasFila18
            ArrayIntUser[51] = ArrayIntUser[52]; // PecasFila18 = PecasFila17
            ArrayIntUser[52] = ArrayIntUser[53]; // PecasFila17 = PecasFila16
            ArrayIntUser[53] = ArrayIntUser[54]; // PecasFila16 = PecasFila15
            ArrayIntUser[54] = ArrayIntUser[55]; // PecasFila15 = PecasFila14
            ArrayIntUser[55] = ArrayIntUser[56]; // PecasFila14 = PecasFila13
            ArrayIntUser[56] = ArrayIntUser[57]; // PecasFila13 = PecasFila12
            ArrayIntUser[57] = ArrayIntUser[58]; // PecasFila12 = PecasFila11
            ArrayIntUser[58] = ArrayIntUser[59]; // PecasFila11 = PecasFila10
            ArrayIntUser[59] = ArrayIntUser[60]; // PecasFila10 = PecasFila9
            ArrayIntUser[60] = ArrayIntUser[61]; // PecasFila9 = PecasFila8
            ArrayIntUser[61] = ArrayIntUser[62]; // PecasFila8 = PecasFila7
            ArrayIntUser[62] = ArrayIntUser[63]; // PecasFila7 = PecasFila6
            ArrayIntUser[63] = ArrayIntUser[64]; // PecasFila6 = PecasFila5
            ArrayIntUser[64] = ArrayIntUser[65]; // PecasFila5 = PecasFila4
            ArrayIntUser[65] = ArrayIntUser[66]; // PecasFila4 = PecasFila3
            ArrayIntUser[66] = ArrayIntUser[67]; // PecasFila3 = PecasFila2
            ArrayIntUser[67] = ArrayIntUser[68]; // PecasFila2 = PecasFila1
            ArrayIntUser[68] = ArrayIntUser[69]; // PecasFila1 = PecasFila0
        }
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[1] |= 1UL << 25; else ArrayBitSystem[1] &= ~(1UL << 25); // $oneShot_002f = $parThis_0000

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[69] = MODBUS_REGISTER[24]; // PecasFila0 = DistPrensaCorte
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[70] = ArrayIntUser[71]; // DistanciaPrensa = PecaTam
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitUser[1] |= 1UL << 8; else ArrayBitUser[1] &= ~(1UL << 8); // Prensa = $parThis_0000

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 144 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[1] >> 26) & 1)) {  // $oneShot_0030
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 26; else ArrayBitSystem[1] &= ~(1UL << 26); // $oneShot_0030 = $scratch

    if (((ArrayBitUser[0] >> 22) & 1)) {  // RErroCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 1; // $scratch2
        ArrayIntUser[29] = ArrayIntUser[29] - ArrayIntUser[3]; // QtdPecasFila = QtdPecasFila - $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitUser[1] |= 1UL << 9; else ArrayBitUser[1] &= ~(1UL << 9); // Corte = $parThis_0000

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 146 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 8) & 1)) {  // Prensa
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 9) & 1)) {  // Corte
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[1] >> 27) & 1)) {  // $oneShot_0031
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 27; else ArrayBitSystem[1] &= ~(1UL << 27); // $oneShot_0031 = $scratch

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 10; else ArrayBitUser[1] &= ~(1UL << 10); // AtualizarFila = $rung_top

    /* ] finish series */

    /* start rung 148 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 11) & 1)) {  // AtualizarPosFila
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 11); // AtualizarPosFila = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[70] = ArrayIntUser[70] - MODBUS_REGISTER[23]; // DistanciaPrensa = DistanciaPrensa - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[69] = ArrayIntUser[69] - MODBUS_REGISTER[23]; // PecasFila0 = PecasFila0 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[68] = ArrayIntUser[68] - MODBUS_REGISTER[23]; // PecasFila1 = PecasFila1 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[67] = ArrayIntUser[67] - MODBUS_REGISTER[23]; // PecasFila2 = PecasFila2 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[66] = ArrayIntUser[66] - MODBUS_REGISTER[23]; // PecasFila3 = PecasFila3 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[65] = ArrayIntUser[65] - MODBUS_REGISTER[23]; // PecasFila4 = PecasFila4 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[64] = ArrayIntUser[64] - MODBUS_REGISTER[23]; // PecasFila5 = PecasFila5 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[63] = ArrayIntUser[63] - MODBUS_REGISTER[23]; // PecasFila6 = PecasFila6 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[62] = ArrayIntUser[62] - MODBUS_REGISTER[23]; // PecasFila7 = PecasFila7 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[61] = ArrayIntUser[61] - MODBUS_REGISTER[23]; // PecasFila8 = PecasFila8 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[60] = ArrayIntUser[60] - MODBUS_REGISTER[23]; // PecasFila9 = PecasFila9 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[59] = ArrayIntUser[59] - MODBUS_REGISTER[23]; // PecasFila10 = PecasFila10 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[58] = ArrayIntUser[58] - MODBUS_REGISTER[23]; // PecasFila11 = PecasFila11 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[57] = ArrayIntUser[57] - MODBUS_REGISTER[23]; // PecasFila12 = PecasFila12 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[56] = ArrayIntUser[56] - MODBUS_REGISTER[23]; // PecasFila13 = PecasFila13 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[55] = ArrayIntUser[55] - MODBUS_REGISTER[23]; // PecasFila14 = PecasFila14 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[54] = ArrayIntUser[54] - MODBUS_REGISTER[23]; // PecasFila15 = PecasFila15 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[53] = ArrayIntUser[53] - MODBUS_REGISTER[23]; // PecasFila16 = PecasFila16 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[52] = ArrayIntUser[52] - MODBUS_REGISTER[23]; // PecasFila17 = PecasFila17 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[51] = ArrayIntUser[51] - MODBUS_REGISTER[23]; // PecasFila18 = PecasFila18 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[50] = ArrayIntUser[50] - MODBUS_REGISTER[23]; // PecasFila19 = PecasFila19 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[49] = ArrayIntUser[49] - MODBUS_REGISTER[23]; // PecasFila20 = PecasFila20 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[48] = ArrayIntUser[48] - MODBUS_REGISTER[23]; // PecasFila21 = PecasFila21 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[47] = ArrayIntUser[47] - MODBUS_REGISTER[23]; // PecasFila22 = PecasFila22 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[46] = ArrayIntUser[46] - MODBUS_REGISTER[23]; // PecasFila23 = PecasFila23 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[45] = ArrayIntUser[45] - MODBUS_REGISTER[23]; // PecasFila24 = PecasFila24 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[44] = ArrayIntUser[44] - MODBUS_REGISTER[23]; // PecasFila25 = PecasFila25 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[43] = ArrayIntUser[43] - MODBUS_REGISTER[23]; // PecasFila26 = PecasFila26 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[42] = ArrayIntUser[42] - MODBUS_REGISTER[23]; // PecasFila27 = PecasFila27 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[41] = ArrayIntUser[41] - MODBUS_REGISTER[23]; // PecasFila28 = PecasFila28 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[40] = ArrayIntUser[40] - MODBUS_REGISTER[23]; // PecasFila29 = PecasFila29 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[39] = ArrayIntUser[39] - MODBUS_REGISTER[23]; // PecasFila30 = PecasFila30 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[38] = ArrayIntUser[38] - MODBUS_REGISTER[23]; // PecasFila31 = PecasFila31 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[37] = ArrayIntUser[37] - MODBUS_REGISTER[23]; // PecasFila32 = PecasFila32 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[36] = ArrayIntUser[36] - MODBUS_REGISTER[23]; // PecasFila33 = PecasFila33 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[35] = ArrayIntUser[35] - MODBUS_REGISTER[23]; // PecasFila34 = PecasFila34 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[34] = ArrayIntUser[34] - MODBUS_REGISTER[23]; // PecasFila35 = PecasFila35 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[33] = ArrayIntUser[33] - MODBUS_REGISTER[23]; // PecasFila36 = PecasFila36 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[32] = ArrayIntUser[32] - MODBUS_REGISTER[23]; // PecasFila37 = PecasFila37 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[31] = ArrayIntUser[31] - MODBUS_REGISTER[23]; // PecasFila38 = PecasFila38 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[30] = ArrayIntUser[30] - MODBUS_REGISTER[23]; // PecasFila39 = PecasFila39 - Deslocamento
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 150 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 10) & 1)) {  // AtualizarFila
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[69]; // DistanciaCorte = PecasFila0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 2; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[68]; // DistanciaCorte = PecasFila1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 3; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[67]; // DistanciaCorte = PecasFila2
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 4; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[66]; // DistanciaCorte = PecasFila3
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 5; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[65]; // DistanciaCorte = PecasFila4
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 6; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[64]; // DistanciaCorte = PecasFila5
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 7; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[63]; // DistanciaCorte = PecasFila6
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 8; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[62]; // DistanciaCorte = PecasFila7
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 9; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[61]; // DistanciaCorte = PecasFila8
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 10; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[60]; // DistanciaCorte = PecasFila9
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 11; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[59]; // DistanciaCorte = PecasFila10
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 12; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[58]; // DistanciaCorte = PecasFila11
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 13; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[57]; // DistanciaCorte = PecasFila12
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 14; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[56]; // DistanciaCorte = PecasFila13
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 15; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[55]; // DistanciaCorte = PecasFila14
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 16; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[54]; // DistanciaCorte = PecasFila15
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 17; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[53]; // DistanciaCorte = PecasFila16
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 18; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[52]; // DistanciaCorte = PecasFila17
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 19; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[51]; // DistanciaCorte = PecasFila18
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 20; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[50]; // DistanciaCorte = PecasFila19
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 21; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[49]; // DistanciaCorte = PecasFila20
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 22; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[48]; // DistanciaCorte = PecasFila21
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 23; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[47]; // DistanciaCorte = PecasFila22
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 24; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[46]; // DistanciaCorte = PecasFila23
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 25; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[45]; // DistanciaCorte = PecasFila24
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 26; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[44]; // DistanciaCorte = PecasFila25
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 27; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[43]; // DistanciaCorte = PecasFila26
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 28; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[42]; // DistanciaCorte = PecasFila27
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 29; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[41]; // DistanciaCorte = PecasFila28
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 30; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[40]; // DistanciaCorte = PecasFila29
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 31; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[39]; // DistanciaCorte = PecasFila30
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 32; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[38]; // DistanciaCorte = PecasFila31
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 33; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[37]; // DistanciaCorte = PecasFila32
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 34; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[36]; // DistanciaCorte = PecasFila33
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 35; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[35]; // DistanciaCorte = PecasFila34
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 36; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[34]; // DistanciaCorte = PecasFila35
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 37; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[33]; // DistanciaCorte = PecasFila36
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 38; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[32]; // DistanciaCorte = PecasFila37
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 39; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[31]; // DistanciaCorte = PecasFila38
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 40; // $scratch2
    if (ArrayIntUser[29] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[72] = ArrayIntUser[30]; // DistanciaCorte = PecasFila39
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 152 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 10) & 1)) {  // AtualizarFila
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[1] >> 12) & 1)) {  // PecaTamAlterado
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 13) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 28) & 1)) {  // $oneShot_0032
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 28; else ArrayBitSystem[1] &= ~(1UL << 28); // $oneShot_0032 = $scratch

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[23] = ArrayIntUser[70]; // Deslocamento = DistanciaPrensa
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if (ArrayIntUser[70] > ArrayIntUser[72]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    ArrayIntUser[3] = 1; // $scratch2
    if (ArrayIntUser[3] > ArrayIntUser[29]) {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[23] = ArrayIntUser[72]; // Deslocamento = DistanciaCorte
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 12); // PecaTamAlterado = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 154 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 13) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 14; else ArrayBitUser[1] &= ~(1UL << 14); // RModoAutoOK = $rung_top

    /* ] finish series */

    /* start rung 155 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 13) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[1] >> 29) & 1)) {  // $oneShot_0033
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 29; else ArrayBitSystem[1] &= ~(1UL << 29); // $oneShot_0033 = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[73] < 999) {
            ArrayIntUser[73]++;
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[73] = 0; // TEsperaInit
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[1] >> 30) & 1)) {  // $oneShot_0034
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 30; else ArrayBitSystem[1] &= ~(1UL << 30); // $oneShot_0034 = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 6; // RStartConfigMan = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 3); // RStartConfigAuto = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 156 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 13) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[1] >> 31) & 1)) {  // $oneShot_0035
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[1] |= 1UL << 31; else ArrayBitSystem[1] &= ~(1UL << 31); // $oneShot_0035 = $scratch

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 3; // RStartConfigAuto = 1
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 6); // RStartConfigMan = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[74] = MODBUS_REGISTER[30]; // CQtdProd = YMbQtd
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 157 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RMaqConfigOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 0) & 1)) {  // $oneShot_0036
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 0; else ArrayBitSystem[2] &= ~(1UL << 0); // $oneShot_0036 = $scratch

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 14) & 1)) {  // RModoAutoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayIntUser[3] = 0; // $scratch2
    if (ArrayIntUser[74] > ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] |= 1UL << 18; // RMaqProduzindo = 1
    }

    /* ] finish series */

    /* start rung 158 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT11) {  // XInvFimPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[75] < 9) {
            ArrayIntUser[75]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[75] = 0; // TFimPosicEstab
    }

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 1) & 1)) {  // $oneShot_0037
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 1; else ArrayBitSystem[2] &= ~(1UL << 1); // $oneShot_0037 = $scratch

    if (!((ArrayBitUser[0] >> 27) & 1)) {  // RMaqIniciarPosic
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 15; // MaqInPos = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 2) & 1)) {  // $oneShot_0038
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 2; else ArrayBitSystem[2] &= ~(1UL << 2); // $oneShot_0038 = $scratch

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 15); // MaqInPos = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 159 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 22) & 1)) {  // RErroCortePerf
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 19) & 1)) {  // ErroPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] &= ~(1UL << 18); // RMaqProduzindo = 0
    }

    /* ] finish series */

    /* start rung 160 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 3) & 1)) {  // $oneShot_0039
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 3; else ArrayBitSystem[2] &= ~(1UL << 3); // $oneShot_0039 = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 4) & 1)) {  // $oneShot_003a
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 4; else ArrayBitSystem[2] &= ~(1UL << 4); // $oneShot_003a = $scratch

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YMbModoAutoAtivo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RMaqConfigOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 5) & 1)) {  // $oneShot_003b
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 5; else ArrayBitSystem[2] &= ~(1UL << 5); // $oneShot_003b = $scratch

    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 25); // RCortarPerfAuto = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 26); // PrensaPerfAuto = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 16); // SrvLigaMotor = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 17); // PecaProduzida = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 15); // MaqInPos = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 16); // SrvLigaMotor = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 162 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[76] = ArrayIntUser[4] - MODBUS_REGISTER[23]; // DifPosic = MaqPosAtual - Deslocamento
    }

    /* ] finish series */

    /* start rung 163 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0

    ArrayIntUser[3] = 50; // $scratch2
    if (ArrayIntUser[3] > ArrayIntUser[76]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    ArrayIntUser[3] = -50; // $scratch2
    if (ArrayIntUser[76] > ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */

    ArrayIntUser[3] = 50000; // $scratch2
    if (ArrayIntUser[3] > ArrayIntUser[76]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    ArrayIntUser[3] = -50000; // $scratch2
    if (ArrayIntUser[76] > ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 18; else ArrayBitUser[1] &= ~(1UL << 18); // RMaqPosicOK = $rung_top

    /* ] finish series */

    /* start rung 164 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 19) & 1)) {  // MaqPosicionar
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 6) & 1)) {  // $oneShot_003c
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 6; else ArrayBitSystem[2] &= ~(1UL << 6); // $oneShot_003c = $scratch

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitUser[1] >> 20) & 1)) {  // RMaqErroPosic
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 6; // ZerarEncPerfil = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 1; // ZerarEncServo = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[77] = 10; // MaqQtdTent
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 7) & 1)) {  // $oneShot_003d
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 7; else ArrayBitSystem[2] &= ~(1UL << 7); // $oneShot_003d = $scratch

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 16); // SrvLigaMotor = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 165 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 20) & 1)) {  // RMaqErroPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[2] >> 8) & 1)) {  // $oneShot_003e
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 8; else ArrayBitSystem[2] &= ~(1UL << 8); // $oneShot_003e = $scratch

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[0] >> 4) & 1)) {  // RSrvMotorParado
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[0] |= 1UL << 1; // ZerarEncServo = 1
    }

    /* ] finish series */

    /* start rung 166 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 15) & 1)) {  // MaqInPos
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 9) & 1)) {  // $oneShot_003f
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 9; else ArrayBitSystem[2] &= ~(1UL << 9); // $oneShot_003f = $scratch

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] &= ~(1UL << 16); // SrvLigaMotor = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 18) & 1)) {  // RMaqPosicOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 21; // RMaqFimPosic = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[1] >> 18) & 1)) {  // RMaqPosicOK
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        if (!((ArrayBitSystem[2] >> 10) & 1)) {  // $oneShot_0040
            ArrayBitSystem[0] |= 1UL << 6; // $scratch
            ArrayIntUser[77] = ArrayIntUser[77] - ((ArrayBitSystem[0] >> 6) & 1); // MaqQtdTent = MaqQtdTent - $scratch
        }
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[2] |= 1UL << 10; else ArrayBitSystem[2] &= ~(1UL << 10); // $oneShot_0040 = $parThis_0001
    if (ArrayIntUser[77] > VAR_NULL) {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    } else {
        ArrayBitSystem[0] |= 1UL << 5; // $parThis_0001 = 1
    }

    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0001
    if (((ArrayBitSystem[2] >> 11) & 1)) {  // $oneShot_0041
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 11; else ArrayBitSystem[2] &= ~(1UL << 11); // $oneShot_0041 = $scratch

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitUser[1] |= 1UL << 20; // RMaqErroPosic = 1
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayIntUser[3] = 10; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[78] = ArrayIntUser[76] / ArrayIntUser[3]; // YMbMaqErroPos = DifPosic / $scratch2
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    if (((ArrayBitUser[1] >> 20) & 1)) {  // RMaqErroPosic
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 1; // ZerarEncServo = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 167 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 1) & 1)) {  // ZerarEncServo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[2] >> 12) & 1)) {  // $oneShot_0042
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 12; else ArrayBitSystem[2] &= ~(1UL << 12); // $oneShot_0042 = $scratch

    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 19) & 1)) {  // MaqPosicionar
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[1] |= 1UL << 16; // SrvLigaMotor = 1
    }

    /* ] finish series */

    /* start rung 168 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 16) & 1)) {  // SrvLigaMotor
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 1) & 1)) {  // ZerarEncServo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 6) & 1)) {  // ZerarEncPerfil
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitUser[0] |= 1UL << 27; else ArrayBitUser[0] &= ~(1UL << 27); // RMaqIniciarPosic = $parThis_0000

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitSystem[2] >> 13) & 1)) {  // $TEspSrvParar_antiglitch
        ArrayIntUser[79] = 9; // TEspSrvParar
    }
    ArrayBitSystem[2] |= 1UL << 13; // $TEspSrvParar_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (ArrayIntUser[79] < 9) {
            ArrayIntUser[79]++;
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0000 = 1
        }
    } else {
        ArrayIntUser[79] = 0; // TEspSrvParar
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 4); // RSrvMotorParado = 0
    } else {
        ArrayBitUser[0] |= 1UL << 4; // RSrvMotorParado = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 170 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 21) & 1)) {  // RMaqFimPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RMaqConfigOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 19; else ArrayBitUser[1] &= ~(1UL << 19); // MaqPosicionar = $rung_top

    /* ] finish series */

    /* start rung 171 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 21) & 1)) {  // RMaqFimPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 30) & 1)) {  // RInvFreioLib
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 14) & 1)) {  // $oneShot_0043
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 14; else ArrayBitSystem[2] &= ~(1UL << 14); // $oneShot_0043 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (ArrayIntUser[72] == MODBUS_REGISTER[23]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    ArrayIntUser[3] = 0; // $scratch2
    if (ArrayIntUser[29] > ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 25; // RCortarPerfAuto = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (ArrayIntUser[70] == MODBUS_REGISTER[23]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 26; // PrensaPerfAuto = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 17; // PecaProduzida = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 11; // AtualizarPosFila = 1
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 172 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!((ArrayBitUser[1] >> 21) & 1)) {  // RMaqFimPosic
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 15) & 1)) {  // RCortePerfAtivo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 16) & 1)) {  // PrensaPerfAtiva
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 25); // RCortarPerfAuto = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 26); // PrensaPerfAuto = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0001
    if (!((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        if (((ArrayBitSystem[2] >> 15) & 1)) {  // $oneShot_0044
            ArrayBitSystem[0] |= 1UL << 5; // $parThis_0001 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 15; else ArrayBitSystem[2] &= ~(1UL << 15); // $oneShot_0044 = $scratch

    if (((ArrayBitUser[0] >> 22) & 1)) {  // RErroCortePerf
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitUser[0] >> 19) & 1)) {  // ErroPrensaPerf
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0001
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitUser[1] &= ~(1UL << 21); // RMaqFimPosic = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parThis_0001
    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 17) & 1)) {  // PecaProduzida
        ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[2] &= ~(1UL << 16); // $parOut_0003 = 0
    if(((ArrayBitSystem[0] >> 10) & 1)) ArrayBitSystem[2] |= 1UL << 17; else ArrayBitSystem[2] &= ~(1UL << 17); // $parThis_0003 = $parThis_0002
    if (((ArrayBitSystem[2] >> 17) & 1)) {  // $parThis_0003
        ArrayBitUser[1] &= ~(1UL << 17); // PecaProduzida = 0
    }

    if (((ArrayBitSystem[2] >> 17) & 1)) {  // $parThis_0003
        ArrayBitSystem[2] |= 1UL << 16; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 10) & 1)) ArrayBitSystem[2] |= 1UL << 17; else ArrayBitSystem[2] &= ~(1UL << 17); // $parThis_0003 = $parThis_0002
    if (((ArrayBitSystem[2] >> 17) & 1)) {  // $parThis_0003
        ArrayIntUser[3] = 1; // $scratch2
        MODBUS_REGISTER[30] = MODBUS_REGISTER[30] - ArrayIntUser[3]; // YMbQtd = YMbQtd - $scratch2
    }

    if (((ArrayBitSystem[2] >> 17) & 1)) {  // $parThis_0003
        ArrayBitSystem[2] |= 1UL << 16; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 10) & 1)) ArrayBitSystem[2] |= 1UL << 17; else ArrayBitSystem[2] &= ~(1UL << 17); // $parThis_0003 = $parThis_0002
    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[2] &= ~(1UL << 18); // $parOut_0004 = 0
    if(((ArrayBitSystem[2] >> 17) & 1)) ArrayBitSystem[2] |= 1UL << 19; else ArrayBitSystem[2] &= ~(1UL << 19); // $parThis_0004 = $parThis_0003
    if (((ArrayBitSystem[2] >> 19) & 1)) {  // $parThis_0004
        if (!((ArrayBitSystem[2] >> 20) & 1)) {  // $oneShot_0045
            ArrayBitSystem[0] |= 1UL << 6; // $scratch
            ArrayIntUser[74] = ArrayIntUser[74] - ((ArrayBitSystem[0] >> 6) & 1); // CQtdProd = CQtdProd - $scratch
        }
    }
    if(((ArrayBitSystem[2] >> 19) & 1)) ArrayBitSystem[2] |= 1UL << 20; else ArrayBitSystem[2] &= ~(1UL << 20); // $oneShot_0045 = $parThis_0004
    if (ArrayIntUser[74] > VAR_NULL) {
        ArrayBitSystem[2] &= ~(1UL << 19); // $parThis_0004 = 0
    } else {
        ArrayBitSystem[2] |= 1UL << 19; // $parThis_0004 = 1
    }

    if (((ArrayBitSystem[2] >> 19) & 1)) {  // $parThis_0004
        ArrayBitSystem[2] |= 1UL << 18; // $parOut_0004 = 1
    }
    if(((ArrayBitSystem[2] >> 17) & 1)) ArrayBitSystem[2] |= 1UL << 19; else ArrayBitSystem[2] &= ~(1UL << 19); // $parThis_0004 = $parThis_0003
    if (((ArrayBitUser[1] >> 13) & 1)) {  // RModoAuto
        ArrayBitSystem[2] &= ~(1UL << 19); // $parThis_0004 = 0
    }

    if (((ArrayBitSystem[2] >> 19) & 1)) {  // $parThis_0004
        ArrayBitSystem[2] |= 1UL << 18; // $parOut_0004 = 1
    }
    if(((ArrayBitSystem[2] >> 18) & 1)) ArrayBitSystem[2] |= 1UL << 17; else ArrayBitSystem[2] &= ~(1UL << 17); // $parThis_0003 = $parOut_0004
    /* ] finish parallel */
    if (((ArrayBitSystem[2] >> 17) & 1)) {  // $parThis_0003
        ArrayBitUser[0] &= ~(1UL << 18); // RMaqProduzindo = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[2] >> 17) & 1)) {  // $parThis_0003
        ArrayBitSystem[2] |= 1UL << 16; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[2] >> 16) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0002 = $parOut_0003
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 174 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 21) & 1)) {  // $oneShot_0046
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 21; else ArrayBitSystem[2] &= ~(1UL << 21); // $oneShot_0046 = $scratch

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] |= (1 << 0);  // YMbModoAuto = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[2] >> 22) & 1)) {  // $oneShot_0047
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 22; else ArrayBitSystem[2] &= ~(1UL << 22); // $oneShot_0047 = $scratch

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 0);  // YMbModoAuto = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0000
    if (((ArrayBitSystem[2] >> 23) & 1)) {  // $oneShot_0048
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 23; else ArrayBitSystem[2] &= ~(1UL << 23); // $oneShot_0048 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[30] = 3; // YMbQtd
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[31] = 1000; // YMbTam
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 175 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // YMbModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[1] |= 1UL << 13; else ArrayBitUser[1] &= ~(1UL << 13); // RModoAuto = $rung_top

    /* ] finish series */

    /* start rung 176 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 13) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 24) & 1)) {  // $oneShot_0049
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 24; else ArrayBitSystem[2] &= ~(1UL << 24); // $oneShot_0049 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[1] |= (1 << 0);  // YMbModoAutoAtivo = 1
    }

    /* ] finish series */

    /* start rung 177 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 2))) {  // YMbLimpaErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 2);  // YMbLimpaErro = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[0] &= ~(1 << 7);  // ErroTamPeca = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] &= ~(1UL << 4); // RMaqConfigErro = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 22); // RErroCortePerf = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 19); // ErroPrensaPerf = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 178 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 5))) {  // YMbPerfSync
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 13) & 1)) {  // RModoAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[1] |= 1UL << 6; // RStartConfigMan = 1
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 5);  // YMbPerfSync = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 179 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 7))) {  // InvSync
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 3) & 1)) {  // RStartConfigAuto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[1] >> 6) & 1)) {  // RStartConfigMan
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayBitUser[1] |= 1UL << 7; // InvSyncAtivo = 1
    }

    /* ] finish series */

    /* start rung 181 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT1) {  // XEmergencia
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 0); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 0;  // YMbErrEmergencia = $rung_top

    /* ] finish series */

    /* start rung 182 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT6) {  // XFaltaFase
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 1); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 1;  // YMbErrFaltaFase = $rung_top

    /* ] finish series */

    /* start rung 183 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT2) {  // XTermBombaHidr
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 2); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 2;  // YMbErrBombaHidr = $rung_top

    /* ] finish series */

    /* start rung 184 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_INPUT_PORT4) {  // ServoOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 3); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 3;  // YMbErrInversor = $rung_top

    /* ] finish series */

    /* start rung 185 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 4) & 1)) {  // RMaqConfigErro
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 5); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 5;  // YMbErrInvComunic = $rung_top

    /* ] finish series */

    /* start rung 186 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 22) & 1)) {  // RErroCortePerf
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[0] &= ~(1 << 6); MODBUS_REGISTER[0] |= ((ArrayBitSystem[0] >> 1) & 1) << 6;  // YMbErrCortePerf = $rung_top

    /* ] finish series */

    /* start rung 188 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // RMaqOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 1); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 1;  // YMbMaqOK = $rung_top

    /* ] finish series */

    /* start rung 189 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 0) & 1)) {  // RMaqConfigOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 2); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 2;  // YMbMaqConfigOK = $rung_top

    /* ] finish series */

    /* start rung 190 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 12) & 1)) {  // RCortePronto
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 3); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 3;  // YMbCortePronto = $rung_top

    /* ] finish series */

    /* start rung 191 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 13) & 1)) {  // RFerramCorteCima
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 4); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 4;  // YMbFerrCorteCima = $rung_top

    /* ] finish series */

    /* start rung 192 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[1] >> 1) & 1)) {  // RPerfManualOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 5); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 5;  // YMbPerfManualOK = $rung_top

    /* ] finish series */

    /* start rung 193 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 18) & 1)) {  // RMaqProduzindo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    MODBUS_REGISTER[1] &= ~(1 << 7); MODBUS_REGISTER[1] |= ((ArrayBitSystem[0] >> 1) & 1) << 7;  // YMbMaqProduzindo = $rung_top

    /* ] finish series */

    /* start rung 195 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $rung_top
    if (((ArrayBitSystem[2] >> 25) & 1)) {  // $oneShot_004a
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[2] |= 1UL << 25; else ArrayBitSystem[2] &= ~(1UL << 25); // $oneShot_004a = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[10] = 1000; // YMbPerfFatorL
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[11] = 0; // YMbPerfFatorH
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[31] = 0; // YMbTam
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[14] = 100; // YMbAutoVel
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[12] = 200; // YMbAutoAcel
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[13] = 150; // YMbAutoDesac
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[17] = 30; // YMbManVel
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[15] = 400; // YMbManAcel
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[16] = 100; // YMbManDesac
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[25] = 5; // YMbFacaTam
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[17] = 1; // WInvModoAuto
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[21] = 0; // WInvModoMan
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[20] = 1000; // YMbEncFator
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[29] = 0; // QtdPecasFila
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[24] = 450000; // DistPrensaCorte
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[71] = 0; // PecaTam
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[70] = ArrayIntUser[71]; // DistanciaPrensa = PecaTam
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[80] = 1000; // YMbSrvFator
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[9] = 0; // ValResetEnc
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[21] = 2500; // YMbEncResol
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[22] = 400; // YMbEncPerim
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 198 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 65536; // $scratch2
        ArrayIntUser[81] = MODBUS_REGISTER[11] * ArrayIntUser[3]; // WMbPerfFator = YMbPerfFatorH * $scratch2
    }

    /* ] finish series */

    /* start rung 199 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[81] = ArrayIntUser[81] + MODBUS_REGISTER[10]; // WMbPerfFator = WMbPerfFator + YMbPerfFatorL
    }

    /* ] finish series */

    /* start rung 201 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[3] = 0; // $scratch2
    if (ArrayIntUser[3] > ArrayIntUser[76]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = -1; // $scratch2
        ArrayIntUser[82] = ArrayIntUser[76] * ArrayIntUser[3]; // Posicao = DifPosic * $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 31; // SentidoAvancar = 1
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 202 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[3] = 0; // $scratch2
    if (ArrayIntUser[3] > ArrayIntUser[76]) {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[82] = ArrayIntUser[76]; // Posicao = DifPosic
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitUser[0] &= ~(1UL << 31); // SentidoAvancar = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 203 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[83] = ArrayIntUser[82] * ArrayIntUser[80]; // WInvTam = Posicao * YMbSrvFator
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 1000; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[83] = ArrayIntUser[83] / ArrayIntUser[3]; // WInvTam = WInvTam / $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 204 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[84] = ArrayIntUser[83]; // WInvTamIntegral = WInvTam
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 100; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[83] = ArrayIntUser[83] / ArrayIntUser[3]; // WInvTam = WInvTam / $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 205 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 100; // $scratch2
        ArrayIntUser[85] = ArrayIntUser[83] * ArrayIntUser[3]; // WInvTamCent = WInvTam * $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[85] = ArrayIntUser[84] - ArrayIntUser[85]; // WInvTamCent = WInvTamIntegral - WInvTamCent
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 206 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[83] = ArrayIntUser[83] * ArrayIntUser[81]; // WInvTam = WInvTam * WMbPerfFator
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 1000; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[83] = ArrayIntUser[83] / ArrayIntUser[3]; // WInvTam = WInvTam / $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 207 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[85] = ArrayIntUser[85] * ArrayIntUser[81]; // WInvTamCent = WInvTamCent * WMbPerfFator
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 100000; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[85] = ArrayIntUser[85] / ArrayIntUser[3]; // WInvTamCent = WInvTamCent / $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 208 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[83] = ArrayIntUser[83] + ArrayIntUser[85]; // WInvTam = WInvTam + WInvTamCent
    }

    /* ] finish series */

    /* start rung 209 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 65536; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[7] = ArrayIntUser[83] / ArrayIntUser[3]; // DeslocamentoH = WInvTam / $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[3] = 65536; // $scratch2
        ArrayIntUser[8] = ArrayIntUser[7] * ArrayIntUser[3]; // DeslocamentoL = DeslocamentoH * $scratch2
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[8] = ArrayIntUser[83] - ArrayIntUser[8]; // DeslocamentoL = WInvTam - DeslocamentoL
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 211 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[86] = MODBUS_REGISTER[31] + MODBUS_REGISTER[25]; // NovoPecaTam = YMbTam + YMbFacaTam
    }

    /* ] finish series */

    /* start rung 212 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 100; // $scratch2
        ArrayIntUser[86] = ArrayIntUser[86] * ArrayIntUser[3]; // NovoPecaTam = NovoPecaTam * $scratch2
    }

    /* ] finish series */

    /* start rung 213 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (ArrayIntUser[86] == ArrayIntUser[71]) {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[87] = ArrayIntUser[71] - ArrayIntUser[70]; // NovoDistancia = PecaTam - DistanciaPrensa
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[87] = ArrayIntUser[86] - ArrayIntUser[87]; // NovoDistancia = NovoPecaTam - NovoDistancia
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 0; // $scratch2
    if (ArrayIntUser[3] > ArrayIntUser[87]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[0] |= (1 << 7);  // ErroTamPeca = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (MODBUS_REGISTER[0] & (1 << 7)) {  // ErroTamPeca
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[70] = ArrayIntUser[87]; // DistanciaPrensa = NovoDistancia
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[71] = ArrayIntUser[86]; // PecaTam = NovoPecaTam
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitUser[1] |= 1UL << 12; // PecaTamAlterado = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 215 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 60; // $scratch2
        ArrayIntUser[18] = MODBUS_REGISTER[14] * ArrayIntUser[3]; // WInvAutoVel = YMbAutoVel * $scratch2
    }

    /* ] finish series */

    /* start rung 216 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 10; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[18] = ArrayIntUser[18] / ArrayIntUser[3]; // WInvAutoVel = WInvAutoVel / $scratch2
    }

    /* ] finish series */

    /* start rung 217 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 60; // $scratch2
        ArrayIntUser[22] = MODBUS_REGISTER[17] * ArrayIntUser[3]; // WInvManVel = YMbManVel * $scratch2
    }

    /* ] finish series */

    /* start rung 218 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[3] = 10; // $scratch2
        if(ArrayIntUser[3]) ArrayIntUser[22] = ArrayIntUser[22] / ArrayIntUser[3]; // WInvManVel = WInvManVel / $scratch2
    }

    /* ] finish series */

    /* start rung 220 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[19] = MODBUS_REGISTER[12]; // WInvAutoAcel = YMbAutoAcel
    }

    /* ] finish series */

    /* start rung 221 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[88] = MODBUS_REGISTER[13]; // WInvAutoDesac = YMbAutoDesac
    }

    /* ] finish series */

    /* start rung 222 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[23] = MODBUS_REGISTER[15]; // WInvManAcel = YMbManAcel
    }

    /* ] finish series */

    /* start rung 223 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[89] = MODBUS_REGISTER[16]; // WInvManDesac = YMbManDesac
    }

    /* ] finish series */

    /* start rung 225 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[5] = ArrayIntUser[25]; // MbInvTensao = WInvTensao
    }

    /* ] finish series */

    /* start rung 226 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[6] = ArrayIntUser[26]; // MbInvCorrente = WInvCorrente
    }

    /* ] finish series */

    /* start rung 227 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[7] = 0; // MbInvTorque
    }

    /* ] finish series */

    /* start rung 228 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[8] = ArrayIntUser[27]; // MbInvDigIn = WInvDigIn
    }

    /* ] finish series */

    /* start rung 229 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[9] = ArrayIntUser[28]; // MbInvDigOut = WInvDigOut
    }

    /* ] finish series */

    /* start rung 231 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 9))) {  // MbTamanhoLista
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 9);  // MbTamanhoLista = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayIntUser[29] = MODBUS_REGISTER[18]; // QtdPecasFila = MbIndiceLista
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 233 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 10))) {  // MbLerPeca
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 10);  // MbLerPeca = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[19] = 0; // MbPecaLista
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (ArrayIntUser[29] > MODBUS_REGISTER[18]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 0; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[69]; // MbPecaLista = PecasFila0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 1; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[68]; // MbPecaLista = PecasFila1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 2; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[67]; // MbPecaLista = PecasFila2
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 3; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[66]; // MbPecaLista = PecasFila3
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 4; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[65]; // MbPecaLista = PecasFila4
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 5; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[64]; // MbPecaLista = PecasFila5
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 6; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[63]; // MbPecaLista = PecasFila6
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 7; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[62]; // MbPecaLista = PecasFila7
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 8; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[61]; // MbPecaLista = PecasFila8
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 9; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[60]; // MbPecaLista = PecasFila9
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 10; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[59]; // MbPecaLista = PecasFila10
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 11; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[58]; // MbPecaLista = PecasFila11
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 12; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[57]; // MbPecaLista = PecasFila12
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 13; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[56]; // MbPecaLista = PecasFila13
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 14; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[55]; // MbPecaLista = PecasFila14
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 15; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[54]; // MbPecaLista = PecasFila15
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 16; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[53]; // MbPecaLista = PecasFila16
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 17; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[52]; // MbPecaLista = PecasFila17
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 18; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[51]; // MbPecaLista = PecasFila18
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 19; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[50]; // MbPecaLista = PecasFila19
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 20; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[49]; // MbPecaLista = PecasFila20
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 21; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[48]; // MbPecaLista = PecasFila21
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 22; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[47]; // MbPecaLista = PecasFila22
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 23; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[46]; // MbPecaLista = PecasFila23
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 24; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[45]; // MbPecaLista = PecasFila24
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 25; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[44]; // MbPecaLista = PecasFila25
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 26; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[43]; // MbPecaLista = PecasFila26
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 27; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[42]; // MbPecaLista = PecasFila27
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 28; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[41]; // MbPecaLista = PecasFila28
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 29; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[40]; // MbPecaLista = PecasFila29
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 30; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[39]; // MbPecaLista = PecasFila30
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 31; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[38]; // MbPecaLista = PecasFila31
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 32; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[37]; // MbPecaLista = PecasFila32
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 33; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[36]; // MbPecaLista = PecasFila33
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 34; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[35]; // MbPecaLista = PecasFila34
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 35; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[34]; // MbPecaLista = PecasFila35
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 36; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[33]; // MbPecaLista = PecasFila36
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 37; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[32]; // MbPecaLista = PecasFila37
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 38; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[31]; // MbPecaLista = PecasFila38
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 39; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[19] = ArrayIntUser[30]; // MbPecaLista = PecasFila39
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[18] = ArrayIntUser[29]; // MbIndiceLista = QtdPecasFila
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 235 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[2] & (1 << 11))) {  // MbGravarPeca
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (ArrayIntUser[29] > MODBUS_REGISTER[18]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 0; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[69] = MODBUS_REGISTER[19]; // PecasFila0 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 1; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[68] = MODBUS_REGISTER[19]; // PecasFila1 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 2; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[67] = MODBUS_REGISTER[19]; // PecasFila2 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 3; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[66] = MODBUS_REGISTER[19]; // PecasFila3 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 4; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[65] = MODBUS_REGISTER[19]; // PecasFila4 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 5; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[64] = MODBUS_REGISTER[19]; // PecasFila5 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 6; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[63] = MODBUS_REGISTER[19]; // PecasFila6 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 7; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[62] = MODBUS_REGISTER[19]; // PecasFila7 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 8; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[61] = MODBUS_REGISTER[19]; // PecasFila8 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 9; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[60] = MODBUS_REGISTER[19]; // PecasFila9 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 10; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[59] = MODBUS_REGISTER[19]; // PecasFila10 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 11; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[58] = MODBUS_REGISTER[19]; // PecasFila11 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 12; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[57] = MODBUS_REGISTER[19]; // PecasFila12 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 13; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[56] = MODBUS_REGISTER[19]; // PecasFila13 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 14; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[55] = MODBUS_REGISTER[19]; // PecasFila14 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 15; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[54] = MODBUS_REGISTER[19]; // PecasFila15 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 16; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[53] = MODBUS_REGISTER[19]; // PecasFila16 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 17; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[52] = MODBUS_REGISTER[19]; // PecasFila17 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 18; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[51] = MODBUS_REGISTER[19]; // PecasFila18 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 19; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[50] = MODBUS_REGISTER[19]; // PecasFila19 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 20; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[49] = MODBUS_REGISTER[19]; // PecasFila20 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 21; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[48] = MODBUS_REGISTER[19]; // PecasFila21 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 22; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[47] = MODBUS_REGISTER[19]; // PecasFila22 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 23; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[46] = MODBUS_REGISTER[19]; // PecasFila23 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 24; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[45] = MODBUS_REGISTER[19]; // PecasFila24 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 25; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[44] = MODBUS_REGISTER[19]; // PecasFila25 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 26; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[43] = MODBUS_REGISTER[19]; // PecasFila26 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 27; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[42] = MODBUS_REGISTER[19]; // PecasFila27 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 28; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[41] = MODBUS_REGISTER[19]; // PecasFila28 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 29; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[40] = MODBUS_REGISTER[19]; // PecasFila29 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 30; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[39] = MODBUS_REGISTER[19]; // PecasFila30 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 31; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[38] = MODBUS_REGISTER[19]; // PecasFila31 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 32; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[37] = MODBUS_REGISTER[19]; // PecasFila32 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 33; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[36] = MODBUS_REGISTER[19]; // PecasFila33 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 34; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[35] = MODBUS_REGISTER[19]; // PecasFila34 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 35; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[34] = MODBUS_REGISTER[19]; // PecasFila35 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 36; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[33] = MODBUS_REGISTER[19]; // PecasFila36 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 37; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[32] = MODBUS_REGISTER[19]; // PecasFila37 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 38; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[31] = MODBUS_REGISTER[19]; // PecasFila38 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = $parThis_0000
    /* iniciando serie [ */
    ArrayIntUser[3] = 39; // $scratch2
    if (MODBUS_REGISTER[18] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayIntUser[30] = MODBUS_REGISTER[19]; // PecasFila39 = MbPecaLista
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[18] = ArrayIntUser[29]; // MbIndiceLista = QtdPecasFila
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] &= ~(1 << 11);  // MbGravarPeca = 0
    }

    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */
}

void PLC_Init(void)
{
	I_SerialReady = 1;
	MODBUS_RS485_MASTER = 1;
	MODBUS_TCP_MASTER = 0;
	ModBUS_SetID(0);

	RS485_Config(9600, 8, 2, 1);

	IP4_ADDR(&IP_ADDRESS, 192,168,2,243);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,2,1);
	IP4_ADDR(&IP_DNS, 192,168,0,10);

    QEIConfig.CaptureMode = QEI_CAPMODE_4X;
    ADC_SetMask(32);
    SSI_Init(24, 0);

    memset((void*)ArrayBitUser, 0, sizeof(ArrayBitUser));
    memset((void*)ArrayBitSystem, 0, sizeof(ArrayBitSystem));
    memset((void*)ArrayIntUser, 0, sizeof(ArrayIntUser));
}
