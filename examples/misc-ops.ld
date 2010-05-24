LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=2400

IO LIST
    Xon at 0
    Yalways at 0
    Ain at 0
END

PROGRAM
RUNG
    COMMENT Exemplos de deslocamento de bits, tabela de busca, relay de controle mestre.\r\n
END
RUNG
    CONTACTS Xon 0
    MASTER_RELAY
END
RUNG
    CONTACTS Rosc 0
    TON Ton 500000
    TOF Tof 500000
    COIL Rosc 1 0 0
END
RUNG
    CONTACTS Rosc 0
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
            CONTACTS Rosc 0
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
    COIL Yalways 0 0 0
END
