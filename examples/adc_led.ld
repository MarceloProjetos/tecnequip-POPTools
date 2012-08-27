POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=9600
PARITY=0
MODBUSID=0
COM=0
IP=192.168.0.251
MASK=255.255.255.0
GW=192.168.0.1
DNS=192.168.0.1
SNTP=9-0:br.pool.ntp.org
X4=1
CANSAVE=0

IO LIST
    ADC1 at 1 0 type 4
    ADC2 at 2 0 type 4
    ADC3 at 3 0 type 4
    ADC4 at 4 0 type 4
    ADC5 at 5 0 type 4
    ADCT at 6 0 type 4
    ADVAL at 21 0 type 2048
    XAD1 at 1 0 type 1
    XAD2 at 2 0 type 1
    XAD3 at 3 0 type 1
    XAD4 at 4 0 type 1
    XAD5 at 5 0 type 1
    XAD6 at 6 0 type 1
    XMAD1 at 20 0 type 1
    XMAD2 at 20 1 type 1
    XMAD3 at 20 2 type 1
    XMAD4 at 20 3 type 1
    XMAD5 at 20 4 type 1
    XMAD6 at 20 5 type 1
    YS1 at 1 0 type 2
    YS10 at 10 0 type 2
    YS11 at 11 0 type 2
    YS12 at 12 0 type 2
    YS13 at 13 0 type 2
    YS14 at 14 0 type 2
    YS15 at 15 0 type 2
    YS16 at 16 0 type 2
    YS2 at 2 0 type 2
    YS3 at 3 0 type 2
    YS4 at 4 0 type 2
    YS5 at 5 0 type 2
    YS6 at 6 0 type 2
    YS7 at 7 0 type 2
    YS8 at 8 0 type 2
    YS9 at 9 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 13/08/2012
END
RUNG
    COMMENT Exemplo de Aplicação - Barra de Leds com valor lido do A/D.\r\nNeste exemplo são utilizadas as saídas digitais para criar uma barra de leds conforme o valor lido do A/D.
END
RUNG
    COMMENT Leitura dos A/Ds.\r\nA seguir são realizadas leituras em todos os canais analógicos.
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
    COMMENT Seleção do A/D atual\r\nConforme a Entrada Digital ou Bit selecionado pelo ModBUS é escolhido qual canal será utilizado.
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
    COMMENT Lógica para criar um indicador com os leds de saída de forma a visualizar o valor do A/D atual.\r\nQuanto maior o valor, mais saídas são ligadas. Assim cria-se o efeito de uma barra progressiva.
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
    COMMENT Declaração de variável para ModBUS e atribuição do valor do A/D atual a ela.\r\nA variável ADVAL é associada ao registrador M1 do ModBUS, permitindo assim a leitura do valor do A/D atual remotamente.
END
RUNG
    MOVE ADVAL ADC
END
