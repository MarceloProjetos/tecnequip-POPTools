// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int GPIO_INPUT_PORT2 = 0;
volatile int GPIO_OUTPUT_PORT11 = 0;
volatile int I_parOut_0000 = 0;
volatile int I_parThis_0000 = 0;
volatile int GPIO_INPUT_PORT3 = 0;
volatile int GPIO_INPUT_PORT4 = 0;
volatile int GPIO_OUTPUT_PORT7 = 0;
volatile int GPIO_INPUT_PORT5 = 0;
volatile int GPIO_INPUT_PORT6 = 0;
volatile int I_parOut_0001 = 0;
volatile int I_parThis_0001 = 0;
volatile int GPIO_OUTPUT_PORT12 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT2) {  // Xa
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT11 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (!GPIO_INPUT_PORT3) {  // Xb
        I_parThis_0000 = 0;
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (!GPIO_INPUT_PORT4) {  // Xc
        I_parThis_0000 = 0;
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT7 = 1;
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT5) {  // Xd
        I_rung_top = 0;
    }
    
    if (GPIO_INPUT_PORT6) {  // Xe
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        GPIO_OUTPUT_PORT7 = 0;
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        GPIO_OUTPUT_PORT12 = 0;
    } else {
        GPIO_OUTPUT_PORT12 = 1;
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    /* ] finish series */
}
