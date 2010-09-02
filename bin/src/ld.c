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

volatile unsigned int U1 = 0;
volatile unsigned int U_TLEDON = 0;

volatile unsigned int I_TLEDOF_antiglitch = 0;
volatile unsigned int U_TLEDOF = 0;
volatile unsigned int I_scratch2 = 0;
volatile unsigned int U_DA = 0;


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
        if (U_TLEDON < 49) {
            U_TLEDON++;
            I_rung_top = 0;
        }
    } else {
        U_TLEDON = 0;
    }
    
    if (!I_TLEDOF_antiglitch) {
        U_TLEDOF = 49;
    }
    I_TLEDOF_antiglitch = 1;
    if (!I_rung_top) {
        if (U_TLEDOF < 49) {
            U_TLEDOF++;
            I_rung_top = 1;
        }
    } else {
        U_TLEDOF = 0;
    }
    
    U1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        I_scratch2 = 100;
        U_DA = U_DA + I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        DAC_Write(U_DA);
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 4095;
    if (U_DA > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        U_DA = 0;
    }
    
    /* ] finish series */
}
