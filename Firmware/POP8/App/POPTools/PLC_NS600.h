// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile int U_EsperaEnviar = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile int VAR_NULL = 0;
volatile unsigned char I_oneShot_0002 = 0;
volatile int U_Espera = 0;
volatile unsigned char I_oneShot_0003 = 0;
volatile unsigned char I_oneShot_0004 = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile int I_scratch2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;

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

    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[1] &= ~(1 << 0);  // MbStatus
    }

    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[0] & (1 << 0)) {  // MBEnviar
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_EsperaEnviar < 299) {
            U_EsperaEnviar++;
            I_rung_top = 0;
        }
    } else {
        U_EsperaEnviar = 0;
    }

    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[0] |= (1 << 0);  // MBEnviar
    }

    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // MBenviar
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0001) {  // $oneShot_0001
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("0", "SPD5934", &VAR_NULL);
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
        if (U_Espera < 9) {
            U_Espera++;
            I_rung_top = 0;
        }
    } else {
        U_Espera = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0003) {  // $oneShot_0003
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Read("0", "OK", &MODBUS_REGISTER[1]);
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

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[0] &= ~(1 << 0);  // MBEnviar
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch2 = 0;
    if (I_scratch2 > MODBUS_REGISTER[1]) {
    } else {
        I_parThis_0000 = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_scratch2 = -1;
        MODBUS_REGISTER[1] = MODBUS_REGISTER[1] * I_scratch2;
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
    if (MODBUS_REGISTER[0] & (1 << 0)) {  // MBEnviar
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // MbStatus
            I_rung_top = 0;
        }
    }

    GPIO_OUTPUT_PORT1 = I_rung_top;

    /* ] finish series */

    /* start rung 5 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[0] & (1 << 0)) {  // MBEnviar
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!(MODBUS_REGISTER[1] & (1 << 1))) {  // MbStatus
            I_rung_top = 0;
        }
    }

    GPIO_OUTPUT_PORT2 = I_rung_top;

    /* ] finish series */

    /* start rung 6 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[0] & (1 << 0)) {  // MBEnviar
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!(MODBUS_REGISTER[1] & (1 << 2))) {  // MbStatus
            I_rung_top = 0;
        }
    }

    GPIO_OUTPUT_PORT3 = I_rung_top;

    /* ] finish series */

    /* start rung 7 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[0] & (1 << 0)) {  // MBEnviar
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!(MODBUS_REGISTER[1] & (1 << 3))) {  // MbStatus
            I_rung_top = 0;
        }
    }

    GPIO_OUTPUT_PORT4 = I_rung_top;

    /* ] finish series */
}
