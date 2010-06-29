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

volatile unsigned int I_C32 = 0;
#define Read_I_C32() I_C32
#define Write_I_C32(x) I_C32 = x

volatile unsigned int U_C32 = 0;
#define Read_U_C32() U_C32
#define Write_U_C32(x) U_C32 = x


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    Write_I_mcr(1);
    
    /* start rung 1 */
    Write_I_rung_top(Read_I_mcr());
    
    /* start series [ */
    if(Read_I_C32() == 0) {
        Write_I_rung_top(0);
    }
    
    Write_U_C32(Read_I_rung_top());
    
    /* ] finish series */
}
