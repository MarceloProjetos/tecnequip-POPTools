// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile int U_var = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile int U_on = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0002 = 0;
volatile unsigned char I_parOut_0001 = 0;
volatile unsigned char I_parThis_0001 = 0;
volatile unsigned char I_off_antiglitch = 0;
volatile int U_off = 0;
volatile unsigned char I_oneShot_0003 = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;
volatile unsigned char GPIO_OUTPUT_PORT5 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_var = 1234;
    }

    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_OUTPUT_PORT1) {  // s1
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

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (!I_SerialAborted) {  // $SerialAborted
        I_parThis_0000 = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        if (!I_oneShot_0000) {  // $oneShot_0000
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &U_var);
                I_oneShot_0000 = I_parThis_0000;
            }
            I_parThis_0000 = 0;
            I_oneShot_0001 = I_parThis_0000;
        }
        if (!I_oneShot_0001) {  // $oneShot_0001
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0001 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &U_var);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0000 = 0;
            }
        }
    } else {
        if (I_oneShot_0000) {  // $oneShot_0000
            I_SerialReady = 1;
        }
        I_oneShot_0000 = I_parThis_0000;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    I_scratch = I_rung_top;
    if (I_oneShot_0002) {  // $oneShot_0002
        I_rung_top = 0;
    }
    I_oneShot_0002 = I_scratch;

    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        GPIO_OUTPUT_PORT1 = 1;
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    /* start series [ */
    if (!I_off_antiglitch) {  // $off_antiglitch
        U_off = 99;
    }
    I_off_antiglitch = 1;
    if (!I_parThis_0001) {  // $parThis_0001
        if (U_off < 99) {
            U_off++;
            I_parThis_0001 = 1;
        }
    } else {
        U_off = 0;
    }

    I_scratch = I_parThis_0001;
    if (!I_parThis_0001) {  // $parThis_0001
        if (I_oneShot_0003) {  // $oneShot_0003
            I_parThis_0001 = 1;
        }
    } else {
        I_parThis_0001 = 0;
    }
    I_oneShot_0003 = I_scratch;

    if (I_parThis_0001) {  // $parThis_0001
        GPIO_OUTPUT_PORT1 = 0;
    }

    /* ] finish series */
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!I_SerialReady) {  // $SerialReady
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT3 = I_rung_top;

    /* ] finish series */

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!I_SerialTimeout) {  // $SerialTimeout
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT4 = I_rung_top;

    /* ] finish series */

    /* start rung 5 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!I_SerialAborted) {  // $SerialAborted
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT5 = I_rung_top;

    /* ] finish series */
}

