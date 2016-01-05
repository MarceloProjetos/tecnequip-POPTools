// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int I_scratch = 0;
volatile int I_oneShot_0000 = 0;
volatile int U_MODBUS = 0;
volatile int GPIO_OUTPUT_PORT18 = 0;
volatile int U_TON = 0;
volatile int I_TOFF_antiglitch = 0;
volatile int U_TOFF = 0;
volatile int I_oneShot_0001 = 0;
volatile int I_oneShot_0002 = 0;
volatile int I_parOut_0000 = 0;
volatile int I_parThis_0000 = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;
volatile int I_oneShot_0003 = 0;
volatile int I_oneShot_0004 = 0;
volatile int I_parOut_0001 = 0;
volatile int I_parThis_0001 = 0;
volatile int GPIO_OUTPUT_PORT2 = 0;

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
        U_MODBUS = 3;
    }

    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_OUTPUT_PORT18) {  // YLED
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TON < 49) {
            U_TON++;
            I_rung_top = 0;
        }
    } else {
        U_TON = 0;
    }

    if (!I_TOFF_antiglitch) {  // $TOFF_antiglitch
        U_TOFF = 49;
    }
    I_TOFF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TOFF < 49) {
            U_TOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TOFF = 0;
    }

    GPIO_OUTPUT_PORT18 = I_rung_top;

    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_OUTPUT_PORT18) {  // YLED
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0001) {  // $oneShot_0001
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &U_MODBUS);
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

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    GPIO_OUTPUT_PORT1 = I_parThis_0000;

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_MODBUS = 1;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT18) {  // YLED
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0003) {  // $oneShot_0003
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &U_MODBUS);
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
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    GPIO_OUTPUT_PORT2 = I_parThis_0001;

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        U_MODBUS = 3;
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    /* ] finish series */
}

