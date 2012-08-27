POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=100000000
BAUD=9600
PARITY=0
MODBUSID=3
COM=0
IP=192.168.0.254
MASK=255.255.255.0
GW=192.168.0.1
DNS=192.168.0.1
SNTP=9-0:br.pool.ntp.org
X4=1
CANSAVE=0

IO LIST
    E1 at 1 0 type 1
    E2 at 2 0 type 1
    S1 at 1 0 type 2
    S2 at 2 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 13/08/2012
END
RUNG
    COMMENT Exemplo de utilização das instruções de ModBUS: Ler Registrador RS-485, Escrever Registrador RS-485, Ler Registrador Ethernet e Escrever Registrador Ethernet.\r\nNeste exemplo sincronizamos através das instruções citadas duas entradas e duas saídas, utilizando redundância para sistemas que devem ser tolerantes a falhas.
END
RUNG
    COMMENT Abaixo são ativados/desativados os bits da variável Entradas conforme estado atual.\r\nSe o resultante for diferente do último valor transmitido, é ativada a instrução Sincronizar.
END
RUNG
    PARALLEL
        SERIES
            CONTACTS E1 0 0
            SET_BIT Entradas 1 0
        END
        SERIES
            CONTACTS E1 1 0
            SET_BIT Entradas 0 0
        END
        SERIES
            CONTACTS E2 0 0
            SET_BIT Entradas 1 1
        END
        SERIES
            CONTACTS E2 1 0
            SET_BIT Entradas 0 1
        END
    END
    NEQ Entradas Atual
    PARALLEL
        COIL Sincronizar 0 1 0 0
        MOVE Atual Entradas
    END
END
RUNG
    COMMENT Quando Sincronizar está ativo, o valor de Atual é transmitido tanto por serial quanto por ethernet.\r\nQuando qualquer uma das instruções termina o envio, Sincronizar é desativado.
END
RUNG
    CONTACTS Sincronizar 0 0
    PARALLEL
        WRITE_MODBUS Atual 0 1 0 1
        WRITE_MODBUS_ETH Atual 0 1 0
    END
    COIL Sincronizar 0 0 1 0
END
RUNG
    COMMENT Aqui realizamos a leitura de um registrador remoto. o valor lido é salvo na variável Saídas.\r\nNote que aqui também utilizamos os comandos ethernet e rs-485 em paralelo. Essa redundância é importante em sistemas que devem ser tolerantes a falhas.
END
RUNG
    CONTACTS Esperar 1 0
    PARALLEL
        READ_MODBUS Saidas 0 2 0 1
        READ_MODBUS_ETH Saidas 0 2 0
    END
    TON ON 500000
    TOF OFF 500000
    COIL Esperar 0 0 0 0
END
RUNG
    COMMENT As duas linhas a seguir verificam se o bit em questão está ligado na variável.\r\nEm caso afirmativo, a saída relacionada é ativada.
END
RUNG
    CHECK_BIT Saidas 1 0
    COIL S1 0 0 0 0
END
RUNG
    CHECK_BIT Saidas 1 1
    COIL S2 0 0 0 0
END
