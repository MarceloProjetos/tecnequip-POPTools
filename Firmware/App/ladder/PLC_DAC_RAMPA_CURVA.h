// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;
volatile int U_TON = 0;
volatile int I_TOFF_antiglitch = 0;
volatile int U_TOFF = 0;
volatile int GPIO_INPUT_PORT1 = 0;
volatile int I_oneShot_0000 = 0;
volatile int I_oneShot_0001 = 0;
volatile int I_oneShot_0002 = 0;
volatile int I_oneShot_0003 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_OUTPUT_PORT1) {  // YLED
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
    
    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // XE1
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT1) {  // YLED
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0000) {  // $oneShot_0000
            I_oneShot_0000 = I_rung_top;
            DAC_StartUp(600, 2047);
        }
    } else {
        I_oneShot_0000 = I_rung_top;
    }
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // XE1
        I_rung_top = 0;
    }

    if (!GPIO_OUTPUT_PORT1) {  // YLED
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0001) {  // $oneShot_0001
            I_oneShot_0001 = I_rung_top;
            DAC_StartDown(600, 2047);
        }
    } else {
        I_oneShot_0001 = I_rung_top;
    }
    
    /* ] finish series */

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT1) {  // XE1
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT1) {  // YLED
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0002) {  // $oneShot_0002
            I_oneShot_0002 = I_rung_top;
            DAC_StartUp(600, -2048);
        }
    } else {
        I_oneShot_0002 = I_rung_top;
    }

    /* ] finish series */

    /* start rung 5 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT1) {  // XE1
        I_rung_top = 0;
    }

    if (!GPIO_OUTPUT_PORT1) {  // YLED
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0003) {  // $oneShot_0003
            I_oneShot_0003 = I_rung_top;
            DAC_StartDown(600, -2048);
        }
    } else {
        I_oneShot_0003 = I_rung_top;
    }

    /* ] finish series */
}

