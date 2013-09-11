#include "poptools.h"

static HWND SetBitDialog;

static HWND NameTextbox;
static HWND BitCombobox;

static LONG_PTR PrevNameProc;

const LPCTSTR ComboboxBitItens[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 
									"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
									"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
									"31"};

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

static void MakeControls(char *varname, vector<eType> types)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, varname,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 16, 50, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        62, 16, 115, 21, SetBitDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	LoadIOListToComboBox(NameTextbox, types); // Vetor vazio, todos os tipos...
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

	HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Bit:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 41, 50, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	BitCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        62, 41, 65, 140, SetBitDialog, NULL, Instance, NULL);
    NiceFont(BitCombobox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        193, 10, 70, 23, SetBitDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        193, 40, 70, 23, SetBitDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

bool ShowVarBitDialog(char *title, char *varname, string *name, int * bit, vector<eType> types)
{
	bool changed = false;

	char name_tmp[MAX_NAME_LEN];

    SetBitDialog = CreateWindowClient(0, "POPToolsDialog",
        title, WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 276, 75, MainWindow, NULL, Instance, NULL);

    MakeControls(varname, types);
   
	int i;

	for (i = 0; i < sizeof(ComboboxBitItens) / sizeof(ComboboxBitItens[0]); i++)
		SendMessage(BitCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBitItens[i]));

	SendMessage(BitCombobox, CB_SETCURSEL, *bit, 0);
    SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)(name->c_str()));

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
		changed = true;

		*name = name_tmp;

		char buf[20];
		SendMessage(BitCombobox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		*bit = atoi(buf);
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SetBitDialog);

	return changed;
}
