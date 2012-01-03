// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int GPIO_INPUT_PORT1 = 0;
volatile int I_oneShot_0000 = 0;
volatile int U_Cwhich = 0;
volatile int I_scratch = 0;
volatile int I_oneShot_0001 = 0;
volatile int I_Tdblclk_antiglitch = 0;
volatile int U_Tdblclk = 0;
volatile int U_Rdblclk = 0;
volatile int U_Rfast = 0;
volatile int I_Tffast_antiglitch = 0;
volatile int U_Tffast = 0;
volatile int U_Tnfast = 0;
volatile int U_Rslow = 0;
volatile int U_Tnslow = 0;
volatile int I_Tfslow_antiglitch = 0;
volatile int U_Tfslow = 0;
volatile int I_parOut_0000 = 0;
volatile int I_parThis_0000 = 0;
volatile int I_scratch2 = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // Xbutton
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0000) {  // $oneShot_0000
            U_Cwhich++;
            if (U_Cwhich < 5) {
            } else {
                U_Cwhich = 0;
            }
        }
    }
    I_oneShot_0000 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // Xbutton
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0001) {  // $oneShot_0001
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0001 = I_scratch;
    
    if (!I_Tdblclk_antiglitch) {  // $Tdblclk_antiglitch
        U_Tdblclk = 49;
    }
    I_Tdblclk_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Tdblclk < 49) {
            U_Tdblclk++;
            I_rung_top = 1;
        }
    } else {
        U_Tdblclk = 0;
    }
    
    U_Rdblclk = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // Xbutton
        I_rung_top = 0;
    }
    
    if (!U_Rdblclk) {  // Rdblclk
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_Cwhich = 3;
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_Rfast) {  // Rfast
        I_rung_top = 0;
    }
    
    if (!I_Tffast_antiglitch) {  // $Tffast_antiglitch
        U_Tffast = 9;
    }
    I_Tffast_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Tffast < 9) {
            U_Tffast++;
            I_rung_top = 1;
        }
    } else {
        U_Tffast = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_Tnfast < 9) {
            U_Tnfast++;
            I_rung_top = 0;
        }
    } else {
        U_Tnfast = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_Rfast = 0;
    } else {
        U_Rfast = 1;
    }
    
    /* ] finish series */
    
    /* start rung 5 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_Rslow) {  // Rslow
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_Tnslow < 9) {
            U_Tnslow++;
            I_rung_top = 0;
        }
    } else {
        U_Tnslow = 0;
    }
    
    if (!I_Tfslow_antiglitch) {  // $Tfslow_antiglitch
        U_Tfslow = 99;
    }
    I_Tfslow_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Tfslow < 99) {
            U_Tfslow++;
            I_rung_top = 1;
        }
    } else {
        U_Tfslow = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_Rslow = 0;
    } else {
        U_Rslow = 1;
    }
    
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch2 = 0;
    if (U_Cwhich == I_scratch2) {
    } else {
        I_parThis_0000 = 0;
    }
    
    if (!U_Rfast) {  // Rfast
        I_parThis_0000 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch2 = 1;
    if (U_Cwhich == I_scratch2) {
    } else {
        I_parThis_0000 = 0;
    }
    
    if (!U_Rslow) {  // Rslow
        I_parThis_0000 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch2 = 2;
    if (U_Cwhich == I_scratch2) {
    } else {
        I_parThis_0000 = 0;
    }
    
    if (U_Rslow) {  // Rslow
        I_parThis_0000 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    I_scratch2 = 3;
    if (U_Cwhich == I_scratch2) {
    } else {
        I_parThis_0000 = 0;
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    I_scratch2 = 4;
    if (U_Cwhich == I_scratch2) {
    } else {
        I_parThis_0000 = 0;
    }
    
    I_parThis_0000 = 0;
    
    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */
}
