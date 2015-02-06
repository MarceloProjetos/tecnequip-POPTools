POPTools:1.2
MICRO=NXP LPC1768 LQFP100
CYCLE=10000
CRYSTAL=100000000
BAUD=19200
PARITY=1
COM=4
IP=192.168.0.235
MASK=255.255.255.0
GW=192.168.0.10
COMPILED=N:\windows\Projetos\Maquinas\BobinadorPP\Bobinador.hex

IO LIST
    EPosAtual at 1 0
    XBombaHidrLigada at 6 0
    XBracoLevantado at 9 0
    XCarroAvancado at 12 0
    XCmdBracoDescer at 20 3
    XCmdBracoSubir at 20 4
    XCmdCarroDescer at 20 13
    XCmdCarroSubir at 20 14
    XCmdFrenagDescer at 20 9
    XCmdFrenagSubir at 20 10
    XCmdGirarAntiHor at 20 6
    XCmdGirarHor at 20 5
    XCmdPlacaAvancar at 20 8
    XCmdPlacaRecuar at 20 7
    XCmdTmbExpandir at 20 1
    XCmdTmbRetrair at 20 2
    XEmergencia at 1 0
    XFimMaterial at 16 0
    XInversorEmerg at 4 0
    XLoopInferior at 15 0
    XLoopSuperior at 14 0
    XMaqModoAuto at 20 0
    XMaqModoManual at 20 15
    XPlacaAvancada at 10 0
    XPlacaRecuada at 11 0
    XPressaoFrenagem at 13 0
    XPressaoTambor at 8 0
    XPressFiltroSujo at 7 0
    XSupervisorFase at 5 0
    XTermicoBomba at 2 0
    XTermicoCarro at 3 0
    YAvancaPlaca at 6 0
    YAvancaTambor at 15 0
    YBombaHidraulica at 1 0
    YDesceBraco at 4 0
    YDesceCarro at 10 0
    YDesceFrenagem at 11 0
    YExpandeTambor at 3 0
    YFreioMotor at 13 0
    YInvLimparErro at 14 0
    YLampAutomatico at 21 6
    YLampCarroAvanca at 21 2
    YLampEmergencia at 21 0
    YLampFiltroSujo at 21 3
    YLampFreioAlto at 21 4
    YLampMandrilRet at 21 5
    YLampManual at 21 7
    YLED_ERRO at 17 0
    YMaqDesligaAuto at 21 10
    YMbEntradas at 20 0
    YMbMaqManualOK at 21 1
    YMbSaidas at 21 0
    YRecuaPlaca at 7 0
    YRecuaTambor at 16 0
    YRetraiTambor at 2 0
    YSobeBraco at 5 0
    YSobeCarro at 9 0
    YSobeFrenagem at 12 0
    YTMR at 18 0
    YVentagem at 8 0
    ZReset at 1 0
END

PROGRAM
RUNG
    COMMENT Programa de controle - Bobinador Prensa 200T
END
RUNG
    COMMENT Versão Inicial: Alessandro Holanda\r\nFinalização: Marcelo Fonseca
END
RUNG
    COMMENT Temporizador - serve de base de tempo para o programa e pisca intermitente do led_cpu para indicar que o programa esta executando
END
RUNG
    CONTACTS YTMR 1 0
    TON TON 1000000
    TOF TOFF 1000000
    COIL YTMR 0 0 0 0
END
RUNG
    COMMENT Inicialização das variáveis na energização da placa
END
RUNG
    OSR
    PARALLEL
        COIL YMbEntradas 0 0 1 0
        MOVE YMbEntradas 0
        COIL YMbSaidas 0 0 1 0
        MOVE YMbSaidas 0
        MOVE SaidasAnt 0
        MOVE ValSaidas 0
        MOVE VelocidadeMin 100
        MOVE VelocidadeMax 6000
        MOVE VelManual 500
        MOVE Velocidade VelocidadeMin
        MOVE Aceleracao 50
        MOVE Desaceleracao 100
        COIL RSlaveUpdateFreq 0 1 0 0
    END
END
RUNG
    COMMENT Checagem de erros da máquina
END
RUNG
    CONTACTS XEmergencia 0 0
    CONTACTS XSupervisorFase 0 0
    CONTACTS XInversorEmerg 0 0
    CONTACTS XTermicoBomba 0 0
    CONTACTS XTermicoCarro 0 0
    CONTACTS RMaqInitOK 0 0
    COIL RMaqOK 0 0 0 0
END
RUNG
    CONTACTS XMaqModoAuto 1 0
    CONTACTS XInversorEmerg 1 0
    CONTACTS YInvLimparErro 1 0
    TON TInvPulsoON 500000
    TOF TInvPulsoOff 100000
    COIL YInvLimparErro 0 0 0 0
END
RUNG
    CONTACTS RMaqSerialLib 0 0
    TON TEspSerialLib 100000
    PARALLEL
        COIL RMaqSerialBusy 0 0 1 0
        COIL RMaqSerialLib 0 0 1 0
    END
END
RUNG
    PARALLEL
        CONTACTS RSaidasCfgErr 0 0
        CONTACTS REntradasCfgErr 0 0
    END
    PARALLEL
        COIL REscravaCfgErr 0 0 0 0
        SERIES
            OSR
            PARALLEL
                COIL REntradasCfg 0 0 1 0
                COIL RSaidasCfg 0 0 1 0
                COIL RSlaveUpdateFreq 0 0 1 0
                COIL RInversorCfg 0 0 1 0
                MOVE InversorCfgCnt 0
            END
        END
    END
END
RUNG
    PARALLEL
        CONTACTS REscravaCfgErr 0 0
        CONTACTS RInversorCfgErr 0 0
    END
    TON TEspLimparErro 500000
    PARALLEL
        COIL RInversorCfgErr 0 0 1 0
        COIL RSaidasCfgErr 0 0 1 0
        COIL REntradasCfgErr 0 0 1 0
        COIL RMaqSerialBusy 0 0 1 0
        COIL RMaqLib 0 0 1 0
    END
END
RUNG
    CONTACTS RInversorCfgErr 0 0
    OSF
    COIL RInversorCfg 0 1 0 0
END
RUNG
    CONTACTS RSaidasCfgErr 0 0
    OSF
    MOVE SaidasAnt -1
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        CONTACTS RInversorCfgErr 0 0
        CONTACTS REscravaCfgErr 0 0
        SERIES
            CONTACTS XMaqModoAuto 0 0
            CONTACTS XFimMaterial 1 0
        END
    END
    COIL YMaqDesligaAuto 0 0 0 10
END
RUNG
    PARALLEL
        CONTACTS RMaqOK 1 0
        CONTACTS RInversorCfgErr 0 0
        CONTACTS REscravaCfgErr 0 0
    END
    COIL YLED_ERRO 0 0 0 0
END
RUNG
    COMMENT Lógica que transmite para a POP escrava o erro atual\r\nOs códigos de erro são classificados por categoria, de forma a permitir que o operador identifique a origem do problema
END
RUNG
    COMMENT Categorias de erro:\r\n1 a 9: Falha na máquina como falta de fase, térmico desarmado, etc.
END
RUNG
    COMMENT 10 a 19: Erros diversos como falha na comunicação\r\n20 a 29: Erros relacionados à operação em modo automático
END
RUNG
    PARALLEL
        CONTACTS YLED_ERRO 0 0
        SERIES
            CONTACTS RMaqAutoOK 1 0
            CONTACTS RMaqStartAuto 1 0
            CONTACTS XMaqModoAuto 0 0
        END
    END
    PARALLEL
        SERIES
            OSF
            MOVE ErroAtual 0
        END
        SERIES
            CONTACTS XMaqModoAuto 0 0
            CONTACTS XFimMaterial 1 0
            MOVE ErroAtual 34
        END
        SERIES
            CONTACTS XMaqModoAuto 0 0
            CONTACTS XPlacaRecuada 1 0
            MOVE ErroAtual 33
        END
        SERIES
            CONTACTS XMaqModoAuto 0 0
            CONTACTS XPressaoTambor 1 0
            MOVE ErroAtual 32
        END
        SERIES
            CONTACTS XMaqModoAuto 0 0
            CONTACTS XPressaoFrenagem 1 0
            MOVE ErroAtual 31
        END
        SERIES
            CONTACTS REscravaCfgErr 0 0
            MOVE ErroAtual 22
        END
        SERIES
            CONTACTS RInversorCfgErr 0 0
            MOVE ErroAtual 21
        END
        SERIES
            CONTACTS XTermicoCarro 1 0
            MOVE ErroAtual 14
        END
        SERIES
            CONTACTS XTermicoBomba 1 0
            MOVE ErroAtual 13
        END
        SERIES
            CONTACTS XInversorEmerg 1 0
            MOVE ErroAtual 12
        END
        SERIES
            CONTACTS XSupervisorFase 1 0
            MOVE ErroAtual 11
        END
    END
END
RUNG
    NEQ ErroAtual ErroAnt
    CONTACTS REnviarErro 1 0
    CONTACTS RMaqSerialBusy 1 0
    PARALLEL
        MOVE ErroAnt ErroAtual
        COIL REnviarErro 0 1 0 0
        COIL RMaqSerialBusy 0 1 0 0
    END
END
RUNG
    CONTACTS REnviarErro 0 0
    PARALLEL
        WRITE_MODBUS ErroAtual 0 2 0
        TON TEnviarErroTO 1000000
    END
    PARALLEL
        COIL REnviarErro 0 0 1 0
        COIL RMaqSerialLib 0 1 0 0
    END
END
RUNG
    COMMENT Faz a leitura das entradas da POP escrava
END
RUNG
    CONTACTS RMaqInitOK 0 0
    CONTACTS REntradasCfg 0 0
    CONTACTS RMaqSerialBusy 1 0
    PARALLEL
        COIL RStartEntCfg 0 1 0 0
        COIL RMaqSerialBusy 0 1 0 0
        COIL REntradasCfg 0 0 1 0
    END
END
RUNG
    CONTACTS RStartEntCfg 0 0
    PARALLEL
        SERIES
            TON TEntradasCfg 1000000
            PARALLEL
                COIL REntradasCfgErr 0 1 0 0
                COIL RStartEntCfg 0 0 1 0
            END
        END
        SERIES
            READ_MODBUS YMbEntradas 0 0 0
            PARALLEL
                COIL RStartEntCfg 0 0 1 0
                COIL RMaqSerialLib 0 1 0 0
            END
        END
    END
END
RUNG
    COMMENT Tempo de pooling do estado das portas da POP escrava\r\nDeve ser SEMPRE maior que o tempo de espera para destravar a serial
END
RUNG
    CONTACTS REntradasCfg 1 0
    CONTACTS RStartEntCfg 1 0
    TON TEspUpdateFreq 150000
    COIL REntradasCfg 0 1 0 0
END
RUNG
    COMMENT Envia o comando para escrever nas saidas da POP escrava
END
RUNG
    CONTACTS RMaqInitOK 0 0
    NEQ SaidasAnt ValSaidas
    CONTACTS RMaqSerialBusy 1 0
    PARALLEL
        COIL RStartSaidasCfg 0 1 0 0
        COIL RMaqSerialBusy 0 1 0 0
        MOVE SaidasAnt ValSaidas
    END
END
RUNG
    CONTACTS RStartSaidasCfg 0 0
    PARALLEL
        SERIES
            TON TSaidasCfgErr 1000000
            PARALLEL
                COIL RSaidasCfgErr 0 1 0 0
                COIL RStartSaidasCfg 0 0 1 0
            END
        END
        SERIES
            WRITE_MODBUS YMbSaidas 0 1 0
            PARALLEL
                COIL RMaqSerialLib 0 1 0 0
                COIL RStartSaidasCfg 0 0 1 0
            END
        END
    END
END
RUNG
    COMMENT Responsável pelo envio da configuração para o inversor atraves do modbus.\r\nFaz 1 tentativa antes de sinalizar erro e paralizar a comunicação até que as flags de erro sejam resetadas
END
RUNG
    CONTACTS RMaqInitOK 0 0
    CONTACTS RInversorCfg 0 0
    CONTACTS RMaqSerialBusy 1 0
    PARALLEL
        COIL RInversorCfg 0 0 1 0
        ADD VelInv Velocidade VelOffset
        SERIES
            GRT VelInv VelocidadeMax
            MOVE VelInv VelocidadeMax
        END
        SERIES
            LES VelInv VelocidadeMin
            MOVE VelInv VelocidadeMin
        END
        COIL RMaqSerialBusy 0 1 0 0
        COIL RStartInvCfg 0 1 0 0
    END
END
RUNG
    CONTACTS RStartInvCfg 0 0
    PARALLEL
        SERIES
            TON TInversorCfgErr 1000000
            PARALLEL
                COIL RInversorCfgErr 0 1 0 0
                COIL RStartInvCfg 0 0 1 0
            END
        END
        SERIES
            WRITE_MODBUS VelInv 1 8193 0
            PARALLEL
                COIL RStartInvCfg 0 0 1 0
                COIL RMaqSerialLib 0 1 0 0
            END
        END
    END
END
RUNG
    COMMENT Leitura e cálculos relacionados ao encoder
END
RUNG
    CONTACTS YTMR 0 0
    PARALLEL
        OSR
        OSF
    END
    PARALLEL
        MOVE PosAnt EPosAtual
        READ_ENC EPosAtual
    END
END
RUNG
    SUB DifPos EPosAtual PosAnt
END
RUNG
    MOVE DifPosAbs DifPos
END
RUNG
    LES DifPos 0
    MUL DifPosAbs DifPos -1
END
RUNG
    CONTACTS XBracoLevantado 0 0
    OSR
    CONTACTS RMaqInitOK 0 0
    PARALLEL
        RESET_ENC ZReset
        COIL RMaqRefOK 0 1 0 0
    END
END
RUNG
    COMMENT Cálculo do ângulo do braço
END
RUNG
    MOVE AnguloAoZerar 75
END
RUNG
    MUL PulsosEmZero AnguloAoZerar 5000
END
RUNG
    DIV PulsosEmZero PulsosEmZero 360
END
RUNG
    SUB AnguloBraco PulsosEmZero EPosAtual
END
RUNG
    MUL AnguloBraco AnguloAoZerar AnguloBraco
END
RUNG
    DIV AnguloBraco AnguloBraco PulsosEmZero
END
RUNG
    COMMENT Cálculo da velocidade conforme ângulo do braço
END
RUNG
    PARALLEL
        MOVE Velocidade 0
        SERIES
            GRT AnguloBraco 5
            MOVE Velocidade 12384
        END
        SERIES
            GRT AnguloBraco 10
            MOVE Velocidade 8456
        END
        SERIES
            GRT AnguloBraco 15
            MOVE Velocidade 5999
        END
        SERIES
            GRT AnguloBraco 20
            MOVE Velocidade 4545
        END
        SERIES
            GRT AnguloBraco 25
            MOVE Velocidade 3629
        END
        SERIES
            GRT AnguloBraco 30
            MOVE Velocidade 3012
        END
        SERIES
            GRT AnguloBraco 35
            MOVE Velocidade 2573
        END
        SERIES
            GRT AnguloBraco 40
            MOVE Velocidade 2246
        END
        SERIES
            GRT AnguloBraco 45
            MOVE Velocidade 1996
        END
        SERIES
            GRT AnguloBraco 50
            MOVE Velocidade 1798
        END
        SERIES
            GRT AnguloBraco 55
            MOVE Velocidade 1638
        END
        SERIES
            GRT AnguloBraco 60
            MOVE Velocidade 1506
        END
        SERIES
            GRT AnguloBraco 65
            MOVE Velocidade 1397
        END
        SERIES
            GRT AnguloBraco 70
            MOVE Velocidade 1305
        END
        SERIES
            GRT AnguloBraco 75
            MOVE Velocidade 1226
        END
        SERIES
            GRT AnguloBraco 80
            MOVE Velocidade 1158
        END
        SERIES
            GRT AnguloBraco 85
            MOVE Velocidade 1100
        END
    END
END
RUNG
    CONTACTS XMaqModoAuto 1 0
    MOVE Velocidade VelManual
END
RUNG
    COMMENT Inicializa as variáveis do modo Manual, na inicialização da POP ou sempre que muda para modo Manual
END
RUNG
    PARALLEL
        SERIES
            CONTACTS XMaqModoAuto 0 0
            OSF
        END
        OSR
    END
    PARALLEL
        MOVE Velocidade VelManual
        COIL RInversorCfg 0 1 0 0
        MOVE InversorCfgCnt 0
    END
END
RUNG
    COMMENT Aguarda até que o inversor esteja pronto para receber as configurações de inicialização para o modo manual\r\nDurante esta espera, liga todas as lâmpadas do painel para testá-las. Teste feito pela POP escrava
END
RUNG
    TON TEspMaqInit 3000000
    COIL RMaqInitOK 0 0 0 0
END
RUNG
    COMMENT Controle das Lâmpadas do painel
END
RUNG
    CONTACTS XEmergencia 1 0
    COIL YLampEmergencia 0 0 0 0
END
RUNG
    PARALLEL
        CONTACTS RMaqAutoOK 0 0
        CONTACTS RMaqStartAuto 0 0
    END
    COIL YLampAutomatico 0 0 0 6
END
RUNG
    CONTACTS RMaqManualOK 0 0
    COIL YLampManual 0 0 0 7
END
RUNG
    CONTACTS XPressFiltroSujo 1 0
    COIL YLampFiltroSujo 0 0 0 3
END
RUNG
    CONTACTS XCarroAvancado 0 0
    COIL YLampCarroAvanca 0 0 0 2
END
RUNG
    CONTACTS XPressaoFrenagem 1 0
    COIL YLampFreioAlto 0 0 0 4
END
RUNG
    CONTACTS XPressaoTambor 1 0
    COIL YLampMandrilRet 0 0 0 5
END
RUNG
    COMMENT Configura as variáveis para o modo Automatico
END
RUNG
    CONTACTS XMaqModoAuto 0 0
    OSR
    PARALLEL
        MOVE Velocidade VelocidadeMin
        COIL RInversorCfg 0 1 0 0
        COIL RInversorCfgErr 0 0 1 0
    END
END
RUNG
    COMMENT Controle da Bomba Hidráulica
END
RUNG
    PARALLEL
        CONTACTS RExpandeTambor 0 0
        CONTACTS RRetraiTambor 0 0
        CONTACTS RDesceBraco 0 0
        CONTACTS RSobeBraco 0 0
        CONTACTS RAvancaPlaca 0 0
        CONTACTS RRecuaPlaca 0 0
        CONTACTS RSobeCarro 0 0
        CONTACTS RDesceCarro 0 0
        CONTACTS RSobeFrenagem 0 0
        CONTACTS RDesceFrenagem 0 0
        CONTACTS XMaqModoAuto 0 0
    END
    COIL RLigarBombaHidr 0 0 0 0
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS YBombaHidraulica 1 0
    CONTACTS RLigarBombaHidr 0 0
    COIL YBombaHidraulica 0 1 0 0
END
RUNG
    CONTACTS YBombaHidraulica 0 0
    CONTACTS XBombaHidrLigada 0 0
    TON TEspBombaHidrOK 1000000
    COIL RBombaHidrOK 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RLigarBombaHidr 1 0
            CONTACTS RBombaHidrOK 0 0
            TON TEspBombaHidrTO 300000000
        END
        CONTACTS RMaqOK 1 0
    END
    COIL YBombaHidraulica 0 0 1 0
END
RUNG
    COMMENT Mapeamento dos comandos da hidráulica para as saídas da POP
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RExpandeTambor 0 0
    COIL YExpandeTambor 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RRetraiTambor 0 0
    COIL YRetraiTambor 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RDesceBraco 0 0
    COIL YDesceBraco 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RSobeBraco 0 0
    COIL YSobeBraco 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RAvancaPlaca 0 0
    COIL YAvancaPlaca 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RRecuaPlaca 0 0
    COIL YRecuaPlaca 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RSobeCarro 0 0
    COIL YSobeCarro 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RDesceCarro 0 0
    COIL YDesceCarro 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RSobeFrenagem 0 0
    COIL YSobeFrenagem 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RDesceFrenagem 0 0
    COIL YDesceFrenagem 0 0 0 0
END
RUNG
    CONTACTS RBombaHidrOK 0 0
    CONTACTS RLigarBombaHidr 0 0
    COIL YVentagem 0 0 0 0
END
RUNG
    COMMENT Lógica de controle manual do bobinador - comandos hidráulicos
END
RUNG
    CONTACTS RMaqOK 0 0
    CONTACTS XMaqModoManual 0 15
    COIL RMaqManualOK 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RMaqManualOK 0 0
            CONTACTS XCmdTmbExpandir 0 1
        END
        CONTACTS RMaqStartAuto 0 0
        CONTACTS RCmdAutoTambor 0 0
    END
    CONTACTS XPressaoTambor 1 0
    COIL RExpandeTambor 0 0 0 0
END
RUNG
    CONTACTS RMaqManualOK 0 0
    CONTACTS XCmdTmbRetrair 0 2
    COIL RRetraiTambor 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RMaqManualOK 0 0
            CONTACTS XCmdBracoDescer 0 3
        END
        CONTACTS RCmdAutoBraco 0 0
        SERIES
            CONTACTS XBracoLevantado 0 0
            CONTACTS RMaqRefOK 1 0
            CONTACTS RCmdAutoBracoRef 0 0
        END
    END
    CONTACTS XPlacaRecuada 0 0
    COIL RDesceBraco 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RMaqManualOK 0 0
            CONTACTS XCmdBracoSubir 0 4
        END
        SERIES
            CONTACTS RMaqStartAuto 0 0
            PARALLEL
                CONTACTS XPlacaRecuada 1 0
                CONTACTS RCmdAutoBracoRef 0 0
            END
        END
    END
    CONTACTS XBracoLevantado 1 0
    COIL RSobeBraco 0 0 0 0
END
RUNG
    CONTACTS RMaqManualOK 0 0
    CONTACTS XCmdPlacaAvancar 0 8
    CONTACTS XBracoLevantado 0 0
    CONTACTS XPlacaAvancada 1 0
    COIL RAvancaPlaca 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RMaqManualOK 0 0
            CONTACTS XCmdPlacaRecuar 0 7
        END
        CONTACTS RMaqStartAuto 0 0
    END
    CONTACTS XBracoLevantado 0 0
    CONTACTS XPlacaRecuada 1 0
    COIL RRecuaPlaca 0 0 0 0
END
RUNG
    CONTACTS RMaqManualOK 0 0
    CONTACTS XCmdCarroSubir 0 14
    COIL RSobeCarro 0 0 0 0
END
RUNG
    CONTACTS RMaqManualOK 0 0
    CONTACTS XCmdCarroDescer 0 13
    COIL RDesceCarro 0 0 0 0
END
RUNG
    CONTACTS RMaqManualOK 0 0
    CONTACTS XCmdFrenagSubir 0 10
    COIL RSobeFrenagem 0 0 0 0
END
RUNG
    PARALLEL
        SERIES
            CONTACTS RMaqManualOK 0 0
            CONTACTS XCmdFrenagDescer 0 9
        END
        CONTACTS RMaqStartAuto 0 0
        CONTACTS RCmdAutoFrenagem 0 0
    END
    CONTACTS XPressaoFrenagem 1 0
    COIL RDesceFrenagem 0 0 0 0
END
RUNG
    COMMENT Lógica de controle do motor do bobinador
END
RUNG
    CONTACTS XPlacaRecuada 0 0
    PARALLEL
        SERIES
            PARALLEL
                SERIES
                    CONTACTS RMaqManualOK 0 0
                    CONTACTS RInversorCfgErr 1 0
                    CONTACTS RInversorCfg 1 0
                    CONTACTS XCmdGirarAntiHor 0 6
                END
                SERIES
                    CONTACTS RMaqAutoOK 0 0
                    CONTACTS XFimMaterial 0 0
                END
            END
            COIL RAvancaTambor 0 0 0 0
        END
        SERIES
            CONTACTS RMaqManualOK 0 0
            CONTACTS RInversorCfgErr 1 0
            CONTACTS RInversorCfg 1 0
            CONTACTS XCmdGirarHor 0 5
            COIL RRecuaTambor 0 0 0 0
        END
    END
END
RUNG
    PARALLEL
        CONTACTS RAvancaTambor 0 0
        CONTACTS RRecuaTambor 0 0
    END
    TOF TEspMotorParar 1000000
    CONTACTS RMaqOK 0 0
    COIL YFreioMotor 0 0 0 0
END
RUNG
    CONTACTS YFreioMotor 0 0
    TON TEspFreioMotorOK 500000
    COIL RFreioMotorOK 0 0 0 0
END
RUNG
    CONTACTS RFreioMotorOK 0 0
    PARALLEL
        SERIES
            CONTACTS RAvancaTambor 0 0
            COIL YAvancaTambor 0 0 0 0
        END
        SERIES
            CONTACTS RRecuaTambor 0 0
            COIL YRecuaTambor 0 0 0 0
        END
    END
END
RUNG
    COMMENT Controle do Bobinador - Modo Automático - Checagens Gerais
END
RUNG
    CONTACTS XMaqModoAuto 0 0
    OSR
    CONTACTS XFimMaterial 0 0
    PARALLEL
        COIL RMaqStartAuto 0 1 0 0
        MOVE VelOffset 0
    END
END
RUNG
    CONTACTS XPressaoFrenagem 0 0
    CONTACTS XPressaoTambor 0 0
    TOF TEspPressaoEstab 2000000
    CONTACTS RMaqOK 0 0
    CONTACTS XMaqModoAuto 0 0
    CONTACTS RMaqStartAuto 1 0
    CONTACTS XPlacaRecuada 0 0
    COIL RMaqAutoOK 0 0 0 0
END
RUNG
    COMMENT Controle do Bobinador - Modo Automático - Iniciando Produção
END
RUNG
    CONTACTS RMaqStartAuto 0 0
    CONTACTS XPlacaRecuada 0 0
    CONTACTS XPressaoFrenagem 0 0
    CONTACTS XPressaoTambor 0 0
    PARALLEL
        SERIES
            CONTACTS RMaqRefOK 0 0
            PARALLEL
                SERIES
                    LES DifPosAbs 500
                    TON TEspBracoEstab 3000000
                    COIL RMaqStartAuto 0 0 1 0
                END
                COIL RCmdAutoBraco 0 0 0 0
            END
        END
        SERIES
            CONTACTS RMaqRefOK 1 0
            COIL RCmdAutoBracoRef 0 0 0 0
        END
    END
END
RUNG
    COMMENT Controle do Bobinador - Modo Automático - Controle de pressões durante produção
END
RUNG
    CONTACTS RMaqAutoOK 0 0
    CONTACTS XPressaoFrenagem 1 0
    COIL RCmdAutoFrenagem 0 0 0 0
END
RUNG
    CONTACTS RMaqAutoOK 0 0
    CONTACTS XPressaoTambor 1 0
    COIL RCmdAutoTambor 0 0 0 0
END
RUNG
    COMMENT Esta rotina faz a aceleração e desaceleração conforme os sensores de looping\r\npara garantir que o bobinador acompanhe a velocidade da prensa
END
RUNG
    CONTACTS RUpdateInv 1 0
    TON TEspUpdateInv 500000
    COIL RUpdateInv 0 0 0 0
END
RUNG
    CONTACTS RMaqAutoOK 0 0
    CONTACTS RUpdateInv 0 0
    PARALLEL
        SERIES
            CONTACTS XLoopSuperior 0 1
            CONTACTS RInversorCfg 1 0
            GRT VelOffset -6000
            PARALLEL
                SUB VelOffset VelOffset Desaceleracao
                COIL RInversorCfg 0 1 0 0
            END
        END
        SERIES
            CONTACTS XLoopInferior 0 0
            CONTACTS RInversorCfg 1 0
            LES VelOffset 6000
            PARALLEL
                ADD VelOffset VelOffset Aceleracao
                COIL RInversorCfg 0 1 0 0
            END
        END
    END
END
RUNG
    COMMENT Flags adicionais transmitidas para a POP escrava
END
RUNG
    CONTACTS RMaqManualOK 0 0
    COIL YMbMaqManualOK 0 0 0 1
END
RUNG
    COMMENT Carrega valor das saídas em variável numérica para poder comparar se houve alteração no estado de alguma delas
END
RUNG
    PARALLEL
        SERIES
            CONTACTS YLampEmergencia 0 0
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 0
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 0
                END
            END
        END
        SERIES
            CONTACTS YMbMaqManualOK 0 1
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 1
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 1
                END
            END
        END
        SERIES
            CONTACTS YLampCarroAvanca 0 2
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 2
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 2
                END
            END
        END
        SERIES
            CONTACTS YLampFiltroSujo 0 3
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 3
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 3
                END
            END
        END
        SERIES
            CONTACTS YLampFreioAlto 0 4
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 4
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 4
                END
            END
        END
        SERIES
            CONTACTS YLampMandrilRet 0 5
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 5
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 5
                END
            END
        END
        SERIES
            CONTACTS YLampAutomatico 0 6
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 6
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 6
                END
            END
        END
        SERIES
            CONTACTS YLampManual 0 7
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 7
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 7
                END
            END
        END
        SERIES
            CONTACTS YMaqDesligaAuto 0 10
            PARALLEL
                SERIES
                    OSR
                    SET_BIT ValSaidas 1 10
                END
                SERIES
                    OSF
                    SET_BIT ValSaidas 0 10
                END
            END
        END
    END
    COIL RFlagDummy 0 0 0 0
END
