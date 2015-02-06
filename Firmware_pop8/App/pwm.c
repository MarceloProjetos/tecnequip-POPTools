#include "pwm.h"
#include <string.h>
#include "chip.h"

extern volatile char GPIO_PWM_ENABLED;
extern volatile char GPIO_OUTPUT_PORT16;

#define PWM_CLK 12500000UL // 12,5 MHz

/**
 * @brief Motor Control PWM register block structure
 */
typedef struct {				/*!< PWM Structure            */
	__IO uint32_t IR;			/*!< Interrupt Register       */
	__IO uint32_t TCR;			/*!< Timer Control Register   */
	__IO uint32_t TC;			/*!< Timer Counter            */
	__IO uint32_t PR;			/*!< Prescale Register        */
	__IO uint32_t PC;			/*!< Prescale Counter         */
	__IO uint32_t MCR;			/*!< Match Control Register   */
	__IO uint32_t MR0[4];		/*!< Match Register           */
	__IO uint32_t CCR;			/*!< Capture Control Register */
	__I  uint32_t CR[2];		/*!< Capture Register         */
	__IO uint32_t RESERVED0[3];	/*!< Reserved Address         */
	__IO uint32_t MR1[3];		/*!< Match Register           */
	__IO uint32_t PCR;			/*!< Control Register         */
	__IO uint32_t LER;			/*!< Load Enable Register     */
	__IO uint32_t RESERVED1[7];	/*!< Reserved Address         */
	__IO uint32_t CTCR;			/*!< Count Control Register   */
} LPC_PWM_T;

#define LPC_PWM1 ((LPC_PWM_T *) LPC_PWM1_BASE)

#define LPC_PWM_MR(pPWM, n) ((n < 4) ? pPWM->MR0[n] : pPWM->MR1[n - 3])

void inline LPC_PWM_MR_SET(LPC_PWM_T *pPWM, uint32_t n, uint32_t val) {
	if(n < 3) {
		pPWM->MR0[n] = val;
	} else {
		pPWM->MR1[n - 3] = val;
	}
}

/******************************************************************************
* PWM Enable
******************************************************************************/
static void PWM_Enable(int enable)
{
	unsigned int mcr_mask = (0x1 << 1), tcr_mask = (0x01 << 0) | (0x01 << 3), pcr_mask = (0x01 << 9);
	unsigned int mcr = LPC_PWM1->MCR, tcr = LPC_PWM1->TCR, pcr = LPC_PWM1->PCR;

	// Reset all registers
	mcr &= ~mcr_mask;
	tcr &= ~tcr_mask;
	pcr &= ~pcr_mask;

	LPC_PWM1->MCR = mcr;
	LPC_PWM1->TCR = tcr;
	LPC_PWM1->PCR = pcr;

	if(enable) {
		LPC_PWM1->PR   = 1;
		LPC_PWM1->MCR |= mcr_mask;
		LPC_PWM1->TCR |= tcr_mask;
		LPC_PWM1->PCR |= pcr_mask;

		mcr = *(uint32_t *)(0x40018014);
		tcr = *(uint32_t *)(0x40018004);
		pcr = *(uint32_t *)(0x4001804c);
	}
}

/******************************************************************************
* PWM Set
******************************************************************************/
void PWM_Set(int duty_cycle, int frequency)
{
	static int PreviousDC = -1, PreviousFreq = -1;

	if(duty_cycle <= 0) {
		duty_cycle = 100; // Se no limite inferior, duty cycle deve ser 100 para que MR0 = MR1
	} else if(duty_cycle >= 100) {
		duty_cycle =   0; // Se no limit superior, duty cycle deve ser 0 para que MR1 = 0
	}

	if(PreviousDC != duty_cycle || PreviousFreq != frequency) {
		// Saving Current Values
		PreviousDC   = duty_cycle;
		PreviousFreq = frequency;

		// Setting PWM Registers
		PWM_Enable(0); // Disable PWM while updating registers
		if(frequency) {
			// Set frequency register - MR0
			LPC_PWM_MR_SET(LPC_PWM1, 0, PWM_CLK / frequency);
			// Set Duty Cycle register - MR1
			LPC_PWM_MR_SET(LPC_PWM1, 1, (LPC_PWM_MR(LPC_PWM1, 0) * duty_cycle) / 100);
			// Set Latch Enable to load MR0 and MR1 on the next cycle
			LPC_PWM1->LER |= 3;
			// Enable PWM after to update registers
			PWM_Enable(1);
		}

		// Update OUTPUT 16
		GPIO_OUTPUT_PORT16 = frequency ? 1 : 0;
	}
}

/******************************************************************************
* PWM Init
******************************************************************************/
void PWM_Init(void)
{
	// 1 - Enable device on PCONP
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PWM1);

	// 2 - Configure pins
	Chip_IOCON_PinMux(LPC_IOCON, 1, 18, 0, IOCON_FUNC2); // P1.18: PWM Canal 1, saida 1

	// 3 - Reserve Output 16 for PWM
	GPIO_PWM_ENABLED = 1;

	// 4 - Reset PWM
	PWM_Set(0, 0);
}
