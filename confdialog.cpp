//-----------------------------------------------------------------------------
// Copyright 2007 Jonathan Westhues
//
// This file is part of LDmicro.
// 
// LDmicro is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// LDmicro is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with LDmicro.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// Dialog for setting the overall PLC parameters. Mostly this relates to
// timing; to set up the timers we need to know the desired cycle time,
// which is configurable, plus the MCU clock (i.e. crystal frequency).
// Jonathan Westhues, Nov 2004
//-----------------------------------------------------------------------------
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
//static HWND PLCCombobox;
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
								_("(GMT-10:00) Hawa�"), _("(GMT-09:00) Alasca"),
								_("(GMT-08:00) Hora do Pac�fico"), _("(GMT-07:00) Hora das Montanhas (EUA e Canad�)"),
								_("(GMT-06:00) Am�rica Central, Hora Central EUA/Canad�"), _("(GMT-05:00) Rio Branco, Lima, Bogot�"),
								_("(GMT-04:00) Manaus, Caracas, La Paz"), _("(GMT-03:00) Brasilia, Buenos Aires"),
								_("(GMT-02:00) Atl�ntico Central"), _("(GMT-01:00) A�ores, Cabo Verde"),
								_("(GMT 00:00) Hora de Greenwich: Londres, Dublin, Lisboa"), _("(GMT+01:00) Berlim, Estocolmo, Roma, Bruxelas"),
								_("(GMT+02:00) Atenas, Helsinque, Leste Europeu, Jerusal�m"), _("(GMT+03:00) Bagd�, Kuwait, Nair�bi, Moscou,Riad"),
								_("(GMT+04:00) Abu Dhabi, Mascate, Hora Padr�o do C�ucaso"), _("(GMT+05:00) Islamabad, Karachi, Ekaterinburgo"),
								_("(GMT+06:00) Almaty, Dacca"), _("(GMT+07:00) Bangcoc, Jacarta, Han�i"),
								_("(GMT+08:00) Pequim, Hong Kong, Taiwan, Cingapura"), _("(GMT+09:00) T�quio, Osaka, Sapporo, Seul, Yakutsk"),
								_("(GMT+10:00) Brisbane, Camberra, Melbourne, Sydney"), _("(GMT+11:00) Magad�, Ilhas Salom�o, Nova Caled�nia"),
								_("(GMT+12:00) Fiji, Kamchatka, Auckland"), _("(GMT+13:00) Nuku'alofa") };

const LPCTSTR ComboboxBaudRateItens[] = { /*_("2400"), _("4800"), _("7200"),*/ _("9600"), _("14400"), _("19200")/*, _("28800"), 
						_("38400"), _("57600"), _("115200")*/ };

const LPCTSTR ComboboxParityItens[] = { _("8-None-1"), _("8-Even-1"), _("8-Odd-1"), _("7-None-1"), _("7-Even-1"), _("7-Odd-1") };

// http://www.ntp.br/
// http://pt.wikipedia.org/wiki/Network_Time_Protocol
// http://tf.nist.gov/tf-cgi/servers.cgi
// https://support.ntp.org/bin/view/Servers/StratumOneTimeServers
// http://support.ntp.org/bin/view/Servers/NTPPoolServers
const LPCTSTR ComboboxSNTPItens[] = { _("br.pool.ntp.org"), _("a.ntp.br"), _("gps.ntp.br"), _("pool.ntp.org"), _("time-nw.nist.gov"), _("time.microsoft.com") };

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

	container_tabs.pszText = "Serial";
	TabCtrl_InsertItem(TabCtrl, 0, &container_tabs);

	container_tabs.pszText = "Rede";
	TabCtrl_InsertItem(TabCtrl, 1, &container_tabs);

	container_tabs.pszText = "Posicionamento";
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

	HWND textLabel5 = CreateWindowEx(0, WC_STATIC, _("Endere�o IP:"),
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

	HWND textLabel16 = CreateWindowEx(0, WC_STATIC, _("Fuso Hor�rio:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 240, 145, 21,  TabChild[1], NULL, Instance, NULL);
    NiceFont(textLabel16);

	GMTCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN,
        205, 240, 155, 145,  TabChild[1], NULL, Instance, NULL);
    NiceFont(GMTCombobox);
    
	DailySaveCheckbox = CreateWindowEx(0, WC_BUTTON, _("Hor�rio de ver�o"),
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

    HWND textLabel21 = CreateWindowEx(0, WC_STATIC, _("N�mero Pulsos:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 120, 145, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(textLabel21);

    PulsesTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 120, 85, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(PulsesTextbox);

    HWND textLabel22 = CreateWindowEx(0, WC_STATIC, _("Fator Corre��o:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        55, 150, 145, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(textLabel22);

    FactorTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        205, 150, 85, 21, TabChild[2], NULL, Instance, NULL);
    NiceFont(FactorTextbox);

    HWND textLabel23 = CreateWindowEx(0, WC_STATIC, _("Fator Multiplica��o:"),
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

void ShowConfDialog(bool NetworkSection)
{
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
/*    sprintf(buf, "%.1f", (Prog.cycleTime / 1000.0));
    SendMessage(CycleTextbox, WM_SETTEXT, 0, (LPARAM)buf);*/

	SendMessage(ip  , IPM_SETADDRESS, 0, MAKEIPADDRESS(Prog.ip  [0], Prog.ip  [1], Prog.ip  [2], Prog.ip  [3]));
	SendMessage(mask, IPM_SETADDRESS, 0, MAKEIPADDRESS(Prog.mask[0], Prog.mask[1], Prog.mask[2], Prog.mask[3]));
	SendMessage(gw  , IPM_SETADDRESS, 0, MAKEIPADDRESS(Prog.gw  [0], Prog.gw  [1], Prog.gw  [2], Prog.gw  [3]));
	SendMessage(dns , IPM_SETADDRESS, 0, MAKEIPADDRESS(Prog.dns [0], Prog.dns [1], Prog.dns [2], Prog.dns [3]));

	/*for (i = 0; i < sizeof(ComboboxPLCItens) / sizeof(ComboboxPLCItens[0]); i++)
		SendMessage(PLCCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxPLCItens[i]));*/

	/*SendMessage(PLCCombobox, CB_SETCURSEL, 0, 0);
	SendMessage(PLCCombobox, CB_SETDROPPEDWIDTH, 100, 0);*/

	for (i = 0; i < sizeof(ComboboxParityItens) / sizeof(ComboboxParityItens[0]); i++)
		SendMessage(ParityCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ComboboxParityItens[i]));

	SendMessage(ParityCombobox, CB_SETCURSEL, Prog.UART, 0);
	SendMessage(ParityCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	sprintf(buf, "%d", Prog.baudRate);
	for (i = 0; i < sizeof(ComboboxBaudRateItens) / sizeof(ComboboxBaudRateItens[0]); i++)
	{
		SendMessage(BaudRateCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBaudRateItens[i]));
		if (_stricmp(ComboboxBaudRateItens[i], buf) == 0)
			SendMessage(BaudRateCombobox, CB_SETCURSEL, i, 0);
	}

	for (i = 0; i < sizeof(ComboboxSNTPItens) / sizeof(ComboboxSNTPItens[0]); i++)
		SendMessage(SNTPCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ComboboxSNTPItens[i]));
	SendMessage(SNTPCombobox, WM_SETTEXT, 0, (LPARAM)Prog.sntp);

	for (i = 0; i < sizeof(ComboboxGMTItens) / sizeof(ComboboxGMTItens[0]); i++)
		SendMessage(GMTCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ComboboxGMTItens[i]));
	SendMessage(GMTCombobox, CB_SETCURSEL, Prog.gmt, 0);
	SendMessage(GMTCombobox, CB_SETDROPPEDWIDTH, 300, 0);

	if (Prog.dailysave)
		SendMessage(DailySaveCheckbox, BM_SETCHECK, BST_CHECKED, 0);

	sprintf(buf, "%d", Prog.diameter);
	SendMessage(DiameterTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%d", Prog.pulses);
	SendMessage(PulsesTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	sprintf(buf, "%05f", Prog.factor);
	SendMessage(FactorTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	if (Prog.x4)
		SendMessage(X4Checkbox, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(X2Checkbox, BM_SETCHECK, BST_CHECKED, 0);

	//SendMessage(BaudRateCombobox, CB_SETCURSEL, 0, 0);
	SendMessage(BaudRateCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	//sprintf(buf, "%.6f", Prog.mcuClock / 1e6);
    //SendMessage(CrystalTextbox, WM_SETTEXT, 0, (LPARAM)buf);

    //sprintf(buf, "%d", Prog.baudRate);
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
        Prog.cycleTime = (int)(1000*atof(buf) + 0.5);
        if(Prog.cycleTime == 0) {
            Error(_("Zero cycle time not valid; resetting to 10 ms."));
            Prog.cycleTime = 10000;
        }*/

        /*SendMessage(CrystalTextbox, WM_GETTEXT, (WPARAM)sizeof(buf),
            (LPARAM)(buf));*/
        //Prog.mcuClock = (int)(1e6*atof(buf) + 0.5);

		// Recebe dados de rede configurados
		SendMessage(ip  , IPM_GETADDRESS, 0, (LPARAM)&ipaddress);
		SendMessage(mask, IPM_GETADDRESS, 0, (LPARAM)&ipmask   );
		SendMessage(gw  , IPM_GETADDRESS, 0, (LPARAM)&ipgw     );
		SendMessage(dns , IPM_GETADDRESS, 0, (LPARAM)&ipdns    );

		// Carrega IP
		Prog.ip[0] = FIRST_IPADDRESS (ipaddress);
		Prog.ip[1] = SECOND_IPADDRESS(ipaddress);
		Prog.ip[2] = THIRD_IPADDRESS (ipaddress);
		Prog.ip[3] = FOURTH_IPADDRESS(ipaddress);

		// Carrega Mascara
		Prog.mask[0] = FIRST_IPADDRESS (ipmask);
		Prog.mask[1] = SECOND_IPADDRESS(ipmask);
		Prog.mask[2] = THIRD_IPADDRESS (ipmask);
		Prog.mask[3] = FOURTH_IPADDRESS(ipmask);

		// Carrega Gateway
		Prog.gw[0] = FIRST_IPADDRESS (ipgw);
		Prog.gw[1] = SECOND_IPADDRESS(ipgw);
		Prog.gw[2] = THIRD_IPADDRESS (ipgw);
		Prog.gw[3] = FOURTH_IPADDRESS(ipgw);

		// Carrega DNS
		Prog.dns[0] = FIRST_IPADDRESS (ipdns);
		Prog.dns[1] = SECOND_IPADDRESS(ipdns);
		Prog.dns[2] = THIRD_IPADDRESS (ipdns);
		Prog.dns[3] = FOURTH_IPADDRESS(ipdns);

        SendMessage(BaudRateCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
            (LPARAM)(buf));
        Prog.baudRate = atoi(buf);

		Prog.UART = SendMessage(ParityCombobox, CB_GETCURSEL, 0, 0);

		SendMessage(SNTPCombobox, WM_GETTEXT, (WPARAM)sizeof(Prog.sntp),
            (LPARAM)(Prog.sntp));

		Prog.gmt = SendMessage(GMTCombobox, CB_GETCURSEL, 0, 0);
		Prog.dailysave = SendMessage(DailySaveCheckbox, BM_GETSTATE, 0, 0) & BST_CHECKED;

		Prog.x4 = (char)SendMessage(X4Checkbox, BM_GETCHECK, 0, 0);
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(ConfDialog);
    return;
}
