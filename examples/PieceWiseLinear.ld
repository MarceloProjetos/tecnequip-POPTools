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
    Posicao at 1 0 type 4096
    ZerarEncoder at 1 0 type 1
END

PROGRAM
RUNG
    COMMENT POPTools v1.4 - Tecnequip Tecnologia em Equipamentos Ltda.\r\nData: 10/08/2012
END
RUNG
    COMMENT Exemplo de utilização das instruções Linearização por Segmentos, Leitura de Encoder e Reser de Encoder.\r\nNeste exemplo atualizamos o D/A conforme o valor lido do encoder.
END
RUNG
    COMMENT A seguir utilizamos uma instrução Ler Encoder.\r\nO valor atual do encoder é carregado na variável Posicao.
END
RUNG
    READ_ENC Posicao
END
RUNG
    COMMENT Abaixo existe uma instrução Reset de Encoder.\r\nQuando ZerarEncoder estiver ativo, a instrução Reset de Encoder irá carregar o valor 0 no encoder.
END
RUNG
    CONTACTS ZerarEncoder 0 0
    RESET_ENC
END
RUNG
    COMMENT Abaixo utilizamos a Linearização por Segmentos.\r\nAtravés desta instrução convertermos o valor de Posicao para Velocidade.
END
RUNG
    COMMENT Neste objeto cadastramos os pontos X,Y de forma a gerar segmentos como em um gráfico.\r\nA conversão é realizada utilizando-se a referência X (variavel Posicao) para gerar o correspondente em Y (variavel Velocidade).
END
RUNG
    PIECEWISE_LINEAR Velocidade Posicao 5 0 5 1000 20 2000 40 3000 30 4000 -10
END
RUNG
    COMMENT Aqui carregamos no D/A o valor de Velocidade selecionado pela Linearização por Segmentos
END
RUNG
    SET_DA Velocidade 2
END
