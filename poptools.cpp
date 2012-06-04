//-----------------------------------------------------------------------------
// Copyright 2007 Jonathan Westhues
//
// This file is part of LDmicro.
// 
// LDmicro is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// LDmicro is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with LDmicro.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// A ladder logic compiler for 8 bit micros: user draws a ladder diagram,
// with an appropriately constrained `schematic editor,' and then we can
// simulated it under Windows or generate PIC/AVR code that performs the
// requested operations. This files contains the program entry point, plus
// most of the UI logic relating to the main window.
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldmicro.h"
#include "freeze.h"
#include "mcutable.h"

HINSTANCE   Instance;
HWND        MainWindow;
HDC         Hdc;

#include <atlbase.h>
CComModule _Module;

ID2D1Factory*		pD2DFactory;
IDWriteFactory*		pWriteFactory;
IWICImagingFactory *	pWICFactory;

SPLASH mysplash;

// parameters used to capture the mouse when implementing our totally non-
// general splitter control
static HHOOK       MouseHookHandle;
static int         MouseY;

// For the open/save dialog boxes
#define LDMICRO_PATTERN "POPTools Projeto Ladder (*.ld)\0*.ld\0" \
                     "All files\0*\0\0"
char CurrentSaveFile[MAX_PATH];
BOOL ProgramChangedNotSaved = FALSE;

#define HEX_PATTERN  "Intel Hex Files (*.hex)\0*.hex\0All files\0*\0\0"
#define C_PATTERN "C Source Files (*.c)\0*.c\0All Files\0*\0\0"
#define INTERPRETED_PATTERN \
    "Interpretable Byte Code Files (*.int)\0*.int\0All Files\0*\0\0"
char CurrentCompileFile[MAX_PATH];

#define TXT_PATTERN  "Text Files (*.txt)\0*.txt\0All files\0*\0\0"

// Internal flags available to the users.
char *InternalFlags[] = { "SerialReady", "SerialTimeout", "SerialAborted", "" };

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

	PopulateRecentListMenu();
}

//-----------------------------------------------------------------------------
// Get a filename with a common dialog box and then save the program to that
// file and then set our default filename to that.
//-----------------------------------------------------------------------------
static BOOL SaveAsDialog(void)
{
    OPENFILENAME ofn;

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hInstance = Instance;
    ofn.lpstrFilter = LDMICRO_PATTERN;
    ofn.lpstrDefExt = "ld";
    ofn.lpstrFile = CurrentSaveFile;
    ofn.nMaxFile = sizeof(CurrentSaveFile);
	ofn.hwndOwner = MainWindow;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if(!GetSaveFileName(&ofn))
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
static void ExportDialog(void)
{
    char exportFile[MAX_PATH];
    OPENFILENAME ofn;

    exportFile[0] = '\0';

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hInstance = Instance;
    ofn.lpstrFilter = TXT_PATTERN;
    ofn.lpstrFile = exportFile;
	ofn.lpstrDefExt = "txt";
    ofn.lpstrTitle = _("Export As Text");
    ofn.nMaxFile = sizeof(exportFile);
	ofn.hwndOwner = MainWindow;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if(!GetSaveFileName(&ofn))
        return;

    ExportDrawingAsText(exportFile);
}

//-----------------------------------------------------------------------------
// If we already have a filename, save the program to that. Otherwise same
// as Save As. Returns TRUE if it worked, else returns FALSE.
//-----------------------------------------------------------------------------
static BOOL SaveProgram(void)
{
    if(strlen(CurrentSaveFile)) {
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

void ChangeFileExtension(char *name, char *ext)
{
	unsigned int pos;

	pos = strlen(name) - 1;
	while(name[pos] != '.' && pos) pos--;
	if(pos) {
		name[pos] = 0;
	}

	sprintf(name, "%s.%s", name, ext);
}

//-----------------------------------------------------------------------------
// Compile the program to a hex file for the target micro. Get the output
// file name if necessary, then call the micro-specific compile routines.
//-----------------------------------------------------------------------------
static BOOL CompileProgram(BOOL compileAs)
{
	BOOL ret = FALSE;

	// check if a valid path and write permission
	if (compileAs || strlen(CurrentCompileFile) > 0)
	{
		// We should to use "a" instead of "w" to avoid to overwrite the current file
		FILE *f = fopen(CurrentCompileFile, "r");
		if(!f)
			CurrentCompileFile[0] = '\0';
		else {
			fclose(f);
			fopen(CurrentCompileFile, "a");
			if(!f)
				CurrentCompileFile[0] = '\0';
			else
				fclose(f);
		}
	}

    if(compileAs || strlen(CurrentCompileFile)==0) 
	{
        OPENFILENAME ofn;

		if(strlen(CurrentSaveFile) && !strlen(CurrentCompileFile)) {
			strcpy(CurrentCompileFile, CurrentSaveFile);
			ChangeFileExtension(CurrentCompileFile, "hex");
		}

        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hInstance = Instance;
        ofn.lpstrTitle = _("Compile To");
        if(Prog.mcu && Prog.mcu->whichIsa == ISA_ANSIC) {
            ofn.lpstrFilter = C_PATTERN;
            ofn.lpstrDefExt = "c";
        } else if(Prog.mcu && Prog.mcu->whichIsa == ISA_INTERPRETED) {
            ofn.lpstrFilter = INTERPRETED_PATTERN;
            ofn.lpstrDefExt = "int";
        } else {
            ofn.lpstrFilter = HEX_PATTERN;
            ofn.lpstrDefExt = "hex";
        }
        ofn.lpstrFile = CurrentCompileFile;
        ofn.nMaxFile = sizeof(CurrentCompileFile);
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

        if(!GetSaveFileName(&ofn)) {
			CurrentCompileFile[0] = '\0';
            goto CompileProgramEnd;
		} else if(ofn.Flags & OFN_EXTENSIONDIFFERENT) {
			if(MessageBox(MainWindow, _("Tipo de arquivo deve ser .hex\nA extens�o ser� alterada automaticamente."),"Aviso", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL) {
				strcpy(CurrentCompileFile, "");
				goto CompileProgramEnd;
			}

		ChangeFileExtension(CurrentCompileFile, "hex");
		}

        // hex output filename is stored in the .ld file
        ProgramChangedNotSaved = TRUE;
    }

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

	StatusBarSetText(0, "Compilando... aguarde");

    SetCursor(LoadCursor(NULL, IDC_WAIT));
  //  switch(Prog.mcu->whichIsa) {
  //      case ISA_ANSIC:         CompileAnsiC(CurrentCompileFile); break;
  //      case ISA_INTERPRETED:   CompileInterpreted(CurrentCompileFile); break;
  //      case ISA_LPC17:         CompileAnsiCToGCC(CurrentCompileFile); break;
		//case ISA_AVR:
		//case ISA_PIC16:
		//	break;
  //      default: oops();
  //  }
	if(CompileAnsiCToGCC(CurrentCompileFile) == 0)
		ret = TRUE;

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	StatusBarSetText(0, "");

//    IntDumpListing("t.pl");

CompileProgramEnd:
	return ret;
}

//-----------------------------------------------------------------------------
// Compile the program to a hex file for the target micro. Get the output
// file name if necessary, then call the micro-specific compile routines.
//-----------------------------------------------------------------------------
static void WriteProgram(BOOL compileAs)
{
	unsigned int iCOMPort;
	BOOL PreviousRunningInBatchMode = RunningInBatchMode;

	if(!POPSettings.COMPortFlash) {
		iCOMPort = LoadCOMPorts(0, 0, 0);
		if(!iCOMPort) {
			Error("POP-7 n�o encontrada!");
			return;
		}
	} else {
		iCOMPort = POPSettings.COMPortFlash;
	}

	RunningInBatchMode = TRUE;
    if (CompileProgram(compileAs)) 
	{
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		RunningInBatchMode = PreviousRunningInBatchMode;
		FlashProgram(CurrentCompileFile, iCOMPort, 230400);
	} 
	else 
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		RunningInBatchMode = PreviousRunningInBatchMode;

		StatusBarSetText(0, "Erro na compilacao !!!");

		if(strlen(CurrentCompileFile))
			CompileProgram(false);
//		CompileSuccessfulMessage("Erro na compilacao !!! ");
	}

	RunningInBatchMode = PreviousRunningInBatchMode;

	StatusBarSetText(0, "");

	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

//-----------------------------------------------------------------------------
// If the program has been modified then give the user the option to save it
// or to cancel the operation they are performing. Return TRUE if they want
// to cancel.
//-----------------------------------------------------------------------------
BOOL CheckSaveUserCancels(void)
{
    if(!ProgramChangedNotSaved) {
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
    OPENFILENAME ofn;

    char tempSaveFile[MAX_PATH] = "";

	if(filename != NULL && strlen(filename)) {
		strcpy(tempSaveFile, filename);
	} else {
	    memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
	    ofn.hInstance = Instance;
		ofn.lpstrFilter = LDMICRO_PATTERN;
	    ofn.lpstrDefExt = "ld";
		ofn.lpstrFile = tempSaveFile;
	    ofn.nMaxFile = sizeof(tempSaveFile);
		ofn.hwndOwner = MainWindow;
	    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	    if(!GetOpenFileName(&ofn))
		    return;
	}

    if(!LoadProjectFromFile(tempSaveFile)) {
        Error(_("Couldn't open '%s'."), tempSaveFile);
        CurrentSaveFile[0] = '\0';
    } else {
        ProgramChangedNotSaved = FALSE;
        strcpy(CurrentSaveFile, tempSaveFile);
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
#define CHANGING_PROGRAM(x) { \
        UndoRemember(); \
        x; \
        ProgramChanged(); \
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
			PopulateRecentListMenu();
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

        case MNU_EXPORT:
            ExportDialog();
            break;

        case MNU_EXIT:
            if(CheckSaveUserCancels()) break;
            PostQuitMessage(0);
            break;

        case MNU_INSERT_COMMENT:
			if(CanInsertComment)
				CHANGING_PROGRAM(AddComment(_("--add comment here--")));
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
        
        case MNU_INSERT_FMTD_STR:
            CHANGING_PROGRAM(AddFormattedString());
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

        case MNU_INSERT_READ_MODBUS_ETH:
            CHANGING_PROGRAM(AddReadModbusEth());
            break;

        case MNU_INSERT_WRITE_MODBUS_ETH:
            CHANGING_PROGRAM(AddWriteModbusEth());
            break;

        case MNU_INSERT_UART_SEND:
            CHANGING_PROGRAM(AddUart(ELEM_UART_SEND));
            break;

        case MNU_INSERT_UART_RECV:
            CHANGING_PROGRAM(AddUart(ELEM_UART_RECV));
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
            break;

        case MNU_REDO:
            UndoRedo();
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
            CHANGING_PROGRAM(PushRungUp());
            break;

        case MNU_PUSH_RUNG_DOWN:
            CHANGING_PROGRAM(PushRungDown());
            break;

        case MNU_DELETE_ELEMENT:
            CHANGING_PROGRAM(DeleteSelectedFromProgram());
            break;

        case MNU_FIND_AND_REPLACE:
			CHANGING_PROGRAM(ShowFARDialog());
            break;

        case MNU_FIND_AND_REPLACE_NEXT:
			FindAndReplace(NULL, NULL, FAR_FIND_NEXT);
            break;

        case MNU_MCU_SETTINGS:
            CHANGING_PROGRAM(ShowConfDialog(false));
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

        case MNU_STOP_SIMULATION:
            StopSimulation();
            break;

        case MNU_SINGLE_CYCLE:
            SimulateOneCycle(TRUE);
            break;

        case MNU_COMPILE:
            if(CheckSaveUserCancels()) break;
            CompileProgram(FALSE);
            break;

        case MNU_COMPILE_AS:
            if(CheckSaveUserCancels()) break;
            CompileProgram(TRUE);
            break;

        case MNU_PROGRAM:
            if(CheckSaveUserCancels()) break;
            WriteProgram(FALSE);
            break;

        case MNU_PROGRAM_AS:
            if(CheckSaveUserCancels()) break;
            WriteProgram(TRUE);
            break;

		case MNU_DEBUG:
			ShowDebugDialog();
			break;

        case MNU_MANUAL:
            ShowHelpDialog(FALSE);
            break;

        case MNU_ABOUT:
            //ShowHelpDialog(TRUE);
			mysplash.Show();
            break;
    }
}

//-----------------------------------------------------------------------------
// WndProc for MainWindow.
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int i, emptyRung;

    switch (msg) {
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
            PaintWindow();

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

            EndPaint(DrawWindow, &ps);
            return 1;
        }

        case WM_KEYDOWN: {

			/*if((GetAsyncKeyState(VK_SHIFT) & 0x8000) && 
				(wParam == 0x31)) {
				CHAR msg[50];
				sprintf(msg, "Codigo da Tecla: wParam:0x%x, lParam:0x%x\n", wParam, lParam);
				MessageBox(hwnd, msg, "Tecla digitada", NULL);
			}*/

			if(wParam == 'M') {
				if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
					ToggleSimulationMode();
					break;
                }
            } else if(wParam == VK_TAB) {
                SetFocus(IoList);
                BlinkCursor(0, 0, 0, 0);
                break;
            } else if(wParam == VK_F1) {
                ShowHelpDialog(FALSE);
                break;
            }

            if(InSimulationMode) {
                switch(wParam) {
                    case ' ':
                        SimulateOneCycle(TRUE);
                        break;

                    case 'R':
                        if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
                            StartSimulation();
                        break;

                    case 'H':
                        if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
                            StopSimulation();
                        break;

                    case VK_DOWN:
                        if(ScrollYOffset < ScrollYOffsetMax)
                            ScrollYOffset++;
                        RefreshScrollbars();
                        InvalidateRect(MainWindow, NULL, FALSE);
                        break;

                    case VK_UP:
                        if(ScrollYOffset > 0)
                            ScrollYOffset--;
                        RefreshScrollbars();
                        InvalidateRect(MainWindow, NULL, FALSE);
                        break;

                    case VK_LEFT:
                        ScrollXOffset -= FONT_WIDTH;
                        if(ScrollXOffset < 0) ScrollXOffset = 0;
                        RefreshScrollbars();
                        InvalidateRect(MainWindow, NULL, FALSE);
                        break;

                    case VK_RIGHT:
                        ScrollXOffset += FONT_WIDTH;
                        if(ScrollXOffset >= ScrollXOffsetMax)
                            ScrollXOffset = ScrollXOffsetMax;
                        RefreshScrollbars();
                        InvalidateRect(MainWindow, NULL, FALSE);
                        break;

                    case VK_RETURN:
                    case VK_ESCAPE:
						if(!POPSettings.ShowSimulationWarnings || ShowTaskDialog(L"Tem certeza que deseja sair da simula��o?", L"O processo ser� interrompido", TD_WARNING_ICON, TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, L"Sempre mostrar avisos da simula��o", &POPSettings.ShowSimulationWarnings) == IDYES) {
	                        ToggleSimulationMode();
						}
	                    break;
                }
                break;
            }

            switch(wParam) {
                case VK_F3:
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						if(!FindAndReplace(NULL, NULL, FAR_FIND_NEXT))
							FindAndReplace(NULL, NULL, FAR_FIND_FIRST);
					} else {
						CHANGING_PROGRAM(ShowFARDialog());
					}
                    break;

                case VK_F5:
                    CompileProgram(FALSE);
                    break;

                case VK_UP:
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        CHANGING_PROGRAM(PushRungUp());
                    } else {
                        MoveCursorKeyboard(wParam);
                    }
                    break;

                case VK_DOWN:
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        CHANGING_PROGRAM(PushRungDown());
                    } else {
                        MoveCursorKeyboard(wParam);
                    }
                    break;

                case VK_RIGHT:
                case VK_LEFT:
                    MoveCursorKeyboard(wParam);
                    break;

                case VK_RETURN:
                    CHANGING_PROGRAM(EditSelectedElement());
                    break;

				case VK_BACK:
                case VK_DELETE:
					i = RungContainingSelected();
					emptyRung = i<0 ? 0 : Prog.rungs[i]->count == 1 && Prog.rungs[i]->contents[0].which == ELEM_PLACEHOLDER;

                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000 || emptyRung) {
                        CHANGING_PROGRAM(DeleteSelectedRung());
                    } else {
                        CHANGING_PROGRAM(DeleteSelectedFromProgram());
                    }
                    break;

				case 191: // VK_F2:
					if(CanInsertComment)
						CHANGING_PROGRAM(AddComment(_("--add comment here--")));
                    break;

                case 'C':
					if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						CHANGING_PROGRAM(AddCounter(ELEM_CTC));
					} else if(!(GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
						CHANGING_PROGRAM(AddContact());
					}
                    break;

                case 'B':
					if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						CHANGING_PROGRAM(AddCheckBit());
					} else {
						CHANGING_PROGRAM(AddSetBit());
					}
                    break;

				// TODO: rather country-specific here

				case 'K':
					if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						CHANGING_PROGRAM(AddReadServoYaskawa());
					} else if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
						CHANGING_PROGRAM(AddWriteServoYaskawa());
					}
                case 'L':
                    CHANGING_PROGRAM(AddCoil());
                    break;

                case 'R':
					if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
						CHANGING_PROGRAM(AddMultisetDA());
					} else {
						CHANGING_PROGRAM(MakeResetOnlySelected());
					}
                    break;

                case 'E':
                    if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                        ExportDialog();
                    } else {
                        CHANGING_PROGRAM(AddReset());
                    }
                    break;

                case 'S':
                    if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                        SaveProgram();
                        UpdateMainWindowTitleBar();
                    } else {
                        CHANGING_PROGRAM(MakeSetOnlySelected());
                    }
                    break;

                case 'N':
                    if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                        if(CheckSaveUserCancels()) break;
                        if(!ProgramChangedNotSaved) {
                            int r = MessageBox(MainWindow, 
                                _("Start new program?"),
                                "POPTools", MB_YESNO | MB_DEFBUTTON2 |
                                MB_ICONQUESTION);
                            if(r == IDNO) break;
                        }
                        NewProgram();
                        strcpy(CurrentSaveFile, "");
                        strcpy(CurrentCompileFile, "");
                        GenerateIoListDontLoseSelection();
                        RefreshScrollbars();
                        UpdateMainWindowTitleBar();
                    } else {
                        CHANGING_PROGRAM(NegateSelected());
                    }
                    break;

                case 'A':
					if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
						CHANGING_PROGRAM(AddReadAdc());
					} else {
						CHANGING_PROGRAM(MakeNormalSelected());
					}
                    break;

                case 'T':
                    CHANGING_PROGRAM(AddTimer(ELEM_RTO));
                    break;

                case 'O':
                    if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                        if(CheckSaveUserCancels()) break;
                        OpenDialog(NULL);
                    } else {
                        CHANGING_PROGRAM(AddTimer(ELEM_TON));
                    }
                    break;

                case 'F':
                    CHANGING_PROGRAM(AddTimer(ELEM_TOF));
                    break;

                case 'I':
                    CHANGING_PROGRAM(AddCounter(ELEM_CTU));
                    break;

                case 'U':
					if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						CHANGING_PROGRAM(AddReadUSS());
					} else if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
						CHANGING_PROGRAM(AddWriteUSS());
					}
                    break;

                case '4':
					if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						CHANGING_PROGRAM(AddReadModbus());
					} else if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
						CHANGING_PROGRAM(AddWriteModbus());
					}
                    break;

                case 'X':
					if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						CHANGING_PROGRAM(AddReadModbusEth());
					} else if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
						CHANGING_PROGRAM(AddWriteModbusEth());
					}
                    break;

                case 'J':
                    CHANGING_PROGRAM(AddCounter(ELEM_CTC));
                    break;

                case 'M':
                    CHANGING_PROGRAM(AddMove());
                    break;

                /*case 'P':
                    CHANGING_PROGRAM(AddReadAdc());
                    break;*/

				case 'Q':
					if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
						CHANGING_PROGRAM(AddResetEnc());
					} else {
						CHANGING_PROGRAM(AddReadEnc());
					}
                    break;

				case 'W':
					CHANGING_PROGRAM(AddSetPwm());
					break;

				case VK_OEM_PLUS:
				case VK_ADD:
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        CHANGING_PROGRAM(AddMath(ELEM_ADD));
                    } else {
                        CHANGING_PROGRAM(AddCmp(ELEM_EQU));
                    } 

                    break;

                case VK_OEM_MINUS:
				case VK_SUBTRACT:
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    } else {
                        CHANGING_PROGRAM(AddMath(ELEM_SUB));
                    }
                    break;

				case 0xc1:
				case 0x6f:
					if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						CHANGING_PROGRAM(AddMath(ELEM_DIV));
					} else {
						CHANGING_PROGRAM(AddEmpty(ELEM_ONE_SHOT_RISING));
					}
					break;
				case 0xe2:
					CHANGING_PROGRAM(AddEmpty(ELEM_ONE_SHOT_FALLING));
					break;
                case '8':
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        CHANGING_PROGRAM(AddMath(ELEM_MUL));
                    }
                    break;

                case 'D':
					if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
						CHANGING_PROGRAM(AddSetDA());
					} else {
						CHANGING_PROGRAM(AddCounter(ELEM_CTD));
					}
                    break;

				case 0x31:
					if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
						CHANGING_PROGRAM(AddCmp(ELEM_NEQ));
					}
					break;
                case VK_OEM_PERIOD:
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        CHANGING_PROGRAM(AddCmp(ELEM_GRT));
                    } else {
                        CHANGING_PROGRAM(AddCmp(ELEM_GEQ));
                    }
                    break;

                case VK_OEM_COMMA:
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        CHANGING_PROGRAM(AddCmp(ELEM_LES));
                    } else {
                        CHANGING_PROGRAM(AddCmp(ELEM_LEQ));
                    }
                    break;

                //case 'V':
                //    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                //        CHANGING_PROGRAM(InsertRung(TRUE));
                //    }
                //    break;

                case VK_INSERT:
                    if(GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        CHANGING_PROGRAM(InsertRung(FALSE));
                    }
					else {
                        CHANGING_PROGRAM(InsertRung(TRUE));
                    }
                    break;

                case 'Z':
                    if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                        UndoUndo();
                    }
                    break;

                case 'Y':
                    if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                        UndoRedo();
                    }
                    break;

                default:
                    break;
            }
            if(wParam != VK_SHIFT && wParam != VK_CONTROL) {
                InvalidateRect(MainWindow, NULL, FALSE);
            }
            break;
        }

        case WM_LBUTTONDBLCLK: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam) - RibbonHeight;
            if(InSimulationMode) {
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
    wc.lpszClassName    = "LDmicro";
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
		ShowTaskDialog(L"Erro ao carregar as prefer�ncias", L"Ser� utilizada a configura��o padr�o", TD_ERROR_ICON, TDCBF_OK_BUTTON);

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
	wchar_t COMPort[10];

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"SimulationSettings", 0);
	XmlSettings.SelectElements(L"ShowWarnings", 0);
	XmlSettings.ChangeElement(0, POPSettings.ShowSimulationWarnings ? L"1" : L"0");

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
	swprintf((wchar_t *)COMPort, 10, L"%d", POPSettings.COMPortFlash);
	XmlSettings.ChangeElement(0, COMPort);

	XmlSettings.ClearActiveSelection();
	XmlSettings.SelectElements(L"PortSettings", 0);
	XmlSettings.SelectElements(L"COMPortDebug", 0);
	swprintf((wchar_t *)COMPort, 10, L"%d", POPSettings.COMPortDebug);
	XmlSettings.ChangeElement(0, COMPort);

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

	Instance = hInstance;

    MainHeap = HeapCreate(0, 1024*64, 0);

	LoadSettings();

    MakeWindowClass();
	MakeDebugWindowClass();
	MakeDialogBoxClass();

	HMENU top = NULL;
#ifdef POPTOOLS_DISABLE_RIBBON	
	top = MakeMainWindowMenus();
#endif

    MainWindow = CreateWindowEx(0, "LDmicro", "",
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
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

#ifndef POPTOOLS_DISABLE_RIBBON	
	InitRibbon(MainWindow);
#endif

	NewProgram();
    strcpy(CurrentSaveFile, "");

    // Check if we're running in non-interactive mode; in that case we should
    // load the file, compile, and exit.
    while(isspace(*lpCmdLine)) {
        lpCmdLine++;
    }
    if(memcmp(lpCmdLine, "/c", 2)==0) {
        RunningInBatchMode = TRUE;

        char *err =
            "Bad command line arguments: run 'poptools /c src.ld dest.hex'";

        char *source = lpCmdLine + 2;
        while(isspace(*source)) {
            source++;
        }
        if(*source == '\0') { Error(err); exit(-1); }
        char *dest = source;
        while(!isspace(*dest) && *dest) {
            dest++;
        }
        if(*dest == '\0') { Error(err); exit(-1); }
        *dest = '\0'; dest++;
        while(isspace(*dest)) {
            dest++;
        }
        if(*dest == '\0') { Error(err); exit(-1); }
        if(!LoadProjectFromFile(source)) {
            Error("Couldn't open '%s', running non-interactively.", source);
            exit(-1);
        }
        strcpy(CurrentCompileFile, dest);
        GenerateIoMapList(-1);
        CompileProgram(FALSE);
        exit(0);
    }

    // We are running interactively, or we would already have exited. We
    // can therefore show the window now, and otherwise set up the GUI.

    ShowWindow(MainWindow, SW_SHOW);
    SetTimer(MainWindow, TIMER_BLINK_CURSOR, 800, BlinkCursor);
    
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
