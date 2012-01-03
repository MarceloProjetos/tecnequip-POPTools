// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int GPIO_OUTPUT_PORT18 = 0;
volatile int U_TON = 0;
volatile int I_TOFF_antiglitch = 0;
volatile int U_TOFF = 0;
volatile int I_parOut_0000 = 0;
volatile int I_parThis_0000 = 0;
volatile int A1 = 0;
volatile int A2 = 0;
volatile int A3 = 0;
volatile int A4 = 0;
volatile int A5 = 0;
volatile int A6 = 0;
volatile int U_ADC = 0;
volatile int I_parOut_0001 = 0;
volatile int I_parThis_0001 = 0;
volatile int GPIO_INPUT_PORT1 = 0;
volatile int I_parOut_0002 = 0;
volatile int I_parThis_0002 = 0;
volatile int GPIO_INPUT_PORT2 = 0;
volatile int I_parOut_0003 = 0;
volatile int I_parThis_0003 = 0;
volatile int GPIO_INPUT_PORT3 = 0;
volatile int I_parOut_0004 = 0;
volatile int I_parThis_0004 = 0;
volatile int GPIO_INPUT_PORT4 = 0;
volatile int I_parOut_0005 = 0;
volatile int I_parThis_0005 = 0;
volatile int GPIO_INPUT_PORT5 = 0;
volatile int I_parOut_0006 = 0;
volatile int I_parThis_0006 = 0;
volatile int GPIO_INPUT_PORT6 = 0;
volatile int I_scratch2 = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;
volatile int GPIO_OUTPUT_PORT2 = 0;
volatile int GPIO_OUTPUT_PORT3 = 0;
volatile int GPIO_OUTPUT_PORT4 = 0;
volatile int GPIO_OUTPUT_PORT5 = 0;
volatile int GPIO_OUTPUT_PORT6 = 0;
volatile int GPIO_OUTPUT_PORT7 = 0;
volatile int GPIO_OUTPUT_PORT8 = 0;
volatile int GPIO_OUTPUT_PORT9 = 0;
volatile int GPIO_OUTPUT_PORT10 = 0;
volatile int GPIO_OUTPUT_PORT11 = 0;
volatile int GPIO_OUTPUT_PORT12 = 0;
volatile int GPIO_OUTPUT_PORT13 = 0;
volatile int GPIO_OUTPUT_PORT14 = 0;
volatile int GPIO_OUTPUT_PORT15 = 0;
volatile int GPIO_OUTPUT_PORT16 = 0;
volatile int I_scratch = 0;
volatile int I_oneShot_0000 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
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
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        A1 = ADC_Read(1);
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        A2 = ADC_Read(2);
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        A3 = ADC_Read(3);
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        A4 = ADC_Read(4);
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        A5 = ADC_Read(5);
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        A6 = ADC_Read(6);
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_ADC = 0;
    }
    
    /* ] finish series */
    
    /* start rung 7 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    if (!GPIO_INPUT_PORT1) {  // XAD1
        I_parThis_0001 = 0;
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMAD1
        I_parThis_0001 = 0;
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_ADC = A1;
    }
    
    /* ] finish series */
    
    /* start rung 8 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0002 = 0;
    I_parThis_0002 = I_rung_top;
    if (!GPIO_INPUT_PORT2) {  // XAD2
        I_parThis_0002 = 0;
    }
    
    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    if (!(MODBUS_REGISTER[0] & (1 << 1))) {  // XMAD2
        I_parThis_0002 = 0;
    }
    
    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_rung_top = I_parOut_0002;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_ADC = A2;
    }
    
    /* ] finish series */
    
    /* start rung 9 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0003 = 0;
    I_parThis_0003 = I_rung_top;
    if (!GPIO_INPUT_PORT3) {  // XAD3
        I_parThis_0003 = 0;
    }
    
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (!(MODBUS_REGISTER[0] & (1 << 2))) {  // XMAD3
        I_parThis_0003 = 0;
    }
    
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_rung_top = I_parOut_0003;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_ADC = A3;
    }
    
    /* ] finish series */
    
    /* start rung 10 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0004 = 0;
    I_parThis_0004 = I_rung_top;
    if (!GPIO_INPUT_PORT4) {  // XAD4
        I_parThis_0004 = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (!(MODBUS_REGISTER[0] & (1 << 3))) {  // XMAD4
        I_parThis_0004 = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_rung_top = I_parOut_0004;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_ADC = A4;
    }
    
    /* ] finish series */
    
    /* start rung 11 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0005 = 0;
    I_parThis_0005 = I_rung_top;
    if (!GPIO_INPUT_PORT5) {  // XAD5
        I_parThis_0005 = 0;
    }
    
    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (!(MODBUS_REGISTER[0] & (1 << 4))) {  // XMAD5
        I_parThis_0005 = 0;
    }
    
    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_rung_top = I_parOut_0005;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_ADC = A5;
    }
    
    /* ] finish series */
    
    /* start rung 12 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0006 = 0;
    I_parThis_0006 = I_rung_top;
    if (!GPIO_INPUT_PORT6) {  // XAD6
        I_parThis_0006 = 0;
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (!(MODBUS_REGISTER[0] & (1 << 5))) {  // XMAD6
        I_parThis_0006 = 0;
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_rung_top = I_parOut_0006;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_ADC = A6;
    }
    
    /* ] finish series */
    
    /* start rung 14 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 0;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 15 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 62;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT2 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 16 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 125;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT3 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 17 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 188;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT4 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 18 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 250;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT5 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 19 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 312;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT6 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 20 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 375;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT7 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 21 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 437;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT8 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 22 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 500;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT9 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 23 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 562;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT10 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 24 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 625;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT11 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 25 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 687;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT12 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 26 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 750;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT13 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 27 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 812;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT14 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 28 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 875;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT15 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 29 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 937;
    if (U_ADC > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT16 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 31 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_rung_top = 0;
    }
    I_oneShot_0000 = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YADVAL
    }
    
    /* ] finish series */
    
    /* start rung 32 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[1] = U_ADC;
    }
    
    /* ] finish series */
}
