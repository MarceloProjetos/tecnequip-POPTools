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
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile int ArrayBitUser_Count = 0;
volatile int ArrayBitUser[0];
volatile int ArrayBitSystem_Count = 1;
volatile int ArrayBitSystem[1];
volatile int ArrayIntUser_Count = 12;
volatile int ArrayIntUser[12];

void PLC_Run(void)
{
    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* start rung 1 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_OUTPUT_PORT1) {  // S1
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[0] < 49) {
            ArrayIntUser[0]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[0] = 0; // ONS1
    }

    if (!((ArrayBitSystem[0] >> 2) & 1)) {  // $OFFS1_antiglitch
        ArrayIntUser[1] = 49; // OFFS1
    }
    ArrayBitSystem[0] |= 1UL << 2; // $OFFS1_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[1] < 49) {
            ArrayIntUser[1]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[1] = 0; // OFFS1
    }

    GPIO_OUTPUT_PORT1 = ((ArrayBitSystem[0] >> 1) & 1); // S1 = $rung_top

    /* ] finish series */

    /* start rung 2 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $oneShot_0000
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $oneShot_0000 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[0] = 321; // ValorMB
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[1] |= (1 << 0);  // EnviarMB = 1
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 3 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[2] = MODBUS_REGISTER[0]; // Valor = ValorMB
    }

    /* ] finish series */

    /* start rung 4 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[3] < 999) {
            ArrayIntUser[3]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[3] = 0; // EsperaON
    }

    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // EnviarMB
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT2) {  // ErroEscritaPOP1
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[0] >> 7) & 1)) {  // $oneShot_0001
            if (I_TcpReady) {  // $TcpReady
                Modbus_Send(0, 3232235773UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &ArrayIntUser[2]);
                ArrayBitSystem[0] |= 1UL << 7; // $oneShot_0001 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
            ArrayBitSystem[0] &= ~(1UL << 8); // $oneShot_0002 = 0
        }
        if (!((ArrayBitSystem[0] >> 8) & 1)) {  // $oneShot_0002
            if (I_TcpReady) {  // $TcpReady
                ArrayBitSystem[0] |= 1UL << 8; // $oneShot_0002 = 1
            } else if (I_TcpTimeout) {  // $TcpTimeout
                I_TcpTimeout = 0; // $TcpTimeout = 0
                Modbus_Send(0, 3232235773UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &ArrayIntUser[2]);
            } else {
                ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 7); // $oneShot_0001 = 0
        ArrayBitSystem[0] &= ~(1UL << 8); // $oneShot_0002 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        if (ArrayIntUser[4] < 9) {
            ArrayIntUser[4]++;
            ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[4] = 0; // EsperaEnviarMB
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[1] &= ~(1 << 0);  // EnviarMB = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        MODBUS_REGISTER[1] |= (1 << 1);  // AtivarSyncMB = 1
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!I_TcpTimeout) {  // $TcpTimeout
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        GPIO_OUTPUT_PORT2 = 1; // ErroEscritaPOP1 = 1
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 5 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // EnviarMB
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[1] & (1 << 1))) {  // AtivarSyncMB
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT3) {  // FimSyncPOP1
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 11) & 1)) {  // $oneShot_0003
            if (I_TcpReady) {  // $TcpReady
                Modbus_Send(0, 3232235773UL, MODBUS_FC_READ_HOLDING_REGISTERS, 0, 1, &ArrayIntUser[5]);
                ArrayBitSystem[0] |= 1UL << 11; // $oneShot_0003 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            ArrayBitSystem[0] &= ~(1UL << 12); // $oneShot_0004 = 0
        }
        if (!((ArrayBitSystem[0] >> 12) & 1)) {  // $oneShot_0004
            if (I_TcpReady) {  // $TcpReady
                ArrayBitSystem[0] |= 1UL << 12; // $oneShot_0004 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 11); // $oneShot_0003 = 0
        ArrayBitSystem[0] &= ~(1UL << 12); // $oneShot_0004 = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[6] < 9) {
            ArrayIntUser[6]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[6] = 0; // EsperaPOP1
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 13) & 1)) {  // $oneShot_0005
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 13; else ArrayBitSystem[0] &= ~(1UL << 13); // $oneShot_0005 = $scratch

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayIntUser[7] = 1; // $scratch2
        ArrayIntUser[5] = ArrayIntUser[5] + ArrayIntUser[7]; // SaidasPOP1 = SaidasPOP1 + $scratch2
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[0] >> 14) & 1)) {  // $oneShot_0006
            if (I_TcpReady) {  // $TcpReady
                Modbus_Send(1, 3232235773UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &ArrayIntUser[5]);
                ArrayBitSystem[0] |= 1UL << 14; // $oneShot_0006 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
            ArrayBitSystem[0] &= ~(1UL << 15); // $oneShot_0007 = 0
        }
        if (!((ArrayBitSystem[0] >> 15) & 1)) {  // $oneShot_0007
            if (I_TcpReady) {  // $TcpReady
                ArrayBitSystem[0] |= 1UL << 15; // $oneShot_0007 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 14); // $oneShot_0006 = 0
        ArrayBitSystem[0] &= ~(1UL << 15); // $oneShot_0007 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        if (ArrayIntUser[8] < 9) {
            ArrayIntUser[8]++;
            ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
        }
    } else {
        ArrayIntUser[8] = 0; // EsperaFimSync
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        GPIO_OUTPUT_PORT3 = 1; // FimSyncPOP1 = 1
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        ArrayIntUser[9] = 0; // SaidasPOP2
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 6 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr
    
    /* iniciando serie [ */
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // EnviarMB
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!(MODBUS_REGISTER[1] & (1 << 1))) {  // AtivarSyncMB
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (!GPIO_OUTPUT_PORT3) {  // FimSyncPOP1
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 16) & 1)) {  // $oneShot_0008
            if (I_TcpReady) {  // $TcpReady
                Modbus_Send(1, 3232235773UL, MODBUS_FC_READ_HOLDING_REGISTERS, 0, 1, &ArrayIntUser[9]);
                ArrayBitSystem[0] |= 1UL << 16; // $oneShot_0008 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            ArrayBitSystem[0] &= ~(1UL << 17); // $oneShot_0009 = 0
        }
        if (!((ArrayBitSystem[0] >> 17) & 1)) {  // $oneShot_0009
            if (I_TcpReady) {  // $TcpReady
                ArrayBitSystem[0] |= 1UL << 17; // $oneShot_0009 = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 16); // $oneShot_0008 = 0
        ArrayBitSystem[0] &= ~(1UL << 17); // $oneShot_0009 = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[10] < 9) {
            ArrayIntUser[10]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[10] = 0; // EsperaPOP2
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 18) & 1)) {  // $oneShot_000a
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 18; else ArrayBitSystem[0] &= ~(1UL << 18); // $oneShot_000a = $scratch

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayIntUser[7] = 1; // $scratch2
        ArrayIntUser[9] = ArrayIntUser[9] + ArrayIntUser[7]; // SaidasPOP2 = SaidasPOP2 + $scratch2
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        if (!((ArrayBitSystem[0] >> 19) & 1)) {  // $oneShot_000b
            if (I_TcpReady) {  // $TcpReady
                Modbus_Send(0, 3232235773UL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &ArrayIntUser[9]);
                ArrayBitSystem[0] |= 1UL << 19; // $oneShot_000b = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
            ArrayBitSystem[0] &= ~(1UL << 20); // $oneShot_000c = 0
        }
        if (!((ArrayBitSystem[0] >> 20) & 1)) {  // $oneShot_000c
            if (I_TcpReady) {  // $TcpReady
                ArrayBitSystem[0] |= 1UL << 20; // $oneShot_000c = 1
            } else {
                ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = 0
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 19); // $oneShot_000b = 0
        ArrayBitSystem[0] &= ~(1UL << 20); // $oneShot_000c = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 9); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        if (ArrayIntUser[11] < 99) {
            ArrayIntUser[11]++;
            ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0001 = 0
        }
    } else {
        ArrayIntUser[11] = 0; // EsperaSync
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0001 = $parThis_0000
    if (!(MODBUS_REGISTER[1] & (1 << 2))) {  // SemEsperaSync
        ArrayBitSystem[0] &= ~(1UL << 10); // $parThis_0001 = 0
    }

    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 9; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 9) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 21); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 22; else ArrayBitSystem[0] &= ~(1UL << 22); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 22) & 1)) {  // $parThis_0002
        GPIO_OUTPUT_PORT3 = 0; // FimSyncPOP1 = 0
    }

    if (((ArrayBitSystem[0] >> 22) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 21; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 22; else ArrayBitSystem[0] &= ~(1UL << 22); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 22) & 1)) {  // $parThis_0002
        ArrayIntUser[5] = 0; // SaidasPOP1
    }

    if (((ArrayBitSystem[0] >> 22) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 21; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 21) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 7 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[1] & (1 << 1))) {  // AtivarSyncMB
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (((ArrayBitSystem[0] >> 23) & 1)) {  // $oneShot_000d
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 23; else ArrayBitSystem[0] &= ~(1UL << 23); // $oneShot_000d = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        GPIO_OUTPUT_PORT3 = 0; // FimSyncPOP1 = 0
    }

    /* ] finish series */
}

void PLC_Init(void)
{
	I_SerialReady = 1;
	MODBUS_RS485_MASTER = 0;
	MODBUS_TCP_MASTER = 1;
	ModBUS_SetID(0);

	RS485_Config(9600, 8, 0, 1);

	IP4_ADDR(&IP_ADDRESS, 192,168,0,254);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,1);
	IP4_ADDR(&IP_DNS, 192,168,0,1);

    QEIConfig.CaptureMode = QEI_CAPMODE_4X;
    ADC_SetMask(0);
    SSI_Init(24, 0);
}
