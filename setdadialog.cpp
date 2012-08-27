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

static HWND SetDADialog;

static HWND RawRadio;
static HWND MvRadio;
static HWND PctRadio;
static HWND NameTextbox;

static void MakeControls(void)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Source:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 10, 50, 21, SetDADialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        62, 10, 195, 321, SetDADialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	LoadIOListToComboBox(NameTextbox, IO_TYPE_ALL);
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Modo de Escrita"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 35, 250, 86, SetDADialog, NULL, Instance, NULL);
    NiceFont(grouper);

    RawRadio = CreateWindowEx(0, WC_BUTTON, _("Resolu��o DA (12 bits)[-2048 ~ 2047]"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 53, 235, 20, SetDADialog, NULL, Instance, NULL);
    NiceFont(RawRadio);

    MvRadio = CreateWindowEx(0, WC_BUTTON, _("Resolu��o DA (mV)[-10000 ~ 10000]"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 73, 235, 20, SetDADialog, NULL, Instance, NULL);
    NiceFont(MvRadio);

    PctRadio = CreateWindowEx(0, WC_BUTTON, _("Resolu��o DA (%)[-100 ~ 100]"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 93, 235, 20, SetDADialog, NULL, Instance, NULL);
    NiceFont(PctRadio);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        265, 10, 70, 23, SetDADialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        265, 40, 70, 23, SetDADialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

void ShowSetDADialog(char *name, int *mode)
{
	int mode_tmp, min, max;
	char name_tmp[MAX_NAME_LEN];

	SetDADialog = CreateWindowClient(0, "LDmicroDialog",
        _("Set D/A"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 345, 128, MainWindow, NULL, Instance, NULL);

    MakeControls();
   
    if (*mode == ELEM_SET_DA_MODE_RAW) {
        SendMessage(RawRadio, BM_SETCHECK, BST_CHECKED, 0);
	} else if (*mode == ELEM_SET_DA_MODE_MV) {
        SendMessage(MvRadio, BM_SETCHECK, BST_CHECKED, 0);
	} else {
        SendMessage(PctRadio, BM_SETCHECK, BST_CHECKED, 0);
	}

    SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)(name));

    EnableWindow(MainWindow, FALSE);
    ShowWindow(SetDADialog, TRUE);
    SetFocus(NameTextbox);
    SendMessage(NameTextbox, EM_SETSEL, 0, -1);

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

        if(IsDialogMessage(SetDADialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
        SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)(name_tmp));

		if(SendMessage(RawRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			mode_tmp = ELEM_SET_DA_MODE_RAW;
			min = -DA_RESOLUTION;
			max =  DA_RESOLUTION - 1;
		} else if(SendMessage(MvRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			mode_tmp = ELEM_SET_DA_MODE_MV;
			min = -10000;
			max = +10000;
		} else {
			mode_tmp = ELEM_SET_DA_MODE_PCT;
			min = -100;
			max = +100;
		}

		if(IsValidNameAndType(name, name_tmp, _("Origem"), VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(name_tmp), min, max)) {
			strcpy(name, name_tmp);
			name[16] = '\0';

			*mode = mode_tmp;
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SetDADialog);
    return;
}
