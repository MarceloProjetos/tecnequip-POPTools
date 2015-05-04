#include "poptools.h"
#include "flashmagicarmcortex.h"

static unsigned long totalWrite = 0;

ProgressStatus ps;

// erase progress function
// return 1 to continue erase
int EraseProgress(int status, unsigned long value, unsigned long value2, void *callbackparam)
{
	TCHAR text[100];

	if (value) {
		ps.iStagePercent = (unsigned int)(((float)value / (float)29) * 100);
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Preparando para gravar... %d%% conclu�do !"), ps.iStagePercent);
	} else {
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Prepara��o conclu�da com sucesso !"));
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
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Verificando... %d%% conclu�do !"), ps.iStagePercent);
	} else {
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Verifica��o conclu�da com sucesso !"));
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
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Gravando... %d%% conclu�do !"), ps.iStagePercent);
	} else {
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Grava��o conclu�da com sucesso !"));
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
	LadderSettingsDetails settings = ladder->getSettingsDetails();
	eModelPLC plcModel             = ladder->getSettingsGeneral().model;

	if(plcModel != eModelPLC_POP7) {
		BaudRate = 115200;
		hexFile = "c:\\lpc4078.hex";
	}

	// use standard timeouts
	fm_set_default_timeouts();

	// generate a debug file containing all ISP commands
	//fm_select_debug_mode(FM_DEBUG_MODE_ON, "fmtest.fmd");

	options.osc            = 12.000;
	sprintf_s(options.comportname, FM_MAXCOMPORTNAMELEN, "COM%d", ComPort);
	options.baudrate       = BaudRate;
	options.selecteddevice = (plcModel == eModelPLC_POP7) ? FM_LPC1768 : FM_LPC4078;
	options.highspeed      = 0;
	options.clocks         = 0;
	options.halfduplex     = 0;
	options.hwconfig       = FM_HWBOOTEXECRTS;
	options.hwt1           = 50;
	options.hwt2           = 100;
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
				sprintf(text, _("N�o foi poss�vel conectar com baudrate %i !"), BaudRate);
				break;
			case FM_ERROR_INVALID_PARAMS:
				sprintf(text, _("Parametros de conex�o inv�lidos !"));
				break;
			case FM_ERROR_CMD:
				sprintf(text, _("Enviado comando inv�lido (conectando) !"));
				break;
			case FM_ERROR_CANCELLED:
				sprintf(text, _("Conex�o cancelada !"));
				break;
			default:
				sprintf(text, _("Ocorreu um erro desconhecido ao conectar !"));
			break;
		}
		goto FlashProgramError;
	}

	ps.szMsg = _("Preparando...");
	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, _("Opera��o cancelada!"));
		goto FlashProgramError;
	}

	// erase whole device except bootloader
	int blocks[FM_MAX_BLOCKS];
	memset(blocks, 0, sizeof(int) * FM_MAX_BLOCKS);
	presults = fm_erase(FM_DEVICE, blocks, 1, EraseProgress, 0, NULL);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_CMD:
				sprintf(text, _("Comando inv�lido enviado ao apagar a mem�ria flash !"));
				break;
			case FM_ERROR_INVALID_PARAMS:
				sprintf(text, _("Parametros inv�lidos ao apagar a mem�ria flash !"));
				break;
			case FM_ERROR_CONNECT:
				sprintf(text, _("N�o foi poss�vel conectar para apagar a mem�ria flash !"));
				break;
			case FM_ERROR_UNSUPPORTED:
				sprintf(text, _("Limpeza da mem�ria flash n�o suportado !"));
			  break;
			case FM_ERROR_CANCELLED:
				sprintf(text, _("Limpeza da mem�ria flash cancelada !"));
				break;
			case FM_ERROR_OPEN:
				sprintf(text, _("N�o foi poss�vel abrir o arquivo .hex ao apagar a mem�ria flash !"));
				break;
			default:
				sprintf(text, _("Ocorreu um erro desconhecido ao apagar a mem�ria flash !"));
				break;
		}
  		fm_disconnect();
		goto FlashProgramError;
	}

	ps.iCurrentStage = PROGRESS_STAGE_WRITING;
	ps.iStagePercent = 0;
	ps.szMsg = _("Gravando...");
	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, _("Opera��o cancelada!"));
		goto FlashProgramError;
	}

	// program device
	presults = fm_program(hexFile, 0, 0, 0, 1, NULL, NULL, 0, FM_NORMAL, FM_PROGOPT_NONE, ProgramProgress, 0);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_PROGRAM:
				sprintf(text, _("Erro durante a grava��o do programa !"));
				break;
			case FM_ERROR_HEX_CHECKSUM:
				sprintf(text, _("Erro de checksum no arquivo de programa (.hex) !"));
				break;
			case FM_ERROR_CHECKSUMS:
				sprintf(text, _("Erro de checksum ao gravar o programa !"));
				break;
			case FM_ERROR_GENERATE_CHECKSUMS:
				sprintf(text, _("Erro ao gerar checksum durante a grava��o do programa !"));
				break;
			case FM_ERROR_FILL:
				sprintf(text, _("Erro ao preencher a mem�ria flash !"));
				break;
			case FM_ERROR_OPEN:
				sprintf(text, _("Erro ao abrir o arquivo de programa (.hex) !"));
				break;
			case FM_ERROR_CONNECT:
				sprintf(text, _("N�o foi poss�vel conectar para gravar o programa !"));
				break;
			case FM_ERROR_CMD:
				sprintf(text, _("Comando inv�lido durante a grava��o do programa !"));
				break;
			case FM_ERROR_ALLOCATION:
				sprintf(text, _("N�o foi poss�vel alocar mem�ria durante a grava��o do programa !"));
				break;
			case FM_ERROR_CANCELLED:
				sprintf(text, _("Grava��o do programa cancelada !"));
				break;
			case FM_ERROR_JIT:
				sprintf(text, _("Erro na execu��o do m�dulo JIT !"));
				break;
			case FM_ERROR_UNSUPPORTED:
				sprintf(text, _("N�o suporta grava��o do programa !"));
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
	ps.szMsg = _("Verificando...");
	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, _("Opera��o cancelada!"));
		goto FlashProgramError;
	}

	// verify device
	presults = fm_verify(hexFile, 0, VerifyProgress, 0);
	if (presults->result != FM_OK)
	{
		switch (presults->result)
		{
			case FM_ERROR_VERIFY:
				sprintf(text, _("Erro na verifica��o !"));
				break;
			case FM_ERROR_OPEN:
				sprintf(text, _("Erro ao abrir o arquivo de programa (.hex) !"));
				break;
			case FM_ERROR_CONNECT:
				sprintf(text, _("N�o foi poss�vel conectar para gravar o programa !"));
				break;
			case FM_ERROR_CANCELLED:
				sprintf(text, _("Verifica��o do programa cancelada !"));
				break;
			case FM_ERROR_PORT:
				sprintf(text, _("Erro ao conectar na porta COM%d !"), ComPort);
				break;
			case FM_ERROR_CMD:
				sprintf(text, _("Enviado comando inv�lido (verificando) !"));
				break;
			case FM_ERROR_UNSUPPORTED:
				sprintf(text, _("N�o suporta verifica��o !"));
				break;
			default:
				sprintf(text, _("Ocorreu um erro desconhecido ao verificar a grava��o do programa !"));
				break;
		}
		presults = fm_disconnect();
		goto FlashProgramError;
	}

	ps.iCurrentStage = PROGRESS_STAGE_FINISHING;
	ps.iStagePercent = 0;
	ps.szMsg = _("Desconectando...");
	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, _("Opera��o cancelada!"));
		goto FlashProgramError;
	}

	// disconnect from device
	presults = fm_disconnect();
	if (presults->result != FM_OK)
	{
		sprintf(text, _("Ocorreu um erro desconhecido ao desconectar !"));
		goto FlashProgramError;
	}

	StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), _("Atualizando o rel�gio RTC da POP..."));

	ps.iCurrentStage = PROGRESS_STAGE_FINISHING;
	ps.iStagePercent = 0;
	ps.szMsg = text;

	if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
		strcpy(text, _("Opera��o cancelada!"));
		goto FlashProgramError;
	}
	StatusBarSetText(0, text);

	for(i=0; i < 200; i++) {
		Sleep(25);
		if(UpdateProgressWindow(NULL) == PROGRESS_STATUS_CANCEL) {
			strcpy(text, _("Opera��o cancelada!"));
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
		StringCchPrintf(text, sizeof(text) / sizeof(TCHAR), "%s [%d]", _("Atualizando o rel�gio RTC da POP..."), trycount);
		ps.szMsg = text;

		if(UpdateProgressWindow(&ps) == PROGRESS_STATUS_CANCEL) {
			break;
		}
		StatusBarSetText(0, text);
	} while(trycount < 10);

	if (!rtcok)
		ladder->ShowDialog(eDialogType_Message, false, _("Rel�gio da POP"), _("A Data/Hora do rel�gio da POP n�o foi atualizada.\n\nN�o foi poss�vel verificar se a data/hora foi atualizada corretamente por que a leitura do RTC retornou 0 bytes !.\n\nAplica��es que dependem do relogio RTC da placa talvez n�o funcionem corretamente."));

	// tell user we are done
	ps.iCurrentStage = PROGRESS_STAGE_FINISHED;
	ps.szMsg = _("Grava��o conclu�da com sucesso");

	settings.ProgramDate = time(NULL);
	ladder->setSettingsDetails(settings);

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