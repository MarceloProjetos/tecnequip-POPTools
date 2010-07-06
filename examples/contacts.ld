LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=2400

IO LIST
    Xa at 2
    Xb at 3
    Xc at 4
    Xd at 5
    Xe at 6
    Yf at 11
    Yg at 7
    Yh at 12
END

PROGRAM
RUNG
    COMMENT Teste de todos os tipos de contatos (NO, NC), e todos os tipos de bobinas (normal, negado, ligado,\r\ndesligado).
END
RUNG
    CONTACTS Xa 0 0
    COIL Yf 0 0 0 0
END
RUNG
    PARALLEL
        CONTACTS Xb 0 0
        CONTACTS Xc 0 0
    END
    COIL Yg 0 1 0 0
END
RUNG
    CONTACTS Xd 0 0
    CONTACTS Xe 1 0
    PARALLEL
        COIL Yg 0 0 1 0
        COIL Yh 1 0 0 0
    END
END
