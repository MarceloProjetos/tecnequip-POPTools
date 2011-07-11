POPTools:1.2
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
    YS1 at 1 0
    YS10 at 10 0
    YS11 at 11 0
    YS12 at 12 0
    YS13 at 13 0
    YS14 at 14 0
    YS15 at 15 0
    YS16 at 16 0
    YS2 at 2 0
    YS3 at 3 0
    YS4 at 4 0
    YS5 at 5 0
    YS6 at 6 0
    YS7 at 7 0
    YS8 at 8 0
    YS9 at 9 0
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
