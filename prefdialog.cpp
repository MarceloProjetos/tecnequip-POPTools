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

static HWND ShowWarnings;
static HWND ClearRecentList;

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
			}
		}
	}

	return CallWindowProc((WNDPROC)PrevPrefDialogProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    ShowWarnings = CreateWindowEx(0, WC_BUTTON, _("Sempre exibir avisos na simulação"),
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        10, 10, 230, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowWarnings);

    ClearRecentList = CreateWindowEx(0, WC_BUTTON, _("Limpar Recentes"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        65, 40, 120, 23, PrefDialog, NULL, Instance, NULL); 
    NiceFont(ClearRecentList);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        250, 10, 70, 23, PrefDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        250, 40, 70, 23, PrefDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

void ShowPrefDialog(void)
{
	PrefDialog = CreateWindowClient(0, "LDmicroDialog",
        _("Preferências"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 330, 73, MainWindow, NULL, Instance, NULL);

	PrevPrefDialogProc = SetWindowLongPtr(PrefDialog, GWLP_WNDPROC, (LONG_PTR)PrefDialogProc);

    MakeControls();

	if(POPSettings.ShowSimulationWarnings) {
        SendMessage(ShowWarnings, BM_SETCHECK, BST_CHECKED, 0);
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
			if(SendMessage(ShowWarnings, BM_GETSTATE, 0, 0) & BST_CHECKED) {
				POPSettings.ShowSimulationWarnings = TRUE;
			} else {
				POPSettings.ShowSimulationWarnings = FALSE;
	        }
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(PrefDialog);
    return;
}
