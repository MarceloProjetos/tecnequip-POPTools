/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;


volatile unsigned int I_mcr = 0;
#define Read_I_mcr() I_mcr
#define Write_I_mcr(x) I_mcr = x

volatile unsigned int I_rung_top = 0;
#define Read_I_rung_top() I_rung_top
#define Write_I_rung_top(x) I_rung_top = x

volatile unsigned int I_E1 = 0;
#define Read_I_E1() I_E1
#define Write_I_E1(x) I_E1 = x

volatile unsigned int I_E2 = 0;
#define Read_I_E2() I_E2
#define Write_I_E2(x) I_E2 = x

volatile unsigned int I_E3 = 0;
#define Read_I_E3() I_E3
#define Write_I_E3(x) I_E3 = x

volatile unsigned int U_S6 = 0;
#define Read_U_S6() U_S6
#define Write_U_S6(x) U_S6 = x

volatile unsigned int I_parOut_0000 = 0;
#define Read_I_parOut_0000() I_parOut_0000
#define Write_I_parOut_0000(x) I_parOut_0000 = x

volatile unsigned int I_parThis_0000 = 0;
#define Read_I_parThis_0000() I_parThis_0000
#define Write_I_parThis_0000(x) I_parThis_0000 = x

volatile unsigned int I_parOut_0001 = 0;
#define Read_I_parOut_0001() I_parOut_0001
#define Write_I_parOut_0001(x) I_parOut_0001 = x

volatile unsigned int I_parThis_0001 = 0;
#define Read_I_parThis_0001() I_parThis_0001
#define Write_I_parThis_0001(x) I_parThis_0001 = x

volatile unsigned int I_E5 = 0;
#define Read_I_E5() I_E5
#define Write_I_E5(x) I_E5 = x

volatile unsigned int I_E6 = 0;
#define Read_I_E6() I_E6
#define Write_I_E6(x) I_E6 = x

volatile unsigned int I_E10 = 0;
#define Read_I_E10() I_E10
#define Write_I_E10(x) I_E10 = x

volatile unsigned int I_E4 = 0;
#define Read_I_E4() I_E4
#define Write_I_E4(x) I_E4 = x

volatile unsigned int U_S4 = 0;
#define Read_U_S4() U_S4
#define Write_U_S4(x) U_S4 = x

volatile unsigned int I_E7 = 0;
#define Read_I_E7() I_E7
#define Write_I_E7(x) I_E7 = x

volatile unsigned int U_S5 = 0;
#define Read_U_S5() U_S5
#define Write_U_S5(x) U_S5 = x

volatile unsigned int I_parOut_0002 = 0;
#define Read_I_parOut_0002() I_parOut_0002
#define Write_I_parOut_0002(x) I_parOut_0002 = x

volatile unsigned int I_parThis_0002 = 0;
#define Read_I_parThis_0002() I_parThis_0002
#define Write_I_parThis_0002(x) I_parThis_0002 = x

volatile unsigned int I_E8 = 0;
#define Read_I_E8() I_E8
#define Write_I_E8(x) I_E8 = x

volatile unsigned int U_S2 = 0;
#define Read_U_S2() U_S2
#define Write_U_S2(x) U_S2 = x

volatile unsigned int I_E9 = 0;
#define Read_I_E9() I_E9
#define Write_I_E9(x) I_E9 = x

volatile unsigned int U_S3 = 0;
#define Read_U_S3() U_S3
#define Write_U_S3(x) U_S3 = x

volatile unsigned int I_parOut_0003 = 0;
#define Read_I_parOut_0003() I_parOut_0003
#define Write_I_parOut_0003(x) I_parOut_0003 = x

volatile unsigned int I_parThis_0003 = 0;
#define Read_I_parThis_0003() I_parThis_0003
#define Write_I_parThis_0003(x) I_parThis_0003 = x

volatile unsigned int U_S1 = 0;
#define Read_U_S1() U_S1
#define Write_U_S1(x) U_S1 = x


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    Write_I_mcr(1);
    
    /* start rung 1 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_I_E1()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_I_E2()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_I_E3()) {
        Write_I_rung_top(0);
    }
    
    Write_U_S6(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 2 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_I_E1()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_I_E3()) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0000(0);
    Write_I_parThis_0000(Read_I_rung_top());
    /* start series [ */
    /* start parallel [ */
    Write_I_parOut_0001(0);
    Write_I_parThis_0001(Read_I_parThis_0000());
    /* start series [ */
    if(Read_I_E5()) {
        Write_I_parThis_0001(0);
    }
    
    if(!Read_I_E6()) {
        Write_I_parThis_0001(0);
    }
    
    /* ] finish series */
    if(Read_I_parThis_0001()) {
        Write_I_parOut_0001(1);
    }
    Write_I_parThis_0001(Read_I_parThis_0000());
    /* start series [ */
    if(!Read_I_E5()) {
        Write_I_parThis_0001(0);
    }
    
    if(!Read_I_E10()) {
        Write_I_parThis_0001(0);
    }
    
    if(!Read_I_E4()) {
        Write_I_parThis_0001(0);
    }
    
    /* ] finish series */
    if(Read_I_parThis_0001()) {
        Write_I_parOut_0001(1);
    }
    Write_I_parThis_0000(Read_I_parOut_0001());
    /* ] finish parallel */
    Write_U_S4(Read_I_parThis_0000());
    
    /* ] finish series */
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_parThis_0000(Read_I_rung_top());
    /* start series [ */
    if(Read_I_E5()) {
        Write_I_parThis_0000(0);
    }
    
    if(!Read_I_E7()) {
        Write_I_parThis_0000(0);
    }
    
    Write_U_S5(Read_I_parThis_0000());
    
    /* ] finish series */
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_rung_top(Read_I_parOut_0000());
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 3 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_I_E1()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_I_E2()) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0002(0);
    Write_I_parThis_0002(Read_I_rung_top());
    /* start series [ */
    if(Read_I_E5()) {
        Write_I_parThis_0002(0);
    }
    
    if(!Read_I_E8()) {
        Write_I_parThis_0002(0);
    }
    
    Write_U_S2(Read_I_parThis_0002());
    
    /* ] finish series */
    if(Read_I_parThis_0002()) {
        Write_I_parOut_0002(1);
    }
    Write_I_parThis_0002(Read_I_rung_top());
    /* start series [ */
    if(Read_I_E5()) {
        Write_I_parThis_0002(0);
    }
    
    if(!Read_I_E9()) {
        Write_I_parThis_0002(0);
    }
    
    Write_U_S3(Read_I_parThis_0002());
    
    /* ] finish series */
    if(Read_I_parThis_0002()) {
        Write_I_parOut_0002(1);
    }
    Write_I_rung_top(Read_I_parOut_0002());
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 4 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    /* start parallel [ */
    Write_I_parOut_0003(0);
    Write_I_parThis_0003(Read_I_rung_top());
    if(!Read_U_S2()) {
        Write_I_parThis_0003(0);
    }
    
    if(Read_I_parThis_0003()) {
        Write_I_parOut_0003(1);
    }
    Write_I_parThis_0003(Read_I_rung_top());
    if(!Read_U_S3()) {
        Write_I_parThis_0003(0);
    }
    
    if(Read_I_parThis_0003()) {
        Write_I_parOut_0003(1);
    }
    Write_I_rung_top(Read_I_parOut_0003());
    /* ] finish parallel */
    Write_U_S1(Read_I_rung_top());
    
    /* ] finish series */
}
