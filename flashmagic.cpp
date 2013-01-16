#include "poptools.h"
#include "flashmagicarmcortex.h"

static unsigned long totalWrite = 0;

ProgressStatus ps;

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

	if (value) {
		ps.iStagePercent = (unsigned int)(((float)value / (float)29) * 100);
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Preparando para gravar... %d%% concluído !"), ps.iStagePercent);
	} else {
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Preparação concluída com sucesso !"));
	}

	ps.szMsg = text;

	StatusBarSetText(0, text);

	return UpdateProgressWindow(&ps) != PROGRESS_STATUS_CANCEL;
}

int VerifyProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	char text[100];

	if (value > totalWrite)
		totalWrite = value;

	if (value) {
		ps.iStagePercent = (unsigned int)(((float)value / (float)totalWrite) * 100);
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Verificando... %d%% concluído !"), ps.iStagePercent);
	} else {
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Verificação concluída com sucesso !"));
	}

	ps.szMsg = text;

	StatusBarSetText(0, text);

	return UpdateProgressWindow(&ps) != PROGRESS_STATUS_CANCEL;
}

// program progress function
// return 1 to continue programming
int ProgramProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	char text[100];

	if (value > totalWrite)
		totalWrite = value;

	if (value) {
		ps.iStagePercent = 100 - (unsigned int)(((float)value / (float)totalWrite) * 100);
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Gravando... %d%% concluído !"), ps.iStagePercent);
	} else {
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Gravação concluída com sucesso !"));
	}

	ps.szMsg = text;

	StatusBarSetText(0, text);

	return UpdateProgressWindow(&ps) != PROGRESS_STATUS_CANCEL;
}

BOOL FlashProgram(char *hexFile, int ComPort, long BaudRate)
{
	int i;
	char text[100];
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

	ps.iCurrentStage = PROGRESS_STAGE_ERASING;
	ps.bStageState   = PROGRESS_STAGE_STATE_OK;
	ps.iStagePercent = 0;

	// connect to the device on the specified com port using the specified baud rate
	presults = fm_connect(&options, sizeof(options));
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
		  case FM_ERROR_PORT:
				sprintf(text, _("Erro ao conectar na porta COM%d !"), ComPort);
			  break;
			case FM_ERROR_BAUDRATE:
				sprintf(text, _("Não foi possível conectar com baudrate %i !"), BaudRate);
				break;
			case FM_ERROR_INVALID_PARAMS:
				sprintf(text, _("Parametros de conexão inválidos !"));
				break;
			case FM_ERROR_CMD:
				sprintf(text, _("Enviado comando inválido (conectando) !"));
				break;
			case FM_ERROR_CANCELLED:
				sprintf(text, _("Conexão cancelada !"));
				break;
			default:
				sprintf(text, _("Ocorreu um erro desconhecido ao conectar !"));
			break;
		}
		goto FlashProgramError;
	}

	ps.szMsg = "Preparando...";
	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, "Operação cancelada!");
		goto FlashProgramError;
	}

	// erase whole device except bootloader
	presults = fm_erase(FM_BLOCKS, 0x3FFFFFFF, 1, EraseProgress, 0, NULL);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_CMD:
				sprintf(text, _("Comando inválido enviado ao apagar a memória flash !"));
				break;
			case FM_ERROR_INVALID_PARAMS:
				sprintf(text, _("Parametros inválidos ao apagar a memória flash !"));
				break;
			case FM_ERROR_CONNECT:
				sprintf(text, _("Não foi possível conectar para apagar a memória flash !"));
				break;
			case FM_ERROR_UNSUPPORTED:
				sprintf(text, _("Limpeza da memória flash não suportado !"));
			  break;
			case FM_ERROR_CANCELLED:
				sprintf(text, _("Limpeza da memória flash cancelada !"));
				break;
			case FM_ERROR_OPEN:
				sprintf(text, _("Não foi possível abrir o arquivo .hex ao apagar a memória flash !"));
				break;
			default:
				sprintf(text, _("Ocorreu um erro desconhecido ao apagar a memória flash !"));
				break;
		}
  		fm_disconnect();
		goto FlashProgramError;
	}

	ps.iCurrentStage = PROGRESS_STAGE_WRITING;
	ps.iStagePercent = 0;
	ps.szMsg = "Gravando...";
	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, "Operação cancelada!");
		goto FlashProgramError;
	}

	// program device
	presults = fm_program(hexFile, 0, 0, 0, 1, NULL, NULL, 0, FM_NORMAL, FM_PROGOPT_NONE, ProgramProgress, 0);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_PROGRAM:
				sprintf(text, _("Erro durante a gravação do programa !"));
				break;
			case FM_ERROR_HEX_CHECKSUM:
				sprintf(text, _("Erro de checksum no arquivo de programa (.hex) !"));
				break;
			case FM_ERROR_CHECKSUMS:
				sprintf(text, _("Erro de checksum ao gravar o programa !"));
				break;
			case FM_ERROR_GENERATE_CHECKSUMS:
				sprintf(text, _("Erro ao gerar checksum durante a gravação do programa !"));
				break;
			case FM_ERROR_FILL:
				sprintf(text, _("Erro ao preencher a memória flash !"));
				break;
			case FM_ERROR_OPEN:
				sprintf(text, _("Erro ao abrir o arquivo de programa (.hex) !"));
				break;
			case FM_ERROR_CONNECT:
				sprintf(text, _("Não foi possível conectar para gravar o programa !"));
				break;
			case FM_ERROR_CMD:
				sprintf(text, _("Comando inválido durante a gravação do programa !"));
				break;
			case FM_ERROR_ALLOCATION:
				sprintf(text, _("Não foi possível alocar memória durante a gravação do programa !"));
				break;
			case FM_ERROR_CANCELLED:
				sprintf(text, _("Gravação do programa cancelada !"));
				break;
			case FM_ERROR_JIT:
				sprintf(text, _("Erro na execução do módulo JIT !"));
				break;
			case FM_ERROR_UNSUPPORTED:
				sprintf(text, _("Não suporta gravação do programa !"));
			  break;
			default:
				sprintf(text, _("Ocorreu um erro desconhecido ao gravar o programa !"));
				break;
		}
	  fm_disconnect();
		goto FlashProgramError;
	}

	ps.iCurrentStage = PROGRESS_STAGE_VERIFYING;
	ps.iStagePercent = 0;
	ps.szMsg = "Verificando...";
	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, "Operação cancelada!");
		goto FlashProgramError;
	}

	// verify device
	presults = fm_verify(hexFile, 0, VerifyProgress, 0);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_VERIFY:
				sprintf(text, _("Erro na verificação !"));
				break;
			case FM_ERROR_OPEN:
				sprintf(text, _("Erro ao abrir o arquivo de programa (.hex) !"));
				break;
			case FM_ERROR_CONNECT:
				sprintf(text, _("Não foi possível conectar para gravar o programa !"));
				break;
			case FM_ERROR_CANCELLED:
				sprintf(text, _("Verificação do programa cancelada !"));
				break;
			case FM_ERROR_PORT:
				sprintf(text, _("Erro ao conectar na porta COM%d !"), ComPort);
				break;
			case FM_ERROR_CMD:
				sprintf(text, _("Enviado comando inválido (verificando) !"));
				break;
			case FM_ERROR_UNSUPPORTED:
				sprintf(text, _("Não suporta verificação !"));
				break;
			default:
				sprintf(text, _("Ocorreu um erro desconhecido ao verificar a gravação do programa !"));
				break;
		}
		presults = fm_disconnect();
		goto FlashProgramError;
	}

	ps.iCurrentStage = PROGRESS_STAGE_FINISHING;
	ps.iStagePercent = 0;
	ps.szMsg = "Desconectando...";
	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, "Operação cancelada!");
		goto FlashProgramError;
	}

	// disconnect from device
	presults = fm_disconnect();
	if (presults->result != FM_OK)
	{
		sprintf(text, _("Ocorreu um erro desconhecido ao desconectar !"));
		goto FlashProgramError;
	}

	StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Atualizando o relógio RTC da POP..."));

	ps.iCurrentStage = PROGRESS_STAGE_FINISHING;
	ps.iStagePercent = 0;
	ps.szMsg = text;

	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, "Operação cancelada!");
		goto FlashProgramError;
	}
	StatusBarSetText(0, text);

	for(i=0; i < 200; i++) {
		Sleep(25);
		if(UpdateProgressWindow(NULL) == PROGRESS_STATUS_CANCEL) {
			strcpy(text, "Operação cancelada!");
			goto FlashProgramError;
		}
	}

	bool rtcok = false;
	int trycount = 0;

	do {
		if(USB_SetDateTime(NULL)) {
			rtcok = true;
			break;
		}

		Sleep(1000);

		trycount++;
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "%s [%d]", _("Atualizando o relógio RTC da POP..."), trycount);
		ps.szMsg = text;

		if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
			break;
		}
		StatusBarSetText(0, text);
	} while(trycount < 10);

	if (!rtcok)
		MessageBox(NULL, _("A Data/Hora do relógio da POP não foi atualizada.\n\nNão foi possível verificar se a data/hora foi atualizada corretamente por que a leitura do RTC retornou 0 bytes !.\n\nAplicações que dependem do relogio RTC da placa talvez não funcionem corretamente."), _("Relógio da POP"), MB_OK | MB_ICONEXCLAMATION);

	// tell user we are done
	ps.iCurrentStage = PROGRESS_STAGE_FINISHED;
	ps.szMsg = _("Gravação concluída com sucesso");

	UpdateProgressWindow(&ps);
	StatusBarSetText(0, ps.szMsg);

	// Sound rec_ok save in Sounds/rec_ok.wav
	PlaySound(MAKEINTRESOURCE(IDB_WAVE_REC_OK),GetModuleHandle(NULL), SND_RESOURCE);
	
	return TRUE;

FlashProgramError:
	// Sound rec_erro save in Sounds/rec_erro.wav
	PlaySound(MAKEINTRESOURCE(IDB_WAVE_REC_ERRO),GetModuleHandle(NULL), SND_RESOURCE);

	ps.bStageState = PROGRESS_STAGE_STATE_FAILED;
	ps.szMsg = text;

	UpdateProgressWindow(&ps);
	StatusBarSetText(0, ps.szMsg);

	return FALSE;
}