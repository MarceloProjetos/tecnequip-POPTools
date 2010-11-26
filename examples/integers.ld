LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=50000
CRYSTAL=10000000
BAUD=2400
PARITY=0
COM=1
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
COMPILED=C:\Users\Alessandro\Documents\Visual Studio 2010\Projects\POPTools\examples\integer.hex

IO LIST
    Xa at 1
    Yno at 16
    Yok at 1
END

PROGRAM
RUNG
    COMMENT Exemplo de manipulacao de variaveis inteiras: mover, calcular e comparacao. 
END
RUNG
    PARALLEL
        SERIES
            CONTACTS Xa 0 0
            MOVE a 1323
        END
        SERIES
            CONTACTS Xa 1 0
            MOVE a -12
        END
    END
END
RUNG
    PARALLEL
        ADD c a 1000
        SUB d a 5
        MUL e a 3
        DIV f e 3
    END
END
RUNG
    PARALLEL
        EQU a 1323
        SHORT
    END
    PARALLEL
        LES c 0
        LEQ d 1318
        SHORT
    END
    SHORT
    PARALLEL
        GRT c 20000
        GEQ e 3969
        OPEN
    END
    COIL Yok 0 0 0 0
END
RUNG
    NEQ e -36
    GRT a 0
    TON Ton 1000000
    COIL Yno 0 0 0 0
END
