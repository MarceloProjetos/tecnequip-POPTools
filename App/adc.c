#include <string.h>
#include "lpc17xx.h"

typedef struct ad_type
{
  unsigned int m[50];
} ads;

struct ad_type ad[5];

typedef struct ads_fator
{
  unsigned int tensao;
  unsigned int fator;
} ad_fator;

//-------------------------------   0V  --- 250mV ---  500mV --- 750mV ------- 1V ------ 1,25V ------ 1,5V ----- 2V ------- 3V ------- 4V -------- 5V ------ 6V ------- 7V ------ 8V ------- 9V -------- 10v ---
struct ads_fator ad0fator[16] = { {7, 0}, {37, 68}, {59, 85}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108} };
struct ads_fator ad1fator[16] = { {7, 0}, {37, 68}, {59, 88}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108} };
struct ads_fator ad2fator[16] = { {7, 0}, {37, 68}, {59, 85}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108} };
struct ads_fator ad3fator[16] = { {7, 0}, {37, 68}, {59, 83}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108} };
/******************************************************************************
* ADC Read
******************************************************************************/
#define ARRAY_SIZEOF(a) (sizeof(a)/sizeof(a[0]))

void ADC_Init(void)
{
	// AD's
	SC->PCONP |= 1 << 12;         // ADC
	PINCON->PINSEL1 &= ~((0x3 << 14) | (0x3 << 16) | (0x3 << 18) | (0x3 << 20)); // AD0.0, AD0.1, AD0.2, AD0.3(REF)
	PINCON->PINSEL1 |= (0x1 << 14) | (0x1 << 16) | (0x1 << 18) | (0x1 << 20);
	PINCON->PINSEL3 |= 0x3 << 30; // AD0.5
	ADC->ADCR = 0x2F | (0x18 << 8) | (1 << 16) | (1 << 21); // Enable AD0:1:2:3:5, 1Mhz, BURST ON, PDN Enable

	// Reset AD´s
	memset(ad, 0, sizeof(ad));

}

unsigned int ADC_Read(unsigned int a)
{
  unsigned int max = 0;
  unsigned int min = 0;
  unsigned int soma = 0;
  unsigned int i = 0;
  unsigned int z = 0;

  unsigned int val = 0;
  unsigned int tensao = 0;
  unsigned char set = 0;

  if (a < 1 || a > 6) return 0;

  a -= 1;

  switch (a)
  {
  case 0:
    if (ADC->ADDR5 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR5 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

	  for (i = 1; i < ARRAY_SIZEOF(ad1fator); i++)
	  {
		  if (tensao < ad0fator[i].tensao)
		  {
			  tensao *= ad0fator[i - 1].fator;
			  tensao /= 100;
			  break;
		  }
	  }

	  val = tensao;
    }
    break;
  case 1:
    if (ADC->ADDR2 & 0x7FFFFFFF)
    {
      val = 0xFFF & (ADC->ADDR2 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

	  for (i = 1; i < ARRAY_SIZEOF(ad1fator); i++)
	  {
		  if (tensao < ad1fator[i].tensao)
		  {
			  tensao *= ad1fator[i - 1].fator;
			  tensao /= 100;
			  break;
		  }
	  }

	  val = tensao;
    }
    break;
  case 2:
    if (ADC->ADDR1 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR1 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

	  for (i = 1; i < ARRAY_SIZEOF(ad2fator); i++)
	  {
		  if (tensao < ad2fator[i].tensao)
		  {
			  tensao *= ad2fator[i - 1].fator;
			  tensao /= 100;
			  break;
		  }
	  }

	  val = tensao;
	}
    break;
  case 3:
    if (ADC->ADDR0 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR0 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

	  for (i = 1; i < ARRAY_SIZEOF(ad3fator); i++)
	  {
		  if (tensao < ad3fator[i].tensao)
		  {
			  tensao *= ad3fator[i - 1].fator;
			  tensao /= 100;
			  break;
		  }
	  }

	  val = tensao;
	}
    break;
  case 4:  // AD CPU
    if (ADC->ADDR4 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR4 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096; // 2 casas decimais, ex: 2,05V

	  tensao *= 48;	// fator de correção
  	  tensao /= 100;

	  val = tensao;
    }
    break;
  case 5:  // SENSOR DE TEMPERATURA
    if (ADC->ADDR5 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR5 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096; // 2 casas decimais, ex: 2,05V

	  tensao *= 48;	// fator de correção
  	  tensao /= 100;

	  val = tensao;
    }
    break;
  }

  soma = 0;
  z = 0;

  if (set == 1)
  {
    for (i = 0; i < ARRAY_SIZEOF(ad[a].m) - 1; i++)
      ad[a].m[i] = ad[a].m[i + 1];

    ad[a].m[ARRAY_SIZEOF(ad[0].m) - 1] = val;

    for (i = 0; i < ARRAY_SIZEOF(ad[a].m); i++)
      if (ad[a].m[max] < ad[a].m[i])
        max = i;

    for (i = 0; i < ARRAY_SIZEOF(ad[a].m); i++)
      if (ad[a].m[min] > ad[a].m[i])
        min = i;

	if (min == max)
	{
		soma = min;
		z = 1;
	}
	else
	{
		for (i = 0; i < ARRAY_SIZEOF(ad[a].m); i++)
		{
		  if (i != min && i != max)
		  {
			  if (ad[a].m[i])
			  {
				soma += ad[a].m[i];
				z++;
			  }
		  }
		}
	}
  }

  return z > 0 ? (unsigned int)(soma / z) : 0;
}
