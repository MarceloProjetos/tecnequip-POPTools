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

volatile unsigned int I_E2 = 0;
#define Read_I_E2() I_E2
#define Write_I_E2(x) I_E2 = x

volatile unsigned int I_parOut_0000 = 0;
#define Read_I_parOut_0000() I_parOut_0000
#define Write_I_parOut_0000(x) I_parOut_0000 = x

volatile unsigned int I_parThis_0000 = 0;
#define Read_I_parThis_0000() I_parThis_0000
#define Write_I_parThis_0000(x) I_parThis_0000 = x

volatile unsigned int I_scratch = 0;
#define Read_I_scratch() I_scratch
#define Write_I_scratch(x) I_scratch = x

volatile unsigned int I_oneShot_0000 = 0;
#define Read_I_oneShot_0000() I_oneShot_0000
#define Write_I_oneShot_0000(x) I_oneShot_0000 = x
volatile int U_M10;

volatile unsigned int I_oneShot_0001 = 0;
#define Read_I_oneShot_0001() I_oneShot_0001
#define Write_I_oneShot_0001(x) I_oneShot_0001 = x

volatile unsigned int I_E1 = 0;
#define Read_I_E1() I_E1
#define Write_I_E1(x) I_E1 = x

volatile unsigned int I_parOut_0001 = 0;
#define Read_I_parOut_0001() I_parOut_0001
#define Write_I_parOut_0001(x) I_parOut_0001 = x

volatile unsigned int I_parThis_0001 = 0;
#define Read_I_parThis_0001() I_parThis_0001
#define Write_I_parThis_0001(x) I_parThis_0001 = x

volatile unsigned int I_oneShot_0002 = 0;
#define Read_I_oneShot_0002() I_oneShot_0002
#define Write_I_oneShot_0002(x) I_oneShot_0002 = x
volatile int U_M5;

volatile unsigned int I_oneShot_0003 = 0;
#define Read_I_oneShot_0003() I_oneShot_0003
#define Write_I_oneShot_0003(x) I_oneShot_0003 = x


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    Write_I_mcr(1);
    
    /* start rung 1 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_I_E2()) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0000(0);
    Write_I_parThis_0000(Read_I_rung_top());
    /* start series [ */
    Write_I_scratch(Read_I_parThis_0000());
    if(Read_I_oneShot_0000()) {
        Write_I_parThis_0000(0);
    }
    Write_I_oneShot_0000(Read_I_scratch());
    
    if(Read_I_parThis_0000()) {
        U_M10 = 12;
    }
    
    /* ] finish series */
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_parThis_0000(Read_I_rung_top());
    /* start series [ */
    Write_I_scratch(Read_I_parThis_0000());
    if(!Read_I_parThis_0000()) {
        if(Read_I_oneShot_0001()) {
            Write_I_parThis_0000(1);
        }
    } else {
        Write_I_parThis_0000(0);
    }
    Write_I_oneShot_0001(Read_I_scratch());
    
    if(Read_I_parThis_0000()) {
        U_M10 = 55;
    }
    
    /* ] finish series */
    if(Read_I_parThis_0000()) {
        Write_I_parOut_0000(1);
    }
    Write_I_rung_top(Read_I_parOut_0000());
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 2 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_I_E1()) {
        Write_I_rung_top(0);
    }
    
    /* start parallel [ */
    Write_I_parOut_0001(0);
    Write_I_parThis_0001(Read_I_rung_top());
    /* start series [ */
    Write_I_scratch(Read_I_parThis_0001());
    if(Read_I_oneShot_0002()) {
        Write_I_parThis_0001(0);
    }
    Write_I_oneShot_0002(Read_I_scratch());
    
    if(Read_I_parThis_0001()) {
        U_M5 = 10;
    }
    
    /* ] finish series */
    if(Read_I_parThis_0001()) {
        Write_I_parOut_0001(1);
    }
    Write_I_parThis_0001(Read_I_rung_top());
    /* start series [ */
    Write_I_scratch(Read_I_parThis_0001());
    if(!Read_I_parThis_0001()) {
        if(Read_I_oneShot_0003()) {
            Write_I_parThis_0001(1);
        }
    } else {
        Write_I_parThis_0001(0);
    }
    Write_I_oneShot_0003(Read_I_scratch());
    
    if(Read_I_parThis_0001()) {
        U_M5 = 7;
    }
    
    /* ] finish series */
    if(Read_I_parThis_0001()) {
        Write_I_parOut_0001(1);
    }
    Write_I_rung_top(Read_I_parOut_0001());
    /* ] finish parallel */
    /* ] finish series */
}
