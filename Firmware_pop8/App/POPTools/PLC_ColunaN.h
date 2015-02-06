// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char U_YLedUser = 0;
volatile int U_TEsperaLedON = 0;
volatile unsigned char I_TEsperaLedOFF_antiglitch = 0;
volatile int U_TEsperaLedOFF = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile unsigned char GPIO_INPUT_PORT1 = 0;
volatile unsigned char GPIO_INPUT_PORT3 = 0;
volatile unsigned char I_parOut_0001 = 0;
volatile unsigned char I_parThis_0001 = 0;
volatile unsigned char U_RErroGeral = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char U_RMaqInitOK = 0;
volatile unsigned char GPIO_INPUT_PORT4 = 0;
volatile unsigned char GPIO_INPUT_PORT2 = 0;
volatile unsigned char I_parOut_0002 = 0;
volatile unsigned char I_parThis_0002 = 0;
volatile unsigned char GPIO_INPUT_PORT15 = 0;
volatile unsigned char GPIO_OUTPUT_PORT8 = 0;
volatile unsigned char I_TSrvEsperaHab_antiglitch = 0;
volatile int U_TSrvEsperaHab = 0;
volatile unsigned char U_RMaqOK = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile unsigned char I_parOut_0003 = 0;
volatile unsigned char I_parThis_0003 = 0;
volatile unsigned char U_RMaqInitStart = 0;
volatile unsigned char U_RMaqInit = 0;
volatile unsigned char U_RMaqInitFim = 0;
volatile unsigned char I_parOut_0004 = 0;
volatile unsigned char I_parThis_0004 = 0;
volatile unsigned char U_RMaqCfgInvErro = 0;
volatile unsigned char U_RMaqCfgSrvErro = 0;
volatile unsigned char U_YLedErro = 0;
volatile unsigned char U_RMaqCfgInvOK = 0;
volatile unsigned char U_RMaqCfgSrvOK = 0;
volatile unsigned char U_RMaqConfigOK = 0;
volatile unsigned char U_RMaqPronta = 0;
volatile unsigned char I_parOut_0005 = 0;
volatile unsigned char I_parThis_0005 = 0;
volatile unsigned char U_RCortePerfAtivo = 0;
volatile unsigned char U_RSrvPosicAtivo = 0;
volatile unsigned char U_RCorteEstatAtivo = 0;
volatile unsigned char U_RCalcCorrAtivo = 0;
volatile unsigned char U_RCalcRelAtivo = 0;
volatile unsigned char U_RCalcTamMinAtivo = 0;
volatile unsigned char U_RMaqOperando = 0;
volatile int U_EMaqPosAtual = 0;
volatile int I_scratch2 = 0;
volatile int U_MaqPosAtual = 0;
volatile int U_PosProxCorte = 0;
volatile int U_MaqPosCorte = 0;
volatile int U_MaqPecaTam = 0;
volatile int U_DifPosCorte = 0;
volatile int U_DifPosCorteInv = 0;
volatile unsigned char U_RCalcVarPerfil = 0;
volatile int U_TTmrCalcPerfil = 0;
volatile unsigned char I_parOut_0006 = 0;
volatile unsigned char I_parThis_0006 = 0;
volatile int U_PerfilVariacao = 0;
volatile int U_PerfilPosAnt = 0;
volatile unsigned char U_RMaqPerfilParado = 0;
volatile unsigned char GPIO_INPUT_PORT10 = 0;
volatile int U_TEspSnsSupON = 0;
volatile unsigned char U_RSnsSupCortePerf = 0;
volatile unsigned char GPIO_INPUT_PORT11 = 0;
volatile int U_TEspSnsInfON = 0;
volatile unsigned char U_RSnsInfCortePerf = 0;
volatile unsigned char U_RBombaHidrOK = 0;
volatile unsigned char U_RCortePronto = 0;
volatile unsigned char GPIO_INPUT_PORT12 = 0;
volatile unsigned char GPIO_INPUT_PORT13 = 0;
volatile unsigned char U_RFerramCorteCima = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char I_parOut_0007 = 0;
volatile unsigned char I_parThis_0007 = 0;
volatile unsigned char U_RMaqInitCorte = 0;
volatile unsigned char U_RMaqProduzindo = 0;
volatile int U_TBombaHidrEstab = 0;
volatile unsigned char I_parOut_0008 = 0;
volatile unsigned char I_parThis_0008 = 0;
volatile int U_TBombaHidrInat = 0;
volatile unsigned char I_parOut_0009 = 0;
volatile unsigned char I_parThis_0009 = 0;
volatile unsigned char U_RModoAuto = 0;
volatile unsigned char U_RErroCortePerf = 0;
volatile unsigned char U_RCortarPerf = 0;
volatile unsigned char I_parOut_000a = 0;
volatile unsigned char I_parThis_000a = 0;
volatile unsigned char I_parOut_000b = 0;
volatile unsigned char I_parThis_000b = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile unsigned char U_RMaqCortePerf = 0;
volatile unsigned char U_RCorteRealizado = 0;
volatile int U_CNumTentPiloto = 0;
volatile unsigned char U_RMaqUsarPiloto = 0;
volatile unsigned char U_RRepeteSemPiloto = 0;
volatile unsigned char I_oneShot_0002 = 0;
volatile unsigned char GPIO_INPUT_PORT7 = 0;
volatile unsigned char I_parOut_000c = 0;
volatile unsigned char I_parThis_000c = 0;
volatile int U_TMaxCortePerf = 0;
volatile unsigned char GPIO_INPUT_PORT16 = 0;
volatile unsigned char U_RMaqModoEstatico = 0;
volatile unsigned char U_RMaqModoAjuste = 0;
volatile unsigned char I_parOut_000d = 0;
volatile unsigned char I_parThis_000d = 0;
volatile unsigned char I_parOut_000e = 0;
volatile unsigned char I_parThis_000e = 0;
volatile int U_TRecuoCorteTO = 0;
volatile unsigned char I_parOut_000f = 0;
volatile unsigned char I_parThis_000f = 0;
volatile unsigned char U_RCortePerfDesce = 0;
volatile unsigned char I_oneShot_0003 = 0;
volatile unsigned char U_RCortePilotar = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile int U_TEspPilotoEstab = 0;
volatile unsigned char U_RCortePilotoCima = 0;
volatile unsigned char GPIO_INPUT_PORT14 = 0;
volatile unsigned char I_oneShot_0004 = 0;
volatile unsigned char I_parOut_0010 = 0;
volatile unsigned char I_parThis_0010 = 0;
volatile int U_TEsperaPilotar = 0;
volatile int U_TPilotoDesceTO = 0;
volatile unsigned char I_parOut_0011 = 0;
volatile unsigned char I_parThis_0011 = 0;
volatile unsigned char I_parOut_0012 = 0;
volatile unsigned char I_parThis_0012 = 0;
volatile unsigned char I_oneShot_0005 = 0;
volatile unsigned char I_parOut_0013 = 0;
volatile unsigned char I_parThis_0013 = 0;
volatile unsigned char U_RErroCortePiloto = 0;
volatile unsigned char I_oneShot_0006 = 0;
volatile unsigned char I_parOut_0014 = 0;
volatile unsigned char I_parThis_0014 = 0;
volatile unsigned char I_parOut_0015 = 0;
volatile unsigned char I_parThis_0015 = 0;
volatile unsigned char U_RInvAtivar = 0;
volatile unsigned char I_parOut_0016 = 0;
volatile unsigned char I_parThis_0016 = 0;
volatile unsigned char U_RMaqStartProd = 0;
volatile unsigned char I_parOut_0017 = 0;
volatile unsigned char I_parThis_0017 = 0;
volatile int U_ZEncResetVal = 0;
volatile unsigned char I_parOut_0018 = 0;
volatile unsigned char I_parThis_0018 = 0;
volatile unsigned char I_parOut_0019 = 0;
volatile unsigned char I_parThis_0019 = 0;
volatile unsigned char I_oneShot_0007 = 0;
volatile unsigned char I_oneShot_0008 = 0;
volatile unsigned char I_oneShot_0009 = 0;
volatile unsigned char I_oneShot_000a = 0;
volatile unsigned char I_oneShot_000b = 0;
volatile unsigned char I_parOut_001a = 0;
volatile unsigned char I_parThis_001a = 0;
volatile unsigned char I_parOut_001b = 0;
volatile unsigned char I_parThis_001b = 0;
volatile unsigned char I_TConfirmaOFF_antiglitch = 0;
volatile int U_TConfirmaOFF = 0;
volatile unsigned char I_oneShot_000c = 0;
volatile unsigned char I_oneShot_000d = 0;
volatile unsigned char I_parOut_001c = 0;
volatile unsigned char I_parThis_001c = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;
volatile unsigned char I_parOut_001d = 0;
volatile unsigned char I_parThis_001d = 0;
volatile unsigned char I_parOut_001e = 0;
volatile unsigned char I_parThis_001e = 0;
volatile unsigned char I_parOut_001f = 0;
volatile unsigned char I_parThis_001f = 0;
volatile unsigned char GPIO_OUTPUT_PORT5 = 0;
volatile unsigned char I_parOut_0020 = 0;
volatile unsigned char I_parThis_0020 = 0;
volatile unsigned char GPIO_OUTPUT_PORT13 = 0;
volatile unsigned char I_parOut_0021 = 0;
volatile unsigned char I_parThis_0021 = 0;
volatile unsigned char I_parOut_0022 = 0;
volatile unsigned char I_parThis_0022 = 0;
volatile unsigned char GPIO_INPUT_PORT5 = 0;
volatile unsigned char GPIO_INPUT_PORT6 = 0;
volatile unsigned char I_parOut_0023 = 0;
volatile unsigned char I_parThis_0023 = 0;
volatile unsigned char I_oneShot_000e = 0;
volatile unsigned char I_oneShot_000f = 0;
volatile unsigned char I_parOut_0024 = 0;
volatile unsigned char I_parThis_0024 = 0;
volatile unsigned char I_parOut_0025 = 0;
volatile unsigned char I_parThis_0025 = 0;
volatile unsigned char I_parOut_0026 = 0;
volatile unsigned char I_parThis_0026 = 0;
volatile unsigned char U_RCalcRelE1Avanca = 0;
volatile unsigned char I_parOut_0027 = 0;
volatile unsigned char I_parThis_0027 = 0;
volatile unsigned char GPIO_OUTPUT_PORT6 = 0;
volatile unsigned char I_parOut_0028 = 0;
volatile unsigned char I_parThis_0028 = 0;
volatile unsigned char U_RAvancarPerfMan = 0;
volatile unsigned char U_RRecuarPerfMan = 0;
volatile unsigned char I_TMantemFreioLib_antiglitch = 0;
volatile int U_TMantemFreioLib = 0;
volatile int U_TEsperaFreioLib = 0;
volatile unsigned char U_RInvFreioLib = 0;
volatile unsigned char I_parOut_0029 = 0;
volatile unsigned char I_parThis_0029 = 0;
volatile unsigned char GPIO_OUTPUT_PORT11 = 0;
volatile unsigned char GPIO_OUTPUT_PORT12 = 0;
volatile unsigned char I_parOut_002a = 0;
volatile unsigned char I_parThis_002a = 0;
volatile unsigned char U_RMaqModoCorrecao = 0;
volatile unsigned char U_RPerfManualOK = 0;
volatile unsigned char I_parOut_002b = 0;
volatile unsigned char I_parThis_002b = 0;
volatile unsigned char I_parOut_002c = 0;
volatile unsigned char I_parThis_002c = 0;
volatile unsigned char I_parOut_002d = 0;
volatile unsigned char I_parThis_002d = 0;
volatile unsigned char U_RBloqueiaRecuo = 0;
volatile unsigned char I_parOut_002e = 0;
volatile unsigned char I_parThis_002e = 0;
volatile int U_TMaxPerfRecua = 0;
volatile unsigned char I_parOut_002f = 0;
volatile unsigned char I_parThis_002f = 0;
volatile unsigned char U_RStartCfgSrvJog = 0;
volatile unsigned char I_TEspFimCfgJog_antiglitch = 0;
volatile int U_TEspFimCfgJog = 0;
volatile unsigned char U_RSrvManualOK = 0;
volatile unsigned char I_parOut_0030 = 0;
volatile unsigned char I_parThis_0030 = 0;
volatile unsigned char GPIO_INPUT_PORT8 = 0;
volatile unsigned char GPIO_INPUT_PORT9 = 0;
volatile unsigned char I_parOut_0031 = 0;
volatile unsigned char I_parThis_0031 = 0;
volatile unsigned char U_RAvancarMesaMan = 0;
volatile unsigned char I_oneShot_0010 = 0;
volatile unsigned char I_parOut_0032 = 0;
volatile unsigned char I_parThis_0032 = 0;
volatile unsigned char I_parOut_0033 = 0;
volatile unsigned char I_parThis_0033 = 0;
volatile unsigned char U_RRecuarMesaMan = 0;
volatile unsigned char I_oneShot_0011 = 0;
volatile unsigned char I_parOut_0034 = 0;
volatile unsigned char I_parThis_0034 = 0;
volatile unsigned char I_oneShot_0012 = 0;
volatile unsigned char I_oneShot_0013 = 0;
volatile unsigned char I_parOut_0035 = 0;
volatile unsigned char I_parThis_0035 = 0;
volatile unsigned char I_oneShot_0014 = 0;
volatile int U_TCfgSrvJogAvcTO = 0;
volatile unsigned char I_oneShot_0015 = 0;
volatile int U_SrvRegConfig = 0;
volatile unsigned char I_oneShot_0016 = 0;
volatile unsigned char I_oneShot_0017 = 0;
volatile unsigned char I_oneShot_0018 = 0;
volatile unsigned char I_parOut_0036 = 0;
volatile unsigned char I_parThis_0036 = 0;
volatile unsigned char I_parOut_0037 = 0;
volatile unsigned char I_parThis_0037 = 0;
volatile unsigned char I_oneShot_0019 = 0;
volatile int U_TCfgSrvJogRecTO = 0;
volatile unsigned char I_oneShot_001a = 0;
volatile unsigned char I_oneShot_001b = 0;
volatile unsigned char I_oneShot_001c = 0;
volatile unsigned char I_oneShot_001d = 0;
volatile unsigned char I_parOut_0038 = 0;
volatile unsigned char I_parThis_0038 = 0;
volatile unsigned char I_parOut_0039 = 0;
volatile unsigned char I_parThis_0039 = 0;
volatile unsigned char I_oneShot_001e = 0;
volatile int U_TCfgSrvJogDesTO = 0;
volatile unsigned char I_oneShot_001f = 0;
volatile unsigned char I_oneShot_0020 = 0;
volatile unsigned char I_oneShot_0021 = 0;
volatile unsigned char I_oneShot_0022 = 0;
volatile unsigned char I_parOut_003a = 0;
volatile unsigned char I_parThis_003a = 0;
volatile unsigned char I_parOut_003b = 0;
volatile unsigned char I_parThis_003b = 0;
volatile unsigned char I_parOut_003c = 0;
volatile unsigned char I_parThis_003c = 0;
volatile unsigned char U_RMaqStartPosic = 0;
volatile int U_VSrvPosMesa = 0;
volatile unsigned char I_parOut_003d = 0;
volatile unsigned char I_parThis_003d = 0;
volatile unsigned char I_parOut_003e = 0;
volatile unsigned char I_parThis_003e = 0;
volatile unsigned char I_parOut_003f = 0;
volatile unsigned char I_parThis_003f = 0;
volatile unsigned char U_RSrvStartPosic = 0;
volatile int U_SrvPosMesa = 0;
volatile unsigned char I_parOut_0040 = 0;
volatile unsigned char I_parThis_0040 = 0;
volatile int U_TCfgSrvPosicTO = 0;
volatile unsigned char I_parOut_0041 = 0;
volatile unsigned char I_parThis_0041 = 0;
volatile unsigned char I_oneShot_0023 = 0;
volatile unsigned char I_oneShot_0024 = 0;
volatile unsigned char I_oneShot_0025 = 0;
volatile unsigned char I_oneShot_0026 = 0;
volatile unsigned char I_oneShot_0027 = 0;
volatile unsigned char I_oneShot_0028 = 0;
volatile int U_TSrvEsperaPosic = 0;
volatile unsigned char U_RSrvReadPosicOK = 0;
volatile unsigned char I_oneShot_0029 = 0;
volatile unsigned char I_oneShot_002a = 0;
volatile unsigned char I_parOut_0042 = 0;
volatile unsigned char I_parThis_0042 = 0;
volatile int U_TEspReadPosicOK = 0;
volatile unsigned char I_oneShot_002b = 0;
volatile unsigned char I_parOut_0043 = 0;
volatile unsigned char I_parThis_0043 = 0;
volatile unsigned char U_RMaqCalcCorrecao = 0;
volatile unsigned char I_parOut_0044 = 0;
volatile unsigned char I_parThis_0044 = 0;
volatile unsigned char U_RPosCorrConfig = 0;
volatile unsigned char I_parOut_0045 = 0;
volatile unsigned char I_parThis_0045 = 0;
volatile unsigned char U_RCalcCorrZero = 0;
volatile unsigned char U_RCalcCorrPosic = 0;
volatile int U_SrvMesaCurso = 0;
volatile unsigned char I_oneShot_002c = 0;
volatile unsigned char I_oneShot_002d = 0;
volatile unsigned char I_oneShot_002e = 0;
volatile unsigned char I_parOut_0046 = 0;
volatile unsigned char I_parThis_0046 = 0;
volatile int U_CalcCorrPosDif = 0;
volatile int U_CalcCorrPosIni = 0;
volatile unsigned char I_oneShot_002f = 0;
volatile unsigned char I_parOut_0047 = 0;
volatile unsigned char I_parThis_0047 = 0;
volatile unsigned char I_oneShot_0030 = 0;
volatile unsigned char I_parOut_0048 = 0;
volatile unsigned char I_parThis_0048 = 0;
volatile unsigned char I_parOut_0049 = 0;
volatile unsigned char I_parThis_0049 = 0;
volatile unsigned char U_RMaqCalcRelacao = 0;
volatile unsigned char I_parOut_004a = 0;
volatile unsigned char I_parThis_004a = 0;
volatile unsigned char U_RPosRelConfig = 0;
volatile unsigned char I_parOut_004b = 0;
volatile unsigned char I_parThis_004b = 0;
volatile unsigned char U_RCalcRelZero = 0;
volatile unsigned char U_RCalcRelE2Fim = 0;
volatile unsigned char I_oneShot_0031 = 0;
volatile unsigned char I_oneShot_0032 = 0;
volatile unsigned char U_RCalcRelE2OK = 0;
volatile unsigned char I_parOut_004c = 0;
volatile unsigned char I_parThis_004c = 0;
volatile int U_CalcRelPerfFtr = 0;
volatile int U_CalcRelSrvFtr = 0;
volatile unsigned char I_oneShot_0033 = 0;
volatile unsigned char U_RCalcRelE1OK = 0;
volatile unsigned char I_parOut_004d = 0;
volatile unsigned char I_parThis_004d = 0;
volatile unsigned char U_RCalcRelE2Start = 0;
volatile unsigned char I_oneShot_0034 = 0;
volatile unsigned char I_parOut_004e = 0;
volatile unsigned char I_parThis_004e = 0;
volatile unsigned char U_RCalcRelE1Start = 0;
volatile unsigned char I_parOut_004f = 0;
volatile unsigned char I_parThis_004f = 0;
volatile int U_TEspEncPrfIniTO = 0;
volatile unsigned char I_oneShot_0035 = 0;
volatile int U_CalcRelPerfIni = 0;
volatile unsigned char I_oneShot_0036 = 0;
volatile unsigned char I_parOut_0050 = 0;
volatile unsigned char I_parThis_0050 = 0;
volatile int U_CalcRelMesaIni = 0;
volatile unsigned char I_parOut_0051 = 0;
volatile unsigned char I_parThis_0051 = 0;
volatile unsigned char I_oneShot_0037 = 0;
volatile unsigned char I_parOut_0052 = 0;
volatile unsigned char I_parThis_0052 = 0;
volatile int U_CalcRelPosFim = 0;
volatile unsigned char I_parOut_0053 = 0;
volatile unsigned char I_parThis_0053 = 0;
volatile unsigned char I_oneShot_0038 = 0;
volatile unsigned char I_parOut_0054 = 0;
volatile unsigned char I_parThis_0054 = 0;
volatile unsigned char U_RCalcRelE1Fim = 0;
volatile unsigned char I_parOut_0055 = 0;
volatile unsigned char I_parThis_0055 = 0;
volatile int U_TEspEncPrfFimTO = 0;
volatile unsigned char I_oneShot_0039 = 0;
volatile int U_CalcRelPerfFim = 0;
volatile unsigned char I_oneShot_003a = 0;
volatile unsigned char I_parOut_0056 = 0;
volatile unsigned char I_parThis_0056 = 0;
volatile int U_CalcRelMesaDif = 0;
volatile int U_CalcRelPerfDif = 0;
volatile unsigned char I_parOut_0057 = 0;
volatile unsigned char I_parThis_0057 = 0;
volatile int U_TEspEncSrvIniTO = 0;
volatile unsigned char I_oneShot_003b = 0;
volatile int U_CalcRelSrvIni = 0;
volatile unsigned char I_oneShot_003c = 0;
volatile unsigned char I_parOut_0058 = 0;
volatile unsigned char I_parThis_0058 = 0;
volatile unsigned char I_parOut_0059 = 0;
volatile unsigned char I_parThis_0059 = 0;
volatile int U_TEspEncSrvFimTO = 0;
volatile unsigned char I_oneShot_003d = 0;
volatile int U_CalcRelSrvFim = 0;
volatile unsigned char I_oneShot_003e = 0;
volatile unsigned char I_parOut_005a = 0;
volatile unsigned char I_parThis_005a = 0;
volatile int U_CalcRelSrvDif = 0;
volatile unsigned char I_oneShot_003f = 0;
volatile unsigned char I_parOut_005b = 0;
volatile unsigned char I_parThis_005b = 0;
volatile unsigned char I_parOut_005c = 0;
volatile unsigned char I_parThis_005c = 0;
volatile unsigned char U_RMaqCalcTamMin = 0;
volatile unsigned char I_parOut_005d = 0;
volatile unsigned char I_parThis_005d = 0;
volatile unsigned char U_RPosTamMinConfig = 0;
volatile unsigned char I_parOut_005e = 0;
volatile unsigned char I_parThis_005e = 0;
volatile unsigned char U_RCalcTamMinZero = 0;
volatile unsigned char U_RCalcTamMinPosic = 0;
volatile unsigned char I_oneShot_0040 = 0;
volatile unsigned char I_oneShot_0041 = 0;
volatile unsigned char I_parOut_005f = 0;
volatile unsigned char I_parThis_005f = 0;
volatile unsigned char U_RStartCfgInvMan = 0;
volatile int U_CalcTamMinIni = 0;
volatile unsigned char I_oneShot_0042 = 0;
volatile unsigned char I_oneShot_0043 = 0;
volatile unsigned char I_oneShot_0044 = 0;
volatile unsigned char I_parOut_0060 = 0;
volatile unsigned char I_parThis_0060 = 0;
volatile unsigned char U_RStartCfgInvAuto = 0;
volatile unsigned char I_oneShot_0045 = 0;
volatile unsigned char I_oneShot_0046 = 0;
volatile unsigned char I_parOut_0061 = 0;
volatile unsigned char I_parThis_0061 = 0;
volatile unsigned char I_parOut_0062 = 0;
volatile unsigned char I_parThis_0062 = 0;
volatile unsigned char U_RStartCfgSrvPar = 0;
volatile unsigned char U_RMaqCfgSrvParOK = 0;
volatile unsigned char I_parOut_0063 = 0;
volatile unsigned char I_parThis_0063 = 0;
volatile unsigned char I_oneShot_0047 = 0;
volatile int U_TCfgSrvIniTO = 0;
volatile unsigned char I_oneShot_0048 = 0;
volatile unsigned char I_oneShot_0049 = 0;
volatile unsigned char I_oneShot_004a = 0;
volatile int U_SrvVelJog = 0;
volatile unsigned char I_oneShot_004b = 0;
volatile unsigned char I_oneShot_004c = 0;
volatile int U_SrvVelPos = 0;
volatile unsigned char I_oneShot_004d = 0;
volatile unsigned char I_parOut_0064 = 0;
volatile unsigned char I_parThis_0064 = 0;
volatile unsigned char I_parOut_0065 = 0;
volatile unsigned char I_parThis_0065 = 0;
volatile int U_TSrvInitAutoTO = 0;
volatile unsigned char I_oneShot_004e = 0;
volatile unsigned char I_oneShot_004f = 0;
volatile unsigned char I_oneShot_0050 = 0;
volatile unsigned char I_oneShot_0051 = 0;
volatile int U_TEspServoON = 0;
volatile unsigned char I_parOut_0066 = 0;
volatile unsigned char I_parThis_0066 = 0;
volatile int U_TSrvStartRefTO = 0;
volatile unsigned char I_oneShot_0052 = 0;
volatile unsigned char I_oneShot_0053 = 0;
volatile unsigned char I_oneShot_0054 = 0;
volatile unsigned char I_oneShot_0055 = 0;
volatile unsigned char I_parOut_0067 = 0;
volatile unsigned char I_parThis_0067 = 0;
volatile int U_CAguardaMesaOK = 0;
volatile unsigned char U_RSrvReadInitOK = 0;
volatile unsigned char I_oneShot_0056 = 0;
volatile int U_SrvRegFlags = 0;
volatile unsigned char I_oneShot_0057 = 0;
volatile unsigned char I_oneShot_0058 = 0;
volatile int U_SrvRegStatus = 0;
volatile unsigned char I_oneShot_0059 = 0;
volatile unsigned char I_parOut_0068 = 0;
volatile unsigned char I_parThis_0068 = 0;
volatile int U_TEspReadInitOK = 0;
volatile unsigned char I_parOut_0069 = 0;
volatile unsigned char I_parThis_0069 = 0;
volatile unsigned char I_parOut_006a = 0;
volatile unsigned char I_parThis_006a = 0;
volatile unsigned char I_oneShot_005a = 0;
volatile unsigned char I_parOut_006b = 0;
volatile unsigned char I_parThis_006b = 0;
volatile unsigned char I_parOut_006c = 0;
volatile unsigned char I_parThis_006c = 0;
volatile int U_TSrvFimRefTO = 0;
volatile unsigned char I_oneShot_005b = 0;
volatile unsigned char I_oneShot_005c = 0;
volatile unsigned char I_oneShot_005d = 0;
volatile unsigned char I_oneShot_005e = 0;
volatile unsigned char I_parOut_006d = 0;
volatile unsigned char I_parThis_006d = 0;
volatile unsigned char I_oneShot_005f = 0;
volatile unsigned char I_parOut_006e = 0;
volatile unsigned char I_parThis_006e = 0;
volatile unsigned char I_oneShot_0060 = 0;
volatile unsigned char U_RMaqCorteEstat = 0;
volatile unsigned char I_parOut_006f = 0;
volatile unsigned char I_parThis_006f = 0;
volatile unsigned char U_RMaqPosConfig = 0;
volatile unsigned char I_parOut_0070 = 0;
volatile unsigned char I_parThis_0070 = 0;
volatile unsigned char U_RCorteEstatZero = 0;
volatile unsigned char U_RCorteEstatPosic = 0;
volatile unsigned char I_parOut_0071 = 0;
volatile unsigned char I_parThis_0071 = 0;
volatile unsigned char I_parOut_0072 = 0;
volatile unsigned char I_parThis_0072 = 0;
volatile unsigned char I_parOut_0073 = 0;
volatile unsigned char I_parThis_0073 = 0;
volatile unsigned char U_RMaqErroPosic = 0;
volatile unsigned char I_oneShot_0061 = 0;
volatile unsigned char I_oneShot_0062 = 0;
volatile unsigned char U_RCorteEstatRetOK = 0;
volatile unsigned char I_oneShot_0063 = 0;
volatile unsigned char I_oneShot_0064 = 0;
volatile unsigned char I_oneShot_0065 = 0;
volatile unsigned char I_oneShot_0066 = 0;
volatile unsigned char I_parOut_0074 = 0;
volatile unsigned char I_parThis_0074 = 0;
volatile unsigned char I_parOut_0075 = 0;
volatile unsigned char I_parThis_0075 = 0;
volatile unsigned char I_parOut_0076 = 0;
volatile unsigned char I_parThis_0076 = 0;
volatile int U_TCorteEstatTO = 0;
volatile unsigned char I_oneShot_0067 = 0;
volatile unsigned char I_oneShot_0068 = 0;
volatile unsigned char I_oneShot_0069 = 0;
volatile unsigned char I_oneShot_006a = 0;
volatile unsigned char I_parOut_0077 = 0;
volatile unsigned char I_parThis_0077 = 0;
volatile unsigned char GPIO_OUTPUT_PORT7 = 0;
volatile unsigned char I_oneShot_006b = 0;
volatile unsigned char I_parOut_0078 = 0;
volatile unsigned char I_parThis_0078 = 0;
volatile unsigned char U_RStartCfgSrvAuto = 0;
volatile unsigned char I_parOut_0079 = 0;
volatile unsigned char I_parThis_0079 = 0;
volatile unsigned char I_oneShot_006c = 0;
volatile int U_TCfgSrvAutoTO = 0;
volatile unsigned char I_oneShot_006d = 0;
volatile unsigned char I_oneShot_006e = 0;
volatile unsigned char I_oneShot_006f = 0;
volatile unsigned char I_oneShot_0070 = 0;
volatile unsigned char I_parOut_007a = 0;
volatile unsigned char I_parThis_007a = 0;
volatile unsigned char U_RStartCfgSrvMan = 0;
volatile unsigned char I_parOut_007b = 0;
volatile unsigned char I_parThis_007b = 0;
volatile unsigned char I_oneShot_0071 = 0;
volatile int U_TCfgSrvManTO = 0;
volatile unsigned char I_oneShot_0072 = 0;
volatile unsigned char I_oneShot_0073 = 0;
volatile unsigned char I_oneShot_0074 = 0;
volatile unsigned char I_oneShot_0075 = 0;
volatile unsigned char I_parOut_007c = 0;
volatile unsigned char I_parThis_007c = 0;
volatile unsigned char I_parOut_007d = 0;
volatile unsigned char I_parThis_007d = 0;
volatile unsigned char I_parOut_007e = 0;
volatile unsigned char I_parThis_007e = 0;
volatile unsigned char I_oneShot_0076 = 0;
volatile int U_TCfgInvAutoTO = 0;
volatile unsigned char I_oneShot_0077 = 0;
volatile int U_WInvAutoVel = 0;
volatile unsigned char I_oneShot_0078 = 0;
volatile unsigned char I_oneShot_0079 = 0;
volatile unsigned char I_oneShot_007a = 0;
volatile unsigned char I_oneShot_007b = 0;
volatile int U_WInvAutoAcel = 0;
volatile unsigned char I_oneShot_007c = 0;
volatile unsigned char I_oneShot_007d = 0;
volatile int U_WInvAutoDesac = 0;
volatile unsigned char I_oneShot_007e = 0;
volatile unsigned char I_parOut_007f = 0;
volatile unsigned char I_parThis_007f = 0;
volatile unsigned char I_parOut_0080 = 0;
volatile unsigned char I_parThis_0080 = 0;
volatile unsigned char I_parOut_0081 = 0;
volatile unsigned char I_parThis_0081 = 0;
volatile unsigned char I_oneShot_007f = 0;
volatile int U_TCfgInvManTO = 0;
volatile unsigned char I_oneShot_0080 = 0;
volatile int U_WInvManVel = 0;
volatile unsigned char I_oneShot_0081 = 0;
volatile unsigned char I_oneShot_0082 = 0;
volatile unsigned char I_oneShot_0083 = 0;
volatile unsigned char I_oneShot_0084 = 0;
volatile int U_WInvManAcel = 0;
volatile unsigned char I_oneShot_0085 = 0;
volatile unsigned char I_oneShot_0086 = 0;
volatile int U_WInvManDesac = 0;
volatile unsigned char I_oneShot_0087 = 0;
volatile unsigned char I_parOut_0082 = 0;
volatile unsigned char I_parThis_0082 = 0;
volatile unsigned char U_RMaqVelReduzida = 0;
volatile unsigned char I_parOut_0083 = 0;
volatile unsigned char I_parThis_0083 = 0;
volatile unsigned char I_oneShot_0088 = 0;
volatile unsigned char I_parOut_0084 = 0;
volatile unsigned char I_parThis_0084 = 0;
volatile unsigned char U_RStartCfgVelRed = 0;
volatile unsigned char U_RStartCfgVelMax = 0;
volatile unsigned char I_oneShot_0089 = 0;
volatile unsigned char I_parOut_0085 = 0;
volatile unsigned char I_parThis_0085 = 0;
volatile unsigned char I_parOut_0086 = 0;
volatile unsigned char I_parThis_0086 = 0;
volatile int U_TCfgVelMaxTO = 0;
volatile unsigned char I_oneShot_008a = 0;
volatile unsigned char I_oneShot_008b = 0;
volatile unsigned char I_oneShot_008c = 0;
volatile unsigned char I_oneShot_008d = 0;
volatile unsigned char I_parOut_0087 = 0;
volatile unsigned char I_parThis_0087 = 0;
volatile int U_TCfgVelRedTO = 0;
volatile unsigned char I_oneShot_008e = 0;
volatile int U_WInvMaxSyncVel = 0;
volatile unsigned char I_oneShot_008f = 0;
volatile unsigned char I_oneShot_0090 = 0;
volatile unsigned char I_oneShot_0091 = 0;
volatile unsigned char U_RModoAutoOK = 0;
volatile unsigned char I_parOut_0088 = 0;
volatile unsigned char I_parThis_0088 = 0;
volatile unsigned char I_oneShot_0092 = 0;
volatile int U_TEspSistemaOK = 0;
volatile unsigned char I_oneShot_0093 = 0;
volatile unsigned char I_parOut_0089 = 0;
volatile unsigned char I_parThis_0089 = 0;
volatile unsigned char I_oneShot_0094 = 0;
volatile unsigned char U_RMaqFimProd = 0;
volatile unsigned char I_parOut_008a = 0;
volatile unsigned char I_parThis_008a = 0;
volatile int U_CQtdProd = 0;
volatile unsigned char I_parOut_008b = 0;
volatile unsigned char I_parThis_008b = 0;
volatile int U_MaqPecaTamMin = 0;
volatile unsigned char GPIO_INPUT_PORT18 = 0;
volatile unsigned char I_parOut_008c = 0;
volatile unsigned char I_parThis_008c = 0;
volatile int U_TEspFimMaterial = 0;
volatile unsigned char I_oneShot_0095 = 0;
volatile unsigned char I_parOut_008d = 0;
volatile unsigned char I_parThis_008d = 0;
volatile unsigned char I_parOut_008e = 0;
volatile unsigned char I_parThis_008e = 0;
volatile unsigned char I_oneShot_0096 = 0;
volatile unsigned char I_parOut_008f = 0;
volatile unsigned char I_parThis_008f = 0;
volatile unsigned char I_parOut_0090 = 0;
volatile unsigned char I_parThis_0090 = 0;
volatile unsigned char U_RModoDinam = 0;
volatile unsigned char U_RSubQtdProd = 0;
volatile unsigned char U_RMaqPosCorteSet = 0;
volatile unsigned char I_oneShot_0097 = 0;
volatile unsigned char I_oneShot_0098 = 0;
volatile unsigned char I_parOut_0091 = 0;
volatile unsigned char I_parThis_0091 = 0;
volatile unsigned char I_oneShot_0099 = 0;
volatile unsigned char I_parOut_0092 = 0;
volatile unsigned char I_parThis_0092 = 0;
volatile unsigned char I_oneShot_009a = 0;
volatile unsigned char I_parOut_0093 = 0;
volatile unsigned char I_parThis_0093 = 0;
volatile unsigned char I_oneShot_009b = 0;
volatile unsigned char I_parOut_0094 = 0;
volatile unsigned char I_parThis_0094 = 0;
volatile unsigned char I_parOut_0095 = 0;
volatile unsigned char I_parThis_0095 = 0;
volatile unsigned char I_parOut_0096 = 0;
volatile unsigned char I_parThis_0096 = 0;
volatile int U_TModoDinamTO = 0;
volatile unsigned char I_oneShot_009c = 0;
volatile unsigned char I_oneShot_009d = 0;
volatile unsigned char I_oneShot_009e = 0;
volatile unsigned char I_oneShot_009f = 0;
volatile int U_ProxFimCurso = 0;
volatile int U_MaqPosReduzVel = 0;
volatile int U_YMbAutoVel = 0;
volatile int U_MaxInvSyncVel = 0;
volatile unsigned char I_parOut_0097 = 0;
volatile unsigned char I_parThis_0097 = 0;
volatile int U_TSrvEsperaAcel = 0;
volatile unsigned char GPIO_INPUT_PORT17 = 0;
volatile unsigned char I_oneShot_00a0 = 0;
volatile unsigned char U_RMaqSincronizado = 0;
volatile unsigned char I_parOut_0098 = 0;
volatile unsigned char I_parThis_0098 = 0;
volatile unsigned char I_parOut_0099 = 0;
volatile unsigned char I_parThis_0099 = 0;
volatile unsigned char I_parOut_009a = 0;
volatile unsigned char I_parThis_009a = 0;
volatile unsigned char I_parOut_009b = 0;
volatile unsigned char I_parThis_009b = 0;
volatile unsigned char I_oneShot_00a1 = 0;
volatile unsigned char I_parOut_009c = 0;
volatile unsigned char I_parThis_009c = 0;
volatile unsigned char I_parOut_009d = 0;
volatile unsigned char I_parThis_009d = 0;
volatile int U_PosLimiteCorte = 0;
volatile unsigned char U_RMaqCortando = 0;
volatile unsigned char I_parOut_009e = 0;
volatile unsigned char I_parThis_009e = 0;
volatile unsigned char U_RPosEstatConfig = 0;
volatile unsigned char I_parOut_009f = 0;
volatile unsigned char I_parThis_009f = 0;
volatile unsigned char I_parOut_00a0 = 0;
volatile unsigned char I_parThis_00a0 = 0;
volatile int U_PosNovoCorte = 0;
volatile unsigned char I_parOut_00a1 = 0;
volatile unsigned char I_parThis_00a1 = 0;
volatile unsigned char U_RMaqFimCorte = 0;
volatile unsigned char I_parOut_00a2 = 0;
volatile unsigned char I_parThis_00a2 = 0;
volatile unsigned char I_oneShot_00a2 = 0;
volatile unsigned char I_parOut_00a3 = 0;
volatile unsigned char I_parThis_00a3 = 0;
volatile unsigned char I_parOut_00a4 = 0;
volatile unsigned char I_parThis_00a4 = 0;
volatile unsigned char I_parOut_00a5 = 0;
volatile unsigned char I_parThis_00a5 = 0;
volatile unsigned char I_oneShot_00a3 = 0;
volatile unsigned char I_parOut_00a6 = 0;
volatile unsigned char I_parThis_00a6 = 0;
volatile unsigned char I_parOut_00a7 = 0;
volatile unsigned char I_parThis_00a7 = 0;
volatile unsigned char I_oneShot_00a4 = 0;
volatile unsigned char I_parOut_00a8 = 0;
volatile unsigned char I_parThis_00a8 = 0;
volatile unsigned char I_oneShot_00a5 = 0;
volatile unsigned char I_parOut_00a9 = 0;
volatile unsigned char I_parThis_00a9 = 0;
volatile unsigned char I_parOut_00aa = 0;
volatile unsigned char I_parThis_00aa = 0;
volatile unsigned char I_parOut_00ab = 0;
volatile unsigned char I_parThis_00ab = 0;
volatile unsigned char I_oneShot_00a6 = 0;
volatile unsigned char I_parOut_00ac = 0;
volatile unsigned char I_parThis_00ac = 0;
volatile unsigned char I_parOut_00ad = 0;
volatile unsigned char I_parThis_00ad = 0;
volatile unsigned char I_parOut_00ae = 0;
volatile unsigned char I_parThis_00ae = 0;
volatile unsigned char I_parOut_00af = 0;
volatile unsigned char I_parThis_00af = 0;
volatile unsigned char I_oneShot_00a7 = 0;
volatile unsigned char I_parOut_00b0 = 0;
volatile unsigned char I_parThis_00b0 = 0;
volatile unsigned char I_oneShot_00a8 = 0;
volatile unsigned char I_parOut_00b1 = 0;
volatile unsigned char I_parThis_00b1 = 0;
volatile unsigned char I_oneShot_00a9 = 0;
volatile unsigned char I_parOut_00b2 = 0;
volatile unsigned char I_parThis_00b2 = 0;
volatile unsigned char I_oneShot_00aa = 0;
volatile unsigned char I_parOut_00b3 = 0;
volatile unsigned char I_parThis_00b3 = 0;
volatile unsigned char I_parOut_00b4 = 0;
volatile unsigned char I_parThis_00b4 = 0;
volatile unsigned char I_oneShot_00ab = 0;
volatile unsigned char I_parOut_00b5 = 0;
volatile unsigned char I_parThis_00b5 = 0;
volatile unsigned char I_oneShot_00ac = 0;
volatile unsigned char I_parOut_00b6 = 0;
volatile unsigned char I_parThis_00b6 = 0;
volatile unsigned char I_parOut_00b7 = 0;
volatile unsigned char I_parThis_00b7 = 0;
volatile unsigned char I_oneShot_00ad = 0;
volatile unsigned char I_oneShot_00ae = 0;
volatile unsigned char I_parOut_00b8 = 0;
volatile unsigned char I_parThis_00b8 = 0;
volatile unsigned char I_parOut_00b9 = 0;
volatile unsigned char I_parThis_00b9 = 0;
volatile unsigned char I_parOut_00ba = 0;
volatile unsigned char I_parThis_00ba = 0;
volatile unsigned char I_oneShot_00af = 0;
volatile unsigned char U_RSrvCheckFimProd = 0;
volatile unsigned char I_oneShot_00b0 = 0;
volatile unsigned char I_oneShot_00b1 = 0;
volatile unsigned char I_parOut_00bb = 0;
volatile unsigned char I_parThis_00bb = 0;
volatile int U_TEspFimProd = 0;
volatile unsigned char I_parOut_00bc = 0;
volatile unsigned char I_parThis_00bc = 0;
volatile unsigned char I_parOut_00bd = 0;
volatile unsigned char I_parThis_00bd = 0;
volatile unsigned char I_parOut_00be = 0;
volatile unsigned char I_parThis_00be = 0;
volatile unsigned char I_oneShot_00b2 = 0;
volatile unsigned char I_parOut_00bf = 0;
volatile unsigned char I_parThis_00bf = 0;
volatile unsigned char I_parOut_00c0 = 0;
volatile unsigned char I_parThis_00c0 = 0;
volatile unsigned char I_parOut_00c1 = 0;
volatile unsigned char I_parThis_00c1 = 0;
volatile unsigned char I_parOut_00c2 = 0;
volatile unsigned char I_parThis_00c2 = 0;
volatile unsigned char I_parOut_00c3 = 0;
volatile unsigned char I_parThis_00c3 = 0;
volatile unsigned char I_oneShot_00b3 = 0;
volatile unsigned char I_parOut_00c4 = 0;
volatile unsigned char I_parThis_00c4 = 0;
volatile unsigned char I_oneShot_00b4 = 0;
volatile unsigned char I_parOut_00c5 = 0;
volatile unsigned char I_parThis_00c5 = 0;
volatile int U_WInvModoAuto = 0;
volatile int U_WInvModoMan = 0;
volatile unsigned char I_oneShot_00b5 = 0;
volatile unsigned char I_parOut_00c6 = 0;
volatile unsigned char I_parThis_00c6 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 156 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_YLedUser) {  // YLedUser
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEsperaLedON < 99) {
            U_TEsperaLedON++;
            I_rung_top = 0;
        }
    } else {
        U_TEsperaLedON = 0;
    }

    if (!I_TEsperaLedOFF_antiglitch) {  // $TEsperaLedOFF_antiglitch
        U_TEsperaLedOFF = 99;
    }
    I_TEsperaLedOFF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEsperaLedOFF < 99) {
            U_TEsperaLedOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TEsperaLedOFF = 0;
    }

    U_YLedUser = I_rung_top;

    /* ] finish series */

    /* start rung 158 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (GPIO_INPUT_PORT1) {  // XEmergencia
        I_parThis_0000 = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (GPIO_INPUT_PORT3) {  // XFaltaFase
        I_parThis_0000 = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    U_RErroGeral = I_parThis_0001;

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0001;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_parThis_0001 = 0;
    }
    I_oneShot_0000 = I_scratch;

    if (I_parThis_0001) {  // $parThis_0001
        U_RMaqInitOK = 0;
    }

    /* ] finish series */
    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 159 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgMaqLib
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 3);  // YMbMaqLib
    }

    /* ] finish series */

    /* start rung 160 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_RErroGeral) {  // RErroGeral
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT4) {  // XInversorOK
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT2) {  // XTermBombaHidr
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0002 = 0;
    I_parThis_0002 = I_rung_top;
    if (!GPIO_INPUT_PORT15) {  // XServoOK
        I_parThis_0002 = 0;
    }

    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    /* start series [ */
    if (GPIO_OUTPUT_PORT8) {  // YSrvHabilita
        I_parThis_0002 = 0;
    }

    if (!I_TSrvEsperaHab_antiglitch) {  // $TSrvEsperaHab_antiglitch
        U_TSrvEsperaHab = 19;
    }
    I_TSrvEsperaHab_antiglitch = 1;
    if (!I_parThis_0002) {  // $parThis_0002
        if (U_TSrvEsperaHab < 19) {
            U_TSrvEsperaHab++;
            I_parThis_0002 = 1;
        }
    } else {
        U_TSrvEsperaHab = 0;
    }

    /* ] finish series */
    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_rung_top = I_parOut_0002;
    /* ] finish parallel */
    if (!(MODBUS_REGISTER[2] & (1 << 3))) {  // YMbMaqLib
        I_rung_top = 0;
    }

    U_RMaqOK = I_rung_top;

    /* ] finish series */

    /* start rung 161 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0001) {  // $oneShot_0001
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0001 = I_scratch;

    /* start parallel [ */
    I_parOut_0003 = 0;
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        MODBUS_REGISTER[2] &= ~(1 << 3);  // YMbMaqLib
    }

    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        GPIO_OUTPUT_PORT8 = 0;
    }

    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        U_RMaqInitStart = 0;
    }

    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        U_RMaqInit = 0;
    }

    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        U_RMaqInitFim = 0;
    }

    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        U_RMaqInitOK = 0;
    }

    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_rung_top = I_parOut_0003;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 162 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0004 = 0;
    I_parThis_0004 = I_rung_top;
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_0004 = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (!U_RMaqCfgInvErro) {  // RMaqCfgInvErro
        I_parThis_0004 = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (!U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_parThis_0004 = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_rung_top = I_parOut_0004;
    /* ] finish parallel */
    U_YLedErro = I_rung_top;

    /* ] finish series */

    /* start rung 163 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqCfgInvOK) {  // RMaqCfgInvOK
        I_rung_top = 0;
    }

    if (!U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_rung_top = 0;
    }

    U_RMaqConfigOK = I_rung_top;

    /* ] finish series */

    /* start rung 164 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RMaqInitOK) {  // RMaqInitOK
        I_rung_top = 0;
    }

    if (!U_RMaqConfigOK) {  // RMaqConfigOK
        I_rung_top = 0;
    }

    U_RMaqPronta = I_rung_top;

    /* ] finish series */

    /* start rung 165 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0005 = 0;
    I_parThis_0005 = I_rung_top;
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_0005 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_parThis_0005 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_parThis_0005 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_parThis_0005 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_parThis_0005 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_parThis_0005 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_rung_top = I_parOut_0005;
    /* ] finish parallel */
    U_RMaqOperando = I_rung_top;

    /* ] finish series */

    /* start rung 167 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[3] & (1 << 1)) {  // XDbgDeslReadEnc
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_EMaqPosAtual = ENC_Read();
    }

    /* ] finish series */

    /* start rung 168 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 2))) {  // XDbgSimulaEnc
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        I_scratch2 = 25;
        U_EMaqPosAtual = U_EMaqPosAtual + I_scratch2;
    }

    /* ] finish series */

    /* start rung 173 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_MaqPosAtual = U_EMaqPosAtual * MODBUS_REGISTER[22];
    }

    /* ] finish series */

    /* start rung 174 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        U_MaqPosAtual = U_MaqPosAtual * I_scratch2;
    }

    /* ] finish series */

    /* start rung 175 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 4;
        if(I_scratch2) U_MaqPosAtual = U_MaqPosAtual / I_scratch2;
    }

    /* ] finish series */

    /* start rung 176 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch2 = 0;
    if (MODBUS_REGISTER[21] > I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if(MODBUS_REGISTER[21]) U_MaqPosAtual = U_MaqPosAtual / MODBUS_REGISTER[21];
    }

    /* ] finish series */

    /* start rung 177 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_MaqPosAtual = U_MaqPosAtual * MODBUS_REGISTER[20];
    }

    /* ] finish series */

    /* start rung 178 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 1000;
        if(I_scratch2) U_MaqPosAtual = U_MaqPosAtual / I_scratch2;
    }

    /* ] finish series */

    /* start rung 179 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_PosProxCorte = U_MaqPosCorte + U_MaqPecaTam;
    }

    /* ] finish series */

    /* start rung 180 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_DifPosCorte = U_PosProxCorte - U_MaqPosAtual;
    }

    /* ] finish series */

    /* start rung 181 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch = 0;
        U_DifPosCorteInv = I_scratch - U_DifPosCorte;
    }

    /* ] finish series */

    /* start rung 182 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_RCalcVarPerfil) {  // RCalcVarPerfil
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TTmrCalcPerfil < 9) {
            U_TTmrCalcPerfil++;
            I_rung_top = 0;
        }
    } else {
        U_TTmrCalcPerfil = 0;
    }

    /* start parallel [ */
    I_parOut_0006 = 0;
    I_parThis_0006 = I_rung_top;
    U_RCalcVarPerfil = I_parThis_0006;

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_PerfilVariacao = U_PerfilPosAnt - U_MaqPosAtual;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_RMaqPerfilParado = 0;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    /* start series [ */
    I_scratch2 = -1;
    if (I_scratch2 > U_PerfilVariacao) {
        I_parThis_0006 = 0;
    }

    I_scratch2 = 1;
    if (U_PerfilVariacao > I_scratch2) {
        I_parThis_0006 = 0;
    }

    if (I_parThis_0006) {  // $parThis_0006
        U_RMaqPerfilParado = 1;
    }

    /* ] finish series */
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_PerfilPosAnt = U_MaqPosAtual;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_rung_top = I_parOut_0006;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 183 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        if(I_scratch2) MODBUS_REGISTER[4] = U_MaqPosAtual / I_scratch2;
    }

    /* ] finish series */

    /* start rung 185 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT10) {  // XSnsSupHidr
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspSnsSupON < 4) {
            U_TEspSnsSupON++;
            I_rung_top = 0;
        }
    } else {
        U_TEspSnsSupON = 0;
    }

    U_RSnsSupCortePerf = I_rung_top;

    /* ] finish series */

    /* start rung 186 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT11) {  // XSnsInfHidr
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspSnsInfON < 4) {
            U_TEspSnsInfON++;
            I_rung_top = 0;
        }
    } else {
        U_TEspSnsInfON = 0;
    }

    U_RSnsInfCortePerf = I_rung_top;

    /* ] finish series */

    /* start rung 187 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    U_RCortePronto = I_rung_top;

    /* ] finish series */

    /* start rung 188 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSnsSupCortePerf) {  // RSnsSupCortePerf
        I_rung_top = 0;
    }

    if (U_RSnsInfCortePerf) {  // RSnsInfCortePerf
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT12) {  // XSnsSupPiloto
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT13) {  // XSnsInfPiloto
        I_rung_top = 0;
    }

    U_RFerramCorteCima = I_rung_top;

    /* ] finish series */

    /* start rung 190 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0007 = 0;
    I_parThis_0007 = I_rung_top;
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    if (!U_RMaqInitCorte) {  // RMaqInitCorte
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_rung_top = I_parOut_0007;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 1;
    }

    /* ] finish series */

    /* start rung 191 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TBombaHidrEstab < 499) {
            U_TBombaHidrEstab++;
            I_rung_top = 0;
        }
    } else {
        U_TBombaHidrEstab = 0;
    }

    U_RBombaHidrOK = I_rung_top;

    /* ] finish series */

    /* start rung 192 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0008 = 0;
    I_parThis_0008 = I_rung_top;
    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_parThis_0008 = 0;
    }

    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_parThis_0008 = 0;
    }

    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_0008 = 0;
    }

    if (I_parThis_0008) {  // $parThis_0008
        if (U_TBombaHidrInat < 29999) {
            U_TBombaHidrInat++;
            I_parThis_0008 = 0;
        }
    } else {
        U_TBombaHidrInat = 0;
    }

    /* ] finish series */
    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_parThis_0008 = I_rung_top;
    /* start series [ */
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_0008 = 0;
    }

    /* start parallel [ */
    I_parOut_0009 = 0;
    I_parThis_0009 = I_parThis_0008;
    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_0009 = 0;
    }

    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_parThis_0009 = I_parThis_0008;
    if (U_RModoAuto) {  // RModoAuto
        I_parThis_0009 = 0;
    }

    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_parThis_0008 = I_parOut_0009;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_rung_top = I_parOut_0008;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 0;
    }

    /* ] finish series */

    /* start rung 194 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    if (U_RErroCortePerf) {  // RErroCortePerf
        I_rung_top = 0;
    }

    if (!U_RCortarPerf) {  // RCortarPerf
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_000a = 0;
    I_parThis_000a = I_rung_top;
    if (U_RMaqOperando) {  // RMaqOperando
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_rung_top = I_parOut_000a;
    /* ] finish parallel */
    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_000b = 0;
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_RCortePerfAtivo = 1;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        GPIO_OUTPUT_PORT3 = 1;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_RCortarPerf = 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        MODBUS_REGISTER[2] &= ~(1 << 1);  // YMbCorteIHM
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_RMaqCortePerf = 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_RCorteRealizado = 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_CNumTentPiloto = 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        MODBUS_REGISTER[1] &= ~(1 << 15);  // YMbDbgAcionouSns
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_RMaqUsarPiloto = 1;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    /* start series [ */
    if (!U_RRepeteSemPiloto) {  // RRepeteSemPiloto
        I_parThis_000b = 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        U_RMaqUsarPiloto = 0;
    }

    /* ] finish series */
    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_RRepeteSemPiloto = 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_rung_top = I_parOut_000b;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 195 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0002) {  // $oneShot_0002
        I_rung_top = 0;
    }
    I_oneShot_0002 = I_scratch;

    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT7) {  // XCortePerfManual
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RRepeteSemPiloto = 1;
    }

    /* ] finish series */

    /* start rung 196 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_000c = 0;
    I_parThis_000c = I_rung_top;
    /* start series [ */
    if (!U_RCortePronto) {  // RCortePronto
        I_parThis_000c = 0;
    }

    if (I_parThis_000c) {  // $parThis_000c
        if (U_TMaxCortePerf < 499) {
            U_TMaxCortePerf++;
            I_parThis_000c = 0;
        }
    } else {
        U_TMaxCortePerf = 0;
    }

    /* ] finish series */
    if (I_parThis_000c) {  // $parThis_000c
        I_parOut_000c = 1;
    }
    I_parThis_000c = I_rung_top;
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_000c = 0;
    }

    if (I_parThis_000c) {  // $parThis_000c
        I_parOut_000c = 1;
    }
    I_parThis_000c = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YMbModoAutoAtivo
        I_parThis_000c = 0;
    }

    if (!GPIO_INPUT_PORT16) {  // XServoProxFim
        I_parThis_000c = 0;
    }

    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_000c = 0;
    }

    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_parThis_000c = 0;
    }

    if (U_RCorteRealizado) {  // RCorteRealizado
        I_parThis_000c = 0;
    }

    /* ] finish series */
    if (I_parThis_000c) {  // $parThis_000c
        I_parOut_000c = 1;
    }
    I_rung_top = I_parOut_000c;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_000d = 0;
    I_parThis_000d = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_000e = 0;
    I_parThis_000e = I_parThis_000d;
    if (I_parThis_000e) {  // $parThis_000e
        if (U_TRecuoCorteTO < 199) {
            U_TRecuoCorteTO++;
            I_parThis_000e = 0;
        }
    } else {
        U_TRecuoCorteTO = 0;
    }

    if (I_parThis_000e) {  // $parThis_000e
        I_parOut_000e = 1;
    }
    I_parThis_000e = I_parThis_000d;
    if (!U_RErroGeral) {  // RErroGeral
        I_parThis_000e = 0;
    }

    if (I_parThis_000e) {  // $parThis_000e
        I_parOut_000e = 1;
    }
    I_parThis_000d = I_parOut_000e;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_000f = 0;
    I_parThis_000f = I_parThis_000d;
    if (I_parThis_000f) {  // $parThis_000f
        U_RCortePerfAtivo = 0;
    }

    if (I_parThis_000f) {  // $parThis_000f
        I_parOut_000f = 1;
    }
    I_parThis_000f = I_parThis_000d;
    if (I_parThis_000f) {  // $parThis_000f
        GPIO_OUTPUT_PORT3 = 0;
    }

    if (I_parThis_000f) {  // $parThis_000f
        I_parOut_000f = 1;
    }
    I_parThis_000d = I_parOut_000f;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {  // $parThis_000d
        U_RErroCortePerf = 1;
    }

    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {  // $parThis_000d
        U_RCortePerfDesce = 0;
    }

    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_000d;
    if (I_oneShot_0003) {  // $oneShot_0003
        I_parThis_000d = 0;
    }
    I_oneShot_0003 = I_scratch;

    if (U_RCortePilotar) {  // RCortePilotar
        I_parThis_000d = 0;
    }

    if (I_parThis_000d) {  // $parThis_000d
        GPIO_OUTPUT_PORT3 = 0;
    }

    /* ] finish series */
    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {  // $parThis_000d
        U_RCortePilotar = 0;
    }

    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {  // $parThis_000d
        GPIO_OUTPUT_PORT2 = 0;
    }

    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_rung_top = I_parOut_000d;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 197 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT13) {  // XSnsInfPiloto
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT12) {  // XSnsSupPiloto
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspPilotoEstab < 4) {
            U_TEspPilotoEstab++;
            I_rung_top = 0;
        }
    } else {
        U_TEspPilotoEstab = 0;
    }

    U_RCortePilotoCima = I_rung_top;

    /* ] finish series */

    /* start rung 198 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT14) {  // XSnsSupTravaMesa
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0004) {  // $oneShot_0004
        I_rung_top = 0;
    }
    I_oneShot_0004 = I_scratch;

    /* start parallel [ */
    I_parOut_0010 = 0;
    I_parThis_0010 = I_rung_top;
    /* start series [ */
    if (!U_RMaqUsarPiloto) {  // RMaqUsarPiloto
        I_parThis_0010 = 0;
    }

    if (I_parThis_0010) {  // $parThis_0010
        U_RCortePilotar = 1;
    }

    /* ] finish series */
    if (I_parThis_0010) {  // $parThis_0010
        I_parOut_0010 = 1;
    }
    I_parThis_0010 = I_rung_top;
    /* start series [ */
    if (U_RMaqUsarPiloto) {  // RMaqUsarPiloto
        I_parThis_0010 = 0;
    }

    if (I_parThis_0010) {  // $parThis_0010
        U_RCortePerfDesce = 1;
    }

    /* ] finish series */
    if (I_parThis_0010) {  // $parThis_0010
        I_parOut_0010 = 1;
    }
    I_rung_top = I_parOut_0010;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 199 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_rung_top = 0;
    }

    if (!U_RCortePilotar) {  // RCortePilotar
        I_rung_top = 0;
    }

    if (!U_RCortePilotoCima) {  // RCortePilotoCima
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT2) {  // YPilotoDescer
        I_rung_top = 0;
    }

    if (U_RErroCortePerf) {  // RErroCortePerf
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEsperaPilotar < 4) {
            U_TEsperaPilotar++;
            I_rung_top = 0;
        }
    } else {
        U_TEsperaPilotar = 0;
    }

    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT2 = 1;
    }

    /* ] finish series */

    /* start rung 200 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (!GPIO_OUTPUT_PORT2) {  // YPilotoDescer
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TPilotoDesceTO < 49) {
            U_TPilotoDesceTO++;
            I_rung_top = 0;
        }
    } else {
        U_TPilotoDesceTO = 0;
    }

    /* start parallel [ */
    I_parOut_0011 = 0;
    I_parThis_0011 = I_rung_top;
    if (GPIO_INPUT_PORT13) {  // XSnsInfPiloto
        I_parThis_0011 = 0;
    }

    if (I_parThis_0011) {  // $parThis_0011
        I_parOut_0011 = 1;
    }
    I_parThis_0011 = I_rung_top;
    if (!GPIO_INPUT_PORT12) {  // XSnsSupPiloto
        I_parThis_0011 = 0;
    }

    if (I_parThis_0011) {  // $parThis_0011
        I_parOut_0011 = 1;
    }
    I_rung_top = I_parOut_0011;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0012 = 0;
    I_parThis_0012 = I_rung_top;
    if (I_parThis_0012) {  // $parThis_0012
        GPIO_OUTPUT_PORT2 = 0;
    }

    if (I_parThis_0012) {  // $parThis_0012
        I_parOut_0012 = 1;
    }
    I_parThis_0012 = I_rung_top;
    /* start series [ */
    if (I_parThis_0012) {  // $parThis_0012
        if (!I_oneShot_0005) {  // $oneShot_0005
            U_CNumTentPiloto++;
        }
    }
    I_oneShot_0005 = I_parThis_0012;
    if (U_CNumTentPiloto < 1) {
        I_parThis_0012 = 0;
    } else {
        I_parThis_0012 = 1;
    }

    /* start parallel [ */
    I_parOut_0013 = 0;
    I_parThis_0013 = I_parThis_0012;
    if (I_parThis_0013) {  // $parThis_0013
        U_RErroCortePerf = 1;
    }

    if (I_parThis_0013) {  // $parThis_0013
        I_parOut_0013 = 1;
    }
    I_parThis_0013 = I_parThis_0012;
    if (I_parThis_0013) {  // $parThis_0013
        U_RErroCortePiloto = 1;
    }

    if (I_parThis_0013) {  // $parThis_0013
        I_parOut_0013 = 1;
    }
    I_parThis_0013 = I_parThis_0012;
    if (I_parThis_0013) {  // $parThis_0013
        U_RCortePilotar = 0;
    }

    if (I_parThis_0013) {  // $parThis_0013
        I_parOut_0013 = 1;
    }
    I_parThis_0013 = I_parThis_0012;
    if (I_parThis_0013) {  // $parThis_0013
        U_CNumTentPiloto = 0;
    }

    if (I_parThis_0013) {  // $parThis_0013
        I_parOut_0013 = 1;
    }
    I_parThis_0012 = I_parOut_0013;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0012) {  // $parThis_0012
        I_parOut_0012 = 1;
    }
    I_rung_top = I_parOut_0012;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 201 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT13) {  // XSnsInfPiloto
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT12) {  // XSnsSupPiloto
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0006) {  // $oneShot_0006
        I_rung_top = 0;
    }
    I_oneShot_0006 = I_scratch;

    if (I_rung_top) {  // $rung_top
        U_RCortePerfDesce = 1;
    }

    /* ] finish series */

    /* start rung 202 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0014 = 0;
    I_parThis_0014 = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT13) {  // XSnsInfPiloto
        I_parThis_0014 = 0;
    }

    if (GPIO_INPUT_PORT12) {  // XSnsSupPiloto
        I_parThis_0014 = 0;
    }

    /* ] finish series */
    if (I_parThis_0014) {  // $parThis_0014
        I_parOut_0014 = 1;
    }
    I_parThis_0014 = I_rung_top;
    if (U_RMaqUsarPiloto) {  // RMaqUsarPiloto
        I_parThis_0014 = 0;
    }

    if (I_parThis_0014) {  // $parThis_0014
        I_parOut_0014 = 1;
    }
    I_rung_top = I_parOut_0014;
    /* ] finish parallel */
    if (!U_RCortePerfDesce) {  // RCortePerfDesce
        I_rung_top = 0;
    }

    if (!U_RSnsInfCortePerf) {  // RSnsInfCortePerf
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0015 = 0;
    I_parThis_0015 = I_rung_top;
    if (I_parThis_0015) {  // $parThis_0015
        U_RCortePerfDesce = 0;
    }

    if (I_parThis_0015) {  // $parThis_0015
        I_parOut_0015 = 1;
    }
    I_parThis_0015 = I_rung_top;
    if (I_parThis_0015) {  // $parThis_0015
        MODBUS_REGISTER[1] |= (1 << 15);  // YMbDbgAcionouSns
    }

    if (I_parThis_0015) {  // $parThis_0015
        I_parOut_0015 = 1;
    }
    I_parThis_0015 = I_rung_top;
    /* start series [ */
    if (!U_RSnsSupCortePerf) {  // RSnsSupCortePerf
        I_parThis_0015 = 0;
    }

    if (I_parThis_0015) {  // $parThis_0015
        U_RInvAtivar = 0;
    }

    /* ] finish series */
    if (I_parThis_0015) {  // $parThis_0015
        I_parOut_0015 = 1;
    }
    I_parThis_0015 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0016 = 0;
    I_parThis_0016 = I_parThis_0015;
    if (GPIO_INPUT_PORT16) {  // XServoProxFim
        I_parThis_0016 = 0;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_parThis_0015;
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_parThis_0016 = 0;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_parThis_0015;
    if (!U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_parThis_0016 = 0;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_parThis_0015;
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_parThis_0016 = 0;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_parThis_0015;
    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_0016 = 0;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0015 = I_parOut_0016;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0017 = 0;
    I_parThis_0017 = I_parThis_0015;
    if (I_parThis_0017) {  // $parThis_0017
        U_RCorteRealizado = 1;
    }

    if (I_parThis_0017) {  // $parThis_0017
        I_parOut_0017 = 1;
    }
    I_parThis_0017 = I_parThis_0015;
    if (I_parThis_0017) {  // $parThis_0017
        U_MaqPosCorte = 0;
    }

    if (I_parThis_0017) {  // $parThis_0017
        I_parOut_0017 = 1;
    }
    I_parThis_0017 = I_parThis_0015;
    if (I_parThis_0017) {  // $parThis_0017
        ENC_Reset();
    }

    if (I_parThis_0017) {  // $parThis_0017
        I_parOut_0017 = 1;
    }
    I_parThis_0015 = I_parOut_0017;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0015) {  // $parThis_0015
        I_parOut_0015 = 1;
    }
    I_rung_top = I_parOut_0015;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 203 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT13) {  // XSnsInfPiloto
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT12) {  // XSnsSupPiloto
        I_rung_top = 0;
    }

    if (U_RCortePerfDesce) {  // RCortePerfDesce
        I_rung_top = 0;
    }

    if (!U_RSnsSupCortePerf) {  // RSnsSupCortePerf
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0018 = 0;
    I_parThis_0018 = I_rung_top;
    if (I_parThis_0018) {  // $parThis_0018
        GPIO_OUTPUT_PORT2 = 0;
    }

    if (I_parThis_0018) {  // $parThis_0018
        I_parOut_0018 = 1;
    }
    I_parThis_0018 = I_rung_top;
    if (I_parThis_0018) {  // $parThis_0018
        U_RCortePilotar = 0;
    }

    if (I_parThis_0018) {  // $parThis_0018
        I_parOut_0018 = 1;
    }
    I_rung_top = I_parOut_0018;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 204 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0019 = 0;
    I_parThis_0019 = I_rung_top;
    /* start series [ */
    if (U_RCortePerfDesce) {  // RCortePerfDesce
        I_parThis_0019 = 0;
    }

    if (!U_RSnsSupCortePerf) {  // RSnsSupCortePerf
        I_parThis_0019 = 0;
    }

    I_scratch = I_parThis_0019;
    if (I_oneShot_0007) {  // $oneShot_0007
        I_parThis_0019 = 0;
    }
    I_oneShot_0007 = I_scratch;

    if (U_RMaqUsarPiloto) {  // RMaqUsarPiloto
        I_parThis_0019 = 0;
    }

    /* ] finish series */
    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_parThis_0019 = I_rung_top;
    /* start series [ */
    if (U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_0019 = 0;
    }

    if (!U_RCortePilotoCima) {  // RCortePilotoCima
        I_parThis_0019 = 0;
    }

    I_scratch = I_parThis_0019;
    if (I_oneShot_0008) {  // $oneShot_0008
        I_parThis_0019 = 0;
    }
    I_oneShot_0008 = I_scratch;

    if (!U_RMaqUsarPiloto) {  // RMaqUsarPiloto
        I_parThis_0019 = 0;
    }

    /* ] finish series */
    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_parThis_0019 = I_rung_top;
    /* start series [ */
    if (!U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_0019 = 0;
    }

    if (GPIO_OUTPUT_PORT2) {  // YPilotoDescer
        I_parThis_0019 = 0;
    }

    if (!U_RCortePilotoCima) {  // RCortePilotoCima
        I_parThis_0019 = 0;
    }

    if (!U_RSnsSupCortePerf) {  // RSnsSupCortePerf
        I_parThis_0019 = 0;
    }

    I_scratch = I_parThis_0019;
    if (I_oneShot_0009) {  // $oneShot_0009
        I_parThis_0019 = 0;
    }
    I_oneShot_0009 = I_scratch;

    /* ] finish series */
    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_rung_top = I_parOut_0019;
    /* ] finish parallel */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (U_RCortePilotar) {  // RCortePilotar
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT3 = 0;
    }

    /* ] finish series */

    /* start rung 205 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT14) {  // XSnsSupTravaMesa
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT3) {  // YCorteTravaMesa
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_000a) {  // $oneShot_000a
        I_rung_top = 0;
    }
    I_oneShot_000a = I_scratch;

    if (I_rung_top) {  // $rung_top
        U_RCortePerfAtivo = 0;
    }

    /* ] finish series */

    /* start rung 206 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_000b) {  // $oneShot_000b
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_000b = I_scratch;

    /* start parallel [ */
    I_parOut_001a = 0;
    I_parThis_001a = I_rung_top;
    if (I_parThis_001a) {  // $parThis_001a
        U_RCortePilotar = 0;
    }

    if (I_parThis_001a) {  // $parThis_001a
        I_parOut_001a = 1;
    }
    I_parThis_001a = I_rung_top;
    if (I_parThis_001a) {  // $parThis_001a
        U_RCortePerfDesce = 0;
    }

    if (I_parThis_001a) {  // $parThis_001a
        I_parOut_001a = 1;
    }
    I_rung_top = I_parOut_001a;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 207 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_001b = 0;
    I_parThis_001b = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT7) {  // XCortePerfManual
        I_parThis_001b = 0;
    }

    if (!I_TConfirmaOFF_antiglitch) {  // $TConfirmaOFF_antiglitch
        U_TConfirmaOFF = 9;
    }
    I_TConfirmaOFF_antiglitch = 1;
    if (!I_parThis_001b) {  // $parThis_001b
        if (U_TConfirmaOFF < 9) {
            U_TConfirmaOFF++;
            I_parThis_001b = 1;
        }
    } else {
        U_TConfirmaOFF = 0;
    }

    I_scratch = I_parThis_001b;
    if (!I_parThis_001b) {  // $parThis_001b
        if (I_oneShot_000c) {  // $oneShot_000c
            I_parThis_001b = 1;
        }
    } else {
        I_parThis_001b = 0;
    }
    I_oneShot_000c = I_scratch;

    /* ] finish series */
    if (I_parThis_001b) {  // $parThis_001b
        I_parOut_001b = 1;
    }
    I_parThis_001b = I_rung_top;
    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_001b = 0;
    }

    I_scratch = I_parThis_001b;
    if (!I_parThis_001b) {  // $parThis_001b
        if (I_oneShot_000d) {  // $oneShot_000d
            I_parThis_001b = 1;
        }
    } else {
        I_parThis_001b = 0;
    }
    I_oneShot_000d = I_scratch;

    if (U_RErroCortePiloto) {  // RErroCortePiloto
        I_parThis_001b = 0;
    }

    /* ] finish series */
    if (I_parThis_001b) {  // $parThis_001b
        I_parOut_001b = 1;
    }
    I_rung_top = I_parOut_001b;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_RRepeteSemPiloto = 0;
    }

    /* ] finish series */

    /* start rung 209 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_001c = 0;
    I_parThis_001c = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT13) {  // XSnsInfPiloto
        I_parThis_001c = 0;
    }

    if (GPIO_INPUT_PORT12) {  // XSnsSupPiloto
        I_parThis_001c = 0;
    }

    /* ] finish series */
    if (I_parThis_001c) {  // $parThis_001c
        I_parOut_001c = 1;
    }
    I_parThis_001c = I_rung_top;
    if (U_RMaqUsarPiloto) {  // RMaqUsarPiloto
        I_parThis_001c = 0;
    }

    if (I_parThis_001c) {  // $parThis_001c
        I_parOut_001c = 1;
    }
    I_rung_top = I_parOut_001c;
    /* ] finish parallel */
    if (!U_RCortePerfDesce) {  // RCortePerfDesce
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT4 = I_rung_top;

    /* ] finish series */

    /* start rung 210 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_001d = 0;
    I_parThis_001d = I_rung_top;
    /* start series [ */
    if (!U_RMaqInitCorte) {  // RMaqInitCorte
        I_parThis_001d = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_parThis_001d = 0;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_001d = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_parThis_001d = 0;
    }

    /* start parallel [ */
    I_parOut_001e = 0;
    I_parThis_001e = I_parThis_001d;
    /* start series [ */
    /* start parallel [ */
    I_parOut_001f = 0;
    I_parThis_001f = I_parThis_001e;
    /* start series [ */
    if (!GPIO_INPUT_PORT13) {  // XSnsInfPiloto
        I_parThis_001f = 0;
    }

    if (GPIO_INPUT_PORT12) {  // XSnsSupPiloto
        I_parThis_001f = 0;
    }

    /* ] finish series */
    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_parThis_001f = I_parThis_001e;
    /* start series [ */
    if (U_RMaqUsarPiloto) {  // RMaqUsarPiloto
        I_parThis_001f = 0;
    }

    if (!GPIO_INPUT_PORT14) {  // XSnsSupTravaMesa
        I_parThis_001f = 0;
    }

    /* ] finish series */
    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_parThis_001e = I_parOut_001f;
    /* ] finish parallel */
    if (U_RCortePerfDesce) {  // RCortePerfDesce
        I_parThis_001e = 0;
    }

    if (U_RSnsSupCortePerf) {  // RSnsSupCortePerf
        I_parThis_001e = 0;
    }

    /* ] finish series */
    if (I_parThis_001e) {  // $parThis_001e
        I_parOut_001e = 1;
    }
    I_parThis_001e = I_parThis_001d;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YMbModoAutoAtivo
        I_parThis_001e = 0;
    }

    if (!GPIO_INPUT_PORT16) {  // XServoProxFim
        I_parThis_001e = 0;
    }

    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_001e = 0;
    }

    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_parThis_001e = 0;
    }

    if (U_RCorteRealizado) {  // RCorteRealizado
        I_parThis_001e = 0;
    }

    /* ] finish series */
    if (I_parThis_001e) {  // $parThis_001e
        I_parOut_001e = 1;
    }
    I_parThis_001d = I_parOut_001e;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_rung_top = I_parOut_001d;
    /* ] finish parallel */
    GPIO_OUTPUT_PORT5 = I_rung_top;

    /* ] finish series */

    /* start rung 211 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0020 = 0;
    I_parThis_0020 = I_rung_top;
    if (!GPIO_OUTPUT_PORT4) {  // YHidrDesce
        I_parThis_0020 = 0;
    }

    if (I_parThis_0020) {  // $parThis_0020
        I_parOut_0020 = 1;
    }
    I_parThis_0020 = I_rung_top;
    if (!GPIO_OUTPUT_PORT5) {  // YHidrSobe
        I_parThis_0020 = 0;
    }

    if (I_parThis_0020) {  // $parThis_0020
        I_parOut_0020 = 1;
    }
    I_rung_top = I_parOut_0020;
    /* ] finish parallel */
    GPIO_OUTPUT_PORT13 = I_rung_top;

    /* ] finish series */

    /* start rung 212 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RErroCortePerf) {  // RErroCortePerf
        I_rung_top = 0;
    }

    if (U_RCortarPerf) {  // RCortarPerf
        I_rung_top = 0;
    }

    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }

    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0021 = 0;
    I_parThis_0021 = I_rung_top;
    if (I_parThis_0021) {  // $parThis_0021
        U_RErroCortePerf = 0;
    }

    if (I_parThis_0021) {  // $parThis_0021
        I_parOut_0021 = 1;
    }
    I_parThis_0021 = I_rung_top;
    if (I_parThis_0021) {  // $parThis_0021
        U_RErroCortePiloto = 0;
    }

    if (I_parThis_0021) {  // $parThis_0021
        I_parOut_0021 = 1;
    }
    I_rung_top = I_parOut_0021;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 213 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0022 = 0;
    I_parThis_0022 = I_rung_top;
    /* start series [ */
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_parThis_0022 = 0;
    }

    if (GPIO_INPUT_PORT5) {  // XPerfManAvanca
        I_parThis_0022 = 0;
    }

    if (GPIO_INPUT_PORT6) {  // XPerfManRecua
        I_parThis_0022 = 0;
    }

    /* start parallel [ */
    I_parOut_0023 = 0;
    I_parThis_0023 = I_parThis_0022;
    /* start series [ */
    if (!GPIO_INPUT_PORT7) {  // XCortePerfManual
        I_parThis_0023 = 0;
    }

    I_scratch = I_parThis_0023;
    if (I_oneShot_000e) {  // $oneShot_000e
        I_parThis_0023 = 0;
    }
    I_oneShot_000e = I_scratch;

    /* ] finish series */
    if (I_parThis_0023) {  // $parThis_0023
        I_parOut_0023 = 1;
    }
    I_parThis_0023 = I_parThis_0022;
    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // YMbCorteIHM
        I_parThis_0023 = 0;
    }

    if (I_parThis_0023) {  // $parThis_0023
        I_parOut_0023 = 1;
    }
    I_parThis_0023 = I_parThis_0022;
    if (!U_RMaqCortePerf) {  // RMaqCortePerf
        I_parThis_0023 = 0;
    }

    if (I_parThis_0023) {  // $parThis_0023
        I_parOut_0023 = 1;
    }
    I_parThis_0023 = I_parThis_0022;
    /* start series [ */
    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_0023 = 0;
    }

    if (!U_RRepeteSemPiloto) {  // RRepeteSemPiloto
        I_parThis_0023 = 0;
    }

    /* ] finish series */
    if (I_parThis_0023) {  // $parThis_0023
        I_parOut_0023 = 1;
    }
    I_parThis_0022 = I_parOut_0023;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YMbModoAutoAtivo
        I_parThis_0022 = 0;
    }

    if (!U_RMaqCortePerf) {  // RMaqCortePerf
        I_parThis_0022 = 0;
    }

    /* ] finish series */
    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_rung_top = I_parOut_0022;
    /* ] finish parallel */
    I_scratch = I_rung_top;
    if (I_oneShot_000f) {  // $oneShot_000f
        I_rung_top = 0;
    }
    I_oneShot_000f = I_scratch;

    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RCortarPerf = 1;
    }

    /* ] finish series */

    /* start rung 215 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0024 = 0;
    I_parThis_0024 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0025 = 0;
    I_parThis_0025 = I_parThis_0024;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YMbModoAutoAtivo
        I_parThis_0025 = 0;
    }

    if (!U_RCortePronto) {  // RCortePronto
        I_parThis_0025 = 0;
    }

    /* ] finish series */
    if (I_parThis_0025) {  // $parThis_0025
        I_parOut_0025 = 1;
    }
    I_parThis_0025 = I_parThis_0024;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0026 = 0;
    I_parThis_0026 = I_parThis_0025;
    if (!U_RCalcRelE1Avanca) {  // RCalcRelE1Avanca
        I_parThis_0026 = 0;
    }

    if (I_parThis_0026) {  // $parThis_0026
        I_parOut_0026 = 1;
    }
    I_parThis_0026 = I_parThis_0025;
    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_parThis_0026 = 0;
    }

    if (I_parThis_0026) {  // $parThis_0026
        I_parOut_0026 = 1;
    }
    I_parThis_0025 = I_parOut_0026;
    /* ] finish parallel */
    if (!U_RFerramCorteCima) {  // RFerramCorteCima
        I_parThis_0025 = 0;
    }

    /* ] finish series */
    if (I_parThis_0025) {  // $parThis_0025
        I_parOut_0025 = 1;
    }
    I_parThis_0024 = I_parOut_0025;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0027 = 0;
    I_parThis_0027 = I_parThis_0024;
    if (!U_RInvAtivar) {  // RInvAtivar
        I_parThis_0027 = 0;
    }

    if (I_parThis_0027) {  // $parThis_0027
        I_parOut_0027 = 1;
    }
    I_parThis_0027 = I_parThis_0024;
    /* start series [ */
    if (!GPIO_OUTPUT_PORT6) {  // YInvFreioMotor
        I_parThis_0027 = 0;
    }

    if (U_RMaqPerfilParado) {  // RMaqPerfilParado
        I_parThis_0027 = 0;
    }

    /* ] finish series */
    if (I_parThis_0027) {  // $parThis_0027
        I_parOut_0027 = 1;
    }
    I_parThis_0024 = I_parOut_0027;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0024) {  // $parThis_0024
        I_parOut_0024 = 1;
    }
    I_parThis_0024 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0028 = 0;
    I_parThis_0028 = I_parThis_0024;
    if (!U_RAvancarPerfMan) {  // RAvancarPerfMan
        I_parThis_0028 = 0;
    }

    if (I_parThis_0028) {  // $parThis_0028
        I_parOut_0028 = 1;
    }
    I_parThis_0028 = I_parThis_0024;
    if (!U_RRecuarPerfMan) {  // RRecuarPerfMan
        I_parThis_0028 = 0;
    }

    if (I_parThis_0028) {  // $parThis_0028
        I_parOut_0028 = 1;
    }
    I_parThis_0024 = I_parOut_0028;
    /* ] finish parallel */
    if (!I_TMantemFreioLib_antiglitch) {  // $TMantemFreioLib_antiglitch
        U_TMantemFreioLib = 499;
    }
    I_TMantemFreioLib_antiglitch = 1;
    if (!I_parThis_0024) {  // $parThis_0024
        if (U_TMantemFreioLib < 499) {
            U_TMantemFreioLib++;
            I_parThis_0024 = 1;
        }
    } else {
        U_TMantemFreioLib = 0;
    }

    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_0024 = 0;
    }

    /* ] finish series */
    if (I_parThis_0024) {  // $parThis_0024
        I_parOut_0024 = 1;
    }
    I_rung_top = I_parOut_0024;
    /* ] finish parallel */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT6 = I_rung_top;

    /* ] finish series */

    /* start rung 216 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT6) {  // YInvFreioMotor
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEsperaFreioLib < 49) {
            U_TEsperaFreioLib++;
            I_rung_top = 0;
        }
    } else {
        U_TEsperaFreioLib = 0;
    }

    U_RInvFreioLib = I_rung_top;

    /* ] finish series */

    /* start rung 217 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RInvFreioLib) {  // RInvFreioLib
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0029 = 0;
    I_parThis_0029 = I_rung_top;
    if (!U_RInvAtivar) {  // RInvAtivar
        I_parThis_0029 = 0;
    }

    if (I_parThis_0029) {  // $parThis_0029
        I_parOut_0029 = 1;
    }
    I_parThis_0029 = I_rung_top;
    if (!U_RAvancarPerfMan) {  // RAvancarPerfMan
        I_parThis_0029 = 0;
    }

    if (I_parThis_0029) {  // $parThis_0029
        I_parOut_0029 = 1;
    }
    I_rung_top = I_parOut_0029;
    /* ] finish parallel */
    GPIO_OUTPUT_PORT11 = I_rung_top;

    /* ] finish series */

    /* start rung 218 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RInvFreioLib) {  // RInvFreioLib
        I_rung_top = 0;
    }

    if (!U_RRecuarPerfMan) {  // RRecuarPerfMan
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT11) {  // YAvancarPerf
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT12 = I_rung_top;

    /* ] finish series */

    /* start rung 220 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_002a = 0;
    I_parThis_002a = I_rung_top;
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_parThis_002a = 0;
    }

    if (I_parThis_002a) {  // $parThis_002a
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    if (!U_RMaqModoCorrecao) {  // RMaqModoCorrecao
        I_parThis_002a = 0;
    }

    if (I_parThis_002a) {  // $parThis_002a
        I_parOut_002a = 1;
    }
    I_rung_top = I_parOut_002a;
    /* ] finish parallel */
    if (!U_RFerramCorteCima) {  // RFerramCorteCima
        I_rung_top = 0;
    }

    if (U_RMaqOperando) {  // RMaqOperando
        I_rung_top = 0;
    }

    if (!U_RMaqCfgInvOK) {  // RMaqCfgInvOK
        I_rung_top = 0;
    }

    U_RPerfManualOK = I_rung_top;

    /* ] finish series */

    /* start rung 221 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RPerfManualOK) {  // RPerfManualOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_002b = 0;
    I_parThis_002b = I_rung_top;
    if (!GPIO_INPUT_PORT5) {  // XPerfManAvanca
        I_parThis_002b = 0;
    }

    if (I_parThis_002b) {  // $parThis_002b
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (!(MODBUS_REGISTER[2] & (1 << 7))) {  // YPerfManAvcIHM
        I_parThis_002b = 0;
    }

    if (I_parThis_002b) {  // $parThis_002b
        I_parOut_002b = 1;
    }
    I_rung_top = I_parOut_002b;
    /* ] finish parallel */
    if (GPIO_INPUT_PORT6) {  // XPerfManRecua
        I_rung_top = 0;
    }

    if (MODBUS_REGISTER[2] & (1 << 8)) {  // YPerfManRecuaIHM
        I_rung_top = 0;
    }

    U_RAvancarPerfMan = I_rung_top;

    /* ] finish series */

    /* start rung 222 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RPerfManualOK) {  // RPerfManualOK
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT5) {  // XPerfManAvanca
        I_rung_top = 0;
    }

    if (MODBUS_REGISTER[2] & (1 << 7)) {  // YPerfManAvcIHM
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_002c = 0;
    I_parThis_002c = I_rung_top;
    if (!GPIO_INPUT_PORT6) {  // XPerfManRecua
        I_parThis_002c = 0;
    }

    if (I_parThis_002c) {  // $parThis_002c
        I_parOut_002c = 1;
    }
    I_parThis_002c = I_rung_top;
    if (!(MODBUS_REGISTER[2] & (1 << 8))) {  // YPerfManRecuaIHM
        I_parThis_002c = 0;
    }

    if (I_parThis_002c) {  // $parThis_002c
        I_parOut_002c = 1;
    }
    I_rung_top = I_parOut_002c;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_002d = 0;
    I_parThis_002d = I_rung_top;
    /* start series [ */
    if (U_RBloqueiaRecuo) {  // RBloqueiaRecuo
        I_parThis_002d = 0;
    }

    U_RRecuarPerfMan = I_parThis_002d;

    /* ] finish series */
    if (I_parThis_002d) {  // $parThis_002d
        I_parOut_002d = 1;
    }
    I_parThis_002d = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_002e = 0;
    I_parThis_002e = I_parThis_002d;
    if (!U_RInvFreioLib) {  // RInvFreioLib
        I_parThis_002e = 0;
    }

    if (I_parThis_002e) {  // $parThis_002e
        I_parOut_002e = 1;
    }
    I_parThis_002e = I_parThis_002d;
    if (!U_RBloqueiaRecuo) {  // RBloqueiaRecuo
        I_parThis_002e = 0;
    }

    if (I_parThis_002e) {  // $parThis_002e
        I_parOut_002e = 1;
    }
    I_parThis_002d = I_parOut_002e;
    /* ] finish parallel */
    if (I_parThis_002d) {  // $parThis_002d
        if (U_TMaxPerfRecua < 299) {
            U_TMaxPerfRecua++;
            I_parThis_002d = 0;
        }
    } else {
        U_TMaxPerfRecua = 0;
    }

    U_RBloqueiaRecuo = I_parThis_002d;

    /* ] finish series */
    if (I_parThis_002d) {  // $parThis_002d
        I_parOut_002d = 1;
    }
    I_rung_top = I_parOut_002d;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 224 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_002f = 0;
    I_parThis_002f = I_rung_top;
    if (!U_RMaqPronta) {  // RMaqPronta
        I_parThis_002f = 0;
    }

    if (I_parThis_002f) {  // $parThis_002f
        I_parOut_002f = 1;
    }
    I_parThis_002f = I_rung_top;
    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_parThis_002f = 0;
    }

    if (!I_TEspFimCfgJog_antiglitch) {  // $TEspFimCfgJog_antiglitch
        U_TEspFimCfgJog = 4;
    }
    I_TEspFimCfgJog_antiglitch = 1;
    if (!I_parThis_002f) {  // $parThis_002f
        if (U_TEspFimCfgJog < 4) {
            U_TEspFimCfgJog++;
            I_parThis_002f = 1;
        }
    } else {
        U_TEspFimCfgJog = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_parThis_002f = 0;
    }

    /* ] finish series */
    if (I_parThis_002f) {  // $parThis_002f
        I_parOut_002f = 1;
    }
    I_rung_top = I_parOut_002f;
    /* ] finish parallel */
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_rung_top = 0;
    }

    if (!U_RFerramCorteCima) {  // RFerramCorteCima
        I_rung_top = 0;
    }

    if (U_RMaqOperando) {  // RMaqOperando
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    U_RSrvManualOK = I_rung_top;

    /* ] finish series */

    /* start rung 225 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvManualOK) {  // RSrvManualOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0030 = 0;
    I_parThis_0030 = I_rung_top;
    if (!GPIO_INPUT_PORT8) {  // XMesaManAvanca
        I_parThis_0030 = 0;
    }

    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    if (!(MODBUS_REGISTER[2] & (1 << 9))) {  // YMesaManAvcIHM
        I_parThis_0030 = 0;
    }

    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_rung_top = I_parOut_0030;
    /* ] finish parallel */
    if (GPIO_INPUT_PORT9) {  // XMesaManRecua
        I_rung_top = 0;
    }

    if (MODBUS_REGISTER[2] & (1 << 10)) {  // YMesaManRecuaIHM
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0031 = 0;
    I_parThis_0031 = I_rung_top;
    U_RAvancarMesaMan = I_parThis_0031;

    if (I_parThis_0031) {  // $parThis_0031
        I_parOut_0031 = 1;
    }
    I_parThis_0031 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0031;
    if (I_oneShot_0010) {  // $oneShot_0010
        I_parThis_0031 = 0;
    }
    I_oneShot_0010 = I_scratch;

    if (I_parThis_0031) {  // $parThis_0031
        U_RStartCfgSrvJog = 1;
    }

    /* ] finish series */
    if (I_parThis_0031) {  // $parThis_0031
        I_parOut_0031 = 1;
    }
    I_rung_top = I_parOut_0031;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 226 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvManualOK) {  // RSrvManualOK
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT8) {  // XMesaManAvanca
        I_rung_top = 0;
    }

    if (MODBUS_REGISTER[2] & (1 << 9)) {  // YMesaManAvcIHM
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0032 = 0;
    I_parThis_0032 = I_rung_top;
    if (!GPIO_INPUT_PORT9) {  // XMesaManRecua
        I_parThis_0032 = 0;
    }

    if (I_parThis_0032) {  // $parThis_0032
        I_parOut_0032 = 1;
    }
    I_parThis_0032 = I_rung_top;
    if (!(MODBUS_REGISTER[2] & (1 << 10))) {  // YMesaManRecuaIHM
        I_parThis_0032 = 0;
    }

    if (I_parThis_0032) {  // $parThis_0032
        I_parOut_0032 = 1;
    }
    I_rung_top = I_parOut_0032;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0033 = 0;
    I_parThis_0033 = I_rung_top;
    U_RRecuarMesaMan = I_parThis_0033;

    if (I_parThis_0033) {  // $parThis_0033
        I_parOut_0033 = 1;
    }
    I_parThis_0033 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0033;
    if (I_oneShot_0011) {  // $oneShot_0011
        I_parThis_0033 = 0;
    }
    I_oneShot_0011 = I_scratch;

    if (I_parThis_0033) {  // $parThis_0033
        U_RStartCfgSrvJog = 1;
    }

    /* ] finish series */
    if (I_parThis_0033) {  // $parThis_0033
        I_parOut_0033 = 1;
    }
    I_rung_top = I_parOut_0033;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 227 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0034 = 0;
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!U_RAvancarMesaMan) {  // RAvancarMesaMan
        I_parThis_0034 = 0;
    }

    I_scratch = I_parThis_0034;
    if (!I_parThis_0034) {  // $parThis_0034
        if (I_oneShot_0012) {  // $oneShot_0012
            I_parThis_0034 = 1;
        }
    } else {
        I_parThis_0034 = 0;
    }
    I_oneShot_0012 = I_scratch;

    if (U_RRecuarMesaMan) {  // RRecuarMesaMan
        I_parThis_0034 = 0;
    }

    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!U_RRecuarMesaMan) {  // RRecuarMesaMan
        I_parThis_0034 = 0;
    }

    I_scratch = I_parThis_0034;
    if (!I_parThis_0034) {  // $parThis_0034
        if (I_oneShot_0013) {  // $oneShot_0013
            I_parThis_0034 = 1;
        }
    } else {
        I_parThis_0034 = 0;
    }
    I_oneShot_0013 = I_scratch;

    if (U_RAvancarMesaMan) {  // RAvancarMesaMan
        I_parThis_0034 = 0;
    }

    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_rung_top = I_parOut_0034;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_RStartCfgSrvJog = 1;
    }

    /* ] finish series */

    /* start rung 228 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    if (!U_RAvancarMesaMan) {  // RAvancarMesaMan
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0035 = 0;
    I_parThis_0035 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0035;
    if (I_oneShot_0014) {  // $oneShot_0014
        I_parThis_0035 = 0;
    }
    I_oneShot_0014 = I_scratch;

    if (I_parThis_0035) {  // $parThis_0035
        U_RMaqCfgSrvOK = 0;
    }

    /* ] finish series */
    if (I_parThis_0035) {  // $parThis_0035
        I_parOut_0035 = 1;
    }
    I_parThis_0035 = I_rung_top;
    /* start series [ */
    if (I_parThis_0035) {  // $parThis_0035
        if (U_TCfgSrvJogAvcTO < 499) {
            U_TCfgSrvJogAvcTO++;
            I_parThis_0035 = 0;
        }
    } else {
        U_TCfgSrvJogAvcTO = 0;
    }

    if (I_parThis_0035) {  // $parThis_0035
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0035) {  // $parThis_0035
        I_parOut_0035 = 1;
    }
    I_parThis_0035 = I_rung_top;
    /* start series [ */
    if (I_parThis_0035) {  // $parThis_0035
        if (!I_oneShot_0015) {  // $oneShot_0015
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0015 = I_parThis_0035;
            }
            I_parThis_0035 = 0;
            I_oneShot_0016 = I_parThis_0035;
        }
        if (!I_oneShot_0016) {  // $oneShot_0016
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0016 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0035 = 0;
            }
        }
    } else {
        if (I_oneShot_0015) {  // $oneShot_0015
            I_SerialReady = 1;
        }
        I_oneShot_0015 = I_parThis_0035;
    }

    if (I_parThis_0035) {  // $parThis_0035
        U_SrvRegConfig |= 1 << 5;
    }

    if (I_parThis_0035) {  // $parThis_0035
        U_SrvRegConfig &= ~(1 << 6);
    }

    if (I_parThis_0035) {  // $parThis_0035
        if (!I_oneShot_0017) {  // $oneShot_0017
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0017 = I_parThis_0035;
            }
            I_parThis_0035 = 0;
            I_oneShot_0018 = I_parThis_0035;
        }
        if (!I_oneShot_0018) {  // $oneShot_0018
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0018 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0035 = 0;
            }
        }
    } else {
        if (I_oneShot_0017) {  // $oneShot_0017
            I_SerialReady = 1;
        }
        I_oneShot_0017 = I_parThis_0035;
    }

    /* start parallel [ */
    I_parOut_0036 = 0;
    I_parThis_0036 = I_parThis_0035;
    if (I_parThis_0036) {  // $parThis_0036
        U_RStartCfgSrvJog = 0;
    }

    if (I_parThis_0036) {  // $parThis_0036
        I_parOut_0036 = 1;
    }
    I_parThis_0036 = I_parThis_0035;
    if (I_parThis_0036) {  // $parThis_0036
        U_RMaqCfgSrvOK = 1;
    }

    if (I_parThis_0036) {  // $parThis_0036
        I_parOut_0036 = 1;
    }
    I_parThis_0035 = I_parOut_0036;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0035) {  // $parThis_0035
        I_parOut_0035 = 1;
    }
    I_rung_top = I_parOut_0035;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 229 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    if (!U_RRecuarMesaMan) {  // RRecuarMesaMan
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0037 = 0;
    I_parThis_0037 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0037;
    if (I_oneShot_0019) {  // $oneShot_0019
        I_parThis_0037 = 0;
    }
    I_oneShot_0019 = I_scratch;

    if (I_parThis_0037) {  // $parThis_0037
        U_RMaqCfgSrvOK = 0;
    }

    /* ] finish series */
    if (I_parThis_0037) {  // $parThis_0037
        I_parOut_0037 = 1;
    }
    I_parThis_0037 = I_rung_top;
    /* start series [ */
    if (I_parThis_0037) {  // $parThis_0037
        if (U_TCfgSrvJogRecTO < 499) {
            U_TCfgSrvJogRecTO++;
            I_parThis_0037 = 0;
        }
    } else {
        U_TCfgSrvJogRecTO = 0;
    }

    if (I_parThis_0037) {  // $parThis_0037
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0037) {  // $parThis_0037
        I_parOut_0037 = 1;
    }
    I_parThis_0037 = I_rung_top;
    /* start series [ */
    if (I_parThis_0037) {  // $parThis_0037
        if (!I_oneShot_001a) {  // $oneShot_001a
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_001a = I_parThis_0037;
            }
            I_parThis_0037 = 0;
            I_oneShot_001b = I_parThis_0037;
        }
        if (!I_oneShot_001b) {  // $oneShot_001b
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_001b = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0037 = 0;
            }
        }
    } else {
        if (I_oneShot_001a) {  // $oneShot_001a
            I_SerialReady = 1;
        }
        I_oneShot_001a = I_parThis_0037;
    }

    if (I_parThis_0037) {  // $parThis_0037
        U_SrvRegConfig &= ~(1 << 5);
    }

    if (I_parThis_0037) {  // $parThis_0037
        U_SrvRegConfig |= 1 << 6;
    }

    if (I_parThis_0037) {  // $parThis_0037
        if (!I_oneShot_001c) {  // $oneShot_001c
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_001c = I_parThis_0037;
            }
            I_parThis_0037 = 0;
            I_oneShot_001d = I_parThis_0037;
        }
        if (!I_oneShot_001d) {  // $oneShot_001d
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_001d = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0037 = 0;
            }
        }
    } else {
        if (I_oneShot_001c) {  // $oneShot_001c
            I_SerialReady = 1;
        }
        I_oneShot_001c = I_parThis_0037;
    }

    /* start parallel [ */
    I_parOut_0038 = 0;
    I_parThis_0038 = I_parThis_0037;
    if (I_parThis_0038) {  // $parThis_0038
        U_RStartCfgSrvJog = 0;
    }

    if (I_parThis_0038) {  // $parThis_0038
        I_parOut_0038 = 1;
    }
    I_parThis_0038 = I_parThis_0037;
    if (I_parThis_0038) {  // $parThis_0038
        U_RMaqCfgSrvOK = 1;
    }

    if (I_parThis_0038) {  // $parThis_0038
        I_parOut_0038 = 1;
    }
    I_parThis_0037 = I_parOut_0038;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0037) {  // $parThis_0037
        I_parOut_0037 = 1;
    }
    I_rung_top = I_parOut_0037;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 230 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    if (U_RAvancarMesaMan) {  // RAvancarMesaMan
        I_rung_top = 0;
    }

    if (U_RRecuarMesaMan) {  // RRecuarMesaMan
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0039 = 0;
    I_parThis_0039 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0039;
    if (I_oneShot_001e) {  // $oneShot_001e
        I_parThis_0039 = 0;
    }
    I_oneShot_001e = I_scratch;

    if (I_parThis_0039) {  // $parThis_0039
        U_RMaqCfgSrvOK = 0;
    }

    /* ] finish series */
    if (I_parThis_0039) {  // $parThis_0039
        I_parOut_0039 = 1;
    }
    I_parThis_0039 = I_rung_top;
    /* start series [ */
    if (I_parThis_0039) {  // $parThis_0039
        if (U_TCfgSrvJogDesTO < 499) {
            U_TCfgSrvJogDesTO++;
            I_parThis_0039 = 0;
        }
    } else {
        U_TCfgSrvJogDesTO = 0;
    }

    if (I_parThis_0039) {  // $parThis_0039
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0039) {  // $parThis_0039
        I_parOut_0039 = 1;
    }
    I_parThis_0039 = I_rung_top;
    /* start series [ */
    if (I_parThis_0039) {  // $parThis_0039
        if (!I_oneShot_001f) {  // $oneShot_001f
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_001f = I_parThis_0039;
            }
            I_parThis_0039 = 0;
            I_oneShot_0020 = I_parThis_0039;
        }
        if (!I_oneShot_0020) {  // $oneShot_0020
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0020 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0039 = 0;
            }
        }
    } else {
        if (I_oneShot_001f) {  // $oneShot_001f
            I_SerialReady = 1;
        }
        I_oneShot_001f = I_parThis_0039;
    }

    if (I_parThis_0039) {  // $parThis_0039
        U_SrvRegConfig &= ~(1 << 5);
    }

    if (I_parThis_0039) {  // $parThis_0039
        U_SrvRegConfig &= ~(1 << 6);
    }

    if (I_parThis_0039) {  // $parThis_0039
        if (!I_oneShot_0021) {  // $oneShot_0021
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0021 = I_parThis_0039;
            }
            I_parThis_0039 = 0;
            I_oneShot_0022 = I_parThis_0039;
        }
        if (!I_oneShot_0022) {  // $oneShot_0022
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0022 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0039 = 0;
            }
        }
    } else {
        if (I_oneShot_0021) {  // $oneShot_0021
            I_SerialReady = 1;
        }
        I_oneShot_0021 = I_parThis_0039;
    }

    /* start parallel [ */
    I_parOut_003a = 0;
    I_parThis_003a = I_parThis_0039;
    if (I_parThis_003a) {  // $parThis_003a
        U_RStartCfgSrvJog = 0;
    }

    if (I_parThis_003a) {  // $parThis_003a
        I_parOut_003a = 1;
    }
    I_parThis_003a = I_parThis_0039;
    if (I_parThis_003a) {  // $parThis_003a
        U_RMaqCfgSrvOK = 1;
    }

    if (I_parThis_003a) {  // $parThis_003a
        I_parOut_003a = 1;
    }
    I_parThis_0039 = I_parOut_003a;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0039) {  // $parThis_0039
        I_parOut_0039 = 1;
    }
    I_rung_top = I_parOut_0039;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 232 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 4))) {  // XDbgSrvPosMan
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 6);  // YMbSrvPosMesaMan
    }

    /* ] finish series */

    /* start rung 233 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    if (!U_RSrvManualOK) {  // RSrvManualOK
        I_rung_top = 0;
    }

    if (U_RAvancarMesaMan) {  // RAvancarMesaMan
        I_rung_top = 0;
    }

    if (U_RRecuarMesaMan) {  // RRecuarMesaMan
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[2] & (1 << 6))) {  // YMbSrvPosMesaMan
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_003b = 0;
    I_parThis_003b = I_rung_top;
    /* start series [ */
    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_parThis_003b = 0;
    }

    /* start parallel [ */
    I_parOut_003c = 0;
    I_parThis_003c = I_parThis_003b;
    if (I_parThis_003c) {  // $parThis_003c
        U_RMaqStartPosic = 1;
    }

    if (I_parThis_003c) {  // $parThis_003c
        I_parOut_003c = 1;
    }
    I_parThis_003c = I_parThis_003b;
    if (I_parThis_003c) {  // $parThis_003c
        I_scratch2 = 10;
        U_VSrvPosMesa = MODBUS_REGISTER[19] * I_scratch2;
    }

    if (I_parThis_003c) {  // $parThis_003c
        I_parOut_003c = 1;
    }
    I_parThis_003c = I_parThis_003b;
    if (I_parThis_003c) {  // $parThis_003c
        I_scratch2 = 1004;
        U_VSrvPosMesa = U_VSrvPosMesa * I_scratch2;
    }

    if (I_parThis_003c) {  // $parThis_003c
        I_parOut_003c = 1;
    }
    I_parThis_003c = I_parThis_003b;
    if (I_parThis_003c) {  // $parThis_003c
        I_scratch2 = 1000;
        if(I_scratch2) U_VSrvPosMesa = U_VSrvPosMesa / I_scratch2;
    }

    if (I_parThis_003c) {  // $parThis_003c
        I_parOut_003c = 1;
    }
    I_parThis_003b = I_parOut_003c;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_003b) {  // $parThis_003b
        I_parOut_003b = 1;
    }
    I_parThis_003b = I_rung_top;
    if (I_parThis_003b) {  // $parThis_003b
        MODBUS_REGISTER[2] &= ~(1 << 6);  // YMbSrvPosMesaMan
    }

    if (I_parThis_003b) {  // $parThis_003b
        I_parOut_003b = 1;
    }
    I_rung_top = I_parOut_003b;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 234 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_003d = 0;
    I_parThis_003d = I_rung_top;
    if (!(MODBUS_REGISTER[3] & (1 << 3))) {  // XDbgStartPosic
        I_parThis_003d = 0;
    }

    if (I_parThis_003d) {  // $parThis_003d
        I_parOut_003d = 1;
    }
    I_parThis_003d = I_rung_top;
    if (!U_RMaqStartPosic) {  // RMaqStartPosic
        I_parThis_003d = 0;
    }

    if (I_parThis_003d) {  // $parThis_003d
        I_parOut_003d = 1;
    }
    I_rung_top = I_parOut_003d;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_003e = 0;
    I_parThis_003e = I_rung_top;
    if (U_RMaqOperando) {  // RMaqOperando
        I_parThis_003e = 0;
    }

    if (I_parThis_003e) {  // $parThis_003e
        I_parOut_003e = 1;
    }
    I_parThis_003e = I_rung_top;
    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_parThis_003e = 0;
    }

    if (I_parThis_003e) {  // $parThis_003e
        I_parOut_003e = 1;
    }
    I_parThis_003e = I_rung_top;
    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_parThis_003e = 0;
    }

    if (I_parThis_003e) {  // $parThis_003e
        I_parOut_003e = 1;
    }
    I_parThis_003e = I_rung_top;
    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_parThis_003e = 0;
    }

    if (I_parThis_003e) {  // $parThis_003e
        I_parOut_003e = 1;
    }
    I_parThis_003e = I_rung_top;
    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_parThis_003e = 0;
    }

    if (I_parThis_003e) {  // $parThis_003e
        I_parOut_003e = 1;
    }
    I_rung_top = I_parOut_003e;
    /* ] finish parallel */
    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_003f = 0;
    I_parThis_003f = I_rung_top;
    if (I_parThis_003f) {  // $parThis_003f
        U_RSrvStartPosic = 1;
    }

    if (I_parThis_003f) {  // $parThis_003f
        I_parOut_003f = 1;
    }
    I_parThis_003f = I_rung_top;
    if (I_parThis_003f) {  // $parThis_003f
        U_RSrvPosicAtivo = 1;
    }

    if (I_parThis_003f) {  // $parThis_003f
        I_parOut_003f = 1;
    }
    I_parThis_003f = I_rung_top;
    if (I_parThis_003f) {  // $parThis_003f
        U_RMaqStartPosic = 0;
    }

    if (I_parThis_003f) {  // $parThis_003f
        I_parOut_003f = 1;
    }
    I_parThis_003f = I_rung_top;
    if (I_parThis_003f) {  // $parThis_003f
        I_scratch2 = 100;
        U_SrvPosMesa = U_VSrvPosMesa * I_scratch2;
    }

    if (I_parThis_003f) {  // $parThis_003f
        I_parOut_003f = 1;
    }
    I_rung_top = I_parOut_003f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 235 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvStartPosic) {  // RSrvStartPosic
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0040 = 0;
    I_parThis_0040 = I_rung_top;
    /* start series [ */
    if (I_parThis_0040) {  // $parThis_0040
        if (U_TCfgSrvPosicTO < 499) {
            U_TCfgSrvPosicTO++;
            I_parThis_0040 = 0;
        }
    } else {
        U_TCfgSrvPosicTO = 0;
    }

    /* start parallel [ */
    I_parOut_0041 = 0;
    I_parThis_0041 = I_parThis_0040;
    if (I_parThis_0041) {  // $parThis_0041
        U_RMaqCfgSrvErro = 1;
    }

    if (I_parThis_0041) {  // $parThis_0041
        I_parOut_0041 = 1;
    }
    I_parThis_0041 = I_parThis_0040;
    if (I_parThis_0041) {  // $parThis_0041
        U_RSrvStartPosic = 0;
    }

    if (I_parThis_0041) {  // $parThis_0041
        I_parOut_0041 = 1;
    }
    I_parThis_0041 = I_parThis_0040;
    if (I_parThis_0041) {  // $parThis_0041
        U_RSrvPosicAtivo = 0;
    }

    if (I_parThis_0041) {  // $parThis_0041
        I_parOut_0041 = 1;
    }
    I_parThis_0040 = I_parOut_0041;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0040) {  // $parThis_0040
        I_parOut_0040 = 1;
    }
    I_parThis_0040 = I_rung_top;
    /* start series [ */
    if (I_parThis_0040) {  // $parThis_0040
        if (!I_oneShot_0023) {  // $oneShot_0023
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15036, 2, &U_SrvPosMesa);
                I_oneShot_0023 = I_parThis_0040;
            }
            I_parThis_0040 = 0;
            I_oneShot_0024 = I_parThis_0040;
        }
        if (!I_oneShot_0024) {  // $oneShot_0024
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0024 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15036, 2, &U_SrvPosMesa);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0040 = 0;
            }
        }
    } else {
        if (I_oneShot_0023) {  // $oneShot_0023
            I_SerialReady = 1;
        }
        I_oneShot_0023 = I_parThis_0040;
    }

    if (I_parThis_0040) {  // $parThis_0040
        if (!I_oneShot_0025) {  // $oneShot_0025
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0025 = I_parThis_0040;
            }
            I_parThis_0040 = 0;
            I_oneShot_0026 = I_parThis_0040;
        }
        if (!I_oneShot_0026) {  // $oneShot_0026
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0026 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0040 = 0;
            }
        }
    } else {
        if (I_oneShot_0025) {  // $oneShot_0025
            I_SerialReady = 1;
        }
        I_oneShot_0025 = I_parThis_0040;
    }

    if (I_parThis_0040) {  // $parThis_0040
        U_SrvRegConfig &= ~(1 << 2);
    }

    if (I_parThis_0040) {  // $parThis_0040
        U_SrvRegConfig |= 1 << 10;
    }

    if (I_parThis_0040) {  // $parThis_0040
        if (!I_oneShot_0027) {  // $oneShot_0027
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0027 = I_parThis_0040;
            }
            I_parThis_0040 = 0;
            I_oneShot_0028 = I_parThis_0040;
        }
        if (!I_oneShot_0028) {  // $oneShot_0028
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0028 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0040 = 0;
            }
        }
    } else {
        if (I_oneShot_0027) {  // $oneShot_0027
            I_SerialReady = 1;
        }
        I_oneShot_0027 = I_parThis_0040;
    }

    if (I_parThis_0040) {  // $parThis_0040
        if (U_TSrvEsperaPosic < 4) {
            U_TSrvEsperaPosic++;
            I_parThis_0040 = 0;
        }
    } else {
        U_TSrvEsperaPosic = 0;
    }

    if (I_parThis_0040) {  // $parThis_0040
        U_RSrvStartPosic = 0;
    }

    /* ] finish series */
    if (I_parThis_0040) {  // $parThis_0040
        I_parOut_0040 = 1;
    }
    I_rung_top = I_parOut_0040;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 236 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 5))) {  // XDbgSrvPosicOK
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_SrvRegConfig = 0;
    }

    /* ] finish series */

    /* start rung 237 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (U_RSrvStartPosic) {  // RSrvStartPosic
        I_rung_top = 0;
    }

    if (U_RSrvReadPosicOK) {  // RSrvReadPosicOK
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0029) {  // $oneShot_0029
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0029 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_002a = I_rung_top;
        }
        if (!I_oneShot_002a) {  // $oneShot_002a
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_002a = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        if (I_oneShot_0029) {  // $oneShot_0029
            I_SerialReady = 1;
        }
        I_oneShot_0029 = I_rung_top;
    }

    /* start parallel [ */
    I_parOut_0042 = 0;
    I_parThis_0042 = I_rung_top;
    /* start series [ */
    if (I_parThis_0042) {  // $parThis_0042
        if (U_TEspReadPosicOK < 49) {
            U_TEspReadPosicOK++;
            I_parThis_0042 = 0;
        }
    } else {
        U_TEspReadPosicOK = 0;
    }

    U_RSrvReadPosicOK = I_parThis_0042;

    /* ] finish series */
    if (I_parThis_0042) {  // $parThis_0042
        I_parOut_0042 = 1;
    }
    I_parThis_0042 = I_rung_top;
    /* start series [ */
    if (I_parThis_0042) {  // $parThis_0042
        if (U_SrvRegConfig & (1 << 10)) {  // SrvRegConfig
            I_parThis_0042 = 0;
        }
    }

    if (I_parThis_0042) {  // $parThis_0042
        U_RSrvPosicAtivo = 0;
    }

    /* ] finish series */
    if (I_parThis_0042) {  // $parThis_0042
        I_parOut_0042 = 1;
    }
    I_rung_top = I_parOut_0042;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 238 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_002b) {  // $oneShot_002b
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_002b = I_scratch;

    if (I_rung_top) {  // $rung_top
        U_RSrvStartPosic = 0;
    }

    /* ] finish series */

    /* start rung 240 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0043 = 0;
    I_parThis_0043 = I_rung_top;
    if (!U_RMaqCalcCorrecao) {  // RMaqCalcCorrecao
        I_parThis_0043 = 0;
    }

    if (I_parThis_0043) {  // $parThis_0043
        I_parOut_0043 = 1;
    }
    I_parThis_0043 = I_rung_top;
    if (!(MODBUS_REGISTER[3] & (1 << 9))) {  // XDbgCalcCorrecao
        I_parThis_0043 = 0;
    }

    if (I_parThis_0043) {  // $parThis_0043
        I_parOut_0043 = 1;
    }
    I_rung_top = I_parOut_0043;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0044 = 0;
    I_parThis_0044 = I_rung_top;
    /* start series [ */
    if (U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_parThis_0044 = 0;
    }

    if (I_parThis_0044) {  // $parThis_0044
        U_RCalcCorrAtivo = 1;
    }

    /* ] finish series */
    if (I_parThis_0044) {  // $parThis_0044
        I_parOut_0044 = 1;
    }
    I_parThis_0044 = I_rung_top;
    if (I_parThis_0044) {  // $parThis_0044
        U_RMaqCalcCorrecao = 0;
    }

    if (I_parThis_0044) {  // $parThis_0044
        I_parOut_0044 = 1;
    }
    I_rung_top = I_parOut_0044;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 241 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    if (U_RPosCorrConfig) {  // RPosCorrConfig
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0045 = 0;
    I_parThis_0045 = I_rung_top;
    /* start series [ */
    if (U_RCalcCorrZero) {  // RCalcCorrZero
        I_parThis_0045 = 0;
    }

    if (U_RCalcCorrPosic) {  // RCalcCorrPosic
        I_parThis_0045 = 0;
    }

    if (I_parThis_0045) {  // $parThis_0045
        U_VSrvPosMesa = 0;
    }

    /* ] finish series */
    if (I_parThis_0045) {  // $parThis_0045
        I_parOut_0045 = 1;
    }
    I_parThis_0045 = I_rung_top;
    /* start series [ */
    if (!U_RCalcCorrZero) {  // RCalcCorrZero
        I_parThis_0045 = 0;
    }

    if (U_RCalcCorrPosic) {  // RCalcCorrPosic
        I_parThis_0045 = 0;
    }

    if (I_parThis_0045) {  // $parThis_0045
        U_VSrvPosMesa = U_SrvMesaCurso;
    }

    /* ] finish series */
    if (I_parThis_0045) {  // $parThis_0045
        I_parOut_0045 = 1;
    }
    I_parThis_0045 = I_rung_top;
    /* start series [ */
    if (!U_RCalcCorrZero) {  // RCalcCorrZero
        I_parThis_0045 = 0;
    }

    if (!U_RCalcCorrPosic) {  // RCalcCorrPosic
        I_parThis_0045 = 0;
    }

    if (I_parThis_0045) {  // $parThis_0045
        U_VSrvPosMesa = 0;
    }

    /* ] finish series */
    if (I_parThis_0045) {  // $parThis_0045
        I_parOut_0045 = 1;
    }
    I_parThis_0045 = I_rung_top;
    if (I_parThis_0045) {  // $parThis_0045
        U_RPosCorrConfig = 1;
    }

    if (I_parThis_0045) {  // $parThis_0045
        I_parOut_0045 = 1;
    }
    I_rung_top = I_parOut_0045;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 242 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_002c) {  // $oneShot_002c
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_002c = I_scratch;

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RPosCorrConfig = 0;
    }

    /* ] finish series */

    /* start rung 243 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_002d) {  // $oneShot_002d
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_002d = I_scratch;

    if (!U_RCalcCorrPosic) {  // RCalcCorrPosic
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcCorrZero) {  // RCalcCorrZero
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RCalcCorrAtivo = 0;
    }

    /* ] finish series */

    /* start rung 244 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_002e) {  // $oneShot_002e
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_002e = I_scratch;

    if (U_RCalcCorrPosic) {  // RCalcCorrPosic
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcCorrZero) {  // RCalcCorrZero
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0046 = 0;
    I_parThis_0046 = I_rung_top;
    if (I_parThis_0046) {  // $parThis_0046
        U_RCalcCorrPosic = 1;
    }

    if (I_parThis_0046) {  // $parThis_0046
        I_parOut_0046 = 1;
    }
    I_parThis_0046 = I_rung_top;
    if (I_parThis_0046) {  // $parThis_0046
        U_CalcCorrPosDif = U_CalcCorrPosIni - U_MaqPosAtual;
    }

    if (I_parThis_0046) {  // $parThis_0046
        I_parOut_0046 = 1;
    }
    I_parThis_0046 = I_rung_top;
    if (I_parThis_0046) {  // $parThis_0046
        I_scratch2 = 1000;
        MODBUS_REGISTER[27] = U_SrvMesaCurso * I_scratch2;
    }

    if (I_parThis_0046) {  // $parThis_0046
        I_parOut_0046 = 1;
    }
    I_parThis_0046 = I_rung_top;
    /* start series [ */
    I_scratch2 = 0;
    if (U_CalcCorrPosDif == I_scratch2) {
        I_parThis_0046 = 0;
    }

    if (I_parThis_0046) {  // $parThis_0046
        if(U_CalcCorrPosDif) MODBUS_REGISTER[27] = MODBUS_REGISTER[27] / U_CalcCorrPosDif;
    }

    /* ] finish series */
    if (I_parThis_0046) {  // $parThis_0046
        I_parOut_0046 = 1;
    }
    I_rung_top = I_parOut_0046;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 245 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_002f) {  // $oneShot_002f
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_002f = I_scratch;

    if (U_RCalcCorrZero) {  // RCalcCorrZero
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0047 = 0;
    I_parThis_0047 = I_rung_top;
    if (I_parThis_0047) {  // $parThis_0047
        U_RCalcCorrZero = 1;
    }

    if (I_parThis_0047) {  // $parThis_0047
        I_parOut_0047 = 1;
    }
    I_parThis_0047 = I_rung_top;
    if (I_parThis_0047) {  // $parThis_0047
        U_CalcCorrPosIni = U_MaqPosAtual;
    }

    if (I_parThis_0047) {  // $parThis_0047
        I_parOut_0047 = 1;
    }
    I_rung_top = I_parOut_0047;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 246 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    if (U_RCalcCorrZero) {  // RCalcCorrZero
        I_rung_top = 0;
    }

    if (!U_RPosCorrConfig) {  // RPosCorrConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 247 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    if (U_RCalcCorrPosic) {  // RCalcCorrPosic
        I_rung_top = 0;
    }

    if (!U_RPosCorrConfig) {  // RPosCorrConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcCorrZero) {  // RCalcCorrZero
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 248 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcCorrPosic) {  // RCalcCorrPosic
        I_rung_top = 0;
    }

    if (!U_RPosCorrConfig) {  // RPosCorrConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 249 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcCorrAtivo) {  // RCalcCorrAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0030) {  // $oneShot_0030
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0030 = I_scratch;

    /* start parallel [ */
    I_parOut_0048 = 0;
    I_parThis_0048 = I_rung_top;
    if (I_parThis_0048) {  // $parThis_0048
        U_RCalcCorrAtivo = 0;
    }

    if (I_parThis_0048) {  // $parThis_0048
        I_parOut_0048 = 1;
    }
    I_parThis_0048 = I_rung_top;
    if (I_parThis_0048) {  // $parThis_0048
        U_RCalcCorrPosic = 0;
    }

    if (I_parThis_0048) {  // $parThis_0048
        I_parOut_0048 = 1;
    }
    I_parThis_0048 = I_rung_top;
    if (I_parThis_0048) {  // $parThis_0048
        U_RCalcCorrZero = 0;
    }

    if (I_parThis_0048) {  // $parThis_0048
        I_parOut_0048 = 1;
    }
    I_rung_top = I_parOut_0048;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 251 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0049 = 0;
    I_parThis_0049 = I_rung_top;
    if (!U_RMaqCalcRelacao) {  // RMaqCalcRelacao
        I_parThis_0049 = 0;
    }

    if (I_parThis_0049) {  // $parThis_0049
        I_parOut_0049 = 1;
    }
    I_parThis_0049 = I_rung_top;
    if (!(MODBUS_REGISTER[3] & (1 << 10))) {  // XDbgCalcRelacao
        I_parThis_0049 = 0;
    }

    if (I_parThis_0049) {  // $parThis_0049
        I_parOut_0049 = 1;
    }
    I_rung_top = I_parOut_0049;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_004a = 0;
    I_parThis_004a = I_rung_top;
    /* start series [ */
    if (U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_parThis_004a = 0;
    }

    if (I_parThis_004a) {  // $parThis_004a
        U_RCalcRelAtivo = 1;
    }

    /* ] finish series */
    if (I_parThis_004a) {  // $parThis_004a
        I_parOut_004a = 1;
    }
    I_parThis_004a = I_rung_top;
    if (I_parThis_004a) {  // $parThis_004a
        U_RMaqCalcRelacao = 0;
    }

    if (I_parThis_004a) {  // $parThis_004a
        I_parOut_004a = 1;
    }
    I_rung_top = I_parOut_004a;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 252 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (U_RPosRelConfig) {  // RPosRelConfig
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_004b = 0;
    I_parThis_004b = I_rung_top;
    /* start series [ */
    if (U_RCalcRelZero) {  // RCalcRelZero
        I_parThis_004b = 0;
    }

    if (I_parThis_004b) {  // $parThis_004b
        U_VSrvPosMesa = 0;
    }

    /* ] finish series */
    if (I_parThis_004b) {  // $parThis_004b
        I_parOut_004b = 1;
    }
    I_parThis_004b = I_rung_top;
    /* start series [ */
    if (!U_RCalcRelZero) {  // RCalcRelZero
        I_parThis_004b = 0;
    }

    if (U_RCalcRelE2Fim) {  // RCalcRelE2Fim
        I_parThis_004b = 0;
    }

    if (I_parThis_004b) {  // $parThis_004b
        U_VSrvPosMesa = U_SrvMesaCurso;
    }

    /* ] finish series */
    if (I_parThis_004b) {  // $parThis_004b
        I_parOut_004b = 1;
    }
    I_parThis_004b = I_rung_top;
    /* start series [ */
    if (!U_RCalcRelZero) {  // RCalcRelZero
        I_parThis_004b = 0;
    }

    if (!U_RCalcRelE2Fim) {  // RCalcRelE2Fim
        I_parThis_004b = 0;
    }

    if (I_parThis_004b) {  // $parThis_004b
        U_VSrvPosMesa = 0;
    }

    /* ] finish series */
    if (I_parThis_004b) {  // $parThis_004b
        I_parOut_004b = 1;
    }
    I_parThis_004b = I_rung_top;
    if (I_parThis_004b) {  // $parThis_004b
        U_RPosRelConfig = 1;
    }

    if (I_parThis_004b) {  // $parThis_004b
        I_parOut_004b = 1;
    }
    I_rung_top = I_parOut_004b;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 253 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0031) {  // $oneShot_0031
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0031 = I_scratch;

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RPosRelConfig = 0;
    }

    /* ] finish series */

    /* start rung 254 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0032) {  // $oneShot_0032
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0032 = I_scratch;

    if (!U_RCalcRelE2OK) {  // RCalcRelE2OK
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_004c = 0;
    I_parThis_004c = I_rung_top;
    if (I_parThis_004c) {  // $parThis_004c
        U_RCalcRelAtivo = 0;
    }

    if (I_parThis_004c) {  // $parThis_004c
        I_parOut_004c = 1;
    }
    I_parThis_004c = I_rung_top;
    if (I_parThis_004c) {  // $parThis_004c
        MODBUS_REGISTER[8] = U_CalcRelPerfFtr;
    }

    if (I_parThis_004c) {  // $parThis_004c
        I_parOut_004c = 1;
    }
    I_parThis_004c = I_rung_top;
    if (I_parThis_004c) {  // $parThis_004c
        MODBUS_REGISTER[9] = U_CalcRelSrvFtr;
    }

    if (I_parThis_004c) {  // $parThis_004c
        I_parOut_004c = 1;
    }
    I_rung_top = I_parOut_004c;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 255 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0033) {  // $oneShot_0033
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0033 = I_scratch;

    if (!U_RCalcRelE1OK) {  // RCalcRelE1OK
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_004d = 0;
    I_parThis_004d = I_rung_top;
    if (I_parThis_004d) {  // $parThis_004d
        U_RCalcRelE2Start = 0;
    }

    if (I_parThis_004d) {  // $parThis_004d
        I_parOut_004d = 1;
    }
    I_parThis_004d = I_rung_top;
    if (I_parThis_004d) {  // $parThis_004d
        U_RCalcRelE2Fim = 1;
    }

    if (I_parThis_004d) {  // $parThis_004d
        I_parOut_004d = 1;
    }
    I_rung_top = I_parOut_004d;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 256 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0034) {  // $oneShot_0034
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0034 = I_scratch;

    if (U_RCalcRelZero) {  // RCalcRelZero
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_004e = 0;
    I_parThis_004e = I_rung_top;
    if (I_parThis_004e) {  // $parThis_004e
        U_RCalcRelZero = 1;
    }

    if (I_parThis_004e) {  // $parThis_004e
        I_parOut_004e = 1;
    }
    I_parThis_004e = I_rung_top;
    if (I_parThis_004e) {  // $parThis_004e
        U_RCalcRelE1Start = 1;
    }

    if (I_parThis_004e) {  // $parThis_004e
        I_parOut_004e = 1;
    }
    I_rung_top = I_parOut_004e;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 257 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcRelE1Start) {  // RCalcRelE1Start
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_004f = 0;
    I_parThis_004f = I_rung_top;
    /* start series [ */
    if (I_parThis_004f) {  // $parThis_004f
        if (U_TEspEncPrfIniTO < 199) {
            U_TEspEncPrfIniTO++;
            I_parThis_004f = 0;
        }
    } else {
        U_TEspEncPrfIniTO = 0;
    }

    if (I_parThis_004f) {  // $parThis_004f
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_004f) {  // $parThis_004f
        I_parOut_004f = 1;
    }
    I_parThis_004f = I_rung_top;
    /* start series [ */
    if (I_parThis_004f) {  // $parThis_004f
        if (!I_oneShot_0035) {  // $oneShot_0035
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15044, 2, &U_CalcRelPerfIni);
                I_oneShot_0035 = I_parThis_004f;
            }
            I_parThis_004f = 0;
            I_oneShot_0036 = I_parThis_004f;
        }
        if (!I_oneShot_0036) {  // $oneShot_0036
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0036 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15044, 2, &U_CalcRelPerfIni);
                I_SerialTimeout = 0;
            } else {
                I_parThis_004f = 0;
            }
        }
    } else {
        if (I_oneShot_0035) {  // $oneShot_0035
            I_SerialReady = 1;
        }
        I_oneShot_0035 = I_parThis_004f;
    }

    /* start parallel [ */
    I_parOut_0050 = 0;
    I_parThis_0050 = I_parThis_004f;
    if (I_parThis_0050) {  // $parThis_0050
        U_RCalcRelE1Avanca = 1;
    }

    if (I_parThis_0050) {  // $parThis_0050
        I_parOut_0050 = 1;
    }
    I_parThis_0050 = I_parThis_004f;
    if (I_parThis_0050) {  // $parThis_0050
        U_RCalcRelE1Start = 0;
    }

    if (I_parThis_0050) {  // $parThis_0050
        I_parOut_0050 = 1;
    }
    I_parThis_0050 = I_parThis_004f;
    if (I_parThis_0050) {  // $parThis_0050
        U_CalcRelMesaIni = U_MaqPosAtual;
    }

    if (I_parThis_0050) {  // $parThis_0050
        I_parOut_0050 = 1;
    }
    I_parThis_004f = I_parOut_0050;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_004f) {  // $parThis_004f
        I_parOut_004f = 1;
    }
    I_rung_top = I_parOut_004f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 258 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcRelE1Avanca) {  // RCalcRelE1Avanca
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0051 = 0;
    I_parThis_0051 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0051;
    if (I_oneShot_0037) {  // $oneShot_0037
        I_parThis_0051 = 0;
    }
    I_oneShot_0037 = I_scratch;

    /* start parallel [ */
    I_parOut_0052 = 0;
    I_parThis_0052 = I_parThis_0051;
    if (I_parThis_0052) {  // $parThis_0052
        U_RInvAtivar = 1;
    }

    if (I_parThis_0052) {  // $parThis_0052
        I_parOut_0052 = 1;
    }
    I_parThis_0052 = I_parThis_0051;
    if (I_parThis_0052) {  // $parThis_0052
        U_CalcRelPosFim = U_CalcRelMesaIni + U_SrvMesaCurso;
    }

    if (I_parThis_0052) {  // $parThis_0052
        I_parOut_0052 = 1;
    }
    I_parThis_0051 = I_parOut_0052;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0051) {  // $parThis_0051
        I_parOut_0051 = 1;
    }
    I_parThis_0051 = I_rung_top;
    /* start series [ */
    if (!U_RInvFreioLib) {  // RInvFreioLib
        I_parThis_0051 = 0;
    }

    /* start parallel [ */
    I_parOut_0053 = 0;
    I_parThis_0053 = I_parThis_0051;
    /* start series [ */
    if (U_CalcRelPosFim > U_MaqPosAtual) {
        I_parThis_0053 = 0;
    }

    if (I_parThis_0053) {  // $parThis_0053
        U_RInvAtivar = 0;
    }

    /* ] finish series */
    if (I_parThis_0053) {  // $parThis_0053
        I_parOut_0053 = 1;
    }
    I_parThis_0053 = I_parThis_0051;
    /* start series [ */
    I_scratch = I_parThis_0053;
    if (!I_parThis_0053) {  // $parThis_0053
        if (I_oneShot_0038) {  // $oneShot_0038
            I_parThis_0053 = 1;
        }
    } else {
        I_parThis_0053 = 0;
    }
    I_oneShot_0038 = I_scratch;

    /* start parallel [ */
    I_parOut_0054 = 0;
    I_parThis_0054 = I_parThis_0053;
    if (I_parThis_0054) {  // $parThis_0054
        U_RCalcRelE1Avanca = 0;
    }

    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_parThis_0053;
    if (I_parThis_0054) {  // $parThis_0054
        U_RCalcRelE1Fim = 1;
    }

    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0053 = I_parOut_0054;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0053) {  // $parThis_0053
        I_parOut_0053 = 1;
    }
    I_parThis_0051 = I_parOut_0053;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0051) {  // $parThis_0051
        I_parOut_0051 = 1;
    }
    I_rung_top = I_parOut_0051;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 259 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcRelE1Fim) {  // RCalcRelE1Fim
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0055 = 0;
    I_parThis_0055 = I_rung_top;
    /* start series [ */
    if (I_parThis_0055) {  // $parThis_0055
        if (U_TEspEncPrfFimTO < 199) {
            U_TEspEncPrfFimTO++;
            I_parThis_0055 = 0;
        }
    } else {
        U_TEspEncPrfFimTO = 0;
    }

    if (I_parThis_0055) {  // $parThis_0055
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0055 = I_rung_top;
    /* start series [ */
    if (I_parThis_0055) {  // $parThis_0055
        if (!I_oneShot_0039) {  // $oneShot_0039
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15044, 2, &U_CalcRelPerfFim);
                I_oneShot_0039 = I_parThis_0055;
            }
            I_parThis_0055 = 0;
            I_oneShot_003a = I_parThis_0055;
        }
        if (!I_oneShot_003a) {  // $oneShot_003a
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_003a = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15044, 2, &U_CalcRelPerfFim);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0055 = 0;
            }
        }
    } else {
        if (I_oneShot_0039) {  // $oneShot_0039
            I_SerialReady = 1;
        }
        I_oneShot_0039 = I_parThis_0055;
    }

    /* start parallel [ */
    I_parOut_0056 = 0;
    I_parThis_0056 = I_parThis_0055;
    if (I_parThis_0056) {  // $parThis_0056
        U_RCalcRelE1OK = 1;
    }

    if (I_parThis_0056) {  // $parThis_0056
        I_parOut_0056 = 1;
    }
    I_parThis_0056 = I_parThis_0055;
    if (I_parThis_0056) {  // $parThis_0056
        U_RCalcRelE1Fim = 0;
    }

    if (I_parThis_0056) {  // $parThis_0056
        I_parOut_0056 = 1;
    }
    I_parThis_0056 = I_parThis_0055;
    if (I_parThis_0056) {  // $parThis_0056
        U_CalcRelMesaDif = U_MaqPosAtual - U_CalcRelMesaIni;
    }

    if (I_parThis_0056) {  // $parThis_0056
        I_parOut_0056 = 1;
    }
    I_parThis_0056 = I_parThis_0055;
    if (I_parThis_0056) {  // $parThis_0056
        U_CalcRelPerfDif = U_CalcRelPerfFim - U_CalcRelPerfIni;
    }

    if (I_parThis_0056) {  // $parThis_0056
        I_parOut_0056 = 1;
    }
    I_parThis_0056 = I_parThis_0055;
    if (I_parThis_0056) {  // $parThis_0056
        I_scratch2 = 1000;
        U_CalcRelMesaDif = U_CalcRelMesaDif * I_scratch2;
    }

    if (I_parThis_0056) {  // $parThis_0056
        I_parOut_0056 = 1;
    }
    I_parThis_0056 = I_parThis_0055;
    /* start series [ */
    I_scratch2 = 0;
    if (U_CalcRelPerfDif == I_scratch2) {
        I_parThis_0056 = 0;
    }

    if (I_parThis_0056) {  // $parThis_0056
        if(U_CalcRelPerfDif) U_CalcRelPerfFtr = U_CalcRelMesaDif / U_CalcRelPerfDif;
    }

    /* ] finish series */
    if (I_parThis_0056) {  // $parThis_0056
        I_parOut_0056 = 1;
    }
    I_parThis_0055 = I_parOut_0056;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_rung_top = I_parOut_0055;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 260 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcRelE1OK) {  // RCalcRelE1OK
        I_rung_top = 0;
    }

    if (U_RCalcRelE2Start) {  // RCalcRelE2Start
        I_rung_top = 0;
    }

    if (U_RCalcRelE2Fim) {  // RCalcRelE2Fim
        I_rung_top = 0;
    }

    if (U_RCalcRelE2OK) {  // RCalcRelE2OK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0057 = 0;
    I_parThis_0057 = I_rung_top;
    /* start series [ */
    if (I_parThis_0057) {  // $parThis_0057
        if (U_TEspEncSrvIniTO < 199) {
            U_TEspEncSrvIniTO++;
            I_parThis_0057 = 0;
        }
    } else {
        U_TEspEncSrvIniTO = 0;
    }

    if (I_parThis_0057) {  // $parThis_0057
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0057) {  // $parThis_0057
        I_parOut_0057 = 1;
    }
    I_parThis_0057 = I_rung_top;
    /* start series [ */
    if (I_parThis_0057) {  // $parThis_0057
        if (!I_oneShot_003b) {  // $oneShot_003b
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15046, 2, &U_CalcRelSrvIni);
                I_oneShot_003b = I_parThis_0057;
            }
            I_parThis_0057 = 0;
            I_oneShot_003c = I_parThis_0057;
        }
        if (!I_oneShot_003c) {  // $oneShot_003c
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_003c = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15046, 2, &U_CalcRelSrvIni);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0057 = 0;
            }
        }
    } else {
        if (I_oneShot_003b) {  // $oneShot_003b
            I_SerialReady = 1;
        }
        I_oneShot_003b = I_parThis_0057;
    }

    /* start parallel [ */
    I_parOut_0058 = 0;
    I_parThis_0058 = I_parThis_0057;
    if (I_parThis_0058) {  // $parThis_0058
        U_RCalcRelE2Start = 1;
    }

    if (I_parThis_0058) {  // $parThis_0058
        I_parOut_0058 = 1;
    }
    I_parThis_0058 = I_parThis_0057;
    if (I_parThis_0058) {  // $parThis_0058
        U_CalcRelMesaIni = U_MaqPosAtual;
    }

    if (I_parThis_0058) {  // $parThis_0058
        I_parOut_0058 = 1;
    }
    I_parThis_0057 = I_parOut_0058;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0057) {  // $parThis_0057
        I_parOut_0057 = 1;
    }
    I_rung_top = I_parOut_0057;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 261 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcRelE2Fim) {  // RCalcRelE2Fim
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0059 = 0;
    I_parThis_0059 = I_rung_top;
    /* start series [ */
    if (I_parThis_0059) {  // $parThis_0059
        if (U_TEspEncSrvFimTO < 199) {
            U_TEspEncSrvFimTO++;
            I_parThis_0059 = 0;
        }
    } else {
        U_TEspEncSrvFimTO = 0;
    }

    if (I_parThis_0059) {  // $parThis_0059
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0059) {  // $parThis_0059
        I_parOut_0059 = 1;
    }
    I_parThis_0059 = I_rung_top;
    /* start series [ */
    if (I_parThis_0059) {  // $parThis_0059
        if (!I_oneShot_003d) {  // $oneShot_003d
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15046, 2, &U_CalcRelSrvFim);
                I_oneShot_003d = I_parThis_0059;
            }
            I_parThis_0059 = 0;
            I_oneShot_003e = I_parThis_0059;
        }
        if (!I_oneShot_003e) {  // $oneShot_003e
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_003e = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15046, 2, &U_CalcRelSrvFim);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0059 = 0;
            }
        }
    } else {
        if (I_oneShot_003d) {  // $oneShot_003d
            I_SerialReady = 1;
        }
        I_oneShot_003d = I_parThis_0059;
    }

    /* start parallel [ */
    I_parOut_005a = 0;
    I_parThis_005a = I_parThis_0059;
    if (I_parThis_005a) {  // $parThis_005a
        U_RCalcRelE2OK = 1;
    }

    if (I_parThis_005a) {  // $parThis_005a
        I_parOut_005a = 1;
    }
    I_parThis_005a = I_parThis_0059;
    if (I_parThis_005a) {  // $parThis_005a
        U_RCalcRelE2Fim = 0;
    }

    if (I_parThis_005a) {  // $parThis_005a
        I_parOut_005a = 1;
    }
    I_parThis_005a = I_parThis_0059;
    if (I_parThis_005a) {  // $parThis_005a
        U_CalcRelMesaDif = U_MaqPosAtual - U_CalcRelMesaIni;
    }

    if (I_parThis_005a) {  // $parThis_005a
        I_parOut_005a = 1;
    }
    I_parThis_005a = I_parThis_0059;
    if (I_parThis_005a) {  // $parThis_005a
        U_CalcRelSrvDif = U_CalcRelSrvFim - U_CalcRelSrvIni;
    }

    if (I_parThis_005a) {  // $parThis_005a
        I_parOut_005a = 1;
    }
    I_parThis_005a = I_parThis_0059;
    if (I_parThis_005a) {  // $parThis_005a
        I_scratch2 = 1000;
        U_CalcRelMesaDif = U_CalcRelMesaDif * I_scratch2;
    }

    if (I_parThis_005a) {  // $parThis_005a
        I_parOut_005a = 1;
    }
    I_parThis_005a = I_parThis_0059;
    /* start series [ */
    I_scratch2 = 0;
    if (U_CalcRelSrvDif == I_scratch2) {
        I_parThis_005a = 0;
    }

    if (I_parThis_005a) {  // $parThis_005a
        if(U_CalcRelSrvDif) U_CalcRelSrvFtr = U_CalcRelMesaDif / U_CalcRelSrvDif;
    }

    /* ] finish series */
    if (I_parThis_005a) {  // $parThis_005a
        I_parOut_005a = 1;
    }
    I_parThis_0059 = I_parOut_005a;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0059) {  // $parThis_0059
        I_parOut_0059 = 1;
    }
    I_rung_top = I_parOut_0059;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 262 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (U_RCalcRelZero) {  // RCalcRelZero
        I_rung_top = 0;
    }

    if (!U_RPosRelConfig) {  // RPosRelConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 263 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcRelE1OK) {  // RCalcRelE1OK
        I_rung_top = 0;
    }

    if (!U_RCalcRelE2Start) {  // RCalcRelE2Start
        I_rung_top = 0;
    }

    if (!U_RPosRelConfig) {  // RPosRelConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 264 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcRelE1OK) {  // RCalcRelE1OK
        I_rung_top = 0;
    }

    if (!U_RCalcRelE2OK) {  // RCalcRelE2OK
        I_rung_top = 0;
    }

    if (!U_RPosRelConfig) {  // RPosRelConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 265 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    if (!U_RCalcRelAtivo) {  // RCalcRelAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_003f) {  // $oneShot_003f
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_003f = I_scratch;

    /* start parallel [ */
    I_parOut_005b = 0;
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelAtivo = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelZero = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelE1Start = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelE1Avanca = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelE1Fim = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelE1OK = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelE2Start = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelE2Fim = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_rung_top;
    if (I_parThis_005b) {  // $parThis_005b
        U_RCalcRelE2OK = 0;
    }

    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_rung_top = I_parOut_005b;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 267 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_005c = 0;
    I_parThis_005c = I_rung_top;
    if (!U_RMaqCalcTamMin) {  // RMaqCalcTamMin
        I_parThis_005c = 0;
    }

    if (I_parThis_005c) {  // $parThis_005c
        I_parOut_005c = 1;
    }
    I_parThis_005c = I_rung_top;
    if (!(MODBUS_REGISTER[3] & (1 << 11))) {  // XDbgCalcTamMin
        I_parThis_005c = 0;
    }

    if (I_parThis_005c) {  // $parThis_005c
        I_parOut_005c = 1;
    }
    I_rung_top = I_parOut_005c;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_005d = 0;
    I_parThis_005d = I_rung_top;
    /* start series [ */
    if (U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_parThis_005d = 0;
    }

    if (I_parThis_005d) {  // $parThis_005d
        U_RCalcTamMinAtivo = 1;
    }

    /* ] finish series */
    if (I_parThis_005d) {  // $parThis_005d
        I_parOut_005d = 1;
    }
    I_parThis_005d = I_rung_top;
    if (I_parThis_005d) {  // $parThis_005d
        U_RMaqCalcTamMin = 0;
    }

    if (I_parThis_005d) {  // $parThis_005d
        I_parOut_005d = 1;
    }
    I_rung_top = I_parOut_005d;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 268 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (U_RPosTamMinConfig) {  // RPosTamMinConfig
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_005e = 0;
    I_parThis_005e = I_rung_top;
    /* start series [ */
    if (U_RCalcTamMinZero) {  // RCalcTamMinZero
        I_parThis_005e = 0;
    }

    if (U_RCalcTamMinPosic) {  // RCalcTamMinPosic
        I_parThis_005e = 0;
    }

    if (I_parThis_005e) {  // $parThis_005e
        U_VSrvPosMesa = 0;
    }

    /* ] finish series */
    if (I_parThis_005e) {  // $parThis_005e
        I_parOut_005e = 1;
    }
    I_parThis_005e = I_rung_top;
    /* start series [ */
    if (!U_RCalcTamMinZero) {  // RCalcTamMinZero
        I_parThis_005e = 0;
    }

    if (U_RCalcTamMinPosic) {  // RCalcTamMinPosic
        I_parThis_005e = 0;
    }

    if (I_parThis_005e) {  // $parThis_005e
        U_VSrvPosMesa = U_SrvMesaCurso;
    }

    /* ] finish series */
    if (I_parThis_005e) {  // $parThis_005e
        I_parOut_005e = 1;
    }
    I_parThis_005e = I_rung_top;
    /* start series [ */
    if (!U_RCalcTamMinZero) {  // RCalcTamMinZero
        I_parThis_005e = 0;
    }

    if (!U_RCalcTamMinPosic) {  // RCalcTamMinPosic
        I_parThis_005e = 0;
    }

    if (I_parThis_005e) {  // $parThis_005e
        U_VSrvPosMesa = 0;
    }

    /* ] finish series */
    if (I_parThis_005e) {  // $parThis_005e
        I_parOut_005e = 1;
    }
    I_parThis_005e = I_rung_top;
    if (I_parThis_005e) {  // $parThis_005e
        U_RPosTamMinConfig = 1;
    }

    if (I_parThis_005e) {  // $parThis_005e
        I_parOut_005e = 1;
    }
    I_rung_top = I_parOut_005e;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 269 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0040) {  // $oneShot_0040
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0040 = I_scratch;

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RPosTamMinConfig = 0;
    }

    /* ] finish series */

    /* start rung 270 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0041) {  // $oneShot_0041
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0041 = I_scratch;

    if (!U_RCalcTamMinPosic) {  // RCalcTamMinPosic
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinZero) {  // RCalcTamMinZero
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_005f = 0;
    I_parThis_005f = I_rung_top;
    if (I_parThis_005f) {  // $parThis_005f
        U_RCalcTamMinAtivo = 0;
    }

    if (I_parThis_005f) {  // $parThis_005f
        I_parOut_005f = 1;
    }
    I_parThis_005f = I_rung_top;
    if (I_parThis_005f) {  // $parThis_005f
        U_RInvAtivar = 0;
    }

    if (I_parThis_005f) {  // $parThis_005f
        I_parOut_005f = 1;
    }
    I_parThis_005f = I_rung_top;
    if (I_parThis_005f) {  // $parThis_005f
        U_RStartCfgInvMan = 1;
    }

    if (I_parThis_005f) {  // $parThis_005f
        I_parOut_005f = 1;
    }
    I_parThis_005f = I_rung_top;
    if (I_parThis_005f) {  // $parThis_005f
        MODBUS_REGISTER[23] = U_MaqPosAtual - U_CalcTamMinIni;
    }

    if (I_parThis_005f) {  // $parThis_005f
        I_parOut_005f = 1;
    }
    I_rung_top = I_parOut_005f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 271 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvStartPosic) {  // RSrvStartPosic
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0042) {  // $oneShot_0042
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0042 = I_scratch;

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_CalcTamMinIni = U_MaqPosAtual;
    }

    /* ] finish series */

    /* start rung 272 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0043) {  // $oneShot_0043
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0043 = I_scratch;

    if (U_RCalcTamMinPosic) {  // RCalcTamMinPosic
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinZero) {  // RCalcTamMinZero
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RCalcTamMinPosic = 1;
    }

    /* ] finish series */

    /* start rung 273 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0044) {  // $oneShot_0044
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0044 = I_scratch;

    if (U_RCalcTamMinZero) {  // RCalcTamMinZero
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0060 = 0;
    I_parThis_0060 = I_rung_top;
    if (I_parThis_0060) {  // $parThis_0060
        U_RCalcTamMinZero = 1;
    }

    if (I_parThis_0060) {  // $parThis_0060
        I_parOut_0060 = 1;
    }
    I_parThis_0060 = I_rung_top;
    if (I_parThis_0060) {  // $parThis_0060
        U_RStartCfgInvAuto = 1;
    }

    if (I_parThis_0060) {  // $parThis_0060
        I_parOut_0060 = 1;
    }
    I_rung_top = I_parOut_0060;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 274 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqCfgInvOK) {  // RMaqCfgInvOK
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0045) {  // $oneShot_0045
        I_rung_top = 0;
    }
    I_oneShot_0045 = I_scratch;

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RInvAtivar = 1;
    }

    /* ] finish series */

    /* start rung 275 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (U_RCalcTamMinZero) {  // RCalcTamMinZero
        I_rung_top = 0;
    }

    if (!U_RPosTamMinConfig) {  // RPosTamMinConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 276 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (U_RCalcTamMinPosic) {  // RCalcTamMinPosic
        I_rung_top = 0;
    }

    if (!U_RPosTamMinConfig) {  // RPosTamMinConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinZero) {  // RCalcTamMinZero
        I_rung_top = 0;
    }

    if (!U_RInvFreioLib) {  // RInvFreioLib
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 277 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinPosic) {  // RCalcTamMinPosic
        I_rung_top = 0;
    }

    if (!U_RPosTamMinConfig) {  // RPosTamMinConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 278 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCalcTamMinAtivo) {  // RCalcTamMinAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0046) {  // $oneShot_0046
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0046 = I_scratch;

    /* start parallel [ */
    I_parOut_0061 = 0;
    I_parThis_0061 = I_rung_top;
    if (I_parThis_0061) {  // $parThis_0061
        U_RCalcTamMinAtivo = 0;
    }

    if (I_parThis_0061) {  // $parThis_0061
        I_parOut_0061 = 1;
    }
    I_parThis_0061 = I_rung_top;
    if (I_parThis_0061) {  // $parThis_0061
        U_RCalcTamMinPosic = 0;
    }

    if (I_parThis_0061) {  // $parThis_0061
        I_parOut_0061 = 1;
    }
    I_parThis_0061 = I_rung_top;
    if (I_parThis_0061) {  // $parThis_0061
        U_RCalcTamMinZero = 0;
    }

    if (I_parThis_0061) {  // $parThis_0061
        I_parOut_0061 = 1;
    }
    I_parThis_0061 = I_rung_top;
    if (I_parThis_0061) {  // $parThis_0061
        U_RInvAtivar = 0;
    }

    if (I_parThis_0061) {  // $parThis_0061
        I_parOut_0061 = 1;
    }
    I_rung_top = I_parOut_0061;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 280 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 6))) {  // XDbgParamSync
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 4);  // YMbParamSync
    }

    /* ] finish series */

    /* start rung 281 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 4))) {  // YMbParamSync
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0062 = 0;
    I_parThis_0062 = I_rung_top;
    if (I_parThis_0062) {  // $parThis_0062
        U_RStartCfgSrvPar = 1;
    }

    if (I_parThis_0062) {  // $parThis_0062
        I_parOut_0062 = 1;
    }
    I_parThis_0062 = I_rung_top;
    if (I_parThis_0062) {  // $parThis_0062
        U_RMaqCfgSrvParOK = 0;
    }

    if (I_parThis_0062) {  // $parThis_0062
        I_parOut_0062 = 1;
    }
    I_parThis_0062 = I_rung_top;
    if (I_parThis_0062) {  // $parThis_0062
        MODBUS_REGISTER[2] &= ~(1 << 4);  // YMbParamSync
    }

    if (I_parThis_0062) {  // $parThis_0062
        I_parOut_0062 = 1;
    }
    I_rung_top = I_parOut_0062;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 282 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgSrvPar) {  // RStartCfgSrvPar
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0063 = 0;
    I_parThis_0063 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0063;
    if (I_oneShot_0047) {  // $oneShot_0047
        I_parThis_0063 = 0;
    }
    I_oneShot_0047 = I_scratch;

    if (I_parThis_0063) {  // $parThis_0063
        U_RMaqCfgSrvParOK = 0;
    }

    /* ] finish series */
    if (I_parThis_0063) {  // $parThis_0063
        I_parOut_0063 = 1;
    }
    I_parThis_0063 = I_rung_top;
    /* start series [ */
    if (I_parThis_0063) {  // $parThis_0063
        if (U_TCfgSrvIniTO < 499) {
            U_TCfgSrvIniTO++;
            I_parThis_0063 = 0;
        }
    } else {
        U_TCfgSrvIniTO = 0;
    }

    if (I_parThis_0063) {  // $parThis_0063
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0063) {  // $parThis_0063
        I_parOut_0063 = 1;
    }
    I_parThis_0063 = I_rung_top;
    /* start series [ */
    if (I_parThis_0063) {  // $parThis_0063
        if (!I_oneShot_0048) {  // $oneShot_0048
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15048, 2, &U_SrvMesaCurso);
                I_oneShot_0048 = I_parThis_0063;
            }
            I_parThis_0063 = 0;
            I_oneShot_0049 = I_parThis_0063;
        }
        if (!I_oneShot_0049) {  // $oneShot_0049
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0049 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15048, 2, &U_SrvMesaCurso);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0063 = 0;
            }
        }
    } else {
        if (I_oneShot_0048) {  // $oneShot_0048
            I_SerialReady = 1;
        }
        I_oneShot_0048 = I_parThis_0063;
    }

    if (I_parThis_0063) {  // $parThis_0063
        if (!I_oneShot_004a) {  // $oneShot_004a
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15034, 2, &U_SrvVelJog);
                I_oneShot_004a = I_parThis_0063;
            }
            I_parThis_0063 = 0;
            I_oneShot_004b = I_parThis_0063;
        }
        if (!I_oneShot_004b) {  // $oneShot_004b
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_004b = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15034, 2, &U_SrvVelJog);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0063 = 0;
            }
        }
    } else {
        if (I_oneShot_004a) {  // $oneShot_004a
            I_SerialReady = 1;
        }
        I_oneShot_004a = I_parThis_0063;
    }

    if (I_parThis_0063) {  // $parThis_0063
        if (!I_oneShot_004c) {  // $oneShot_004c
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15032, 2, &U_SrvVelPos);
                I_oneShot_004c = I_parThis_0063;
            }
            I_parThis_0063 = 0;
            I_oneShot_004d = I_parThis_0063;
        }
        if (!I_oneShot_004d) {  // $oneShot_004d
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_004d = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15032, 2, &U_SrvVelPos);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0063 = 0;
            }
        }
    } else {
        if (I_oneShot_004c) {  // $oneShot_004c
            I_SerialReady = 1;
        }
        I_oneShot_004c = I_parThis_0063;
    }

    /* start parallel [ */
    I_parOut_0064 = 0;
    I_parThis_0064 = I_parThis_0063;
    if (I_parThis_0064) {  // $parThis_0064
        U_RStartCfgSrvPar = 0;
    }

    if (I_parThis_0064) {  // $parThis_0064
        I_parOut_0064 = 1;
    }
    I_parThis_0064 = I_parThis_0063;
    if (I_parThis_0064) {  // $parThis_0064
        U_RMaqCfgSrvParOK = 1;
    }

    if (I_parThis_0064) {  // $parThis_0064
        I_parOut_0064 = 1;
    }
    I_parThis_0063 = I_parOut_0064;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0063) {  // $parThis_0063
        I_parOut_0063 = 1;
    }
    I_rung_top = I_parOut_0063;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 284 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RMaqConfigOK) {  // RMaqConfigOK
        I_rung_top = 0;
    }

    if (U_RMaqInitOK) {  // RMaqInitOK
        I_rung_top = 0;
    }

    if (U_RMaqInit) {  // RMaqInit
        I_rung_top = 0;
    }

    if (U_RMaqInitStart) {  // RMaqInitStart
        I_rung_top = 0;
    }

    if (U_RMaqInitFim) {  // RMaqInitFim
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqInitStart = 1;
    }

    /* ] finish series */

    /* start rung 285 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqInitStart) {  // RMaqInitStart
        I_rung_top = 0;
    }

    if (U_RFerramCorteCima) {  // RFerramCorteCima
        I_rung_top = 0;
    }

    U_RMaqInitCorte = I_rung_top;

    /* ] finish series */

    /* start rung 286 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqInitStart) {  // RMaqInitStart
        I_rung_top = 0;
    }

    if (!U_RFerramCorteCima) {  // RFerramCorteCima
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT8) {  // YSrvHabilita
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0065 = 0;
    I_parThis_0065 = I_rung_top;
    /* start series [ */
    if (I_parThis_0065) {  // $parThis_0065
        if (U_TSrvInitAutoTO < 199) {
            U_TSrvInitAutoTO++;
            I_parThis_0065 = 0;
        }
    } else {
        U_TSrvInitAutoTO = 0;
    }

    if (I_parThis_0065) {  // $parThis_0065
        U_RMaqInitStart = 0;
    }

    /* ] finish series */
    if (I_parThis_0065) {  // $parThis_0065
        I_parOut_0065 = 1;
    }
    I_parThis_0065 = I_rung_top;
    /* start series [ */
    if (I_parThis_0065) {  // $parThis_0065
        if (!I_oneShot_004e) {  // $oneShot_004e
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_004e = I_parThis_0065;
            }
            I_parThis_0065 = 0;
            I_oneShot_004f = I_parThis_0065;
        }
        if (!I_oneShot_004f) {  // $oneShot_004f
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_004f = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0065 = 0;
            }
        }
    } else {
        if (I_oneShot_004e) {  // $oneShot_004e
            I_SerialReady = 1;
        }
        I_oneShot_004e = I_parThis_0065;
    }

    if (I_parThis_0065) {  // $parThis_0065
        U_SrvRegConfig |= 1 << 2;
    }

    if (I_parThis_0065) {  // $parThis_0065
        if (!I_oneShot_0050) {  // $oneShot_0050
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0050 = I_parThis_0065;
            }
            I_parThis_0065 = 0;
            I_oneShot_0051 = I_parThis_0065;
        }
        if (!I_oneShot_0051) {  // $oneShot_0051
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0051 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0065 = 0;
            }
        }
    } else {
        if (I_oneShot_0050) {  // $oneShot_0050
            I_SerialReady = 1;
        }
        I_oneShot_0050 = I_parThis_0065;
    }

    if (I_parThis_0065) {  // $parThis_0065
        GPIO_OUTPUT_PORT8 = 1;
    }

    /* ] finish series */
    if (I_parThis_0065) {  // $parThis_0065
        I_parOut_0065 = 1;
    }
    I_rung_top = I_parOut_0065;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 287 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqInitStart) {  // RMaqInitStart
        I_rung_top = 0;
    }

    if (!U_RFerramCorteCima) {  // RFerramCorteCima
        I_rung_top = 0;
    }

    if (!GPIO_OUTPUT_PORT8) {  // YSrvHabilita
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspServoON < 49) {
            U_TEspServoON++;
            I_rung_top = 0;
        }
    } else {
        U_TEspServoON = 0;
    }

    /* start parallel [ */
    I_parOut_0066 = 0;
    I_parThis_0066 = I_rung_top;
    /* start series [ */
    if (I_parThis_0066) {  // $parThis_0066
        if (U_TSrvStartRefTO < 199) {
            U_TSrvStartRefTO++;
            I_parThis_0066 = 0;
        }
    } else {
        U_TSrvStartRefTO = 0;
    }

    if (I_parThis_0066) {  // $parThis_0066
        U_RMaqInitStart = 0;
    }

    /* ] finish series */
    if (I_parThis_0066) {  // $parThis_0066
        I_parOut_0066 = 1;
    }
    I_parThis_0066 = I_rung_top;
    /* start series [ */
    if (I_parThis_0066) {  // $parThis_0066
        if (!I_oneShot_0052) {  // $oneShot_0052
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0052 = I_parThis_0066;
            }
            I_parThis_0066 = 0;
            I_oneShot_0053 = I_parThis_0066;
        }
        if (!I_oneShot_0053) {  // $oneShot_0053
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0053 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0066 = 0;
            }
        }
    } else {
        if (I_oneShot_0052) {  // $oneShot_0052
            I_SerialReady = 1;
        }
        I_oneShot_0052 = I_parThis_0066;
    }

    if (I_parThis_0066) {  // $parThis_0066
        U_SrvRegConfig |= 1 << 4;
    }

    if (I_parThis_0066) {  // $parThis_0066
        if (!I_oneShot_0054) {  // $oneShot_0054
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0054 = I_parThis_0066;
            }
            I_parThis_0066 = 0;
            I_oneShot_0055 = I_parThis_0066;
        }
        if (!I_oneShot_0055) {  // $oneShot_0055
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0055 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0066 = 0;
            }
        }
    } else {
        if (I_oneShot_0054) {  // $oneShot_0054
            I_SerialReady = 1;
        }
        I_oneShot_0054 = I_parThis_0066;
    }

    /* start parallel [ */
    I_parOut_0067 = 0;
    I_parThis_0067 = I_parThis_0066;
    if (I_parThis_0067) {  // $parThis_0067
        U_RMaqInitStart = 0;
    }

    if (I_parThis_0067) {  // $parThis_0067
        I_parOut_0067 = 1;
    }
    I_parThis_0067 = I_parThis_0066;
    if (I_parThis_0067) {  // $parThis_0067
        U_RMaqInit = 1;
    }

    if (I_parThis_0067) {  // $parThis_0067
        I_parOut_0067 = 1;
    }
    I_parThis_0067 = I_parThis_0066;
    if (I_parThis_0067) {  // $parThis_0067
        U_CAguardaMesaOK = 0;
    }

    if (I_parThis_0067) {  // $parThis_0067
        I_parOut_0067 = 1;
    }
    I_parThis_0066 = I_parOut_0067;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0066) {  // $parThis_0066
        I_parOut_0066 = 1;
    }
    I_rung_top = I_parOut_0066;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 288 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RMaqInit) {  // RMaqInit
        I_rung_top = 0;
    }

    if (U_RSrvReadInitOK) {  // RSrvReadInitOK
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0056) {  // $oneShot_0056
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15000, 1, &U_SrvRegFlags);
                I_oneShot_0056 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0057 = I_rung_top;
        }
        if (!I_oneShot_0057) {  // $oneShot_0057
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0057 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15000, 1, &U_SrvRegFlags);
                I_SerialTimeout = 0;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        if (I_oneShot_0056) {  // $oneShot_0056
            I_SerialReady = 1;
        }
        I_oneShot_0056 = I_rung_top;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0058) {  // $oneShot_0058
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15031, 1, &U_SrvRegStatus);
                I_oneShot_0058 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0059 = I_rung_top;
        }
        if (!I_oneShot_0059) {  // $oneShot_0059
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0059 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15031, 1, &U_SrvRegStatus);
                I_SerialTimeout = 0;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        if (I_oneShot_0058) {  // $oneShot_0058
            I_SerialReady = 1;
        }
        I_oneShot_0058 = I_rung_top;
    }

    /* start parallel [ */
    I_parOut_0068 = 0;
    I_parThis_0068 = I_rung_top;
    /* start series [ */
    if (I_parThis_0068) {  // $parThis_0068
        if (U_TEspReadInitOK < 99) {
            U_TEspReadInitOK++;
            I_parThis_0068 = 0;
        }
    } else {
        U_TEspReadInitOK = 0;
    }

    U_RSrvReadInitOK = I_parThis_0068;

    /* ] finish series */
    if (I_parThis_0068) {  // $parThis_0068
        I_parOut_0068 = 1;
    }
    I_parThis_0068 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0069 = 0;
    I_parThis_0069 = I_parThis_0068;
    /* start series [ */
    if (I_parThis_0069) {  // $parThis_0069
        if (!(U_SrvRegFlags & (1 << 2))) {  // SrvRegFlags
            I_parThis_0069 = 0;
        }
    }

    /* start parallel [ */
    I_parOut_006a = 0;
    I_parThis_006a = I_parThis_0069;
    if (I_parThis_006a) {  // $parThis_006a
        if (!(U_SrvRegStatus & (1 << 6))) {  // SrvRegStatus
            I_parThis_006a = 0;
        }
    }

    if (I_parThis_006a) {  // $parThis_006a
        I_parOut_006a = 1;
    }
    I_parThis_006a = I_parThis_0069;
    if (I_parThis_006a) {  // $parThis_006a
        if (!I_oneShot_005a) {  // $oneShot_005a
            U_CAguardaMesaOK++;
        }
    }
    I_oneShot_005a = I_parThis_006a;
    if (U_CAguardaMesaOK < 5) {
        I_parThis_006a = 0;
    } else {
        I_parThis_006a = 1;
    }

    if (I_parThis_006a) {  // $parThis_006a
        I_parOut_006a = 1;
    }
    I_parThis_0069 = I_parOut_006a;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0069) {  // $parThis_0069
        I_parOut_0069 = 1;
    }
    I_parThis_0069 = I_parThis_0068;
    if (!(MODBUS_REGISTER[3] & (1 << 8))) {  // XDbgSrvInitOK
        I_parThis_0069 = 0;
    }

    if (I_parThis_0069) {  // $parThis_0069
        I_parOut_0069 = 1;
    }
    I_parThis_0068 = I_parOut_0069;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_006b = 0;
    I_parThis_006b = I_parThis_0068;
    if (I_parThis_006b) {  // $parThis_006b
        U_RMaqInitFim = 1;
    }

    if (I_parThis_006b) {  // $parThis_006b
        I_parOut_006b = 1;
    }
    I_parThis_006b = I_parThis_0068;
    if (I_parThis_006b) {  // $parThis_006b
        U_RMaqInit = 0;
    }

    if (I_parThis_006b) {  // $parThis_006b
        I_parOut_006b = 1;
    }
    I_parThis_006b = I_parThis_0068;
    if (I_parThis_006b) {  // $parThis_006b
        U_CAguardaMesaOK = 0;
    }

    if (I_parThis_006b) {  // $parThis_006b
        I_parOut_006b = 1;
    }
    I_parThis_0068 = I_parOut_006b;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0068) {  // $parThis_0068
        I_parOut_0068 = 1;
    }
    I_rung_top = I_parOut_0068;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 289 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RMaqInitFim) {  // RMaqInitFim
        I_rung_top = 0;
    }

    if (U_RSrvStartPosic) {  // RSrvStartPosic
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_006c = 0;
    I_parThis_006c = I_rung_top;
    /* start series [ */
    if (I_parThis_006c) {  // $parThis_006c
        if (U_TSrvFimRefTO < 199) {
            U_TSrvFimRefTO++;
            I_parThis_006c = 0;
        }
    } else {
        U_TSrvFimRefTO = 0;
    }

    if (I_parThis_006c) {  // $parThis_006c
        U_RMaqInitFim = 0;
    }

    /* ] finish series */
    if (I_parThis_006c) {  // $parThis_006c
        I_parOut_006c = 1;
    }
    I_parThis_006c = I_rung_top;
    /* start series [ */
    if (I_parThis_006c) {  // $parThis_006c
        if (!I_oneShot_005b) {  // $oneShot_005b
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_005b = I_parThis_006c;
            }
            I_parThis_006c = 0;
            I_oneShot_005c = I_parThis_006c;
        }
        if (!I_oneShot_005c) {  // $oneShot_005c
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_005c = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_006c = 0;
            }
        }
    } else {
        if (I_oneShot_005b) {  // $oneShot_005b
            I_SerialReady = 1;
        }
        I_oneShot_005b = I_parThis_006c;
    }

    if (I_parThis_006c) {  // $parThis_006c
        U_SrvRegConfig &= ~(1 << 4);
    }

    if (I_parThis_006c) {  // $parThis_006c
        if (!I_oneShot_005d) {  // $oneShot_005d
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_005d = I_parThis_006c;
            }
            I_parThis_006c = 0;
            I_oneShot_005e = I_parThis_006c;
        }
        if (!I_oneShot_005e) {  // $oneShot_005e
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_005e = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_006c = 0;
            }
        }
    } else {
        if (I_oneShot_005d) {  // $oneShot_005d
            I_SerialReady = 1;
        }
        I_oneShot_005d = I_parThis_006c;
    }

    /* start parallel [ */
    I_parOut_006d = 0;
    I_parThis_006d = I_parThis_006c;
    if (I_parThis_006d) {  // $parThis_006d
        U_RMaqStartPosic = 1;
    }

    if (I_parThis_006d) {  // $parThis_006d
        I_parOut_006d = 1;
    }
    I_parThis_006d = I_parThis_006c;
    if (I_parThis_006d) {  // $parThis_006d
        U_VSrvPosMesa = 0;
    }

    if (I_parThis_006d) {  // $parThis_006d
        I_parOut_006d = 1;
    }
    I_parThis_006c = I_parOut_006d;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_006c) {  // $parThis_006c
        I_parOut_006c = 1;
    }
    I_rung_top = I_parOut_006c;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 290 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RMaqInitFim) {  // RMaqInitFim
        I_rung_top = 0;
    }

    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_005f) {  // $oneShot_005f
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_005f = I_scratch;

    /* start parallel [ */
    I_parOut_006e = 0;
    I_parThis_006e = I_rung_top;
    if (I_parThis_006e) {  // $parThis_006e
        U_RMaqInitFim = 0;
    }

    if (I_parThis_006e) {  // $parThis_006e
        I_parOut_006e = 1;
    }
    I_parThis_006e = I_rung_top;
    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_parThis_006e = 0;
    }

    if (I_parThis_006e) {  // $parThis_006e
        U_RMaqInitOK = 1;
    }

    /* ] finish series */
    if (I_parThis_006e) {  // $parThis_006e
        I_parOut_006e = 1;
    }
    I_rung_top = I_parOut_006e;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 292 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 7))) {  // XDbgCorteEstat
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0060) {  // $oneShot_0060
        I_rung_top = 0;
    }
    I_oneShot_0060 = I_scratch;

    if (I_rung_top) {  // $rung_top
        U_RMaqCorteEstat = 1;
    }

    /* ] finish series */

    /* start rung 293 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    if (!U_RMaqCorteEstat) {  // RMaqCorteEstat
        I_rung_top = 0;
    }

    if (U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_006f = 0;
    I_parThis_006f = I_rung_top;
    if (I_parThis_006f) {  // $parThis_006f
        U_RCorteEstatAtivo = 1;
    }

    if (I_parThis_006f) {  // $parThis_006f
        I_parOut_006f = 1;
    }
    I_parThis_006f = I_rung_top;
    if (I_parThis_006f) {  // $parThis_006f
        U_RMaqPosConfig = 0;
    }

    if (I_parThis_006f) {  // $parThis_006f
        I_parOut_006f = 1;
    }
    I_parThis_006f = I_rung_top;
    if (I_parThis_006f) {  // $parThis_006f
        U_RMaqCorteEstat = 0;
    }

    if (I_parThis_006f) {  // $parThis_006f
        I_parOut_006f = 1;
    }
    I_rung_top = I_parOut_006f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 294 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (U_RMaqPosConfig) {  // RMaqPosConfig
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0070 = 0;
    I_parThis_0070 = I_rung_top;
    /* start series [ */
    if (U_RCorteEstatZero) {  // RCorteEstatZero
        I_parThis_0070 = 0;
    }

    if (U_RCorteEstatPosic) {  // RCorteEstatPosic
        I_parThis_0070 = 0;
    }

    if (I_parThis_0070) {  // $parThis_0070
        U_VSrvPosMesa = 0;
    }

    /* ] finish series */
    if (I_parThis_0070) {  // $parThis_0070
        I_parOut_0070 = 1;
    }
    I_parThis_0070 = I_rung_top;
    /* start series [ */
    if (!U_RCorteEstatZero) {  // RCorteEstatZero
        I_parThis_0070 = 0;
    }

    if (U_RCorteEstatPosic) {  // RCorteEstatPosic
        I_parThis_0070 = 0;
    }

    /* start parallel [ */
    I_parOut_0071 = 0;
    I_parThis_0071 = I_parThis_0070;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0072 = 0;
    I_parThis_0072 = I_parThis_0071;
    I_scratch2 = 0;
    if (I_scratch2 > U_DifPosCorteInv) {
    } else {
        I_parThis_0072 = 0;
    }

    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_parThis_0071;
    if (U_DifPosCorteInv > U_SrvMesaCurso) {
    } else {
        I_parThis_0072 = 0;
    }

    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0071 = I_parOut_0072;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0073 = 0;
    I_parThis_0073 = I_parThis_0071;
    if (I_parThis_0073) {  // $parThis_0073
        U_RMaqErroPosic = 1;
    }

    if (I_parThis_0073) {  // $parThis_0073
        I_parOut_0073 = 1;
    }
    I_parThis_0073 = I_parThis_0071;
    if (I_parThis_0073) {  // $parThis_0073
        U_RCorteEstatAtivo = 0;
    }

    if (I_parThis_0073) {  // $parThis_0073
        I_parOut_0073 = 1;
    }
    I_parThis_0071 = I_parOut_0073;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_parThis_0070;
    /* start series [ */
    I_scratch2 = 0;
    if (I_scratch2 > U_DifPosCorteInv) {
        I_parThis_0071 = 0;
    }

    if (U_DifPosCorteInv > U_SrvMesaCurso) {
        I_parThis_0071 = 0;
    }

    if (I_parThis_0071) {  // $parThis_0071
        U_VSrvPosMesa = U_DifPosCorteInv;
    }

    /* ] finish series */
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0070 = I_parOut_0071;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0070) {  // $parThis_0070
        I_parOut_0070 = 1;
    }
    I_parThis_0070 = I_rung_top;
    /* start series [ */
    if (!U_RCorteEstatZero) {  // RCorteEstatZero
        I_parThis_0070 = 0;
    }

    if (!U_RCorteEstatPosic) {  // RCorteEstatPosic
        I_parThis_0070 = 0;
    }

    if (I_parThis_0070) {  // $parThis_0070
        U_VSrvPosMesa = 0;
    }

    /* ] finish series */
    if (I_parThis_0070) {  // $parThis_0070
        I_parOut_0070 = 1;
    }
    I_parThis_0070 = I_rung_top;
    if (I_parThis_0070) {  // $parThis_0070
        U_RMaqPosConfig = 1;
    }

    if (I_parThis_0070) {  // $parThis_0070
        I_parOut_0070 = 1;
    }
    I_rung_top = I_parOut_0070;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 295 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0061) {  // $oneShot_0061
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0061 = I_scratch;

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqPosConfig = 0;
    }

    /* ] finish series */

    /* start rung 296 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0062) {  // $oneShot_0062
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0062 = I_scratch;

    if (!U_RCorteEstatPosic) {  // RCorteEstatPosic
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (!U_RCorteEstatZero) {  // RCorteEstatZero
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RCorteEstatRetOK = 1;
    }

    /* ] finish series */

    /* start rung 297 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0063) {  // $oneShot_0063
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0063 = I_scratch;

    if (U_RCorteEstatPosic) {  // RCorteEstatPosic
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (!U_RCorteEstatZero) {  // RCorteEstatZero
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RCorteEstatPosic = 1;
    }

    /* ] finish series */

    /* start rung 298 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0064) {  // $oneShot_0064
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0064 = I_scratch;

    if (U_RCorteEstatZero) {  // RCorteEstatZero
        I_rung_top = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RCorteEstatZero = 1;
    }

    /* ] finish series */

    /* start rung 299 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (U_RCorteEstatZero) {  // RCorteEstatZero
        I_rung_top = 0;
    }

    if (!U_RMaqPosConfig) {  // RMaqPosConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 300 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (U_RCorteEstatPosic) {  // RCorteEstatPosic
        I_rung_top = 0;
    }

    if (!U_RMaqPosConfig) {  // RMaqPosConfig
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    if (!U_RCorteEstatZero) {  // RCorteEstatZero
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */

    /* start rung 301 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (!U_RCorteEstatPosic) {  // RCorteEstatPosic
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0065) {  // $oneShot_0065
        I_rung_top = 0;
    }
    I_oneShot_0065 = I_scratch;

    if (I_rung_top) {  // $rung_top
        U_RMaqCortePerf = 1;
    }

    /* ] finish series */

    /* start rung 302 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0066) {  // $oneShot_0066
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0066 = I_scratch;

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (!U_RCorteEstatPosic) {  // RCorteEstatPosic
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0074 = 0;
    I_parThis_0074 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_0075 = 0;
    I_parThis_0075 = I_parThis_0074;
    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_parThis_0075 = 0;
    }

    if (I_parThis_0075) {  // $parThis_0075
        I_parOut_0075 = 1;
    }
    I_parThis_0075 = I_parThis_0074;
    if (U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_0075 = 0;
    }

    if (I_parThis_0075) {  // $parThis_0075
        I_parOut_0075 = 1;
    }
    I_parThis_0074 = I_parOut_0075;
    /* ] finish parallel */
    if (I_parThis_0074) {  // $parThis_0074
        U_RMaqStartPosic = 1;
    }

    /* ] finish series */
    if (I_parThis_0074) {  // $parThis_0074
        I_parOut_0074 = 1;
    }
    I_parThis_0074 = I_rung_top;
    /* start series [ */
    if (U_RMaqStartPosic) {  // RMaqStartPosic
        I_parThis_0074 = 0;
    }

    if (I_parThis_0074) {  // $parThis_0074
        U_RCorteEstatRetOK = 1;
    }

    /* ] finish series */
    if (I_parThis_0074) {  // $parThis_0074
        I_parOut_0074 = 1;
    }
    I_rung_top = I_parOut_0074;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 303 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (!U_RCorteEstatRetOK) {  // RCorteEstatRetOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0076 = 0;
    I_parThis_0076 = I_rung_top;
    /* start series [ */
    if (I_parThis_0076) {  // $parThis_0076
        if (U_TCorteEstatTO < 199) {
            U_TCorteEstatTO++;
            I_parThis_0076 = 0;
        }
    } else {
        U_TCorteEstatTO = 0;
    }

    if (I_parThis_0076) {  // $parThis_0076
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0076) {  // $parThis_0076
        I_parOut_0076 = 1;
    }
    I_parThis_0076 = I_rung_top;
    /* start series [ */
    if (I_parThis_0076) {  // $parThis_0076
        if (!I_oneShot_0067) {  // $oneShot_0067
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0067 = I_parThis_0076;
            }
            I_parThis_0076 = 0;
            I_oneShot_0068 = I_parThis_0076;
        }
        if (!I_oneShot_0068) {  // $oneShot_0068
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0068 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0076 = 0;
            }
        }
    } else {
        if (I_oneShot_0067) {  // $oneShot_0067
            I_SerialReady = 1;
        }
        I_oneShot_0067 = I_parThis_0076;
    }

    if (I_parThis_0076) {  // $parThis_0076
        U_SrvRegConfig |= 1 << 2;
    }

    if (I_parThis_0076) {  // $parThis_0076
        if (!I_oneShot_0069) {  // $oneShot_0069
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0069 = I_parThis_0076;
            }
            I_parThis_0076 = 0;
            I_oneShot_006a = I_parThis_0076;
        }
        if (!I_oneShot_006a) {  // $oneShot_006a
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_006a = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0076 = 0;
            }
        }
    } else {
        if (I_oneShot_0069) {  // $oneShot_0069
            I_SerialReady = 1;
        }
        I_oneShot_0069 = I_parThis_0076;
    }

    /* start parallel [ */
    I_parOut_0077 = 0;
    I_parThis_0077 = I_parThis_0076;
    if (I_parThis_0077) {  // $parThis_0077
        U_RCorteEstatAtivo = 0;
    }

    if (I_parThis_0077) {  // $parThis_0077
        I_parOut_0077 = 1;
    }
    I_parThis_0077 = I_parThis_0076;
    if (I_parThis_0077) {  // $parThis_0077
        GPIO_OUTPUT_PORT7 = 0;
    }

    if (I_parThis_0077) {  // $parThis_0077
        I_parOut_0077 = 1;
    }
    I_parThis_0076 = I_parOut_0077;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0076) {  // $parThis_0076
        I_parOut_0076 = 1;
    }
    I_rung_top = I_parOut_0076;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 304 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_006b) {  // $oneShot_006b
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_006b = I_scratch;

    /* start parallel [ */
    I_parOut_0078 = 0;
    I_parThis_0078 = I_rung_top;
    if (I_parThis_0078) {  // $parThis_0078
        U_RCorteEstatAtivo = 0;
    }

    if (I_parThis_0078) {  // $parThis_0078
        I_parOut_0078 = 1;
    }
    I_parThis_0078 = I_rung_top;
    if (I_parThis_0078) {  // $parThis_0078
        U_RCorteEstatPosic = 0;
    }

    if (I_parThis_0078) {  // $parThis_0078
        I_parOut_0078 = 1;
    }
    I_parThis_0078 = I_rung_top;
    if (I_parThis_0078) {  // $parThis_0078
        U_RCorteEstatRetOK = 0;
    }

    if (I_parThis_0078) {  // $parThis_0078
        I_parOut_0078 = 1;
    }
    I_parThis_0078 = I_rung_top;
    if (I_parThis_0078) {  // $parThis_0078
        U_RCorteEstatZero = 0;
    }

    if (I_parThis_0078) {  // $parThis_0078
        I_parOut_0078 = 1;
    }
    I_rung_top = I_parOut_0078;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 306 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgSrvAuto) {  // RStartCfgSrvAuto
        I_rung_top = 0;
    }

    if (!U_RMaqCfgSrvParOK) {  // RMaqCfgSrvParOK
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0079 = 0;
    I_parThis_0079 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0079;
    if (I_oneShot_006c) {  // $oneShot_006c
        I_parThis_0079 = 0;
    }
    I_oneShot_006c = I_scratch;

    if (I_parThis_0079) {  // $parThis_0079
        U_RMaqCfgSrvOK = 0;
    }

    /* ] finish series */
    if (I_parThis_0079) {  // $parThis_0079
        I_parOut_0079 = 1;
    }
    I_parThis_0079 = I_rung_top;
    /* start series [ */
    if (I_parThis_0079) {  // $parThis_0079
        if (U_TCfgSrvAutoTO < 499) {
            U_TCfgSrvAutoTO++;
            I_parThis_0079 = 0;
        }
    } else {
        U_TCfgSrvAutoTO = 0;
    }

    if (I_parThis_0079) {  // $parThis_0079
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0079) {  // $parThis_0079
        I_parOut_0079 = 1;
    }
    I_parThis_0079 = I_rung_top;
    /* start series [ */
    if (I_parThis_0079) {  // $parThis_0079
        if (!I_oneShot_006d) {  // $oneShot_006d
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_006d = I_parThis_0079;
            }
            I_parThis_0079 = 0;
            I_oneShot_006e = I_parThis_0079;
        }
        if (!I_oneShot_006e) {  // $oneShot_006e
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_006e = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0079 = 0;
            }
        }
    } else {
        if (I_oneShot_006d) {  // $oneShot_006d
            I_SerialReady = 1;
        }
        I_oneShot_006d = I_parThis_0079;
    }

    if (I_parThis_0079) {  // $parThis_0079
        U_SrvRegConfig |= 1 << 2;
    }

    if (I_parThis_0079) {  // $parThis_0079
        if (!I_oneShot_006f) {  // $oneShot_006f
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_006f = I_parThis_0079;
            }
            I_parThis_0079 = 0;
            I_oneShot_0070 = I_parThis_0079;
        }
        if (!I_oneShot_0070) {  // $oneShot_0070
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0070 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0079 = 0;
            }
        }
    } else {
        if (I_oneShot_006f) {  // $oneShot_006f
            I_SerialReady = 1;
        }
        I_oneShot_006f = I_parThis_0079;
    }

    /* start parallel [ */
    I_parOut_007a = 0;
    I_parThis_007a = I_parThis_0079;
    if (I_parThis_007a) {  // $parThis_007a
        U_RStartCfgSrvAuto = 0;
    }

    if (I_parThis_007a) {  // $parThis_007a
        I_parOut_007a = 1;
    }
    I_parThis_007a = I_parThis_0079;
    if (I_parThis_007a) {  // $parThis_007a
        U_RMaqCfgSrvOK = 1;
    }

    if (I_parThis_007a) {  // $parThis_007a
        I_parOut_007a = 1;
    }
    I_parThis_0079 = I_parOut_007a;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0079) {  // $parThis_0079
        I_parOut_0079 = 1;
    }
    I_rung_top = I_parOut_0079;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 307 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgSrvMan) {  // RStartCfgSrvMan
        I_rung_top = 0;
    }

    if (!U_RMaqCfgSrvParOK) {  // RMaqCfgSrvParOK
        I_rung_top = 0;
    }

    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_007b = 0;
    I_parThis_007b = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_007b;
    if (I_oneShot_0071) {  // $oneShot_0071
        I_parThis_007b = 0;
    }
    I_oneShot_0071 = I_scratch;

    if (I_parThis_007b) {  // $parThis_007b
        U_RMaqCfgSrvOK = 0;
    }

    /* ] finish series */
    if (I_parThis_007b) {  // $parThis_007b
        I_parOut_007b = 1;
    }
    I_parThis_007b = I_rung_top;
    /* start series [ */
    if (I_parThis_007b) {  // $parThis_007b
        if (U_TCfgSrvManTO < 499) {
            U_TCfgSrvManTO++;
            I_parThis_007b = 0;
        }
    } else {
        U_TCfgSrvManTO = 0;
    }

    if (I_parThis_007b) {  // $parThis_007b
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_007b) {  // $parThis_007b
        I_parOut_007b = 1;
    }
    I_parThis_007b = I_rung_top;
    /* start series [ */
    if (I_parThis_007b) {  // $parThis_007b
        if (!I_oneShot_0072) {  // $oneShot_0072
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0072 = I_parThis_007b;
            }
            I_parThis_007b = 0;
            I_oneShot_0073 = I_parThis_007b;
        }
        if (!I_oneShot_0073) {  // $oneShot_0073
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0073 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_007b = 0;
            }
        }
    } else {
        if (I_oneShot_0072) {  // $oneShot_0072
            I_SerialReady = 1;
        }
        I_oneShot_0072 = I_parThis_007b;
    }

    if (I_parThis_007b) {  // $parThis_007b
        U_SrvRegConfig &= ~(1 << 2);
    }

    if (I_parThis_007b) {  // $parThis_007b
        if (!I_oneShot_0074) {  // $oneShot_0074
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_0074 = I_parThis_007b;
            }
            I_parThis_007b = 0;
            I_oneShot_0075 = I_parThis_007b;
        }
        if (!I_oneShot_0075) {  // $oneShot_0075
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0075 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_007b = 0;
            }
        }
    } else {
        if (I_oneShot_0074) {  // $oneShot_0074
            I_SerialReady = 1;
        }
        I_oneShot_0074 = I_parThis_007b;
    }

    /* start parallel [ */
    I_parOut_007c = 0;
    I_parThis_007c = I_parThis_007b;
    if (I_parThis_007c) {  // $parThis_007c
        U_RStartCfgSrvMan = 0;
    }

    if (I_parThis_007c) {  // $parThis_007c
        I_parOut_007c = 1;
    }
    I_parThis_007c = I_parThis_007b;
    if (I_parThis_007c) {  // $parThis_007c
        U_RMaqCfgSrvOK = 1;
    }

    if (I_parThis_007c) {  // $parThis_007c
        I_parOut_007c = 1;
    }
    I_parThis_007b = I_parOut_007c;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_007b) {  // $parThis_007b
        I_parOut_007b = 1;
    }
    I_rung_top = I_parOut_007b;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 309 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 5))) {  // YMbInvParamSync
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_007d = 0;
    I_parThis_007d = I_rung_top;
    if (I_parThis_007d) {  // $parThis_007d
        MODBUS_REGISTER[2] &= ~(1 << 5);  // YMbInvParamSync
    }

    if (I_parThis_007d) {  // $parThis_007d
        I_parOut_007d = 1;
    }
    I_parThis_007d = I_rung_top;
    if (I_parThis_007d) {  // $parThis_007d
        U_RStartCfgInvMan = 1;
    }

    if (I_parThis_007d) {  // $parThis_007d
        I_parOut_007d = 1;
    }
    I_rung_top = I_parOut_007d;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 310 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgInvAuto) {  // RStartCfgInvAuto
        I_rung_top = 0;
    }

    if (U_RMaqCfgInvErro) {  // RMaqCfgInvErro
        I_rung_top = 0;
    }

    if (!U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_007e = 0;
    I_parThis_007e = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_007e;
    if (I_oneShot_0076) {  // $oneShot_0076
        I_parThis_007e = 0;
    }
    I_oneShot_0076 = I_scratch;

    if (I_parThis_007e) {  // $parThis_007e
        U_RMaqCfgInvOK = 0;
    }

    /* ] finish series */
    if (I_parThis_007e) {  // $parThis_007e
        I_parOut_007e = 1;
    }
    I_parThis_007e = I_rung_top;
    /* start series [ */
    if (I_parThis_007e) {  // $parThis_007e
        if (U_TCfgInvAutoTO < 499) {
            U_TCfgInvAutoTO++;
            I_parThis_007e = 0;
        }
    } else {
        U_TCfgInvAutoTO = 0;
    }

    if (I_parThis_007e) {  // $parThis_007e
        U_RMaqCfgInvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_007e) {  // $parThis_007e
        I_parOut_007e = 1;
    }
    I_parThis_007e = I_rung_top;
    /* start series [ */
    if (I_parThis_007e) {  // $parThis_007e
        if (!I_oneShot_0077) {  // $oneShot_0077
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 105, 0, 0, &U_WInvAutoVel);
                I_oneShot_0077 = I_parThis_007e;
            }
            I_parThis_007e = 0;
            I_oneShot_0078 = I_parThis_007e;
        }
        if (!I_oneShot_0078) {  // $oneShot_0078
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0078 = 1;
            } else {
                I_parThis_007e = 0;
            }
        }
    } else {
        I_oneShot_0077 = I_parThis_007e;
    }

    if (I_parThis_007e) {  // $parThis_007e
        if (!I_oneShot_0079) {  // $oneShot_0079
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 104, 0, 0, &U_WInvAutoVel);
                I_oneShot_0079 = I_parThis_007e;
            }
            I_parThis_007e = 0;
            I_oneShot_007a = I_parThis_007e;
        }
        if (!I_oneShot_007a) {  // $oneShot_007a
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_007a = 1;
            } else {
                I_parThis_007e = 0;
            }
        }
    } else {
        I_oneShot_0079 = I_parThis_007e;
    }

    if (I_parThis_007e) {  // $parThis_007e
        if (!I_oneShot_007b) {  // $oneShot_007b
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 102, 0, 0, &U_WInvAutoAcel);
                I_oneShot_007b = I_parThis_007e;
            }
            I_parThis_007e = 0;
            I_oneShot_007c = I_parThis_007e;
        }
        if (!I_oneShot_007c) {  // $oneShot_007c
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_007c = 1;
            } else {
                I_parThis_007e = 0;
            }
        }
    } else {
        I_oneShot_007b = I_parThis_007e;
    }

    if (I_parThis_007e) {  // $parThis_007e
        if (!I_oneShot_007d) {  // $oneShot_007d
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 103, 0, 0, &U_WInvAutoDesac);
                I_oneShot_007d = I_parThis_007e;
            }
            I_parThis_007e = 0;
            I_oneShot_007e = I_parThis_007e;
        }
        if (!I_oneShot_007e) {  // $oneShot_007e
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_007e = 1;
            } else {
                I_parThis_007e = 0;
            }
        }
    } else {
        I_oneShot_007d = I_parThis_007e;
    }

    /* start parallel [ */
    I_parOut_007f = 0;
    I_parThis_007f = I_parThis_007e;
    if (I_parThis_007f) {  // $parThis_007f
        U_RStartCfgInvAuto = 0;
    }

    if (I_parThis_007f) {  // $parThis_007f
        I_parOut_007f = 1;
    }
    I_parThis_007f = I_parThis_007e;
    if (I_parThis_007f) {  // $parThis_007f
        U_RMaqCfgInvOK = 1;
    }

    if (I_parThis_007f) {  // $parThis_007f
        I_parOut_007f = 1;
    }
    I_parThis_007e = I_parOut_007f;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_007e) {  // $parThis_007e
        I_parOut_007e = 1;
    }
    I_rung_top = I_parOut_007e;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 311 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgInvMan) {  // RStartCfgInvMan
        I_rung_top = 0;
    }

    if (U_RMaqCfgInvErro) {  // RMaqCfgInvErro
        I_rung_top = 0;
    }

    if (!U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0080 = 0;
    I_parThis_0080 = I_rung_top;
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_parThis_0080 = 0;
    }

    if (I_parThis_0080) {  // $parThis_0080
        I_parOut_0080 = 1;
    }
    I_parThis_0080 = I_rung_top;
    if (!U_RMaqModoCorrecao) {  // RMaqModoCorrecao
        I_parThis_0080 = 0;
    }

    if (I_parThis_0080) {  // $parThis_0080
        I_parOut_0080 = 1;
    }
    I_rung_top = I_parOut_0080;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0081 = 0;
    I_parThis_0081 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0081;
    if (I_oneShot_007f) {  // $oneShot_007f
        I_parThis_0081 = 0;
    }
    I_oneShot_007f = I_scratch;

    if (I_parThis_0081) {  // $parThis_0081
        U_RMaqCfgInvOK = 0;
    }

    /* ] finish series */
    if (I_parThis_0081) {  // $parThis_0081
        I_parOut_0081 = 1;
    }
    I_parThis_0081 = I_rung_top;
    /* start series [ */
    if (I_parThis_0081) {  // $parThis_0081
        if (U_TCfgInvManTO < 499) {
            U_TCfgInvManTO++;
            I_parThis_0081 = 0;
        }
    } else {
        U_TCfgInvManTO = 0;
    }

    if (I_parThis_0081) {  // $parThis_0081
        U_RMaqCfgInvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0081) {  // $parThis_0081
        I_parOut_0081 = 1;
    }
    I_parThis_0081 = I_rung_top;
    /* start series [ */
    if (I_parThis_0081) {  // $parThis_0081
        if (!I_oneShot_0080) {  // $oneShot_0080
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 105, 0, 0, &U_WInvManVel);
                I_oneShot_0080 = I_parThis_0081;
            }
            I_parThis_0081 = 0;
            I_oneShot_0081 = I_parThis_0081;
        }
        if (!I_oneShot_0081) {  // $oneShot_0081
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0081 = 1;
            } else {
                I_parThis_0081 = 0;
            }
        }
    } else {
        I_oneShot_0080 = I_parThis_0081;
    }

    if (I_parThis_0081) {  // $parThis_0081
        if (!I_oneShot_0082) {  // $oneShot_0082
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 104, 0, 0, &U_WInvManVel);
                I_oneShot_0082 = I_parThis_0081;
            }
            I_parThis_0081 = 0;
            I_oneShot_0083 = I_parThis_0081;
        }
        if (!I_oneShot_0083) {  // $oneShot_0083
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0083 = 1;
            } else {
                I_parThis_0081 = 0;
            }
        }
    } else {
        I_oneShot_0082 = I_parThis_0081;
    }

    if (I_parThis_0081) {  // $parThis_0081
        if (!I_oneShot_0084) {  // $oneShot_0084
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 102, 0, 0, &U_WInvManAcel);
                I_oneShot_0084 = I_parThis_0081;
            }
            I_parThis_0081 = 0;
            I_oneShot_0085 = I_parThis_0081;
        }
        if (!I_oneShot_0085) {  // $oneShot_0085
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0085 = 1;
            } else {
                I_parThis_0081 = 0;
            }
        }
    } else {
        I_oneShot_0084 = I_parThis_0081;
    }

    if (I_parThis_0081) {  // $parThis_0081
        if (!I_oneShot_0086) {  // $oneShot_0086
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 103, 0, 0, &U_WInvManDesac);
                I_oneShot_0086 = I_parThis_0081;
            }
            I_parThis_0081 = 0;
            I_oneShot_0087 = I_parThis_0081;
        }
        if (!I_oneShot_0087) {  // $oneShot_0087
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0087 = 1;
            } else {
                I_parThis_0081 = 0;
            }
        }
    } else {
        I_oneShot_0086 = I_parThis_0081;
    }

    /* start parallel [ */
    I_parOut_0082 = 0;
    I_parThis_0082 = I_parThis_0081;
    if (I_parThis_0082) {  // $parThis_0082
        U_RStartCfgInvMan = 0;
    }

    if (I_parThis_0082) {  // $parThis_0082
        I_parOut_0082 = 1;
    }
    I_parThis_0082 = I_parThis_0081;
    if (I_parThis_0082) {  // $parThis_0082
        U_RMaqCfgInvOK = 1;
    }

    if (I_parThis_0082) {  // $parThis_0082
        I_parOut_0082 = 1;
    }
    I_parThis_0081 = I_parOut_0082;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0081) {  // $parThis_0081
        I_parOut_0081 = 1;
    }
    I_rung_top = I_parOut_0081;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 313 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqVelReduzida) {  // RMaqVelReduzida
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0083 = 0;
    I_parThis_0083 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0083;
    if (I_oneShot_0088) {  // $oneShot_0088
        I_parThis_0083 = 0;
    }
    I_oneShot_0088 = I_scratch;

    /* start parallel [ */
    I_parOut_0084 = 0;
    I_parThis_0084 = I_parThis_0083;
    if (I_parThis_0084) {  // $parThis_0084
        U_RStartCfgVelRed = 1;
    }

    if (I_parThis_0084) {  // $parThis_0084
        I_parOut_0084 = 1;
    }
    I_parThis_0084 = I_parThis_0083;
    if (I_parThis_0084) {  // $parThis_0084
        U_RStartCfgVelMax = 0;
    }

    if (I_parThis_0084) {  // $parThis_0084
        I_parOut_0084 = 1;
    }
    I_parThis_0083 = I_parOut_0084;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0083) {  // $parThis_0083
        I_parOut_0083 = 1;
    }
    I_parThis_0083 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0083;
    if (!I_parThis_0083) {  // $parThis_0083
        if (I_oneShot_0089) {  // $oneShot_0089
            I_parThis_0083 = 1;
        }
    } else {
        I_parThis_0083 = 0;
    }
    I_oneShot_0089 = I_scratch;

    /* start parallel [ */
    I_parOut_0085 = 0;
    I_parThis_0085 = I_parThis_0083;
    if (I_parThis_0085) {  // $parThis_0085
        U_RStartCfgVelMax = 1;
    }

    if (I_parThis_0085) {  // $parThis_0085
        I_parOut_0085 = 1;
    }
    I_parThis_0085 = I_parThis_0083;
    if (I_parThis_0085) {  // $parThis_0085
        U_RStartCfgVelRed = 0;
    }

    if (I_parThis_0085) {  // $parThis_0085
        I_parOut_0085 = 1;
    }
    I_parThis_0083 = I_parOut_0085;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0083) {  // $parThis_0083
        I_parOut_0083 = 1;
    }
    I_rung_top = I_parOut_0083;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 314 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgVelMax) {  // RStartCfgVelMax
        I_rung_top = 0;
    }

    if (U_RMaqCfgInvErro) {  // RMaqCfgInvErro
        I_rung_top = 0;
    }

    if (!U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0086 = 0;
    I_parThis_0086 = I_rung_top;
    /* start series [ */
    if (I_parThis_0086) {  // $parThis_0086
        if (U_TCfgVelMaxTO < 299) {
            U_TCfgVelMaxTO++;
            I_parThis_0086 = 0;
        }
    } else {
        U_TCfgVelMaxTO = 0;
    }

    if (I_parThis_0086) {  // $parThis_0086
        U_RMaqCfgInvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0086) {  // $parThis_0086
        I_parOut_0086 = 1;
    }
    I_parThis_0086 = I_rung_top;
    /* start series [ */
    if (I_parThis_0086) {  // $parThis_0086
        if (!I_oneShot_008a) {  // $oneShot_008a
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 105, 0, 0, &U_WInvAutoVel);
                I_oneShot_008a = I_parThis_0086;
            }
            I_parThis_0086 = 0;
            I_oneShot_008b = I_parThis_0086;
        }
        if (!I_oneShot_008b) {  // $oneShot_008b
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_008b = 1;
            } else {
                I_parThis_0086 = 0;
            }
        }
    } else {
        I_oneShot_008a = I_parThis_0086;
    }

    if (I_parThis_0086) {  // $parThis_0086
        if (!I_oneShot_008c) {  // $oneShot_008c
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 104, 0, 0, &U_WInvAutoVel);
                I_oneShot_008c = I_parThis_0086;
            }
            I_parThis_0086 = 0;
            I_oneShot_008d = I_parThis_0086;
        }
        if (!I_oneShot_008d) {  // $oneShot_008d
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_008d = 1;
            } else {
                I_parThis_0086 = 0;
            }
        }
    } else {
        I_oneShot_008c = I_parThis_0086;
    }

    if (I_parThis_0086) {  // $parThis_0086
        U_RStartCfgVelMax = 0;
    }

    /* ] finish series */
    if (I_parThis_0086) {  // $parThis_0086
        I_parOut_0086 = 1;
    }
    I_rung_top = I_parOut_0086;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 315 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartCfgVelRed) {  // RStartCfgVelRed
        I_rung_top = 0;
    }

    if (U_RMaqCfgInvErro) {  // RMaqCfgInvErro
        I_rung_top = 0;
    }

    if (!U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0087 = 0;
    I_parThis_0087 = I_rung_top;
    /* start series [ */
    if (I_parThis_0087) {  // $parThis_0087
        if (U_TCfgVelRedTO < 299) {
            U_TCfgVelRedTO++;
            I_parThis_0087 = 0;
        }
    } else {
        U_TCfgVelRedTO = 0;
    }

    if (I_parThis_0087) {  // $parThis_0087
        U_RMaqCfgInvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0087) {  // $parThis_0087
        I_parOut_0087 = 1;
    }
    I_parThis_0087 = I_rung_top;
    /* start series [ */
    if (I_parThis_0087) {  // $parThis_0087
        if (!I_oneShot_008e) {  // $oneShot_008e
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 105, 0, 0, &U_WInvMaxSyncVel);
                I_oneShot_008e = I_parThis_0087;
            }
            I_parThis_0087 = 0;
            I_oneShot_008f = I_parThis_0087;
        }
        if (!I_oneShot_008f) {  // $oneShot_008f
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_008f = 1;
            } else {
                I_parThis_0087 = 0;
            }
        }
    } else {
        I_oneShot_008e = I_parThis_0087;
    }

    if (I_parThis_0087) {  // $parThis_0087
        if (!I_oneShot_0090) {  // $oneShot_0090
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(0, 104, 0, 0, &U_WInvMaxSyncVel);
                I_oneShot_0090 = I_parThis_0087;
            }
            I_parThis_0087 = 0;
            I_oneShot_0091 = I_parThis_0087;
        }
        if (!I_oneShot_0091) {  // $oneShot_0091
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0091 = 1;
            } else {
                I_parThis_0087 = 0;
            }
        }
    } else {
        I_oneShot_0090 = I_parThis_0087;
    }

    if (I_parThis_0087) {  // $parThis_0087
        U_RStartCfgVelRed = 0;
    }

    /* ] finish series */
    if (I_parThis_0087) {  // $parThis_0087
        I_parOut_0087 = 1;
    }
    I_rung_top = I_parOut_0087;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 317 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RModoAuto) {  // RModoAuto
        I_rung_top = 0;
    }

    U_RModoAutoOK = I_rung_top;

    /* ] finish series */

    /* start rung 318 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0088 = 0;
    I_parThis_0088 = I_rung_top;
    /* start series [ */
    if (!U_RModoAuto) {  // RModoAuto
        I_parThis_0088 = 0;
    }

    I_scratch = I_parThis_0088;
    if (!I_parThis_0088) {  // $parThis_0088
        if (I_oneShot_0092) {  // $oneShot_0092
            I_parThis_0088 = 1;
        }
    } else {
        I_parThis_0088 = 0;
    }
    I_oneShot_0092 = I_scratch;

    /* ] finish series */
    if (I_parThis_0088) {  // $parThis_0088
        I_parOut_0088 = 1;
    }
    I_parThis_0088 = I_rung_top;
    /* start series [ */
    if (I_parThis_0088) {  // $parThis_0088
        if (U_TEspSistemaOK < 999) {
            U_TEspSistemaOK++;
            I_parThis_0088 = 0;
        }
    } else {
        U_TEspSistemaOK = 0;
    }

    I_scratch = I_parThis_0088;
    if (I_oneShot_0093) {  // $oneShot_0093
        I_parThis_0088 = 0;
    }
    I_oneShot_0093 = I_scratch;

    /* ] finish series */
    if (I_parThis_0088) {  // $parThis_0088
        I_parOut_0088 = 1;
    }
    I_rung_top = I_parOut_0088;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0089 = 0;
    I_parThis_0089 = I_rung_top;
    if (I_parThis_0089) {  // $parThis_0089
        U_RStartCfgInvMan = 1;
    }

    if (I_parThis_0089) {  // $parThis_0089
        I_parOut_0089 = 1;
    }
    I_parThis_0089 = I_rung_top;
    if (I_parThis_0089) {  // $parThis_0089
        U_RStartCfgInvAuto = 0;
    }

    if (I_parThis_0089) {  // $parThis_0089
        I_parOut_0089 = 1;
    }
    I_parThis_0089 = I_rung_top;
    if (I_parThis_0089) {  // $parThis_0089
        U_RStartCfgSrvMan = 1;
    }

    if (I_parThis_0089) {  // $parThis_0089
        I_parOut_0089 = 1;
    }
    I_parThis_0089 = I_rung_top;
    if (I_parThis_0089) {  // $parThis_0089
        U_RStartCfgSrvAuto = 0;
    }

    if (I_parThis_0089) {  // $parThis_0089
        I_parOut_0089 = 1;
    }
    I_rung_top = I_parOut_0089;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 319 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RModoAuto) {  // RModoAuto
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0094) {  // $oneShot_0094
        I_rung_top = 0;
    }
    I_oneShot_0094 = I_scratch;

    if (U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }

    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RMaqFimProd) {  // RMaqFimProd
        I_rung_top = 0;
    }

    if (U_RMaqErroPosic) {  // RMaqErroPosic
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_008a = 0;
    I_parThis_008a = I_rung_top;
    if (I_parThis_008a) {  // $parThis_008a
        U_RStartCfgInvAuto = 1;
    }

    if (I_parThis_008a) {  // $parThis_008a
        I_parOut_008a = 1;
    }
    I_parThis_008a = I_rung_top;
    if (I_parThis_008a) {  // $parThis_008a
        U_RStartCfgInvMan = 0;
    }

    if (I_parThis_008a) {  // $parThis_008a
        I_parOut_008a = 1;
    }
    I_parThis_008a = I_rung_top;
    if (I_parThis_008a) {  // $parThis_008a
        U_RStartCfgSrvAuto = 1;
    }

    if (I_parThis_008a) {  // $parThis_008a
        I_parOut_008a = 1;
    }
    I_parThis_008a = I_rung_top;
    if (I_parThis_008a) {  // $parThis_008a
        U_RStartCfgSrvMan = 0;
    }

    if (I_parThis_008a) {  // $parThis_008a
        I_parOut_008a = 1;
    }
    I_parThis_008a = I_rung_top;
    if (I_parThis_008a) {  // $parThis_008a
        U_CQtdProd = MODBUS_REGISTER[30];
    }

    if (I_parThis_008a) {  // $parThis_008a
        I_parOut_008a = 1;
    }
    I_parThis_008a = I_rung_top;
    if (I_parThis_008a) {  // $parThis_008a
        U_RMaqModoEstatico = 0;
    }

    if (I_parThis_008a) {  // $parThis_008a
        I_parOut_008a = 1;
    }
    I_parThis_008a = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_008b = 0;
    I_parThis_008b = I_parThis_008a;
    if (U_MaqPecaTamMin > U_MaqPecaTam) {
    } else {
        I_parThis_008b = 0;
    }

    if (I_parThis_008b) {  // $parThis_008b
        I_parOut_008b = 1;
    }
    I_parThis_008b = I_parThis_008a;
    if (GPIO_INPUT_PORT18) {  // XSnsFimMaterial
        I_parThis_008b = 0;
    }

    if (I_parThis_008b) {  // $parThis_008b
        I_parOut_008b = 1;
    }
    I_parThis_008a = I_parOut_008b;
    /* ] finish parallel */
    if (I_parThis_008a) {  // $parThis_008a
        U_RMaqModoEstatico = 1;
    }

    /* ] finish series */
    if (I_parThis_008a) {  // $parThis_008a
        I_parOut_008a = 1;
    }
    I_rung_top = I_parOut_008a;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 320 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_008c = 0;
    I_parThis_008c = I_rung_top;
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_008c = 0;
    }

    if (I_parThis_008c) {  // $parThis_008c
        I_parOut_008c = 1;
    }
    I_parThis_008c = I_rung_top;
    if (!U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_parThis_008c = 0;
    }

    if (I_parThis_008c) {  // $parThis_008c
        I_parOut_008c = 1;
    }
    I_parThis_008c = I_rung_top;
    if (!U_RMaqCfgInvErro) {  // RMaqCfgInvErro
        I_parThis_008c = 0;
    }

    if (I_parThis_008c) {  // $parThis_008c
        I_parOut_008c = 1;
    }
    I_parThis_008c = I_rung_top;
    /* start series [ */
    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_008c = 0;
    }

    if (GPIO_INPUT_PORT18) {  // XSnsFimMaterial
        I_parThis_008c = 0;
    }

    if (I_parThis_008c) {  // $parThis_008c
        if (U_TEspFimMaterial < 49) {
            U_TEspFimMaterial++;
            I_parThis_008c = 0;
        }
    } else {
        U_TEspFimMaterial = 0;
    }

    /* ] finish series */
    if (I_parThis_008c) {  // $parThis_008c
        I_parOut_008c = 1;
    }
    I_parThis_008c = I_rung_top;
    /* start series [ */
    if (!U_RMaqErroPosic) {  // RMaqErroPosic
        I_parThis_008c = 0;
    }

    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_parThis_008c = 0;
    }

    /* ] finish series */
    if (I_parThis_008c) {  // $parThis_008c
        I_parOut_008c = 1;
    }
    I_parThis_008c = I_rung_top;
    /* start series [ */
    if (!U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_008c = 0;
    }

    I_scratch = I_parThis_008c;
    if (I_oneShot_0095) {  // $oneShot_0095
        I_parThis_008c = 0;
    }
    I_oneShot_0095 = I_scratch;

    if (U_RErroCortePiloto) {  // RErroCortePiloto
        I_parThis_008c = 0;
    }

    if (GPIO_INPUT_PORT16) {  // XServoProxFim
        I_parThis_008c = 0;
    }

    /* ] finish series */
    if (I_parThis_008c) {  // $parThis_008c
        I_parOut_008c = 1;
    }
    I_rung_top = I_parOut_008c;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_008d = 0;
    I_parThis_008d = I_rung_top;
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_008d = 0;
    }

    if (I_parThis_008d) {  // $parThis_008d
        I_parOut_008d = 1;
    }
    I_parThis_008d = I_rung_top;
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_parThis_008d = 0;
    }

    if (I_parThis_008d) {  // $parThis_008d
        I_parOut_008d = 1;
    }
    I_parThis_008d = I_rung_top;
    if (!U_RMaqFimProd) {  // RMaqFimProd
        I_parThis_008d = 0;
    }

    if (I_parThis_008d) {  // $parThis_008d
        I_parOut_008d = 1;
    }
    I_rung_top = I_parOut_008d;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_008e = 0;
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        U_RMaqProduzindo = 0;
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        U_RMaqStartProd = 0;
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        U_RMaqFimProd = 0;
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        U_RMaqCorteEstat = 0;
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        U_RMaqStartPosic = 0;
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        GPIO_OUTPUT_PORT7 = 0;
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        U_RMaqCortePerf = 0;
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_parThis_008e = I_rung_top;
    if (I_parThis_008e) {  // $parThis_008e
        U_RMaqModoEstatico = 0;
    }

    if (I_parThis_008e) {  // $parThis_008e
        I_parOut_008e = 1;
    }
    I_rung_top = I_parOut_008e;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 321 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_RModoAuto) {  // RModoAuto
        I_rung_top = 0;
    }

    if (U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }

    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RMaqFimProd) {  // RMaqFimProd
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YMbModoAutoAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }

    /* ] finish series */

    /* start rung 322 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0096) {  // $oneShot_0096
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0096 = I_scratch;

    /* start parallel [ */
    I_parOut_008f = 0;
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RMaqCortePerf = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    /* start series [ */
    if (U_RMaqFimProd) {  // RMaqFimProd
        I_parThis_008f = 0;
    }

    /* start parallel [ */
    I_parOut_0090 = 0;
    I_parThis_0090 = I_parThis_008f;
    if (I_parThis_0090) {  // $parThis_0090
        U_RInvAtivar = 0;
    }

    if (I_parThis_0090) {  // $parThis_0090
        I_parOut_0090 = 1;
    }
    I_parThis_0090 = I_parThis_008f;
    if (I_parThis_0090) {  // $parThis_0090
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }

    if (I_parThis_0090) {  // $parThis_0090
        I_parOut_0090 = 1;
    }
    I_parThis_008f = I_parOut_0090;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RModoDinam = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RMaqModoAjuste = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RMaqModoCorrecao = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RMaqCorteEstat = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RMaqStartPosic = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RMaqVelReduzida = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RMaqModoEstatico = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RSubQtdProd = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RMaqPosCorteSet = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RStartCfgInvAuto = 0;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_parThis_008f = I_rung_top;
    if (I_parThis_008f) {  // $parThis_008f
        U_RStartCfgInvMan = 1;
    }

    if (I_parThis_008f) {  // $parThis_008f
        I_parOut_008f = 1;
    }
    I_rung_top = I_parOut_008f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 324 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqConfigOK) {  // RMaqConfigOK
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0097) {  // $oneShot_0097
        I_rung_top = 0;
    }
    I_oneShot_0097 = I_scratch;

    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (!U_RModoAutoOK) {  // RModoAutoOK
        I_rung_top = 0;
    }

    I_scratch2 = 0;
    if (U_CQtdProd > I_scratch2) {
    } else {
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqStartProd = 1;
    }

    /* ] finish series */

    /* start rung 325 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }

    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0098) {  // $oneShot_0098
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0098 = I_scratch;

    if (U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0091 = 0;
    I_parThis_0091 = I_rung_top;
    /* start series [ */
    if (U_DifPosCorteInv > U_SrvMesaCurso) {
    } else {
        I_parThis_0091 = 0;
    }

    if (I_parThis_0091) {  // $parThis_0091
        U_RMaqErroPosic = 1;
    }

    /* ] finish series */
    if (I_parThis_0091) {  // $parThis_0091
        I_parOut_0091 = 1;
    }
    I_parThis_0091 = I_rung_top;
    /* start series [ */
    I_scratch2 = 0;
    if (U_DifPosCorte > I_scratch2) {
        I_parThis_0091 = 0;
    }

    if (U_RMaqErroPosic) {  // RMaqErroPosic
        I_parThis_0091 = 0;
    }

    if (I_parThis_0091) {  // $parThis_0091
        U_RMaqCorteEstat = 1;
    }

    /* ] finish series */
    if (I_parThis_0091) {  // $parThis_0091
        I_parOut_0091 = 1;
    }
    I_parThis_0091 = I_rung_top;
    /* start series [ */
    I_scratch2 = 0;
    if (U_DifPosCorte > I_scratch2) {
    } else {
        I_parThis_0091 = 0;
    }

    if (I_parThis_0091) {  // $parThis_0091
        U_RMaqStartProd = 0;
    }

    /* ] finish series */
    if (I_parThis_0091) {  // $parThis_0091
        I_parOut_0091 = 1;
    }
    I_rung_top = I_parOut_0091;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 326 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0099) {  // $oneShot_0099
        I_rung_top = 0;
    }
    I_oneShot_0099 = I_scratch;

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0092 = 0;
    I_parThis_0092 = I_rung_top;
    if (I_parThis_0092) {  // $parThis_0092
        U_VSrvPosMesa = 0;
    }

    if (I_parThis_0092) {  // $parThis_0092
        I_parOut_0092 = 1;
    }
    I_parThis_0092 = I_rung_top;
    if (I_parThis_0092) {  // $parThis_0092
        U_RMaqStartPosic = 1;
    }

    if (I_parThis_0092) {  // $parThis_0092
        I_parOut_0092 = 1;
    }
    I_rung_top = I_parOut_0092;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 327 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_009a) {  // $oneShot_009a
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_009a = I_scratch;

    /* start parallel [ */
    I_parOut_0093 = 0;
    I_parThis_0093 = I_rung_top;
    if (I_parThis_0093) {  // $parThis_0093
        U_RMaqStartProd = 0;
    }

    if (I_parThis_0093) {  // $parThis_0093
        I_parOut_0093 = 1;
    }
    I_parThis_0093 = I_rung_top;
    /* start series [ */
    if (U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_0093 = 0;
    }

    if (I_parThis_0093) {  // $parThis_0093
        U_RSubQtdProd = 1;
    }

    /* ] finish series */
    if (I_parThis_0093) {  // $parThis_0093
        I_parOut_0093 = 1;
    }
    I_rung_top = I_parOut_0093;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 328 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_009b) {  // $oneShot_009b
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_009b = I_scratch;

    /* start parallel [ */
    I_parOut_0094 = 0;
    I_parThis_0094 = I_rung_top;
    /* start series [ */
    if (!U_RModoAutoOK) {  // RModoAutoOK
        I_parThis_0094 = 0;
    }

    if (U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_0094 = 0;
    }

    /* start parallel [ */
    I_parOut_0095 = 0;
    I_parThis_0095 = I_parThis_0094;
    if (I_parThis_0095) {  // $parThis_0095
        U_RMaqProduzindo = 1;
    }

    if (I_parThis_0095) {  // $parThis_0095
        I_parOut_0095 = 1;
    }
    I_parThis_0095 = I_parThis_0094;
    /* start series [ */
    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_0095 = 0;
    }

    if (I_parThis_0095) {  // $parThis_0095
        U_RInvAtivar = 1;
    }

    /* ] finish series */
    if (I_parThis_0095) {  // $parThis_0095
        I_parOut_0095 = 1;
    }
    I_parThis_0094 = I_parOut_0095;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0094) {  // $parThis_0094
        I_parOut_0094 = 1;
    }
    I_parThis_0094 = I_rung_top;
    /* start series [ */
    if (U_RModoAutoOK) {  // RModoAutoOK
        I_parThis_0094 = 0;
    }

    if (I_parThis_0094) {  // $parThis_0094
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }

    /* ] finish series */
    if (I_parThis_0094) {  // $parThis_0094
        I_parOut_0094 = 1;
    }
    I_rung_top = I_parOut_0094;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 329 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RModoDinam) {  // RModoDinam
        I_rung_top = 0;
    }

    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0096 = 0;
    I_parThis_0096 = I_rung_top;
    /* start series [ */
    if (I_parThis_0096) {  // $parThis_0096
        if (U_TModoDinamTO < 499) {
            U_TModoDinamTO++;
            I_parThis_0096 = 0;
        }
    } else {
        U_TModoDinamTO = 0;
    }

    if (I_parThis_0096) {  // $parThis_0096
        U_RMaqCfgSrvErro = 1;
    }

    /* ] finish series */
    if (I_parThis_0096) {  // $parThis_0096
        I_parOut_0096 = 1;
    }
    I_parThis_0096 = I_rung_top;
    /* start series [ */
    if (I_parThis_0096) {  // $parThis_0096
        if (!I_oneShot_009c) {  // $oneShot_009c
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_009c = I_parThis_0096;
            }
            I_parThis_0096 = 0;
            I_oneShot_009d = I_parThis_0096;
        }
        if (!I_oneShot_009d) {  // $oneShot_009d
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_009d = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0096 = 0;
            }
        }
    } else {
        if (I_oneShot_009c) {  // $oneShot_009c
            I_SerialReady = 1;
        }
        I_oneShot_009c = I_parThis_0096;
    }

    if (I_parThis_0096) {  // $parThis_0096
        U_SrvRegConfig |= 1 << 2;
    }

    if (I_parThis_0096) {  // $parThis_0096
        if (!I_oneShot_009e) {  // $oneShot_009e
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_oneShot_009e = I_parThis_0096;
            }
            I_parThis_0096 = 0;
            I_oneShot_009f = I_parThis_0096;
        }
        if (!I_oneShot_009f) {  // $oneShot_009f
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_009f = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 15030, 1, &U_SrvRegConfig);
                I_SerialTimeout = 0;
            } else {
                I_parThis_0096 = 0;
            }
        }
    } else {
        if (I_oneShot_009e) {  // $oneShot_009e
            I_SerialReady = 1;
        }
        I_oneShot_009e = I_parThis_0096;
    }

    if (I_parThis_0096) {  // $parThis_0096
        U_RModoDinam = 1;
    }

    /* ] finish series */
    if (I_parThis_0096) {  // $parThis_0096
        I_parOut_0096 = 1;
    }
    I_rung_top = I_parOut_0096;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 330 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 3;
        U_ProxFimCurso = U_SrvMesaCurso * I_scratch2;
    }

    /* ] finish series */

    /* start rung 331 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 4;
        if(I_scratch2) U_ProxFimCurso = U_ProxFimCurso / I_scratch2;
    }

    /* ] finish series */

    /* start rung 333 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (U_MaqPosReduzVel > U_DifPosCorte) {
    } else {
        I_rung_top = 0;
    }

    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT7) {  // YSrvEscravoAtivo
        I_rung_top = 0;
    }

    if (U_YMbAutoVel > U_MaxInvSyncVel) {
    } else {
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqVelReduzida = 1;
    }

    /* ] finish series */

    /* start rung 334 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (U_DifPosCorte > MODBUS_REGISTER[11]) {
        I_rung_top = 0;
    }

    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT7) {  // YSrvEscravoAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0097 = 0;
    I_parThis_0097 = I_rung_top;
    if (I_parThis_0097) {  // $parThis_0097
        GPIO_OUTPUT_PORT7 = 1;
    }

    if (I_parThis_0097) {  // $parThis_0097
        I_parOut_0097 = 1;
    }
    I_parThis_0097 = I_rung_top;
    if (I_parThis_0097) {  // $parThis_0097
        U_RCorteRealizado = 0;
    }

    if (I_parThis_0097) {  // $parThis_0097
        I_parOut_0097 = 1;
    }
    I_rung_top = I_parOut_0097;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 335 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (!GPIO_OUTPUT_PORT7) {  // YSrvEscravoAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TSrvEsperaAcel < 49) {
            U_TSrvEsperaAcel++;
            I_rung_top = 0;
        }
    } else {
        U_TSrvEsperaAcel = 0;
    }

    if (!GPIO_INPUT_PORT17) {  // XSrvMesaSync
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_00a0) {  // $oneShot_00a0
        I_rung_top = 0;
    }
    I_oneShot_00a0 = I_scratch;

    if (U_RMaqSincronizado) {  // RMaqSincronizado
        I_rung_top = 0;
    }

    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0098 = 0;
    I_parThis_0098 = I_rung_top;
    if (I_parThis_0098) {  // $parThis_0098
        U_RMaqCortePerf = 1;
    }

    if (I_parThis_0098) {  // $parThis_0098
        I_parOut_0098 = 1;
    }
    I_parThis_0098 = I_rung_top;
    if (I_parThis_0098) {  // $parThis_0098
        U_RMaqSincronizado = 1;
    }

    if (I_parThis_0098) {  // $parThis_0098
        I_parOut_0098 = 1;
    }
    I_rung_top = I_parOut_0098;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 336 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0099 = 0;
    I_parThis_0099 = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT16) {  // XServoProxFim
        I_parThis_0099 = 0;
    }

    if (U_RCorteRealizado) {  // RCorteRealizado
        I_parThis_0099 = 0;
    }

    /* ] finish series */
    if (I_parThis_0099) {  // $parThis_0099
        I_parOut_0099 = 1;
    }
    I_parThis_0099 = I_rung_top;
    if (U_DifPosCorteInv > U_ProxFimCurso) {
    } else {
        I_parThis_0099 = 0;
    }

    if (I_parThis_0099) {  // $parThis_0099
        I_parOut_0099 = 1;
    }
    I_rung_top = I_parOut_0099;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_009a = 0;
    I_parThis_009a = I_rung_top;
    if (I_parThis_009a) {  // $parThis_009a
        U_RMaqModoAjuste = 1;
    }

    if (I_parThis_009a) {  // $parThis_009a
        I_parOut_009a = 1;
    }
    I_parThis_009a = I_rung_top;
    if (I_parThis_009a) {  // $parThis_009a
        GPIO_OUTPUT_PORT7 = 0;
    }

    if (I_parThis_009a) {  // $parThis_009a
        I_parOut_009a = 1;
    }
    I_parThis_009a = I_rung_top;
    if (I_parThis_009a) {  // $parThis_009a
        U_RMaqVelReduzida = 0;
    }

    if (I_parThis_009a) {  // $parThis_009a
        I_parOut_009a = 1;
    }
    I_rung_top = I_parOut_009a;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 337 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    if (!U_RMaqSincronizado) {  // RMaqSincronizado
        I_rung_top = 0;
    }

    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_009b = 0;
    I_parThis_009b = I_rung_top;
    if (I_parThis_009b) {  // $parThis_009b
        U_RMaqCortePerf = 0;
    }

    if (I_parThis_009b) {  // $parThis_009b
        I_parOut_009b = 1;
    }
    I_parThis_009b = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_009b;
    if (!I_parThis_009b) {  // $parThis_009b
        if (I_oneShot_00a1) {  // $oneShot_00a1
            I_parThis_009b = 1;
        }
    } else {
        I_parThis_009b = 0;
    }
    I_oneShot_00a1 = I_scratch;

    /* start parallel [ */
    I_parOut_009c = 0;
    I_parThis_009c = I_parThis_009b;
    if (I_parThis_009c) {  // $parThis_009c
        GPIO_OUTPUT_PORT7 = 0;
    }

    if (I_parThis_009c) {  // $parThis_009c
        I_parOut_009c = 1;
    }
    I_parThis_009c = I_parThis_009b;
    if (I_parThis_009c) {  // $parThis_009c
        U_RMaqVelReduzida = 0;
    }

    if (I_parThis_009c) {  // $parThis_009c
        I_parOut_009c = 1;
    }
    I_parThis_009c = I_parThis_009b;
    /* start series [ */
    /* start parallel [ */
    I_parOut_009d = 0;
    I_parThis_009d = I_parThis_009c;
    if (!U_RErroCortePiloto) {  // RErroCortePiloto
        I_parThis_009d = 0;
    }

    if (I_parThis_009d) {  // $parThis_009d
        I_parOut_009d = 1;
    }
    I_parThis_009d = I_parThis_009c;
    /* start series [ */
    if (!U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_009d = 0;
    }

    if (!GPIO_INPUT_PORT16) {  // XServoProxFim
        I_parThis_009d = 0;
    }

    /* ] finish series */
    if (I_parThis_009d) {  // $parThis_009d
        I_parOut_009d = 1;
    }
    I_parThis_009c = I_parOut_009d;
    /* ] finish parallel */
    if (I_parThis_009c) {  // $parThis_009c
        U_RMaqModoAjuste = 1;
    }

    /* ] finish series */
    if (I_parThis_009c) {  // $parThis_009c
        I_parOut_009c = 1;
    }
    I_parThis_009b = I_parOut_009c;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_009b) {  // $parThis_009b
        I_parOut_009b = 1;
    }
    I_rung_top = I_parOut_009b;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 339 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_SrvMesaCurso > U_MaqPecaTam) {
    } else {
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_PosLimiteCorte = U_ProxFimCurso;
    }

    /* ] finish series */

    /* start rung 340 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_SrvMesaCurso > U_MaqPecaTam) {
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_PosLimiteCorte = 0;
    }

    /* ] finish series */

    /* start rung 341 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (U_PosLimiteCorte > U_DifPosCorteInv) {
    } else {
        I_rung_top = 0;
    }

    if (U_RMaqCortando) {  // RMaqCortando
        I_rung_top = 0;
    }

    if (U_RInvAtivar) {  // RInvAtivar
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RInvAtivar = 1;
    }

    /* ] finish series */

    /* start rung 342 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (U_PosLimiteCorte > U_DifPosCorteInv) {
        I_rung_top = 0;
    }

    if (U_RMaqCortando) {  // RMaqCortando
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_009e = 0;
    I_parThis_009e = I_rung_top;
    if (I_parThis_009e) {  // $parThis_009e
        U_RMaqCortando = 1;
    }

    if (I_parThis_009e) {  // $parThis_009e
        I_parOut_009e = 1;
    }
    I_parThis_009e = I_rung_top;
    if (I_parThis_009e) {  // $parThis_009e
        U_RInvAtivar = 0;
    }

    if (I_parThis_009e) {  // $parThis_009e
        I_parOut_009e = 1;
    }
    I_parThis_009e = I_rung_top;
    if (I_parThis_009e) {  // $parThis_009e
        U_RPosEstatConfig = 0;
    }

    if (I_parThis_009e) {  // $parThis_009e
        I_parOut_009e = 1;
    }
    I_rung_top = I_parOut_009e;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 343 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (!U_RMaqCortando) {  // RMaqCortando
        I_rung_top = 0;
    }

    if (U_RInvFreioLib) {  // RInvFreioLib
        I_rung_top = 0;
    }

    if (U_RPosEstatConfig) {  // RPosEstatConfig
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_009f = 0;
    I_parThis_009f = I_rung_top;
    /* start series [ */
    if (U_RMaqPosCorteSet) {  // RMaqPosCorteSet
        I_parThis_009f = 0;
    }

    /* start parallel [ */
    I_parOut_00a0 = 0;
    I_parThis_00a0 = I_parThis_009f;
    if (I_parThis_00a0) {  // $parThis_00a0
        U_PosNovoCorte = U_DifPosCorteInv + U_MaqPecaTam;
    }

    if (I_parThis_00a0) {  // $parThis_00a0
        I_parOut_00a0 = 1;
    }
    I_parThis_00a0 = I_parThis_009f;
    if (I_parThis_00a0) {  // $parThis_00a0
        U_RMaqPosCorteSet = 1;
    }

    if (I_parThis_00a0) {  // $parThis_00a0
        I_parOut_00a0 = 1;
    }
    I_parThis_009f = I_parOut_00a0;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_009f) {  // $parThis_009f
        I_parOut_009f = 1;
    }
    I_parThis_009f = I_rung_top;
    /* start series [ */
    if (U_MaqPecaTam > U_PosNovoCorte) {
    } else {
        I_parThis_009f = 0;
    }

    /* start parallel [ */
    I_parOut_00a1 = 0;
    I_parThis_00a1 = I_parThis_009f;
    if (I_parThis_00a1) {  // $parThis_00a1
        U_RMaqFimCorte = 1;
    }

    if (I_parThis_00a1) {  // $parThis_00a1
        I_parOut_00a1 = 1;
    }
    I_parThis_00a1 = I_parThis_009f;
    if (I_parThis_00a1) {  // $parThis_00a1
        U_VSrvPosMesa = 0;
    }

    if (I_parThis_00a1) {  // $parThis_00a1
        I_parOut_00a1 = 1;
    }
    I_parThis_00a1 = I_parThis_009f;
    if (I_parThis_00a1) {  // $parThis_00a1
        U_RInvAtivar = 1;
    }

    if (I_parThis_00a1) {  // $parThis_00a1
        I_parOut_00a1 = 1;
    }
    I_parThis_009f = I_parOut_00a1;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_009f) {  // $parThis_009f
        I_parOut_009f = 1;
    }
    I_parThis_009f = I_rung_top;
    /* start series [ */
    if (U_RMaqFimCorte) {  // RMaqFimCorte
        I_parThis_009f = 0;
    }

    /* start parallel [ */
    I_parOut_00a2 = 0;
    I_parThis_00a2 = I_parThis_009f;
    if (I_parThis_00a2) {  // $parThis_00a2
        U_PosNovoCorte = U_PosNovoCorte - U_MaqPecaTam;
    }

    if (I_parThis_00a2) {  // $parThis_00a2
        I_parOut_00a2 = 1;
    }
    I_parThis_00a2 = I_parThis_009f;
    if (I_parThis_00a2) {  // $parThis_00a2
        U_VSrvPosMesa = U_PosNovoCorte;
    }

    if (I_parThis_00a2) {  // $parThis_00a2
        I_parOut_00a2 = 1;
    }
    I_parThis_009f = I_parOut_00a2;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_009f) {  // $parThis_009f
        I_parOut_009f = 1;
    }
    I_parThis_009f = I_rung_top;
    if (I_parThis_009f) {  // $parThis_009f
        U_RMaqStartPosic = 1;
    }

    if (I_parThis_009f) {  // $parThis_009f
        I_parOut_009f = 1;
    }
    I_parThis_009f = I_rung_top;
    if (I_parThis_009f) {  // $parThis_009f
        U_RPosEstatConfig = 1;
    }

    if (I_parThis_009f) {  // $parThis_009f
        I_parOut_009f = 1;
    }
    I_rung_top = I_parOut_009f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 344 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_00a2) {  // $oneShot_00a2
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_00a2 = I_scratch;

    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00a3 = 0;
    I_parThis_00a3 = I_rung_top;
    /* start series [ */
    if (!U_RMaqFimCorte) {  // RMaqFimCorte
        I_parThis_00a3 = 0;
    }

    /* start parallel [ */
    I_parOut_00a4 = 0;
    I_parThis_00a4 = I_parThis_00a3;
    if (I_parThis_00a4) {  // $parThis_00a4
        U_RMaqCortando = 0;
    }

    if (I_parThis_00a4) {  // $parThis_00a4
        I_parOut_00a4 = 1;
    }
    I_parThis_00a4 = I_parThis_00a3;
    if (I_parThis_00a4) {  // $parThis_00a4
        U_RMaqPosCorteSet = 0;
    }

    if (I_parThis_00a4) {  // $parThis_00a4
        I_parOut_00a4 = 1;
    }
    I_parThis_00a4 = I_parThis_00a3;
    if (I_parThis_00a4) {  // $parThis_00a4
        U_RMaqFimCorte = 0;
    }

    if (I_parThis_00a4) {  // $parThis_00a4
        I_parOut_00a4 = 1;
    }
    I_parThis_00a4 = I_parThis_00a3;
    if (I_parThis_00a4) {  // $parThis_00a4
        U_RPosEstatConfig = 0;
    }

    if (I_parThis_00a4) {  // $parThis_00a4
        I_parOut_00a4 = 1;
    }
    I_parThis_00a3 = I_parOut_00a4;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00a3) {  // $parThis_00a3
        I_parOut_00a3 = 1;
    }
    I_parThis_00a3 = I_rung_top;
    /* start series [ */
    if (!U_RMaqCortando) {  // RMaqCortando
        I_parThis_00a3 = 0;
    }

    if (I_parThis_00a3) {  // $parThis_00a3
        U_RMaqCortePerf = 1;
    }

    /* ] finish series */
    if (I_parThis_00a3) {  // $parThis_00a3
        I_parOut_00a3 = 1;
    }
    I_rung_top = I_parOut_00a3;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 345 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_00a5 = 0;
    I_parThis_00a5 = I_rung_top;
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_parThis_00a5 = 0;
    }

    if (I_parThis_00a5) {  // $parThis_00a5
        I_parOut_00a5 = 1;
    }
    I_parThis_00a5 = I_rung_top;
    if (!U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_parThis_00a5 = 0;
    }

    if (I_parThis_00a5) {  // $parThis_00a5
        I_parOut_00a5 = 1;
    }
    I_rung_top = I_parOut_00a5;
    /* ] finish parallel */
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_00a3) {  // $oneShot_00a3
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_00a3 = I_scratch;

    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (!U_RMaqCortando) {  // RMaqCortando
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00a6 = 0;
    I_parThis_00a6 = I_rung_top;
    /* start series [ */
    if (!U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_00a6 = 0;
    }

    if (I_parThis_00a6) {  // $parThis_00a6
        U_RMaqModoAjuste = 1;
    }

    /* ] finish series */
    if (I_parThis_00a6) {  // $parThis_00a6
        I_parOut_00a6 = 1;
    }
    I_parThis_00a6 = I_rung_top;
    /* start series [ */
    if (U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_parThis_00a6 = 0;
    }

    /* start parallel [ */
    I_parOut_00a7 = 0;
    I_parThis_00a7 = I_parThis_00a6;
    if (I_parThis_00a7) {  // $parThis_00a7
        U_RPosEstatConfig = 0;
    }

    if (I_parThis_00a7) {  // $parThis_00a7
        I_parOut_00a7 = 1;
    }
    I_parThis_00a7 = I_parThis_00a6;
    if (I_parThis_00a7) {  // $parThis_00a7
        U_RSubQtdProd = 1;
    }

    if (I_parThis_00a7) {  // $parThis_00a7
        I_parOut_00a7 = 1;
    }
    I_parThis_00a6 = I_parOut_00a7;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00a6) {  // $parThis_00a6
        I_parOut_00a6 = 1;
    }
    I_rung_top = I_parOut_00a6;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 346 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_00a4) {  // $oneShot_00a4
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_00a4 = I_scratch;

    /* start parallel [ */
    I_parOut_00a8 = 0;
    I_parThis_00a8 = I_rung_top;
    if (I_parThis_00a8) {  // $parThis_00a8
        U_RMaqCortando = 0;
    }

    if (I_parThis_00a8) {  // $parThis_00a8
        I_parOut_00a8 = 1;
    }
    I_parThis_00a8 = I_rung_top;
    if (I_parThis_00a8) {  // $parThis_00a8
        U_RMaqFimCorte = 0;
    }

    if (I_parThis_00a8) {  // $parThis_00a8
        I_parOut_00a8 = 1;
    }
    I_parThis_00a8 = I_rung_top;
    if (I_parThis_00a8) {  // $parThis_00a8
        U_RPosEstatConfig = 0;
    }

    if (I_parThis_00a8) {  // $parThis_00a8
        I_parOut_00a8 = 1;
    }
    I_rung_top = I_parOut_00a8;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 348 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_00a5) {  // $oneShot_00a5
        I_rung_top = 0;
    }
    I_oneShot_00a5 = I_scratch;

    /* start parallel [ */
    I_parOut_00a9 = 0;
    I_parThis_00a9 = I_rung_top;
    if (I_parThis_00a9) {  // $parThis_00a9
        U_RInvAtivar = 0;
    }

    if (I_parThis_00a9) {  // $parThis_00a9
        I_parOut_00a9 = 1;
    }
    I_parThis_00a9 = I_rung_top;
    if (I_parThis_00a9) {  // $parThis_00a9
        U_RModoDinam = 0;
    }

    if (I_parThis_00a9) {  // $parThis_00a9
        I_parOut_00a9 = 1;
    }
    I_parThis_00a9 = I_rung_top;
    /* start series [ */
    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_00a9 = 0;
    }

    /* start parallel [ */
    I_parOut_00aa = 0;
    I_parThis_00aa = I_parThis_00a9;
    if (I_parThis_00aa) {  // $parThis_00aa
        U_RErroCortePerf = 0;
    }

    if (I_parThis_00aa) {  // $parThis_00aa
        I_parOut_00aa = 1;
    }
    I_parThis_00aa = I_parThis_00a9;
    if (I_parThis_00aa) {  // $parThis_00aa
        U_RErroCortePiloto = 0;
    }

    if (I_parThis_00aa) {  // $parThis_00aa
        I_parOut_00aa = 1;
    }
    I_parThis_00a9 = I_parOut_00aa;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00a9) {  // $parThis_00a9
        I_parOut_00a9 = 1;
    }
    I_parThis_00a9 = I_rung_top;
    /* start series [ */
    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_00a9 = 0;
    }

    if (I_parThis_00a9) {  // $parThis_00a9
        U_RMaqModoCorrecao = 1;
    }

    /* ] finish series */
    if (I_parThis_00a9) {  // $parThis_00a9
        I_parOut_00a9 = 1;
    }
    I_rung_top = I_parOut_00a9;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 349 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00ab = 0;
    I_parThis_00ab = I_rung_top;
    /* start series [ */
    if (U_RMaqModoCorrecao) {  // RMaqModoCorrecao
        I_parThis_00ab = 0;
    }

    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_parThis_00ab = 0;
    }

    /* ] finish series */
    if (I_parThis_00ab) {  // $parThis_00ab
        I_parOut_00ab = 1;
    }
    I_parThis_00ab = I_rung_top;
    /* start series [ */
    if (!U_RMaqModoCorrecao) {  // RMaqModoCorrecao
        I_parThis_00ab = 0;
    }

    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_parThis_00ab = 0;
    }

    /* ] finish series */
    if (I_parThis_00ab) {  // $parThis_00ab
        I_parOut_00ab = 1;
    }
    I_rung_top = I_parOut_00ab;
    /* ] finish parallel */
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_00a6) {  // $oneShot_00a6
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_00a6 = I_scratch;

    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00ac = 0;
    I_parThis_00ac = I_rung_top;
    /* start series [ */
    if (U_RMaqErroPosic) {  // RMaqErroPosic
        I_parThis_00ac = 0;
    }

    if (U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_00ac = 0;
    }

    /* start parallel [ */
    I_parOut_00ad = 0;
    I_parThis_00ad = I_parThis_00ac;
    /* start series [ */
    if (U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_00ad = 0;
    }

    if (I_parThis_00ad) {  // $parThis_00ad
        U_RInvAtivar = 1;
    }

    /* ] finish series */
    if (I_parThis_00ad) {  // $parThis_00ad
        I_parOut_00ad = 1;
    }
    I_parThis_00ad = I_parThis_00ac;
    if (I_parThis_00ad) {  // $parThis_00ad
        U_RMaqModoAjuste = 0;
    }

    if (I_parThis_00ad) {  // $parThis_00ad
        I_parOut_00ad = 1;
    }
    I_parThis_00ad = I_parThis_00ac;
    if (I_parThis_00ad) {  // $parThis_00ad
        U_RMaqModoCorrecao = 0;
    }
    
    if (I_parThis_00ad) {  // $parThis_00ad
        I_parOut_00ad = 1;
    }
    I_parThis_00ac = I_parOut_00ad;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00ac) {  // $parThis_00ac
        I_parOut_00ac = 1;
    }
    I_parThis_00ac = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_00ae = 0;
    I_parThis_00ae = I_parThis_00ac;
    if (!U_RMaqErroPosic) {  // RMaqErroPosic
        I_parThis_00ae = 0;
    }

    if (I_parThis_00ae) {  // $parThis_00ae
        I_parOut_00ae = 1;
    }
    I_parThis_00ae = I_parThis_00ac;
    if (!U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_00ae = 0;
    }

    if (I_parThis_00ae) {  // $parThis_00ae
        I_parOut_00ae = 1;
    }
    I_parThis_00ac = I_parOut_00ae;
    /* ] finish parallel */
    if (I_parThis_00ac) {  // $parThis_00ac
        U_RMaqModoCorrecao = 1;
    }

    /* ] finish series */
    if (I_parThis_00ac) {  // $parThis_00ac
        I_parOut_00ac = 1;
    }
    I_rung_top = I_parOut_00ac;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 350 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_rung_top = 0;
    }

    if (U_RMaqModoCorrecao) {  // RMaqModoCorrecao
        I_rung_top = 0;
    }

    if (U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_RMaqCorteEstat = 1;
    }

    /* ] finish series */

    /* start rung 351 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoCorrecao) {  // RMaqModoCorrecao
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00af = 0;
    I_parThis_00af = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_00af;
    if (I_oneShot_00a7) {  // $oneShot_00a7
        I_parThis_00af = 0;
    }
    I_oneShot_00a7 = I_scratch;

    /* start parallel [ */
    I_parOut_00b0 = 0;
    I_parThis_00b0 = I_parThis_00af;
    if (I_parThis_00b0) {  // $parThis_00b0
        U_RStartCfgInvMan = 1;
    }

    if (I_parThis_00b0) {  // $parThis_00b0
        I_parOut_00b0 = 1;
    }
    I_parThis_00b0 = I_parThis_00af;
    if (I_parThis_00b0) {  // $parThis_00b0
        U_RStartCfgInvAuto = 0;
    }

    if (I_parThis_00b0) {  // $parThis_00b0
        I_parOut_00b0 = 1;
    }
    I_parThis_00af = I_parOut_00b0;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00af) {  // $parThis_00af
        I_parOut_00af = 1;
    }
    I_parThis_00af = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_00af;
    if (!I_parThis_00af) {  // $parThis_00af
        if (I_oneShot_00a8) {  // $oneShot_00a8
            I_parThis_00af = 1;
        }
    } else {
        I_parThis_00af = 0;
    }
    I_oneShot_00a8 = I_scratch;

    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_00af = 0;
    }

    /* start parallel [ */
    I_parOut_00b1 = 0;
    I_parThis_00b1 = I_parThis_00af;
    if (I_parThis_00b1) {  // $parThis_00b1
        U_RStartCfgInvAuto = 1;
    }

    if (I_parThis_00b1) {  // $parThis_00b1
        I_parOut_00b1 = 1;
    }
    I_parThis_00b1 = I_parThis_00af;
    if (I_parThis_00b1) {  // $parThis_00b1
        U_RStartCfgInvMan = 0;
    }

    if (I_parThis_00b1) {  // $parThis_00b1
        I_parOut_00b1 = 1;
    }
    I_parThis_00af = I_parOut_00b1;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00af) {  // $parThis_00af
        I_parOut_00af = 1;
    }
    I_rung_top = I_parOut_00af;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 352 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoCorrecao) {  // RMaqModoCorrecao
        I_rung_top = 0;
    }

    if (!U_RMaqCfgInvOK) {  // RMaqCfgInvOK
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT7) {  // XCortePerfManual
        I_rung_top = 0;
    }

    if (U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    if (U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_00a9) {  // $oneShot_00a9
        I_rung_top = 0;
    }
    I_oneShot_00a9 = I_scratch;

    /* start parallel [ */
    I_parOut_00b2 = 0;
    I_parThis_00b2 = I_rung_top;
    /* start series [ */
    if (!U_RErroCortePerf) {  // RErroCortePerf
        I_parThis_00b2 = 0;
    }

    if (I_parThis_00b2) {  // $parThis_00b2
        U_RMaqCortePerf = 1;
    }

    /* ] finish series */
    if (I_parThis_00b2) {  // $parThis_00b2
        I_parOut_00b2 = 1;
    }
    I_parThis_00b2 = I_rung_top;
    /* start series [ */
    if (!U_RMaqErroPosic) {  // RMaqErroPosic
        I_parThis_00b2 = 0;
    }

    if (I_parThis_00b2) {  // $parThis_00b2
        U_RMaqModoCorrecao = 0;
    }

    /* ] finish series */
    if (I_parThis_00b2) {  // $parThis_00b2
        I_parOut_00b2 = 1;
    }
    I_parThis_00b2 = I_rung_top;
    if (I_parThis_00b2) {  // $parThis_00b2
        U_RMaqErroPosic = 0;
    }

    if (I_parThis_00b2) {  // $parThis_00b2
        I_parOut_00b2 = 1;
    }
    I_parThis_00b2 = I_rung_top;
    if (I_parThis_00b2) {  // $parThis_00b2
        U_RErroCortePerf = 0;
    }

    if (I_parThis_00b2) {  // $parThis_00b2
        I_parOut_00b2 = 1;
    }
    I_parThis_00b2 = I_rung_top;
    if (I_parThis_00b2) {  // $parThis_00b2
        U_RErroCortePiloto = 0;
    }

    if (I_parThis_00b2) {  // $parThis_00b2
        I_parOut_00b2 = 1;
    }
    I_rung_top = I_parOut_00b2;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 353 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoCorrecao) {  // RMaqModoCorrecao
        I_rung_top = 0;
    }

    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_00aa) {  // $oneShot_00aa
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_00aa = I_scratch;

    if (U_RErroCortePerf) {  // RErroCortePerf
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00b3 = 0;
    I_parThis_00b3 = I_rung_top;
    if (I_parThis_00b3) {  // $parThis_00b3
        U_VSrvPosMesa = 0;
    }

    if (I_parThis_00b3) {  // $parThis_00b3
        I_parOut_00b3 = 1;
    }
    I_parThis_00b3 = I_rung_top;
    if (I_parThis_00b3) {  // $parThis_00b3
        U_RMaqStartPosic = 1;
    }

    if (I_parThis_00b3) {  // $parThis_00b3
        I_parOut_00b3 = 1;
    }
    I_rung_top = I_parOut_00b3;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 355 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_00b4 = 0;
    I_parThis_00b4 = I_rung_top;
    /* start series [ */
    if (!U_RSubQtdProd) {  // RSubQtdProd
        I_parThis_00b4 = 0;
    }

    I_scratch = I_parThis_00b4;
    if (I_oneShot_00ab) {  // $oneShot_00ab
        I_parThis_00b4 = 0;
    }
    I_oneShot_00ab = I_scratch;

    /* ] finish series */
    if (I_parThis_00b4) {  // $parThis_00b4
        I_parOut_00b4 = 1;
    }
    I_parThis_00b4 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_00b5 = 0;
    I_parThis_00b5 = I_parThis_00b4;
    if (!GPIO_OUTPUT_PORT7) {  // YSrvEscravoAtivo
        I_parThis_00b5 = 0;
    }

    if (I_parThis_00b5) {  // $parThis_00b5
        I_parOut_00b5 = 1;
    }
    I_parThis_00b5 = I_parThis_00b4;
    if (!U_RMaqModoAjuste) {  // RMaqModoAjuste
        I_parThis_00b5 = 0;
    }

    if (I_parThis_00b5) {  // $parThis_00b5
        I_parOut_00b5 = 1;
    }
    I_parThis_00b4 = I_parOut_00b5;
    /* ] finish parallel */
    I_scratch = I_parThis_00b4;
    if (!I_parThis_00b4) {  // $parThis_00b4
        if (I_oneShot_00ac) {  // $oneShot_00ac
            I_parThis_00b4 = 1;
        }
    } else {
        I_parThis_00b4 = 0;
    }
    I_oneShot_00ac = I_scratch;

    /* ] finish series */
    if (I_parThis_00b4) {  // $parThis_00b4
        I_parOut_00b4 = 1;
    }
    I_rung_top = I_parOut_00b4;
    /* ] finish parallel */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00b6 = 0;
    I_parThis_00b6 = I_rung_top;
    if (I_parThis_00b6) {  // $parThis_00b6
        U_RSubQtdProd = 0;
    }

    if (I_parThis_00b6) {  // $parThis_00b6
        I_parOut_00b6 = 1;
    }
    I_parThis_00b6 = I_rung_top;
    if (I_parThis_00b6) {  // $parThis_00b6
        U_RMaqSincronizado = 0;
    }

    if (I_parThis_00b6) {  // $parThis_00b6
        I_parOut_00b6 = 1;
    }
    I_parThis_00b6 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_00b7 = 0;
    I_parThis_00b7 = I_parThis_00b6;
    /* start series [ */
    if (I_parThis_00b7) {  // $parThis_00b7
        if (!I_oneShot_00ad) {  // $oneShot_00ad
            I_scratch = 1;
            U_CQtdProd = U_CQtdProd - I_scratch;
        }
    }
    I_oneShot_00ad = I_parThis_00b7;
    if (U_CQtdProd < 1) {
        I_parThis_00b7 = 0;
    } else {
        I_parThis_00b7 = 1;
    }

    I_scratch = I_parThis_00b7;
    if (!I_parThis_00b7) {  // $parThis_00b7
        if (I_oneShot_00ae) {  // $oneShot_00ae
            I_parThis_00b7 = 1;
        }
    } else {
        I_parThis_00b7 = 0;
    }
    I_oneShot_00ae = I_scratch;

    /* ] finish series */
    if (I_parThis_00b7) {  // $parThis_00b7
        I_parOut_00b7 = 1;
    }
    I_parThis_00b7 = I_parThis_00b6;
    if (U_RModoAuto) {  // RModoAuto
        I_parThis_00b7 = 0;
    }

    if (I_parThis_00b7) {  // $parThis_00b7
        I_parOut_00b7 = 1;
    }
    I_parThis_00b6 = I_parOut_00b7;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_00b8 = 0;
    I_parThis_00b8 = I_parThis_00b6;
    if (I_parThis_00b8) {  // $parThis_00b8
        U_RMaqProduzindo = 0;
    }

    if (I_parThis_00b8) {  // $parThis_00b8
        I_parOut_00b8 = 1;
    }
    I_parThis_00b8 = I_parThis_00b6;
    if (I_parThis_00b8) {  // $parThis_00b8
        U_RMaqFimProd = 1;
    }

    if (I_parThis_00b8) {  // $parThis_00b8
        I_parOut_00b8 = 1;
    }
    I_parThis_00b8 = I_parThis_00b6;
    /* start series [ */
    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_parThis_00b8 = 0;
    }

    /* start parallel [ */
    I_parOut_00b9 = 0;
    I_parThis_00b9 = I_parThis_00b8;
    if (I_parThis_00b9) {  // $parThis_00b9
        U_VSrvPosMesa = 0;
    }

    if (I_parThis_00b9) {  // $parThis_00b9
        I_parOut_00b9 = 1;
    }
    I_parThis_00b9 = I_parThis_00b8;
    if (I_parThis_00b9) {  // $parThis_00b9
        U_RMaqStartPosic = 1;
    }

    if (I_parThis_00b9) {  // $parThis_00b9
        I_parOut_00b9 = 1;
    }
    I_parThis_00b8 = I_parOut_00b9;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00b8) {  // $parThis_00b8
        I_parOut_00b8 = 1;
    }
    I_parThis_00b6 = I_parOut_00b8;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00b6) {  // $parThis_00b6
        I_parOut_00b6 = 1;
    }
    I_rung_top = I_parOut_00b6;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 356 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_00ba = 0;
    I_parThis_00ba = I_rung_top;
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_00ba = 0;
    }

    if (I_parThis_00ba) {  // $parThis_00ba
        I_parOut_00ba = 1;
    }
    I_parThis_00ba = I_rung_top;
    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_00ba = 0;
    }

    I_scratch = I_parThis_00ba;
    if (!I_parThis_00ba) {  // $parThis_00ba
        if (I_oneShot_00af) {  // $oneShot_00af
            I_parThis_00ba = 1;
        }
    } else {
        I_parThis_00ba = 0;
    }
    I_oneShot_00af = I_scratch;

    /* ] finish series */
    if (I_parThis_00ba) {  // $parThis_00ba
        I_parOut_00ba = 1;
    }
    I_rung_top = I_parOut_00ba;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[30] = U_CQtdProd;
    }

    /* ] finish series */

    /* start rung 357 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqFimProd) {  // RMaqFimProd
        I_rung_top = 0;
    }

    if (U_RSrvCheckFimProd) {  // RSrvCheckFimProd
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_00b0) {  // $oneShot_00b0
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15031, 1, &U_SrvRegStatus);
                I_oneShot_00b0 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_00b1 = I_rung_top;
        }
        if (!I_oneShot_00b1) {  // $oneShot_00b1
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_00b1 = 1;
            } else if (I_SerialTimeout) {  // $SerialTimeout
                Modbus_Send(3, MODBUS_FC_READ_HOLDING_REGISTERS, 15031, 1, &U_SrvRegStatus);
                I_SerialTimeout = 0;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        if (I_oneShot_00b0) {  // $oneShot_00b0
            I_SerialReady = 1;
        }
        I_oneShot_00b0 = I_rung_top;
    }

    /* start parallel [ */
    I_parOut_00bb = 0;
    I_parThis_00bb = I_rung_top;
    /* start series [ */
    if (I_parThis_00bb) {  // $parThis_00bb
        if (U_TEspFimProd < 49) {
            U_TEspFimProd++;
            I_parThis_00bb = 0;
        }
    } else {
        U_TEspFimProd = 0;
    }

    /* start parallel [ */
    I_parOut_00bc = 0;
    I_parThis_00bc = I_parThis_00bb;
    U_RSrvCheckFimProd = I_parThis_00bc;

    if (I_parThis_00bc) {  // $parThis_00bc
        I_parOut_00bc = 1;
    }
    I_parThis_00bc = I_parThis_00bb;
    if (I_parThis_00bc) {  // $parThis_00bc
        U_RInvAtivar = 0;
    }

    if (I_parThis_00bc) {  // $parThis_00bc
        I_parOut_00bc = 1;
    }
    I_parThis_00bb = I_parOut_00bc;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00bb) {  // $parThis_00bb
        I_parOut_00bb = 1;
    }
    I_parThis_00bb = I_rung_top;
    /* start series [ */
    if (I_parThis_00bb) {  // $parThis_00bb
        if (!(U_SrvRegStatus & (1 << 6))) {  // SrvRegStatus
            I_parThis_00bb = 0;
        }
    }

    /* start parallel [ */
    I_parOut_00bd = 0;
    I_parThis_00bd = I_parThis_00bb;
    if (I_parThis_00bd) {  // $parThis_00bd
        U_RMaqFimProd = 0;
    }

    if (I_parThis_00bd) {  // $parThis_00bd
        I_parOut_00bd = 1;
    }
    I_parThis_00bd = I_parThis_00bb;
    if (I_parThis_00bd) {  // $parThis_00bd
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }

    if (I_parThis_00bd) {  // $parThis_00bd
        I_parOut_00bd = 1;
    }
    I_parThis_00bd = I_parThis_00bb;
    if (I_parThis_00bd) {  // $parThis_00bd
        U_RInvAtivar = 0;
    }

    if (I_parThis_00bd) {  // $parThis_00bd
        I_parOut_00bd = 1;
    }
    I_parThis_00bb = I_parOut_00bd;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_00bb) {  // $parThis_00bb
        I_parOut_00bb = 1;
    }
    I_rung_top = I_parOut_00bb;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 359 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // XMbModoAuto
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00be = 0;
    I_parThis_00be = I_rung_top;
    U_RModoAuto = I_parThis_00be;

    if (I_parThis_00be) {  // $parThis_00be
        I_parOut_00be = 1;
    }
    I_parThis_00be = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_00be;
    if (I_oneShot_00b2) {  // $oneShot_00b2
        I_parThis_00be = 0;
    }
    I_oneShot_00b2 = I_scratch;

    if (I_parThis_00be) {  // $parThis_00be
        MODBUS_REGISTER[1] |= (1 << 0);  // YMbModoAutoAtivo
    }

    /* ] finish series */
    if (I_parThis_00be) {  // $parThis_00be
        I_parOut_00be = 1;
    }
    I_rung_top = I_parOut_00be;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 360 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 2))) {  // YMbLimpaErro
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00bf = 0;
    I_parThis_00bf = I_rung_top;
    if (I_parThis_00bf) {  // $parThis_00bf
        U_RMaqCfgInvErro = 0;
    }

    if (I_parThis_00bf) {  // $parThis_00bf
        I_parOut_00bf = 1;
    }
    I_parThis_00bf = I_rung_top;
    if (I_parThis_00bf) {  // $parThis_00bf
        U_RMaqCfgSrvErro = 0;
    }

    if (I_parThis_00bf) {  // $parThis_00bf
        I_parOut_00bf = 1;
    }
    I_parThis_00bf = I_rung_top;
    if (I_parThis_00bf) {  // $parThis_00bf
        U_RMaqErroPosic = 0;
    }

    if (I_parThis_00bf) {  // $parThis_00bf
        I_parOut_00bf = 1;
    }
    I_parThis_00bf = I_rung_top;
    if (I_parThis_00bf) {  // $parThis_00bf
        MODBUS_REGISTER[2] &= ~(1 << 2);  // YMbLimpaErro
    }

    if (I_parThis_00bf) {  // $parThis_00bf
        I_parOut_00bf = 1;
    }
    I_rung_top = I_parOut_00bf;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 361 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 11))) {  // YMbCalcCorrecao
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00c0 = 0;
    I_parThis_00c0 = I_rung_top;
    if (I_parThis_00c0) {  // $parThis_00c0
        U_RMaqCalcCorrecao = 1;
    }

    if (I_parThis_00c0) {  // $parThis_00c0
        I_parOut_00c0 = 1;
    }
    I_parThis_00c0 = I_rung_top;
    if (I_parThis_00c0) {  // $parThis_00c0
        MODBUS_REGISTER[2] &= ~(1 << 11);  // YMbCalcCorrecao
    }

    if (I_parThis_00c0) {  // $parThis_00c0
        I_parOut_00c0 = 1;
    }
    I_rung_top = I_parOut_00c0;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 362 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 12))) {  // YMbCalcRelacao
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00c1 = 0;
    I_parThis_00c1 = I_rung_top;
    if (I_parThis_00c1) {  // $parThis_00c1
        U_RMaqCalcRelacao = 1;
    }

    if (I_parThis_00c1) {  // $parThis_00c1
        I_parOut_00c1 = 1;
    }
    I_parThis_00c1 = I_rung_top;
    if (I_parThis_00c1) {  // $parThis_00c1
        MODBUS_REGISTER[2] &= ~(1 << 12);  // YMbCalcRelacao
    }

    if (I_parThis_00c1) {  // $parThis_00c1
        I_parOut_00c1 = 1;
    }
    I_rung_top = I_parOut_00c1;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 363 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 13))) {  // YMbCalcTamMin
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_00c2 = 0;
    I_parThis_00c2 = I_rung_top;
    if (I_parThis_00c2) {  // $parThis_00c2
        U_RMaqCalcTamMin = 1;
    }

    if (I_parThis_00c2) {  // $parThis_00c2
        I_parOut_00c2 = 1;
    }
    I_parThis_00c2 = I_rung_top;
    if (I_parThis_00c2) {  // $parThis_00c2
        MODBUS_REGISTER[2] &= ~(1 << 13);  // YMbCalcTamMin
    }

    if (I_parThis_00c2) {  // $parThis_00c2
        I_parOut_00c2 = 1;
    }
    I_rung_top = I_parOut_00c2;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 365 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // XEmergencia
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 0); MODBUS_REGISTER[0] |= I_rung_top << 0;  // YMbErrEmergencia

    /* ] finish series */

    /* start rung 366 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT3) {  // XFaltaFase
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 1); MODBUS_REGISTER[0] |= I_rung_top << 1;  // YMbErrFaltaFase

    /* ] finish series */

    /* start rung 367 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT2) {  // XTermBombaHidr
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 2); MODBUS_REGISTER[0] |= I_rung_top << 2;  // YMbErrBombaHidr

    /* ] finish series */

    /* start rung 368 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT4) {  // XInversorOK
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 3); MODBUS_REGISTER[0] |= I_rung_top << 3;  // YMbErrInversor

    /* ] finish series */

    /* start rung 369 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT15) {  // XServoOK
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 4); MODBUS_REGISTER[0] |= I_rung_top << 4;  // YMbErrServo

    /* ] finish series */

    /* start rung 370 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqCfgInvErro) {  // RMaqCfgInvErro
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 5); MODBUS_REGISTER[0] |= I_rung_top << 5;  // YMbErrInvComunic

    /* ] finish series */

    /* start rung 371 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 6); MODBUS_REGISTER[0] |= I_rung_top << 6;  // YMbErrSrvComunic

    /* ] finish series */

    /* start rung 372 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqErroPosic) {  // RMaqErroPosic
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 7); MODBUS_REGISTER[0] |= I_rung_top << 7;  // YMbErrPosic

    /* ] finish series */

    /* start rung 373 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RErroCortePerf) {  // RErroCortePerf
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 8); MODBUS_REGISTER[0] |= I_rung_top << 8;  // YMbErrCorte

    /* ] finish series */

    /* start rung 374 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[2] & (1 << 3)) {  // YMbMaqLib
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 9); MODBUS_REGISTER[0] |= I_rung_top << 9;  // YMbMaqDesativada

    /* ] finish series */

    /* start rung 376 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSrvPosicAtivo) {  // RSrvPosicAtivo
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 1); MODBUS_REGISTER[1] |= I_rung_top << 1;  // YMbSrvPosicAtivo

    /* ] finish series */

    /* start rung 377 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 2); MODBUS_REGISTER[1] |= I_rung_top << 2;  // YMbMaqProduzindo

    /* ] finish series */

    /* start rung 378 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCortePerfAtivo) {  // RCortePerfAtivo
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 3); MODBUS_REGISTER[1] |= I_rung_top << 3;  // YMbCorteAtivo

    /* ] finish series */

    /* start rung 379 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RCorteEstatAtivo) {  // RCorteEstatAtivo
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 4); MODBUS_REGISTER[1] |= I_rung_top << 4;  // YMbCrtEstatAtivo

    /* ] finish series */

    /* start rung 380 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_00c3 = 0;
    I_parThis_00c3 = I_rung_top;
    if (!U_RMaqInitStart) {  // RMaqInitStart
        I_parThis_00c3 = 0;
    }

    if (I_parThis_00c3) {  // $parThis_00c3
        I_parOut_00c3 = 1;
    }
    I_parThis_00c3 = I_rung_top;
    if (!U_RMaqInit) {  // RMaqInit
        I_parThis_00c3 = 0;
    }

    if (I_parThis_00c3) {  // $parThis_00c3
        I_parOut_00c3 = 1;
    }
    I_parThis_00c3 = I_rung_top;
    if (!U_RMaqInitFim) {  // RMaqInitFim
        I_parThis_00c3 = 0;
    }

    if (I_parThis_00c3) {  // $parThis_00c3
        I_parOut_00c3 = 1;
    }
    I_rung_top = I_parOut_00c3;
    /* ] finish parallel */
    MODBUS_REGISTER[1] &= ~(1 << 5); MODBUS_REGISTER[1] |= I_rung_top << 5;  // YMbMaqIniciando

    /* ] finish series */

    /* start rung 381 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqInitOK) {  // RMaqInitOK
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 6); MODBUS_REGISTER[1] |= I_rung_top << 6;  // YMbMaqInitOK

    /* ] finish series */

    /* start rung 382 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RFerramCorteCima) {  // RFerramCorteCima
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 7); MODBUS_REGISTER[1] |= I_rung_top << 7;  // YMbFerramCrtCima

    /* ] finish series */

    /* start rung 383 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 8); MODBUS_REGISTER[1] |= I_rung_top << 8;  // YMbMaqPronta

    /* ] finish series */

    /* start rung 384 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOperando) {  // RMaqOperando
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 9); MODBUS_REGISTER[1] |= I_rung_top << 9;  // YMbMaqOperando

    /* ] finish series */

    /* start rung 385 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqCfgInvOK) {  // RMaqCfgInvOK
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 10); MODBUS_REGISTER[1] |= I_rung_top << 10;  // YMbMaqCfgInvOK

    /* ] finish series */

    /* start rung 386 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 11); MODBUS_REGISTER[1] |= I_rung_top << 11;  // YMbMaqCfgSrvOK

    /* ] finish series */

    /* start rung 387 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 12); MODBUS_REGISTER[1] |= I_rung_top << 12;  // YMbMaqModoEstat

    /* ] finish series */

    /* start rung 388 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqCortando) {  // RMaqCortando
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 13); MODBUS_REGISTER[1] |= I_rung_top << 13;  // YMbMaqCortando

    /* ] finish series */

    /* start rung 389 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqCortePerf) {  // RMaqCortePerf
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 14); MODBUS_REGISTER[1] |= I_rung_top << 14;  // YMbMaqCortePerf

    /* ] finish series */

    /* start rung 391 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_00b3) {  // $oneShot_00b3
        I_rung_top = 0;
    }
    I_oneShot_00b3 = I_scratch;

    /* start parallel [ */
    I_parOut_00c4 = 0;
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        ENC_Reset();
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        U_MaqPosCorte = 0;
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[11] &= ~(1 << 0);  // YMbOffsetCorte
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[12] &= ~(1 << 0);  // YMbAutoAcel
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[13] &= ~(1 << 0);  // YMbAutoDesac
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[14] &= ~(1 << 0);  // YMbDinamVel
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[26] &= ~(1 << 0);  // YMbEstatVel
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[15] &= ~(1 << 0);  // YMbManAcel
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[16] &= ~(1 << 0);  // YMbManDesac
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[17] &= ~(1 << 0);  // YMbManVel
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[20] &= ~(1 << 0);  // YMbEncFator
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[21] &= ~(1 << 0);  // YMbEncResol
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[22] &= ~(1 << 0);  // YMbEncPerim
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[31] &= ~(1 << 0);  // YMbTam
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[30] &= ~(1 << 0);  // YMbQtd
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_parThis_00c4 = I_rung_top;
    if (I_parThis_00c4) {  // $parThis_00c4
        MODBUS_REGISTER[4] &= ~(1 << 0);  // YMbEncMesa
    }

    if (I_parThis_00c4) {  // $parThis_00c4
        I_parOut_00c4 = 1;
    }
    I_rung_top = I_parOut_00c4;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 392 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_00b4) {  // $oneShot_00b4
        I_rung_top = 0;
    }
    I_oneShot_00b4 = I_scratch;

    /* start parallel [ */
    I_parOut_00c5 = 0;
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[31] = 3000;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[30] = 30;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[14] = 60;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[26] = 70;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[12] = 200;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[13] = 75;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[17] = 30;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[15] = 400;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[16] = 100;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        U_MaxInvSyncVel = 30;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        U_WInvModoAuto = 1;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        U_WInvModoMan = 0;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[20] = 1000;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[21] = 2500;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[22] = 400;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_parThis_00c5 = I_rung_top;
    if (I_parThis_00c5) {  // $parThis_00c5
        MODBUS_REGISTER[11] = 4;
    }

    if (I_parThis_00c5) {  // $parThis_00c5
        I_parOut_00c5 = 1;
    }
    I_rung_top = I_parOut_00c5;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 393 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_00b5) {  // $oneShot_00b5
        I_rung_top = 0;
    }
    I_oneShot_00b5 = I_scratch;

    /* start parallel [ */
    I_parOut_00c6 = 0;
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        U_MaqPosReduzVel = 2500;
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[10] &= ~(1 << 0);  // YMbMesaCurso
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[10] = 800;
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[19] &= ~(1 << 0);  // YMbSrvPosMesa
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[19] = 0;
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[24] &= ~(1 << 0);  // YMbSrvVelJog
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[24] = 50;
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[25] &= ~(1 << 0);  // YMbSrvVelPos
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[25] = 100;
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[27] &= ~(1 << 0);  // YMbNovoFator
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[27] = 1000;
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[18] &= ~(1 << 0);  // YMbTamMin
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        I_scratch2 = 2;
        MODBUS_REGISTER[18] = MODBUS_REGISTER[10] * I_scratch2;
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[23] &= ~(1 << 0);  // YMbNovoTamMin
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[8] &= ~(1 << 0);  // YMbDbgRelPerf
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_parThis_00c6 = I_rung_top;
    if (I_parThis_00c6) {  // $parThis_00c6
        MODBUS_REGISTER[9] &= ~(1 << 0);  // YMbDbgRelSrv
    }

    if (I_parThis_00c6) {  // $parThis_00c6
        I_parOut_00c6 = 1;
    }
    I_rung_top = I_parOut_00c6;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 396 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        U_MaqPecaTam = MODBUS_REGISTER[31] * I_scratch2;
    }

    /* ] finish series */

    /* start rung 398 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        U_SrvMesaCurso = MODBUS_REGISTER[10] * I_scratch2;
    }

    /* ] finish series */

    /* start rung 399 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        U_MaqPecaTamMin = MODBUS_REGISTER[18] * I_scratch2;
    }

    /* ] finish series */

    /* start rung 400 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 2;
        U_ZEncResetVal = U_SrvMesaCurso * I_scratch2;
    }

    /* ] finish series */

    /* start rung 402 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 22000;
        U_SrvVelPos = MODBUS_REGISTER[25] * I_scratch2;
    }

    /* ] finish series */

    /* start rung 403 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 100;
        if(I_scratch2) U_SrvVelPos = U_SrvVelPos / I_scratch2;
    }

    /* ] finish series */

    /* start rung 404 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 5000;
        U_SrvVelJog = MODBUS_REGISTER[24] * I_scratch2;
    }

    /* ] finish series */

    /* start rung 405 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 100;
        if(I_scratch2) U_SrvVelJog = U_SrvVelJog / I_scratch2;
    }

    /* ] finish series */

    /* start rung 407 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 60;
        U_WInvAutoVel = MODBUS_REGISTER[14] * I_scratch2;
    }

    /* ] finish series */

    /* start rung 408 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqModoEstatico) {  // RMaqModoEstatico
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        I_scratch2 = 60;
        U_WInvAutoVel = MODBUS_REGISTER[26] * I_scratch2;
    }

    /* ] finish series */

    /* start rung 409 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        if(I_scratch2) U_WInvAutoVel = U_WInvAutoVel / I_scratch2;
    }

    /* ] finish series */

    /* start rung 410 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 60;
        U_WInvManVel = MODBUS_REGISTER[17] * I_scratch2;
    }

    /* ] finish series */

    /* start rung 411 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        if(I_scratch2) U_WInvManVel = U_WInvManVel / I_scratch2;
    }

    /* ] finish series */

    /* start rung 412 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 60;
        U_WInvMaxSyncVel = U_MaxInvSyncVel * I_scratch2;
    }

    /* ] finish series */

    /* start rung 413 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10;
        if(I_scratch2) U_WInvMaxSyncVel = U_WInvMaxSyncVel / I_scratch2;
    }

    /* ] finish series */

    /* start rung 415 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_WInvAutoAcel = MODBUS_REGISTER[12];
    }

    /* ] finish series */

    /* start rung 416 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_WInvAutoDesac = MODBUS_REGISTER[13];
    }

    /* ] finish series */

    /* start rung 417 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_WInvManAcel = MODBUS_REGISTER[15];
    }

    /* ] finish series */

    /* start rung 418 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_WInvManDesac = MODBUS_REGISTER[16];
    }

    /* ] finish series */
}
