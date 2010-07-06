LDmicro0.1
MICRO=Microchip PIC16F876 28-PDIP or 28-SOIC
CYCLE=10000
CRYSTAL=20000000
BAUD=2400

IO LIST
    Xbutton at 14
    Yled at 15
END

PROGRAM
RUNG
    CONTACTS Rosc 0 0
    TON Tosc_on 250000
    TOF Tosc_off 250000
    COIL Rosc 1 0 0 0
END
RUNG
    CONTACTS Xbutton 0 0
    CTC Cstate 2
END
RUNG
    PARALLEL
        EQU Cstate 1
        SERIES
            EQU Cstate 2
            CONTACTS Rosc 0 0
        END
    END
    COIL Yled 0 0 0 0
END
