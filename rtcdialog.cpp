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
// Dialog for setting the properties of a set of contacts: negated or not,
// plus the name
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>

#include "ldmicro.h"

static HWND RTCDialog;

static HWND WDayRadio;
static HWND WDayCheckbox0;
static HWND WDayCheckbox1;
static HWND WDayCheckbox2;
static HWND WDayCheckbox3;
static HWND WDayCheckbox4;
static HWND WDayCheckbox5;
static HWND WDayCheckbox6;
static HWND MDayRadio;
static HWND HourTextbox;
static HWND MinTextbox;
static HWND SecTextbox;
static HWND MDayCombobox;
static HWND MonthCombobox;
static HWND YearCombobox;

static LONG_PTR PrevNameProc;

const LPCTSTR MDayItens[] = { _("1"), _("2"), _("3"), _("4"), _("5"), _("6"), _("7"), _("8"), _("9"), _("10"), 
									_("11"), _("12"), _("13"), _("14"), _("15"), _("16"), _("17"), _("18"), _("19"), _("20"), 
									_("21"), _("22"), _("23"), _("24"), _("25"), _("26"), _("27"), _("28"), _("29"), _("30"), 
									_("31")};

const LPCTSTR MonthItens[] = { _("*"), _("1"), _("2"), _("3"), _("4"), _("5"), _("6"), _("7"), _("8"), _("9"), _("10"), 
									_("11"), _("12")};

const LPCTSTR YearItens[] = { _("*"), _("2011"), _("2012"), _("2013"), _("2014"), _("2015"), _("2016"), _("2017"), _("2018"), _("2019"), _("2020")};

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
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Dias"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 3, 290, 95, RTCDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    WDayRadio = CreateWindowEx(0, WC_BUTTON, _("Dia da Semana"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        5, 21, 110, 20, grouper, NULL, Instance, NULL);
    NiceFont(WDayRadio);

    HWND textLabel0 = CreateWindowEx(0, WC_STATIC, _("D"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        92, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel0);

    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, _("S"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        115, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel1);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("T"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        140, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel2);

    HWND textLabel3 = CreateWindowEx(0, WC_STATIC, _("Q"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        165, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel3);

    HWND textLabel4 = CreateWindowEx(0, WC_STATIC, _("Q"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        190, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel4);

    HWND textLabel5 = CreateWindowEx(0, WC_STATIC, _("S"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        215, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel5);

    HWND textLabel6 = CreateWindowEx(0, WC_STATIC, _("S"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        240, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel6);

    WDayCheckbox0 = CreateWindowEx(0, WC_BUTTON, NULL,
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        122, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    WDayCheckbox1 = CreateWindowEx(0, WC_BUTTON, NULL,
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        145, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    WDayCheckbox2 = CreateWindowEx(0, WC_BUTTON, NULL,
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        170, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    WDayCheckbox3 = CreateWindowEx(0, WC_BUTTON, NULL,
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        195, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    WDayCheckbox4 = CreateWindowEx(0, WC_BUTTON, NULL,
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        220, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    WDayCheckbox5 = CreateWindowEx(0, WC_BUTTON, NULL,
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        245, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    WDayCheckbox6 = CreateWindowEx(0, WC_BUTTON, NULL,
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        270, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    MDayRadio = CreateWindowEx(0, WC_BUTTON, _("Dia do Mês"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        5, 61, 100, 20, grouper, NULL, Instance, NULL);
    NiceFont(MDayRadio);

	MDayCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        122, 61, 45, 140, grouper, NULL, Instance, NULL);
    NiceFont(MDayCombobox);

	MonthCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        167, 61, 45, 140, grouper, NULL, Instance, NULL);
    NiceFont(MonthCombobox);

	YearCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        212, 61, 65, 140, grouper, NULL, Instance, NULL);
    NiceFont(YearCombobox);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Horário:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 105, 50, 25, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    HourTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        130, 105, 30, 21, RTCDialog, NULL, Instance, NULL);
    FixedFont(HourTextbox);

    MinTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        160, 105, 30, 21, RTCDialog, NULL, Instance, NULL);
    FixedFont(MinTextbox);

    SecTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        190, 105, 30, 21, RTCDialog, NULL, Instance, NULL);
    FixedFont(SecTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        150, 140, 70, 23, RTCDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        225, 140, 70, 23, RTCDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(HourTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

void ShowRTCDialog(unsigned char * wday, unsigned char * mday, unsigned char * month, unsigned int * year, unsigned char * hour, unsigned char * minute, unsigned char * second)
{
    RTCDialog = CreateWindowClient(0, "LDmicroDialog",
        _("RTC"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 300, 170, MainWindow, NULL, Instance, NULL);

    MakeControls();
   
	int i;

	for (i = 0; i < sizeof(MDayItens) / sizeof(MDayItens[0]); i++)
		SendMessage(MDayCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)MDayItens[i]));

	for (i = 0; i < sizeof(MonthItens) / sizeof(MonthItens[0]); i++)
		SendMessage(MonthCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)MonthItens[i]));

	for (i = 0; i < sizeof(YearItens) / sizeof(YearItens[0]); i++)
		SendMessage(YearCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)YearItens[i]));

	SendMessage(MDayCombobox, CB_SETCURSEL, *mday - 1, 0);
	SendMessage(MonthCombobox, CB_SETCURSEL, *month, 0);
	SendMessage(YearCombobox, CB_SETCURSEL, *year > 0 ? *year - 2010 : 0, 0);

    if (*wday & (1 << 7))
        SendMessage(WDayRadio, BM_SETCHECK, BST_CHECKED, 0);
	else
        SendMessage(MDayRadio, BM_SETCHECK, BST_CHECKED, 0);

	char chour[10], cminute[10], csecond[10];

	_itoa(*hour, chour, 10);
	_itoa(*minute, cminute, 10);
	_itoa(*second, csecond, 10);

    SendMessage(HourTextbox, WM_SETTEXT, 0, (LPARAM)(chour));
	SendMessage(MinTextbox, WM_SETTEXT, 0, (LPARAM)(cminute));
	SendMessage(SecTextbox, WM_SETTEXT, 0, (LPARAM)(csecond));

	if (*wday & 1) SendMessage(WDayCheckbox0, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 2) SendMessage(WDayCheckbox1, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 4) SendMessage(WDayCheckbox2, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 8) SendMessage(WDayCheckbox3, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 16) SendMessage(WDayCheckbox4, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 32) SendMessage(WDayCheckbox5, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 64) SendMessage(WDayCheckbox6, BM_SETCHECK, BST_CHECKED, 0);
	
    EnableWindow(MainWindow, FALSE);
    ShowWindow(RTCDialog, TRUE);
    SetFocus(HourTextbox);
    SendMessage(HourTextbox, EM_SETSEL, 0, -1);

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

        if(IsDialogMessage(RTCDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	if(!DialogCancel)
	{
        SendMessage(HourTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(chour));
        SendMessage(MinTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(cminute));
        SendMessage(SecTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(csecond));

		if (atoi(chour) > 23)
			Error(_("Hora invalida, deve estar entre 0 e 23."));
		else if (atoi(cminute) > 59)
			Error(_("Minuto invalido, deve estar entre 0 e 59."));
		else if (atoi(csecond) > 59)
			Error(_("Segundo invalido, deve estar entre 0 e 59."));
		else
		{
			char buf[20];
			SendMessage(MDayCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
				(LPARAM)(buf));
			*mday = atoi(buf);

			SendMessage(MonthCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
				(LPARAM)(buf));
			*month = atoi(buf);

			SendMessage(YearCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
				(LPARAM)(buf));
			*year = atoi(buf);

			*hour = atoi(chour);
			*minute = atoi(cminute);
			*second = atoi(csecond);

			*wday = 0;

			if(SendMessage(WDayRadio, BM_GETSTATE, 0, 0) & BST_CHECKED)
				*wday |= 1 << 7;
			else
				*wday &= ~(1 << 7);

			if (SendMessage(WDayCheckbox0, BM_GETSTATE, 0, 0) & BST_CHECKED) *wday |= 1;
			if (SendMessage(WDayCheckbox1, BM_GETSTATE, 0, 0) & BST_CHECKED) *wday |= 2;
			if (SendMessage(WDayCheckbox2, BM_GETSTATE, 0, 0) & BST_CHECKED) *wday |= 4;
			if (SendMessage(WDayCheckbox3, BM_GETSTATE, 0, 0) & BST_CHECKED) *wday |= 8;
			if (SendMessage(WDayCheckbox4, BM_GETSTATE, 0, 0) & BST_CHECKED) *wday |= 16;
			if (SendMessage(WDayCheckbox5, BM_GETSTATE, 0, 0) & BST_CHECKED) *wday |= 32;
			if (SendMessage(WDayCheckbox6, BM_GETSTATE, 0, 0) & BST_CHECKED) *wday |= 64;
		}
    }

	EnableWindow(MainWindow, TRUE);
	DestroyWindow(RTCDialog);
	return;
}
