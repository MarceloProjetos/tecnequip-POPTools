#include <string.h>
#include "lpc17xx.h"

#define AD_BUFFER_SIZE 9
#define AD_MINMAX_SIZE (AD_BUFFER_SIZE/3)

static unsigned int admax[AD_MINMAX_SIZE], admin[AD_MINMAX_SIZE];

typedef struct ad_type
{
  unsigned int m[AD_BUFFER_SIZE];
} ads;

struct ad_type ad[6];

typedef struct ads_fator
{
  unsigned int tensao;
  unsigned int fator;
} ad_fator;

//-------------------------------   0V  --- 250mV ---  500mV --- 750mV ------- 1V ------ 1,25V ------ 1,5V ----- 2V ------- 3V ------- 4V -------- 5V ------ 6V ------- 7V ------ 8V ------- 9V -------- 10v ---
struct ads_fator adfator[][17] = {
							     { {7,  0}, {37, 68}, {59, 85}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108}, {0, 108} },
							     { {7,  0}, {37, 68}, {59, 88}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108}, {0, 108} },
							     { {7,  0}, {37, 68}, {59, 85}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108}, {0, 108} },
							     { {7,  0}, {37, 68}, {59, 83}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108}, {0, 108} },
							     { {0, 48} },
							     { {0, 48} },
};

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

unsigned int ADC_Mask = 0;

void ADC_SetMask(unsigned int ad_mask)
{
	ADC_Mask = ad_mask;
}

void ADC_Update(void)
{
  unsigned int i, a, val;

  for(a=0; ADC_Mask>>a; a++) {
	  if((ADC_Mask>>a) && 1) {
		  switch (a)
		  {
		  case 0:
		    val = ADC->ADDR5;
		    break;
		  case 1:
		    val = ADC->ADDR2;
		    break;
		  case 2:
		    val = ADC->ADDR1;
		    break;
		  case 3:
		    val = ADC->ADDR0;
		    break;
		  case 4:  // AD CPU
		    val = ADC->ADDR3;
		    break;
		  case 5:  // SENSOR DE TEMPERATURA
		    val = ADC->ADDR4;
		  }

		  if (val & 0x80000000)
		  {
		    val = 0xFFF & (val >> 4);

		    val = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

		    for (i = 0; ; i++) // Sai quando encontrar item
			  {
		    	// Se tensao = 0, este é o último item. Devemos usá-lo de qualquer forma
				  if (adfator[a][i].tensao ? val < adfator[a][i+1].tensao : 1)
				  {
					  val *= adfator[a][i].fator;
					  val /= 100;
					  break;
				  }
			  }

		    for (i = 0; i < AD_BUFFER_SIZE - 1; i++)
		      ad[a].m[i] = ad[a].m[i + 1];

		    ad[a].m[i] = val;
		  }
	  }
  }
}

unsigned int ADC_Read(unsigned int a)
{
  unsigned int i, idx, z, soma;

  if (a < 1 || a > 6) return 0;

  a--; // A/D starts from zero

  soma = 0;
  for(z=0; z<AD_MINMAX_SIZE; z++) {
	  admax[z] =                0;
	  admin[z] = (unsigned int)-1; // Garante que min tem o maior valor possível
  }

  for (i = 0; i < AD_BUFFER_SIZE; i++) {
    soma += ad[a].m[i];

    // Busca na lista admax se o item atual deve entrar na lista
    for(z=0; z<AD_MINMAX_SIZE; z++) {
      if (admax[z] < ad[a].m[i]) {
        // Encontrou um elemento para ser adicionado.
        // Procura a partir deste ponto pelo menor item da lista
        // pois ele não deve mais fazer parte dela
        for(idx=z+1; idx<AD_MINMAX_SIZE; idx++) {
        	if(admax[idx] < admax[z])
        		z = idx;
        }
        admax[z] = ad[a].m[i];
        break;
      }
    }

    // Busca na lista admin se o item atual deve entrar na lista
    for(z=0; z<AD_MINMAX_SIZE; z++) {
      if (admin[z] > ad[a].m[i]) {
        // Encontrou um elemento para ser adicionado.
        // Procura a partir deste ponto pelo maior item da lista
        // pois ele não deve mais fazer parte dela
        for(idx=z+1; idx<AD_MINMAX_SIZE; idx++) {
          if(admin[idx] > admin[z])
        	z = idx;
        }
        admin[z] = ad[a].m[i];
        break;
      }
    }
  }

  for(z=0; z<AD_MINMAX_SIZE; z++) {
	  soma -= admax[z] + admin[z];
  }

  return soma / (AD_BUFFER_SIZE - 2*AD_MINMAX_SIZE);
}
