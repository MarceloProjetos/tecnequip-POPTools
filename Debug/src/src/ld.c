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

volatile unsigned int U_XEMERGENCIA = 0;
#define Read_U_XEMERGENCIA() U_XEMERGENCIA
#define Write_U_XEMERGENCIA(x) U_XEMERGENCIA = x

volatile unsigned int U_YCONTATO = 0;
#define Read_U_YCONTATO() U_YCONTATO
#define Write_U_YCONTATO(x) U_YCONTATO = x


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    Write_I_mcr(1);
    
    /* start rung 1 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_XEMERGENCIA()) {
        Write_I_rung_top(0);
    }
    
    if(!Read_U_XEMERGENCIA()) {
        Write_I_rung_top(0);
    }
    
    Write_U_YCONTATO(Read_I_rung_top());
    
    /* ] finish series */
}
