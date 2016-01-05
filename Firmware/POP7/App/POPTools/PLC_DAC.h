// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;
volatile int U_TLEDON = 0;
volatile int I_TLEDOF_antiglitch = 0;
volatile int U_TLEDOF = 0;
volatile int I_scratch2 = 0;
volatile int U_DA = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_OUTPUT_PORT1) {  // YS1
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TLEDON < 9) {
            U_TLEDON++;
            I_rung_top = 0;
        }
    } else {
        U_TLEDON = 0;
    }
    
    if (!I_TLEDOF_antiglitch) {  // $TLEDOF_antiglitch
        U_TLEDOF = 9;
    }
    I_TLEDOF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TLEDOF < 9) {
            U_TLEDOF++;
            I_rung_top = 1;
        }
    } else {
        U_TLEDOF = 0;
    }
    
    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_OUTPUT_PORT1) {  // YS1
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 100;
        U_DA = U_DA + I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 4095;
    if (U_DA > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_DA = 0;
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        DAC_Write(U_DA);
    }
    
    /* ] finish series */
}
