LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=9600
PARITY=0
COM=1
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
COMPILED=C:\Users\Alessandro\Documents\Visual Studio 2010\Projects\POPTools\examples\timers.hex

IO LIST
    Xdown at 1
    Xres at 2
    Xup at 3
    Ydown at 2
    Ypulse at 1
    Yup at 3
END

PROGRAM
RUNG
    COMMENT Exemplo de timers (TON, TOF, RTO), contadores (CTC, CTU, CTD) e o reset (RES). 
END
RUNG
    CONTACTS Rosc 0 0
    TOF Tof 100000
    TON Ton 100000
    COIL Rosc 1 0 0 0
END
RUNG
    CONTACTS Rosc 0 0
    CONTACTS Xup 0 0
    CTU Ccnt 20
    RTO Trto 10000000
    COIL Yup 0 0 0 0
END
RUNG
    CONTACTS Rosc 0 0
    CONTACTS Xdown 0 0
    CTD Ccnt 10
    COIL Ydown 0 0 0 0
END
RUNG
    CONTACTS Xres 0 0
    PARALLEL
        OSR
        OSF
    END
    RES Trto
END
RUNG
    CONTACTS Rosc 0 0
    CTC Ccirc 7
END
RUNG
    EQU Ccirc 3
    OSF
    TOF Tpulse 200000
    COIL Ypulse 0 0 0 0
END
