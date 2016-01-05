// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int GPIO_OUTPUT_PORT18 = 0;
volatile int U_TON = 0;
volatile int I_TOFF_antiglitch = 0;
volatile int U_TOFF = 0;
volatile int I_scratch = 0;
volatile int I_oneShot_0000 = 0;
volatile int I_parOut_0000 = 0;
volatile int I_parThis_0000 = 0;
volatile int U_SaidasAnt = 0;
volatile int U_ValSaidas = 0;
volatile int U_VelocidadeMin = 0;
volatile int U_VelocidadeMax = 0;
volatile int U_VelManual = 0;
volatile int U_Velocidade = 0;
volatile int U_Aceleracao = 0;
volatile int U_Desaceleracao = 0;
volatile int U_RSlaveUpdateFreq = 0;
volatile int GPIO_INPUT_PORT1 = 0;
volatile int GPIO_INPUT_PORT5 = 0;
volatile int GPIO_INPUT_PORT4 = 0;
volatile int GPIO_INPUT_PORT2 = 0;
volatile int GPIO_INPUT_PORT3 = 0;
volatile int U_RMaqInitOK = 0;
volatile int U_RMaqOK = 0;
volatile int GPIO_OUTPUT_PORT14 = 0;
volatile int U_TInvPulsoON = 0;
volatile int I_TInvPulsoOff_antiglitch = 0;
volatile int U_TInvPulsoOff = 0;
volatile int U_RMaqSerialLib = 0;
volatile int U_TEspSerialLib = 0;
volatile int I_parOut_0001 = 0;
volatile int I_parThis_0001 = 0;
volatile int U_RMaqSerialBusy = 0;
volatile int I_parOut_0002 = 0;
volatile int I_parThis_0002 = 0;
volatile int U_RSaidasCfgErr = 0;
volatile int U_REntradasCfgErr = 0;
volatile int I_parOut_0003 = 0;
volatile int I_parThis_0003 = 0;
volatile int U_REscravaCfgErr = 0;
volatile int I_oneShot_0001 = 0;
volatile int I_parOut_0004 = 0;
volatile int I_parThis_0004 = 0;
volatile int U_REntradasCfg = 0;
volatile int U_RSaidasCfg = 0;
volatile int U_RInversorCfg = 0;
volatile int U_InversorCfgCnt = 0;
volatile int I_parOut_0005 = 0;
volatile int I_parThis_0005 = 0;
volatile int U_RInversorCfgErr = 0;
volatile int U_TEspLimparErro = 0;
volatile int I_parOut_0006 = 0;
volatile int I_parThis_0006 = 0;
volatile int U_RMaqLib = 0;
volatile int I_oneShot_0002 = 0;
volatile int I_oneShot_0003 = 0;
volatile int I_parOut_0007 = 0;
volatile int I_parThis_0007 = 0;
volatile int GPIO_INPUT_PORT16 = 0;
volatile int I_parOut_0008 = 0;
volatile int I_parThis_0008 = 0;
volatile int GPIO_OUTPUT_PORT17 = 0;
volatile int I_parOut_0009 = 0;
volatile int I_parThis_0009 = 0;
volatile int U_RMaqAutoOK = 0;
volatile int U_RMaqStartAuto = 0;
volatile int I_parOut_000a = 0;
volatile int I_parThis_000a = 0;
volatile int I_oneShot_0004 = 0;
volatile int U_ErroAtual = 0;
volatile int GPIO_INPUT_PORT11 = 0;
volatile int GPIO_INPUT_PORT8 = 0;
volatile int GPIO_INPUT_PORT13 = 0;
volatile int U_ErroAnt = 0;
volatile int U_REnviarErro = 0;
volatile int I_parOut_000b = 0;
volatile int I_parThis_000b = 0;
volatile int I_parOut_000c = 0;
volatile int I_parThis_000c = 0;
volatile int I_oneShot_0005 = 0;
volatile int I_oneShot_0006 = 0;
volatile int U_TEnviarErroTO = 0;
volatile int I_parOut_000d = 0;
volatile int I_parThis_000d = 0;
volatile int I_parOut_000e = 0;
volatile int I_parThis_000e = 0;
volatile int U_RStartEntCfg = 0;
volatile int I_parOut_000f = 0;
volatile int I_parThis_000f = 0;
volatile int U_TEntradasCfg = 0;
volatile int I_parOut_0010 = 0;
volatile int I_parThis_0010 = 0;
volatile int I_oneShot_0007 = 0;
volatile int I_oneShot_0008 = 0;
volatile int I_parOut_0011 = 0;
volatile int I_parThis_0011 = 0;
volatile int U_TEspUpdateFreq = 0;
volatile int I_parOut_0012 = 0;
volatile int I_parThis_0012 = 0;
volatile int U_RStartSaidasCfg = 0;
volatile int I_parOut_0013 = 0;
volatile int I_parThis_0013 = 0;
volatile int U_TSaidasCfgErr = 0;
volatile int I_parOut_0014 = 0;
volatile int I_parThis_0014 = 0;
volatile int I_oneShot_0009 = 0;
volatile int I_oneShot_000a = 0;
volatile int I_parOut_0015 = 0;
volatile int I_parThis_0015 = 0;
volatile int I_parOut_0016 = 0;
volatile int I_parThis_0016 = 0;
volatile int U_VelInv = 0;
volatile int U_VelOffset = 0;
volatile int U_RStartInvCfg = 0;
volatile int I_parOut_0017 = 0;
volatile int I_parThis_0017 = 0;
volatile int U_TInversorCfgErr = 0;
volatile int I_parOut_0018 = 0;
volatile int I_parThis_0018 = 0;
volatile int I_oneShot_000b = 0;
volatile int I_oneShot_000c = 0;
volatile int I_parOut_0019 = 0;
volatile int I_parThis_0019 = 0;
volatile int I_parOut_001a = 0;
volatile int I_parThis_001a = 0;
volatile int I_oneShot_000d = 0;
volatile int I_oneShot_000e = 0;
volatile int I_parOut_001b = 0;
volatile int I_parThis_001b = 0;
volatile int U_PosAnt = 0;
volatile int U_EPosAtual = 0;
volatile int U_DifPos = 0;
volatile int U_DifPosAbs = 0;
volatile int I_scratch2 = 0;
volatile int GPIO_INPUT_PORT9 = 0;
volatile int I_oneShot_000f = 0;
volatile int I_parOut_001c = 0;
volatile int I_parThis_001c = 0;
volatile int U_ZReset = 0;
volatile int U_RMaqRefOK = 0;
volatile int U_AnguloAoZerar = 0;
volatile int U_PulsosEmZero = 0;
volatile int U_AnguloBraco = 0;
volatile int I_parOut_001d = 0;
volatile int I_parThis_001d = 0;
volatile int I_parOut_001e = 0;
volatile int I_parThis_001e = 0;
volatile int I_oneShot_0010 = 0;
volatile int I_oneShot_0011 = 0;
volatile int I_parOut_001f = 0;
volatile int I_parThis_001f = 0;
volatile int U_TEspMaqInit = 0;
volatile int I_parOut_0020 = 0;
volatile int I_parThis_0020 = 0;
volatile int U_RMaqManualOK = 0;
volatile int GPIO_INPUT_PORT7 = 0;
volatile int GPIO_INPUT_PORT12 = 0;
volatile int I_oneShot_0012 = 0;
volatile int I_parOut_0021 = 0;
volatile int I_parThis_0021 = 0;
volatile int I_parOut_0022 = 0;
volatile int I_parThis_0022 = 0;
volatile int U_RExpandeTambor = 0;
volatile int U_RRetraiTambor = 0;
volatile int U_RDesceBraco = 0;
volatile int U_RSobeBraco = 0;
volatile int U_RAvancaPlaca = 0;
volatile int U_RRecuaPlaca = 0;
volatile int U_RSobeCarro = 0;
volatile int U_RDesceCarro = 0;
volatile int U_RSobeFrenagem = 0;
volatile int U_RDesceFrenagem = 0;
volatile int U_RLigarBombaHidr = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;
volatile int GPIO_INPUT_PORT6 = 0;
volatile int U_TEspBombaHidrOK = 0;
volatile int U_RBombaHidrOK = 0;
volatile int I_parOut_0023 = 0;
volatile int I_parThis_0023 = 0;
volatile int U_TEspBombaHidrTO = 0;
volatile int GPIO_OUTPUT_PORT3 = 0;
volatile int GPIO_OUTPUT_PORT2 = 0;
volatile int GPIO_OUTPUT_PORT4 = 0;
volatile int GPIO_OUTPUT_PORT5 = 0;
volatile int GPIO_OUTPUT_PORT6 = 0;
volatile int GPIO_OUTPUT_PORT7 = 0;
volatile int GPIO_OUTPUT_PORT9 = 0;
volatile int GPIO_OUTPUT_PORT10 = 0;
volatile int GPIO_OUTPUT_PORT12 = 0;
volatile int GPIO_OUTPUT_PORT11 = 0;
volatile int GPIO_OUTPUT_PORT8 = 0;
volatile int I_parOut_0024 = 0;
volatile int I_parThis_0024 = 0;
volatile int U_RCmdAutoTambor = 0;
volatile int I_parOut_0025 = 0;
volatile int I_parThis_0025 = 0;
volatile int U_RCmdAutoBraco = 0;
volatile int U_RCmdAutoBracoRef = 0;
volatile int I_parOut_0026 = 0;
volatile int I_parThis_0026 = 0;
volatile int I_parOut_0027 = 0;
volatile int I_parThis_0027 = 0;
volatile int GPIO_INPUT_PORT10 = 0;
volatile int I_parOut_0028 = 0;
volatile int I_parThis_0028 = 0;
volatile int I_parOut_0029 = 0;
volatile int I_parThis_0029 = 0;
volatile int U_RCmdAutoFrenagem = 0;
volatile int I_parOut_002a = 0;
volatile int I_parThis_002a = 0;
volatile int I_parOut_002b = 0;
volatile int I_parThis_002b = 0;
volatile int U_RAvancaTambor = 0;
volatile int U_RRecuaTambor = 0;
volatile int I_parOut_002c = 0;
volatile int I_parThis_002c = 0;
volatile int I_TEspMotorParar_antiglitch = 0;
volatile int U_TEspMotorParar = 0;
volatile int GPIO_OUTPUT_PORT13 = 0;
volatile int U_TEspFreioMotorOK = 0;
volatile int U_RFreioMotorOK = 0;
volatile int I_parOut_002d = 0;
volatile int I_parThis_002d = 0;
volatile int GPIO_OUTPUT_PORT15 = 0;
volatile int GPIO_OUTPUT_PORT16 = 0;
volatile int I_oneShot_0013 = 0;
volatile int I_parOut_002e = 0;
volatile int I_parThis_002e = 0;
volatile int I_TEspPressaoEstab_antiglitch = 0;
volatile int U_TEspPressaoEstab = 0;
volatile int I_parOut_002f = 0;
volatile int I_parThis_002f = 0;
volatile int I_parOut_0030 = 0;
volatile int I_parThis_0030 = 0;
volatile int U_TEspBracoEstab = 0;
volatile int U_RUpdateInv = 0;
volatile int U_TEspUpdateInv = 0;
volatile int I_parOut_0031 = 0;
volatile int I_parThis_0031 = 0;
volatile int GPIO_INPUT_PORT14 = 0;
volatile int I_parOut_0032 = 0;
volatile int I_parThis_0032 = 0;
volatile int GPIO_INPUT_PORT15 = 0;
volatile int I_parOut_0033 = 0;
volatile int I_parThis_0033 = 0;
volatile int I_parOut_0034 = 0;
volatile int I_parThis_0034 = 0;
volatile int I_parOut_0035 = 0;
volatile int I_parThis_0035 = 0;
volatile int I_oneShot_0014 = 0;
volatile int I_oneShot_0015 = 0;
volatile int I_parOut_0036 = 0;
volatile int I_parThis_0036 = 0;
volatile int I_oneShot_0016 = 0;
volatile int I_oneShot_0017 = 0;
volatile int I_parOut_0037 = 0;
volatile int I_parThis_0037 = 0;
volatile int I_oneShot_0018 = 0;
volatile int I_oneShot_0019 = 0;
volatile int I_parOut_0038 = 0;
volatile int I_parThis_0038 = 0;
volatile int I_oneShot_001a = 0;
volatile int I_oneShot_001b = 0;
volatile int I_parOut_0039 = 0;
volatile int I_parThis_0039 = 0;
volatile int I_oneShot_001c = 0;
volatile int I_oneShot_001d = 0;
volatile int I_parOut_003a = 0;
volatile int I_parThis_003a = 0;
volatile int I_oneShot_001e = 0;
volatile int I_oneShot_001f = 0;
volatile int I_parOut_003b = 0;
volatile int I_parThis_003b = 0;
volatile int I_oneShot_0020 = 0;
volatile int I_oneShot_0021 = 0;
volatile int I_parOut_003c = 0;
volatile int I_parThis_003c = 0;
volatile int I_oneShot_0022 = 0;
volatile int I_oneShot_0023 = 0;
volatile int I_parOut_003d = 0;
volatile int I_parThis_003d = 0;
volatile int I_oneShot_0024 = 0;
volatile int I_oneShot_0025 = 0;
volatile int U_RFlagDummy = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_OUTPUT_PORT18) {  // YTMR
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TON < 99) {
            U_TON++;
            I_rung_top = 0;
        }
    } else {
        U_TON = 0;
    }

    if (!I_TOFF_antiglitch) {  // $TOFF_antiglitch
        U_TOFF = 99;
    }
    I_TOFF_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TOFF < 99) {
            U_TOFF++;
            I_rung_top = 1;
        }
    } else {
        U_TOFF = 0;
    }

    GPIO_OUTPUT_PORT18 = I_rung_top;

    /* ] finish series */

    /* start rung 6 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch = I_rung_top;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_rung_top = 0;
    }
    I_oneShot_0000 = I_scratch;

    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[0] &= ~(1 << 0);  // YMbEntradas
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[0] = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[1] &= ~(1 << 0);  // YMbSaidas
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        MODBUS_REGISTER[1] = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_SaidasAnt = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_ValSaidas = 0;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_VelocidadeMin = 100;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_VelocidadeMax = 6000;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_VelManual = 500;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_Velocidade = U_VelocidadeMin;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_Aceleracao = 50;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_Desaceleracao = 100;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    if (I_parThis_0000) {  // $parThis_0000
        U_RSlaveUpdateFreq = 1;
    }

    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 8 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT1) {  // XEmergencia
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT5) {  // XSupervisorFase
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT4) {  // XInversorEmerg
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT2) {  // XTermicoBomba
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT3) {  // XTermicoCarro
        I_rung_top = 0;
    }

    if (!U_RMaqInitOK) {  // RMaqInitOK
        I_rung_top = 0;
    }

    U_RMaqOK = I_rung_top;

    /* ] finish series */

    /* start rung 9 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[0] & (1 << 0)) {  // XMaqModoAuto
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT4) {  // XInversorEmerg
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT14) {  // YInvLimparErro
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TInvPulsoON < 49) {
            U_TInvPulsoON++;
            I_rung_top = 0;
        }
    } else {
        U_TInvPulsoON = 0;
    }

    if (!I_TInvPulsoOff_antiglitch) {  // $TInvPulsoOff_antiglitch
        U_TInvPulsoOff = 9;
    }
    I_TInvPulsoOff_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TInvPulsoOff < 9) {
            U_TInvPulsoOff++;
            I_rung_top = 1;
        }
    } else {
        U_TInvPulsoOff = 0;
    }

    GPIO_OUTPUT_PORT14 = I_rung_top;

    /* ] finish series */

    /* start rung 10 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqSerialLib) {  // RMaqSerialLib
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspSerialLib < 9) {
            U_TEspSerialLib++;
            I_rung_top = 0;
        }
    } else {
        U_TEspSerialLib = 0;
    }

    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        U_RMaqSerialBusy = 0;
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_rung_top;
    if (I_parThis_0001) {  // $parThis_0001
        U_RMaqSerialLib = 0;
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_rung_top = I_parOut_0001;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 11 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0002 = 0;
    I_parThis_0002 = I_rung_top;
    if (!U_RSaidasCfgErr) {  // RSaidasCfgErr
        I_parThis_0002 = 0;
    }

    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    if (!U_REntradasCfgErr) {  // REntradasCfgErr
        I_parThis_0002 = 0;
    }

    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_rung_top = I_parOut_0002;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0003 = 0;
    I_parThis_0003 = I_rung_top;
    U_REscravaCfgErr = I_parThis_0003;

    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_0003;
    if (I_oneShot_0001) {  // $oneShot_0001
        I_parThis_0003 = 0;
    }
    I_oneShot_0001 = I_scratch;

    /* start parallel [ */
    I_parOut_0004 = 0;
    I_parThis_0004 = I_parThis_0003;
    if (I_parThis_0004) {  // $parThis_0004
        U_REntradasCfg = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_parThis_0003;
    if (I_parThis_0004) {  // $parThis_0004
        U_RSaidasCfg = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_parThis_0003;
    if (I_parThis_0004) {  // $parThis_0004
        U_RSlaveUpdateFreq = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_parThis_0003;
    if (I_parThis_0004) {  // $parThis_0004
        U_RInversorCfg = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_parThis_0003;
    if (I_parThis_0004) {  // $parThis_0004
        U_InversorCfgCnt = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0003 = I_parOut_0004;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_rung_top = I_parOut_0003;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 12 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0005 = 0;
    I_parThis_0005 = I_rung_top;
    if (!U_REscravaCfgErr) {  // REscravaCfgErr
        I_parThis_0005 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    if (!U_RInversorCfgErr) {  // RInversorCfgErr
        I_parThis_0005 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_rung_top = I_parOut_0005;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        if (U_TEspLimparErro < 49) {
            U_TEspLimparErro++;
            I_rung_top = 0;
        }
    } else {
        U_TEspLimparErro = 0;
    }

    /* start parallel [ */
    I_parOut_0006 = 0;
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_RInversorCfgErr = 0;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_RSaidasCfgErr = 0;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_REntradasCfgErr = 0;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_RMaqSerialBusy = 0;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    if (I_parThis_0006) {  // $parThis_0006
        U_RMaqLib = 0;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_rung_top = I_parOut_0006;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 13 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RInversorCfgErr) {  // RInversorCfgErr
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0002) {  // $oneShot_0002
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0002 = I_scratch;

    if (I_rung_top) {  // $rung_top
        U_RInversorCfg = 1;
    }

    /* ] finish series */

    /* start rung 14 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RSaidasCfgErr) {  // RSaidasCfgErr
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (!I_rung_top) {  // $rung_top
        if (I_oneShot_0003) {  // $oneShot_0003
            I_rung_top = 1;
        }
    } else {
        I_rung_top = 0;
    }
    I_oneShot_0003 = I_scratch;

    if (I_rung_top) {  // $rung_top
        U_SaidasAnt = -1;
    }

    /* ] finish series */

    /* start rung 15 */
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
    if (!U_RInversorCfgErr) {  // RInversorCfgErr
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    if (!U_REscravaCfgErr) {  // REscravaCfgErr
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_parThis_0007 = 0;
    }

    if (GPIO_INPUT_PORT16) {  // XFimMaterial
        I_parThis_0007 = 0;
    }

    /* ] finish series */
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_rung_top = I_parOut_0007;
    /* ] finish parallel */
    MODBUS_REGISTER[1] &= ~(1 << 10); MODBUS_REGISTER[1] |= I_rung_top << 10;  // YMaqDesligaAuto

    /* ] finish series */

    /* start rung 16 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0008 = 0;
    I_parThis_0008 = I_rung_top;
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_0008 = 0;
    }

    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_parThis_0008 = I_rung_top;
    if (!U_RInversorCfgErr) {  // RInversorCfgErr
        I_parThis_0008 = 0;
    }

    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_parThis_0008 = I_rung_top;
    if (!U_REscravaCfgErr) {  // REscravaCfgErr
        I_parThis_0008 = 0;
    }

    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_rung_top = I_parOut_0008;
    /* ] finish parallel */
    GPIO_OUTPUT_PORT17 = I_rung_top;

    /* ] finish series */

    /* start rung 20 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0009 = 0;
    I_parThis_0009 = I_rung_top;
    if (!GPIO_OUTPUT_PORT17) {  // YLED_ERRO
        I_parThis_0009 = 0;
    }

    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_parThis_0009 = I_rung_top;
    /* start series [ */
    if (U_RMaqAutoOK) {  // RMaqAutoOK
        I_parThis_0009 = 0;
    }

    if (U_RMaqStartAuto) {  // RMaqStartAuto
        I_parThis_0009 = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_parThis_0009 = 0;
    }

    /* ] finish series */
    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_rung_top = I_parOut_0009;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_000a = 0;
    I_parThis_000a = I_rung_top;
    /* start series [ */
    I_scratch = I_parThis_000a;
    if (!I_parThis_000a) {  // $parThis_000a
        if (I_oneShot_0004) {  // $oneShot_0004
            I_parThis_000a = 1;
        }
    } else {
        I_parThis_000a = 0;
    }
    I_oneShot_0004 = I_scratch;

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 0;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_parThis_000a = 0;
    }

    if (GPIO_INPUT_PORT16) {  // XFimMaterial
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 34;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_parThis_000a = 0;
    }

    if (GPIO_INPUT_PORT11) {  // XPlacaRecuada
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 33;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_parThis_000a = 0;
    }

    if (GPIO_INPUT_PORT8) {  // XPressaoTambor
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 32;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_parThis_000a = 0;
    }

    if (GPIO_INPUT_PORT13) {  // XPressaoFrenagem
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 31;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (!U_REscravaCfgErr) {  // REscravaCfgErr
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 22;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (!U_RInversorCfgErr) {  // RInversorCfgErr
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 21;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (GPIO_INPUT_PORT3) {  // XTermicoCarro
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 14;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (GPIO_INPUT_PORT2) {  // XTermicoBomba
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 13;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (GPIO_INPUT_PORT4) {  // XInversorEmerg
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 12;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_rung_top;
    /* start series [ */
    if (GPIO_INPUT_PORT5) {  // XSupervisorFase
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        U_ErroAtual = 11;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_rung_top = I_parOut_000a;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 21 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_ErroAtual == U_ErroAnt) {
        I_rung_top = 0;
    }

    if (U_REnviarErro) {  // REnviarErro
        I_rung_top = 0;
    }

    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_000b = 0;
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_ErroAnt = U_ErroAtual;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_REnviarErro = 1;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        U_RMaqSerialBusy = 1;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_rung_top = I_parOut_000b;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 22 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_REnviarErro) {  // REnviarErro
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_000c = 0;
    I_parThis_000c = I_rung_top;
    if (I_parThis_000c) {  // $parThis_000c
        if (!I_oneShot_0005) {  // $oneShot_0005
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 2, 1, &U_ErroAtual);
                I_oneShot_0005 = I_parThis_000c;
            }
            I_parThis_000c = 0;
            I_oneShot_0006 = I_parThis_000c;
        }
        if (!I_oneShot_0006) {  // $oneShot_0006
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0006 = 1;
            } else {
                I_parThis_000c = 0;
            }
        }
    } else {
        I_oneShot_0005 = I_parThis_000c;
    }

    if (I_parThis_000c) {  // $parThis_000c
        I_parOut_000c = 1;
    }
    I_parThis_000c = I_rung_top;
    if (I_parThis_000c) {  // $parThis_000c
        if (U_TEnviarErroTO < 99) {
            U_TEnviarErroTO++;
            I_parThis_000c = 0;
        }
    } else {
        U_TEnviarErroTO = 0;
    }

    if (I_parThis_000c) {  // $parThis_000c
        I_parOut_000c = 1;
    }
    I_rung_top = I_parOut_000c;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_000d = 0;
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {  // $parThis_000d
        U_REnviarErro = 0;
    }

    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_parThis_000d = I_rung_top;
    if (I_parThis_000d) {  // $parThis_000d
        U_RMaqSerialLib = 1;
    }

    if (I_parThis_000d) {  // $parThis_000d
        I_parOut_000d = 1;
    }
    I_rung_top = I_parOut_000d;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 24 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqInitOK) {  // RMaqInitOK
        I_rung_top = 0;
    }

    if (!U_REntradasCfg) {  // REntradasCfg
        I_rung_top = 0;
    }

    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_000e = 0;
    I_parThis_000e = I_rung_top;
    if (I_parThis_000e) {  // $parThis_000e
        U_RStartEntCfg = 1;
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
    I_parThis_000e = I_rung_top;
    if (I_parThis_000e) {  // $parThis_000e
        U_REntradasCfg = 0;
    }

    if (I_parThis_000e) {  // $parThis_000e
        I_parOut_000e = 1;
    }
    I_rung_top = I_parOut_000e;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 25 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartEntCfg) {  // RStartEntCfg
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_000f = 0;
    I_parThis_000f = I_rung_top;
    /* start series [ */
    if (I_parThis_000f) {  // $parThis_000f
        if (U_TEntradasCfg < 99) {
            U_TEntradasCfg++;
            I_parThis_000f = 0;
        }
    } else {
        U_TEntradasCfg = 0;
    }

    /* start parallel [ */
    I_parOut_0010 = 0;
    I_parThis_0010 = I_parThis_000f;
    if (I_parThis_0010) {  // $parThis_0010
        U_REntradasCfgErr = 1;
    }

    if (I_parThis_0010) {  // $parThis_0010
        I_parOut_0010 = 1;
    }
    I_parThis_0010 = I_parThis_000f;
    if (I_parThis_0010) {  // $parThis_0010
        U_RStartEntCfg = 0;
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
    I_parThis_000f = I_rung_top;
    /* start series [ */
    if (I_parThis_000f) {  // $parThis_000f
        if (!I_oneShot_0007) {  // $oneShot_0007
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(0, MODBUS_FC_READ_HOLDING_REGISTERS, 0, 1, &MODBUS_REGISTER[0]);
                I_oneShot_0007 = I_parThis_000f;
            }
            I_parThis_000f = 0;
            I_oneShot_0008 = I_parThis_000f;
        }
        if (!I_oneShot_0008) {  // $oneShot_0008
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0008 = 1;
            } else {
                I_parThis_000f = 0;
            }
        }
    } else {
        I_oneShot_0007 = I_parThis_000f;
    }

    /* start parallel [ */
    I_parOut_0011 = 0;
    I_parThis_0011 = I_parThis_000f;
    if (I_parThis_0011) {  // $parThis_0011
        U_RStartEntCfg = 0;
    }

    if (I_parThis_0011) {  // $parThis_0011
        I_parOut_0011 = 1;
    }
    I_parThis_0011 = I_parThis_000f;
    if (I_parThis_0011) {  // $parThis_0011
        U_RMaqSerialLib = 1;
    }

    if (I_parThis_0011) {  // $parThis_0011
        I_parOut_0011 = 1;
    }
    I_parThis_000f = I_parOut_0011;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_000f) {  // $parThis_000f
        I_parOut_000f = 1;
    }
    I_rung_top = I_parOut_000f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 27 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_REntradasCfg) {  // REntradasCfg
        I_rung_top = 0;
    }

    if (U_RStartEntCfg) {  // RStartEntCfg
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspUpdateFreq < 14) {
            U_TEspUpdateFreq++;
            I_rung_top = 0;
        }
    } else {
        U_TEspUpdateFreq = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_REntradasCfg = 1;
    }

    /* ] finish series */

    /* start rung 29 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqInitOK) {  // RMaqInitOK
        I_rung_top = 0;
    }

    if (U_SaidasAnt == U_ValSaidas) {
        I_rung_top = 0;
    }

    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0012 = 0;
    I_parThis_0012 = I_rung_top;
    if (I_parThis_0012) {  // $parThis_0012
        U_RStartSaidasCfg = 1;
    }

    if (I_parThis_0012) {  // $parThis_0012
        I_parOut_0012 = 1;
    }
    I_parThis_0012 = I_rung_top;
    if (I_parThis_0012) {  // $parThis_0012
        U_RMaqSerialBusy = 1;
    }

    if (I_parThis_0012) {  // $parThis_0012
        I_parOut_0012 = 1;
    }
    I_parThis_0012 = I_rung_top;
    if (I_parThis_0012) {  // $parThis_0012
        U_SaidasAnt = U_ValSaidas;
    }

    if (I_parThis_0012) {  // $parThis_0012
        I_parOut_0012 = 1;
    }
    I_rung_top = I_parOut_0012;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 30 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartSaidasCfg) {  // RStartSaidasCfg
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0013 = 0;
    I_parThis_0013 = I_rung_top;
    /* start series [ */
    if (I_parThis_0013) {  // $parThis_0013
        if (U_TSaidasCfgErr < 99) {
            U_TSaidasCfgErr++;
            I_parThis_0013 = 0;
        }
    } else {
        U_TSaidasCfgErr = 0;
    }

    /* start parallel [ */
    I_parOut_0014 = 0;
    I_parThis_0014 = I_parThis_0013;
    if (I_parThis_0014) {  // $parThis_0014
        U_RSaidasCfgErr = 1;
    }

    if (I_parThis_0014) {  // $parThis_0014
        I_parOut_0014 = 1;
    }
    I_parThis_0014 = I_parThis_0013;
    if (I_parThis_0014) {  // $parThis_0014
        U_RStartSaidasCfg = 0;
    }

    if (I_parThis_0014) {  // $parThis_0014
        I_parOut_0014 = 1;
    }
    I_parThis_0013 = I_parOut_0014;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0013) {  // $parThis_0013
        I_parOut_0013 = 1;
    }
    I_parThis_0013 = I_rung_top;
    /* start series [ */
    if (I_parThis_0013) {  // $parThis_0013
        if (!I_oneShot_0009) {  // $oneShot_0009
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 1, 1, &MODBUS_REGISTER[1]);
                I_oneShot_0009 = I_parThis_0013;
            }
            I_parThis_0013 = 0;
            I_oneShot_000a = I_parThis_0013;
        }
        if (!I_oneShot_000a) {  // $oneShot_000a
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_000a = 1;
            } else {
                I_parThis_0013 = 0;
            }
        }
    } else {
        I_oneShot_0009 = I_parThis_0013;
    }

    /* start parallel [ */
    I_parOut_0015 = 0;
    I_parThis_0015 = I_parThis_0013;
    if (I_parThis_0015) {  // $parThis_0015
        U_RMaqSerialLib = 1;
    }

    if (I_parThis_0015) {  // $parThis_0015
        I_parOut_0015 = 1;
    }
    I_parThis_0015 = I_parThis_0013;
    if (I_parThis_0015) {  // $parThis_0015
        U_RStartSaidasCfg = 0;
    }

    if (I_parThis_0015) {  // $parThis_0015
        I_parOut_0015 = 1;
    }
    I_parThis_0013 = I_parOut_0015;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0013) {  // $parThis_0013
        I_parOut_0013 = 1;
    }
    I_rung_top = I_parOut_0013;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 32 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqInitOK) {  // RMaqInitOK
        I_rung_top = 0;
    }

    if (!U_RInversorCfg) {  // RInversorCfg
        I_rung_top = 0;
    }

    if (U_RMaqSerialBusy) {  // RMaqSerialBusy
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0016 = 0;
    I_parThis_0016 = I_rung_top;
    if (I_parThis_0016) {  // $parThis_0016
        U_RInversorCfg = 0;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_rung_top;
    if (I_parThis_0016) {  // $parThis_0016
        U_VelInv = U_Velocidade + U_VelOffset;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_rung_top;
    /* start series [ */
    if (U_VelInv > U_VelocidadeMax) {
    } else {
        I_parThis_0016 = 0;
    }

    if (I_parThis_0016) {  // $parThis_0016
        U_VelInv = U_VelocidadeMax;
    }

    /* ] finish series */
    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_rung_top;
    /* start series [ */
    if (U_VelocidadeMin > U_VelInv) {
    } else {
        I_parThis_0016 = 0;
    }

    if (I_parThis_0016) {  // $parThis_0016
        U_VelInv = U_VelocidadeMin;
    }

    /* ] finish series */
    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_rung_top;
    if (I_parThis_0016) {  // $parThis_0016
        U_RMaqSerialBusy = 1;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_parThis_0016 = I_rung_top;
    if (I_parThis_0016) {  // $parThis_0016
        U_RStartInvCfg = 1;
    }

    if (I_parThis_0016) {  // $parThis_0016
        I_parOut_0016 = 1;
    }
    I_rung_top = I_parOut_0016;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 33 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RStartInvCfg) {  // RStartInvCfg
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0017 = 0;
    I_parThis_0017 = I_rung_top;
    /* start series [ */
    if (I_parThis_0017) {  // $parThis_0017
        if (U_TInversorCfgErr < 99) {
            U_TInversorCfgErr++;
            I_parThis_0017 = 0;
        }
    } else {
        U_TInversorCfgErr = 0;
    }

    /* start parallel [ */
    I_parOut_0018 = 0;
    I_parThis_0018 = I_parThis_0017;
    if (I_parThis_0018) {  // $parThis_0018
        U_RInversorCfgErr = 1;
    }

    if (I_parThis_0018) {  // $parThis_0018
        I_parOut_0018 = 1;
    }
    I_parThis_0018 = I_parThis_0017;
    if (I_parThis_0018) {  // $parThis_0018
        U_RStartInvCfg = 0;
    }

    if (I_parThis_0018) {  // $parThis_0018
        I_parOut_0018 = 1;
    }
    I_parThis_0017 = I_parOut_0018;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0017) {  // $parThis_0017
        I_parOut_0017 = 1;
    }
    I_parThis_0017 = I_rung_top;
    /* start series [ */
    if (I_parThis_0017) {  // $parThis_0017
        if (!I_oneShot_000b) {  // $oneShot_000b
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(1, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 8193, 1, &U_VelInv);
                I_oneShot_000b = I_parThis_0017;
            }
            I_parThis_0017 = 0;
            I_oneShot_000c = I_parThis_0017;
        }
        if (!I_oneShot_000c) {  // $oneShot_000c
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_000c = 1;
            } else {
                I_parThis_0017 = 0;
            }
        }
    } else {
        I_oneShot_000b = I_parThis_0017;
    }

    /* start parallel [ */
    I_parOut_0019 = 0;
    I_parThis_0019 = I_parThis_0017;
    if (I_parThis_0019) {  // $parThis_0019
        U_RStartInvCfg = 0;
    }

    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_parThis_0019 = I_parThis_0017;
    if (I_parThis_0019) {  // $parThis_0019
        U_RMaqSerialLib = 1;
    }

    if (I_parThis_0019) {  // $parThis_0019
        I_parOut_0019 = 1;
    }
    I_parThis_0017 = I_parOut_0019;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0017) {  // $parThis_0017
        I_parOut_0017 = 1;
    }
    I_rung_top = I_parOut_0017;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 35 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT18) {  // YTMR
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_001a = 0;
    I_parThis_001a = I_rung_top;
    I_scratch = I_parThis_001a;
    if (I_oneShot_000d) {  // $oneShot_000d
        I_parThis_001a = 0;
    }
    I_oneShot_000d = I_scratch;

    if (I_parThis_001a) {  // $parThis_001a
        I_parOut_001a = 1;
    }
    I_parThis_001a = I_rung_top;
    I_scratch = I_parThis_001a;
    if (!I_parThis_001a) {  // $parThis_001a
        if (I_oneShot_000e) {  // $oneShot_000e
            I_parThis_001a = 1;
        }
    } else {
        I_parThis_001a = 0;
    }
    I_oneShot_000e = I_scratch;

    if (I_parThis_001a) {  // $parThis_001a
        I_parOut_001a = 1;
    }
    I_rung_top = I_parOut_001a;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_001b = 0;
    I_parThis_001b = I_rung_top;
    if (I_parThis_001b) {  // $parThis_001b
        U_PosAnt = U_EPosAtual;
    }

    if (I_parThis_001b) {  // $parThis_001b
        I_parOut_001b = 1;
    }
    I_parThis_001b = I_rung_top;
    if (I_parThis_001b) {  // $parThis_001b
        U_EPosAtual = ENC_Read();
    }

    if (I_parThis_001b) {  // $parThis_001b
        I_parOut_001b = 1;
    }
    I_rung_top = I_parOut_001b;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 36 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_DifPos = U_EPosAtual - U_PosAnt;
    }

    /* ] finish series */

    /* start rung 37 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_DifPosAbs = U_DifPos;
    }

    /* ] finish series */

    /* start rung 38 */
    I_rung_top = I_mcr;

    /* start series [ */
    I_scratch2 = 0;
    if (I_scratch2 > U_DifPos) {
    } else {
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        I_scratch2 = -1;
        U_DifPosAbs = U_DifPos * I_scratch2;
    }

    /* ] finish series */

    /* start rung 39 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT9) {  // XBracoLevantado
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_000f) {  // $oneShot_000f
        I_rung_top = 0;
    }
    I_oneShot_000f = I_scratch;

    if (!U_RMaqInitOK) {  // RMaqInitOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_001c = 0;
    I_parThis_001c = I_rung_top;
    if (I_parThis_001c) {  // $parThis_001c
        ENC_Reset();
    }

    if (I_parThis_001c) {  // $parThis_001c
        I_parOut_001c = 1;
    }
    I_parThis_001c = I_rung_top;
    if (I_parThis_001c) {  // $parThis_001c
        U_RMaqRefOK = 1;
    }

    if (I_parThis_001c) {  // $parThis_001c
        I_parOut_001c = 1;
    }
    I_rung_top = I_parOut_001c;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 41 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_AnguloAoZerar = 75;
    }

    /* ] finish series */

    /* start rung 42 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 5000;
        U_PulsosEmZero = U_AnguloAoZerar * I_scratch2;
    }

    /* ] finish series */

    /* start rung 43 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        I_scratch2 = 360;
        U_PulsosEmZero = U_PulsosEmZero / I_scratch2;
    }

    /* ] finish series */

    /* start rung 44 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_AnguloBraco = U_PulsosEmZero - U_EPosAtual;
    }

    /* ] finish series */

    /* start rung 45 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_AnguloBraco = U_AnguloAoZerar * U_AnguloBraco;
    }

    /* ] finish series */

    /* start rung 46 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_AnguloBraco = U_AnguloBraco / U_PulsosEmZero;
    }

    /* ] finish series */

    /* start rung 48 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_001d = 0;
    I_parThis_001d = I_rung_top;
    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 5;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 12384;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 10;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 8456;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 15;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 5999;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 20;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 4545;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 25;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 3629;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 30;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 3012;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 35;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 2573;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 40;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 2246;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 45;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1996;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 50;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1798;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 55;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1638;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 60;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1506;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 65;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1397;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 70;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1305;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 75;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1226;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 80;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1158;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_parThis_001d = I_rung_top;
    /* start series [ */
    I_scratch2 = 85;
    if (U_AnguloBraco > I_scratch2) {
    } else {
        I_parThis_001d = 0;
    }

    if (I_parThis_001d) {  // $parThis_001d
        U_Velocidade = 1100;
    }

    /* ] finish series */
    if (I_parThis_001d) {  // $parThis_001d
        I_parOut_001d = 1;
    }
    I_rung_top = I_parOut_001d;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 49 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[0] & (1 << 0)) {  // XMaqModoAuto
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        U_Velocidade = U_VelManual;
    }

    /* ] finish series */

    /* start rung 51 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_001e = 0;
    I_parThis_001e = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_parThis_001e = 0;
    }

    I_scratch = I_parThis_001e;
    if (!I_parThis_001e) {  // $parThis_001e
        if (I_oneShot_0010) {  // $oneShot_0010
            I_parThis_001e = 1;
        }
    } else {
        I_parThis_001e = 0;
    }
    I_oneShot_0010 = I_scratch;

    /* ] finish series */
    if (I_parThis_001e) {  // $parThis_001e
        I_parOut_001e = 1;
    }
    I_parThis_001e = I_rung_top;
    I_scratch = I_parThis_001e;
    if (I_oneShot_0011) {  // $oneShot_0011
        I_parThis_001e = 0;
    }
    I_oneShot_0011 = I_scratch;

    if (I_parThis_001e) {  // $parThis_001e
        I_parOut_001e = 1;
    }
    I_rung_top = I_parOut_001e;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_001f = 0;
    I_parThis_001f = I_rung_top;
    if (I_parThis_001f) {  // $parThis_001f
        U_Velocidade = U_VelManual;
    }

    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_parThis_001f = I_rung_top;
    if (I_parThis_001f) {  // $parThis_001f
        U_RInversorCfg = 1;
    }

    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_parThis_001f = I_rung_top;
    if (I_parThis_001f) {  // $parThis_001f
        U_InversorCfgCnt = 0;
    }

    if (I_parThis_001f) {  // $parThis_001f
        I_parOut_001f = 1;
    }
    I_rung_top = I_parOut_001f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 53 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        if (U_TEspMaqInit < 299) {
            U_TEspMaqInit++;
            I_rung_top = 0;
        }
    } else {
        U_TEspMaqInit = 0;
    }

    U_RMaqInitOK = I_rung_top;

    /* ] finish series */

    /* start rung 55 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // XEmergencia
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 0); MODBUS_REGISTER[1] |= I_rung_top << 0;  // YLampEmergencia

    /* ] finish series */

    /* start rung 56 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0020 = 0;
    I_parThis_0020 = I_rung_top;
    if (!U_RMaqAutoOK) {  // RMaqAutoOK
        I_parThis_0020 = 0;
    }

    if (I_parThis_0020) {  // $parThis_0020
        I_parOut_0020 = 1;
    }
    I_parThis_0020 = I_rung_top;
    if (!U_RMaqStartAuto) {  // RMaqStartAuto
        I_parThis_0020 = 0;
    }

    if (I_parThis_0020) {  // $parThis_0020
        I_parOut_0020 = 1;
    }
    I_rung_top = I_parOut_0020;
    /* ] finish parallel */
    MODBUS_REGISTER[1] &= ~(1 << 6); MODBUS_REGISTER[1] |= I_rung_top << 6;  // YLampAutomatico

    /* ] finish series */

    /* start rung 57 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 7); MODBUS_REGISTER[1] |= I_rung_top << 7;  // YLampManual

    /* ] finish series */

    /* start rung 58 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT7) {  // XPressFiltroSujo
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 3); MODBUS_REGISTER[1] |= I_rung_top << 3;  // YLampFiltroSujo

    /* ] finish series */

    /* start rung 59 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT12) {  // XCarroAvancado
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 2); MODBUS_REGISTER[1] |= I_rung_top << 2;  // YLampCarroAvanca

    /* ] finish series */

    /* start rung 60 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT13) {  // XPressaoFrenagem
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 4); MODBUS_REGISTER[1] |= I_rung_top << 4;  // YLampFreioAlto

    /* ] finish series */

    /* start rung 61 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT8) {  // XPressaoTambor
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 5); MODBUS_REGISTER[1] |= I_rung_top << 5;  // YLampMandrilRet

    /* ] finish series */

    /* start rung 63 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0012) {  // $oneShot_0012
        I_rung_top = 0;
    }
    I_oneShot_0012 = I_scratch;

    /* start parallel [ */
    I_parOut_0021 = 0;
    I_parThis_0021 = I_rung_top;
    if (I_parThis_0021) {  // $parThis_0021
        U_Velocidade = U_VelocidadeMin;
    }

    if (I_parThis_0021) {  // $parThis_0021
        I_parOut_0021 = 1;
    }
    I_parThis_0021 = I_rung_top;
    if (I_parThis_0021) {  // $parThis_0021
        U_RInversorCfg = 1;
    }

    if (I_parThis_0021) {  // $parThis_0021
        I_parOut_0021 = 1;
    }
    I_parThis_0021 = I_rung_top;
    if (I_parThis_0021) {  // $parThis_0021
        U_RInversorCfgErr = 0;
    }

    if (I_parThis_0021) {  // $parThis_0021
        I_parOut_0021 = 1;
    }
    I_rung_top = I_parOut_0021;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 65 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0022 = 0;
    I_parThis_0022 = I_rung_top;
    if (!U_RExpandeTambor) {  // RExpandeTambor
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RRetraiTambor) {  // RRetraiTambor
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RDesceBraco) {  // RDesceBraco
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RSobeBraco) {  // RSobeBraco
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RAvancaPlaca) {  // RAvancaPlaca
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RRecuaPlaca) {  // RRecuaPlaca
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RSobeCarro) {  // RSobeCarro
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RDesceCarro) {  // RDesceCarro
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RSobeFrenagem) {  // RSobeFrenagem
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!U_RDesceFrenagem) {  // RDesceFrenagem
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_parThis_0022 = I_rung_top;
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_parThis_0022 = 0;
    }

    if (I_parThis_0022) {  // $parThis_0022
        I_parOut_0022 = 1;
    }
    I_rung_top = I_parOut_0022;
    /* ] finish parallel */
    U_RLigarBombaHidr = I_rung_top;

    /* ] finish series */

    /* start rung 66 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (GPIO_OUTPUT_PORT1) {  // YBombaHidraulica
        I_rung_top = 0;
    }

    if (!U_RLigarBombaHidr) {  // RLigarBombaHidr
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 1;
    }

    /* ] finish series */

    /* start rung 67 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT1) {  // YBombaHidraulica
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT6) {  // XBombaHidrLigada
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspBombaHidrOK < 99) {
            U_TEspBombaHidrOK++;
            I_rung_top = 0;
        }
    } else {
        U_TEspBombaHidrOK = 0;
    }

    U_RBombaHidrOK = I_rung_top;

    /* ] finish series */

    /* start rung 68 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0023 = 0;
    I_parThis_0023 = I_rung_top;
    /* start series [ */
    if (U_RLigarBombaHidr) {  // RLigarBombaHidr
        I_parThis_0023 = 0;
    }

    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_parThis_0023 = 0;
    }

    if (I_parThis_0023) {  // $parThis_0023
        if (U_TEspBombaHidrTO < 29999) {
            U_TEspBombaHidrTO++;
            I_parThis_0023 = 0;
        }
    } else {
        U_TEspBombaHidrTO = 0;
    }

    /* ] finish series */
    if (I_parThis_0023) {  // $parThis_0023
        I_parOut_0023 = 1;
    }
    I_parThis_0023 = I_rung_top;
    if (U_RMaqOK) {  // RMaqOK
        I_parThis_0023 = 0;
    }

    if (I_parThis_0023) {  // $parThis_0023
        I_parOut_0023 = 1;
    }
    I_rung_top = I_parOut_0023;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        GPIO_OUTPUT_PORT1 = 0;
    }

    /* ] finish series */

    /* start rung 70 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RExpandeTambor) {  // RExpandeTambor
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT3 = I_rung_top;

    /* ] finish series */

    /* start rung 71 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RRetraiTambor) {  // RRetraiTambor
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT2 = I_rung_top;

    /* ] finish series */

    /* start rung 72 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RDesceBraco) {  // RDesceBraco
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT4 = I_rung_top;

    /* ] finish series */

    /* start rung 73 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RSobeBraco) {  // RSobeBraco
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT5 = I_rung_top;

    /* ] finish series */

    /* start rung 74 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RAvancaPlaca) {  // RAvancaPlaca
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT6 = I_rung_top;

    /* ] finish series */

    /* start rung 75 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RRecuaPlaca) {  // RRecuaPlaca
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT7 = I_rung_top;

    /* ] finish series */

    /* start rung 76 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RSobeCarro) {  // RSobeCarro
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT9 = I_rung_top;

    /* ] finish series */

    /* start rung 77 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RDesceCarro) {  // RDesceCarro
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT10 = I_rung_top;

    /* ] finish series */

    /* start rung 78 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RSobeFrenagem) {  // RSobeFrenagem
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT12 = I_rung_top;

    /* ] finish series */

    /* start rung 79 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RDesceFrenagem) {  // RDesceFrenagem
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT11 = I_rung_top;

    /* ] finish series */

    /* start rung 80 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RBombaHidrOK) {  // RBombaHidrOK
        I_rung_top = 0;
    }

    if (!U_RLigarBombaHidr) {  // RLigarBombaHidr
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT8 = I_rung_top;

    /* ] finish series */

    /* start rung 82 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 15))) {  // XMaqModoManual
        I_rung_top = 0;
    }

    U_RMaqManualOK = I_rung_top;

    /* ] finish series */

    /* start rung 83 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0024 = 0;
    I_parThis_0024 = I_rung_top;
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_0024 = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 1))) {  // XCmdTmbExpandir
        I_parThis_0024 = 0;
    }

    /* ] finish series */
    if (I_parThis_0024) {  // $parThis_0024
        I_parOut_0024 = 1;
    }
    I_parThis_0024 = I_rung_top;
    if (!U_RMaqStartAuto) {  // RMaqStartAuto
        I_parThis_0024 = 0;
    }

    if (I_parThis_0024) {  // $parThis_0024
        I_parOut_0024 = 1;
    }
    I_parThis_0024 = I_rung_top;
    if (!U_RCmdAutoTambor) {  // RCmdAutoTambor
        I_parThis_0024 = 0;
    }

    if (I_parThis_0024) {  // $parThis_0024
        I_parOut_0024 = 1;
    }
    I_rung_top = I_parOut_0024;
    /* ] finish parallel */
    if (GPIO_INPUT_PORT8) {  // XPressaoTambor
        I_rung_top = 0;
    }

    U_RExpandeTambor = I_rung_top;

    /* ] finish series */

    /* start rung 84 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 2))) {  // XCmdTmbRetrair
        I_rung_top = 0;
    }

    U_RRetraiTambor = I_rung_top;

    /* ] finish series */

    /* start rung 85 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0025 = 0;
    I_parThis_0025 = I_rung_top;
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_0025 = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 3))) {  // XCmdBracoDescer
        I_parThis_0025 = 0;
    }

    /* ] finish series */
    if (I_parThis_0025) {  // $parThis_0025
        I_parOut_0025 = 1;
    }
    I_parThis_0025 = I_rung_top;
    if (!U_RCmdAutoBraco) {  // RCmdAutoBraco
        I_parThis_0025 = 0;
    }

    if (I_parThis_0025) {  // $parThis_0025
        I_parOut_0025 = 1;
    }
    I_parThis_0025 = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT9) {  // XBracoLevantado
        I_parThis_0025 = 0;
    }

    if (U_RMaqRefOK) {  // RMaqRefOK
        I_parThis_0025 = 0;
    }

    if (!U_RCmdAutoBracoRef) {  // RCmdAutoBracoRef
        I_parThis_0025 = 0;
    }

    /* ] finish series */
    if (I_parThis_0025) {  // $parThis_0025
        I_parOut_0025 = 1;
    }
    I_rung_top = I_parOut_0025;
    /* ] finish parallel */
    if (!GPIO_INPUT_PORT11) {  // XPlacaRecuada
        I_rung_top = 0;
    }

    U_RDesceBraco = I_rung_top;

    /* ] finish series */

    /* start rung 86 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0026 = 0;
    I_parThis_0026 = I_rung_top;
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_0026 = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 4))) {  // XCmdBracoSubir
        I_parThis_0026 = 0;
    }

    /* ] finish series */
    if (I_parThis_0026) {  // $parThis_0026
        I_parOut_0026 = 1;
    }
    I_parThis_0026 = I_rung_top;
    /* start series [ */
    if (!U_RMaqStartAuto) {  // RMaqStartAuto
        I_parThis_0026 = 0;
    }

    /* start parallel [ */
    I_parOut_0027 = 0;
    I_parThis_0027 = I_parThis_0026;
    if (GPIO_INPUT_PORT11) {  // XPlacaRecuada
        I_parThis_0027 = 0;
    }

    if (I_parThis_0027) {  // $parThis_0027
        I_parOut_0027 = 1;
    }
    I_parThis_0027 = I_parThis_0026;
    if (!U_RCmdAutoBracoRef) {  // RCmdAutoBracoRef
        I_parThis_0027 = 0;
    }

    if (I_parThis_0027) {  // $parThis_0027
        I_parOut_0027 = 1;
    }
    I_parThis_0026 = I_parOut_0027;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0026) {  // $parThis_0026
        I_parOut_0026 = 1;
    }
    I_rung_top = I_parOut_0026;
    /* ] finish parallel */
    if (GPIO_INPUT_PORT9) {  // XBracoLevantado
        I_rung_top = 0;
    }

    U_RSobeBraco = I_rung_top;

    /* ] finish series */

    /* start rung 87 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 8))) {  // XCmdPlacaAvancar
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT9) {  // XBracoLevantado
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT10) {  // XPlacaAvancada
        I_rung_top = 0;
    }

    U_RAvancaPlaca = I_rung_top;

    /* ] finish series */

    /* start rung 88 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0028 = 0;
    I_parThis_0028 = I_rung_top;
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_0028 = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 7))) {  // XCmdPlacaRecuar
        I_parThis_0028 = 0;
    }

    /* ] finish series */
    if (I_parThis_0028) {  // $parThis_0028
        I_parOut_0028 = 1;
    }
    I_parThis_0028 = I_rung_top;
    if (!U_RMaqStartAuto) {  // RMaqStartAuto
        I_parThis_0028 = 0;
    }

    if (I_parThis_0028) {  // $parThis_0028
        I_parOut_0028 = 1;
    }
    I_rung_top = I_parOut_0028;
    /* ] finish parallel */
    if (!GPIO_INPUT_PORT9) {  // XBracoLevantado
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT11) {  // XPlacaRecuada
        I_rung_top = 0;
    }

    U_RRecuaPlaca = I_rung_top;

    /* ] finish series */

    /* start rung 89 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 14))) {  // XCmdCarroSubir
        I_rung_top = 0;
    }

    U_RSobeCarro = I_rung_top;

    /* ] finish series */

    /* start rung 90 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 13))) {  // XCmdCarroDescer
        I_rung_top = 0;
    }

    U_RDesceCarro = I_rung_top;

    /* ] finish series */

    /* start rung 91 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 10))) {  // XCmdFrenagSubir
        I_rung_top = 0;
    }

    U_RSobeFrenagem = I_rung_top;

    /* ] finish series */

    /* start rung 92 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0029 = 0;
    I_parThis_0029 = I_rung_top;
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_0029 = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 9))) {  // XCmdFrenagDescer
        I_parThis_0029 = 0;
    }

    /* ] finish series */
    if (I_parThis_0029) {  // $parThis_0029
        I_parOut_0029 = 1;
    }
    I_parThis_0029 = I_rung_top;
    if (!U_RMaqStartAuto) {  // RMaqStartAuto
        I_parThis_0029 = 0;
    }

    if (I_parThis_0029) {  // $parThis_0029
        I_parOut_0029 = 1;
    }
    I_parThis_0029 = I_rung_top;
    if (!U_RCmdAutoFrenagem) {  // RCmdAutoFrenagem
        I_parThis_0029 = 0;
    }

    if (I_parThis_0029) {  // $parThis_0029
        I_parOut_0029 = 1;
    }
    I_rung_top = I_parOut_0029;
    /* ] finish parallel */
    if (GPIO_INPUT_PORT13) {  // XPressaoFrenagem
        I_rung_top = 0;
    }

    U_RDesceFrenagem = I_rung_top;

    /* ] finish series */

    /* start rung 94 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT11) {  // XPlacaRecuada
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_002a = 0;
    I_parThis_002a = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_002b = 0;
    I_parThis_002b = I_parThis_002a;
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_002b = 0;
    }

    if (U_RInversorCfgErr) {  // RInversorCfgErr
        I_parThis_002b = 0;
    }

    if (U_RInversorCfg) {  // RInversorCfg
        I_parThis_002b = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 6))) {  // XCmdGirarAntiHor
        I_parThis_002b = 0;
    }

    /* ] finish series */
    if (I_parThis_002b) {  // $parThis_002b
        I_parOut_002b = 1;
    }
    I_parThis_002b = I_parThis_002a;
    /* start series [ */
    if (!U_RMaqAutoOK) {  // RMaqAutoOK
        I_parThis_002b = 0;
    }

    if (!GPIO_INPUT_PORT16) {  // XFimMaterial
        I_parThis_002b = 0;
    }

    /* ] finish series */
    if (I_parThis_002b) {  // $parThis_002b
        I_parOut_002b = 1;
    }
    I_parThis_002a = I_parOut_002b;
    /* ] finish parallel */
    U_RAvancaTambor = I_parThis_002a;

    /* ] finish series */
    if (I_parThis_002a) {  // $parThis_002a
        I_parOut_002a = 1;
    }
    I_parThis_002a = I_rung_top;
    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_parThis_002a = 0;
    }

    if (U_RInversorCfgErr) {  // RInversorCfgErr
        I_parThis_002a = 0;
    }

    if (U_RInversorCfg) {  // RInversorCfg
        I_parThis_002a = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 5))) {  // XCmdGirarHor
        I_parThis_002a = 0;
    }

    U_RRecuaTambor = I_parThis_002a;

    /* ] finish series */
    if (I_parThis_002a) {  // $parThis_002a
        I_parOut_002a = 1;
    }
    I_rung_top = I_parOut_002a;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 95 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_002c = 0;
    I_parThis_002c = I_rung_top;
    if (!U_RAvancaTambor) {  // RAvancaTambor
        I_parThis_002c = 0;
    }

    if (I_parThis_002c) {  // $parThis_002c
        I_parOut_002c = 1;
    }
    I_parThis_002c = I_rung_top;
    if (!U_RRecuaTambor) {  // RRecuaTambor
        I_parThis_002c = 0;
    }

    if (I_parThis_002c) {  // $parThis_002c
        I_parOut_002c = 1;
    }
    I_rung_top = I_parOut_002c;
    /* ] finish parallel */
    if (!I_TEspMotorParar_antiglitch) {  // $TEspMotorParar_antiglitch
        U_TEspMotorParar = 99;
    }
    I_TEspMotorParar_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEspMotorParar < 99) {
            U_TEspMotorParar++;
            I_rung_top = 1;
        }
    } else {
        U_TEspMotorParar = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT13 = I_rung_top;

    /* ] finish series */

    /* start rung 96 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT13) {  // YFreioMotor
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspFreioMotorOK < 49) {
            U_TEspFreioMotorOK++;
            I_rung_top = 0;
        }
    } else {
        U_TEspFreioMotorOK = 0;
    }

    U_RFreioMotorOK = I_rung_top;

    /* ] finish series */

    /* start rung 97 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RFreioMotorOK) {  // RFreioMotorOK
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_002d = 0;
    I_parThis_002d = I_rung_top;
    /* start series [ */
    if (!U_RAvancaTambor) {  // RAvancaTambor
        I_parThis_002d = 0;
    }

    GPIO_OUTPUT_PORT15 = I_parThis_002d;

    /* ] finish series */
    if (I_parThis_002d) {  // $parThis_002d
        I_parOut_002d = 1;
    }
    I_parThis_002d = I_rung_top;
    /* start series [ */
    if (!U_RRecuaTambor) {  // RRecuaTambor
        I_parThis_002d = 0;
    }

    GPIO_OUTPUT_PORT16 = I_parThis_002d;

    /* ] finish series */
    if (I_parThis_002d) {  // $parThis_002d
        I_parOut_002d = 1;
    }
    I_rung_top = I_parOut_002d;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 99 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_rung_top = 0;
    }

    I_scratch = I_rung_top;
    if (I_oneShot_0013) {  // $oneShot_0013
        I_rung_top = 0;
    }
    I_oneShot_0013 = I_scratch;

    if (!GPIO_INPUT_PORT16) {  // XFimMaterial
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_002e = 0;
    I_parThis_002e = I_rung_top;
    if (I_parThis_002e) {  // $parThis_002e
        U_RMaqStartAuto = 1;
    }

    if (I_parThis_002e) {  // $parThis_002e
        I_parOut_002e = 1;
    }
    I_parThis_002e = I_rung_top;
    if (I_parThis_002e) {  // $parThis_002e
        U_VelOffset = 0;
    }

    if (I_parThis_002e) {  // $parThis_002e
        I_parOut_002e = 1;
    }
    I_rung_top = I_parOut_002e;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 100 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_INPUT_PORT13) {  // XPressaoFrenagem
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT8) {  // XPressaoTambor
        I_rung_top = 0;
    }

    if (!I_TEspPressaoEstab_antiglitch) {  // $TEspPressaoEstab_antiglitch
        U_TEspPressaoEstab = 199;
    }
    I_TEspPressaoEstab_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEspPressaoEstab < 199) {
            U_TEspPressaoEstab++;
            I_rung_top = 1;
        }
    } else {
        U_TEspPressaoEstab = 0;
    }

    if (!U_RMaqOK) {  // RMaqOK
        I_rung_top = 0;
    }

    if (!(MODBUS_REGISTER[0] & (1 << 0))) {  // XMaqModoAuto
        I_rung_top = 0;
    }

    if (U_RMaqStartAuto) {  // RMaqStartAuto
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT11) {  // XPlacaRecuada
        I_rung_top = 0;
    }

    U_RMaqAutoOK = I_rung_top;

    /* ] finish series */

    /* start rung 102 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqStartAuto) {  // RMaqStartAuto
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT11) {  // XPlacaRecuada
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT13) {  // XPressaoFrenagem
        I_rung_top = 0;
    }

    if (!GPIO_INPUT_PORT8) {  // XPressaoTambor
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_002f = 0;
    I_parThis_002f = I_rung_top;
    /* start series [ */
    if (!U_RMaqRefOK) {  // RMaqRefOK
        I_parThis_002f = 0;
    }

    /* start parallel [ */
    I_parOut_0030 = 0;
    I_parThis_0030 = I_parThis_002f;
    /* start series [ */
    I_scratch2 = 500;
    if (I_scratch2 > U_DifPosAbs) {
    } else {
        I_parThis_0030 = 0;
    }

    if (I_parThis_0030) {  // $parThis_0030
        if (U_TEspBracoEstab < 299) {
            U_TEspBracoEstab++;
            I_parThis_0030 = 0;
        }
    } else {
        U_TEspBracoEstab = 0;
    }

    if (I_parThis_0030) {  // $parThis_0030
        U_RMaqStartAuto = 0;
    }

    /* ] finish series */
    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_0030 = I_parThis_002f;
    U_RCmdAutoBraco = I_parThis_0030;

    if (I_parThis_0030) {  // $parThis_0030
        I_parOut_0030 = 1;
    }
    I_parThis_002f = I_parOut_0030;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_002f) {  // $parThis_002f
        I_parOut_002f = 1;
    }
    I_parThis_002f = I_rung_top;
    /* start series [ */
    if (U_RMaqRefOK) {  // RMaqRefOK
        I_parThis_002f = 0;
    }

    U_RCmdAutoBracoRef = I_parThis_002f;

    /* ] finish series */
    if (I_parThis_002f) {  // $parThis_002f
        I_parOut_002f = 1;
    }
    I_rung_top = I_parOut_002f;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 104 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqAutoOK) {  // RMaqAutoOK
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT13) {  // XPressaoFrenagem
        I_rung_top = 0;
    }

    U_RCmdAutoFrenagem = I_rung_top;

    /* ] finish series */

    /* start rung 105 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqAutoOK) {  // RMaqAutoOK
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT8) {  // XPressaoTambor
        I_rung_top = 0;
    }

    U_RCmdAutoTambor = I_rung_top;

    /* ] finish series */

    /* start rung 107 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (U_RUpdateInv) {  // RUpdateInv
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_TEspUpdateInv < 49) {
            U_TEspUpdateInv++;
            I_rung_top = 0;
        }
    } else {
        U_TEspUpdateInv = 0;
    }

    U_RUpdateInv = I_rung_top;

    /* ] finish series */

    /* start rung 108 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqAutoOK) {  // RMaqAutoOK
        I_rung_top = 0;
    }

    if (!U_RUpdateInv) {  // RUpdateInv
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0031 = 0;
    I_parThis_0031 = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT14) {  // XLoopSuperior
        I_parThis_0031 = 0;
    }

    if (U_RInversorCfg) {  // RInversorCfg
        I_parThis_0031 = 0;
    }

    I_scratch2 = -6000;
    if (U_VelOffset > I_scratch2) {
    } else {
        I_parThis_0031 = 0;
    }

    /* start parallel [ */
    I_parOut_0032 = 0;
    I_parThis_0032 = I_parThis_0031;
    if (I_parThis_0032) {  // $parThis_0032
        U_VelOffset = U_VelOffset - U_Desaceleracao;
    }

    if (I_parThis_0032) {  // $parThis_0032
        I_parOut_0032 = 1;
    }
    I_parThis_0032 = I_parThis_0031;
    if (I_parThis_0032) {  // $parThis_0032
        U_RInversorCfg = 1;
    }

    if (I_parThis_0032) {  // $parThis_0032
        I_parOut_0032 = 1;
    }
    I_parThis_0031 = I_parOut_0032;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0031) {  // $parThis_0031
        I_parOut_0031 = 1;
    }
    I_parThis_0031 = I_rung_top;
    /* start series [ */
    if (!GPIO_INPUT_PORT15) {  // XLoopInferior
        I_parThis_0031 = 0;
    }

    if (U_RInversorCfg) {  // RInversorCfg
        I_parThis_0031 = 0;
    }

    I_scratch2 = 6000;
    if (I_scratch2 > U_VelOffset) {
    } else {
        I_parThis_0031 = 0;
    }

    /* start parallel [ */
    I_parOut_0033 = 0;
    I_parThis_0033 = I_parThis_0031;
    if (I_parThis_0033) {  // $parThis_0033
        U_VelOffset = U_VelOffset + U_Aceleracao;
    }

    if (I_parThis_0033) {  // $parThis_0033
        I_parOut_0033 = 1;
    }
    I_parThis_0033 = I_parThis_0031;
    if (I_parThis_0033) {  // $parThis_0033
        U_RInversorCfg = 1;
    }

    if (I_parThis_0033) {  // $parThis_0033
        I_parOut_0033 = 1;
    }
    I_parThis_0031 = I_parOut_0033;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0031) {  // $parThis_0031
        I_parOut_0031 = 1;
    }
    I_rung_top = I_parOut_0031;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 110 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RMaqManualOK) {  // RMaqManualOK
        I_rung_top = 0;
    }

    MODBUS_REGISTER[1] &= ~(1 << 1); MODBUS_REGISTER[1] |= I_rung_top << 1;  // YMbMaqManualOK

    /* ] finish series */

    /* start rung 112 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0034 = 0;
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 0))) {  // YLampEmergencia
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_0035 = 0;
    I_parThis_0035 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0035;
    if (I_oneShot_0014) {  // $oneShot_0014
        I_parThis_0035 = 0;
    }
    I_oneShot_0014 = I_scratch;

    if (I_parThis_0035) {  // $parThis_0035
        U_ValSaidas |= 1 << 0;
    }

    /* ] finish series */
    if (I_parThis_0035) {  // $parThis_0035
        I_parOut_0035 = 1;
    }
    I_parThis_0035 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0035;
    if (!I_parThis_0035) {  // $parThis_0035
        if (I_oneShot_0015) {  // $oneShot_0015
            I_parThis_0035 = 1;
        }
    } else {
        I_parThis_0035 = 0;
    }
    I_oneShot_0015 = I_scratch;

    if (I_parThis_0035) {  // $parThis_0035
        U_ValSaidas &= ~(1 << 0);
    }

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
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 1))) {  // YMbMaqManualOK
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_0036 = 0;
    I_parThis_0036 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0036;
    if (I_oneShot_0016) {  // $oneShot_0016
        I_parThis_0036 = 0;
    }
    I_oneShot_0016 = I_scratch;

    if (I_parThis_0036) {  // $parThis_0036
        U_ValSaidas |= 1 << 1;
    }

    /* ] finish series */
    if (I_parThis_0036) {  // $parThis_0036
        I_parOut_0036 = 1;
    }
    I_parThis_0036 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0036;
    if (!I_parThis_0036) {  // $parThis_0036
        if (I_oneShot_0017) {  // $oneShot_0017
            I_parThis_0036 = 1;
        }
    } else {
        I_parThis_0036 = 0;
    }
    I_oneShot_0017 = I_scratch;

    if (I_parThis_0036) {  // $parThis_0036
        U_ValSaidas &= ~(1 << 1);
    }

    /* ] finish series */
    if (I_parThis_0036) {  // $parThis_0036
        I_parOut_0036 = 1;
    }
    I_parThis_0034 = I_parOut_0036;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 2))) {  // YLampCarroAvanca
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_0037 = 0;
    I_parThis_0037 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0037;
    if (I_oneShot_0018) {  // $oneShot_0018
        I_parThis_0037 = 0;
    }
    I_oneShot_0018 = I_scratch;

    if (I_parThis_0037) {  // $parThis_0037
        U_ValSaidas |= 1 << 2;
    }

    /* ] finish series */
    if (I_parThis_0037) {  // $parThis_0037
        I_parOut_0037 = 1;
    }
    I_parThis_0037 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0037;
    if (!I_parThis_0037) {  // $parThis_0037
        if (I_oneShot_0019) {  // $oneShot_0019
            I_parThis_0037 = 1;
        }
    } else {
        I_parThis_0037 = 0;
    }
    I_oneShot_0019 = I_scratch;

    if (I_parThis_0037) {  // $parThis_0037
        U_ValSaidas &= ~(1 << 2);
    }

    /* ] finish series */
    if (I_parThis_0037) {  // $parThis_0037
        I_parOut_0037 = 1;
    }
    I_parThis_0034 = I_parOut_0037;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 3))) {  // YLampFiltroSujo
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_0038 = 0;
    I_parThis_0038 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0038;
    if (I_oneShot_001a) {  // $oneShot_001a
        I_parThis_0038 = 0;
    }
    I_oneShot_001a = I_scratch;

    if (I_parThis_0038) {  // $parThis_0038
        U_ValSaidas |= 1 << 3;
    }

    /* ] finish series */
    if (I_parThis_0038) {  // $parThis_0038
        I_parOut_0038 = 1;
    }
    I_parThis_0038 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0038;
    if (!I_parThis_0038) {  // $parThis_0038
        if (I_oneShot_001b) {  // $oneShot_001b
            I_parThis_0038 = 1;
        }
    } else {
        I_parThis_0038 = 0;
    }
    I_oneShot_001b = I_scratch;

    if (I_parThis_0038) {  // $parThis_0038
        U_ValSaidas &= ~(1 << 3);
    }

    /* ] finish series */
    if (I_parThis_0038) {  // $parThis_0038
        I_parOut_0038 = 1;
    }
    I_parThis_0034 = I_parOut_0038;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 4))) {  // YLampFreioAlto
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_0039 = 0;
    I_parThis_0039 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0039;
    if (I_oneShot_001c) {  // $oneShot_001c
        I_parThis_0039 = 0;
    }
    I_oneShot_001c = I_scratch;

    if (I_parThis_0039) {  // $parThis_0039
        U_ValSaidas |= 1 << 4;
    }

    /* ] finish series */
    if (I_parThis_0039) {  // $parThis_0039
        I_parOut_0039 = 1;
    }
    I_parThis_0039 = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_0039;
    if (!I_parThis_0039) {  // $parThis_0039
        if (I_oneShot_001d) {  // $oneShot_001d
            I_parThis_0039 = 1;
        }
    } else {
        I_parThis_0039 = 0;
    }
    I_oneShot_001d = I_scratch;

    if (I_parThis_0039) {  // $parThis_0039
        U_ValSaidas &= ~(1 << 4);
    }

    /* ] finish series */
    if (I_parThis_0039) {  // $parThis_0039
        I_parOut_0039 = 1;
    }
    I_parThis_0034 = I_parOut_0039;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 5))) {  // YLampMandrilRet
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_003a = 0;
    I_parThis_003a = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_003a;
    if (I_oneShot_001e) {  // $oneShot_001e
        I_parThis_003a = 0;
    }
    I_oneShot_001e = I_scratch;

    if (I_parThis_003a) {  // $parThis_003a
        U_ValSaidas |= 1 << 5;
    }

    /* ] finish series */
    if (I_parThis_003a) {  // $parThis_003a
        I_parOut_003a = 1;
    }
    I_parThis_003a = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_003a;
    if (!I_parThis_003a) {  // $parThis_003a
        if (I_oneShot_001f) {  // $oneShot_001f
            I_parThis_003a = 1;
        }
    } else {
        I_parThis_003a = 0;
    }
    I_oneShot_001f = I_scratch;

    if (I_parThis_003a) {  // $parThis_003a
        U_ValSaidas &= ~(1 << 5);
    }

    /* ] finish series */
    if (I_parThis_003a) {  // $parThis_003a
        I_parOut_003a = 1;
    }
    I_parThis_0034 = I_parOut_003a;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 6))) {  // YLampAutomatico
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_003b = 0;
    I_parThis_003b = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_003b;
    if (I_oneShot_0020) {  // $oneShot_0020
        I_parThis_003b = 0;
    }
    I_oneShot_0020 = I_scratch;

    if (I_parThis_003b) {  // $parThis_003b
        U_ValSaidas |= 1 << 6;
    }

    /* ] finish series */
    if (I_parThis_003b) {  // $parThis_003b
        I_parOut_003b = 1;
    }
    I_parThis_003b = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_003b;
    if (!I_parThis_003b) {  // $parThis_003b
        if (I_oneShot_0021) {  // $oneShot_0021
            I_parThis_003b = 1;
        }
    } else {
        I_parThis_003b = 0;
    }
    I_oneShot_0021 = I_scratch;

    if (I_parThis_003b) {  // $parThis_003b
        U_ValSaidas &= ~(1 << 6);
    }

    /* ] finish series */
    if (I_parThis_003b) {  // $parThis_003b
        I_parOut_003b = 1;
    }
    I_parThis_0034 = I_parOut_003b;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 7))) {  // YLampManual
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_003c = 0;
    I_parThis_003c = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_003c;
    if (I_oneShot_0022) {  // $oneShot_0022
        I_parThis_003c = 0;
    }
    I_oneShot_0022 = I_scratch;

    if (I_parThis_003c) {  // $parThis_003c
        U_ValSaidas |= 1 << 7;
    }

    /* ] finish series */
    if (I_parThis_003c) {  // $parThis_003c
        I_parOut_003c = 1;
    }
    I_parThis_003c = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_003c;
    if (!I_parThis_003c) {  // $parThis_003c
        if (I_oneShot_0023) {  // $oneShot_0023
            I_parThis_003c = 1;
        }
    } else {
        I_parThis_003c = 0;
    }
    I_oneShot_0023 = I_scratch;

    if (I_parThis_003c) {  // $parThis_003c
        U_ValSaidas &= ~(1 << 7);
    }

    /* ] finish series */
    if (I_parThis_003c) {  // $parThis_003c
        I_parOut_003c = 1;
    }
    I_parThis_0034 = I_parOut_003c;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_parThis_0034 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 10))) {  // YMaqDesligaAuto
        I_parThis_0034 = 0;
    }

    /* start parallel [ */
    I_parOut_003d = 0;
    I_parThis_003d = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_003d;
    if (I_oneShot_0024) {  // $oneShot_0024
        I_parThis_003d = 0;
    }
    I_oneShot_0024 = I_scratch;

    if (I_parThis_003d) {  // $parThis_003d
        U_ValSaidas |= 1 << 10;
    }

    /* ] finish series */
    if (I_parThis_003d) {  // $parThis_003d
        I_parOut_003d = 1;
    }
    I_parThis_003d = I_parThis_0034;
    /* start series [ */
    I_scratch = I_parThis_003d;
    if (!I_parThis_003d) {  // $parThis_003d
        if (I_oneShot_0025) {  // $oneShot_0025
            I_parThis_003d = 1;
        }
    } else {
        I_parThis_003d = 0;
    }
    I_oneShot_0025 = I_scratch;

    if (I_parThis_003d) {  // $parThis_003d
        U_ValSaidas &= ~(1 << 10);
    }

    /* ] finish series */
    if (I_parThis_003d) {  // $parThis_003d
        I_parOut_003d = 1;
    }
    I_parThis_0034 = I_parOut_003d;
    /* ] finish parallel */
    /* ] finish series */
    if (I_parThis_0034) {  // $parThis_0034
        I_parOut_0034 = 1;
    }
    I_rung_top = I_parOut_0034;
    /* ] finish parallel */
    U_RFlagDummy = I_rung_top;

    /* ] finish series */
}
