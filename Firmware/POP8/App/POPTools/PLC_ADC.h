// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int U_ROSC = 0;
volatile int U_TTON = 0;
volatile int I_TTOFF_antiglitch = 0;
volatile int U_TTOFF = 0;
volatile int A3 = 0;
volatile int I_oneShot_0000 = 0;
volatile int I_oneShot_0001 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_ROSC) {  // ROSC
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TTON < 99) {
            U_TTON++;
            I_rung_top = 0;
        }
    } else {
        U_TTON = 0;
    }
    
    if (!I_TTOFF_antiglitch) {  // $TTOFF_antiglitch
        U_TTOFF = 99;
    }
    I_TTOFF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TTOFF < 99) {
            U_TTOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TTOFF = 0;
    }
    
    U_ROSC = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        A3 = ADC_Read(6);
    }
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_ROSC) {  // ROSC
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0000) {  // $oneShot_0000
            if (I_SerialReady) {  // $SerialReady
                Format_String_Write("AD: %d\r\n", &A3);
                I_oneShot_0000 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0001 = I_rung_top;
        }
        if (!I_oneShot_0001) {  // $oneShot_0001
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0001 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0000 = I_rung_top;
    }
    
    /* ] finish series */
}
