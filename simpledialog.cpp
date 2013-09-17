#include "poptools.h"

static HWND SimpleDialog;

#define MAX_BOXES 5

static HWND Textboxes[MAX_BOXES];
static HWND Labels[MAX_BOXES];

static LONG_PTR PrevAlnumOnlyProc[MAX_BOXES];
static LONG_PTR PrevNumOnlyProc[MAX_BOXES];

static BOOL NoCheckingOnBox[MAX_BOXES];

//-----------------------------------------------------------------------------
// Don't allow any characters other than -A-Za-z0-9_ in the box.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MyAlnumOnlyProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isalpha(wParam) || isdigit(wParam) || wParam == '_' ||
            wParam == '\b' || wParam == '-' || wParam == '\''))
        {
            return 0;
        }
    }

    int i;
    for(i = 0; i < MAX_BOXES; i++) {
        if(hwnd == Textboxes[i]) {
            return CallWindowProc((WNDPROC)PrevAlnumOnlyProc[i], hwnd, msg, 
                wParam, lParam);
        }
    }
    oops();
}

//-----------------------------------------------------------------------------
// Don't allow any characters other than -0-9. in the box.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MyNumOnlyProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isdigit(wParam) || wParam == '.' || wParam == '\b' 
            || wParam == '-' || iscontrol(wParam)))
        {
            return 0;
        }
    }

    int i;
    for(i = 0; i < MAX_BOXES; i++) {
        if(hwnd == Textboxes[i]) {
            return CallWindowProc((WNDPROC)PrevNumOnlyProc[i], hwnd, msg, 
                wParam, lParam);
        }
    }
    oops();
}

static void MakeControls(int boxes, char **labels, DWORD fixedFontMask,
	DWORD useComboBox, vector<eType> types)
{
    int i;
    HDC hdc = GetDC(SimpleDialog);
    SelectObject(hdc, MyNiceFont);

    SIZE si;

    int maxLen = 0;
    for(i = 0; i < boxes; i++) {
        GetTextExtentPoint32(hdc, labels[i], strlen(labels[i]), &si);
        if(si.cx > maxLen) maxLen = si.cx;
    }

    int adj;
    if(maxLen > 70) {
        adj = maxLen - 70;
    } else {
        adj = 0;
    }

    for(i = 0; i < boxes; i++) {
        GetTextExtentPoint32(hdc, labels[i], strlen(labels[i]), &si);

        Labels[i] = CreateWindowEx(0, WC_STATIC, labels[i],
            WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
            (80 + adj) - si.cx - 4, 13 + i*30, si.cx, 21,
            SimpleDialog, NULL, Instance, NULL);
        NiceFont(Labels[i]);

		if(useComboBox & (1 << i)) {
	        Textboxes[i] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
		        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS |
			    CBS_SORT | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL,
				80 + adj, 12 + 30*i, 120 - adj, 321,
				SimpleDialog, NULL, Instance, NULL);

			LoadIOListToComboBox(Textboxes[i], types);
			SendMessage(Textboxes[i], CB_SETDROPPEDWIDTH, 300, 0);
		} else {
	        Textboxes[i] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
		        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS |
			    WS_VISIBLE,
				80 + adj, 12 + 30*i, 120 - adj, 21,
				SimpleDialog, NULL, Instance, NULL);
		}

        if(fixedFontMask & (1 << i)) {
            FixedFont(Textboxes[i]);
        } else {
            NiceFont(Textboxes[i]);
        }
    }
    ReleaseDC(SimpleDialog, hdc);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        218, 11, 70, 23, SimpleDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        218, 41, 70, 23, SimpleDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

// Default para types: Vetor vazio, todos os tipos...
BOOL ShowSimpleDialog(char *title, int boxes, char **labels, DWORD numOnlyMask,
    DWORD alnumOnlyMask, DWORD fixedFontMask, DWORD useComboBox, char **dests,
	POINT ElemStart, POINT ElemSize, POINT GridSize, vector<eType> types = vector<eType>())
{
    BOOL didCancel;
	POINT start = { 100, 100 }, size = { 304, 15 + 30*(boxes < 2 ? 2 : boxes) };

    if(boxes > MAX_BOXES) oops();

	// Se tamanho for definido, devemos posicionar a janela
	if(ElemSize.x > 0 && ElemSize.y > 0) {
		int offset = 50; // espacamento entre a janela e a borda do elemento

		// Primeiro corrige as coordenadas para representar um valorabsoluto com relacao
		// ao canto superior esquerdo da tela ao inves do canto de DrawWindow
		RECT rWindow;

		GetWindowRect(DrawWindow, &rWindow);

		ElemStart.x += rWindow.left - ScrollXOffset;
		ElemStart.y += rWindow.top  - ScrollYOffset * GridSize.y;

		// Primeiro tenta posicionar sobre o elemento
		if(ElemStart.y > (size.y + offset)) {
			start.y = ElemStart.y - (size.y + offset);
		} else { // Senao posiciona abaixo
			start.y = ElemStart.y + ElemSize.y + offset;
		}

		start.x = ElemStart.x + (ElemSize.x - size.x)/2;
		if(start.x < 0) {
			start.x = 0;
		} else if(start.x + size.x > rWindow.right) {
			start.x = rWindow.right - size.x;
		}
	}

    SimpleDialog = CreateWindowClient(0, "POPToolsDialog", title, 
        WS_OVERLAPPED | WS_SYSMENU,
        start.x, start.y, size.x, size.y , MainWindow, NULL,
        Instance, NULL);

    MakeControls(boxes, labels, fixedFontMask, useComboBox, types);
  
    int i;

    for(i = 0; i < boxes; i++) {
        SendMessage(Textboxes[i], WM_SETTEXT, (WPARAM)17, (LPARAM)dests[i]);

        if(numOnlyMask & (1 << i)) {
            PrevNumOnlyProc[i] = SetWindowLongPtr(Textboxes[i], GWLP_WNDPROC, 
                (LONG_PTR)MyNumOnlyProc);
        }
        if(alnumOnlyMask & (1 << i)) {
            PrevAlnumOnlyProc[i] = SetWindowLongPtr(Textboxes[i], GWLP_WNDPROC, 
                (LONG_PTR)MyAlnumOnlyProc);
        }
    }

    EnableWindow(MainWindow, FALSE);
    ShowWindow(SimpleDialog, TRUE);
    SetFocus(Textboxes[0]);
    SendMessage(Textboxes[0], EM_SETSEL, 0, -1);

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

        if(IsDialogMessage(SimpleDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    didCancel = DialogCancel;

	char get[64];
	char * tmp = new char[sizeof(get) * boxes];

	memcpy(tmp, *dests, sizeof(char*) * boxes);

    if(!didCancel) {
        for(i = 0; i < boxes; i++) {
            if(NoCheckingOnBox[i]) {
                SendMessage(Textboxes[i], WM_GETTEXT, 17, (LPARAM)get);
                strcpy(tmp + (i * sizeof(get)), get);
            } else {
                SendMessage(Textboxes[i], WM_GETTEXT, (WPARAM)17, (LPARAM)get);

                if( (!strchr(get, '\'')) ||
                        (get[0] == '\'' && get[2] == '\'' && strlen(get)==3) )
                {
                    if(strlen(get) == 0) {
                        Error(_("Empty textbox; not permitted."));
						break;
                    } else {
                        strcpy(tmp + (i * sizeof(get)), get);
                    }
                } else {
                    Error(_("Bad use of quotes: <%s>"), get);
					break;
                }
            }
        }
		if (i == boxes)
		{
			for (i = 0; i < boxes; i++)
				strcpy(dests[i], tmp + (i * sizeof(get)));
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SimpleDialog);

	delete [] tmp;

    return !didCancel;
}

bool ShowUSSDialog(int which, string *name, int *id, int *parameter, int *parameter_set, int *index)
{
	bool changed = false;
	POINT start = { 0, 0 }, size = { 0, 0 }, GridSize = { 0, 0 };

	char name_temp[100];
	strcpy(name_temp, name->c_str());

    char i[100];
    sprintf(i, "%d", *id);

    char param[100];
    sprintf(param, "%d", *parameter);

    char param_set[100];
    sprintf(param_set, "%d", *parameter_set);

    char idx[100];
    sprintf(idx, "%d", *index);

	char *DialogName;
	char *VarName;
	eType type;

	if(which == ELEM_READ_USS) {
		type = eType_ReadUSS;
		DialogName = _("Ler Parâmetro por USS");
		VarName    = _("Destination:");
	} else if(which == ELEM_WRITE_USS) {
		type = eType_WriteUSS;
		DialogName = _("Escrever Parâmetro por USS");
		VarName    = _("Origem:");
	} else oops();

	char *labels[] = { VarName, _("ID:"), _("Parametro:"), _("Set de Parametro:"), _("Indice:") };
    char *dests[] = { name_temp, i, param, param_set, idx };
    if(ShowSimpleDialog(DialogName, 5, labels, 0x1E, 0x1, 0x1E, 0x1, dests, start, size, GridSize)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*name), name_temp, type         , _("Nome"            ), VALIDATE_IS_VAR   , 0, 0) &&
			ladder->IsValidNameAndType(0                    , i        , eType_Pending, _("ID"              ), VALIDATE_IS_NUMBER, 0, 0) &&
			ladder->IsValidNameAndType(0                    , param    , eType_Pending, _("Parâmetro"       ), VALIDATE_IS_NUMBER, 0, 0) &&
			ladder->IsValidNameAndType(0                    , param_set, eType_Pending, _("Set de Parâmetro"), VALIDATE_IS_NUMBER, 0, 3) &&
			ladder->IsValidNameAndType(0                    , idx      , eType_Pending, _("Índice"          ), VALIDATE_IS_NUMBER, 0, 0)) {
				changed = true;

				*name = name_temp;
				*id = atoi(i);
				*parameter = atoi(param);
				*parameter_set = atoi(param_set);
				*index = atoi(idx);
		}
	}

	return changed;
}

bool ShowUartDialog(int which, string *name)
{
	bool changed = false;
	POINT start = { 0, 0 }, size = { 0, 0 }, GridSize = { 0, 0 };

	char name_tmp[MAX_NAME_LEN];
    char *labels[] = { (which == ELEM_UART_RECV) ? _("Destination:") :
        _("Source:") };
    char *dests[] = { name_tmp };
	eType type = (which == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART;

	strcpy(name_tmp, name->c_str());

    if(ShowSimpleDialog((which == ELEM_UART_RECV) ? _("Receive from UART") :
        _("Send to UART"), 1, labels, 0, 0x1, 0x1, 0x1, dests, start, size, GridSize)) {
			if(ladder->IsValidNameAndType(ladder->getIdIO(*name), name_tmp, type, (which == ELEM_UART_RECV) ? _("Destino") : _("Origem"), (which == ELEM_UART_RECV) ? VALIDATE_IS_VAR : VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
				changed = true;

				*name = name_tmp;
		}
	}

	return changed;
}

bool ShowFormattedStringDialog(int mode_write, string *var, char *string)
{
	bool changed = false;
	POINT start = { 0, 0 }, size = { 0, 0 }, GridSize = { 0, 0 };

	char var_tmp[MAX_NAME_LEN], string_tmp[MAX_LOOK_UP_TABLE_LEN];
    char *labels[] = { _("Destination:"), _("String:") };
    char *dests[] = { var_tmp, string_tmp };
	eType type = mode_write ? eType_TxUART : eType_RxUART;

	strcpy(var_tmp, var->c_str());
	strcpy(string_tmp, string);

	NoCheckingOnBox[0] = TRUE;
    NoCheckingOnBox[1] = TRUE;
    if(ShowSimpleDialog(mode_write ? _("Send Formatted String Over UART") : _("Receive Formatted String Over UART"), 2, labels, 0x0, 0x1, 0x3, 0x1, dests, start, size, GridSize)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*var), var_tmp, type, _("Variável"), VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
				changed = true;

				*var = var_tmp;
				strcpy(string, string_tmp);
		}
	}

	NoCheckingOnBox[0] = FALSE;
    NoCheckingOnBox[1] = FALSE;

	return changed;
}

bool ShowServoYaskawaDialog(int mode_write, int * id, string *var, char *string)
{
	bool changed = false;
	POINT start = { 0, 0 }, size = { 0, 0 }, GridSize = { 0, 0 };

	char var_tmp[MAX_NAME_LEN], string_tmp[MAX_LOOK_UP_TABLE_LEN], id_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Id:"), _("Variable:"), _("String:") };
    char *dests[] = { id_tmp, var_tmp, string_tmp };
    NoCheckingOnBox[1] = TRUE;

    sprintf(id_tmp, "%d", *id);
	strcpy(var_tmp, var->c_str());
	strcpy(string_tmp, string);

	eType type = mode_write ? eType_WriteYaskawa : eType_ReadYaskawa;

    if(ShowSimpleDialog(mode_write ? _("Write Servo Yaskawa") : _("Read Servo Yaskawa"), 3, labels, 0x1, 0x2, 0x7, 0x2, dests, start, size, GridSize)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*var), var_tmp, type, _("Variável"), VALIDATE_IS_VAR   , 0, 0) &&
		   ladder->IsValidNameAndType(0, id_tmp , eType_Pending, _("ID"      ), VALIDATE_IS_NUMBER, 0, 0)) {
				changed = true;

				*var = var_tmp;

				*id = atoi(id_tmp);
				strcpy(string, string_tmp);
		}
	}

	NoCheckingOnBox[1] = FALSE;

	return changed;
}

void ShowSimulationVarSetDialog(char *name, char *val)
{
	POINT start = { 0, 0 }, size = { 0, 0 }, GridSize = { 0, 0 };

	char val_tmp[MAX_NAME_LEN];
	char *labels[] = { _("Valor:") };
    char *dests[] = { val_tmp };

	strcpy(val_tmp, val);

	if(ShowSimpleDialog(_("Novo Valor"), 1, labels, 0x1, 0x0, 0x1, 0, dests, start, size, GridSize)) {
		if(ladder->IsValidNameAndType(0, val_tmp, eType_Pending, _("Valor"), VALIDATE_IS_NUMBER, 0, 0)) {
			strcpy(val, val_tmp);
		}
	}
}

bool ShowVarDialog(char *title, char *varname, string *name, vector<eType> types)
{
	POINT start = { 0, 0 }, size = { 0, 0 }, GridSize = { 0, 0 };

	return ShowVarDialog(title, varname, name, start, size, GridSize, types);
}

bool ShowVarDialog(char *title, char *varname, string *name, POINT start, POINT size, POINT GridSize, vector<eType> types)
{
	bool changed = false;

	char name_tmp[MAX_NAME_LEN];

	char *labels[] = { varname };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name->c_str());

	if(ShowSimpleDialog(title, 1, labels, 0, 0x1, 0x1, 0x1, dests, start, size, GridSize, types)) {
		changed = true;
		*name = name_tmp;
	}

	return changed;
}
