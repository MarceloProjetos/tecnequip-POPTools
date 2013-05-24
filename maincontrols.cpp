#include "poptools.h"

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
BOOL         RealTimeSimulationRunning;

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
    if(InSimulationMode) {
        if(RealTimeSimulationRunning) {
            strcpy(line, _("POPTools - Simulation (Running)"));
        } else {
            strcpy(line, _("POPTools - Simulation (Stopped)"));
        }
    } else {
        strcpy(line, _("POPTools - Program Editor"));
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
    EnableInterfaceItem(MNU_PUSH_RUNG_UP, canPushUp);
    EnableInterfaceItem(MNU_PUSH_RUNG_DOWN, canPushDown);
    EnableInterfaceItem(MNU_DELETE_RUNG, (Prog.numRungs > 1));

    EnableInterfaceItem(MNU_NEGATE, canNegate);
    EnableInterfaceItem(MNU_MAKE_NORMAL, canNormal);
    EnableInterfaceItem(MNU_MAKE_RESET_ONLY, canResetOnly);
    EnableInterfaceItem(MNU_MAKE_SET_ONLY, canSetOnly);

    EnableInterfaceItem(MNU_INSERT_COMMENT, canInsertComment);

    EnableInterfaceItem(MNU_DELETE_ELEMENT, canDelete);

    int t;
    t = canInsertEnd;
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
	EnableInterfaceItem(MNU_INSERT_POS, t);
	EnableInterfaceItem(MNU_INSERT_SQRT, t);
	EnableInterfaceItem(MNU_INSERT_ABS, t);
	EnableInterfaceItem(MNU_INSERT_RAND, t);

    t = canInsertOther;
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
	EnableInterfaceItem(MNU_INSERT_READ_MODBUS, t && Prog.settings.mb_list_size); // Can't be enabled if there is no registered nodes
	EnableInterfaceItem(MNU_INSERT_WRITE_MODBUS, t && Prog.settings.mb_list_size); // Can't be enabled if there is no registered nodes
	EnableInterfaceItem(MNU_INSERT_READ_USS, t);
	EnableInterfaceItem(MNU_INSERT_WRITE_USS, t);
	EnableInterfaceItem(MNU_READ_SERVO_YASKAWA, t);
	EnableInterfaceItem(MNU_WRITE_SERVO_YASKAWA, t);
	EnableInterfaceItem(MNU_READ_FMTD_STR, t);
	EnableInterfaceItem(MNU_WRITE_FMTD_STR, t);
	EnableInterfaceItem(MNU_INSERT_PARALLEL, t);
}

//-----------------------------------------------------------------------------
// Set the enabled state of the undo/redo menus.
//-----------------------------------------------------------------------------
void SetUndoEnabled(BOOL undoEnabled, BOOL redoEnabled)
{
    EnableMenuItem(EditMenu, MNU_UNDO, undoEnabled ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(EditMenu, MNU_REDO, redoEnabled ? MF_ENABLED : MF_GRAYED);
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

//    InvalidateRect(MainWindow, NULL, FALSE);
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
	sprintf(buf, _("IP: %d.%d.%d.%d"), Prog.settings.ip[0], Prog.settings.ip[1], Prog.settings.ip[2], Prog.settings.ip[3]);
    SendMessage(StatusBar, SB_SETTEXT, 1, (LPARAM)buf);

	sprintf(buf, _("ModBUS ID: %d"), Prog.settings.ModBUSID);
    SendMessage(StatusBar, SB_SETTEXT, 2, (LPARAM)buf);

    if(Prog.mcu && (Prog.mcu->whichIsa == ISA_ANSIC || Prog.mcu->whichIsa == ISA_INTERPRETED))
    {
		strcpy(buf, "");
    } else {
		sprintf(buf, _("RS-485: %d bps, %d bits de dados, %s, Bits de Parada: %d"), Prog.settings.baudRate, SerialConfig[Prog.settings.UART].bByteSize,
			SerialParityString[SerialConfig[Prog.settings.UART].bParity], SerialConfig[Prog.settings.UART].bStopBits == ONESTOPBIT ? 1 : 2);
    }
    SendMessage(StatusBar, SB_SETTEXT, 3, (LPARAM)buf);

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

	if(!main.bottom && !main.top) // Window height = 0, window minimized. Does nothing!
		return;

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
	LVCOLUMN col;
	col.mask = LVCF_TEXT;

	InSimulationMode = !InSimulationMode;
	ClearListWP();

	SetApplicationMode();

    if(InSimulationMode) {
		RemoveParallelStart(0, NULL);
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
		RibbonSetCmdState(cmdSimulationPause      , FALSE);
		RibbonSetCmdState(cmdSimulationStop       , FALSE);
		RibbonSetCmdState(cmdSimulationStart      , TRUE );
		RibbonSetCmdState(cmdSimulationSingleCycle, TRUE );
#endif

        // Recheck InSimulationMode, because there could have been a compile
        // error, which would have kicked us out of simulation mode.
        if(UartFunctionUsed() && InSimulationMode) {
            ShowUartSimulationWindow();
        }

		col.pszText = _("Interromper se =");
		
		ClearSimulationData();

		if(InSimulationMode)
			SimulationServer_Start();
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

		col.pszText = _("Pin on Processor");

		if(UartFunctionUsed()) {
            DestroyUartSimulationWindow();
        }

		SimulationServer_Stop();
    }

    UpdateMainWindowTitleBar();

    DrawMenuBar(MainWindow);
    InvalidateRect(MainWindow, NULL, FALSE);
	ListView_SetColumn(IoList, LV_IO_PIN, &col);
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
	RibbonSetCmdState(cmdSimulationPause      , TRUE );
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

	ClearSimulationData();

#ifdef POPTOOLS_DISABLE_RIBBON
    EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_ENABLED);
    EnableMenuItem(SimulateMenu, MNU_STOP_SIMULATION, MF_GRAYED);
#else
	RibbonSetCmdState(cmdSimulationPause      , FALSE);
	RibbonSetCmdState(cmdSimulationStop       , FALSE);
	RibbonSetCmdState(cmdSimulationStart      , TRUE );
	RibbonSetCmdState(cmdSimulationSingleCycle, TRUE );
#endif
    KillTimer(MainWindow, TIMER_SIMULATE);

    UpdateMainWindowTitleBar();
}

//-----------------------------------------------------------------------------
// Stop real-time simulation. Have to update the controls grayed status
// to reflect this.
//-----------------------------------------------------------------------------
void PauseSimulation(void)
{
    RealTimeSimulationRunning = FALSE;

#ifdef POPTOOLS_DISABLE_RIBBON
    EnableMenuItem(SimulateMenu, MNU_START_SIMULATION, MF_ENABLED);
    EnableMenuItem(SimulateMenu, MNU_STOP_SIMULATION, MF_GRAYED);
#else
	RibbonSetCmdState(cmdSimulationPause      , FALSE);
	RibbonSetCmdState(cmdSimulationStart      , TRUE );
	RibbonSetCmdState(cmdSimulationSingleCycle, TRUE );
#endif
    KillTimer(MainWindow, TIMER_SIMULATE);

    UpdateMainWindowTitleBar();
}
