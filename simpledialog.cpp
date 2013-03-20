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

			LoadIOListToComboBox(Textboxes[i], IO_TYPE_ALL);
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

void ShowTimerDialog(int which, int *delay, char *name)
{
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
    strcpy(nameBuf, name);
    char *dests[] = { nameBuf, delBuf };

    if(ShowSimpleDialog(s, 2, labels, (1 << 1), (1 << 0), (1 << 0), (1 << 0), dests)) {
		if(IsValidNameAndType(name  , nameBuf, _("Nome") , VALIDATE_IS_VAR   , GetTypeFromName(nameBuf), 0, 0) &&
		   IsValidNameAndType(delBuf, delBuf , _("Tempo"), VALIDATE_IS_NUMBER, GetTypeFromName(delBuf ), 1, 2147483)) {
			   *delay = 1000*atoi(delBuf);
			   if(*delay % Prog.settings.cycleTime) {
				   *delay = (1 + *delay / Prog.settings.cycleTime) * Prog.settings.cycleTime;
				   Error(_("Tempo de ciclo deve ser múltiplo de %d! Será utilizado %d."),
							Prog.settings.cycleTime/1000, *delay/1000);
			   }
		        strncpy(name, nameBuf, 16);
				name[16] = '\0';
		}
    }
}

void ShowCounterDialog(int which, int *maxV, char *name)
{
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
	strcpy(name_tmp, name);
    char *dests[] = { name_tmp, maxS };
    if(ShowSimpleDialog(title, 2, labels, 0x2, 0x1, 0x1, 0x1, dests)) {
		if(IsValidNameAndType(name, name_tmp, _("Nome") , VALIDATE_IS_VAR   , GetTypeFromName(name_tmp), 0, 0) &&
		   IsValidNameAndType(maxS, maxS    , _("Valor"), VALIDATE_IS_NUMBER, GetTypeFromName(maxS    ), 0, 0)) {
				strncpy(name, name_tmp, 16);
				name[16] = '\0';
				*maxV = atoi(maxS);
		}
	}
}

void ShowCmpDialog(int which, char *op1, char *op2)
{
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

	strcpy(op1_tmp, op1);
	strcpy(op2_tmp, op2);

	if(ShowSimpleDialog(title, 2, labels, 0, 0x3, 0x3, 0x3, dests)) {
		if(IsValidNameAndType(op1, op1_tmp, NULL, VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(op1_tmp), 0, 0) &&
		   IsValidNameAndType(op2, op2_tmp, NULL, VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(op2_tmp), 0, 0)) {
			   strcpy(op1, op1_tmp);
			   strcpy(op2, op2_tmp);
		}
	}
}

void ShowSqrtDialog(char *dest, char *src)
{
	char dest_tmp[MAX_NAME_LEN], src_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:"), _("Source:") };
    char *dests[] = { dest_tmp, src_tmp };

	strcpy(src_tmp , src );
	strcpy(dest_tmp, dest);

	if(ShowSimpleDialog(_("Square Root"), 2, labels, 0, 0x3, 0x3, 0x3, dests)) {
		if(IsValidNameAndType(dest, dest_tmp, _("Destino"), VALIDATE_IS_VAR          , GetTypeFromName(dest_tmp), 0, 0) &&
		   IsValidNameAndType(src , src_tmp , _("Origem") , VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(src_tmp ), 0, 0)) {
			strcpy(src , src_tmp );
			strcpy(dest, dest_tmp);
		}
	}
}

void ShowRandDialog(char *var, char *min, char *max)
{
	char var_tmp[MAX_NAME_LEN], min_tmp[MAX_NAME_LEN], max_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:"), _("Mínimo:"), _("Máximo:") };
    char *dests [] = { var_tmp, min_tmp, max_tmp };

	strcpy(var_tmp, var);
	strcpy(min_tmp, min);
	strcpy(max_tmp, max);

	if(ShowSimpleDialog(_("Rand"), 3, labels, 0, 0x7, 0x7, 0x7, dests)) {
		if(IsValidNameAndType(var, var_tmp, _("Destino"), VALIDATE_IS_VAR          , GetTypeFromName(var_tmp), 0, 0) &&
		   IsValidNameAndType(min, min_tmp, _("Mínimo") , VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(min_tmp), 0, 0) &&
		   IsValidNameAndType(min, max_tmp, _("Máximo") , VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(max_tmp), 0, 0)) {
			strcpy(var, var_tmp);
			strcpy(min, min_tmp);
			strcpy(max, max_tmp);
		}
	}
}

void ShowAbsDialog(char *dest, char *src)
{
	char dest_tmp[MAX_NAME_LEN], src_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:"), _("Source:") };
    char *dests[] = { dest_tmp, src_tmp };

	strcpy(src_tmp , src );
	strcpy(dest_tmp, dest);

	if(ShowSimpleDialog(_("Abs"), 2, labels, 0, 0x3, 0x3, 0x3, dests)) {
		if(IsValidNameAndType(dest, dest_tmp, _("Destino"), VALIDATE_IS_VAR          , GetTypeFromName(dest_tmp), 0, 0) &&
		   IsValidNameAndType(src , src_tmp , _("Origem") , VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(src_tmp ), 0, 0)) {
			strcpy(src , src_tmp );
			strcpy(dest, dest_tmp);
		}
	}
}

void ShowReadAdcDialog(char *name)
{
	char name_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:") };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name);

	if(ShowSimpleDialog(_("Read A/D Converter"), 1, labels, 0, 0x1, 0x1, 0x1, dests)) {
		if(IsValidNameAndType(name, name_tmp, _("Destino"), VALIDATE_IS_VAR, GetTypeFromName(name_tmp), 0, 0)) {
			strcpy(name, name_tmp);
			name[16] = '\0';
		}
	}
}

void ShowReadEncDialog(char *name)
{
	char name_tmp[MAX_NAME_LEN];

    char *labels[] = { _("Destination:") };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name);

	if(ShowSimpleDialog(_("Read Encoder"), 1, labels, 0, 0x1, 0x1, 0x1, dests)) {
		if(IsValidNameAndType(name, name_tmp, _("Destino"), VALIDATE_IS_VAR, GetTypeFromName(name_tmp), 0, 0)) {
			strcpy(name, name_tmp);
			name[16] = '\0';
		}
	}
}

void ShowResetEncDialog(char *name)
{
	char name_tmp[MAX_NAME_LEN];

    char *labels[] = { _("Destination:") };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name);

	if(ShowSimpleDialog(_("Write Encoder"), 1, labels, 0, 0x1, 0x1, 0x1, dests)) {
		if(IsValidNameAndType(name, name_tmp, _("Destino"), VALIDATE_IS_VAR, GetTypeFromName(name_tmp), 0, 0)) {
			strcpy(name, name_tmp);
			name[16] = '\0';
		}
	}
}

void ShowReadUSSDialog(char *name, int *id, int *parameter, int *parameter_set, int *index)
{
	char name_temp[100];
	strcpy(name_temp, name);

    char i[100];
    sprintf(i, "%d", *id);

    char param[100];
    sprintf(param, "%d", *parameter);

    char param_set[100];
    sprintf(param_set, "%d", *parameter_set);

    char idx[100];
    sprintf(idx, "%d", *index);

	char *labels[] = { _("Destination:"), _("ID:"), _("Parametro:"), _("Set de Parametro:"), _("Indice:") };
    char *dests[] = { name_temp, i, param, param_set, idx };
    if(ShowSimpleDialog(_("Ler Parâmetro por USS"), 5, labels, 0x1E, 0x1, 0x1E, 0x1, dests)) {
		if(IsValidNameAndType(name     , name_temp, _("Nome"            ), VALIDATE_IS_VAR   , GetTypeFromName(name_temp), 0, 0) &&
		   IsValidNameAndType(i        , i        , _("ID"              ), VALIDATE_IS_NUMBER, GetTypeFromName(i        ), 0, 0) &&
		   IsValidNameAndType(param    , param    , _("Parâmetro"       ), VALIDATE_IS_NUMBER, GetTypeFromName(param    ), 0, 0) &&
		   IsValidNameAndType(param_set, param_set, _("Set de Parâmetro"), VALIDATE_IS_NUMBER, GetTypeFromName(param_set), 0, 3) &&
		   IsValidNameAndType(idx      , idx      , _("Índice"          ), VALIDATE_IS_NUMBER, GetTypeFromName(idx      ), 0, 0)) {
				strcpy(name, name_temp);
				*id = atoi(i);
				*parameter = atoi(param);
				*parameter_set = atoi(param_set);
				*index = atoi(idx);
		}
	}
}

void ShowWriteUSSDialog(char *name, int *id, int *parameter, int *parameter_set, int *index)
{
	char name_temp[100];
	strcpy(name_temp, name);

    char i[100];
    sprintf(i, "%d", *id);

    char param[100];
    sprintf(param, "%d", *parameter);

    char param_set[100];
    sprintf(param_set, "%d", *parameter_set);

    char idx[100];
    sprintf(idx, "%d", *index);

	char *labels[] = { _("Origem:"), _("ID:"), _("Parametro:"), _("Set de Parametro:"), _("Indice:") };
    char *dests[] = { name_temp, i, param, param_set, idx };

	if(ShowSimpleDialog(_("Escrever Parâmetro por USS"), 5, labels, 0x1E, 0x1, 0x1E, 0x1, dests)) {
		if(IsValidNameAndType(name     , name_temp, _("Nome"            ), VALIDATE_IS_VAR   , GetTypeFromName(name_temp), 0, 0) &&
		   IsValidNameAndType(i        , i        , _("ID"              ), VALIDATE_IS_NUMBER, GetTypeFromName(i        ), 0, 0) &&
		   IsValidNameAndType(param    , param    , _("Parâmetro"       ), VALIDATE_IS_NUMBER, GetTypeFromName(param    ), 0, 0) &&
		   IsValidNameAndType(param_set, param_set, _("Set de Parâmetro"), VALIDATE_IS_NUMBER, GetTypeFromName(param_set), 0, 3) &&
		   IsValidNameAndType(idx      , idx      , _("Índice"          ), VALIDATE_IS_NUMBER, GetTypeFromName(idx      ), 0, 0)) {
				strcpy(name, name_temp);
				*id = atoi(i);
				*parameter = atoi(param);
				*parameter_set = atoi(param_set);
				*index = atoi(idx);
		}
	}
}

void ShowSetPwmDialog(char *name, int *targetFreq)
{
    char freq[100], name_tmp[MAX_NAME_LEN];
    sprintf(freq, "%d", *targetFreq);

    char *labels[] = { _("Duty cycle var:"), _("Frequency (Hz):") };
    char *dests[] = { name_tmp, freq };

	strcpy(name_tmp, name);

	if(ShowSimpleDialog(_("Set PWM Duty Cycle"), 2, labels, 0x2, 0x1, 0x1, 0x1, dests)) {
		if(IsValidNameAndType(name, name_tmp, _("Variável"  ), VALIDATE_IS_VAR   , GetTypeFromName(name_tmp), 0,    0) &&
		   IsValidNameAndType(freq, freq    , _("Frequência"), VALIDATE_IS_NUMBER, GetTypeFromName(freq    ), 1, 2000)) {
			   strcpy(name, name_tmp);
				*targetFreq = atoi(freq);
		}
	}
}

void ShowUartDialog(int which, char *name)
{
	char name_tmp[MAX_NAME_LEN];
    char *labels[] = { (which == ELEM_UART_RECV) ? _("Destination:") :
        _("Source:") };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name);

    if(ShowSimpleDialog((which == ELEM_UART_RECV) ? _("Receive from UART") :
	        _("Send to UART"), 1, labels, 0, 0x1, 0x1, 0x1, dests)) {
		if(IsValidNameAndType(name, name_tmp, (which == ELEM_UART_RECV) ? _("Destino") : _("Origem"), (which == ELEM_UART_RECV) ? VALIDATE_IS_VAR : VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(name_tmp), 0, 0)) {
			strcpy(name, name_tmp);
		}
	}
}

void ShowShiftRegisterDialog(char *name, int *stages)
{
    char stagesStr[20], name_tmp[MAX_NAME_LEN];
    sprintf(stagesStr, "%d", *stages);

    char *labels[] = { _("Name:"), _("Stages:") };
    char *dests[] = { name_tmp, stagesStr };

	strcpy(name_tmp, name);

	if(ShowSimpleDialog(_("Shift Register"), 2, labels, 0x2, 0x1, 0x1, 0x1, dests)) {
		if(IsValidNameAndType(name     , name_tmp , _("Nome"    ), VALIDATE_IS_VAR          , GetTypeFromName(name_tmp ), 0,   0) &&
		   IsValidNameAndType(stagesStr, stagesStr, _("Estágios"), VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(stagesStr), 1, 200)) {
				*stages = atoi(stagesStr);
				strcpy(name, name_tmp);
		}
	}
}

void ShowFormattedStringDialog(char *var, char *string)
{
	char var_tmp[MAX_NAME_LEN], string_tmp[MAX_LOOK_UP_TABLE_LEN];
    char *labels[] = { _("Destination:"), _("String:") };
    char *dests[] = { var_tmp, string_tmp };

	strcpy(var_tmp   , var   );
	strcpy(string_tmp, string);

	NoCheckingOnBox[0] = TRUE;
    NoCheckingOnBox[1] = TRUE;
    if(ShowSimpleDialog(_("Formatted String Over UART"), 2, labels, 0x0, 0x1, 0x3, 0x1, dests)) {
		if(IsValidNameAndType(var, var_tmp , _("Variável"), VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(var_tmp ), 0, 0)) {
				strcpy(var   , var_tmp   );
				strcpy(string, string_tmp);
		}
	}

	NoCheckingOnBox[0] = FALSE;
    NoCheckingOnBox[1] = FALSE;
}

void ShowServoYaskawaDialog(char * id, char *var, char *string)
{
	char var_tmp[MAX_NAME_LEN], string_tmp[MAX_LOOK_UP_TABLE_LEN], id_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Id:"), _("Variable:"), _("String:") };
    char *dests[] = { id_tmp, var_tmp, string_tmp };
    NoCheckingOnBox[1] = TRUE;

	strcpy(id_tmp    , id    );
	strcpy(var_tmp   , var   );
	strcpy(string_tmp, string);

    if(ShowSimpleDialog(_("Servo Yaskawa"), 3, labels, 0x1, 0x2, 0x7, 0x2, dests)) {
		if(IsValidNameAndType(var, var_tmp, _("Variável"), VALIDATE_IS_VAR   , GetTypeFromName(var_tmp ), 0, 0) &&
		   IsValidNameAndType(id , id_tmp , _("ID"      ), VALIDATE_IS_NUMBER, GetTypeFromName(id_tmp  ), 0, 0)) {
				strcpy(id    , id_tmp    );
				strcpy(var   , var_tmp   );
				strcpy(string, string_tmp);
		}
	}

	NoCheckingOnBox[1] = FALSE;
}

void ShowSimulationVarSetDialog(char *name, char *val)
{
	char val_tmp[MAX_NAME_LEN];
	char *labels[] = { _("Valor:") };
    char *dests[] = { val_tmp };

	strcpy(val_tmp, val);

	if(ShowSimpleDialog(_("Novo Valor"), 1, labels, 0x1, 0x0, 0x1, 0, dests)) {
		if(IsValidNameAndType(val, val_tmp, _("Valor"), VALIDATE_IS_NUMBER, GetTypeFromName(val_tmp), 0, 0)) {
			strcpy(val, val_tmp);
		}
	}
}

void ShowPersistDialog(char *var)
{
	char var_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Variable:") };
    char *dests[] = { var_tmp };

	strcpy(var_tmp, var);

	if(ShowSimpleDialog(_("Make Persistent"), 1, labels, 0, 1, 1, 1, dests)) {
		if(IsValidNameAndType(var, var_tmp, _("Variável"), VALIDATE_IS_VAR, GetTypeFromName(var_tmp), 0, 0)) {
			strcpy(var, var_tmp);
		}
	}
}
