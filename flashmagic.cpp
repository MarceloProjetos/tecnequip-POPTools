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
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Preparando para gravar... %d%% concluído !", (unsigned int)(((float)value / (float)29) * 100));
	else
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Preparação concluída com sucesso !");

	StatusBarSetText(0, text);

	return 1;
}

int VerifyProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	char text[100];

	if (value > totalWrite)
		totalWrite = value;

	if (value)
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Verificando... %d%% concluído !", (unsigned int)(((float)value / (float)totalWrite) * 100));
	else
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Verificação concluída com sucesso !");

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
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Gravando... %d%% concluído !", 100 - (unsigned int)(((float)value / (float)totalWrite) * 100));
	else
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Gravação concluída com sucesso !");

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

	StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Atualizando o relógio RTC da POP...");

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
			StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "Atualizando o relógio RTC da POP...[%d]", trycount);
			StatusBarSetText(0, text);
		} while(trycount < 10);
	}

	if (reply.ExceptionCode != MODBUS_EXCEPTION_NONE)
		MessageBox(NULL, _("A Data/Hora do relógio da POP não foi atualizada.\n\nNão foi possível verificar se a data/hora foi atualizada corretamente por que a leitura do RTC retornou 0 bytes !.\n\nAplicações que dependem do relogio RTC da placa talvez não funcionem corretamente."), _("Relógio da POP"), MB_OK | MB_ICONEXCLAMATION);

	CloseCOMPort();

	// tell user we are done
	StatusBarSetText(0, "Gravação concluída com sucesso");

	ProgramSuccessfulMessage("Gravação concluída com sucesso !");

	return TRUE;
}