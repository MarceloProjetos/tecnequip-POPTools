#include "poptools.h"

static HWND SimpleDialog;

#define MAX_BOXES 5

static HWND Textboxes[MAX_BOXES];
static HWND Labels[MAX_BOXES];

static LONG_PTR PrevAlnumOnlyProc[MAX_BOXES];
static LONG_PTR PrevNumOnlyProc[MAX_BOXES];

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

static void MakeControls(int boxes, const char **labels, DWORD fixedFontMask,
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

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        218, 41, 70, 23, SimpleDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

// Default para types: Vetor vazio, todos os tipos...
BOOL ShowSimpleDialog(const char *title, int boxes, const char **labels, DWORD numOnlyMask,
    DWORD alnumOnlyMask, DWORD fixedFontMask, DWORD useComboBox, char **dests,
	POINT ElemStart, POINT ElemSize, POINT GridSize, vector<eType> types, bool isTxt)
{
    BOOL didCancel;
	POINT size = { 304, 15 + 30*(boxes < 2 ? 2 : boxes) };
	POINT start = getWindowStart(ElemStart, ElemSize, size, GridSize);

    if(boxes > MAX_BOXES) oops();

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

	char get[1024];
	char * tmp = new char[sizeof(get) * boxes];

	memcpy(tmp, *dests, sizeof(char*) * boxes);

    if(!didCancel) {
        for(i = 0; i < boxes; i++) {
            SendMessage(Textboxes[i], WM_GETTEXT, (WPARAM)(isTxt ? sizeof(get) : 17), (LPARAM)get);

            if( (!strchr(get, '\'')) ||
                    (get[0] == '\'' && get[2] == '\'' && strlen(get)==3) )
            {
                if(strlen(get) == 0) {
                    Error(_("Texto vazio; não é permitido"));
					break;
                } else {
                    strcpy(tmp + (i * sizeof(get)), get);
                }
            } else {
                Error(_("Mau uso das aspas: <%s>"), get);
				break;
            }
        }
		if (i == boxes)
		{
			for (i = 0; i < boxes; i++)
				strcpy(dests[i], tmp + (i * sizeof(get)));
		} else {
			didCancel = true;
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SimpleDialog);
	SetFocus(MainWindow);

	delete [] tmp;

    return !didCancel;
}

void ShowSimulationVarSetDialog(const char *name, char *val)
{
	POINT start = { 0, 0 }, size = { 0, 0 }, GridSize = { 0, 0 };

	char val_tmp[MAX_NAME_LEN];
	const char *labels[] = { _("Valor:") };
    char *dests[] = { val_tmp };

	strcpy(val_tmp, val);

	if(ShowSimpleDialog(_("Novo Valor"), 1, labels, 0x1, 0x0, 0x1, 0, dests, start, size, GridSize, vector<eType>(), false)) {
		if(ladder->IsValidNameAndType(0, val_tmp, eType_Pending, _("Valor"), VALIDATE_IS_NUMBER, 0, 0)) {
			strcpy(val, val_tmp);
		}
	}
}

bool ShowVarDialog(const char *title, const char *varname, string *name, POINT start, POINT size, POINT GridSize, vector<eType> types, bool isTxt)
{
	bool changed = false;

	char name_tmp[1024];

	const char *labels[] = { varname };
    char *dests[] = { name_tmp };

	strcpy(name_tmp, name->c_str());

	if(ShowSimpleDialog(title, 1, labels, 0, 0x1, 0x1, 0x1, dests, start, size, GridSize, types, isTxt)) {
		changed = true;
		*name = name_tmp;
	}

	return changed;
}
