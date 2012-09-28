#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>

#include "poptools.h"

static HWND CheckBitDialog;

static HWND SetBitRadio;
static HWND ClearBitRadio;
static HWND NameTextbox;
static HWND BitCombobox;

static LONG_PTR PrevNameProc;

const LPCTSTR ComboboxBitItens[] = { "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", "10", 
									"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
									"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
									"31"/*, "32"*/};

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
        7, 3, 120, 65, CheckBitDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Name:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        135, 16, 50, 21, CheckBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    SetBitRadio = CreateWindowEx(0, WC_BUTTON, _("Ligado"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 21, 100, 20, CheckBitDialog, NULL, Instance, NULL);
    NiceFont(SetBitRadio);

    ClearBitRadio = CreateWindowEx(0, WC_BUTTON, _("Desligado"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 100, 20, CheckBitDialog, NULL, Instance, NULL);
    NiceFont(ClearBitRadio);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        190, 16, 115, 321, CheckBitDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	LoadIOListToComboBox(NameTextbox, IO_TYPE_ALL);
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Bit:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        135, 41, 50, 21, CheckBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	BitCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        190, 41, 65, 140, CheckBitDialog, NULL, Instance, NULL);
    NiceFont(BitCombobox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        321, 10, 70, 23, CheckBitDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        321, 40, 70, 23, CheckBitDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

void ShowCheckBitDialog(char *name, int * set, int * bit)
{
	char name_tmp[MAX_NAME_LEN];

	CheckBitDialog = CreateWindowClient(0, "LDmicroDialog",
        _("Check Bit"), WS_OVERLAPPED | WS_SYSMENU,
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
    ShowWindow(CheckBitDialog, TRUE);
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

        if(IsDialogMessage(CheckBitDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
        SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)(name_tmp));

		if(IsValidNameAndType(name, name_tmp, GetTypeFromName(name))) {
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
    DestroyWindow(CheckBitDialog);
    return;
}
