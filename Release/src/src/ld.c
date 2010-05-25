/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10000) - 1;


volatile unsigned int I_mcr = 0;
#define Read_I_mcr() I_mcr
#define Write_I_mcr(x) I_mcr = x

volatile unsigned int I_rung_top = 0;
#define Read_I_rung_top() I_rung_top
#define Write_I_rung_top(x) I_rung_top = x

volatile unsigned int I_E15 = 0;
#define Read_I_E15() I_E15
#define Write_I_E15(x) I_E15 = x

volatile unsigned int I_E9 = 0;
#define Read_I_E9() I_E9
#define Write_I_E9(x) I_E9 = x

volatile unsigned int U_S14 = 0;
#define Read_U_S14() U_S14
#define Write_U_S14(x) U_S14 = x


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    Write_I_mcr(1);
    
    /* start rung 1 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
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
    if(!Read_I_E9()) {
        Write_I_rung_top(0);
    }
    
    Write_U_S14(Read_I_rung_top());
    
    /* ] finish series */
}
