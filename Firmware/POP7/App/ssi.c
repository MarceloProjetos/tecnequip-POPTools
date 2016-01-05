#include "lpc17xx.h"
#include "ssi.h"

/******************************************************************************
* Encoder SSI
******************************************************************************/

struct SSI_Settings {
	unsigned int Size; // EncAbsoluto.xls: B1
	unsigned int Mode;

	unsigned int MaxPos; // EncAbsoluto.xls: B3

	unsigned int SetPos_StartRealPosition; // EncAbsoluto.xls: B4
	unsigned int SetPos_StartNewPosition; // EncAbsoluto.xls: B5
	unsigned int SetPos_Breakpoint; // EncAbsoluto.xls: B14
};

static struct SSI_Settings settings;

void SSI_Init(unsigned int size, unsigned int mode)
{
	settings.Mode = mode;
	settings.Size = size;

	settings.MaxPos = (unsigned int)((1L << settings.Size) - 1);

	settings.SetPos_Breakpoint        = 0;
	settings.SetPos_StartNewPosition  = 0;
	settings.SetPos_StartRealPosition = 0;

	GPIO4->FIODIR |= 1UL << 28;
}

#define SSP_CLK_LOW  0
#define SSP_CLK_HIGH 1

static void SSI_ChangeCLK(unsigned int level)
{
	volatile unsigned int wait = 0, val = 1 << 28;

	// Configurar sinal para o nível solicitado
	if(level == SSP_CLK_HIGH) {
		GPIO4->FIOSET = val;
	} else {
		GPIO4->FIOCLR = val;
	}

	// Aguarda o tempo necessário para o sinal
	while(wait++ < 10);
}

static unsigned int Gray2Bin(unsigned int gray)
{
	unsigned int bin = 0, i = 32, last = 0, current;

	while(i--) {
		current = ((gray >> i) & 1) ^ last;
		bin |= current << i;
		last = current;
	}

	return bin;
}

unsigned int SSI_Read(void)
{
	unsigned int i;
	unsigned int NewPosition = 0; // EncAbsoluto.xls: B9
	int BreakAdjust; // EncAbsoluto.xls: B15

  // Configurar Nível Baixo
  SSI_ChangeCLK(SSP_CLK_LOW);

  for (i = 0; i < settings.Size; i++ )
  {
	  // Configurar Nível Alto
	  SSI_ChangeCLK(SSP_CLK_HIGH);

	  // Ler dado
	  NewPosition = (NewPosition << 1) | ((GPIO4->FIOPIN >> 29)&1);

	  // Configurar Nível Baixo
	  SSI_ChangeCLK(SSP_CLK_LOW);
  }

  // Configurar Nível Alto
  SSI_ChangeCLK(SSP_CLK_HIGH);

  if(settings.Mode == SSI_MODE_GRAY) {
	  NewPosition = Gray2Bin(NewPosition);
  }

  /*** Calcula valor a retornar conforme offset configurado ***/
  if(settings.SetPos_Breakpoint) {
	  // Calcula Ajuste de Quebra: B15 em EncAbsoluto.xls
	  BreakAdjust = 0;

	  if(settings.SetPos_StartNewPosition > settings.SetPos_StartRealPosition) {
		  if(NewPosition > settings.SetPos_Breakpoint) {
			  BreakAdjust = settings.MaxPos - NewPosition;
			  BreakAdjust++;
		  }
  	  } else {
		  if(NewPosition < settings.SetPos_Breakpoint) {
			  BreakAdjust = settings.SetPos_Breakpoint - NewPosition;
		  }
  	  }

	  // Calcula a nova posicao: B16 em EncAbsoluto.xls
	  if(!BreakAdjust) {
		  NewPosition  = NewPosition - settings.SetPos_StartRealPosition;
		  NewPosition += settings.SetPos_StartNewPosition;
	  } else if(settings.SetPos_StartNewPosition > settings.SetPos_StartRealPosition) {
		  NewPosition  = settings.SetPos_StartNewPosition - settings.SetPos_StartRealPosition;
		  NewPosition -= BreakAdjust;
	  } else {
		  NewPosition  = settings.MaxPos - BreakAdjust;
		  NewPosition++;
	  }
  }

  return  NewPosition;
}

void SSI_SetPosition(int offset)
{
	// Zera valores para uma leitura "limpa" do encoder
	settings.SetPos_Breakpoint        = 0;
	settings.SetPos_StartNewPosition  = 0;
	settings.SetPos_StartRealPosition = 0;

	settings.SetPos_StartRealPosition = SSI_Read();
	settings.SetPos_StartNewPosition  = offset;

	// Calcula Ponto de Quebra: B14 em EncAbsoluto.xls
	if(settings.SetPos_StartNewPosition > settings.SetPos_StartRealPosition) {
		settings.SetPos_Breakpoint  = settings.MaxPos - settings.SetPos_StartNewPosition;
		settings.SetPos_Breakpoint += settings.SetPos_StartRealPosition;
	} else {
		settings.SetPos_Breakpoint  = settings.SetPos_StartRealPosition - settings.SetPos_StartNewPosition;
	}
}
