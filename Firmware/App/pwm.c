#include "pwm.h"
#include <string.h>
#include "lpc17xx.h"
#include <lpc17xx_clkpwr.h>

extern volatile char GPIO_PWM_ENABLED;
extern volatile char GPIO_OUTPUT_PORT16;

#define PWM_CLK 12500000UL // 12,5 MHz

/******************************************************************************
* PWM Enable
******************************************************************************/
static void PWM_Enable(int enable)
{
	unsigned int mcr_mask = (0x1 << 1), tcr_mask = (0x01 << 0) | (0x01 << 3), pcr_mask = (0x01 << 9);

	// Reset all registers
	PWM1->MCR &= ~mcr_mask;
	PWM1->TCR &= ~tcr_mask;
	PWM1->PCR &= ~pcr_mask;

	if(enable) {
		PWM1->PR   = 1;
		PWM1->MCR |= mcr_mask;
		PWM1->TCR |= tcr_mask;
		PWM1->PCR |= pcr_mask;
	}
}

/******************************************************************************
* PWM Set
******************************************************************************/
void PWM_Set(int duty_cycle, int frequency)
{
	static int PreviousDC = -1, PreviousFreq = -1;

	if(PreviousDC != duty_cycle || PreviousFreq != frequency) {
		// Saving Current Values
		PreviousDC   = duty_cycle;
		PreviousFreq = frequency;

		// Setting PWM Registers
		PWM_Enable(0); // Disable PWM while updating registers
		if(frequency) {
			// Set frequency register - MR0
			PWM1->MR0 = PWM_CLK / frequency;
			// Set Duty Cycle register - MR1
			PWM1->MR1 = (PWM1->MR0 * duty_cycle) / 100;
			PWM_Enable(1); // Enable PWM after to update registers
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
	SC->PCONP |= CLKPWR_PCONP_PCPWM1;

	// 2 - Configure clock
	SC->PCLKSEL0 &= ~CLKPWR_PCLKSEL_BITMASK(CLKPWR_PCLKSEL_PWM1);
	SC->PCLKSEL0 |=  CLKPWR_PCLKSEL_SET    (CLKPWR_PCLKSEL_PWM1, CLKPWR_PCLKSEL_CCLK_DIV_4);

	// 3 - Configure pins
	PINCON->PINSEL3  &= ~(0x3 << 4);
	PINCON->PINSEL3  |=  (0x2 << 4);
	PINCON->PINMODE3 &= ~(0x3 << 4);
	PINCON->PINMODE3 |=  (0x2 << 4);

	// 4 - Reserve Output 16 for PWM
	GPIO_PWM_ENABLED = 1;

	// 5 - Reset PWM
	PWM_Set(0, 0);
}
