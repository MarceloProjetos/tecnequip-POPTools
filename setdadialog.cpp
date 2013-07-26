#include "poptools.h"

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

	LoadIOListToComboBox(NameTextbox, vector<eType>()); // Vetor vazio, todos os tipos...
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Modo de Escrita"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 35, 250, 86, SetDADialog, NULL, Instance, NULL);
    NiceFont(grouper);

    RawRadio = CreateWindowEx(0, WC_BUTTON, _("Resolução DA (12 bits)[-2048 ~ 2047]"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 53, 235, 20, SetDADialog, NULL, Instance, NULL);
    NiceFont(RawRadio);

    MvRadio = CreateWindowEx(0, WC_BUTTON, _("Resolução DA (mV)[-10000 ~ 10000]"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 73, 235, 20, SetDADialog, NULL, Instance, NULL);
    NiceFont(MvRadio);

    PctRadio = CreateWindowEx(0, WC_BUTTON, _("Resolução DA (%)[-100 ~ 100]"),
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

bool ShowSetDADialog(string *name, int *mode)
{
	bool changed = false;

	int mode_tmp, min, max;
	char name_tmp[MAX_NAME_LEN];

	SetDADialog = CreateWindowClient(0, "POPToolsDialog",
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

    SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)(name->c_str()));

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

		if(ladder->IsValidNameAndType(ladder->getIdIO(*name), name_tmp, eType_SetDAC, _("Origem"), VALIDATE_IS_VAR_OR_NUMBER, min, max)) {
			changed = true;

			*name = name_tmp;
			*mode = mode_tmp;
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SetDADialog);

	return changed;
}
