#include <string.h>
#include "chip.h"

#define AD_BUFFER_SIZE 9
#define AD_MINMAX_SIZE (AD_BUFFER_SIZE/3)

static unsigned int admax[AD_MINMAX_SIZE], admin[AD_MINMAX_SIZE];

static ADC_CLOCK_SETUP_T ADCSetup;

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
							     { {0, 100} },
};

/******************************************************************************
* ADC Read
******************************************************************************/
#define ARRAY_SIZEOF(a) (sizeof(a)/sizeof(a[0]))

void ADC_Init(void)
{
	// Valor de configuracao dos GPIOs: com histerese
	uint32_t gpio_mode = IOCON_HYS_EN;

	// Configura os Pinos
	Chip_IOCON_PinMux(LPC_IOCON, 0, 23, gpio_mode, IOCON_FUNC1); // P0.23 - Canal 0
	Chip_IOCON_PinMux(LPC_IOCON, 0, 24, gpio_mode, IOCON_FUNC1); // P0.24 - Canal 1
	Chip_IOCON_PinMux(LPC_IOCON, 0, 25, gpio_mode, IOCON_FUNC1); // P0.25 - Canal 2
	Chip_IOCON_PinMux(LPC_IOCON, 0, 26, gpio_mode, IOCON_FUNC1); // P0.26 - Canal 3
	Chip_IOCON_PinMux(LPC_IOCON, 1, 30, gpio_mode, IOCON_FUNC3); // P1.30 - Canal 4
	Chip_IOCON_PinMux(LPC_IOCON, 1, 31, gpio_mode, IOCON_FUNC3); // P1.31 - Canal 5

	// Inicializa o controlador
	Chip_ADC_Init(LPC_ADC, &ADCSetup);

	// Habilita os canais
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH3, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH4, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH5, ENABLE);

	// Configuracao do A/D
	ADCSetup.burstMode = true;
	Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ADC_MAX_SAMPLE_RATE);
	Chip_ADC_SetBurstCmd(LPC_ADC, ADCSetup.burstMode);

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
  uint16_t i, a, adval = 0, channel = 0;
  uint32_t val;

  for(a=0; ADC_Mask>>a; a++) {
	  if((ADC_Mask>>a) & 1) {
		  switch (a)
		  {
		  case 0:
		    channel = ADC_CH0;
		    break;
		  case 1:
			    channel = ADC_CH1;
		    break;
		  case 2:
			    channel = ADC_CH2;
		    break;
		  case 3:
			    channel = ADC_CH5;
		    break;
		  case 4:  // AD CPU
			    channel = ADC_CH3;
		    break;
		  case 5:  // SENSOR DE TEMPERATURA
			    channel = ADC_CH4;
		  }

		  if(Chip_ADC_ReadValue(LPC_ADC, channel, &adval) == SUCCESS) {
			val = adval; // Transafere o valor lido para um uint32_t
		    if(a == 5) { // Sensor de Temperatura
              val = (val * 3300) / 4096;	// 2 casas decimais, ex: 1,04V
		    } else {
              val = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V
		    }

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

// Definições para a conversão do A/D do sensor de temperatura
#define AD_TEMP_RANGE_HIGH (3277)
#define AD_TEMP_RANGE_MID  (1938)
#define AD_TEMP_RANGE_LOW  ( 538)

#define AD_TEMP_GAP_HIGH   (AD_TEMP_RANGE_HIGH - AD_TEMP_RANGE_MID)
#define AD_TEMP_GAP_LOW    (AD_TEMP_RANGE_MID  - AD_TEMP_RANGE_LOW)

#define AD_TEMP_HIGHEST    (- 50)
#define AD_TEMP_MID        (+ 50)
#define AD_TEMP_LOWEST     (+150)

int ADC_Read(unsigned int a)
{
  int val_ad;
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

  val_ad = soma / (AD_BUFFER_SIZE - 2*AD_MINMAX_SIZE);

  // Checa se alguma conversão adicional deve ser realizada no valor a ser retornado
  switch(a) {
    case 5: { // SENSOR DE TEMPERATURA
      if(val_ad >= AD_TEMP_RANGE_HIGH) {
    	val_ad = AD_TEMP_HIGHEST;
      } else if(val_ad > AD_TEMP_RANGE_MID) {
        val_ad = AD_TEMP_HIGHEST + ((AD_TEMP_MID    - AD_TEMP_HIGHEST)*(AD_TEMP_RANGE_HIGH - val_ad))/AD_TEMP_GAP_HIGH;
      } else if(val_ad == AD_TEMP_RANGE_MID) {
    	val_ad = AD_TEMP_MID;
	  } else if(val_ad > AD_TEMP_RANGE_LOW) {
	    val_ad = AD_TEMP_MID     + ((AD_TEMP_LOWEST - AD_TEMP_MID    )*(AD_TEMP_RANGE_MID  - val_ad))/AD_TEMP_GAP_LOW;
	  } else {
		val_ad = AD_TEMP_LOWEST;
	  }
    }
  }

  return val_ad;
}
