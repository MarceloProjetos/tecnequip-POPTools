POPTools:1.2
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=9600
PARITY=0
COM=1
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
COMPILED=C:\Users\Alessandro\Documents\Visual Studio 2010\Projects\POPTools\examples\var-timer.hex

IO LIST
    Apot at 1 0
    Xin at 1 0
    Yout at 1 0
END

PROGRAM
RUNG
    COMMENT Exemplo de variavel de atraso com timer. Quando Rdel_input e setado, Rdel_output e setado \r\nt_max ciclos depois. Quando Rdel_input e resetado, Rdel_output e resetado imediatamente.
END
RUNG
    CONTACTS Rdel_input 0 0
    LEQ t_count t_max
    ADD t_count t_count 1
END
RUNG
    CONTACTS Rdel_input 1 0
    MOVE t_count 0
END
RUNG
    PARALLEL
        GRT t_count t_max
        SERIES
            CONTACTS Rdel_input 0 0
            EQU t_max 0
        END
    END
    COIL Rdel_output 0 0 0 0
END
RUNG
    READ_ADC Apot
END
RUNG
    DIV t_max Apot 2
END
RUNG
    CONTACTS Xin 0 0
    COIL Rdel_input 0 0 0 0
END
RUNG
    CONTACTS Rdel_output 0 0
    COIL Yout 0 0 0 0
END
