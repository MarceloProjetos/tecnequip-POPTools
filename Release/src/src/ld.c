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

volatile unsigned int U_YEMERGENCIA = 0;
#define Read_U_YEMERGENCIA() U_YEMERGENCIA
#define Write_U_YEMERGENCIA(x) U_YEMERGENCIA = x

volatile unsigned int U_YCONTATORA = 0;
#define Read_U_YCONTATORA() U_YCONTATORA
#define Write_U_YCONTATORA(x) U_YCONTATORA = x


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    Write_I_mcr(1);
    
    /* start rung 1 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(!Read_U_YEMERGENCIA()) {
        Write_I_rung_top(0);
    }
    
    Write_U_YCONTATORA(Read_I_rung_top());
    
    /* ] finish series */
}
