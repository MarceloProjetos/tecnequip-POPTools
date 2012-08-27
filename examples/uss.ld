POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=100000000
BAUD=19200
PARITY=1
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
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 10/08/2012
END
RUNG
    COMMENT Exemplo de utilização das instruções Leitura USS e Escrita USS.\r\nNeste exemplo o valor do parâmetro 103 é lido e posteriormente gravado no parâmetro 102.
END
RUNG
    COMMENT Nessa linha a variável Valor é carregada com 100.\r\nDevido à instrução Borda de Subida, essa atribuição ocorrerá apenas no primeiro ciclo de execução do diagrama ladder.
END
RUNG
    OSR
    MOVE Valor 100
END
RUNG
    COMMENT Na linha abaixo utilizamos as instruções Leitura USS e Escrita USS.\r\nApós uma espera inicial de 3 segundos, ativará Escrita USS e em seguida a Leitura USS. A lógica irá então aguardar 2 segundos antes de reiniciar o ciclo.
END
RUNG
    COMMENT Em Escrita USS, o conteúdo de valor é gravado no parâmetro 102.\r\nEm Leitura USS, o conteúdo do parâmetro 103 é lido e armazenado na variável valor.
END
RUNG
    TON EspInit 3000000
    CONTACTS Aguardar 1 0
    WRITE_USS Valor 0 102 0 0
    READ_USS Valor 0 103 0 0
    TON ON 1000000
    TOF OFF 1000000
    COIL Aguardar 0 0 0 0
END
