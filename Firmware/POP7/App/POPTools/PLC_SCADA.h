// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char U_YCPU = 0;
volatile int U_TON = 0;
volatile unsigned char I_TOFF_antiglitch = 0;
volatile int U_TOFF = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile unsigned char U_YERRO = 0;
volatile unsigned char U_RMbCPU = 0;
volatile unsigned char U_RMbErro = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char I_parOut_0001 = 0;
volatile unsigned char I_parThis_0001 = 0;
volatile unsigned char I_parOut_0002 = 0;
volatile unsigned char I_parThis_0002 = 0;
volatile int A1 = 0;
volatile unsigned char I_parOut_0003 = 0;
volatile unsigned char I_parThis_0003 = 0;
volatile int A2 = 0;
volatile unsigned char I_parOut_0004 = 0;
volatile unsigned char I_parThis_0004 = 0;
volatile int A3 = 0;
volatile unsigned char I_parOut_0005 = 0;
volatile unsigned char I_parThis_0005 = 0;
volatile int A4 = 0;
volatile int U_EENC1 = 0;
volatile int I_scratch2 = 0;
volatile unsigned char I_parOut_0006 = 0;
volatile unsigned char I_parThis_0006 = 0;
volatile int VAR_NULL = 0;
volatile unsigned char GPIO_INPUT_PORT1 = 0;
volatile unsigned char GPIO_INPUT_PORT2 = 0;
volatile unsigned char GPIO_INPUT_PORT3 = 0;
volatile unsigned char GPIO_INPUT_PORT4 = 0;
volatile unsigned char GPIO_INPUT_PORT5 = 0;
volatile unsigned char GPIO_INPUT_PORT6 = 0;
volatile unsigned char GPIO_INPUT_PORT7 = 0;
volatile unsigned char GPIO_INPUT_PORT8 = 0;
volatile unsigned char GPIO_INPUT_PORT9 = 0;
volatile unsigned char GPIO_INPUT_PORT10 = 0;
volatile unsigned char GPIO_INPUT_PORT11 = 0;
volatile unsigned char GPIO_INPUT_PORT12 = 0;
volatile unsigned char GPIO_INPUT_PORT13 = 0;
volatile unsigned char GPIO_INPUT_PORT14 = 0;
volatile unsigned char GPIO_INPUT_PORT15 = 0;
volatile unsigned char GPIO_INPUT_PORT16 = 0;
volatile unsigned char GPIO_INPUT_PORT17 = 0;
volatile unsigned char GPIO_INPUT_PORT18 = 0;
volatile unsigned char GPIO_INPUT_PORT19 = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;
volatile unsigned char GPIO_OUTPUT_PORT5 = 0;
volatile unsigned char GPIO_OUTPUT_PORT6 = 0;
volatile unsigned char GPIO_OUTPUT_PORT7 = 0;
volatile unsigned char GPIO_OUTPUT_PORT8 = 0;
volatile unsigned char GPIO_OUTPUT_PORT9 = 0;
volatile unsigned char GPIO_OUTPUT_PORT10 = 0;
volatile unsigned char GPIO_OUTPUT_PORT11 = 0;
volatile unsigned char GPIO_OUTPUT_PORT12 = 0;
volatile unsigned char GPIO_OUTPUT_PORT13 = 0;
volatile unsigned char GPIO_OUTPUT_PORT14 = 0;
volatile unsigned char GPIO_OUTPUT_PORT15 = 0;
volatile unsigned char GPIO_OUTPUT_PORT16 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_YCPU) {  // YCPU
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
    
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    U_YCPU = I_parThis_0000;
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_YERRO = 0;
    } else {
        U_YERRO = 1;
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_YCPU) {  // YCPU
        I_rung_top = 0;
    }
    
    U_RMbCPU = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_YERRO) {  // YERRO
        I_rung_top = 0;
    }
    
    U_RMbErro = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_rung_top = 0;
    }
    I_oneShot_0000 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        MODBUS_REGISTER[3] &= ~(1 << 0);  // YValDA
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        MODBUS_REGISTER[4] &= ~(1 << 0);  // YValAD1
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        MODBUS_REGISTER[5] &= ~(1 << 0);  // YValAD2
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        MODBUS_REGISTER[6] &= ~(1 << 0);  // YValAD3
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        MODBUS_REGISTER[7] &= ~(1 << 0);  // YValAD4
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        MODBUS_REGISTER[8] &= ~(1 << 0);  // YValENC1
    }
    
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 8 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0002 = 0;
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {  // $parThis_0002
        A1 = ADC_Read(1);
    }
    
    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {  // $parThis_0002
        MODBUS_REGISTER[4] = A1;
    }
    
    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_rung_top = I_parOut_0002;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 9 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0003 = 0;
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        A2 = ADC_Read(2);
    }
    
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        MODBUS_REGISTER[5] = A2;
    }
    
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_rung_top = I_parOut_0003;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 10 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0004 = 0;
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        A3 = ADC_Read(3);
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        MODBUS_REGISTER[6] = A3;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_rung_top = I_parOut_0004;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 11 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0005 = 0;
    I_parThis_0005 = I_rung_top;
    if (I_parThis_0005) {  // $parThis_0005
        A4 = ADC_Read(4);
    }
    
    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (I_parThis_0005) {  // $parThis_0005
        MODBUS_REGISTER[7] = A4;
    }
    
    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_rung_top = I_parOut_0005;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 13 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        DAC_Write(DAC_Conv(MODBUS_REGISTER[3], 0));
    }
    
    /* ] finish series */
    
    /* start rung 15 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_EENC1 = ENC_Read();
    }
    
    /* ] finish series */
    
    /* start rung 16 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        if(I_scratch2) MODBUS_REGISTER[8] = U_EENC1 / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 17 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[9] & (1 << 0))) {  // YZerarEnc1
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0006 = 0;
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        ENC_Reset();
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        MODBUS_REGISTER[9] &= ~(1 << 0);  // YZerarEnc1
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_rung_top = I_parOut_0006;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 19 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT1) {  // XE1
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 0); MODBUS_REGISTER[0] |= I_rung_top << 0;  // YE1
    
    /* ] finish series */
    
    /* start rung 20 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT2) {  // XE2
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 1); MODBUS_REGISTER[0] |= I_rung_top << 1;  // YE2
    
    /* ] finish series */
    
    /* start rung 21 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT3) {  // XE3
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 2); MODBUS_REGISTER[0] |= I_rung_top << 2;  // YE3
    
    /* ] finish series */
    
    /* start rung 22 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT4) {  // XE4
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 3); MODBUS_REGISTER[0] |= I_rung_top << 3;  // YE4
    
    /* ] finish series */
    
    /* start rung 23 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT5) {  // XE5
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 4); MODBUS_REGISTER[0] |= I_rung_top << 4;  // YE5
    
    /* ] finish series */
    
    /* start rung 24 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT6) {  // XE6
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 5); MODBUS_REGISTER[0] |= I_rung_top << 5;  // YE6
    
    /* ] finish series */
    
    /* start rung 25 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT7) {  // XE7
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 6); MODBUS_REGISTER[0] |= I_rung_top << 6;  // YE7
    
    /* ] finish series */
    
    /* start rung 26 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT8) {  // XE8
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 7); MODBUS_REGISTER[0] |= I_rung_top << 7;  // YE8
    
    /* ] finish series */
    
    /* start rung 27 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT9) {  // XE9
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 8); MODBUS_REGISTER[0] |= I_rung_top << 8;  // YE9
    
    /* ] finish series */
    
    /* start rung 28 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT10) {  // XE10
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 9); MODBUS_REGISTER[0] |= I_rung_top << 9;  // YE10
    
    /* ] finish series */
    
    /* start rung 29 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT11) {  // XE11
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 10); MODBUS_REGISTER[0] |= I_rung_top << 10;  // YE11
    
    /* ] finish series */
    
    /* start rung 30 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT12) {  // XE12
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 11); MODBUS_REGISTER[0] |= I_rung_top << 11;  // YE12
    
    /* ] finish series */
    
    /* start rung 31 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT13) {  // XE13
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 12); MODBUS_REGISTER[0] |= I_rung_top << 12;  // YE13
    
    /* ] finish series */
    
    /* start rung 32 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT14) {  // XE14
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 13); MODBUS_REGISTER[0] |= I_rung_top << 13;  // YE14
    
    /* ] finish series */
    
    /* start rung 33 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT15) {  // XE15
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 14); MODBUS_REGISTER[0] |= I_rung_top << 14;  // YE15
    
    /* ] finish series */
    
    /* start rung 34 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT16) {  // XE16
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 15); MODBUS_REGISTER[0] |= I_rung_top << 15;  // YE16
    
    /* ] finish series */
    
    /* start rung 35 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT17) {  // XE17
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 0); MODBUS_REGISTER[1] |= I_rung_top << 0;  // YE17
    
    /* ] finish series */
    
    /* start rung 36 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT18) {  // XE18
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 1); MODBUS_REGISTER[1] |= I_rung_top << 1;  // YE18
    
    /* ] finish series */
    
    /* start rung 37 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT19) {  // XE19
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 2); MODBUS_REGISTER[1] |= I_rung_top << 2;  // YE19
    
    /* ] finish series */
    
    /* start rung 39 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // XS1
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 40 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // XS2
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT2 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 41 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 2))) {  // XS3
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT3 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 42 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 3))) {  // XS4
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT4 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 43 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 4))) {  // XS5
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT5 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 44 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 5))) {  // XS6
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT6 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 45 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 6))) {  // XS7
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT7 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 46 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 7))) {  // XS8
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT8 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 47 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 8))) {  // XS9
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT9 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 48 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 9))) {  // XS10
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT10 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 49 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 10))) {  // XS11
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT11 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 50 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 11))) {  // XS12
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT12 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 51 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 12))) {  // XS13
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT13 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 52 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 13))) {  // XS14
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT14 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 53 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 14))) {  // XS15
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT15 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 54 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 15))) {  // XS16
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT16 = I_rung_top;
    
    /* ] finish series */
}
