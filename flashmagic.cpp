#include <windows.h>
#include <Shlobj.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

#include "ldmicro.h"
#include "intcode.h"

#include "flashmagicarmcortex.h"

static unsigned long totalWrite = 0;

// erase progress function
// return 1 to continue erase
int EraseProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	char text[100];

	if (value)
		sprintf(text, "Preparando para gravar... %d%% conclu�do !", (unsigned int)(((float)value / (float)29) * 100));
	else
		sprintf(text, "Prepara��o conclu�da com sucesso !");

	StatusBarSetText(0, text);

	return 1;
}

int VerifyProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	char text[100];

	if (value > totalWrite)
		totalWrite = value;

	if (value)
		sprintf(text, "Verificando... %d%% conclu�do !", (unsigned int)(((float)value / (float)totalWrite) * 100));
	else
		sprintf(text, "Verifica��o conclu�da com sucesso !");

	StatusBarSetText(0, text);

	return 1;
}

// program progress function
// return 1 to continue programming
int ProgramProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	char text[100];

	if (value > totalWrite)
		totalWrite = value;

	if (value)
		sprintf(text, "Gravando... %d%% conclu�do !", 100 - (unsigned int)(((float)value / (float)totalWrite) * 100));
	else
		sprintf(text, "Grava��o conclu�da com sucesso !");

	StatusBarSetText(0, text);

	return 1;
}

BOOL FlashProgram(char *hexFile, int ComPort, long BaudRate)
{
  fm_results *presults;
  fm_connectoptions_com options;

  // use standard timeouts
	fm_set_default_timeouts();

  // generate a debug file containing all ISP commands
  //fm_select_debug_mode(FM_DEBUG_MODE_ON, "fmtest.fmd");

  options.osc            = 12.000;
  options.port           = ComPort;
  options.baudrate       = BaudRate;
  options.selecteddevice = FM_LPC1768;
  options.highspeed      = 0;
  options.clocks         = 0;
  options.halfduplex     = 0;
  options.hwconfig       = FM_HWBOOTEXEC;
  options.hwt1           = 200;
  options.hwt2           = 200;
  options.i2caddr        = 0;
  options.maxbaudrate    = 230400;
  options.usinginterface = 0;
  options.interfacetype  = FM_INTERFACETYPE_NONE;

	// connect to the device on the specified com port using the specified baud rate
  presults = fm_connect(&options, sizeof(options));
  if (presults->result != FM_OK)
  {
		switch (presults->result)
		{
		  case FM_ERROR_PORT:
				Error("Erro ao conectar na porta COM%d !", ComPort);
			  break;
			case FM_ERROR_BAUDRATE:
				Error("N�o foi poss�vel conectar com baudrate %i !", BaudRate);
				break;
			case FM_ERROR_INVALID_PARAMS:
				Error("Parametros de conex�o inv�lidos !");
				break;
			case FM_ERROR_CMD:
				Error("Enviado comando inv�lido (conectando) !");
				break;
			case FM_ERROR_CANCELLED:
				Error("Conex�o cancelada !");
				break;
			default:
				Error("Ocorreu um erro desconhecido ao conectar !");
				break;
		}
		return FALSE;
  }

	// erase whole device except bootloader
  presults = fm_erase(FM_BLOCKS, 0x3FFFFFFF, 1, EraseProgress, 0, NULL);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_CMD:
				Error("Comando inv�lido enviado ao apagar a mem�ria flash !");
				break;
			case FM_ERROR_INVALID_PARAMS:
				Error("Parametros inv�lidos ao apagar a mem�ria flash !");
				break;
			case FM_ERROR_CONNECT:
				Error("N�o foi poss�vel conectar para apagar a mem�ria flash !");
				break;
			case FM_ERROR_UNSUPPORTED:
				Error("Limpeza da mem�ria flash n�o suportado !");
			  break;
			case FM_ERROR_CANCELLED:
				Error("Limpeza da mem�ria flash cancelada !");
				break;
			case FM_ERROR_OPEN:
				Error("N�o foi poss�vel abrir o arquivo .hex ao apagar a mem�ria flash !");
				break;
			default:
				Error("Ocorreu um erro desconhecido ao apagar a mem�ria flash !");
				break;
		}
  	fm_disconnect();
		return FALSE;
	}

	// program device
	presults = fm_program(hexFile, 0, 0, 0, 1, NULL, NULL, 0, FM_NORMAL, FM_PROGOPT_NONE, ProgramProgress, 0);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_PROGRAM:
				Error("Erro durante a grava��o do programa !");
				break;
			case FM_ERROR_HEX_CHECKSUM:
				Error("Erro de checksum no arquivo de programa (.hex) !");
				break;
			case FM_ERROR_CHECKSUMS:
				Error("Erro de checksum ao gravar o programa !");
				break;
			case FM_ERROR_GENERATE_CHECKSUMS:
				Error("Erro ao gerar checksum durante a grava��o do programa !");
				break;
			case FM_ERROR_FILL:
				Error("Erro ao preencher a mem�ria flash !");
				break;
			case FM_ERROR_OPEN:
				Error("Erro ao abrir o arquivo de programa (.hex) !");
				break;
			case FM_ERROR_CONNECT:
				Error("N�o foi poss�vel conectar para gravar o programa !");
				break;
			case FM_ERROR_CMD:
				Error("Comando inv�lido durante a grava��o do programa !");
				break;
			case FM_ERROR_ALLOCATION:
				Error("N�o foi poss�vel alocar mem�ria durante a grava��o do programa !");
				break;
			case FM_ERROR_CANCELLED:
				Error("Grava��o do programa cancelada !");
				break;
			case FM_ERROR_JIT:
				Error("Erro na execu��o do m�dulo JIT !");
				break;
			case FM_ERROR_UNSUPPORTED:
				Error("N�o suporta grava��o do programa !");
			  break;
			default:
				Error("Ocorreu um erro desconhecido ao gravar o programa !");
				break;
		}
	  fm_disconnect();
      return FALSE;
	}

	// verify device
	presults = fm_verify(hexFile, 0, VerifyProgress, 0);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_VERIFY:
				Error("Erro na verifica��o !");
				break;
			case FM_ERROR_OPEN:
				Error("Erro ao abrir o arquivo de programa (.hex) !");
				break;
			case FM_ERROR_CONNECT:
				Error("N�o foi poss�vel conectar para gravar o programa !");
				break;
			case FM_ERROR_CANCELLED:
				Error("Grava��o do programa cancelada !");
				break;
			case FM_ERROR_PORT:
				Error("Erro ao conectar na porta COM%d !", ComPort);
				break;
			case FM_ERROR_CMD:
				Error("Enviado comando inv�lido (verificando) !");
				break;
			case FM_ERROR_UNSUPPORTED:
				Error("N�o suporta verifica��o !");
				break;
			default:
				Error("Ocorreu um erro desconhecido ao verificar a grava��o do programa !");
				break;
		}
	  fm_disconnect();
      return FALSE;
	}

	// disconnect from device
	fm_disconnect();

	// tell user we are done
	StatusBarSetText(0, "Grava��o conclu�da com sucesso");

	ProgramSuccessfulMessage("Grava��o conclu�da com sucesso !");

	return TRUE;
}