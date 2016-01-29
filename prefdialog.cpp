#include "poptools.h"

static HWND PrefDialog;

static HWND ShowWarningsYes;
static HWND ShowWarningsNo;
static HWND GenMemoryMapYes;
static HWND GenMemoryMapNo;
static HWND ShowCompileWarningYes;
static HWND ShowCompileWarningNo;
static HWND Show3DLadderYes;
static HWND Show3DLadderNo;
static HWND ClearRecentList;
static HWND LanguageCombobox;
static HWND ComPortFlashCombobox;
static HWND ComPortDebugCombobox;
static HWND AutoSaveIntervalCombobox;

static LONG_PTR PrevPrefDialogProc;

#define GROUP_LEFT		10
#define GROUP_WIDTH		390
#define GROUP_HEIGHT	70
#define LABEL_LEFT		30
#define LABEL_WIDTH		180
#define FIELD_LEFT		215
#define FIELD_WIDTH		170
#define PADDING_TOP		10
#define MARGIN_TOP		30

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
	// Opção de Idioma
    HWND textlabel = CreateWindowEx(0, WC_STATIC, _("Idioma atual:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, PADDING_TOP + MARGIN_TOP, LABEL_WIDTH, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel);

	LanguageCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        FIELD_LEFT, PADDING_TOP + MARGIN_TOP, FIELD_WIDTH, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(LanguageCombobox);

    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Idioma da Interface"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | SS_LEFT,
        GROUP_LEFT, PADDING_TOP, GROUP_WIDTH, GROUP_HEIGHT, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

	// Opção de Simulação
    textlabel = CreateWindowEx(0, WC_STATIC, _("Ao cancelar:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, GROUP_HEIGHT + (PADDING_TOP * 2) + MARGIN_TOP, LABEL_WIDTH, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel);

    ShowWarningsYes = CreateWindowEx(0, WC_BUTTON, _("Sim"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        FIELD_LEFT, GROUP_HEIGHT + (PADDING_TOP * 2) + MARGIN_TOP, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowWarningsYes);

    ShowWarningsNo = CreateWindowEx(0, WC_BUTTON, _("Não"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        FIELD_LEFT + 70, GROUP_HEIGHT + (PADDING_TOP * 2) + MARGIN_TOP, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowWarningsNo);

    grouper = CreateWindowEx(0, WC_BUTTON, _("Avisos durante a simulação"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        GROUP_LEFT, GROUP_HEIGHT + (PADDING_TOP * 2), GROUP_WIDTH, GROUP_HEIGHT, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

	// Ladder 3D
    textlabel = CreateWindowEx(0, WC_STATIC, _("Exibir Ladder 3D:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, (GROUP_HEIGHT * 2) + (PADDING_TOP * 3) + MARGIN_TOP, LABEL_WIDTH, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel);

    Show3DLadderYes = CreateWindowEx(0, WC_BUTTON, _("Sim"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        FIELD_LEFT, (GROUP_HEIGHT * 2) + (PADDING_TOP * 3) + MARGIN_TOP, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(Show3DLadderYes);

    Show3DLadderNo = CreateWindowEx(0, WC_BUTTON, _("Não"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        FIELD_LEFT + 70, (GROUP_HEIGHT * 2) + (PADDING_TOP * 3) + MARGIN_TOP, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(Show3DLadderNo);

    grouper = CreateWindowEx(0, WC_BUTTON, _("Aparência do Ladder"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        GROUP_LEFT, (GROUP_HEIGHT * 2) + (PADDING_TOP * 3), GROUP_WIDTH, GROUP_HEIGHT, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

	// Mapa de memoria
    textlabel = CreateWindowEx(0, WC_STATIC, _("Gerar mapa de memória:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, (GROUP_HEIGHT * 3) + (PADDING_TOP * 3) + MARGIN_TOP, LABEL_WIDTH, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel);

    GenMemoryMapYes = CreateWindowEx(0, WC_BUTTON, _("Sim"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        FIELD_LEFT, (GROUP_HEIGHT * 3) + (PADDING_TOP * 3) + MARGIN_TOP, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(GenMemoryMapYes);

    GenMemoryMapNo = CreateWindowEx(0, WC_BUTTON, _("Não"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
		FIELD_LEFT + 70, (GROUP_HEIGHT * 3) + (PADDING_TOP * 3) + MARGIN_TOP, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(GenMemoryMapNo);

    textlabel = CreateWindowEx(0, WC_STATIC, _("Exibir avisos:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, (GROUP_HEIGHT * 3) + (PADDING_TOP * 5) + MARGIN_TOP, LABEL_WIDTH, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel);

    ShowCompileWarningYes = CreateWindowEx(0, WC_BUTTON, _("Sim"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        FIELD_LEFT, (GROUP_HEIGHT * 3) + (PADDING_TOP * 5) + MARGIN_TOP, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowCompileWarningYes);

    ShowCompileWarningNo = CreateWindowEx(0, WC_BUTTON, _("Não"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        FIELD_LEFT + 70, (GROUP_HEIGHT * 3) + (PADDING_TOP * 5) + MARGIN_TOP, 40, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(ShowCompileWarningNo);

    grouper = CreateWindowEx(0, WC_BUTTON, _("Compilação"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        GROUP_LEFT, (GROUP_HEIGHT * 3) + (PADDING_TOP * 4), GROUP_WIDTH, GROUP_HEIGHT, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

	// Intervalo do backup
    HWND textlabel4 = CreateWindowEx(0, WC_STATIC, _("Intervalo:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, (GROUP_HEIGHT * 4) + (PADDING_TOP * 5) + MARGIN_TOP, LABEL_WIDTH, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textlabel4);

	AutoSaveIntervalCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        FIELD_LEFT, (GROUP_HEIGHT * 4) + (PADDING_TOP * 5) + MARGIN_TOP, FIELD_WIDTH, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(AutoSaveIntervalCombobox);

    HWND grouper2 = CreateWindowEx(0, WC_BUTTON, _("Cópia de Segurança"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        GROUP_LEFT, (GROUP_HEIGHT * 4) + (PADDING_TOP * 5), GROUP_WIDTH, GROUP_HEIGHT, PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper2);

	// Portas Seriais
    HWND textLabel1 = CreateWindowEx(0, WC_STATIC, _("Porta de Gravação:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, (GROUP_HEIGHT * 5) + (PADDING_TOP * 5) + MARGIN_TOP, LABEL_WIDTH, 21, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel1);

	ComPortFlashCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        FIELD_LEFT, (GROUP_HEIGHT * 5) + (PADDING_TOP * 5) + MARGIN_TOP, FIELD_WIDTH, FIELD_WIDTH, PrefDialog, NULL, Instance, NULL);
    NiceFont(ComPortFlashCombobox);

	// Porta de Depuração
    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Porta de Depuração:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, (GROUP_HEIGHT * 5) + (PADDING_TOP * 6) + MARGIN_TOP + MARGIN_TOP, LABEL_WIDTH, 21, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	ComPortDebugCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        FIELD_LEFT, (GROUP_HEIGHT * 5) + (PADDING_TOP * 6) + MARGIN_TOP + MARGIN_TOP, FIELD_WIDTH, 100, PrefDialog, NULL, Instance, NULL);
    NiceFont(ComPortDebugCombobox);

    grouper = CreateWindowEx(0, WC_BUTTON, _("Portas Seriais"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        GROUP_LEFT, (GROUP_HEIGHT * 5) + (PADDING_TOP * 6), GROUP_WIDTH, (int)(GROUP_HEIGHT * 1.5), PrefDialog, NULL, Instance, NULL);
    NiceFont(grouper);

	// Projetos Recentes
	ClearRecentList = CreateWindowEx(0, WC_BUTTON, _("Limpar!"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
        FIELD_LEFT, (GROUP_HEIGHT * 6) + (PADDING_TOP * 7) + MARGIN_TOP, FIELD_WIDTH, 30, PrefDialog, NULL, Instance, NULL); 
    NiceFont(ClearRecentList);

    HWND textLabel3 = CreateWindowEx(0, WC_STATIC, _("Projetos Recentes:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
        LABEL_LEFT, (GROUP_HEIGHT * 6) + (PADDING_TOP * 7) + MARGIN_TOP, LABEL_WIDTH, 20, PrefDialog, NULL, Instance, NULL);
    NiceFont(textLabel3);

	// Botoes Ok e Cancelar
    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        FIELD_LEFT, (GROUP_HEIGHT * 7) + (PADDING_TOP * 8), 70, 30, PrefDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        FIELD_LEFT + FIELD_WIDTH - 70, (GROUP_HEIGHT * 7) + (PADDING_TOP * 8), 70, 30, PrefDialog, NULL, Instance, NULL); 
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

	POINT size = { 410, 610 };
	POINT start = getWindowStart(size);

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

	if(POPSettings.GenerateMemoryMap) {
		SendMessage(GenMemoryMapYes, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(GenMemoryMapNo , BM_SETCHECK, BST_CHECKED, 0);
	}

	if(POPSettings.ShowCompileWarnings) {
		SendMessage(ShowCompileWarningYes, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(ShowCompileWarningNo , BM_SETCHECK, BST_CHECKED, 0);
	}

	if(POPSettings.Show3DLadder) {
		SendMessage(Show3DLadderYes, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(Show3DLadderNo , BM_SETCHECK, BST_CHECKED, 0);
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

		if(SendMessage(GenMemoryMapYes, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			POPSettings.GenerateMemoryMap = TRUE;
		} else {
			POPSettings.GenerateMemoryMap = FALSE;
	    }

		if(SendMessage(ShowCompileWarningYes, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			POPSettings.ShowCompileWarnings = TRUE;
		} else {
			POPSettings.ShowCompileWarnings = FALSE;
	    }

		if(SendMessage(Show3DLadderYes, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			POPSettings.Show3DLadder = TRUE;
		} else {
			POPSettings.Show3DLadder = FALSE;
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
