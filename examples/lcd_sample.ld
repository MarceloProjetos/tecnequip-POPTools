LDmicro0.1
MICRO=Microchip PIC16F877 40-PDIP
CYCLE=20000
CRYSTAL=4000000
BAUD=2400
COMPILED=C:\ipac_back\ladder\ladder\cummins.hex

IO LIST
    Y_latch_lcd at 0
END

PROGRAM
RUNG
    COMMENT program that controls a serial LCD and shows four variables at the same time\r\n varsamp1, varsamp2, varsamp3 and varsamp4
END
RUNG
    COMMENT Initializes Vel. Transmission, clean display and positions cursor at row1 col1
END
RUNG
    LOOK_UP_TABLE send_command command 17 0 13 254 1 254 12 254 128 254 192 254 132 254 140 254 196 254 204
END
RUNG
    TON T_delay_lcd 5000000
    COIL R_delay_lcd 0 0 0 0
END
RUNG
    COMMENT drives a transistor to prevent the command from reaching the LCD in the first\r\n 5 seconds, use this as there is a problem in ldmicro that prevents proper operation of serial lcd
END
RUNG
    CONTACTS R_delay_lcd 1 0
    COIL Y_latch_lcd 0 0 0 0
END
RUNG
    CONTACTS R_delay_lcd 0 0
    CONTACTS R_osc 1 0
    TOF T_ini1 70000
    TON T_ini2 250000
    COIL R_osc 0 0 0 0
END
RUNG
    CONTACTS R_delay_lcd 0 0
    CONTACTS R_osc 0 0
    OSR
    ADD command command 1
END
RUNG
    EQU command 7
    COIL R_row1 0 0 0 0
END
RUNG
    EQU command 9
    COIL R_row2 0 0 0 0
END
RUNG
    EQU command 10
    COIL R_reset_rows 0 1 0 0
END
RUNG
    EQU command 11
    PARALLEL
        MOVE send_var varsamp1
        COIL R_var1 0 0 0 0
    END
END
RUNG
    EQU command 13
    PARALLEL
        COIL R_var2 0 0 0 0
        MOVE send_var varsamp2
    END
END
RUNG
    EQU command 15
    PARALLEL
        COIL R_var3 0 0 0 0
        MOVE send_var varsamp3
    END
END
RUNG
    EQU command 17
    PARALLEL
        COIL R_var4 0 0 0 0
        MOVE send_var varsamp4
        SERIES
            TON T_loop 180000
            MOVE command 5
        END
    END
END
RUNG
    COMMENT redraw, if necessary
END
RUNG
    CONTACTS R_redraw_lcd 0 0
    PARALLEL
        OSR
        OSF
    END
    PARALLEL
        MOVE command 0
        COIL R_reset_rows 0 0 1 0
    END
END
RUNG
    COMMENT send commands to LCD (Init, clear, position etc)
END
RUNG
    CONTACTS R_osc 0 0
    OSR
    UART_SEND send_command
END
RUNG
    COMMENT send vars_name  to LCD,  row1 and row2
END
RUNG
    CONTACTS R_reset_rows 1 0
    CONTACTS R_row1 0 0
    FORMATTED_STRING (none) 12 86 97 114 49 32 32 32 32 86 97 114 50
END
RUNG
    CONTACTS R_reset_rows 1 0
    CONTACTS R_row2 0 0
    FORMATTED_STRING (none) 12 86 97 114 51 32 32 32 32 86 97 114 52
END
RUNG
    COMMENT send vars to LCD
END
RUNG
    PARALLEL
        CONTACTS R_var1 0 0
        CONTACTS R_var2 0 0
        CONTACTS R_var3 0 0
        CONTACTS R_var4 0 0
    END
    CONTACTS R_osc 0 0
    CONTACTS R_send_var_delay 1 0
    FORMATTED_STRING send_var 2 92 52
    COIL R_send_var_delay 0 0 0 0
END
