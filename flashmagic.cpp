#include <windows.h>
#include <Shlobj.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <time.h>

#include "ldmicro.h"
#include "intcode.h"

#include "flashmagicarmcortex.h"

#include <Strsafe.h>

static unsigned long totalWrite = 0;

void ShowError(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    //ExitProcess(dw); 
}

// erase progress function
// return 1 to continue erase
int EraseProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	TCHAR text[100];

	if (value)
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Preparando para gravar... %d%% conclu�do !", (unsigned int)(((float)value / (float)29) * 100));
	else
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Prepara��o conclu�da com sucesso !");

	StatusBarSetText(0, text);

	return 1;
}

int VerifyProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	char text[100];

	if (value > totalWrite)
		totalWrite = value;

	if (value)
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Verificando... %d%% conclu�do !", (unsigned int)(((float)value / (float)totalWrite) * 100));
	else
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Verifica��o conclu�da com sucesso !");

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
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Gravando... %d%% conclu�do !", 100 - (unsigned int)(((float)value / (float)totalWrite) * 100));
	else
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Grava��o conclu�da com sucesso !");

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

	totalWrite = 0;

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
		presults = fm_disconnect();
		return FALSE;
	}

	// disconnect from device
	presults = fm_disconnect();
	if (presults->result != FM_OK)
	{
		Error("Ocorreu um erro desconhecido ao desconectar !");
		return FALSE;
	}

	char text[100];

	StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Atualizando o rel�gio RTC da POP...");

	StatusBarSetText(0, text);

	Sleep(5000);

	struct MODBUS_Reply reply;

	memset(&reply, 0, sizeof(reply));
	reply.ExceptionCode = MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE;

	if(OpenCOMPort(POPSettings.COMPortFlash, 115200, 8, NOPARITY, ONESTOPBIT)) {
		int trycount = 0;
		BYTE cWriteBuffer[16];
		MODBUS_FCD_Data mbdata1, mbdata2;

		time_t rawtime;
		struct tm * t;

		time ( &rawtime );
		t = localtime ( &rawtime );

		t->tm_year += 1900;
		t->tm_mon++;
		t->tm_sec = t->tm_sec > 59 ? 59 : t->tm_sec;

		memset(cWriteBuffer, 0, sizeof(cWriteBuffer));

		memcpy(&cWriteBuffer[0], &t->tm_mday, 1);
		memcpy(&cWriteBuffer[1], &t->tm_mon , 1);
		memcpy(&cWriteBuffer[2], &t->tm_year, 2);
		memcpy(&cWriteBuffer[4], &t->tm_hour, 1);
		memcpy(&cWriteBuffer[5], &t->tm_min , 1);
		memcpy(&cWriteBuffer[6], &t->tm_sec , 1);
		memcpy(&cWriteBuffer[7], &t->tm_wday, 1);
		memcpy(&cWriteBuffer[8], &t->tm_yday, 2);

		mbdata1.write_multiple_registers.quant = 3;
		mbdata1.write_multiple_registers.size  = mbdata1.write_multiple_registers.quant*2;
		mbdata1.write_multiple_registers.start = 1;
		mbdata1.write_multiple_registers.val   = cWriteBuffer;

		mbdata2.write_multiple_registers.quant = 2;
		mbdata2.write_multiple_registers.size  = mbdata2.write_multiple_registers.quant*2;
		mbdata2.write_multiple_registers.start = 4;
		mbdata2.write_multiple_registers.val   = cWriteBuffer+6;

		do {
			reply = Modbus_RTU_Send(&MBDev_Serial, 0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, &mbdata1);
			if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
				reply = Modbus_RTU_Send(&MBDev_Serial, 0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, &mbdata2);
				if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
					break;
				}
			}

			Sleep(1000);

			trycount++;
			StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Atualizando o rel�gio RTC da POP...[%d]", trycount);
			StatusBarSetText(0, text);
		} while(trycount < 10);
	}

	if (reply.ExceptionCode != MODBUS_EXCEPTION_NONE)
		MessageBox(NULL, _("A Data/Hora do rel�gio da POP n�o foi atualizada.\n\nN�o foi poss�vel verificar se a data/hora foi atualizada corretamente por que a leitura do RTC retornou 0 bytes !.\n\nAplica��es que dependem do relogio RTC da placa talvez n�o funcionem corretamente."), _("Rel�gio da POP"), MB_OK | MB_ICONEXCLAMATION);

	CloseCOMPort();

	// tell user we are done
	StatusBarSetText(0, "Grava��o conclu�da com sucesso");

	ProgramSuccessfulMessage("Grava��o conclu�da com sucesso !");

	return TRUE;
}