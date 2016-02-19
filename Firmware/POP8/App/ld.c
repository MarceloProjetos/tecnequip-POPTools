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
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile int ArrayBitUser_Count = 1;
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
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[0] < 299) {
            ArrayIntUser[0]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[0] = 0; // seq7
    }

    if (((ArrayBitUser[0] >> 0) & 1)) {  // PiscaOK
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (GPIO_OUTPUT_PORT2) {  // EsperaPisca
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 2); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 4) & 1)) {  // $oneShot_0
            if (I_SerialReady) {  // $SerialReady
                CAN_Write(SendMsgBuf);
                ArrayBitSystem[0] |= 1UL << 4; // $oneShot_0 = 1
            }
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = 0
            ArrayBitSystem[0] &= ~(1UL << 5); // $oneShot_1 = 0
        }
        if (!((ArrayBitSystem[0] >> 5) & 1)) {  // $oneShot_1
            ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = 0
            if (I_SerialReady) {  // $SerialReady
                ArrayBitSystem[0] |= 1UL << 5; // $oneShot_1 = 1
            } else if (I_SerialTimeout) {  // $SerialTimeout
                I_SerialTimeout = 0; // $SerialTimeout = 0
                CAN_Write(SendMsgBuf);
            }
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 4); // $oneShot_0 = 0
        ArrayBitSystem[0] &= ~(1UL << 5); // $oneShot_1 = 0
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitUser[0] |= 1UL << 0; else ArrayBitUser[0] &= ~(1UL << 0); // PiscaOK = $parThis_0

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    if (!I_SerialAborted) {  // $SerialAborted
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = 0
    }

    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = $parThis_0
    if (((ArrayBitSystem[0] >> 7) & 1)) {  // $oneShot_2
        ArrayBitSystem[0] &= ~(1UL << 3); // $parThis_0 = 0
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $oneShot_2 = $scratch

    if (!((ArrayBitSystem[0] >> 8) & 1)) {  // $seq0_antiglitch
        ArrayIntUser[2] = 299; // seq0
    }
    ArrayBitSystem[0] |= 1UL << 8; // $seq0_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        if (ArrayIntUser[2] < 299) {
            ArrayIntUser[2]++;
            ArrayBitSystem[0] |= 1UL << 3; // $parThis_0 = 1
        }
    } else {
        ArrayIntUser[2] = 0; // seq0
    }

    GPIO_OUTPUT_PORT2 = ((ArrayBitSystem[0] >> 3) & 1); // EsperaPisca = $parThis_0

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 2; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 2 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[3] = 0; // $scratch2_int
    if (ArrayIntUser[4] > ArrayIntUser[3]) { // PiscaMB > $scratch2_int
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT1 = ((ArrayBitSystem[0] >> 1) & 1); // Pisca = $rung_top

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

	IP4_ADDR(&IP_ADDRESS, 192,168,1,222);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,1,1);
	IP4_ADDR(&IP_DNS, 192,168,1,1);

    memset((void*)ArrayBitUser, 0, sizeof(ArrayBitUser));
    memset((void*)ArrayBitSystem, 0, sizeof(ArrayBitSystem));
    memset((void*)ArrayIntUser, 0, sizeof(ArrayIntUser));
}

// Funcao que contem os passos adicionais para inicializacao de um ciclo de execucao, quando necessario
// Ex.: Atualizacao da leitura dos canais A/D (Quando em uso)
void Cycle_Init(void)
{
}
