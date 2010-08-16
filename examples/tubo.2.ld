LDmicro0.1
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=4000000
BAUD=9600
COMPILED=C:\Users\Tecnequip\Documents\tubo.hex

IO LIST
    XCortarCanto at 15
    XCorteTuboManual at 19
    XEmergencia at 11
    XErroDesbob at 12
    XFaltaFase at 18
    XInvFimPosic at 1
    XInversorOK at 3
    XMbCorteIHM at 22
    XMbModoAuto at 23
    XPerfFimMaterial at 5
    XPerfManAvanca at 13
    XPerfManRecua at 14
    XPressaoArOK at 4
    XSnsInfCrtCanto at 17
    XSnsInfHidr at 7
    XSnsInfSerra at 9
    XSnsSupCrtCanto at 16
    XSnsSupHidr at 6
    XSnsSupSerra at 8
    XTermBombaHidr at 2
    YAvancarCrtCanto at 15
    YAvancarPerf at 7
    YBombaCrtCanto at 14
    YHidrDesce at 2
    YHidrSobe at 3
    YInvFreioMotor at 6
    YInvPosicao at 1
    YInvResetPos at 10
    YLigarBombaHidr at 17
    YLigarSerra at 13
    YMbAutoAcel at 32
    YMbAutoDesacel at 33
    YMbAutoVel at 34
    YMbEncFator at 40
    YMbEncPerim at 42
    YMbEncResol at 41
    YMbErrBombaHidr at 20
    YMbErrDesbob at 20
    YMbErrEmergencia at 20
    YMbErrFaltaFase at 20
    YMbErrInvComunic at 20
    YMbErrInversor at 20
    YMbFacaTam at 45
    YMbLimpaErro at 21
    YMbManAcel at 35
    YMbManDesacel at 36
    YMbManVel at 37
    YMbPerfFatorH at 31
    YMbPerfFatorL at 30
    YMbQtd at 50
    YMbSerraTam at 46
    YMbTam at 51
    YMbTipoCorte at 47
    YRecuarCrtCanto at 16
    YRecuarPerf at 8
    YSerraDesce at 11
    YSerraMordDir at 4
    YSerraMordEsq at 5
    YSerraParar at 12
END

PROGRAM
RUNG
    COMMENT Programa de Controle da Perfiladeira de Tubo - v1.0\r\nDesenvolvido por Marcelo Utikawa da Fonseca em 23/07/2010
END
RUNG
    COMMENT Descricao das flags utilizadas no programa:
END
RUNG
    COMMENT AvancarPerfMan\r\n   Ativa quando acionado botao de avanco manual da perfiladeira e a maquina em modo manual
END
RUNG
    COMMENT BloqueiaRecuo\r\n   Se ficar acionado o recuo manual da perfiladeira por mais que 3 segundos, desliga a perfiladeira
END
RUNG
    COMMENT BombaHidrOK\r\n   Indica que a bomba hidraulica esta ligada e estabilizada, podendo assim realizar um ciclo de corte
END
RUNG
    COMMENT CortarTubo\r\n   Indica que deve ser realizado um ciclo de corte do tubo. Flag geral, independente da origem da solicitacao (manual, automatica, IHM)
END
RUNG
    COMMENT CortarTuboAuto\r\n   Flag gerada pelo loop de producao do modo automatico, solicitando o corte do tubo
END
RUNG
    COMMENT CortePronto\r\n   Ativo quando a maquina esta pronta para realizar um corte, seja pela serra ou pelo modo hidraulico
END
RUNG
    COMMENT CorteTuboAtivo\r\n   Flag indicando que um ciclo de corte esta sendo realizado
END
RUNG
    COMMENT CorteTuboDesce\r\n   Ativo quando, durante um ciclo de corte, a maquina esta realizando o movimento de descida, seja pela serra ou pelo modo hidraulico
END
RUNG
    COMMENT CrtCantoAtivo\r\n   Indica que esta sendo realizado um ciclo da ferramenta de corte de canto. Desativa apenas quando operador soltar o pedal de comando
END
RUNG
    COMMENT CrtCantoDesce\r\n   Ativo quando, durante um ciclo da ferramenta de corte de canto, a maquina esta realizando o movimento de descida
END
RUNG
    COMMENT CrtCantoOK\r\n   Indica que a bomba hidraulica do corte de canto esta ligada e estabilizada, podendo assim realizar um ciclo de corte
END
RUNG
    COMMENT ErroCorteTubo\r\n   Caso ocorra um erro durante o ciclo de corte do tubo, esta flag e ativada. Desativa automaticamente quando desligar o comando de corte manual ou encerrar a producao automatica
END
RUNG
    COMMENT ErroGeral\r\n   Erro grave na maquina: emergencia, falta de fase
END
RUNG
    COMMENT FerramCorteCima\r\n   Indica que as ferramentas de corte de tudo (serra e hidraulica) estao na posicao superior, permitindo o movimento do perfil
END
RUNG
    COMMENT InvFreioLib\r\n   Ativo quando o freio da perfiladeira estiver ligado e liberado, permitindo o movimento do perfil
END
RUNG
    COMMENT MaqConfigOK\r\n   Indica que a maquina foi configurada e pode iniciar a producao automatica. Ativo apenas apos a transmissao de todos os parametros ao inversor
END
RUNG
    COMMENT MaqFimPosic\r\n   Ativo apos o inversor terminar o posicionamento. Desativa apos o reset da medida do encoder
END
RUNG
    COMMENT MaqIniciarPosic\r\n   Ativo durante o posicionamento do perfil no modo automatico. Desativa apos o termino do posicionamento
END
RUNG
    COMMENT MaqOK\r\n   Indica que a maquina esta sem qualquer erro e pode operar
END
RUNG
    COMMENT MaqProduzindo\r\n   Indica que a maquina esta no loop de producao. Somente desativa caso ocorrer algum erro, desativar a producao ou terminar a quantidade de pecas programada
END
RUNG
    COMMENT ModoAuto\r\n   Flag ativada pelo ModBUS para configurar o modo automatico e iniciar o loop de producao
END
RUNG
    COMMENT ModoAutoOK\r\n   Indica que a maquina esta pronta para operar no modo automatico, ou seja, sem erros e com o modo automatico ativado
END
RUNG
    COMMENT ModoCorteSerra\r\n   Ativo quando selecionado para realizar o ciclo de corte usando a Serra ao inves do Corte Hidraulico
END
RUNG
    COMMENT PerfManualOK\r\n   Flag que indica que a perfiladeira esta pronta para movimentar em modo manual
END
RUNG
    COMMENT RecuarPerfMan\r\n   Ativa quando acionado botao de recuo manual da perfiladeira e a maquina em modo manual
END
RUNG
    COMMENT SerraOK\r\n   Indica que a serra esta ligada e estabilizada, podendo assim realizar um ciclo de corte
END
RUNG
    COMMENT SnsInfCorteTubo\r\n   Representa o estado do sensor inferior do tipo de corte selecionado
END
RUNG
    COMMENT SnsSupCorteTubo\r\n   Representa o estado do sensor superior do tipo de corte selecionado
END
RUNG
    COMMENT Descricao dos Timers utilizados no programa:
END
RUNG
    COMMENT BombaHidrEstab\r\n   Espera para estabilizar a pressao hidraulica apos ligar a bomba de corte do tubo
END
RUNG
    COMMENT BombaHidrInat\r\n   Se depois de 5 minutos nao houve nenhum ciclo de corte, a bomba e desligada automaticamente
END
RUNG
    COMMENT CrtCantoEstab\r\n   Espera para estabilizar a pressao hidraulica apos ligar a bomba de corte de canto
END
RUNG
    COMMENT EsperaFreioLib\r\n   Aguarda a liberacao mecanica do freio da perfiladeira apos ele ser ligado
END
RUNG
    COMMENT FimPosicEstab\r\n   Espera o sinal de fim de posicionamento ficar alto por pelo menos 1 segundo, indicando que a maquina realmente terminou o posicionamento
END
RUNG
    COMMENT MaxCorteCanto\r\n   Indica que depois de 10 segundos o ciclo de corte da ferramenta de canto nao terminou
END
RUNG
    COMMENT MaxCorteTubo\r\n   Indica que depois de 15 segundos o ciclo de corte da ferramenta de tubo nao terminou
END
RUNG
    COMMENT MaxPerfRecua\r\n   Tempo maximo permitido para a perfiladeira recuar no modo manual, evitando problemas com o perfil
END
RUNG
    COMMENT SerraEstab\r\n   Espera para estabilizar a serra apos ser ligada
END
RUNG
    COMMENT SerraInat\r\n   Se depois de 5 minutos nao houve nenhum ciclo de corte, a serra e desligada automaticamente
END
RUNG
    COMMENT MantemFreioLib\r\n   Depois que o movimento manual da perfiladeira e desligado, mantem-se o freio ativo para permitir a maquina desacelerar antes de ativar o freio novamente
END
RUNG
    COMMENT PulsoSerraPara\r\n   Mantem o sinal de parada rapida da serra ativo por 1 segundo, garantindo que ela receba o sinal
END
RUNG
    COMMENT Descricao dos Contadores utilizados no programa:
END
RUNG
    COMMENT QtdProd\r\n   Contagem de pecas que restam produzir antes de finalizar o loop de producao
END
RUNG
    COMMENT Checagens gerais da Maquina
END
RUNG
    PARALLEL
        CONTACTS XEmergencia 1 0
        CONTACTS XFaltaFase 1 0
    END
    COIL RErroGeral 0 0 0 0
END
RUNG
    CONTACTS RErroGeral 1 0
    CONTACTS XInversorOK 0 0
    CONTACTS XPressaoArOK 0 0
    PARALLEL
        CONTACTS RModoCorteSerra 0 0
        CONTACTS XTermBombaHidr 0 0
    END
    COIL RMaqOK 0 0 0 0
END
RUNG
    COMMENT Logica de controle da ferramenta de corte de canto
END
RUNG
    CONTACTS RErroGeral 1 0
    CONTACTS XCortarCanto 0 0
    CONTACTS RCrtCantoAtivo 1 0
    PARALLEL
        COIL RCrtCantoAtivo 0 1 0 0
        COIL YBombaCrtCanto 0 1 0 0
    END
END
RUNG
    CONTACTS YBombaCrtCanto 0 0
    TON TCrtCantoEstab 1500000
    PARALLEL
        SERIES
            OSR
            COIL RCrtCantoDesce 0 1 0 0
        END
        COIL RCrtCantoOK 0 0 0 0
    END
END
RUNG
    CONTACTS RCrtCantoOK 0 0
    PARALLEL
        TON TMaxCorteCanto 10000000
        CONTACTS RErroGeral 0 0
    END
    PARALLEL
        COIL YBombaCrtCanto 0 0 1 0
        COIL RCrtCantoDesce 0 0 1 0
    END
END
RUNG
    CONTACTS RCrtCantoOK 0 0
    CONTACTS RCrtCantoDesce 0 0
    PARALLEL
        COIL YAvancarCrtCanto 0 0 0 0
        SERIES
            CONTACTS XSnsInfCrtCanto 0 0
            COIL RCrtCantoDesce 0 0 1 0
        END
    END
END
RUNG
    CONTACTS RCrtCantoOK 0 0
    CONTACTS RCrtCantoDesce 1 0
    PARALLEL
        COIL YRecuarCrtCanto 0 0 0 0
        SERIES
            CONTACTS XSnsSupCrtCanto 0 0
            COIL YBombaCrtCanto 0 0 1 0
        END
    END
END
RUNG
    CONTACTS XCortarCanto 1 0
    CONTACTS RCrtCantoAtivo 0 0
    CONTACTS YBombaCrtCanto 1 0
    COIL RCrtCantoAtivo 0 0 1 0
END
RUNG
    COMMENT Logica de controle do corte do tubo - Logicas Gerais
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RModoCorteSerra 0 0
            CONTACTS XSnsSupSerra 0 0
        END
        SERIES
            CONTACTS RModoCorteSerra 1 0
            CONTACTS XSnsSupHidr 0 0
        END
    END
    COIL RSnsSupCorteTubo 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RModoCorteSerra 0 0
            CONTACTS XSnsInfSerra 0 0
        END
        SERIES
            CONTACTS RModoCorteSerra 1 0
            CONTACTS XSnsInfHidr 0 0
        END
    END
    COIL RSnsInfCorteTubo 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RModoCorteSerra 0 0
            CONTACTS RSerraOK 0 0
        END
        SERIES
            CONTACTS RModoCorteSerra 1 0
            CONTACTS RBombaHidrOK 0 0
        END
    END
    COIL RCortePronto 0 0 0 0
END
RUNG
    CONTACTS XSnsSupSerra 0 0
    CONTACTS XSnsSupHidr 0 0
    CONTACTS XSnsInfSerra 1 0
    CONTACTS XSnsInfHidr 1 0
    COIL RFerramCorteCima 0 0 0 0
END
RUNG
    COMMENT Logica de controle do corte do tubo - Controle da Bomba Hidraulica
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RModoCorteSerra 1 0
    CONTACTS YLigarBombaHidr 1 0
    CONTACTS RCorteTuboAtivo 0 0
    COIL YLigarBombaHidr 0 1 0 0
END
RUNG
    CONTACTS RModoCorteSerra 1 0
    CONTACTS YLigarBombaHidr 0 0
    TON TBombaHidrEstab 5000000
    COIL RBombaHidrOK 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RBombaHidrOK 0 0
            CONTACTS RCorteTuboAtivo 1 0
            TON TBombaHidrInat 300000000
        END
        CONTACTS RModoCorteSerra 0 0
        CONTACTS RMaqOK 1 0
    END
    COIL YLigarBombaHidr 0 0 1 0
END
RUNG
    COMMENT Logica de controle do corte do tubo - Controle da Serra
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RModoCorteSerra 0 0
    CONTACTS YLigarSerra 1 0
    CONTACTS RCorteTuboAtivo 0 0
    COIL YLigarSerra 0 1 0 0
END
RUNG
    CONTACTS RModoCorteSerra 0 0
    CONTACTS YLigarSerra 0 0
    TON TSerraEstab 5000000
    COIL RSerraOK 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RSerraOK 0 0
            CONTACTS RCorteTuboAtivo 1 0
            TON TSerraInat 300000000
        END
        CONTACTS RModoCorteSerra 1 0
        CONTACTS RMaqOK 1 0
    END
    COIL YLigarSerra 0 0 1 0
END
RUNG
    COMMENT Logica de controle do corte do tubo - Operacao de Corte
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RErroCorteTubo 1 0
    CONTACTS RCortarTubo 0 0
    CONTACTS RCorteTuboAtivo 1 0
    PARALLEL
        COIL RCorteTuboAtivo 0 1 0 0
        COIL RCorteTuboDesce 0 1 0 0
        RES TBombaHidrInat
        RES TSerraInat
    END
END
RUNG
    CONTACTS RCorteTuboAtivo 0 0
    PARALLEL
        SERIES
            CONTACTS RCortePronto 0 0
            TON TMaxCorteTubo 15000000
        END
        CONTACTS RMaqOK 1 0
    END
    PARALLEL
        COIL RErroCorteTubo 0 1 0 0
        COIL RCorteTuboDesce 0 0 1 0
        COIL RCorteTuboAtivo 0 0 1 0
    END
END
RUNG
    CONTACTS RCorteTuboAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS RCorteTuboDesce 0 0
    CONTACTS RSnsInfCorteTubo 0 0
    COIL RCorteTuboDesce 0 0 1 0
END
RUNG
    CONTACTS RCorteTuboAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS RCorteTuboDesce 1 0
    CONTACTS RSnsSupCorteTubo 0 0
    COIL RCorteTuboAtivo 0 0 1 0
END
RUNG
    COMMENT Logica de controle do corte do tubo - Mapeamento entre reles internos para entradas e saidas
END
RUNG
    CONTACTS RCorteTuboAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS RCorteTuboDesce 0 0
    PARALLEL
        SERIES
            CONTACTS RModoCorteSerra 0 0
            COIL YSerraDesce 0 1 0 0
        END
        SERIES
            CONTACTS RModoCorteSerra 1 0
            COIL YHidrDesce 0 0 0 0
        END
    END
END
RUNG
    CONTACTS RCorteTuboAtivo 0 0
    CONTACTS RCortePronto 0 0
    CONTACTS RCorteTuboDesce 1 0
    PARALLEL
        SERIES
            CONTACTS RModoCorteSerra 0 0
            COIL YSerraDesce 0 0 1 0
        END
        SERIES
            CONTACTS RModoCorteSerra 1 0
            COIL YHidrSobe 0 0 0 0
        END
    END
END
RUNG
    PARALLEL
        CONTACTS RErroCorteTubo 0 0
        CONTACTS RModoCorteSerra 1 0
    END
    COIL YSerraDesce 0 0 1 0
END
RUNG
    CONTACTS RMaqOK 0 0
    PARALLEL
        SERIES
            CONTACTS RModoAuto 1 0
            CONTACTS XPerfManAvanca 1 0
            CONTACTS XPerfManRecua 1 0
            PARALLEL
                SERIES
                    CONTACTS RModoCorteSerra 1 0
                    CONTACTS XCorteTuboManual 0 0
                END
                CONTACTS XMbCorteIHM 0 0
            END
        END
        SERIES
            CONTACTS RModoAuto 0 0
            CONTACTS RMaqProduzindo 0 0
            CONTACTS RCortarTuboAuto 0 0
        END
    END
    COIL RCortarTubo 0 0 0 0
END
RUNG
    CONTACTS RCorteTuboAtivo 0 0
    CONTACTS RModoCorteSerra 1 0
    PARALLEL
        COIL YSerraMordEsq 0 0 0 0
        COIL YSerraMordDir 0 0 0 0
    END
END
RUNG
    CONTACTS RErroCorteTubo 0 0
    CONTACTS RModoCorteSerra 0 0
    OSR
    TOF TPulsoSerraPara 1000000
    COIL YSerraParar 0 0 0 0
END
RUNG
    CONTACTS RErroCorteTubo 0 0
    CONTACTS RCortarTubo 1 0
    CONTACTS RMaqProduzindo 1 0
    COIL RErroCorteTubo 0 0 1 0
END
RUNG
    COMMENT Controle da Perfiladeira - Logica de Saidas
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RModoAuto 0 0
            CONTACTS RMaqIniciarPosic 0 0
        END
        SERIES
            PARALLEL
                CONTACTS RAvancarPerfMan 0 0
                CONTACTS RRecuarPerfMan 0 0
            END
            TOF TMantemFreioLib 2000000
        END
    END
    CONTACTS RMaqOK 0 0
    CONTACTS RCorteTuboAtivo 1 0
    COIL YInvFreioMotor 0 0 0 0
END
RUNG
    CONTACTS YInvFreioMotor 0 0
    TON TEsperaFreioLib 2000000
    COIL RInvFreioLib 0 0 0 0
END
RUNG
    CONTACTS RInvFreioLib 0 0
    PARALLEL
        CONTACTS RMaqIniciarPosic 0 0
        CONTACTS RAvancarPerfMan 0 0
    END
    COIL YAvancarPerf 0 0 0 0
END
RUNG
    CONTACTS RInvFreioLib 0 0
    CONTACTS RRecuarPerfMan 0 0
    CONTACTS YAvancarPerf 1 0
    COIL YRecuarPerf 0 0 0 0
END
RUNG
    COMMENT Logica de controle manual da perfiladeira
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RModoAuto 1 0
    CONTACTS RFerramCorteCima 0 0
    CONTACTS RCorteTuboAtivo 1 0
    CONTACTS RMaqConfigOK 0 0
    COIL RPerfManualOK 0 0 0 0
END
RUNG
    CONTACTS RPerfManualOK 0 0
    CONTACTS XPerfManAvanca 0 0
    CONTACTS XPerfManRecua 1 0
    COIL RAvancarPerfMan 0 0 0 0
END
RUNG
    CONTACTS RPerfManualOK 0 0
    CONTACTS XPerfManAvanca 1 0
    CONTACTS XPerfManRecua 0 0
    PARALLEL
        SERIES
            CONTACTS RBloqueiaRecuo 1 0
            COIL RRecuarPerfMan 0 0 0 0
        END
        SERIES
            PARALLEL
                CONTACTS RInvFreioLib 0 0
                CONTACTS RBloqueiaRecuo 0 0
            END
            TON TMaxPerfRecua 3000000
            COIL RBloqueiaRecuo 0 0 0 0
        END
    END
END
RUNG
    COMMENT Logica de configuracao do inversor em cada modo de operacao
END
RUNG
    CONTACTS RStartConfigAuto 0 0
    CONTACTS RMaqConfigErro 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqConfigOK 0 0 1 0
        END
        SERIES
            TON TConfigAutoTO 2000000
            COIL RMaqConfigErro 0 1 0 0
        END
        SERIES
            WRITE_USS WInvAutoVel 0 0 0 0
            WRITE_USS WInvAutoAcel 0 0 0 0
            WRITE_USS WInvAutoDesacel 0 0 0 0
            WRITE_USS WInvTam 0 0 0 0
            WRITE_USS WMbPerfFator 0 0 0 0
            READ_USS Gnew 0 0 0 0
            READ_USS Gnew 0 0 0 0
            READ_USS Gnew 0 0 0 0
            READ_USS Gnew 0 0 0 0
            READ_USS Gnew 0 0 0 0
            WRITE_USS Wnew 0 0 0 0
            PARALLEL
                COIL RStartConfigAuto 0 0 1 0
                COIL RMaqConfigOK 0 1 0 0
            END
        END
    END
END
RUNG
    CONTACTS RStartConfigMan 0 0
    CONTACTS RMaqConfigErro 1 0
    PARALLEL
        SERIES
            OSR
            COIL RMaqConfigOK 0 0 1 0
        END
        SERIES
            TON TConfigManTO 2000000
            COIL RMaqConfigErro 0 1 0 0
        END
        SERIES
            WRITE_USS WInvModoMan 1 2 3 4
            WRITE_USS WInvManVel 0 0 0 0
            WRITE_USS WInvManAcel 0 0 0 0
            WRITE_USS WInvManDesacel 0 0 0 0
            PARALLEL
                COIL RStartConfigMan 0 0 1 0
                COIL RMaqConfigOK 0 1 0 0
            END
        END
    END
END
RUNG
    COMMENT Logica de operacao automatica da perfiladeira - Checagens Gerais
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS RModoAuto 0 0
    COIL RModoAutoOK 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RModoAuto 0 0
            OSF
        END
        OSR
    END
    PARALLEL
        COIL RStartConfigMan 0 1 0 0
        COIL RStartConfigAuto 0 0 1 0
    END
END
RUNG
    CONTACTS RModoAuto 0 0
    OSR
    PARALLEL
        COIL RStartConfigAuto 0 1 0 0
        COIL RStartConfigMan 0 0 1 0
        MOVE CQtdProd YMbQtd
    END
END
RUNG
    CONTACTS RMaqConfigOK 0 0
    OSR
    CONTACTS RModoAutoOK 0 0
    CONTACTS XPerfFimMaterial 0 0
    GRT CQtdProd 0
    PARALLEL
        COIL RMaqProduzindo 0 1 0 0
        COIL YInvPosicao 0 1 0 0
    END
END
RUNG
    CONTACTS XInvFimPosic 0 0
    TON TFimPosicEstab 1000000
    PARALLEL
        SERIES
            CONTACTS RMaqIniciarPosic 0 0
            COIL RMaqFimPosic 0 1 0 0
        END
        SERIES
            OSF
            COIL RMaqFimPosic 0 0 1 0
        END
    END
END
RUNG
    PARALLEL
        CONTACTS RModoAutoOK 1 0
        CONTACTS RErroCorteTubo 0 0
    END
    CONTACTS RMaqProduzindo 0 0
    COIL RMaqProduzindo 0 0 1 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    OSF
    PARALLEL
        COIL RCortarTuboAuto 0 0 1 0
        COIL YInvResetPos 0 0 1 0
        COIL YInvPosicao 0 0 1 0
    END
END
RUNG
    COMMENT Logica de operacao automatica da perfiladeira - Loop de Producao
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqFimPosic 1 0
    CONTACTS YInvResetPos 1 0
    COIL RMaqIniciarPosic 0 0 0 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqFimPosic 0 0
    CONTACTS YInvFreioMotor 1 0
    OSR
    COIL RCortarTuboAuto 0 1 0 0
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS RMaqFimPosic 0 0
    CONTACTS RCorteTuboAtivo 0 0
    PARALLEL
        COIL RCortarTuboAuto 0 0 1 0
        SERIES
            OSF
            COIL YInvResetPos 0 1 0 0
        END
    END
END
RUNG
    CONTACTS RMaqProduzindo 0 0
    CONTACTS YInvResetPos 0 0
    CONTACTS RMaqFimPosic 1 0
    PARALLEL
        COIL YInvResetPos 0 0 1 0
        SERIES
            PARALLEL
                SERIES
                    CTD CQtdProd 1
                    OSF
                END
                CONTACTS XPerfFimMaterial 1 0
                CONTACTS RModoAuto 1 0
            END
            COIL RMaqProduzindo 0 0 1 0
        END
    END
END
RUNG
    COMMENT Mapeamento de variaveis do ModBUS
END
RUNG
    CONTACTS XMbModoAuto 0 0
    COIL RModoAuto 0 0 0 0
END
RUNG
    CONTACTS YMbTipoCorte 0 0
    COIL RModoCorteSerra 0 0 0 0
END
RUNG
    CONTACTS YMbLimpaErro 0 0
    PARALLEL
        COIL RMaqConfigErro 0 0 1 0
        COIL YMbLimpaErro 0 0 1 0
    END
END
RUNG
    CONTACTS XEmergencia 1 0
    COIL YMbErrEmergencia 0 0 0 0
END
RUNG
    CONTACTS XFaltaFase 1 0
    COIL YMbErrFaltaFase 0 0 0 1
END
RUNG
    CONTACTS XTermBombaHidr 1 0
    COIL YMbErrBombaHidr 0 0 0 2
END
RUNG
    CONTACTS XInversorOK 1 0
    COIL YMbErrInversor 0 0 0 3
END
RUNG
    CONTACTS XErroDesbob 0 0
    COIL YMbErrDesbob 0 0 0 4
END
RUNG
    CONTACTS RMaqConfigErro 0 0
    COIL YMbErrInvComunic 0 0 0 5
END
RUNG
    COMMENT Declaracao de variaveis do ModBUS
END
RUNG
    OSR
    PARALLEL
        COIL YMbPerfFatorL 0 0 1 0
        COIL YMbPerfFatorH 0 0 1 0
        COIL YMbAutoAcel 0 0 1 0
        COIL YMbAutoDesacel 0 0 1 0
        COIL YMbAutoVel 0 0 1 0
        COIL YMbManAcel 0 0 1 0
        COIL YMbManDesacel 0 0 1 0
        COIL YMbManVel 0 0 1 0
        COIL YMbEncFator 0 0 1 0
        COIL YMbEncResol 0 0 1 0
        COIL YMbEncPerim 0 0 1 0
        COIL YMbFacaTam 0 0 1 0
        COIL YMbSerraTam 0 0 1 0
    END
END
RUNG
    OSR
    PARALLEL
        COIL YMbTam 0 0 1 0
        COIL YMbQtd 0 0 1 0
        MOVE YMbPerfFatorL 5
        MOVE YMbPerfFatorH 8
        MOVE YMbTam 0
        MOVE YMbAutoVel 0
        MOVE YMbManVel 0
        MOVE YMbFacaTam 0
        MOVE YMbSerraTam 0
        MOVE WInvModoAuto 1
        MOVE WInvModoMan 0
    END
END
RUNG
    COMMENT Calculando variaveis conforme os valores recebidos pelo ModBUS
END
RUNG
    COMMENT Calculando fator de relacao entre o numero de voltas do motor e do encoder
END
RUNG
    MUL WMbPerfFator YMbPerfFatorH 65536
END
RUNG
    ADD WMbPerfFator WMbPerfFator YMbPerfFatorL
END
RUNG
    COMMENT Calculando tamanho da peca para enviar ao inversor
END
RUNG
    CONTACTS RModoCorteSerra 0 0
    ADD WInvTam YMbTam YMbSerraTam
END
RUNG
    CONTACTS RModoCorteSerra 1 0
    ADD WInvTam YMbTam YMbFacaTam
END
RUNG
    MUL WInvTam WInvTam YMbEncFator
END
RUNG
    DIV WInvTam WInvTam 1000
END
RUNG
    MUL WInvTam WInvTam WMbPerfFator
END
RUNG
    COMMENT Calculando parametros de velocidade para enviar ao inversor
END
RUNG
    MUL WInvAutoVel YMbAutoVel 60
END
RUNG
    DIV WInvAutoVel WInvAutoVel 10
END
RUNG
    MUL WInvManVel YMbManVel 60
END
RUNG
    DIV WInvManVel WInvManVel 10
END
