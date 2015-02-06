#ifndef __ADC_H__
#define __ADC_H__

void ADC_Init(void);
unsigned int ADC_Read(unsigned int a);
void ADC_SetMask(unsigned int ad_mask);
void ADC_Update(void);

#endif

