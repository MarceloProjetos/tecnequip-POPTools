unsigned int LadderWaitingYaskawa_Write_Debug = 0;

#define Yaskawa_Write_Debug(p1,p2,p3) do { LadderWaitingYaskawa_Write_Debug = 1; Yaskawa_Write(p1, p2, p3); LadderWaitingYaskawa_Write_Debug=0; } while (0);

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
volatile unsigned char GPIO_INPUT_PORT4 = 0;
volatile unsigned char I_parOut_0001 = 0;
volatile unsigned char I_parThis_0001 = 0;
volatile unsigned char U_RErroGeral = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char U_RMaqInitOK = 0;
volatile int U_PrsValErro = 0;
volatile unsigned char U_RPrsPausaSrvOK = 0;
volatile unsigned char U_RMaqCfgSrvErro = 0;
volatile unsigned char U_RMaqProduzindo = 0;
volatile unsigned char U_RMaqSerialBusy = 0;
volatile int U_TEspChecarSrvOK = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile unsigned char I_parOut_0002 = 0;
volatile unsigned char I_parThis_0002 = 0;
volatile unsigned char U_RPrsChecarSrvOK = 0;
volatile unsigned char I_oneShot_0002 = 0;
volatile int VAR_NULL = 0;
volatile unsigned char I_oneShot_0003 = 0;
volatile int U_TEspRespChecar = 0;
volatile unsigned char I_oneShot_0004 = 0;
volatile unsigned char I_oneShot_0005 = 0;
volatile unsigned char I_parOut_0003 = 0;
volatile unsigned char I_parThis_0003 = 0;
volatile unsigned char U_RServoOK = 0;
volatile int I_scratch2 = 0;
volatile unsigned char U_RCfgSrvErro = 0;
volatile unsigned char I_oneShot_0006 = 0;
volatile unsigned char I_parOut_0004 = 0;
volatile unsigned char I_parThis_0004 = 0;
volatile unsigned char U_RSerialReservada = 0;
volatile unsigned char U_RMaqLerOKSerial = 0;
volatile unsigned char U_RStartDesabSrv = 0;
volatile unsigned char U_RStartZerarEnc = 0;
volatile unsigned char U_RStartCorrigeEnc = 0;
volatile unsigned char U_RStartCfgSrvJog = 0;
volatile unsigned char U_RStartCfgSrv = 0;
volatile unsigned char GPIO_INPUT_PORT18 = 0;
volatile unsigned char I_TEspPressaoAr_antiglitch = 0;
volatile int U_TEspPressaoAr = 0;
volatile unsigned char U_RPressaoArOK = 0;
volatile unsigned char GPIO_INPUT_PORT5 = 0;
volatile unsigned char U_RErroBombaHidr = 0;
volatile unsigned char GPIO_INPUT_PORT3 = 0;
volatile unsigned char I_parOut_0005 = 0;
volatile unsigned char I_parThis_0005 = 0;
volatile unsigned char GPIO_INPUT_PORT8 = 0;
volatile unsigned char U_RMaqOK = 0;
volatile unsigned char I_oneShot_0007 = 0;
volatile unsigned char I_parOut_0006 = 0;
volatile unsigned char I_parThis_0006 = 0;
volatile unsigned char U_RAplanLigarMotor = 0;
volatile unsigned char U_RMaqDesabSrv = 0;
volatile unsigned char U_RMaqCfgSrvOK = 0;
volatile unsigned char GPIO_OUTPUT_PORT11 = 0;
volatile unsigned char I_oneShot_0008 = 0;
volatile unsigned char I_parOut_0007 = 0;
volatile unsigned char I_parThis_0007 = 0;
volatile unsigned char U_YLedErro = 0;
volatile unsigned char U_RMaqConfigOK = 0;
volatile unsigned char U_RMaqPronta = 0;
volatile unsigned char I_parOut_0008 = 0;
volatile unsigned char I_parThis_0008 = 0;
volatile unsigned char U_RModoAuto = 0;
volatile unsigned char U_RMaqStartProd = 0;
volatile unsigned char U_RMaqManualOK = 0;
volatile int U_EMaqPosAtual = 0;
volatile int U_MaqPosAtual = 0;
volatile unsigned char GPIO_INPUT_PORT6 = 0;
volatile int U_TEspAplanFechar = 0;
volatile unsigned char I_parOut_0009 = 0;
volatile unsigned char I_parThis_0009 = 0;
volatile unsigned char U_RAplanFechada = 0;
volatile unsigned char I_oneShot_0009 = 0;
volatile unsigned char I_parOut_000a = 0;
volatile unsigned char I_parThis_000a = 0;
volatile unsigned char U_RPrsAvancoOK = 0;
volatile unsigned char U_RAplanAvancoOK = 0;
volatile unsigned char U_RAplanAbre = 0;
volatile unsigned char U_RAplanFecha = 0;
volatile unsigned char U_RAplanSobe = 0;
volatile unsigned char U_RAplanDesce = 0;
volatile unsigned char I_parOut_000b = 0;
volatile unsigned char I_parThis_000b = 0;
volatile unsigned char I_oneShot_000a = 0;
volatile unsigned char U_RAplanExtSubir = 0;
volatile unsigned char I_oneShot_000b = 0;
volatile unsigned char I_parOut_000c = 0;
volatile unsigned char I_parThis_000c = 0;
volatile unsigned char I_oneShot_000c = 0;
volatile unsigned char U_RAplanExpandir = 0;
volatile unsigned char I_oneShot_000d = 0;
volatile int U_TEspRetSerial = 0;
volatile unsigned char I_oneShot_000e = 0;
volatile int U_AplanRetSerial = 0;
volatile unsigned char I_oneShot_000f = 0;
volatile unsigned char I_parOut_000d = 0;
volatile unsigned char I_parThis_000d = 0;
volatile unsigned char U_RAplanCorrigeEnc = 0;
volatile unsigned char I_parOut_000e = 0;
volatile unsigned char I_parThis_000e = 0;
volatile int U_MaqPosAtualCorr = 0;
volatile int U_FatorCorrServo = 0;
volatile unsigned char I_oneShot_0010 = 0;
volatile unsigned char I_oneShot_0011 = 0;
volatile unsigned char I_parOut_000f = 0;
volatile unsigned char I_parThis_000f = 0;
volatile unsigned char I_oneShot_0012 = 0;
volatile unsigned char I_parOut_0010 = 0;
volatile unsigned char I_parThis_0010 = 0;
volatile unsigned char I_parOut_0011 = 0;
volatile unsigned char I_parThis_0011 = 0;
volatile unsigned char U_RStartDesabSVOFF = 0;
volatile unsigned char I_parOut_0012 = 0;
volatile unsigned char I_parThis_0012 = 0;
volatile int U_TEspFimDesabSrv = 0;
volatile unsigned char I_parOut_0013 = 0;
volatile unsigned char I_parThis_0013 = 0;
volatile unsigned char I_oneShot_0013 = 0;
volatile unsigned char I_oneShot_0014 = 0;
volatile unsigned char I_parOut_0014 = 0;
volatile unsigned char I_parThis_0014 = 0;
volatile unsigned char I_oneShot_0015 = 0;
volatile unsigned char I_oneShot_0016 = 0;
volatile unsigned char I_oneShot_0017 = 0;
volatile unsigned char I_parOut_0015 = 0;
volatile unsigned char I_parThis_0015 = 0;
volatile unsigned char I_oneShot_0018 = 0;
volatile unsigned char I_parOut_0016 = 0;
volatile unsigned char I_parThis_0016 = 0;
volatile unsigned char U_RAplanZerarEnc = 0;
volatile unsigned char U_RSrvMotorParado = 0;
volatile unsigned char I_parOut_0017 = 0;
volatile unsigned char I_parThis_0017 = 0;
volatile int U_EncPosZero = 0;
volatile unsigned char I_oneShot_0019 = 0;
volatile unsigned char U_RMaqForaPosic = 0;
volatile int U_DifPosic = 0;
volatile unsigned char I_oneShot_001a = 0;
volatile unsigned char I_oneShot_001b = 0;
volatile unsigned char I_parOut_0018 = 0;
volatile unsigned char I_parThis_0018 = 0;
volatile unsigned char I_oneShot_001c = 0;
volatile unsigned char I_parOut_0019 = 0;
volatile unsigned char I_parThis_0019 = 0;
volatile unsigned char I_parOut_001a = 0;
volatile unsigned char I_parThis_001a = 0;
volatile int U_ZEncPerfil = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile unsigned char I_parOut_001b = 0;
volatile unsigned char I_parThis_001b = 0;
volatile unsigned char I_parOut_001c = 0;
volatile unsigned char I_parThis_001c = 0;
volatile unsigned char GPIO_INPUT_PORT13 = 0;
volatile int U_TBombaHidrTO = 0;
volatile unsigned char GPIO_INPUT_PORT2 = 0;
volatile int U_TBombaHidrEstab = 0;
volatile unsigned char U_RBombaHidrOK = 0;
volatile unsigned char I_parOut_001d = 0;
volatile unsigned char I_parThis_001d = 0;
volatile int U_TBombaHidrInat = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile unsigned char I_parOut_001e = 0;
volatile unsigned char I_parThis_001e = 0;
volatile unsigned char GPIO_OUTPUT_PORT9 = 0;
volatile unsigned char GPIO_OUTPUT_PORT8 = 0;
volatile unsigned char I_parOut_001f = 0;
volatile unsigned char I_parThis_001f = 0;
volatile unsigned char GPIO_OUTPUT_PORT10 = 0;
volatile unsigned char I_parOut_0020 = 0;
volatile unsigned char I_parThis_0020 = 0;
volatile unsigned char I_oneShot_001d = 0;
volatile unsigned char I_parOut_0021 = 0;
volatile unsigned char I_parThis_0021 = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;
volatile unsigned char GPIO_OUTPUT_PORT5 = 0;
volatile unsigned char I_oneShot_001e = 0;
volatile unsigned char I_parOut_0022 = 0;
volatile unsigned char I_parThis_0022 = 0;
volatile unsigned char I_parOut_0023 = 0;
volatile unsigned char I_parThis_0023 = 0;
volatile int U_TEspAtuarExt = 0;
volatile unsigned char I_parOut_0024 = 0;
volatile unsigned char I_parThis_0024 = 0;
volatile unsigned char I_parOut_0025 = 0;
volatile unsigned char I_parThis_0025 = 0;
volatile unsigned char I_oneShot_001f = 0;
volatile unsigned char I_parOut_0026 = 0;
volatile unsigned char I_parThis_0026 = 0;
volatile unsigned char GPIO_OUTPUT_PORT6 = 0;
volatile unsigned char GPIO_OUTPUT_PORT7 = 0;
volatile unsigned char I_oneShot_0020 = 0;
volatile unsigned char I_parOut_0027 = 0;
volatile unsigned char I_parThis_0027 = 0;
volatile unsigned char GPIO_INPUT_PORT7 = 0;
volatile unsigned char I_parOut_0028 = 0;
volatile unsigned char I_parThis_0028 = 0;
volatile unsigned char I_oneShot_0021 = 0;
volatile unsigned char I_oneShot_0022 = 0;
volatile unsigned char I_parOut_0029 = 0;
volatile unsigned char I_parThis_0029 = 0;
volatile unsigned char I_parOut_002a = 0;
volatile unsigned char I_parThis_002a = 0;
volatile unsigned char I_TEspFimCfgJog_antiglitch = 0;
volatile int U_TEspFimCfgJog = 0;
volatile unsigned char U_RAplanManualOK = 0;
volatile unsigned char I_parOut_002b = 0;
volatile unsigned char I_parThis_002b = 0;
volatile unsigned char GPIO_INPUT_PORT9 = 0;
volatile unsigned char GPIO_INPUT_PORT10 = 0;
volatile unsigned char I_parOut_002c = 0;
volatile unsigned char I_parThis_002c = 0;
volatile unsigned char U_RAplanAvancarMan = 0;
volatile unsigned char I_oneShot_0023 = 0;
volatile unsigned char U_RMaqCfgSrvJog = 0;
volatile unsigned char I_parOut_002d = 0;
volatile unsigned char I_parThis_002d = 0;
volatile unsigned char I_parOut_002e = 0;
volatile unsigned char I_parThis_002e = 0;
volatile unsigned char U_RAplanRecuarMan = 0;
volatile unsigned char I_oneShot_0024 = 0;
volatile unsigned char I_parOut_002f = 0;
volatile unsigned char I_parThis_002f = 0;
volatile unsigned char I_oneShot_0025 = 0;
volatile unsigned char I_oneShot_0026 = 0;
volatile unsigned char U_RAplanAvancaMan = 0;
volatile unsigned char I_parOut_0030 = 0;
volatile unsigned char I_parThis_0030 = 0;
volatile unsigned char U_RCfgJogFrente = 0;
volatile unsigned char U_RCfgJogRecua = 0;
volatile unsigned char U_RCfgJogParar = 0;
volatile unsigned char I_parOut_0031 = 0;
volatile unsigned char I_parThis_0031 = 0;
volatile unsigned char I_oneShot_0027 = 0;
volatile int U_TCfgSrvJogAvcTO = 0;
volatile unsigned char I_oneShot_0028 = 0;
volatile int U_AplanVelManual = 0;
volatile unsigned char I_oneShot_0029 = 0;
volatile unsigned char I_parOut_0032 = 0;
volatile unsigned char I_parThis_0032 = 0;
volatile unsigned char I_oneShot_002a = 0;
volatile unsigned char I_parOut_0033 = 0;
volatile unsigned char I_parThis_0033 = 0;
volatile unsigned char I_parOut_0034 = 0;
volatile unsigned char I_parThis_0034 = 0;
volatile unsigned char I_oneShot_002b = 0;
volatile int U_TCfgSrvJogRecTO = 0;
volatile unsigned char I_oneShot_002c = 0;
volatile unsigned char I_oneShot_002d = 0;
volatile unsigned char I_parOut_0035 = 0;
volatile unsigned char I_parThis_0035 = 0;
volatile unsigned char I_oneShot_002e = 0;
volatile unsigned char I_parOut_0036 = 0;
volatile unsigned char I_parThis_0036 = 0;
volatile unsigned char I_parOut_0037 = 0;
volatile unsigned char I_parThis_0037 = 0;
volatile unsigned char I_oneShot_002f = 0;
volatile int U_TCfgSrvJogDesTO = 0;
volatile unsigned char I_oneShot_0030 = 0;
volatile unsigned char I_oneShot_0031 = 0;
volatile unsigned char I_parOut_0038 = 0;
volatile unsigned char I_parThis_0038 = 0;
volatile unsigned char I_oneShot_0032 = 0;
volatile unsigned char I_parOut_0039 = 0;
volatile unsigned char I_parThis_0039 = 0;
volatile unsigned char I_oneShot_0033 = 0;
volatile unsigned char I_parOut_003a = 0;
volatile unsigned char I_parThis_003a = 0;
volatile unsigned char GPIO_INPUT_PORT12 = 0;
volatile int U_TEspCameON = 0;
volatile unsigned char I_TEspCameOFF_antiglitch = 0;
volatile int U_TEspCameOFF = 0;
volatile unsigned char U_RPrsCameParar = 0;
volatile unsigned char GPIO_INPUT_PORT11 = 0;
volatile int U_TEspAvancoON = 0;
volatile unsigned char I_TEspAvancoOFF_antiglitch = 0;
volatile int U_TEspAvancoOFF = 0;
volatile unsigned char I_oneShot_0034 = 0;
volatile unsigned char I_parOut_003b = 0;
volatile unsigned char I_parThis_003b = 0;
volatile unsigned char U_RPrsMotorLigado = 0;
volatile int U_TEspMotorParar = 0;
volatile unsigned char U_RPrsMotorParado = 0;
volatile unsigned char I_parOut_003c = 0;
volatile unsigned char I_parThis_003c = 0;
volatile unsigned char I_oneShot_0035 = 0;
volatile unsigned char I_oneShot_0036 = 0;
volatile unsigned char U_RPrsNovoSentido = 0;
volatile unsigned char I_parOut_003d = 0;
volatile unsigned char I_parThis_003d = 0;
volatile unsigned char I_parOut_003e = 0;
volatile unsigned char I_parThis_003e = 0;
volatile unsigned char I_parOut_003f = 0;
volatile unsigned char I_parThis_003f = 0;
volatile unsigned char GPIO_OUTPUT_PORT14 = 0;
volatile unsigned char GPIO_OUTPUT_PORT13 = 0;
volatile unsigned char I_parOut_0040 = 0;
volatile unsigned char I_parThis_0040 = 0;
volatile unsigned char I_parOut_0041 = 0;
volatile unsigned char I_parThis_0041 = 0;
volatile unsigned char I_oneShot_0037 = 0;
volatile unsigned char I_parOut_0042 = 0;
volatile unsigned char I_parThis_0042 = 0;
volatile unsigned char U_RPrsIniciarCiclo = 0;
volatile int U_TEspPrsMotorOK = 0;
volatile unsigned char U_RPrsMotorOK = 0;
volatile unsigned char I_parOut_0043 = 0;
volatile unsigned char I_parThis_0043 = 0;
volatile unsigned char GPIO_INPUT_PORT14 = 0;
volatile unsigned char I_parOut_0044 = 0;
volatile unsigned char I_parThis_0044 = 0;
volatile unsigned char GPIO_INPUT_PORT17 = 0;
volatile unsigned char U_RBloquearPrensa = 0;
volatile unsigned char I_parOut_0045 = 0;
volatile unsigned char I_parThis_0045 = 0;
volatile unsigned char U_RPrsPararMartelo = 0;
volatile unsigned char I_parOut_0046 = 0;
volatile unsigned char I_parThis_0046 = 0;
volatile unsigned char I_oneShot_0038 = 0;
volatile unsigned char I_parOut_0047 = 0;
volatile unsigned char I_parThis_0047 = 0;
volatile unsigned char GPIO_OUTPUT_PORT16 = 0;
volatile unsigned char I_parOut_0048 = 0;
volatile unsigned char I_parThis_0048 = 0;
volatile unsigned char I_oneShot_0039 = 0;
volatile unsigned char I_parOut_0049 = 0;
volatile unsigned char I_parThis_0049 = 0;
volatile unsigned char I_oneShot_003a = 0;
volatile unsigned char I_parOut_004a = 0;
volatile unsigned char I_parThis_004a = 0;
volatile int U_PrsCiclos = 0;
volatile unsigned char I_oneShot_003b = 0;
volatile int U_PrsCiclosMil = 0;
volatile unsigned char GPIO_INPUT_PORT15 = 0;
volatile unsigned char I_TEspCmdBM1_antiglitch = 0;
volatile int U_TEspCmdBM1 = 0;
volatile unsigned char I_parOut_004b = 0;
volatile unsigned char I_parThis_004b = 0;
volatile unsigned char U_RPrsCmdBM1Blq = 0;
volatile unsigned char U_RPrsCmdBM1OK = 0;
volatile int U_TTOCmdBM1 = 0;
volatile unsigned char GPIO_INPUT_PORT16 = 0;
volatile unsigned char I_TEspCmdBM2_antiglitch = 0;
volatile int U_TEspCmdBM2 = 0;
volatile unsigned char I_parOut_004c = 0;
volatile unsigned char I_parThis_004c = 0;
volatile unsigned char U_RPrsCmdBM2Blq = 0;
volatile unsigned char U_RPrsCmdBM2OK = 0;
volatile int U_TTOCmdBM2 = 0;
volatile unsigned char U_RPrsCmdBMOK = 0;
volatile unsigned char I_parOut_004d = 0;
volatile unsigned char I_parThis_004d = 0;
volatile unsigned char I_parOut_004e = 0;
volatile unsigned char I_parThis_004e = 0;
volatile unsigned char I_parOut_004f = 0;
volatile unsigned char I_parThis_004f = 0;
volatile unsigned char I_oneShot_003c = 0;
volatile unsigned char I_parOut_0050 = 0;
volatile unsigned char I_parThis_0050 = 0;
volatile unsigned char I_oneShot_003d = 0;
volatile unsigned char GPIO_OUTPUT_PORT12 = 0;
volatile unsigned char I_oneShot_003e = 0;
volatile unsigned char I_parOut_0051 = 0;
volatile unsigned char I_parThis_0051 = 0;
volatile unsigned char I_oneShot_003f = 0;
volatile unsigned char I_parOut_0052 = 0;
volatile unsigned char I_parThis_0052 = 0;
volatile unsigned char U_RMaqCfgSrv = 0;
volatile unsigned char I_parOut_0053 = 0;
volatile unsigned char I_parThis_0053 = 0;
volatile unsigned char I_parOut_0054 = 0;
volatile unsigned char I_parThis_0054 = 0;
volatile unsigned char I_oneShot_0040 = 0;
volatile unsigned char I_oneShot_0041 = 0;
volatile unsigned char I_parOut_0055 = 0;
volatile unsigned char I_parThis_0055 = 0;
volatile unsigned char U_RCfgSrvParP1OK = 0;
volatile unsigned char U_RCfgSrvParP2OK = 0;
volatile unsigned char U_RCfgSrvParP3OK = 0;
volatile unsigned char U_RCfgSrvParP4OK = 0;
volatile unsigned char U_RCfgSrvParP5OK = 0;
volatile unsigned char U_RCfgSrvParP6OK = 0;
volatile unsigned char U_RCfgSrvParP7OK = 0;
volatile int U_TCfgSrvIniTO = 0;
volatile unsigned char I_oneShot_0042 = 0;
volatile unsigned char I_oneShot_0043 = 0;
volatile int U_TCfgSrvParP1Fim = 0;
volatile unsigned char I_parOut_0056 = 0;
volatile unsigned char I_parThis_0056 = 0;
volatile unsigned char I_oneShot_0044 = 0;
volatile unsigned char I_oneShot_0045 = 0;
volatile unsigned char I_oneShot_0046 = 0;
volatile unsigned char I_parOut_0057 = 0;
volatile unsigned char I_parThis_0057 = 0;
volatile unsigned char I_oneShot_0047 = 0;
volatile unsigned char I_oneShot_0048 = 0;
volatile int U_AplanVelMax = 0;
volatile unsigned char I_oneShot_0049 = 0;
volatile unsigned char I_parOut_0058 = 0;
volatile unsigned char I_parThis_0058 = 0;
volatile unsigned char I_oneShot_004a = 0;
volatile unsigned char I_oneShot_004b = 0;
volatile int U_AplanRampa = 0;
volatile unsigned char I_oneShot_004c = 0;
volatile unsigned char I_parOut_0059 = 0;
volatile unsigned char I_parThis_0059 = 0;
volatile unsigned char I_oneShot_004d = 0;
volatile unsigned char I_oneShot_004e = 0;
volatile unsigned char I_oneShot_004f = 0;
volatile unsigned char I_parOut_005a = 0;
volatile unsigned char I_parThis_005a = 0;
volatile unsigned char I_oneShot_0050 = 0;
volatile unsigned char I_oneShot_0051 = 0;
volatile int U_AplanPassoCorr = 0;
volatile unsigned char I_oneShot_0052 = 0;
volatile int U_TEspCfgSrvPar5 = 0;
volatile unsigned char I_parOut_005b = 0;
volatile unsigned char I_parThis_005b = 0;
volatile unsigned char I_oneShot_0053 = 0;
volatile unsigned char I_oneShot_0054 = 0;
volatile int U_AplanVelAuto = 0;
volatile unsigned char I_oneShot_0055 = 0;
volatile int U_TEspCfgSrvPar6 = 0;
volatile unsigned char I_parOut_005c = 0;
volatile unsigned char I_parThis_005c = 0;
volatile unsigned char I_oneShot_0056 = 0;
volatile unsigned char I_oneShot_0057 = 0;
volatile unsigned char I_oneShot_0058 = 0;
volatile unsigned char I_parOut_005d = 0;
volatile unsigned char I_parThis_005d = 0;
volatile unsigned char I_oneShot_0059 = 0;
volatile unsigned char I_parOut_005e = 0;
volatile unsigned char I_parThis_005e = 0;
volatile unsigned char U_RModoAutoOK = 0;
volatile unsigned char I_parOut_005f = 0;
volatile unsigned char I_parThis_005f = 0;
volatile unsigned char GPIO_INPUT_PORT19 = 0;
volatile int U_TEspDeslSrvInPos = 0;
volatile unsigned char I_parOut_0060 = 0;
volatile unsigned char I_parThis_0060 = 0;
volatile unsigned char I_parOut_0061 = 0;
volatile unsigned char I_parThis_0061 = 0;
volatile unsigned char U_RMaqErroPosic = 0;
volatile unsigned char I_oneShot_005a = 0;
volatile unsigned char I_parOut_0062 = 0;
volatile unsigned char I_parThis_0062 = 0;
volatile unsigned char U_RMaqInicioAuto = 0;
volatile unsigned char U_RMaqPosicionando = 0;
volatile unsigned char I_oneShot_005b = 0;
volatile unsigned char I_parOut_0063 = 0;
volatile unsigned char I_parThis_0063 = 0;
volatile int U_TEspExtRepouso = 0;
volatile unsigned char I_oneShot_005c = 0;
volatile unsigned char I_oneShot_005d = 0;
volatile unsigned char I_parOut_0064 = 0;
volatile unsigned char I_parThis_0064 = 0;
volatile unsigned char I_parOut_0065 = 0;
volatile unsigned char I_parThis_0065 = 0;
volatile int U_AplanPasso = 0;
volatile unsigned char I_parOut_0066 = 0;
volatile unsigned char I_parThis_0066 = 0;
volatile unsigned char U_RMaqPosicOK = 0;
volatile unsigned char I_parOut_0067 = 0;
volatile unsigned char I_parThis_0067 = 0;
volatile unsigned char I_oneShot_005e = 0;
volatile unsigned char I_parOut_0068 = 0;
volatile unsigned char I_parThis_0068 = 0;
volatile unsigned char I_oneShot_005f = 0;
volatile unsigned char I_parOut_0069 = 0;
volatile unsigned char I_parThis_0069 = 0;
volatile unsigned char I_parOut_006a = 0;
volatile unsigned char I_parThis_006a = 0;
volatile unsigned char I_oneShot_0060 = 0;
volatile unsigned char I_parOut_006b = 0;
volatile unsigned char I_parThis_006b = 0;
volatile unsigned char I_oneShot_0061 = 0;
volatile unsigned char I_oneShot_0062 = 0;
volatile unsigned char I_parOut_006c = 0;
volatile unsigned char I_parThis_006c = 0;
volatile unsigned char I_oneShot_0063 = 0;
volatile unsigned char I_parOut_006d = 0;
volatile unsigned char I_parThis_006d = 0;
volatile unsigned char I_oneShot_0064 = 0;
volatile unsigned char I_TEspSrvParar_antiglitch = 0;
volatile int U_TEspSrvParar = 0;
volatile unsigned char I_parOut_006e = 0;
volatile unsigned char I_parThis_006e = 0;
volatile unsigned char I_oneShot_0065 = 0;
volatile unsigned char I_oneShot_0066 = 0;
volatile unsigned char I_parOut_006f = 0;
volatile unsigned char I_parThis_006f = 0;
volatile unsigned char I_oneShot_0067 = 0;
volatile unsigned char I_oneShot_0068 = 0;
volatile unsigned char I_parOut_0070 = 0;
volatile unsigned char I_parThis_0070 = 0;
volatile unsigned char I_TEspInvLimpaErr_antiglitch = 0;
volatile int U_TEspInvLimpaErr = 0;
volatile unsigned char GPIO_OUTPUT_PORT15 = 0;
volatile unsigned char I_oneShot_0069 = 0;
volatile unsigned char I_parOut_0071 = 0;
volatile unsigned char I_parThis_0071 = 0;
volatile unsigned char I_oneShot_006a = 0;
volatile unsigned char I_parOut_0072 = 0;
volatile unsigned char I_parThis_0072 = 0;

void PLC_Run(void)
{
    I_mcr = 1;
    
    /* start rung 52 */
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
    
    /* start rung 54 */
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
    if (GPIO_INPUT_PORT4) {  // XFaltaFase
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
    
    /* start rung 55 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgMaqLib
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 1);  // YMbMaqLib
    }
    
    /* ] finish series */
    
    /* start rung 56 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 5))) {  // XDbgValErro48
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_PrsValErro = 48;
    }
    
    /* ] finish series */
    
    /* start rung 57 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 4))) {  // XDbgValErro0
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_PrsValErro = 0;
    }
    
    /* ] finish series */
    
    /* start rung 58 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RPrsPausaSrvOK) {  // RPrsPausaSrvOK
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspChecarSrvOK < 199) {
            U_TEspChecarSrvOK++;
            I_rung_top = 0;
        }
    } else {
        U_TEspChecarSrvOK = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0001) {  // $oneShot_0001
        I_rung_top = 0;
    }
    I_oneShot_0001 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0002 = 0;
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {  // $parThis_0002
        U_RPrsChecarSrvOK = 1;
    }
    
    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {  // $parThis_0002
        U_RMaqSerialBusy = 1;
    }
    
    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_rung_top = I_parOut_0002;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 59 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPrsChecarSrvOK) {  // RPrsChecarSrvOK
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0002) {  // $oneShot_0002
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "ERR", &VAR_NULL);
                I_oneShot_0002 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0003 = I_rung_top;
        }
        if (!I_oneShot_0003) {  // $oneShot_0003
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0003 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0002 = I_rung_top;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspRespChecar < 4) {
            U_TEspRespChecar++;
            I_rung_top = 0;
        }
    } else {
        U_TEspRespChecar = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0004) {  // $oneShot_0004
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Read("0", "ERR NONE", &U_PrsValErro);
                I_oneShot_0004 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0005 = I_rung_top;
        }
        if (!I_oneShot_0005) {  // $oneShot_0005
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0005 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0004 = I_rung_top;
    }
    
    /* start parallel [ */
    I_parOut_0003 = 0;
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        U_RServoOK = 0;
    }
    
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        U_RMaqSerialBusy = 0;
    }
    
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    if (I_parThis_0003) {  // $parThis_0003
        U_RPrsChecarSrvOK = 0;
    }
    
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    /* start series [ */
    I_scratch2 = 0;
    if (U_PrsValErro == I_scratch2) {
    } else {
        I_parThis_0003 = 0;
    }
    
    if (I_parThis_0003) {  // $parThis_0003
        U_RServoOK = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_rung_top = I_parOut_0003;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 60 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RCfgSrvErro) {  // RCfgSrvErro
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0006) {  // $oneShot_0006
        I_rung_top = 0;
    }
    I_oneShot_0006 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0004 = 0;
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        U_RMaqSerialBusy = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        U_RSerialReservada = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        U_RMaqLerOKSerial = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        U_RStartDesabSrv = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        U_RStartZerarEnc = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        U_RStartCorrigeEnc = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        U_RStartCfgSrvJog = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    if (I_parThis_0004) {  // $parThis_0004
        U_RStartCfgSrv = 0;
    }
    
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_rung_top = I_parOut_0004;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 61 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT18) {  // XPressaoArOK
        I_rung_top = 0;
    }
    
    if (!I_TEspPressaoAr_antiglitch) {  // $TEspPressaoAr_antiglitch
        U_TEspPressaoAr = 49;
    }
    I_TEspPressaoAr_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEspPressaoAr < 49) {
            U_TEspPressaoAr++;
            I_rung_top = 1;
        }
    } else {
        U_TEspPressaoAr = 0;
    }
    
    U_RPressaoArOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 62 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RErroGeral) {  // RErroGeral
        I_rung_top = 0;
    }
    
    if (!GPIO_INPUT_PORT5) {  // XInversorOK
        I_rung_top = 0;
    }
    
    if (U_RErroBombaHidr) {  // RErroBombaHidr
        I_rung_top = 0;
    }
    
    if (!GPIO_INPUT_PORT3) {  // XTermMartelo
        I_rung_top = 0;
    }
    
    if (!U_RPressaoArOK) {  // RPressaoArOK
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0005 = 0;
    I_parThis_0005 = I_rung_top;
    if (!GPIO_INPUT_PORT8) {  // XDesbobOK
        I_parThis_0005 = 0;
    }
    
    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    
    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_rung_top = I_parOut_0005;
    /* ] finish parallel */
    if (!U_RServoOK) {  // RServoOK
        I_rung_top = 0;
    }
    
    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // YMbMaqLib
        I_rung_top = 0;
    }
    
    U_RMaqOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 63 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0007) {  // $oneShot_0007
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0007 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0006 = 0;
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        MODBUS_REGISTER[2] &= ~(1 << 1);  // YMbMaqLib
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_RAplanLigarMotor = 0;
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_RMaqDesabSrv = 1;
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_RMaqCfgSrvOK = 0;
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        GPIO_OUTPUT_PORT11 = 0;
    }
    
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_rung_top = I_parOut_0006;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 64 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0008) {  // $oneShot_0008
        I_rung_top = 0;
    }
    I_oneShot_0008 = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        U_RMaqDesabSrv = 1;
    }
    
    /* ] finish series */
    
    /* start rung 65 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0007 = 0;
    I_parThis_0007 = I_rung_top;
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_0007 = 0;
    }
    
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    if (!U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_parThis_0007 = 0;
    }
    
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_rung_top = I_parOut_0007;
    /* ] finish parallel */
    U_YLedErro = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 66 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_rung_top = 0;
    }
    
    U_RMaqConfigOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 67 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }
    
    if (!U_RMaqConfigOK) {  // RMaqConfigOK
        I_rung_top = 0;
    }
    
    U_RMaqPronta = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 68 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0008 = 0;
    I_parThis_0008 = I_rung_top;
    if (U_RModoAuto) {  // RModoAuto
        I_parThis_0008 = 0;
    }
    
    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_parThis_0008 = I_rung_top;
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_parThis_0008 = 0;
    }
    
    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_rung_top = I_parOut_0008;
    /* ] finish parallel */
    U_RMaqManualOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 71 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (MODBUS_REGISTER[3] & (1 << 1)) {  // XDbgDeslReadEnc
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_EMaqPosAtual = ENC_Read();
    }
    
    /* ] finish series */
    
    /* start rung 72 */
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
    
    /* start rung 73 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgSimulaEncNeg
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 20;
        U_EMaqPosAtual = U_EMaqPosAtual - I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 78 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_MaqPosAtual = U_EMaqPosAtual * MODBUS_REGISTER[22];
    }
    
    /* ] finish series */
    
    /* start rung 79 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 100;
        U_MaqPosAtual = U_MaqPosAtual * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 80 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 4;
        if(I_scratch2) U_MaqPosAtual = U_MaqPosAtual / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 81 */
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
    
    /* start rung 82 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_MaqPosAtual = U_MaqPosAtual * MODBUS_REGISTER[20];
    }
    
    /* ] finish series */
    
    /* start rung 83 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10000;
        if(I_scratch2) U_MaqPosAtual = U_MaqPosAtual / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 84 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[5] = U_MaqPosAtual;
    }
    
    /* ] finish series */
    
    /* start rung 86 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT6) {  // XAplanFechada
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspAplanFechar < 19) {
            U_TEspAplanFechar++;
            I_rung_top = 0;
        }
    } else {
        U_TEspAplanFechar = 0;
    }
    
    /* start parallel [ */
    I_parOut_0009 = 0;
    I_parThis_0009 = I_rung_top;
    U_RAplanFechada = I_parThis_0009;
    
    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_parThis_0009 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0009;
    if (I_oneShot_0009) {  // $oneShot_0009
        I_parThis_0009 = 0;
    }
    I_oneShot_0009 = I_scratch;
    
    /* start parallel [ */
    I_parOut_000a = 0;
    I_parThis_000a = I_parThis_0009;
    if (I_parThis_000a) {  // $parThis_000a
        MODBUS_REGISTER[4] &= ~(1 << 0);  // YAplanManAvcIHM
    }
    
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_parThis_0009;
    if (I_parThis_000a) {  // $parThis_000a
        MODBUS_REGISTER[4] &= ~(1 << 1);  // YAplanManRecIHM
    }
    
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_0009 = I_parOut_000a;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_rung_top = I_parOut_0009;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 87 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanFechada) {  // RAplanFechada
        I_rung_top = 0;
    }
    
    if (!U_RPrsAvancoOK) {  // RPrsAvancoOK
        I_rung_top = 0;
    }
    
    U_RAplanAvancoOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 88 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }
    
    if (!(MODBUS_REGISTER[4] & (1 << 2))) {  // YMbAplanAbre
        I_rung_top = 0;
    }
    
    U_RAplanAbre = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 89 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }
    
    if (!(MODBUS_REGISTER[4] & (1 << 3))) {  // YMbAplanFecha
        I_rung_top = 0;
    }
    
    U_RAplanFecha = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 90 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }
    
    if (!(MODBUS_REGISTER[4] & (1 << 4))) {  // YMbAplanSobe
        I_rung_top = 0;
    }
    
    U_RAplanSobe = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 91 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }
    
    if (!(MODBUS_REGISTER[4] & (1 << 5))) {  // YMbAplanDesce
        I_rung_top = 0;
    }
    
    U_RAplanDesce = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 92 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[4] & (1 << 6))) {  // YMbAplanExtSubir
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_000b = 0;
    I_parThis_000b = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_000b;
    if (I_oneShot_000a) {  // $oneShot_000a
        I_parThis_000b = 0;
    }
    I_oneShot_000a = I_scratch;
    
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {  // $parThis_000b
        U_RAplanExtSubir = 1;
    }
    
    /* ] finish series */
    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_000b;
    if (!I_parThis_000b) {  // $parThis_000b
        if (I_oneShot_000b) {  // $oneShot_000b
            I_parThis_000b = 1;
        }
    } else {
        I_parThis_000b = 0;
    }
    I_oneShot_000b = I_scratch;
    
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_000b = 0;
    }
    
    if (I_parThis_000b) {  // $parThis_000b
        U_RAplanExtSubir = 0;
    }
    
    /* ] finish series */
    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_rung_top = I_parOut_000b;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 93 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[4] & (1 << 7))) {  // YMbAplanExpandir
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_000c = 0;
    I_parThis_000c = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_000c;
    if (I_oneShot_000c) {  // $oneShot_000c
        I_parThis_000c = 0;
    }
    I_oneShot_000c = I_scratch;
    
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_000c = 0;
    }
    
    if (I_parThis_000c) {  // $parThis_000c
        U_RAplanExpandir = 1;
    }
    
    /* ] finish series */
    if (I_parThis_000c) {  // $parThis_000c
        I_parOut_000c = 1;
    }
    I_parThis_000c = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_000c;
    if (!I_parThis_000c) {  // $parThis_000c
        if (I_oneShot_000d) {  // $oneShot_000d
            I_parThis_000c = 1;
        }
    } else {
        I_parThis_000c = 0;
    }
    I_oneShot_000d = I_scratch;
    
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_000c = 0;
    }
    
    if (I_parThis_000c) {  // $parThis_000c
        U_RAplanExpandir = 0;
    }
    
    /* ] finish series */
    if (I_parThis_000c) {  // $parThis_000c
        I_parOut_000c = 1;
    }
    I_rung_top = I_parOut_000c;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 95 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspRetSerial < 9) {
            U_TEspRetSerial++;
            I_rung_top = 0;
        }
    } else {
        U_TEspRetSerial = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_000e) {  // $oneShot_000e
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Read("0", "OK", &U_AplanRetSerial);
                I_oneShot_000e = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_000f = I_rung_top;
        }
        if (!I_oneShot_000f) {  // $oneShot_000f
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_000f = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_000e = I_rung_top;
    }
    
    /* start parallel [ */
    I_parOut_000d = 0;
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {  // $parThis_000d
        U_RMaqLerOKSerial = 0;
    }
    
    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    /* start series [ */
    I_scratch2 = 0;
    if (I_scratch2 > U_AplanRetSerial) {
    } else {
        I_parThis_000d = 0;
    }
    
    if (I_parThis_000d) {  // $parThis_000d
        U_RMaqCfgSrvErro = 1;
    }
    
    /* ] finish series */
    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_rung_top = I_parOut_000d;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 97 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanCorrigeEnc) {  // RAplanCorrigeEnc
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_000e = 0;
    I_parThis_000e = I_rung_top;
    if (I_parThis_000e) {  // $parThis_000e
        U_RStartCorrigeEnc = 1;
    }
    
    if (I_parThis_000e) {  // $parThis_000e
        I_parOut_000e = 1;
    }
    I_parThis_000e = I_rung_top;
    if (I_parThis_000e) {  // $parThis_000e
        U_RMaqSerialBusy = 1;
    }
    
    if (I_parThis_000e) {  // $parThis_000e
        I_parOut_000e = 1;
    }
    I_rung_top = I_parOut_000e;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 98 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_MaqPosAtualCorr = U_MaqPosAtual * U_FatorCorrServo;
    }
    
    /* ] finish series */
    
    /* start rung 99 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10000;
        if(I_scratch2) U_MaqPosAtualCorr = U_MaqPosAtualCorr / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 100 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartCorrigeEnc) {  // RStartCorrigeEnc
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0010) {  // $oneShot_0010
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "ZSET%d", &U_MaqPosAtualCorr);
                I_oneShot_0010 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0011 = I_rung_top;
        }
        if (!I_oneShot_0011) {  // $oneShot_0011
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0011 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0010 = I_rung_top;
    }
    
    /* start parallel [ */
    I_parOut_000f = 0;
    I_parThis_000f = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_000f;
    if (I_oneShot_0012) {  // $oneShot_0012
        I_parThis_000f = 0;
    }
    I_oneShot_0012 = I_scratch;
    
    if (I_parThis_000f) {  // $parThis_000f
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_000f) {  // $parThis_000f
        I_parOut_000f = 1;
    }
    I_parThis_000f = I_rung_top;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_000f = 0;
    }
    
    /* start parallel [ */
    I_parOut_0010 = 0;
    I_parThis_0010 = I_parThis_000f;
    if (I_parThis_0010) {  // $parThis_0010
        U_RStartCorrigeEnc = 0;
    }
    
    if (I_parThis_0010) {  // $parThis_0010
        I_parOut_0010 = 1;
    }
    I_parThis_0010 = I_parThis_000f;
    if (I_parThis_0010) {  // $parThis_0010
        U_RAplanCorrigeEnc = 0;
    }
    
    if (I_parThis_0010) {  // $parThis_0010
        I_parOut_0010 = 1;
    }
    I_parThis_0010 = I_parThis_000f;
    if (I_parThis_0010) {  // $parThis_0010
        U_RMaqSerialBusy = 0;
    }
    
    if (I_parThis_0010) {  // $parThis_0010
        I_parOut_0010 = 1;
    }
    I_parThis_000f = I_parOut_0010;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_000f) {  // $parThis_000f
        I_parOut_000f = 1;
    }
    I_rung_top = I_parOut_000f;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 102 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqDesabSrv) {  // RMaqDesabSrv
        I_rung_top = 0;
    }
    
    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0011 = 0;
    I_parThis_0011 = I_rung_top;
    if (I_parThis_0011) {  // $parThis_0011
        U_RStartDesabSrv = 1;
    }
    
    if (I_parThis_0011) {  // $parThis_0011
        I_parOut_0011 = 1;
    }
    I_parThis_0011 = I_rung_top;
    if (I_parThis_0011) {  // $parThis_0011
        U_RMaqDesabSrv = 0;
    }
    
    if (I_parThis_0011) {  // $parThis_0011
        I_parOut_0011 = 1;
    }
    I_parThis_0011 = I_rung_top;
    if (I_parThis_0011) {  // $parThis_0011
        U_RStartDesabSVOFF = 0;
    }
    
    if (I_parThis_0011) {  // $parThis_0011
        I_parOut_0011 = 1;
    }
    I_parThis_0011 = I_rung_top;
    if (I_parThis_0011) {  // $parThis_0011
        U_RMaqSerialBusy = 1;
    }
    
    if (I_parThis_0011) {  // $parThis_0011
        I_parOut_0011 = 1;
    }
    I_rung_top = I_parOut_0011;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 103 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartDesabSrv) {  // RStartDesabSrv
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0012 = 0;
    I_parThis_0012 = I_rung_top;
    /* start series [ */
    if (I_parThis_0012) {  // $parThis_0012
        if (U_TEspFimDesabSrv < 199) {
            U_TEspFimDesabSrv++;
            I_parThis_0012 = 0;
        }
    } else {
        U_TEspFimDesabSrv = 0;
    }
    
    /* start parallel [ */
    I_parOut_0013 = 0;
    I_parThis_0013 = I_parThis_0012;
    if (I_parThis_0013) {  // $parThis_0013
        U_RMaqCfgSrvErro = 1;
    }
    
    if (I_parThis_0013) {  // $parThis_0013
        I_parOut_0013 = 1;
    }
    I_parThis_0013 = I_parThis_0012;
    if (I_parThis_0013) {  // $parThis_0013
        U_RStartDesabSrv = 0;
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
    I_parThis_0012 = I_rung_top;
    /* start series [ */
    if (U_RStartDesabSVOFF) {  // RStartDesabSVOFF
        I_parThis_0012 = 0;
    }
    
    if (I_parThis_0012) {  // $parThis_0012
        if (!I_oneShot_0013) {  // $oneShot_0013
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "SKIP", &VAR_NULL);
                I_oneShot_0013 = I_parThis_0012;
            }
            I_parThis_0012 = 0;
            I_oneShot_0014 = I_parThis_0012;
        }
        if (!I_oneShot_0014) {  // $oneShot_0014
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0014 = 1;
            } else {
                I_parThis_0012 = 0;
            }
        }
    } else {
        I_oneShot_0013 = I_parThis_0012;
    }
    
    /* start parallel [ */
    I_parOut_0014 = 0;
    I_parThis_0014 = I_parThis_0012;
    /* start series [ */
    I_scratch = I_parThis_0014;
    if (I_oneShot_0015) {  // $oneShot_0015
        I_parThis_0014 = 0;
    }
    I_oneShot_0015 = I_scratch;
    
    if (I_parThis_0014) {  // $parThis_0014
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0014) {  // $parThis_0014
        I_parOut_0014 = 1;
    }
    I_parThis_0014 = I_parThis_0012;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0014 = 0;
    }
    
    if (I_parThis_0014) {  // $parThis_0014
        U_RStartDesabSVOFF = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0014) {  // $parThis_0014
        I_parOut_0014 = 1;
    }
    I_parThis_0012 = I_parOut_0014;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0012) {  // $parThis_0012
        I_parOut_0012 = 1;
    }
    I_parThis_0012 = I_rung_top;
    /* start series [ */
    if (!U_RStartDesabSVOFF) {  // RStartDesabSVOFF
        I_parThis_0012 = 0;
    }
    
    if (I_parThis_0012) {  // $parThis_0012
        if (!I_oneShot_0016) {  // $oneShot_0016
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "SVOFF", &VAR_NULL);
                I_oneShot_0016 = I_parThis_0012;
            }
            I_parThis_0012 = 0;
            I_oneShot_0017 = I_parThis_0012;
        }
        if (!I_oneShot_0017) {  // $oneShot_0017
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0017 = 1;
            } else {
                I_parThis_0012 = 0;
            }
        }
    } else {
        I_oneShot_0016 = I_parThis_0012;
    }
    
    /* start parallel [ */
    I_parOut_0015 = 0;
    I_parThis_0015 = I_parThis_0012;
    /* start series [ */
    I_scratch = I_parThis_0015;
    if (I_oneShot_0018) {  // $oneShot_0018
        I_parThis_0015 = 0;
    }
    I_oneShot_0018 = I_scratch;
    
    if (I_parThis_0015) {  // $parThis_0015
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0015) {  // $parThis_0015
        I_parOut_0015 = 1;
    }
    I_parThis_0015 = I_parThis_0012;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0015 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0016 = 0;
    I_parThis_0016 = I_parThis_0015;
    if (I_parThis_0016) {  // $parThis_0016
        U_RStartDesabSrv = 0;
    }
    
    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_parThis_0015;
    if (I_parThis_0016) {  // $parThis_0016
        U_RMaqCfgSrvOK = 0;
    }
    
    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_parThis_0015;
    if (I_parThis_0016) {  // $parThis_0016
        U_RMaqSerialBusy = 0;
    }
    
    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0015 = I_parOut_0016;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0015) {  // $parThis_0015
        I_parOut_0015 = 1;
    }
    I_parThis_0012 = I_parOut_0015;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0012) {  // $parThis_0012
        I_parOut_0012 = 1;
    }
    I_rung_top = I_parOut_0012;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 105 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanZerarEnc) {  // RAplanZerarEnc
        I_rung_top = 0;
    }
    
    if (!U_RSrvMotorParado) {  // RSrvMotorParado
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0017 = 0;
    I_parThis_0017 = I_rung_top;
    if (I_parThis_0017) {  // $parThis_0017
        U_RStartZerarEnc = 1;
    }
    
    if (I_parThis_0017) {  // $parThis_0017
        I_parOut_0017 = 1;
    }
    I_parThis_0017 = I_rung_top;
    if (I_parThis_0017) {  // $parThis_0017
        U_RMaqSerialBusy = 1;
    }
    
    if (I_parThis_0017) {  // $parThis_0017
        I_parOut_0017 = 1;
    }
    I_parThis_0017 = I_rung_top;
    if (I_parThis_0017) {  // $parThis_0017
        U_EncPosZero = 0;
    }
    
    if (I_parThis_0017) {  // $parThis_0017
        I_parOut_0017 = 1;
    }
    I_rung_top = I_parOut_0017;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 106 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartZerarEnc) {  // RStartZerarEnc
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0019) {  // $oneShot_0019
        I_rung_top = 0;
    }
    I_oneShot_0019 = I_scratch;
    
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    if (U_RMaqForaPosic) {  // RMaqForaPosic
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_EncPosZero = U_DifPosic;
    }
    
    /* ] finish series */
    
    /* start rung 107 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartZerarEnc) {  // RStartZerarEnc
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_001a) {  // $oneShot_001a
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "ZSET%d", &U_EncPosZero);
                I_oneShot_001a = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_001b = I_rung_top;
        }
        if (!I_oneShot_001b) {  // $oneShot_001b
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_001b = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_001a = I_rung_top;
    }
    
    /* start parallel [ */
    I_parOut_0018 = 0;
    I_parThis_0018 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0018;
    if (I_oneShot_001c) {  // $oneShot_001c
        I_parThis_0018 = 0;
    }
    I_oneShot_001c = I_scratch;
    
    if (I_parThis_0018) {  // $parThis_0018
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0018) {  // $parThis_0018
        I_parOut_0018 = 1;
    }
    I_parThis_0018 = I_rung_top;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0018 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0019 = 0;
    I_parThis_0019 = I_parThis_0018;
    if (I_parThis_0019) {  // $parThis_0019
        U_RAplanZerarEnc = 0;
    }
    
    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_parThis_0019 = I_parThis_0018;
    if (I_parThis_0019) {  // $parThis_0019
        U_RStartZerarEnc = 0;
    }
    
    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_parThis_0019 = I_parThis_0018;
    if (I_parThis_0019) {  // $parThis_0019
        U_RMaqSerialBusy = 0;
    }
    
    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_parThis_0019 = I_parThis_0018;
    /* start series [ */
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_parThis_0019 = 0;
    }
    
    /* start parallel [ */
    I_parOut_001a = 0;
    I_parThis_001a = I_parThis_0019;
    if (I_parThis_001a) {  // $parThis_001a
        ENC_Reset();
    }
    
    if (I_parThis_001a) {  // $parThis_001a
        I_parOut_001a = 1;
    }
    I_parThis_001a = I_parThis_0019;
    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 1))) {  // XDbgDeslReadEnc
        I_parThis_001a = 0;
    }
    
    if (I_parThis_001a) {  // $parThis_001a
        U_EMaqPosAtual = 0;
    }
    
    /* ] finish series */
    if (I_parThis_001a) {  // $parThis_001a
        I_parOut_001a = 1;
    }
    I_parThis_0019 = I_parOut_001a;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_parThis_0018 = I_parOut_0019;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0018) {  // $parThis_0018
        I_parOut_0018 = 1;
    }
    I_rung_top = I_parOut_0018;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 109 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }
    
    if (GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_001b = 0;
    I_parThis_001b = I_rung_top;
    if (!U_RAplanAbre) {  // RAplanAbre
        I_parThis_001b = 0;
    }
    
    if (I_parThis_001b) {  // $parThis_001b
        I_parOut_001b = 1;
    }
    I_parThis_001b = I_rung_top;
    if (!U_RAplanFecha) {  // RAplanFecha
        I_parThis_001b = 0;
    }
    
    if (I_parThis_001b) {  // $parThis_001b
        I_parOut_001b = 1;
    }
    I_parThis_001b = I_rung_top;
    if (!U_RAplanSobe) {  // RAplanSobe
        I_parThis_001b = 0;
    }
    
    if (I_parThis_001b) {  // $parThis_001b
        I_parOut_001b = 1;
    }
    I_parThis_001b = I_rung_top;
    if (!U_RAplanDesce) {  // RAplanDesce
        I_parThis_001b = 0;
    }
    
    if (I_parThis_001b) {  // $parThis_001b
        I_parOut_001b = 1;
    }
    I_rung_top = I_parOut_001b;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 1;
    }
    
    /* ] finish series */
    
    /* start rung 110 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_001c = 0;
    I_parThis_001c = I_rung_top;
    /* start series [ */
    if (!GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        I_parThis_001c = 0;
    }
    
    if (GPIO_INPUT_PORT13) {  // XBombaHidrLigada
        I_parThis_001c = 0;
    }
    
    if (I_parThis_001c) {  // $parThis_001c
        if (U_TBombaHidrTO < 499) {
            U_TBombaHidrTO++;
            I_parThis_001c = 0;
        }
    } else {
        U_TBombaHidrTO = 0;
    }
    
    /* ] finish series */
    if (I_parThis_001c) {  // $parThis_001c
        I_parOut_001c = 1;
    }
    I_parThis_001c = I_rung_top;
    if (GPIO_INPUT_PORT2) {  // XTermBombaHidr
        I_parThis_001c = 0;
    }
    
    if (I_parThis_001c) {  // $parThis_001c
        I_parOut_001c = 1;
    }
    I_rung_top = I_parOut_001c;
    /* ] finish parallel */
    U_RErroBombaHidr = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 111 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_OUTPUT_PORT1) {  // YLigarBombaHidr
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TBombaHidrEstab < 299) {
            U_TBombaHidrEstab++;
            I_rung_top = 0;
        }
    } else {
        U_TBombaHidrEstab = 0;
    }
    
    U_RBombaHidrOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 112 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_001d = 0;
    I_parThis_001d = I_rung_top;
    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_parThis_001d = 0;
    }
    
    if (U_RAplanSobe) {  // RAplanSobe
        I_parThis_001d = 0;
    }
    
    if (U_RAplanDesce) {  // RAplanDesce
        I_parThis_001d = 0;
    }
    
    if (U_RAplanAbre) {  // RAplanAbre
        I_parThis_001d = 0;
    }
    
    if (U_RAplanFecha) {  // RAplanFecha
        I_parThis_001d = 0;
    }
    
    if (I_parThis_001d) {  // $parThis_001d
        if (U_TBombaHidrInat < 29999) {
            U_TBombaHidrInat++;
            I_parThis_001d = 0;
        }
    } else {
        U_TBombaHidrInat = 0;
    }
    
    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_001d = 0;
    }
    
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_rung_top = I_parOut_001d;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 0;
    }
    
    /* ] finish series */
    
    /* start rung 114 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }
    
    if (!U_RAplanAbre) {  // RAplanAbre
        I_rung_top = 0;
    }
    
    if (GPIO_OUTPUT_PORT3) {  // YHidrFecha
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT2 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 115 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }
    
    if (!U_RAplanFecha) {  // RAplanFecha
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_001e = 0;
    I_parThis_001e = I_rung_top;
    /* start series [ */
    if (U_RAplanFechada) {  // RAplanFechada
        I_parThis_001e = 0;
    }
    
    GPIO_OUTPUT_PORT3 = I_parThis_001e;
    
    /* ] finish series */
    if (I_parThis_001e) {  // $parThis_001e
        I_parOut_001e = 1;
    }
    I_parThis_001e = I_rung_top;
    /* start series [ */
    if (!U_RAplanFechada) {  // RAplanFechada
        I_parThis_001e = 0;
    }
    
    if (I_parThis_001e) {  // $parThis_001e
        MODBUS_REGISTER[4] &= ~(1 << 3);  // YMbAplanFecha
    }
    
    /* ] finish series */
    if (I_parThis_001e) {  // $parThis_001e
        I_parOut_001e = 1;
    }
    I_rung_top = I_parOut_001e;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 116 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }
    
    if (!U_RAplanSobe) {  // RAplanSobe
        I_rung_top = 0;
    }
    
    if (GPIO_OUTPUT_PORT9) {  // YHidrDesce
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT8 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 117 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }
    
    if (!U_RAplanDesce) {  // RAplanDesce
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT9 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 118 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_001f = 0;
    I_parThis_001f = I_rung_top;
    if (!GPIO_OUTPUT_PORT2) {  // YHidrAbre
        I_parThis_001f = 0;
    }
    
    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_parThis_001f = I_rung_top;
    if (!GPIO_OUTPUT_PORT3) {  // YHidrFecha
        I_parThis_001f = 0;
    }
    
    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_parThis_001f = I_rung_top;
    if (!GPIO_OUTPUT_PORT9) {  // YHidrDesce
        I_parThis_001f = 0;
    }
    
    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_parThis_001f = I_rung_top;
    if (!GPIO_OUTPUT_PORT8) {  // YHidrSobe
        I_parThis_001f = 0;
    }
    
    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_rung_top = I_parOut_001f;
    /* ] finish parallel */
    GPIO_OUTPUT_PORT10 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 119 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanExtSubir) {  // RAplanExtSubir
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0020 = 0;
    I_parThis_0020 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0020;
    if (I_oneShot_001d) {  // $oneShot_001d
        I_parThis_0020 = 0;
    }
    I_oneShot_001d = I_scratch;
    
    /* start parallel [ */
    I_parOut_0021 = 0;
    I_parThis_0021 = I_parThis_0020;
    if (I_parThis_0021) {  // $parThis_0021
        GPIO_OUTPUT_PORT4 = 1;
    }
    
    if (I_parThis_0021) {  // $parThis_0021
        I_parOut_0021 = 1;
    }
    I_parThis_0021 = I_parThis_0020;
    if (I_parThis_0021) {  // $parThis_0021
        GPIO_OUTPUT_PORT5 = 0;
    }
    
    if (I_parThis_0021) {  // $parThis_0021
        I_parOut_0021 = 1;
    }
    I_parThis_0020 = I_parOut_0021;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0020) {  // $parThis_0020
        I_parOut_0020 = 1;
    }
    I_parThis_0020 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0020;
    if (!I_parThis_0020) {  // $parThis_0020
        if (I_oneShot_001e) {  // $oneShot_001e
            I_parThis_0020 = 1;
        }
    } else {
        I_parThis_0020 = 0;
    }
    I_oneShot_001e = I_scratch;
    
    /* start parallel [ */
    I_parOut_0022 = 0;
    I_parThis_0022 = I_parThis_0020;
    if (I_parThis_0022) {  // $parThis_0022
        GPIO_OUTPUT_PORT4 = 0;
    }
    
    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_parThis_0020;
    if (I_parThis_0022) {  // $parThis_0022
        GPIO_OUTPUT_PORT5 = 1;
    }
    
    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0020 = I_parOut_0022;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0020) {  // $parThis_0020
        I_parOut_0020 = 1;
    }
    I_rung_top = I_parOut_0020;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 120 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0023 = 0;
    I_parThis_0023 = I_rung_top;
    if (!GPIO_OUTPUT_PORT4) {  // YAplanExtSobe
        I_parThis_0023 = 0;
    }
    
    if (I_parThis_0023) {  // $parThis_0023
        I_parOut_0023 = 1;
    }
    I_parThis_0023 = I_rung_top;
    if (!GPIO_OUTPUT_PORT5) {  // YAplanExtDesce
        I_parThis_0023 = 0;
    }
    
    if (I_parThis_0023) {  // $parThis_0023
        I_parOut_0023 = 1;
    }
    I_rung_top = I_parOut_0023;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        if (U_TEspAtuarExt < 99) {
            U_TEspAtuarExt++;
            I_rung_top = 0;
        }
    } else {
        U_TEspAtuarExt = 0;
    }
    
    /* start parallel [ */
    I_parOut_0024 = 0;
    I_parThis_0024 = I_rung_top;
    if (I_parThis_0024) {  // $parThis_0024
        GPIO_OUTPUT_PORT4 = 0;
    }
    
    if (I_parThis_0024) {  // $parThis_0024
        I_parOut_0024 = 1;
    }
    I_parThis_0024 = I_rung_top;
    if (I_parThis_0024) {  // $parThis_0024
        GPIO_OUTPUT_PORT5 = 0;
    }
    
    if (I_parThis_0024) {  // $parThis_0024
        I_parOut_0024 = 1;
    }
    I_rung_top = I_parOut_0024;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 121 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanExpandir) {  // RAplanExpandir
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0025 = 0;
    I_parThis_0025 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0025;
    if (I_oneShot_001f) {  // $oneShot_001f
        I_parThis_0025 = 0;
    }
    I_oneShot_001f = I_scratch;
    
    /* start parallel [ */
    I_parOut_0026 = 0;
    I_parThis_0026 = I_parThis_0025;
    if (I_parThis_0026) {  // $parThis_0026
        GPIO_OUTPUT_PORT6 = 1;
    }
    
    if (I_parThis_0026) {  // $parThis_0026
        I_parOut_0026 = 1;
    }
    I_parThis_0026 = I_parThis_0025;
    if (I_parThis_0026) {  // $parThis_0026
        GPIO_OUTPUT_PORT7 = 0;
    }
    
    if (I_parThis_0026) {  // $parThis_0026
        I_parOut_0026 = 1;
    }
    I_parThis_0025 = I_parOut_0026;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0025) {  // $parThis_0025
        I_parOut_0025 = 1;
    }
    I_parThis_0025 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0025;
    if (!I_parThis_0025) {  // $parThis_0025
        if (I_oneShot_0020) {  // $oneShot_0020
            I_parThis_0025 = 1;
        }
    } else {
        I_parThis_0025 = 0;
    }
    I_oneShot_0020 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0027 = 0;
    I_parThis_0027 = I_parThis_0025;
    if (I_parThis_0027) {  // $parThis_0027
        GPIO_OUTPUT_PORT6 = 0;
    }
    
    if (I_parThis_0027) {  // $parThis_0027
        I_parOut_0027 = 1;
    }
    I_parThis_0027 = I_parThis_0025;
    if (I_parThis_0027) {  // $parThis_0027
        GPIO_OUTPUT_PORT7 = 1;
    }
    
    if (I_parThis_0027) {  // $parThis_0027
        I_parOut_0027 = 1;
    }
    I_parThis_0025 = I_parOut_0027;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0025) {  // $parThis_0025
        I_parOut_0025 = 1;
    }
    I_rung_top = I_parOut_0025;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 122 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT7) {  // XAplanExtRecuada
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0028 = 0;
    I_parThis_0028 = I_rung_top;
    I_scratch = I_parThis_0028;
    if (I_oneShot_0021) {  // $oneShot_0021
        I_parThis_0028 = 0;
    }
    I_oneShot_0021 = I_scratch;
    
    if (I_parThis_0028) {  // $parThis_0028
        I_parOut_0028 = 1;
    }
    I_parThis_0028 = I_rung_top;
    I_scratch = I_parThis_0028;
    if (!I_parThis_0028) {  // $parThis_0028
        if (I_oneShot_0022) {  // $oneShot_0022
            I_parThis_0028 = 1;
        }
    } else {
        I_parThis_0028 = 0;
    }
    I_oneShot_0022 = I_scratch;
    
    if (I_parThis_0028) {  // $parThis_0028
        I_parOut_0028 = 1;
    }
    I_rung_top = I_parOut_0028;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0029 = 0;
    I_parThis_0029 = I_rung_top;
    if (I_parThis_0029) {  // $parThis_0029
        GPIO_OUTPUT_PORT6 = 0;
    }
    
    if (I_parThis_0029) {  // $parThis_0029
        I_parOut_0029 = 1;
    }
    I_parThis_0029 = I_rung_top;
    if (I_parThis_0029) {  // $parThis_0029
        GPIO_OUTPUT_PORT7 = 0;
    }
    
    if (I_parThis_0029) {  // $parThis_0029
        I_parOut_0029 = 1;
    }
    I_rung_top = I_parOut_0029;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 124 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_002a = 0;
    I_parThis_002a = I_rung_top;
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_002a = 0;
    }
    
    if (I_parThis_002a) {  // $parThis_002a
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_parThis_002a = 0;
    }
    
    if (!I_TEspFimCfgJog_antiglitch) {  // $TEspFimCfgJog_antiglitch
        U_TEspFimCfgJog = 4;
    }
    I_TEspFimCfgJog_antiglitch = 1;
    if (!I_parThis_002a) {  // $parThis_002a
        if (U_TEspFimCfgJog < 4) {
            U_TEspFimCfgJog++;
            I_parThis_002a = 1;
        }
    } else {
        U_TEspFimCfgJog = 0;
    }
    
    if (!U_RMaqOK) {  // RMaqOK
        I_parThis_002a = 0;
    }
    
    /* ] finish series */
    if (I_parThis_002a) {  // $parThis_002a
        I_parOut_002a = 1;
    }
    I_rung_top = I_parOut_002a;
    /* ] finish parallel */
    if (!U_RAplanAvancoOK) {  // RAplanAvancoOK
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    U_RAplanManualOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 125 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanManualOK) {  // RAplanManualOK
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_002b = 0;
    I_parThis_002b = I_rung_top;
    if (!GPIO_INPUT_PORT9) {  // XAplanManAvanca
        I_parThis_002b = 0;
    }
    
    if (I_parThis_002b) {  // $parThis_002b
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_rung_top;
    if (!(MODBUS_REGISTER[4] & (1 << 0))) {  // YAplanManAvcIHM
        I_parThis_002b = 0;
    }
    
    if (I_parThis_002b) {  // $parThis_002b
        I_parOut_002b = 1;
    }
    I_rung_top = I_parOut_002b;
    /* ] finish parallel */
    if (GPIO_INPUT_PORT10) {  // XAplanManRecua
        I_rung_top = 0;
    }
    
    if (MODBUS_REGISTER[4] & (1 << 1)) {  // YAplanManRecIHM
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_002c = 0;
    I_parThis_002c = I_rung_top;
    U_RAplanAvancarMan = I_parThis_002c;
    
    if (I_parThis_002c) {  // $parThis_002c
        I_parOut_002c = 1;
    }
    I_parThis_002c = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_002c;
    if (I_oneShot_0023) {  // $oneShot_0023
        I_parThis_002c = 0;
    }
    I_oneShot_0023 = I_scratch;
    
    if (I_parThis_002c) {  // $parThis_002c
        U_RMaqCfgSrvJog = 1;
    }
    
    /* ] finish series */
    if (I_parThis_002c) {  // $parThis_002c
        I_parOut_002c = 1;
    }
    I_rung_top = I_parOut_002c;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 126 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanManualOK) {  // RAplanManualOK
        I_rung_top = 0;
    }
    
    if (GPIO_INPUT_PORT9) {  // XAplanManAvanca
        I_rung_top = 0;
    }
    
    if (MODBUS_REGISTER[4] & (1 << 0)) {  // YAplanManAvcIHM
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_002d = 0;
    I_parThis_002d = I_rung_top;
    if (!GPIO_INPUT_PORT10) {  // XAplanManRecua
        I_parThis_002d = 0;
    }
    
    if (I_parThis_002d) {  // $parThis_002d
        I_parOut_002d = 1;
    }
    I_parThis_002d = I_rung_top;
    if (!(MODBUS_REGISTER[4] & (1 << 1))) {  // YAplanManRecIHM
        I_parThis_002d = 0;
    }
    
    if (I_parThis_002d) {  // $parThis_002d
        I_parOut_002d = 1;
    }
    I_rung_top = I_parOut_002d;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_002e = 0;
    I_parThis_002e = I_rung_top;
    U_RAplanRecuarMan = I_parThis_002e;
    
    if (I_parThis_002e) {  // $parThis_002e
        I_parOut_002e = 1;
    }
    I_parThis_002e = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_002e;
    if (I_oneShot_0024) {  // $oneShot_0024
        I_parThis_002e = 0;
    }
    I_oneShot_0024 = I_scratch;
    
    if (I_parThis_002e) {  // $parThis_002e
        U_RMaqCfgSrvJog = 1;
    }
    
    /* ] finish series */
    if (I_parThis_002e) {  // $parThis_002e
        I_parOut_002e = 1;
    }
    I_rung_top = I_parOut_002e;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 127 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_002f = 0;
    I_parThis_002f = I_rung_top;
    /* start series [ */
    if (!U_RAplanAvancarMan) {  // RAplanAvancarMan
        I_parThis_002f = 0;
    }
    
    I_scratch = I_parThis_002f;
    if (!I_parThis_002f) {  // $parThis_002f
        if (I_oneShot_0025) {  // $oneShot_0025
            I_parThis_002f = 1;
        }
    } else {
        I_parThis_002f = 0;
    }
    I_oneShot_0025 = I_scratch;
    
    if (U_RAplanRecuarMan) {  // RAplanRecuarMan
        I_parThis_002f = 0;
    }
    
    /* ] finish series */
    if (I_parThis_002f) {  // $parThis_002f
        I_parOut_002f = 1;
    }
    I_parThis_002f = I_rung_top;
    /* start series [ */
    if (!U_RAplanRecuarMan) {  // RAplanRecuarMan
        I_parThis_002f = 0;
    }
    
    I_scratch = I_parThis_002f;
    if (!I_parThis_002f) {  // $parThis_002f
        if (I_oneShot_0026) {  // $oneShot_0026
            I_parThis_002f = 1;
        }
    } else {
        I_parThis_002f = 0;
    }
    I_oneShot_0026 = I_scratch;
    
    if (U_RAplanAvancaMan) {  // RAplanAvancaMan
        I_parThis_002f = 0;
    }
    
    /* ] finish series */
    if (I_parThis_002f) {  // $parThis_002f
        I_parOut_002f = 1;
    }
    I_rung_top = I_parOut_002f;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_RMaqCfgSrvJog = 1;
    }
    
    /* ] finish series */
    
    /* start rung 128 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqCfgSrvJog) {  // RMaqCfgSrvJog
        I_rung_top = 0;
    }
    
    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    if (U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0030 = 0;
    I_parThis_0030 = I_rung_top;
    if (I_parThis_0030) {  // $parThis_0030
        U_RStartCfgSrvJog = 1;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    if (I_parThis_0030) {  // $parThis_0030
        U_RMaqCfgSrvJog = 0;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    if (I_parThis_0030) {  // $parThis_0030
        U_RMaqSerialBusy = 1;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    if (I_parThis_0030) {  // $parThis_0030
        U_RCfgJogFrente = 0;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    if (I_parThis_0030) {  // $parThis_0030
        U_RCfgJogRecua = 0;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    if (I_parThis_0030) {  // $parThis_0030
        U_RCfgJogParar = 0;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    /* start series [ */
    if (!U_RAplanAvancarMan) {  // RAplanAvancarMan
        I_parThis_0030 = 0;
    }
    
    if (U_RAplanRecuarMan) {  // RAplanRecuarMan
        I_parThis_0030 = 0;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        U_RCfgJogFrente = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    /* start series [ */
    if (!U_RAplanRecuarMan) {  // RAplanRecuarMan
        I_parThis_0030 = 0;
    }
    
    if (U_RAplanAvancarMan) {  // RAplanAvancarMan
        I_parThis_0030 = 0;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        U_RCfgJogRecua = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_rung_top;
    /* start series [ */
    if (U_RCfgJogFrente) {  // RCfgJogFrente
        I_parThis_0030 = 0;
    }
    
    if (U_RCfgJogRecua) {  // RCfgJogRecua
        I_parThis_0030 = 0;
    }
    
    if (I_parThis_0030) {  // $parThis_0030
        U_RCfgJogParar = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_rung_top = I_parOut_0030;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 129 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    if (!U_RCfgJogFrente) {  // RCfgJogFrente
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0031 = 0;
    I_parThis_0031 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0031;
    if (I_oneShot_0027) {  // $oneShot_0027
        I_parThis_0031 = 0;
    }
    I_oneShot_0027 = I_scratch;
    
    if (I_parThis_0031) {  // $parThis_0031
        U_RMaqCfgSrvOK = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0031) {  // $parThis_0031
        I_parOut_0031 = 1;
    }
    I_parThis_0031 = I_rung_top;
    /* start series [ */
    if (I_parThis_0031) {  // $parThis_0031
        if (U_TCfgSrvJogAvcTO < 99) {
            U_TCfgSrvJogAvcTO++;
            I_parThis_0031 = 0;
        }
    } else {
        U_TCfgSrvJogAvcTO = 0;
    }
    
    if (I_parThis_0031) {  // $parThis_0031
        U_RMaqCfgSrvErro = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0031) {  // $parThis_0031
        I_parOut_0031 = 1;
    }
    I_parThis_0031 = I_rung_top;
    /* start series [ */
    if (I_parThis_0031) {  // $parThis_0031
        if (!I_oneShot_0028) {  // $oneShot_0028
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "JOGP%d", &U_AplanVelManual);
                I_oneShot_0028 = I_parThis_0031;
            }
            I_parThis_0031 = 0;
            I_oneShot_0029 = I_parThis_0031;
        }
        if (!I_oneShot_0029) {  // $oneShot_0029
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0029 = 1;
            } else {
                I_parThis_0031 = 0;
            }
        }
    } else {
        I_oneShot_0028 = I_parThis_0031;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_parThis_0031 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0032 = 0;
    I_parThis_0032 = I_parThis_0031;
    /* start series [ */
    I_scratch = I_parThis_0032;
    if (I_oneShot_002a) {  // $oneShot_002a
        I_parThis_0032 = 0;
    }
    I_oneShot_002a = I_scratch;
    
    if (I_parThis_0032) {  // $parThis_0032
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0032) {  // $parThis_0032
        I_parOut_0032 = 1;
    }
    I_parThis_0032 = I_parThis_0031;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0032 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0033 = 0;
    I_parThis_0033 = I_parThis_0032;
    if (I_parThis_0033) {  // $parThis_0033
        U_RStartCfgSrvJog = 0;
    }
    
    if (I_parThis_0033) {  // $parThis_0033
        I_parOut_0033 = 1;
    }
    I_parThis_0033 = I_parThis_0032;
    if (I_parThis_0033) {  // $parThis_0033
        U_RMaqCfgSrvOK = 1;
    }
    
    if (I_parThis_0033) {  // $parThis_0033
        I_parOut_0033 = 1;
    }
    I_parThis_0032 = I_parOut_0033;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0032) {  // $parThis_0032
        I_parOut_0032 = 1;
    }
    I_parThis_0031 = I_parOut_0032;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0031) {  // $parThis_0031
        I_parOut_0031 = 1;
    }
    I_rung_top = I_parOut_0031;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 130 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    if (!U_RCfgJogRecua) {  // RCfgJogRecua
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0034 = 0;
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0034;
    if (I_oneShot_002b) {  // $oneShot_002b
        I_parThis_0034 = 0;
    }
    I_oneShot_002b = I_scratch;
    
    if (I_parThis_0034) {  // $parThis_0034
        U_RMaqCfgSrvOK = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (I_parThis_0034) {  // $parThis_0034
        if (U_TCfgSrvJogRecTO < 99) {
            U_TCfgSrvJogRecTO++;
            I_parThis_0034 = 0;
        }
    } else {
        U_TCfgSrvJogRecTO = 0;
    }
    
    if (I_parThis_0034) {  // $parThis_0034
        U_RMaqCfgSrvErro = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (I_parThis_0034) {  // $parThis_0034
        if (!I_oneShot_002c) {  // $oneShot_002c
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "JOGN%d", &U_AplanVelManual);
                I_oneShot_002c = I_parThis_0034;
            }
            I_parThis_0034 = 0;
            I_oneShot_002d = I_parThis_0034;
        }
        if (!I_oneShot_002d) {  // $oneShot_002d
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_002d = 1;
            } else {
                I_parThis_0034 = 0;
            }
        }
    } else {
        I_oneShot_002c = I_parThis_0034;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_parThis_0034 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0035 = 0;
    I_parThis_0035 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0035;
    if (I_oneShot_002e) {  // $oneShot_002e
        I_parThis_0035 = 0;
    }
    I_oneShot_002e = I_scratch;
    
    if (I_parThis_0035) {  // $parThis_0035
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0035) {  // $parThis_0035
        I_parOut_0035 = 1;
    }
    I_parThis_0035 = I_parThis_0034;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0035 = 0;
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
    I_parThis_0034 = I_parOut_0035;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_rung_top = I_parOut_0034;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 131 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    if (!U_RCfgJogParar) {  // RCfgJogParar
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0037 = 0;
    I_parThis_0037 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0037;
    if (I_oneShot_002f) {  // $oneShot_002f
        I_parThis_0037 = 0;
    }
    I_oneShot_002f = I_scratch;
    
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
        if (U_TCfgSrvJogDesTO < 99) {
            U_TCfgSrvJogDesTO++;
            I_parThis_0037 = 0;
        }
    } else {
        U_TCfgSrvJogDesTO = 0;
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
        if (!I_oneShot_0030) {  // $oneShot_0030
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "SKIP", &VAR_NULL);
                I_oneShot_0030 = I_parThis_0037;
            }
            I_parThis_0037 = 0;
            I_oneShot_0031 = I_parThis_0037;
        }
        if (!I_oneShot_0031) {  // $oneShot_0031
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0031 = 1;
            } else {
                I_parThis_0037 = 0;
            }
        }
    } else {
        I_oneShot_0030 = I_parThis_0037;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_parThis_0037 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0038 = 0;
    I_parThis_0038 = I_parThis_0037;
    /* start series [ */
    I_scratch = I_parThis_0038;
    if (I_oneShot_0032) {  // $oneShot_0032
        I_parThis_0038 = 0;
    }
    I_oneShot_0032 = I_scratch;
    
    if (I_parThis_0038) {  // $parThis_0038
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0038) {  // $parThis_0038
        I_parOut_0038 = 1;
    }
    I_parThis_0038 = I_parThis_0037;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0038 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0039 = 0;
    I_parThis_0039 = I_parThis_0038;
    if (I_parThis_0039) {  // $parThis_0039
        U_RStartCfgSrvJog = 0;
    }
    
    if (I_parThis_0039) {  // $parThis_0039
        I_parOut_0039 = 1;
    }
    I_parThis_0039 = I_parThis_0038;
    if (I_parThis_0039) {  // $parThis_0039
        U_RMaqCfgSrvOK = 1;
    }
    
    if (I_parThis_0039) {  // $parThis_0039
        I_parOut_0039 = 1;
    }
    I_parThis_0038 = I_parOut_0039;
    /* ] finish parallel */
    /* ] finish series */
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
    
    /* start rung 132 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartCfgSrvJog) {  // RStartCfgSrvJog
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
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
    
    /* start parallel [ */
    I_parOut_003a = 0;
    I_parThis_003a = I_rung_top;
    if (I_parThis_003a) {  // $parThis_003a
        U_RStartCfgSrvJog = 0;
    }
    
    if (I_parThis_003a) {  // $parThis_003a
        I_parOut_003a = 1;
    }
    I_parThis_003a = I_rung_top;
    if (I_parThis_003a) {  // $parThis_003a
        U_RMaqSerialBusy = 0;
    }
    
    if (I_parThis_003a) {  // $parThis_003a
        I_parOut_003a = 1;
    }
    I_rung_top = I_parOut_003a;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 136 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT12) {  // XPrsCameParar
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspCameON < 4) {
            U_TEspCameON++;
            I_rung_top = 0;
        }
    } else {
        U_TEspCameON = 0;
    }
    
    if (!I_TEspCameOFF_antiglitch) {  // $TEspCameOFF_antiglitch
        U_TEspCameOFF = 4;
    }
    I_TEspCameOFF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEspCameOFF < 4) {
            U_TEspCameOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TEspCameOFF = 0;
    }
    
    U_RPrsCameParar = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 137 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT11) {  // XPrsAvancoOK
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspAvancoON < 4) {
            U_TEspAvancoON++;
            I_rung_top = 0;
        }
    } else {
        U_TEspAvancoON = 0;
    }
    
    if (!I_TEspAvancoOFF_antiglitch) {  // $TEspAvancoOFF_antiglitch
        U_TEspAvancoOFF = 4;
    }
    I_TEspAvancoOFF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEspAvancoOFF < 4) {
            U_TEspAvancoOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TEspAvancoOFF = 0;
    }
    
    U_RPrsAvancoOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 139 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 3))) {  // XDbgPrsLigaMotor
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0034) {  // $oneShot_0034
        I_rung_top = 0;
    }
    I_oneShot_0034 = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[4] |= (1 << 8);  // YMbPrsLigaMotor
    }
    
    /* ] finish series */
    
    /* start rung 140 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_003b = 0;
    I_parThis_003b = I_rung_top;
    if (!U_RErroGeral) {  // RErroGeral
        I_parThis_003b = 0;
    }
    
    if (I_parThis_003b) {  // $parThis_003b
        I_parOut_003b = 1;
    }
    I_parThis_003b = I_rung_top;
    if (GPIO_INPUT_PORT5) {  // XInversorOK
        I_parThis_003b = 0;
    }
    
    if (I_parThis_003b) {  // $parThis_003b
        I_parOut_003b = 1;
    }
    I_rung_top = I_parOut_003b;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[4] &= ~(1 << 8);  // YMbPrsLigaMotor
    }
    
    /* ] finish series */
    
    /* start rung 141 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RPrsMotorLigado) {  // RPrsMotorLigado
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspMotorParar < 11999) {
            U_TEspMotorParar++;
            I_rung_top = 0;
        }
    } else {
        U_TEspMotorParar = 0;
    }
    
    U_RPrsMotorParado = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 142 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 4))) {  // YMbPrsSentidoInv
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_003c = 0;
    I_parThis_003c = I_rung_top;
    I_scratch = I_parThis_003c;
    if (I_oneShot_0035) {  // $oneShot_0035
        I_parThis_003c = 0;
    }
    I_oneShot_0035 = I_scratch;
    
    if (I_parThis_003c) {  // $parThis_003c
        I_parOut_003c = 1;
    }
    I_parThis_003c = I_rung_top;
    I_scratch = I_parThis_003c;
    if (!I_parThis_003c) {  // $parThis_003c
        if (I_oneShot_0036) {  // $oneShot_0036
            I_parThis_003c = 1;
        }
    } else {
        I_parThis_003c = 0;
    }
    I_oneShot_0036 = I_scratch;
    
    if (I_parThis_003c) {  // $parThis_003c
        I_parOut_003c = 1;
    }
    I_rung_top = I_parOut_003c;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        U_RPrsNovoSentido = 1;
    }
    
    /* ] finish series */
    
    /* start rung 143 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_003d = 0;
    I_parThis_003d = I_rung_top;
    if (!U_RMaqPronta) {  // RMaqPronta
        I_parThis_003d = 0;
    }
    
    if (I_parThis_003d) {  // $parThis_003d
        I_parOut_003d = 1;
    }
    I_parThis_003d = I_rung_top;
    if (!U_RPrsMotorLigado) {  // RPrsMotorLigado
        I_parThis_003d = 0;
    }
    
    if (I_parThis_003d) {  // $parThis_003d
        I_parOut_003d = 1;
    }
    I_rung_top = I_parOut_003d;
    /* ] finish parallel */
    if (!(MODBUS_REGISTER[4] & (1 << 8))) {  // YMbPrsLigaMotor
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_003e = 0;
    I_parThis_003e = I_rung_top;
    if (!U_RPrsMotorParado) {  // RPrsMotorParado
        I_parThis_003e = 0;
    }
    
    if (I_parThis_003e) {  // $parThis_003e
        I_parOut_003e = 1;
    }
    I_parThis_003e = I_rung_top;
    if (U_RPrsNovoSentido) {  // RPrsNovoSentido
        I_parThis_003e = 0;
    }
    
    if (I_parThis_003e) {  // $parThis_003e
        I_parOut_003e = 1;
    }
    I_rung_top = I_parOut_003e;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_003f = 0;
    I_parThis_003f = I_rung_top;
    /* start series [ */
    if (MODBUS_REGISTER[2] & (1 << 4)) {  // YMbPrsSentidoInv
        I_parThis_003f = 0;
    }
    
    GPIO_OUTPUT_PORT14 = I_parThis_003f;
    
    /* ] finish series */
    if (I_parThis_003f) {  // $parThis_003f
        I_parOut_003f = 1;
    }
    I_parThis_003f = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 4))) {  // YMbPrsSentidoInv
        I_parThis_003f = 0;
    }
    
    GPIO_OUTPUT_PORT13 = I_parThis_003f;
    
    /* ] finish series */
    if (I_parThis_003f) {  // $parThis_003f
        I_parOut_003f = 1;
    }
    I_parThis_003f = I_rung_top;
    if (I_parThis_003f) {  // $parThis_003f
        U_RPrsNovoSentido = 0;
    }
    
    if (I_parThis_003f) {  // $parThis_003f
        I_parOut_003f = 1;
    }
    I_rung_top = I_parOut_003f;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 144 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0040 = 0;
    I_parThis_0040 = I_rung_top;
    if (!GPIO_OUTPUT_PORT14) {  // YPrsLigaNormal
        I_parThis_0040 = 0;
    }
    
    if (I_parThis_0040) {  // $parThis_0040
        I_parOut_0040 = 1;
    }
    I_parThis_0040 = I_rung_top;
    if (!GPIO_OUTPUT_PORT13) {  // YPrsLigaInv
        I_parThis_0040 = 0;
    }
    
    if (I_parThis_0040) {  // $parThis_0040
        I_parOut_0040 = 1;
    }
    I_rung_top = I_parOut_0040;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0041 = 0;
    I_parThis_0041 = I_rung_top;
    U_RPrsMotorLigado = I_parThis_0041;
    
    if (I_parThis_0041) {  // $parThis_0041
        I_parOut_0041 = 1;
    }
    I_parThis_0041 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0041;
    if (I_oneShot_0037) {  // $oneShot_0037
        I_parThis_0041 = 0;
    }
    I_oneShot_0037 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0042 = 0;
    I_parThis_0042 = I_parThis_0041;
    if (I_parThis_0042) {  // $parThis_0042
        MODBUS_REGISTER[4] &= ~(1 << 9);  // YMbPrsIniciar
    }
    
    if (I_parThis_0042) {  // $parThis_0042
        I_parOut_0042 = 1;
    }
    I_parThis_0042 = I_parThis_0041;
    if (I_parThis_0042) {  // $parThis_0042
        U_RPrsIniciarCiclo = 0;
    }
    
    if (I_parThis_0042) {  // $parThis_0042
        I_parOut_0042 = 1;
    }
    I_parThis_0041 = I_parOut_0042;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0041) {  // $parThis_0041
        I_parOut_0041 = 1;
    }
    I_rung_top = I_parOut_0041;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 145 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPrsMotorLigado) {  // RPrsMotorLigado
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspPrsMotorOK < 1499) {
            U_TEspPrsMotorOK++;
            I_rung_top = 0;
        }
    } else {
        U_TEspPrsMotorOK = 0;
    }
    
    U_RPrsMotorOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 147 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }
    
    if (!U_RPrsMotorOK) {  // RPrsMotorOK
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0043 = 0;
    I_parThis_0043 = I_rung_top;
    /* start series [ */
    if (!GPIO_OUTPUT_PORT14) {  // YPrsLigaNormal
        I_parThis_0043 = 0;
    }
    
    if (!U_RPrsIniciarCiclo) {  // RPrsIniciarCiclo
        I_parThis_0043 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0043) {  // $parThis_0043
        I_parOut_0043 = 1;
    }
    I_parThis_0043 = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT14) {  // XPrsCmdAjuste
        I_parThis_0043 = 0;
    }
    
    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_0043 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0043) {  // $parThis_0043
        I_parOut_0043 = 1;
    }
    I_rung_top = I_parOut_0043;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0044 = 0;
    I_parThis_0044 = I_rung_top;
    if (I_parThis_0044) {  // $parThis_0044
        MODBUS_REGISTER[4] &= ~(1 << 10);  // YMbPrsParar
    }
    
    if (I_parThis_0044) {  // $parThis_0044
        I_parOut_0044 = 1;
    }
    I_parThis_0044 = I_rung_top;
    if (I_parThis_0044) {  // $parThis_0044
        U_RPrsIniciarCiclo = 0;
    }
    
    if (I_parThis_0044) {  // $parThis_0044
        I_parOut_0044 = 1;
    }
    I_parThis_0044 = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT17) {  // XPrsDeslContinuo
        I_parThis_0044 = 0;
    }
    
    if (U_RBloquearPrensa) {  // RBloquearPrensa
        I_parThis_0044 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0045 = 0;
    I_parThis_0045 = I_parThis_0044;
    /* start series [ */
    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_0045 = 0;
    }
    
    if (I_parThis_0045) {  // $parThis_0045
        U_RPrsPararMartelo = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0045) {  // $parThis_0045
        I_parOut_0045 = 1;
    }
    I_parThis_0045 = I_parThis_0044;
    if (I_parThis_0045) {  // $parThis_0045
        GPIO_OUTPUT_PORT11 = 1;
    }
    
    if (I_parThis_0045) {  // $parThis_0045
        I_parOut_0045 = 1;
    }
    I_parThis_0044 = I_parOut_0045;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0044) {  // $parThis_0044
        I_parOut_0044 = 1;
    }
    I_rung_top = I_parOut_0044;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 148 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0046 = 0;
    I_parThis_0046 = I_rung_top;
    /* start series [ */
    if (!U_RPrsCameParar) {  // RPrsCameParar
        I_parThis_0046 = 0;
    }
    
    I_scratch = I_parThis_0046;
    if (I_oneShot_0038) {  // $oneShot_0038
        I_parThis_0046 = 0;
    }
    I_oneShot_0038 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0047 = 0;
    I_parThis_0047 = I_parThis_0046;
    if (!U_RPrsPararMartelo) {  // RPrsPararMartelo
        I_parThis_0047 = 0;
    }
    
    if (I_parThis_0047) {  // $parThis_0047
        I_parOut_0047 = 1;
    }
    I_parThis_0047 = I_parThis_0046;
    /* start series [ */
    if (!U_RAplanLigarMotor) {  // RAplanLigarMotor
        I_parThis_0047 = 0;
    }
    
    if (GPIO_OUTPUT_PORT16) {  // YSrvHabilita
        I_parThis_0047 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0047) {  // $parThis_0047
        I_parOut_0047 = 1;
    }
    I_parThis_0046 = I_parOut_0047;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0048 = 0;
    I_parThis_0048 = I_parThis_0046;
    if (GPIO_INPUT_PORT14) {  // XPrsCmdAjuste
        I_parThis_0048 = 0;
    }
    
    if (I_parThis_0048) {  // $parThis_0048
        I_parOut_0048 = 1;
    }
    I_parThis_0048 = I_parThis_0046;
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_0048 = 0;
    }
    
    if (I_parThis_0048) {  // $parThis_0048
        I_parOut_0048 = 1;
    }
    I_parThis_0046 = I_parOut_0048;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0046) {  // $parThis_0046
        I_parOut_0046 = 1;
    }
    I_parThis_0046 = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT14) {  // XPrsCmdAjuste
        I_parThis_0046 = 0;
    }
    
    I_scratch = I_parThis_0046;
    if (!I_parThis_0046) {  // $parThis_0046
        if (I_oneShot_0039) {  // $oneShot_0039
            I_parThis_0046 = 1;
        }
    } else {
        I_parThis_0046 = 0;
    }
    I_oneShot_0039 = I_scratch;
    
    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_0046 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0046) {  // $parThis_0046
        I_parOut_0046 = 1;
    }
    I_rung_top = I_parOut_0046;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0049 = 0;
    I_parThis_0049 = I_rung_top;
    if (I_parThis_0049) {  // $parThis_0049
        GPIO_OUTPUT_PORT11 = 0;
    }
    
    if (I_parThis_0049) {  // $parThis_0049
        I_parOut_0049 = 1;
    }
    I_parThis_0049 = I_rung_top;
    if (I_parThis_0049) {  // $parThis_0049
        U_RPrsPararMartelo = 0;
    }
    
    if (I_parThis_0049) {  // $parThis_0049
        I_parOut_0049 = 1;
    }
    I_parThis_0049 = I_rung_top;
    if (I_parThis_0049) {  // $parThis_0049
        U_RPrsIniciarCiclo = 0;
    }
    
    if (I_parThis_0049) {  // $parThis_0049
        I_parOut_0049 = 1;
    }
    I_rung_top = I_parOut_0049;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 149 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPrsMotorLigado) {  // RPrsMotorLigado
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_003a) {  // $oneShot_003a
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_003a = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT11 = 0;
    }
    
    /* ] finish series */
    
    /* start rung 151 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 5))) {  // YMbPrsCiclosNovo
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_004a = 0;
    I_parThis_004a = I_rung_top;
    if (I_parThis_004a) {  // $parThis_004a
        MODBUS_REGISTER[2] &= ~(1 << 5);  // YMbPrsCiclosNovo
    }
    
    if (I_parThis_004a) {  // $parThis_004a
        I_parOut_004a = 1;
    }
    I_parThis_004a = I_rung_top;
    if (I_parThis_004a) {  // $parThis_004a
        I_scratch2 = 1000;
        U_PrsCiclos = MODBUS_REGISTER[18] * I_scratch2;
    }
    
    if (I_parThis_004a) {  // $parThis_004a
        I_parOut_004a = 1;
    }
    I_parThis_004a = I_rung_top;
    if (I_parThis_004a) {  // $parThis_004a
        U_PrsCiclos = U_PrsCiclos + MODBUS_REGISTER[17];
    }
    
    if (I_parThis_004a) {  // $parThis_004a
        I_parOut_004a = 1;
    }
    I_rung_top = I_parOut_004a;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 152 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPrsCameParar) {  // RPrsCameParar
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_003b) {  // $oneShot_003b
        I_rung_top = 0;
    }
    I_oneShot_003b = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 1;
        U_PrsCiclos = U_PrsCiclos + I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 153 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 1000;
        if(I_scratch2) MODBUS_REGISTER[16] = U_PrsCiclos / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 154 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 1000;
        U_PrsCiclosMil = MODBUS_REGISTER[16] * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 155 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[15] = U_PrsCiclos - U_PrsCiclosMil;
    }
    
    /* ] finish series */
    
    /* start rung 157 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT15) {  // XPrsCmdBM1
        I_rung_top = 0;
    }
    
    if (!I_TEspCmdBM1_antiglitch) {  // $TEspCmdBM1_antiglitch
        U_TEspCmdBM1 = 49;
    }
    I_TEspCmdBM1_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEspCmdBM1 < 49) {
            U_TEspCmdBM1++;
            I_rung_top = 1;
        }
    } else {
        U_TEspCmdBM1 = 0;
    }
    
    /* start parallel [ */
    I_parOut_004b = 0;
    I_parThis_004b = I_rung_top;
    /* start series [ */
    if (U_RPrsCmdBM1Blq) {  // RPrsCmdBM1Blq
        I_parThis_004b = 0;
    }
    
    U_RPrsCmdBM1OK = I_parThis_004b;
    
    /* ] finish series */
    if (I_parThis_004b) {  // $parThis_004b
        I_parOut_004b = 1;
    }
    I_parThis_004b = I_rung_top;
    /* start series [ */
    if (I_parThis_004b) {  // $parThis_004b
        if (U_TTOCmdBM1 < 49) {
            U_TTOCmdBM1++;
            I_parThis_004b = 0;
        }
    } else {
        U_TTOCmdBM1 = 0;
    }
    
    U_RPrsCmdBM1Blq = I_parThis_004b;
    
    /* ] finish series */
    if (I_parThis_004b) {  // $parThis_004b
        I_parOut_004b = 1;
    }
    I_rung_top = I_parOut_004b;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 158 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT16) {  // XPrsCmdBM2
        I_rung_top = 0;
    }
    
    if (!I_TEspCmdBM2_antiglitch) {  // $TEspCmdBM2_antiglitch
        U_TEspCmdBM2 = 49;
    }
    I_TEspCmdBM2_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEspCmdBM2 < 49) {
            U_TEspCmdBM2++;
            I_rung_top = 1;
        }
    } else {
        U_TEspCmdBM2 = 0;
    }
    
    /* start parallel [ */
    I_parOut_004c = 0;
    I_parThis_004c = I_rung_top;
    /* start series [ */
    if (U_RPrsCmdBM2Blq) {  // RPrsCmdBM2Blq
        I_parThis_004c = 0;
    }
    
    U_RPrsCmdBM2OK = I_parThis_004c;
    
    /* ] finish series */
    if (I_parThis_004c) {  // $parThis_004c
        I_parOut_004c = 1;
    }
    I_parThis_004c = I_rung_top;
    /* start series [ */
    if (I_parThis_004c) {  // $parThis_004c
        if (U_TTOCmdBM2 < 49) {
            U_TTOCmdBM2++;
            I_parThis_004c = 0;
        }
    } else {
        U_TTOCmdBM2 = 0;
    }
    
    U_RPrsCmdBM2Blq = I_parThis_004c;
    
    /* ] finish series */
    if (I_parThis_004c) {  // $parThis_004c
        I_parOut_004c = 1;
    }
    I_rung_top = I_parOut_004c;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 159 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPrsCmdBM1OK) {  // RPrsCmdBM1OK
        I_rung_top = 0;
    }
    
    if (!U_RPrsCmdBM2OK) {  // RPrsCmdBM2OK
        I_rung_top = 0;
    }
    
    if (!GPIO_INPUT_PORT17) {  // XPrsDeslContinuo
        I_rung_top = 0;
    }
    
    U_RPrsCmdBMOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 161 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }
    
    if (!U_RPrsMotorOK) {  // RPrsMotorOK
        I_rung_top = 0;
    }
    
    if (!GPIO_OUTPUT_PORT14) {  // YPrsLigaNormal
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_004d = 0;
    I_parThis_004d = I_rung_top;
    if (!(MODBUS_REGISTER[4] & (1 << 9))) {  // YMbPrsIniciar
        I_parThis_004d = 0;
    }
    
    if (I_parThis_004d) {  // $parThis_004d
        I_parOut_004d = 1;
    }
    I_parThis_004d = I_rung_top;
    if (!U_RPrsCmdBMOK) {  // RPrsCmdBMOK
        I_parThis_004d = 0;
    }
    
    if (I_parThis_004d) {  // $parThis_004d
        I_parOut_004d = 1;
    }
    I_rung_top = I_parOut_004d;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_004e = 0;
    I_parThis_004e = I_rung_top;
    if (I_parThis_004e) {  // $parThis_004e
        U_RPrsIniciarCiclo = 1;
    }
    
    if (I_parThis_004e) {  // $parThis_004e
        I_parOut_004e = 1;
    }
    I_parThis_004e = I_rung_top;
    if (I_parThis_004e) {  // $parThis_004e
        MODBUS_REGISTER[4] &= ~(1 << 9);  // YMbPrsIniciar
    }
    
    if (I_parThis_004e) {  // $parThis_004e
        I_parOut_004e = 1;
    }
    I_parThis_004e = I_rung_top;
    if (I_parThis_004e) {  // $parThis_004e
        U_RBloquearPrensa = 0;
    }
    
    if (I_parThis_004e) {  // $parThis_004e
        I_parOut_004e = 1;
    }
    I_rung_top = I_parOut_004e;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 163 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_OUTPUT_PORT11) {  // YPrsEmbreagem
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_004f = 0;
    I_parThis_004f = I_rung_top;
    if (GPIO_INPUT_PORT17) {  // XPrsDeslContinuo
        I_parThis_004f = 0;
    }
    
    if (I_parThis_004f) {  // $parThis_004f
        I_parOut_004f = 1;
    }
    I_parThis_004f = I_rung_top;
    if (!(MODBUS_REGISTER[4] & (1 << 10))) {  // YMbPrsParar
        I_parThis_004f = 0;
    }
    
    if (I_parThis_004f) {  // $parThis_004f
        I_parOut_004f = 1;
    }
    I_rung_top = I_parOut_004f;
    /* ] finish parallel */
    I_scratch = I_rung_top;
    if (I_oneShot_003c) {  // $oneShot_003c
        I_rung_top = 0;
    }
    I_oneShot_003c = I_scratch;
    
    /* start parallel [ */
    I_parOut_0050 = 0;
    I_parThis_0050 = I_rung_top;
    if (I_parThis_0050) {  // $parThis_0050
        U_RPrsPararMartelo = 1;
    }
    
    if (I_parThis_0050) {  // $parThis_0050
        I_parOut_0050 = 1;
    }
    I_parThis_0050 = I_rung_top;
    if (I_parThis_0050) {  // $parThis_0050
        MODBUS_REGISTER[4] &= ~(1 << 10);  // YMbPrsParar
    }
    
    if (I_parThis_0050) {  // $parThis_0050
        I_parOut_0050 = 1;
    }
    I_rung_top = I_parOut_0050;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 164 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT17) {  // XPrsDeslContinuo
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_003d) {  // $oneShot_003d
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_003d = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        U_RBloquearPrensa = 1;
    }
    
    /* ] finish series */
    
    /* start rung 166 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (MODBUS_REGISTER[1] & (1 << 0)) {  // YMbModoAutoAtivo
        I_rung_top = 0;
    }
    
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }
    
    if (GPIO_OUTPUT_PORT11) {  // YPrsEmbreagem
        I_rung_top = 0;
    }
    
    GPIO_OUTPUT_PORT12 = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 169 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 6))) {  // XDbgParamSync
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_003e) {  // $oneShot_003e
        I_rung_top = 0;
    }
    I_oneShot_003e = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 3);  // YMbParamSync
    }
    
    /* ] finish series */
    
    /* start rung 170 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0051 = 0;
    I_parThis_0051 = I_rung_top;
    if (!(MODBUS_REGISTER[2] & (1 << 3))) {  // YMbParamSync
        I_parThis_0051 = 0;
    }
    
    if (I_parThis_0051) {  // $parThis_0051
        I_parOut_0051 = 1;
    }
    I_parThis_0051 = I_rung_top;
    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_parThis_0051 = 0;
    }
    
    I_scratch = I_parThis_0051;
    if (I_oneShot_003f) {  // $oneShot_003f
        I_parThis_0051 = 0;
    }
    I_oneShot_003f = I_scratch;
    
    if (U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_parThis_0051 = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0051) {  // $parThis_0051
        I_parOut_0051 = 1;
    }
    I_rung_top = I_parOut_0051;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0052 = 0;
    I_parThis_0052 = I_rung_top;
    if (I_parThis_0052) {  // $parThis_0052
        U_RMaqCfgSrv = 1;
    }
    
    if (I_parThis_0052) {  // $parThis_0052
        I_parOut_0052 = 1;
    }
    I_parThis_0052 = I_rung_top;
    if (I_parThis_0052) {  // $parThis_0052
        MODBUS_REGISTER[2] &= ~(1 << 3);  // YMbParamSync
    }
    
    if (I_parThis_0052) {  // $parThis_0052
        I_parOut_0052 = 1;
    }
    I_rung_top = I_parOut_0052;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 171 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqCfgSrv) {  // RMaqCfgSrv
        I_rung_top = 0;
    }
    
    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0053 = 0;
    I_parThis_0053 = I_rung_top;
    if (I_parThis_0053) {  // $parThis_0053
        U_RStartCfgSrv = 1;
    }
    
    if (I_parThis_0053) {  // $parThis_0053
        I_parOut_0053 = 1;
    }
    I_parThis_0053 = I_rung_top;
    if (I_parThis_0053) {  // $parThis_0053
        U_RMaqCfgSrv = 0;
    }
    
    if (I_parThis_0053) {  // $parThis_0053
        I_parOut_0053 = 1;
    }
    I_parThis_0053 = I_rung_top;
    if (I_parThis_0053) {  // $parThis_0053
        U_RMaqSerialBusy = 1;
    }
    
    if (I_parThis_0053) {  // $parThis_0053
        I_parOut_0053 = 1;
    }
    I_rung_top = I_parOut_0053;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 172 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RStartCfgSrv) {  // RStartCfgSrv
        I_rung_top = 0;
    }
    
    if (U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0054 = 0;
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0054;
    if (I_oneShot_0040) {  // $oneShot_0040
        I_parThis_0054 = 0;
    }
    I_oneShot_0040 = I_scratch;
    
    if (I_parThis_0054) {  // $parThis_0054
        U_RMaqCfgSrvOK = 0;
    }
    
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0054;
    if (!I_parThis_0054) {  // $parThis_0054
        if (I_oneShot_0041) {  // $oneShot_0041
            I_parThis_0054 = 1;
        }
    } else {
        I_parThis_0054 = 0;
    }
    I_oneShot_0041 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0055 = 0;
    I_parThis_0055 = I_parThis_0054;
    if (I_parThis_0055) {  // $parThis_0055
        U_RCfgSrvParP1OK = 0;
    }
    
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0055 = I_parThis_0054;
    if (I_parThis_0055) {  // $parThis_0055
        U_RCfgSrvParP2OK = 0;
    }
    
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0055 = I_parThis_0054;
    if (I_parThis_0055) {  // $parThis_0055
        U_RCfgSrvParP3OK = 0;
    }
    
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0055 = I_parThis_0054;
    if (I_parThis_0055) {  // $parThis_0055
        U_RCfgSrvParP4OK = 0;
    }
    
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0055 = I_parThis_0054;
    if (I_parThis_0055) {  // $parThis_0055
        U_RCfgSrvParP5OK = 0;
    }
    
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0055 = I_parThis_0054;
    if (I_parThis_0055) {  // $parThis_0055
        U_RCfgSrvParP6OK = 0;
    }
    
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0055 = I_parThis_0054;
    if (I_parThis_0055) {  // $parThis_0055
        U_RCfgSrvParP7OK = 0;
    }
    
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0055 = I_parThis_0054;
    if (I_parThis_0055) {  // $parThis_0055
        U_RMaqSerialBusy = 0;
    }
    
    if (I_parThis_0055) {  // $parThis_0055
        I_parOut_0055 = 1;
    }
    I_parThis_0054 = I_parOut_0055;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (I_parThis_0054) {  // $parThis_0054
        if (U_TCfgSrvIniTO < 1499) {
            U_TCfgSrvIniTO++;
            I_parThis_0054 = 0;
        }
    } else {
        U_TCfgSrvIniTO = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        U_RMaqCfgSrvErro = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (U_RCfgSrvParP1OK) {  // RCfgSrvParP1OK
        I_parThis_0054 = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (!I_oneShot_0042) {  // $oneShot_0042
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "RES", &VAR_NULL);
                I_oneShot_0042 = I_parThis_0054;
            }
            I_parThis_0054 = 0;
            I_oneShot_0043 = I_parThis_0054;
        }
        if (!I_oneShot_0043) {  // $oneShot_0043
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0043 = 1;
            } else {
                I_parThis_0054 = 0;
            }
        }
    } else {
        I_oneShot_0042 = I_parThis_0054;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (U_TCfgSrvParP1Fim < 499) {
            U_TCfgSrvParP1Fim++;
            I_parThis_0054 = 0;
        }
    } else {
        U_TCfgSrvParP1Fim = 0;
    }
    
    /* start parallel [ */
    I_parOut_0056 = 0;
    I_parThis_0056 = I_parThis_0054;
    /* start series [ */
    I_scratch = I_parThis_0056;
    if (I_oneShot_0044) {  // $oneShot_0044
        I_parThis_0056 = 0;
    }
    I_oneShot_0044 = I_scratch;
    
    if (I_parThis_0056) {  // $parThis_0056
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0056) {  // $parThis_0056
        I_parOut_0056 = 1;
    }
    I_parThis_0056 = I_parThis_0054;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0056 = 0;
    }
    
    if (I_parThis_0056) {  // $parThis_0056
        U_RCfgSrvParP1OK = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0056) {  // $parThis_0056
        I_parOut_0056 = 1;
    }
    I_parThis_0054 = I_parOut_0056;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (!U_RCfgSrvParP1OK) {  // RCfgSrvParP1OK
        I_parThis_0054 = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (!I_oneShot_0045) {  // $oneShot_0045
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "ARES", &VAR_NULL);
                I_oneShot_0045 = I_parThis_0054;
            }
            I_parThis_0054 = 0;
            I_oneShot_0046 = I_parThis_0054;
        }
        if (!I_oneShot_0046) {  // $oneShot_0046
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0046 = 1;
            } else {
                I_parThis_0054 = 0;
            }
        }
    } else {
        I_oneShot_0045 = I_parThis_0054;
    }
    
    /* start parallel [ */
    I_parOut_0057 = 0;
    I_parThis_0057 = I_parThis_0054;
    /* start series [ */
    I_scratch = I_parThis_0057;
    if (I_oneShot_0047) {  // $oneShot_0047
        I_parThis_0057 = 0;
    }
    I_oneShot_0047 = I_scratch;
    
    if (I_parThis_0057) {  // $parThis_0057
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0057) {  // $parThis_0057
        I_parOut_0057 = 1;
    }
    I_parThis_0057 = I_parThis_0054;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0057 = 0;
    }
    
    if (I_parThis_0057) {  // $parThis_0057
        U_RCfgSrvParP2OK = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0057) {  // $parThis_0057
        I_parOut_0057 = 1;
    }
    I_parThis_0054 = I_parOut_0057;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (!U_RCfgSrvParP2OK) {  // RCfgSrvParP2OK
        I_parThis_0054 = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (!I_oneShot_0048) {  // $oneShot_0048
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "SPD%d", &U_AplanVelMax);
                I_oneShot_0048 = I_parThis_0054;
            }
            I_parThis_0054 = 0;
            I_oneShot_0049 = I_parThis_0054;
        }
        if (!I_oneShot_0049) {  // $oneShot_0049
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0049 = 1;
            } else {
                I_parThis_0054 = 0;
            }
        }
    } else {
        I_oneShot_0048 = I_parThis_0054;
    }
    
    /* start parallel [ */
    I_parOut_0058 = 0;
    I_parThis_0058 = I_parThis_0054;
    /* start series [ */
    I_scratch = I_parThis_0058;
    if (I_oneShot_004a) {  // $oneShot_004a
        I_parThis_0058 = 0;
    }
    I_oneShot_004a = I_scratch;
    
    if (I_parThis_0058) {  // $parThis_0058
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0058) {  // $parThis_0058
        I_parOut_0058 = 1;
    }
    I_parThis_0058 = I_parThis_0054;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0058 = 0;
    }
    
    if (I_parThis_0058) {  // $parThis_0058
        U_RCfgSrvParP3OK = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0058) {  // $parThis_0058
        I_parOut_0058 = 1;
    }
    I_parThis_0054 = I_parOut_0058;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (!U_RCfgSrvParP3OK) {  // RCfgSrvParP3OK
        I_parThis_0054 = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (!I_oneShot_004b) {  // $oneShot_004b
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "ACC%d", &U_AplanRampa);
                I_oneShot_004b = I_parThis_0054;
            }
            I_parThis_0054 = 0;
            I_oneShot_004c = I_parThis_0054;
        }
        if (!I_oneShot_004c) {  // $oneShot_004c
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_004c = 1;
            } else {
                I_parThis_0054 = 0;
            }
        }
    } else {
        I_oneShot_004b = I_parThis_0054;
    }
    
    /* start parallel [ */
    I_parOut_0059 = 0;
    I_parThis_0059 = I_parThis_0054;
    /* start series [ */
    I_scratch = I_parThis_0059;
    if (I_oneShot_004d) {  // $oneShot_004d
        I_parThis_0059 = 0;
    }
    I_oneShot_004d = I_scratch;
    
    if (I_parThis_0059) {  // $parThis_0059
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0059) {  // $parThis_0059
        I_parOut_0059 = 1;
    }
    I_parThis_0059 = I_parThis_0054;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_0059 = 0;
    }
    
    if (I_parThis_0059) {  // $parThis_0059
        U_RCfgSrvParP4OK = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0059) {  // $parThis_0059
        I_parOut_0059 = 1;
    }
    I_parThis_0054 = I_parOut_0059;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (!U_RCfgSrvParP4OK) {  // RCfgSrvParP4OK
        I_parThis_0054 = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (!I_oneShot_004e) {  // $oneShot_004e
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "DEC%d", &U_AplanRampa);
                I_oneShot_004e = I_parThis_0054;
            }
            I_parThis_0054 = 0;
            I_oneShot_004f = I_parThis_0054;
        }
        if (!I_oneShot_004f) {  // $oneShot_004f
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_004f = 1;
            } else {
                I_parThis_0054 = 0;
            }
        }
    } else {
        I_oneShot_004e = I_parThis_0054;
    }
    
    /* start parallel [ */
    I_parOut_005a = 0;
    I_parThis_005a = I_parThis_0054;
    /* start series [ */
    I_scratch = I_parThis_005a;
    if (I_oneShot_0050) {  // $oneShot_0050
        I_parThis_005a = 0;
    }
    I_oneShot_0050 = I_scratch;
    
    if (I_parThis_005a) {  // $parThis_005a
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_005a) {  // $parThis_005a
        I_parOut_005a = 1;
    }
    I_parThis_005a = I_parThis_0054;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_005a = 0;
    }
    
    if (I_parThis_005a) {  // $parThis_005a
        U_RCfgSrvParP5OK = 1;
    }
    
    /* ] finish series */
    if (I_parThis_005a) {  // $parThis_005a
        I_parOut_005a = 1;
    }
    I_parThis_0054 = I_parOut_005a;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (!U_RCfgSrvParP5OK) {  // RCfgSrvParP5OK
        I_parThis_0054 = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (!I_oneShot_0051) {  // $oneShot_0051
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "POST000=%d", &U_AplanPassoCorr);
                I_oneShot_0051 = I_parThis_0054;
            }
            I_parThis_0054 = 0;
            I_oneShot_0052 = I_parThis_0054;
        }
        if (!I_oneShot_0052) {  // $oneShot_0052
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0052 = 1;
            } else {
                I_parThis_0054 = 0;
            }
        }
    } else {
        I_oneShot_0051 = I_parThis_0054;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (U_TEspCfgSrvPar5 < 19) {
            U_TEspCfgSrvPar5++;
            I_parThis_0054 = 0;
        }
    } else {
        U_TEspCfgSrvPar5 = 0;
    }
    
    /* start parallel [ */
    I_parOut_005b = 0;
    I_parThis_005b = I_parThis_0054;
    /* start series [ */
    I_scratch = I_parThis_005b;
    if (I_oneShot_0053) {  // $oneShot_0053
        I_parThis_005b = 0;
    }
    I_oneShot_0053 = I_scratch;
    
    if (I_parThis_005b) {  // $parThis_005b
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_005b = I_parThis_0054;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_005b = 0;
    }
    
    if (I_parThis_005b) {  // $parThis_005b
        U_RCfgSrvParP6OK = 1;
    }
    
    /* ] finish series */
    if (I_parThis_005b) {  // $parThis_005b
        I_parOut_005b = 1;
    }
    I_parThis_0054 = I_parOut_005b;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (!U_RCfgSrvParP6OK) {  // RCfgSrvParP6OK
        I_parThis_0054 = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (!I_oneShot_0054) {  // $oneShot_0054
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "SPDT000=%d", &U_AplanVelAuto);
                I_oneShot_0054 = I_parThis_0054;
            }
            I_parThis_0054 = 0;
            I_oneShot_0055 = I_parThis_0054;
        }
        if (!I_oneShot_0055) {  // $oneShot_0055
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0055 = 1;
            } else {
                I_parThis_0054 = 0;
            }
        }
    } else {
        I_oneShot_0054 = I_parThis_0054;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (U_TEspCfgSrvPar6 < 19) {
            U_TEspCfgSrvPar6++;
            I_parThis_0054 = 0;
        }
    } else {
        U_TEspCfgSrvPar6 = 0;
    }
    
    /* start parallel [ */
    I_parOut_005c = 0;
    I_parThis_005c = I_parThis_0054;
    /* start series [ */
    I_scratch = I_parThis_005c;
    if (I_oneShot_0056) {  // $oneShot_0056
        I_parThis_005c = 0;
    }
    I_oneShot_0056 = I_scratch;
    
    if (I_parThis_005c) {  // $parThis_005c
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_005c) {  // $parThis_005c
        I_parOut_005c = 1;
    }
    I_parThis_005c = I_parThis_0054;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_005c = 0;
    }
    
    if (I_parThis_005c) {  // $parThis_005c
        U_RCfgSrvParP7OK = 1;
    }
    
    /* ] finish series */
    if (I_parThis_005c) {  // $parThis_005c
        I_parOut_005c = 1;
    }
    I_parThis_0054 = I_parOut_005c;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_parThis_0054 = I_rung_top;
    /* start series [ */
    if (!U_RCfgSrvParP7OK) {  // RCfgSrvParP7OK
        I_parThis_0054 = 0;
    }
    
    if (I_parThis_0054) {  // $parThis_0054
        if (!I_oneShot_0057) {  // $oneShot_0057
            if (I_SerialReady) {  // $SerialReady
                Yaskawa_Write_Debug("0", "SVON", &VAR_NULL);
                I_oneShot_0057 = I_parThis_0054;
            }
            I_parThis_0054 = 0;
            I_oneShot_0058 = I_parThis_0054;
        }
        if (!I_oneShot_0058) {  // $oneShot_0058
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0058 = 1;
            } else {
                I_parThis_0054 = 0;
            }
        }
    } else {
        I_oneShot_0057 = I_parThis_0054;
    }
    
    /* start parallel [ */
    I_parOut_005d = 0;
    I_parThis_005d = I_parThis_0054;
    /* start series [ */
    I_scratch = I_parThis_005d;
    if (I_oneShot_0059) {  // $oneShot_0059
        I_parThis_005d = 0;
    }
    I_oneShot_0059 = I_scratch;
    
    if (I_parThis_005d) {  // $parThis_005d
        U_RMaqLerOKSerial = 1;
    }
    
    /* ] finish series */
    if (I_parThis_005d) {  // $parThis_005d
        I_parOut_005d = 1;
    }
    I_parThis_005d = I_parThis_0054;
    /* start series [ */
    if (U_RMaqLerOKSerial) {  // RMaqLerOKSerial
        I_parThis_005d = 0;
    }
    
    /* start parallel [ */
    I_parOut_005e = 0;
    I_parThis_005e = I_parThis_005d;
    if (I_parThis_005e) {  // $parThis_005e
        U_RStartCfgSrv = 0;
    }
    
    if (I_parThis_005e) {  // $parThis_005e
        I_parOut_005e = 1;
    }
    I_parThis_005e = I_parThis_005d;
    if (I_parThis_005e) {  // $parThis_005e
        U_RMaqCfgSrvOK = 1;
    }
    
    if (I_parThis_005e) {  // $parThis_005e
        I_parOut_005e = 1;
    }
    I_parThis_005e = I_parThis_005d;
    if (I_parThis_005e) {  // $parThis_005e
        U_RAplanZerarEnc = 1;
    }
    
    if (I_parThis_005e) {  // $parThis_005e
        I_parOut_005e = 1;
    }
    I_parThis_005d = I_parOut_005e;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_005d) {  // $parThis_005d
        I_parOut_005d = 1;
    }
    I_parThis_0054 = I_parOut_005d;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0054) {  // $parThis_0054
        I_parOut_0054 = 1;
    }
    I_rung_top = I_parOut_0054;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 174 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }
    
    if (!U_RModoAuto) {  // RModoAuto
        I_rung_top = 0;
    }
    
    if (MODBUS_REGISTER[2] & (1 << 4)) {  // YMbPrsSentidoInv
        I_rung_top = 0;
    }
    
    U_RModoAutoOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 175 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_005f = 0;
    I_parThis_005f = I_rung_top;
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_005f = 0;
    }
    
    if (I_parThis_005f) {  // $parThis_005f
        I_parOut_005f = 1;
    }
    I_parThis_005f = I_rung_top;
    if (!U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_parThis_005f = 0;
    }
    
    if (I_parThis_005f) {  // $parThis_005f
        I_parOut_005f = 1;
    }
    I_parThis_005f = I_rung_top;
    /* start series [ */
    if (!GPIO_OUTPUT_PORT16) {  // YSrvHabilita
        I_parThis_005f = 0;
    }
    
    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        I_parThis_005f = 0;
    }
    
    if (I_parThis_005f) {  // $parThis_005f
        if (U_TEspDeslSrvInPos < 299) {
            U_TEspDeslSrvInPos++;
            I_parThis_005f = 0;
        }
    } else {
        U_TEspDeslSrvInPos = 0;
    }
    
    /* ] finish series */
    if (I_parThis_005f) {  // $parThis_005f
        I_parOut_005f = 1;
    }
    I_parThis_005f = I_rung_top;
    /* start series [ */
    if (GPIO_INPUT_PORT6) {  // XAplanFechada
        I_parThis_005f = 0;
    }
    
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_005f = 0;
    }
    
    /* ] finish series */
    if (I_parThis_005f) {  // $parThis_005f
        I_parOut_005f = 1;
    }
    I_rung_top = I_parOut_005f;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0060 = 0;
    I_parThis_0060 = I_rung_top;
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_0060 = 0;
    }
    
    if (I_parThis_0060) {  // $parThis_0060
        I_parOut_0060 = 1;
    }
    I_parThis_0060 = I_rung_top;
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_parThis_0060 = 0;
    }
    
    if (I_parThis_0060) {  // $parThis_0060
        I_parOut_0060 = 1;
    }
    I_rung_top = I_parOut_0060;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0061 = 0;
    I_parThis_0061 = I_rung_top;
    if (I_parThis_0061) {  // $parThis_0061
        U_RMaqProduzindo = 0;
    }
    
    if (I_parThis_0061) {  // $parThis_0061
        I_parOut_0061 = 1;
    }
    I_parThis_0061 = I_rung_top;
    if (I_parThis_0061) {  // $parThis_0061
        U_RMaqStartProd = 0;
    }
    
    if (I_parThis_0061) {  // $parThis_0061
        I_parOut_0061 = 1;
    }
    I_parThis_0061 = I_rung_top;
    if (I_parThis_0061) {  // $parThis_0061
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }
    
    if (I_parThis_0061) {  // $parThis_0061
        I_parOut_0061 = 1;
    }
    I_parThis_0061 = I_rung_top;
    /* start series [ */
    if (!GPIO_OUTPUT_PORT16) {  // YSrvHabilita
        I_parThis_0061 = 0;
    }
    
    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        I_parThis_0061 = 0;
    }
    
    if (I_parThis_0061) {  // $parThis_0061
        U_RMaqErroPosic = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0061) {  // $parThis_0061
        I_parOut_0061 = 1;
    }
    I_rung_top = I_parOut_0061;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 176 */
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
    
    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YMbModoAutoAtivo
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }
    
    /* ] finish series */
    
    /* start rung 177 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_005a) {  // $oneShot_005a
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_005a = I_scratch;
    
    /* start parallel [ */
    I_parOut_0062 = 0;
    I_parThis_0062 = I_rung_top;
    if (I_parThis_0062) {  // $parThis_0062
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }
    
    if (I_parThis_0062) {  // $parThis_0062
        I_parOut_0062 = 1;
    }
    I_parThis_0062 = I_rung_top;
    if (I_parThis_0062) {  // $parThis_0062
        U_RPrsPararMartelo = 1;
    }
    
    if (I_parThis_0062) {  // $parThis_0062
        I_parOut_0062 = 1;
    }
    I_parThis_0062 = I_rung_top;
    if (I_parThis_0062) {  // $parThis_0062
        U_RAplanLigarMotor = 0;
    }
    
    if (I_parThis_0062) {  // $parThis_0062
        I_parOut_0062 = 1;
    }
    I_parThis_0062 = I_rung_top;
    if (I_parThis_0062) {  // $parThis_0062
        U_RMaqInicioAuto = 0;
    }
    
    if (I_parThis_0062) {  // $parThis_0062
        I_parOut_0062 = 1;
    }
    I_parThis_0062 = I_rung_top;
    if (I_parThis_0062) {  // $parThis_0062
        U_RMaqPosicionando = 0;
    }
    
    if (I_parThis_0062) {  // $parThis_0062
        I_parOut_0062 = 1;
    }
    I_rung_top = I_parOut_0062;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 179 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RModoAuto) {  // RModoAuto
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_005b) {  // $oneShot_005b
        I_rung_top = 0;
    }
    I_oneShot_005b = I_scratch;
    
    if (U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }
    
    if (U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    if (U_RMaqErroPosic) {  // RMaqErroPosic
        I_rung_top = 0;
    }
    
    if (!U_RMaqConfigOK) {  // RMaqConfigOK
        I_rung_top = 0;
    }
    
    if (MODBUS_REGISTER[2] & (1 << 4)) {  // YMbPrsSentidoInv
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0063 = 0;
    I_parThis_0063 = I_rung_top;
    if (I_parThis_0063) {  // $parThis_0063
        U_RMaqStartProd = 1;
    }
    
    if (I_parThis_0063) {  // $parThis_0063
        I_parOut_0063 = 1;
    }
    I_parThis_0063 = I_rung_top;
    if (I_parThis_0063) {  // $parThis_0063
        MODBUS_REGISTER[4] &= ~(1 << 6);  // YMbAplanExtSubir
    }
    
    if (I_parThis_0063) {  // $parThis_0063
        I_parOut_0063 = 1;
    }
    I_parThis_0063 = I_rung_top;
    if (I_parThis_0063) {  // $parThis_0063
        MODBUS_REGISTER[4] &= ~(1 << 7);  // YMbAplanExpandir
    }
    
    if (I_parThis_0063) {  // $parThis_0063
        I_parOut_0063 = 1;
    }
    I_parThis_0063 = I_rung_top;
    if (I_parThis_0063) {  // $parThis_0063
        U_RAplanZerarEnc = 1;
    }
    
    if (I_parThis_0063) {  // $parThis_0063
        I_parOut_0063 = 1;
    }
    I_rung_top = I_parOut_0063;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 180 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }
    
    if (U_RPrsMotorLigado) {  // RPrsMotorLigado
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[4] |= (1 << 8);  // YMbPrsLigaMotor
    }
    
    /* ] finish series */
    
    /* start rung 181 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }
    
    if (GPIO_INPUT_PORT6) {  // XAplanFechada
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[4] |= (1 << 3);  // YMbAplanFecha
    }
    
    /* ] finish series */
    
    /* start rung 182 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if (U_TEspExtRepouso < 299) {
            U_TEspExtRepouso++;
            I_rung_top = 0;
        }
    } else {
        U_TEspExtRepouso = 0;
    }
    
    if (!U_RPrsMotorOK) {  // RPrsMotorOK
        I_rung_top = 0;
    }
    
    if (!GPIO_INPUT_PORT6) {  // XAplanFechada
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_005c) {  // $oneShot_005c
        I_rung_top = 0;
    }
    I_oneShot_005c = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        U_RMaqStartProd = 0;
    }
    
    /* ] finish series */
    
    /* start rung 183 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_005d) {  // $oneShot_005d
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_005d = I_scratch;
    
    /* start parallel [ */
    I_parOut_0064 = 0;
    I_parThis_0064 = I_rung_top;
    /* start series [ */
    if (!U_RModoAutoOK) {  // RModoAutoOK
        I_parThis_0064 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0065 = 0;
    I_parThis_0065 = I_parThis_0064;
    if (I_parThis_0065) {  // $parThis_0065
        U_RMaqProduzindo = 1;
    }
    
    if (I_parThis_0065) {  // $parThis_0065
        I_parOut_0065 = 1;
    }
    I_parThis_0065 = I_parThis_0064;
    if (I_parThis_0065) {  // $parThis_0065
        U_RPrsIniciarCiclo = 1;
    }
    
    if (I_parThis_0065) {  // $parThis_0065
        I_parOut_0065 = 1;
    }
    I_parThis_0065 = I_parThis_0064;
    if (I_parThis_0065) {  // $parThis_0065
        U_RMaqInicioAuto = 1;
    }
    
    if (I_parThis_0065) {  // $parThis_0065
        I_parOut_0065 = 1;
    }
    I_parThis_0065 = I_parThis_0064;
    if (I_parThis_0065) {  // $parThis_0065
        U_RBloquearPrensa = 0;
    }
    
    if (I_parThis_0065) {  // $parThis_0065
        I_parOut_0065 = 1;
    }
    I_parThis_0064 = I_parOut_0065;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0064) {  // $parThis_0064
        I_parOut_0064 = 1;
    }
    I_parThis_0064 = I_rung_top;
    /* start series [ */
    if (U_RModoAutoOK) {  // RModoAutoOK
        I_parThis_0064 = 0;
    }
    
    if (I_parThis_0064) {  // $parThis_0064
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbModoAutoAtivo
    }
    
    /* ] finish series */
    if (I_parThis_0064) {  // $parThis_0064
        I_parOut_0064 = 1;
    }
    I_rung_top = I_parOut_0064;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 185 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_DifPosic = U_MaqPosAtual - U_AplanPasso;
    }
    
    /* ] finish series */
    
    /* start rung 186 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_0066 = 0;
    I_parThis_0066 = I_rung_top;
    I_scratch2 = -75;
    if (I_scratch2 > U_DifPosic) {
    } else {
        I_parThis_0066 = 0;
    }
    
    if (I_parThis_0066) {  // $parThis_0066
        I_parOut_0066 = 1;
    }
    I_parThis_0066 = I_rung_top;
    I_scratch2 = 75;
    if (U_DifPosic > I_scratch2) {
    } else {
        I_parThis_0066 = 0;
    }
    
    if (I_parThis_0066) {  // $parThis_0066
        I_parOut_0066 = 1;
    }
    I_rung_top = I_parOut_0066;
    /* ] finish parallel */
    U_RMaqForaPosic = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 187 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 30;
    if (I_scratch2 > U_DifPosic) {
    } else {
        I_rung_top = 0;
    }
    
    I_scratch2 = -30;
    if (U_DifPosic > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    U_RMaqPosicOK = I_rung_top;
    
    /* ] finish series */
    
    /* start rung 188 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPrsAvancoOK) {  // RPrsAvancoOK
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0067 = 0;
    I_parThis_0067 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0067;
    if (I_oneShot_005e) {  // $oneShot_005e
        I_parThis_0067 = 0;
    }
    I_oneShot_005e = I_scratch;
    
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_0067 = 0;
    }
    
    if (U_RMaqErroPosic) {  // RMaqErroPosic
        I_parThis_0067 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0068 = 0;
    I_parThis_0068 = I_parThis_0067;
    if (I_parThis_0068) {  // $parThis_0068
        U_RAplanLigarMotor = 1;
    }
    
    if (I_parThis_0068) {  // $parThis_0068
        I_parOut_0068 = 1;
    }
    I_parThis_0068 = I_parThis_0067;
    if (I_parThis_0068) {  // $parThis_0068
        U_RMaqInicioAuto = 0;
    }
    
    if (I_parThis_0068) {  // $parThis_0068
        I_parOut_0068 = 1;
    }
    I_parThis_0067 = I_parOut_0068;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0067) {  // $parThis_0067
        I_parOut_0067 = 1;
    }
    I_parThis_0067 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0067;
    if (!I_parThis_0067) {  // $parThis_0067
        if (I_oneShot_005f) {  // $oneShot_005f
            I_parThis_0067 = 1;
        }
    } else {
        I_parThis_0067 = 0;
    }
    I_oneShot_005f = I_scratch;
    
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_parThis_0067 = 0;
    }
    
    /* start parallel [ */
    I_parOut_0069 = 0;
    I_parThis_0069 = I_parThis_0067;
    /* start series [ */
    if (!U_RMaqForaPosic) {  // RMaqForaPosic
        I_parThis_0069 = 0;
    }
    
    if (U_RMaqInicioAuto) {  // RMaqInicioAuto
        I_parThis_0069 = 0;
    }
    
    /* start parallel [ */
    I_parOut_006a = 0;
    I_parThis_006a = I_parThis_0069;
    if (I_parThis_006a) {  // $parThis_006a
        U_RMaqErroPosic = 1;
    }
    
    if (I_parThis_006a) {  // $parThis_006a
        I_parOut_006a = 1;
    }
    I_parThis_006a = I_parThis_0069;
    if (I_parThis_006a) {  // $parThis_006a
        U_RPrsPararMartelo = 1;
    }
    
    if (I_parThis_006a) {  // $parThis_006a
        I_parOut_006a = 1;
    }
    I_parThis_006a = I_parThis_0069;
    if (I_parThis_006a) {  // $parThis_006a
        I_scratch2 = 10;
        if(I_scratch2) MODBUS_REGISTER[14] = U_DifPosic / I_scratch2;
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
    I_parThis_0069 = I_parThis_0067;
    /* start series [ */
    if (U_RMaqErroPosic) {  // RMaqErroPosic
        I_parThis_0069 = 0;
    }
    
    if (I_parThis_0069) {  // $parThis_0069
        U_RAplanZerarEnc = 1;
    }
    
    /* ] finish series */
    if (I_parThis_0069) {  // $parThis_0069
        I_parOut_0069 = 1;
    }
    I_parThis_0069 = I_parThis_0067;
    if (I_parThis_0069) {  // $parThis_0069
        U_RAplanLigarMotor = 0;
    }
    
    if (I_parThis_0069) {  // $parThis_0069
        I_parOut_0069 = 1;
    }
    I_parThis_0069 = I_parThis_0067;
    if (I_parThis_0069) {  // $parThis_0069
        U_RMaqPosicionando = 0;
    }
    
    if (I_parThis_0069) {  // $parThis_0069
        I_parOut_0069 = 1;
    }
    I_parThis_0067 = I_parOut_0069;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0067) {  // $parThis_0067
        I_parOut_0067 = 1;
    }
    I_rung_top = I_parOut_0067;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 189 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqErroPosic) {  // RMaqErroPosic
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0060) {  // $oneShot_0060
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0060 = I_scratch;
    
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    if (!U_RAplanAvancoOK) {  // RAplanAvancoOK
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_RAplanLigarMotor = 1;
    }
    
    /* ] finish series */
    
    /* start rung 190 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_006b = 0;
    I_parThis_006b = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        I_parThis_006b = 0;
    }
    
    I_scratch = I_parThis_006b;
    if (I_oneShot_0061) {  // $oneShot_0061
        I_parThis_006b = 0;
    }
    I_oneShot_0061 = I_scratch;
    
    /* ] finish series */
    if (I_parThis_006b) {  // $parThis_006b
        I_parOut_006b = 1;
    }
    I_parThis_006b = I_rung_top;
    /* start series [ */
    if (!U_RMaqPosicOK) {  // RMaqPosicOK
        I_parThis_006b = 0;
    }
    
    I_scratch = I_parThis_006b;
    if (!I_parThis_006b) {  // $parThis_006b
        if (I_oneShot_0062) {  // $oneShot_0062
            I_parThis_006b = 1;
        }
    } else {
        I_parThis_006b = 0;
    }
    I_oneShot_0062 = I_scratch;
    
    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        I_parThis_006b = 0;
    }
    
    if (!U_RMaqPosicionando) {  // RMaqPosicionando
        I_parThis_006b = 0;
    }
    
    /* ] finish series */
    if (I_parThis_006b) {  // $parThis_006b
        I_parOut_006b = 1;
    }
    I_rung_top = I_parOut_006b;
    /* ] finish parallel */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_006c = 0;
    I_parThis_006c = I_rung_top;
    if (I_parThis_006c) {  // $parThis_006c
        U_RAplanLigarMotor = 0;
    }
    
    if (I_parThis_006c) {  // $parThis_006c
        I_parOut_006c = 1;
    }
    I_parThis_006c = I_rung_top;
    /* start series [ */
    if (!U_RMaqPosicOK) {  // RMaqPosicOK
        I_parThis_006c = 0;
    }
    
    if (GPIO_OUTPUT_PORT11) {  // YPrsEmbreagem
        I_parThis_006c = 0;
    }
    
    if (I_parThis_006c) {  // $parThis_006c
        U_RPrsIniciarCiclo = 1;
    }
    
    /* ] finish series */
    if (I_parThis_006c) {  // $parThis_006c
        I_parOut_006c = 1;
    }
    I_parThis_006c = I_rung_top;
    /* start series [ */
    if (U_RMaqPosicOK) {  // RMaqPosicOK
        I_parThis_006c = 0;
    }
    
    if (!U_RAplanAvancoOK) {  // RAplanAvancoOK
        I_parThis_006c = 0;
    }
    
    if (I_parThis_006c) {  // $parThis_006c
        U_RAplanCorrigeEnc = 1;
    }
    
    /* ] finish series */
    if (I_parThis_006c) {  // $parThis_006c
        I_parOut_006c = 1;
    }
    I_rung_top = I_parOut_006c;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 191 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanCorrigeEnc) {  // RAplanCorrigeEnc
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
    
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    if (!U_RAplanAvancoOK) {  // RAplanAvancoOK
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_RAplanLigarMotor = 1;
    }
    
    /* ] finish series */
    
    /* start rung 192 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanLigarMotor) {  // RAplanLigarMotor
        I_rung_top = 0;
    }
    
    if (U_RAplanCorrigeEnc) {  // RAplanCorrigeEnc
        I_rung_top = 0;
    }
    
    if (U_RAplanZerarEnc) {  // RAplanZerarEnc
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_006d = 0;
    I_parThis_006d = I_rung_top;
    GPIO_OUTPUT_PORT16 = I_parThis_006d;
    
    if (I_parThis_006d) {  // $parThis_006d
        I_parOut_006d = 1;
    }
    I_parThis_006d = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_006d;
    if (I_oneShot_0064) {  // $oneShot_0064
        I_parThis_006d = 0;
    }
    I_oneShot_0064 = I_scratch;
    
    if (I_parThis_006d) {  // $parThis_006d
        U_RMaqPosicionando = 1;
    }
    
    /* ] finish series */
    if (I_parThis_006d) {  // $parThis_006d
        I_parOut_006d = 1;
    }
    I_parThis_006d = I_rung_top;
    /* start series [ */
    if (!I_TEspSrvParar_antiglitch) {  // $TEspSrvParar_antiglitch
        U_TEspSrvParar = 9;
    }
    I_TEspSrvParar_antiglitch = 1;
    if (!I_parThis_006d) {  // $parThis_006d
        if (U_TEspSrvParar < 9) {
            U_TEspSrvParar++;
            I_parThis_006d = 1;
        }
    } else {
        U_TEspSrvParar = 0;
    }
    
    if (I_parThis_006d) {  // $parThis_006d
        U_RSrvMotorParado = 0;
    } else {
        U_RSrvMotorParado = 1;
    }
    
    /* ] finish series */
    if (I_parThis_006d) {  // $parThis_006d
        I_parOut_006d = 1;
    }
    I_rung_top = I_parOut_006d;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 194 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    /* start parallel [ */
    I_parOut_006e = 0;
    I_parThis_006e = I_rung_top;
    /* start series [ */
    if (!U_RPrsAvancoOK) {  // RPrsAvancoOK
        I_parThis_006e = 0;
    }
    
    I_scratch = I_parThis_006e;
    if (I_oneShot_0065) {  // $oneShot_0065
        I_parThis_006e = 0;
    }
    I_oneShot_0065 = I_scratch;
    
    /* ] finish series */
    if (I_parThis_006e) {  // $parThis_006e
        I_parOut_006e = 1;
    }
    I_parThis_006e = I_rung_top;
    /* start series [ */
    if (!U_RModoAuto) {  // RModoAuto
        I_parThis_006e = 0;
    }
    
    I_scratch = I_parThis_006e;
    if (!I_parThis_006e) {  // $parThis_006e
        if (I_oneShot_0066) {  // $oneShot_0066
            I_parThis_006e = 1;
        }
    } else {
        I_parThis_006e = 0;
    }
    I_oneShot_0066 = I_scratch;
    
    if (GPIO_OUTPUT_PORT11) {  // YPrsEmbreagem
        I_parThis_006e = 0;
    }
    
    /* ] finish series */
    if (I_parThis_006e) {  // $parThis_006e
        I_parOut_006e = 1;
    }
    I_rung_top = I_parOut_006e;
    /* ] finish parallel */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    if (U_RModoAuto) {  // RModoAuto
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_RMaqProduzindo = 0;
    }
    
    /* ] finish series */
    
    /* start rung 196 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // XMbModoAuto
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_006f = 0;
    I_parThis_006f = I_rung_top;
    U_RModoAuto = I_parThis_006f;
    
    if (I_parThis_006f) {  // $parThis_006f
        I_parOut_006f = 1;
    }
    I_parThis_006f = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_006f;
    if (I_oneShot_0067) {  // $oneShot_0067
        I_parThis_006f = 0;
    }
    I_oneShot_0067 = I_scratch;
    
    if (I_parThis_006f) {  // $parThis_006f
        MODBUS_REGISTER[1] |= (1 << 0);  // YMbModoAutoAtivo
    }
    
    /* ] finish series */
    if (I_parThis_006f) {  // $parThis_006f
        I_parOut_006f = 1;
    }
    I_rung_top = I_parOut_006f;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 197 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[3] & (1 << 0))) {  // XDbgLimpaErro
        I_rung_top = 0;
    }
    
    I_scratch = I_rung_top;
    if (I_oneShot_0068) {  // $oneShot_0068
        I_rung_top = 0;
    }
    I_oneShot_0068 = I_scratch;
    
    if (I_rung_top) {  // $rung_top
        MODBUS_REGISTER[2] |= (1 << 2);  // YMbLimpaErro
    }
    
    /* ] finish series */
    
    /* start rung 198 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 2))) {  // YMbLimpaErro
        I_rung_top = 0;
    }
    
    /* start parallel [ */
    I_parOut_0070 = 0;
    I_parThis_0070 = I_rung_top;
    if (I_parThis_0070) {  // $parThis_0070
        U_RMaqCfgSrvErro = 0;
    }
    
    if (I_parThis_0070) {  // $parThis_0070
        I_parOut_0070 = 1;
    }
    I_parThis_0070 = I_rung_top;
    if (I_parThis_0070) {  // $parThis_0070
        U_RMaqErroPosic = 0;
    }
    
    if (I_parThis_0070) {  // $parThis_0070
        I_parOut_0070 = 1;
    }
    I_parThis_0070 = I_rung_top;
    /* start series [ */
    if (!I_TEspInvLimpaErr_antiglitch) {  // $TEspInvLimpaErr_antiglitch
        U_TEspInvLimpaErr = 9;
    }
    I_TEspInvLimpaErr_antiglitch = 1;
    if (!I_parThis_0070) {  // $parThis_0070
        if (U_TEspInvLimpaErr < 9) {
            U_TEspInvLimpaErr++;
            I_parThis_0070 = 1;
        }
    } else {
        U_TEspInvLimpaErr = 0;
    }
    
    GPIO_OUTPUT_PORT15 = I_parThis_0070;
    
    /* ] finish series */
    if (I_parThis_0070) {  // $parThis_0070
        I_parOut_0070 = 1;
    }
    I_parThis_0070 = I_rung_top;
    if (I_parThis_0070) {  // $parThis_0070
        MODBUS_REGISTER[2] &= ~(1 << 2);  // YMbLimpaErro
    }
    
    if (I_parThis_0070) {  // $parThis_0070
        I_parOut_0070 = 1;
    }
    I_rung_top = I_parOut_0070;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 200 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // XEmergencia
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 0); MODBUS_REGISTER[0] |= I_rung_top << 0;  // YMbErrEmergencia
    
    /* ] finish series */
    
    /* start rung 201 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT4) {  // XFaltaFase
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 1); MODBUS_REGISTER[0] |= I_rung_top << 1;  // YMbErrFaltaFase
    
    /* ] finish series */
    
    /* start rung 202 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RErroBombaHidr) {  // RErroBombaHidr
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 2); MODBUS_REGISTER[0] |= I_rung_top << 2;  // YMbErrBombaHidr
    
    /* ] finish series */
    
    /* start rung 203 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT3) {  // XTermMartelo
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 3); MODBUS_REGISTER[0] |= I_rung_top << 3;  // YMbErrMartelo
    
    /* ] finish series */
    
    /* start rung 204 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RPressaoArOK) {  // RPressaoArOK
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 4); MODBUS_REGISTER[0] |= I_rung_top << 4;  // YMbErrPressaoAr
    
    /* ] finish series */
    
    /* start rung 205 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT5) {  // XInversorOK
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 5); MODBUS_REGISTER[0] |= I_rung_top << 5;  // YMbErrInversor
    
    /* ] finish series */
    
    /* start rung 206 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (U_RServoOK) {  // RServoOK
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 6); MODBUS_REGISTER[0] |= I_rung_top << 6;  // YMbErrServo
    
    /* ] finish series */
    
    /* start rung 207 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT8) {  // XDesbobOK
        I_rung_top = 0;
    }
    
    I_rung_top = 0;
    
    MODBUS_REGISTER[0] &= ~(1 << 7); MODBUS_REGISTER[0] |= I_rung_top << 7;  // YMbErrDesbob
    
    /* ] finish series */
    
    /* start rung 208 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqCfgSrvErro) {  // RMaqCfgSrvErro
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 8); MODBUS_REGISTER[0] |= I_rung_top << 8;  // YMbErrSrvComunic
    
    /* ] finish series */
    
    /* start rung 209 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqErroPosic) {  // RMaqErroPosic
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[0] &= ~(1 << 9); MODBUS_REGISTER[0] |= I_rung_top << 9;  // YMbErrPosic
    
    /* ] finish series */
    
    /* start rung 211 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 1); MODBUS_REGISTER[1] |= I_rung_top << 1;  // YMbMaqOK
    
    /* ] finish series */
    
    /* start rung 212 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqPronta) {  // RMaqPronta
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 2); MODBUS_REGISTER[1] |= I_rung_top << 2;  // YMbMaqPronta
    
    /* ] finish series */
    
    /* start rung 213 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqCfgSrvOK) {  // RMaqCfgSrvOK
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 3); MODBUS_REGISTER[1] |= I_rung_top << 3;  // YMbMaqCfgSrvOK
    
    /* ] finish series */
    
    /* start rung 214 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanAvancoOK) {  // RAplanAvancoOK
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 4); MODBUS_REGISTER[1] |= I_rung_top << 4;  // YMbAplanAvancoOK
    
    /* ] finish series */
    
    /* start rung 215 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPrsMotorParado) {  // RPrsMotorParado
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 5); MODBUS_REGISTER[1] |= I_rung_top << 5;  // YMbPrsParado
    
    /* ] finish series */
    
    /* start rung 216 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RPrsMotorLigado) {  // RPrsMotorLigado
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 6); MODBUS_REGISTER[1] |= I_rung_top << 6;  // YMbPrsLigado
    
    /* ] finish series */
    
    /* start rung 217 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqProduzindo) {  // RMaqProduzindo
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 7); MODBUS_REGISTER[1] |= I_rung_top << 7;  // YMbMaqProduzindo
    
    /* ] finish series */
    
    /* start rung 218 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqStartProd) {  // RMaqStartProd
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 8); MODBUS_REGISTER[1] |= I_rung_top << 8;  // YMbMaqStartProd
    
    /* ] finish series */
    
    /* start rung 219 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqForaPosic) {  // RMaqForaPosic
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 9); MODBUS_REGISTER[1] |= I_rung_top << 9;  // YMbMaqForaPosic
    
    /* ] finish series */
    
    /* start rung 220 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT6) {  // XAplanFechada
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 12); MODBUS_REGISTER[1] |= I_rung_top << 12;  // YMbAplanAberta
    
    /* ] finish series */
    
    /* start rung 221 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RAplanExtSubir) {  // RAplanExtSubir
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 11); MODBUS_REGISTER[1] |= I_rung_top << 11;  // YMbAplanExtCima
    
    /* ] finish series */
    
    /* start rung 222 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (GPIO_INPUT_PORT7) {  // XAplanExtRecuada
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 10); MODBUS_REGISTER[1] |= I_rung_top << 10;  // YMbAplanExtAvanc
    
    /* ] finish series */
    
    /* start rung 223 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!GPIO_INPUT_PORT19) {  // XSrvInPos
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 13); MODBUS_REGISTER[1] |= I_rung_top << 13;  // YMbSrvInPos
    
    /* ] finish series */
    
    /* start rung 224 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (!U_RMaqPosicOK) {  // RMaqPosicOK
        I_rung_top = 0;
    }
    
    MODBUS_REGISTER[1] &= ~(1 << 14); MODBUS_REGISTER[1] |= I_rung_top << 14;  // YMbPosicOK
    
    /* ] finish series */
    
    /* start rung 226 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_0069) {  // $oneShot_0069
        I_rung_top = 0;
    }
    I_oneShot_0069 = I_scratch;
    
    /* start parallel [ */
    I_parOut_0071 = 0;
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        ENC_Reset();
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[11] &= ~(1 << 0);  // YMbAplanAcelMS
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[12] &= ~(1 << 0);  // YMbAplanVelAuto
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[13] &= ~(1 << 0);  // YMbAplanVelMan
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[10] &= ~(1 << 0);  // YMbAplanPasso
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[20] &= ~(1 << 0);  // YMbEncFator
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[21] &= ~(1 << 0);  // YMbEncResol
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[22] &= ~(1 << 0);  // YMbEncPerim
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[18] &= ~(1 << 0);  // YMbNovoCiclosMil
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[17] &= ~(1 << 0);  // YMbNovoCiclosUnd
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[16] &= ~(1 << 0);  // YMbPrsCiclosMil
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[15] &= ~(1 << 0);  // YMbPrsCiclosUnid
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[14] &= ~(1 << 0);  // YMbMaqErroPos
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_parThis_0071 = I_rung_top;
    if (I_parThis_0071) {  // $parThis_0071
        MODBUS_REGISTER[5] &= ~(1 << 0);  // YMbPosAtual
    }
    
    if (I_parThis_0071) {  // $parThis_0071
        I_parOut_0071 = 1;
    }
    I_rung_top = I_parOut_0071;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 227 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_006a) {  // $oneShot_006a
        I_rung_top = 0;
    }
    I_oneShot_006a = I_scratch;
    
    /* start parallel [ */
    I_parOut_0072 = 0;
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        U_AplanVelMax = 5934;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        U_FatorCorrServo = 10875;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[12] = 100;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[13] = 30;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[11] = 100;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[10] = 320;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[20] = 10000;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[21] = 2500;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[22] = 400;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        U_PrsCiclos = 0;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[18] = 0;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_parThis_0072 = I_rung_top;
    if (I_parThis_0072) {  // $parThis_0072
        MODBUS_REGISTER[17] = 0;
    }
    
    if (I_parThis_0072) {  // $parThis_0072
        I_parOut_0072 = 1;
    }
    I_rung_top = I_parOut_0072;
    /* ] finish parallel */
    /* ] finish series */
    
    /* start rung 230 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_AplanVelAuto = U_AplanVelMax * MODBUS_REGISTER[12];
    }
    
    /* ] finish series */
    
    /* start rung 231 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 100;
        if(I_scratch2) U_AplanVelAuto = U_AplanVelAuto / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 232 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_AplanVelManual = U_AplanVelMax * MODBUS_REGISTER[13];
    }
    
    /* ] finish series */
    
    /* start rung 233 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 100;
        if(I_scratch2) U_AplanVelManual = U_AplanVelManual / I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 234 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 0;
    if (MODBUS_REGISTER[11] > I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        if(MODBUS_REGISTER[11]) U_AplanRampa = U_AplanVelMax / MODBUS_REGISTER[11];
    }
    
    /* ] finish series */
    
    /* start rung 235 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    I_scratch2 = 0;
    if (U_AplanRampa == I_scratch2) {
    } else {
        I_rung_top = 0;
    }
    
    if (I_rung_top) {  // $rung_top
        U_AplanRampa = 1;
    }
    
    /* ] finish series */
    
    /* start rung 236 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 100;
        U_AplanPasso = MODBUS_REGISTER[10] * I_scratch2;
    }
    
    /* ] finish series */
    
    /* start rung 237 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_AplanPassoCorr = U_AplanPasso * U_FatorCorrServo;
    }
    
    /* ] finish series */
    
    /* start rung 238 */
    I_rung_top = I_mcr;
    
    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 10000;
        if(I_scratch2) U_AplanPassoCorr = U_AplanPassoCorr / I_scratch2;
    }
    
    /* ] finish series */
}
