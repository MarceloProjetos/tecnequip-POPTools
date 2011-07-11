POPTools:1.2
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=9600
PARITY=0
COM=1
IP=192.168.0.251
MASK=255.255.255.0
GW=192.168.0.1
COMPILED=C:\Users\Tecnequip\Documents\adc_led.hex

IO LIST
    XAD1 at 1 0
    XAD2 at 2 0
    XAD3 at 3 0
    XAD4 at 4 0
    XAD5 at 5 0
    XAD6 at 6 0
    YS1 at 1 0
    YS2 at 2 0
    YS3 at 3 0
    YS4 at 4 0
    YS5 at 5 0
    YS6 at 6 0
    YS7 at 7 0
    YS8 at 8 0
    YS9 at 9 0
    YS10 at 10 0
    YS11 at 11 0
    YS12 at 12 0
    YS13 at 13 0
    YS14 at 14 0
    YS15 at 15 0
    YS16 at 16 0
    XMAD1 at 20 0
    XMAD2 at 20 1
    XMAD3 at 20 2
    XMAD4 at 20 3
    XMAD5 at 20 4
    XMAD6 at 20 5
    YADVAL at 21 0
    ADC1 at 1 0
    ADC2 at 2 0
    ADC3 at 3 0
    ADC4 at 4 0
    ADC5 at 5 0
    YLED at 18 0
    ADCT at 6 0
END

PROGRAM
RUNG
    COMMENT Código para piscar o led de CPU
END
RUNG
    CONTACTS YLED 1 0
    TON TON 500000
    TOF TOFF 500000
    COIL YLED 0 0 0 0
END
RUNG
    COMMENT Leitura dos A/Ds
END
RUNG
    PARALLEL
        READ_ADC ADC1
        READ_ADC ADC2
        READ_ADC ADC3
        READ_ADC ADC4
        READ_ADC ADC5
        READ_ADC ADCT
    END
END
RUNG
    COMMENT Seleção do A/D atual
END
RUNG
    MOVE ADC 0
END
RUNG
    PARALLEL
        CONTACTS XAD1 0 0
        CONTACTS XMAD1 0 0
    END
    MOVE ADC ADC1
END
RUNG
    PARALLEL
        CONTACTS XAD2 0 0
        CONTACTS XMAD2 0 1
    END
    MOVE ADC ADC2
END
RUNG
    PARALLEL
        CONTACTS XAD3 0 0
        CONTACTS XMAD3 0 2
    END
    MOVE ADC ADC3
END
RUNG
    PARALLEL
        CONTACTS XAD4 0 0
        CONTACTS XMAD4 0 3
    END
    MOVE ADC ADC4
END
RUNG
    PARALLEL
        CONTACTS XAD5 0 4
        CONTACTS XMAD5 0 4
    END
    MOVE ADC ADC5
END
RUNG
    PARALLEL
        CONTACTS XAD6 0 5
        CONTACTS XMAD6 0 5
    END
    MOVE ADC ADCT
END
RUNG
    COMMENT Lógica para criar um indicador com os leds de saída de forma a visualizar o valor do A/D atual
END
RUNG
    GRT ADC 0
    COIL YS1 0 0 0 0
END
RUNG
    GRT ADC 62
    COIL YS2 0 0 0 0
END
RUNG
    GRT ADC 125
    COIL YS3 0 0 0 0
END
RUNG
    GRT ADC 188
    COIL YS4 0 0 0 0
END
RUNG
    GRT ADC 250
    COIL YS5 0 0 0 0
END
RUNG
    GRT ADC 312
    COIL YS6 0 0 0 0
END
RUNG
    GRT ADC 375
    COIL YS7 0 0 0 0
END
RUNG
    GRT ADC 437
    COIL YS8 0 0 0 0
END
RUNG
    GRT ADC 500
    COIL YS9 0 0 0 0
END
RUNG
    GRT ADC 562
    COIL YS10 0 0 0 0
END
RUNG
    GRT ADC 625
    COIL YS11 0 0 0 0
END
RUNG
    GRT ADC 687
    COIL YS12 0 0 0 0
END
RUNG
    GRT ADC 750
    COIL YS13 0 0 0 0
END
RUNG
    GRT ADC 812
    COIL YS14 0 0 0 0
END
RUNG
    GRT ADC 875
    COIL YS15 0 0 0 0
END
RUNG
    GRT ADC 937
    COIL YS16 0 0 0 0
END
RUNG
    COMMENT Declaração de variável para ModBUS e atribuição do valor do A/D atual a ela
END
RUNG
    OSR
    COIL YADVAL 0 0 1 0
END
RUNG
    MOVE YADVAL ADC
END
