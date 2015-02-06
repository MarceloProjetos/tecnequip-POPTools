#ifndef __GPIO_H__
#define __GPIO_H__

#define HTONS(n) (unsigned short int)((((unsigned short int) (n)) << 8) | (((unsigned short int) (n)) >> 8))

void 			GPIO_Init();
unsigned int 	GPIO_Output(void);
unsigned int 	GPIO_Input();

#endif
