POPTools:1.2
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=2400
PARITY=0
COM=1
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
COMPILED=C:\Users\Alessandro\Documents\Visual Studio 2010\Projects\POPTools\examples\contacts.hex

IO LIST
    Xa at 2 0
    Xb at 3 0
    Xc at 4 0
    Xd at 5 0
    Xe at 6 0
    Yf at 11 0
    Yg at 7 0
    Yh at 12 0
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
