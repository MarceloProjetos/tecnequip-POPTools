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
// All the simple dialogs that just require a row of text boxes: timer name
// and delay, counter name and count, move and arithmetic destination and
// operands. Try to reuse code a bit.
// Jonathan Westhues, Nov 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldmicro.h"

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
            || wParam == '-'))
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

    SimpleDialog = CreateWindowClient(0, "LDmicroDialog", title, 
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
/*
BOOL ShowSimpleDialogWithCheckbox(char *title, int boxes, char **labels, DWORD numOnlyMask,
    DWORD alnumOnlyMask, DWORD checkOnlyMask, DWORD fixedFontMask, char **dests)
{
    BOOL didCancel;

    if(boxes > MAX_BOXES) oops();

    SimpleDialog = CreateWindowClient(0, "LDmicroDialog", title, 
        WS_OVERLAPPED | WS_SYSMENU, 100, 100, 304, 15 + 30*(boxes < 2 ? 2 : boxes), MainWindow, NULL,
        Instance, NULL);

    MakeControls(boxes, labels, fixedFontMask);
  
    int i;
    for(i = 0; i < boxes; i++) {

        if(numOnlyMask & (1 << i)) {
	        SendMessage(Textboxes[i], WM_SETTEXT, 0, (LPARAM)dests[i]);
            PrevNumOnlyProc[i] = SetWindowLongPtr(Textboxes[i], GWLP_WNDPROC, 
                (LONG_PTR)MyNumOnlyProc);
        }
        if(alnumOnlyMask & (1 << i)) {
	        SendMessage(Textboxes[i], WM_SETTEXT, 0, (LPARAM)dests[i]);
            PrevAlnumOnlyProc[i] = SetWindowLongPtr(Textboxes[i], GWLP_WNDPROC, 
                (LONG_PTR)MyAlnumOnlyProc);
        }
        if(alnumOnlyMask & (1 << i)) {
	        SendMessage(Textboxes[i], WM_SETTEXT, 0, (LPARAM)dests[i]);
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

    if(!didCancel) {
        for(i = 0; i < boxes; i++) {
            if(NoCheckingOnBox[i]) {
                char get[64];
                SendMessage(Textboxes[i], WM_GETTEXT, 16, (LPARAM)get);
                strcpy(dests[i], get);
            } else {
                char get[20];
                SendMessage(Textboxes[i], WM_GETTEXT, 16, (LPARAM)get);

                if( (!strchr(get, '\'')) ||
                        (get[0] == '\'' && get[2] == '\'' && strlen(get)==3) )
                {
                    if(strlen(get) == 0) {
                        Error(_("Empty textbox; not permitted."));
                    } else {
                        strcpy(dests[i], get);
                    }
                } else {
                    Error(_("Bad use of quotes: <%s>"), get);
                }
            }
        }
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SimpleDialog);

    return !didCancel;
}
*/
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
    sprintf(delBuf, "%.3f", (*delay / 1000.0));
    strcpy(nameBuf, name+1);
    char *dests[] = { nameBuf, delBuf };

    if(ShowSimpleDialog(s, 2, labels, (1 << 1), (1 << 0), (1 << 0), (1 << 0), dests)) {
        name[0] = 'T';
        strncpy(name+1, nameBuf, 15);
		name[16] = '\0';
        double del = atof(delBuf);
        if(del > 2140000) { // 2**31/1000, don't overflow signed int
            Error(_("Delay too long; maximum is 2**31 us."));
        } else if(del <= 0) {
            Error(_("Delay cannot be zero or negative."));
        } else {
            *delay = (int)(1000*del + 0.5);
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
        _("True if >= :")) };
    char maxS[128];
    sprintf(maxS, "%d", *maxV);
    char *dests[] = { name+1, maxS };
    ShowSimpleDialog(title, 2, labels, 0x2, 0x1, 0x1, 0x1, dests);
	name[16] = '\0';
    *maxV = atoi(maxS);
}

void ShowCmpDialog(int which, char *op1, char *op2)
{
    char *title;
    char *l2;
    switch(which) {
        case ELEM_EQU:
            title = _("If Equals");
            l2 = "= :";
            break;

        case ELEM_NEQ:
            title = _("If Not Equals");
            l2 = "/= :";
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
    char *dests[] = { op1, op2 };
    ShowSimpleDialog(title, 2, labels, 0, 0x3, 0x3, 0x3, dests);

}

void ShowMoveDialog(char *dest, char *src)
{
	char dest_tmp[MAX_NAME_LEN], src_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:"), _("Source:") };
    char *dests[] = { dest_tmp, src_tmp };

	strcpy(src_tmp , src );
	strcpy(dest_tmp, dest);
    ShowSimpleDialog(_("Move"), 2, labels, 0, 0x3, 0x3, 0x3, dests);

	if(IsNumber(dest_tmp)) {
        Error(_("Destino n�o pode ser n�mero!"));
	} else {
		strcpy(src , src_tmp );
		strcpy(dest, dest_tmp);
	}
}

void ShowReadAdcDialog(char *name)
{
    char *labels[] = { _("Destination:") };
    char *dests[] = { name };
    ShowSimpleDialog(_("Read A/D Converter"), 1, labels, 0, 0x1, 0x1, 0x1, dests);
	name[15] = '\0';
}

/*void ShowSetBitDialog(char *name)
{
    char *labels[] = { _("Destination:") };
    char *dests[] = { name };
    ShowSimpleDialog(_("Set/Reset Bit"), 1, labels, 0, 0x1, 0x1, dests);
}*/

void ShowSetDADialog(char *name)
{
    //char val[10];
    //sprintf(name, "%.2f", (float)(*name) / 10000.0);

	char *labels[] = { _("Value:") };
    char *dests[] = { name };
    ShowSimpleDialog(_("Conversor Digital/Analogico (DA)"), 1, labels, 0x0, 0x1, 0x1, 0x1, dests);

	//*name = (int)(10000 * atof(name) + 0.5);
}
void ShowReadEncDialog(char *name)
{
    char *labels[] = { _("Destination:") };
    char *dests[] = { name };
    ShowSimpleDialog(_("Read Encoder"), 1, labels, 0, 0x1, 0x1, 0x1, dests);
	name[15] = '\0';
}

void ShowResetEncDialog(char *name)
{
    char *labels[] = { _("Destination:") };
    char *dests[] = { name };
    ShowSimpleDialog(_("Reset Encoder"), 1, labels, 0, 0x1, 0x1, 0x1, dests);
	name[15] = '\0';
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
    ShowSimpleDialog(_("Le Parametro do Inversor da Nord"), 5, labels, 0x1E, 0x1, 0x1E, 0x1, dests);

	if(IsNumber(name_temp)) {
		Error(_("Obrigat�rio usar vari�vel ao inv�s de n�mero no campo 'Destino'"));
	} else {
		strcpy(name, name_temp);
		*id = atoi(i);
		*parameter = atoi(param);
		*parameter_set = atoi(param_set);
		*index = atoi(idx);
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
    ShowSimpleDialog(_("Escreve Parametro no Inversor da Nord"), 5, labels, 0x1E, 0x1, 0x1E, 0x1, dests);

	if(IsNumber(name_temp)) {
		Error(_("Obrigat�rio usar vari�vel ao inv�s de n�mero no campo 'Destino'"));
	} else {
		strcpy(name, name_temp);
		*id = atoi(i);
		*parameter = atoi(param);
		*parameter_set = atoi(param_set);
		*index = atoi(idx);
	}
}

void ShowSetPwmDialog(char *name, int *targetFreq)
{
    char freq[100];
    sprintf(freq, "%d", *targetFreq);

    char *labels[] = { _("Duty cycle var:"), _("Frequency (Hz):") };
    char *dests[] = { name, freq };
    ShowSimpleDialog(_("Set PWM Duty Cycle"), 2, labels, 0x2, 0x1, 0x1, 0x1, dests);

    *targetFreq = atoi(freq);
}

void ShowUartDialog(int which, char *name)
{
    char *labels[] = { (which == ELEM_UART_RECV) ? _("Destination:") :
        _("Source:") };
    char *dests[] = { name };

    ShowSimpleDialog((which == ELEM_UART_RECV) ? _("Receive from UART") :
        _("Send to UART"), 1, labels, 0, 0x1, 0x1, 0x1, dests);

}

void ShowMathDialog(int which, char *dest, char *op1, char *op2)
{
    char *l2, *title;
    if(which == ELEM_ADD) {
        l2 = "+ :";
        title = _("Add");
    } else if(which == ELEM_SUB) {
        l2 = "- :";
        title = _("Subtract");
    } else if(which == ELEM_MUL) {
        l2 = "* :";
        title = _("Multiply");
    } else if(which == ELEM_DIV) {
        l2 = "/ :";
        title = _("Divide");
    } else oops();

	char dest_tmp[MAX_NAME_LEN], op1_tmp[MAX_NAME_LEN], op2_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Destination:"), _("is set := :"), l2 };
    char *dests[] = { dest_tmp, op1_tmp, op2_tmp };

	strcpy(op1_tmp , op1 );
	strcpy(op2_tmp , op2 );
	strcpy(dest_tmp, dest);
	ShowSimpleDialog(title, 3, labels, 0, 0x7, 0x7, 0x7, dests);

	if(IsNumber(dest_tmp)) {
        Error(_("Destino n�o pode ser n�mero!"));
	} else {
		strcpy(op1 , op1_tmp );
		strcpy(op2 , op2_tmp );
		strcpy(dest, dest_tmp);
	}
}

void ShowShiftRegisterDialog(char *name, int *stages)
{
    char stagesStr[20];
    sprintf(stagesStr, "%d", *stages);

    char *labels[] = { _("Name:"), _("Stages:") };
    char *dests[] = { name, stagesStr };
    ShowSimpleDialog(_("Shift Register"), 2, labels, 0x2, 0x1, 0x1, 0x1, dests);

    *stages = atoi(stagesStr);

    if(*stages <= 0 || *stages >= 200) {
        Error(_("Not a reasonable size for a shift register."));
        *stages = 1;
    }
}

void ShowFormattedStringDialog(char *var, char *string)
{
    char *labels[] = { _("Variable:"), _("String:") };
    char *dests[] = { var, string };
    NoCheckingOnBox[0] = TRUE;
    NoCheckingOnBox[1] = TRUE;
    ShowSimpleDialog(_("Formatted String Over UART"), 2, labels, 0x0,
        0x1, 0x3, 0x1, dests);
    NoCheckingOnBox[0] = FALSE;
    NoCheckingOnBox[1] = FALSE;
}

void ShowServoYaskawaDialog(char * id, char *var, char *string)
{
    char *labels[] = { _("Id:"), _("Variable:"), _("String:") };
    char *dests[] = { id, var, string };
    NoCheckingOnBox[1] = TRUE;
    ShowSimpleDialog(_("Servo Yaskawa"), 3, labels, 0x1,
        0x2, 0x7, 0x2, dests);

	/*while (*string)
		*(string++) = toupper(*string);*/

	NoCheckingOnBox[1] = FALSE;
}

void ShowSimulationVarSetDialog(char *name, char *val)
{
    char *labels[] = { _("Valor:") };
    char *dests[] = { val };

	ShowSimpleDialog(_("Novo Valor"), 1, labels, 0x1, 0x0, 0x1, 0, dests);
}

void ShowPersistDialog(char *var)
{
	char var_tmp[MAX_NAME_LEN];
    char *labels[] = { _("Variable:") };
    char *dests[] = { var_tmp };

	strcpy(var_tmp, var);
    ShowSimpleDialog(_("Make Persistent"), 1, labels, 0, 1, 1, 1, dests);

	if(IsNumber(var_tmp)) {
        Error(_("Vari�vel n�o pode ser n�mero!"));
	} else {
		strcpy(var, var_tmp);
	}
}
