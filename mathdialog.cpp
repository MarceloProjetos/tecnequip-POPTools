#include "poptools.h"

static HWND MathDialog;

static HWND DestTextbox;
static HWND Op1Textbox;
static HWND Op2Textbox;

static void MakeControls(char *op) // op: text for current operator
{
	// Destination
	HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Destination:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        7, 9, 50, 21, MathDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    DestTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        7, 30, 90, 221, MathDialog, NULL, Instance, NULL);
    FixedFont(DestTextbox);

	LoadIOListToComboBox(DestTextbox, vectorTypesVar);
	SendMessage(DestTextbox, CB_SETDROPPEDWIDTH, 200, 0);

	// Text equal ( = )
	textLabel = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        100, 25, 32, 32, MathDialog, NULL, Instance, NULL);

	HBITMAP hBmp = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(IDB_ARROW_LEFT),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(textLabel,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp);
/*
	textLabel = CreateWindowEx(0, WC_STATIC, "<--",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        100, 32, 20, 21, MathDialog, NULL, Instance, NULL);
    NiceFont(textLabel);
*/
	// Operator 1
	textLabel = CreateWindowEx(0, WC_STATIC, op ? _("Operator 1") : _("Source:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        135, 9, 70, 21, MathDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    Op1Textbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        135, 30, 90, 221, MathDialog, NULL, Instance, NULL);
    FixedFont(Op1Textbox);

	LoadIOListToComboBox(Op1Textbox, vectorTypesVar);
	SendMessage(Op1Textbox, CB_SETDROPPEDWIDTH, 200, 0);

	if(op) {
		// Text between operators
		textLabel = CreateWindowEx(0, WC_STATIC, op,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
			195, 32, 50, 21, MathDialog, NULL, Instance, NULL);
		NiceFont(textLabel);

		// Operator 2
		textLabel = CreateWindowEx(0, WC_STATIC, _("Operator 2"),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
			255, 9, 70, 21, MathDialog, NULL, Instance, NULL);
		NiceFont(textLabel);

		Op2Textbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
			WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
			255, 30, 90, 221, MathDialog, NULL, Instance, NULL);
		FixedFont(Op2Textbox);

		LoadIOListToComboBox(Op2Textbox, vectorTypesVar);
		SendMessage(Op2Textbox, CB_SETDROPPEDWIDTH, 200, 0);
	}

	// Dialog buttons
    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        195 - (op ? 0 : 117), 60, 70, 23, MathDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        275 - (op ? 0 : 117), 60, 70, 23, MathDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

bool ShowMathDialog(int which, pair<unsigned long, int> *dest, pair<unsigned long, int> *op1, pair<unsigned long, int> *op2)
{
	bool changed = false;

	int width = 352;
    char *l2, *title;

	if(which == ELEM_ADD) {
        l2 = "+";
        title = _("Add");
    } else if(which == ELEM_SUB) {
        l2 = "-";
        title = _("Subtract");
    } else if(which == ELEM_MUL) {
        l2 = "*";
        title = _("Multiply");
    } else if(which == ELEM_DIV) {
        l2 = "/";
        title = _("Divide");
    } else if(which == ELEM_MOD) {
        l2 = "%";
        title = _("Modulo");
    } else if(which == ELEM_MOVE) {
        l2 = NULL;
		width -= 117;
        title = _("Move");
    } else oops();

	MathDialog = CreateWindowClient(0, "POPToolsDialog",
        title, WS_OVERLAPPED | WS_SYSMENU,
        450, 250, width, 90, MainWindow, NULL, Instance, NULL);

    MakeControls(l2);
   
	char op1_tmp[MAX_NAME_LEN], op2_tmp[MAX_NAME_LEN];

	strcpy(op1_tmp, ladder->getNameIO(*op1).c_str());

	SendMessage(DestTextbox, WM_SETTEXT, 0, (LPARAM)(ladder->getNameIO(dest->first).c_str()));
    SendMessage(Op1Textbox , WM_SETTEXT, 0, (LPARAM)(op1_tmp));

	if(l2) {
		strcpy(op2_tmp, ladder->getNameIO(*op2).c_str());
	    SendMessage(Op2Textbox , WM_SETTEXT, 0, (LPARAM)(op2_tmp));
	}

    EnableWindow(MainWindow, FALSE);
    ShowWindow(MathDialog, TRUE);

	SetFocus(DestTextbox);
    SendMessage(DestTextbox, EM_SETSEL, 0, -1);

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

        if(IsDialogMessage(MathDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
		char dest_tmp[MAX_NAME_LEN];

		// Se variavel sem tipo, usa tipo geral.
		eType op1_type = eType_General, op2_type = eType_General;
		SendMessage(DestTextbox, WM_GETTEXT, (WPARAM)17, (LPARAM)(dest_tmp));
        SendMessage(Op1Textbox , WM_GETTEXT, (WPARAM)17, (LPARAM)(op1_tmp ));
		if(l2) {
		    SendMessage(Op2Textbox , WM_GETTEXT, (WPARAM)17, (LPARAM)(op2_tmp ));
			op2_type = ladder->getTypeIO(ladder->getNameIO(op2->first), op2_tmp, eType_General, true);
		}

		op1_type = ladder->getTypeIO(ladder->getNameIO(op1->first), op1_tmp, eType_General, true);

		if(ladder->IsValidNameAndType      (dest->first, dest_tmp, eType_General, _("Destino"), VALIDATE_IS_VAR          , 0, 0) &&
			ladder->IsValidNameAndType     (op1->first , op1_tmp , op1_type     , NULL        , VALIDATE_IS_VAR_OR_NUMBER, 0, 0) &&
		   (l2 ? ladder->IsValidNameAndType(op2->first , op2_tmp , op2_type     , NULL        , VALIDATE_IS_VAR_OR_NUMBER, 0, 0) : true)) {
				tGetIO pin;
				vector<tGetIO> pins;

				pin.pin   = pair<unsigned long, int>(dest->first, 0);
				pin.name  = dest_tmp;
				pin.isBit = false;
				pin.type  = eType_General;
				pins.push_back(pin);

				pin.pin   = pair<unsigned long, int>(op1->first, 0);
				pin.name  = op1_tmp;
				pin.isBit = false;
				pin.type  = op1_type;
				pins.push_back(pin);

				if(l2) {
					pin.pin   = pair<unsigned long, int>(op2->first, 0);
					pin.name  = op2_tmp;
					pin.isBit = false;
					pin.type  = op2_type;
					pins.push_back(pin);
				}

				// Se variavel alterada e valida, atualiza o pino
				if(ladder->getIO(pins)) {
					changed = true;
					*dest = pins[0].pin;
					*op1  = pins[1].pin;
					if(l2) {
						*op2 = pins[2].pin;
					}
				}
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(MathDialog);

	return changed;
}
