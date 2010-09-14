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

volatile unsigned int CYCLE_TIME = 10;
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

volatile unsigned int U1 = 0;
volatile unsigned int U_TON = 0;

volatile unsigned int I_TOFF_antiglitch = 0;
volatile unsigned int U_TOFF = 0;


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U1) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TON < 49) {
            U_TON++;
            I_rung_top = 0;
        }
    } else {
        U_TON = 0;
    }
    
    if (!I_TOFF_antiglitch) {
        U_TOFF = 49;
    }
    I_TOFF_antiglitch = 1;
    if (!I_rung_top) {
        if (U_TOFF < 49) {
            U_TOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TOFF = 0;
    }
    
    U1 = I_rung_top;
    
    /* ] finish series */
}
