#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <time.h>

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

const LPCTSTR MDayItens[] = { _("Todos"), "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", 
									"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
									"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
									"31"};

const LPCTSTR MonthItens[] = { _("Todos"), _("Janeiro"), _("Fevereiro"), _("Março"), _("Abril"), _("Maio"), _("Junho"), _("Julho"), _("Agosto"), _("Setembro"), _("Outubro"), 
									_("Novembro"), _("Dezembro")};

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
	char *WeekDays = _("SMTWTFS"), WeekDay[2] = { 0, 0 };
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Dias"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 3, 350, 95, RTCDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    WDayRadio = CreateWindowEx(0, WC_BUTTON, _("Dia da Semana"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        15, 30, 110, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(WDayRadio);

	WeekDay[0] = WeekDays[0];
    HWND textLabel0 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        90, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel0);

	WeekDay[0] = WeekDays[1];
    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        125, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel1);

	WeekDay[0] = WeekDays[2];
    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        160, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel2);

	WeekDay[0] = WeekDays[3];
    HWND textLabel3 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        195, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel3);

	WeekDay[0] = WeekDays[4];
    HWND textLabel4 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        230, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel4);

	WeekDay[0] = WeekDays[5];
    HWND textLabel5 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        265, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel5);

	WeekDay[0] = WeekDays[6];
    HWND textLabel6 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        300, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel6);

    WDayCheckbox0 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        120, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    WDayCheckbox1 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        155, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox1);

    WDayCheckbox2 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        190, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox2);

    WDayCheckbox3 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        225, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox3);

    WDayCheckbox4 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        260, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox4);

    WDayCheckbox5 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        295, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox5);

    WDayCheckbox6 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        330, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox6);

    MDayRadio = CreateWindowEx(0, WC_BUTTON, _("Dia do Mês"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        15, 61, 100, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(MDayRadio);

	MDayCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        120, 61, 65, 140, grouper, NULL, Instance, NULL);
    NiceFont(MDayCombobox);

    HWND textLabel7 = CreateWindowEx(0, WC_STATIC, "/",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        183, 63, 13, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel7);

	MonthCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        200, 61, 65, 140, grouper, NULL, Instance, NULL);
    NiceFont(MonthCombobox);

    HWND textLabel8 = CreateWindowEx(0, WC_STATIC, "/",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        263, 63, 13, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel8);

	YearCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        280, 61, 65, 140, grouper, NULL, Instance, NULL);
    NiceFont(YearCombobox);

	//Icone de edição do relogio
	HWND textLabel47 = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        10, 100, 64, 64, RTCDialog, NULL, Instance, NULL);

	HBITMAP hBmp = (HBITMAP) LoadImage(NULL,"imgs/time_config.bmp",IMAGE_BITMAP,0,0, LR_LOADFROMFILE| LR_DEFAULTSIZE);
	SendMessage(textLabel47,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Horário:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        180, 115, 50, 25, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

	// Campo de horas do horario
    HourTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        236, 115, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(HourTextbox);

    HWND textLabel9 = CreateWindowEx(0, WC_STATIC, ":",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        262, 115, 13, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel9);

    // Campo de minutos do horario
	MinTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        279, 115, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(MinTextbox);

    HWND textLabel10 = CreateWindowEx(0, WC_STATIC, ":",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        306, 115, 13, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel10);

	// Campo de segundos do horario
    SecTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        324, 115, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(SecTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        210, 150, 70, 23, RTCDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        285, 150, 70, 23, RTCDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(HourTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

void ShowRTCDialog(unsigned char * wday, unsigned char * mday, unsigned char * month, unsigned int * year, unsigned char * hour, unsigned char * minute, unsigned char * second)
{
	time_t now;
	struct tm *t;
	char buf[10];
	unsigned int y;

	now = time(NULL);
	t = localtime(&now);
	y = t->tm_year + 1900;

    RTCDialog = CreateWindowClient(0, "LDmicroDialog",
        _("RTC"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 363, 180, MainWindow, NULL, Instance, NULL);

    MakeControls();
   
	int i;

	for (i = 0; i < sizeof(MDayItens) / sizeof(MDayItens[0]); i++)
		SendMessage(MDayCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)MDayItens[i]));

	for (i = 0; i < sizeof(MonthItens) / sizeof(MonthItens[0]); i++)
		SendMessage(MonthCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)MonthItens[i]));

	SendMessage(YearCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)_("Todos")));
	for (i = 0; i < 10; i++) {
		sprintf(buf, "%d", y+i);
		SendMessage(YearCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)buf));
	}

	SendMessage(MDayCombobox, CB_SETCURSEL, *mday, 0);
	SendMessage(MonthCombobox, CB_SETCURSEL, *month, 0);
	SendMessage(YearCombobox, CB_SETCURSEL, (*year >= y) ? *year - y + 1 : 0, 0);

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

		if(SendMessage(WDayRadio, BM_GETSTATE, 0, 0) & BST_CHECKED)
		{
			EnableWindow(MDayCombobox, FALSE);
			EnableWindow(MonthCombobox, FALSE);
			EnableWindow(YearCombobox, FALSE);
			EnableWindow(WDayCheckbox0, TRUE);
			EnableWindow(WDayCheckbox1, TRUE);
			EnableWindow(WDayCheckbox2, TRUE);
			EnableWindow(WDayCheckbox3, TRUE);
			EnableWindow(WDayCheckbox4, TRUE);
			EnableWindow(WDayCheckbox5, TRUE);
			EnableWindow(WDayCheckbox6, TRUE);
		}
		else
		{
			EnableWindow(MDayCombobox, TRUE);
			EnableWindow(MonthCombobox, TRUE);
			EnableWindow(YearCombobox, TRUE);
			EnableWindow(WDayCheckbox0, FALSE);
			EnableWindow(WDayCheckbox1, FALSE);
			EnableWindow(WDayCheckbox2, FALSE);
			EnableWindow(WDayCheckbox3, FALSE);
			EnableWindow(WDayCheckbox4, FALSE);
			EnableWindow(WDayCheckbox5, FALSE);
			EnableWindow(WDayCheckbox6, FALSE);
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

		/*
		char cday[10];
		char cmonth[10];
		char cyear[10];

		SendMessage(MDayCombobox, WM_GETTEXT, (WPARAM)sizeof(cday),
			(LPARAM)(cday));

		SendMessage(MonthCombobox, WM_GETTEXT, (WPARAM)sizeof(cmonth),
			(LPARAM)(cmonth));

		SendMessage(YearCombobox, WM_GETTEXT, (WPARAM)sizeof(cyear),
			(LPARAM)(cyear));
		*/

		int y, m ,d;

		d = SendMessage(MDayCombobox , CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		m = SendMessage(MonthCombobox, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

		ComboBox_GetLBText(YearCombobox, ComboBox_GetCurSel(YearCombobox), buf);
		y = atoi(buf);

		if (atoi(chour) > 23)
			Error(_("Hora invalida, deve estar entre 0 e 23."));
		else if (atoi(cminute) > 59)
			Error(_("Minuto invalido, deve estar entre 0 e 59."));
		else if (atoi(csecond) > 59)
			Error(_("Segundo invalido, deve estar entre 0 e 59."));
		else
		{
			bool valid = true;

			if(SendMessage(MDayRadio, BM_GETSTATE, 0, 0) & BST_CHECKED)
			{
				int MaxDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

				/*
				d = atoi(cday);
				m = atoi(cmonth);
				y = atoi(cyear);
				*/
				if(!y || (!(y%4) && (!(y%400) || (y%100)))) // Se qualquer ano ou bissexto (divisivel por 4 e nao for por 100 ou for por 400)
					MaxDays[1]++; // Fevereiro pode ter 29 dias

				if (m && d > MaxDays[m-1])
				{
					Error(_("Data inválida."));
					valid = false;
				}

			}
			
			if (valid)
			{
				/*
				*mday = atoi(cday);
				*month = atoi(cmonth);
				*year = atoi(cyear);
				*/

				*mday = d;
				*month = m;
				*year = y;

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
    }

	EnableWindow(MainWindow, TRUE);
	DestroyWindow(RTCDialog);
	return;
}
