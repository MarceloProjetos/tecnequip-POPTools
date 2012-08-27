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
    Ligado at 1 0 type 2
    LigarBit0 at 1 0 type 1
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 09/08/2012
END
RUNG
    COMMENT Exemplo de utilização de instruções de Variáveis.\r\nDemonstra-se o uso das instruções Mover Valor, Liga/Desliga Bit, Checar Bit e Persistente.
END
RUNG
    COMMENT A linha a seguir contém a instrução Mover Valor.\r\nÉ uma atribuição, onde o valor indicado em Origem é salvo na variável indicada.
END
RUNG
    MOVE valor 100
END
RUNG
    COMMENT Abaixo utilizamos uma instrução Liga/Desliga Bit.\r\nSe LigarBit0 estiver ativado, o bit 0 da variável indicada será ativado.
END
RUNG
    CONTACTS LigarBit0 0 0
    SET_BIT valor 1 0
END
RUNG
    COMMENT A linha a seguir utiliza uma instrução Checar Bit para verificar se determinado bit da variável está ligado.\r\nEm caso afirmativo, a saída da instrução é ativada, ligando a bobina Ligado.
END
RUNG
    CHECK_BIT valor 1 0
    COIL Ligado 0 0 0 0
END
RUNG
    COMMENT A linha a seguir utiliza a instrução Persistente. Ao ligar o CLP, a variável indicada é carregada com o valor gravado na memória interna.\r\nDurante a execução, sempre que valor é alterado, ele é gravado na memória interna para que seja recuperado caso o CLP seja desligado.
END
RUNG
    PERSIST valor
END
