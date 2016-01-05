/**
 * @file	: lpc17xx_qei.c
 * @brief	: Contains all functions support for QEI firmware library on LPC17xx
 * @version	: 1.0
 * @date	: 26. May. 2009
 * @author	: HieuNguyen
 **************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @addtogroup QEI
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "chip.h"
#include "qei.h"

/* Private Types -------------------------------------------------------------- */
/** @defgroup QEI_Private_Types
 * @{
 */

/**
 * @brief QEI configuration union type definition
 */
typedef union {
	QEI_CFG_Type bmQEIConfig;
	uint32_t ulQEIConfig;
} QEI_CFGOPT_Type;

/**
 * @}
 */

QEI_CFG_Type QEIConfig;

#define ENC_RES                         2048UL  /**< Encoder resolution (PPR) */

volatile int ENC_VAL = 0;

/************************** ENCODER *************************/
/** Position Counter */
__IO uint32_t PosCnt;
/** Velocity Accumulator */
__IO uint32_t VeloAcc;
/** Times of Velocity capture */
__IO uint32_t VeloCapCnt;
/** Flag indicates Times of Velocity capture is enough to read out */
__IO FlagStatus VeloAccFlag;

/* Public Functions ----------------------------------------------------------- */
/** @addtogroup QEI_Public_Functions
 * @{
 */

void QEI_IRQHandler(void)
{
  //uint32_t tmp;

  // Check whether if velocity timer overflow
  if (QEI_GetIntStatus(LPC_QEI, QEI_INTFLAG_TIM_Int) == SET)
  {
    if (VeloAccFlag == RESET) {
      // Get current velocity captured and update to accumulate
      //VeloAcc += QEI_GetVelocityCap(QEI);
      //tmp = QEI_GetVelocityCap(QEI);
      //VeloAcc += tmp;
      //_DBH32(tmp); _DBG_("");

      // Update Velocity capture times
      VeloAccFlag = ((VeloCapCnt++) >= MAX_CAP_TIMES) ? SET : RESET;
    }
    // Reset Interrupt flag pending
    QEI_IntClear(LPC_QEI, QEI_INTFLAG_TIM_Int);
  }

  // Check whether if direction change occurred
  if (QEI_GetIntStatus(LPC_QEI, QEI_INTFLAG_DIR_Int) == SET) {
    // Reset Interrupt flag pending
    QEI_IntClear(LPC_QEI, QEI_INTFLAG_DIR_Int);
  }
}

/*********************************************************************//**
 * @brief		Resets value for each type of QEI value, such as velocity,
 * 				counter, position, etc..
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulResetType		QEI Reset Type, should be one of the following:
 * 								- QEI_RESET_POS: Reset Position Counter
 * 								- QEI_RESET_POSOnIDX: Reset Position Counter on Index signal
 * 								- QEI_RESET_VEL: Reset Velocity
 * 								- QEI_RESET_IDX: Reset Index Counter
 * @return		None
 **********************************************************************/
void QEI_Reset(LPC_QEI_T *QEIx, unsigned int ulResetType)
{
	VeloAccFlag = RESET;

	QEIx->CON = ulResetType;

	PosCnt = 0;
	// Reset value of Acc and Acc count to default
	VeloAcc = 0;
	VeloCapCnt = 0;
}

/*********************************************************************//**
 * @brief		Initializes the QEI peripheral according to the specified
*               parameters in the QEI_ConfigStruct.
 * @param[in]	QEIx				QEI peripheral, should be QEI
 * @param[in]	QEI_ConfigStruct	Pointer to a QEI_CFG_Type structure
*                    that contains the configuration information for the
*                    specified QEI peripheral
 * @return		None
 **********************************************************************/
void QEI_Initialize(LPC_QEI_T *QEIx, QEI_CFG_Type *QEI_ConfigStruct)
{
	// Modos dos pinos: com pull-up e histerese
	uint32_t gpio_mode = IOCON_MODE_PULLUP | IOCON_HYS_EN;

	/* Set up power for QEI module */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_QEI);

	  // P1.20 - PHA
	  Chip_IOCON_PinMux    (LPC_IOCON, 1, 20, gpio_mode, IOCON_FUNC3);

	  // P1.23 - PHB
	  Chip_IOCON_PinMux    (LPC_IOCON, 1, 23, gpio_mode, IOCON_FUNC3);

	  // P1.24 - IDX
	  Chip_IOCON_PinMux    (LPC_IOCON, 1, 24, gpio_mode, IOCON_FUNC3);

	// Reset all remaining value in QEI peripheral
	QEIx->CON       = QEI_CON_RESP | QEI_CON_RESV | QEI_CON_RESI;
	QEIx->MAXPOS    = 0x00;
	QEIx->CMPOS0    = 0x00;
	QEIx->CMPOS1    = 0x00;
	QEIx->CMPOS2    = 0x00;
	QEIx->INXCMP1   = 0x00;
	QEIx->INXCMP2   = 0x00;
	QEIx->LOAD      = 0x00;
	QEIx->VELCOMP   = 0x00;
	QEIx->FILTERPHA = 0x00;
	QEIx->FILTERPHB = 0x00;
	QEIx->FILTERINX = 0x00;
	// Disable all Interrupt
	QEIx->IEC       = QEI_IECLR_BITMASK;
	// Clear all Interrupt pending
	QEIx->CLR       = QEI_INTCLR_BITMASK;
	// Set QEI configuration value corresponding to its setting up value
	QEIx->CONF      = ((QEI_CFGOPT_Type *)QEI_ConfigStruct)->ulQEIConfig;
}


/*********************************************************************//**
 * @brief		De-initializes the QEI peripheral registers to their
*                  default reset values.
 * @param[in]	QEIx				QEI peripheral, should be QEI
 * @return 		None
 **********************************************************************/
void QEI_DeInit(LPC_QEI_T *QEIx)
{
	/* Turn off clock and power for QEI module */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_QEI);
}


/*****************************************************************************//**
* @brief		Fills each QIE_InitStruct member with its default value:
* 				- DirectionInvert = QEI_DIRINV_NONE
* 				- SignalMode = QEI_SIGNALMODE_QUAD
* 				- CaptureMode = QEI_CAPMODE_4X
* 				- InvertIndex = QEI_INVINX_NONE
* @param[in]	QIE_InitStruct Pointer to a QEI_CFG_Type structure
*                    which will be initialized.
* @return		None
*******************************************************************************/
void QEI_ConfigStructInit(QEI_CFG_Type *QIE_InitStruct)
{
	QIE_InitStruct->CaptureMode = QEI_CAPMODE_4X;
	QIE_InitStruct->DirectionInvert = QEI_DIRINV_NONE;
	QIE_InitStruct->InvertIndex = QEI_INVINX_NONE;
	QIE_InitStruct->SignalMode = QEI_SIGNALMODE_QUAD;
}


/*********************************************************************//**
 * @brief		Check whether if specified flag status is set or not
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @param[in]	ulFlagType	Status Flag Type, should be one of the following:
 * 							- QEI_STATUS_DIR: Direction Status
 * @return		New Status of this status flag (SET or RESET)
 **********************************************************************/
FlagStatus QEI_GetStatus(LPC_QEI_T *QEIx, unsigned int ulFlagType)
{
	return ((QEIx->STAT & ulFlagType) ? SET : RESET);
}

/*********************************************************************//**
 * @brief		Get current position value in QEI peripheral
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @return		Current position value of QEI peripheral
 **********************************************************************/
unsigned int QEI_GetPosition(LPC_QEI_T *QEIx)
{
	return (QEIx->POS);
}

/*********************************************************************//**
 * @brief		Set max position value for QEI peripheral
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @param[in]	ulMaxPos	Max position value to set
 * @return		None
 **********************************************************************/
void QEI_SetMaxPosition(LPC_QEI_T *QEIx, unsigned int ulMaxPos)
{
	QEIx->MAXPOS = ulMaxPos;
}

/*********************************************************************//**
 * @brief		Set position compare value for QEI peripheral
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @param[in]	bPosCompCh	Compare Position channel, should be:
 * 							- QEI_COMPPOS_CH_0: QEI compare position channel 0
 * 							- QEI_COMPPOS_CH_1: QEI compare position channel 1
 * 							- QEI_COMPPOS_CH_2: QEI compare position channel 2
 * @param[in]	ulPosComp	Compare Position value to set
 * @return		None
 **********************************************************************/
void QEI_SetPositionComp(LPC_QEI_T *QEIx, unsigned char bPosCompCh, unsigned int ulPosComp)
{
	uint32_t *tmp;

	tmp = (uint32_t *) (&(QEIx->CMPOS0) + bPosCompCh * 4);
	*tmp = ulPosComp;

}

/*********************************************************************//**
 * @brief		Get current index counter of QEI peripheral
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @return		Current value of QEI index counter
 **********************************************************************/
unsigned int QEI_GetIndex(LPC_QEI_T *QEIx)
{
	return (QEIx->INXCNT);
}

/*********************************************************************//**
 * @brief		Set value for index compare in QEI peripheral
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @param[in]	ulIndexComp		Compare Index Value to set
 * @return		None
 **********************************************************************/
void QEI_SetIndexComp(LPC_QEI_T *QEIx, unsigned int ulIndexComp)
{
	QEIx->INXCMP0 = ulIndexComp;
}

/*********************************************************************//**
 * @brief		Set timer reload value for QEI peripheral. When the velocity timer is
 * 				over-flow, the value that set for Timer Reload register will be loaded
 * 				into the velocity timer for next period. The calculated velocity in RPM
 * 				therefore will be affect by this value.
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	QEIReloadStruct	QEI reload structure
 * @return		None
 **********************************************************************/
void QEI_SetTimerReload(LPC_QEI_T *QEIx, QEI_RELOADCFG_Type *QEIReloadStruct)
{
	uint32_t pclk;

	if (QEIReloadStruct->ReloadOption == QEI_TIMERRELOAD_TICKVAL) {
		QEIx->LOAD = QEIReloadStruct->ReloadValue - 1;
	} else {
		pclk = SystemCoreClock/4;//(uint64_t)CLKPWR_GetPCLK(32);
		pclk = (pclk /(1000000/QEIReloadStruct->ReloadValue)) - 1;
		QEIx->LOAD = pclk;
	}
}

/*********************************************************************//**
 * @brief		Get current timer counter in QEI peripheral
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @return		Current timer counter in QEI peripheral
 **********************************************************************/
unsigned int QEI_GetTimer(LPC_QEI_T *QEIx)
{
	return (QEIx->TIME);
}

/*********************************************************************//**
 * @brief		Get current velocity pulse counter in current time period
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @return		Current velocity pulse counter value
 **********************************************************************/
unsigned int QEI_GetVelocity(LPC_QEI_T *QEIx)
{
	return (QEIx->VEL);
}

/*********************************************************************//**
 * @brief		Get the most recently measured velocity of the QEI. When
 * 				the Velocity timer in QEI is over-flow, the current velocity
 * 				value will be loaded into Velocity Capture register.
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @return		The most recently measured velocity value
 **********************************************************************/
unsigned int QEI_GetVelocityCap(LPC_QEI_T *QEIx)
{
	return (QEIx->CAP);
}

/*********************************************************************//**
 * @brief		Set Velocity Compare value for QEI peripheral
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulVelComp		Compare Velocity value to set
 * @return		None
 **********************************************************************/
void QEI_SetVelocityComp(LPC_QEI_T *QEIx, unsigned int ulVelComp)
{
	QEIx->VELCOMP = ulVelComp;
}

/*********************************************************************//**
 * @brief		Set value of sampling count for the digital filter in
 * 				QEI peripheral
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulSamplingPulse	Value of sampling count to set
 * @return		None
 **********************************************************************/
void QEI_SetDigiFilter(LPC_QEI_T *QEIx, unsigned int ulSamplingPulse)
{
	QEIx->FILTERPHA = ulSamplingPulse;
	QEIx->FILTERPHB = ulSamplingPulse;
	QEIx->FILTERINX = ulSamplingPulse;
}

/*********************************************************************//**
 * @brief		Check whether if specified interrupt flag status in QEI
 * 				peripheral is set or not
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulIntType		Interrupt Flag Status type, should be:
								- QEI_INTFLAG_INX_Int: index pulse was detected interrupt
								- QEI_INTFLAG_TIM_Int: Velocity timer over flow interrupt
								- QEI_INTFLAG_VELC_Int: Capture velocity is less than compare interrupt
								- QEI_INTFLAG_DIR_Int: Change of direction interrupt
								- QEI_INTFLAG_ERR_Int: An encoder phase error interrupt
								- QEI_INTFLAG_ENCLK_Int: An encoder clock pulse was detected interrupt
								- QEI_INTFLAG_POS0_Int: position 0 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_POS1_Int: position 1 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_POS2_Int: position 2 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_REV_Int: Index compare value is equal to the current
														index count interrupt
								- QEI_INTFLAG_POS0REV_Int: Combined position 0 and revolution count interrupt
								- QEI_INTFLAG_POS1REV_Int: Combined position 1 and revolution count interrupt
								- QEI_INTFLAG_POS2REV_Int: Combined position 2 and revolution count interrupt
 * @return		New State of specified interrupt flag status (SET or RESET)
 **********************************************************************/
FlagStatus QEI_GetIntStatus(LPC_QEI_T *QEIx, unsigned int ulIntType)
{
	return((QEIx->INTSTAT & ulIntType) ? SET : RESET);
}

/*********************************************************************//**
 * @brief		Enable/Disable specified interrupt in QEI peripheral
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulIntType		Interrupt Flag Status type, should be:
 * 								- QEI_INTFLAG_INX_Int: index pulse was detected interrupt
 *								- QEI_INTFLAG_TIM_Int: Velocity timer over flow interrupt
 *								- QEI_INTFLAG_VELC_Int: Capture velocity is less than compare interrupt
 * 								- QEI_INTFLAG_DIR_Int: Change of direction interrupt
 *  							- QEI_INTFLAG_ERR_Int: An encoder phase error interrupt
 * 								- QEI_INTFLAG_ENCLK_Int: An encoder clock pulse was detected interrupt
 *								- QEI_INTFLAG_POS0_Int: position 0 compare value is equal to the
 *														current position interrupt
 *								- QEI_INTFLAG_POS1_Int: position 1 compare value is equal to the
 *														current position interrupt
 *								- QEI_INTFLAG_POS2_Int: position 2 compare value is equal to the
 *														current position interrupt
 *								- QEI_INTFLAG_REV_Int: Index compare value is equal to the current
 *														index count interrupt
 *								- QEI_INTFLAG_POS0REV_Int: Combined position 0 and revolution count interrupt
 *								- QEI_INTFLAG_POS1REV_Int: Combined position 1 and revolution count interrupt
 *								- QEI_INTFLAG_POS2REV_Int: Combined position 2 and revolution count interrupt
 * @param[in]	NewState		New function state, should be:
 *								- DISABLE
 *								- ENABLE
 * @return		None
 **********************************************************************/
void QEI_IntCmd(LPC_QEI_T *QEIx, unsigned int ulIntType, FunctionalState NewState)
{
	if (NewState == ENABLE) {
		QEIx->IES = ulIntType;
	} else {
		QEIx->IEC = ulIntType;
	}
}


/*********************************************************************//**
 * @brief		Sets (forces) specified interrupt in QEI peripheral
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulIntType		Interrupt Flag Status type, should be:
								- QEI_INTFLAG_INX_Int: index pulse was detected interrupt
								- QEI_INTFLAG_TIM_Int: Velocity timer over flow interrupt
								- QEI_INTFLAG_VELC_Int: Capture velocity is less than compare interrupt
								- QEI_INTFLAG_DIR_Int: Change of direction interrupt
								- QEI_INTFLAG_ERR_Int: An encoder phase error interrupt
								- QEI_INTFLAG_ENCLK_Int: An encoder clock pulse was detected interrupt
								- QEI_INTFLAG_POS0_Int: position 0 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_POS1_Int: position 1 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_POS2_Int: position 2 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_REV_Int: Index compare value is equal to the current
														index count interrupt
								- QEI_INTFLAG_POS0REV_Int: Combined position 0 and revolution count interrupt
								- QEI_INTFLAG_POS1REV_Int: Combined position 1 and revolution count interrupt
								- QEI_INTFLAG_POS2REV_Int: Combined position 2 and revolution count interrupt
 * @return		None
 **********************************************************************/
void QEI_IntSet(LPC_QEI_T *QEIx, unsigned int ulIntType)
{
	QEIx->SET = ulIntType;
}

/*********************************************************************//**
 * @brief		Clear (force) specified interrupt (pending) in QEI peripheral
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulIntType		Interrupt Flag Status type, should be:
								- QEI_INTFLAG_INX_Int: index pulse was detected interrupt
								- QEI_INTFLAG_TIM_Int: Velocity timer over flow interrupt
								- QEI_INTFLAG_VELC_Int: Capture velocity is less than compare interrupt
								- QEI_INTFLAG_DIR_Int: Change of direction interrupt
								- QEI_INTFLAG_ERR_Int: An encoder phase error interrupt
								- QEI_INTFLAG_ENCLK_Int: An encoder clock pulse was detected interrupt
								- QEI_INTFLAG_POS0_Int: position 0 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_POS1_Int: position 1 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_POS2_Int: position 2 compare value is equal to the
														current position interrupt
								- QEI_INTFLAG_REV_Int: Index compare value is equal to the current
														index count interrupt
								- QEI_INTFLAG_POS0REV_Int: Combined position 0 and revolution count interrupt
								- QEI_INTFLAG_POS1REV_Int: Combined position 1 and revolution count interrupt
								- QEI_INTFLAG_POS2REV_Int: Combined position 2 and revolution count interrupt
 * @return		None
 **********************************************************************/
void QEI_IntClear(LPC_QEI_T *QEIx, unsigned int ulIntType)
{
	QEIx->CLR = ulIntType;
}


/*********************************************************************//**
 * @brief		Calculates the actual velocity in RPM passed via velocity
 * 				capture value and Pulse Per Round (of the encoder) value
 * 				parameter input.
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulVelCapValue	Velocity capture input value that can
 * 								be got from QEI_GetVelocityCap() function
 * @param[in]	ulPPR			Pulse per round of encoder
 * @return		The actual value of velocity in RPM (Round per minute)
 **********************************************************************/
unsigned int QEI_CalculateRPM(LPC_QEI_T *QEIx, unsigned int ulVelCapValue, unsigned int ulPPR)
{
	uint32_t rpm, clock, Load, edges;

	// Get current Clock rate for timer input
	clock = SystemCoreClock/4;//(uint64_t)CLKPWR_GetPCLK(32);
	// Get Timer load value (velocity capture period)
	Load  = QEIx->LOAD + 1;
	// Get Edge
	edges = (QEIx->CONF & QEI_CONF_CAPMODE) ? 4 : 2;
	// Calculate RPM
	rpm = ((clock * ulVelCapValue * 60) / (Load * ulPPR * edges));

	return (uint32_t)(rpm);
}

void QEI_Init(void)
{
	/* Configure the NVIC Preemption Priority Bits:
	* two (2) bits of preemption priority, six (6) bits of sub-priority.
	* Since the Number of Bits used for Priority Levels is five (5), so the
	* actual bit number of sub-priority is three (3)
	*/
	//NVIC_SetPriorityGrouping(0x05);

	/* Initialize QEI configuration structure to default value */
	// Capture Mode from ld.c
/*	#if CAP_MODE
	QEIConfig.CaptureMode = QEI_CAPMODE_4X;
	#else
	QEIConfig.CaptureMode = QEI_CAPMODE_2X;
	#endif*/
	QEIConfig.DirectionInvert = QEI_DIRINV_NONE;
	QEIConfig.InvertIndex = QEI_INVINX_NONE;
	#if SIGNAL_MODE
	QEIConfig.SignalMode = QEI_SIGNALMODE_CLKDIR;
	#else
	QEIConfig.SignalMode = QEI_SIGNALMODE_QUAD;
	#endif

	/* Initialize QEI peripheral with given configuration structure */
	QEI_Initialize(LPC_QEI, &QEIConfig);

	// Set timer reload value for  QEI that used to set velocity capture period
	//ReloadConfig.ReloadOption = QEI_TIMERRELOAD_USVAL;
	//ReloadConfig.ReloadValue = CAP_PERIOD;
	//QEI_SetTimerReload(QEI, &ReloadConfig);

	// Set Max Position
	QEI_SetMaxPosition(LPC_QEI, 0xffffffff);

	/* preemption = 1, sub-priority = 1 */
	//NVIC_SetPriority(QEI_IRQn, ((0x01<<3)|0x01));

	// Reset VeloAccFlag
	//VeloAccFlag = RESET;
	// Reset value of Acc and Acc count to default
	//VeloAcc = 0;
	//VeloCapCnt = 0;

	// Enable interrupt for velocity Timer overflow for capture velocity into Acc */
	//QEI_IntCmd(QEI, QEI_INTFLAG_TIM_Int, ENABLE);
	// Enable interrupt for direction change */
	//QEI_IntCmd(QEI, QEI_INTFLAG_DIR_Int, ENABLE);

	/* Enable interrupt for QEI  */
	//NVIC_EnableIRQ(QEI_IRQn);
}

/**
 * @}
 */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */

