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
    AtivarMotor at 1 0 type 1
    UsarPWM at 2 0 type 1
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 10/08/2012
END
RUNG
    COMMENT Exemplo de utilização das instruções Saída PWM e Rampa de Aceleração/Desaceleração.\r\nNeste exemplo simulamos a aceleração de um motor através de controle analógico (Rampa) ou PWM.
END
RUNG
    COMMENT A linha a seguir utiliza uma instrução Rampa de Aceleração/Desaceleração.\r\nSe AtivaMotor estiver ligado e UsarPWM desligado, Será gerada a rampa de aceleração conforme definido
END
RUNG
    CONTACTS AtivarMotor 0 0
    CONTACTS UsarPWM 1 0
    MULTISET_DA 600 100 1 1 1 600 100 20 5
END
RUNG
    COMMENT Abaixo carregamos o valor 100 em LarguraMax.\r\nEssa variável é utilizada para checar se estamos excedendo o limite do PWM.
END
RUNG
    MOVE LarguraMax 100
END
RUNG
    COMMENT A lógica a seguir serve para gerar um cilo de 1 segundo.\r\nA variável Largura é então incrementada em 5 a cada intervalo de tempo.
END
RUNG
    CONTACTS AtivarMotor 0 0
    CONTACTS UsarPWM 0 0
    CONTACTS Ciclo 0 0
    TON ON 500000
    TOF OFF 500000
    PARALLEL
        COIL Ciclo 0 0 0 0
        SERIES
            OSR
            ADD Largura Largura 5
        END
    END
END
RUNG
    COMMENT Aqui checamos se o limite foi ultrapassado.\r\nSe Largura for maior que LarguraMax, então Largura será carregado com o valor de LarguraMax.
END
RUNG
    GRT Largura LarguraMax
    MOVE Largura LarguraMax
END
RUNG
    COMMENT Aqui utilizamos a instrução Saída PWM.\r\nQuando AtivarMotor estiver ligado e UsarPWM também estiver ligado, será gerada uma onda PWM com Largura e a frequência selecionados no pino atribuído.
END
RUNG
    CONTACTS AtivarMotor 0 0
    CONTACTS UsarPWM 0 0
    SET_PWM Largura 1000
END
