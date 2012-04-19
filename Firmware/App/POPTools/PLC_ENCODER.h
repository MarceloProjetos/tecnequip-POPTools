// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile int U_encoder = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_encoder = ENC_Read();
    }

    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[8] = U_encoder;
    }

    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_rung_top = 0;
    }
    I_oneShot_0000 = I_scratch;

    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[8] &= ~(1 << 0);  // MbEncoder
    }

    /* ] finish series */
}
