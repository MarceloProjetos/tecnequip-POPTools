LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=100000000
BAUD=9600
PARITY=1
COM=4
IP=192.168.0.235
MASK=255.255.255.0
GW=192.168.0.10
COMPILED=C:\Users\Tecnequip\Documents\enc.hex

IO LIST
    XResetEnc at 1
    YS1 at 1
    YS2 at 2
    YS3 at 3
    YS4 at 4
    EValEnc at 1
    ZResetVal at 1
END

PROGRAM
RUNG
    READ_ENC EValEnc
END
RUNG
    GRT EValEnc 100
    COIL YS1 0 0 0 0
END
RUNG
    LES EValEnc 0
    COIL YS2 0 0 0 0
END
RUNG
    GRT EValEnc 50000
    COIL YS3 0 0 0 0
END
RUNG
    LES EValEnc -100
    COIL YS4 0 0 0 0
END
RUNG
    CONTACTS XResetEnc 0 0
    OSR
    RESET_ENC ZResetVal
END
