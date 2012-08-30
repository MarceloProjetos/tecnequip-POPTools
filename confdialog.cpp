#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>

#include "ldmicro.h"

// Quantidade de paginas do TabControl da janela de Configuracao
#define CONF_NPAGES 3

static HWND ConfDialog;
static HWND TabChild[CONF_NPAGES];
static HWND TabCtrl;

static HWND CrystalTextbox;
static HWND CycleTextbox;
static HWND BaudTextbox;
static HWND ModBUSIDTextbox;
static HWND BaudRateCombobox;
static HWND ParityCombobox;
static HWND SNTPCombobox;
static HWND GMTCombobox;
static HWND DailySaveCheckbox;

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
								_("(GMT-10:00) Hawaí"), _("(GMT-09:00) Alasca"),
								_("(GMT-08:00) Hora do Pacífico"), _("(GMT-07:00) Hora das Montanhas (EUA e Canadá)"),
								_("(GMT-06:00) América Central, Hora Central EUA/Canadá"), _("(GMT-05:00) Lima, Bogotá"),
								_("(GMT-04:00) Rio Branco, Manaus, Caracas, La Paz"), _("(GMT-03:00) Brasilia, Buenos Aires"),
								_("(GMT-02:00) Atlântico Central"), _("(GMT-01:00) Açores, Cabo Verde"),
								_("(GMT 00:00) Hora de Greenwich: Londres, Dublin, Lisboa"), _("(GMT+01:00) Berlim, Estocolmo, Roma, Bruxelas"),
								_("(GMT+02:00) Atenas, Helsinque, Leste Europeu, Jerusalém"), _("(GMT+03:00) Bagdá, Kuwait, Nairóbi, Moscou,Riad"),
								_("(GMT+04:00) Abu Dhabi, Mascate, Hora Padrão do Cáucaso"), _("(GMT+05:00) Islamabad, Karachi, Ekaterinburgo"),
								_("(GMT+06:00) Almaty, Dacca"), _("(GMT+07:00) Bangcoc, Jacarta, Hanói"),
								_("(GMT+08:00) Pequim, Hong Kong, Taiwan, Cingapura"), _("(GMT+09:00) Tóquio, Osaka, Sapporo, Seul, Yakutsk"),
								_("(GMT+10:00) Brisbane, Camberra, Melbourne, Sydney"), _("(GMT+11:00) Magadã, Ilhas Salomão, Nova Caledônia"),
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

char *SerialParityString[] = { _("Sem Paridade"), _("Paridade Ímpar"), _("Paridade Par") };

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

void OnTabChange(void)
{
	unsigned int i, CurrentTab = TabCtrl_GetCurSel(TabCtrl);
	for(i=0; i<CONF_NPAGES; i++) {
		ShowWindow(TabChild[i], i==CurrentTab ? TRUE : FALSE);
	}
}

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK ConfDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
        case WM_NOTIFY:
            if(((LPNMHDR)lParam)->code == TCN_SELCHANGING) {
				return FALSE;
	        } else if(((LPNMHDR)lParam)->code == TCN_SELCHANGE) {
				OnTabChange();
	        }
            break;

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

static void MakeControls(void)
{
	TCITEM container_tabs;
/*
	HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Cycle Time (ms):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 13, 145, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    CycleTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 12, 85, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(CycleTextbox);
	*/
    /*HWND textLabel2 = CreateWindowEx(0, WC_STATIC,
        _("Modelo do CLP:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        0, 43, 150, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);*/

	/*PLCCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 42, 85, 100, ConfDialog, NULL, Instance, NULL);
    NiceFont(PLCCombobox);*/

	TabCtrl = CreateWindow(WC_TABCONTROL, _("TabControl"),
		WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD,
        5, 5, 325, 250, ConfDialog, NULL, Instance, NULL);
    NiceFont(TabCtrl);

	container_tabs.mask   = TCIF_TEXT;

	container_tabs.pszText = _("Serial");
	TabCtrl_InsertItem(TabCtrl, 0, &container_tabs);

	container_tabs.pszText = _("Rede");
	TabCtrl_InsertItem(TabCtrl, 1, &container_tabs);

	container_tabs.pszText = _("Posicionamento");
	TabCtrl_InsertItem(TabCtrl, 2, &container_tabs);

    TabChild[0] = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        -50, -50, 600, 600, TabCtrl, NULL, Instance, NULL);
    NiceFont(TabChild[0]);

    TabChild[1] = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | WS_CLIPSIBLINGS | BS_GROUPBOX | WS_VISIBLE,
        -50, -50, 600, 600, TabCtrl, NULL, Instance, NULL);
    NiceFont(TabChild[1]);

    TabChild[2] = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | WS_CLIPSIBLINGS | BS_GROUPBOX | WS_VISIBLE,
        -50, -50, 600, 600, TabCtrl, NULL, Instance, NULL);
    NiceFont(TabChild[2]);

    HWND textLabel3 = CreateWindowEx(0, WC_STATIC, _("RS485 Baud Rate (bps):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 90, 145, 21, TabChild[0], NULL, Instance, NULL);
    NiceFont(textLabel3);

	BaudRateCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        205, 90, 155, 100, TabChild[0], NULL, Instance, NULL);
    NiceFont(BaudRateCombobox);

    HWND textLabel31 = CreateWindowEx(0, WC_STATIC, _("Data/Paridade/Stop:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 120, 145, 21, TabChild[0], NULL, Instance, NULL);
    NiceFont(textLabel31);

	ParityCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        205, 120, 155, 100, TabChild[0], NULL, Instance, NULL);
    NiceFont(ParityCombobox);

    HWND textLabel4 = CreateWindowEx(0, WC_STATIC, _("ModBUS ID:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 150, 145, 21, TabChild[0], NULL, Instance, NULL);
    NiceFont(textLabel4);

    ModBUSIDTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 150, 155, 21, TabChild[0], NULL, Instance, NULL);
    NiceFont(ModBUSIDTextbox);

	HWND textLabel5 = CreateWindowEx(0, WC_STATIC, _("Endereço IP:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 90, 145, 21, TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel5);

    ip = CreateWindowEx(0, WC_IPADDRESS, _("IP"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 90, 155, 21,  TabChild[1], NULL, Instance, NULL);
//    NiceFont(ip);

    HWND textLabel6 = CreateWindowEx(0, WC_STATIC, _("Mascara:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 120, 145, 21,  TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel6);
	
    mask = CreateWindowEx(0, WC_IPADDRESS, _("MASK"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 120, 155, 21,  TabChild[1], NULL, Instance, NULL);
//    NiceFont(mask);

    HWND textLabel7 = CreateWindowEx(0, WC_STATIC, _("Gateway:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 150, 145, 21,  TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel7);

    gw = CreateWindowEx(0, WC_IPADDRESS, _("GW"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 150, 155, 21,  TabChild[1], NULL, Instance, NULL);
//    NiceFont(gw);

    HWND textLabel11 = CreateWindowEx(0, WC_STATIC, _("DNS:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 180, 145, 21,  TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel11);

    dns = CreateWindowEx(0, WC_IPADDRESS, _("DNS"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 180, 155, 21,  TabChild[1], NULL, Instance, NULL);
//    NiceFont(dns);

    HWND textLabel15 = CreateWindowEx(0, WC_STATIC, _("Servidor de Tempo:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 210, 145, 21,  TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel15);

	SNTPCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN,
        205, 210, 155, 145,  TabChild[1], NULL, Instance, NULL);
    NiceFont(SNTPCombobox);

	HWND textLabel16 = CreateWindowEx(0, WC_STATIC, _("Fuso Horário:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 240, 145, 21,  TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel16);

	GMTCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN,
        205, 240, 155, 145,  TabChild[1], NULL, Instance, NULL);
    NiceFont(GMTCombobox);
    
	DailySaveCheckbox = CreateWindowEx(0, WC_BUTTON, _("Horário de verão"),
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        205, 270, 160, 20,  TabChild[1], NULL, Instance, NULL);
    NiceFont(DailySaveCheckbox);

    HWND textLabel20 = CreateWindowEx(0, WC_STATIC, _("Diametro Roda:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 90, 145, 21,  TabChild[2], NULL, Instance, NULL);
    NiceFont(textLabel20);

    DiameterTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 90, 85, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(DiameterTextbox);

    HWND textLabel21 = CreateWindowEx(0, WC_STATIC, _("Número Pulsos:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 120, 145, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(textLabel21);

    PulsesTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 120, 85, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(PulsesTextbox);

    HWND textLabel22 = CreateWindowEx(0, WC_STATIC, _("Fator Correção:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 150, 145, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(textLabel22);

    FactorTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 150, 85, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(FactorTextbox);

    HWND textLabel23 = CreateWindowEx(0, WC_STATIC, _("Fator Multiplicação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 180, 145, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(textLabel23);

	X2Checkbox = CreateWindowEx(0, WC_BUTTON, _("x 2"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        205, 180, 40, 20, TabChild[2], NULL, Instance, NULL);
    NiceFont(X2Checkbox);

	X4Checkbox = CreateWindowEx(0, WC_BUTTON, _("x 4"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        245, 180, 40, 20, TabChild[2], NULL, Instance, NULL);
    NiceFont(X4Checkbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        185, 260, 70, 23, ConfDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        260, 260, 70, 23, ConfDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

//    PrevCycleProc = SetWindowLongPtr(CycleTextbox, GWLP_WNDPROC, 
//        (LONG_PTR)MyNumberProc);

//    PrevCrystalProc = SetWindowLongPtr(CrystalTextbox, GWLP_WNDPROC, 
//        (LONG_PTR)MyNumberProc);

    PrevBaudProc = SetWindowLongPtr(BaudTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNumberProc);

	OnTabChange();
}

bool ShowConfDialog(bool NetworkSection)
{
	bool changed = false;
	unsigned int i, ipaddress, ipmask, ipgw, ipdns;
    // The window's height will be resized later, to fit the explanation text.
    ConfDialog = CreateWindowClient(0, "LDmicroDialog", _("PLC Configuration"),
        WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 335, 288, MainWindow, NULL, Instance, NULL);

	PrevConfDialogProc = SetWindowLongPtr(ConfDialog, GWLP_WNDPROC, 
        (LONG_PTR)ConfDialogProc);

    MakeControls();
	if(NetworkSection) {
		TabCtrl_SetCurSel(TabCtrl, 1);
		OnTabChange();
	}

    char buf[16];
/*    sprintf(buf, "%.1f", (Prog.settings.cycleTime / 1000.0));
    SendMessage(CycleTextbox, WM_SETTEXT, 0, (LPARAM)buf);*/

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

    EnableWindow(MainWindow, FALSE);
    ShowWindow(ConfDialog, TRUE);
    SetFocus(CycleTextbox);

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

		changed = true;
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(ConfDialog);
    return changed;
}
