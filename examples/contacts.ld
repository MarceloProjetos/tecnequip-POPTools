POPTools:1.4
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=2400
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
    SaidaInvertida at 1 0 type 2
    SaidaNormal at 2 0 type 2
    SaidaSetReset at 3 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 07/08/2012
END
RUNG
    COMMENT Exemplo de utilização de contatos, relés internos e bobinas.\r\nContatos NA/NF, Bobinas invertidas, Set e Reset
END
RUNG
    COMMENT A lógica abaixo utiliza um relé interno para criar um sinal liga/desliga.\r\nEsse sinal alterna entre alto e baixo a cada 1 segundo.
END
RUNG
    CONTACTS ReleInterno 1 0
    TON ON 1000000
    TOF OFF 1000000
    COIL ReleInterno 0 0 0 0
END
RUNG
    COMMENT Abaixo existem duas bobinas. Uma delas (SaidaNormal) liga e desliga conforme o estado do contato (Entrada).\r\nA outra bobina (SaidaInvertida) usa o sinal invertido, ficando ligada quando o contato estiver desligado e vice-versa.
END
RUNG
    CONTACTS Entrada 0 0
    PARALLEL
        COIL SaidaNormal 0 0 0 0
        COIL SaidaInvertida 1 0 0 0
    END
END
RUNG
    COMMENT A lógica abaixo inicia com contato usando o sinal NF. Assim, quando a entrada associada estiver desligada, sua saída fica ativada, acionando a bobina.\r\nPor sua vez, a bobina usa o modo Set, ligando a saída quando ativada. A saída não é desligada ao desativar o sinal.
END
RUNG
    CONTACTS Entrada 1 0
    COIL SaidaSetReset 0 1 0 0
END
RUNG
    COMMENT Abaixo o contato usa o sinal de ReleInterno para ativar a bobina (SaidaSetReset).\r\nQuando ativada a bobina, a saída é desligada. Quando a bobina não está ativa, nada acontece.
END
RUNG
    CONTACTS ReleInterno 0 0
    COIL SaidaSetReset 0 0 1 0
END
