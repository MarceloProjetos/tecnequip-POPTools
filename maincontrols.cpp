#include "poptools.h"

// scrollbars for the ladder logic area
static HWND         HorizScrollBar;
static HWND         VertScrollBar;
int                 ScrollWidth;
int                 ScrollHeight;
BOOL                NeedHoriz;

// Window where to draw schematic
HWND                DrawWindow;
HWND                TabCtrl;
static LONG_PTR     PrevDrawWindowProc;

// status bar at the bottom of the screen, to display settings
static HWND         StatusBar;

// listview used to maintain the list of I/O pins with symbolic names, plus
// the internal relay too
HWND                IoList;
int                 IoListHeight;
int                 IoListTop;

// whether the simulation is running in real time
BOOL         RealTimeSimulationRunning;

// Altura das abas
int TabHeight = 25;

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

    TabCtrl = CreateWindowEx(0, WC_TABCONTROL, _("TabControl"),
		WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD,
		0, 0, 1, 1, MainWindow, NULL, Instance, NULL);// The tabCtrl size inside windows
	NiceFont(TabCtrl);

	DrawWindow = CreateWindowEx(0, WC_STATIC, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
		0, 0, 1, 1, MainWindow, NULL, Instance, NULL);
	PrevDrawWindowProc = SetWindowLongPtr(DrawWindow, GWLP_WNDPROC, (LONG_PTR)DrawWindowProc);
	RefreshDrawWindow();

	StatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 
        _("POPTools iniciado"), MainWindow, 0);
    int edges[] = { 250, 370, 490, -1 };
    SendMessage(StatusBar, SB_SETPARTS, 4, (LPARAM)edges);

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
    if(ladder->getContext().inSimulationMode) {
        if(RealTimeSimulationRunning) {
            strcpy(line, _("POPTools - Simulation (Running)"));
        } else {
            strcpy(line, _("POPTools - Simulation (Stopped)"));
        }
    } else {
        strcpy(line, _("POPTools - Program Editor"));
    }

	char buf[1024];
	string currentFilename = ladder->getCurrentFilename();
    if(currentFilename.size() > 0) {
        sprintf(line+strlen(line), " - %d.%d.%d.%d - %s%s", 
			wMajor,
			wMinor,
			wBuild,
			wRevision,
			currentFilename.c_str(),
			ladder->getContext().programChangedNotSaved ? " *" : "");

		unsigned int pos = currentFilename.find_last_of("\\");
		string basename = currentFilename.substr(pos + 1);
		strcpy(buf, basename.c_str());
    } else {
		strcpy(buf, _(" - (not yet saved)"));
        sprintf(line+strlen(line), " - %d.%d.%d.%d%s", 
			wMajor,
			wMinor,
			wBuild,
			wRevision,
			buf);
    }

	TCITEM container_tabs;
	container_tabs.mask   = TCIF_TEXT;

	container_tabs.pszText = buf;
	TabCtrl_SetItem(TabCtrl, TabCtrl_GetCurSel(TabCtrl), &container_tabs);

	SetWindowText(MainWindow, line);
}

//-----------------------------------------------------------------------------
// Set the enabled state of the logic menu items to reflect where we are on
// the schematic (e.g. can't insert two coils in series).
//-----------------------------------------------------------------------------
void SetMenusEnabled(LadderContext *context)
{
	EnableInterfaceItem(MNU_PUSH_RUNG_UP, context->canPushUp);
    EnableInterfaceItem(MNU_PUSH_RUNG_DOWN, context->canPushDown);
    EnableInterfaceItem(MNU_DELETE_RUNG, context->canDeleteRung);

    EnableInterfaceItem(MNU_NEGATE, context->canNegate);
    EnableInterfaceItem(MNU_MAKE_NORMAL, context->canNormal);
    EnableInterfaceItem(MNU_MAKE_RESET_ONLY, context->canResetOnly);
    EnableInterfaceItem(MNU_MAKE_SET_ONLY, context->canSetOnly);

    EnableInterfaceItem(MNU_INSERT_COMMENT, context->canInsertComment);

    EnableInterfaceItem(MNU_DELETE_ELEMENT, context->canDelete);

    int t;
    t = context->canInsertEnd;
    EnableInterfaceItem(MNU_INSERT_COIL, t);
    EnableInterfaceItem(MNU_INSERT_RES, t);
    EnableInterfaceItem(MNU_INSERT_MOV, t);
    EnableInterfaceItem(MNU_INSERT_ADD, t);
    EnableInterfaceItem(MNU_INSERT_SUB, t);
    EnableInterfaceItem(MNU_INSERT_MUL, t);
    EnableInterfaceItem(MNU_INSERT_DIV, t);
    EnableInterfaceItem(MNU_INSERT_MOD, t);
    EnableInterfaceItem(MNU_INSERT_CTC, t);
    EnableInterfaceItem(MNU_INSERT_PERSIST, t);
    EnableInterfaceItem(MNU_INSERT_READ_ADC, t);
    EnableInterfaceItem(MNU_INSERT_SET_PWM, t);
    EnableInterfaceItem(MNU_INSERT_MASTER_RLY, t);
    EnableInterfaceItem(MNU_INSERT_SHIFT_REG, t);
    EnableInterfaceItem(MNU_INSERT_LUT, t);
    EnableInterfaceItem(MNU_INSERT_PWL, t);
	EnableInterfaceItem(MNU_INSERT_MULTISET_DA, t);
	EnableInterfaceItem(MNU_INSERT_SET_DA, t);
	EnableInterfaceItem(MNU_INSERT_READ_ENC, t);
	EnableInterfaceItem(MNU_INSERT_RESET_ENC, t);
	EnableInterfaceItem(MNU_INSERT_PID, t);
	EnableInterfaceItem(MNU_INSERT_SQRT, t);
	EnableInterfaceItem(MNU_INSERT_ABS, t);
	EnableInterfaceItem(MNU_INSERT_RAND, t);

    t = context->canInsertOther;
    EnableInterfaceItem(MNU_INSERT_TON, t);
    EnableInterfaceItem(MNU_INSERT_TOF, t);
    EnableInterfaceItem(MNU_INSERT_OSR, t);
    EnableInterfaceItem(MNU_INSERT_OSF, t);
    EnableInterfaceItem(MNU_INSERT_RTO, t);
    EnableInterfaceItem(MNU_INSERT_CONTACTS, t);
    EnableInterfaceItem(MNU_INSERT_CTU, t);
    EnableInterfaceItem(MNU_INSERT_CTD, t);
    EnableInterfaceItem(MNU_INSERT_EQU, t);
    EnableInterfaceItem(MNU_INSERT_NEQ, t);
    EnableInterfaceItem(MNU_INSERT_GRT, t);
    EnableInterfaceItem(MNU_INSERT_GEQ, t);
    EnableInterfaceItem(MNU_INSERT_LES, t);
    EnableInterfaceItem(MNU_INSERT_LEQ, t);
    EnableInterfaceItem(MNU_INSERT_SHORT, t);
    EnableInterfaceItem(MNU_INSERT_OPEN, t);
    EnableInterfaceItem(MNU_INSERT_UART_SEND, t);
    EnableInterfaceItem(MNU_INSERT_UART_RECV, t);
	EnableInterfaceItem(MNU_INSERT_RTC, t);
	EnableInterfaceItem(MNU_INSERT_CHECK_BIT, t);
	EnableInterfaceItem(MNU_INSERT_SET_BIT, t);
	EnableInterfaceItem(MNU_INSERT_READ_MODBUS, t);
	EnableInterfaceItem(MNU_INSERT_WRITE_MODBUS, t);
	EnableInterfaceItem(MNU_INSERT_READ_USS, t);
	EnableInterfaceItem(MNU_INSERT_WRITE_USS, t);
	EnableInterfaceItem(MNU_READ_SERVO_YASKAWA, t);
	EnableInterfaceItem(MNU_WRITE_SERVO_YASKAWA, t);
	EnableInterfaceItem(MNU_READ_FMTD_STR, t);
	EnableInterfaceItem(MNU_WRITE_FMTD_STR, t);
	EnableInterfaceItem(MNU_INSERT_PARALLEL, t);
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
            main.right - ScrollWidth, ScrollHeight, TRUE);
        ShowWindow(HorizScrollBar, SW_SHOW);
        EnableWindow(HorizScrollBar, TRUE);
    } else {
        ShowWindow(HorizScrollBar, SW_HIDE);
    }
    MoveWindow(VertScrollBar, main.right - ScrollWidth, RibbonHeight + 1 + TabHeight, ScrollWidth,
        IoListTop - RibbonHeight - TabHeight - 3 - (NeedHoriz ? ScrollHeight : 0), TRUE);
}

//-----------------------------------------------------------------------------
// Respond to a WM_VSCROLL sent to the main window, presumably by the one and
// only vertical scrollbar that it has as a child.
//-----------------------------------------------------------------------------
void VscrollProc(WPARAM wParam)
{
	int offset = 2;
    int prevY = ScrollYOffset;
    switch(LOWORD(wParam)) {
        case SB_LINEUP:
        case SB_PAGEUP:
            if(ScrollYOffset > offset) {
                ScrollYOffset -= offset;
            } else {
				ScrollYOffset = 0;
			}
            break;

        case SB_LINEDOWN:
        case SB_PAGEDOWN:
            if(ScrollYOffset < (ScrollYOffsetMax + offset)) {
                ScrollYOffset += offset;
            } else {
				ScrollYOffset = ScrollYOffsetMax - 1;
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

        InvalidateRect(DrawWindow, NULL, FALSE);
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

        InvalidateRect(DrawWindow, NULL, FALSE);
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
    char buf[256];
	McuIoInfo *mcu = ladder->getMCU();

	LadderSettingsUART        settingsUart    = ladder->getSettingsUART();
	LadderSettingsNetwork     settingsNetwork = ladder->getSettingsNetwork();
	LadderSettingsModbusSlave settingsMbSlave = ladder->getSettingsModbusSlave();

	sprintf(buf, _("IP: %d.%d.%d.%d"), FIRST_IPADDRESS(settingsNetwork.ip), SECOND_IPADDRESS(settingsNetwork.ip),
		THIRD_IPADDRESS(settingsNetwork.ip), FOURTH_IPADDRESS(settingsNetwork.ip));

	SendMessage(StatusBar, SB_SETTEXT, 1, (LPARAM)buf);

	sprintf(buf, _("ModBUS ID: %d"), settingsMbSlave.ModBUSID);
    SendMessage(StatusBar, SB_SETTEXT, 2, (LPARAM)buf);

    if(mcu != nullptr && (mcu->whichIsa == ISA_ANSIC || mcu->whichIsa == ISA_INTERPRETED))
    {
		strcpy(buf, "");
    } else {
		sprintf(buf, _("RS-485: %d bps, %d bits de dados, %s, Bits de Parada: %d"), settingsUart.baudRate, SerialConfig[settingsUart.UART].bByteSize,
			SerialParityString[SerialConfig[settingsUart.UART].bParity], SerialConfig[settingsUart.UART].bStopBits == ONESTOPBIT ? 1 : 2);
    }
    SendMessage(StatusBar, SB_SETTEXT, 3, (LPARAM)buf);
}

//-----------------------------------------------------------------------------
// Called when we have to update position / dimension of the draw window. Adjust the size of the
// draw window to fit inside available main window area.
//-----------------------------------------------------------------------------
void RefreshDrawWindow()
{
    RECT main;
    GetClientRect(MainWindow, &main);

	// Move a janela de abas
	MoveWindow(TabCtrl, 0, RibbonHeight, main.right, TabHeight, TRUE);

	// Move a janela de desenho
    MoveWindow(DrawWindow, 0, RibbonHeight + TabHeight, main.right, IoListTop - RibbonHeight - TabHeight, TRUE);
}

// Funcao que oculta/exibe as abas com os diagramas abertos
void ShowTabCtrl(bool visible)
{
	if(visible) {
		TabHeight = 25;
		ShowWindow(TabCtrl, SW_SHOW);
	} else {
		TabHeight = 0;
		ShowWindow(TabCtrl, SW_HIDE);
	}

	RefreshDrawWindow();
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

	if(!main.bottom && !main.top) // Window height = 0, window minimized. Does nothing!
		return;

	ladder->NeedRedraw(true);

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

	if (UartSimulationWindow != NULL) {
		MoveWindow(IoList, 0, IoListTop, main.right / 2, IoListHeight, TRUE);
		MoveWindow(UartSimulationWindow, main.right / 2, IoListTop, main.right / 2, IoListHeight, TRUE);

		RECT text;
		GetClientRect(UartSimulationWindow, &text);
		SetWindowPos(UartSimulationTextControl, 0, 1, 0, text.right - 2, text.bottom, NULL);
	} else if (FARWindow != NULL) {
		RECT r = { main.right - FarWidth, IoListTop, FarWidth, IoListHeight };
		MoveWindow(IoList, 0, IoListTop, r.left, IoListHeight, TRUE);
		MoveFARWindow(r);
	} else {
		MoveWindow(IoList, 0, IoListTop, main.right, IoListHeight, TRUE);
	}

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
	LVCOLUMN col;
	col.mask = LVCF_TEXT;

	ladder->setSimulationState(!ladder->getContext().inSimulationMode);
	ClearListWP();

	SetApplicationMode();

	if(ladder->getContext().inSimulationMode) {
		if(FARWindow != NULL) {
			DestroyWindow(FARWindow);
		}
		ShowTabCtrl(false);
	} else if(ladderList.size() > 1) {
		ShowTabCtrl(true);
	}

	if(ladder->getContext().inSimulationMode) {
		RibbonSetCmdState(cmdFileSave             , FALSE);
		RibbonSetCmdState(cmdUndo                 , FALSE);
		RibbonSetCmdState(cmdRedo                 , FALSE);
		RibbonSetCmdState(cmdSimulationPause      , FALSE);
		RibbonSetCmdState(cmdSimulationStop       , FALSE);
		RibbonSetCmdState(cmdSimulationStart      , TRUE );
		RibbonSetCmdState(cmdSimulationSingleCycle, TRUE );

		LogSimulation(false);

        // Recheck InSimulationMode, because there could have been a compile
        // error, which would have kicked us out of simulation mode.
        if(ladder->UartFunctionUsed() && ladder->getContext().inSimulationMode) {
            ShowUartSimulationWindow();
        }

		col.pszText = _("Interromper se =");
		
		ClearSimulationData();

		if(ladder->getContext().inSimulationMode)
			SimulationServer_Start();
	} else {
        RealTimeSimulationRunning = FALSE;
        KillTimer(MainWindow, TIMER_SIMULATE);

		LogSimulation(false);

		RibbonSetCmdState(cmdFileSave, TRUE);
		RibbonSetCmdState(cmdUndo    , TRUE);
		RibbonSetCmdState(cmdRedo    , TRUE);

		col.pszText = _("Pin on Processor");

		if(ladder->UartFunctionUsed()) {
            DestroyUartSimulationWindow();
        }

		SimulationServer_Stop();
    }

    UpdateMainWindowTitleBar();

    DrawMenuBar(MainWindow);
    InvalidateRect(MainWindow, NULL, FALSE);
	ListView_SetColumn(IoList, LV_IO_PIN, &col);
	ListView_RedrawItems(IoList, 0, ladder->getCountIO() - 1);
}

//-----------------------------------------------------------------------------
// Start real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void StartSimulation(void)
{
    RealTimeSimulationRunning = TRUE;

	RibbonSetCmdState(cmdSimulationPause      , TRUE );
	RibbonSetCmdState(cmdSimulationStop       , TRUE );
	RibbonSetCmdState(cmdSimulationStart      , FALSE);
	RibbonSetCmdState(cmdSimulationSingleCycle, FALSE);

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
	KillTimer(MainWindow, TIMER_SIMULATE);

	LogSimulation(false);

	ClearSimulationData();

	RibbonSetCmdState(cmdSimulationPause      , FALSE);
	RibbonSetCmdState(cmdSimulationStop       , FALSE);
	RibbonSetCmdState(cmdSimulationStart      , TRUE );
	RibbonSetCmdState(cmdSimulationSingleCycle, TRUE );

    UpdateMainWindowTitleBar();
}

//-----------------------------------------------------------------------------
// Stop real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void PauseSimulation(void)
{
    RealTimeSimulationRunning = FALSE;

	RibbonSetCmdState(cmdSimulationPause      , FALSE);
	RibbonSetCmdState(cmdSimulationStart      , TRUE );
	RibbonSetCmdState(cmdSimulationSingleCycle, TRUE );

	KillTimer(MainWindow, TIMER_SIMULATE);

    UpdateMainWindowTitleBar();
}
