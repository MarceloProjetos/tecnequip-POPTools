// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* iniciando serie [ */
    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */
}
