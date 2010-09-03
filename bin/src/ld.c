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

volatile unsigned int I_parOut_0000 = 0;

volatile unsigned int I_parThis_0000 = 0;
volatile unsigned int U_POSICAO = 0;
volatile unsigned int U_POSICAO2 = 0;

volatile unsigned int U_RS1 = 0;
volatile unsigned int U_TON = 0;

volatile unsigned int I_TOFF_antiglitch = 0;
volatile unsigned int U_TOFF = 0;

volatile unsigned int I_scratch = 0;

volatile unsigned int I_oneShot_0000 = 0;

volatile unsigned int U1 = 0;

volatile unsigned int I_oneShot_0001 = 0;

volatile unsigned int I_oneShot_0002 = 0;

volatile unsigned int I_oneShot_0003 = 0;

volatile unsigned int I_oneShot_0004 = 0;

volatile unsigned int I_oneShot_0005 = 0;

volatile unsigned int I_oneShot_0006 = 0;

volatile unsigned int I_oneShot_0007 = 0;

volatile unsigned int U2 = 0;

volatile unsigned int I_oneShot_0008 = 0;

volatile unsigned int I_oneShot_0009 = 0;

volatile unsigned int I_oneShot_000a = 0;

volatile unsigned int I_oneShot_000b = 0;

volatile unsigned int I_oneShot_000c = 0;

volatile unsigned int I_oneShot_000d = 0;


/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */
void PlcCycle(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {
        U_POSICAO = 12345;
    }
    
    if (I_parThis_0000) {
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {
        U_POSICAO2 = 54321;
    }
    
    if (I_parThis_0000) {
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RS1) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (U_TON < 999) {
            U_TON++;
            I_rung_top = 0;
        }
    } else {
        U_TON = 0;
    }
    
    if (!I_TOFF_antiglitch) {
        U_TOFF = 999;
    }
    I_TOFF_antiglitch = 1;
    if (!I_rung_top) {
        if (U_TOFF < 999) {
            U_TOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TOFF = 0;
    }
    
    U_RS1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RS1) {
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0000) {
        I_rung_top = 0;
    }
    I_oneShot_0000 = I_scratch;
    
    if (I_rung_top) {
        U1 = 1;
    }
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U1) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (!I_oneShot_0001) {
            if (I_SerialReady) {
                uss_set_param(0, 613, 0, 1, &U_POSICAO);
                I_oneShot_0001 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0002 = I_rung_top;
        }
        if (!I_oneShot_0002) {
            if (I_SerialReady) {
                I_oneShot_0002 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0001 = I_rung_top;
    }
    
    if (I_rung_top) {
        if (!I_oneShot_0003) {
            if (I_SerialReady) {
                uss_set_param(0, 613, 0, 2, &U_POSICAO);
                I_oneShot_0003 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0004 = I_rung_top;
        }
        if (!I_oneShot_0004) {
            if (I_SerialReady) {
                I_oneShot_0004 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0003 = I_rung_top;
    }
    
    if (I_rung_top) {
        if (!I_oneShot_0005) {
            if (I_SerialReady) {
                uss_set_param(0, 613, 0, 3, &U_POSICAO);
                I_oneShot_0005 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0006 = I_rung_top;
        }
        if (!I_oneShot_0006) {
            if (I_SerialReady) {
                I_oneShot_0006 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0005 = I_rung_top;
    }
    
    if (I_rung_top) {
        U1 = 0;
    }
    
    /* ] finish series */
    
    /* start rung 5 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RS1) {
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {
        if (I_oneShot_0007) {
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0007 = I_scratch;
    
    if (I_rung_top) {
        U2 = 1;
    }
    
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U2) {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {
        if (!I_oneShot_0008) {
            if (I_SerialReady) {
                uss_set_param(0, 613, 0, 1, &U_POSICAO2);
                I_oneShot_0008 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0009 = I_rung_top;
        }
        if (!I_oneShot_0009) {
            if (I_SerialReady) {
                I_oneShot_0009 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0008 = I_rung_top;
    }
    
    if (I_rung_top) {
        if (!I_oneShot_000a) {
            if (I_SerialReady) {
                uss_set_param(0, 613, 0, 2, &U_POSICAO2);
                I_oneShot_000a = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_000b = I_rung_top;
        }
        if (!I_oneShot_000b) {
            if (I_SerialReady) {
                I_oneShot_000b = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_000a = I_rung_top;
    }
    
    if (I_rung_top) {
        if (!I_oneShot_000c) {
            if (I_SerialReady) {
                uss_set_param(0, 613, 0, 3, &U_POSICAO2);
                I_oneShot_000c = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_000d = I_rung_top;
        }
        if (!I_oneShot_000d) {
            if (I_SerialReady) {
                I_oneShot_000d = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_000c = I_rung_top;
    }
    
    if (I_rung_top) {
        U2 = 0;
    }
    
    /* ] finish series */
}
