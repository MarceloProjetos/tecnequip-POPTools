#include "poptools.h"

static HWND ResetDialog;

static HWND NameTextbox;

static LONG_PTR PrevNameProc;

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
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Name:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 16, 50, 21, ResetDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        62, 16, 115, 321, ResetDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	LoadIOListToComboBox(NameTextbox, IO_TYPE_TOF | IO_TYPE_TON | IO_TYPE_RTO | IO_TYPE_COUNTER);
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        193, 10, 70, 23, ResetDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        193, 40, 70, 23, ResetDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

bool ShowResetDialog(LadderElemReset *elem, unsigned long *idName)
{
	bool changed = false;
	string sname = ladder->getNameIO(*idName);
	const char *name = sname.c_str();
	mapDetails detailsIO = ladder->getDetailsIO(*idName);

	char name_tmp[MAX_NAME_LEN];

    ResetDialog = CreateWindowClient(0, "POPToolsDialog",
        _("Reset"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 276, 75, MainWindow, NULL, Instance, NULL);

    MakeControls();
   
    SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)(name));

    EnableWindow(MainWindow, FALSE);
    ShowWindow(ResetDialog, TRUE);
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

        if(IsDialogMessage(ResetDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
		SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)(name_tmp));
		eType type = ladder->getDetailsIO(name_tmp).type;

		if(ladder->IsValidNameAndType(*idName, name_tmp, type, _("Destino"), VALIDATE_IS_VAR, 0, 0)) {
			pair<unsigned long, int> pin = pair<unsigned long, int>(*idName, 0);

			// No caso do reset, ele deve ser associado a uma variavel ja existente visto que ele deve
			// zerar um contador / timer que existe! Assim checa diretamente com ele se aceita a variavel.
			if(!elem->acceptIO(*idName, type)) {
				ladder->ShowDialog(true, "Contador/Temporizador inválido", "Selecione um contador/temporizador válido!");
			} else if(ladder->getIO(pin, name_tmp, false, type)) {// Variavel valida!
				changed = true;

				*idName = pin.first;
			}
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(ResetDialog);

	return changed;
}
