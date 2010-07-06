LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=6000000
BAUD=2400
COMPILED=C:\ldmicro\C51\C51.c

IO LIST
    X_run_stop at 9
    X_suiwei at 7
    X_wendu at 3
    Xprog at 8
    Y_baojing at 13
    Y_fanzhuan at 14
    Y_jiare at 15
    Y_jinsui at 16
    Y_paisui at 17
    Y_piaoxi at 18
    Y_tuosui at 2
    Y_xiyi at 19
    Y_zhengzhuan at 12
END

PROGRAM
RUNG
    CONTACTS Rb 1 0
    TON Tb0 500000
    TOF Tb1 500000
    COIL Rb 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS R_off 0 0
            CONTACTS Rb 1 0
        END
        CONTACTS R_baojing 0 0
    END
    COIL Y_baojing 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS X_suiwei 0 0
            CONTACTS Y_paisui 0 0
        END
        CONTACTS Y_jinsui 0 0
    END
    TON T_int1 200000000
    COIL R_baojing 0 1 0 0
END
RUNG
    CONTACTS X_run_stop 0 0
    OSF
    SHORT
    PARALLEL
        COIL R_run 1 0 0 0
        COIL RS 0 1 0 0
    END
END
RUNG
    PARALLEL
        CONTACTS R_off 0 0
        CONTACTS R_baojing 0 0
    END
    COIL R_run 0 0 1 0
END
RUNG
    CONTACTS Xprog 0 0
    CONTACTS RS 1 0
    CTC Cprog 7
END
RUNG
    PARALLEL
        EQU xiyi 0
        CONTACTS RXPT 0 0
    END
    LOOK_UP_TABLE xiyi Cprog 8 0 255 24 45 24 40 24 35 60
END
RUNG
    NEQ Cprog 0
    NEQ Cprog 2
    NEQ Cprog 4
    NEQ Cprog 6
    COIL RXPT 0 0 0 0
END
RUNG
    EQU Cprog 0
    CONTACTS Rb 1 0
    COIL Y_xiyi 0 0 0 0
END
RUNG
    PARALLEL
        CONTACTS Rb 1 0
        CONTACTS RXPT 0 0
        EQU Cprog 0
    END
    LEQ Cprog 6
    COIL Y_piaoxi 0 0 0 0
END
RUNG
    PARALLEL
        CONTACTS Rb 1 0
        CONTACTS RXPT 1 0
    END
    LES Cprog 8
    COIL Y_tuosui 0 0 0 0
END
RUNG
    CONTACTS RXPT 1 0
    CONTACTS R_run 0 0
    CONTACTS Y_paisui 1 0
    MASTER_RELAY
END
RUNG
    LES xiyi 46
    COIL R45 0 0 0 0
END
RUNG
    CONTACTS R0 1 0
    TON T_int2 4000000
    COIL R1 0 0 0 0
END
RUNG
    CONTACTS R0 1 0
    CONTACTS R45 0 0
    EQU T_int2 1
    SHORT
    PARALLEL
        MOVE T_int2 100
        SERIES
            LES xiyi 16
            MOVE T_int2 300
        END
    END
END
RUNG
    CONTACTS R1 0 0
    TON T_int3 11000000
    COIL R0 0 0 0 0
END
RUNG
    CONTACTS R1 0 0
    CONTACTS R45 0 0
    EQU T_int3 1
    SHORT
    PARALLEL
        MOVE T_int3 400
        SERIES
            LES xiyi 16
            MOVE T_int3 700
        END
    END
END
RUNG
    PARALLEL
        CONTACTS X_wendu 0 0
        CONTACTS R45 0 0
    END
    CONTACTS X_suiwei 0 0
    CONTACTS R0 0 0
    PARALLEL
        SUB xiyi xiyi 1
        SERIES
            EQU xiyi 0
            ADD Cprog Cprog 1
        END
    END
END
RUNG
    PARALLEL
        SERIES
            CONTACTS X_suiwei 1 0
            CONTACTS R45 0 0
            TOF T_int4 20000000
        END
        CONTACTS X_suiwei 1 0
    END
    TON Tjs 2550000
    COIL Y_jinsui 0 0 0 0
END
RUNG
    CONTACTS X_suiwei 0 0
    CONTACTS X_wendu 1 0
    CONTACTS R45 1 0
    COIL Y_jiare 0 0 0 0
END
RUNG
    MASTER_RELAY
END
RUNG
    CONTACTS RXPT 0 0
    CONTACTS R_run 0 0
    SHORT
    PARALLEL
        MASTER_RELAY
        SERIES
            TOF T_int5 10000000
            COIL Y_paisui 0 0 0 0
        END
    END
END
RUNG
    CONTACTS X_suiwei 1 0
    TON T_int6 10000000
    CONTACTS R00 1 0
    TON T01 2000000
    COIL R11 0 0 0 0
END
RUNG
    CONTACTS R11 0 0
    TON T_int7 3000000
    COIL R00 0 0 0 0
END
RUNG
    CONTACTS R00 0 0
    CTU Ctuosui 12
    SHORT
    PARALLEL
        COIL R22 0 0 0 0
        SERIES
            GRT Ctuosui xiyi
            PARALLEL
                RES Ctuosui
                SERIES
                    CTU Cprog 8
                    COIL R_off 0 1 0 0
                END
            END
        END
    END
END
RUNG
    MASTER_RELAY
END
RUNG
    CONTACTS Y_paisui 0 0
    COIL R010 0 0 1 0
END
RUNG
    CONTACTS R1 1 0
    CONTACTS R11 1 0
    CONTACTS R22 1 0
    OPEN
    CONTACTS R_run 0 0
    COIL R111 0 0 0 0
END
RUNG
    CONTACTS R111 0 0
    CONTACTS R010 1 0
    COIL Y_zhengzhuan 0 0 0 0
END
RUNG
    CONTACTS R111 0 0
    CONTACTS R010 0 0
    COIL Y_fanzhuan 0 0 0 0
END
RUNG
    CONTACTS R0 0 0
    COIL R010 1 0 0 0
END
