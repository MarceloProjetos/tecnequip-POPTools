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
    { eBoardType_None, 0, { { 0 } }, 0, 0 }
};

// Variaveis PLC
volatile int ArrayBitUser_Count = 1;
volatile int ArrayBitUser[1];
volatile int ArrayBitSystem_Count = 1;
volatile int ArrayBitSystem[1];
volatile int ArrayIntUser_Count = 6;
volatile int ArrayIntUser[6];

// Funcao que executa um ciclo de processamento da logica criada pelo usuario
void PLC_Run(void)
{
    Cycle_Init();

    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* iniciando linha 2 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[0] < 9) {
            ArrayIntUser[0]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[0] = 0; // seq0
    }

    if (!((ArrayBitSystem[0] >> 2) & 1)) {  // $seq1_antiglitch
        ArrayIntUser[1] = 9; // seq1
    }
    ArrayBitSystem[0] |= 1UL << 2; // $seq1_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[1] < 9) {
            ArrayIntUser[1]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[1] = 0; // seq1
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 0; else ArrayBitUser[0] &= ~(1UL << 0); // Loop = $rung_top

    /* terminando serie [ */

    /* iniciando linha 4 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $oneShot_0
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $oneShot_0 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[2] = ArrayIntUser[2] + ArrayIntUser[3]; // X = X + offset
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[4] = 12; // $scratch2_int
    if (ArrayIntUser[2] == ArrayIntUser[4]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[3] = -1; // offset
    }

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[4] = 0; // $scratch2_int
    if (ArrayIntUser[2] == ArrayIntUser[4]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[3] = 1; // offset
    }

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 6 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 7) & 1)) {  // $oneShot_1
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $oneShot_1 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[4] = 0; // $scratch2_int
    if (ArrayIntUser[2] > ArrayIntUser[4]) { // X > $scratch2_int
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 8); // $parOut_1 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_1 = $parThis_0
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_1
        ArrayIntUser[4] = 1; // $scratch2_int
        ArrayIntUser[5] = ArrayIntUser[2] - ArrayIntUser[4]; // X2 = X - $scratch2_int
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_1 = $parThis_0
    ArrayIntUser[4] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_1
        if (!((ArrayBitSystem[0] >> 10) & 1)) {  // $oneShot_2
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[4],ArrayIntUser[5]); XP_lcd_WriteText(" ", &ArrayIntUser[2]);
                ArrayBitSystem[0] |= 1UL << 10; // $oneShot_2 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 10) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_1 = $oneShot_2
    } else {
        ArrayBitSystem[0] &= ~(1UL << 10); // $oneShot_2 = 0
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 8) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $parOut_1
    /* terminando paralelo [ */
    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    ArrayIntUser[4] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 11) & 1)) {  // $oneShot_3
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[4],ArrayIntUser[2]); XP_lcd_WriteText("POPTools ", &ArrayIntUser[2]);
                ArrayBitSystem[0] |= 1UL << 11; // $oneShot_3 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 11) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $oneShot_3
    } else {
        ArrayBitSystem[0] &= ~(1UL << 11); // $oneShot_3 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
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

    XP_lcd_Init(39, 0);

    memset((void*)ArrayBitUser, 0, sizeof(ArrayBitUser));
    memset((void*)ArrayBitSystem, 0, sizeof(ArrayBitSystem));
    memset((void*)ArrayIntUser, 0, sizeof(ArrayIntUser));
}

// Funcao que contem os passos adicionais para inicializacao de um ciclo de execucao, quando necessario
// Ex.: Atualizacao da leitura dos canais A/D (Quando em uso)
void Cycle_Init(void)
{
}
