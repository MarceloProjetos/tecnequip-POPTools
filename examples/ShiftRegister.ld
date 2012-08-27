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
    registro0 at 1 0 type 4
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 10/08/2012
END
RUNG
    COMMENT Exemplo de utilização da instrução Tabela de Troca\r\nNeste exemplo calculamos a média dos cinco últimos valores lidos do A/D.
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
    COMMENT A seguir utilizamos a instrução Tabela de Troca. Ela foi configurada para ter 5 passos.\r\nAutomaticamente são criadas 5 variáveis: registro0 a registro4. Estas variáveis armazenam os valores que são adicionados na Tabela de Troca.
END
RUNG
    COMMENT Sempre que a instrução Tabela de Troca é ativada, o valor da última variável (no caso registro4) é descartada.\r\nA seguir todos os elementos são deslocados como em uma fila: registro4 recebe o valor de registro3, registro3 recebe o valor de registro2, etc.
END
RUNG
    COMMENT A seguir fazemos uma leitura no A/D, armazenando o valor lido em registro0.\r\nAssim, sempre que Ciclo é ativado, os valores na Tabela de Troca são deslocados e registro0 é atualizado com o novo valor lido.
END
RUNG
    CONTACTS Ciclo 0 0
    OSR
    PARALLEL
        SHIFT_REGISTER registro 5
        READ_ADC registro0
    END
END
RUNG
    COMMENT As instruções a seguir realizam a soma de todos os elementos da Tabela de Troca.\r\nAo final a variável soma contém a soma dos elementos registro0 a registro4.
END
RUNG
    PARALLEL
        ADD soma registro0 registro1
        ADD soma soma registro2
        ADD soma soma registro3
        ADD soma soma registro4
    END
END
RUNG
    COMMENT Agora calculamos a média dos valores contidos na Tabela de Troca.\r\nPara isso basta dividirmos a variável soma por 5. O resultado é armazenado na variável media.
END
RUNG
    DIV media soma 5
END
