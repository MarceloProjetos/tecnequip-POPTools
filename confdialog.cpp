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

static HWND ConfDialog;

static HWND CrystalTextbox;
static HWND CycleTextbox;
static HWND BaudTextbox;
static HWND PLCCombobox;
static HWND BaudRateCombobox;
static HWND ComPortCombobox;

static HWND ip[4];
static HWND mask[4];
static HWND gw[4];

static LONG_PTR PrevCrystalProc;
static LONG_PTR PrevCycleProc;
static LONG_PTR PrevBaudProc;
static LONG_PTR PrevComPortProc;

static LONG_PTR PrevIpProc[4];
static LONG_PTR PrevMaskProc[4];
static LONG_PTR PrevGwProc[4];

const LPCTSTR ComboboxPLCItens[] = { _("POP7"), _("POP9") };

const LPCTSTR ComboboxComPortItens[] = { _("COM1"), _("COM2"), _("COM3"), _("COM4"), _("COM5"), _("COM6"), _("COM7"), 
						_("COM8"), _("COM9"), _("COM10"), _("COM11"), _("COM12"), _("COM13"), _("COM14"), _("COM15"), _("COM16"),
						_("COM17"), _("COM18"), _("COM19"), _("COM20"), _("COM21"), _("COM22") };

const LPCTSTR ComboboxBaudRateItens[] = { _("2400"), _("4800"), _("7200"), _("9600"), _("14400"), _("19200"), _("28800"), 
						_("38400"), _("57600"), _("115200") };

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
    else if(hwnd ==ComPortCombobox)
        t = PrevComPortProc;
    else
        oops();

    return CallWindowProc((WNDPROC)t, hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK MyByteProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isdigit(wParam) || wParam == '\b')) {
            return 0;
        }
    }

	HWND h = NULL; // handler
    char buf[16];
    if(hwnd == ip[0])
		h = ip[0];
    else if(hwnd == ip[1])
		h = ip[1];
    else if(hwnd == ip[2])
		h = ip[2];
    else if(hwnd == ip[3])
		h = ip[3];
    else if(hwnd == mask[0])
		h = mask[0];
    else if(hwnd == mask[1])
		h = mask[1];
    else if(hwnd == mask[2])
		h = mask[2];
    else if(hwnd == mask[3])
		h = mask[3];
    else if(hwnd == gw[0])
		h = gw[0];
    else if(hwnd == gw[1])
		h = gw[1];
    else if(hwnd == gw[2])
		h = gw[2];
    else if(hwnd == gw[3])
		h = gw[3];

	if (h != NULL)
	{
		DefWindowProc(h, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		if (atoi(buf) > 255)
			return 0;
	}

	LONG_PTR t;
    if(hwnd == ip[0])
        t = PrevIpProc[0];
    else if(hwnd == ip[1])
        t = PrevIpProc[1];
    else if(hwnd == ip[2])
        t = PrevIpProc[2];
    else if(hwnd == ip[3])
        t = PrevIpProc[3];
    else if(hwnd == mask[0])
        t = PrevMaskProc[0];
    else if(hwnd == mask[1])
        t = PrevMaskProc[1];
    else if(hwnd == mask[2])
        t = PrevMaskProc[2];
    else if(hwnd == mask[3])
        t = PrevMaskProc[3];
    else if(hwnd == gw[0])
        t = PrevGwProc[0];
    else if(hwnd == gw[1])
        t = PrevGwProc[1];
    else if(hwnd == gw[2])
        t = PrevGwProc[2];
    else if(hwnd == gw[3])
        t = PrevGwProc[3];
    else
        oops();

    return CallWindowProc((WNDPROC)t, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Cycle Time (ms):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 13, 145, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    CycleTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 12, 85, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(CycleTextbox);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC,
        _("Modelo do CLP:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        0, 43, 150, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	PLCCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 42, 85, 100, ConfDialog, NULL, Instance, NULL);
    NiceFont(PLCCombobox);

    HWND textLabel3 = CreateWindowEx(0, WC_STATIC, _("RS485 Baud Rate (bps):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 73, 145, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel3);

	BaudRateCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 72, 85, 100, ConfDialog, NULL, Instance, NULL);
    NiceFont(BaudRateCombobox);

    HWND textLabel4 = CreateWindowEx(0, WC_STATIC, _("Porta Gravação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 105, 145, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel4);

	ComPortCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        155, 105, 85, 100, ConfDialog, NULL, Instance, NULL);
    NiceFont(ComPortCombobox);

	HWND textLabel5 = CreateWindowEx(0, WC_STATIC, _("Endereço IP:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 146, 145, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel5);

    ip[0] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 146, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(ip[0]);

	HWND textPoint1 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        189, 152, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

	ip[1] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        195, 146, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(ip[1]);

	HWND textPoint2 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        229, 152, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

    ip[2] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        235, 146, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(ip[2]);

	HWND textPoint3 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        269, 152, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

    ip[3] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        275, 146, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(ip[3]);

    HWND textLabel6 = CreateWindowEx(0, WC_STATIC, _("Mascara:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 173, 145, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel6);

    mask[0] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 173, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(mask[0]);

	HWND textPoint4 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        189, 179, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

    mask[1] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        195, 173, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(mask[1]);

	HWND textPoint5 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        229, 179, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

    mask[2] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        235, 173, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(mask[2]);

	HWND textPoint6 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        269, 179, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

    mask[3] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        275, 173, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(mask[3]);
	
    HWND textLabel7 = CreateWindowEx(0, WC_STATIC, _("Gateway:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 200, 145, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textLabel7);

    gw[0] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        155, 200, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(gw[0]);

	HWND textPoint7 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        189, 206, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

    gw[1] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        195, 200, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(gw[1]);

	HWND textPoint8 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        229, 206, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

    gw[2] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        235, 200, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(gw[2]);

	HWND textPoint9 = CreateWindowEx(0, WC_STATIC, _("."),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        269, 206, 5, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(textPoint1);

    gw[3] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        275, 200, 35, 21, ConfDialog, NULL, Instance, NULL);
    NiceFont(gw[3]);
	
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Configuração de Rede"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        5, 125, 325, 105, ConfDialog, NULL, Instance, NULL);
    NiceFont(grouper);

	if(!UartFunctionUsed()) {   
        EnableWindow(BaudRateCombobox, FALSE);
        EnableWindow(textLabel3, FALSE);
    }

    if(Prog.mcu && (Prog.mcu->whichIsa == ISA_ANSIC ||
        Prog.mcu->whichIsa == ISA_INTERPRETED)) 
    {
        //EnableWindow(CrystalTextbox, FALSE);
        EnableWindow(textLabel2, FALSE);
    }

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        258, 11, 70, 23, ConfDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        258, 41, 70, 23, ConfDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevCycleProc = SetWindowLongPtr(CycleTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNumberProc);

    PrevCrystalProc = SetWindowLongPtr(CrystalTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNumberProc);

    PrevBaudProc = SetWindowLongPtr(BaudTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNumberProc);

    PrevIpProc[0] = SetWindowLongPtr(ip[0], GWLP_WNDPROC, 
        (LONG_PTR)MyByteProc);

	PrevIpProc[1] = SetWindowLongPtr(ip[1], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

	PrevIpProc[2] = SetWindowLongPtr(ip[2], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

	PrevIpProc[3] = SetWindowLongPtr(ip[3], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

    PrevMaskProc[0] = SetWindowLongPtr(mask[0], GWLP_WNDPROC, 
	    (LONG_PTR)MyByteProc);

	PrevMaskProc[1] = SetWindowLongPtr(mask[1], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

	PrevMaskProc[2] = SetWindowLongPtr(mask[2], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

	PrevMaskProc[3] = SetWindowLongPtr(mask[3], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

    PrevGwProc[0] = SetWindowLongPtr(gw[0], GWLP_WNDPROC, 
	    (LONG_PTR)MyByteProc);

	PrevGwProc[1] = SetWindowLongPtr(gw[1], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

	PrevGwProc[2] = SetWindowLongPtr(gw[2], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

	PrevGwProc[3] = SetWindowLongPtr(gw[3], GWLP_WNDPROC, 
		(LONG_PTR)MyByteProc);

}

void ShowConfDialog(void)
{
    // The window's height will be resized later, to fit the explanation text.
    ConfDialog = CreateWindowClient(0, "LDmicroDialog", _("PLC Configuration"),
        WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 335, 235, NULL, NULL, Instance, NULL);

    MakeControls();
   
    char buf[16];
    sprintf(buf, "%.1f", (Prog.cycleTime / 1000.0));
    SendMessage(CycleTextbox, WM_SETTEXT, 0, (LPARAM)buf);

	int i;

	for (i = 0; i < 4; i++)
	{
		sprintf(buf, "%d", Prog.ip[i]);
		SendMessage(ip[i], WM_SETTEXT, 0, (LPARAM)buf);

		sprintf(buf, "%d", Prog.mask[i]);
		SendMessage(mask[i], WM_SETTEXT, 0, (LPARAM)buf);

		sprintf(buf, "%d", Prog.gw[i]);
		SendMessage(gw[i], WM_SETTEXT, 0, (LPARAM)buf);
	}

	for (i = 0; i < sizeof(ComboboxPLCItens) / sizeof(ComboboxPLCItens[0]); i++)
		SendMessage(PLCCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxPLCItens[i]));

	SendMessage(PLCCombobox, CB_SETCURSEL, 0, 0);
	SendMessage(PLCCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	for (i = 0; i < sizeof(ComboboxComPortItens) / sizeof(ComboboxComPortItens[0]); i++)
		SendMessage(ComPortCombobox, CB_INSERTSTRING, i, (LPARAM)((LPCTSTR)ComboboxComPortItens[i]));

	SendMessage(ComPortCombobox, CB_SETCURSEL, Prog.comPort ? Prog.comPort - 1 : 0, 0);
	SendMessage(ComPortCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	for (i = 0; i < sizeof(ComboboxBaudRateItens) / sizeof(ComboboxBaudRateItens[0]); i++)
		SendMessage(BaudRateCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBaudRateItens[i]));

	SendMessage(BaudRateCombobox, CB_SETCURSEL, 3, 0);
	SendMessage(BaudRateCombobox, CB_SETDROPPEDWIDTH, 100, 0);

	//sprintf(buf, "%.6f", Prog.mcuClock / 1e6);
    //SendMessage(CrystalTextbox, WM_SETTEXT, 0, (LPARAM)buf);

    sprintf(buf, "%d", Prog.baudRate);
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
        SendMessage(CycleTextbox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
        Prog.cycleTime = (int)(1000*atof(buf) + 0.5);
        if(Prog.cycleTime == 0) {
            Error(_("Zero cycle time not valid; resetting to 10 ms."));
            Prog.cycleTime = 10000;
        }

        /*SendMessage(CrystalTextbox, WM_GETTEXT, (WPARAM)sizeof(buf),
            (LPARAM)(buf));*/
        //Prog.mcuClock = (int)(1e6*atof(buf) + 0.5);

		for (i = 0; i < 4; i++)
		{
			SendMessage(ip[i], WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
			if (atoi(buf) > 255)
				Error(_("IP invalido. Deve ser entre 0.0.0.0 a 255.255.255.255 !"));
			else 
				Prog.ip[i] = atoi(buf);

			SendMessage(mask[i], WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));

			if (atoi(buf) > 255)
				Error(_("Mascara invalida. Deve ser entre 0.0.0.0 a 255.255.255.255 !"));
			else 
				Prog.mask[i] = atoi(buf);

			SendMessage(gw[i], WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));

			if (atoi(buf) > 255)
				Error(_("Gateway invalido. Deve ser entre 0.0.0.0 a 255.255.255.255 !"));
			else 
				Prog.gw[i] = atoi(buf);
		}

        SendMessage(BaudRateCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
            (LPARAM)(buf));
        Prog.baudRate = atoi(buf);

		Prog.comPort = SendMessage(ComPortCombobox, CB_GETCURSEL, 0, 0) + 1;
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(ConfDialog);
    return;
}
