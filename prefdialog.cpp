#include "poptools.h"

static HWND PrefDialog;

static HWND ShowWarningsYes;
static HWND ShowWarningsNo;
static HWND ClearRecentList;
static HWND LanguageCombobox;
static HWND ComPortFlashCombobox;
static HWND ComPortDebugCombobox;
static HWND AutoSaveIntervalCombobox;

static LONG_PTR PrevPrefDialogProc;

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK PrefDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_COMMAND: {
            HWND h = (HWND)lParam;
			if(wParam == BN_CLICKED && h == ClearRecentList) {
				memset(POPSettings.recent_list, 0, sizeof(POPSettings.recent_list));
				break;
			} else if(h == ComPortFlashCombobox && HIWORD(wParam) == CBN_DROPDOWN) {
				LoadCOMPorts(ComPortFlashCombobox, POPSettings.COMPortFlash, true);
			} else if(h == ComPortDebugCombobox && HIWORD(wParam) == CBN_DROPDOWN) {
				LoadCOMPorts(ComPortDebugCombobox, POPSettings.COMPortDebug, false);
			}
		}
	}

	return CallWindowProc((WNDPROC)PrevPrefDialogProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND textlabel = CreateWindowEx(0, WC_STATIC, _("Idioma atual:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 25, 110, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel);

	LanguageCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        135, 23, 135, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(LanguageCombobox);

    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Idioma da Interface"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        10, 5, 270, 50, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    textlabel = CreateWindowEx(0, WC_STATIC, _("Ao cancelar:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 75, 110, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel);

    ShowWarningsYes = CreateWindowEx(0, WC_BUTTON, _("Sim"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        145, 75, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowWarningsYes);

    ShowWarningsNo = CreateWindowEx(0, WC_BUTTON, _("Não"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        210, 75, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowWarningsNo);

    grouper = CreateWindowEx(0, WC_BUTTON, _("Avisos durante a simulação"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        10, 55, 270, 50, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    HWND textlabel4 = CreateWindowEx(0, WC_STATIC, _("Intervalo:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 125, 110, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel4);

	AutoSaveIntervalCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        135, 123, 135, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(AutoSaveIntervalCombobox);

    HWND grouper2 = CreateWindowEx(0, WC_BUTTON, _("Cópia de Segurança"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        10, 105, 270, 50, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper2);

    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, _("Porta de Gravação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 175, 120, 21, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel1);

	ComPortFlashCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        135, 173, 135, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(ComPortFlashCombobox);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Porta de Depuração:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 205, 120, 21, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	ComPortDebugCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        135, 203, 135, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(ComPortDebugCombobox);

    grouper = CreateWindowEx(0, WC_BUTTON, _("Portas Seriais"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        10, 155, 270, 80, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    HWND textLabel3 = CreateWindowEx(0, WC_STATIC, _("Projetos Recentes:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        10, 245, 120, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel3);

	ClearRecentList = CreateWindowEx(0, WC_BUTTON, _("Limpar!"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
        135, 240, 145, 30, PrefDialog, NULL, Instance, NULL); 
    NiceFont(ClearRecentList);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        135, 280, 70, 30, PrefDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        210, 280, 70, 30, PrefDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

void ShowPrefDialog(void)
{
	int i;
	struct {
		int   interval;
		char *text;
	} AutoSaveItems[] = {
		{  0, "Desativado" },
		{  1, "1 Minuto"   },
		{  5, "5 Minutos"  },
		{ 10, "10 Minutos" },
		{ 15, "15 Minutos" },
		{ 30, "30 Minutos" },
		{ 60, "1 Hora"     },
		{  0, NULL         }
	};

	RECT rDrawWindow;
	GetWindowRect(DrawWindow, &rDrawWindow);

	POINT size = { 290, 320 };
	POINT start = {
		rDrawWindow.left + (rDrawWindow.right  - rDrawWindow.left - size.x)/2,
		rDrawWindow.top  + (rDrawWindow.bottom - rDrawWindow.top  - size.y)/2,
	};

	PrefDialog = CreateWindowClient(0, "POPToolsDialog",
        _("Preferências"), WS_OVERLAPPED | WS_SYSMENU,
		start.x, start.y, size.x, size.y, MainWindow, NULL, Instance, NULL);

	PrevPrefDialogProc = SetWindowLongPtr(PrefDialog, GWLP_WNDPROC, (LONG_PTR)PrefDialogProc);

    MakeControls();

	LoadCOMPorts(ComPortFlashCombobox, POPSettings.COMPortFlash, true);
	SendMessage (ComPortFlashCombobox, CB_SETDROPPEDWIDTH, 300, 0);

	LoadCOMPorts(ComPortDebugCombobox, POPSettings.COMPortDebug, false);
	SendMessage (ComPortDebugCombobox, CB_SETDROPPEDWIDTH, 300, 0);

	// Carrega as strings de idiomas suportados
	SendMessage(LanguageCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)"Português"));
	SendMessage(LanguageCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)"English"));
	SendMessage(LanguageCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)"Español"));
	SendMessage(LanguageCombobox, CB_SETCURSEL, POPSettings.idLanguage, 0);

	// Load Combobox items
	for (i = 0; AutoSaveItems[i].text; i++)
		SendMessage(AutoSaveIntervalCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)_(AutoSaveItems[i].text)));
	// Search for item to be selected
	for (i = 0; AutoSaveItems[i].text; i++) {
		if(AutoSaveItems[i].interval >= POPSettings.AutoSaveInterval)
			break;
	}
	// If not found, choose the last item
	if(AutoSaveItems[i].text == NULL)
		i--;

	// Select item
	SendMessage(AutoSaveIntervalCombobox, CB_SETCURSEL, i, 0);

	if(POPSettings.ShowSimulationWarnings) {
        SendMessage(ShowWarningsYes, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(ShowWarningsNo , BM_SETCHECK, BST_CHECKED, 0);
	}

    EnableWindow(MainWindow, FALSE);
    ShowWindow(PrefDialog, TRUE);
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

        if(IsDialogMessage(PrefDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
		char buf[16];
        int index = SendMessage(AutoSaveIntervalCombobox, CB_GETCURSEL, 0, 0);
		POPSettings.AutoSaveInterval = AutoSaveItems[index > 0 ? index : 0].interval;
		SetAutoSaveInterval(POPSettings.AutoSaveInterval);

		SendMessage(ComPortFlashCombobox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		POPSettings.COMPortFlash = atoi(&buf[3]);

        SendMessage(ComPortDebugCombobox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		POPSettings.COMPortDebug = atoi(&buf[3]);

		if(SendMessage(ShowWarningsYes, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			POPSettings.ShowSimulationWarnings = TRUE;
		} else {
			POPSettings.ShowSimulationWarnings = FALSE;
	    }

		// Configura o idioma e avisa para reiniciar o programa
		index = SendMessage(LanguageCombobox, CB_GETCURSEL, 0, 0);
		if(index != POPSettings.idLanguage) {
			POPSettings.idLanguage = index;
			Warning(_("Idioma alterado"),
				_("O idioma da interface foi alterado.\nFavor reiniciar o programa para que as mudanças tenham efeito"));
		}
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(PrefDialog);
	SetFocus(MainWindow);
    return;
}
