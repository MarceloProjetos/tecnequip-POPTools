LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=2400
COMPILED=C:\Users\Tecnequip\Documents\timers-counters.hex

IO LIST
    Xdown at 0
    Xres at 0
    Xup at 0
    Ydown at 0
    Ypulse at 0
    Yup at 0
END

PROGRAM
RUNG
    COMMENT Exemplo de timers (TON, TOF, RTO), contadores (CTC, CTU, CTD) e o reset (RES). 
END
RUNG
    CONTACTS Rosc 0
    TOF Tof 100000
    TON Ton 100000
    COIL Rosc 1 0 0
END
RUNG
    CONTACTS Rosc 0
    CONTACTS Xup 0
    CTU Ccnt 20
    RTO Trto 10000000
    COIL Yup 0 0 0
END
RUNG
    CONTACTS Rosc 0
    CONTACTS Xdown 0
    CTD Ccnt 10
    COIL Ydown 0 0 0
END
RUNG
    CONTACTS Xres 0
    PARALLEL
        OSR
        OSF
    END
    RES Trto
END
RUNG
    CONTACTS Rosc 0
    CTC Ccirc 7
END
RUNG
    EQU Ccirc 3
    OSF
    TOF Tpulse 200000
    COIL Ypulse 0 0 0
END
