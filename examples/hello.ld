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
COMPILED=C:\Users\Alessandro\Documents\Visual Studio 2010\Projects\POPTools\examples\hello.hex

IO LIST
END

PROGRAM
RUNG
    COMMENT Exemplo onde se envia a string "Hello, world" a cada segundo pela serial.\r\nInclui ainda um contador
END
RUNG
    CONTACTS Rosc 0 0
    TON Tosc 1000000
    COIL Rosc 1 0 0 0
END
RUNG
    CONTACTS Rosc 0 0
    OSR
    ADD cnt cnt 1
END
RUNG
    CONTACTS Rosc 0 0
    WRITE_FORMATTED_STRING cnt Hello, world: %d
END
