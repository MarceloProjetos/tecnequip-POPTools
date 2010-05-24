LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=2400
COMPILED=C:\Users\Tecnequip\Documents\var-timer.hex

IO LIST
    Xin at 0
    Yout at 0
    Apot at 0
END

PROGRAM
RUNG
    COMMENT Exemplo de variavel de atraso com timer. Quando Rdel_input e setado, Rdel_output e setado \r\nt_max ciclos depois. Quando Rdel_input e resetado, Rdel_output e resetado imediatamente.
END
RUNG
    CONTACTS Rdel_input 0
    LEQ t_count t_max
    ADD t_count t_count 1
END
RUNG
    CONTACTS Rdel_input 1
    MOVE t_count 0
END
RUNG
    PARALLEL
        GRT t_count t_max
        SERIES
            CONTACTS Rdel_input 0
            EQU t_max 0
        END
    END
    COIL Rdel_output 0 0 0
END
RUNG
    READ_ADC Apot
END
RUNG
    DIV t_max Apot 2
END
RUNG
    CONTACTS Xin 0
    COIL Rdel_input 0 0 0
END
RUNG
    CONTACTS Rdel_output 0
    COIL Yout 0 0 0
END
