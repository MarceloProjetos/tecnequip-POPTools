LDmicro0.1
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
    COMMENT Very simple, just print 'Hello, world' over serial, over and over.
END
RUNG
    CONTACTS Rosc 0 0
    TON Tosc 1000000
    COIL Rosc 1 0 0 0
END
RUNG
    COMMENT For no particular reason, keep a counter that increments with each\r\nline that we print.
END
RUNG
    CONTACTS Rosc 0 0
    OSR
    ADD cnt cnt 1
END
RUNG
    CONTACTS Rosc 0 0
    FORMATTED_STRING cnt 21 72 101 108 108 111 44 32 119 111 114 108 100 58 32 92 45 53 92 114 92 110
END
