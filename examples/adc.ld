POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
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
    AD at 3 0 type 4
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 07/08/2012
END
RUNG
    COMMENT Exemplo de utilização da instrução de leitura A/D (Conversor Analógico -> Digital)\r\nEsta lógica transmite uma string com o valor atual do A/D pela interface RS-485 a cada dois segundos
END
RUNG
    COMMENT A linha abaixo serve para criar um relé que fica alternando a cada 1s.\r\nAssim, ele fica 1 segundo em nível alto e mais 1 segundo em nível baixo
END
RUNG
    CONTACTS OSC 1 0
    TON TON 1000000
    TOF TOFF 1000000
    COIL OSC 0 0 0 0
END
RUNG
    COMMENT Agora o valor do A/D é lido.\r\nA cada ciclo o A/D carrega o valor atual na variável AD
END
RUNG
    READ_ADC AD
END
RUNG
    COMMENT A seguir está a lógica de transmissão da mensagem.\r\nQuando OSC ligar, o comando seguinte (Escrita de string formatada) faz a transmissão do texto pela RS-485
END
RUNG
    CONTACTS OSC 0 0
    WRITE_FORMATTED_STRING AD AD: %d\r\n
END
