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
// Common controls in the main window. The main window consists of the drawing
// area, where the ladder diagram is displayed, plus various controls for
// scrolling, I/O list, menus.
// Jonathan Westhues, Nov 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include "ldmicro.h"

// scrollbars for the ladder logic area
static HWND         HorizScrollBar;
static HWND         VertScrollBar;
int                 ScrollWidth;
int                 ScrollHeight;
BOOL                NeedHoriz;

// Window where to draw schematic
HWND                DrawWindow;
static LONG_PTR     PrevDrawWindowProc;

// status bar at the bottom of the screen, to display settings
static HWND         StatusBar;

// have to get back to the menus to gray/ungray, check/uncheck things
static HMENU        FileMenu;
static HMENU        RecentMenu;
static HMENU        EditMenu;
static HMENU        InstructionMenu;
static HMENU		ConditionalMenu;
static HMENU		MathematicMenu;
static HMENU		ComunicationMenu;
static HMENU		MotorControlMenu;
static HMENU        ProcessorMenu;
static HMENU        SimulateMenu;
static HMENU        TopMenu;

// listview used to maintain the list of I/O pins with symbolic names, plus
// the internal relay too
HWND                IoList;
static int          IoListSelectionPoint;
static BOOL         IoListOutOfSync;
int                 IoListHeight;
int                 IoListTop;

// Program Changed and Not Saved ?
extern BOOL ProgramChangedNotSaved;

// whether the simulation is running in real time
static BOOL         RealTimeSimulationRunning;

void StatusBarSetText(int bar, char * text)
{
	SendMessage(StatusBar, SB_SETTEXT, bar, (LPARAM)text);
}

static LRESULT CALLBACK DrawWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_PAINT:
			SendMessage(MainWindow, WM_PAINT, 0, 0);
			break;
	}

	return CallWindowProc((WNDPROC)PrevDrawWindowProc, hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Create the standard Windows controls used in the main window: a Listview
// for the I/O list, and a status bar for settings.
//-----------------------------------------------------------------------------
void MakeMainWindowControls(void)
{
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
#define LV_ADD_COLUMN(hWnd, i, w, s) do { \
        lvc.iSubItem = i; \
        lvc.pszText = s; \
        lvc.iOrder = 0; \
        lvc.cx = w; \
        ListView_InsertColumn(hWnd, i, &lvc); \
    } while(0)
    IoList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_CHILD |
        LVS_REPORT | LVS_SHOWSELALWAYS | WS_TABSTOP |
        LVS_SINGLESEL | WS_CLIPSIBLINGS, 
        12, 25, 300, 300, MainWindow, NULL, Instance, NULL);
    ListView_SetExtendedListViewStyle(IoList, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES);

    int typeWidth = 85;
    int pinWidth = 100;
    int portWidth = 90;
    
    LV_ADD_COLUMN(IoList, LV_IO_NAME,  250,       _("Name"));
    LV_ADD_COLUMN(IoList, LV_IO_TYPE,  typeWidth, _("Type"));
    LV_ADD_COLUMN(IoList, LV_IO_STATE, 100,       _("State"));
    LV_ADD_COLUMN(IoList, LV_IO_PIN,   pinWidth,  _("Pin on Processor"));
    LV_ADD_COLUMN(IoList, LV_IO_PORT,  portWidth, _("MCU Port"));

    HorizScrollBar = CreateWindowEx(0, WC_SCROLLBAR, "", WS_CHILD |
        SBS_HORZ | SBS_BOTTOMALIGN | WS_VISIBLE | WS_CLIPSIBLINGS, 
        100, 100, 100, 100, MainWindow, NULL, Instance, NULL);
    VertScrollBar = CreateWindowEx(0, WC_SCROLLBAR, "", WS_CHILD |
        SBS_VERT | SBS_LEFTALIGN | WS_VISIBLE | WS_CLIPSIBLINGS, 
        200, 100, 100, 100, MainWindow, NULL, Instance, NULL);
    RECT scroll;
    GetWindowRect(HorizScrollBar, &scroll);
    ScrollHeight = scroll.bottom - scroll.top;
    GetWindowRect(VertScrollBar, &scroll);
    ScrollWidth = scroll.right - scroll.left;

    DrawWindow = CreateWindowEx(0, WC_STATIC, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
		0, 0, 1, 1, MainWindow, NULL, Instance, NULL);
	PrevDrawWindowProc = SetWindowLongPtr(DrawWindow, GWLP_WNDPROC, (LONG_PTR)DrawWindowProc);
	RefreshDrawWindow();

	StatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
        "POPTools iniciado", MainWindow, 0);
    int edges[] = { 250, 370, -1 };
    SendMessage(StatusBar, SB_SETPARTS, 3, (LPARAM)edges);

    ShowWindow(IoList, SW_SHOW);
}

//-----------------------------------------------------------------------------
// Set up the title bar text for the main window; indicate whether we are in
// simulation or editing mode, and indicate the filename.
//-----------------------------------------------------------------------------
struct VS_VERSIONINFO
{
    WORD                wLength;
    WORD                wValueLength;
    WORD                wType;
    WCHAR               szKey[1];
    WORD                wPadding1[1];
    VS_FIXEDFILEINFO    Value;
    WORD                wPadding2[1];
    WORD                wChildren[1];
};

struct
{
    WORD wLanguage;
    WORD wCodePage;
} *lpTranslate;

// these macros help to align on r-byte boundaries (thanks Ted Peck)
#define roundoffs(a,b,r) (((BYTE *) (b) - (BYTE *) (a) + ((r) - 1)) & ~((r) - 1))
#define roundpos(a,b,r) (((BYTE *) (a)) + roundoffs(a,b,r))

void UpdateMainWindowTitleBar(void)
{
	LPCTSTR lpszFilename = TEXT("POPTools.exe");
	DWORD   dwHandle,
			dwSize;

	WORD wMajor, wMinor, wBuild, wRevision;
	//DWORD dwFileVersionMS, dwFileVersionLS, dwProductVersionMS, dwProductVersionLS;

	// determine the size of the resource information

	dwSize = GetFileVersionInfoSize(lpszFilename, &dwHandle);
	if (0 < dwSize)
	{
		LPBYTE lpBuffer = new BYTE[dwSize];

		GetFileVersionInfo(lpszFilename, 0, dwSize, lpBuffer);

		// 'point to' the start of the version information block

		VS_VERSIONINFO *pVerInfo = (VS_VERSIONINFO *) lpBuffer;

		// the fixed section starts right after the 'VS_VERSION_INFO' string

		LPBYTE pOffsetBytes = (BYTE *) &pVerInfo->szKey[wcslen(pVerInfo->szKey) + 1];

		VS_FIXEDFILEINFO *pFixedInfo = (VS_FIXEDFILEINFO *) roundpos(pVerInfo, pOffsetBytes, 4);

        // increment the numbers!
        /*dwFileVersionMS    = HIWORD(pFixedInfo->dwFileVersionMS);
        dwFileVersionLS    = LOWORD(pFixedInfo->dwFileVersionLS);
        dwProductVersionMS = HIWORD(pFixedInfo->dwProductVersionMS);
        dwProductVersionLS = LOWORD(pFixedInfo->dwProductVersionLS);*/

		wMajor    = HIWORD(pFixedInfo->dwFileVersionMS);
        wMinor    = LOWORD(pFixedInfo->dwFileVersionMS);
		wBuild    = HIWORD(pFixedInfo->dwFileVersionLS);
        wRevision = LOWORD(pFixedInfo->dwFileVersionLS);

		delete [] lpBuffer;
	}

    char line[MAX_PATH+100];
    if(InSimulationMode) {
        if(RealTimeSimulationRunning) {
            strcpy(line, _("LDmicro - Simulation (Running)"));
        } else {
            strcpy(line, _("LDmicro - Simulation (Stopped)"));
        }
    } else {
        strcpy(line, _("LDmicro - Program Editor"));
    }
    if(strlen(CurrentSaveFile) > 0) {
        sprintf(line+strlen(line), " - %d.%d.%d.%d - %s%s", 
			wMajor,
			wMinor,
			wBuild,
			wRevision,
			CurrentSaveFile,
			ProgramChangedNotSaved ? " *" : "");
    } else {
        sprintf(line+strlen(line), " - %d.%d.%d.%d%s", 
			wMajor,
			wMinor,
			wBuild,
			wRevision,
			_(" - (not yet saved)"));
    }

    SetWindowText(MainWindow, line);
}

//-----------------------------------------------------------------------------
// Set the enabled state of the logic menu items to reflect where we are on
// the schematic (e.g. can't insert two coils in series).
//-----------------------------------------------------------------------------
void SetMenusEnabled(BOOL canNegate, BOOL canNormal, BOOL canResetOnly,
    BOOL canSetOnly, BOOL canDelete, BOOL canInsertEnd, BOOL canInsertOther,
    BOOL canPushDown, BOOL canPushUp, BOOL canInsertComment)
{
    EnableMenuItem(EditMenu, MNU_PUSH_RUNG_UP,
        canPushUp ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, MNU_PUSH_RUNG_DOWN,
        canPushDown ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, MNU_DELETE_RUNG,
        (Prog.numRungs > 1) ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(InstructionMenu, MNU_NEGATE,
        canNegate ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MNU_MAKE_NORMAL,
        canNormal ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MNU_MAKE_RESET_ONLY,
        canResetOnly ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(InstructionMenu, MNU_MAKE_SET_ONLY,
        canSetOnly ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(InstructionMenu, MNU_INSERT_COMMENT,
        canInsertComment ? MF_ENABLED : MF_GRAYED);

    EnableMenuItem(EditMenu, MNU_DELETE_ELEMENT,
        canDelete ? MF_ENABLED : MF_GRAYED);

    int t;
    t = canInsertEnd ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(InstructionMenu, MNU_INSERT_COIL, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_RES, t);
    EnableMenuItem(MathematicMenu, MNU_INSERT_MOV, t);
    EnableMenuItem(MathematicMenu, MNU_INSERT_ADD, t);
    EnableMenuItem(MathematicMenu, MNU_INSERT_SUB, t);
    EnableMenuItem(MathematicMenu, MNU_INSERT_MUL, t);
    EnableMenuItem(MathematicMenu, MNU_INSERT_DIV, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_CTC, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_PERSIST, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_READ_ADC, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_SET_PWM, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_MASTER_RLY, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_SHIFT_REG, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_LUT, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_PWL, t);

    t = canInsertOther ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(InstructionMenu, MNU_INSERT_TON, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_TOF, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_OSR, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_OSF, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_RTO, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_CONTACTS, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_CTU, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_CTD, t);
    EnableMenuItem(ConditionalMenu, MNU_INSERT_EQU, t);
    EnableMenuItem(ConditionalMenu, MNU_INSERT_NEQ, t);
    EnableMenuItem(ConditionalMenu, MNU_INSERT_GRT, t);
    EnableMenuItem(ConditionalMenu, MNU_INSERT_GEQ, t);
    EnableMenuItem(ConditionalMenu, MNU_INSERT_LES, t);
    EnableMenuItem(ConditionalMenu, MNU_INSERT_LEQ, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_SHORT, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_OPEN, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_UART_SEND, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_UART_RECV, t);
    EnableMenuItem(InstructionMenu, MNU_INSERT_FMTD_STR, t);
}

//-----------------------------------------------------------------------------
// Set the enabled state of the undo/redo menus.
//-----------------------------------------------------------------------------
void SetUndoEnabled(BOOL undoEnabled, BOOL redoEnabled)
{
    EnableMenuItem(EditMenu, MNU_UNDO, undoEnabled ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, MNU_REDO, redoEnabled ? MF_ENABLED : MF_GRAYED);
}

void PopulateRecentListMenu(void)
{
	int i, len, mid;
	char menu_text[MAX_RECENT_MENU_LEN+1];

	// Clear all current items
	for(i=0; i<MAX_RECENT_ITEMS; i++) {
		DeleteMenu(RecentMenu, MNU_RECENT_START + i, 0);
	}

	DeleteMenu(RecentMenu, 0, 0);
	DeleteMenu(RecentMenu, MNU_RECENT_CLEAR, 0);

	if(POPSettings.recent_list[0] != NULL && strlen(POPSettings.recent_list[0])) {
		for(i=0; i<MAX_RECENT_ITEMS; i++) {
			if(POPSettings.recent_list[i]==NULL || !strlen(POPSettings.recent_list[i]))
				break;

			len = strlen(POPSettings.recent_list[i]);
			mid = MAX_RECENT_MENU_LEN/2;
			if(len > MAX_RECENT_MENU_LEN) {
				strncpy(menu_text      , POPSettings.recent_list[i]        , mid-3);
				strncpy(menu_text+mid  , POPSettings.recent_list[i]+len-mid, mid  );
				strncpy(menu_text+mid-3, "...", 3);
				menu_text[MAX_RECENT_MENU_LEN] = 0;
			} else {
				strcpy(menu_text, POPSettings.recent_list[i]);
			}
			AppendMenu(RecentMenu, MF_STRING, MNU_RECENT_START + i, menu_text);
		}

		AppendMenu(RecentMenu, MF_SEPARATOR,0, "");
		AppendMenu(RecentMenu, MF_STRING, MNU_RECENT_CLEAR, _("Limpar Recentes"));
	} else {
		AppendMenu(RecentMenu, MF_STRING | MF_GRAYED, MNU_RECENT_CLEAR, _("Vazio..."));
	}

	DrawMenuBar(MainWindow);
}

//-----------------------------------------------------------------------------
// Create the top-level menu bar for the main window. Mostly static, but we
// create the "select processor" menu from the list in mcutable.h dynamically.
//-----------------------------------------------------------------------------
HMENU MakeMainWindowMenus(void)
{
    HMENU settings, compile, help;

#ifndef POP7_ONLY
    int i;
#endif

    FileMenu = CreatePopupMenu();
    AppendMenu(FileMenu, MF_STRING,   MNU_NEW,    _("&New\tCtrl+N"));
    AppendMenu(FileMenu, MF_STRING,   MNU_OPEN,   _("&Open...\tCtrl+O"));

	RecentMenu = CreatePopupMenu();
	PopulateRecentListMenu();
	AppendMenu(FileMenu, MF_STRING | MF_POPUP, (UINT_PTR)RecentMenu, _("Projetos Recentes"));

	AppendMenu(FileMenu, MF_STRING,   MNU_SAVE,   _("&Save\tCtrl+S"));
    AppendMenu(FileMenu, MF_STRING,   MNU_SAVE_AS,_("Save &As..."));
    AppendMenu(FileMenu, MF_SEPARATOR,0,          "");
    AppendMenu(FileMenu, MF_STRING,   MNU_EXPORT,
        _("&Export As Text...\tCtrl+E"));
    AppendMenu(FileMenu, MF_SEPARATOR,0,          "");
    AppendMenu(FileMenu, MF_STRING,   MNU_EXIT,   _("E&xit"));

    EditMenu = CreatePopupMenu();
    AppendMenu(EditMenu, MF_STRING, MNU_UNDO, _("&Undo\tCtrl+Z"));
    AppendMenu(EditMenu, MF_STRING, MNU_REDO, _("&Redo\tCtrl+Y"));
    AppendMenu(EditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(EditMenu, MF_STRING, MNU_INSERT_RUNG_BEFORE,
        _("Insert Rung &Before\tShift+INS"));
    AppendMenu(EditMenu, MF_STRING, MNU_INSERT_RUNG_AFTER,
        _("Insert Rung &After\tINS"));
    AppendMenu(EditMenu, MF_STRING, MNU_PUSH_RUNG_UP,
        _("Move Selected Rung &Up\tShift+Up"));
    AppendMenu(EditMenu, MF_STRING, MNU_PUSH_RUNG_DOWN,
        _("Move Selected Rung &Down\tShift+Down"));
    AppendMenu(EditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(EditMenu, MF_STRING, MNU_DELETE_ELEMENT,
        _("&Delete Selected Element\tDel"));
    AppendMenu(EditMenu, MF_STRING, MNU_DELETE_RUNG,
        _("D&elete Rung\tShift+Del"));
    AppendMenu(EditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(EditMenu, MF_STRING, MNU_FIND_AND_REPLACE,
        _("Localizar e Substituir...\tF3"));
    AppendMenu(EditMenu, MF_STRING, MNU_FIND_AND_REPLACE_NEXT,
        _("Localizar próxima...\tShift+F3"));

    InstructionMenu = CreatePopupMenu();
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_COMMENT,
        _("Insert Co&mment\tF2"));
    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_CONTACTS,
        _("Insert &Contacts\tC"));
    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_OSR,
        _("Insert OSR (One Shot Rising)\t/"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_OSF,
        _("Insert OSF (One Shot Falling)\t\\"));
    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_TON,
        _("Insert T&ON (Delayed Turn On)\tO"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_TOF,
        _("Insert TO&F (Delayed Turn Off)\tF"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_RTO,
        _("Insert R&TO (Retentive Delayed Turn On)\tT"));
	AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_RTC,
        _("Inserir RTC (Agendador de Tarefa)"));
	AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_CTU,
        _("Insert CT&U (Count Up)\tI"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_CTD,
        _("Insert CT&D (Count Down)\tD"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_CTC,
        _("Insert CT&C (Count Circular)\tShift+C"));
    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);

    //AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_OPEN,
        _("Insert Open-Circuit"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_SHORT,
        _("Insert Short-Circuit"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_MASTER_RLY,
        _("Insert Master Control Relay"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_SET_BIT, 
        _("Set/Reset BIT\tB"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_CHECK_BIT, 
        _("Check BIT\tShift+B"));
    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_COIL,
        _("Insert Coi&l\tL"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_RES,
        _("Insert R&ES (Counter/RTO Reset)\tE"));
    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_MOV,
        _("Insert MOV (Move)\tM"));
    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);

	ConditionalMenu = CreatePopupMenu();
    AppendMenu(ConditionalMenu, MF_STRING, MNU_INSERT_EQU,
        _("Insert EQU (Compare for Equals)\t="));
    AppendMenu(ConditionalMenu, MF_STRING, MNU_INSERT_NEQ,
        _("Insert NEQ (Compare for Not Equals)\t!"));
    AppendMenu(ConditionalMenu, MF_STRING, MNU_INSERT_GRT,
        _("Insert GRT (Compare for Greater Than)\t>"));
    AppendMenu(ConditionalMenu, MF_STRING, MNU_INSERT_GEQ,
        _("Insert GEQ (Compare for Greater Than or Equal)\t."));
    AppendMenu(ConditionalMenu, MF_STRING, MNU_INSERT_LES,
        _("Insert LES (Compare for Less Than)\t<"));
    AppendMenu(ConditionalMenu, MF_STRING, MNU_INSERT_LEQ,
        _("Insert LEQ (Compare for Less Than or Equal)\t,"));
	AppendMenu(InstructionMenu, MF_STRING | MF_POPUP, (UINT_PTR)ConditionalMenu,
        _("Instruções Condicionais"));

	MathematicMenu = CreatePopupMenu();
	AppendMenu(MathematicMenu, MF_STRING, MNU_INSERT_ADD,
        _("Insert ADD (32-bit Integer Add)\t+"));
    AppendMenu(MathematicMenu, MF_STRING, MNU_INSERT_SUB,
        _("Insert SUB (32-bit Integer Subtract)\t-"));
    AppendMenu(MathematicMenu, MF_STRING, MNU_INSERT_MUL,
        _("Insert MUL (32-bit Integer Multiply)\t*"));
    AppendMenu(MathematicMenu, MF_STRING, MNU_INSERT_DIV,
        _("Insert DIV (32-bit Integer Divide)\tShift+/"));
	AppendMenu(InstructionMenu, MF_STRING | MF_POPUP, (UINT_PTR)MathematicMenu,
        _("Instruções Matemáticas"));

    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_SHIFT_REG,
        _("Insert Shift Register"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_LUT,
        _("Insert Look-Up Table"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_PWL,
        _("Insert Piecewise Linear"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_READ_ADC,
        _("Insert A/D Converter Read\tCtrl+A"));

    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);

	MotorControlMenu = CreatePopupMenu();
	AppendMenu(MotorControlMenu, MF_STRING, MNU_INSERT_SET_PWM,
        _("Insert Set PWM Output\tW"));
    AppendMenu(MotorControlMenu, MF_STRING, MNU_INSERT_SET_DA,
        _("Conversor Digital/Analogico (D/A)\tCtrl+D"));
    AppendMenu(MotorControlMenu, MF_STRING, MNU_INSERT_READ_ENC,
        _("Leitura do Encoder Quadratura\tQ"));
    AppendMenu(MotorControlMenu, MF_STRING, MNU_INSERT_RESET_ENC,
        _("Reset do Encoder Quadratura\tCtrl+Q"));
    AppendMenu(MotorControlMenu, MF_STRING, MNU_INSERT_MULTISET_DA,
        _("Rampa Aceleração/Desaceleração\tCtrl+R"));
    //AppendMenu(MotorControlMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(InstructionMenu, MF_STRING | MF_POPUP, (UINT_PTR)MotorControlMenu,
        _("Controle de Motor"));

	AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);

	ComunicationMenu = CreatePopupMenu();
    AppendMenu(ComunicationMenu, MF_STRING, MNU_READ_FMTD_STR,
        _("Receive Formatted String Over UART"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_WRITE_FMTD_STR,
        _("Send Formatted String Over UART"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_READ_SERVO_YASKAWA,
        _("Leitura NS-600 Yaskawa\tShift+K"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_WRITE_SERVO_YASKAWA,
        _("Escrita NS-600 Yaskawa\tCtrl+K"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_INSERT_UART_SEND,
        _("Insert &UART Send"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_INSERT_UART_RECV,
        _("Insert &UART Receive"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_INSERT_READ_USS,
        _("Leitura de Parametro do USS\tShift+U"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_INSERT_WRITE_USS,
        _("Escrita de Parametro do USS\tCtrl+U"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_INSERT_READ_MODBUS,
        _("Leitura de registrador do MODBUS RS485\tShift+4"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_INSERT_WRITE_MODBUS,
        _("Escrita em registrador do MODBUS RS485\tCtrl+4"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_INSERT_READ_MODBUS_ETH,
        _("Leitura de registrador do MODBUS Ethernet\tShift+X"));
    AppendMenu(ComunicationMenu, MF_STRING, MNU_INSERT_WRITE_MODBUS_ETH,
        _("Escrita em registrador do MODBUS Ethernet\tCtrl+X"));
	AppendMenu(InstructionMenu, MF_STRING | MF_POPUP, (UINT_PTR)ComunicationMenu,
        _("Comunicação"));

    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(InstructionMenu, MF_STRING, MNU_INSERT_PERSIST,
        _("Insert Make Persistent"));
    AppendMenu(InstructionMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(InstructionMenu, MF_STRING, MNU_MAKE_NORMAL,
        _("Make Norm&al\tA"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_NEGATE,
        _("Make &Negated\tN"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_MAKE_SET_ONLY,
        _("Make &Set-Only\tS"));
    AppendMenu(InstructionMenu, MF_STRING, MNU_MAKE_RESET_ONLY,
        _("Make &Reset-Only\tR"));

    settings = CreatePopupMenu();
    AppendMenu(settings, MF_STRING, MNU_MCU_SETTINGS, _("&MCU Parameters..."));
#ifndef POP7_ONLY
    ProcessorMenu = CreatePopupMenu();
    for(i = 0; i < NUM_SUPPORTED_MCUS; i++) {
        AppendMenu(ProcessorMenu, MF_STRING, MNU_PROCESSOR_0+i, 
            SupportedMcus[i].mcuName);
    }
    AppendMenu(ProcessorMenu, MF_STRING, MNU_PROCESSOR_0+i, 
        _("(no microcontroller)"));
    AppendMenu(settings, MF_STRING | MF_POPUP, (UINT_PTR)ProcessorMenu,
        _("&Microcontroller"));
#endif
	AppendMenu(settings, MF_SEPARATOR, 0, NULL);
    AppendMenu(settings, MF_STRING, MNU_MCU_PREFERENCES, _("Preferências"));

    SimulateMenu = CreatePopupMenu();
    AppendMenu(SimulateMenu, MF_STRING, MNU_SIMULATION_MODE,
        _("Si&mulation Mode\tCtrl+M"));
    AppendMenu(SimulateMenu, MF_STRING | MF_GRAYED, MNU_START_SIMULATION,
        _("Start &Real-Time Simulation\tCtrl+R"));
    AppendMenu(SimulateMenu, MF_STRING | MF_GRAYED, MNU_STOP_SIMULATION,
        _("&Halt Simulation\tCtrl+H"));
    AppendMenu(SimulateMenu, MF_STRING | MF_GRAYED, MNU_SINGLE_CYCLE,
        _("Single &Cycle\tSpace"));

    compile = CreatePopupMenu();
    AppendMenu(compile, MF_STRING, MNU_COMPILE, _("&Compile\tF5"));
    AppendMenu(compile, MF_STRING, MNU_COMPILE_AS, _("Compile &As..."));
	AppendMenu(compile, MF_SEPARATOR, 0, NULL);
    AppendMenu(compile, MF_STRING, MNU_PROGRAM, _("&Gravar\tF6"));
    //AppendMenu(compile, MF_STRING, MNU_PROGRAM_AS, _("Gravar &como..."));
	AppendMenu(compile, MF_SEPARATOR, 0, NULL);
    AppendMenu(compile, MF_STRING, MNU_DEBUG, _("&Depurar\tF7"));

    help = CreatePopupMenu();
    //AppendMenu(help, MF_STRING, MNU_MANUAL, _("&Manual...\tF1"));
//#ifdef ZERO
    AppendMenu(help, MF_STRING, MNU_ABOUT, _("&About..."));
//#endif

    TopMenu = CreateMenu();
    AppendMenu(TopMenu, MF_STRING | MF_POPUP, (UINT_PTR)FileMenu, _("&File"));
    AppendMenu(TopMenu, MF_STRING | MF_POPUP, (UINT_PTR)EditMenu, _("&Edit"));
    AppendMenu(TopMenu, MF_STRING | MF_POPUP, (UINT_PTR)settings,
        _("&Settings"));
    AppendMenu(TopMenu, MF_STRING | MF_POPUP, (UINT_PTR)InstructionMenu,
        _("&Instruction"));
    AppendMenu(TopMenu, MF_STRING | MF_POPUP, (UINT_PTR)SimulateMenu,
        _("Si&mulate"));
    AppendMenu(TopMenu, MF_STRING | MF_POPUP, (UINT_PTR)compile, 
        _("&Ações"));
    AppendMenu(TopMenu, MF_STRING | MF_POPUP, (UINT_PTR)help, _("&Help"));

    return TopMenu;
}

//-----------------------------------------------------------------------------
// Adjust the size and visibility of the scrollbars as necessary, either due
// to a change in the size of the program or a change in the size of the
// window.
//-----------------------------------------------------------------------------
void RefreshScrollbars(void)
{
    SCROLLINFO vert, horiz;
    SetUpScrollbars(&NeedHoriz, &horiz, &vert);
    SetScrollInfo(HorizScrollBar, SB_CTL, &horiz, TRUE);
    SetScrollInfo(VertScrollBar, SB_CTL, &vert, TRUE);

    RECT main;
    GetClientRect(MainWindow, &main);

    if(NeedHoriz) {
        MoveWindow(HorizScrollBar, 0, IoListTop - ScrollHeight - 2,
            main.right - ScrollWidth - 2, ScrollHeight, TRUE);
        ShowWindow(HorizScrollBar, SW_SHOW);
        EnableWindow(HorizScrollBar, TRUE);
    } else {
        ShowWindow(HorizScrollBar, SW_HIDE);
    }
    MoveWindow(VertScrollBar, main.right - ScrollWidth - 2, RibbonHeight + 1, ScrollWidth,
        NeedHoriz ? (IoListTop - ScrollHeight - RibbonHeight - 4) : (IoListTop - RibbonHeight - 3), TRUE);

//    MoveWindow(VertScrollBar, main.right - ScrollWidth - 2, 1, ScrollWidth,
//        NeedHoriz ? (IoListTop - ScrollHeight - 4) : (IoListTop - 3), TRUE);

    InvalidateRect(MainWindow, NULL, FALSE);
}

//-----------------------------------------------------------------------------
// Respond to a WM_VSCROLL sent to the main window, presumably by the one and
// only vertical scrollbar that it has as a child.
//-----------------------------------------------------------------------------
void VscrollProc(WPARAM wParam)
{
    int prevY = ScrollYOffset;
    switch(LOWORD(wParam)) {
        case SB_LINEUP:
        case SB_PAGEUP:
            if(ScrollYOffset > 0) {
                ScrollYOffset--;
            }
            break;

        case SB_LINEDOWN:
        case SB_PAGEDOWN:
            if(ScrollYOffset < ScrollYOffsetMax) {
                ScrollYOffset++;
            }
            break;

        case SB_TOP:
            ScrollYOffset = 0;
            break;

        case SB_BOTTOM:
            ScrollYOffset = ScrollYOffsetMax;
            break;

        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            ScrollYOffset = HIWORD(wParam);
            break;
    }
    if(prevY != ScrollYOffset) {
        SCROLLINFO si;
        si.cbSize = sizeof(si);
        si.fMask = SIF_POS;
        si.nPos = ScrollYOffset;
        SetScrollInfo(VertScrollBar, SB_CTL, &si, TRUE);

        InvalidateRect(MainWindow, NULL, FALSE);
    }
}

//-----------------------------------------------------------------------------
// Respond to a WM_HSCROLL sent to the main window, presumably by the one and
// only horizontal scrollbar that it has as a child.
//-----------------------------------------------------------------------------
void HscrollProc(WPARAM wParam)
{
    int prevX = ScrollXOffset;
    switch(LOWORD(wParam)) {
        case SB_LINEUP:
            ScrollXOffset -= FONT_WIDTH;
            break;

        case SB_PAGEUP:
            ScrollXOffset -= POS_WIDTH*FONT_WIDTH;
            break;

        case SB_LINEDOWN:
            ScrollXOffset += FONT_WIDTH;
            break;

        case SB_PAGEDOWN:
            ScrollXOffset += POS_WIDTH*FONT_WIDTH;
            break;

        case SB_TOP:
            ScrollXOffset = 0;
            break;

        case SB_BOTTOM:
            ScrollXOffset = ScrollXOffsetMax;
            break;

        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            ScrollXOffset = HIWORD(wParam);
            break;
    }

    if(ScrollXOffset > ScrollXOffsetMax) ScrollXOffset = ScrollXOffsetMax;
    if(ScrollXOffset < 0) ScrollXOffset = 0;

    if(prevX != ScrollXOffset) {
        SCROLLINFO si;
        si.cbSize = sizeof(si);
        si.fMask = SIF_POS;
        si.nPos = ScrollXOffset;
        SetScrollInfo(HorizScrollBar, SB_CTL, &si, TRUE);

        InvalidateRect(MainWindow, NULL, FALSE);
    }
}

//-----------------------------------------------------------------------------
// Cause the status bar and the list view to be in sync with the actual data
// structures describing the settings and the I/O configuration. Listview
// does callbacks to get the strings it displays, so it just needs to know
// how many elements to populate.
//-----------------------------------------------------------------------------
void RefreshControlsToSettings(void)
{
    int i;

    if(!IoListOutOfSync) {
        IoListSelectionPoint = -1;
        for(i = 0; i < Prog.io.count; i++) {
            if(ListView_GetItemState(IoList, i, LVIS_SELECTED)) {
                IoListSelectionPoint = i;
                break;
            }
        }
    }

    ListView_DeleteAllItems(IoList);
    for(i = 0; i < Prog.io.count; i++) {
        LVITEM lvi;
        lvi.mask        = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
        lvi.state       = lvi.stateMask = 0;
        lvi.iItem       = i;
        lvi.iSubItem    = 0;
        lvi.pszText     = LPSTR_TEXTCALLBACK;
        lvi.lParam      = i;

        if(ListView_InsertItem(IoList, &lvi) < 0) oops();
    }
    if(IoListSelectionPoint >= 0) {
        for(i = 0; i < Prog.io.count; i++) {
            ListView_SetItemState(IoList, i, 0, LVIS_SELECTED);
        }
        ListView_SetItemState(IoList, IoListSelectionPoint, LVIS_SELECTED,
            LVIS_SELECTED);
        ListView_EnsureVisible(IoList, IoListSelectionPoint, FALSE);
    }
    IoListOutOfSync = FALSE;

    /*if(Prog.mcu) {
        SendMessage(StatusBar, SB_SETTEXT, 0, (LPARAM)Prog.mcu->mcuName);
    } else {
        SendMessage(StatusBar, SB_SETTEXT, 0, (LPARAM)_("no MCU selected"));
    }*/
    char buf[256];
    sprintf(buf, _("cycle time %.2f ms"), (double)Prog.cycleTime/1000.0);
    SendMessage(StatusBar, SB_SETTEXT, 1, (LPARAM)buf);

    if(Prog.mcu && (Prog.mcu->whichIsa == ISA_ANSIC || Prog.mcu->whichIsa == ISA_INTERPRETED))
    {
		strcpy(buf, "");
    } else {
        sprintf(buf, _("processor clock %.4f MHz"), (double)Prog.mcuClock/1000000.0);
    }
    SendMessage(StatusBar, SB_SETTEXT, 2, (LPARAM)buf);

    for(i = 0; i < NUM_SUPPORTED_MCUS; i++) 
	{
        if(&SupportedMcus[i] == Prog.mcu) 
		{
            CheckMenuItem(ProcessorMenu, MNU_PROCESSOR_0+i, MF_CHECKED);
        } else {
            CheckMenuItem(ProcessorMenu, MNU_PROCESSOR_0+i, MF_UNCHECKED);
        }
    }
    // `(no microcontroller)' setting
    if(!Prog.mcu) {
        CheckMenuItem(ProcessorMenu, MNU_PROCESSOR_0+i, MF_CHECKED);
    } else {
        CheckMenuItem(ProcessorMenu, MNU_PROCESSOR_0+i, MF_UNCHECKED);
    }
}

//-----------------------------------------------------------------------------
// Regenerate the I/O list, keeping the selection in the same place if
// possible.
//-----------------------------------------------------------------------------
void GenerateIoListDontLoseSelection(void)
{
    int i;
    IoListSelectionPoint = -1;
    for(i = 0; i < Prog.io.count; i++) {
        if(ListView_GetItemState(IoList, i, LVIS_SELECTED)) {
            IoListSelectionPoint = i;
            break;
        }
    }
    IoListSelectionPoint = GenerateIoMapList(IoListSelectionPoint);
    // can't just update the listview index; if I/O has been added then the
    // new selection point might be out of range till we refill it
    IoListOutOfSync = TRUE;
    RefreshControlsToSettings();
}

//-----------------------------------------------------------------------------
// Called when we have to update position / dimension of the draw window. Adjust the size of the
// draw window to fit inside available main window area.
//-----------------------------------------------------------------------------
void RefreshDrawWindow()
{
    RECT main;
    GetClientRect(MainWindow, &main);

    MoveWindow(DrawWindow, 0, RibbonHeight, main.right, IoListTop - RibbonHeight, TRUE);
}

//-----------------------------------------------------------------------------
// Called when the main window has been resized. Adjust the size of the
// status bar and the listview to reflect the new window size.
//-----------------------------------------------------------------------------
void MainWindowResized(void)
{
    RECT main;
    GetClientRect(MainWindow, &main);

    RECT status;
    GetWindowRect(StatusBar, &status);
    int statusHeight = status.bottom - status.top;

    MoveWindow(StatusBar, 0, main.bottom - statusHeight, main.right,
        statusHeight, TRUE);

    // Make sure that the I/O list can't disappear entirely.
    if(IoListHeight < 30) {
        IoListHeight = 30;
    }
    IoListTop = main.bottom - IoListHeight - statusHeight;
    // Make sure that we can't drag the top of the I/O list above the
    // bottom of the menu bar, because it then becomes inaccessible.
    if(IoListTop < 5) {
        IoListHeight = main.bottom - statusHeight - 5;
        IoListTop = main.bottom - IoListHeight - statusHeight;
    }
	if (UartSimulationWindow != NULL)
	{
		MoveWindow(IoList, 0, IoListTop, main.right / 2, IoListHeight, TRUE);
		MoveWindow(UartSimulationWindow, main.right / 2, IoListTop, main.right / 2, IoListHeight, TRUE);

		RECT text;
		GetClientRect(UartSimulationWindow, &text);
		SetWindowPos(UartSimulationTextControl, 0, text.left + 1, text.top, text.right - 2, text.bottom, NULL);
	}
	else
		MoveWindow(IoList, 0, IoListTop, main.right, IoListHeight, TRUE);

    RefreshScrollbars();
	RefreshDrawWindow();

    InvalidateRect(MainWindow, NULL, FALSE);
}

//-----------------------------------------------------------------------------
// Toggle whether we are in simulation mode. A lot of options are only
// available in one mode or the other.
//-----------------------------------------------------------------------------
void ToggleSimulationMode(void)
{
    InSimulationMode = !InSimulationMode;

	SetApplicationMode();

    if(InSimulationMode) {
#ifdef POPTOOLS_DISABLE_RIBBON
        EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_ENABLED);
        EnableMenuItem(SimulateMenu, MNU_SINGLE_CYCLE, MF_ENABLED);

        EnableMenuItem(FileMenu, MNU_OPEN, MF_GRAYED);
        EnableMenuItem(FileMenu, MNU_SAVE, MF_GRAYED);
        EnableMenuItem(FileMenu, MNU_SAVE_AS, MF_GRAYED);
        EnableMenuItem(FileMenu, MNU_NEW, MF_GRAYED);
        EnableMenuItem(FileMenu, MNU_EXPORT, MF_GRAYED);

        EnableMenuItem(TopMenu, 1, MF_GRAYED | MF_BYPOSITION);
        EnableMenuItem(TopMenu, 2, MF_GRAYED | MF_BYPOSITION);
        EnableMenuItem(TopMenu, 3, MF_GRAYED | MF_BYPOSITION);
        EnableMenuItem(TopMenu, 5, MF_GRAYED | MF_BYPOSITION);
    
        CheckMenuItem(SimulateMenu, MNU_SIMULATION_MODE, MF_CHECKED);
#else
		RibbonSetCmdState(cmdFileSave             , FALSE);
		RibbonSetCmdState(cmdUndo                 , FALSE);
		RibbonSetCmdState(cmdRedo                 , FALSE);
		RibbonSetCmdState(cmdSimulationStop       , FALSE);
		RibbonSetCmdState(cmdSimulationStart      , TRUE );
		RibbonSetCmdState(cmdSimulationSingleCycle, TRUE );
#endif

        // Recheck InSimulationMode, because there could have been a compile
        // error, which would have kicked us out of simulation mode.
        if(UartFunctionUsed() && InSimulationMode) {
            ShowUartSimulationWindow();
        }

        ClearSimulationData();

	} else {
        RealTimeSimulationRunning = FALSE;
        KillTimer(MainWindow, TIMER_SIMULATE);

#ifdef POPTOOLS_DISABLE_RIBBON
        EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_GRAYED);
        EnableMenuItem(SimulateMenu, MNU_STOP_SIMULATION, MF_GRAYED);
        EnableMenuItem(SimulateMenu, MNU_SINGLE_CYCLE, MF_GRAYED);

        EnableMenuItem(FileMenu, MNU_OPEN, MF_ENABLED);
        EnableMenuItem(FileMenu, MNU_SAVE, MF_ENABLED);
        EnableMenuItem(FileMenu, MNU_SAVE_AS, MF_ENABLED);
        EnableMenuItem(FileMenu, MNU_NEW, MF_ENABLED);
        EnableMenuItem(FileMenu, MNU_EXPORT, MF_ENABLED);

        EnableMenuItem(TopMenu, 1, MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(TopMenu, 2, MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(TopMenu, 3, MF_ENABLED | MF_BYPOSITION);
        EnableMenuItem(TopMenu, 5, MF_ENABLED | MF_BYPOSITION);

        CheckMenuItem(SimulateMenu, MNU_SIMULATION_MODE, MF_UNCHECKED);
#else
		RibbonSetCmdState(cmdFileSave, TRUE);
		RibbonSetCmdState(cmdUndo    , TRUE);
		RibbonSetCmdState(cmdRedo    , TRUE);
#endif

        if(UartFunctionUsed()) {
            DestroyUartSimulationWindow();
        }
    }

    UpdateMainWindowTitleBar();

    DrawMenuBar(MainWindow);
    InvalidateRect(MainWindow, NULL, FALSE);
    ListView_RedrawItems(IoList, 0, Prog.io.count - 1);
}

//-----------------------------------------------------------------------------
// Start real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void StartSimulation(void)
{
    RealTimeSimulationRunning = TRUE;

#ifdef POPTOOLS_DISABLE_RIBBON
    EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_GRAYED);
    EnableMenuItem(SimulateMenu, MNU_STOP_SIMULATION, MF_ENABLED);
#else
	RibbonSetCmdState(cmdSimulationStop       , TRUE );
	RibbonSetCmdState(cmdSimulationStart      , FALSE);
	RibbonSetCmdState(cmdSimulationSingleCycle, FALSE);
#endif
    StartSimulationTimer();

    UpdateMainWindowTitleBar();
}

//-----------------------------------------------------------------------------
// Stop real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void StopSimulation(void)
{
    RealTimeSimulationRunning = FALSE;

#ifdef POPTOOLS_DISABLE_RIBBON
    EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_ENABLED);
    EnableMenuItem(SimulateMenu, MNU_STOP_SIMULATION, MF_GRAYED);
#else
	RibbonSetCmdState(cmdSimulationStop       , FALSE);
	RibbonSetCmdState(cmdSimulationStart      , TRUE );
	RibbonSetCmdState(cmdSimulationSingleCycle, TRUE );
#endif
    KillTimer(MainWindow, TIMER_SIMULATE);

    UpdateMainWindowTitleBar();
}
