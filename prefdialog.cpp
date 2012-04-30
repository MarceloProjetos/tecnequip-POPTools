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
// Dialog for setting the properties of a set of contacts: negated or not,
// plus the name
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>

#include "ldmicro.h"

static HWND PrefDialog;

static HWND ShowWarningsYes;
static HWND ShowWarningsNo;
static HWND ClearRecentList;
static HWND ComPortFlashCombobox;
static HWND ComPortDebugCombobox;

static LONG_PTR PrevPrefDialogProc;

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK PrefDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_COMMAND: {
            HWND h = (HWND)lParam;
			if(wParam == BN_CLICKED && h == ClearRecentList) {
				memset(POPSettings.recent_list, 0, sizeof(POPSettings.recent_list));
				PopulateRecentListMenu();
				break;
			} else if(h == ComPortFlashCombobox && HIWORD(wParam) == CBN_DROPDOWN) {
				LoadCOMPorts(ComPortFlashCombobox, POPSettings.COMPortFlash, true);
			} else if(h == ComPortDebugCombobox && HIWORD(wParam) == CBN_DROPDOWN) {
				LoadCOMPorts(ComPortDebugCombobox, POPSettings.COMPortDebug, false);
			}
		}
	}

	return CallWindowProc((WNDPROC)PrevPrefDialogProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND textlabel = CreateWindowEx(0, WC_STATIC, _("Ao cancelar:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 25, 110, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel);

    ShowWarningsYes = CreateWindowEx(0, WC_BUTTON, _("Sim"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        145, 25, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowWarningsYes);

    ShowWarningsNo = CreateWindowEx(0, WC_BUTTON, _("Não"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        210, 25, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowWarningsNo);

    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Avisos durante a simulação"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        10, 5, 270, 50, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, _("Porta Gravação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 60, 110, 21, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel1);

	ComPortFlashCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        125, 60, 155, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(ComPortFlashCombobox);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Porta Depuração:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 90, 110, 21, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	ComPortDebugCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        125, 90, 155, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(ComPortDebugCombobox);

    HWND textLabel3 = CreateWindowEx(0, WC_STATIC, _("Projetos Recentes:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 125, 110, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel3);

	ClearRecentList = CreateWindowEx(0, WC_BUTTON, _("Limpar!"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        125, 120, 155, 30, PrefDialog, NULL, Instance, NULL); 
    NiceFont(ClearRecentList);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        290, 10, 70, 67, PrefDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        290, 83, 70, 67, PrefDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

void ShowPrefDialog(void)
{
	PrefDialog = CreateWindowClient(0, "LDmicroDialog",
        _("Preferências"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 370, 160, MainWindow, NULL, Instance, NULL);

	PrevPrefDialogProc = SetWindowLongPtr(PrefDialog, GWLP_WNDPROC, (LONG_PTR)PrefDialogProc);

    MakeControls();

	LoadCOMPorts(ComPortFlashCombobox, POPSettings.COMPortFlash, true);
	SendMessage (ComPortFlashCombobox, CB_SETDROPPEDWIDTH, 300, 0);

	LoadCOMPorts(ComPortDebugCombobox, POPSettings.COMPortDebug, false);
	SendMessage (ComPortDebugCombobox, CB_SETDROPPEDWIDTH, 300, 0);

	if(POPSettings.ShowSimulationWarnings) {
        SendMessage(ShowWarningsYes, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(ShowWarningsNo , BM_SETCHECK, BST_CHECKED, 0);
	}

    EnableWindow(MainWindow, FALSE);
    ShowWindow(PrefDialog, TRUE);

    MSG msg;
    DWORD ret;
    DialogDone = FALSE;
    DialogCancel = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                DialogDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
                break;
            }
        }

        if(IsDialogMessage(PrefDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
		char buf[16];

        SendMessage(ComPortFlashCombobox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		POPSettings.COMPortFlash = atoi(&buf[3]);

        SendMessage(ComPortDebugCombobox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		POPSettings.COMPortDebug = atoi(&buf[3]);

		if(SendMessage(ShowWarningsYes, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			POPSettings.ShowSimulationWarnings = TRUE;
		} else {
			POPSettings.ShowSimulationWarnings = FALSE;
	    }
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(PrefDialog);
    return;
}
