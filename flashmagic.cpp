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
		sprintf(text, "Preparando para gravar... %d%% concluído !", (unsigned int)(((float)value / (float)29) * 100));
	else
		sprintf(text, "Preparação concluída com sucesso !");

	StatusBarSetText(0, text);

	return 1;
}

int VerifyProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	char text[100];

	if (value > totalWrite)
		totalWrite = value;

	if (value)
		sprintf(text, "Verificando... %d%% concluído !", (unsigned int)(((float)value / (float)totalWrite) * 100));
	else
		sprintf(text, "Verificação concluída com sucesso !");

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
		sprintf(text, "Gravando... %d%% concluído !", 100 - (unsigned int)(((float)value / (float)totalWrite) * 100));
	else
		sprintf(text, "Gravação concluída com sucesso !");

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
				Error("Não foi possível conectar com baudrate %i !", BaudRate);
				break;
			case FM_ERROR_INVALID_PARAMS:
				Error("Parametros de conexão inválidos !");
				break;
			case FM_ERROR_CMD:
				Error("Enviado comando inválido (conectando) !");
				break;
			case FM_ERROR_CANCELLED:
				Error("Conexão cancelada !");
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
				Error("Comando inválido enviado ao apagar a memória flash !");
				break;
			case FM_ERROR_INVALID_PARAMS:
				Error("Parametros inválidos ao apagar a memória flash !");
				break;
			case FM_ERROR_CONNECT:
				Error("Não foi possível conectar para apagar a memória flash !");
				break;
			case FM_ERROR_UNSUPPORTED:
				Error("Limpeza da memória flash não suportado !");
			  break;
			case FM_ERROR_CANCELLED:
				Error("Limpeza da memória flash cancelada !");
				break;
			case FM_ERROR_OPEN:
				Error("Não foi possível abrir o arquivo .hex ao apagar a memória flash !");
				break;
			default:
				Error("Ocorreu um erro desconhecido ao apagar a memória flash !");
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
				Error("Erro durante a gravação do programa !");
				break;
			case FM_ERROR_HEX_CHECKSUM:
				Error("Erro de checksum no arquivo de programa (.hex) !");
				break;
			case FM_ERROR_CHECKSUMS:
				Error("Erro de checksum ao gravar o programa !");
				break;
			case FM_ERROR_GENERATE_CHECKSUMS:
				Error("Erro ao gerar checksum durante a gravação do programa !");
				break;
			case FM_ERROR_FILL:
				Error("Erro ao preencher a memória flash !");
				break;
			case FM_ERROR_OPEN:
				Error("Erro ao abrir o arquivo de programa (.hex) !");
				break;
			case FM_ERROR_CONNECT:
				Error("Não foi possível conectar para gravar o programa !");
				break;
			case FM_ERROR_CMD:
				Error("Comando inválido durante a gravação do programa !");
				break;
			case FM_ERROR_ALLOCATION:
				Error("Não foi possível alocar memória durante a gravação do programa !");
				break;
			case FM_ERROR_CANCELLED:
				Error("Gravação do programa cancelada !");
				break;
			case FM_ERROR_JIT:
				Error("Erro na execução do módulo JIT !");
				break;
			case FM_ERROR_UNSUPPORTED:
				Error("Não suporta gravação do programa !");
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
				Error("Erro na verificação !");
				break;
			case FM_ERROR_OPEN:
				Error("Erro ao abrir o arquivo de programa (.hex) !");
				break;
			case FM_ERROR_CONNECT:
				Error("Não foi possível conectar para gravar o programa !");
				break;
			case FM_ERROR_CANCELLED:
				Error("Gravação do programa cancelada !");
				break;
			case FM_ERROR_PORT:
				Error("Erro ao conectar na porta COM%d !", ComPort);
				break;
			case FM_ERROR_CMD:
				Error("Enviado comando inválido (verificando) !");
				break;
			case FM_ERROR_UNSUPPORTED:
				Error("Não suporta verificação !");
				break;
			default:
				Error("Ocorreu um erro desconhecido ao verificar a gravação do programa !");
				break;
		}
	  fm_disconnect();
      return FALSE;
	}

	// disconnect from device
	fm_disconnect();

	// tell user we are done
	StatusBarSetText(0, "Gravação concluída com sucesso");

	ProgramSuccessfulMessage("Gravação concluída com sucesso !");

	return TRUE;
}