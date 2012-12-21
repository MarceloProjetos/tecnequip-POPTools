#include "poptools.h"

static HWND ConfDialog;
static HWND ConfigTreeView;
static HWND GroupCommNetwork;
static HWND GroupCommTimeZone;
static HWND GroupCommSerial;
static HWND GroupPosicEncInc;
static HWND GroupPosicEncAbs;
static HWND GroupModBUSSlave;
static HWND GroupModBUSMaster;

static HWND CrystalTextbox;
static HWND CycleTextbox;
static HWND BaudTextbox;
static HWND ModBUSIDTextbox;
static HWND BaudRateCombobox;
static HWND ParityCombobox;
static HWND SNTPCombobox;
static HWND GMTCombobox;
static HWND DailySaveCheckbox;
static HWND SSISizeTextbox;
static HWND SSIModeCombobox;
static HWND ip;
static HWND mask;
static HWND gw;
static HWND dns;

static HWND DiameterTextbox;
static HWND PulsesTextbox;
static HWND FactorTextbox;
static HWND X4Checkbox;
static HWND X2Checkbox;

static LONG_PTR PrevConfDialogProc;
static LONG_PTR PrevCrystalProc;
static LONG_PTR PrevCycleProc;
static LONG_PTR PrevBaudProc;

//const LPCTSTR ComboboxPLCItens[] = { _("POP7"), _("POP9") };

const LPCTSTR ComboboxGMTItens[] = { _("(GMT-12:00) Linha de Data Internacional Oeste"), _("(GMT-11:00) Ilhas Midway,Samoa"),
								_("(GMT-10:00) Hawa�"), _("(GMT-09:00) Alasca"),
								_("(GMT-08:00) Hora do Pac�fico"), _("(GMT-07:00) Hora das Montanhas (EUA e Canad�)"),
								_("(GMT-06:00) Am�rica Central, Hora Central EUA/Canad�"), _("(GMT-05:00) Lima, Bogot�"),
								_("(GMT-04:00) Rio Branco, Manaus, Caracas, La Paz"), _("(GMT-03:00) Brasilia, Buenos Aires"),
								_("(GMT-02:00) Atl�ntico Central"), _("(GMT-01:00) A�ores, Cabo Verde"),
								_("(GMT 00:00) Hora de Greenwich: Londres, Dublin, Lisboa"), _("(GMT+01:00) Berlim, Estocolmo, Roma, Bruxelas"),
								_("(GMT+02:00) Atenas, Helsinque, Leste Europeu, Jerusal�m"), _("(GMT+03:00) Bagd�, Kuwait, Nair�bi, Moscou,Riad"),
								_("(GMT+04:00) Abu Dhabi, Mascate, Hora Padr�o do C�ucaso"), _("(GMT+05:00) Islamabad, Karachi, Ekaterinburgo"),
								_("(GMT+06:00) Almaty, Dacca"), _("(GMT+07:00) Bangcoc, Jacarta, Han�i"),
								_("(GMT+08:00) Pequim, Hong Kong, Taiwan, Cingapura"), _("(GMT+09:00) T�quio, Osaka, Sapporo, Seul, Yakutsk"),
								_("(GMT+10:00) Brisbane, Camberra, Melbourne, Sydney"), _("(GMT+11:00) Magad�, Ilhas Salom�o, Nova Caled�nia"),
								_("(GMT+12:00) Fiji, Kamchatka, Auckland"), _("(GMT+13:00) Nuku'alofa") };

const LPCTSTR ComboboxBaudRateItens[] = { /*"2400", "4800", "7200",*/ "9600", "14400", "19200"/*, "28800", 
						"38400", "57600", "115200"*/ };

// http://www.ntp.br/
// http://pt.wikipedia.org/wiki/Network_Time_Protocol
// http://tf.nist.gov/tf-cgi/servers.cgi
// https://support.ntp.org/bin/view/Servers/StratumOneTimeServers
// http://support.ntp.org/bin/view/Servers/NTPPoolServers
const LPCTSTR ComboboxSNTPItens[] = { "br.pool.ntp.org", "a.ntp.br", "gps.ntp.br", "pool.ntp.org", "time-nw.nist.gov", "time.microsoft.com" };

struct strSerialConfig SerialConfig[] = {
	{ 8, NOPARITY  , ONESTOPBIT, _("8-None-1") },
	{ 8, EVENPARITY, ONESTOPBIT, _("8-Even-1") },
	{ 8, ODDPARITY , ONESTOPBIT, _("8-Odd-1" ) },
	{ 7, NOPARITY  , ONESTOPBIT, _("7-None-1") },
	{ 7, EVENPARITY, ONESTOPBIT, _("7-Even-1") },
	{ 7, ODDPARITY , ONESTOPBIT, _("7-Odd-1" ) }
};

char *EncAbsConfig[] = { _("Leitura Gray"), _("Leitura Bin�rio") };

char *SerialParityString[] = { _("Sem Paridade"), _("Paridade �mpar"), _("Paridade Par") };

#define CONFTVI_ID_COMM          0
#define CONFTVI_ID_COMM_NETWORK  1
#define CONFTVI_ID_COMM_FUSE     2
#define CONFTVI_ID_COMM_SERIAL   3
#define CONFTVI_ID_POSIC         4
#define CONFTVI_ID_POSIC_ENCINC  5
#define CONFTVI_ID_POSIC_ENCABS  6
#define CONFTVI_ID_MODBUS        7
#define CONFTVI_ID_MODBUS_MASTER 8
#define CONFTVI_ID_MODBUS_SLAVE  9

void OnSelChanged(int ID)
{
	int i;
	HWND group, groups[] = { GroupCommNetwork, GroupCommTimeZone, GroupCommSerial, GroupPosicEncInc,
								GroupPosicEncAbs, GroupModBUSMaster, GroupModBUSSlave };

	switch(ID) {
		case CONFTVI_ID_COMM:
		case CONFTVI_ID_COMM_NETWORK:
			group = GroupCommNetwork;
			break;

		case CONFTVI_ID_COMM_FUSE:
			group = GroupCommTimeZone;
			break;

		case CONFTVI_ID_COMM_SERIAL:
			group = GroupCommSerial;
			break;

		case CONFTVI_ID_POSIC:
		case CONFTVI_ID_POSIC_ENCINC:
			group = GroupPosicEncInc;
			break;

		case CONFTVI_ID_POSIC_ENCABS:
			group = GroupPosicEncAbs;
			break;

		case CONFTVI_ID_MODBUS:
		case CONFTVI_ID_MODBUS_MASTER:
			group = GroupModBUSMaster;
			break;

		case CONFTVI_ID_MODBUS_SLAVE:
			group = GroupModBUSSlave;
			break;

		default:
			return;
	}

	for(i=0; i < ARRAY_SIZE(groups); i++) {
		EnableWindow(groups[i], groups[i] == group ? TRUE          : FALSE  );
		ShowWindow  (groups[i], groups[i] == group ? SW_SHOWNORMAL : SW_HIDE);
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

	LONG_PTR t;
    if(hwnd == CrystalTextbox)
        t = PrevCrystalProc;
    else if(hwnd == CycleTextbox)
        t = PrevCycleProc;
    else if(hwnd == BaudTextbox)
        t = PrevBaudProc;
    else
        oops();

    return CallWindowProc((WNDPROC)t, hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK ConfDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
        case WM_NOTIFY: {
			LPNMHDR nm = (LPNMHDR)lParam;

			if(nm->hwndFrom == ConfigTreeView) {
				switch(nm->code) {
				case NM_CUSTOMDRAW:
					return CDRF_DODEFAULT;
					break;

				case TVN_SELCHANGED:
					OnSelChanged(((LPNMTREEVIEW)lParam)->itemNew.lParam);
					return 0;

				default:
					return 0;
				}
	        }
            break;
		}

		case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == OkButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
            } else if(h == CancelButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
			}
            break;
			}

        case WM_CLOSE:
        case WM_DESTROY:
            DialogDone = TRUE;
            DialogCancel = TRUE;
            break;
	}

	return CallWindowProc((WNDPROC)PrevConfDialogProc, hwnd, msg, wParam, lParam);
}

// Adds items to a tree-view control. 
// Returns the handle to the newly added item. 
// hwndTV - handle to the tree-view control. 
// lpszItem - text of the item to add. 
// nLevel - level at which to add the item. 
//
// g_nClosed, and g_nDocument - global indexes of the images.

HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, int nLevel, int nID)
{ 
    TVITEM tvi; 
    TVINSERTSTRUCT tvins; 
    static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
    static HTREEITEM hPrevRootItem = NULL; 
    static HTREEITEM hPrevLev2Item = NULL; 

    tvi.mask = TVIF_TEXT | TVIF_PARAM; 

    // Set the text of the item. 
    tvi.pszText = lpszItem; 
    tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 

    // Save the heading ID in the item's application-defined 
    // data area. 
    tvi.lParam = (LPARAM)nID; 
    tvins.item = tvi; 
    tvins.hInsertAfter = hPrev; 

    // Set the parent item based on the specified level. 
    if (nLevel == 1) 
        tvins.hParent = TVI_ROOT; 
    else if (nLevel == 2) 
        tvins.hParent = hPrevRootItem; 
    else 
        tvins.hParent = hPrevLev2Item; 

    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
        0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

    if (hPrev == NULL)
        return NULL;

    // Save the handle to the item. 
    if (nLevel == 1) 
        hPrevRootItem = hPrev; 
    else if (nLevel == 2) 
        hPrevLev2Item = hPrev; 

    return hPrev; 
} 

// Extracts heading text and heading levels from a global 
// array and passes them to a function that adds them as
// parent and child items to a tree-view control. 
// Returns TRUE if successful, or FALSE otherwise. 
// hwndTV - handle to the tree-view control. 

struct {
	char tchHeading[100];
	int tchLevel;
	int tchID;
} g_rgDocHeadings[] = {
	{ "Comunica��o"            , 1, CONFTVI_ID_COMM          },
		{ "Rede"               , 2, CONFTVI_ID_COMM_NETWORK  },
		{ "Fuso Hor�rio"       , 2, CONFTVI_ID_COMM_FUSE     },
		{ "Serial"             , 2, CONFTVI_ID_COMM_SERIAL   },
	{ "Posicionamento"         , 1, CONFTVI_ID_POSIC         },
		{ "Encoder Incremental", 2, CONFTVI_ID_POSIC_ENCINC  },
		{ "Encoder Absoluto"   , 2, CONFTVI_ID_POSIC_ENCABS  },
	{ "ModBUS"                 , 1, CONFTVI_ID_MODBUS        },
		{ "Mestre"             , 2, CONFTVI_ID_MODBUS_MASTER },
		{ "Escravo"            , 2, CONFTVI_ID_MODBUS_SLAVE  },
};

BOOL InitTreeViewItems(HWND hwndTV)
{ 
    HTREEITEM hti;

    // g_rgDocHeadings is an application-defined global array of 
    // the following structures: 
    //     typedef struct 
    //       { 
    //         TCHAR tchHeading[MAX_HEADING_LEN]; 
    //         int tchLevel; 
    //     } Heading; 
    for (int i = 0; i < ARRAYSIZE(g_rgDocHeadings); i++) 
    { 
        // Add the item to the tree-view control. 
        hti = AddItemToTree(hwndTV, g_rgDocHeadings[i].tchHeading, 
            g_rgDocHeadings[i].tchLevel, g_rgDocHeadings[i].tchID);

        if (hti == NULL)
            return FALSE;
    } 

    return TRUE; 
}

static void MakeControls(void)
{
	HWND textLabel;

	ConfigTreeView = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_TREEVIEW, _("Ir Para:"),
		TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        7, 7, 200, 223, ConfDialog, NULL, Instance, NULL);
    NiceFont(ConfigTreeView);

	InitTreeViewItems(ConfigTreeView);

	// Group - Network Communication
	GroupCommNetwork = CreateWindowEx(0, WC_STATIC, "",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 200, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupCommNetwork);

	textLabel = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        308, 75, 96, 96, GroupCommNetwork, NULL, Instance, NULL);

	HBITMAP hBmp = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(IDB_NETWORK_CONFIG),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(textLabel,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Endere�o IP:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 0, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);

    ip = CreateWindowEx(0, WC_IPADDRESS, _("IP"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 0, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
//    NiceFont(ip);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Mascara:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 30, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);
	
    mask = CreateWindowEx(0, WC_IPADDRESS, _("MASK"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 30, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
//    NiceFont(mask);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Gateway:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 60, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);

    gw = CreateWindowEx(0, WC_IPADDRESS, _("GW"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 60, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
//    NiceFont(gw);

    textLabel = CreateWindowEx(0, WC_STATIC, _("DNS:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 90, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);

    dns = CreateWindowEx(0, WC_IPADDRESS, _("DNS"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 90, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
//    NiceFont(dns);

	// Group - Time Zone
    GroupCommTimeZone = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 85, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupCommTimeZone);

	textLabel = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        205, 5, 96, 96, GroupCommTimeZone, NULL, Instance, NULL);

	HBITMAP hBmp2 = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(IDB_TIME_CONFIG),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(textLabel,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp2);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Servidor de Tempo:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 0, 140, 21, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(textLabel);

	SNTPCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN,
        155, 0, 140, 145, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(SNTPCombobox);

	textLabel = CreateWindowEx(0, WC_STATIC, _("Fuso Hor�rio:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 30, 140, 21, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(textLabel);

	GMTCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 30, 140, 145, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(GMTCombobox);

	DailySaveCheckbox = CreateWindowEx(0, WC_BUTTON, _("Hor�rio de ver�o"),
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        155, 58, 140, 21, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(DailySaveCheckbox);

	// Group - Serial Communication
    GroupCommSerial = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 85, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupCommSerial);

	textLabel = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        310, 5, 96, 96, GroupCommSerial, NULL, Instance, NULL);

	HBITMAP hBmp3 = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(IDB_RS485_CONFIG),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(textLabel,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp3);

	textLabel = CreateWindowEx(0, WC_STATIC, _("RS485 Baud Rate(bps):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 1, 140, 21, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(textLabel);

	BaudRateCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 0, 140, 100, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(BaudRateCombobox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Data/Paridade/Stop:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 30, 140, 21, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(textLabel);

	ParityCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 29, 140, 100, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(ParityCombobox);

	// Group - Incremental Encoder
    GroupPosicEncInc = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 85, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupPosicEncInc);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Diametro Roda:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 0, 140, 21, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

    DiameterTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,   
        155, 0, 85, 21, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(DiameterTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("N�mero Pulsos:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 30, 140, 21, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

    PulsesTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 30, 85, 21, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(PulsesTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Fator Corre��o:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 60, 140, 21, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

    FactorTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 60, 85, 21, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(FactorTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Fator Multiplica��o:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 90, 140, 21, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

	X2Checkbox = CreateWindowEx(0, WC_BUTTON, _("x 2"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        155, 90, 45, 20, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(X2Checkbox);

	X4Checkbox = CreateWindowEx(0, WC_BUTTON, _("x 4"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        200, 90, 40, 20, GroupPosicEncInc, NULL, Instance, NULL);
    NiceFont(X4Checkbox);

	// Group - Absolute Encoder
    GroupPosicEncAbs = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 85, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupPosicEncAbs);

	textLabel = CreateWindowEx(0, WC_STATIC, _("Modo de Leitura:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        15, 25, 120, 21, GroupPosicEncAbs, NULL, Instance, NULL);
    NiceFont(textLabel);

	SSIModeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        145, 25, 140, 100, GroupPosicEncAbs, NULL, Instance, NULL);
    NiceFont(SSIModeCombobox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Resolu��o em bits:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        15, 55, 120, 21, GroupPosicEncAbs, NULL, Instance, NULL);
    NiceFont(textLabel);

    SSISizeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        145, 55, 85, 21, GroupPosicEncAbs, NULL, Instance, NULL);
    NiceFont(SSISizeTextbox);

	// Group - ModBUS Master
    GroupModBUSMaster = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 85, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupModBUSMaster);

	// Group - ModBUS Slave
    GroupModBUSSlave = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 85, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupModBUSSlave);

    textLabel = CreateWindowEx(0, WC_STATIC, _("ModBUS ID:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 1, 140, 21, GroupModBUSSlave, NULL, Instance, NULL);
    NiceFont(textLabel);

    ModBUSIDTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 0, 140, 21, GroupModBUSSlave, NULL, Instance, NULL);
    NiceFont(ModBUSIDTextbox);

	/*** Properties Tab - Start ***
    HWND textLabel20 = CreateWindowEx(0, WC_STATIC, _("Diametro Roda:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 90, 140, 21,  TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel20);

    DiameterTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,   
        205, 90, 85, 21, TabChild[1], NULL, Instance, NULL);
    NiceFont(DiameterTextbox);

    HWND textLabel21 = CreateWindowEx(0, WC_STATIC, _("N�mero Pulsos:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 120, 140, 21, TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel21);

    PulsesTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 120, 85, 21, TabChild[1], NULL, Instance, NULL);
    NiceFont(PulsesTextbox);

    HWND textLabel22 = CreateWindowEx(0, WC_STATIC, _("Fator Corre��o:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 150, 140, 21, TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel22);

    FactorTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 150, 85, 21, TabChild[1], NULL, Instance, NULL);
    NiceFont(FactorTextbox);
*/
	/*** Properties Tab - End ***/

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        365, 207, 69, 23, ConfDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        441, 207, 69, 23, ConfDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

//    PrevCrystalProc = SetWindowLongPtr(CrystalTextbox, GWLP_WNDPROC, 
//        (LONG_PTR)MyNumberProc);

    PrevBaudProc = SetWindowLongPtr(BaudTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNumberProc);

//	OnTabChange();
}

bool ShowConfDialog(bool NetworkSection)
{
	bool changed = false;
	unsigned int i, ipaddress, ipmask, ipgw, ipdns;
    // The window's height will be resized later, to fit the explanation text.
    ConfDialog = CreateWindowClient(0, "POPToolsDialog", _("PLC Configuration"),
        WS_OVERLAPPED | WS_SYSMENU,
        150, 160, 517, 237, MainWindow, NULL, Instance, NULL); //The cofig windows size

	PrevConfDialogProc = SetWindowLongPtr(ConfDialog, GWLP_WNDPROC, 
        (LONG_PTR)ConfDialogProc);

    MakeControls();
	if(NetworkSection) {
//		TabCtrl_SetCurSel(TabCtrl, 1);
//		OnTabChange();
	}

    char buf[16];

	SendMessage(ip  , IPM_SETADDRESS, 0, MAKEIPADDRESS(Prog.settings.ip  [0], Prog.settings.ip  [1], Prog.settings.ip  [2], Prog.settings.ip  [3]));
	SendMessage(mask, IPM_SETADDRESS, 0, MAKEIPADDRESS(Prog.settings.mask[0], Prog.settings.mask[1], Prog.settings.mask[2], Prog.settings.mask[3]));
	SendMessage(gw  , IPM_SETADDRESS, 0, MAKEIPADDRESS(Prog.settings.gw  [0], Prog.settings.gw  [1], Prog.settings.gw  [2], Prog.settings.gw  [3]));
	SendMessage(dns , IPM_SETADDRESS, 0, MAKEIPADDRESS(Prog.settings.dns [0], Prog.settings.dns [1], Prog.settings.dns [2], Prog.settings.dns [3]));

	/*for (i = 0; i < sizeof(ComboboxPLCItens) / sizeof(ComboboxPLCItens[0]); i++)
		SendMessage(PLCCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxPLCItens[i]));*/

	/*SendMessage(PLCCombobox, CB_SETCURSEL, 0, 0);
	SendMessage(PLCCombobox, CB_SETDROPPEDWIDTH, 100, 0);*/

	for (i = 0; i < sizeof(SerialConfig) / sizeof(SerialConfig[0]); i++)
		SendMessage(ParityCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)SerialConfig[i].ConfigName));

	SendMessage(ParityCombobox, CB_SETCURSEL, Prog.settings.UART, 0);
	SendMessage(ParityCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	sprintf(buf, "%d", Prog.settings.baudRate);
	for (i = 0; i < sizeof(ComboboxBaudRateItens) / sizeof(ComboboxBaudRateItens[0]); i++)
	{
		SendMessage(BaudRateCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBaudRateItens[i]));
		if (_stricmp(ComboboxBaudRateItens[i], buf) == 0)
			SendMessage(BaudRateCombobox, CB_SETCURSEL, i, 0);
	}

	sprintf(buf, "%d", Prog.settings.ModBUSID);
	SendMessage(ModBUSIDTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	for (i = 0; i < sizeof(ComboboxSNTPItens) / sizeof(ComboboxSNTPItens[0]); i++)
		SendMessage(SNTPCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ComboboxSNTPItens[i]));
	SendMessage(SNTPCombobox, WM_SETTEXT, 0, (LPARAM)Prog.settings.sntp);

	for (i = 0; i < sizeof(ComboboxGMTItens) / sizeof(ComboboxGMTItens[0]); i++)
		SendMessage(GMTCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ComboboxGMTItens[i]));
	SendMessage(GMTCombobox, CB_SETCURSEL, Prog.settings.gmt, 0);
	SendMessage(GMTCombobox, CB_SETDROPPEDWIDTH, 300, 0);

	for (i = 0; i < sizeof(EncAbsConfig) / sizeof(EncAbsConfig[0]); i++)
		SendMessage(SSIModeCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)EncAbsConfig[i]));
	SendMessage(SSIModeCombobox, CB_SETCURSEL, Prog.settings.ssi_mode, 0);

	if (Prog.settings.dailysave)
		SendMessage(DailySaveCheckbox, BM_SETCHECK, BST_CHECKED, 0);

	sprintf(buf, "%d", Prog.settings.diameter);
	SendMessage(DiameterTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%d", Prog.settings.pulses);
	SendMessage(PulsesTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%05f", Prog.settings.factor);
	SendMessage(FactorTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	if (Prog.settings.x4)
		SendMessage(X4Checkbox, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(X2Checkbox, BM_SETCHECK, BST_CHECKED, 0);

	//SendMessage(BaudRateCombobox, CB_SETCURSEL, 0, 0);
	SendMessage(BaudRateCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	//sprintf(buf, "%.6f", Prog.settings.mcuClock / 1e6);
    //SendMessage(CrystalTextbox, WM_SETTEXT, 0, (LPARAM)buf);

    //sprintf(buf, "%d", Prog.settings.baudRate);
    //SendMessage(BaudTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%d", Prog.settings.ssi_size);
	SendMessage(SSISizeTextbox, WM_SETTEXT, 0, (LPARAM)buf);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(ConfDialog, TRUE);
    SetFocus(OkButton);

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

        if(IsDialogMessage(ConfDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
        char buf[16];
/*        SendMessage(CycleTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
        Prog.settings.cycleTime = (int)(1000*atof(buf) + 0.5);
        if(Prog.settings.cycleTime == 0) {
            Error(_("Zero cycle time not valid; resetting to 10 ms."));
            Prog.settings.cycleTime = 10000;
        }*/

        /*SendMessage(CrystalTextbox, WM_GETTEXT, (WPARAM)sizeof(buf),
            (LPARAM)(buf));*/
        //Prog.settings.mcuClock = (int)(1e6*atof(buf) + 0.5);

		// Recebe dados de rede configurados
		SendMessage(ip  , IPM_GETADDRESS, 0, (LPARAM)&ipaddress);
		SendMessage(mask, IPM_GETADDRESS, 0, (LPARAM)&ipmask   );
		SendMessage(gw  , IPM_GETADDRESS, 0, (LPARAM)&ipgw     );
		SendMessage(dns , IPM_GETADDRESS, 0, (LPARAM)&ipdns    );

		// Carrega IP
		Prog.settings.ip[0] = FIRST_IPADDRESS (ipaddress);
		Prog.settings.ip[1] = SECOND_IPADDRESS(ipaddress);
		Prog.settings.ip[2] = THIRD_IPADDRESS (ipaddress);
		Prog.settings.ip[3] = FOURTH_IPADDRESS(ipaddress);

		// Carrega Mascara
		Prog.settings.mask[0] = FIRST_IPADDRESS (ipmask);
		Prog.settings.mask[1] = SECOND_IPADDRESS(ipmask);
		Prog.settings.mask[2] = THIRD_IPADDRESS (ipmask);
		Prog.settings.mask[3] = FOURTH_IPADDRESS(ipmask);

		// Carrega Gateway
		Prog.settings.gw[0] = FIRST_IPADDRESS (ipgw);
		Prog.settings.gw[1] = SECOND_IPADDRESS(ipgw);
		Prog.settings.gw[2] = THIRD_IPADDRESS (ipgw);
		Prog.settings.gw[3] = FOURTH_IPADDRESS(ipgw);

		// Carrega DNS
		Prog.settings.dns[0] = FIRST_IPADDRESS (ipdns);
		Prog.settings.dns[1] = SECOND_IPADDRESS(ipdns);
		Prog.settings.dns[2] = THIRD_IPADDRESS (ipdns);
		Prog.settings.dns[3] = FOURTH_IPADDRESS(ipdns);

        SendMessage(BaudRateCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
            (LPARAM)(buf));
        Prog.settings.baudRate = atoi(buf);

		Prog.settings.UART = SendMessage(ParityCombobox, CB_GETCURSEL, 0, 0);

        SendMessage(ModBUSIDTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		Prog.settings.ModBUSID = atoi(buf);
		if(Prog.settings.ModBUSID < 0)
			Prog.settings.ModBUSID = 0;

		SendMessage(SNTPCombobox, WM_GETTEXT, (WPARAM)sizeof(Prog.settings.sntp),
            (LPARAM)(Prog.settings.sntp));

		Prog.settings.gmt = SendMessage(GMTCombobox, CB_GETCURSEL, 0, 0);
		Prog.settings.dailysave = SendMessage(DailySaveCheckbox, BM_GETSTATE, 0, 0) & BST_CHECKED;

		Prog.settings.x4 = (char)SendMessage(X4Checkbox, BM_GETCHECK, 0, 0);

		SendMessage(SSISizeTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		Prog.settings.ssi_size = atoi(buf);
		if(Prog.settings.ssi_size < 1) {
			Prog.settings.ssi_size = 1;
		} else if(Prog.settings.ssi_size > 32) {
			Prog.settings.ssi_size = 32;
		}

		Prog.settings.ssi_mode = SendMessage(SSIModeCombobox, CB_GETCURSEL, 0, 0);

		changed = true;
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(ConfDialog);
    return changed;
}
