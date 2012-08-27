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
    NumeroPrimo at 1 0 type 2
    ValorDentroFaixa at 2 0 type 2
    ValorForaFaixa20 at 3 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 10/08/2012
END
RUNG
    COMMENT Exemplo de utilização de instruções condicionais.\r\nDemonstra-se o uso das instruções Igual, Diferente, Maior, Menor, Maior Ou Igual e Menor Ou Igual.
END
RUNG
    COMMENT A linha a seguir carrega 100 na variável valor.\r\nEsta é a variável que será utilizada para as comparações no restante do diagrama ladder.
END
RUNG
    MOVE valor 100
END
RUNG
    COMMENT Abaixo utilizamos dois condicionais: Menor e Maior Ou Igual.\r\nComo estão na sequência (série), ValorForaFaixa20 somente será ativado se valor for menor que 20 E maior ou igual a 30
END
RUNG
    LES valor 20
    GEQ valor 30
    COIL ValorForaFaixa20 0 0 0 0
END
RUNG
    COMMENT Abaixo utilizamos algumas instruções condicionais Igual em paralelo.\r\nSe qualquer um desses valores coincidir, NumeroPrimo será ativado
END
RUNG
    PARALLEL
        EQU valor 1
        EQU valor 3
        EQU valor 5
        EQU valor 7
        EQU valor 11
    END
    COIL NumeroPrimo 0 0 0 0
END
RUNG
    COMMENT Na linha abaixo utilizamos dois condicionais em série: Maior e Menor Ou Igual.\r\nQuando o valor estiver entre 21 e 30, ambos serão ativados, ligando a bobina ValorDentroFaixa.
END
RUNG
    GRT valor 20
    LEQ valor 30
    COIL ValorDentroFaixa 0 0 0 0
END
RUNG
    COMMENT Abaixo utilizamos o condicional Diferente para verificar se valor é diferente de zero, impedindo a divisão por zero.\r\nObservação: esse é apenas um exemplo. A instrução Dividir já faz essa checagem internamente.
END
RUNG
    NEQ valor 0
    DIV destino 100 valor
END
