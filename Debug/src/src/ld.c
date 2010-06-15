/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;

extern unsigned int RS232Write(char * buffer, unsigned int size);


volatile unsigned short int I_mcr = 0;
#define Read_I_mcr() I_mcr
#define Write_I_mcr(x) I_mcr = x

volatile unsigned short int I_rung_top = 0;
#define Read_I_rung_top() I_rung_top
#define Write_I_rung_top(x) I_rung_top = x

volatile unsigned short int I_E1 = 0;
#define Read_I_E1() I_E1
#define Write_I_E1(x) I_E1 = x

volatile unsigned short int I_E2 = 0;
#define Read_I_E2() I_E2
#define Write_I_E2(x) I_E2 = x

volatile unsigned short int I_E3 = 0;
#define Read_I_E3() I_E3
#define Write_I_E3(x) I_E3 = x

volatile unsigned short int I_E15 = 0;
#define Read_I_E15() I_E15
#define Write_I_E15(x) I_E15 = x

volatile unsigned short int I_parOut_0000 = 0;
#define Read_I_parOut_0000() I_parOut_0000
#define Write_I_parOut_0000(x) I_parOut_0000 = x

volatile unsigned short int I_parThis_0000 = 0;
#define Read_I_parThis_0000() I_parThis_0000
#define Write_I_parThis_0000(x) I_parThis_0000 = x

volatile unsigned short int U_M1 = 0;
#define Read_U_M1() U_M1
#define Write_U_M1(x) U_M1 = x

volatile unsigned short int U_M2 = 0;
#define Read_U_M2() U_M2
#define Write_U_M2(x) U_M2 = x

volatile unsigned short int U_S1 = 0;
#define Read_U_S1() U_S1
#define Write_U_S1(x) U_S1 = x

volatile unsigned short int I_parOut_0001 = 0;
#define Read_I_parOut_0001() I_parOut_0001
#define Write_I_parOut_0001(x) I_parOut_0001 = x

volatile unsigned short int I_parThis_0001 = 0;
#define Read_I_parThis_0001() I_parThis_0001
#define Write_I_parThis_0001(x) I_parThis_0001 = x

volatile unsigned short int U_M3 = 0;
#define Read_U_M3() U_M3
#define Write_U_M3(x) U_M3 = x

volatile unsigned short int I_E10 = 0;
#define Read_I_E10() I_E10
#define Write_I_E10(x) I_E10 = x
volatile unsigned short int U_TRESISTENCIA1;

volatile unsigned short int I_parOut_0002 = 0;
#define Read_I_parOut_0002() I_parOut_0002
#define Write_I_parOut_0002(x) I_parOut_0002 = x

volatile unsigned short int I_parThis_0002 = 0;
#define Read_I_parThis_0002() I_parThis_0002
#define Write_I_parThis_0002(x) I_parThis_0002 = x

volatile unsigned short int U_S2 = 0;
#define Read_U_S2() U_S2
#define Write_U_S2(x) U_S2 = x
volatile unsigned short int U_TRESISTENCIA2;

volatile unsigned short int U_S3 = 0;
#define Read_U_S3() U_S3
#define Write_U_S3(x) U_S3 = x

volatile unsigned short int I_parOut_0003 = 0;
#define Read_I_parOut_0003() I_parOut_0003
#define Write_I_parOut_0003(x) I_parOut_0003 = x

volatile unsigned short int I_parThis_0003 = 0;
#define Read_I_parThis_0003() I_parThis_0003
#define Write_I_parThis_0003(x) I_parThis_0003 = x

volatile unsigned short int I_parOut_0004 = 0;
#define Read_I_parOut_0004() I_parOut_0004
#define Write_I_parOut_0004(x) I_parOut_0004 = x

volatile unsigned short int I_parThis_0004 = 0;
#define Read_I_parThis_0004() I_parThis_0004
#define Write_I_parThis_0004(x) I_parThis_0004 = x

volatile unsigned short int I_E11 = 0;
#define Read_I_E11() I_E11
#define Write_I_E11(x) I_E11 = x

volatile unsigned short int I_E12 = 0;
#define Read_I_E12() I_E12
#define Write_I_E12(x) I_E12 = x

volatile unsigned short int I_E4 = 0;
#define Read_I_E4() I_E4
#define Write_I_E4(x) I_E4 = x

volatile unsigned short int U_M4 = 0;
#define Read_U_M4() U_M4
#define Write_U_M4(x) U_M4 = x

volatile unsigned short int U_S4 = 0;
#define Read_U_S4() U_S4
#define Write_U_S4(x) U_S4 = x

volatile unsigned short int I_parOut_0005 = 0;
#define Read_I_parOut_0005() I_parOut_0005
#define Write_I_parOut_0005(x) I_parOut_0005 = x

volatile unsigned short int I_parThis_0005 = 0;
#define Read_I_parThis_0005() I_parThis_0005
#define Write_I_parThis_0005(x) I_parThis_0005 = x
volatile unsigned short int U_TSTART2;

volatile unsigned short int I_E5 = 0;
#define Read_I_E5() I_E5
#define Write_I_E5(x) I_E5 = x

volatile unsigned short int U_M5 = 0;
#define Read_U_M5() U_M5
#define Write_U_M5(x) U_M5 = x

volatile unsigned short int U_S6 = 0;
#define Read_U_S6() U_S6
#define Write_U_S6(x) U_S6 = x

volatile unsigned short int I_parOut_0006 = 0;
#define Read_I_parOut_0006() I_parOut_0006
#define Write_I_parOut_0006(x) I_parOut_0006 = x

volatile unsigned short int I_parThis_0006 = 0;
#define Read_I_parThis_0006() I_parThis_0006
#define Write_I_parThis_0006(x) I_parThis_0006 = x
volatile unsigned short int U_TSTART3;

volatile unsigned short int I_E6 = 0;
#define Read_I_E6() I_E6
#define Write_I_E6(x) I_E6 = x

volatile unsigned short int U_M6 = 0;
#define Read_U_M6() U_M6
#define Write_U_M6(x) U_M6 = x

volatile unsigned short int U_S8 = 0;
#define Read_U_S8() U_S8
#define Write_U_S8(x) U_S8 = x

volatile unsigned short int U_RALARME_START1 = 0;
#define Read_U_RALARME_START1() U_RALARME_START1
#define Write_U_RALARME_START1(x) U_RALARME_START1 = x

volatile unsigned short int I_parOut_0007 = 0;
#define Read_I_parOut_0007() I_parOut_0007
#define Write_I_parOut_0007(x) I_parOut_0007 = x

volatile unsigned short int I_parThis_0007 = 0;
#define Read_I_parThis_0007() I_parThis_0007
#define Write_I_parThis_0007(x) I_parThis_0007 = x

volatile unsigned short int I_oneShot_0000 = 0;
#define Read_I_oneShot_0000() I_oneShot_0000
#define Write_I_oneShot_0000(x) I_oneShot_0000 = x
volatile unsigned short int U_CALARM_START1;
volatile unsigned short int U_TALARME_START1;

volatile unsigned short int U_S5 = 0;
#define Read_U_S5() U_S5
#define Write_U_S5(x) U_S5 = x

volatile unsigned short int U_RALARME_START2 = 0;
#define Read_U_RALARME_START2() U_RALARME_START2
#define Write_U_RALARME_START2(x) U_RALARME_START2 = x

volatile unsigned short int I_parOut_0008 = 0;
#define Read_I_parOut_0008() I_parOut_0008
#define Write_I_parOut_0008(x) I_parOut_0008 = x

volatile unsigned short int I_parThis_0008 = 0;
#define Read_I_parThis_0008() I_parThis_0008
#define Write_I_parThis_0008(x) I_parThis_0008 = x

volatile unsigned short int I_oneShot_0001 = 0;
#define Read_I_oneShot_0001() I_oneShot_0001
#define Write_I_oneShot_0001(x) I_oneShot_0001 = x
volatile unsigned short int U_CALARM_START2;
volatile unsigned short int U_TALARME_START2;

volatile unsigned short int U_S7 = 0;
#define Read_U_S7() U_S7
#define Write_U_S7(x) U_S7 = x

volatile unsigned short int U_RALARME_START3 = 0;
#define Read_U_RALARME_START3() U_RALARME_START3
#define Write_U_RALARME_START3(x) U_RALARME_START3 = x

volatile unsigned short int I_parOut_0009 = 0;
#define Read_I_parOut_0009() I_parOut_0009
#define Write_I_parOut_0009(x) I_parOut_0009 = x

volatile unsigned short int I_parThis_0009 = 0;
#define Read_I_parThis_0009() I_parThis_0009
#define Write_I_parThis_0009(x) I_parThis_0009 = x

volatile unsigned short int I_oneShot_0002 = 0;
#define Read_I_oneShot_0002() I_oneShot_0002
#define Write_I_oneShot_0002(x) I_oneShot_0002 = x
volatile unsigned short int U_CALARME_START3;
volatile unsigned short int U_TALARME_START3;

volatile unsigned short int U_S9 = 0;
#define Read_U_S9() U_S9
#define Write_U_S9(x) U_S9 = x

volatile unsigned short int U_M7 = 0;
#define Read_U_M7() U_M7
#define Write_U_M7(x) U_M7 = x

volatile unsigned short int U_M8 = 0;
#define Read_U_M8() U_M8
#define Write_U_M8(x) U_M8 = x

volatile unsigned short int U_M9 = 0;
#define Read_U_M9() U_M9
#define Write_U_M9(x) U_M9 = x
volatile unsigned short int U_CALARM_START3;


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
    
    if(!Read_I_E15()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_I_mcr()) {
        Write_I_mcr(1);
    } else {
        Write_I_mcr(Read_I_rung_top());
    }
    
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
    if(!Read_I_E10()) {
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
    
    if(!Read_I_E11()) {
        Write_I_parThis_0004(0);
    }
    
    if(!Read_I_E12()) {
        Write_I_parThis_0004(0);
    }
    
    if(Read_I_E4()) {
        Write_I_parThis_0004(0);
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
    
    if(!Read_U_S4()) {
        Write_I_parThis_0005(0);
    }
    
    if(Read_I_parThis_0005()) {
        if(U_TSTART2 < 499) {
            U_TSTART2++;
            Write_I_parThis_0005(0);
        }
    } else {
        U_TSTART2 = 0;
    }
    
    if(Read_I_E5()) {
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
    
    if(!Read_U_S6()) {
        Write_I_parThis_0006(0);
    }
    
    if(Read_I_parThis_0006()) {
        if(U_TSTART3 < 499) {
            U_TSTART3++;
            Write_I_parThis_0006(0);
        }
    } else {
        U_TSTART3 = 0;
    }
    
    if(Read_I_E6()) {
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
    Write_U_S8(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 6 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_I_E4()) {
        Write_I_rung_top(0);
    }
    
    if(Read_U_RALARME_START1()) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0007(0);
    Write_I_parThis_0007(Read_I_rung_top());
    /* start series [ */
    if(Read_I_parThis_0007()) {
        if(!Read_I_oneShot_0000()) {
            U_CALARM_START1++;
        }
    }
    Write_I_oneShot_0000(Read_I_parThis_0007());
    if(U_CALARM_START1 < 3) {
        Write_I_parThis_0007(0);
    } else {
        Write_I_parThis_0007(1);
    }
    
    Write_U_RALARME_START1(Read_I_parThis_0007());
    
    /* ] finish series */
    if(Read_I_parThis_0007()) {
        Write_I_parOut_0007(1);
    }
    Write_I_parThis_0007(Read_I_rung_top());
    /* start series [ */
    if(Read_I_parThis_0007()) {
        if(U_TALARME_START1 < 499) {
            U_TALARME_START1++;
            Write_I_parThis_0007(0);
        }
    } else {
        U_TALARME_START1 = 0;
    }
    
    Write_U_S5(Read_I_parThis_0007());
    
    /* ] finish series */
    if(Read_I_parThis_0007()) {
        Write_I_parOut_0007(1);
    }
    Write_I_rung_top(Read_I_parOut_0007());
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 7 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_I_E5()) {
        Write_I_rung_top(0);
    }
    
    if(Read_U_RALARME_START2()) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0008(0);
    Write_I_parThis_0008(Read_I_rung_top());
    /* start series [ */
    if(Read_I_parThis_0008()) {
        if(!Read_I_oneShot_0001()) {
            U_CALARM_START2++;
        }
    }
    Write_I_oneShot_0001(Read_I_parThis_0008());
    if(U_CALARM_START2 < 3) {
        Write_I_parThis_0008(0);
    } else {
        Write_I_parThis_0008(1);
    }
    
    Write_U_RALARME_START2(Read_I_parThis_0008());
    
    /* ] finish series */
    if(Read_I_parThis_0008()) {
        Write_I_parOut_0008(1);
    }
    Write_I_parThis_0008(Read_I_rung_top());
    /* start series [ */
    if(Read_I_parThis_0008()) {
        if(U_TALARME_START2 < 499) {
            U_TALARME_START2++;
            Write_I_parThis_0008(0);
        }
    } else {
        U_TALARME_START2 = 0;
    }
    
    Write_U_S7(Read_I_parThis_0008());
    
    /* ] finish series */
    if(Read_I_parThis_0008()) {
        Write_I_parOut_0008(1);
    }
    Write_I_rung_top(Read_I_parOut_0008());
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 8 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_I_E6()) {
        Write_I_rung_top(0);
    }
    
    if(Read_U_RALARME_START3()) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0009(0);
    Write_I_parThis_0009(Read_I_rung_top());
    /* start series [ */
    if(Read_I_parThis_0009()) {
        if(!Read_I_oneShot_0002()) {
            U_CALARME_START3++;
        }
    }
    Write_I_oneShot_0002(Read_I_parThis_0009());
    if(U_CALARME_START3 < 3) {
        Write_I_parThis_0009(0);
    } else {
        Write_I_parThis_0009(1);
    }
    
    Write_U_RALARME_START3(Read_I_parThis_0009());
    
    /* ] finish series */
    if(Read_I_parThis_0009()) {
        Write_I_parOut_0009(1);
    }
    Write_I_parThis_0009(Read_I_rung_top());
    /* start series [ */
    if(Read_I_parThis_0009()) {
        if(U_TALARME_START3 < 499) {
            U_TALARME_START3++;
            Write_I_parThis_0009(0);
        }
    } else {
        U_TALARME_START3 = 0;
    }
    
    Write_U_S9(Read_I_parThis_0009());
    
    /* ] finish series */
    if(Read_I_parThis_0009()) {
        Write_I_parOut_0009(1);
    }
    Write_I_rung_top(Read_I_parOut_0009());
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 9 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_M7()) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        U_CALARM_START1 = 0;
    }
    
    /* ] finish series */
    
    /* start rung 10 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_M8()) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        U_CALARM_START2 = 0;
    }
    
    /* ] finish series */
    
    /* start rung 11 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_M9()) {
        Write_I_rung_top(0);
    }
    
    if(Read_I_rung_top()) {
        U_CALARM_START3 = 0;
    }
    
    /* ] finish series */
}
