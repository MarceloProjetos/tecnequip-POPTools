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
volatile int ArrayIntUser_Count = 14;
volatile int ArrayIntUser[14];

// Funcao que executa um ciclo de processamento da logica criada pelo usuario
void PLC_Run(void)
{
    Cycle_Init();

    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* iniciando linha 3 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[0] = 5; // $scratch2_int
    if (ArrayIntUser[1] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[2] < 9) {
            ArrayIntUser[2]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[2] = 0; // seq0
    }

    if (!((ArrayBitSystem[0] >> 2) & 1)) {  // $seq1_antiglitch
        ArrayIntUser[3] = 9; // seq1
    }
    ArrayBitSystem[0] |= 1UL << 2; // $seq1_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[3] < 9) {
            ArrayIntUser[3]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[3] = 0; // seq1
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 0; else ArrayBitUser[0] &= ~(1UL << 0); // Loop = $rung_top

    /* terminando serie [ */

    /* iniciando linha 5 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 4) & 1)) {  // $oneShot_0
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 4; else ArrayBitSystem[0] &= ~(1UL << 4); // $oneShot_0 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[4] = 1; // Offset_POPTools
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[5] = 1; // Offset_POP8
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[6] = 20; // X_MaisInfo
    }

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
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[7] = ArrayIntUser[7] + ArrayIntUser[4]; // X_POPTools = X_POPTools + Offset_POPTools
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 12; // $scratch2_int
    if (ArrayIntUser[7] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[4] = -1; // Offset_POPTools
    }

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 0; // $scratch2_int
    if (ArrayIntUser[7] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[4] = 1; // Offset_POPTools
    }

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 7 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 8) & 1)) {  // $oneShot_2
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 8; else ArrayBitSystem[0] &= ~(1UL << 8); // $oneShot_2 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[8] = ArrayIntUser[8] + ArrayIntUser[5]; // X_POP8 = X_POP8 + Offset_POP8
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 15; // $scratch2_int
    if (ArrayIntUser[8] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[5] = -1; // Offset_POP8
    }

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 0; // $scratch2_int
    if (ArrayIntUser[8] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[5] = 1; // Offset_POP8
    }

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 8 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $oneShot_3
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $oneShot_3 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[0] = 1; // $scratch2_int
        ArrayIntUser[6] = ArrayIntUser[6] - ArrayIntUser[0]; // X_MaisInfo = X_MaisInfo - $scratch2_int
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = -40; // $scratch2_int
    if (ArrayIntUser[6] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[6] = 20; // X_MaisInfo
    }

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 10 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 10) & 1)) {  // $oneShot_4
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $oneShot_4 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 0; // $scratch2_int
    if (ArrayIntUser[7] > ArrayIntUser[0]) { // X_POPTools > $scratch2_int
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 11); // $parOut_1 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $parThis_0
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayIntUser[0] = 1; // $scratch2_int
        ArrayIntUser[9] = ArrayIntUser[7] - ArrayIntUser[0]; // X2 = X_POPTools - $scratch2_int
    }

    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $parThis_0
    /* iniciando serie [ */
    ArrayIntUser[0] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        if (!((ArrayBitSystem[0] >> 13) & 1)) {  // $oneShot_5
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[9]); XP_lcd_WriteText(" ", &ArrayIntUser[7]);
                ArrayBitSystem[0] |= 1UL << 13; // $oneShot_5 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 13) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $oneShot_5
    } else {
        ArrayBitSystem[0] &= ~(1UL << 13); // $oneShot_5 = 0
    }

    if(((ArrayBitSystem[0] >> 12) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // dummy = $parThis_1

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 11) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $parOut_1
    /* terminando paralelo [ */
    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 14) & 1)) {  // $oneShot_6
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[7]); XP_lcd_WriteText("POPTools ", &ArrayIntUser[7]);
                ArrayBitSystem[0] |= 1UL << 14; // $oneShot_6 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 14) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $oneShot_6
    } else {
        ArrayBitSystem[0] &= ~(1UL << 14); // $oneShot_6 = 0
    }

    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // dummy = $parThis_0

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 11 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 15) & 1)) {  // $oneShot_7
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 15; else ArrayBitSystem[0] &= ~(1UL << 15); // $oneShot_7 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 0; // $scratch2_int
    if (ArrayIntUser[8] > ArrayIntUser[0]) { // X_POP8 > $scratch2_int
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 11); // $parOut_1 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $parThis_0
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayIntUser[0] = 1; // $scratch2_int
        ArrayIntUser[9] = ArrayIntUser[8] - ArrayIntUser[0]; // X2 = X_POP8 - $scratch2_int
    }

    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $parThis_0
    /* iniciando serie [ */
    ArrayIntUser[0] = 2; // $scratch2_int
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        if (!((ArrayBitSystem[0] >> 16) & 1)) {  // $oneShot_8
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[9]); XP_lcd_WriteText(" ", &ArrayIntUser[7]);
                ArrayBitSystem[0] |= 1UL << 16; // $oneShot_8 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 16) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $oneShot_8
    } else {
        ArrayBitSystem[0] &= ~(1UL << 16); // $oneShot_8 = 0
    }

    if(((ArrayBitSystem[0] >> 12) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // dummy = $parThis_1

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 11) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $parOut_1
    /* terminando paralelo [ */
    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 2; // $scratch2_int
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 17) & 1)) {  // $oneShot_9
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[8]); XP_lcd_WriteText("POP-8 ", &ArrayIntUser[8]);
                ArrayBitSystem[0] |= 1UL << 17; // $oneShot_9 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 17) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $oneShot_9
    } else {
        ArrayBitSystem[0] &= ~(1UL << 17); // $oneShot_9 = 0
    }

    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // dummy = $parThis_0

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 12 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 18) & 1)) {  // $oneShot_10
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 18; else ArrayBitSystem[0] &= ~(1UL << 18); // $oneShot_10 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 20; // $scratch2_int
    if (ArrayIntUser[6] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }

    ArrayIntUser[0] = 2; // $scratch2_int
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 19) & 1)) {  // $oneShot_11
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[9]); XP_lcd_WriteText("                    ", &ArrayIntUser[7]);
                ArrayBitSystem[0] |= 1UL << 19; // $oneShot_11 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 19) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $oneShot_11
    } else {
        ArrayBitSystem[0] &= ~(1UL << 19); // $oneShot_11 = 0
    }

    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // dummy = $parThis_0

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 2; // $scratch2_int
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 20) & 1)) {  // $oneShot_12
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[6]); XP_lcd_WriteText("Mais informacoes: www.tecnequip.com.br ", &ArrayIntUser[6]);
                ArrayBitSystem[0] |= 1UL << 20; // $oneShot_12 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 20) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $oneShot_12
    } else {
        ArrayBitSystem[0] &= ~(1UL << 20); // $oneShot_12 = 0
    }

    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // dummy = $parThis_0

    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 14 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // Loop
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayIntUser[0] = 100; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 21) & 1)) {  // $oneShot_13
            ArrayIntUser[10]++;
        }
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 21; else ArrayBitSystem[0] &= ~(1UL << 21); // $oneShot_13 = $rung_top
    if (ArrayIntUser[0] > ArrayIntUser[10]) { // $scratch2_int > EsperaInformacao
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    } else {
        ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 22) & 1)) {  // $oneShot_14
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 22; else ArrayBitSystem[0] &= ~(1UL << 22); // $oneShot_14 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[1] = 0; // Etapa
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayIntUser[11] = 0; // EsperaAnimacao
    }

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 16 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[0] = 4; // $scratch2_int
    if (ArrayIntUser[1] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitUser[0] >> 2) & 1)) {  // Espera
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayIntUser[12] = 0; // $scratch_int
    ArrayIntUser[0] = 3; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 23) & 1)) {  // $oneShot_15
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[12]); XP_lcd_WriteText("%hd/%hm/%ha %hh:%hM:%hs", &MODBUS_REGISTER[0]);
                ArrayBitSystem[0] |= 1UL << 23; // $oneShot_15 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 23) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $oneShot_15
    } else {
        ArrayBitSystem[0] &= ~(1UL << 23); // $oneShot_15 = 0
    }

    if (!((ArrayBitSystem[0] >> 24) & 1)) {  // $seq2_antiglitch
        ArrayIntUser[13] = 9; // seq2
    }
    ArrayBitSystem[0] |= 1UL << 24; // $seq2_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[13] < 9) {
            ArrayIntUser[13]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[13] = 0; // seq2
    }

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 5); // $parOut_0 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitUser[0] |= 1UL << 2; else ArrayBitUser[0] &= ~(1UL << 2); // Espera = $parThis_0

    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[0] = 50; // $scratch2_int
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 25) & 1)) {  // $oneShot_16
            ArrayIntUser[11]++;
        }
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 25; else ArrayBitSystem[0] &= ~(1UL << 25); // $oneShot_16 = $parThis_0
    if (ArrayIntUser[0] > ArrayIntUser[11]) { // $scratch2_int > EsperaAnimacao
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    } else {
        ArrayBitSystem[0] |= 1UL << 6; // $parThis_0 = 1
    }

    ArrayIntUser[12] = 0; // $scratch_int
    ArrayIntUser[0] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        if (!((ArrayBitSystem[0] >> 26) & 1)) {  // $oneShot_17
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_Clear();
                ArrayBitSystem[0] |= 1UL << 26; // $oneShot_17 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 26) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $oneShot_17
    } else {
        ArrayBitSystem[0] &= ~(1UL << 26); // $oneShot_17 = 0
    }

    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $scratch = $parThis_0
    if (((ArrayBitSystem[0] >> 27) & 1)) {  // $oneShot_18
        ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = 0
    }
    if(((ArrayBitSystem[0] >> 3) & 1)) ArrayBitSystem[0] |= 1UL << 27; else ArrayBitSystem[0] &= ~(1UL << 27); // $oneShot_18 = $scratch

    /* iniciando paralelo [ */
    ArrayBitSystem[0] &= ~(1UL << 11); // $parOut_1 = 0
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $parThis_0
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayIntUser[1] = 5; // Etapa
    }

    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 6) & 1)) ArrayBitSystem[0] |= 1UL << 12; else ArrayBitSystem[0] &= ~(1UL << 12); // $parThis_1 = $parThis_0
    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayIntUser[10] = 0; // EsperaInformacao
    }

    if (((ArrayBitSystem[0] >> 12) & 1)) {  // $parThis_1
        ArrayBitSystem[0] |= 1UL << 11; // $parOut_1 = 1
    }
    if(((ArrayBitSystem[0] >> 11) & 1)) ArrayBitSystem[0] |= 1UL << 6; else ArrayBitSystem[0] &= ~(1UL << 6); // $parThis_0 = $parOut_1
    /* terminando paralelo [ */
    /* terminando serie [ */
    if (((ArrayBitSystem[0] >> 6) & 1)) {  // $parThis_0
        ArrayBitSystem[0] |= 1UL << 5; // $parOut_0 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0
    /* terminando paralelo [ */
    /* terminando serie [ */

    /* iniciando linha 17 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[0] = 3; // $scratch2_int
    if (ArrayIntUser[1] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayIntUser[12] = 0; // $scratch_int
    ArrayIntUser[0] = 2; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 28) & 1)) {  // $oneShot_19
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[12]); XP_lcd_WriteText("Projeto: %p", &MODBUS_REGISTER[0]);
                ArrayBitSystem[0] |= 1UL << 28; // $oneShot_19 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 28) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $oneShot_19
    } else {
        ArrayBitSystem[0] &= ~(1UL << 28); // $oneShot_19 = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[1] = 4; // Etapa
    }

    /* terminando serie [ */

    /* iniciando linha 18 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[0] = 2; // $scratch2_int
    if (ArrayIntUser[1] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayIntUser[12] = 0; // $scratch_int
    ArrayIntUser[0] = 1; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 29) & 1)) {  // $oneShot_20
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[12]); XP_lcd_WriteText("M: %m", &MODBUS_REGISTER[0]);
                ArrayBitSystem[0] |= 1UL << 29; // $oneShot_20 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 29) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $oneShot_20
    } else {
        ArrayBitSystem[0] &= ~(1UL << 29); // $oneShot_20 = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[1] = 3; // Etapa
    }

    /* terminando serie [ */

    /* iniciando linha 19 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[0] = 1; // $scratch2_int
    if (ArrayIntUser[1] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayIntUser[12] = 0; // $scratch_int
    ArrayIntUser[0] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 30) & 1)) {  // $oneShot_21
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_MoveCursor(ArrayIntUser[0],ArrayIntUser[12]); XP_lcd_WriteText("IP: %i", &MODBUS_REGISTER[0]);
                ArrayBitSystem[0] |= 1UL << 30; // $oneShot_21 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 30) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $oneShot_21
    } else {
        ArrayBitSystem[0] &= ~(1UL << 30); // $oneShot_21 = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[1] = 2; // Etapa
    }

    /* terminando serie [ */

    /* iniciando linha 20 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    ArrayIntUser[0] = 0; // $scratch2_int
    if (ArrayIntUser[1] == ArrayIntUser[0]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    ArrayIntUser[12] = 0; // $scratch_int
    ArrayIntUser[0] = 0; // $scratch2_int
    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 31) & 1)) {  // $oneShot_22
            if (I_LCDReady) {  // $LCDReady
                XP_lcd_Clear();
                ArrayBitSystem[0] |= 1UL << 31; // $oneShot_22 = 1
            }
        }
        if(((ArrayBitSystem[0] >> 31) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $oneShot_22
    } else {
        ArrayBitSystem[0] &= ~(1UL << 31); // $oneShot_22 = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        ArrayIntUser[1] = 1; // Etapa
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
