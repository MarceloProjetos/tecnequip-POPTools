LDmicro0.1
CYCLE=10000
CRYSTAL=4000000
BAUD=2400

IO LIST
    YTEMP_HI at 0
    YTEMP_HI_HI at 0
    Avar at 0
END

PROGRAM
RUNG
    CONTACTS RUpdate_L 1
    TON TUpdate_H 500000
    TOF TUpdate_L 500000
    COIL RUpdate_L 0 0 0
END
RUNG
    READ_ADC Avar
END
RUNG
    COMMENT TEMPERATURE ALARM HYSTERISIS
END
RUNG
    CONTACTS RUpdate_L 0
    FORMATTED_STRING Avar 14 84 101 109 112 61 92 45 53 39 67 92 114 92 110
    PARALLEL
        SERIES
            GEQ Avar 800
            COIL YTEMP_HI_HI 0 1 0
        END
        SERIES
            GEQ Avar 700
            COIL YTEMP_HI 0 1 0
        END
        SERIES
            LEQ Avar 750
            COIL YTEMP_HI_HI 0 0 1
        END
        SERIES
            LEQ Avar 650
            COIL YTEMP_HI 0 0 1
        END
    END
END
