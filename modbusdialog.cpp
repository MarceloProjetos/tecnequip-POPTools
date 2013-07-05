#include "poptools.h"

static HWND SimpleDialog;

#define MAX_BOXES 5

static HWND Textboxes[MAX_BOXES];
static HWND Labels[MAX_BOXES];

static LONG_PTR PrevAlnumOnlyProc[MAX_BOXES];
static LONG_PTR PrevNumOnlyProc[MAX_BOXES];

static BOOL NoCheckingOnBox[MAX_BOXES];

static HWND SetBitDialog;

static HWND SetBitRadio;
static HWND ClearBitRadio;
static HWND NameTextbox;
static HWND AddressTextbox;
static HWND ElemTextbox;
static HWND RetransmitCheckbox;

static LONG_PTR PrevNameProc;

/*** Helper Functions for MbNodeList ***/

// Populate a Combobox with the names of current items on ModBUS Node List, including an optional "New" item
void PopulateModBUSMasterCombobox(HWND h, bool has_new)
{
	unsigned int index = 0, i = 0;
	LadderMbNode node = ladder->mbGetNodeByIndex(0);

	SendMessage(h, CB_RESETCONTENT, 0, 0);

	if(has_new) {
		SendMessage(h, CB_INSERTSTRING, i++, (LPARAM)((LPCTSTR)"< NOVO >"));
	}

	while(node.name.size() > 0) {
		SendMessage(h, CB_INSERTSTRING, i++, (LPARAM)((LPCTSTR)node.name.c_str()));
		node = ladder->mbGetNodeByIndex(++index);
	}

	SendMessage(h, CB_SETCURSEL, 0, 0);
}

/*** End of Helper Functions for MbNodeList ***/

//-----------------------------------------------------------------------------
// Don't allow any characters other than -A-Za-z0-9_ in the box.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MyAlnumOnlyProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isalpha(wParam) || isdigit(wParam) || wParam == '_' ||
            wParam == '\b' || wParam == '-' || wParam == '\''))
        {
            return 0;
        }
    }

    int i;
    for(i = 0; i < MAX_BOXES; i++) {
        if(hwnd == Textboxes[i]) {
            return CallWindowProc((WNDPROC)PrevAlnumOnlyProc[i], hwnd, msg, 
                wParam, lParam);
        }
    }
    oops();
}

//-----------------------------------------------------------------------------
// Don't allow any characters other than -0-9. in the box.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MyNumOnlyProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isdigit(wParam) || wParam == '.' || wParam == '\b' 
            || wParam == '-'))
        {
            return 0;
        }
    }

    int i;
    for(i = 0; i < MAX_BOXES; i++) {
        if(hwnd == Textboxes[i]) {
            return CallWindowProc((WNDPROC)PrevNumOnlyProc[i], hwnd, msg, 
                wParam, lParam);
        }
    }
    oops();
}

static void MakeControls(void)
{

    SetBitRadio = CreateWindowEx(0, WC_BUTTON, _("32 bits"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 21, 65, 20, SetBitDialog, NULL, Instance, NULL);
    NiceFont(SetBitRadio);

    ClearBitRadio = CreateWindowEx(0, WC_BUTTON, _("16 bits"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 65, 20, SetBitDialog, NULL, Instance, NULL);
    NiceFont(ClearBitRadio);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Variavel:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        115, 16, 60, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        190, 16, 115, 321, SetBitDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	LoadIOListToComboBox(NameTextbox, vector<eType>()); // Vetor vazio, todos os tipos...
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, _("Elemento:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        115, 41, 60, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel1);

    ElemTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        190, 41, 115, 100, SetBitDialog, NULL, Instance, NULL);
    NiceFont(ElemTextbox);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Endereço:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        115, 71, 60, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

    AddressTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        190, 71, 115, 21, SetBitDialog, NULL, Instance, NULL);
    FixedFont(AddressTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        321, 10, 70, 23, SetBitDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        321, 40, 70, 23, SetBitDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Registrador"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 3, 85, 65, SetBitDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    RetransmitCheckbox = CreateWindowEx(0, WC_BUTTON, _("Retransmitir"),
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        7, 70, 100, 40, SetBitDialog, NULL, Instance, NULL);
    NiceFont(RetransmitCheckbox);

    //PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
    //    (LONG_PTR)MyNameProc);
}

bool ShowModbusDialog(int mode_write, pair<unsigned long, int> *name, int *elem, int *address, bool *set, bool *retransmitir)
{
	bool changed = false;

	char name_temp[MAX_NAME_LEN];
    SetBitDialog = CreateWindowClient(0, "POPToolsDialog",
        mode_write ? _("Write ModBUS") : _("Read ModBUS"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 404, 115, MainWindow, NULL, Instance, NULL);

    MakeControls();

    char addr[100];
    sprintf(addr, "%d", *address);

	PopulateModBUSMasterCombobox(ElemTextbox, false);
	SendMessage(ElemTextbox, CB_SETCURSEL, ladder->mbGetIndexByNodeID(*elem), 0);
	SendMessage(ElemTextbox, CB_SETDROPPEDWIDTH, 200, 0);

	if (*set)
        SendMessage(SetBitRadio, BM_SETCHECK, BST_CHECKED, 0);
	else
        SendMessage(ClearBitRadio, BM_SETCHECK, BST_CHECKED, 0);

    if (*retransmitir)
        SendMessage(RetransmitCheckbox, BM_SETCHECK, BST_CHECKED, 0);
	else
        SendMessage(RetransmitCheckbox, BM_SETCHECK, BST_UNCHECKED, 0);

    SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)(ladder->getNameIO(*name).c_str()));
	SendMessage(AddressTextbox, WM_SETTEXT, 0, (LPARAM)(addr));

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
        SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)(name_temp));
		if(ladder->IsValidNameAndType(name->first, name_temp, mode_write ? eType_WriteModbus : eType_ReadModbus)) {
			pair<unsigned long, int> pin = *name;
			if(ladder->getIO(pin, name_temp, false, mode_write ? eType_WriteModbus : eType_ReadModbus)) { // Variavel valida!
				changed = true;

				*name = pin;

				if(SendMessage(SetBitRadio, BM_GETSTATE, 0, 0) & BST_CHECKED)
					*set = TRUE;
				else
					*set = FALSE;

				if(SendMessage(RetransmitCheckbox, BM_GETSTATE, 0, 0) & BST_CHECKED)
					*retransmitir = TRUE;
				else
					*retransmitir = FALSE;

				ladder->mbDelRef(*elem);
				*elem = ladder->mbGetNodeIDByIndex(SendMessage(ElemTextbox, CB_GETCURSEL, 0, 0));
				ladder->mbAddRef(*elem);
				SendMessage(AddressTextbox, WM_GETTEXT, 16, (LPARAM)(addr));

				*address = atoi(addr);
			}
		}
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SetBitDialog);

	return changed;
}
