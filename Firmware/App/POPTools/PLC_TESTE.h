volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile unsigned char GPIO_INPUT_PORT1 = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char I_parOut_0001 = 0;
volatile unsigned char I_parThis_0001 = 0;
volatile unsigned char U_ciclo = 0;
volatile int U_on = 0;
volatile unsigned char I_off_antiglitch = 0;
volatile int U_off = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile int U_ValorSaida = 0;
volatile int I_scratch2 = 0;
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
	MODBUS_REGISTER[5] = SSI_Read(25, SSI_MODE_GRAY);

    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // NovoValor
        I_parThis_0000 = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT1) {  // DbgStart
        I_parThis_0000 = 0;
    }

    I_scratch = I_parThis_0000;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_parThis_0000 = 0;
    }
    I_oneShot_0000 = I_scratch;

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        if (MODBUS_REGISTER[2] < 0) {
            I_scratch = -1;
            MODBUS_REGISTER[1] = MODBUS_REGISTER[2] * I_scratch;
        } else {
            MODBUS_REGISTER[1] = MODBUS_REGISTER[2];
        }
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        MODBUS_REGISTER[2] = -20 + (rand() % (85 - -20 + 1));
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        MODBUS_REGISTER[0] &= ~(1 << 0);  // NovoValor
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    if (U_ciclo) {  // ciclo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_on < 49) {
            U_on++;
            I_rung_top = 0;
        }
    } else {
        U_on = 0;
    }

    if (!I_off_antiglitch) {  // $off_antiglitch
        U_off = 49;
    }
    I_off_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_off < 49) {
            U_off++;
            I_rung_top = 1;
        }
    } else {
        U_off = 0;
    }

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    U_ciclo = I_parThis_0000;

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* iniciando serie [ */
    I_scratch = I_parThis_0000;
    if (I_oneShot_0001) {  // $oneShot_0001
        I_parThis_0000 = 0;
    }
    I_oneShot_0001 = I_scratch;

    if (I_parThis_0000) {  // $parThis_0000
        U_ValorSaida = 1 + (rand() % (16 - 1 + 1));
    }

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 1;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 2;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT2 = I_rung_top;

    /* ] finish series */

    /* start rung 5 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 3;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT3 = I_rung_top;

    /* ] finish series */

    /* start rung 6 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 4;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT4 = I_rung_top;

    /* ] finish series */

    /* start rung 7 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 5;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT5 = I_rung_top;

    /* ] finish series */

    /* start rung 8 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 6;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT6 = I_rung_top;

    /* ] finish series */

    /* start rung 9 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 7;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT7 = I_rung_top;

    /* ] finish series */

    /* start rung 10 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 8;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT8 = I_rung_top;

    /* ] finish series */

    /* start rung 11 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 9;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT9 = I_rung_top;

    /* ] finish series */

    /* start rung 12 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 10;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT10 = I_rung_top;

    /* ] finish series */

    /* start rung 13 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 11;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT11 = I_rung_top;

    /* ] finish series */

    /* start rung 14 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 12;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT12 = I_rung_top;

    /* ] finish series */

    /* start rung 15 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 13;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT13 = I_rung_top;

    /* ] finish series */

    /* start rung 16 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 14;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT14 = I_rung_top;

    /* ] finish series */

    /* start rung 17 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 15;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT15 = I_rung_top;

    /* ] finish series */

    /* start rung 18 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 16;
    if (U_ValorSaida == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT16 = I_rung_top;

    /* ] finish series */
}
