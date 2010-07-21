/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;
volatile unsigned int M[32];
volatile int ENC1;

extern unsigned int RS232Write(char c);
extern unsigned int ADCRead(unsigned int i);
extern unsigned int ENCRead(unsigned int i);
extern unsigned int ENCReset(unsigned int i);

volatile unsigned int I_mcr = 0;

volatile unsigned int I_rung_top = 0;

volatile unsigned int I1 = 0;

volatile unsigned int I2 = 0;
volatile unsigned int U_EXP = 0;

volatile unsigned int I3 = 0;

volatile unsigned int I4 = 0;

volatile unsigned int I5 = 0;
volatile unsigned int I_scratch2 = 0;
volatile unsigned int U_SET1 = 0;
volatile unsigned int U_SET2 = 0;
volatile unsigned int U_SET3 = 0;
volatile unsigned int U_SET4 = 0;
volatile unsigned int U_SET5 = 0;
volatile unsigned int U_SET6 = 0;
volatile unsigned int U_SET7 = 0;
volatile unsigned int U_SET8 = 0;
volatile unsigned int U_SET9 = 0;
volatile unsigned int U_SET10 = 0;
volatile unsigned int U_SET11 = 0;
volatile unsigned int U_SET12 = 0;
volatile unsigned int U_SET13 = 0;
volatile unsigned int U_SET14 = 0;
volatile unsigned int U_SET15 = 0;
volatile unsigned int U_SET16 = 0;

volatile unsigned int U1 = 0;

volatile unsigned int U2 = 0;

volatile unsigned int U3 = 0;

volatile unsigned int U4 = 0;

volatile unsigned int U5 = 0;

volatile unsigned int U6 = 0;

volatile unsigned int U7 = 0;

volatile unsigned int U8 = 0;

volatile unsigned int U9 = 0;

volatile unsigned int U10 = 0;

volatile unsigned int U11 = 0;

volatile unsigned int U12 = 0;

volatile unsigned int U13 = 0;

volatile unsigned int U14 = 0;

volatile unsigned int U15 = 0;

volatile unsigned int U16 = 0;


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I1) {
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
        ENC1 = ENCRead(0);
    }
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I2) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_EXP = 1;
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I3) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_EXP = 16;
    }
    
    /* ] finish series */
    
    /* start rung 5 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I4) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_EXP = 4096;
    }
    
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!I5) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_EXP = 268435456;
    }
    
    /* ] finish series */
    
    /* start rung 7 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 1;
        U_SET1 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 8 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 2;
        U_SET2 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 9 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 3;
        U_SET3 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 10 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 4;
        U_SET4 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 11 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 5;
        U_SET5 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 12 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 6;
        U_SET6 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 13 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 7;
        U_SET7 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 14 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 8;
        U_SET8 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 15 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 9;
        U_SET9 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 16 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 10;
        U_SET10 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 17 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 11;
        U_SET11 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 18 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 12;
        U_SET12 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 19 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 13;
        U_SET13 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 20 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 14;
        U_SET14 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 21 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 15;
        U_SET15 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 22 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 16;
        U_SET16 = U_EXP * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 23 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET1) {
    } else {
        I_rung_top = 0;
    }
    
    U1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 24 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET2) {
    } else {
        I_rung_top = 0;
    }
    
    U2 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 25 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET3) {
    } else {
        I_rung_top = 0;
    }
    
    U3 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 26 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET4) {
    } else {
        I_rung_top = 0;
    }
    
    U4 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 27 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET5) {
    } else {
        I_rung_top = 0;
    }
    
    U5 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 28 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET6) {
    } else {
        I_rung_top = 0;
    }
    
    U6 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 29 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET7) {
    } else {
        I_rung_top = 0;
    }
    
    U7 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 30 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET8) {
    } else {
        I_rung_top = 0;
    }
    
    U8 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 31 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET9) {
    } else {
        I_rung_top = 0;
    }
    
    U9 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 32 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET10) {
    } else {
        I_rung_top = 0;
    }
    
    U10 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 33 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET11) {
    } else {
        I_rung_top = 0;
    }
    
    U11 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 34 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET12) {
    } else {
        I_rung_top = 0;
    }
    
    U12 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 35 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET13) {
    } else {
        I_rung_top = 0;
    }
    
    U13 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 36 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET14) {
    } else {
        I_rung_top = 0;
    }
    
    U14 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 37 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET15) {
    } else {
        I_rung_top = 0;
    }
    
    U15 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 38 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (ENC1 > U_SET16) {
    } else {
        I_rung_top = 0;
    }
    
    U16 = I_rung_top;
    
    /* ] finish series */
}
