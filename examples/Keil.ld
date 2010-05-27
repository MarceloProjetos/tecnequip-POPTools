LDmicro0.1
MICRO=Atmel AT89C2051 20-PDIP
CYCLE=10000
CRYSTAL=4000000
BAUD=2400

IO LIST
    X_run_stop at 0
    Y1 at 0
    Y2 at 0
    Y3 at 0
END

PROGRAM
RUNG
    CONTACTS X_run_stop 0
    OSF
    COIL R0 1 0 0
END
RUNG
    CONTACTS R0 0
    MASTER_RELAY
END
RUNG
    CONTACTS Rnew 0
    TON Tnew 1000000
    PARALLEL
        SERIES
            OPEN
            COIL Rnew 0 0 0
        END
        SERIES
            OSF
            COIL R01 1 0 0
        END
    END
END
RUNG
    CONTACTS R01 0
    SHORT
    PARALLEL
        CTC Cj 2
        COIL Y1 0 0 0
        SERIES
            GRT Cj 0
            PARALLEL
                COIL Y2 0 0 0
                SERIES
                    GRT Cj 1
                    COIL Y3 0 0 0
                END
            END
        END
    END
END
RUNG
    MASTER_RELAY
END
