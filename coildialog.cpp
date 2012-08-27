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
// Dialog for setting the properties of a relay coils: negated or not,
// plus the name, plus set-only or reset-only
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>

#include "ldmicro.h"

static HWND CoilDialog;

static HWND SourceInternalRelayRadio;
static HWND SourceMcuPinRadio;
static HWND NegatedRadio;
static HWND NormalRadio;
static HWND SetOnlyRadio;
static HWND ResetOnlyRadio;
static HWND NameTextbox;
static HWND BitTextbox;

static LONG_PTR PrevNameProc;
static LONG_PTR PrevCoilDialogProc;

#define MAX_IO_SEEN_PREVIOUSLY 512
extern struct {
    char    name[MAX_NAME_LEN];
    int     type;
    int     pin;
	int		bit;
} IoSeenPreviously[MAX_IO_SEEN_PREVIOUSLY];
extern int IoSeenPreviouslyCount;
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
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Type"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 120, 105, CoilDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    NormalRadio = CreateWindowEx(0, WC_BUTTON, _("( ) Normal"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 21, 100, 20, CoilDialog, NULL, Instance, NULL);
    NiceFont(NormalRadio);

    NegatedRadio = CreateWindowEx(0, WC_BUTTON, _("(/) Negated"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 100, 20, CoilDialog, NULL, Instance, NULL);
    NiceFont(NegatedRadio);

    SetOnlyRadio = CreateWindowEx(0, WC_BUTTON, _("(S) Set-Only"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 61, 100, 20, CoilDialog, NULL, Instance, NULL);
    NiceFont(SetOnlyRadio);

    ResetOnlyRadio = CreateWindowEx(0, WC_BUTTON, _("(R) Reset-Only"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 81, 105, 20, CoilDialog, NULL, Instance, NULL);
    NiceFont(ResetOnlyRadio);

    HWND grouper2 = CreateWindowEx(0, WC_BUTTON, _("Source"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        140, 3, 120, 65, CoilDialog, NULL, Instance, NULL);
    NiceFont(grouper2);

    SourceInternalRelayRadio = CreateWindowEx(0, WC_BUTTON, _("Internal Relay"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        149, 21, 100, 20, CoilDialog, NULL, Instance, NULL);
    NiceFont(SourceInternalRelayRadio);

    SourceMcuPinRadio = CreateWindowEx(0, WC_BUTTON, _("Pin on MCU"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        149, 41, 100, 20, CoilDialog, NULL, Instance, NULL);
    NiceFont(SourceMcuPinRadio); 

    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Name:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        135, 80, 50, 21, CoilDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        190, 80, 100, 321, CoilDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	LoadIOListToComboBox(NameTextbox, IO_TYPE_DIG_OUTPUT | IO_TYPE_INTERNAL_RELAY);
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Bit:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        135, 105, 50, 21, CoilDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	BitTextbox = CreateWindowEx(0, WC_STATIC, _("0"),
        WS_CHILD |  WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        190, 105, 65, 20, CoilDialog, NULL, Instance, NULL);
    NiceFont(BitTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        276, 10, 70, 23, CoilDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        276, 40, 70, 23, CoilDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK CoilDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char name[MAX_NAME_LEN];
	switch(msg) {
		case WM_COMMAND:
            HWND h = (HWND)lParam;
            if(wParam == BN_CLICKED) {
				SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)name);
				if(h == SourceInternalRelayRadio) {
					LoadIOListToComboBox(NameTextbox, IO_TYPE_INTERNAL_RELAY);
				} else if(h == SourceMcuPinRadio) {
					LoadIOListToComboBox(NameTextbox, IO_TYPE_DIG_OUTPUT);
				}
				SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)name);
            }
            break;
	}

	return CallWindowProc((WNDPROC)PrevCoilDialogProc, hwnd, msg, wParam, lParam);
}

void ShowCoilDialog(BOOL *negated, BOOL *setOnly, BOOL *resetOnly, char *name, unsigned char * bit)
{
	unsigned int type;
	char name_tmp[MAX_NAME_LEN];

	CoilDialog = CreateWindowClient(0, "LDmicroDialog",
        _("Coil"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 359, 135, MainWindow, NULL, Instance, NULL);
    RECT r;
    GetClientRect(CoilDialog, &r);

	PrevCoilDialogProc = SetWindowLongPtr(CoilDialog, GWLP_WNDPROC, 
        (LONG_PTR)CoilDialogProc);

    MakeControls();

	char cbit[10];
	_itoa(*bit, cbit, 10);
	SetWindowText(BitTextbox, cbit );

    if(GetTypeFromName(name) == IO_TYPE_INTERNAL_RELAY) {
        SendMessage(SourceInternalRelayRadio, BM_SETCHECK, BST_CHECKED, 0);
		LoadIOListToComboBox(NameTextbox, IO_TYPE_INTERNAL_RELAY);
    } else {
        SendMessage(SourceMcuPinRadio, BM_SETCHECK, BST_CHECKED, 0);
		LoadIOListToComboBox(NameTextbox, IO_TYPE_DIG_OUTPUT);
    }

	strcpy(name_tmp, name);
    SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)name_tmp);

	if(*negated) {
        SendMessage(NegatedRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else if(*setOnly) {
        SendMessage(SetOnlyRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else if(*resetOnly) {
        SendMessage(ResetOnlyRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(NormalRadio, BM_SETCHECK, BST_CHECKED, 0);
    }

    EnableWindow(MainWindow, FALSE);
    ShowWindow(CoilDialog, TRUE);
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

        if(IsDialogMessage(CoilDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
        SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)name_tmp);
        if(SendMessage(SourceInternalRelayRadio, BM_GETSTATE, 0, 0)
            & BST_CHECKED)
        {
            type = IO_TYPE_INTERNAL_RELAY;
        } else {
            type = IO_TYPE_DIG_OUTPUT;
        }

		if(IsValidNameAndType(name, name_tmp, type)) {
			strcpy(name, name_tmp);
            UpdateTypeInCircuit(name, type);

	        if(SendMessage(NormalRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
		        *negated = FALSE;
			    *setOnly = FALSE;
				*resetOnly = FALSE;
	        } else if(SendMessage(NegatedRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
		        *negated = TRUE;
			    *setOnly = FALSE;
				*resetOnly = FALSE;
	        } else if(SendMessage(SetOnlyRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
		        *negated = FALSE;
			    *setOnly = TRUE;
				*resetOnly = FALSE;
	        } else if(SendMessage(ResetOnlyRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
	            *negated = FALSE;
		        *setOnly = FALSE;
			    *resetOnly = TRUE;
	        }

			int i;

			*bit = 0;
			for (i = 0; i < MAX_IO_SEEN_PREVIOUSLY; i++)
			{
				if (_stricmp(IoSeenPreviously[i].name, name)==0)
					*bit = IoSeenPreviously[i].bit;
			}
			//char cbit[10];
			//_itoa(*bit, cbit, 10);
			//SetWindowText(BitTextbox, cbit );
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(CoilDialog);
    return;
}
