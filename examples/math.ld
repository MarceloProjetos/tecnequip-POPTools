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
    Acrescenta1 at 1 0 type 1
    Acrescenta10 at 2 0 type 1
    Acrescenta100 at 3 0 type 1
    Dividir at 5 0 type 1
    Multiplicar at 6 0 type 1
    Somar at 7 0 type 1
    Subtrair at 8 0 type 1
    Zerar at 4 0 type 1
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 10/08/2012
END
RUNG
    COMMENT Exemplo de utilização de instruções matemáticas.\r\nDemonstra-se o uso das instruções Somar, Subtrair, Multiplicar e Dividir pela implementação de uma calculadora.
END
RUNG
    COMMENT As linhas a seguir acrescentam respectivamente 1, 10 e 100 à variável valor.\r\nExiste o Borda de Subida para que some apenas 1 vez e não a cada ciclo enquanto a entrada estiver ligada.
END
RUNG
    CONTACTS Acrescenta1 0 0
    OSR
    ADD valor valor 1
END
RUNG
    CONTACTS Acrescenta10 0 0
    OSR
    ADD valor valor 10
END
RUNG
    CONTACTS Acrescenta100 0 0
    OSR
    ADD valor valor 100
END
RUNG
    COMMENT Abaixo está a lógica para zerar as variáveis resultado e valor.\r\nQuando Zerar for ativado, resultado recebe zero e ativamos ZerarValor para que a variável valor seja zerada também.
END
RUNG
    CONTACTS Zerar 0 0
    OSR
    PARALLEL
        MOVE resultado 0
        COIL ZerarValor 0 1 0 0
    END
END
RUNG
    COMMENT As quatro linha abaixo executam respectivamente as operações Somar, Subtrair, Multiplicar e Dividir. Também ativam ZerarValor para que a variável valor seja zerada.\r\nA instrução Borda de Subida impede que as operações sejam executadas continuamente enquanto a entrada correspondente estiver ativada.
END
RUNG
    CONTACTS Somar 0 0
    OSR
    PARALLEL
        ADD resultado resultado valor
        COIL ZerarValor 0 1 0 0
    END
END
RUNG
    CONTACTS Subtrair 0 0
    OSR
    PARALLEL
        SUB resultado resultado valor
        COIL ZerarValor 0 1 0 0
    END
END
RUNG
    CONTACTS Multiplicar 0 0
    OSR
    PARALLEL
        MUL resultado resultado valor
        COIL ZerarValor 0 1 0 0
    END
END
RUNG
    CONTACTS Dividir 0 0
    OSR
    PARALLEL
        DIV resultado resultado valor
        COIL ZerarValor 0 1 0 0
    END
END
RUNG
    COMMENT Nessa lógica a variável valor é zerada sempre que ZerarValor estiver ativado.\r\nA variável valor recebe 0 e o próprio ZerarValor se desativa para que a variável não continue sendo zerada a cada ciclo.
END
RUNG
    CONTACTS ZerarValor 0 0
    PARALLEL
        COIL ZerarValor 0 0 1 0
        MOVE valor 0
    END
END
