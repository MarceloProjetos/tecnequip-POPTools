/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "ld.h"

// Prototipo para a funcao que inicializa um ciclo de execucao
void Cycle_Init(void);

// Flag que indica se o ModBUS na interface RS-485 funcionara como master (true) ou slave (false)
extern volatile unsigned char 	MODBUS_RS485_MASTER; // 0 = Slave, 1 = Master
// Flag que indica se o ModBUS na interface Ethernet funcionara como master (true) ou slave (false)
extern volatile unsigned char 	MODBUS_TCP_MASTER; // 0 = Slave, 1 = Master
// Vetor com os registradores acessiveis pelo ModBUS (Holding Registers)
extern volatile int 			MODBUS_REGISTER[32];
// Ultima hora lida do RTC, atualizada ao iniciar um ciclo de execucao
extern struct tm 				RTC_NowTM;
// As Variaveis a seguir armazenam a data/hora inicial e final (respectivamente) de um elemento de
// agendamento de tarefa, compartilhado com todos os elementos para economizar memoria.
struct tm 						RTC_StartTM;
struct tm 						RTC_EndTM;

extern volatile unsigned int 	I_LCDReady;
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

struct strExpansionBoard expansionBoards[] = {
    { eBoardType_Output, 33, { { 0 } }, 0, 0 },
    { eBoardType_None, 0, { { 0 } }, 0, 0 }
};

// Variaveis PLC
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile int ArrayBitUser_Count = 1;
volatile int ArrayBitUser[1];
volatile int ArrayBitSystem_Count = 1;
volatile int ArrayBitSystem[1];
volatile int ArrayIntUser_Count = 10;
volatile int ArrayIntUser[10];

// Funcao que executa um ciclo de processamento da logica criada pelo usuario
void PLC_Run(void)
{
    Cycle_Init();

    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* iniciando linha 2 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 2; else ArrayBitSystem[0] &= ~(1UL << 2); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $oneShot_0
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $oneShot_0 = $scratch

    ArrayIntUser[0] = 0; // $scratch_int
    ArrayIntUser[1] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $oneShot_1
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_Clear();
                ArrayBitSystem[0] |= 1UL << 4; // $oneShot_1 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $oneShot_1
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $oneShot_1 = 0
    }

    ArrayIntUser[0] = 1; // $scratch_int
    ArrayIntUser[1] = 2; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 5) & 1)) {  // $oneShot_2
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[1]); XP_lcd_WriteText("*** POPTools ***");
                ArrayBitSystem[0] |= 1UL << 5; // $oneShot_2 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $oneShot_2
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $oneShot_2 = 0
    }

    /* terminando serie [ */

    /* iniciando linha 3 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 0) & 1)) {  // piscaBL
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[3] < 99) {
            ArrayIntUser[3]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[3] = 0; // seq2
    }

    if (!((ArrayBitSystem[0] >> 6) & 1)) {  // $seq3_antiglitch
        ArrayIntUser[4] = 99; // seq3
    }
    ArrayBitSystem[0] |= 1UL << 6; // $seq3_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[4] < 99) {
            ArrayIntUser[4]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[4] = 0; // seq3
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 0; else ArrayBitUser[0] &= ~(1UL << 0); // piscaBL = $rung_top

    /* terminando serie [ */

    /* iniciando linha 4 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // piscaBL
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 7); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    if(((ArrayBitSystem[0] >> 8) & 1)) ArrayBitSystem[0] |= 1UL << 2; else ArrayBitSystem[0] &= ~(1UL << 2); // $scratch = $parThis_0
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $oneShot_3
        ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = 0
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $oneShot_3 = $scratch

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    if(((ArrayBitSystem[0] >> 8) & 1)) ArrayBitSystem[0] |= 1UL << 2; else ArrayBitSystem[0] &= ~(1UL << 2); // $scratch = $parThis_0
    if (!((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        if (((ArrayBitSystem[0] >> 10) & 1)) {  // $oneShot_4
            ArrayBitSystem[0] |= 1UL << 8; // $parThis_0 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = 0
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $oneShot_4 = $scratch

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 11); // $parOut_1 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $rung_top
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayIntUser[5] = 0; // BL
    }

    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // piscaBL
        ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = 0
    }

    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayIntUser[5] = 1; // BL
    }

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $rung_top
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        if (!((ArrayBitSystem[0] >> 13) & 1)) {  // $oneShot_5
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_setBL(ArrayIntUser[5]);
                ArrayBitSystem[0] |= 1UL << 13; // $oneShot_5 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 13) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $oneShot_5
    } else {
        ArrayBitSystem[0] &= ~(1UL << 13); // $oneShot_5 = 0
    }

    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 11) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_1
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 6 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (expansionBoards[0].value.bits.bit0) {  // XP_S1
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[8] < 49) {
            ArrayIntUser[8]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[8] = 0; // seq0
    }

    if (!((ArrayBitSystem[0] >> 14) & 1)) {  // $seq1_antiglitch
        ArrayIntUser[9] = 49; // seq1
    }
    ArrayBitSystem[0] |= 1UL << 14; // $seq1_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[9] < 49) {
            ArrayIntUser[9]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[9] = 0; // seq1
    }

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 7); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    expansionBoards[0].value.bits.bit0 = ((ArrayBitSystem[0] >> 8) & 1); // XP_S1 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    expansionBoards[0].value.bits.bit1 = ((ArrayBitSystem[0] >> 8) & 1); // XP_S2 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    expansionBoards[0].value.bits.bit2 = ((ArrayBitSystem[0] >> 8) & 1); // XP_S3 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    expansionBoards[0].value.bits.bit3 = ((ArrayBitSystem[0] >> 8) & 1); // XP_S4 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    expansionBoards[0].value.bits.bit4 = ((ArrayBitSystem[0] >> 8) & 1); // XP_S5 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    expansionBoards[0].value.bits.bit5 = ((ArrayBitSystem[0] >> 8) & 1); // XP_S6 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    expansionBoards[0].value.bits.bit6 = ((ArrayBitSystem[0] >> 8) & 1); // XP_S7 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    expansionBoards[0].value.bits.bit7 = ((ArrayBitSystem[0] >> 8) & 1); // XP_S8 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $parThis_0 = $rung_top
    GPIO_OUTPUT_PORT1 = ((ArrayBitSystem[0] >> 8) & 1); // x2 = $parThis_0

    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 7; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */
}

// Funcao que contem os passos adicionais para inicializacao da Ethernet, quando necessario
// Ex.: Configuracao de servidores utilizados para sincronismo de data/hora via SNTP (Quando em uso)
void PLC_Net_Init(void)
{
}

// Funcao que inicializa o controlador, executado apenas ao ligar o equipamento
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

    XP_lcd_Init(0);

    memset((void*)ArrayBitUser, 0, sizeof(ArrayBitUser));
    memset((void*)ArrayBitSystem, 0, sizeof(ArrayBitSystem));
    memset((void*)ArrayIntUser, 0, sizeof(ArrayIntUser));
}

// Funcao que contem os passos adicionais para inicializacao de um ciclo de execucao, quando necessario
// Ex.: Atualizacao da leitura dos canais A/D (Quando em uso)
void Cycle_Init(void)
{
}
