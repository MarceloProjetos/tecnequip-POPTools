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
SNTP=9-0:pool.ntp.org
X4=1
CANSAVE=0

IO LIST
    YS1 at 1 0 type 2
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 08/08/2012
END
RUNG
    COMMENT Exemplo de utilização do D/A (Conversor Digital -> Analógico).\r\nAtravés de variável que é incrementada a cada 100 ms, o D/A oscila entre -10V e +10V
END
RUNG
    COMMENT A lógica a seguir oscila a saída a cada 100ms\r\nEssa é a base de tempo utilizada para atualização do D/A
END
RUNG
    CONTACTS YS1 1 0
    TON TLEDON 100000
    TOF TLEDOF 100000
    COIL YS1 0 0 0 0
END
RUNG
    COMMENT Quando a saída S1 é ativada, a variável DA é acrescida de 250.\r\nPara que a variável não seja atualizada a cada ciclo enquanto S1 estiver ativada, usa-se o detector de borda de subida.
END
RUNG
    CONTACTS YS1 0 0
    OSR
    ADD DA DA 250
END
RUNG
    COMMENT O D/A da POP-7 pode variar entre -10V e +10V.\r\nAssim verificamos se o limite superior foi ultrapassado e, nesse caso, atualizamos a variável para o limite inferior.
END
RUNG
    GRT DA 10000
    MOVE DA -10000
END
RUNG
    COMMENT A seguir o D/A é atualizado.\r\nEle está configurado para receber o valor em mV, realizando a conversão para o valor direto de 12 bits internamente.
END
RUNG
    SET_DA DA 1
END
