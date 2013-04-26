#include "poptools.h"
#include "mcutable.h"

extern void Draw_Init(void);
extern void Draw_Start(void);
extern void Draw_End(void);

HINSTANCE   Instance;
HWND        MainWindow;
HDC         Hdc;

CComModule _Module;

ID2D1Factory*			pD2DFactory;
IDWriteFactory*			pWriteFactory;
IWICImagingFactory *	pWICFactory;

SPLASH mysplash;

// parameters used to capture the mouse when implementing our totally non-
// general splitter control
static HHOOK       MouseHookHandle;
static int         MouseY;

// For the open/save dialog boxes
#define LDMICRO_PATTERN _("POPTools Projeto Ladder (*.ld)\0*.ld\0All files\0*\0\0")
char CurrentSaveFile[MAX_PATH];
BOOL ProgramChangedNotSaved = FALSE;

#define HEX_PATTERN  _("Intel Hex Files (*.hex)\0*.hex\0All files\0*\0\0")
#define C_PATTERN    _("Linguagem C (*.c)\0*.c\0Todos os Arquivos\0*\0\0")
#define INTERPRETED_PATTERN \
    "Interpretable Byte Code Files (*.int)\0*.int\0All Files\0*\0\0"
char CurrentCompileFile[MAX_PATH];

#define TXT_PATTERN  _("Arquivos de Texto (*.txt)\0*.txt\0Todos os Arquivos\0*\0\0")

// Internal flags available to the users.
char *InternalFlags[] = { "SerialReady", "SerialTimeout", "SerialAborted", "RampActive", "TcpReady", "TcpTimeout", "" };

// Internal variables available to the users.
char *InternalVars[][MAX_NAME_LEN] = { { "IncPerimRoda" , "IncPulsosVolta", "IncFatorCorr" , "AbsPerimRoda" , "AbsFatorCorr" , "" },
									   { "INC_Perimeter", "INC_PPR"       , "INC_Factor10k", "ABS_Perimeter", "ABS_Factor10k", "" } };

// Everything relating to the PLC's program, I/O configuration, processor
// choice, and so on--basically everything that would be saved in the
// project file.
PlcProgram Prog;

// Settings structure
Settings POPSettings;
XMLWrapper XmlSettings;

// Functions to show Task Dialogs. Task Dialog is a replacement for MessageBox since Windows Vista
int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
	TASKDIALOG_BUTTON *pButtons, TASKDIALOG_BUTTON *pRadioButtons, PCWSTR pszVerificationText, PCWSTR pszExpandedInformation,
	int *pnRadioButton, BOOL *pfVerificationFlagChecked)
{
	HRESULT hr;
	int iButton = 0;
	UINT cButtons = 0, cRadioButtons = 0;
	TASKDIALOGCONFIG tdConfig;

	tdConfig.cbSize     = sizeof(TASKDIALOGCONFIG);
	tdConfig.hwndParent = MainWindow;
	tdConfig.hInstance  = Instance;

	tdConfig.dwFlags = 0;
	if(pszVerificationText && pfVerificationFlagChecked && *pfVerificationFlagChecked)
		tdConfig.dwFlags |= TDF_VERIFICATION_FLAG_CHECKED;

	if(pButtons != NULL) {
		tdConfig.dwFlags |= TDF_USE_COMMAND_LINKS;
		for(cButtons=0; pButtons[cButtons].pszButtonText != NULL; cButtons++)
			;
	}

	if(pRadioButtons != NULL) {
		for(cRadioButtons=0; pRadioButtons[cRadioButtons].pszButtonText != NULL; cRadioButtons++)
			;
	}

	// TODO: Internacionalizar
	tdConfig.pszWindowTitle     = L"POPTools";
	tdConfig.pszMainIcon        = pszMainIcon;
	tdConfig.pszMainInstruction = pszMainInstruction;
	tdConfig.pszContent         = pszContent;

	tdConfig.dwCommonButtons = dwCommonButtons;
	tdConfig.cButtons        = cButtons;
	tdConfig.pButtons        = pButtons;
	tdConfig.nDefaultButton  = IDNO;
	tdConfig.cRadioButtons   = cRadioButtons;
	tdConfig.pRadioButtons   = pRadioButtons;

	tdConfig.pszVerificationText     = pszVerificationText;
	tdConfig.pszExpandedInformation  = pszExpandedInformation;
	// TODO: Internacionalizar
	tdConfig.pszExpandedControlText  = L"Mais informa��es";
	tdConfig.pszCollapsedControlText = L"Menos informa��es";
	tdConfig.hFooterIcon             = NULL;
	tdConfig.pszFooterIcon           = NULL;
	tdConfig.pszFooter               = NULL;

	tdConfig.pfCallback     = NULL;
	tdConfig.lpCallbackData = NULL;
	tdConfig.cxWidth        = 0;

	hr = TaskDialogIndirect(&tdConfig, &iButton, pnRadioButton, pfVerificationFlagChecked);
	return iButton;
}

int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons)
{
	return ShowTaskDialog(pszMainInstruction, pszContent, pszMainIcon, dwCommonButtons, NULL, NULL, NULL, NULL, NULL, NULL);
}

int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
	PCWSTR pszVerificationText, BOOL *pfVerificationFlagChecked)
{
	return ShowTaskDialog(pszMainInstruction, pszContent, pszMainIcon, dwCommonButtons, NULL, NULL, pszVerificationText, NULL, NULL, pfVerificationFlagChecked);
}

int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR *pszButtons)
{
	int ret;
	const unsigned int ID_OFFSET = 1000;
	unsigned int i, qty;
	TASKDIALOG_BUTTON *pButtons;

	if(pszButtons == NULL)
		return 0;

	for(qty=0; pszButtons[qty] != NULL; qty++)
		;

	pButtons = new TASKDIALOG_BUTTON[qty+1];

	for(i=0; i < qty; i++) {
		(pButtons+i)->nButtonID = i+ID_OFFSET+1;
		(pButtons+i)->pszButtonText = pszButtons[i];
	}
	(pButtons+i)->pszButtonText = NULL;

	ret = ShowTaskDialog(pszMainInstruction, pszContent, NULL, 0, pButtons, NULL, NULL, NULL, NULL, NULL);
	if(ret)
		ret -= ID_OFFSET;

	delete [] pButtons;

	return ret;
}

int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
	PCWSTR *pszRadioButtons, int *pnRadioButton)
{
	int ret;
	const unsigned int ID_OFFSET = 1000;
	unsigned int i, qty;
	TASKDIALOG_BUTTON *pRadioButtons;

	if(pszRadioButtons == NULL)
		return 0;

	for(qty=0; pszRadioButtons[qty] != NULL; qty++)
		;

	pRadioButtons = new TASKDIALOG_BUTTON[qty+1];

	for(i=0; i < qty; i++) {
		(pRadioButtons+i)->nButtonID = i+ID_OFFSET+1;
		(pRadioButtons+i)->pszButtonText = pszRadioButtons[i];
	}
	(pRadioButtons+i)->pszButtonText = NULL;

	ret = ShowTaskDialog(pszMainInstruction, pszContent, pszMainIcon, dwCommonButtons, NULL, pRadioButtons, NULL, NULL, pnRadioButton, NULL);
	if(ret)
		ret -= ID_OFFSET;

	delete [] pRadioButtons;

	return ret;
}

void UpdateRecentList(char *filename)
{
	unsigned int i;

	if(filename == NULL || !strlen(filename))
		return;

	// Search for first free position
	for(i=0; i<MAX_RECENT_ITEMS; i++) {
		if(!_stricmp(filename, POPSettings.recent_list[i])) // it is already in the list, break to put it to top
			break;
		else if(!strlen(POPSettings.recent_list[i])) // Empty item, break
			break;
	}

	if(i > MAX_RECENT_ITEMS-1)
		i = MAX_RECENT_ITEMS-1;

	while(i) {
		strcpy(POPSettings.recent_list[i], POPSettings.recent_list[i-1]);
		i--;
	}

	strcpy(POPSettings.recent_list[0], filename);
}

//-----------------------------------------------------------------------------
// Get a filename with a common dialog box and then save the program to that
// file and then set our default filename to that.
//-----------------------------------------------------------------------------
static BOOL SaveAsDialog(void)
{
	if(!Prog.settings.canSave) {
		CurrentSaveFile[0] = '\0';
	}

	FileDialogShow(SaveLadder, "ld", CurrentSaveFile);
	if(!strlen(CurrentSaveFile))
		return FALSE;

	if(!SaveProjectToFile(CurrentSaveFile)) {
        Error(_("Couldn't write to '%s'."), CurrentSaveFile);
        return FALSE;
    } else {
		UpdateRecentList(CurrentSaveFile);
        ProgramChangedNotSaved = FALSE;
        return TRUE;
    }
}

//-----------------------------------------------------------------------------
// Get a filename with a common dialog box and then export the program as
// an ASCII art drawing.
//-----------------------------------------------------------------------------
static void PrintDocument(void)
{
	char szTempPath[MAX_PATH];

	GetTempPath(sizeof(szTempPath), szTempPath);

	strcat(szTempPath, "POPTools\\");
	if (!CreateDirectory(szTempPath, NULL))
	{
		DWORD err = GetLastError();	
		if (err != ERROR_ALREADY_EXISTS) {
			Error(_("Erro durante a impress�o!"));
			return;
		}
	}

	strcat(szTempPath, "ld.txt");
	ExportDrawingAsText(szTempPath);

	PRINTDLG pd;

	// Initialize PRINTDLG
	ZeroMemory(&pd, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner   = MainWindow;
	pd.hDevMode    = NULL;     // Don't forget to free or store hDevMode.
	pd.hDevNames   = NULL;     // Don't forget to free or store hDevNames.
	pd.Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC; 
	pd.nCopies     = 1;
	pd.nFromPage   = 0xFFFF; 
	pd.nToPage     = 0xFFFF; 
	pd.nMinPage    = 1; 
	pd.nMaxPage    = 0xFFFF; 

	if (PrintDlg(&pd)==TRUE) {
		DWORD bytes_written;
		DOC_INFO_1 docinfo1;
		LPDEVMODE  dm = (LPDEVMODE )GlobalLock(pd.hDevMode);
		LPDEVNAMES dn = (LPDEVNAMES)GlobalLock(pd.hDevNames);
		HANDLE print_handle = NULL;
		FILE *file_handle;
		char buf[1024];

		OpenPrinter((char *)dm->dmDeviceName, &print_handle, NULL);
		if(print_handle == NULL){
			Error(_("Erro abrindo impressora!\n"));
		} else {
			file_handle = fopen(szTempPath, "r");
			if(file_handle == NULL) {
				Error(_("Erro iniciando impress�o!\n"));
			} else {
				docinfo1.pDocName = "POPTools";
				docinfo1.pOutputFile = NULL;
				docinfo1.pDatatype = "text";

				StartDocPrinter(print_handle, 1, (LPBYTE)&docinfo1);
				StartPagePrinter(print_handle);

				while(fgets(buf, sizeof(buf), file_handle) != NULL) {
					WritePrinter(print_handle, buf, strlen(buf), &bytes_written);
				}

				EndPagePrinter(print_handle);
				EndDocPrinter(print_handle);

				fclose(file_handle);
			}

			ClosePrinter(print_handle);
		}

		// Free DEVMODE and DEVNAMES
		GlobalFree(pd.hDevMode);
		GlobalFree(pd.hDevNames);

		// Delete DC when done.
		DeleteDC(pd.hDC);
	}
}

//-----------------------------------------------------------------------------
// Get a filename with a common dialog box and then export the program as
// an ASCII art drawing.
//-----------------------------------------------------------------------------
static void ExportDialog(void)
{
    char exportFile[MAX_PATH];

	if(strlen(CurrentSaveFile) != 0) {
		strcpy(exportFile, CurrentSaveFile);
		ChangeFileExtension(exportFile, "txt");
	} else {
	    exportFile[0] = '\0';
	}

	FileDialogShow(SaveText, "txt", exportFile);
	if(!strlen(exportFile))
		return;
    ExportDrawingAsText(exportFile);
}

//-----------------------------------------------------------------------------
// Get a filename with a common dialog box and then export the program as
// an ASCII art drawing.
//-----------------------------------------------------------------------------
static void SaveAsAnsiC(void)
{
    char exportFile[MAX_PATH];

	if(strlen(CurrentSaveFile) != 0) {
		strcpy(exportFile, CurrentSaveFile);
		ChangeFileExtension(exportFile, "c");
	} else {
	    exportFile[0] = '\0';
	}

	FileDialogShow(SaveC, "c", exportFile);
	if(!strlen(exportFile))
		return;

    if(GenerateIntermediateCode() && GenerateCFile(exportFile))
		MessageBox(MainWindow, _("Arquivo gerado com sucesso!"), _("Sucesso"), MB_OK);
}

//-----------------------------------------------------------------------------
// If we already have a filename, save the program to that. Otherwise same
// as Save As. Returns TRUE if it worked, else returns FALSE.
//-----------------------------------------------------------------------------
static BOOL SaveProgram(void)
{
    if(strlen(CurrentSaveFile) && Prog.settings.canSave) {
        if(!SaveProjectToFile(CurrentSaveFile)) {
            Error(_("Couldn't write to '%s'."), CurrentSaveFile);
            return FALSE;
        } else {
            ProgramChangedNotSaved = FALSE;
            return TRUE;
        }
    } else {
        return SaveAsDialog();
    }
}

//-----------------------------------------------------------------------------
// Functions to Create, Show and Control a window to give user the progress
// status of compile/program operations
//-----------------------------------------------------------------------------
static HWND hProgressWindow = NULL;
static HWND hCompileProgressBar;
static HWND hErasingProgressBar;
static HWND hWritingProgressBar;
static HWND hVerifyingProgressBar;
static HWND hCompileOKLabel;
static HWND hErasingOKLabel;
static HWND hWritingOKLabel;
static HWND hVerifyingOKLabel;
static HWND hMessageLabel;

void HideProgressWindow(void)
{
	if(hProgressWindow != NULL) {
	    DestroyWindow(hProgressWindow);
		hProgressWindow = NULL;
	    EnableWindow(MainWindow, TRUE);
	}
}

int UpdateProgressWindow(ProgressStatus *ps)
{
	static ProgressStatus last = { -1, -1, -1, NULL };
	char text[300];
	HWND hProgressBars  [] = { hCompileProgressBar, hErasingProgressBar, hWritingProgressBar, hVerifyingProgressBar };
	HWND hProgressLabels[] = { hCompileOKLabel    , hErasingOKLabel    , hWritingOKLabel    , hVerifyingOKLabel     };
	int i, iStages = sizeof(hProgressBars)/sizeof(&hProgressBars[0]);

	if(hProgressWindow == NULL) return PROGRESS_STATUS_DONE;

	if(ps != NULL) {
		if(ps->iCurrentStage != last.iCurrentStage) {
			for(i=last.iCurrentStage; i < ps->iCurrentStage && i < PROGRESS_STAGE_FINISHING; i++) {
				if(!i) {
					DWORD dwStyle = GetWindowLong(hProgressBars[i], GWL_STYLE);
					SetWindowLong(hProgressBars[i], GWL_STYLE, dwStyle & ~PBS_MARQUEE);
				}

				SendMessage   (hProgressBars  [i], PBM_SETPOS, 100, 0);
				Static_SetText(hProgressLabels[i], "OK");
			}
		}

		if(ps->iCurrentStage < PROGRESS_STAGE_FINISHING) {
			if(ps->iCurrentStage && ps->iStagePercent != last.iStagePercent) {
				SendMessage(hProgressBars[ps->iCurrentStage], PBM_SETPOS, ps->iStagePercent, 0);
			}

			if(ps->bStageState != PROGRESS_STAGE_STATE_OK && ps->bStageState != last.bStageState) {
				Static_SetText(hProgressLabels[ps->iCurrentStage], _("ERRO"));
				SendMessage(hProgressBars[ps->iCurrentStage], PBM_SETSTATE, PBST_ERROR, 0);
				SendMessage(hProgressBars[ps->iCurrentStage], PBM_SETPOS, ps->iStagePercent, 0);
				if(ps->iCurrentStage == PROGRESS_STAGE_COMPILING) {
					RECT rMain, rProgress, rButton;
					int offset = 200, width, height;

					// Resize Progress Window
					GetWindowRect(MainWindow     , &rMain);
					GetWindowRect(hProgressWindow, &rProgress);

					width  = rProgress.right  - rProgress.left;
					height = rProgress.bottom - rProgress.top + offset;

					rProgress.top  = rMain.top  + (rMain.bottom - rMain.top  - height)/2;
					rProgress.left = rMain.left + (rMain.right  - rMain.left - width )/2;

					MoveWindow(hProgressWindow, rProgress.left, rProgress.top, width, height, TRUE);

					// Move OK/Cancel Button
					GetWindowRect(CancelButton   , &rButton);
					GetClientRect(hProgressWindow, &rProgress);

					width  = rButton.right  - rButton.left;
					height = rButton.bottom - rButton.top;

					rButton.top  = rProgress.bottom - height - 5;
					rButton.left = (rProgress.right - width)/2;

					MoveWindow(CancelButton, rButton.left, rButton.top, width, height, TRUE);

					// Create the object that will hold the Log text
					HWND LogTextBox = CreateWindowEx(0, WC_EDIT, "",
						WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE,
						5, rButton.top - offset, rProgress.right - 10, offset - 5, hProgressWindow, NULL, Instance, NULL);
					NiceFont(LogTextBox);

					// Load log text into LogTextBox
					char *buf, linebuf[1024];
					int pos, first = 1;
					FILE *f = fopen(OutputLog, "r");

					if(f == NULL) {
						Edit_SetText(LogTextBox, _("Log indispon�vel"));
					} else {
						fseek(f, 0, SEEK_END);
						buf = (char *)malloc(ftell(f) + 100);
						buf[0] = 0;
						fseek(f, 0, SEEK_SET);

						while(fgets(linebuf, sizeof(linebuf), f)) {
							char *inbuf = strstr(linebuf, "ld.c");
							if(inbuf != NULL) {
								if(first) {
									first = 0;
								} else {
									pos = strlen(inbuf);
									if(inbuf[pos-1] == 10) {
										inbuf[pos-1] = 13; // CR
										inbuf[pos  ] = 10; // LF
										inbuf[pos+1] = 0;
									}
									strcat(buf, inbuf);
								}
							}
						}
						fclose(f);

						Edit_SetText(LogTextBox, buf);
	//					free(buf);
					}

					ShowWindow(LogTextBox, TRUE);
				}
			}
		}

		if((ps->iCurrentStage == PROGRESS_STAGE_FINISHED && ps->iCurrentStage != last.iCurrentStage) ||
			(ps->bStageState != PROGRESS_STAGE_STATE_OK && ps->bStageState != last.bStageState)) {
			DialogDone = FALSE;
			Button_Enable(CancelButton, TRUE);
			Button_SetText(CancelButton, _("OK"));
		}

		Static_GetText(hMessageLabel, text, sizeof(text));
		if(strcmp(ps->szMsg, text)) {
			Static_SetText(hMessageLabel, ps->szMsg);
		}

		last = *ps;
	}

    MSG msg;
    DWORD ret;

	while((ret = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))) 
	{
		if (IsDialogMessage(hProgressWindow, &msg)) 
			continue;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

	if(DialogDone) {
		Button_Enable(CancelButton, FALSE);
		Button_GetText(CancelButton, text, sizeof(text));
		if(!strcmp(text, _("OK"))) {
			return PROGRESS_STATUS_DONE;
		} else {
			return PROGRESS_STATUS_CANCEL;
		}
	}

	return PROGRESS_STATUS_OK;
}

void ShowProgressWindow(int mode)
{
	RECT rMain, rProgress;
	int offset = 0, width, height;
	HWND TextLabel;

	if(hProgressWindow != NULL) {
		HideProgressWindow();
	}

	// Creating Progress Window
	hProgressWindow = CreateWindowClient(0, "POPToolsDialog",
        _("Progress"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 360, 100, MainWindow, NULL, Instance, NULL);

	// Compile Progress Controls
    TextLabel = CreateWindowEx(0, WC_STATIC, _("Compilando :"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 12, 85, 20, hProgressWindow, NULL, Instance, NULL);
    NiceFont(TextLabel);

	hCompileProgressBar = CreateWindowEx(0, PROGRESS_CLASS, "", 
		WS_CHILD | WS_VISIBLE | PBS_MARQUEE,
		105, 10, 200, 20, hProgressWindow, NULL, Instance, NULL);

    hCompileOKLabel = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        310, 12, 40, 20, hProgressWindow, NULL, Instance, NULL);
    NiceFont(hCompileOKLabel);

	if(mode == PROGRESS_MODE_PROGRAM) {
		// Erasing Progress Controls
		TextLabel = CreateWindowEx(0, WC_STATIC, _("Preparando :"),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
			10, 42, 85, 20, hProgressWindow, NULL, Instance, NULL);
		NiceFont(TextLabel);

		hErasingProgressBar = CreateWindowEx(0, PROGRESS_CLASS, "", 
			WS_CHILD | WS_VISIBLE,
			105, 40, 200, 20, hProgressWindow, NULL, Instance, NULL);

		hErasingOKLabel = CreateWindowEx(0, WC_STATIC, "",
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
			310, 42, 40, 20, hProgressWindow, NULL, Instance, NULL);
		NiceFont(hErasingOKLabel);

		// Writing Progress Controls
		TextLabel = CreateWindowEx(0, WC_STATIC, _("Gravando :"),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
			10, 72, 85, 20, hProgressWindow, NULL, Instance, NULL);
		NiceFont(TextLabel);

		hWritingProgressBar = CreateWindowEx(0, PROGRESS_CLASS, "", 
			WS_CHILD | WS_VISIBLE,
			105, 70, 200, 20, hProgressWindow, NULL, Instance, NULL);

		hWritingOKLabel = CreateWindowEx(0, WC_STATIC, "",
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
			310, 72, 40, 20, hProgressWindow, NULL, Instance, NULL);
		NiceFont(hWritingOKLabel);

		// Verifying Progress Controls
		TextLabel = CreateWindowEx(0, WC_STATIC, _("Verificando :"),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
			10, 102, 85, 20, hProgressWindow, NULL, Instance, NULL);
		NiceFont(TextLabel);

		hVerifyingProgressBar = CreateWindowEx(0, PROGRESS_CLASS, "", 
			WS_CHILD | WS_VISIBLE,
			105, 100, 200, 20, hProgressWindow, NULL, Instance, NULL);

		hVerifyingOKLabel = CreateWindowEx(0, WC_STATIC, "",
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
			310, 102, 40, 20, hProgressWindow, NULL, Instance, NULL);
		NiceFont(hVerifyingOKLabel);

		offset = 90;
	}

	GetWindowRect(MainWindow     , &rMain);
	GetWindowRect(hProgressWindow, &rProgress);

	width  = rProgress.right  - rProgress.left;
	height = rProgress.bottom - rProgress.top + offset;

	rProgress.top  = rMain.top  + (rMain.bottom - rMain.top  - height)/2;
	rProgress.left = rMain.left + (rMain.right  - rMain.left - width )/2;

	MoveWindow(hProgressWindow, rProgress.left, rProgress.top, width, height, TRUE);

	hMessageLabel = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        10, 40 + offset, 340, 20, hProgressWindow, NULL, Instance, NULL);
    NiceFont(hMessageLabel);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        145, 70 + offset, 70, 23, hProgressWindow, NULL, Instance, NULL); 
    NiceFont(CancelButton);

	SendMessage(hCompileProgressBar, PBM_SETMARQUEE, 1, 0);

	DialogDone = FALSE;

	EnableWindow(MainWindow, FALSE);
    ShowWindow(hProgressWindow, TRUE);
}

//-----------------------------------------------------------------------------
// Compile the program to a hex file for the target micro. Get the output
// file name if necessary, then call the micro-specific compile routines.
//-----------------------------------------------------------------------------
static BOOL CompileProgram(BOOL ShowSuccessMessage)
{
	int err;
	BOOL ret = FALSE;
	ProgressStatus ps;
	char text[100];

	RemoveParallelStart(0, NULL);

	if(!GenerateIntermediateCode()) goto CompileProgramEnd;

    if(Prog.mcu == NULL) {
        Error(_("Must choose a target microcontroller before compiling."));
        goto CompileProgramEnd;
    } 

    if(UartFunctionUsed() && Prog.mcu->uartNeeds.rxPin == 0) {
        Error(_("UART function used but not supported for this micro."));
        goto CompileProgramEnd;
    }
    
    if(PwmFunctionUsed() && Prog.mcu->pwmNeedsPin == 0) {
        Error(_("PWM function used but not supported for this micro."));
        goto CompileProgramEnd;
    }

	StatusBarSetText(0, _("Compilando... aguarde !"));

    SetCursor(LoadCursor(NULL, IDC_WAIT));

	if(ShowSuccessMessage) {
		ShowProgressWindow(PROGRESS_MODE_COMPILE);

		ps.iCurrentStage = PROGRESS_STAGE_COMPILING;
		ps.bStageState   = PROGRESS_STAGE_STATE_OK;
		ps.iStagePercent = 0;
		ps.szMsg = _("Compilando...");

		UpdateProgressWindow(&ps);
	}

	if((err = CompileAnsiCToGCC(ShowSuccessMessage)) == 0)
		ret = TRUE;

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	StatusBarSetText(0, "");

	if(ShowSuccessMessage) {
		if(ret) {
			ps.iCurrentStage = PROGRESS_STAGE_FINISHED;
			ps.bStageState   = PROGRESS_STAGE_STATE_OK;
			ps.iStagePercent = 0;
			ps.szMsg = _("Compilado com sucesso!");
		} else {
			ps.iCurrentStage = PROGRESS_STAGE_COMPILING;
			ps.bStageState   = PROGRESS_STAGE_STATE_FAILED;
			ps.iStagePercent = 0;
			sprintf(text, _("Erro durante Compila��o! C�digo de erro: %d"), err);
			ps.szMsg = text;
		}

		while(UpdateProgressWindow(&ps) != PROGRESS_STATUS_DONE);

		HideProgressWindow();
	}

CompileProgramEnd:
	return ret;
}

//-----------------------------------------------------------------------------
// Compile the program to a hex file for the target micro. Get the output
// file name if necessary, then call the micro-specific compile routines.
//-----------------------------------------------------------------------------
static void WriteProgram(void)
{
	ProgressStatus ps;
	unsigned int iCOMPort;

	if(!POPSettings.COMPortFlash) {
		iCOMPort = LoadCOMPorts(0, 0, 0);
		if(!iCOMPort) {
			Error(_("POP-7 n�o encontrada!"));
			return;
		}
	} else {
		iCOMPort = POPSettings.COMPortFlash;
	}

	ShowProgressWindow(PROGRESS_MODE_PROGRAM);

	ps.iCurrentStage = PROGRESS_STAGE_COMPILING;
	ps.bStageState   = PROGRESS_STAGE_STATE_OK;
	ps.iStagePercent = 0;
	ps.szMsg = _("Compilando...");

	UpdateProgressWindow(&ps);

	if (CompileProgram(FALSE)) 
	{
		ps.iCurrentStage = PROGRESS_STAGE_ERASING;
		ps.bStageState   = PROGRESS_STAGE_STATE_OK;
		ps.iStagePercent = 0;
		ps.szMsg = _("Conectando ao CLP...");

		UpdateProgressWindow(&ps);

		SetCursor(LoadCursor(NULL, IDC_WAIT));
		FlashProgram(CurrentCompileFile, iCOMPort, 230400);
	} 
	else 
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		ps.iCurrentStage = PROGRESS_STAGE_COMPILING;
		ps.bStageState   = PROGRESS_STAGE_STATE_FAILED;
		ps.iStagePercent = 0;
		ps.szMsg = _("Erro na compilacao !!!");

		UpdateProgressWindow(&ps);

		StatusBarSetText(0, _("Erro na compilacao !!!"));
	}

	StatusBarSetText(0, "");

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	while(UpdateProgressWindow(NULL) != PROGRESS_STATUS_DONE);
	HideProgressWindow();
}

//-----------------------------------------------------------------------------
// If the program has been modified then give the user the option to save it
// or to cancel the operation they are performing. Return TRUE if they want
// to cancel.
//-----------------------------------------------------------------------------
BOOL CheckSaveUserCancels(void)
{
	if(!ProgramChangedNotSaved || !Prog.settings.canSave) {
        // no problem
        return FALSE;
    }

    int r = MessageBox(MainWindow, 
        _("The program has changed since it was last saved.\r\n\r\n"
        "Do you want to save the changes?"), "POPTools",
        MB_YESNOCANCEL | MB_ICONWARNING);
    switch(r) {
        case IDYES:
            if(SaveProgram())
                return FALSE;
            else
                return TRUE;

        case IDNO:
            return FALSE;

        case IDCANCEL:
            return TRUE;

        default:
            oops();
    }
}

//-----------------------------------------------------------------------------
// Load a new program from a file. If it succeeds then set our default filename
// to that, else we end up with an empty file then.
//-----------------------------------------------------------------------------
static void OpenDialog(char *filename)
{
    char tempSaveFile[MAX_PATH] = "";

	if(filename != NULL && strlen(filename)) {
		strcpy(tempSaveFile, filename);
	} else {
		FileDialogShow(LoadLadder, "ld", tempSaveFile);
		if(!strlen(tempSaveFile))
			return;
	}

    if(!LoadProjectFromFile(tempSaveFile)) {
        Error(_("Couldn't open '%s'."), tempSaveFile);
        CurrentSaveFile[0] = '\0';
		NewProgram();
    } else {
        ProgramChangedNotSaved = FALSE;
        strcpy(CurrentSaveFile, tempSaveFile);
		ChangeFileExtension(CurrentSaveFile, "ld");
        UndoFlush();

		UpdateRecentList(tempSaveFile);
	}

    GenerateIoListDontLoseSelection();
    RefreshScrollbars();
    UpdateMainWindowTitleBar();
}

//-----------------------------------------------------------------------------
// Housekeeping required when the program changes: mark the program as
// changed so that we ask if user wants to save before exiting, and update
// the I/O list.
//-----------------------------------------------------------------------------
void ProgramChanged(void)
{
    ProgramChangedNotSaved = TRUE;
    GenerateIoListDontLoseSelection();
    RefreshScrollbars();
    UpdateMainWindowTitleBar();
}

//-----------------------------------------------------------------------------
// Hook that we install when the user starts dragging the `splitter,' in case
// they drag it out of the narrow area of the drawn splitter bar. Resize
// the listview in response to mouse move, and unhook ourselves when they
// release the mouse button.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MouseHook(int code, WPARAM wParam, LPARAM lParam)
{
    switch(code) {
        case HC_ACTION: {
            MSLLHOOKSTRUCT *mhs = (MSLLHOOKSTRUCT *)lParam;

            switch(wParam) {
                case WM_MOUSEMOVE: {
                    int dy = MouseY - mhs->pt.y;
                   
                    IoListHeight += dy;
                    if(IoListHeight < 50) IoListHeight = 50;
                    MouseY = mhs->pt.y;
                    MainWindowResized();

                    break;
                }

                case WM_LBUTTONUP:
                    UnhookWindowsHookEx(MouseHookHandle);
                    break;
            }
            break;
        }
    }
    return CallNextHookEx(MouseHookHandle, code, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Handle a selection from the menu bar of the main window.
//-----------------------------------------------------------------------------
void ProcessMenu(int code)
{
	unsigned int recent_index;

	if(code >= MNU_PROCESSOR_0 && code < MNU_PROCESSOR_0+NUM_SUPPORTED_MCUS) {
        strcpy(CurrentCompileFile, "");
        Prog.mcu = &SupportedMcus[code - MNU_PROCESSOR_0];
        RefreshControlsToSettings();
        return;
    }
    if(code == MNU_PROCESSOR_0+NUM_SUPPORTED_MCUS) {
        Prog.mcu = NULL;
        strcpy(CurrentCompileFile, "");
        RefreshControlsToSettings();
        return;
    }

    switch(code) {
        case MNU_NEW:
            if(CheckSaveUserCancels()) break;
            NewProgram();
			ProgramChangedNotSaved = FALSE;
            strcpy(CurrentSaveFile, "");
            strcpy(CurrentCompileFile, "");
            GenerateIoListDontLoseSelection();
            RefreshScrollbars();
            UpdateMainWindowTitleBar();
            break;

        case MNU_OPEN:
            if(CheckSaveUserCancels()) break;
            OpenDialog(NULL);
            break;

        case MNU_RECENT_CLEAR:
			memset(POPSettings.recent_list, 0, sizeof(POPSettings.recent_list));
			break;

        case MNU_RECENT_START+0:
        case MNU_RECENT_START+1:
        case MNU_RECENT_START+2:
        case MNU_RECENT_START+3:
        case MNU_RECENT_START+4:
        case MNU_RECENT_START+5:
        case MNU_RECENT_START+6:
        case MNU_RECENT_START+7:
        case MNU_RECENT_START+8:
        case MNU_RECENT_START+9:
			if(InSimulationMode) {
				Error(_("Favor interromper a simula��o primeiro!"));
				break;
			}
            if(CheckSaveUserCancels()) break;
			recent_index = code - MNU_RECENT_START;
			OpenDialog(POPSettings.recent_list[recent_index]);
            break;

        case MNU_SAVE:
            SaveProgram();
            UpdateMainWindowTitleBar();
            break;

        case MNU_SAVE_AS:
            SaveAsDialog();
            UpdateMainWindowTitleBar();
            break;

		case MNU_SAVE_AS_C:
			SaveAsAnsiC();
			break;

        case MNU_EXPORT:
            ExportDialog();
            break;

        case MNU_PRINT:
            PrintDocument();
            break;

        case MNU_EXIT:
			if(InSimulationMode) {
				ToggleSimulationMode();
			} else {
				if(CheckSaveUserCancels()) break;
				PostQuitMessage(0);
			}
            break;

        case MNU_INSERT_COMMENT:
			if(CanInsertComment)
				CHANGING_PROGRAM(AddComment(_("--add comment here--")));
            break;

        case MNU_INSERT_PARALLEL:
            CHANGING_PROGRAM(AddParallelStart());
            break;

        case MNU_INSERT_CONTACTS:
            CHANGING_PROGRAM(AddContact());
            break;

        case MNU_INSERT_COIL:
            CHANGING_PROGRAM(AddCoil());
            break;

        case MNU_INSERT_TON:
            CHANGING_PROGRAM(AddTimer(ELEM_TON));
            break;

        case MNU_INSERT_TOF:
            CHANGING_PROGRAM(AddTimer(ELEM_TOF));
            break;

        case MNU_INSERT_RTO:
            CHANGING_PROGRAM(AddTimer(ELEM_RTO));
            break;

        case MNU_INSERT_RTC:
            CHANGING_PROGRAM(AddRTC(ELEM_RTC));
            break;

        case MNU_INSERT_CTU:
            CHANGING_PROGRAM(AddCounter(ELEM_CTU));
            break;

        case MNU_INSERT_CTD:
            CHANGING_PROGRAM(AddCounter(ELEM_CTD));
            break;

        case MNU_INSERT_CTC:
            CHANGING_PROGRAM(AddCounter(ELEM_CTC));
            break;

        case MNU_INSERT_RES:
            CHANGING_PROGRAM(AddReset());
            break;

        case MNU_INSERT_OPEN:
            CHANGING_PROGRAM(AddEmpty(ELEM_OPEN));
            break;

        case MNU_INSERT_SHORT:
            CHANGING_PROGRAM(AddEmpty(ELEM_SHORT));
            break;

        case MNU_INSERT_MASTER_RLY:
            CHANGING_PROGRAM(AddMasterRelay());
            break;

        case MNU_INSERT_SET_BIT:
            CHANGING_PROGRAM(AddSetBit());
            break;

        case MNU_INSERT_CHECK_BIT:
            CHANGING_PROGRAM(AddCheckBit());
            break;

        case MNU_INSERT_SHIFT_REG:
            CHANGING_PROGRAM(AddShiftRegister());
            break;

        case MNU_INSERT_LUT:
            CHANGING_PROGRAM(AddLookUpTable());
            break;
        
        case MNU_INSERT_PWL:
            CHANGING_PROGRAM(AddPiecewiseLinear());
            break;
        
        case MNU_INSERT_OSR:
            CHANGING_PROGRAM(AddEmpty(ELEM_ONE_SHOT_RISING));
            break;

        case MNU_INSERT_OSF:
            CHANGING_PROGRAM(AddEmpty(ELEM_ONE_SHOT_FALLING));
            break;

        case MNU_INSERT_MOV:
            CHANGING_PROGRAM(AddMove());
            break;

        case MNU_INSERT_SET_PWM:
            CHANGING_PROGRAM(AddSetPwm());
            break;

        case MNU_INSERT_SET_DA:
            CHANGING_PROGRAM(AddSetDA());
            break;

		case MNU_INSERT_READ_ADC:
            CHANGING_PROGRAM(AddReadAdc());
            break;

        case MNU_INSERT_READ_ENC:
            CHANGING_PROGRAM(AddReadEnc());
            break;

        case MNU_INSERT_RESET_ENC:
            CHANGING_PROGRAM(AddResetEnc());
            break;
			
		case MNU_INSERT_MULTISET_DA:
            CHANGING_PROGRAM(AddMultisetDA());
            break;

        case MNU_READ_FMTD_STR:
            CHANGING_PROGRAM(AddReadFormatString());
            break;

        case MNU_WRITE_FMTD_STR:
            CHANGING_PROGRAM(AddWriteFormatString());
            break;

		case MNU_READ_SERVO_YASKAWA:
            CHANGING_PROGRAM(AddReadServoYaskawa());
            break;

        case MNU_WRITE_SERVO_YASKAWA:
            CHANGING_PROGRAM(AddWriteServoYaskawa());
            break;

		case MNU_INSERT_READ_USS:
            CHANGING_PROGRAM(AddReadUSS());
            break;

        case MNU_INSERT_WRITE_USS:
            CHANGING_PROGRAM(AddWriteUSS());
            break;

        case MNU_INSERT_READ_MODBUS:
            CHANGING_PROGRAM(AddReadModbus());
            break;

        case MNU_INSERT_WRITE_MODBUS:
            CHANGING_PROGRAM(AddWriteModbus());
            break;

        case MNU_INSERT_UART_SEND:
            CHANGING_PROGRAM(AddUart(ELEM_UART_SEND));
            break;

        case MNU_INSERT_UART_RECV:
            CHANGING_PROGRAM(AddUart(ELEM_UART_RECV));
            break;

        case MNU_INSERT_SQRT:
            CHANGING_PROGRAM(AddSqrt());
            break;

        case MNU_INSERT_RAND:
            CHANGING_PROGRAM(AddRand());
            break;

        case MNU_INSERT_ABS:
            CHANGING_PROGRAM(AddAbs());
            break;

        case MNU_INSERT_PERSIST:
            CHANGING_PROGRAM(AddPersist());
            break;

        {
            int elem;
            case MNU_INSERT_ADD: elem = ELEM_ADD; goto math;
            case MNU_INSERT_SUB: elem = ELEM_SUB; goto math;
            case MNU_INSERT_MUL: elem = ELEM_MUL; goto math;
            case MNU_INSERT_DIV: elem = ELEM_DIV; goto math;
            case MNU_INSERT_MOD: elem = ELEM_MOD; goto math;
math:
                CHANGING_PROGRAM(AddMath(elem));
                break;
        }

        {
            int elem;
            case MNU_INSERT_EQU: elem = ELEM_EQU; goto cmp;
            case MNU_INSERT_NEQ: elem = ELEM_NEQ; goto cmp;
            case MNU_INSERT_GRT: elem = ELEM_GRT; goto cmp;
            case MNU_INSERT_GEQ: elem = ELEM_GEQ; goto cmp;
            case MNU_INSERT_LES: elem = ELEM_LES; goto cmp;
            case MNU_INSERT_LEQ: elem = ELEM_LEQ; goto cmp;
cmp:    
                CHANGING_PROGRAM(AddCmp(elem));
                break;
        } 

        case MNU_MAKE_NORMAL:
            CHANGING_PROGRAM(MakeNormalSelected());
            break;

        case MNU_NEGATE:
            CHANGING_PROGRAM(NegateSelected());
            break;

        case MNU_MAKE_SET_ONLY:
            CHANGING_PROGRAM(MakeSetOnlySelected());
            break;

        case MNU_MAKE_RESET_ONLY:
            CHANGING_PROGRAM(MakeResetOnlySelected());
            break;

        case MNU_UNDO:
            UndoUndo();
			ProgramChanged();
            break;

        case MNU_REDO:
            UndoRedo();
			ProgramChanged();
            break;

        case MNU_INSERT_RUNG_BEFORE:
            CHANGING_PROGRAM(InsertRung(FALSE));
            break;

        case MNU_INSERT_RUNG_AFTER:
            CHANGING_PROGRAM(InsertRung(TRUE));
            break;

        case MNU_DELETE_RUNG:
            CHANGING_PROGRAM(DeleteSelectedRung());
            break;

        case MNU_PUSH_RUNG_UP:
            CHANGING_PROGRAM(PushRung(true));
            break;

        case MNU_PUSH_RUNG_DOWN:
            CHANGING_PROGRAM(PushRung(false));
            break;

        case MNU_GO_HOME:
			MoveCursorKeyboard(VK_HOME, TRUE);
            break;

        case MNU_GO_END:
			MoveCursorKeyboard(VK_END, TRUE);
            break;

        case MNU_COPY_ELEMENT: {
            CopyLeaf(Selected, SelectedWhich);
            break;
		}

        case MNU_PASTE_ELEMENT:
            CHANGING_PROGRAM(PasteLeaf());
            break;

        case MNU_COPY_RUNG: {
			int i = RungContainingSelected();
//            CopyLeaf(Selected, SelectedWhich);
			if(i >= 0) {
				CopyRung(Prog.rungs[i]);
			}
            break;
		}

        case MNU_PASTE_RUNG_BEFORE:
//            CHANGING_PROGRAM(PasteLeaf());
            CHANGING_PROGRAM(PasteRung(false));
            break;

        case MNU_PASTE_RUNG_AFTER:
//            CHANGING_PROGRAM(PasteLeaf());
            CHANGING_PROGRAM(PasteRung(true));
            break;

        case MNU_CUT_ELEMENT:
			CopyLeaf(Selected, SelectedWhich);
            CHANGING_PROGRAM(DeleteSelectedFromProgram());
            break;

        case MNU_DELETE_ELEMENT:
            CHANGING_PROGRAM(DeleteSelectedFromProgram());
            break;

        case MNU_FIND_AND_REPLACE:
			CHANGING_PROGRAM(ShowFARDialog());
            break;

        case MNU_FIND_AND_REPLACE_NEXT:
			if(!FindAndReplace(NULL, NULL, FAR_FIND_NEXT))
				FindAndReplace(NULL, NULL, FAR_FIND_FIRST);
            break;

        case MNU_MCU_SETTINGS:
            CHANGING_PROGRAM(ShowConfDialog(false));
			WhatCanWeDoFromCursorAndTopology();
            break;

        case MNU_MCU_PREFERENCES:
            ShowPrefDialog();
            break;

        case MNU_SIMULATION_MODE:
            if(!InSimulationMode && CheckSaveUserCancels()) break;
            ToggleSimulationMode();
            break;

        case MNU_START_SIMULATION:
            StartSimulation();
            break;

        case MNU_PAUSE_SIMULATION:
            PauseSimulation();
            break;

        case MNU_STOP_SIMULATION:
            StopSimulation();
            break;

        case MNU_SINGLE_CYCLE:
			RibbonSetCmdState(cmdSimulationStop, TRUE);
            SimulateOneCycle(TRUE);
            break;

        case MNU_COMPILE:
            if(CheckSaveUserCancels()) break;
            CompileProgram(TRUE);
            break;

        case MNU_PROGRAM:
            if(CheckSaveUserCancels()) break;
            WriteProgram();
            break;

		case MNU_DEBUG:
			ShowDebugDialog();
			break;

        case MNU_MANUAL:
			OpenCHM();
//            ShowHelpDialog(FALSE);
            break;

        case MNU_KEYBOARD_MANUAL:
			OpenCHM(6);
//            ShowHelpDialog(FALSE);
            break;

		case MNU_EXAMPLE_COMMENT:
            OpenDialog("examples\\comment.ld");
			break;
		case MNU_EXAMPLE_CONTACTS:
		case MNU_EXAMPLE_COIL:
            OpenDialog("examples\\contacts.ld");
			break;
		case MNU_EXAMPLE_TON:
		case MNU_EXAMPLE_TOF:
		case MNU_EXAMPLE_RTO:
		case MNU_EXAMPLE_RES:
		case MNU_EXAMPLE_CTU:
		case MNU_EXAMPLE_CTD:
		case MNU_EXAMPLE_CTC:
            OpenDialog("examples\\timers-counters.ld");
			break;
		case MNU_EXAMPLE_OSR:
		case MNU_EXAMPLE_OSF:
            OpenDialog("examples\\OneShot.ld");
			break;
		case MNU_EXAMPLE_ADD:
		case MNU_EXAMPLE_SUB:
		case MNU_EXAMPLE_MUL:
		case MNU_EXAMPLE_DIV:
		case MNU_EXAMPLE_SQRT:
            OpenDialog("examples\\math.ld");
			break;
		case MNU_EXAMPLE_RAND:
		case MNU_EXAMPLE_ABS:
            OpenDialog("examples\\abs_rand.ld");
			break;
		case MNU_EXAMPLE_MOV:
		case MNU_EXAMPLE_SET_BIT:
		case MNU_EXAMPLE_CHECK_BIT:
		case MNU_EXAMPLE_PERSIST:
            OpenDialog("examples\\variables.ld");
			break;
		case MNU_EXAMPLE_READ_ADC:
            OpenDialog("examples\\adc.ld");
            break;
		case MNU_EXAMPLE_SET_PWM:
		case MNU_EXAMPLE_MULTISET_DA:
            OpenDialog("examples\\motors.ld");
            break;
		case MNU_EXAMPLE_UART_SEND:
		case MNU_EXAMPLE_UART_RECV:
            OpenDialog("examples\\char.ld");
            break;
		case MNU_EXAMPLE_EQU:
		case MNU_EXAMPLE_NEQ:
		case MNU_EXAMPLE_GRT:
		case MNU_EXAMPLE_GEQ:
		case MNU_EXAMPLE_LES:
		case MNU_EXAMPLE_LEQ:
            OpenDialog("examples\\conditionals.ld");
            break;
		case MNU_EXAMPLE_OPEN:
		case MNU_EXAMPLE_SHORT:
		case MNU_EXAMPLE_MASTER_RLY:
            OpenDialog("examples\\OpenShort.ld");
            break;
		case MNU_EXAMPLE_SHIFT_REG:
            OpenDialog("examples\\ShiftRegister.ld");
            break;
		case MNU_EXAMPLE_LUT:
            OpenDialog("examples\\LookUpTable.ld");
            break;
		case MNU_EXAMPLE_PWL:
		case MNU_EXAMPLE_READ_ENC:
		case MNU_EXAMPLE_RESET_ENC:
            OpenDialog("examples\\PieceWiseLinear.ld");
            break;
		case MNU_EXAMPLE_READ_USS:
		case MNU_EXAMPLE_WRITE_USS:
            OpenDialog("examples\\uss.ld");
            break;
		case MNU_EXAMPLE_SET_DA:
            OpenDialog("examples\\da.ld");
            break;
		case MNU_EXAMPLE_READ_MODBUS:
		case MNU_EXAMPLE_WRITE_MODBUS:
            OpenDialog("examples\\modbus.ld");
            break;
		case MNU_EXAMPLE_READ_FMTD_STR:
		case MNU_EXAMPLE_WRITE_FMTD_STR:
            OpenDialog("examples\\string.ld");
            break;
		case MNU_EXAMPLE_READ_SERVO_YASK:
		case MNU_EXAMPLE_WRITE_SERVO_YASK:
            OpenDialog("examples\\ns600.ld");
            break;
		case MNU_EXAMPLE_RTC:
            OpenDialog("examples\\rtc.ld");
            break;
		case MNU_EXAMPLE_ADC_LED:
            OpenDialog("examples\\adc_led.ld");
            break;
		case MNU_EXAMPLE_SEMAPHORE:
            OpenDialog("examples\\semaphore.ld");
            break;

		case MNU_EXAMPLE_TESTER:
            OpenDialog("examples\\Tester.ld");
            break;

        case MNU_ABOUT:
			mysplash.Show();
            break;
    }
}

//-----------------------------------------------------------------------------
// WndProc for MainWindow.
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
		case WM_SOCKET:
			SimulationServer_Message(wParam, lParam);
			return 1;
			break;

		case WM_CREATE: 
			mysplash.Init(hwnd);

			//mysplash.Show();
			//simulate lengthy window initialization
			//Sleep(3000);
			//hide the splash screen as the main window appears
			//mysplash.Hide();

			break;
        case WM_ERASEBKGND:
            break;

        case WM_SETFOCUS:
            InvalidateRect(MainWindow, NULL, FALSE);
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
		    InvalidateRect(DrawWindow, NULL, FALSE);
            Hdc = BeginPaint(DrawWindow, &ps);

            // This draws the schematic.
			Draw_Start();
            PaintWindow();
			Draw_End();

			RECT r;
            // Fill around the scroll bars
            if(NeedHoriz) {
                r.top = IoListTop - ScrollHeight - 2;
                r.bottom = IoListTop - 2;
                FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            }
            GetClientRect(MainWindow, &r);
            r.left = r.right - ScrollWidth - 2;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));

            // Draw the splitter thing to grab to resize the I/O listview.
            GetClientRect(DrawWindow, &r);
            r.top = IoListTop - 2 - RibbonHeight;
            r.bottom = IoListTop - RibbonHeight;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
            r.top = IoListTop - 2 - RibbonHeight;
            r.bottom = IoListTop - 1 - RibbonHeight;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));
            r.top = IoListTop - RibbonHeight;
            r.bottom = IoListTop + 1 - RibbonHeight;
            FillRect(Hdc, &r, (HBRUSH)GetStockObject(DKGRAY_BRUSH));

            EndPaint(DrawWindow, &ps);

			Hdc = BeginPaint(hwnd, &ps);

//            EndPaint(hwnd, &ps);
            return 1;
        }

		case WM_INPUTLANGCHANGE:
			KeyboardHandlers_LangChanged();
			break;

        case WM_KEYDOWN: {
			if(wParam != VK_SHIFT && wParam != VK_CONTROL) {
				KeyboardHandlers_Execute(wParam);
                InvalidateRect(MainWindow, NULL, FALSE);
            }
            break;
        }

        case WM_LBUTTONDBLCLK: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam) - RibbonHeight;

			if(x < X_PADDING - FONT_WIDTH + 3) {
				ToggleBreakPoint(y);
			} else if(InSimulationMode) {
                EditElementMouseDoubleclick(x, y);
            } else {
                CHANGING_PROGRAM(EditElementMouseDoubleclick(x, y));
            }
            InvalidateRect(MainWindow, NULL, FALSE);
            break;
        }

        case WM_LBUTTONDOWN: {
			if(mysplash.SHOWING)
                mysplash.Hide();

            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if((y > (IoListTop - 9)) && (y < (IoListTop + 3))) {
                POINT pt;
                pt.x = x; pt.y = y;
                ClientToScreen(MainWindow, &pt);
                MouseY = pt.y;
                MouseHookHandle = SetWindowsHookEx(WH_MOUSE_LL,
                        (HOOKPROC)MouseHook, Instance, 0);
            }
            if(!InSimulationMode) MoveCursorMouseClick(x, y);

            SetFocus(MainWindow);
            InvalidateRect(MainWindow, NULL, FALSE);
            break;
        }
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            if((y > (IoListTop - 9)) && (y < (IoListTop + 3))) {
                SetCursor(LoadCursor(NULL, IDC_SIZENS));
            } else {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
            }
            
            break;
        }
        case WM_MOUSEWHEEL: {
            if((GET_WHEEL_DELTA_WPARAM(wParam)) > 0) {
                VscrollProc(SB_LINEUP);
            } else {
                VscrollProc(SB_LINEDOWN);
            }
            break;
        }

        case WM_SIZE:
            MainWindowResized();
            break;

        case WM_NOTIFY: {
            NMHDR *h = (NMHDR *)lParam;
            if(h->hwndFrom == IoList) {
                IoMapListProc(h);
			}
            return 0;
        }
        case WM_VSCROLL:
            VscrollProc(wParam);
            break;

        case WM_HSCROLL:
            HscrollProc(wParam);
            break;

        case WM_COMMAND:
            ProcessMenu(LOWORD(wParam));
            InvalidateRect(MainWindow, NULL, FALSE);
            break;

        case WM_CLOSE:
			if(InSimulationMode) {
				ProcessMenu(MNU_SIMULATION_MODE);
				break;
			}
        case WM_DESTROY:
            if(CheckSaveUserCancels()) break;

            PostQuitMessage(0);
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

//-----------------------------------------------------------------------------
// Create our window class; nothing exciting.
//-----------------------------------------------------------------------------
static BOOL MakeWindowClass()
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)MainWndProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName    = "POPTools";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 32, 32, 0);
    wc.hIconSm          = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 16, 16, 0);

    return RegisterClassEx(&wc);
}

//-----------------------------------------------------------------------------
// Create our window class; nothing exciting.
//-----------------------------------------------------------------------------
extern LRESULT CALLBACK DebugDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static BOOL MakeDebugWindowClass()
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                          CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)DebugDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "POPDebugDialog";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 32, 32, 0);
    wc.hIconSm          = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 16, 16, 0);

    return RegisterClassEx(&wc);
}

#define SETTINGS_FILE (L"POPTools\\settings.xml")

void LoadSettings(void)
{
	char *pStr;
	PWSTR settings_path;
	wchar_t *settings_file;
	unsigned int i, totallen;
	XMLWrapperElementList *pElementList, *pCurrentElementList;

	memset(&POPSettings, 0, sizeof(POPSettings));

	SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &settings_path);
	totallen = wcslen(settings_path) + wcslen(SETTINGS_FILE) + 1;
	settings_file = new wchar_t[totallen+1];
	swprintf(settings_file, totallen+1, L"%s\\%s", settings_path, SETTINGS_FILE);

	if(!XmlSettings.Open(settings_file)) {
		// TODO: Internacionalizar
		ShowTaskDialog(L"Erro ao carregar as prefer�ncias", L"Ser� utilizada a configura��o padr�o", TD_ERROR_ICON, TDCBF_OK_BUTTON);

		POPSettings.AutoSaveInterval = 0;
		POPSettings.ShowSimulationWarnings = TRUE;

		delete [] settings_file;

		return;
	}

	XmlSettings.SelectElements(L"SimulationSettings", 0);
	pElementList = XmlSettings.GetElementList(-1);
	if(pElementList != NULL) {
		pCurrentElementList = pElementList->children;

		while(pCurrentElementList != NULL) {
			if(!wcscmp(pCurrentElementList->element.name, L"ShowWarnings")) {
				pStr = _com_util::ConvertBSTRToString(pCurrentElementList->element.value);
				POPSettings.ShowSimulationWarnings = atoi(pStr) ? TRUE : FALSE;
				delete pStr;
			}

			pCurrentElementList = pCurrentElementList->next;
		}

		XmlSettings.FreeElementList(pElementList);
	}

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"ProgramSettings", 0);
	pElementList = XmlSettings.GetElementList(-1);
	if(pElementList != NULL) {
		pCurrentElementList = pElementList->children;

		while(pCurrentElementList != NULL) {
			if(!wcscmp(pCurrentElementList->element.name, L"AutoSaveInterval")) {
				pStr = _com_util::ConvertBSTRToString(pCurrentElementList->element.value);
				POPSettings.AutoSaveInterval = atoi(pStr);
				if(POPSettings.AutoSaveInterval < 0)
					POPSettings.AutoSaveInterval = 0;
				delete pStr;
			}

			pCurrentElementList = pCurrentElementList->next;
		}

		XmlSettings.FreeElementList(pElementList);
	}

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"RecentList", 0);
	pElementList = XmlSettings.GetElementList(-1);

	if(pElementList != NULL) {
		char *szFile;

		pCurrentElementList = pElementList->children;

		for(i=0; pCurrentElementList != NULL && pCurrentElementList->element.value != NULL && i<MAX_RECENT_ITEMS; i++) {
			szFile = _com_util::ConvertBSTRToString(pCurrentElementList->element.value);
			strcpy(POPSettings.recent_list[i], szFile);
			delete [] szFile;
			pCurrentElementList = pCurrentElementList->next;
		}

		XmlSettings.FreeElementList(pElementList);
	}

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"PortSettings", 0);
	pElementList = XmlSettings.GetElementList(-1);

	if(pElementList != NULL) {
		pCurrentElementList = pElementList->children;

		while(pCurrentElementList != NULL) {
			if(!wcscmp(pCurrentElementList->element.name, L"COMPortFlash")) {
				pStr = _com_util::ConvertBSTRToString(pCurrentElementList->element.value);
				POPSettings.COMPortFlash = atoi(pStr);
				delete pStr;
			} else if(!wcscmp(pCurrentElementList->element.name, L"COMPortDebug")) {
				pStr = _com_util::ConvertBSTRToString(pCurrentElementList->element.value);
				POPSettings.COMPortDebug = atoi(pStr);
				delete pStr;
			}

			pCurrentElementList = pCurrentElementList->next;
		}

		XmlSettings.FreeElementList(pElementList);
	}

	delete [] settings_file;
}

void SaveSettings(void)
{
	unsigned int i;
	wchar_t tmp[10];

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"SimulationSettings", 0);
	XmlSettings.SelectElements(L"ShowWarnings", 0);
	XmlSettings.ChangeElement(0, POPSettings.ShowSimulationWarnings ? L"1" : L"0");

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"ProgramSettings", 0);
	XmlSettings.SelectElements(L"AutoSaveInterval", 0);
	swprintf((wchar_t *)tmp, 10, L"%d", POPSettings.AutoSaveInterval);
	XmlSettings.ChangeElement(0, tmp);

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"RecentList", 0);
	XmlSettings.DeleteChildren(0);

	for(i=0; i<MAX_RECENT_ITEMS; i++) {
		if(!strlen(POPSettings.recent_list[i])) // empty item, break
			break;

		XmlSettings.AddElement(0, -1, "RecentFile", POPSettings.recent_list[i]);
	}

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"PortSettings", 0);
	XmlSettings.SelectElements(L"COMPortFlash", 0);
	swprintf((wchar_t *)tmp, 10, L"%d", POPSettings.COMPortFlash);
	XmlSettings.ChangeElement(0, tmp);

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"PortSettings", 0);
	XmlSettings.SelectElements(L"COMPortDebug", 0);
	swprintf((wchar_t *)tmp, 10, L"%d", POPSettings.COMPortDebug);
	XmlSettings.ChangeElement(0, tmp);

	XmlSettings.Save(NULL);
}

// Creates resources that are not bound to a particular device.
// Their lifetime effectively extends for the duration of the
// application.
static HRESULT CreateDeviceIndependentResources()
{
	HRESULT hr;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&pWriteFactory));
	}
	
	if (SUCCEEDED(hr))
	{
		// Create WIC factory.
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<void **>(&pWICFactory)
			);
	}

	return hr;
}	

static void DiscardDeviceIndependentResources()
{
	SafeRelease(&pWICFactory);
	SafeRelease(&pWriteFactory);
	SafeRelease(&pD2DFactory);
}

//-----------------------------------------------------------------------------
// Entry point into the program.
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, INT nCmdShow)
{
	WSADATA wsaData;
	int iResult;

	srand((unsigned int)time(NULL));

	Instance = hInstance;

    MainHeap = HeapCreate(0, 1024*64, 0);

	LoadSettings();

    MakeWindowClass();
	MakeDebugWindowClass();
	MakeDialogBoxClass();

	HMENU top = NULL;

    MainWindow = CreateWindowEx(0, "POPTools", "",
        WS_OVERLAPPED | WS_THICKFRAME | WS_CLIPCHILDREN | WS_MAXIMIZEBOX |
        WS_MINIMIZEBOX | WS_SYSMENU,
        10, 10, 800, 600, NULL, top, Instance, NULL);
    ThawWindowPos(MainWindow);
    IoListHeight = 100;
    ThawDWORD(IoListHeight);

    InitCommonControls();
    InitForDrawing();
	CreateDeviceIndependentResources();

    MakeMainWindowControls();
    MainWindowResized();

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		Error(_("WSAStartup failed: %d\n"), iResult);
		return 1;
	}

	InitRibbon(MainWindow);

	KeyboardHandlers_Init();

	NewProgram();
    strcpy(CurrentSaveFile, "");

    ShowWindow(MainWindow, SW_SHOW);
    SetTimer(MainWindow, TIMER_BLINK_CURSOR, 800, BlinkCursor);
	SetAutoSaveInterval(POPSettings.AutoSaveInterval);

    if(strlen(lpCmdLine) > 0) {
        char line[MAX_PATH];
        if(*lpCmdLine == '"') { 
            strcpy(line, lpCmdLine+1);
        } else {
            strcpy(line, lpCmdLine);
        }
        if(strchr(line, '"')) *strchr(line, '"') = '\0';

        char *s;
        GetFullPathName(line, sizeof(CurrentSaveFile), CurrentSaveFile, &s);
        OpenDialog(CurrentSaveFile);
        UndoFlush();
    }

    GenerateIoListDontLoseSelection();
    RefreshScrollbars();
    UpdateMainWindowTitleBar();

	// Initialize ModBUS protocol and devices
	Init_MBDev();
	Init_MBDev_Slave();

	Draw_Init();

	MSG msg;
    DWORD ret;
    while(ret = GetMessage(&msg, NULL, 0, 0)) {
        if(msg.hwnd == IoList && msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_TAB) {
                SetFocus(MainWindow);
                continue;
            }
        }
        if(msg.message == WM_KEYDOWN && msg.wParam != VK_UP &&
            msg.wParam != VK_DOWN && msg.wParam != VK_RETURN && msg.wParam
            != VK_SHIFT)
        {
            if(msg.hwnd == IoList) {
                msg.hwnd = MainWindow;
                SetFocus(MainWindow);
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    FreezeWindowPos(MainWindow);
    FreezeDWORD(IoListHeight);

	SaveSettings();

	WSACleanup();

#ifndef POPTOOLS_DISABLE_RIBBON	
	DestroyRibbon();
#endif

	return 0;
}
