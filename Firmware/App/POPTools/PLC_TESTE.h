volatile unsigned char GPIO_INPUT_PORT1 = 0;
volatile int VAR_NULL = 0;
volatile unsigned char GPIO_OUTPUT_PORT3 = 0;
volatile unsigned char GPIO_OUTPUT_PORT2 = 0;
volatile unsigned char GPIO_OUTPUT_PORT1 = 0;
volatile int ArrayBitUser_Count = 1;
volatile int ArrayBitUser[1];
volatile int ArrayBitSystem_Count = 1;
volatile int ArrayBitSystem[1];
volatile int ArrayIntUser_Count = 4;
volatile int ArrayIntUser[4];

void PLC_Run(void)
{
    ArrayBitSystem[0] |= 1UL << 0; // $mcr = 1

    /* start rung 4 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 2; else ArrayBitSystem[0] &= ~(1UL << 2); // $scratch = $rung_top
    if (((ArrayBitSystem[0] >> 3) & 1)) {  // $oneShot_0000
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 3; else ArrayBitSystem[0] &= ~(1UL << 3); // $oneShot_0000 = $scratch

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[2] = 20; // EsperaVerde
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[1] = 3; // EsperaAmarelo
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        MODBUS_REGISTER[0] = 30; // EsperaVermelho
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitUser[0] |= 1UL << 0; // AmareloAtivo = 1
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayIntUser[0] = MODBUS_REGISTER[1]; // Tempo = EsperaAmarelo
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 6 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (((ArrayBitUser[0] >> 1) & 1)) {  // Ciclo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[1] < 49) {
            ArrayIntUser[1]++;
            ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
        }
    } else {
        ArrayIntUser[1] = 0; // ON
    }

    if (!((ArrayBitSystem[0] >> 6) & 1)) {  // $OFF_antiglitch
        ArrayIntUser[2] = 49; // OFF
    }
    ArrayBitSystem[0] |= 1UL << 6; // $OFF_antiglitch = 1
    if (!((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (ArrayIntUser[2] < 49) {
            ArrayIntUser[2]++;
            ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
        }
    } else {
        ArrayIntUser[2] = 0; // OFF
    }

    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitUser[0] |= 1UL << 1; else ArrayBitUser[0] &= ~(1UL << 1); // Ciclo = $rung_top

    /* ] finish series */

    /* start rung 8 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        RTC_StartTM.tm_year = 0; RTC_StartTM.tm_mon = 0; RTC_StartTM.tm_mday = 0; RTC_StartTM.tm_hour = 23; RTC_StartTM.tm_min = 0; RTC_StartTM.tm_sec = 0;
        RTC_EndTM.tm_year = 0; RTC_EndTM.tm_mon = 0; RTC_EndTM.tm_mday = 0; RTC_EndTM.tm_hour = 23; RTC_EndTM.tm_min = 0; RTC_EndTM.tm_sec = 0;
        if((127 & (1 << RTC_NowTM.tm_wday)) && (RTC_NowTM.tm_hour == RTC_StartTM.tm_hour) && (RTC_NowTM.tm_min  == RTC_StartTM.tm_min ) && (RTC_NowTM.tm_sec  == RTC_StartTM.tm_sec )) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5);
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT1) {  // ForcarPiscante
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 2; else ArrayBitSystem[0] &= ~(1UL << 2); // $scratch = $parThis_0000
    if (((ArrayBitSystem[0] >> 7) & 1)) {  // $oneShot_0001
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 7; else ArrayBitSystem[0] &= ~(1UL << 7); // $oneShot_0001 = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 8); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 2; // ModoPiscante = 1
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 3); // VerdeAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 0); // AmareloAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 4); // VermelhoAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 8) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 10 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        RTC_StartTM.tm_year = 0; RTC_StartTM.tm_mon = 0; RTC_StartTM.tm_mday = 0; RTC_StartTM.tm_hour = 6; RTC_StartTM.tm_min = 0; RTC_StartTM.tm_sec = 0;
        RTC_EndTM.tm_year = 0; RTC_EndTM.tm_mon = 0; RTC_EndTM.tm_mday = 0; RTC_EndTM.tm_hour = 6; RTC_EndTM.tm_min = 0; RTC_EndTM.tm_sec = 0;
        if((127 & (1 << RTC_NowTM.tm_wday)) && (RTC_NowTM.tm_hour == RTC_StartTM.tm_hour) && (RTC_NowTM.tm_min  == RTC_StartTM.tm_min ) && (RTC_NowTM.tm_sec  == RTC_StartTM.tm_sec )) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5);
    }
    
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!GPIO_INPUT_PORT1) {  // ForcarPiscante
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 2; else ArrayBitSystem[0] &= ~(1UL << 2); // $scratch = $parThis_0000
    if (!((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        if (((ArrayBitSystem[0] >> 10) & 1)) {  // $oneShot_0002
            ArrayBitSystem[0] |= 1UL << 5; // $parThis_0000 = 1
        }
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }
    if(((ArrayBitSystem[0] >> 2) & 1)) ArrayBitSystem[0] |= 1UL << 10; else ArrayBitSystem[0] &= ~(1UL << 10); // $oneShot_0002 = $scratch

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 8); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 2); // ModoPiscante = 0
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 0; // AmareloAtivo = 1
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $rung_top
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayIntUser[0] = MODBUS_REGISTER[1]; // Tempo = EsperaAmarelo
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 8) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 14 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 1) & 1)) {  // Ciclo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    if (((ArrayBitSystem[0] >> 1) & 1)) {  // $rung_top
        if (!((ArrayBitSystem[0] >> 11) & 1)) {  // $oneShot_0003
            ArrayBitSystem[0] |= 1UL << 2; // $scratch
            ArrayIntUser[0] = ArrayIntUser[0] - ((ArrayBitSystem[0] >> 2) & 1); // Tempo = Tempo - $scratch
        }
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 11; else ArrayBitSystem[0] &= ~(1UL << 11); // $oneShot_0003 = $rung_top
    if (ArrayIntUser[0] > VAR_NULL) {
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    } else {
        ArrayBitSystem[0] |= 1UL << 1; // $rung_top = 1
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 4) & 1)) {  // VermelhoAtivo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 8); // $parOut_0001 = 0
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitUser[0] |= 1UL << 3; // VerdeAtivo = 1
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitUser[0] &= ~(1UL << 4); // VermelhoAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 9; else ArrayBitSystem[0] &= ~(1UL << 9); // $parThis_0001 = $parThis_0000
    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayIntUser[0] = MODBUS_REGISTER[2]; // Tempo = EsperaVerde
    }

    if (((ArrayBitSystem[0] >> 9) & 1)) {  // $parThis_0001
        ArrayBitSystem[0] |= 1UL << 8; // $parOut_0001 = 1
    }
    if(((ArrayBitSystem[0] >> 8) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $parOut_0001
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // AmareloAtivo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 12); // $parOut_0002 = 0
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 13; else ArrayBitSystem[0] &= ~(1UL << 13); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 13) & 1)) {  // $parThis_0002
        ArrayBitUser[0] |= 1UL << 4; // VermelhoAtivo = 1
    }

    if (((ArrayBitSystem[0] >> 13) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 12; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 13; else ArrayBitSystem[0] &= ~(1UL << 13); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 13) & 1)) {  // $parThis_0002
        ArrayBitUser[0] &= ~(1UL << 0); // AmareloAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 13) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 12; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 13; else ArrayBitSystem[0] &= ~(1UL << 13); // $parThis_0002 = $parThis_0000
    if (((ArrayBitSystem[0] >> 13) & 1)) {  // $parThis_0002
        ArrayIntUser[0] = MODBUS_REGISTER[0]; // Tempo = EsperaVermelho
    }

    if (((ArrayBitSystem[0] >> 13) & 1)) {  // $parThis_0002
        ArrayBitSystem[0] |= 1UL << 12; // $parOut_0002 = 1
    }
    if(((ArrayBitSystem[0] >> 12) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $parOut_0002
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    ArrayIntUser[3] = 0; // $scratch2
    if (ArrayIntUser[0] == ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    if (!((ArrayBitUser[0] >> 3) & 1)) {  // VerdeAtivo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 14); // $parOut_0003 = 0
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 15; else ArrayBitSystem[0] &= ~(1UL << 15); // $parThis_0003 = $parThis_0000
    if (((ArrayBitSystem[0] >> 15) & 1)) {  // $parThis_0003
        ArrayBitUser[0] |= 1UL << 0; // AmareloAtivo = 1
    }

    if (((ArrayBitSystem[0] >> 15) & 1)) {  // $parThis_0003
        ArrayBitSystem[0] |= 1UL << 14; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 15; else ArrayBitSystem[0] &= ~(1UL << 15); // $parThis_0003 = $parThis_0000
    if (((ArrayBitSystem[0] >> 15) & 1)) {  // $parThis_0003
        ArrayBitUser[0] &= ~(1UL << 3); // VerdeAtivo = 0
    }

    if (((ArrayBitSystem[0] >> 15) & 1)) {  // $parThis_0003
        ArrayBitSystem[0] |= 1UL << 14; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 5) & 1)) ArrayBitSystem[0] |= 1UL << 15; else ArrayBitSystem[0] &= ~(1UL << 15); // $parThis_0003 = $parThis_0000
    if (((ArrayBitSystem[0] >> 15) & 1)) {  // $parThis_0003
        ArrayIntUser[0] = MODBUS_REGISTER[1]; // Tempo = EsperaAmarelo
    }

    if (((ArrayBitSystem[0] >> 15) & 1)) {  // $parThis_0003
        ArrayBitSystem[0] |= 1UL << 14; // $parOut_0003 = 1
    }
    if(((ArrayBitSystem[0] >> 14) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $parOut_0003
    /* ] finish parallel */
    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    /* ] finish series */

    /* start rung 16 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 3) & 1)) {  // VerdeAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    ArrayIntUser[3] = 5; // $scratch2
    if (ArrayIntUser[0] > ArrayIntUser[3]) {
    } else {
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 1) & 1)) {  // Ciclo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    GPIO_OUTPUT_PORT3 = ((ArrayBitSystem[0] >> 1) & 1); // Verde = $rung_top

    /* ] finish series */

    /* start rung 18 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    /* start parallel [ */
    ArrayBitSystem[0] &= ~(1UL << 4); // $parOut_0000 = 0
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    if (!((ArrayBitUser[0] >> 0) & 1)) {  // AmareloAtivo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 1) & 1)) ArrayBitSystem[0] |= 1UL << 5; else ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = $rung_top
    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 2) & 1)) {  // ModoPiscante
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    if (!((ArrayBitUser[0] >> 1) & 1)) {  // Ciclo
        ArrayBitSystem[0] &= ~(1UL << 5); // $parThis_0000 = 0
    }

    /* ] finish series */
    if (((ArrayBitSystem[0] >> 5) & 1)) {  // $parThis_0000
        ArrayBitSystem[0] |= 1UL << 4; // $parOut_0000 = 1
    }
    if(((ArrayBitSystem[0] >> 4) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $parOut_0000
    /* ] finish parallel */
    GPIO_OUTPUT_PORT2 = ((ArrayBitSystem[0] >> 1) & 1); // Amarelo = $rung_top

    /* ] finish series */

    /* start rung 20 */
    if(((ArrayBitSystem[0] >> 0) & 1)) ArrayBitSystem[0] |= 1UL << 1; else ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = $mcr

    /* iniciando serie [ */
    if (!((ArrayBitUser[0] >> 4) & 1)) {  // VermelhoAtivo
        ArrayBitSystem[0] &= ~(1UL << 1); // $rung_top = 0
    }

    GPIO_OUTPUT_PORT1 = ((ArrayBitSystem[0] >> 1) & 1); // Vermelho = $rung_top

    /* ] finish series */
}
