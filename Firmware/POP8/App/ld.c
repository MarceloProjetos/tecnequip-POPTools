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

extern LPC_CAN_T LPC_CAN;
extern CAN_MSG_T SendMsgBuf;
extern CAN_MSG_T RcvMsgBuf;

struct strExpansionBoard expansionBoards[] = {
    { eBoardType_None, 0, { { 0 } }, 0, 0 }
};

// Variaveis PLC
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile int ArrayBitUser_Count = 0;
volatile int ArrayBitUser[1];
volatile int ArrayBitSystem_Count = 1;
volatile int ArrayBitSystem[1];
volatile int ArrayIntUser_Count = 5;
volatile int ArrayIntUser[5];

// Funcao que executa um ciclo de processamento da logica criada pelo usuario
void PLC_Run(void)
{
    Cycle_Init();

    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* iniciando linha 1 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 2; else ArrayBitSystem[0] &= ~(1UL << 2); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $oneShot_0
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $oneShot_0 = $scratch

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[0] = 9876; // v
    }

    /* terminando serie [ */

    /* iniciando linha 2 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_OUTPUT_PORT1) {  // e
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[1] < 9) {
            ArrayIntUser[1]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[1] = 0; // seq0
    }

    if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $seq1_antiglitch
        ArrayIntUser[2] = 9; // seq1
    }
    ArrayBitSystem[0] |= 1UL << 4; // $seq1_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[2] < 9) {
            ArrayIntUser[2]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[2] = 0; // seq1
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 5) & 1)) {  // $oneShot_1
            if (I_SerialReady) {  // $SerialReady
                CAN_Read(0, sizeof(ArrayIntUser[0]), &ArrayIntUser[0]);
                ArrayBitSystem[0] |= 1UL << 5; // $oneShot_1 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            ArrayBitSystem[0] &= ~(1UL << 6); // $oneShot_2 = 0
        }
        if (!((ArrayBitSystem[0] >> 6) & 1)) {  // $oneShot_2
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 6; // $oneShot_2 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                CAN_Read(0, sizeof(ArrayIntUser[0]), &ArrayIntUser[0]);
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $oneShot_1 = 0
        ArrayBitSystem[0] &= ~(1UL << 6); // $oneShot_2 = 0
    }

    ArrayIntUser[3] = 0; // $scratch_int
    ArrayIntUser[4] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 7) & 1)) {  // $oneShot_3
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[4],ArrayIntUser[3]); XP_lcd_WriteText("valor: %d", &ArrayIntUser[0]);
                ArrayBitSystem[0] |= 1UL << 7; // $oneShot_3 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 7) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $oneShot_3
    } else {
        ArrayBitSystem[0] &= ~(1UL << 7); // $oneShot_3 = 0
    }

    GPIO_OUTPUT_PORT1 = ((ArrayBitSystem[0] >> 1) & 1); // e = $rung_top

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
	MODBUS_TCP_MASTER = 0;
	ModBUS_SetID(0);
	ModBUS_SetAppName("POPTools");

	CAN_Init();
	CAN_Config(125);

	IP4_ADDR(&IP_ADDRESS, 192,168,0,254);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,1);
	IP4_ADDR(&IP_DNS, 192,168,0,1);

    XP_lcd_Init(39, 0);

    memset((void*)ArrayBitSystem, 0, sizeof(ArrayBitSystem));
    memset((void*)ArrayIntUser, 0, sizeof(ArrayIntUser));
}

// Funcao que contem os passos adicionais para inicializacao de um ciclo de execucao, quando necessario
// Ex.: Atualizacao da leitura dos canais A/D (Quando em uso)
void Cycle_Init(void)
{
}
