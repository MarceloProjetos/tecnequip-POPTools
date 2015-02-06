// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile unsigned char U_acelerar = 0;
volatile int U_on = 0;
volatile unsigned char I_off_antiglitch = 0;
volatile int U_off = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile unsigned char I_oneShot_0002 = 0;

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
        MODBUS_REGISTER[0] = 600;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[1] = -10000;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[2] = -10000;
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
    if (U_acelerar) {  // acelerar
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_on < 99) {
            U_on++;
            I_rung_top = 0;
        }
    } else {
        U_on = 0;
    }
    
    if (!I_off_antiglitch) {  // $off_antiglitch
        U_off = 99;
    }
    I_off_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_off < 99) {
            U_off++;
            I_rung_top = 1;
        }
    } else {
        U_off = 0;
    }
    
    U_acelerar = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_acelerar) {  // acelerar
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0001) {  // $oneShot_0001
            I_oneShot_0001 = I_rung_top;
            DAC_Start(2, 0, 10, 5, MODBUS_REGISTER[0], DAC_Conv(MODBUS_REGISTER[1], 1));
        }
    } else {
        I_oneShot_0001 = I_rung_top;
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_acelerar) {  // acelerar
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0002) {  // $oneShot_0002
            I_oneShot_0002 = I_rung_top;
            DAC_Start(2, 1, 10, 5, MODBUS_REGISTER[0], DAC_Conv(MODBUS_REGISTER[2], 1));
        }
    } else {
        I_oneShot_0002 = I_rung_top;
    }
    
    /* ] finish series */
}
