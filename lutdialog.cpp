#include "poptools.h"

static HWND LutDialog;

static HWND AsStringCheckbox;
static HWND CountTextbox;
static HWND Labels;

static HWND StringTextbox;

static BOOL WasAsString;
static int WasCount;

static HWND ValuesTextbox[MAX_LOOK_UP_TABLE_LEN];
static LONG_PTR PrevValuesProc[MAX_LOOK_UP_TABLE_LEN];
static HWND ValuesLabel[MAX_LOOK_UP_TABLE_LEN];

static SWORD ValuesCache[MAX_LOOK_UP_TABLE_LEN];

static LONG_PTR PrevDestProc;
static LONG_PTR PrevIndexProc;
static LONG_PTR PrevCountProc;

//-----------------------------------------------------------------------------
// Don't allow any characters other than 0-9 and minus in the values.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MyNumberProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isdigit(wParam) || wParam == '\b' || wParam == '-')) {
            return 0;
        }
    }
    
    WNDPROC w;
    int i;
    for(i = 0; i < MAX_LOOK_UP_TABLE_LEN; i++) {
        if(hwnd == ValuesTextbox[i]) {
            w = (WNDPROC)PrevValuesProc[i];
            break;
        }
    }
    if(i == MAX_LOOK_UP_TABLE_LEN) oops();

    return CallWindowProc(w, hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Don't allow any characters other than 0-9 in the count.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MyDigitsProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isdigit(wParam) || wParam == '\b')) {
            return 0;
        }
    }

    return CallWindowProc((WNDPROC)PrevCountProc, hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Make the controls that are guaranteed not to move around as the count/
// as string settings change. This is different for the piecewise linear,
// because in that case we should not provide a checkbox to change whether
// the table is edited as a string or table.
//-----------------------------------------------------------------------------
static void MakeFixedControls(BOOL forPwl)
{
    Labels = CreateWindowEx(0,WC_STATIC, forPwl ? _("Points:") : _("Count:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        0, 10, 78, 21, LutDialog, NULL, Instance, NULL);
    NiceFont(Labels);

    CountTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        85, 10, 120, 21, LutDialog, NULL, Instance, NULL);
    NiceFont(CountTextbox);

    if(!forPwl) {
		Labels = CreateWindowEx(0,WC_STATIC, _("Edit table of ASCII values like a string"),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT,
			27, 35, 205, 34, LutDialog, NULL, Instance, NULL);
		NiceFont(Labels);

		AsStringCheckbox = CreateWindowEx(0, WC_BUTTON, 
            "", WS_CHILD |
                WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_AUTOCHECKBOX,
            10, 40, 220, 21, LutDialog, NULL, Instance, NULL);
        NiceFont(AsStringCheckbox);
    }

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        231, 10, 70, 23, LutDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        231, 40, 70, 23, LutDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevCountProc = SetWindowLongPtr(CountTextbox, GWLP_WNDPROC, 
        (LONG_PTR)MyDigitsProc);
}

//-----------------------------------------------------------------------------
// Destroy all of the controls so that we can start anew. This is necessary
// because if the size of the LUT changes, or if the user switches from 
// table entry to string entry, we must completely reconfigure the dialog.
//-----------------------------------------------------------------------------
static void DestroyLutControls(void)
{
    if(WasAsString) {
        // Nothing to do; we constantly update the cache from the user-
        // specified string, because we might as well do that when we
        // calculate the length.
    } else {
        int i;
        for(i = 0; i < WasCount; i++) {
            char buf[20];
            SendMessage(ValuesTextbox[i], WM_GETTEXT, (WPARAM)16, (LPARAM)buf);
            ValuesCache[i] = atoi(buf);
        }
    }

    DestroyWindow(StringTextbox);

    int i;
    for(i = 0; i < MAX_LOOK_UP_TABLE_LEN; i++) {
        DestroyWindow(ValuesTextbox[i]);
        DestroyWindow(ValuesLabel[i]);
    }
}

//-----------------------------------------------------------------------------
// Make the controls that hold the LUT. The exact configuration of the dialog
// will depend on (a) whether the user chose table-type or string-type entry,
// and for table-type entry, on (b) the number of entries, and on (c)
// whether we are editing a PWL table (list of points) or a straight LUT.
//-----------------------------------------------------------------------------
static void MakeLutControls(BOOL asString, int count, BOOL forPwl)
{
    // Remember these, so that we know from where to cache stuff if we have
    // to destroy these textboxes and make something new later.
    WasAsString = asString;
    WasCount = count;

    if(forPwl && asString) oops();

    if(asString) {
        char str[3*MAX_LOOK_UP_TABLE_LEN+1];
        int i, j;
        j = 0;
        for(i = 0; i < count; i++) {
            int c = ValuesCache[i];
            if(c >= 32 && c <= 127 && c != '\\') {
                str[j++] = c;
            } else if(c == '\\') {
                str[j++] = '\\';
                str[j++] = '\\';
            } else if(c == '\r') {
                str[j++] = '\\';
                str[j++] = 'r';
            } else if(c == '\b') {
                str[j++] = '\\';
                str[j++] = 'b';
            } else if(c == '\f') {
                str[j++] = '\\';
                str[j++] = 'f';
            } else if(c == '\n') {
                str[j++] = '\\';
                str[j++] = 'n';
            } else {
                str[j++] = 'X';
            }
        }
        str[j++] = '\0';
        StringTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, str,
            WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | 
                WS_VISIBLE,
            10, 70, 294, 21, LutDialog, NULL, Instance, NULL);
        FixedFont(StringTextbox);
        SendMessage(CountTextbox, EM_SETREADONLY, (WPARAM)TRUE, 0);
        MoveWindow(LutDialog, 100, 30, 320, 125, TRUE);
    } else {
        int i, breakpoint = (count+1)/2;
        int base = 80;

		for(i = 0; i < count; i++) {
            int x, y;

            if(i < breakpoint) {
                x = 10;
                y = base+30*i;
            } else {
                x = 160;
                y = base+30*(i-breakpoint);
            }

            char buf[20];
            sprintf(buf, "%d", ValuesCache[i]);
            ValuesTextbox[i] = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, buf,
                WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS |
                    WS_VISIBLE,
                x+30, y, 80, 21, LutDialog, NULL, Instance, NULL);
            NiceFont(ValuesTextbox[i]);

            if(forPwl) {
                sprintf(buf, "%c%d:", (i & 1) ? 'y' : 'x', i/2);
            } else {
                sprintf(buf, "%2d:", i);
            }
            ValuesLabel[i] = CreateWindowEx(0, WC_STATIC, buf,
                WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                x, y+3, 100, 21, LutDialog, NULL, Instance, NULL);
            FixedFont(ValuesLabel[i]);

            PrevValuesProc[i] = SetWindowLongPtr(ValuesTextbox[i],
                GWLP_WNDPROC, (LONG_PTR)MyNumberProc);
        }
        SendMessage(CountTextbox, EM_SETREADONLY, (WPARAM)FALSE, 0);

        MoveWindow(LutDialog, 100, 30, 320, base + (count > 0 ? (breakpoint + 1)*30 : 20), TRUE);
    }
}

//-----------------------------------------------------------------------------
// Decode a string into a look-up table; store the values in ValuesCache[],
// and update the count checkbox (which in string mode is read-only) to
// reflect the new length. Returns FALSE if the new string is too long, else
// TRUE.
//-----------------------------------------------------------------------------
BOOL StringToValuesCache(char *str, int *c)
{
    int count = 0;
    while(*str) {
        if(*str == '\\') {
            str++;
            switch(*str) {
                case 'r': ValuesCache[count++] = '\r'; break;
                case 'n': ValuesCache[count++] = '\n'; break;
                case 'f': ValuesCache[count++] = '\f'; break;
                case 'b': ValuesCache[count++] = '\b'; break;
                default:  ValuesCache[count++] = *str; break;
            }
        } else {
            ValuesCache[count++] = *str;
        }
        if(*str) {
            str++;
        }
        if(count >= 32) {
            return FALSE;
        }
    }

    char buf[10];
    sprintf(buf, "%d", count);
    SendMessage(CountTextbox, WM_SETTEXT, 0, (LPARAM)(buf));
    *c = count;
    return TRUE;
}

//-----------------------------------------------------------------------------
// Show the look-up table dialog. This one is nasty, mostly because there are
// two ways to enter a look-up table: as a table, or as a string. Presumably
// I should convert between those two representations on the fly, as the user
// edit things, so I do.
//-----------------------------------------------------------------------------
bool ShowLookUpTableDialog(LadderElemLUTProp *t)
{
	bool changed = false;

    // First copy over all the stuff from the leaf structure; in particular,
    // we need our own local copy of the table entries, because it would be
    // bad to update those in the leaf before the user clicks okay (as he
    // might cancel).
    int count = t->count;
    bool asString = t->editAsString;
    memset(ValuesCache, 0, sizeof(ValuesCache));
    int i;
    for(i = 0; i < count; i++) {
        ValuesCache[i] = t->vals[i];
    }

    // Now create the dialog's fixed controls, plus the changing (depending
    // on show style/entry count) controls for the initial configuration.
    LutDialog = CreateWindowClient(0, "POPToolsDialog",
        _("Look-Up Table"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 320, 375, MainWindow, NULL, Instance, NULL);
    MakeFixedControls(FALSE);
    MakeLutControls(asString, count, FALSE);
  
    // Set up the controls to reflect the initial configuration.
    char buf[30];
    sprintf(buf, "%d", t->count);
    SendMessage(CountTextbox, WM_SETTEXT, 0, (LPARAM)buf);
    if(asString) {
        SendMessage(AsStringCheckbox, BM_SETCHECK, BST_CHECKED, 0);
    }

    // And show the window
    EnableWindow(MainWindow, FALSE);
    ShowWindow(LutDialog, TRUE);
    SetFocus(CountTextbox);
    SendMessage(CountTextbox, EM_SETSEL, 0, -1);

    char PrevTableAsString[1024] = "";

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

        if(!IsDialogMessage(LutDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Are we in table mode? In that case watch the (user-editable) count
        // field, and use that to determine how many textboxes to show.
        char buf[20];
        SendMessage(CountTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)buf);
        if(atoi(buf) != count && !asString) {
            count = atoi(buf);
            if(count < 0 || count > 32) {
                count = 0;
                SendMessage(CountTextbox, WM_SETTEXT, 0, (LPARAM)"");
            }
            DestroyLutControls();
            MakeLutControls(asString, count, FALSE);
        }

        // Are we in string mode? In that case watch the string textbox,
        // and use that to update the (read-only) count field.
        if(asString) {
            char scratch[1024];
            SendMessage(StringTextbox, WM_GETTEXT, (WPARAM)sizeof(scratch),
                (LPARAM)scratch);
            if(_stricmp(scratch, PrevTableAsString)!=0) {
                if(StringToValuesCache(scratch, &count)) {
                    strcpy(PrevTableAsString, scratch);
                } else {
                    // Too long; put back the old one
                    SendMessage(StringTextbox, WM_SETTEXT, 0,
                        (LPARAM)PrevTableAsString);
                }
            }
        }

        // Did we just change modes?
        bool x = SendMessage(AsStringCheckbox, BM_GETCHECK, 0, 0)==BST_CHECKED;
        if((x && !asString) || (!x && asString)) {
            asString = x;
            DestroyLutControls();
            MakeLutControls(asString, count, FALSE);
        }

    }

    if(!DialogCancel) {
        DestroyLutControls();
		changed = true;

		// The call to DestroyLutControls updated ValuesCache, so just read
		// them out of there (whichever mode we were in before).
		int i;
		for(i = 0; i < count; i++) {
			t->vals[i] = ValuesCache[i];
		}

		t->count = count;
		t->editAsString = asString;
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(LutDialog);

	return changed;
}

//-----------------------------------------------------------------------------
// Show the piecewise linear table dialog. This one can only be edited in
// only a single format, which makes things easier than before.
//-----------------------------------------------------------------------------
bool ShowPiecewiseLinearDialog(LadderElemPiecewiseProp *t)
{
	bool changed = false;

    // First copy over all the stuff from the leaf structure; in particular,
    // we need our own local copy of the table entries, because it would be
    // bad to update those in the leaf before the user clicks okay (as he
    // might cancel).
    int count = t->count;
    memset(ValuesCache, 0, sizeof(ValuesCache));
    int i;
    for(i = 0; i < count*2; i++) {
        ValuesCache[i] = t->vals[i];
    }

    // Now create the dialog's fixed controls, plus the changing (depending
    // on show style/entry count) controls for the initial configuration.
    LutDialog = CreateWindowClient(0, "POPToolsDialog",
        _("Piecewise Linear Table"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 320, 375, MainWindow, NULL, Instance, NULL);
    MakeFixedControls(TRUE);
    MakeLutControls(FALSE, count*2, TRUE);
  
    // Set up the controls to reflect the initial configuration.
    char buf[30];
    sprintf(buf, "%d", t->count);
    SendMessage(CountTextbox, WM_SETTEXT, 0, (LPARAM)buf);

    // And show the window
    EnableWindow(MainWindow, FALSE);
    ShowWindow(LutDialog, TRUE);
    SetFocus(CountTextbox);
    SendMessage(CountTextbox, EM_SETSEL, 0, -1);

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

        if(!IsDialogMessage(LutDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Watch the (user-editable) count field, and use that to
        // determine how many textboxes to show.
        char buf[20];
        SendMessage(CountTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)buf);
        if(atoi(buf) != count) {
            count = atoi(buf);
            if(count < 0 || count > 10) {
                count = 0;
                SendMessage(CountTextbox, WM_SETTEXT, 0, (LPARAM)"");
            }
            DestroyLutControls();
            MakeLutControls(FALSE, count*2, TRUE);
        }
    }

    if(!DialogCancel) {
		DestroyLutControls();
		changed = true;

		// The call to DestroyLutControls updated ValuesCache, so just read
		// them out of there.
		int i;
		for(i = 0; i < count*2; i++) {
			t->vals[i] = ValuesCache[i];
		}

		t->count = count;
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(LutDialog);

	return changed;
}
