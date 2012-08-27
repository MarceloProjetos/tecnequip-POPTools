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
    ComunicacaoOK at 1 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 13/08/2012
END
RUNG
    COMMENT Exemplo de utilização das instruções Leitura de Caracter e Escrita de Caracter.\r\nA cada 0,5 segundo é enviado um caracter escolhido em uma lista. Se o eco for igual ao caracter enviado, ativa saída indicando que a comunicação está OK.
END
RUNG
    COMMENT Na lógica a seguir criamos uma variável Ciclo que irá ligar e desligar a cada 0,5 segundo.\r\nA cada ciclo, um contador circular (Indice) é incrementado
END
RUNG
    CONTACTS Ciclo 1 0
    TOF ON 250000
    TON OFF 250000
    PARALLEL
        COIL Ciclo 0 0 0 0
        CTC Indice 5
    END
END
RUNG
    COMMENT A seguir carregamos o valor de char.\r\nA instrução utilizada (Tabela de Busca) contém um texto onde seleciono um caracter na posicao de Indice.
END
RUNG
    LOOK_UP_TABLE char Indice 6 1 65 66 67 68 69 70
END
RUNG
    COMMENT Quando ciclo é ativado, enviamos um caracter pela serial e então um caracter é lido na sequência.\r\nSe for igual, consideramos que a comunicação está OK.
END
RUNG
    CONTACTS Ciclo 0 0
    UART_SEND char
    UART_RECV retorno
    EQU char retorno
    COIL ComunicacaoOK 0 0 0 0
END
