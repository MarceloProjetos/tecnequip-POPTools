POPTools:1.2
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=9600
PARITY=0
COM=1
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
COMPILED=C:\Users\Alessandro\Documents\Visual Studio 2010\Projects\POPTools\examples\misc-ops.hex

IO LIST
    Ain at 1 0
    Xon at 1 0
    Yalways at 1 0
END

PROGRAM
RUNG
    COMMENT Exemplos de deslocamento de bits, tabela de busca, relay de controle mestre.\r\n
END
RUNG
    CONTACTS Xon 0 0
    MASTER_RELAY
END
RUNG
    CONTACTS Rosc 0 0
    TON Ton 500000
    TOF Tof 500000
    COIL Rosc 1 0 0 0
END
RUNG
    CONTACTS Rosc 0 0
    SHIFT_REGISTER reg 7
END
RUNG
    PARALLEL
        READ_ADC Ain
        MOVE reg0 Ain
    END
END
RUNG
    PARALLEL
        SERIES
            CONTACTS Rosc 0 0
            CTC Ci 4
        END
        LOOK_UP_TABLE duty Ci 5 0 10 70 80 70 10
        SET_PWM duty 10000
    END
END
RUNG
    MASTER_RELAY
END
RUNG
    COIL Yalways 0 0 0 0
END
