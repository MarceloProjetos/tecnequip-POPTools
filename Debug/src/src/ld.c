/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;

extern unsigned int RS232Write(char * buffer, unsigned int size);


volatile unsigned int I_mcr = 0;
#define Read_I_mcr() I_mcr
#define Write_I_mcr(x) I_mcr = x

volatile unsigned int I_rung_top = 0;
#define Read_I_rung_top() I_rung_top
#define Write_I_rung_top(x) I_rung_top = x

volatile unsigned int I_E1 = 0;
#define Read_I_E1() I_E1
#define Write_I_E1(x) I_E1 = x

volatile unsigned int I_E3 = 0;
#define Read_I_E3() I_E3
#define Write_I_E3(x) I_E3 = x

volatile unsigned int I_E15 = 0;
#define Read_I_E15() I_E15
#define Write_I_E15(x) I_E15 = x

volatile unsigned int U_REMERGENCIA = 0;
#define Read_U_REMERGENCIA() U_REMERGENCIA
#define Write_U_REMERGENCIA(x) U_REMERGENCIA = x

volatile unsigned int I_parOut_0000 = 0;
#define Read_I_parOut_0000() I_parOut_0000
#define Write_I_parOut_0000(x) I_parOut_0000 = x

volatile unsigned int I_parThis_0000 = 0;
#define Read_I_parThis_0000() I_parThis_0000
#define Write_I_parThis_0000(x) I_parThis_0000 = x

volatile unsigned int U_M1 = 0;
#define Read_U_M1() U_M1
#define Write_U_M1(x) U_M1 = x

volatile unsigned int U_M2 = 0;
#define Read_U_M2() U_M2
#define Write_U_M2(x) U_M2 = x

volatile unsigned int U_S1 = 0;
#define Read_U_S1() U_S1
#define Write_U_S1(x) U_S1 = x

volatile unsigned int I_parOut_0001 = 0;
#define Read_I_parOut_0001() I_parOut_0001
#define Write_I_parOut_0001(x) I_parOut_0001 = x

volatile unsigned int I_parThis_0001 = 0;
#define Read_I_parThis_0001() I_parThis_0001
#define Write_I_parThis_0001(x) I_parThis_0001 = x

volatile unsigned int U_M3 = 0;
#define Read_U_M3() U_M3
#define Write_U_M3(x) U_M3 = x

volatile unsigned int I_E10 = 0;
#define Read_I_E10() I_E10
#define Write_I_E10(x) I_E10 = x

volatile unsigned int I_E2 = 0;
#define Read_I_E2() I_E2
#define Write_I_E2(x) I_E2 = x
volatile unsigned int U_TRESISTENCIA1;

volatile unsigned int I_parOut_0002 = 0;
#define Read_I_parOut_0002() I_parOut_0002
#define Write_I_parOut_0002(x) I_parOut_0002 = x

volatile unsigned int I_parThis_0002 = 0;
#define Read_I_parThis_0002() I_parThis_0002
#define Write_I_parThis_0002(x) I_parThis_0002 = x

volatile unsigned int U_S2 = 0;
#define Read_U_S2() U_S2
#define Write_U_S2(x) U_S2 = x
volatile unsigned int U_TRESISTENCIA2;

volatile unsigned int U_S3 = 0;
#define Read_U_S3() U_S3
#define Write_U_S3(x) U_S3 = x

volatile unsigned int I_parOut_0003 = 0;
#define Read_I_parOut_0003() I_parOut_0003
#define Write_I_parOut_0003(x) I_parOut_0003 = x

volatile unsigned int I_parThis_0003 = 0;
#define Read_I_parThis_0003() I_parThis_0003
#define Write_I_parThis_0003(x) I_parThis_0003 = x

volatile unsigned int I_parOut_0004 = 0;
#define Read_I_parOut_0004() I_parOut_0004
#define Write_I_parOut_0004(x) I_parOut_0004 = x

volatile unsigned int I_parThis_0004 = 0;
#define Read_I_parThis_0004() I_parThis_0004
#define Write_I_parThis_0004(x) I_parThis_0004 = x
volatile unsigned int U_TSTART1;

volatile unsigned int U_M4 = 0;
#define Read_U_M4() U_M4
#define Write_U_M4(x) U_M4 = x

volatile unsigned int U_S4 = 0;
#define Read_U_S4() U_S4
#define Write_U_S4(x) U_S4 = x

volatile unsigned int I_parOut_0005 = 0;
#define Read_I_parOut_0005() I_parOut_0005
#define Write_I_parOut_0005(x) I_parOut_0005 = x

volatile unsigned int I_parThis_0005 = 0;
#define Read_I_parThis_0005() I_parThis_0005
#define Write_I_parThis_0005(x) I_parThis_0005 = x

volatile unsigned int I_E4 = 0;
#define Read_I_E4() I_E4
#define Write_I_E4(x) I_E4 = x

volatile unsigned int U_M5 = 0;
#define Read_U_M5() U_M5
#define Write_U_M5(x) U_M5 = x

volatile unsigned int U_S6 = 0;
#define Read_U_S6() U_S6
#define Write_U_S6(x) U_S6 = x

volatile unsigned int I_parOut_0006 = 0;
#define Read_I_parOut_0006() I_parOut_0006
#define Write_I_parOut_0006(x) I_parOut_0006 = x

volatile unsigned int I_parThis_0006 = 0;
#define Read_I_parThis_0006() I_parThis_0006
#define Write_I_parThis_0006(x) I_parThis_0006 = x

volatile unsigned int I_E5 = 0;
#define Read_I_E5() I_E5
#define Write_I_E5(x) I_E5 = x

volatile unsigned int U_M6 = 0;
#define Read_U_M6() U_M6
#define Write_U_M6(x) U_M6 = x

volatile unsigned int U_S8 = 0;
#define Read_U_S8() U_S8
#define Write_U_S8(x) U_S8 = x

volatile unsigned int U_M7 = 0;
#define Read_U_M7() U_M7
#define Write_U_M7(x) U_M7 = x

volatile unsigned int U_S5 = 0;
#define Read_U_S5() U_S5
#define Write_U_S5(x) U_S5 = x

volatile unsigned int U_M8 = 0;
#define Read_U_M8() U_M8
#define Write_U_M8(x) U_M8 = x

volatile unsigned int U_S7 = 0;
#define Read_U_S7() U_S7
#define Write_U_S7(x) U_S7 = x

volatile unsigned int U_M9 = 0;
#define Read_U_M9() U_M9
#define Write_U_M9(x) U_M9 = x

volatile unsigned int U_S9 = 0;
#define Read_U_S9() U_S9
#define Write_U_S9(x) U_S9 = x

volatile unsigned int I_parOut_0007 = 0;
#define Read_I_parOut_0007() I_parOut_0007
#define Write_I_parOut_0007(x) I_parOut_0007 = x

volatile unsigned int I_parThis_0007 = 0;
#define Read_I_parThis_0007() I_parThis_0007
#define Write_I_parThis_0007(x) I_parThis_0007 = x

volatile unsigned int I_E6 = 0;
#define Read_I_E6() I_E6
#define Write_I_E6(x) I_E6 = x

volatile unsigned int U_S15 = 0;
#define Read_U_S15() U_S15
#define Write_U_S15(x) U_S15 = x


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
    
    if(!Read_I_E3()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_I_E15()) {
        Write_I_rung_top(0);
    }
    
    Write_U_REMERGENCIA(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 2 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    /* start parallel [ */
    Write_I_parOut_0000(0);
    Write_I_parThis_0000(Read_I_rung_top());
    if(!Read_U_M1()) {
        Write_I_parThis_0000(0);
    }
    
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_parThis_0000(Read_I_rung_top());
    if(!Read_U_M2()) {
        Write_I_parThis_0000(0);
    }
    
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_rung_top(Read_I_parOut_0000());
    /* ] finish parallel */
    if(!Read_U_REMERGENCIA()) {
        Write_I_rung_top(0);
    }
    
    Write_U_S1(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 3 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    /* start parallel [ */
    Write_I_parOut_0001(0);
    Write_I_parThis_0001(Read_I_rung_top());
    if(!Read_U_M1()) {
        Write_I_parThis_0001(0);
    }
    
    if(Read_I_parThis_0001()) {
        Write_I_parOut_0001(1);
    }
    Write_I_parThis_0001(Read_I_rung_top());
    if(!Read_U_M3()) {
        Write_I_parThis_0001(0);
    }
    
    if(Read_I_parThis_0001()) {
        Write_I_parOut_0001(1);
    }
    Write_I_rung_top(Read_I_parOut_0001());
    /* ] finish parallel */
    if(!Read_U_REMERGENCIA()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_I_E10()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_I_E2()) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        if(U_TRESISTENCIA1 < 499) {
            U_TRESISTENCIA1++;
            Write_I_rung_top(0);
        }
    } else {
        U_TRESISTENCIA1 = 0;
    }
    
    /* start parallel [ */
    Write_I_parOut_0002(0);
    Write_I_parThis_0002(Read_I_rung_top());
    Write_U_S2(Read_I_parThis_0002());
    
    if(Read_I_parThis_0002()) {
        Write_I_parOut_0002(1);
    }
    Write_I_parThis_0002(Read_I_rung_top());
    /* start series [ */
    if(Read_I_parThis_0002()) {
        if(U_TRESISTENCIA2 < 499) {
            U_TRESISTENCIA2++;
            Write_I_parThis_0002(0);
        }
    } else {
        U_TRESISTENCIA2 = 0;
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
    /* start series [ */
    /* start parallel [ */
    Write_I_parOut_0004(0);
    Write_I_parThis_0004(Read_I_parThis_0003());
    /* start series [ */
    if(!Read_U_M1()) {
        Write_I_parThis_0004(0);
    }
    
    if(!Read_I_E10()) {
        Write_I_parThis_0004(0);
    }
    
    if(Read_I_parThis_0004()) {
        if(U_TSTART1 < 1499) {
            U_TSTART1++;
            Write_I_parThis_0004(0);
        }
    } else {
        U_TSTART1 = 0;
    }
    
    /* ] finish series */
    if(Read_I_parThis_0004()) {
        Write_I_parOut_0004(1);
    }
    Write_I_parThis_0004(Read_I_parThis_0003());
    if(!Read_U_M4()) {
        Write_I_parThis_0004(0);
    }
    
    if(Read_I_parThis_0004()) {
        Write_I_parOut_0004(1);
    }
    Write_I_parThis_0003(Read_I_parOut_0004());
    /* ] finish parallel */
    if(!Read_U_REMERGENCIA()) {
        Write_I_parThis_0003(0);
    }
    
    Write_U_S4(Read_I_parThis_0003());
    
    /* ] finish series */
    if(Read_I_parThis_0003()) {
        Write_I_parOut_0003(1);
    }
    Write_I_parThis_0003(Read_I_rung_top());
    /* start series [ */
    /* start parallel [ */
    Write_I_parOut_0005(0);
    Write_I_parThis_0005(Read_I_parThis_0003());
    /* start series [ */
    if(!Read_U_M1()) {
        Write_I_parThis_0005(0);
    }
    
    if(!Read_I_E4()) {
        Write_I_parThis_0005(0);
    }
    
    /* ] finish series */
    if(Read_I_parThis_0005()) {
        Write_I_parOut_0005(1);
    }
    Write_I_parThis_0005(Read_I_parThis_0003());
    if(!Read_U_M5()) {
        Write_I_parThis_0005(0);
    }
    
    if(Read_I_parThis_0005()) {
        Write_I_parOut_0005(1);
    }
    Write_I_parThis_0003(Read_I_parOut_0005());
    /* ] finish parallel */
    if(!Read_U_REMERGENCIA()) {
        Write_I_parThis_0003(0);
    }
    
    Write_U_S6(Read_I_parThis_0003());
    
    /* ] finish series */
    if(Read_I_parThis_0003()) {
        Write_I_parOut_0003(1);
    }
    Write_I_rung_top(Read_I_parOut_0003());
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 5 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    /* start parallel [ */
    Write_I_parOut_0006(0);
    Write_I_parThis_0006(Read_I_rung_top());
    /* start series [ */
    if(!Read_U_M1()) {
        Write_I_parThis_0006(0);
    }
    
    if(!Read_I_E5()) {
        Write_I_parThis_0006(0);
    }
    
    /* ] finish series */
    if(Read_I_parThis_0006()) {
        Write_I_parOut_0006(1);
    }
    Write_I_parThis_0006(Read_I_rung_top());
    if(!Read_U_M6()) {
        Write_I_parThis_0006(0);
    }
    
    if(Read_I_parThis_0006()) {
        Write_I_parOut_0006(1);
    }
    Write_I_rung_top(Read_I_parOut_0006());
    /* ] finish parallel */
    if(!Read_U_REMERGENCIA()) {
        Write_I_rung_top(0);
    }
    
    Write_U_S8(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 6 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_M7()) {
        Write_I_rung_top(0);
    }
    
    Write_U_S5(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 7 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_M8()) {
        Write_I_rung_top(0);
    }
    
    Write_U_S7(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 8 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_M9()) {
        Write_I_rung_top(0);
    }
    
    Write_U_S9(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 9 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_M1()) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0007(0);
    Write_I_parThis_0007(Read_I_rung_top());
    if(Read_U_REMERGENCIA()) {
        Write_I_parThis_0007(0);
    }
    
    if(Read_I_parThis_0007()) {
        Write_I_parOut_0007(1);
    }
    Write_I_parThis_0007(Read_I_rung_top());
    if(Read_I_E4()) {
        Write_I_parThis_0007(0);
    }
    
    if(Read_I_parThis_0007()) {
        Write_I_parOut_0007(1);
    }
    Write_I_parThis_0007(Read_I_rung_top());
    if(Read_I_E5()) {
        Write_I_parThis_0007(0);
    }
    
    if(Read_I_parThis_0007()) {
        Write_I_parOut_0007(1);
    }
    Write_I_parThis_0007(Read_I_rung_top());
    if(Read_I_E6()) {
        Write_I_parThis_0007(0);
    }
    
    if(Read_I_parThis_0007()) {
        Write_I_parOut_0007(1);
    }
    Write_I_rung_top(Read_I_parOut_0007());
    /* ] finish parallel */
    Write_U_S15(Read_I_rung_top());
    
    /* ] finish series */
}
