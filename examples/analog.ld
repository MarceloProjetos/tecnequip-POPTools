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
COMPILED=C:\Users\Alessandro\Documents\Visual Studio 2010\Projects\POPTools\examples\analog.hex

IO LIST
    YTEMP_HI at 1
    YTEMP_HI_HI at 2
    Avar at 1
END

PROGRAM
RUNG
    CONTACTS RUpdate_L 1 0
    TON TUpdate_H 500000
    TOF TUpdate_L 500000
    COIL RUpdate_L 0 0 0 0
END
RUNG
    READ_ADC Avar
END
RUNG
    COMMENT TEMPERATURE ALARM HYSTERISIS
END
RUNG
    CONTACTS RUpdate_L 0 0
    FORMATTED_STRING Avar 14 84 101 109 112 61 92 45 53 39 67 92 114 92 110
    PARALLEL
        SERIES
            GEQ Avar 800
            COIL YTEMP_HI_HI 0 1 0 0
        END
        SERIES
            GEQ Avar 700
            COIL YTEMP_HI 0 1 0 0
        END
        SERIES
            LEQ Avar 750
            COIL YTEMP_HI_HI 0 0 1 0
        END
        SERIES
            LEQ Avar 650
            COIL YTEMP_HI 0 0 1 0
        END
    END
END
