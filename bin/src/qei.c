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
#include "lpc17xx.h"
#include "type.h"
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


/* Public Functions ----------------------------------------------------------- */
/** @addtogroup QEI_Public_Functions
 * @{
 */

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
void QEI_Reset(QEI_TypeDef *QEIx, uint32_t ulResetType)
{
	QEIx->QEICON = ulResetType;
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
void QEI_Init(QEI_TypeDef *QEIx, QEI_CFG_Type *QEI_ConfigStruct)
{
	/* Set up power for QEI module */
	SC -> PCONP |= (1 << 18);

  PINCON -> PINSEL3 &= ~0x0003C300;
  PINCON -> PINSEL3 |= 0x000014100;

  /* As default, peripheral clock for QEI module
	 * is set to FCCLK / 4 */
  SC->PCLKSEL1 &= ~(0x3UL); // bit0 e bit1 representam QEI no PCLKSEL1
  SC->PCLKSEL1 |= 1;

	// Reset all remaining value in QEI peripheral
	QEIx->QEICON = QEI_CON_RESP | QEI_CON_RESV | QEI_CON_RESI;
	QEIx->QEIMAXPOS = 0x00;
	QEIx->CMPOS0 = 0x00;
	QEIx->CMPOS1 = 0x00;
	QEIx->CMPOS2 = 0x00;
	QEIx->INXCMP = 0x00;
	QEIx->QEILOAD = 0x00;
	QEIx->VELCOMP = 0x00;
	QEIx->FILTER = 0x00;
	// Disable all Interrupt
	QEIx->QEIIEC = QEI_IECLR_BITMASK;
	// Clear all Interrupt pending
	QEIx->QEICLR = QEI_INTCLR_BITMASK;
	// Set QEI configuration value corresponding to its setting up value
	QEIx->QEICONF = ((QEI_CFGOPT_Type *)QEI_ConfigStruct)->ulQEIConfig;
}


/*********************************************************************//**
 * @brief		De-initializes the QEI peripheral registers to their
*                  default reset values.
 * @param[in]	QEIx				QEI peripheral, should be QEI
 * @return 		None
 **********************************************************************/
void QEI_DeInit(QEI_TypeDef *QEIx)
{
	/* Turn off clock and power for QEI module */
  SC -> PCONP &= ~(1 << 18);
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
FlagStatus QEI_GetStatus(QEI_TypeDef *QEIx, uint32_t ulFlagType)
{
	return ((QEIx->QEISTAT & ulFlagType) ? SET : RESET);
}

/*********************************************************************//**
 * @brief		Get current position value in QEI peripheral
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @return		Current position value of QEI peripheral
 **********************************************************************/
uint32_t QEI_GetPosition(QEI_TypeDef *QEIx)
{
	return (QEIx->QEIPOS);
}

/*********************************************************************//**
 * @brief		Set max position value for QEI peripheral
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @param[in]	ulMaxPos	Max position value to set
 * @return		None
 **********************************************************************/
void QEI_SetMaxPosition(QEI_TypeDef *QEIx, uint32_t ulMaxPos)
{
	QEIx->QEIMAXPOS = ulMaxPos;
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
void QEI_SetPositionComp(QEI_TypeDef *QEIx, uint8_t bPosCompCh, uint32_t ulPosComp)
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
uint32_t QEI_GetIndex(QEI_TypeDef *QEIx)
{
	return (QEIx->INXCNT);
}

/*********************************************************************//**
 * @brief		Set value for index compare in QEI peripheral
 * @param[in]	QEIx		QEI peripheral, should be QEI
 * @param[in]	ulIndexComp		Compare Index Value to set
 * @return		None
 **********************************************************************/
void QEI_SetIndexComp(QEI_TypeDef *QEIx, uint32_t ulIndexComp)
{
	QEIx->INXCMP = ulIndexComp;
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
void QEI_SetTimerReload(QEI_TypeDef *QEIx, QEI_RELOADCFG_Type *QEIReloadStruct)
{
	uint32_t pclk;

	if (QEIReloadStruct->ReloadOption == QEI_TIMERRELOAD_TICKVAL) {
		QEIx->QEILOAD = QEIReloadStruct->ReloadValue - 1;
	} else {
		pclk = SystemFrequency/4;//(uint64_t)CLKPWR_GetPCLK(32);
		pclk = (pclk /(1000000/QEIReloadStruct->ReloadValue)) - 1;
		QEIx->QEILOAD = pclk;
	}
}

/*********************************************************************//**
 * @brief		Get current timer counter in QEI peripheral
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @return		Current timer counter in QEI peripheral
 **********************************************************************/
uint32_t QEI_GetTimer(QEI_TypeDef *QEIx)
{
	return (QEIx->QEITIME);
}

/*********************************************************************//**
 * @brief		Get current velocity pulse counter in current time period
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @return		Current velocity pulse counter value
 **********************************************************************/
uint32_t QEI_GetVelocity(QEI_TypeDef *QEIx)
{
	return (QEIx->QEIVEL);
}

/*********************************************************************//**
 * @brief		Get the most recently measured velocity of the QEI. When
 * 				the Velocity timer in QEI is over-flow, the current velocity
 * 				value will be loaded into Velocity Capture register.
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @return		The most recently measured velocity value
 **********************************************************************/
uint32_t QEI_GetVelocityCap(QEI_TypeDef *QEIx)
{
	return (QEIx->QEICAP);
}

/*********************************************************************//**
 * @brief		Set Velocity Compare value for QEI peripheral
 * @param[in]	QEIx			QEI peripheral, should be QEI
 * @param[in]	ulVelComp		Compare Velocity value to set
 * @return		None
 **********************************************************************/
void QEI_SetVelocityComp(QEI_TypeDef *QEIx, uint32_t ulVelComp)
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
void QEI_SetDigiFilter(QEI_TypeDef *QEIx, uint32_t ulSamplingPulse)
{
	QEIx->FILTER = ulSamplingPulse;
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
FlagStatus QEI_GetIntStatus(QEI_TypeDef *QEIx, uint32_t ulIntType)
{
	return((QEIx->QEIINTSTAT & ulIntType) ? SET : RESET);
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
void QEI_IntCmd(QEI_TypeDef *QEIx, uint32_t ulIntType, FunctionalState NewState)
{
	if (NewState == ENABLE) {
		QEIx->QEIIES = ulIntType;
	} else {
		QEIx->QEIIEC = ulIntType;
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
void QEI_IntSet(QEI_TypeDef *QEIx, uint32_t ulIntType)
{
	QEIx->QEISET = ulIntType;
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
void QEI_IntClear(QEI_TypeDef *QEIx, uint32_t ulIntType)
{
	QEIx->QEICLR = ulIntType;
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
uint32_t QEI_CalculateRPM(QEI_TypeDef *QEIx, uint32_t ulVelCapValue, uint32_t ulPPR)
{
	uint32_t rpm, clock, Load, edges;

	// Get current Clock rate for timer input
	clock = SystemFrequency/4;//(uint64_t)CLKPWR_GetPCLK(32);
	// Get Timer load value (velocity capture period)
	Load  = QEIx->QEILOAD + 1;
	// Get Edge
	edges = (QEIx->QEICONF & QEI_CONF_CAPMODE) ? 4 : 2;
	// Calculate RPM
	rpm = ((clock * ulVelCapValue * 60) / (Load * ulPPR * edges));

	return (uint32_t)(rpm);
}


/**
 * @}
 */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */

