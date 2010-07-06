LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=50000
CRYSTAL=20000000
BAUD=2400

IO LIST
    Xfaster at 0
    Xforward at 0
    Xreverse at 0
    Xslower at 0
    YcoilA_0 at 0
    YcoilA_1 at 0
    YcoilB_0 at 0
    YcoilB_1 at 0
END

PROGRAM
RUNG
    COMMENT The step rate is determined by the cycle time and speed variable. If the speed is equal to 20, then Rmaybe_step\r\nfires every cycle, so the step rate is 1/Tcycle. So the step rate is in general (speed/20)/Tcycle.
END
RUNG
    OSR
    MOVE speed 8
END
RUNG
    PARALLEL
        SERIES
            CONTACTS Xfaster 0 0
            OSR
            LES speed 20
            ADD speed speed 1
        END
        SERIES
            CONTACTS Xslower 0 0
            OSR
            GRT speed 0
            SUB speed speed 1
        END
    END
END
RUNG
    COMMENT Update the position of the motor, according to the current speed and direction.
END
RUNG
    PARALLEL
        ADD accum accum speed
        SERIES
            GRT accum 20
            PARALLEL
                SUB accum accum 20
                COIL Rmaybe_step 0 0 0 0
            END
        END
    END
END
RUNG
    PARALLEL
        SERIES
            CONTACTS Xforward 0 0
            CONTACTS Rmaybe_step 0 0
            ADD seq seq 1
        END
        SERIES
            CONTACTS Xreverse 0 0
            CONTACTS Rmaybe_step 0 0
            SUB seq seq 1
        END
    END
END
RUNG
    COMMENT Fix up the stepper position so that it's between 0 and 3 (inclusive).
END
RUNG
    PARALLEL
        SERIES
            GRT seq 3
            MOVE seq 0
        END
        SERIES
            LES seq 0
            MOVE seq 3
        END
    END
END
RUNG
    COMMENT The stepper motor is controlled by these four output pins. I am assuming a\r\nunipolar stepper, with low-side (NPN or n-FET) switches.
END
RUNG
    PARALLEL
        SERIES
            PARALLEL
                EQU seq 0
                EQU seq 1
            END
            COIL YcoilA_0 0 0 0 0
        END
        SERIES
            CONTACTS YcoilA_0 1 0
            COIL YcoilA_1 0 0 0 0
        END
        SERIES
            PARALLEL
                EQU seq 0
                EQU seq 3
            END
            COIL YcoilB_0 0 0 0 0
        END
        SERIES
            CONTACTS YcoilB_0 1 0
            COIL YcoilB_1 0 0 0 0
        END
    END
END
