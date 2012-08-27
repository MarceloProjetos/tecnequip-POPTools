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
    Entrada at 1 0 type 1
    Saida at 1 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 09/08/2012
END
RUNG
    COMMENT Exemplo de uso de comentário\r\nInstruções sobre como inserir um comentário no diagrama ladder
END
RUNG
    COMMENT Para inserir um comentário, a linha deve estar vazia, ou seja, sem qualquer outra instrução.\r\nA linha abaixo aceita a inserção de um comentário.
END
RUNG
    PLACEHOLDER
END
RUNG
    COMMENT Já a linha abaixo não permite inclusão de comentário
END
RUNG
    CONTACTS Entrada 0 0
    COIL Saida 0 0 0 0
END
