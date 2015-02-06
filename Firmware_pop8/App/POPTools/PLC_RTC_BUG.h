// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char I_parOut_0000 = 0;
volatile unsigned char I_parThis_0000 = 0;
volatile unsigned char I_parOut_0001 = 0;
volatile unsigned char I_parThis_0001 = 0;
volatile unsigned char GPIO_OUTPUT_PORT6 = 0;
volatile int U_TSTART3 = 0;
volatile unsigned char U_REMERGENCIA = 0;
volatile unsigned char GPIO_OUTPUT_PORT8 = 0;
volatile unsigned char I_parOut_0002 = 0;
volatile unsigned char I_parThis_0002 = 0;
volatile unsigned char I_parOut_0003 = 0;
volatile unsigned char I_parThis_0003 = 0;
volatile unsigned char GPIO_OUTPUT_PORT5 = 0;
volatile int U_TRST_START1 = 0;
volatile unsigned char I_parOut_0004 = 0;
volatile unsigned char I_parThis_0004 = 0;
volatile unsigned char GPIO_OUTPUT_PORT7 = 0;
volatile int U_TRST_START2 = 0;
volatile unsigned char I_parOut_0005 = 0;
volatile unsigned char I_parThis_0005 = 0;
volatile unsigned char GPIO_OUTPUT_PORT9 = 0;
volatile int U_TRST_START3 = 0;
volatile unsigned char GPIO_INPUT_PORT1 = 0;
volatile unsigned char GPIO_INPUT_PORT15 = 0;
volatile unsigned char I_parOut_0006 = 0;
volatile unsigned char I_parThis_0006 = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile int U_TTMR_BOMBA_LIGA = 0;
volatile unsigned char GPIO_INPUT_PORT10 = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;
volatile unsigned char GPIO_INPUT_PORT4 = 0;
volatile unsigned char GPIO_INPUT_PORT5 = 0;
volatile unsigned char GPIO_INPUT_PORT6 = 0;
volatile unsigned char I_parOut_0007 = 0;
volatile unsigned char I_parThis_0007 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile int U_TTIMEOUT_RES1 = 0;
volatile unsigned char GPIO_INPUT_PORT11 = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile int U_TTIMEOUT_RES2 = 0;
volatile unsigned char GPIO_INPUT_PORT12 = 0;
volatile int U_TBOMBA_LIGADA = 0;
volatile unsigned char I_parOut_0008 = 0;
volatile unsigned char I_parThis_0008 = 0;
volatile unsigned char U_RAtivarSonoro = 0;
volatile unsigned char GPIO_OUTPUT_PORT15 = 0;
volatile unsigned char I_parOut_0009 = 0;
volatile unsigned char I_parThis_0009 = 0;
volatile unsigned char I_parOut_000a = 0;
volatile unsigned char I_parThis_000a = 0;
volatile unsigned char I_scratch = 0;
volatile unsigned char I_oneShot_0000 = 0;
volatile unsigned char U_RPausaSonoro = 0;
volatile int U_TDesligaPausa = 0;
volatile unsigned char GPIO_OUTPUT_PORT14 = 0;
volatile int U_TEspSonoroON = 0;
volatile unsigned char I_TEspSonoroOFF_antiglitch = 0;
volatile int U_TEspSonoroOFF = 0;
volatile int U_TLigaPausa = 0;
volatile unsigned char I_parOut_000b = 0;
volatile unsigned char I_parThis_000b = 0;
volatile unsigned char I_oneShot_0001 = 0;
volatile unsigned char I_TEspSirene_antiglitch = 0;
volatile int U_TEspSirene = 0;
volatile unsigned char GPIO_OUTPUT_PORT16 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0000 = 0;
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // XMODO_BANHO
        I_parThis_0000 = 0;
    }

    /* start parallel [ */
    I_parOut_0001 = 0;
    I_parThis_0001 = I_parThis_0000;
    if (!GPIO_OUTPUT_PORT6) {  // YSTART2
        I_parThis_0001 = 0;
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0001 = I_parThis_0000;
    if (MODBUS_REGISTER[2] & (1 << 2)) {  // XSTART2_ATIVO
        I_parThis_0001 = 0;
    }

    if (I_parThis_0001) {  // $parThis_0001
        I_parOut_0001 = 1;
    }
    I_parThis_0000 = I_parOut_0001;
    /* ] finish parallel */
    if (I_parThis_0000) {  // $parThis_0000
        if (U_TSTART3 < 1999) {
            U_TSTART3++;
            I_parThis_0000 = 0;
        }
    } else {
        U_TSTART3 = 0;
    }

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_parThis_0000 = I_rung_top;
    /* start series [ */
    if (MODBUS_REGISTER[2] & (1 << 1)) {  // XMODO_BANHO
        I_parThis_0000 = 0;
    }

    if (!(MODBUS_REGISTER[1] & (1 << 4))) {  // XLIGA_START3
        I_parThis_0000 = 0;
    }

    /* ] finish series */
    if (I_parThis_0000) {  // $parThis_0000
        I_parOut_0000 = 1;
    }
    I_rung_top = I_parOut_0000;
    /* ] finish parallel */
    if (!U_REMERGENCIA) {  // REMERGENCIA
        I_rung_top = 0;
    }

    GPIO_OUTPUT_PORT8 = I_rung_top;

    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (MODBUS_REGISTER[2] & (1 << 1)) {  // XMODO_BANHO
        I_rung_top = 0;
    }

    if (U_REMERGENCIA) {  // REMERGENCIA
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0002 = 0;
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {  // $parThis_0002
        MODBUS_REGISTER[1] &= ~(1 << 2);  // YRST_LIGA_START1
    }

    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {  // $parThis_0002
        MODBUS_REGISTER[1] &= ~(1 << 3);  // YRST_LIGA_START2
    }

    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_parThis_0002 = I_rung_top;
    if (I_parThis_0002) {  // $parThis_0002
        MODBUS_REGISTER[1] &= ~(1 << 4);  // YRST_LIGA_START3
    }

    if (I_parThis_0002) {  // $parThis_0002
        I_parOut_0002 = 1;
    }
    I_rung_top = I_parOut_0002;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 5))) {  // XRST_ALARME_ST1
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0003 = 0;
    I_parThis_0003 = I_rung_top;
    GPIO_OUTPUT_PORT5 = I_parThis_0003;

    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_parThis_0003 = I_rung_top;
    /* start series [ */
    if (I_parThis_0003) {  // $parThis_0003
        if (U_TRST_START1 < 99) {
            U_TRST_START1++;
            I_parThis_0003 = 0;
        }
    } else {
        U_TRST_START1 = 0;
    }

    if (I_parThis_0003) {  // $parThis_0003
        MODBUS_REGISTER[1] &= ~(1 << 5);  // YCLR_START1
    }

    /* ] finish series */
    if (I_parThis_0003) {  // $parThis_0003
        I_parOut_0003 = 1;
    }
    I_rung_top = I_parOut_0003;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 6))) {  // XRST_ALARME_ST2
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0004 = 0;
    I_parThis_0004 = I_rung_top;
    GPIO_OUTPUT_PORT7 = I_parThis_0004;

    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_parThis_0004 = I_rung_top;
    /* start series [ */
    if (I_parThis_0004) {  // $parThis_0004
        if (U_TRST_START2 < 99) {
            U_TRST_START2++;
            I_parThis_0004 = 0;
        }
    } else {
        U_TRST_START2 = 0;
    }

    if (I_parThis_0004) {  // $parThis_0004
        MODBUS_REGISTER[1] &= ~(1 << 6);  // YCLR_START2
    }

    /* ] finish series */
    if (I_parThis_0004) {  // $parThis_0004
        I_parOut_0004 = 1;
    }
    I_rung_top = I_parOut_0004;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 5 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!(MODBUS_REGISTER[1] & (1 << 7))) {  // XRST_ALARME_ST3
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0005 = 0;
    I_parThis_0005 = I_rung_top;
    GPIO_OUTPUT_PORT9 = I_parThis_0005;

    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_parThis_0005 = I_rung_top;
    /* start series [ */
    if (I_parThis_0005) {  // $parThis_0005
        if (U_TRST_START3 < 99) {
            U_TRST_START3++;
            I_parThis_0005 = 0;
        }
    } else {
        U_TRST_START3 = 0;
    }

    if (I_parThis_0005) {  // $parThis_0005
        MODBUS_REGISTER[1] &= ~(1 << 7);  // YCLR_START3
    }

    /* ] finish series */
    if (I_parThis_0005) {  // $parThis_0005
        I_parOut_0005 = 1;
    }
    I_rung_top = I_parOut_0005;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 6 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT1) {  // XEMERGENCIA
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 0); MODBUS_REGISTER[0] |= I_rung_top << 0;  // YST_EMERGENCIA

    /* ] finish series */

    /* start rung 7 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_INPUT_PORT15) {  // XSUPERVISOR_FASE
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 1); MODBUS_REGISTER[0] |= I_rung_top << 1;  // YST_FASE

    /* ] finish series */

    /* start rung 8 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0006 = 0;
    I_parThis_0006 = I_rung_top;
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // XMODO_AQUECIMENT
        I_parThis_0006 = 0;
    }

    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_parThis_0006 = I_rung_top;
    /* start series [ */
    if (MODBUS_REGISTER[2] & (1 << 0)) {  // XMODO_AQUECIMENT
        I_parThis_0006 = 0;
    }

    if (!GPIO_OUTPUT_PORT1) {  // YLIGA_BOMBA
        I_parThis_0006 = 0;
    }

    /* ] finish series */
    if (I_parThis_0006) {  // $parThis_0006
        I_parOut_0006 = 1;
    }
    I_rung_top = I_parOut_0006;
    /* ] finish parallel */
    if (I_rung_top) {  // $rung_top
        if (U_TTMR_BOMBA_LIGA < 499) {
            U_TTMR_BOMBA_LIGA++;
            I_rung_top = 0;
        }
    } else {
        U_TTMR_BOMBA_LIGA = 0;
    }

    if (GPIO_INPUT_PORT10) {  // XBOMBA_LIGADA
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 2); MODBUS_REGISTER[0] |= I_rung_top << 2;  // YST_ERRO_BOMBA

    /* ] finish series */

    /* start rung 9 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT4) {  // YSTART1
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT4) {  // XALARME_START1
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 3); MODBUS_REGISTER[0] |= I_rung_top << 3;  // YST_ALARME1

    /* ] finish series */

    /* start rung 10 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT6) {  // YSTART2
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT5) {  // XALARME_START2
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 4); MODBUS_REGISTER[0] |= I_rung_top << 4;  // YST_ALARME2

    /* ] finish series */

    /* start rung 11 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT8) {  // YSTART3
        I_rung_top = 0;
    }

    if (GPIO_INPUT_PORT6) {  // XALARME_START3
        I_rung_top = 0;
    }

    MODBUS_REGISTER[0] &= ~(1 << 5); MODBUS_REGISTER[0] |= I_rung_top << 5;  // YST_ALARME3

    /* ] finish series */

    /* start rung 12 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_0007 = 0;
    I_parThis_0007 = I_rung_top;
    if (U_REMERGENCIA) {  // REMERGENCIA
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // XMODO_AQUECIMENT
        I_parThis_0007 = 0;
    }

    if (!GPIO_OUTPUT_PORT2) {  // YRESISTENCIA1
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        if (U_TTIMEOUT_RES1 < 499) {
            U_TTIMEOUT_RES1++;
            I_parThis_0007 = 0;
        }
    } else {
        U_TTIMEOUT_RES1 = 0;
    }

    if (GPIO_INPUT_PORT11) {  // XRESISTENCIA1
        I_parThis_0007 = 0;
    }

    /* ] finish series */
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // XMODO_AQUECIMENT
        I_parThis_0007 = 0;
    }

    if (!GPIO_OUTPUT_PORT3) {  // YRESISTENCIA2
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        if (U_TTIMEOUT_RES2 < 499) {
            U_TTIMEOUT_RES2++;
            I_parThis_0007 = 0;
        }
    } else {
        U_TTIMEOUT_RES2 = 0;
    }

    if (GPIO_INPUT_PORT12) {  // XRESISTENCIA2
        I_parThis_0007 = 0;
    }

    /* ] finish series */
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // XMODO_BANHO
        I_parThis_0007 = 0;
    }

    if (!GPIO_OUTPUT_PORT4) {  // YSTART1
        I_parThis_0007 = 0;
    }

    if (GPIO_INPUT_PORT4) {  // XALARME_START1
        I_parThis_0007 = 0;
    }

    /* ] finish series */
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // XMODO_BANHO
        I_parThis_0007 = 0;
    }

    if (!GPIO_OUTPUT_PORT6) {  // YSTART2
        I_parThis_0007 = 0;
    }

    if (GPIO_INPUT_PORT5) {  // XALARME_START2
        I_parThis_0007 = 0;
    }

    if (!(MODBUS_REGISTER[2] & (1 << 2))) {  // XSTART2_ATIVO
        I_parThis_0007 = 0;
    }

    /* ] finish series */
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 1))) {  // XMODO_BANHO
        I_parThis_0007 = 0;
    }

    if (!GPIO_OUTPUT_PORT8) {  // YSTART3
        I_parThis_0007 = 0;
    }

    if (GPIO_INPUT_PORT6) {  // XALARME_START3
        I_parThis_0007 = 0;
    }

    /* ] finish series */
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_parThis_0007 = I_rung_top;
    /* start series [ */
    if (!(MODBUS_REGISTER[2] & (1 << 0))) {  // XMODO_AQUECIMENT
        I_parThis_0007 = 0;
    }

    if (!GPIO_OUTPUT_PORT1) {  // YLIGA_BOMBA
        I_parThis_0007 = 0;
    }

    if (I_parThis_0007) {  // $parThis_0007
        if (U_TBOMBA_LIGADA < 499) {
            U_TBOMBA_LIGADA++;
            I_parThis_0007 = 0;
        }
    } else {
        U_TBOMBA_LIGADA = 0;
    }

    if (GPIO_INPUT_PORT10) {  // XBOMBA_LIGADA
        I_parThis_0007 = 0;
    }

    /* ] finish series */
    if (I_parThis_0007) {  // $parThis_0007
        I_parOut_0007 = 1;
    }
    I_rung_top = I_parOut_0007;
    /* ] finish parallel */
    /* start parallel [ */
    I_parOut_0008 = 0;
    I_parThis_0008 = I_rung_top;
    U_RAtivarSonoro = I_parThis_0008;

    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_parThis_0008 = I_rung_top;
    if (I_parThis_0008) {  // $parThis_0008
        GPIO_OUTPUT_PORT15 = 0;
    } else {
        GPIO_OUTPUT_PORT15 = 1;
    }

    if (I_parThis_0008) {  // $parThis_0008
        I_parOut_0008 = 1;
    }
    I_rung_top = I_parOut_0008;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 13 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!U_RAtivarSonoro) {  // RAtivarSonoro
        I_rung_top = 0;
    }

    /* start parallel [ */
    I_parOut_0009 = 0;
    I_parThis_0009 = I_rung_top;
    /* start series [ */
    /* start parallel [ */
    I_parOut_000a = 0;
    I_parThis_000a = I_parThis_0009;
    I_scratch = I_parThis_000a;
    if (I_oneShot_0000) {  // $oneShot_0000
        I_parThis_000a = 0;
    }
    I_oneShot_0000 = I_scratch;

    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_000a = I_parThis_0009;
    /* start series [ */
    if (!U_RPausaSonoro) {  // RPausaSonoro
        I_parThis_000a = 0;
    }

    if (I_parThis_000a) {  // $parThis_000a
        if (U_TDesligaPausa < 29999) {
            U_TDesligaPausa++;
            I_parThis_000a = 0;
        }
    } else {
        U_TDesligaPausa = 0;
    }

    /* ] finish series */
    if (I_parThis_000a) {  // $parThis_000a
        I_parOut_000a = 1;
    }
    I_parThis_0009 = I_parOut_000a;
    /* ] finish parallel */
    if (I_parThis_0009) {  // $parThis_0009
        U_RPausaSonoro = 0;
    }

    /* ] finish series */
    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_parThis_0009 = I_rung_top;
    /* start series [ */
    if (U_RPausaSonoro) {  // RPausaSonoro
        I_parThis_0009 = 0;
    }

    if (GPIO_OUTPUT_PORT14) {  // YSONORO
        I_parThis_0009 = 0;
    }

    if (I_parThis_0009) {  // $parThis_0009
        if (U_TEspSonoroON < 49) {
            U_TEspSonoroON++;
            I_parThis_0009 = 0;
        }
    } else {
        U_TEspSonoroON = 0;
    }

    if (!I_TEspSonoroOFF_antiglitch) {  // $TEspSonoroOFF_antiglitch
        U_TEspSonoroOFF = 49;
    }
    I_TEspSonoroOFF_antiglitch = 1;
    if (!I_parThis_0009) {  // $parThis_0009
        if (U_TEspSonoroOFF < 49) {
            U_TEspSonoroOFF++;
            I_parThis_0009 = 1;
        }
    } else {
        U_TEspSonoroOFF = 0;
    }

    GPIO_OUTPUT_PORT14 = I_parThis_0009;

    /* ] finish series */
    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_parThis_0009 = I_rung_top;
    /* start series [ */
    if (U_RPausaSonoro) {  // RPausaSonoro
        I_parThis_0009 = 0;
    }

    if (I_parThis_0009) {  // $parThis_0009
        if (U_TLigaPausa < 2999) {
            U_TLigaPausa++;
            I_parThis_0009 = 0;
        }
    } else {
        U_TLigaPausa = 0;
    }

    if (I_parThis_0009) {  // $parThis_0009
        U_RPausaSonoro = 1;
    }

    /* ] finish series */
    if (I_parThis_0009) {  // $parThis_0009
        I_parOut_0009 = 1;
    }
    I_rung_top = I_parOut_0009;
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 15 */
    I_rung_top = I_mcr;

    /* start series [ */
    /* start parallel [ */
    I_parOut_000b = 0;
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 7 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 8 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 9 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 10 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 11 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 12 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 13 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 14 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 15 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 16 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 17 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 18 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 19 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 20 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 21 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_parThis_000b = I_rung_top;
    if (I_parThis_000b) {  // $parThis_000b
        RTC_Now = RTC_GetTime();
        I_parThis_000b = (62 & (1 << RTC_Now.Wday)) && RTC_Now.Hour == 22 && RTC_Now.Min == 0 && RTC_Now.Sec == 0;
    }

    if (I_parThis_000b) {  // $parThis_000b
        I_parOut_000b = 1;
    }
    I_rung_top = I_parOut_000b;
    /* ] finish parallel */
    I_scratch = I_rung_top;
    if (I_oneShot_0001) {  // $oneShot_0001
        I_rung_top = 0;
    }
    I_oneShot_0001 = I_scratch;

    if (!I_TEspSirene_antiglitch) {  // $TEspSirene_antiglitch
        U_TEspSirene = 99;
    }
    I_TEspSirene_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_TEspSirene < 99) {
            U_TEspSirene++;
            I_rung_top = 1;
        }
    } else {
        U_TEspSirene = 0;
    }

    GPIO_OUTPUT_PORT16 = I_rung_top;

    /* ] finish series */
}

