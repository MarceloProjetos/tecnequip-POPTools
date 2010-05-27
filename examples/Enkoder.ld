LDmicro0.1
MICRO=Atmel AVR ATmega128 64-TQFP
CYCLE=2000
CRYSTAL=8000000
BAUD=9600
COMPILED=C:\Documents and Settings\labozidar\My Documents\kkk.hex

IO LIST
    X_A at 19
    X_B at 18
END

PROGRAM
RUNG
    PARALLEL
        SERIES
            CONTACTS X_A 0
            PARALLEL
                SERIES
                    OSR
                    CONTACTS X_B 0
                END
                SERIES
                    OSF
                    CONTACTS X_B 1
                END
            END
        END
        SERIES
            CONTACTS X_B 0
            PARALLEL
                SERIES
                    OSR
                    CONTACTS X_A 1
                END
                SERIES
                    OSF
                    CONTACTS X_A 0
                END
            END
        END
    END
    SUB Enkoder Enkoder 1
END
RUNG
    PARALLEL
        SERIES
            CONTACTS X_A 0
            PARALLEL
                SERIES
                    OSR
                    CONTACTS X_B 1
                END
                SERIES
                    OSF
                    CONTACTS X_B 0
                END
            END
        END
        SERIES
            CONTACTS X_B 0
            PARALLEL
                SERIES
                    OSR
                    CONTACTS X_A 0
                END
                SERIES
                    OSF
                    CONTACTS X_A 1
                END
            END
        END
    END
    ADD Enkoder Enkoder 1
END
RUNG
    COMMENT Pokazivanje stanja na displeju
END
RUNG
    CONTACTS R_osc 0
    TON T_osc 100000
    TOF T_oosc 100000
    COIL R_osc 1 0 0
END
RUNG
    CONTACTS R_osc 0
    OSF
    FORMATTED_STRING Enkoder 22 80 111 115 105 116 105 111 110 32 32 32 92 45 52 92 120 70 69 92 120 48 50
    OSF
    FORMATTED_STRING Text 26 32 32 32 69 110 107 111 100 101 114 32 32 32 32 32 32 92 49 92 120 70 69 92 120 67 48
END
RUNG
    MOVE Text Text
END
