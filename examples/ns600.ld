POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=19200
PARITY=4
MODBUSID=0
COM=0
IP=192.168.0.235
MASK=255.255.255.0
GW=192.168.0.10
DNS=192.168.0.1
SNTP=9-0:br.pool.ntp.org
X4=1
CANSAVE=0

IO LIST
    MaqConfigPos at 1 0 type 1
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 13/08/2012
END
RUNG
    COMMENT Exemplo de utilização das instruções Leitura de NS-600 Yaskawa e Escrita de NS-600 Yaskawa.\r\nNeste exemplo realizamos a escrita de um valor no encoder, aguardando a resposta do comando.
END
RUNG
    COMMENT Aqui utilizamos uma instrução Leitura de NS-600 Yaskawa para receber a resposta do equipamento.\r\nNessa lógica se recebe retorno de um comando pela serial, carregando o retorno em AplanRetSerial. Se menor que 0, indica erro na recepção da resposta
END
RUNG
    CONTACTS MaqLerOKSerial 0 0
    TON EspRetSerial 50000
    READ_SERVO_YASKAWA 0 AplanRetSerial OK
    PARALLEL
        COIL MaqLerOKSerial 0 0 1 0
        SERIES
            LES AplanRetSerial 0
            COIL MaqCfgSrvErro 0 1 0 0
        END
    END
END
RUNG
    COMMENT Aqui iniciamos a operação para configuração do valor do encoder no NS-600.\r\nSe não há erro e a serial não está ocupada, ativa StartConfigPos e marca a serial como ocupada.
END
RUNG
    CONTACTS MaqConfigPos 0 0
    CONTACTS MaqCfgSrvErro 1 0
    CONTACTS MaqSerialBusy 1 0
    PARALLEL
        COIL StartConfigPos 0 1 0 0
        COIL MaqSerialBusy 0 1 0 0
    END
END
RUNG
    COMMENT A seguir utilizamos a instrução Escrita no NS-600 Yaskawa. A string com o comando ZSET será formatada, substituindo-se o %d pelo valor de MaqNovaPosicao.\r\nApós a transmissão, ativa-se MaqLerSerialOK para aguardar a resposta do comando, quando desativamos a nossa lógica e liberamos a serial.
END
RUNG
    CONTACTS StartConfigPos 0 0
    WRITE_SERVO_YASKAWA 0 MaqNovaPosicao ZSET%d
    PARALLEL
        SERIES
            OSR
            COIL MaqLerOKSerial 0 1 0 0
        END
        SERIES
            CONTACTS MaqLerOKSerial 1 0
            PARALLEL
                COIL StartConfigPos 0 0 1 0
                COIL MaqSerialBusy 0 0 1 0
            END
        END
    END
END
