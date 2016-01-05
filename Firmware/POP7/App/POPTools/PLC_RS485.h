// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int GPIO_OUTPUT_PORT18 = 0;
volatile int U_TON = 0;
volatile int I_TOFF_antiglitch = 0;
volatile int U_TOFF = 0;
volatile int I_oneShot_0000 = 0;
volatile int U_NS_VAR = 0;
volatile int I_oneShot_0001 = 0;
volatile int I_oneShot_0002 = 0;
volatile int I_oneShot_0003 = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;
volatile int I_oneShot_0004 = 0;
volatile int U_USS_VAR = 0;
volatile int I_oneShot_0005 = 0;
volatile int I_oneShot_0006 = 0;
volatile int I_oneShot_0007 = 0;
volatile int GPIO_OUTPUT_PORT2 = 0;
volatile int I_oneShot_0008 = 0;
volatile int U_MB_485_VAR = 0;
volatile int I_oneShot_0009 = 0;
volatile int I_oneShot_000a = 0;
volatile int I_oneShot_000b = 0;
volatile int GPIO_OUTPUT_PORT3 = 0;
volatile int I_oneShot_000c = 0;
volatile int U_MB_ETH_VAR = 0;
volatile int I_oneShot_000d = 0;
volatile int I_oneShot_000e = 0;
volatile int U_Vnew = 0;
volatile int I_oneShot_000f = 0;
volatile int GPIO_OUTPUT_PORT4 = 0;
volatile int I_oneShot_0010 = 0;
volatile int U_STR_VAR = 0;
volatile int I_oneShot_0011 = 0;
volatile int I_oneShot_0012 = 0;
volatile int I_oneShot_0013 = 0;
volatile int GPIO_OUTPUT_PORT5 = 0;
volatile int U_CHAR_VAR = 0;
volatile int GPIO_OUTPUT_PORT6 = 0;
volatile int U_RSET5 = 0;
volatile int GPIO_OUTPUT_PORT7 = 0;
volatile int I_scratch = 0;
volatile int I_scratch2 = 0;
volatile int U_TISET_DA = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_OUTPUT_PORT18) {  // YLED
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TON < 49) {
            U_TON++;
            I_rung_top = 0;
        }
    } else {
        U_TON = 0;
    }
    
    if (!I_TOFF_antiglitch) {  // $TOFF_antiglitch
        U_TOFF = 49;
    }
    I_TOFF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TOFF < 49) {
            U_TOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TOFF = 0;
    }
    
    GPIO_OUTPUT_PORT18 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 2 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0000) {  // $oneShot_0000
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Read("1", "value: %d", &U_NS_VAR);
                I_oneShot_0000 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0001 = I_rung_top;
        }
        if (!I_oneShot_0001) {  // $oneShot_0001
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0001 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0000 = I_rung_top;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0002) {  // $oneShot_0002
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write("1", "value: %d", &U_NS_VAR);
                I_oneShot_0002 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0003 = I_rung_top;
        }
        if (!I_oneShot_0003) {  // $oneShot_0003
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0003 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0002 = I_rung_top;
    }
    
    GPIO_OUTPUT_PORT1 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 3 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0004) {  // $oneShot_0004
            if (I_SerialReady) {  // $SerialReady
                USS_Get_Param(0, 0, 0, 0, &U_USS_VAR);
                I_oneShot_0004 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0005 = I_rung_top;
        }
        if (!I_oneShot_0005) {  // $oneShot_0005
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0005 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0004 = I_rung_top;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0006) {  // $oneShot_0006
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 0, 0, 0, &U_USS_VAR);
                I_oneShot_0006 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0007 = I_rung_top;
        }
        if (!I_oneShot_0007) {  // $oneShot_0007
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0007 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0006 = I_rung_top;
    }
    
    GPIO_OUTPUT_PORT2 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 4 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0008) {  // $oneShot_0008
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, MODBUS_FC_READ_HOLDING_REGISTERS, 10, 1, &U_MB_485_VAR);
                I_oneShot_0008 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0009 = I_rung_top;
        }
        if (!I_oneShot_0009) {  // $oneShot_0009
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0009 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0008 = I_rung_top;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_000a) {  // $oneShot_000a
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 10, 1, &U_MB_485_VAR);
                I_oneShot_000a = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_000b = I_rung_top;
        }
        if (!I_oneShot_000b) {  // $oneShot_000b
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_000b = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_000a = I_rung_top;
    }
    
    GPIO_OUTPUT_PORT3 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 5 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_000c) {  // $oneShot_000c
            if (I_SerialReady) {  // $SerialReady
                Modbus_TCP_Send(1, MODBUS_FC_READ_HOLDING_REGISTERS, 0, 1, &U_MB_ETH_VAR);
                I_oneShot_000c = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_000d = I_rung_top;
        }
        if (!I_oneShot_000d) {  // $oneShot_000d
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_000d = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_000c = I_rung_top;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_000e) {  // $oneShot_000e
            if (I_SerialReady) {  // $SerialReady
                Modbus_TCP_Send(0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 0, 1, &U_Vnew);
                I_oneShot_000e = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_000f = I_rung_top;
        }
        if (!I_oneShot_000f) {  // $oneShot_000f
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_000f = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_000e = I_rung_top;
    }
    
    GPIO_OUTPUT_PORT4 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 6 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0010) {  // $oneShot_0010
            if (I_SerialReady) {  // $SerialReady
                Format_String_Read("value: %d\r\n", &U_STR_VAR);
                I_oneShot_0010 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0011 = I_rung_top;
        }
        if (!I_oneShot_0011) {  // $oneShot_0011
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0011 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0010 = I_rung_top;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0012) {  // $oneShot_0012
            if (I_SerialReady) {  // $SerialReady
                Format_String_Write("value: %d\r\n", &U_STR_VAR);
                I_oneShot_0012 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0013 = I_rung_top;
        }
        if (!I_oneShot_0013) {  // $oneShot_0013
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0013 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0012 = I_rung_top;
    }
    
    GPIO_OUTPUT_PORT5 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 7 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_rung_top = RS232_Write((unsigned char *)&U_CHAR_VAR, 1);
    }
    
    if (I_rung_top) {  // $rung_top
        I_rung_top = RS232_Read((unsigned char *)&U_CHAR_VAR, 1);
    }
    
    GPIO_OUTPUT_PORT6 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 8 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        RTC_Now = RTC_GetTime();
        I_rung_top = (127 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 21 && RTC_Now.Min == 50 && RTC_Now.Sec == 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_RSET5 = 1;
    }
    
    /* ] finish series */
    
    /* start rung 9 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RSET5) {  // RSET5
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT7 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 10 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_OUTPUT_PORT18) {  // YLED
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        I_scratch = 600;
        I_scratch2 = 2047;
        U_TISET_DA = I_scratch * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 11 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_OUTPUT_PORT18) {  // YLED
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        I_scratch = 600;
        I_scratch2 = 2047;
        U_TISET_DA = I_scratch * I_scratch2;
    }
    
    /* ] finish series */
}
