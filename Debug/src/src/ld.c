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

volatile unsigned int U_S18 = 0;
#define Read_U_S18() U_S18
#define Write_U_S18(x) U_S18 = x

volatile unsigned int U_S17 = 0;
#define Read_U_S17() U_S17
#define Write_U_S17(x) U_S17 = x


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    Write_I_mcr(1);
    
    /* start rung 1 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    Write_U_S18(Read_I_rung_top());
    
    /* ] finish series */
    
    /* start rung 2 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    Write_U_S17(Read_I_rung_top());
    
    /* ] finish series */
}
