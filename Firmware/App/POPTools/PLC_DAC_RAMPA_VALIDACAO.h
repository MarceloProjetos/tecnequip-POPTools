// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char GPIO_INPUT_PORT1 = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile unsigned char I_oneShot_0002 = 0;
volatile int U_Etapa = 0;
volatile unsigned char I_oneShot_0003 = 0;
volatile int I_scratch2 = 0;
volatile unsigned char I_oneShot_0004 = 0;
volatile int U_EspEtapa1 = 0;
volatile unsigned char I_oneShot_0005 = 0;
volatile int U_EspEtapa2 = 0;
volatile unsigned char I_oneShot_0006 = 0;
volatile int U_EspEtapa3 = 0;
volatile unsigned char I_oneShot_0007 = 0;
volatile int U_EspEtapa4 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT1) {  // DbgStart
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0000;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_parThis_0000 = 0;
    }
    I_oneShot_0000 = I_scratch;

    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[0] |= (1 << 0);  // Start
    }

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0000;
    if (!I_parThis_0000) {  // $parThis_0000
        if (I_oneShot_0001) {  // $oneShot_0001
            I_parThis_0000 = 1;
        }
    } else {
        I_parThis_0000 = 0;
    }
    I_oneShot_0001 = I_scratch;

    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[0] &= ~(1 << 0);  // Start
    }

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // Start
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0000;
    if (I_oneShot_0002) {  // $oneShot_0002
        I_parThis_0000 = 0;
    }
    I_oneShot_0002 = I_scratch;

    if (I_parThis_0000) {  // $parThis_0000
        U_Etapa = 1;
    }

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0000;
    if (!I_parThis_0000) {  // $parThis_0000
        if (I_oneShot_0003) {  // $oneShot_0003
            I_parThis_0000 = 1;
        }
    } else {
        I_parThis_0000 = 0;
    }
    I_oneShot_0003 = I_scratch;

    if (I_parThis_0000) {  // $parThis_0000
        U_Etapa = 0;
    }

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch2 = 1;
    if (U_Etapa == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        if (!I_oneShot_0004) {  // $oneShot_0004
            I_oneShot_0004 = I_parThis_0000;
            DAC_Start(1, 1, 10, 5, 600, 819);
        }
    } else {
        I_oneShot_0004 = I_parThis_0000;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    if (I_parThis_0000) {  // $parThis_0000
        if (U_EspEtapa1 < 109) {
            U_EspEtapa1++;
            I_parThis_0000 = 0;
        }
    } else {
        U_EspEtapa1 = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        U_Etapa = 2;
    }

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch2 = 2;
    if (U_Etapa == I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        if (!I_oneShot_0005) {  // $oneShot_0005
            I_oneShot_0005 = I_parThis_0000;
            DAC_Start(2, 1, 10, 5, 1600, -819);
        }
    } else {
        I_oneShot_0005 = I_parThis_0000;
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    if (I_parThis_0000) {  // $parThis_0000
        if (U_EspEtapa2 < 159) {
            U_EspEtapa2++;
            I_parThis_0000 = 0;
        }
    } else {
        U_EspEtapa2 = 0;
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        U_Etapa = 3;
    }
    
    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 5 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 3;
    if (U_Etapa == I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        if (!I_oneShot_0006) {  // $oneShot_0006
            I_oneShot_0006 = I_parThis_0000;
            DAC_Start(2, 1, 10, 5, 900, -2048);
        }
    } else {
        I_oneShot_0006 = I_parThis_0000;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    if (I_parThis_0000) {  // $parThis_0000
        if (U_EspEtapa3 < 109) {
            U_EspEtapa3++;
            I_parThis_0000 = 0;
        }
    } else {
        U_EspEtapa3 = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        U_Etapa = 4;
    }
    
    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 4;
    if (U_Etapa == I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        if (!I_oneShot_0007) {  // $oneShot_0007
            I_oneShot_0007 = I_parThis_0000;
            DAC_Start(2, 1, 10, 5, 630, 204);
        }
    } else {
        I_oneShot_0007 = I_parThis_0000;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    if (I_parThis_0000) {  // $parThis_0000
        if (U_EspEtapa4 < 95) {
            U_EspEtapa4++;
            I_parThis_0000 = 0;
        }
    } else {
        U_EspEtapa4 = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        U_Etapa = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */
}
