LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=100000000
BAUD=9600
PARITY=0
COM=3
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
COMPILED=C:\Users\Tecnequip\Documents\saidas.hex

IO LIST
    YS1 at 1
    YS10 at 10
    YS11 at 11
    YS12 at 12
    YS13 at 13
    YS14 at 14
    YS15 at 15
    YS16 at 16
    YS2 at 2
    YS3 at 3
    YS4 at 4
    YS5 at 5
    YS6 at 6
    YS7 at 7
    YS8 at 8
    YS9 at 9
END

PROGRAM
RUNG
    CONTACTS RTMR 1 0
    TON TON 500000
    TOF TOFF 500000
    COIL RTMR 0 0 0 0
END
RUNG
    CONTACTS RTMR 0 0
    PARALLEL
        COIL YS1 0 0 0 0
        COIL YS3 0 0 0 0
        COIL YS5 0 0 0 0
        COIL YS7 0 0 0 0
        COIL YS9 0 0 0 0
        COIL YS11 0 0 0 0
        COIL YS13 0 0 0 0
        COIL YS15 0 0 0 0
    END
END
RUNG
    CONTACTS RTMR 1 0
    PARALLEL
        COIL YS2 0 0 0 0
        COIL YS4 0 0 0 0
        COIL YS6 0 0 0 0
        COIL YS8 0 0 0 0
        COIL YS10 0 0 0 0
        COIL YS12 0 0 0 0
        COIL YS14 0 0 0 0
        COIL YS16 0 0 0 0
    END
END
