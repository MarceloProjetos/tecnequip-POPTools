#include "poptools.h"

static HWND DebugDialog;

#define INPUT_SIZE  19
#define OUTPUT_SIZE 16

static HWND ModeTCPRadio;
static HWND Mode485Radio;
static HWND SendRadio;
static HWND SendButton;
static HWND ReceiveRadio;
static HWND NameTextbox;
static HWND AddressTextbox;
static HWND IDTextbox;
static HWND txtIP;
static HWND txtCOM;
static HWND RetransmitCheckbox;
static HWND ConfigButton;
static HWND ClearButton;
static HWND OkButton;
static HWND CommList;
static HWND txtInfoDate;
static HWND txtInfoTime;
static HWND SetDateTimeButton;
static HWND CheckInput[19];
static HWND CheckOutput[16];

static LONG_PTR PrevIdProc;

static bool DebugDone = false;

// CommList Column index
#define COMMLIST_COLUMN_IFACE 0x00
#define COMMLIST_COLUMN_REPLY 0x01
#define COMMLIST_COLUMN_ID    0x02
#define COMMLIST_COLUMN_FC    0x03
#define COMMLIST_COLUMN_REG   0x04
#define COMMLIST_COLUMN_VALUE 0x05

// RotateList definitions
#define ROTATELIST_SIZE    50
#define ROTATELIST_COLUMNS  6

// RotateList Structs
struct RotateListItem {
	char txt[ROTATELIST_COLUMNS][100];
};

struct RotateList {
	bool clear;
	unsigned int start, end;
	struct RotateListItem items[ROTATELIST_SIZE];
};

// RotateList Functions
void RotateList_IncPointer(unsigned int &p)
{
	p++;
	if(p == ROTATELIST_SIZE) {
		p = 0;
	}
}

void RotateList_IncStart(struct RotateList *rl)
{
	RotateList_IncPointer(rl->start);
}

void RotateList_IncEnd(struct RotateList *rl)
{
	if(rl->start == rl->end && !rl->clear) {
		RotateList_IncPointer(rl->start);
	}

	RotateList_IncPointer(rl->end);
}

unsigned int RotateList_GetCount(struct RotateList *rl)
{
	unsigned int count;

	if(rl->start < rl->end) {
		count = rl->end - rl->start;
	} else {
		count = ROTATELIST_SIZE - rl->start + rl->end;
	}

	return count;
}

unsigned int RotateList_GetIndex(struct RotateList *rl, unsigned int item)
{
	unsigned int idx;

	if(item >= RotateList_GetCount(rl)) {
		return 0;
	}

	idx = rl->start + item;
	if(idx >= ROTATELIST_SIZE) {
		idx -= ROTATELIST_SIZE;
	}

	return idx;
};

unsigned int RotateList_GetPos(struct RotateList *rl, unsigned int index)
{
	if(index >= RotateList_GetCount(rl)) {
		return 0;
	} else if(index >= rl->start) {
		return index - rl->start;
	} else {
		return (ROTATELIST_SIZE - 1 - rl->start) + (rl->end - index); // (right len) + (left len)
	}
};

char *RotateList_GetItem(struct RotateList *rl, unsigned int idx, unsigned int column)
{
	if(idx >= RotateList_GetCount(rl)) {
		return "";
	}

	return rl->items[idx].txt[column];
}

unsigned int RotateList_InsertItem(struct RotateList *rl, struct RotateListItem *item)
{
	unsigned int i, idx = rl->end;

	for(i=0; i<6; i++) { // 6 = Columns Count
		strcpy(rl->items[idx].txt[i], item->txt[i]);
	}

	RotateList_IncEnd(rl);

	rl->clear = false;

	return idx;
}

void RotateList_Init(struct RotateList *rl)
{
	rl->clear = true;
	rl->start = rl->end = 0;
	memset(rl->items, 0, sizeof(rl->items));
}

struct RotateList RotateLog;

char *LogGet(unsigned int item, unsigned int column)
{
	return RotateList_GetItem(&RotateLog, RotateList_GetIndex(&RotateLog, item), column);
}

unsigned int LogPut(bool mode_tcp, unsigned int Id, unsigned int FunctionCode, unsigned int Reg, unsigned int Val, unsigned int ExceptionCode)
{
	struct RotateListItem item;

	strcpy (item.txt[COMMLIST_COLUMN_IFACE],           mode_tcp ? _("Ethernet") : _("Serial"));
	strcpy (item.txt[COMMLIST_COLUMN_REPLY],           ExceptionCode == MODBUS_EXCEPTION_NONE ? _("OK") : _("ERRO"));
	sprintf(item.txt[COMMLIST_COLUMN_ID   ], "%d"    , Id);
	strcpy (item.txt[COMMLIST_COLUMN_FC   ],           FunctionCode == MODBUS_FC_READ_HOLDING_REGISTERS ? _("Ler") : _("Escrever"));
	sprintf(item.txt[COMMLIST_COLUMN_REG  ], "%d"    , Reg);
	sprintf(item.txt[COMMLIST_COLUMN_VALUE], "0x%04X", Val);

	return RotateList_InsertItem(&RotateLog, &item);
}

static void CommListProc(NMHDR *h)
{
    switch(h->code) {
        case LVN_GETDISPINFO: {
            NMLVDISPINFO *i = (NMLVDISPINFO *)h;
            switch(i->item.iSubItem) {
                case COMMLIST_COLUMN_ID:
                case COMMLIST_COLUMN_FC:
                case COMMLIST_COLUMN_REG:
                case COMMLIST_COLUMN_VALUE:
                case COMMLIST_COLUMN_IFACE:
                case COMMLIST_COLUMN_REPLY:
                        strcpy(i->item.pszText, LogGet(RotateList_GetCount(&RotateLog) - i->item.lParam - 1, i->item.iSubItem));
                    break;
            }
            break;
        }
    }
}

int CALLBACK CompareCommList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return lParam1 > lParam2 ? -1 : +1;
}

void CommList_InsertItem(struct MODBUS_Device *dev, unsigned int FunctionCode, MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
	int val = 0;
	unsigned int i, reg = 0;
	bool mode_tcp = MODBUS_MODE_IS_TCP(dev->mode);

	// Check if invalid
	if(data == NULL || reply == NULL) {
		return;
	}

	switch(FunctionCode) {
	case MODBUS_FC_READ_HOLDING_REGISTERS:
		reg = data->read_holding_registers.start;
		val = (int)(reply->reply.read_holding_registers.data[1])<<8 | reply->reply.read_holding_registers.data[0];
		break;

	case MODBUS_FC_WRITE_SINGLE_REGISTER:
		reg = data->write_single_register.address;
		val = data->write_single_register.val;
		break;
	}

	LogPut(mode_tcp, dev->identification.Id, FunctionCode, reg, val, reply->ExceptionCode);

	ListView_DeleteAllItems(CommList);
	for(i=0; i<RotateList_GetCount(&RotateLog); i++) {
		LVITEM lvi;
		lvi.mask        = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
		lvi.state       = lvi.stateMask = 0;
		lvi.iItem       = i;
		lvi.iSubItem    = 0;
		lvi.pszText     = LPSTR_TEXTCALLBACK;
		lvi.lParam      = i;

		if(ListView_InsertItem(CommList, &lvi) < 0) oops();
	}

//	ListView_RedrawItems(CommList, 0, RotateList_GetCount(&RotateLog) - 1);
}

/*** Trecho relacionado ao protocolo ModBUS/TCP ***/

void MB_Transfer(bool mode_send, bool mode_tcp, bool retry)
{
	MODBUS_FCD_Data MBData;
	MODBUS_Reply MBReply;

	char txt[MAX_NAME_LEN];
	unsigned int iID, iReg;
	int iVal;

	/*** Get parameters ***/
	SendMessage(IDTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)txt);
	iID = atoi(txt);

	SendMessage(AddressTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)txt);
	iReg = atoi(txt);

	SendMessage(NameTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)txt);
	iVal = atoi(txt);

	/*** Check parameters ***/
	if(iID < 0) {
		Error(_("ID deve ser maior ou igual a zero!"));
	} else if(iReg < 0) {
		Error(_("Registrador deve ser maior que zero!"));
	} else { // All OK, let's start the communication!
		MODBUS_Device *mbdev;
		unsigned int retries = retry ? 2 : 0;

		if(mode_tcp) {
			mbdev = &MBDev_TCP;
		} else {
			mbdev = &MBDev_Serial;
			if(!OpenCOMPort(POPSettings.COMPortDebug, Prog.settings.baudRate, SerialConfig[Prog.settings.UART].bByteSize,
					SerialConfig[Prog.settings.UART].bParity, SerialConfig[Prog.settings.UART].bStopBits)) {
				Error(_("Erro abrindo porta serial!"));
				return;
			}
		}

		mbdev->identification.Id = iID;

		if(mode_send) {
			MBData.write_single_register.address = iReg;
			MBData.write_single_register.val = iVal;
		} else {
			MBData.read_holding_registers.quant = 1;
			MBData.read_holding_registers.start = iReg;
		}

		do {
			MBReply = Modbus_RTU_Send(mbdev, 0, mode_send ? MODBUS_FC_WRITE_SINGLE_REGISTER : MODBUS_FC_READ_HOLDING_REGISTERS, &MBData);

			if(MBReply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
				break;
			} else if (retries) {
				Sleep(100);
			}
		} while(retries--);

		CloseCOMPort(); // If COM Port closed, does nothing...

		CommList_InsertItem(mbdev, mode_send ? MODBUS_FC_WRITE_SINGLE_REGISTER : MODBUS_FC_READ_HOLDING_REGISTERS, &MBData, &MBReply);
	}
}

/*** Fim do ModBUS/TCP ***/

static void SyncConfigToScreen(void)
{
	char buf[30];

	sprintf(buf, "%d.%d.%d.%d", Prog.settings.ip[0], Prog.settings.ip[1], Prog.settings.ip[2], Prog.settings.ip[3]);
	SendMessage(txtIP, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "COM%d", POPSettings.COMPortDebug);
	SendMessage(txtCOM, WM_SETTEXT, 0, (LPARAM)buf);
}

void CALLBACK DebugUpdateInfo(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
	struct tm t;

	if(USB_GetDateTime(&t)) { // POP Connected and Date/Time retrieved
		char buf[MAX_NAME_LEN];

		sprintf(buf, "%02d/%02d/%04d", t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
		SendMessage(txtInfoDate, WM_SETTEXT, 0, (LPARAM)buf);

		sprintf(buf, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
		SendMessage(txtInfoTime, WM_SETTEXT, 0, (LPARAM)buf);

		// Now we will update Input/Output state
		int index;
		unsigned int io_state;

		// First, Input state
		io_state = USB_GetInput();
		for(index = ARRAY_SIZE(CheckInput) - 1; index >= 0; index--)
			SendMessage(CheckInput[index], BM_SETCHECK, ((io_state >> index) & 1) ? BST_CHECKED : BST_UNCHECKED,  0);

		// Then, Output state
		io_state = USB_GetOutput();
		for(index = ARRAY_SIZE(CheckOutput) - 1; index >= 0; index--)
			SendMessage(CheckOutput[index], BM_SETCHECK, ((io_state >> index) & 1) ? BST_CHECKED : BST_UNCHECKED,  0);
	}

}

//-----------------------------------------------------------------------------
// Don't allow any characters other than 0-9. in the text boxes.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MyNumberProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isdigit(wParam) || wParam == '.' || wParam == '\b')) {
            return 0;
        }
    }

    return CallWindowProc((WNDPROC)PrevIdProc, hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
LRESULT CALLBACK DebugDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_NOTIFY: {
            NMHDR *h = (NMHDR *)lParam;
            if(h->hwndFrom == CommList) {
                CommListProc(h);
			}
            return 0;
		}

		case WM_COMMAND: {
            HWND h = (HWND)lParam;
			if(wParam == BN_CLICKED) {
				if(h == SendButton) {
					MB_Transfer(SendMessage(SendRadio, BM_GETSTATE, 0, 0) & BST_CHECKED, SendMessage(ModeTCPRadio, BM_GETSTATE, 0, 0) & BST_CHECKED, SendMessage(RetransmitCheckbox, BM_GETSTATE, 0, 0) & BST_CHECKED);
			    } else if(h == OkButton) {
					DebugDone = TRUE;
				} else if(h == ConfigButton) {
					Button_Enable(OkButton    , FALSE);
					Button_Enable(SendButton  , FALSE);
					Button_Enable(ConfigButton, FALSE);

					if(SendMessage(ModeTCPRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
						ShowConfDialog(true);
					} else {
						ShowPrefDialog();
					}

					SyncConfigToScreen();
					EnableWindow(MainWindow, FALSE);

					Button_Enable(OkButton    , TRUE);
					Button_Enable(SendButton  , TRUE);
					Button_Enable(ConfigButton, TRUE);
				} else if(h == ClearButton) {
					RotateList_Init(&RotateLog);
					ListView_DeleteAllItems(CommList);
				} else if(h == SetDateTimeButton) {
					USB_SetDateTime(NULL);
				}

				break;
			}
		 }

        case WM_DESTROY: {
			HWND h = (HWND)lParam;
			if(h == DebugDialog)
	            DebugDone = TRUE;
            break;
		 }

        case WM_CLOSE:
            DebugDone = TRUE;
			break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

static void MakeControls(void)
{
	int index;
    LVCOLUMN lvc;

	SendRadio = CreateWindowEx(0, WC_BUTTON, _("Escrever"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 21, 65, 20, DebugDialog, NULL, Instance, NULL);
    NiceFont(SendRadio);

    ReceiveRadio = CreateWindowEx(0, WC_BUTTON, _("Ler"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 41, 70, 20, DebugDialog, NULL, Instance, NULL);
    NiceFont(ReceiveRadio);

    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Operação"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 3, 85, 65, DebugDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Valor:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        380, 16, 70, 21, DebugDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    NameTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        455, 16, 115, 21, DebugDialog, NULL, Instance, NULL);
    FixedFont(NameTextbox);

    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, _("ID:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        380, 43, 70, 21, DebugDialog, NULL, Instance, NULL);
    NiceFont(textLabel1);

    IDTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        455, 43, 115, 21, DebugDialog, NULL, Instance, NULL);
    FixedFont(IDTextbox);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Registrador:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        374, 71, 76, 21, DebugDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

    AddressTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        455, 71, 115, 21, DebugDialog, NULL, Instance, NULL);
    FixedFont(AddressTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("Sair"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        589, 520, 70, 30, DebugDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    ClearButton = CreateWindowEx(0, WC_BUTTON, _("Limpar Registro"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        267, 367, 120, 30, DebugDialog, NULL, Instance, NULL); 
    NiceFont(ClearButton);

    ConfigButton = CreateWindowEx(0, WC_BUTTON, _("Configurações"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        7, 520, 120, 30, DebugDialog, NULL, Instance, NULL); 
    NiceFont(ConfigButton);

    SendButton = CreateWindowEx(0, WC_BUTTON, _("Executar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        580, 16, 77, 75, DebugDialog, NULL, Instance, NULL); 
    NiceFont(SendButton);

	ModeTCPRadio = CreateWindowEx(0, WC_BUTTON, _("Ethernet"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        109, 21, 65, 20, DebugDialog, NULL, Instance, NULL);
    NiceFont(ModeTCPRadio);

    Mode485Radio = CreateWindowEx(0, WC_BUTTON, _("Serial"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        109, 41, 70, 20, DebugDialog, NULL, Instance, NULL);
    NiceFont(Mode485Radio);

    HWND grouper2 = CreateWindowEx(0, WC_BUTTON, _("Interface"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        100, 3, 85, 65, DebugDialog, NULL, Instance, NULL);
    NiceFont(grouper2);

    HWND grouper3 = CreateWindowEx(0, WC_BUTTON, _("Configurações"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        193, 3, 179, 89, DebugDialog, NULL, Instance, NULL);
    NiceFont(grouper3);

    textLabel = CreateWindowEx(0, WC_STATIC, _("IP:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 25, 65, 21, grouper3, NULL, Instance, NULL);
    NiceFont(textLabel);

    txtIP = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        85, 25, 85, 21, grouper3, NULL, Instance, NULL);
    NiceFont(txtIP);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Porta de Depuração:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        2, 46, 73, 41, grouper3, NULL, Instance, NULL);
    NiceFont(textLabel);

    txtCOM = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        85, 56, 85, 21, grouper3, NULL, Instance, NULL);
    NiceFont(txtCOM);

    RetransmitCheckbox = CreateWindowEx(0, WC_BUTTON, _("Retransmitir Pacote"),
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        31, 75, 130, 20, DebugDialog, NULL, Instance, NULL);
    NiceFont(RetransmitCheckbox);

    CommList = CreateWindowEx(WS_EX_STATICEDGE, WC_LISTVIEW, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE |
		LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
        17, 120, 630, 240, DebugDialog, NULL, Instance, NULL);
    ListView_SetExtendedListViewStyle(CommList, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES);
    FixedFont(CommList);

    HWND grouperReply = CreateWindowEx(0, WC_BUTTON, _("Registro"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 100, 650, 305, DebugDialog, NULL, Instance, NULL);
    NiceFont(grouperReply);

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;

#define LV_ADD_COLUMN(hWnd, i, w, s) do { \
        lvc.iSubItem = i; \
        lvc.pszText = s; \
        lvc.iOrder = 0; \
        lvc.cx = w; \
        ListView_InsertColumn(hWnd, i, &lvc); \
    } while(0)

    LV_ADD_COLUMN(CommList, COMMLIST_COLUMN_IFACE, 100, _("Interface"));
    LV_ADD_COLUMN(CommList, COMMLIST_COLUMN_REPLY, 100, _("Resposta"));
    LV_ADD_COLUMN(CommList, COMMLIST_COLUMN_ID   , 100, _("ID"));
    LV_ADD_COLUMN(CommList, COMMLIST_COLUMN_FC   , 100, _("Função"));
    LV_ADD_COLUMN(CommList, COMMLIST_COLUMN_REG  , 100, _("Registrador"));
    LV_ADD_COLUMN(CommList, COMMLIST_COLUMN_VALUE, 100, _("Valor"));

	ListView_SetBkColor     (CommList, RGB(0x00,0x00,0x00));
	ListView_SetTextBkColor (CommList, RGB(0x00,0x00,0x00));
	ListView_SetTextColor   (CommList, RGB(0x00,0xFF,0xFF));

    SetDateTimeButton = CreateWindowEx(0, WC_BUTTON, _("Atualizar Data/Hora"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        17, 470, 150, 30, DebugDialog, NULL, Instance, NULL); 
    NiceFont(SetDateTimeButton);

    grouper = CreateWindowEx(0, WC_BUTTON, _("Informações da POP-7 (Lido a partir da interface USB)"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 410, 650, 100, DebugDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Data:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 20, 50, 18, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

    txtInfoDate = CreateWindowEx(0, WC_STATIC, _("--/--/----"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        70, 20, 100, 18, grouper, NULL, Instance, NULL);
    NiceFont(txtInfoDate);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Hora:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 40, 50, 18, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

    txtInfoTime = CreateWindowEx(0, WC_STATIC, _("--:--:--"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        70, 40, 100, 18, grouper, NULL, Instance, NULL);
    NiceFont(txtInfoTime);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Entradas:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        200, 40, 60, 18, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Saídas:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        200, 60, 60, 18, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

	for(index = ARRAY_SIZE(CheckInput) - 1; index >= 0; index--) {
		char buf[3];
		sprintf(buf, "%d", index+1);
		textLabel = CreateWindowEx(0, WC_STATIC, buf,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
			270 + index*20, 20, 15, 18, grouper, NULL, Instance, NULL);
		NiceFont(textLabel);

		CheckInput[index] = CreateWindowEx(0, WC_BUTTON, "",
			WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
			270 + index*20, 40, 15, 18, grouper, NULL, Instance, NULL);
		NiceFont(CheckInput[index]);

		if(index < ARRAY_SIZE(CheckOutput)) {
			CheckOutput[index] = CreateWindowEx(0, WC_BUTTON, "",
				WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
				270 + index*20, 60, 15, 18, grouper, NULL, Instance, NULL);
			NiceFont(CheckOutput[index]);
		}
	}

	PrevIdProc = SetWindowLongPtr(IDTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNumberProc);

    SetTimer(DebugDialog, TIMER_DEBUG_INFO, 1000, DebugUpdateInfo);
}

void ShowDebugDialog(void)
{
	RotateList_Init(&RotateLog);

	DebugDialog = CreateWindowClient(0, "POPDebugDialog",
        _("Debug"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 664, 555, MainWindow, NULL, Instance, NULL);

    MakeControls();
	SyncConfigToScreen();

	EnableWindow(MainWindow, FALSE);
    ShowWindow(DebugDialog, TRUE);
    SetFocus(NameTextbox);

	SendMessage(SendRadio         , BM_SETCHECK, BST_CHECKED,  0);
	SendMessage(ModeTCPRadio      , BM_SETCHECK, BST_CHECKED,  0);
	SendMessage(RetransmitCheckbox, BM_SETCHECK, BST_CHECKED,  0);

    MSG msg;
    DWORD ret;
    DebugDone = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DebugDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_ESCAPE) {
                DebugDone = TRUE;
                break;
            }
        }
        if(IsDialogMessage(DebugDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	EnableWindow(MainWindow, TRUE);
    DestroyWindow(DebugDialog);
    return;
}
