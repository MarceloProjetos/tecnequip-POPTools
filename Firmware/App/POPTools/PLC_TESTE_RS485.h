// Variaveis PLC
volatile int I_mcr = 0;
volatile int I_rung_top = 0;
volatile int U_val = 0;
volatile int GPIO_OUTPUT_PORT18 = 0;
volatile int U_Ton = 0;
volatile int I_Toff_antiglitch = 0;
volatile int U_Toff = 0;
volatile int I_oneShot_0000 = 0;
volatile int I_oneShot_0001 = 0;
volatile int I_oneShot_0002 = 0;
volatile int I_oneShot_0003 = 0;
volatile int GPIO_OUTPUT_PORT17 = 0;
volatile int U_Ton1 = 0;
volatile int I_Toff1_antiglitch = 0;
volatile int U_Toff1 = 0;
volatile int GPIO_OUTPUT_PORT1 = 0;
volatile int U_Ton2 = 0;
volatile int I_Toff2_antiglitch = 0;
volatile int U_Toff2 = 0;
volatile int GPIO_OUTPUT_PORT2 = 0;
volatile int U_Ton3 = 0;
volatile int I_Toff3_antiglitch = 0;
volatile int U_Toff3 = 0;
volatile int GPIO_OUTPUT_PORT3 = 0;
volatile int U_Ton4 = 0;
volatile int I_Toff4_antiglitch = 0;
volatile int U_Toff4 = 0;
volatile int GPIO_OUTPUT_PORT4 = 0;
volatile int U_Ton5 = 0;
volatile int I_Toff5_antiglitch = 0;
volatile int U_Toff5 = 0;
volatile int GPIO_OUTPUT_PORT5 = 0;
volatile int U_Ton6 = 0;
volatile int I_Toff6_antiglitch = 0;
volatile int U_Toff6 = 0;
volatile int GPIO_OUTPUT_PORT6 = 0;
volatile int U_Ton7 = 0;
volatile int I_Toff7_antiglitch = 0;
volatile int U_Toff7 = 0;
volatile int GPIO_OUTPUT_PORT7 = 0;
volatile int U_Ton8 = 0;
volatile int I_Toff8_antiglitch = 0;
volatile int U_Toff8 = 0;
volatile int GPIO_OUTPUT_PORT8 = 0;
volatile int U_Ton9 = 0;
volatile int I_Toff9_antiglitch = 0;
volatile int U_Toff9 = 0;
volatile int GPIO_OUTPUT_PORT9 = 0;
volatile int U_Ton10 = 0;
volatile int I_Toff10_antiglitch = 0;
volatile int U_Toff10 = 0;
volatile int GPIO_OUTPUT_PORT10 = 0;
volatile int U_Ton11 = 0;
volatile int I_Toff11_antiglitch = 0;
volatile int U_Toff11 = 0;
volatile int GPIO_OUTPUT_PORT11 = 0;
volatile int U_Ton12 = 0;
volatile int I_Toff12_antiglitch = 0;
volatile int U_Toff12 = 0;
volatile int GPIO_OUTPUT_PORT12 = 0;
volatile int U_Ton13 = 0;
volatile int I_Toff13_antiglitch = 0;
volatile int U_Toff13 = 0;
volatile int GPIO_OUTPUT_PORT13 = 0;
volatile int U_Ton14 = 0;
volatile int I_Toff14_antiglitch = 0;
volatile int U_Toff14 = 0;
volatile int GPIO_OUTPUT_PORT14 = 0;
volatile int U_Ton15 = 0;
volatile int I_Toff15_antiglitch = 0;
volatile int U_Toff15 = 0;
volatile int GPIO_OUTPUT_PORT15 = 0;
volatile int U_Ton16 = 0;
volatile int I_Toff16_antiglitch = 0;
volatile int U_Toff16 = 0;
volatile int GPIO_OUTPUT_PORT16 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (I_rung_top) {  // $rung_top
        U_val = 100;
    }

    /* ] finish series */

    /* start rung 2 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (GPIO_OUTPUT_PORT18) {  // Yled
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton < 499) {
            U_Ton++;
            I_rung_top = 0;
        }
    } else {
        U_Ton = 0;
    }

    if (!I_Toff_antiglitch) {  // $Toff_antiglitch
        U_Toff = 499;
    }
    I_Toff_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff < 499) {
            U_Toff++;
            I_rung_top = 1;
        }
    } else {
        U_Toff = 0;
    }

    GPIO_OUTPUT_PORT18 = I_rung_top;

    /* ] finish series */

    /* start rung 3 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT18) {  // Yled
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0000) {  // $oneShot_0000
            if (I_SerialReady) {  // $SerialReady
                USS_Set_Param(1, 2, 3, 0, &U_val);
                I_oneShot_0000 = I_rung_top;
            }
            I_rung_top = 0;
            I_oneShot_0001 = I_rung_top;
        }
        if (!I_oneShot_0001) {  // $oneShot_0001
            if (I_SerialReady) {  // $SerialReady
                I_oneShot_0001 = 1;
            } else {
                I_rung_top = 0;
            }
        }
    } else {
        I_oneShot_0000 = I_rung_top;
    }

    if (I_rung_top) {  // $rung_top
        if (!I_oneShot_0002) {  // $oneShot_0002
            if (I_SerialReady) {  // $SerialReady
                Modbus_Send(10, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, 20, 1, &U_val);
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

    GPIO_OUTPUT_PORT17 = I_rung_top;

    /* ] finish series */

    /* start rung 4 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT18) {  // Yled
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton1 < 29) {
            U_Ton1++;
            I_rung_top = 0;
        }
    } else {
        U_Ton1 = 0;
    }

    if (!I_Toff1_antiglitch) {  // $Toff1_antiglitch
        U_Toff1 = 29;
    }
    I_Toff1_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff1 < 29) {
            U_Toff1++;
            I_rung_top = 1;
        }
    } else {
        U_Toff1 = 0;
    }

    GPIO_OUTPUT_PORT1 = I_rung_top;

    /* ] finish series */

    /* start rung 5 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT1) {  // Ys1
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton2 < 29) {
            U_Ton2++;
            I_rung_top = 0;
        }
    } else {
        U_Ton2 = 0;
    }

    if (!I_Toff2_antiglitch) {  // $Toff2_antiglitch
        U_Toff2 = 29;
    }
    I_Toff2_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff2 < 29) {
            U_Toff2++;
            I_rung_top = 1;
        }
    } else {
        U_Toff2 = 0;
    }

    GPIO_OUTPUT_PORT2 = I_rung_top;

    /* ] finish series */

    /* start rung 6 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT2) {  // Ys2
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton3 < 29) {
            U_Ton3++;
            I_rung_top = 0;
        }
    } else {
        U_Ton3 = 0;
    }

    if (!I_Toff3_antiglitch) {  // $Toff3_antiglitch
        U_Toff3 = 29;
    }
    I_Toff3_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff3 < 29) {
            U_Toff3++;
            I_rung_top = 1;
        }
    } else {
        U_Toff3 = 0;
    }

    GPIO_OUTPUT_PORT3 = I_rung_top;

    /* ] finish series */

    /* start rung 7 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT3) {  // Ys3
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton4 < 29) {
            U_Ton4++;
            I_rung_top = 0;
        }
    } else {
        U_Ton4 = 0;
    }

    if (!I_Toff4_antiglitch) {  // $Toff4_antiglitch
        U_Toff4 = 29;
    }
    I_Toff4_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff4 < 29) {
            U_Toff4++;
            I_rung_top = 1;
        }
    } else {
        U_Toff4 = 0;
    }

    GPIO_OUTPUT_PORT4 = I_rung_top;

    /* ] finish series */

    /* start rung 8 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT4) {  // Ys4
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton5 < 29) {
            U_Ton5++;
            I_rung_top = 0;
        }
    } else {
        U_Ton5 = 0;
    }

    if (!I_Toff5_antiglitch) {  // $Toff5_antiglitch
        U_Toff5 = 29;
    }
    I_Toff5_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff5 < 29) {
            U_Toff5++;
            I_rung_top = 1;
        }
    } else {
        U_Toff5 = 0;
    }

    GPIO_OUTPUT_PORT5 = I_rung_top;

    /* ] finish series */

    /* start rung 9 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT5) {  // Ys5
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton6 < 29) {
            U_Ton6++;
            I_rung_top = 0;
        }
    } else {
        U_Ton6 = 0;
    }

    if (!I_Toff6_antiglitch) {  // $Toff6_antiglitch
        U_Toff6 = 29;
    }
    I_Toff6_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff6 < 29) {
            U_Toff6++;
            I_rung_top = 1;
        }
    } else {
        U_Toff6 = 0;
    }

    GPIO_OUTPUT_PORT6 = I_rung_top;

    /* ] finish series */

    /* start rung 10 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT6) {  // Ys6
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton7 < 29) {
            U_Ton7++;
            I_rung_top = 0;
        }
    } else {
        U_Ton7 = 0;
    }

    if (!I_Toff7_antiglitch) {  // $Toff7_antiglitch
        U_Toff7 = 29;
    }
    I_Toff7_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff7 < 29) {
            U_Toff7++;
            I_rung_top = 1;
        }
    } else {
        U_Toff7 = 0;
    }

    GPIO_OUTPUT_PORT7 = I_rung_top;

    /* ] finish series */

    /* start rung 11 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT7) {  // Ys7
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton8 < 29) {
            U_Ton8++;
            I_rung_top = 0;
        }
    } else {
        U_Ton8 = 0;
    }

    if (!I_Toff8_antiglitch) {  // $Toff8_antiglitch
        U_Toff8 = 29;
    }
    I_Toff8_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff8 < 29) {
            U_Toff8++;
            I_rung_top = 1;
        }
    } else {
        U_Toff8 = 0;
    }

    GPIO_OUTPUT_PORT8 = I_rung_top;

    /* ] finish series */

    /* start rung 12 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT8) {  // Ys8
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton9 < 29) {
            U_Ton9++;
            I_rung_top = 0;
        }
    } else {
        U_Ton9 = 0;
    }

    if (!I_Toff9_antiglitch) {  // $Toff9_antiglitch
        U_Toff9 = 29;
    }
    I_Toff9_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff9 < 29) {
            U_Toff9++;
            I_rung_top = 1;
        }
    } else {
        U_Toff9 = 0;
    }

    GPIO_OUTPUT_PORT9 = I_rung_top;

    /* ] finish series */

    /* start rung 13 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT9) {  // Ys9
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton10 < 29) {
            U_Ton10++;
            I_rung_top = 0;
        }
    } else {
        U_Ton10 = 0;
    }

    if (!I_Toff10_antiglitch) {  // $Toff10_antiglitch
        U_Toff10 = 29;
    }
    I_Toff10_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff10 < 29) {
            U_Toff10++;
            I_rung_top = 1;
        }
    } else {
        U_Toff10 = 0;
    }

    GPIO_OUTPUT_PORT10 = I_rung_top;

    /* ] finish series */

    /* start rung 14 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT10) {  // Ys10
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton11 < 29) {
            U_Ton11++;
            I_rung_top = 0;
        }
    } else {
        U_Ton11 = 0;
    }

    if (!I_Toff11_antiglitch) {  // $Toff11_antiglitch
        U_Toff11 = 29;
    }
    I_Toff11_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff11 < 29) {
            U_Toff11++;
            I_rung_top = 1;
        }
    } else {
        U_Toff11 = 0;
    }

    GPIO_OUTPUT_PORT11 = I_rung_top;

    /* ] finish series */

    /* start rung 15 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT11) {  // Ys11
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton12 < 29) {
            U_Ton12++;
            I_rung_top = 0;
        }
    } else {
        U_Ton12 = 0;
    }

    if (!I_Toff12_antiglitch) {  // $Toff12_antiglitch
        U_Toff12 = 29;
    }
    I_Toff12_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff12 < 29) {
            U_Toff12++;
            I_rung_top = 1;
        }
    } else {
        U_Toff12 = 0;
    }

    GPIO_OUTPUT_PORT12 = I_rung_top;

    /* ] finish series */

    /* start rung 16 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT12) {  // Ys12
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton13 < 29) {
            U_Ton13++;
            I_rung_top = 0;
        }
    } else {
        U_Ton13 = 0;
    }

    if (!I_Toff13_antiglitch) {  // $Toff13_antiglitch
        U_Toff13 = 29;
    }
    I_Toff13_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff13 < 29) {
            U_Toff13++;
            I_rung_top = 1;
        }
    } else {
        U_Toff13 = 0;
    }

    GPIO_OUTPUT_PORT13 = I_rung_top;

    /* ] finish series */

    /* start rung 17 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT13) {  // Ys13
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton14 < 29) {
            U_Ton14++;
            I_rung_top = 0;
        }
    } else {
        U_Ton14 = 0;
    }

    if (!I_Toff14_antiglitch) {  // $Toff14_antiglitch
        U_Toff14 = 29;
    }
    I_Toff14_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff14 < 29) {
            U_Toff14++;
            I_rung_top = 1;
        }
    } else {
        U_Toff14 = 0;
    }

    GPIO_OUTPUT_PORT14 = I_rung_top;

    /* ] finish series */

    /* start rung 18 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT14) {  // Ys14
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton15 < 29) {
            U_Ton15++;
            I_rung_top = 0;
        }
    } else {
        U_Ton15 = 0;
    }

    if (!I_Toff15_antiglitch) {  // $Toff15_antiglitch
        U_Toff15 = 29;
    }
    I_Toff15_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff15 < 29) {
            U_Toff15++;
            I_rung_top = 1;
        }
    } else {
        U_Toff15 = 0;
    }

    GPIO_OUTPUT_PORT15 = I_rung_top;

    /* ] finish series */

    /* start rung 19 */
    I_rung_top = I_mcr;

    /* start series [ */
    if (!GPIO_OUTPUT_PORT15) {  // Ys15
        I_rung_top = 0;
    }

    if (I_rung_top) {  // $rung_top
        if (U_Ton16 < 29) {
            U_Ton16++;
            I_rung_top = 0;
        }
    } else {
        U_Ton16 = 0;
    }

    if (!I_Toff16_antiglitch) {  // $Toff16_antiglitch
        U_Toff16 = 29;
    }
    I_Toff16_antiglitch = 1;
    if (!I_rung_top) {  // $rung_top
        if (U_Toff16 < 29) {
            U_Toff16++;
            I_rung_top = 1;
        }
    } else {
        U_Toff16 = 0;
    }

    GPIO_OUTPUT_PORT16 = I_rung_top;

    /* ] finish series */
}
