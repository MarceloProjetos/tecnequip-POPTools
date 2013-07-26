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

static void MakeControls(int boxes, char **labels, DWORD fixedFontMask, DWORD useComboBox)
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

			LoadIOListToComboBox(Textboxes[i], vector<eType>()); // Vetor vazio, todos os tipos...
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

BOOL ShowSimpleDialog(char *title, int boxes, char **labels, DWORD numOnlyMask,
    DWORD alnumOnlyMask, DWORD fixedFontMask, DWORD useComboBox, char **dests)
{
    BOOL didCancel;

    if(boxes > MAX_BOXES) oops();

    SimpleDialog = CreateWindowClient(0, "POPToolsDialog", title, 
        WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 304, 15 + 30*(boxes < 2 ? 2 : boxes), MainWindow, NULL,
        Instance, NULL);

    MakeControls(boxes, labels, fixedFontMask, useComboBox);
  
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

    return !didCancel;
}

bool ShowTimerDialog(int which, int *delay, string *sName)
{
	bool changed = false;
	mapDetails detailsIO = ladder->getDetailsIO(*sName);

	char *s;
    switch(which) 
	{ 
        case ELEM_TON: s = _("Turn-On Delay"); break;
        case ELEM_TOF: s = _("Turn-Off Delay"); break;
        case ELEM_RTO: s = _("Retentive Turn-On Delay"); break;
        default: oops(); break;
    }

    char *labels[] = { _("Name:"), _("Delay (ms):") };

    char delBuf[20];
    char nameBuf[20];
    sprintf(delBuf, "%d", *delay / 1000);
	strcpy(nameBuf, sName->c_str());
    char *dests[] = { nameBuf, delBuf };

    if(ShowSimpleDialog(s, 2, labels, (1 << 1), (1 << 0), (1 << 0), (1 << 0), dests)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*sName), nameBuf, detailsIO.type, _("Nome" ), VALIDATE_IS_VAR   , 0, 0) &&
		   ladder->IsValidNameAndType(0                      , delBuf , eType_Pending , _("Tempo"), VALIDATE_IS_NUMBER, 1, 2147483)) {
				changed = true;

				*sName = nameBuf;

				*delay = 1000*atoi(delBuf);

				LadderSettingsGeneral settings = ladder->getSettingsGeneral();
				if(*delay % settings.cycleTime) {
					*delay = (1 + *delay / settings.cycleTime) * settings.cycleTime;
					Error(_("Tempo de ciclo deve ser múltiplo de %d! Será utilizado %d."),
						settings.cycleTime/1000, *delay/1000);
				}
		}
    }

	return changed;
}

bool ShowCounterDialog(int which, int *maxV, string *sName)
{
	bool changed = false;

	char *title;

    switch(which) {
        case ELEM_CTU:  title = _("Count Up"); break;
        case ELEM_CTD:  title = _("Count Down"); break;
        case ELEM_CTC:  title = _("Circular Counter"); break;

        default: oops();
    }

    char *labels[] = { _("Name:"), (which == ELEM_CTC ? _("Max value:") : 
		(which == ELEM_CTU ? _("True if >= :") : _("True if <= :"))) };
    char maxS[MAX_NAME_LEN], name_tmp[MAX_NAME_LEN];
    sprintf(maxS, "%d", *maxV);
	strcpy(name_tmp, sName->c_str());
    char *dests[] = { name_tmp, maxS };
    if(ShowSimpleDialog(title, 2, labels, 0x2, 0x1, 0x1, 0x1, dests)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*sName), name_tmp, eType_Counter, _("Nome" ), VALIDATE_IS_VAR   , 0, 0) &&
		   ladder->IsValidNameAndType(0                      , maxS    , eType_Pending, _("Valor"), VALIDATE_IS_NUMBER, 0, 0)) {
				changed = true;

				*sName = name_tmp;
				*maxV  = atoi(maxS);
		}
	}

	return changed;
}

bool ShowCmpDialog(int which, string *op1, string *op2)
{
	bool changed = false;

	char op1_tmp[MAX_NAME_LEN], op2_tmp[MAX_NAME_LEN];

	char *title;
    char *l2;
    switch(which) {
        case ELEM_EQU:
            title = _("If Equals");
            l2 = "= :";
            break;

        case ELEM_NEQ:
            title = _("If Not Equals");
            l2 = "<> :";
            break;

        case ELEM_GRT:
            title = _("If Greater Than");
            l2 = "> :";
            break;

        case ELEM_GEQ:
            title = _("If Greater Than or Equal To");
            l2 = ">= :";
            break;

        case ELEM_LES:
            title = _("If Less Than");
            l2 = "< :";
            break;

        case ELEM_LEQ:
            title = _("If Less Than or Equal To");
            l2 = "<= :";
            break;

        default:
            oops();
    }
    char *labels[] = { _("'Closed' if:"), l2 };
    char *dests[] = { op1_tmp, op2_tmp };

	strcpy(op1_tmp, op1->c_str());
	strcpy(op2_tmp, op2->c_str());

	if(ShowSimpleDialog(title, 2, labels, 0, 0x3, 0x3, 0x3, dests)) {
		// Se variavel sem tipo, usa tipo geral.
		eType op1_type = ladder->getTypeIO(*op1, op1_tmp, eType_General, true);
		eType op2_type = ladder->getTypeIO(*op2, op2_tmp, eType_General, true);

		if(ladder->IsValidNameAndType(ladder->getIdIO(*op1), op1_tmp, op1_type, NULL, VALIDATE_IS_VAR_OR_NUMBER, 0, 0) &&
			ladder->IsValidNameAndType(ladder->getIdIO(*op2), op2_tmp, op2_type, NULL, VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
				changed = true;
				*op1 = op1_tmp;
				*op2 = op2_tmp;
		}
	}

	return changed;
}

bool ShowSqrtDialog(string *dest, string *src)
{
	bool changed = false;

	char dest_tmp[MAX_NAME_LEN], src_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:"), _("Source:") };
    char *dests[] = { dest_tmp, src_tmp };

	strcpy(src_tmp , src ->c_str());
	strcpy(dest_tmp, dest->c_str());

	if(ShowSimpleDialog(_("Square Root"), 2, labels, 0, 0x3, 0x3, 0x3, dests)) {
		// Se variavel sem tipo, usa tipo geral.
		eType src_type = ladder->getTypeIO(*src, src_tmp, eType_General, true);

		if(ladder->IsValidNameAndType(ladder->getIdIO(*dest), dest_tmp, eType_General, _("Destino"), VALIDATE_IS_VAR          , 0, 0) &&
			ladder->IsValidNameAndType(ladder->getIdIO(*src), src_tmp , src_type, _("Origem") , VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
				changed = true;
				*src  = src_tmp;
				*dest = dest_tmp;
		}
	}

	return changed;
}

bool ShowRandDialog(string *var, string *min, string *max)
{
	bool changed = false;

	char var_tmp[MAX_NAME_LEN], min_tmp[MAX_NAME_LEN], max_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:"), _("Mínimo:"), _("Máximo:") };
    char *dests [] = { var_tmp, min_tmp, max_tmp };

	strcpy(var_tmp , var->c_str());
	strcpy(min_tmp , min->c_str());
	strcpy(max_tmp , max->c_str());

	if(ShowSimpleDialog(_("Rand"), 3, labels, 0, 0x7, 0x7, 0x7, dests)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*var), var_tmp, eType_General , _("Destino"), VALIDATE_IS_VAR          , 0, 0) &&
			ladder->IsValidNameAndType(ladder->getIdIO(*min), min_tmp, eType_General, _("Mínimo") , VALIDATE_IS_VAR_OR_NUMBER, 0, 0) &&
			ladder->IsValidNameAndType(ladder->getIdIO(*max), max_tmp, eType_General, _("Máximo") , VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
				changed = true;
				*var = var_tmp;
				*min = min_tmp;
				*max = max_tmp;
		}
	}

	return changed;
}

bool ShowAbsDialog(string *dest, string *src)
{
	bool changed = false;

	char dest_tmp[MAX_NAME_LEN], src_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:"), _("Source:") };
    char *dests[] = { dest_tmp, src_tmp };

	strcpy(src_tmp , src ->c_str());
	strcpy(dest_tmp, dest->c_str());

	if(ShowSimpleDialog(_("Abs"), 2, labels, 0, 0x3, 0x3, 0x3, dests)) {
		// Se variavel sem tipo, usa tipo geral.
		eType src_type = ladder->getTypeIO(*src, src_tmp, eType_General, true);

		if(ladder->IsValidNameAndType(ladder->getIdIO(*dest), dest_tmp, eType_General, _("Destino"), VALIDATE_IS_VAR          , 0, 0) &&
			ladder->IsValidNameAndType(ladder->getIdIO(*src), src_tmp , src_type     , _("Origem") , VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
				changed = true;
				*src  = src_tmp;
				*dest = dest_tmp;
		}
	}

	return changed;
}

bool ShowReadAdcDialog(string *name)
{
	bool changed = false;

	char name_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:") };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name->c_str());

	if(ShowSimpleDialog(_("Read A/D Converter"), 1, labels, 0, 0x1, 0x1, 0x1, dests)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*name), name_tmp, eType_ReadADC, _("Destino"), VALIDATE_IS_VAR, 0, 0)) {
			changed = true;

			*name = name_tmp;
		}
	}

	return changed;
}

bool ShowReadEncDialog(string *name)
{
	bool changed = false;

	char name_tmp[MAX_NAME_LEN];

    char *labels[] = { _("Destination:") };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name->c_str());

	if(ShowSimpleDialog(_("Read Encoder"), 1, labels, 0, 0x1, 0x1, 0x1, dests)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*name), name_tmp, eType_ReadEnc, _("Destino"), VALIDATE_IS_VAR, 0, 0)) {
			changed = true;

			*name = name_tmp;
		}
	}

	return changed;
}

bool ShowResetEncDialog(string *name)
{
	bool changed = false;

	char name_tmp[MAX_NAME_LEN];

    char *labels[] = { _("Destination:") };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name->c_str());

	if(ShowSimpleDialog(_("Write Encoder"), 1, labels, 0, 0x1, 0x1, 0x1, dests)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*name), name_tmp, eType_ResetEnc, _("Destino"), VALIDATE_IS_VAR, 0, 0)) {
			changed = true;

			*name = name_tmp;
		}
	}

	return changed;
}

bool ShowUSSDialog(int which, string *name, int *id, int *parameter, int *parameter_set, int *index)
{
	bool changed = false;

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
    if(ShowSimpleDialog(DialogName, 5, labels, 0x1E, 0x1, 0x1E, 0x1, dests)) {
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

bool ShowSetPwmDialog(string *name, int *targetFreq)
{
	bool changed = false;

	char freq[100], name_tmp[MAX_NAME_LEN];
    sprintf(freq, "%d", *targetFreq);

    char *labels[] = { _("Duty cycle var:"), _("Frequency (Hz):") };
    char *dests[] = { name_tmp, freq };

	strcpy(name_tmp, name->c_str());

	if(ShowSimpleDialog(_("Set PWM Duty Cycle"), 2, labels, 0x2, 0x1, 0x1, 0x1, dests)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*name), name_tmp, eType_PWM    , _("Variável"  ), VALIDATE_IS_VAR   , 0,    0) &&
		   ladder->IsValidNameAndType(0                     , freq    , eType_Pending, _("Frequência"), VALIDATE_IS_NUMBER, 1, 2000)) {
				changed = true;

				*name = name_tmp;
				*targetFreq = atoi(freq);
		}
	}

	return changed;
}

bool ShowUartDialog(int which, string *name)
{
	bool changed = false;

	char name_tmp[MAX_NAME_LEN];
    char *labels[] = { (which == ELEM_UART_RECV) ? _("Destination:") :
        _("Source:") };
    char *dests[] = { name_tmp };
	eType type = (which == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART;

	strcpy(name_tmp, name->c_str());

    if(ShowSimpleDialog((which == ELEM_UART_RECV) ? _("Receive from UART") :
        _("Send to UART"), 1, labels, 0, 0x1, 0x1, 0x1, dests)) {
			if(ladder->IsValidNameAndType(ladder->getIdIO(*name), name_tmp, type, (which == ELEM_UART_RECV) ? _("Destino") : _("Origem"), (which == ELEM_UART_RECV) ? VALIDATE_IS_VAR : VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
				changed = true;

				*name = name_tmp;
		}
	}

	return changed;
}

bool ShowShiftRegisterDialog(char *name, int *stages)
{
	bool changed = false;

	char stagesStr[20], name_tmp[MAX_NAME_LEN];
    sprintf(stagesStr, "%d", *stages);

    char *labels[] = { _("Name:"), _("Stages:") };
    char *dests[] = { name_tmp, stagesStr };

	strcpy(name_tmp, name);

	if(ShowSimpleDialog(_("Shift Register"), 2, labels, 0x2, 0x1, 0x1, 0x1, dests)) {
		if(ladder->IsValidNameAndType(0, stagesStr, eType_Pending, _("Estágios"), VALIDATE_IS_VAR_OR_NUMBER, 1, 200)) {
			changed = true;

			*stages = atoi(stagesStr);
			strcpy(name, name_tmp);
		}
	}

	return changed;
}

bool ShowFormattedStringDialog(int mode_write, string *var, char *string)
{
	bool changed = false;

	char var_tmp[MAX_NAME_LEN], string_tmp[MAX_LOOK_UP_TABLE_LEN];
    char *labels[] = { _("Destination:"), _("String:") };
    char *dests[] = { var_tmp, string_tmp };
	eType type = mode_write ? eType_TxUART : eType_RxUART;

	strcpy(var_tmp, var->c_str());
	strcpy(string_tmp, string);

	NoCheckingOnBox[0] = TRUE;
    NoCheckingOnBox[1] = TRUE;
    if(ShowSimpleDialog(mode_write ? _("Send Formatted String Over UART") : _("Receive Formatted String Over UART"), 2, labels, 0x0, 0x1, 0x3, 0x1, dests)) {
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

	char var_tmp[MAX_NAME_LEN], string_tmp[MAX_LOOK_UP_TABLE_LEN], id_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Id:"), _("Variable:"), _("String:") };
    char *dests[] = { id_tmp, var_tmp, string_tmp };
    NoCheckingOnBox[1] = TRUE;

    sprintf(id_tmp, "%d", *id);
	strcpy(var_tmp, var->c_str());
	strcpy(string_tmp, string);

	eType type = mode_write ? eType_WriteYaskawa : eType_ReadYaskawa;

    if(ShowSimpleDialog(mode_write ? _("Write Servo Yaskawa") : _("Read Servo Yaskawa"), 3, labels, 0x1, 0x2, 0x7, 0x2, dests)) {
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
	char val_tmp[MAX_NAME_LEN];
	char *labels[] = { _("Valor:") };
    char *dests[] = { val_tmp };

	strcpy(val_tmp, val);

	if(ShowSimpleDialog(_("Novo Valor"), 1, labels, 0x1, 0x0, 0x1, 0, dests)) {
		if(ladder->IsValidNameAndType(0, val_tmp, eType_Pending, _("Valor"), VALIDATE_IS_NUMBER, 0, 0)) {
			strcpy(val, val_tmp);
		}
	}
}

bool ShowPersistDialog(string *var)
{
	bool changed = false;

	char var_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Variable:") };
    char *dests[] = { var_tmp };

	strcpy(var_tmp, var->c_str());

	if(ShowSimpleDialog(_("Make Persistent"), 1, labels, 0, 1, 1, 1, dests)) {
		if(ladder->IsValidNameAndType(ladder->getIdIO(*var), var_tmp, eType_General, _("Variável"), VALIDATE_IS_VAR, 0, 0)) {
			changed = true;

			*var = var_tmp;
		}
	}

	return changed;
}
