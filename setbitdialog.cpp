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

static HWND SetBitDialog;

static HWND SetBitRadio;
static HWND ClearBitRadio;
static HWND NameTextbox;
static HWND BitCombobox;

static LONG_PTR PrevNameProc;

const LPCTSTR ComboboxBitItens[] = { _("0"), _("1"), _("2"), _("3"), _("4"), _("5"), _("6"), _("7"), _("8"), _("9"), _("10"), 
									_("11"), _("12"), _("13"), _("14"), _("15"), _("16"), _("17"), _("18"), _("19"), _("20"), 
									_("21"), _("22"), _("23"), _("24"), _("25"), _("26"), _("27"), _("28"), _("29"), _("30"), 
									_("31")/*, _("32")*/};

//-----------------------------------------------------------------------------
// Don't allow any characters other than A-Za-z0-9_ in the name.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MyNameProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isalpha(wParam) || isdigit(wParam) || wParam == '_' ||
            wParam == '\b'))
        {
            return 0;
        }
    }

    return CallWindowProc((WNDPROC)PrevNameProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Operação bit"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 3, 120, 65, SetBitDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Name:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        135, 16, 50, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    SetBitRadio = CreateWindowEx(0, WC_BUTTON, _("Ligar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 21, 100, 20, SetBitDialog, NULL, Instance, NULL);
    NiceFont(SetBitRadio);

    ClearBitRadio = CreateWindowEx(0, WC_BUTTON, _("Desligar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 100, 20, SetBitDialog, NULL, Instance, NULL);
    NiceFont(ClearBitRadio);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        190, 16, 115, 21, SetBitDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	LoadIOListToComboBox(NameTextbox, IO_TYPE_ALL);
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

	HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Bit:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        135, 41, 50, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	BitCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        190, 41, 65, 140, SetBitDialog, NULL, Instance, NULL);
    NiceFont(BitCombobox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        321, 10, 70, 23, SetBitDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        321, 40, 70, 23, SetBitDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

void ShowSetBitDialog(char *name, int * set, int * bit)
{
	char name_tmp[MAX_NAME_LEN];

    SetBitDialog = CreateWindowClient(0, "LDmicroDialog",
        _("Set Bit"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 404, 75, MainWindow, NULL, Instance, NULL);

    MakeControls();
   
	int i;

	for (i = 0; i < sizeof(ComboboxBitItens) / sizeof(ComboboxBitItens[0]); i++)
		SendMessage(BitCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBitItens[i]));

	SendMessage(BitCombobox, CB_SETCURSEL, *bit, 0);

    if (*set)
        SendMessage(SetBitRadio, BM_SETCHECK, BST_CHECKED, 0);
	else
        SendMessage(ClearBitRadio, BM_SETCHECK, BST_CHECKED, 0);

    SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)(name));

    EnableWindow(MainWindow, FALSE);
    ShowWindow(SetBitDialog, TRUE);
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

        if(IsDialogMessage(SetBitDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
        SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)(name_tmp));
		if(IsValidNameAndType(name, name_tmp, "Nome", VALIDATE_IS_VAR, GetTypeFromName(name_tmp), 0, 0)) {
			strcpy(name, name_tmp);

	        if(SendMessage(SetBitRadio, BM_GETSTATE, 0, 0) & BST_CHECKED)
		        *set = TRUE;
			else
				*set = FALSE;

			char buf[20];
		    SendMessage(BitCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
			    (LPARAM)(buf));
	        *bit = atoi(buf);
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SetBitDialog);
    return;
}
