/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;
volatile unsigned int U_M[32];

extern unsigned int RS232Write(char c);
extern unsigned int ADCRead(unsigned int i);


volatile unsigned int I_mcr = 0;

volatile unsigned int I_rung_top = 0;

volatile unsigned int I_E1 = 0;

volatile unsigned int I_E3 = 0;

volatile unsigned int I_E15 = 0;

volatile unsigned int U_REMERGENCIA = 0;

volatile unsigned int I_parOut_0000 = 0;

volatile unsigned int I_parThis_0000 = 0;

volatile unsigned int U_S1 = 0;

volatile unsigned int I_parOut_0001 = 0;

volatile unsigned int I_parThis_0001 = 0;
volatile unsigned int U_TRESISTENCIA1 = 0;

volatile unsigned int I_E10 = 0;

volatile unsigned int I_E2 = 0;

volatile unsigned int I_parOut_0002 = 0;

volatile unsigned int I_parThis_0002 = 0;

volatile unsigned int U_S2 = 0;
volatile unsigned int U_TRESISTENCIA2 = 0;

volatile unsigned int U_S3 = 0;

volatile unsigned int I_parOut_0003 = 0;

volatile unsigned int I_parThis_0003 = 0;

volatile unsigned int I_parOut_0004 = 0;

volatile unsigned int I_parThis_0004 = 0;
volatile unsigned int U_TSTART1 = 0;

volatile unsigned int U_S4 = 0;

volatile unsigned int I_parOut_0005 = 0;

volatile unsigned int I_parThis_0005 = 0;
volatile unsigned int U_TSTART2 = 0;

volatile unsigned int U_S6 = 0;

volatile unsigned int I_parOut_0006 = 0;

volatile unsigned int I_parThis_0006 = 0;
volatile unsigned int U_TSTART3 = 0;

volatile unsigned int U_S8 = 0;

volatile unsigned int I_parOut_0007 = 0;

volatile unsigned int I_parThis_0007 = 0;

volatile unsigned int I_parOut_0008 = 0;

volatile unsigned int I_parThis_0008 = 0;

volatile unsigned int U_S5 = 0;
volatile unsigned int U_TRST_START1 = 0;

volatile unsigned int I_parOut_0009 = 0;

volatile unsigned int I_parThis_0009 = 0;

volatile unsigned int U_S7 = 0;
volatile unsigned int U_TRST_START2 = 0;

volatile unsigned int I_parOut_000a = 0;

volatile unsigned int I_parThis_000a = 0;

volatile unsigned int U_S9 = 0;
volatile unsigned int U_TRST_START3 = 0;

volatile unsigned int I_parOut_000b = 0;

volatile unsigned int I_parThis_000b = 0;

volatile unsigned int I_E4 = 0;

volatile unsigned int I_E5 = 0;

volatile unsigned int I_E6 = 0;

volatile unsigned int U_S15 = 0;
volatile unsigned int U_TTMR_BOMBA_LIGA = 0;

volatile unsigned int I_parOut_000c = 0;

volatile unsigned int I_parThis_000c = 0;
volatile unsigned int U_TBOMBA_LIGADA = 0;

volatile unsigned int U_S14 = 0;
volatile unsigned int U_TON = 0;

volatile unsigned int I_TOFF_antiglitch = 0;
volatile unsigned int U_TOFF = 0;


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I_E1) {
        I_rung_top = 0;
    }
    
    if (!I_E3) {
        I_rung_top = 0;
    }
    
    if (!I_E15) {
        I_rung_top = 0;
    }
    
    U_REMERGENCIA = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (!(U_M[2] & (1 << 0))) {  // XMODO_AQUECIMENT
        I_parThis_0000 = 0;
    }
    
    if (I_parThis_0000) {
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    if (U_M[2] & (1 << 0)) {  // XMODO_AQUECIMENT
        I_parThis_0000 = 0;
    }
    
    if (!(U_M[1] & (1 << 0))) {  // XLIGA_BOMBA
        I_parThis_0000 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0000) {
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    if (!U_REMERGENCIA) {
        I_rung_top = 0;
    }
    
    U_S1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_M[2] & (1 << 0)) {  // XMODO_AQUECIMENT
        I_rung_top = 0;
    }
    
    if (U_REMERGENCIA) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_M[1] &= ~(1 << 0);  // YRST_LIGA_BOMBA
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    /* start series [ */
    if (!(U_M[2] & (1 << 0))) {  // XMODO_AQUECIMENT
        I_parThis_0001 = 0;
    }
    
    if (I_parThis_0001) {
        if (U_TRESISTENCIA1 < 499) {
            U_TRESISTENCIA1++;
            I_parThis_0001 = 0;
        }
    } else {
        U_TRESISTENCIA1 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0001) {
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    /* start series [ */
    if (U_M[2] & (1 << 0)) {  // XMODO_AQUECIMENT
        I_parThis_0001 = 0;
    }
    
    if (!(U_M[1] & (1 << 1))) {  // XLIGA_GRUPO_RES
        I_parThis_0001 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0001) {
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    if (!U_REMERGENCIA) {
        I_rung_top = 0;
    }
    
    if (!I_E10) {
        I_rung_top = 0;
    }
    
    if (!I_E2) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0002 = 0;
    I_parThis_0002 = I_rung_top;
    U_S2 = I_parThis_0002;
    
    if (I_parThis_0002) {
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    /* start series [ */
    if (I_parThis_0002) {
        if (U_TRESISTENCIA2 < 499) {
            U_TRESISTENCIA2++;
            I_parThis_0002 = 0;
        }
    } else {
        U_TRESISTENCIA2 = 0;
    }
    
    U_S3 = I_parThis_0002;
    
    /* ] finish series */
    if (I_parThis_0002) {
        I_parOut_0002 = 1;
    }
    I_rung_top = I_parOut_0002;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 5 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_M[2] & (1 << 0)) {  // XMODO_AQUECIMENT
        I_rung_top = 0;
    }
    
    if (U_REMERGENCIA) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_M[1] &= ~(1 << 1);  // YRST_GRUPO_RES
    }
    
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0003 = 0;
    I_parThis_0003 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0004 = 0;
    I_parThis_0004 = I_parThis_0003;
    /* start series [ */
    if (!I_E10) {
        I_parThis_0004 = 0;
    }
    
    if (!(U_M[2] & (1 << 1))) {  // XMODO_BANHO
        I_parThis_0004 = 0;
    }
    
    if (I_parThis_0004) {
        if (U_TSTART1 < 1499) {
            U_TSTART1++;
            I_parThis_0004 = 0;
        }
    } else {
        U_TSTART1 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0004) {
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_parThis_0003;
    /* start series [ */
    if (U_M[2] & (1 << 1)) {  // XMODO_BANHO
        I_parThis_0004 = 0;
    }
    
    if (!(U_M[1] & (1 << 2))) {  // XLIGA_START1
        I_parThis_0004 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0004) {
        I_parOut_0004 = 1;
    }
    I_parThis_0003 = I_parOut_0004;
    /* ] finish parallel */
    if (!U_REMERGENCIA) {
        I_parThis_0003 = 0;
    }
    
    U_S4 = I_parThis_0003;
    
    /* ] finish series */
    if (I_parThis_0003) {
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0005 = 0;
    I_parThis_0005 = I_parThis_0003;
    /* start series [ */
    if (!(U_M[2] & (1 << 1))) {  // XMODO_BANHO
        I_parThis_0005 = 0;
    }
    
    if (!U_S4) {
        I_parThis_0005 = 0;
    }
    
    if (I_parThis_0005) {
        if (U_TSTART2 < 499) {
            U_TSTART2++;
            I_parThis_0005 = 0;
        }
    } else {
        U_TSTART2 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0005) {
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_parThis_0003;
    /* start series [ */
    if (U_M[2] & (1 << 1)) {  // XMODO_BANHO
        I_parThis_0005 = 0;
    }
    
    if (!(U_M[1] & (1 << 3))) {  // XLIGA_START2
        I_parThis_0005 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0005) {
        I_parOut_0005 = 1;
    }
    I_parThis_0003 = I_parOut_0005;
    /* ] finish parallel */
    if (!U_REMERGENCIA) {
        I_parThis_0003 = 0;
    }
    
    U_S6 = I_parThis_0003;
    
    /* ] finish series */
    if (I_parThis_0003) {
        I_parOut_0003 = 1;
    }
    I_rung_top = I_parOut_0003;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 7 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0006 = 0;
    I_parThis_0006 = I_rung_top;
    /* start series [ */
    if (!(U_M[2] & (1 << 1))) {  // XMODO_BANHO
        I_parThis_0006 = 0;
    }
    
    if (!U_S6) {
        I_parThis_0006 = 0;
    }
    
    if (I_parThis_0006) {
        if (U_TSTART3 < 499) {
            U_TSTART3++;
            I_parThis_0006 = 0;
        }
    } else {
        U_TSTART3 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0006) {
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    /* start series [ */
    if (U_M[2] & (1 << 1)) {  // XMODO_BANHO
        I_parThis_0006 = 0;
    }
    
    if (!(U_M[1] & (1 << 4))) {  // XLIGA_START3
        I_parThis_0006 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0006) {
        I_parOut_0006 = 1;
    }
    I_rung_top = I_parOut_0006;
    /* ] finish parallel */
    if (!U_REMERGENCIA) {
        I_rung_top = 0;
    }
    
    U_S8 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 8 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_M[2] & (1 << 1)) {  // XMODO_BANHO
        I_rung_top = 0;
    }
    
    if (U_REMERGENCIA) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0007 = 0;
    I_parThis_0007 = I_rung_top;
    if (I_parThis_0007) {
        U_M[1] &= ~(1 << 2);  // YRST_LIGA_START1
    }
    
    if (I_parThis_0007) {
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    if (I_parThis_0007) {
        U_M[1] &= ~(1 << 3);  // YRST_LIGA_START2
    }
    
    if (I_parThis_0007) {
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    if (I_parThis_0007) {
        U_M[1] &= ~(1 << 4);  // YRST_LIGA_START3
    }
    
    if (I_parThis_0007) {
        I_parOut_0007 = 1;
    }
    I_rung_top = I_parOut_0007;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 9 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(U_M[1] & (1 << 5))) {  // YRST_ALARME_ST1
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0008 = 0;
    I_parThis_0008 = I_rung_top;
    U_S5 = I_parThis_0008;
    
    if (I_parThis_0008) {
        I_parOut_0008 = 1;
    }
    I_parThis_0008 = I_rung_top;
    /* start series [ */
    if (I_parThis_0008) {
        if (U_TRST_START1 < 99) {
            U_TRST_START1++;
            I_parThis_0008 = 0;
        }
    } else {
        U_TRST_START1 = 0;
    }
    
    if (I_parThis_0008) {
        U_M[1] &= ~(1 << 5);  // YCLR_START1
    }
    
    /* ] finish series */
    if (I_parThis_0008) {
        I_parOut_0008 = 1;
    }
    I_rung_top = I_parOut_0008;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 10 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(U_M[1] & (1 << 6))) {  // XRST_ALARME_ST2
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0009 = 0;
    I_parThis_0009 = I_rung_top;
    U_S7 = I_parThis_0009;
    
    if (I_parThis_0009) {
        I_parOut_0009 = 1;
    }
    I_parThis_0009 = I_rung_top;
    /* start series [ */
    if (I_parThis_0009) {
        if (U_TRST_START2 < 99) {
            U_TRST_START2++;
            I_parThis_0009 = 0;
        }
    } else {
        U_TRST_START2 = 0;
    }
    
    if (I_parThis_0009) {
        U_M[1] &= ~(1 << 6);  // YCLR_START2
    }
    
    /* ] finish series */
    if (I_parThis_0009) {
        I_parOut_0009 = 1;
    }
    I_rung_top = I_parOut_0009;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 11 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(U_M[1] & (1 << 7))) {  // XRST_ALARME_ST3
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_000a = 0;
    I_parThis_000a = I_rung_top;
    U_S9 = I_parThis_000a;
    
    if (I_parThis_000a) {
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (I_parThis_000a) {
        if (U_TRST_START3 < 99) {
            U_TRST_START3++;
            I_parThis_000a = 0;
        }
    } else {
        U_TRST_START3 = 0;
    }
    
    if (I_parThis_000a) {
        U_M[1] &= ~(1 << 7);  // YCLR_START3
    }
    
    /* ] finish series */
    if (I_parThis_000a) {
        I_parOut_000a = 1;
    }
    I_rung_top = I_parOut_000a;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 12 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(U_M[2] & (1 << 1))) {  // XMODO_BANHO
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_000b = 0;
    I_parThis_000b = I_rung_top;
    if (U_REMERGENCIA) {
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_E4) {
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_E5) {
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_E6) {
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {
        I_parOut_000b = 1;
    }
    I_rung_top = I_parOut_000b;
    /* ] finish parallel */
    U_S15 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 13 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_E1) {
        I_rung_top = 0;
    }
    
    U_M[0] &= ~(1 << 0); U_M[0] |= I_rung_top << 0;  // YST_EMERGENCIA
    
    /* ] finish series */
    
    /* start rung 14 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_E15) {
        I_rung_top = 0;
    }
    
    U_M[0] &= ~(1 << 1); U_M[0] |= I_rung_top << 1;  // YST_FASE
    
    /* ] finish series */
    
    /* start rung 15 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_S1) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TTMR_BOMBA_LIGA < 499) {
            U_TTMR_BOMBA_LIGA++;
            I_rung_top = 0;
        }
    } else {
        U_TTMR_BOMBA_LIGA = 0;
    }
    
    if (I_E10) {
        I_rung_top = 0;
    }
    
    U_M[0] &= ~(1 << 2); U_M[0] |= I_rung_top << 2;  // YST_ERRO_BOMBA
    
    /* ] finish series */
    
    /* start rung 16 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_S4) {
        I_rung_top = 0;
    }
    
    if (I_E4) {
        I_rung_top = 0;
    }
    
    U_M[0] &= ~(1 << 3); U_M[0] |= I_rung_top << 3;  // YST_ALARME1
    
    /* ] finish series */
    
    /* start rung 17 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_S6) {
        I_rung_top = 0;
    }
    
    if (I_E5) {
        I_rung_top = 0;
    }
    
    U_M[0] &= ~(1 << 4); U_M[0] |= I_rung_top << 4;  // YST_ALARME2
    
    /* ] finish series */
    
    /* start rung 18 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_S8) {
        I_rung_top = 0;
    }
    
    if (I_E6) {
        I_rung_top = 0;
    }
    
    U_M[0] &= ~(1 << 5); U_M[0] |= I_rung_top << 5;  // YST_ALARME3
    
    /* ] finish series */
    
    /* start rung 19 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_000c = 0;
    I_parThis_000c = I_rung_top;
    if (U_REMERGENCIA) {
        I_parThis_000c = 0;
    }
    
    if (I_parThis_000c) {
        I_parOut_000c = 1;
    }
    I_parThis_000c = I_rung_top;
    /* start series [ */
    if (!U_S1) {
        I_parThis_000c = 0;
    }
    
    if (I_parThis_000c) {
        if (U_TBOMBA_LIGADA < 499) {
            U_TBOMBA_LIGADA++;
            I_parThis_000c = 0;
        }
    } else {
        U_TBOMBA_LIGADA = 0;
    }
    
    if (I_E10) {
        I_parThis_000c = 0;
    }
    
    /* ] finish series */
    if (I_parThis_000c) {
        I_parOut_000c = 1;
    }
    I_rung_top = I_parOut_000c;
    /* ] finish parallel */
    if (U_S14) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TON < 49) {
            U_TON++;
            I_rung_top = 0;
        }
    } else {
        U_TON = 0;
    }
    
    if (!I_TOFF_antiglitch) {
        U_TOFF = 49;
    }
    I_TOFF_antiglitch = 1;
    if (!I_rung_top) {
        if (U_TOFF < 49) {
            U_TOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TOFF = 0;
    }
    
    U_S14 = I_rung_top;
    
    /* ] finish series */
}
