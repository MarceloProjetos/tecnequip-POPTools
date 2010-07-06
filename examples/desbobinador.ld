LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=2400
COMPILED=C:\Users\Tecnequip\Documents\Desbobinador.hex

IO LIST
    XBT_AUTOMATICO at 5
    XBT_AVANCA at 6
    XBT_EXPANDE at 8
    XBT_RETORNA at 7
    XBT_RETRAI at 9
    XEMERGENCIA at 1
    XFIM_CURSO at 4
    XHIDRAULICA at 2
    XINVERSOR at 3
    XLINK_PERFILADEI at 10
    YAVANCA at 4
    YEXPANDE at 2
    YHIDRAULICA at 1
    YLINK at 6
    YRETORNA at 5
    YRETRAI at 3
END

PROGRAM
RUNG
    CONTACTS XEMERGENCIA 0 0
    CONTACTS XHIDRAULICA 0 0
    CONTACTS XINVERSOR 0 0
    COIL YLINK 0 0 0 0
END
RUNG
    CONTACTS XEMERGENCIA 0 0
    CONTACTS XINVERSOR 0 0
    PARALLEL
        SERIES
            PARALLEL
                SERIES
                    CONTACTS XBT_AUTOMATICO 1 0
                    CONTACTS XBT_AVANCA 0 0
                END
                SERIES
                    CONTACTS XBT_AUTOMATICO 0 0
                    CONTACTS XLINK_PERFILADEI 0 0
                    CONTACTS XFIM_CURSO 0 0
                END
            END
            COIL YAVANCA 0 0 0 0
        END
        SERIES
            CONTACTS XBT_AUTOMATICO 1 0
            CONTACTS XBT_RETORNA 0 0
            COIL YRETORNA 0 0 0 0
        END
    END
END
RUNG
    CONTACTS XEMERGENCIA 0 0
    CONTACTS XHIDRAULICA 0 0
    PARALLEL
        SERIES
            CONTACTS XBT_AUTOMATICO 1 0
            CONTACTS XBT_EXPANDE 0 0
            COIL YEXPANDE 0 0 0 0
        END
        SERIES
            CONTACTS XBT_AUTOMATICO 1 0
            CONTACTS XBT_RETRAI 0 0
            COIL YRETRAI 0 0 0 0
        END
    END
END
RUNG
    PARALLEL
        CONTACTS YEXPANDE 0 0
        CONTACTS YRETRAI 0 0
    END
    COIL YHIDRAULICA 0 0 0 0
END
