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

static HWND FARDialog;

static HWND SearchTextbox;
static HWND ReplaceTextbox;
static HWND SearchNextButton;
static HWND ReplaceButton;
static HWND ReplaceAllButton;

static LONG_PTR PrevFARDialogProc;
static LONG_PTR PrevNameProc;

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK FARDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	unsigned int matches, validate_mode;
	wchar_t texto[100];
	char search_text[MAX_NAME_LEN], new_text[MAX_NAME_LEN];

	switch (msg) {
		case WM_COMMAND: {
            HWND h = (HWND)lParam;
			if(wParam == BN_CLICKED) {
				// Read search parameters
				SendMessage(SearchTextbox , WM_GETTEXT, (WPARAM)MAX_NAME_LEN, (LPARAM)(search_text));
				SendMessage(ReplaceTextbox, WM_GETTEXT, (WPARAM)MAX_NAME_LEN, (LPARAM)(new_text   ));

				validate_mode = IsValidNumber(search_text) ? VALIDATE_IS_NUMBER : VALIDATE_IS_VAR;

				if(h == SearchNextButton) {
					matches = FindAndReplace(search_text, NULL, FAR_FIND_NEXT);
		            if(!matches) {
						matches = FindAndReplace(search_text, NULL, FAR_FIND_FIRST);
						if(!matches)
							Error(_("Nao encontrado!"));
					}
			    } else if(h == ReplaceButton) {
					if(IsValidNameAndType(search_text, new_text, "Substituir por", validate_mode | VALIDATE_DONT_ASK | VALIDATE_TYPES_MUST_MATCH | VALIDATE_ACCEPT_IO_PENDING, GetTypeFromName(search_text), 0, 0)) {
			            matches = FindAndReplace(search_text, new_text, FAR_REPLACE_NEXT);
			            if(!matches) {
							matches = FindAndReplace(search_text, new_text, FAR_REPLACE_FIRST);
							if(!matches)
								Error(_("Nao encontrado!"));
						}
					}
	            } else if(h == ReplaceAllButton) {
					if(IsValidNameAndType(search_text, new_text, "Substituir por", validate_mode | VALIDATE_DONT_ASK | VALIDATE_TYPES_MUST_MATCH | VALIDATE_ACCEPT_IO_PENDING, GetTypeFromName(search_text), 0, 0)) {
			            matches = FindAndReplace(search_text, new_text, FAR_REPLACE_ALL);
						swprintf(texto, sizeof(texto)/sizeof(*texto), L"Encontrada(s) %d ocorr�ncia(s)", matches);
						ShowTaskDialog(texto, NULL, TD_INFORMATION_ICON, TDCBF_OK_BUTTON);
					}
				}

				if(matches)
					PaintWindow();

				break;
			}
		}
	}

	return CallWindowProc((WNDPROC)PrevFARDialogProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Localizar:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 16, 90, 21, FARDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    SearchTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        100, 16, 191, 321, FARDialog, NULL, Instance, NULL);
    FixedFont(SearchTextbox);

	LoadIOListToComboBox(SearchTextbox, IO_TYPE_ALL);
	SendMessage(SearchTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Substituir por:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 46, 90, 21, FARDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

    ReplaceTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        100, 46, 191, 321, FARDialog, NULL, Instance, NULL);
    FixedFont(ReplaceTextbox);

	LoadIOListToComboBox(ReplaceTextbox, IO_TYPE_ALL);
	SendMessage(ReplaceTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("Sair"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        216, 75, 75, 53, FARDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

	SearchNextButton = CreateWindowEx(0, WC_BUTTON, _("Localizar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        5, 75, 75, 53, FARDialog, NULL, Instance, NULL); 
    NiceFont(SearchNextButton);

    ReplaceButton = CreateWindowEx(0, WC_BUTTON, _("Substituir"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        85, 75, 126, 23, FARDialog, NULL, Instance, NULL); 
    NiceFont(ReplaceButton);

    ReplaceAllButton = CreateWindowEx(0, WC_BUTTON, _("Substituir Todos"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        85, 105, 126, 23, FARDialog, NULL, Instance, NULL); 
    NiceFont(ReplaceAllButton);
}

void ShowFARDialog()
{
    FARDialog = CreateWindowClient(0, "LDmicroDialog",
        _("Localizar e Substituir"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 296, 135, MainWindow, NULL, Instance, NULL);

	PrevFARDialogProc = SetWindowLongPtr(FARDialog, GWLP_WNDPROC, (LONG_PTR)FARDialogProc);

    MakeControls();
   
	SendMessage(SearchTextbox , WM_SETTEXT, 0, (LPARAM)(POPSettings.last_search_text));
	SendMessage(ReplaceTextbox, WM_SETTEXT, 0, (LPARAM)(POPSettings.last_new_text   ));

    EnableWindow(MainWindow, FALSE);
    ShowWindow(FARDialog, TRUE);
    SetFocus(SearchTextbox);

    MSG msg;
    DWORD ret;
    DialogDone = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN || msg.wParam == VK_ESCAPE) {
                DialogDone = TRUE;
                break;
            }
		}

        if(IsDialogMessage(FARDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(FARDialog);
    return;
}
