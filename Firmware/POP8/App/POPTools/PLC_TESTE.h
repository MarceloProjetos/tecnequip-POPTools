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
volatile int SavedE2P = 0;
volatile int ArrayBitUser_Count = 0;
volatile int ArrayBitUser[1];
volatile int ArrayBitSystem_Count = 1;
volatile int ArrayBitSystem[1];
volatile int ArrayIntUser_Count = 3;
volatile int ArrayIntUser[3];

// Funcao que executa um ciclo de processamento da logica criada pelo usuario
void PLC_Run(void)
{
    Cycle_Init();

    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* iniciando linha 2 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (GPIO_OUTPUT_PORT1) {  // pisca
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[0] < 49) {
            ArrayIntUser[0]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[0] = 0; // seq0
    }

    if (!((ArrayBitSystem[0] >> 2) & 1)) {  // $seq1_antiglitch
        ArrayIntUser[1] = 49; // seq1
    }
    ArrayBitSystem[0] |= 1UL << 2; // $seq1_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[1] < 49) {
            ArrayIntUser[1]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[1] = 0; // seq1
    }

    GPIO_OUTPUT_PORT1 = ((ArrayBitSystem[0] >> 1) & 1); // pisca = $rung_top

    /* terminando serie [ */

    /* iniciando linha 4 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // Gravar
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 3); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0
        MODBUS_REGISTER[0] &= ~(1 << 0);  // Gravar = 0
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0
        ArrayIntUser[2] = MODBUS_REGISTER[2]; // DadoSalvo = MbNovoDado
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0
        MODBUS_REGISTER[2] = 0; // MbNovoDado
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0
        static int SavedE2P = 0;
        if (!((ArrayBitSystem[0] >> 5) & 1)) {  // $oneShot_0
            ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = 0
            if(E2P_Busy()) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6);
            if (!((ArrayBitSystem[0] >> 6) & 1)) {  // $scratch
                if(E2P_Read((void *)&ArrayIntUser[2], 0, 4)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5);
                SavedE2P = ArrayIntUser[2]; // $S_SavedE2P = DadoSalvo
            }
        } else {
            if (SavedE2P == ArrayIntUser[2]) {
            } else {
                ArrayBitSystem[0] &= ~(1UL << 6); // $scratch = 0
                if(E2P_Busy()) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6);
                if (!((ArrayBitSystem[0] >> 6) & 1)) {  // $scratch
                    E2P_Write((void *)&ArrayIntUser[2], 0, 4);
                }
            }
        }
    }

    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 3; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 6 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        MODBUS_REGISTER[1] = ArrayIntUser[2]; // MbDado = DadoSalvo
    }

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

	IP4_ADDR(&IP_ADDRESS, 192,168,0,240);
	IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	IP4_ADDR(&IP_GATEWAY, 192,168,0,1);
	IP4_ADDR(&IP_DNS, 192,168,0,1);

    memset((void*)ArrayBitSystem, 0, sizeof(ArrayBitSystem));
    memset((void*)ArrayIntUser, 0, sizeof(ArrayIntUser));
}

// Funcao que contem os passos adicionais para inicializacao de um ciclo de execucao, quando necessario
// Ex.: Atualizacao da leitura dos canais A/D (Quando em uso)
void Cycle_Init(void)
{
}
