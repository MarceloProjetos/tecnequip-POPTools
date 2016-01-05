// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;
volatile int GPIO_OUTPUT_PORT2 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XFLAG0
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT1 = I_rung_top;

    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 1))) {  // XFLAG1
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT2 = I_rung_top;

    /* ] finish series */
}

