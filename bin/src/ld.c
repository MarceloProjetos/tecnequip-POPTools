/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "uss.h"
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;
volatile unsigned int M[32];
volatile int ENC1;

extern unsigned int RS232Write(char c);
extern unsigned int ADCRead(unsigned int i);
extern unsigned int ENCRead(unsigned int i);
extern unsigned int ENCReset(unsigned int i);
extern volatile unsigned int I_USSReady;

volatile unsigned int I_mcr = 0;

volatile unsigned int I_rung_top = 0;

volatile unsigned int I_parOut_0000 = 0;

volatile unsigned int I_parThis_0000 = 0;

volatile unsigned int I11 = 0;

volatile unsigned int I18 = 0;

volatile unsigned int U_RErroGeral = 0;

volatile unsigned int I3 = 0;

volatile unsigned int I4 = 0;

volatile unsigned int I_parOut_0001 = 0;

volatile unsigned int I_parThis_0001 = 0;

volatile unsigned int U_RModoCorteSerra = 0;

volatile unsigned int I2 = 0;

volatile unsigned int U_RMaqOK = 0;

volatile unsigned int I15 = 0;

volatile unsigned int U_RCrtCantoAtivo = 0;

volatile unsigned int I_parOut_0002 = 0;

volatile unsigned int I_parThis_0002 = 0;

volatile unsigned int U14 = 0;
volatile unsigned int U_TCrtCantoEstab = 0;

volatile unsigned int I_parOut_0003 = 0;

volatile unsigned int I_parThis_0003 = 0;

volatile unsigned int I_scratch = 0;

volatile unsigned int I_oneShot_0000 = 0;

volatile unsigned int U_RCrtCantoDesce = 0;

volatile unsigned int U_RCrtCantoOK = 0;

volatile unsigned int I_parOut_0004 = 0;

volatile unsigned int I_parThis_0004 = 0;
volatile unsigned int U_TMaxCorteCanto = 0;

volatile unsigned int I_parOut_0005 = 0;

volatile unsigned int I_parThis_0005 = 0;

volatile unsigned int I_parOut_0006 = 0;

volatile unsigned int I_parThis_0006 = 0;

volatile unsigned int U15 = 0;

volatile unsigned int I17 = 0;

volatile unsigned int I_parOut_0007 = 0;

volatile unsigned int I_parThis_0007 = 0;

volatile unsigned int U16 = 0;

volatile unsigned int I16 = 0;

volatile unsigned int I_parOut_0008 = 0;

volatile unsigned int I_parThis_0008 = 0;

volatile unsigned int I8 = 0;

volatile unsigned int I6 = 0;

volatile unsigned int U_RSnsSupCorteTubo = 0;

volatile unsigned int I_parOut_0009 = 0;

volatile unsigned int I_parThis_0009 = 0;

volatile unsigned int I9 = 0;

volatile unsigned int I7 = 0;

volatile unsigned int U_RSnsInfCorteTubo = 0;

volatile unsigned int I_parOut_000a = 0;

volatile unsigned int I_parThis_000a = 0;

volatile unsigned int U_RSerraOK = 0;

volatile unsigned int U_RBombaHidrOK = 0;

volatile unsigned int U_RCortePronto = 0;

volatile unsigned int U_RFerramCorteCima = 0;

volatile unsigned int U17 = 0;

volatile unsigned int U_RCorteTuboAtivo = 0;
volatile unsigned int U_TBombaHidrEstab = 0;

volatile unsigned int I_parOut_000b = 0;

volatile unsigned int I_parThis_000b = 0;
volatile unsigned int U_TBombaHidrInat = 0;

volatile unsigned int U13 = 0;
volatile unsigned int U_TSerraEstab = 0;

volatile unsigned int I_parOut_000c = 0;

volatile unsigned int I_parThis_000c = 0;
volatile unsigned int U_TSerraInat = 0;

volatile unsigned int U_RErroCorteTubo = 0;

volatile unsigned int U_RCortarTubo = 0;

volatile unsigned int I_parOut_000d = 0;

volatile unsigned int I_parThis_000d = 0;

volatile unsigned int U_RCorteTuboDesce = 0;

volatile unsigned int I_parOut_000e = 0;

volatile unsigned int I_parThis_000e = 0;
volatile unsigned int U_TMaxCorteTubo = 0;

volatile unsigned int I_parOut_000f = 0;

volatile unsigned int I_parThis_000f = 0;

volatile unsigned int I_parOut_0010 = 0;

volatile unsigned int I_parThis_0010 = 0;

volatile unsigned int U11 = 0;

volatile unsigned int U2 = 0;

volatile unsigned int I_parOut_0011 = 0;

volatile unsigned int I_parThis_0011 = 0;

volatile unsigned int U3 = 0;

volatile unsigned int I_parOut_0012 = 0;

volatile unsigned int I_parThis_0012 = 0;

volatile unsigned int I_parOut_0013 = 0;

volatile unsigned int I_parThis_0013 = 0;

volatile unsigned int U_RModoAuto = 0;

volatile unsigned int I13 = 0;

volatile unsigned int I14 = 0;

volatile unsigned int I_parOut_0014 = 0;

volatile unsigned int I_parThis_0014 = 0;

volatile unsigned int I19 = 0;

volatile unsigned int U_RMaqProduzindo = 0;

volatile unsigned int U_RCortarTuboAuto = 0;

volatile unsigned int I_parOut_0015 = 0;

volatile unsigned int I_parThis_0015 = 0;

volatile unsigned int U5 = 0;

volatile unsigned int U4 = 0;

volatile unsigned int I_oneShot_0001 = 0;

volatile unsigned int I_TPulsoSerraPara_antiglitch = 0;
volatile unsigned int U_TPulsoSerraPara = 0;

volatile unsigned int U12 = 0;

volatile unsigned int I_parOut_0016 = 0;

volatile unsigned int I_parThis_0016 = 0;

volatile unsigned int U_RMaqIniciarPosic = 0;

volatile unsigned int I_parOut_0017 = 0;

volatile unsigned int I_parThis_0017 = 0;

volatile unsigned int U_RAvancarPerfMan = 0;

volatile unsigned int U_RRecuarPerfMan = 0;

volatile unsigned int I_TMantemFreioLib_antiglitch = 0;
volatile unsigned int U_TMantemFreioLib = 0;

volatile unsigned int U6 = 0;
volatile unsigned int U_TEsperaFreioLib = 0;

volatile unsigned int U_RInvFreioLib = 0;

volatile unsigned int I_parOut_0018 = 0;

volatile unsigned int I_parThis_0018 = 0;

volatile unsigned int U7 = 0;

volatile unsigned int U8 = 0;

volatile unsigned int U_RMaqConfigOK = 0;

volatile unsigned int U_RPerfManualOK = 0;

volatile unsigned int I_parOut_0019 = 0;

volatile unsigned int I_parThis_0019 = 0;

volatile unsigned int U_RBloqueiaRecuo = 0;

volatile unsigned int I_parOut_001a = 0;

volatile unsigned int I_parThis_001a = 0;
volatile unsigned int U_TMaxPerfRecua = 0;

volatile unsigned int U_RStartConfigAuto = 0;

volatile unsigned int U_RMaqConfigErro = 0;

volatile unsigned int I_parOut_001b = 0;

volatile unsigned int I_parThis_001b = 0;

volatile unsigned int I_oneShot_0002 = 0;
volatile unsigned int U_TConfigAutoTO = 0;

volatile unsigned int I_oneShot_0003 = 0;

volatile unsigned int I_USSReady = 0;
volatile unsigned int U_WInvAutoVel = 0;

volatile unsigned int I_oneShot_0004 = 0;

volatile unsigned int I_oneShot_0005 = 0;
volatile unsigned int U_WInvAutoAcel = 0;

volatile unsigned int I_oneShot_0006 = 0;

volatile unsigned int I_oneShot_0007 = 0;
volatile unsigned int U_WInvAutoDesacel = 0;

volatile unsigned int I_oneShot_0008 = 0;

volatile unsigned int I_oneShot_0009 = 0;
volatile unsigned int U_WInvTam = 0;

volatile unsigned int I_oneShot_000a = 0;

volatile unsigned int I_oneShot_000b = 0;
volatile unsigned int U_WMbPerfFator = 0;

volatile unsigned int I_oneShot_000c = 0;

volatile unsigned int I_parOut_001c = 0;

volatile unsigned int I_parThis_001c = 0;

volatile unsigned int U_RStartConfigMan = 0;

volatile unsigned int I_parOut_001d = 0;

volatile unsigned int I_parThis_001d = 0;

volatile unsigned int I_oneShot_000d = 0;
volatile unsigned int U_TConfigManTO = 0;

volatile unsigned int I_oneShot_000e = 0;
volatile unsigned int U_WInvModoMan = 0;

volatile unsigned int I_oneShot_000f = 0;

volatile unsigned int I_oneShot_0010 = 0;
volatile unsigned int U_WInvManVel = 0;

volatile unsigned int I_oneShot_0011 = 0;

volatile unsigned int I_oneShot_0012 = 0;
volatile unsigned int U_WInvManAcel = 0;

volatile unsigned int I_oneShot_0013 = 0;

volatile unsigned int I_oneShot_0014 = 0;
volatile unsigned int U_WInvManDesacel = 0;

volatile unsigned int I_oneShot_0015 = 0;

volatile unsigned int I_parOut_001e = 0;

volatile unsigned int I_parThis_001e = 0;

volatile unsigned int U_RModoAutoOK = 0;

volatile unsigned int I_parOut_001f = 0;

volatile unsigned int I_parThis_001f = 0;

volatile unsigned int I_oneShot_0016 = 0;

volatile unsigned int I_oneShot_0017 = 0;

volatile unsigned int I_parOut_0020 = 0;

volatile unsigned int I_parThis_0020 = 0;

volatile unsigned int I_oneShot_0018 = 0;

volatile unsigned int I_parOut_0021 = 0;

volatile unsigned int I_parThis_0021 = 0;
volatile unsigned int U_CQtdProd = 0;

volatile unsigned int I_oneShot_0019 = 0;

volatile unsigned int I5 = 0;
volatile unsigned int I_scratch2 = 0;

volatile unsigned int I_parOut_0022 = 0;

volatile unsigned int I_parThis_0022 = 0;

volatile unsigned int U9 = 0;

volatile unsigned int I10 = 0;
volatile unsigned int U_TFimPosicEstab = 0;

volatile unsigned int I_parOut_0023 = 0;

volatile unsigned int I_parThis_0023 = 0;

volatile unsigned int U_RMaqFimPosic = 0;

volatile unsigned int I_oneShot_001a = 0;

volatile unsigned int I_parOut_0024 = 0;

volatile unsigned int I_parThis_0024 = 0;

volatile unsigned int I_oneShot_001b = 0;

volatile unsigned int I_parOut_0025 = 0;

volatile unsigned int I_parThis_0025 = 0;

volatile unsigned int U10 = 0;

volatile unsigned int I_oneShot_001c = 0;

volatile unsigned int I_parOut_0026 = 0;

volatile unsigned int I_parThis_0026 = 0;

volatile unsigned int I_oneShot_001d = 0;

volatile unsigned int I_parOut_0027 = 0;

volatile unsigned int I_parThis_0027 = 0;

volatile unsigned int I_parOut_0028 = 0;

volatile unsigned int I_parThis_0028 = 0;

volatile unsigned int I_oneShot_001e = 0;

volatile unsigned int I_oneShot_001f = 0;

volatile unsigned int I_parOut_0029 = 0;

volatile unsigned int I_parThis_0029 = 0;

volatile unsigned int I12 = 0;

volatile unsigned int I_oneShot_0020 = 0;

volatile unsigned int I_parOut_002a = 0;

volatile unsigned int I_parThis_002a = 0;

volatile unsigned int I_oneShot_0021 = 0;

volatile unsigned int I_parOut_002b = 0;

volatile unsigned int I_parThis_002b = 0;
volatile unsigned int U_WInvModoAuto = 0;


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    I_mcr = 1;
    
    /* start rung 47 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I11) {
        I_parThis_0000 = 0;
    }
    
    if (I_parThis_0000) {
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I18) {
        I_parThis_0000 = 0;
    }
    
    if (I_parThis_0000) {
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    U_RErroGeral = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 48 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RErroGeral) {
        I_rung_top = 0;
    }
    
    if (!I3) {
        I_rung_top = 0;
    }
    
    if (!I4) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    if (!U_RModoCorteSerra) {
        I_parThis_0001 = 0;
    }
    
    if (I_parThis_0001) {
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (!I2) {
        I_parThis_0001 = 0;
    }
    
    if (I_parThis_0001) {
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    U_RMaqOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 50 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RErroGeral) {
        I_rung_top = 0;
    }
    
    if (!I15) {
        I_rung_top = 0;
    }
    
    if (U_RCrtCantoAtivo) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0002 = 0;
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {
        U_RCrtCantoAtivo = 1;
    }
    
    if (I_parThis_0002) {
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {
        U14 = 1;
    }
    
    if (I_parThis_0002) {
        I_parOut_0002 = 1;
    }
    I_rung_top = I_parOut_0002;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 51 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U14) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TCrtCantoEstab < 149) {
            U_TCrtCantoEstab++;
            I_rung_top = 0;
        }
    } else {
        U_TCrtCantoEstab = 0;
    }
    
    /* start parallel [ */
    I_parOut_0003 = 0;
    I_parThis_0003 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0003;
    if (I_oneShot_0000) {
        I_parThis_0003 = 0;
    }
    I_oneShot_0000 = I_scratch;
    
    if (I_parThis_0003) {
        U_RCrtCantoDesce = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0003) {
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    U_RCrtCantoOK = I_parThis_0003;
    
    if (I_parThis_0003) {
        I_parOut_0003 = 1;
    }
    I_rung_top = I_parOut_0003;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 52 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCrtCantoOK) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0004 = 0;
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {
        if (U_TMaxCorteCanto < 999) {
            U_TMaxCorteCanto++;
            I_parThis_0004 = 0;
        }
    } else {
        U_TMaxCorteCanto = 0;
    }
    
    if (I_parThis_0004) {
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (!U_RErroGeral) {
        I_parThis_0004 = 0;
    }
    
    if (I_parThis_0004) {
        I_parOut_0004 = 1;
    }
    I_rung_top = I_parOut_0004;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0005 = 0;
    I_parThis_0005 = I_rung_top;
    if (I_parThis_0005) {
        U14 = 0;
    }
    
    if (I_parThis_0005) {
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (I_parThis_0005) {
        U_RCrtCantoDesce = 0;
    }
    
    if (I_parThis_0005) {
        I_parOut_0005 = 1;
    }
    I_rung_top = I_parOut_0005;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 53 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCrtCantoOK) {
        I_rung_top = 0;
    }
    
    if (!U_RCrtCantoDesce) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0006 = 0;
    I_parThis_0006 = I_rung_top;
    U15 = I_parThis_0006;
    
    if (I_parThis_0006) {
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    /* start series [ */
    if (!I17) {
        I_parThis_0006 = 0;
    }
    
    if (I_parThis_0006) {
        U_RCrtCantoDesce = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0006) {
        I_parOut_0006 = 1;
    }
    I_rung_top = I_parOut_0006;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 54 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCrtCantoOK) {
        I_rung_top = 0;
    }
    
    if (U_RCrtCantoDesce) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0007 = 0;
    I_parThis_0007 = I_rung_top;
    U16 = I_parThis_0007;
    
    if (I_parThis_0007) {
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    /* start series [ */
    if (!I16) {
        I_parThis_0007 = 0;
    }
    
    if (I_parThis_0007) {
        U14 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0007) {
        I_parOut_0007 = 1;
    }
    I_rung_top = I_parOut_0007;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 55 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I15) {
        I_rung_top = 0;
    }
    
    if (!U_RCrtCantoAtivo) {
        I_rung_top = 0;
    }
    
    if (U14) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_RCrtCantoAtivo = 0;
    }
    
    /* ] finish series */
    
    /* start rung 57 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0008 = 0;
    I_parThis_0008 = I_rung_top;
    /* start series [ */
    if (!U_RModoCorteSerra) {
        I_parThis_0008 = 0;
    }
    
    if (!I8) {
        I_parThis_0008 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0008) {
        I_parOut_0008 = 1;
    }
    I_parThis_0008 = I_rung_top;
    /* start series [ */
    if (U_RModoCorteSerra) {
        I_parThis_0008 = 0;
    }
    
    if (!I6) {
        I_parThis_0008 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0008) {
        I_parOut_0008 = 1;
    }
    I_rung_top = I_parOut_0008;
    /* ] finish parallel */
    U_RSnsSupCorteTubo = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 58 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0009 = 0;
    I_parThis_0009 = I_rung_top;
    /* start series [ */
    if (!U_RModoCorteSerra) {
        I_parThis_0009 = 0;
    }
    
    if (!I9) {
        I_parThis_0009 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0009) {
        I_parOut_0009 = 1;
    }
    I_parThis_0009 = I_rung_top;
    /* start series [ */
    if (U_RModoCorteSerra) {
        I_parThis_0009 = 0;
    }
    
    if (!I7) {
        I_parThis_0009 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0009) {
        I_parOut_0009 = 1;
    }
    I_rung_top = I_parOut_0009;
    /* ] finish parallel */
    U_RSnsInfCorteTubo = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 59 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_000a = 0;
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (!U_RModoCorteSerra) {
        I_parThis_000a = 0;
    }
    
    if (!U_RSerraOK) {
        I_parThis_000a = 0;
    }
    
    /* ] finish series */
    if (I_parThis_000a) {
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (U_RModoCorteSerra) {
        I_parThis_000a = 0;
    }
    
    if (!U_RBombaHidrOK) {
        I_parThis_000a = 0;
    }
    
    /* ] finish series */
    if (I_parThis_000a) {
        I_parOut_000a = 1;
    }
    I_rung_top = I_parOut_000a;
    /* ] finish parallel */
    U_RCortePronto = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 60 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I8) {
        I_rung_top = 0;
    }
    
    if (!I6) {
        I_rung_top = 0;
    }
    
    if (I9) {
        I_rung_top = 0;
    }
    
    if (I7) {
        I_rung_top = 0;
    }
    
    U_RFerramCorteCima = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 62 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {
        I_rung_top = 0;
    }
    
    if (U_RModoCorteSerra) {
        I_rung_top = 0;
    }
    
    if (U17) {
        I_rung_top = 0;
    }
    
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U17 = 1;
    }
    
    /* ] finish series */
    
    /* start rung 63 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RModoCorteSerra) {
        I_rung_top = 0;
    }
    
    if (!U17) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TBombaHidrEstab < 499) {
            U_TBombaHidrEstab++;
            I_rung_top = 0;
        }
    } else {
        U_TBombaHidrEstab = 0;
    }
    
    U_RBombaHidrOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 64 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_000b = 0;
    I_parThis_000b = I_rung_top;
    /* start series [ */
    if (!U_RBombaHidrOK) {
        I_parThis_000b = 0;
    }
    
    if (U_RCorteTuboAtivo) {
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {
        if (U_TBombaHidrInat < 29999) {
            U_TBombaHidrInat++;
            I_parThis_000b = 0;
        }
    } else {
        U_TBombaHidrInat = 0;
    }
    
    /* ] finish series */
    if (I_parThis_000b) {
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (!U_RModoCorteSerra) {
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (U_RMaqOK) {
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {
        I_parOut_000b = 1;
    }
    I_rung_top = I_parOut_000b;
    /* ] finish parallel */
    if (I_rung_top) {
        U17 = 0;
    }
    
    /* ] finish series */
    
    /* start rung 66 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {
        I_rung_top = 0;
    }
    
    if (!U_RModoCorteSerra) {
        I_rung_top = 0;
    }
    
    if (U13) {
        I_rung_top = 0;
    }
    
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U13 = 1;
    }
    
    /* ] finish series */
    
    /* start rung 67 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RModoCorteSerra) {
        I_rung_top = 0;
    }
    
    if (!U13) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TSerraEstab < 499) {
            U_TSerraEstab++;
            I_rung_top = 0;
        }
    } else {
        U_TSerraEstab = 0;
    }
    
    U_RSerraOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 68 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_000c = 0;
    I_parThis_000c = I_rung_top;
    /* start series [ */
    if (!U_RSerraOK) {
        I_parThis_000c = 0;
    }
    
    if (U_RCorteTuboAtivo) {
        I_parThis_000c = 0;
    }
    
    if (I_parThis_000c) {
        if (U_TSerraInat < 29999) {
            U_TSerraInat++;
            I_parThis_000c = 0;
        }
    } else {
        U_TSerraInat = 0;
    }
    
    /* ] finish series */
    if (I_parThis_000c) {
        I_parOut_000c = 1;
    }
    I_parThis_000c = I_rung_top;
    if (U_RModoCorteSerra) {
        I_parThis_000c = 0;
    }
    
    if (I_parThis_000c) {
        I_parOut_000c = 1;
    }
    I_parThis_000c = I_rung_top;
    if (U_RMaqOK) {
        I_parThis_000c = 0;
    }
    
    if (I_parThis_000c) {
        I_parOut_000c = 1;
    }
    I_rung_top = I_parOut_000c;
    /* ] finish parallel */
    if (I_rung_top) {
        U13 = 0;
    }
    
    /* ] finish series */
    
    /* start rung 70 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {
        I_rung_top = 0;
    }
    
    if (U_RErroCorteTubo) {
        I_rung_top = 0;
    }
    
    if (!U_RCortarTubo) {
        I_rung_top = 0;
    }
    
    if (U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_000d = 0;
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {
        U_RCorteTuboAtivo = 1;
    }
    
    if (I_parThis_000d) {
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {
        U_RCorteTuboDesce = 1;
    }
    
    if (I_parThis_000d) {
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {
        U_TBombaHidrInat = 0;
    }
    
    if (I_parThis_000d) {
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {
        U_TSerraInat = 0;
    }
    
    if (I_parThis_000d) {
        I_parOut_000d = 1;
    }
    I_rung_top = I_parOut_000d;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 71 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_000e = 0;
    I_parThis_000e = I_rung_top;
    /* start series [ */
    if (!U_RCortePronto) {
        I_parThis_000e = 0;
    }
    
    if (I_parThis_000e) {
        if (U_TMaxCorteTubo < 1499) {
            U_TMaxCorteTubo++;
            I_parThis_000e = 0;
        }
    } else {
        U_TMaxCorteTubo = 0;
    }
    
    /* ] finish series */
    if (I_parThis_000e) {
        I_parOut_000e = 1;
    }
    I_parThis_000e = I_rung_top;
    if (U_RMaqOK) {
        I_parThis_000e = 0;
    }
    
    if (I_parThis_000e) {
        I_parOut_000e = 1;
    }
    I_rung_top = I_parOut_000e;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_000f = 0;
    I_parThis_000f = I_rung_top;
    if (I_parThis_000f) {
        U_RErroCorteTubo = 1;
    }
    
    if (I_parThis_000f) {
        I_parOut_000f = 1;
    }
    I_parThis_000f = I_rung_top;
    if (I_parThis_000f) {
        U_RCorteTuboDesce = 0;
    }
    
    if (I_parThis_000f) {
        I_parOut_000f = 1;
    }
    I_parThis_000f = I_rung_top;
    if (I_parThis_000f) {
        U_RCorteTuboAtivo = 0;
    }
    
    if (I_parThis_000f) {
        I_parOut_000f = 1;
    }
    I_rung_top = I_parOut_000f;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 72 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    if (!U_RCortePronto) {
        I_rung_top = 0;
    }
    
    if (!U_RCorteTuboDesce) {
        I_rung_top = 0;
    }
    
    if (!U_RSnsInfCorteTubo) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_RCorteTuboDesce = 0;
    }
    
    /* ] finish series */
    
    /* start rung 73 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    if (!U_RCortePronto) {
        I_rung_top = 0;
    }
    
    if (U_RCorteTuboDesce) {
        I_rung_top = 0;
    }
    
    if (!U_RSnsSupCorteTubo) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_RCorteTuboAtivo = 0;
    }
    
    /* ] finish series */
    
    /* start rung 75 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    if (!U_RCortePronto) {
        I_rung_top = 0;
    }
    
    if (!U_RCorteTuboDesce) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0010 = 0;
    I_parThis_0010 = I_rung_top;
    /* start series [ */
    if (!U_RModoCorteSerra) {
        I_parThis_0010 = 0;
    }
    
    if (I_parThis_0010) {
        U11 = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0010) {
        I_parOut_0010 = 1;
    }
    I_parThis_0010 = I_rung_top;
    /* start series [ */
    if (U_RModoCorteSerra) {
        I_parThis_0010 = 0;
    }
    
    U2 = I_parThis_0010;
    
    /* ] finish series */
    if (I_parThis_0010) {
        I_parOut_0010 = 1;
    }
    I_rung_top = I_parOut_0010;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 76 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    if (!U_RCortePronto) {
        I_rung_top = 0;
    }
    
    if (U_RCorteTuboDesce) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0011 = 0;
    I_parThis_0011 = I_rung_top;
    /* start series [ */
    if (!U_RModoCorteSerra) {
        I_parThis_0011 = 0;
    }
    
    if (I_parThis_0011) {
        U11 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0011) {
        I_parOut_0011 = 1;
    }
    I_parThis_0011 = I_rung_top;
    /* start series [ */
    if (U_RModoCorteSerra) {
        I_parThis_0011 = 0;
    }
    
    U3 = I_parThis_0011;
    
    /* ] finish series */
    if (I_parThis_0011) {
        I_parOut_0011 = 1;
    }
    I_rung_top = I_parOut_0011;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 77 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0012 = 0;
    I_parThis_0012 = I_rung_top;
    if (!U_RErroCorteTubo) {
        I_parThis_0012 = 0;
    }
    
    if (I_parThis_0012) {
        I_parOut_0012 = 1;
    }
    I_parThis_0012 = I_rung_top;
    if (U_RModoCorteSerra) {
        I_parThis_0012 = 0;
    }
    
    if (I_parThis_0012) {
        I_parOut_0012 = 1;
    }
    I_rung_top = I_parOut_0012;
    /* ] finish parallel */
    if (I_rung_top) {
        U11 = 0;
    }
    
    /* ] finish series */
    
    /* start rung 78 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0013 = 0;
    I_parThis_0013 = I_rung_top;
    /* start series [ */
    if (U_RModoAuto) {
        I_parThis_0013 = 0;
    }
    
    if (I13) {
        I_parThis_0013 = 0;
    }
    
    if (I14) {
        I_parThis_0013 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0014 = 0;
    I_parThis_0014 = I_parThis_0013;
    /* start series [ */
    if (U_RModoCorteSerra) {
        I_parThis_0014 = 0;
    }
    
    if (!I19) {
        I_parThis_0014 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0014) {
        I_parOut_0014 = 1;
    }
    I_parThis_0014 = I_parThis_0013;
    if (!(M[2] & (1 << 0))) {  // XMbCorteIHM
        I_parThis_0014 = 0;
    }
    
    if (I_parThis_0014) {
        I_parOut_0014 = 1;
    }
    I_parThis_0013 = I_parOut_0014;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0013) {
        I_parOut_0013 = 1;
    }
    I_parThis_0013 = I_rung_top;
    /* start series [ */
    if (!U_RModoAuto) {
        I_parThis_0013 = 0;
    }
    
    if (!U_RMaqProduzindo) {
        I_parThis_0013 = 0;
    }
    
    if (!U_RCortarTuboAuto) {
        I_parThis_0013 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0013) {
        I_parOut_0013 = 1;
    }
    I_rung_top = I_parOut_0013;
    /* ] finish parallel */
    U_RCortarTubo = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 79 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    if (U_RModoCorteSerra) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0015 = 0;
    I_parThis_0015 = I_rung_top;
    U5 = I_parThis_0015;
    
    if (I_parThis_0015) {
        I_parOut_0015 = 1;
    }
    I_parThis_0015 = I_rung_top;
    U4 = I_parThis_0015;
    
    if (I_parThis_0015) {
        I_parOut_0015 = 1;
    }
    I_rung_top = I_parOut_0015;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 80 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RErroCorteTubo) {
        I_rung_top = 0;
    }
    
    if (!U_RModoCorteSerra) {
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0001) {
        I_rung_top = 0;
    }
    I_oneShot_0001 = I_scratch;
    
    if (!I_TPulsoSerraPara_antiglitch) {
        U_TPulsoSerraPara = 99;
    }
    I_TPulsoSerraPara_antiglitch = 1;
    if (!I_rung_top) {
        if (U_TPulsoSerraPara < 99) {
            U_TPulsoSerraPara++;
            I_rung_top = 1;
        }
    } else {
        U_TPulsoSerraPara = 0;
    }
    
    U12 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 81 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RErroCorteTubo) {
        I_rung_top = 0;
    }
    
    if (U_RCortarTubo) {
        I_rung_top = 0;
    }
    
    if (U_RMaqProduzindo) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_RErroCorteTubo = 0;
    }
    
    /* ] finish series */
    
    /* start rung 83 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0016 = 0;
    I_parThis_0016 = I_rung_top;
    /* start series [ */
    if (!U_RModoAuto) {
        I_parThis_0016 = 0;
    }
    
    if (!U_RMaqIniciarPosic) {
        I_parThis_0016 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0016) {
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0017 = 0;
    I_parThis_0017 = I_parThis_0016;
    if (!U_RAvancarPerfMan) {
        I_parThis_0017 = 0;
    }
    
    if (I_parThis_0017) {
        I_parOut_0017 = 1;
    }
    I_parThis_0017 = I_parThis_0016;
    if (!U_RRecuarPerfMan) {
        I_parThis_0017 = 0;
    }
    
    if (I_parThis_0017) {
        I_parOut_0017 = 1;
    }
    I_parThis_0016 = I_parOut_0017;
    /* ] finish parallel */
    if (!I_TMantemFreioLib_antiglitch) {
        U_TMantemFreioLib = 199;
    }
    I_TMantemFreioLib_antiglitch = 1;
    if (!I_parThis_0016) {
        if (U_TMantemFreioLib < 199) {
            U_TMantemFreioLib++;
            I_parThis_0016 = 1;
        }
    } else {
        U_TMantemFreioLib = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0016) {
        I_parOut_0016 = 1;
    }
    I_rung_top = I_parOut_0016;
    /* ] finish parallel */
    if (!U_RMaqOK) {
        I_rung_top = 0;
    }
    
    if (U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    U6 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 84 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U6) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TEsperaFreioLib < 199) {
            U_TEsperaFreioLib++;
            I_rung_top = 0;
        }
    } else {
        U_TEsperaFreioLib = 0;
    }
    
    U_RInvFreioLib = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 85 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RInvFreioLib) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0018 = 0;
    I_parThis_0018 = I_rung_top;
    if (!U_RMaqIniciarPosic) {
        I_parThis_0018 = 0;
    }
    
    if (I_parThis_0018) {
        I_parOut_0018 = 1;
    }
    I_parThis_0018 = I_rung_top;
    if (!U_RAvancarPerfMan) {
        I_parThis_0018 = 0;
    }
    
    if (I_parThis_0018) {
        I_parOut_0018 = 1;
    }
    I_rung_top = I_parOut_0018;
    /* ] finish parallel */
    U7 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 86 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RInvFreioLib) {
        I_rung_top = 0;
    }
    
    if (!U_RRecuarPerfMan) {
        I_rung_top = 0;
    }
    
    if (U7) {
        I_rung_top = 0;
    }
    
    U8 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 88 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {
        I_rung_top = 0;
    }
    
    if (U_RModoAuto) {
        I_rung_top = 0;
    }
    
    if (!U_RFerramCorteCima) {
        I_rung_top = 0;
    }
    
    if (U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    if (!U_RMaqConfigOK) {
        I_rung_top = 0;
    }
    
    U_RPerfManualOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 89 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPerfManualOK) {
        I_rung_top = 0;
    }
    
    if (!I13) {
        I_rung_top = 0;
    }
    
    if (I14) {
        I_rung_top = 0;
    }
    
    U_RAvancarPerfMan = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 90 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPerfManualOK) {
        I_rung_top = 0;
    }
    
    if (I13) {
        I_rung_top = 0;
    }
    
    if (!I14) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0019 = 0;
    I_parThis_0019 = I_rung_top;
    /* start series [ */
    if (U_RBloqueiaRecuo) {
        I_parThis_0019 = 0;
    }
    
    U_RRecuarPerfMan = I_parThis_0019;
    
    /* ] finish series */
    if (I_parThis_0019) {
        I_parOut_0019 = 1;
    }
    I_parThis_0019 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_001a = 0;
    I_parThis_001a = I_parThis_0019;
    if (!U_RInvFreioLib) {
        I_parThis_001a = 0;
    }
    
    if (I_parThis_001a) {
        I_parOut_001a = 1;
    }
    I_parThis_001a = I_parThis_0019;
    if (!U_RBloqueiaRecuo) {
        I_parThis_001a = 0;
    }
    
    if (I_parThis_001a) {
        I_parOut_001a = 1;
    }
    I_parThis_0019 = I_parOut_001a;
    /* ] finish parallel */
    if (I_parThis_0019) {
        if (U_TMaxPerfRecua < 299) {
            U_TMaxPerfRecua++;
            I_parThis_0019 = 0;
        }
    } else {
        U_TMaxPerfRecua = 0;
    }
    
    U_RBloqueiaRecuo = I_parThis_0019;
    
    /* ] finish series */
    if (I_parThis_0019) {
        I_parOut_0019 = 1;
    }
    I_rung_top = I_parOut_0019;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 92 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartConfigAuto) {
        I_rung_top = 0;
    }
    
    if (U_RMaqConfigErro) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_001b = 0;
    I_parThis_001b = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_001b;
    if (I_oneShot_0002) {
        I_parThis_001b = 0;
    }
    I_oneShot_0002 = I_scratch;
    
    if (I_parThis_001b) {
        U_RMaqConfigOK = 0;
    }
    
    /* ] finish series */
    if (I_parThis_001b) {
        I_parOut_001b = 1;
    }
    I_parThis_001b = I_rung_top;
    /* start series [ */
    if (I_parThis_001b) {
        if (U_TConfigAutoTO < 199) {
            U_TConfigAutoTO++;
            I_parThis_001b = 0;
        }
    } else {
        U_TConfigAutoTO = 0;
    }
    
    if (I_parThis_001b) {
        U_RMaqConfigErro = 1;
    }
    
    /* ] finish series */
    if (I_parThis_001b) {
        I_parOut_001b = 1;
    }
    I_parThis_001b = I_rung_top;
    /* start series [ */
    if (I_parThis_001b) {
        if (!I_oneShot_0003) {
            if (I_USSReady) {
                uss_set_param(0, 0, 0, &U_WInvAutoVel);
                I_oneShot_0003 = I_parThis_001b;
            }
            I_parThis_001b = 0;
            I_oneShot_0004 = I_parThis_001b;
        }
        if (!I_oneShot_0004) {
            if (I_USSReady) {
                I_oneShot_0004 = 1;
            } else {
                I_parThis_001b = 0;
            }
        }
    } else {
        I_oneShot_0003 = I_parThis_001b;
    }
    
    if (I_parThis_001b) {
        if (!I_oneShot_0005) {
            if (I_USSReady) {
                uss_set_param(0, 0, 0, &U_WInvAutoAcel);
                I_oneShot_0005 = I_parThis_001b;
            }
            I_parThis_001b = 0;
            I_oneShot_0006 = I_parThis_001b;
        }
        if (!I_oneShot_0006) {
            if (I_USSReady) {
                I_oneShot_0006 = 1;
            } else {
                I_parThis_001b = 0;
            }
        }
    } else {
        I_oneShot_0005 = I_parThis_001b;
    }
    
    if (I_parThis_001b) {
        if (!I_oneShot_0007) {
            if (I_USSReady) {
                uss_set_param(0, 0, 0, &U_WInvAutoDesacel);
                I_oneShot_0007 = I_parThis_001b;
            }
            I_parThis_001b = 0;
            I_oneShot_0008 = I_parThis_001b;
        }
        if (!I_oneShot_0008) {
            if (I_USSReady) {
                I_oneShot_0008 = 1;
            } else {
                I_parThis_001b = 0;
            }
        }
    } else {
        I_oneShot_0007 = I_parThis_001b;
    }
    
    if (I_parThis_001b) {
        if (!I_oneShot_0009) {
            if (I_USSReady) {
                uss_set_param(0, 0, 0, &U_WInvTam);
                I_oneShot_0009 = I_parThis_001b;
            }
            I_parThis_001b = 0;
            I_oneShot_000a = I_parThis_001b;
        }
        if (!I_oneShot_000a) {
            if (I_USSReady) {
                I_oneShot_000a = 1;
            } else {
                I_parThis_001b = 0;
            }
        }
    } else {
        I_oneShot_0009 = I_parThis_001b;
    }
    
    if (I_parThis_001b) {
        if (!I_oneShot_000b) {
            if (I_USSReady) {
                uss_set_param(0, 0, 0, &U_WMbPerfFator);
                I_oneShot_000b = I_parThis_001b;
            }
            I_parThis_001b = 0;
            I_oneShot_000c = I_parThis_001b;
        }
        if (!I_oneShot_000c) {
            if (I_USSReady) {
                I_oneShot_000c = 1;
            } else {
                I_parThis_001b = 0;
            }
        }
    } else {
        I_oneShot_000b = I_parThis_001b;
    }
    
    /* start parallel [ */
    I_parOut_001c = 0;
    I_parThis_001c = I_parThis_001b;
    if (I_parThis_001c) {
        U_RStartConfigAuto = 0;
    }
    
    if (I_parThis_001c) {
        I_parOut_001c = 1;
    }
    I_parThis_001c = I_parThis_001b;
    if (I_parThis_001c) {
        U_RMaqConfigOK = 1;
    }
    
    if (I_parThis_001c) {
        I_parOut_001c = 1;
    }
    I_parThis_001b = I_parOut_001c;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_001b) {
        I_parOut_001b = 1;
    }
    I_rung_top = I_parOut_001b;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 93 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartConfigMan) {
        I_rung_top = 0;
    }
    
    if (U_RMaqConfigErro) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_001d = 0;
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_001d;
    if (I_oneShot_000d) {
        I_parThis_001d = 0;
    }
    I_oneShot_000d = I_scratch;
    
    if (I_parThis_001d) {
        U_RMaqConfigOK = 0;
    }
    
    /* ] finish series */
    if (I_parThis_001d) {
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    if (I_parThis_001d) {
        if (U_TConfigManTO < 199) {
            U_TConfigManTO++;
            I_parThis_001d = 0;
        }
    } else {
        U_TConfigManTO = 0;
    }
    
    if (I_parThis_001d) {
        U_RMaqConfigErro = 1;
    }
    
    /* ] finish series */
    if (I_parThis_001d) {
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    if (I_parThis_001d) {
        if (!I_oneShot_000e) {
            if (I_USSReady) {
                uss_set_param(1, 2, 4, &U_WInvModoMan);
                I_oneShot_000e = I_parThis_001d;
            }
            I_parThis_001d = 0;
            I_oneShot_000f = I_parThis_001d;
        }
        if (!I_oneShot_000f) {
            if (I_USSReady) {
                I_oneShot_000f = 1;
            } else {
                I_parThis_001d = 0;
            }
        }
    } else {
        I_oneShot_000e = I_parThis_001d;
    }
    
    if (I_parThis_001d) {
        if (!I_oneShot_0010) {
            if (I_USSReady) {
                uss_set_param(0, 0, 0, &U_WInvManVel);
                I_oneShot_0010 = I_parThis_001d;
            }
            I_parThis_001d = 0;
            I_oneShot_0011 = I_parThis_001d;
        }
        if (!I_oneShot_0011) {
            if (I_USSReady) {
                I_oneShot_0011 = 1;
            } else {
                I_parThis_001d = 0;
            }
        }
    } else {
        I_oneShot_0010 = I_parThis_001d;
    }
    
    if (I_parThis_001d) {
        if (!I_oneShot_0012) {
            if (I_USSReady) {
                uss_set_param(0, 0, 0, &U_WInvManAcel);
                I_oneShot_0012 = I_parThis_001d;
            }
            I_parThis_001d = 0;
            I_oneShot_0013 = I_parThis_001d;
        }
        if (!I_oneShot_0013) {
            if (I_USSReady) {
                I_oneShot_0013 = 1;
            } else {
                I_parThis_001d = 0;
            }
        }
    } else {
        I_oneShot_0012 = I_parThis_001d;
    }
    
    if (I_parThis_001d) {
        if (!I_oneShot_0014) {
            if (I_USSReady) {
                uss_set_param(0, 0, 0, &U_WInvManDesacel);
                I_oneShot_0014 = I_parThis_001d;
            }
            I_parThis_001d = 0;
            I_oneShot_0015 = I_parThis_001d;
        }
        if (!I_oneShot_0015) {
            if (I_USSReady) {
                I_oneShot_0015 = 1;
            } else {
                I_parThis_001d = 0;
            }
        }
    } else {
        I_oneShot_0014 = I_parThis_001d;
    }
    
    /* start parallel [ */
    I_parOut_001e = 0;
    I_parThis_001e = I_parThis_001d;
    if (I_parThis_001e) {
        U_RStartConfigMan = 0;
    }
    
    if (I_parThis_001e) {
        I_parOut_001e = 1;
    }
    I_parThis_001e = I_parThis_001d;
    if (I_parThis_001e) {
        U_RMaqConfigOK = 1;
    }
    
    if (I_parThis_001e) {
        I_parOut_001e = 1;
    }
    I_parThis_001d = I_parOut_001e;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_001d) {
        I_parOut_001d = 1;
    }
    I_rung_top = I_parOut_001d;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 95 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {
        I_rung_top = 0;
    }
    
    if (!U_RModoAuto) {
        I_rung_top = 0;
    }
    
    U_RModoAutoOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 96 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_001f = 0;
    I_parThis_001f = I_rung_top;
    /* start series [ */
    if (!U_RModoAuto) {
        I_parThis_001f = 0;
    }
    
    I_scratch = I_parThis_001f;
    if (!I_parThis_001f) {
        if (I_oneShot_0016) {
            I_parThis_001f = 1;
        }
    } else {
        I_parThis_001f = 0;
    }
    I_oneShot_0016 = I_scratch;
    
    /* ] finish series */
    if (I_parThis_001f) {
        I_parOut_001f = 1;
    }
    I_parThis_001f = I_rung_top;
    I_scratch = I_parThis_001f;
    if (I_oneShot_0017) {
        I_parThis_001f = 0;
    }
    I_oneShot_0017 = I_scratch;
    
    if (I_parThis_001f) {
        I_parOut_001f = 1;
    }
    I_rung_top = I_parOut_001f;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0020 = 0;
    I_parThis_0020 = I_rung_top;
    if (I_parThis_0020) {
        U_RStartConfigMan = 1;
    }
    
    if (I_parThis_0020) {
        I_parOut_0020 = 1;
    }
    I_parThis_0020 = I_rung_top;
    if (I_parThis_0020) {
        U_RStartConfigAuto = 0;
    }
    
    if (I_parThis_0020) {
        I_parOut_0020 = 1;
    }
    I_rung_top = I_parOut_0020;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 97 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RModoAuto) {
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0018) {
        I_rung_top = 0;
    }
    I_oneShot_0018 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0021 = 0;
    I_parThis_0021 = I_rung_top;
    if (I_parThis_0021) {
        U_RStartConfigAuto = 1;
    }
    
    if (I_parThis_0021) {
        I_parOut_0021 = 1;
    }
    I_parThis_0021 = I_rung_top;
    if (I_parThis_0021) {
        U_RStartConfigMan = 0;
    }
    
    if (I_parThis_0021) {
        I_parOut_0021 = 1;
    }
    I_parThis_0021 = I_rung_top;
    if (I_parThis_0021) {
        U_CQtdProd = M[30];
    }
    
    if (I_parThis_0021) {
        I_parOut_0021 = 1;
    }
    I_rung_top = I_parOut_0021;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 98 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqConfigOK) {
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0019) {
        I_rung_top = 0;
    }
    I_oneShot_0019 = I_scratch;
    
    if (!U_RModoAutoOK) {
        I_rung_top = 0;
    }
    
    if (!I5) {
        I_rung_top = 0;
    }
    
    I_scratch2 = 0;
    if (U_CQtdProd > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0022 = 0;
    I_parThis_0022 = I_rung_top;
    if (I_parThis_0022) {
        U_RMaqProduzindo = 1;
    }
    
    if (I_parThis_0022) {
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (I_parThis_0022) {
        U9 = 1;
    }
    
    if (I_parThis_0022) {
        I_parOut_0022 = 1;
    }
    I_rung_top = I_parOut_0022;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 99 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I10) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TFimPosicEstab < 99) {
            U_TFimPosicEstab++;
            I_rung_top = 0;
        }
    } else {
        U_TFimPosicEstab = 0;
    }
    
    /* start parallel [ */
    I_parOut_0023 = 0;
    I_parThis_0023 = I_rung_top;
    /* start series [ */
    if (!U_RMaqIniciarPosic) {
        I_parThis_0023 = 0;
    }
    
    if (I_parThis_0023) {
        U_RMaqFimPosic = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0023) {
        I_parOut_0023 = 1;
    }
    I_parThis_0023 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0023;
    if (!I_parThis_0023) {
        if (I_oneShot_001a) {
            I_parThis_0023 = 1;
        }
    } else {
        I_parThis_0023 = 0;
    }
    I_oneShot_001a = I_scratch;
    
    if (I_parThis_0023) {
        U_RMaqFimPosic = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0023) {
        I_parOut_0023 = 1;
    }
    I_rung_top = I_parOut_0023;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 100 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0024 = 0;
    I_parThis_0024 = I_rung_top;
    if (U_RModoAutoOK) {
        I_parThis_0024 = 0;
    }
    
    if (I_parThis_0024) {
        I_parOut_0024 = 1;
    }
    I_parThis_0024 = I_rung_top;
    if (!U_RErroCorteTubo) {
        I_parThis_0024 = 0;
    }
    
    if (I_parThis_0024) {
        I_parOut_0024 = 1;
    }
    I_rung_top = I_parOut_0024;
    /* ] finish parallel */
    if (!U_RMaqProduzindo) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_RMaqProduzindo = 0;
    }
    
    /* ] finish series */
    
    /* start rung 101 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqProduzindo) {
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {
        if (I_oneShot_001b) {
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_001b = I_scratch;
    
    /* start parallel [ */
    I_parOut_0025 = 0;
    I_parThis_0025 = I_rung_top;
    if (I_parThis_0025) {
        U_RCortarTuboAuto = 0;
    }
    
    if (I_parThis_0025) {
        I_parOut_0025 = 1;
    }
    I_parThis_0025 = I_rung_top;
    if (I_parThis_0025) {
        U10 = 0;
    }
    
    if (I_parThis_0025) {
        I_parOut_0025 = 1;
    }
    I_parThis_0025 = I_rung_top;
    if (I_parThis_0025) {
        U9 = 0;
    }
    
    if (I_parThis_0025) {
        I_parOut_0025 = 1;
    }
    I_rung_top = I_parOut_0025;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 103 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqProduzindo) {
        I_rung_top = 0;
    }
    
    if (U_RMaqFimPosic) {
        I_rung_top = 0;
    }
    
    if (U10) {
        I_rung_top = 0;
    }
    
    U_RMaqIniciarPosic = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 104 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqProduzindo) {
        I_rung_top = 0;
    }
    
    if (!U_RMaqFimPosic) {
        I_rung_top = 0;
    }
    
    if (U6) {
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_001c) {
        I_rung_top = 0;
    }
    I_oneShot_001c = I_scratch;
    
    if (I_rung_top) {
        U_RCortarTuboAuto = 1;
    }
    
    /* ] finish series */
    
    /* start rung 105 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqProduzindo) {
        I_rung_top = 0;
    }
    
    if (!U_RMaqFimPosic) {
        I_rung_top = 0;
    }
    
    if (!U_RCorteTuboAtivo) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0026 = 0;
    I_parThis_0026 = I_rung_top;
    if (I_parThis_0026) {
        U_RCortarTuboAuto = 0;
    }
    
    if (I_parThis_0026) {
        I_parOut_0026 = 1;
    }
    I_parThis_0026 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0026;
    if (!I_parThis_0026) {
        if (I_oneShot_001d) {
            I_parThis_0026 = 1;
        }
    } else {
        I_parThis_0026 = 0;
    }
    I_oneShot_001d = I_scratch;
    
    if (I_parThis_0026) {
        U10 = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0026) {
        I_parOut_0026 = 1;
    }
    I_rung_top = I_parOut_0026;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 106 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqProduzindo) {
        I_rung_top = 0;
    }
    
    if (!U10) {
        I_rung_top = 0;
    }
    
    if (U_RMaqFimPosic) {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0027 = 0;
    I_parThis_0027 = I_rung_top;
    if (I_parThis_0027) {
        U10 = 0;
    }
    
    if (I_parThis_0027) {
        I_parOut_0027 = 1;
    }
    I_parThis_0027 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0028 = 0;
    I_parThis_0028 = I_parThis_0027;
    /* start series [ */
    if (I_parThis_0028) {
        if (!I_oneShot_001e) {
            I_scratch = 1;
            U_CQtdProd = U_CQtdProd - I_scratch;
        }
    }
    I_oneShot_001e = I_parThis_0028;
    if (U_CQtdProd < 1) {
        I_parThis_0028 = 0;
    } else {
        I_parThis_0028 = 1;
    }
    
    I_scratch = I_parThis_0028;
    if (!I_parThis_0028) {
        if (I_oneShot_001f) {
            I_parThis_0028 = 1;
        }
    } else {
        I_parThis_0028 = 0;
    }
    I_oneShot_001f = I_scratch;
    
    /* ] finish series */
    if (I_parThis_0028) {
        I_parOut_0028 = 1;
    }
    I_parThis_0028 = I_parThis_0027;
    if (I5) {
        I_parThis_0028 = 0;
    }
    
    if (I_parThis_0028) {
        I_parOut_0028 = 1;
    }
    I_parThis_0028 = I_parThis_0027;
    if (U_RModoAuto) {
        I_parThis_0028 = 0;
    }
    
    if (I_parThis_0028) {
        I_parOut_0028 = 1;
    }
    I_parThis_0027 = I_parOut_0028;
    /* ] finish parallel */
    if (I_parThis_0027) {
        U_RMaqProduzindo = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0027) {
        I_parOut_0027 = 1;
    }
    I_rung_top = I_parOut_0027;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 108 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(M[3] & (1 << 0))) {  // XMbModoAuto
        I_rung_top = 0;
    }
    
    U_RModoAuto = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 109 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(M[27] & (1 << 0))) {  // YMbTipoCorte
        I_rung_top = 0;
    }
    
    U_RModoCorteSerra = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 110 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(M[1] & (1 << 0))) {  // YMbLimpaErro
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0029 = 0;
    I_parThis_0029 = I_rung_top;
    if (I_parThis_0029) {
        U_RMaqConfigErro = 0;
    }
    
    if (I_parThis_0029) {
        I_parOut_0029 = 1;
    }
    I_parThis_0029 = I_rung_top;
    if (I_parThis_0029) {
        M[1] &= ~(1 << 0);  // YMbLimpaErro
    }
    
    if (I_parThis_0029) {
        I_parOut_0029 = 1;
    }
    I_rung_top = I_parOut_0029;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 111 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I11) {
        I_rung_top = 0;
    }
    
    M[0] &= ~(1 << 0); M[0] |= I_rung_top << 0;  // YMbErrEmergencia
    
    /* ] finish series */
    
    /* start rung 112 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I18) {
        I_rung_top = 0;
    }
    
    M[0] &= ~(1 << 1); M[0] |= I_rung_top << 1;  // YMbErrFaltaFase
    
    /* ] finish series */
    
    /* start rung 113 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I2) {
        I_rung_top = 0;
    }
    
    M[0] &= ~(1 << 2); M[0] |= I_rung_top << 2;  // YMbErrBombaHidr
    
    /* ] finish series */
    
    /* start rung 114 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I3) {
        I_rung_top = 0;
    }
    
    M[0] &= ~(1 << 3); M[0] |= I_rung_top << 3;  // YMbErrInversor
    
    /* ] finish series */
    
    /* start rung 115 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I12) {
        I_rung_top = 0;
    }
    
    M[0] &= ~(1 << 4); M[0] |= I_rung_top << 4;  // YMbErrDesbob
    
    /* ] finish series */
    
    /* start rung 116 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqConfigErro) {
        I_rung_top = 0;
    }
    
    M[0] &= ~(1 << 5); M[0] |= I_rung_top << 5;  // YMbErrInvComunic
    
    /* ] finish series */
    
    /* start rung 118 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_0020) {
        I_rung_top = 0;
    }
    I_oneShot_0020 = I_scratch;
    
    /* start parallel [ */
    I_parOut_002a = 0;
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[10] &= ~(1 << 0);  // YMbPerfFatorL
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[11] &= ~(1 << 0);  // YMbPerfFatorH
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[12] &= ~(1 << 0);  // YMbAutoAcel
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[13] &= ~(1 << 0);  // YMbAutoDesacel
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[14] &= ~(1 << 0);  // YMbAutoVel
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[15] &= ~(1 << 0);  // YMbManAcel
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[16] &= ~(1 << 0);  // YMbManDesacel
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[17] &= ~(1 << 0);  // YMbManVel
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[20] &= ~(1 << 0);  // YMbEncFator
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[21] &= ~(1 << 0);  // YMbEncResol
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[22] &= ~(1 << 0);  // YMbEncPerim
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[25] &= ~(1 << 0);  // YMbFacaTam
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (I_parThis_002a) {
        M[26] &= ~(1 << 0);  // YMbSerraTam
    }
    
    if (I_parThis_002a) {
        I_parOut_002a = 1;
    }
    I_rung_top = I_parOut_002a;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 119 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_0021) {
        I_rung_top = 0;
    }
    I_oneShot_0021 = I_scratch;
    
    /* start parallel [ */
    I_parOut_002b = 0;
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[31] &= ~(1 << 0);  // YMbTam
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[30] &= ~(1 << 0);  // YMbQtd
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[10] = 5;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[11] = 8;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[31] = 0;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[14] = 0;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[17] = 0;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[25] = 0;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        M[26] = 0;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        U_WInvModoAuto = 1;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (I_parThis_002b) {
        U_WInvModoMan = 0;
    }
    
    if (I_parThis_002b) {
        I_parOut_002b = 1;
    }
    I_rung_top = I_parOut_002b;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 122 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 65536;
        U_WMbPerfFator = M[11] * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 123 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        U_WMbPerfFator = U_WMbPerfFator + M[10];
    }
    
    /* ] finish series */
    
    /* start rung 125 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RModoCorteSerra) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_WInvTam = M[31] + M[26];
    }
    
    /* ] finish series */
    
    /* start rung 126 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RModoCorteSerra) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_WInvTam = M[31] + M[25];
    }
    
    /* ] finish series */
    
    /* start rung 127 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        U_WInvTam = U_WInvTam * M[20];
    }
    
    /* ] finish series */
    
    /* start rung 128 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 1000;
        U_WInvTam = U_WInvTam / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 129 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        U_WInvTam = U_WInvTam * U_WMbPerfFator;
    }
    
    /* ] finish series */
    
    /* start rung 131 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 60;
        U_WInvAutoVel = M[14] * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 132 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 10;
        U_WInvAutoVel = U_WInvAutoVel / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 133 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 60;
        U_WInvManVel = M[17] * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 134 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 10;
        U_WInvManVel = U_WInvManVel / I_scratch2;
    }
    
    /* ] finish series */
}
