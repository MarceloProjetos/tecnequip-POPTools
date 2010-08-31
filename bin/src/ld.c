/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "uss.h"
#include "modbus.h"

extern void DAC_Write(unsigned int val);
extern void modbus_send(unsigned char id,
                        int fc,
                        unsigned short int address,
                        unsigned short int size,
                        volatile unsigned int * value);

volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;
volatile unsigned int M[32];
volatile int ENC1;

extern struct MB_Device modbus_master;
extern unsigned int RS232Write(char c);
extern unsigned int ADCRead(unsigned int i);
extern unsigned int ENCRead(unsigned int i);
extern unsigned int ENCReset(unsigned int i);
extern volatile unsigned int I_SerialReady;

volatile unsigned int I_mcr = 0;

volatile unsigned int I_rung_top = 0;
volatile unsigned int U_DA = 0;


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        DAC_Write(U_DA);
    }
    
    /* ] finish series */
}
