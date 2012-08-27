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
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 10/08/2012
END
RUNG
    COMMENT Exemplo de utilização da instrução Tabela de Busca\r\nNeste exemplo atualizamos o D/A com o valor indicado na posição selecionada.
END
RUNG
    COMMENT A lógica abaixo utiliza um relé interno para criar um sinal liga/desliga.\r\nEsse sinal alterna entre alto e baixo a cada 0,5 segundos.
END
RUNG
    CONTACTS Ciclo 1 0
    TON ON 500000
    TOF OFF 500000
    COIL Ciclo 0 0 0 0
END
RUNG
    COMMENT Agora utilizamos um contador circular para gerar os valores em index.\r\nA variável index será utlizada para selecionar uma posição na Tabela de Busca.
END
RUNG
    CONTACTS Ciclo 0 0
    OSR
    CTC index 4
END
RUNG
    COMMENT Abaixo utilizamos a Tabela de Busca para carregar a variável Velocidade.\r\nO valor carregado é selecionado pela variável de índice (index).
END
RUNG
    LOOK_UP_TABLE Velocidade index 5 0 -30 -10 0 50 100
END
RUNG
    COMMENT Aqui carregamos no D/A o valor de Velocidade selecionado pela Tabela de Busca.
END
RUNG
    SET_DA Velocidade 2
END
