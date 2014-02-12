#include "poptools.h"

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
static HWND StartHourTextbox;
static HWND StartMinTextbox;
static HWND StartSecTextbox;
static HWND StartMDayCombobox;
static HWND StartMonthCombobox;
static HWND StartYearCombobox;
static HWND EndHourTextbox;
static HWND EndMinTextbox;
static HWND EndSecTextbox;
static HWND EndMDayCombobox;
static HWND EndMonthCombobox;
static HWND EndYearCombobox;
static HWND ModeFixedRadio;
static HWND ModeContinuousRadio;
static HWND ModeIntermittentRadio;

static LONG_PTR PrevNameProc;

const LPCTSTR MonthItens[] = { "Todos", "Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho",
								"Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"};

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
	char WeekDays[8], WeekDay[2] = { 0, 0 };

	strcpy(WeekDays, _("DSTQQSS"));

	HWND grouper1 = CreateWindowEx(0, WC_BUTTON, _("Modo"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 3, 380, 45, RTCDialog, NULL, Instance, NULL);
    NiceFont(grouper1);

    ModeFixedRadio = CreateWindowEx(0, WC_BUTTON, _("Data Específica"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        7, 20, 110, 20, grouper1, NULL, Instance, NULL);
    NiceFont(ModeFixedRadio);

    ModeContinuousRadio = CreateWindowEx(0, WC_BUTTON, _("Período Contínuo"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        120, 20, 120, 20, grouper1, NULL, Instance, NULL);
    NiceFont(ModeContinuousRadio);

    ModeIntermittentRadio = CreateWindowEx(0, WC_BUTTON, _("Período Intermitente"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE,
        243, 20, 135, 20, grouper1, NULL, Instance, NULL);
    NiceFont(ModeIntermittentRadio);

	HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Dias"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 48, 380, 125, RTCDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    WDayRadio = CreateWindowEx(0, WC_BUTTON, _("Dia da Semana"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        15, 75, 110, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(WDayRadio);

	WeekDay[0] = WeekDays[0];
    HWND textLabel0 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        90, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel0);

	WeekDay[0] = WeekDays[1];
    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        130, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel1);

	WeekDay[0] = WeekDays[2];
    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        170, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel2);

	WeekDay[0] = WeekDays[3];
    HWND textLabel3 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        210, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel3);

	WeekDay[0] = WeekDays[4];
    HWND textLabel4 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        250, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel4);

	WeekDay[0] = WeekDays[5];
    HWND textLabel5 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        290, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel5);

	WeekDay[0] = WeekDays[6];
    HWND textLabel6 = CreateWindowEx(0, WC_STATIC, WeekDay,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        330, 21, 40, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel6);

    WDayCheckbox0 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        120, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox0);

    WDayCheckbox1 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        160, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox1);

    WDayCheckbox2 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        200, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox2);

    WDayCheckbox3 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        240, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox3);

    WDayCheckbox4 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        280, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox4);

    WDayCheckbox5 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        320, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox5);

    WDayCheckbox6 = CreateWindowEx(0, WC_BUTTON, "",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        360, 35, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(WDayCheckbox6);

    MDayRadio = CreateWindowEx(0, WC_BUTTON, _("Dia do Mês"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE,
        15, 121, 80, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(MDayRadio);

	StartMDayCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        120, 61, 65, 140, grouper, NULL, Instance, NULL);
    NiceFont(StartMDayCombobox);

    HWND textLabel7 = CreateWindowEx(0, WC_STATIC, "/",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        183, 63, 13, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel7);

	StartMonthCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        200, 61, 95, 140, grouper, NULL, Instance, NULL);
    NiceFont(StartMonthCombobox);

    HWND textLabel8 = CreateWindowEx(0, WC_STATIC, "/",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        293, 63, 13, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel8);

	StartYearCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        310, 61, 65, 140, grouper, NULL, Instance, NULL);
    NiceFont(StartYearCombobox);

	EndMDayCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        120, 91, 65, 140, grouper, NULL, Instance, NULL);
    NiceFont(EndMDayCombobox);

    HWND textLabel = CreateWindowEx(0, WC_STATIC, "/",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        183, 93, 13, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

	EndMonthCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        200, 91, 95, 140, grouper, NULL, Instance, NULL);
    NiceFont(EndMonthCombobox);

    textLabel = CreateWindowEx(0, WC_STATIC, "/",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        293, 93, 13, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

	EndYearCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        310, 91, 65, 140, grouper, NULL, Instance, NULL);
    NiceFont(EndYearCombobox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("De"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        95, 63, 20, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Até"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        95, 93, 20, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

	//Icone de edição do relogio
	HWND textLabel47 = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        60, 180, 64, 64, RTCDialog, NULL, Instance, NULL);

	HBITMAP hBmp = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(IDB_TIME_CONFIG),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(textLabel47,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Horário Inicial:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        110, 180, 150, 25, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

	// Campo de horas do horario
    StartHourTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        266, 180, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(StartHourTextbox);

    HWND textLabel9 = CreateWindowEx(0, WC_STATIC, ":",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        292, 180, 13, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel9);

    // Campo de minutos do horario
	StartMinTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        309, 180, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(StartMinTextbox);

    HWND textLabel10 = CreateWindowEx(0, WC_STATIC, ":",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        336, 180, 13, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel10);

	// Campo de segundos do horario
    StartSecTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        354, 180, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(StartSecTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Horário Final:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        110, 210, 150, 25, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

	// Campo de horas do horario
    EndHourTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        266, 210, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(EndHourTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, ":",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        292, 210, 13, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    // Campo de minutos do horario
	EndMinTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        309, 210, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(EndMinTextbox);

    textLabel = CreateWindowEx(0, WC_STATIC, ":",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        336, 210, 13, 20, RTCDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

	// Campo de segundos do horario
    EndSecTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        354, 210, 30, 20, RTCDialog, NULL, Instance, NULL);
    FixedFont(EndSecTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        240, 235, 70, 23, RTCDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        315, 235, 70, 23, RTCDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevNameProc = SetWindowLongPtr(StartHourTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyNameProc);
}

bool ShowRTCDialog(int *mode, unsigned char *wday, struct tm *start, struct tm *end,
	POINT ElemStart, POINT ElemSize, POINT GridSize)
{
	bool changed = false;

	time_t now;
	struct tm *t;
	char buf[1024];
	int y;

	POINT startPoint = { 100, 100 }, size = { 393, 265 };

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
			startPoint.y = ElemStart.y - (size.y + offset);
		} else { // Senao posiciona abaixo
			startPoint.y = ElemStart.y + ElemSize.y + offset;
		}

		startPoint.x = ElemStart.x + (ElemSize.x - size.x)/2;
		if(startPoint.x < 0) {
			startPoint.x = 0;
		} else if(startPoint.x + size.x > rWindow.right) {
			startPoint.x = rWindow.right - size.x;
		}
	}

	now = time(NULL);
	t = localtime(&now);
	y = t->tm_year + 1900;

    RTCDialog = CreateWindowClient(0, "POPToolsDialog",
        _("Agendador de Tarefa"), WS_OVERLAPPED | WS_SYSMENU,
        startPoint.x, startPoint.y, size.x, size.y, MainWindow, NULL, Instance, NULL);

    MakeControls();
   
	int i;

	for (i = 0; i <= 31; i++) {
		if(i == 0) {
			strcpy(buf, _("Todos"));
		} else {
			sprintf(buf, "%d", i);
		}

		SendMessage(StartMDayCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)buf));
		SendMessage(EndMDayCombobox  , CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)buf));
	}

	for (i = 0; i < sizeof(MonthItens) / sizeof(MonthItens[0]); i++) {
		SendMessage(StartMonthCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)_(MonthItens[i])));
		SendMessage(EndMonthCombobox  , CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)_(MonthItens[i])));
	}

	SendMessage(StartYearCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)_("Todos")));
	SendMessage(EndYearCombobox  , CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)_("Todos")));
	for (i = 0; i < 10; i++) {
		sprintf(buf, "%d", y+i);
		SendMessage(StartYearCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)buf));
		SendMessage(EndYearCombobox  , CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)buf));
	}

	SendMessage(StartMDayCombobox, CB_SETCURSEL, start->tm_mday, 0);
	SendMessage(StartMonthCombobox, CB_SETCURSEL, start->tm_mon, 0);
	SendMessage(StartYearCombobox, CB_SETCURSEL, (start->tm_year >= y) ? start->tm_year - y + 1 : 0, 0);

	SendMessage(EndMDayCombobox, CB_SETCURSEL, end->tm_mday, 0);
	SendMessage(EndMonthCombobox, CB_SETCURSEL, end->tm_mon, 0);
	SendMessage(EndYearCombobox, CB_SETCURSEL, (end->tm_year >= y) ? end->tm_year - y + 1 : 0, 0);

    switch(*mode) {
	case ELEM_RTC_MODE_CONTINUOUS:
        SendMessage(ModeContinuousRadio, BM_SETCHECK, BST_CHECKED, 0);
		break;

	case ELEM_RTC_MODE_INTERMITTENT:
        SendMessage(ModeIntermittentRadio, BM_SETCHECK, BST_CHECKED, 0);
		break;

	default:
	case ELEM_RTC_MODE_FIXED:
        SendMessage(ModeFixedRadio, BM_SETCHECK, BST_CHECKED, 0);
		break;
	}

	if (*wday & (1 << 7))
        SendMessage(WDayRadio, BM_SETCHECK, BST_CHECKED, 0);
	else
        SendMessage(MDayRadio, BM_SETCHECK, BST_CHECKED, 0);

	char cstarthour[10], cstartminute[10], cstartsecond[10];
	char cendhour  [10], cendminute  [10], cendsecond  [10];

	_itoa(start->tm_hour, cstarthour, 10);
	_itoa(start->tm_min, cstartminute, 10);
	_itoa(start->tm_sec, cstartsecond, 10);

    SendMessage(StartHourTextbox, WM_SETTEXT, 0, (LPARAM)(cstarthour));
	SendMessage(StartMinTextbox, WM_SETTEXT, 0, (LPARAM)(cstartminute));
	SendMessage(StartSecTextbox, WM_SETTEXT, 0, (LPARAM)(cstartsecond));

	_itoa(end->tm_hour, cendhour, 10);
	_itoa(end->tm_min, cendminute, 10);
	_itoa(end->tm_sec, cendsecond, 10);

    SendMessage(EndHourTextbox, WM_SETTEXT, 0, (LPARAM)(cendhour));
	SendMessage(EndMinTextbox, WM_SETTEXT, 0, (LPARAM)(cendminute));
	SendMessage(EndSecTextbox, WM_SETTEXT, 0, (LPARAM)(cendsecond));

	if (*wday & 1) SendMessage(WDayCheckbox0, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 2) SendMessage(WDayCheckbox1, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 4) SendMessage(WDayCheckbox2, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 8) SendMessage(WDayCheckbox3, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 16) SendMessage(WDayCheckbox4, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 32) SendMessage(WDayCheckbox5, BM_SETCHECK, BST_CHECKED, 0);
	if (*wday & 64) SendMessage(WDayCheckbox6, BM_SETCHECK, BST_CHECKED, 0);
	
    EnableWindow(MainWindow, FALSE);
    ShowWindow(RTCDialog, TRUE);
    SetFocus(StartHourTextbox);
    SendMessage(StartHourTextbox, EM_SETSEL, 0, -1);

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

		if(SendMessage(ModeFixedRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			EnableWindow(EndHourTextbox, FALSE);
			EnableWindow(EndMinTextbox, FALSE);
			EnableWindow(EndSecTextbox, FALSE);
		} else {
			EnableWindow(EndHourTextbox, TRUE);
			EnableWindow(EndMinTextbox, TRUE);
			EnableWindow(EndSecTextbox, TRUE);
		}

		if(SendMessage(WDayRadio, BM_GETSTATE, 0, 0) & BST_CHECKED)
		{
			EnableWindow(StartMDayCombobox, FALSE);
			EnableWindow(StartMonthCombobox, FALSE);
			EnableWindow(StartYearCombobox, FALSE);
			EnableWindow(EndMDayCombobox, FALSE);
			EnableWindow(EndMonthCombobox, FALSE);
			EnableWindow(EndYearCombobox, FALSE);
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
			EnableWindow(StartMDayCombobox, TRUE);
			EnableWindow(StartMonthCombobox, TRUE);
			EnableWindow(StartYearCombobox, TRUE);
			EnableWindow(WDayCheckbox0, FALSE);
			EnableWindow(WDayCheckbox1, FALSE);
			EnableWindow(WDayCheckbox2, FALSE);
			EnableWindow(WDayCheckbox3, FALSE);
			EnableWindow(WDayCheckbox4, FALSE);
			EnableWindow(WDayCheckbox5, FALSE);
			EnableWindow(WDayCheckbox6, FALSE);

			if(SendMessage(ModeFixedRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
				EnableWindow(EndMDayCombobox, FALSE);
				EnableWindow(EndMonthCombobox, FALSE);
				EnableWindow(EndYearCombobox, FALSE);
			} else {
				EnableWindow(EndMDayCombobox, TRUE);
				EnableWindow(EndMonthCombobox, TRUE);
				EnableWindow(EndYearCombobox, TRUE);
			}
		}

        if(IsDialogMessage(RTCDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	if(!DialogCancel)
	{
		int sy, sm, sd, sh, smin, ss, ey, em, ed, eh, emin, es, m;

        SendMessage(StartHourTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(cstarthour));
        SendMessage(StartMinTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(cstartminute));
        SendMessage(StartSecTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(cstartsecond));
        SendMessage(EndHourTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(cendhour));
        SendMessage(EndMinTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(cendminute));
        SendMessage(EndSecTextbox, WM_GETTEXT, (WPARAM)3, (LPARAM)(cendsecond));

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

		sd = SendMessage(StartMDayCombobox , CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		sm = SendMessage(StartMonthCombobox, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

		ComboBox_GetLBText(StartYearCombobox, ComboBox_GetCurSel(StartYearCombobox), buf);
		sy = atoi(buf);

		ed = SendMessage(EndMDayCombobox , CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		em = SendMessage(EndMonthCombobox, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

		ComboBox_GetLBText(EndYearCombobox, ComboBox_GetCurSel(EndYearCombobox), buf);
		ey = atoi(buf);

		if (atoi(cstarthour) > 23 || atoi(cendhour) > 23)
			Error(_("Hora invalida, deve estar entre 0 e 23."));
		else if (atoi(cstartminute) > 59 || atoi(cendminute) > 59)
			Error(_("Minuto invalido, deve estar entre 0 e 59."));
		else if (atoi(cstartsecond) > 59 || atoi(cendsecond) > 59)
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
				if(!sy || (!(sy%4) && (!(sy%400) || (sy%100)))) // Se qualquer ano ou bissexto (divisivel por 4 e nao for por 100 ou for por 400)
					MaxDays[1] = 29; // Fevereiro pode ter 29 dias

				if (sm && sd > MaxDays[sm-1])
				{
					Error(_("Data inicial inválida."));
					valid = false;
				}

				if(valid) {
					MaxDays[1] = 28; // Fevereiro tem 28 dias
					if(!ey || (!(ey%4) && (!(ey%400) || (ey%100)))) // Se qualquer ano ou bissexto (divisivel por 4 e nao for por 100 ou for por 400)
						MaxDays[1] = 29; // Fevereiro pode ter 29 dias

					if (em && ed > MaxDays[em-1])
					{
						Error(_("Data final inválida."));
						valid = false;
					}
				}

			}
			
			if (valid) {
				if(SendMessage(ModeIntermittentRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
					m = ELEM_RTC_MODE_INTERMITTENT;
				} else if(SendMessage(ModeContinuousRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
					m = ELEM_RTC_MODE_CONTINUOUS;
				} else {
					m = ELEM_RTC_MODE_FIXED;
				}

				sh = atoi(cstarthour);
				smin = atoi(cstartminute);
				ss = atoi(cstartsecond);

				eh = atoi(cendhour);
				emin = atoi(cendminute);
				es = atoi(cendsecond);

				if(m != ELEM_RTC_MODE_FIXED) { // Using date/time range, End should be at least same as Start.
					if(ey && ey < sy) { // End year lower than start year
						valid = false;
					} else if(ey && sy && ey == sy) { // Same year
						if(em && em < sm) { // End month lower than start month
							valid = false;
						} else if (em && sm && em == sm) { // Same month
							if(ed && ed < sd) { // End day lower than start day
								valid = false;
							} else if(ed && sd && ed == sd) { // Same day
								if(eh < sh) { // End hour lower than start hour
									valid = false;
								} else if(eh == sh) { // Same hour
									if(emin < smin) { // End minute lower than start minute
										valid = false;
									} else if(emin == smin) { // Same minute
										if(es <= ss) { // End second lower or equal than start second
											valid = false;
										}
									}
								}
							}
						}
					}
				}

				if(!valid) {
					Error(_("Data final deve ser maior que a data inicial!"));
				}
			}

			if (valid)
			{
				changed = true;

				*mode = m;

				/*
				*mday = atoi(cday);
				*month = atoi(cmonth);
				*year = atoi(cyear);
				*/

				start->tm_mday = sd;
				start->tm_mon = sm;
				start->tm_year = sy;

				end->tm_mday = ed;
				end->tm_mon = em;
				end->tm_year = ey;

				start->tm_hour = sh;
				start->tm_min = smin;
				start->tm_sec = ss;

				end->tm_hour = eh;
				end->tm_min = emin;
				end->tm_sec = es;

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
	SetFocus(MainWindow);

	return changed;
}
