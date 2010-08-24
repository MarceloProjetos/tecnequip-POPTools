/*****************************************************************************
 * Tecnequip Tecnologia em Equipamentos Ltda                                 *
 *****************************************************************************/
#include "uss.h"
volatile unsigned int TIME_INTERVAL = ((25000000/1000) * 10) - 1;
volatile unsigned int M[32];
volatile int ENC1;

extern unsigned int RS232Write(char c);
extern unsigned int ADCRead(unsigned int i);
extern unsigned int ENCRead(unsigned int i);
extern unsigned int ENCReset(unsigned int i);
extern volatile unsigned int I_USSReady;

volatile unsigned int I_mcr = 0;

volatile unsigned int I_rung_top = 0;

volatile unsigned int I_oneShot_0000 = 0;

volatile unsigned int I_ModbusReady = 0;
volatile unsigned int U_Knew = 0;

volatile unsigned int I_oneShot_0001 = 0;

volatile unsigned int I_oneShot_0002 = 0;
volatile unsigned int U_Vnew = 0;

volatile unsigned int I_oneShot_0003 = 0;


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {
        if (!I_oneShot_0000) {
            if (I_ModbusReady) {
                ModbusReadSingleRegister(0, 0, &U_Knew);
                I_oneShot_0000 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0001 = I_rung_top;
        }
        if (!I_oneShot_0001) {
            if (I_ModbusReady) {
                I_oneShot_0001 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0000 = I_rung_top;
    }
    
    if (I_rung_top) {
        if (!I_oneShot_0002) {
            if (I_ModbusReady) {
                ModbusWriteSingleRegister(0, 0, &U_Vnew);
                I_oneShot_0002 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0003 = I_rung_top;
        }
        if (!I_oneShot_0003) {
            if (I_ModbusReady) {
                I_oneShot_0003 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0002 = I_rung_top;
    }
    
    /* ] finish series */
}
