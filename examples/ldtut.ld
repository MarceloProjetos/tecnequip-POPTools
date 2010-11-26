LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=20000000
BAUD=9600
PARITY=0
COM=1
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
COMPILED=C:\Users\Alessandro\Documents\Visual Studio 2010\Projects\POPTools\examples\çdutils.hex

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
