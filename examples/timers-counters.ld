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
    AtivarRTO at 1 0 type 1
    ZerarRTO at 2 0 type 1
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 08/08/2012
END
RUNG
    COMMENT Exemplo de utilização de contadores e timers.\r\nA partir da base de tempo definido pela variável ciclo, um contador é incrementado e então decrementado. Existe também um timer retentivo que desativa o ciclo.
END
RUNG
    COMMENT A lógica a seguir usa dois temporizadores: um de Atraso Para Ligar e um Atraso Para Desligar.\r\nO atraso para ligar aguarda o tempo indicado para ativar a saída. O atraso para desligar somente desliga a saída após passado o tempo indicado
END
RUNG
    CONTACTS DesligaCiclo 1 0
    CONTACTS Ciclo 1 0
    TON ON 500000
    TOF OFF 500000
    COIL Ciclo 0 0 0 0
END
RUNG
    COMMENT A cada vez que Ciclo é ativado, se Decrementar também está ativado o contador é decrementado em 1.\r\nQuando o contador ficar menor que 0, Decrementar é desligado e a lógica passa a incrementar o outro contador.
END
RUNG
    CONTACTS Ciclo 0 0
    CONTACTS Decrementar 0 0
    CTD Contador 0
    COIL Decrementar 0 0 1 0
END
RUNG
    COMMENT A cada vez que Ciclo é ativado, se Decrementar está desativado o contador é incrementado em 1.\r\nQuando o contador atingir o valor 10, Decrementar é ligado e a lógica passa a decrementar o outro contador.
END
RUNG
    CONTACTS Ciclo 0 0
    CONTACTS Decrementar 1 0
    CTU Contador 10
    COIL Decrementar 0 1 0 0
END
RUNG
    COMMENT A lógica a seguir ativa o contador circular sempre que Ciclo é ativado.\r\nO contador circular incrementa seu valor até o valor limite e então reinicia a contagem na próxima vez em que é ativado.
END
RUNG
    CONTACTS Ciclo 0 0
    CTC Circular 5
END
RUNG
    COMMENT A seguir é utilizado um temporizador retentivo, isto é, o seu contador não é zerado automaticamente quando sua entrada é desligada.\r\nAssim, mesmo que a sua entrada seja desligada, o contador permanece no valor atual, inclusive mantendo ativa a sua saída se já atingiu o tempo limite.
END
RUNG
    CONTACTS AtivarRTO 0 0
    RTO RET 5000000
    COIL DesligaCiclo 0 0 0 0
END
RUNG
    COMMENT Na lógica a seguir, quando ZerarRTO é ativado, o valor do temporizador retentivo é zerado.\r\nEsta é a única forma de se zerar o valor do temporizador retentivo
END
RUNG
    CONTACTS ZerarRTO 0 0
    RES RET
END
