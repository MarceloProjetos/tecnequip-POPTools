// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char I_scratch = 0;
volatile int I_scratch2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;
    I_scratch2 = rand();
    /* iniciando serie [ */
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0000) {  // $oneShot_0000
            I_scratch = 0;
            I_scratch = E2P_Busy();
            if (!I_scratch) {  // $scratch
                I_oneShot_0000 = E2P_Read((void *)&MODBUS_REGISTER[0], 0, 2);
            }
        } else {
			I_scratch = 0;
			I_scratch = E2P_Busy();
			if (!I_scratch) {  // $scratch
				E2P_Read((void *)&I_scratch2, 0, 2);
				if (I_scratch2 == MODBUS_REGISTER[0]) {
				} else {
					E2P_Write((void *)&MODBUS_REGISTER[0], 0, 2);
				}
			}
        }
    }

    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 0;
    if (I_scratch2 > MODBUS_REGISTER[0]) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 0;
    if (MODBUS_REGISTER[0] == I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT2 = I_rung_top;

    /* ] finish series */

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* iniciando serie [ */
    I_scratch2 = 0;
    if (MODBUS_REGISTER[0] > I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT3 = I_rung_top;

    /* ] finish series */
}
