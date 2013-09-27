#include "poptools.h"

static HWND ConfDialog;
static HWND ConfigTreeView;
static HWND GroupInfoProject;
static HWND GroupInfoDetails;
static HWND GroupCommNetwork;
static HWND GroupCommTimeZone;
static HWND GroupCommSerial;
static HWND GroupInterfaceDA;
static HWND GroupInterfaceEncInc;
static HWND GroupInterfaceEncAbs;
static HWND GroupModBUSSlave;
static HWND GroupModBUSMaster;

static HWND InfoProjectTextbox;
static HWND InfoDeveloperTextbox;
static HWND InfoDescriptionTextbox;
static HWND InfoFWVersionLabel;
static HWND InfoBuildNumberLabel;
static HWND InfoCompileDateLabel;
static HWND InfoProgramDateLabel;
static HWND ModBUSIDTextbox;
static HWND BaudRateCombobox;
static HWND ParityCombobox;
static HWND SNTPEnableCheckbox;
static HWND SNTPCombobox;
static HWND GMTCombobox;
static HWND DailySaveCheckbox;
static HWND AbortModeCombobox;
static HWND MBelem;
static HWND MBok;
static HWND MBdelete;
static HWND MBip;
static HWND MBname;
static HWND MBid;
static HWND MBiface;
static HWND ip;
static HWND mask;
static HWND gw;
static HWND dns;

static HWND IncConvModeCombobox;
static HWND PerimeterTextbox;
static HWND PulsesTextbox;
static HWND FactorTextbox;
static HWND X4Checkbox;
static HWND X2Checkbox;

static HWND SSISizeTextbox;
static HWND SSISizeRevTextbox;
static HWND SSIModeCombobox;
static HWND SSIConvModeCombobox;
static HWND SSIPerimeterTextbox;
static HWND SSIFactorTextbox;

static LONG_PTR PrevConfDialogProc;
static LONG_PTR PrevBaudProc;

//const LPCTSTR ComboboxPLCItens[] = { _("POP7"), _("POP9") };

const LPCTSTR ComboboxGMTItens[] = { _("(GMT-12) Linha de Data Internacional Oeste"), _("(GMT-11) Ilhas Midway,Samoa"),
								_("(GMT-10) Hawaí"), _("(GMT-09) Alasca"),
								_("(GMT-08) Hora do Pacífico"), _("(GMT-07) Hora das Montanhas (EUA e Canadá)"),
								_("(GMT-06) América Central, Hora Central EUA/Canadá"), _("(GMT-05) Lima, Bogotá"),
								_("(GMT-04) Rio Branco, Manaus, Caracas, La Paz"), _("(GMT-03) Brasilia, Buenos Aires"),
								_("(GMT-02) Atlântico Central"), _("(GMT-01) Açores, Cabo Verde"),
								_("(GMT 00) Hora de Greenwich: Londres, Dublin, Lisboa"), _("(GMT+01) Berlim, Estocolmo, Roma, Bruxelas"),
								_("(GMT+02) Atenas, Helsinque, Leste Europeu, Jerusalém"), _("(GMT+03) Bagdá, Kuwait, Nairóbi, Moscou,Riad"),
								_("(GMT+04) Abu Dhabi, Mascate, Hora Padrão do Cáucaso"), _("(GMT+05) Islamabad, Karachi, Ekaterinburgo"),
								_("(GMT+06) Almaty, Dacca"), _("(GMT+07) Bangcoc, Jacarta, Hanói"),
								_("(GMT+08) Pequim, Hong Kong, Taiwan, Cingapura"), _("(GMT+09) Tóquio, Osaka, Sapporo, Seul, Yakutsk"),
								_("(GMT+10) Brisbane, Camberra, Melbourne, Sydney"), _("(GMT+11) Magadã, Ilhas Salomão, Nova Caledônia"),
								_("(GMT+12) Fiji, Kamchatka, Auckland"), _("(GMT+13) Nuku'alofa") };

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

char *EncAbsConfig[] = { _("Leitura Gray"), _("Leitura Binário") };

char *SerialParityString[] = { _("Sem Paridade"), _("Paridade Ímpar"), _("Paridade Par") };

char *ModBUSInterfaces[] = { _("RS-485"), _("Ethernet") };

char *EncoderConvModes[] = { _("Sem conversão"), _("Metros"), _("Milímetros"), _("Décimos de milímetro") };

#define CONFTVI_ID_INFO              0
#define CONFTVI_ID_INFO_PROJECT      1
#define CONFTVI_ID_INFO_DETAILS      2
#define CONFTVI_ID_COMM              3
#define CONFTVI_ID_COMM_NETWORK      4
#define CONFTVI_ID_COMM_FUSE         5
#define CONFTVI_ID_COMM_SERIAL       6
#define CONFTVI_ID_INTERFACE         7
#define CONFTVI_ID_INTERFACE_DA      8
#define CONFTVI_ID_INTERFACE_ENCINC  9
#define CONFTVI_ID_INTERFACE_ENCABS 10
#define CONFTVI_ID_MODBUS           11
#define CONFTVI_ID_MODBUS_MASTER    12
#define CONFTVI_ID_MODBUS_SLAVE     13

void OnSelChanged(int ID)
{
	int i;
	HWND group, groups[] = { GroupInfoProject, GroupInfoDetails, GroupCommNetwork, GroupCommTimeZone, GroupCommSerial,
							GroupInterfaceDA, GroupInterfaceEncInc, GroupInterfaceEncAbs, GroupModBUSMaster, GroupModBUSSlave };

	switch(ID) {
		case CONFTVI_ID_INFO:
		case CONFTVI_ID_INFO_PROJECT:
			group = GroupInfoProject;
			break;

		case CONFTVI_ID_INFO_DETAILS:
			group = GroupInfoDetails;
			break;

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

		case CONFTVI_ID_INTERFACE:
		case CONFTVI_ID_INTERFACE_DA:
			group = GroupInterfaceDA;
			break;

		case CONFTVI_ID_INTERFACE_ENCINC:
			group = GroupInterfaceEncInc;
			break;

		case CONFTVI_ID_INTERFACE_ENCABS:
			group = GroupInterfaceEncAbs;
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

void LoadMBMasterControls(int elem)
{
	if(elem > 0) {
		char buf[10];
		LadderMbNode node = ladder->mbGetNodeByIndex(elem-1);

		if(node.name.size() > 0) {
			sprintf(buf, "%d", node.id);

			EnableWindow(MBip  , (node.iface == eMbTypeNode_Ethernet) ? TRUE : FALSE);
			SendMessage(MBiface, CB_SETCURSEL  , node.iface, 0);
			SendMessage(MBname , WM_SETTEXT    , 0, (LPARAM)node.name.c_str());
			SendMessage(MBip   , IPM_SETADDRESS, 0, node.ip);
			SendMessage(MBid   , WM_SETTEXT    , 0, (LPARAM)buf);
		}

		Button_SetText(MBok, _("Alterar"));
		EnableWindow(MBdelete, TRUE);
	} else {
		LadderSettingsNetwork network = ladder->getSettingsNetwork();
		EnableWindow(MBip  , TRUE);
		SendMessage(MBiface, CB_SETCURSEL  , 1, 0); // Ethernet
		SendMessage(MBname , WM_SETTEXT    , 0, (LPARAM)"");
		SendMessage(MBip   , IPM_SETADDRESS, 0, network.ip);
		SendMessage(MBid   , WM_SETTEXT    , 0, (LPARAM)"1");

		Button_SetText(MBok, _("Adicionar"));
		EnableWindow(MBdelete, FALSE);
	}
}

//-----------------------------------------------------------------------------
// Window proc for the ModBUS Master Grouper. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK ConfDialogProc_ModBusMasterGrouper(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == MBok && wParam == BN_CLICKED) {
				char buf[MAX_NAME_LEN];
				PCWSTR msg;
				int i = ComboBox_GetCurSel(MBelem), pos = 1;

				// Check if name already in the list
				SendMessage(MBname, WM_GETTEXT, (WPARAM)17, (LPARAM)buf);
				LadderMbNode new_node = ladder->mbGetNodeByNodeID(ladder->mbGetNodeIDByName(buf));

				bool nameOK = false;
				if(i == ladder->mbGetIndexByNodeID(ladder->mbGetNodeIDByName(buf)) + 1) {
					nameOK = true;
				}

				if(new_node.name.size() > 0 && !nameOK) {
					ShowTaskDialog(L"Já existe um elemento com esse nome!", NULL, TD_ERROR_ICON, TDCBF_OK_BUTTON);
				} else { // Name not found,  we can insert / update element.
					if(!i) { // New element
						i = ladder->mbCreateNode(buf); // Create the new node

						if(i < 0) {
							msg = L"Erro ao criar elemento!";
						} else {
							msg = L"Elemento adicionado com sucesso!";

							new_node = ladder->mbGetNodeByIndex(i);
						}
					} else { // Updating existent element
						msg = L"Elemento alterado com sucesso!";

						new_node = ladder->mbGetNodeByIndex(i-1);
					}

					if(new_node.name.size() > 0) {
						int NodeID = ladder->mbGetNodeIDByName(new_node.name);
						new_node.name = buf;

						SendMessage(MBid, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
						new_node.id = min(max(0, atoi(buf)), 127);

						switch(SendMessage(MBiface, CB_GETCURSEL, 0, 0)) {
						default:
						case 0: new_node.iface = eMbTypeNode_RS485   ; break;
						case 1: new_node.iface = eMbTypeNode_Ethernet; break;
						}

						SendMessage(MBip, IPM_GETADDRESS, 0, (LPARAM)&new_node.ip);

						ladder->mbUpdateNode(NodeID, new_node);

						PopulateModBUSMasterCombobox(MBelem, true);
						LoadMBMasterControls(0);
					}

					ShowTaskDialog(msg, NULL, (new_node.name.size() == 0) ? TD_INFORMATION_ICON : TD_ERROR_ICON, TDCBF_OK_BUTTON);
				}
            } else if(h == MBdelete && wParam == BN_CLICKED) {
				int i = ComboBox_GetCurSel(MBelem) - 1;
				int NodeID = ladder->mbGetNodeIDByIndex(i);

				if(ladder->mbGetNodeCount(NodeID) > 0) {
					ShowTaskDialog(L"Este elemento está em uso!", L"Primeiro remova sua referência de todas as instruções que o utilizam.",
						TD_ERROR_ICON, TDCBF_OK_BUTTON);
				} else if(ShowTaskDialog(L"Tem certeza que deseja excluir o item selecionado?", NULL,
					TD_WARNING_ICON, TDCBF_YES_BUTTON | TDCBF_NO_BUTTON) == IDYES) {
						ladder->mbDeleteNode(NodeID);
						PopulateModBUSMasterCombobox(MBelem, true);
						LoadMBMasterControls(0);
				}
            } else if(h == MBelem && HIWORD(wParam) == CBN_SELCHANGE) {
				LoadMBMasterControls(ComboBox_GetCurSel(MBelem));
			} else if(h == MBiface && HIWORD(wParam) == CBN_SELCHANGE) {
				if(ComboBox_GetCurSel(MBiface) == eMbTypeNode_RS485) {
					SendMessage (MBip, IPM_SETADDRESS, 0, 0);
					EnableWindow(MBip, FALSE);
				} else {
					LadderSettingsNetwork network = ladder->getSettingsNetwork();
					SendMessage (MBip, IPM_SETADDRESS, 0, network.ip);
					EnableWindow(MBip, TRUE);
				}
			}
            break;
			}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
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
	{ "Informações"            , 1, CONFTVI_ID_INFO             },
		{ "Projeto"            , 2, CONFTVI_ID_INFO_PROJECT     },
		{ "Detalhes"           , 2, CONFTVI_ID_INFO_DETAILS     },
	{ "Comunicação"            , 1, CONFTVI_ID_COMM             },
		{ "Rede"               , 2, CONFTVI_ID_COMM_NETWORK     },
		{ "Fuso Horário"       , 2, CONFTVI_ID_COMM_FUSE        },
		{ "Serial"             , 2, CONFTVI_ID_COMM_SERIAL      },
	{ "Interfaces"             , 1, CONFTVI_ID_INTERFACE        },
		{ "Saída Analógica"    , 2, CONFTVI_ID_INTERFACE_DA     },
		{ "Encoder Incremental", 2, CONFTVI_ID_INTERFACE_ENCINC },
		{ "Encoder Absoluto"   , 2, CONFTVI_ID_INTERFACE_ENCABS },
	{ "ModBUS"                 , 1, CONFTVI_ID_MODBUS           },
		{ "Mestre"             , 2, CONFTVI_ID_MODBUS_MASTER    },
		{ "Escravo"            , 2, CONFTVI_ID_MODBUS_SLAVE     },
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

	// Group - Project Details
	GroupInfoProject = CreateWindowEx(0, WC_STATIC, "",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupInfoProject);

	textLabel = CreateWindowEx(0, WC_STATIC, _("Preencha abaixo os dados referentes ao projeto para sua referência."),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        5, 5, 285, 54, GroupInfoProject, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Nome do Projeto:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 67, 140, 21, GroupInfoProject, NULL, Instance, NULL);
    NiceFont(textLabel);

    InfoProjectTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 67, 140, 21, GroupInfoProject, NULL, Instance, NULL);
    NiceFont(InfoProjectTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Autor do Projeto:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 97, 140, 21, GroupInfoProject, NULL, Instance, NULL);
    NiceFont(textLabel);

    InfoDeveloperTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 97, 140, 21, GroupInfoProject, NULL, Instance, NULL);
    NiceFont(InfoDeveloperTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Observações:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 127, 140, 21, GroupInfoProject, NULL, Instance, NULL);
    NiceFont(textLabel);

    InfoDescriptionTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN,
        155, 127, 140, 70, GroupInfoProject, NULL, Instance, NULL);
    NiceFont(InfoDescriptionTextbox);

	// Group - Project Stats
	GroupInfoDetails = CreateWindowEx(0, WC_STATIC, "",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupInfoDetails);

	textLabel = CreateWindowEx(0, WC_STATIC, _("Abaixo são exibidas informações sobre o projeto."),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        5, 5, 285, 54, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Versão do Firmware:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 67, 140, 21, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(textLabel);

    InfoFWVersionLabel = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 67, 140, 21, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(InfoFWVersionLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Número da Compilação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 97, 140, 21, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(textLabel);

    InfoBuildNumberLabel = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 97, 140, 21, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(InfoBuildNumberLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Data de Compilação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 127, 140, 21, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(textLabel);

    InfoCompileDateLabel = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 127, 140, 21, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(InfoCompileDateLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Data de Gravação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 157, 140, 21, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(textLabel);

    InfoProgramDateLabel = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 157, 140, 21, GroupInfoDetails, NULL, Instance, NULL);
    NiceFont(InfoProgramDateLabel);

	// Group - Network Communication
	GroupCommNetwork = CreateWindowEx(0, WC_STATIC, "",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupCommNetwork);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Ajuste a configuração da rede conforme a sua necessidade"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        5, 5, 195, 54, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);

	textLabel = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        205, 0, 64, 64, GroupCommNetwork, NULL, Instance, NULL);

	HBITMAP hBmp = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(IDB_NETWORK_CONFIG),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(textLabel,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Endereço IP:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 67, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);

    ip = CreateWindowEx(0, WC_IPADDRESS, _("IP"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 67, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
//    NiceFont(ip);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Mascara:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 97, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);
	
    mask = CreateWindowEx(0, WC_IPADDRESS, _("MASK"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 97, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
//    NiceFont(mask);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Gateway:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 127, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);

    gw = CreateWindowEx(0, WC_IPADDRESS, _("GW"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 127, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
//    NiceFont(gw);

    textLabel = CreateWindowEx(0, WC_STATIC, _("DNS:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 157, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
    NiceFont(textLabel);

    dns = CreateWindowEx(0, WC_IPADDRESS, _("DNS"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 157, 140, 21, GroupCommNetwork, NULL, Instance, NULL);
//    NiceFont(dns);

	// Group - Time Zone
    GroupCommTimeZone = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupCommTimeZone);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Ajuste o fuso horário e indique se o horário de verão está ativado"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        5, 5, 195, 62, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(textLabel);

	textLabel = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        205, 0, 72, 72, GroupCommTimeZone, NULL, Instance, NULL);

	HBITMAP hBmp2 = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(IDB_TIME_CONFIG),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(textLabel,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp2);

	SNTPEnableCheckbox = CreateWindowEx(0, WC_BUTTON, _("Ativar sincronização"),
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        155, 75, 140, 21, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(SNTPEnableCheckbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Servidor de Tempo:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 105, 140, 21, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(textLabel);

	SNTPCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN,
        155, 105, 140, 145, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(SNTPCombobox);

	textLabel = CreateWindowEx(0, WC_STATIC, _("Fuso Horário:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 135, 140, 21, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(textLabel);

	GMTCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 135, 140, 145, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(GMTCombobox);

	DailySaveCheckbox = CreateWindowEx(0, WC_BUTTON, _("Horário de verão"),
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        155, 165, 140, 21, GroupCommTimeZone, NULL, Instance, NULL);
    NiceFont(DailySaveCheckbox);

	// Group - Serial Communication
    GroupCommSerial = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupCommSerial);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Ajuste aqui a configuração da interface RS-485 da POP-7"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        5, 5, 195, 54, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(textLabel);

	textLabel = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        205, 0, 64, 64, GroupCommSerial, NULL, Instance, NULL);

	HBITMAP hBmp3 = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(IDB_RS485_CONFIG),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(textLabel,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp3);

	textLabel = CreateWindowEx(0, WC_STATIC, _("RS485 Baud Rate(bps):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 67, 140, 21, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(textLabel);

	BaudRateCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 67, 140, 100, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(BaudRateCombobox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Data/Paridade/Stop:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 97, 140, 21, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(textLabel);

	ParityCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 97, 140, 100, GroupCommSerial, NULL, Instance, NULL);
    NiceFont(ParityCombobox);

	// Group - Analog Output
    GroupInterfaceDA = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupInterfaceDA);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Este Modo de Abandono será utilizado em todas as rampas de D/A selecionadas para o modo Padrão."),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        5, 5, 285, 74, GroupInterfaceDA, NULL, Instance, NULL);
    NiceFont(textLabel);

    HWND Label = CreateWindowEx(0, WC_STATIC, _("Modo de Abandono:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 84, 140, 21, GroupInterfaceDA, NULL, Instance, NULL);
    NiceFont(Label);

	AbortModeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 82, 130, 100, GroupInterfaceDA, NULL, Instance, NULL);
    NiceFont(AbortModeCombobox);

	// Group - Incremental Encoder
    GroupInterfaceEncInc = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupInterfaceEncInc);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Modo de Conversão:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 0, 140, 21, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

	IncConvModeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 0, 130, 100, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(IncConvModeCombobox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Perímetro Roda:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 30, 140, 21, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

    PerimeterTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,   
        155, 30, 85, 21, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(PerimeterTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Número Pulsos:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 60, 140, 21, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

    PulsesTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 60, 85, 21, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(PulsesTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Fator Correção:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 90, 140, 21, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

    FactorTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 90, 85, 21, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(FactorTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Fator Multiplicação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 120, 140, 21, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(textLabel);

	X2Checkbox = CreateWindowEx(0, WC_BUTTON, _("x 2"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        155, 120, 45, 20, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(X2Checkbox);

	X4Checkbox = CreateWindowEx(0, WC_BUTTON, _("x 4"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        200, 120, 40, 20, GroupInterfaceEncInc, NULL, Instance, NULL);
    NiceFont(X4Checkbox);

	// Group - Absolute Encoder
    GroupInterfaceEncAbs = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupInterfaceEncAbs);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Modo de Conversão:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 0, 140, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(textLabel);

	SSIConvModeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 0, 130, 100, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(SSIConvModeCombobox);

	textLabel = CreateWindowEx(0, WC_STATIC, _("Modo de Leitura:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 30, 140, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(textLabel);

	SSIModeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 30, 130, 100, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(SSIModeCombobox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Resolução total em bits:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 60, 140, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(textLabel);

    SSISizeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 60, 85, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(SSISizeTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Sendo bits por volta:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 90, 140, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(textLabel);

    SSISizeRevTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 90, 85, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(SSISizeRevTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Perímetro Roda:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 120, 140, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(textLabel);

   SSIPerimeterTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,   
        155, 120, 85, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(SSIPerimeterTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Fator Correção:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 150, 140, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(textLabel);

    SSIFactorTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 150, 85, 21, GroupInterfaceEncAbs, NULL, Instance, NULL);
    NiceFont(SSIFactorTextbox);

	// Group - ModBUS Master
    GroupModBUSMaster = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupModBUSMaster);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Elemento:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 0, 80, 21, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(textLabel);

	MBelem = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        95, 0, 200, 100, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(MBelem);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Nome:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        25, 45, 110, 21, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(textLabel);

    MBname = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        145, 45, 140, 21, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(MBname);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Interface:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        25, 75, 110, 21, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(textLabel);

	MBiface = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        145, 75, 140, 100, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(MBiface);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Endereço IP:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        25, 105, 110, 21, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(textLabel);

    MBip = CreateWindowEx(0, WC_IPADDRESS, _("IP"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        145, 105, 140, 21, GroupModBUSMaster, NULL, Instance, NULL);
//    NiceFont(MBip);

    textLabel = CreateWindowEx(0, WC_STATIC, _("ID do ModBUS:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        25, 135, 110, 21, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(textLabel);

    MBid = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        145, 135, 140, 21, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(MBid);

    MBok = CreateWindowEx(0, WC_BUTTON, _("Adicionar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        137, 165, 69, 23, GroupModBUSMaster, NULL, Instance, NULL); 
    NiceFont(MBok);

    MBdelete = CreateWindowEx(0, WC_BUTTON, _("Excluir"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        216, 165, 69, 23, GroupModBUSMaster, NULL, Instance, NULL); 
    NiceFont(MBdelete);

    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Dados do Elemento Selecionado"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        10, 25, 285, 170, GroupModBUSMaster, NULL, Instance, NULL);
    NiceFont(grouper);
	SetWindowLongPtr(GroupModBUSMaster, GWLP_WNDPROC, (LONG_PTR)ConfDialogProc_ModBusMasterGrouper);

	// Group - ModBUS Slave
    GroupModBUSSlave = CreateWindowEx(0, WC_STATIC, "",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        215, 7, 295, 198, ConfDialog, NULL, Instance, NULL);
    NiceFont(GroupModBUSSlave);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Preencha aqui o ID utilizado pela interface ModBUS quando atuando em modo escravo.\r\nPreencha com valor entre 0 e 127"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_CENTER,
        5, 5, 285, 74, GroupModBUSSlave, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("ModBUS ID:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 85, 140, 21, GroupModBUSSlave, NULL, Instance, NULL);
    NiceFont(textLabel);

    ModBUSIDTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 84, 140, 21, GroupModBUSSlave, NULL, Instance, NULL);
    NiceFont(ModBUSIDTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,// | BS_DEFPUSHBUTTON,
        365, 207, 69, 23, ConfDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        441, 207, 69, 23, ConfDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

bool ShowConfDialog(eConfSection confSection)
{
	bool changed = false;
	unsigned int i;
    // The window's height will be resized later, to fit the explanation text.
    ConfDialog = CreateWindowClient(0, "POPToolsDialog", _("PLC Configuration"),
        WS_OVERLAPPED | WS_SYSMENU,
        150, 160, 517, 237, MainWindow, NULL, Instance, NULL); //The cofig windows size

	PrevConfDialogProc = SetWindowLongPtr(ConfDialog, GWLP_WNDPROC, 
        (LONG_PTR)ConfDialogProc);

    MakeControls();
	switch(confSection) {
	case eConfSection_Network:
		OnSelChanged(CONFTVI_ID_COMM);
		break;

	case eConfSection_EncInc:
		OnSelChanged(CONFTVI_ID_INTERFACE_ENCINC);
		break;

	case eConfSection_EncSSI:
		OnSelChanged(CONFTVI_ID_INTERFACE_ENCABS);
		break;

	default:
	case eConfSection_Default:
		OnSelChanged(0); // Initial Group
	}

    char buf[1024];
	time_t rawtime;
	struct tm * timeinfo;

	LadderSettingsGeneral            settingsGeneral = ladder->getSettingsGeneral           ();
	LadderSettingsUART               settingsUart    = ladder->getSettingsUART              ();
	LadderSettingsNetwork            settingsNetwork = ladder->getSettingsNetwork           ();
	LadderSettingsSNTP               settingsSntp    = ladder->getSettingsSNTP              ();
	LadderSettingsEncoderIncremental settingsEncInc  = ladder->getSettingsEncoderIncremental();
	LadderSettingsEncoderSSI         settingsEncSSI  = ladder->getSettingsEncoderSSI        ();
	LadderSettingsDAC                settingsDac     = ladder->getSettingsDAC               ();
	LadderSettingsModbusSlave        settingsMbSlave = ladder->getSettingsModbusSlave       ();
	LadderSettingsInformation        settingsInfo    = ladder->getSettingsInformation       ();

	SendMessage(InfoProjectTextbox    , WM_SETTEXT, 0, (LPARAM)settingsInfo.Name       .c_str());
	SendMessage(InfoDeveloperTextbox  , WM_SETTEXT, 0, (LPARAM)settingsInfo.Developer  .c_str());
	SendMessage(InfoDescriptionTextbox, WM_SETTEXT, 0, (LPARAM)settingsInfo.Description.c_str());
	SendMessage(InfoFWVersionLabel    , WM_SETTEXT, 0, (LPARAM)settingsInfo.FWVersion  .c_str());

	sprintf(buf, "%d", settingsInfo.BuildNumber);
	SendMessage(InfoBuildNumberLabel, WM_SETTEXT, 0, (LPARAM)buf);

	if(settingsInfo.CompileDate) {
		rawtime = settingsInfo.CompileDate;
		timeinfo = localtime(&rawtime);
		strftime(buf, sizeof(buf), "%H:%M:%S %d/%m/%Y", timeinfo);
	} else {
		strcpy(buf, _("Indefinido"));
	}
	SendMessage(InfoCompileDateLabel, WM_SETTEXT, 0, (LPARAM)buf);

	if(settingsInfo.ProgramDate) {
		rawtime = settingsInfo.ProgramDate;
		timeinfo = localtime(&rawtime);
		strftime(buf, sizeof(buf), "%H:%M:%S %d/%m/%Y", timeinfo);
	} else {
		strcpy(buf, _("Indefinido"));
	}
	SendMessage(InfoProgramDateLabel, WM_SETTEXT, 0, (LPARAM)buf);

	SendMessage(ip  , IPM_SETADDRESS, 0, settingsNetwork.ip  );
	SendMessage(mask, IPM_SETADDRESS, 0, settingsNetwork.mask);
	SendMessage(gw  , IPM_SETADDRESS, 0, settingsNetwork.gw  );
	SendMessage(dns , IPM_SETADDRESS, 0, settingsNetwork.dns );

	PopulateAbortModeCombobox(AbortModeCombobox, false);
	SendMessage(AbortModeCombobox, CB_SETCURSEL, settingsDac.ramp_abort_mode-1, 0);

	for (i = 0; i < sizeof(ModBUSInterfaces) / sizeof(ModBUSInterfaces[0]); i++)
		SendMessage(MBiface, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ModBUSInterfaces[i]));

	for (i = 0; i < sizeof(EncoderConvModes) / sizeof(EncoderConvModes[0]); i++) {
		SendMessage(IncConvModeCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)EncoderConvModes[i]));
		SendMessage(SSIConvModeCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)EncoderConvModes[i]));
	}
	SendMessage(IncConvModeCombobox, CB_SETCURSEL, settingsEncInc.conv_mode, 0);
	SendMessage(SSIConvModeCombobox, CB_SETCURSEL, settingsEncSSI.conv_mode, 0);

	PopulateModBUSMasterCombobox(MBelem, true);
	LoadMBMasterControls(0);

	for (i = 0; i < sizeof(SerialConfig) / sizeof(SerialConfig[0]); i++)
		SendMessage(ParityCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)SerialConfig[i].ConfigName));

	SendMessage(ParityCombobox, CB_SETCURSEL, settingsUart.UART, 0);
	SendMessage(ParityCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	sprintf(buf, "%d", settingsUart.baudRate);
	for (i = 0; i < sizeof(ComboboxBaudRateItens) / sizeof(ComboboxBaudRateItens[0]); i++)
	{
		SendMessage(BaudRateCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBaudRateItens[i]));
		if (_stricmp(ComboboxBaudRateItens[i], buf) == 0)
			SendMessage(BaudRateCombobox, CB_SETCURSEL, i, 0);
	}

	sprintf(buf, "%d",settingsMbSlave.ModBUSID);
	SendMessage(ModBUSIDTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	for (i = 0; i < sizeof(ComboboxSNTPItens) / sizeof(ComboboxSNTPItens[0]); i++)
		SendMessage(SNTPCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ComboboxSNTPItens[i]));
	SendMessage(SNTPCombobox, WM_SETTEXT, 0, (LPARAM)settingsSntp.sntp_server.c_str());

	for (i = 0; i < sizeof(ComboboxGMTItens) / sizeof(ComboboxGMTItens[0]); i++)
		SendMessage(GMTCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ComboboxGMTItens[i]));
	SendMessage(GMTCombobox, CB_SETCURSEL, settingsSntp.gmt, 0);
	SendMessage(GMTCombobox, CB_SETDROPPEDWIDTH, 300, 0);

	for (i = 0; i < sizeof(EncAbsConfig) / sizeof(EncAbsConfig[0]); i++)
		SendMessage(SSIModeCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)EncAbsConfig[i]));
	SendMessage(SSIModeCombobox, CB_SETCURSEL, settingsEncSSI.mode, 0);

	if (settingsSntp.dailysave)
		SendMessage(DailySaveCheckbox, BM_SETCHECK, BST_CHECKED, 0);

	if (settingsSntp.sntp_enable)
		SendMessage(SNTPEnableCheckbox, BM_SETCHECK, BST_CHECKED, 0);

	sprintf(buf, "%d", settingsEncInc.perimeter);
	SendMessage(PerimeterTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%d", settingsEncInc.pulses);
	SendMessage(PulsesTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%05f", settingsEncInc.factor);
	SendMessage(FactorTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	if (settingsEncInc.x4)
		SendMessage(X4Checkbox, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(X2Checkbox, BM_SETCHECK, BST_CHECKED, 0);

	//SendMessage(BaudRateCombobox, CB_SETCURSEL, 0, 0);
	SendMessage(BaudRateCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	sprintf(buf, "%d", settingsEncSSI.perimeter);
	SendMessage(SSIPerimeterTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%05f", settingsEncSSI.factor);
	SendMessage(SSIFactorTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%d", settingsEncSSI.size);
	SendMessage(SSISizeTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%d", settingsEncSSI.size_bpr);
	SendMessage(SSISizeRevTextbox, WM_SETTEXT, 0, (LPARAM)buf);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(ConfDialog, TRUE);
    SetFocus(OkButton);

	ladder->CheckpointBegin(_("Alterar Configurações"));

    MSG msg;
    DWORD ret;
    DialogDone = FALSE;
    DialogCancel = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN && GetFocus() != InfoDescriptionTextbox) {
//                DialogDone = TRUE;
//                break;
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
		SendMessage(InfoProjectTextbox    , WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		settingsInfo.Name        = buf;

		SendMessage(InfoDeveloperTextbox  , WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		settingsInfo.Developer   = buf;

		SendMessage(InfoDescriptionTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)buf);
		settingsInfo.Description = buf;

		ladder->setSettingsInformation(settingsInfo);

		// Recebe dados de rede configurados
		SendMessage(ip  , IPM_GETADDRESS, 0, (LPARAM)&settingsNetwork.ip  );
		SendMessage(mask, IPM_GETADDRESS, 0, (LPARAM)&settingsNetwork.mask);
		SendMessage(gw  , IPM_GETADDRESS, 0, (LPARAM)&settingsNetwork.gw  );
		SendMessage(dns , IPM_GETADDRESS, 0, (LPARAM)&settingsNetwork.dns );

		ladder->setSettingsNetwork(settingsNetwork);

        SendMessage(BaudRateCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
            (LPARAM)(buf));
        settingsUart.baudRate = atoi(buf);

		settingsUart.UART = SendMessage(ParityCombobox, CB_GETCURSEL, 0, 0);

		ladder->setSettingsUART(settingsUart);

        SendMessage(ModBUSIDTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsMbSlave.ModBUSID = min(max(0, atoi(buf)), 127);

		ladder->setSettingsModbusSlave(settingsMbSlave);

		SendMessage(SNTPCombobox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsSntp.sntp_server = buf;

		settingsSntp.gmt = SendMessage(GMTCombobox, CB_GETCURSEL, 0, 0);
		settingsSntp.dailysave   = (SendMessage(DailySaveCheckbox , BM_GETSTATE, 0, 0) & BST_CHECKED) ? true : false;
		settingsSntp.sntp_enable = (SendMessage(SNTPEnableCheckbox, BM_GETSTATE, 0, 0) & BST_CHECKED) ? true : false;

		ladder->setSettingsSNTP(settingsSntp);

		// Atualizacao do Encoder Incremental
		settingsEncInc.conv_mode = SendMessage(IncConvModeCombobox, CB_GETCURSEL, 0, 0);

		SendMessage(PerimeterTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsEncInc.perimeter = max(0, atoi(buf));

        SendMessage(PulsesTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsEncInc.pulses = min(max(0, atoi(buf)), 5000);

        SendMessage(FactorTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsEncInc.factor = (float)atof(buf);
		if(settingsEncInc.factor < 0.0)
			settingsEncInc.factor = 1.0;

		settingsEncInc.x4 = (SendMessage(X4Checkbox, BM_GETCHECK, 0, 0) & BST_CHECKED) ? true : false;

		ladder->setSettingsEncoderIncremental(settingsEncInc);

		SendMessage(SSISizeTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsEncSSI.size = min(max(1, atoi(buf)), 32);

		SendMessage(SSISizeRevTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsEncSSI.size_bpr =  min(max(1, atoi(buf)), settingsEncSSI.size);

		settingsEncSSI.mode = SendMessage(SSIModeCombobox, CB_GETCURSEL, 0, 0);

        SendMessage(SSIPerimeterTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsEncSSI.perimeter = max(0, atoi(buf));

        SendMessage(SSIFactorTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		settingsEncSSI.factor = (float)atof(buf);
		if(settingsEncSSI.factor < 0.0)
			settingsEncSSI.factor = 1.0;

		settingsEncSSI.conv_mode = SendMessage(SSIConvModeCombobox, CB_GETCURSEL, 0, 0);

		ladder->setSettingsEncoderSSI(settingsEncSSI);

		settingsDac.ramp_abort_mode = SendMessage(AbortModeCombobox, CB_GETCURSEL, 0, 0) + 1;

		ladder->setSettingsDAC(settingsDac);

		changed = true;
	} else {
		ladder->CheckpointRollback();
	}

	ladder->CheckpointEnd();

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(ConfDialog);
    return changed;
}
