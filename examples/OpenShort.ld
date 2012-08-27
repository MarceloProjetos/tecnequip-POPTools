POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=100000000
BAUD=9600
PARITY=0
MODBUSID=0
COM=0
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
DNS=192.168.0.1
SNTP=9-0:br.pool.ntp.org
X4=1
CANSAVE=0

IO LIST
    AcendeDuranteDia at 3 0 type 1
    AtivarLogica at 1 0 type 1
    Lampada at 1 0 type 2
    ModoCrianca at 5 0 type 1
    PermitePulso at 4 0 type 1
    SensorLumin at 2 0 type 1
    UsarAtraso at 6 0 type 1
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 09/08/2012
END
RUNG
    COMMENT Exemplo de utilização de instruções adicionais de I/O: chave Geral, Circuito Fechado e Circuito Aberto.\r\nÉ demonstrado como desativar trechos de lógica utilizando as instruções citadas, sem a necessidade de excluir lógica que pode ser usada posteriormente.
END
RUNG
    COMMENT Na linha abaixo, quando AtivarLogica estiver ligado, Chave Geral também está e tudo funciona normalmente.\r\nQuando AtivarLogica estiver desligado, a Chave Geral é desligada e toda lógica até a próxima Chave Geral é desativada também.
END
RUNG
    CONTACTS AtivarLogica 0 0
    MASTER_RELAY
END
RUNG
    COMMENT A linha a seguir demonstra a utilidade da instrução Circuito Fechado para desabilitar um trecho de lógica.\r\nO sinal de Acender chega a Lampada através do Circuito Fechado ao invés de ser necessário remover toda a lógica.
END
RUNG
    CONTACTS Acender 0 0
    PARALLEL
        SERIES
            PARALLEL
                CONTACTS SensorLumin 1 0
                SERIES
                    PARALLEL
                        CONTACTS AcendeDuranteDia 0 0
                        SERIES
                            OSR
                            CONTACTS PermitePulso 0 0
                        END
                    END
                    CONTACTS ModoCrianca 1 0
                END
            END
            PARALLEL
                TON Espera 100000
                CONTACTS UsarAtraso 1 0
            END
        END
        SHORT
    END
    CONTACTS Desligar 1 0
    COIL Lampada 0 0 0 0
END
RUNG
    COMMENT Abaixo encontra-se outra Chave Geral.\r\nNeste ponto a Chave Geral é ligada novamente e as lógicas seguintes são executadas normalmente.
END
RUNG
    MASTER_RELAY
END
RUNG
    COMMENT Abaixo utilizamos um Circuito Aberto para desativar um outro trecho de lógica.\r\nNesse caso a lógica foi desativada de forma que o sinal não chegue até Desligar.
END
RUNG
    CONTACTS Lampada 0 0
    TON PiscaON 5000000
    TOF PiscaOFF 5000000
    OPEN
    COIL Desligar 0 0 0 0
END
