#include "poptools.h"

static HWND ContactsDialog;

static HWND NegatedCheckbox;
static HWND SourceInternalRelayRadio;
static HWND SourceInputPinRadio;
static HWND SourceOutputPinRadio;
static HWND InternalFlagRadio;
static HWND NameTextbox;
static HWND BitTextbox;

static LONG_PTR PrevNameProc;
static LONG_PTR PrevContactsDialogProc;

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
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Source"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 3, 120, 105, ContactsDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    SourceInternalRelayRadio = CreateWindowEx(0, WC_BUTTON, _("Internal Relay"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 21, 100, 20, ContactsDialog, NULL, Instance, NULL);
    NiceFont(SourceInternalRelayRadio);

    SourceInputPinRadio = CreateWindowEx(0, WC_BUTTON, _("Input pin"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 100, 20, ContactsDialog, NULL, Instance, NULL);
    NiceFont(SourceInputPinRadio);

    SourceOutputPinRadio = CreateWindowEx(0, WC_BUTTON, _("Pin on MCU"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 61, 100, 20, ContactsDialog, NULL, Instance, NULL);
    NiceFont(SourceOutputPinRadio);

    InternalFlagRadio = CreateWindowEx(0, WC_BUTTON, _("Flag Interna"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 81, 100, 20, ContactsDialog, NULL, Instance, NULL);
    NiceFont(InternalFlagRadio);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Name:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        135, 16, 50, 21, ContactsDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        190, 16, 115, 321, ContactsDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    NegatedCheckbox = CreateWindowEx(0, WC_BUTTON, _("|/| Negated"),
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        190, 85, 160, 20, ContactsDialog, NULL, Instance, NULL);
    NiceFont(NegatedCheckbox);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Bit:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        135, 54, 50, 21, ContactsDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	BitTextbox = CreateWindowEx(0, WC_STATIC, _("0"),
        WS_CHILD |  WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        190, 54, 65, 20, ContactsDialog, NULL, Instance, NULL);
    NiceFont(BitTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        321, 10, 70, 23, ContactsDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        321, 40, 70, 23, ContactsDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK ContactsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char name[MAX_NAME_LEN];
	switch(msg) {
		case WM_COMMAND:
            HWND h = (HWND)lParam;
            if(wParam == BN_CLICKED) {
				SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)name);
				if(h == SourceInternalRelayRadio) {
					LoadIOListToComboBox(NameTextbox, IO_TYPE_INTERNAL_RELAY);
				} else if(h == SourceInputPinRadio) {
					LoadIOListToComboBox(NameTextbox, IO_TYPE_DIG_INPUT);
				} else if(h == SourceOutputPinRadio) {
					LoadIOListToComboBox(NameTextbox, IO_TYPE_DIG_OUTPUT);
				} else if(h == InternalFlagRadio) {
					LoadIOListToComboBox(NameTextbox, IO_TYPE_INTERNAL_FLAG);
				}
				SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)name);
            }
            break;
	}

	return CallWindowProc((WNDPROC)PrevContactsDialogProc, hwnd, msg, wParam, lParam);
}

void ShowContactsDialog(BOOL *negated, char *name, unsigned char * bit)
{
	char name_tmp[MAX_NAME_LEN];
	unsigned int type;

	ContactsDialog = CreateWindowClient(0, "POPToolsDialog",
        _("Contacts"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 404, 115, MainWindow, NULL, Instance, NULL);

	PrevContactsDialogProc = SetWindowLongPtr(ContactsDialog, GWLP_WNDPROC, 
        (LONG_PTR)ContactsDialogProc);

    MakeControls();
 
	char cbit[10];
	_itoa(*bit, cbit, 10);
	SetWindowText(BitTextbox, cbit );

    if(GetTypeFromName(name) == IO_TYPE_INTERNAL_RELAY) {
        SendMessage(SourceInternalRelayRadio, BM_SETCHECK, BST_CHECKED, 0);
		LoadIOListToComboBox(NameTextbox, IO_TYPE_INTERNAL_RELAY);
    } else if(GetTypeFromName(name) == IO_TYPE_DIG_OUTPUT) {
        SendMessage(SourceOutputPinRadio, BM_SETCHECK, BST_CHECKED, 0);
		LoadIOListToComboBox(NameTextbox, IO_TYPE_DIG_OUTPUT);
    } else if(GetTypeFromName(name) == IO_TYPE_DIG_INPUT) {
        SendMessage(SourceInputPinRadio, BM_SETCHECK, BST_CHECKED, 0);
		LoadIOListToComboBox(NameTextbox, IO_TYPE_DIG_INPUT);
    } else { // Internal Flag
        SendMessage(InternalFlagRadio, BM_SETCHECK, BST_CHECKED, 0);
		LoadIOListToComboBox(NameTextbox, IO_TYPE_INTERNAL_FLAG);
    }
    if(*negated) {
        SendMessage(NegatedCheckbox, BM_SETCHECK, BST_CHECKED, 0);
    }

	strcpy(name_tmp, name);
	SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)name_tmp);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(ContactsDialog, TRUE);
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

        if(IsDialogMessage(ContactsDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
        SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)name_tmp);
        if(SendMessage(SourceInternalRelayRadio, BM_GETSTATE, 0, 0)
	        & BST_CHECKED)
		{
			type = IO_TYPE_INTERNAL_RELAY;
        } else if(SendMessage(SourceInputPinRadio, BM_GETSTATE, 0, 0)
	        & BST_CHECKED)
		{
			type = IO_TYPE_DIG_INPUT;
        } else if(SendMessage(InternalFlagRadio, BM_GETSTATE, 0, 0)
	        & BST_CHECKED)
		{
			type = IO_TYPE_INTERNAL_FLAG;
        } else {
			type = IO_TYPE_DIG_OUTPUT;
		}

		if(IsValidNameAndType(name, name_tmp, type)) {
			strcpy(name, name_tmp);
			UpdateTypeInCircuit(name, type);

			if(SendMessage(NegatedCheckbox, BM_GETSTATE, 0, 0) & BST_CHECKED) {
		        *negated = TRUE;
			} else {
				*negated = FALSE;
	        }

			*bit = 0;
			int i;
			for (i = 0; i < MAX_IO_SEEN_PREVIOUSLY; i++)
			{
				if (_stricmp(IoSeenPreviously[i].name, name)==0)
					*bit = IoSeenPreviously[i].bit;
			}

			/*char cbit[10];
			_itoa(*bit, cbit, 10);
			SetWindowText(BitTextbox, cbit );*/
		}

    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(ContactsDialog);
    return;
}
