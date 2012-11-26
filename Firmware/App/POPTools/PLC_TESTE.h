// Variaveis PLC
volatile unsigned char I_mcr = 0;
volatile unsigned char I_rung_top = 0;
volatile unsigned char GPIO_OUTPUT_PORT4 = 0;

void PLC_Run(void)
{
    I_mcr = 1;

    /* start rung 1 */
    I_rung_top = I_mcr;
    
    /* iniciando serie [ */
    if (I_rung_top) {  // $rung_top
        RTC_StartTM.tm_year = 2012; RTC_StartTM.tm_mon = 11; RTC_StartTM.tm_mday = 23; RTC_StartTM.tm_hour = 14; RTC_StartTM.tm_min = 38; RTC_StartTM.tm_sec = 30;
        RTC_EndTM.tm_year = 2012; RTC_EndTM.tm_mon = 11; RTC_EndTM.tm_mday = 23; RTC_EndTM.tm_hour = 14; RTC_EndTM.tm_min = 39; RTC_EndTM.tm_sec = 0;

        RTC_StartTM = *AdjustDate(RTC_StartTM, RTC_GETDATE_MODE_START);
        RTC_EndTM   = *AdjustDate(RTC_EndTM  , RTC_GETDATE_MODE_END  );

        I_rung_top = RTC_OutputState(RTC_StartTM, RTC_EndTM, RTC_NowTM, RTC_MODE_DATE_CONTINUOUS, 127);
    }
    
    GPIO_OUTPUT_PORT4 = I_rung_top;
    
    /* ] finish series */
}
