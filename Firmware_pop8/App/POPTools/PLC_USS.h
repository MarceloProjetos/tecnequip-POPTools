// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile int U_EspInit = 0;
volatile unsigned char U_Aguardar = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile unsigned char I_oneShot_0002 = 0;
volatile unsigned char I_oneShot_0003 = 0;
volatile unsigned char I_oneShot_0004 = 0;
volatile int U_ON = 0;
volatile unsigned char I_OFF_antiglitch = 0;
volatile int U_OFF = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_rung_top = 0;
    }
    I_oneShot_0000 = I_scratch;

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[0] &= ~(1 << 0);  // Valor
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[0] = 100;
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        if (U_EspInit < 299) {
            U_EspInit++;
            I_rung_top = 0;
        }
    } else {
        U_EspInit = 0;
    }
    
    if (U_Aguardar) {  // Aguardar
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0001) {  // $oneShot_0001
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 102, 0, 0, &MODBUS_REGISTER[0]);
                I_oneShot_0001 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0002 = I_rung_top;
        }
        if (!I_oneShot_0002) {  // $oneShot_0002
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0002 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0001 = I_rung_top;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0003) {  // $oneShot_0003
            if (I_SerialReady) {  // $SerialReady
                USS_Get_Param(0, 103, 0, 0, &MODBUS_REGISTER[0]);
                I_oneShot_0003 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0004 = I_rung_top;
        }
        if (!I_oneShot_0004) {  // $oneShot_0004
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0004 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0003 = I_rung_top;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_ON < 99) {
            U_ON++;
            I_rung_top = 0;
        }
    } else {
        U_ON = 0;
    }
    
    if (!I_OFF_antiglitch) {  // $OFF_antiglitch
        U_OFF = 99;
    }
    I_OFF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_OFF < 99) {
            U_OFF++;
            I_rung_top = 1;
        }
    } else {
        U_OFF = 0;
    }
    
    U_Aguardar = I_rung_top;
    
    /* ] finish series */
}
