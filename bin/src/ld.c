/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;
//volatile unsigned int U_M[32];

extern unsigned int RS232Write(char c);
extern unsigned int ADCRead(unsigned int i);
extern unsigned int ENCRead(unsigned int i);
extern unsigned int ENCReset(unsigned int i);

volatile unsigned int I_mcr = 0;

volatile unsigned int I_rung_top = 0;

volatile unsigned int I_E1 = 0;
volatile unsigned int I_scratch2 = 0;

volatile unsigned int U_S1 = 0;

volatile unsigned int U_S2 = 0;

volatile unsigned int U_S3 = 0;

volatile unsigned int U_S4 = 0;

volatile unsigned int U_S5 = 0;

volatile unsigned int U_S6 = 0;

volatile unsigned int U_S7 = 0;

volatile unsigned int U_S8 = 0;

volatile unsigned int U_S9 = 0;

volatile unsigned int U_S10 = 0;

volatile unsigned int U_S11 = 0;

volatile unsigned int U_S12 = 0;

volatile unsigned int U_S13 = 0;

volatile unsigned int U_S14 = 0;

volatile unsigned int U_S15 = 0;

volatile unsigned int U_S16 = 0;


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I_E1) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        ENCReset(0);
    }
    
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        U_ENC1 = ENCRead(0);
    }
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 100;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 1000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S2 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 5 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 5000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S3 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 6000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S4 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 7 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 7000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S5 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 8 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 8000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S6 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 9 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 9000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S7 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 10 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 10000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S8 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 11 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 12000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S9 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 12 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 15000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S10 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 13 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 18000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S11 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 14 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 20000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S12 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 15 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 22000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S13 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 16 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 24000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S14 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 17 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 26000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S15 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 18 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 30000;
    if (U_ENC1 > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_S16 = I_rung_top;
    
    /* ] finish series */
}
