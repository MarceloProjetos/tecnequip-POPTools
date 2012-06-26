// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile int U_on = 0;
volatile unsigned char I_off_antiglitch = 0;
volatile int U_off = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_OUTPUT_PORT1) {  // S1
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_on < 199) {
            U_on++;
            I_rung_top = 0;
        }
    } else {
        U_on = 0;
    }
    
    if (!I_off_antiglitch) {  // $off_antiglitch
        U_off = 199;
    }
    I_off_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_off < 199) {
            U_off++;
            I_rung_top = 1;
        }
    } else {
        U_off = 0;
    }
    
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    GPIO_OUTPUT_PORT1 = I_parThis_0000;
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    MODBUS_REGISTER[0] &= ~(1 << 0); MODBUS_REGISTER[0] |= I_parThis_0000 << 0;  // Mb0
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[1] &= ~(1 << 0);  // Mb1
    } else {
        MODBUS_REGISTER[1] |= (1 << 0);  // Mb1
    }
    
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */
}
