/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;

extern unsigned int RS232Write(char c);
extern unsigned int ADCRead(unsigned int i);


volatile unsigned int I_mcr = 0;
#define Read_I_mcr() I_mcr
#define Write_I_mcr(x) I_mcr = x

volatile unsigned int I_rung_top = 0;
#define Read_I_rung_top() I_rung_top
#define Write_I_rung_top(x) I_rung_top = x

volatile unsigned int U_ROSC = 0;
#define Read_U_ROSC() U_ROSC
#define Write_U_ROSC(x) U_ROSC = x
volatile unsigned int U_TTON = 0;

volatile unsigned int I_TTOFF_antiglitch = 0;
#define Read_I_TTOFF_antiglitch() I_TTOFF_antiglitch
#define Write_I_TTOFF_antiglitch(x) I_TTOFF_antiglitch = x
volatile unsigned int U_TTOFF = 0;

volatile unsigned int I_parOut_0000 = 0;
#define Read_I_parOut_0000() I_parOut_0000
#define Write_I_parOut_0000(x) I_parOut_0000 = x

volatile unsigned int I_parThis_0000 = 0;
#define Read_I_parThis_0000() I_parThis_0000
#define Write_I_parThis_0000(x) I_parThis_0000 = x
volatile unsigned int U_A1 = 0;
volatile unsigned int U_A2 = 0;
volatile unsigned int U_A3 = 0;
volatile unsigned int U_A4 = 0;
volatile unsigned int U_A5 = 0;
volatile unsigned int U_ADC = 0;

volatile unsigned int I_E1 = 0;
#define Read_I_E1() I_E1
#define Write_I_E1(x) I_E1 = x

volatile unsigned int I_E2 = 0;
#define Read_I_E2() I_E2
#define Write_I_E2(x) I_E2 = x

volatile unsigned int I_E3 = 0;
#define Read_I_E3() I_E3
#define Write_I_E3(x) I_E3 = x

volatile unsigned int I_E4 = 0;
#define Read_I_E4() I_E4
#define Write_I_E4(x) I_E4 = x

volatile unsigned int I_E5 = 0;
#define Read_I_E5() I_E5
#define Write_I_E5(x) I_E5 = x
volatile unsigned int U_ADC5 = 0;
volatile unsigned int I_scratch2 = 0;

volatile unsigned int U_S1 = 0;
#define Read_U_S1() U_S1
#define Write_U_S1(x) U_S1 = x

volatile unsigned int U_S2 = 0;
#define Read_U_S2() U_S2
#define Write_U_S2(x) U_S2 = x

volatile unsigned int U_S3 = 0;
#define Read_U_S3() U_S3
#define Write_U_S3(x) U_S3 = x

volatile unsigned int U_S4 = 0;
#define Read_U_S4() U_S4
#define Write_U_S4(x) U_S4 = x

volatile unsigned int U_S5 = 0;
#define Read_U_S5() U_S5
#define Write_U_S5(x) U_S5 = x

volatile unsigned int U_S6 = 0;
#define Read_U_S6() U_S6
#define Write_U_S6(x) U_S6 = x

volatile unsigned int U_S7 = 0;
#define Read_U_S7() U_S7
#define Write_U_S7(x) U_S7 = x

volatile unsigned int U_S8 = 0;
#define Read_U_S8() U_S8
#define Write_U_S8(x) U_S8 = x

volatile unsigned int U_S9 = 0;
#define Read_U_S9() U_S9
#define Write_U_S9(x) U_S9 = x

volatile unsigned int U_S10 = 0;
#define Read_U_S10() U_S10
#define Write_U_S10(x) U_S10 = x

volatile unsigned int U_S11 = 0;
#define Read_U_S11() U_S11
#define Write_U_S11(x) U_S11 = x

volatile unsigned int U_S12 = 0;
#define Read_U_S12() U_S12
#define Write_U_S12(x) U_S12 = x

volatile unsigned int U_S13 = 0;
#define Read_U_S13() U_S13
#define Write_U_S13(x) U_S13 = x

volatile unsigned int U_S14 = 0;
#define Read_U_S14() U_S14
#define Write_U_S14(x) U_S14 = x

volatile unsigned int U_S15 = 0;
#define Read_U_S15() U_S15
#define Write_U_S15(x) U_S15 = x

volatile unsigned int U_S16 = 0;
#define Read_U_S16() U_S16
#define Write_U_S16(x) U_S16 = x


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    Write_I_mcr(1);
    
    /* start rung 1 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_U_ROSC()) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        if(U_TTON < 99) {
            U_TTON++;
            Write_I_rung_top(0);
        }
    } else {
        U_TTON = 0;
    }
    
    if(Read_I_TTOFF_antiglitch() == 0) {
        U_TTOFF = 99;
    }
    Write_I_TTOFF_antiglitch(1);
    if(Read_I_rung_top() == 0) {
        if(U_TTOFF < 99) {
            U_TTOFF++;
            Write_I_rung_top(1);
        }
    } else {
        U_TTOFF = 0;
    }
    
    Write_U_ROSC(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 2 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_U_ROSC() == 0) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0000(0);
    Write_I_parThis_0000(Read_I_rung_top());
    if(Read_I_parThis_0000()) {
        U_A1 = ADCRead(1);
    }
    
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_parThis_0000(Read_I_rung_top());
    if(Read_I_parThis_0000()) {
        U_A2 = ADCRead(2);
    }
    
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_parThis_0000(Read_I_rung_top());
    if(Read_I_parThis_0000()) {
        U_A3 = ADCRead(3);
    }
    
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_parThis_0000(Read_I_rung_top());
    if(Read_I_parThis_0000()) {
        U_A4 = ADCRead(4);
    }
    
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_parThis_0000(Read_I_rung_top());
    if(Read_I_parThis_0000()) {
        U_A5 = ADCRead(5);
    }
    
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_rung_top(Read_I_parOut_0000());
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 3 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_I_rung_top()) {
        U_ADC = 0;
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_I_E1() == 0) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        U_ADC = U_A1;
    }
    
    /* ] finish series */
    
    /* start rung 5 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_I_E2() == 0) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        U_ADC = U_A2;
    }
    
    /* ] finish series */
    
    /* start rung 6 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_I_E3() == 0) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        U_ADC = U_A3;
    }
    
    /* ] finish series */
    
    /* start rung 7 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_I_E4() == 0) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        U_ADC = U_A4;
    }
    
    /* ] finish series */
    
    /* start rung 8 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_I_E5() == 0) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        U_ADC = U_ADC5;
    }
    
    /* ] finish series */
    
    /* start rung 9 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 0;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S1(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 10 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 256;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S2(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 11 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 512;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S3(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 12 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 768;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S4(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 13 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 1024;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S5(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 14 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 1280;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S6(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 15 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 1536;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S7(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 16 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 1792;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S8(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 17 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 2048;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S9(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 18 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 2304;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S10(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 19 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 2560;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S11(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 20 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 2816;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S12(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 21 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 3072;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S13(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 22 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 3328;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S14(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 23 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 3584;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S15(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 24 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    I_scratch2 = 3840;
    if(U_ADC > I_scratch2) {
    } else {
        Write_I_rung_top(0);
    }
    
    Write_U_S16(Read_I_rung_top());
    
    /* ] finish series */
}
