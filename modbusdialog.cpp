#include "poptools.h"

static HWND SetBitDialog;

static HWND NameTextbox;
static HWND AddressTextbox;
static HWND ElemTextbox;

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

static void MakeControls(void)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Variavel:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 16, 60, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        82, 16, 115, 321, SetBitDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

	vector<eType> types;
	types.push_back(eType_General);
	types.push_back(eType_ReadModbus);
	types.push_back(eType_WriteModbus);

	LoadIOListToComboBox(NameTextbox, types);
	SendMessage(NameTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, _("Elemento:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 41, 60, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel1);

    ElemTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        82, 41, 115, 100, SetBitDialog, NULL, Instance, NULL);
    NiceFont(ElemTextbox);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Endereço:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 71, 60, 21, SetBitDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

    AddressTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        82, 71, 115, 21, SetBitDialog, NULL, Instance, NULL);
    FixedFont(AddressTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        213, 10, 70, 23, SetBitDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        213, 40, 70, 23, SetBitDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    //PrevNameProc = SetWindowLongPtr(NameTextbox, GWLP_WNDPROC, 
    //    (LONG_PTR)MyNameProc);
}

bool ShowModbusDialog(int mode_write, string *name, int *elem, int *address,
	POINT ElemStart, POINT ElemSize, POINT GridSize)
{
	bool changed = false;

	POINT start = { 100, 100 }, size = { 296, 99 };

	// Se tamanho for definido, devemos posicionar a janela
	if(ElemSize.x > 0 && ElemSize.y > 0) {
		int offset = 50; // espacamento entre a janela e a borda do elemento

		// Primeiro corrige as coordenadas para representar um valor absoluto com relacao
		// ao canto superior esquerdo da tela ao inves do canto de DrawWindow
		RECT rWindow;

		GetWindowRect(DrawWindow, &rWindow);

		ElemStart.x += rWindow.left - ScrollXOffset;
		ElemStart.y += rWindow.top  - ScrollYOffset * GridSize.y;

		// Primeiro tenta posicionar sobre o elemento
		if(ElemStart.y > (size.y + offset)) {
			start.y = ElemStart.y - (size.y + offset);
		} else { // Senao posiciona abaixo
			start.y = ElemStart.y + ElemSize.y + offset;
		}

		start.x = ElemStart.x + (ElemSize.x - size.x)/2;
		if(start.x < 0) {
			start.x = 0;
		} else if(start.x + size.x > rWindow.right) {
			start.x = rWindow.right - size.x;
		}
	}

	char name_temp[MAX_NAME_LEN];
    SetBitDialog = CreateWindowClient(0, "POPToolsDialog",
        mode_write ? _("Escrever Registrador do ModBUS") : _("Ler Registrador do ModBUS"), WS_OVERLAPPED | WS_SYSMENU,
        start.x, start.y, size.x, size.y, MainWindow, NULL, Instance, NULL);

    MakeControls();

    char addr[100];
    sprintf(addr, "%d", *address);

	PopulateModBUSMasterCombobox(ElemTextbox, false);
	SendMessage(ElemTextbox, CB_SETCURSEL, ladder->mbGetIndexByNodeID(*elem), 0);
	SendMessage(ElemTextbox, CB_SETDROPPEDWIDTH, 200, 0);

    SendMessage(NameTextbox, WM_SETTEXT, 0, (LPARAM)(name->c_str()));
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
		SendMessage(AddressTextbox, WM_GETTEXT, 16, (LPARAM)(addr));

		if(ladder->IsValidNumber(addr)) {
			changed = true;

	        SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)(name_temp));
			*name = name_temp;

			ladder->mbDelRef(*elem);
			*elem = ladder->mbGetNodeIDByIndex(SendMessage(ElemTextbox, CB_GETCURSEL, 0, 0));
			ladder->mbAddRef(*elem);

			*address = atoi(addr);
		} else {
			ladder->ShowDialog(eDialogType_Error, false, _("Número Inválido"), _("Número '%s' inválido!"), addr);
		}
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SetBitDialog);
	SetFocus(MainWindow);

	return changed;
}
