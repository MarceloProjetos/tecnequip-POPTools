#include "poptools.h"

const LONG FarWidth = 296, FarHeight = 135;
static bool FARWindowDone = TRUE;

HWND FARWindow = NULL;

static HWND FarOkButton;

static HWND SearchTextbox;
static HWND ReplaceTextbox;
static HWND SearchNextButton;
static HWND ReplaceButton;
static HWND ReplaceAllButton;

static LONG_PTR PrevNameProc;

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK FARWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	unsigned int matches, validate_mode;
	char texto[100];
	char search_text[MAX_NAME_LEN], new_text[MAX_NAME_LEN];

	switch (msg) {
        case WM_DESTROY:
			if(FARWindow != NULL) {
				FARWindow = NULL;
				MainWindowResized();
			}
            break;

		case WM_COMMAND: {
            HWND h = (HWND)lParam;
			if(wParam == BN_CLICKED) {
				if(h == FarOkButton) {
					FARWindowDone = TRUE;
				} else {
					// Read search parameters
					SendMessage(SearchTextbox , WM_GETTEXT, (WPARAM)MAX_NAME_LEN, (LPARAM)(search_text));
					SendMessage(ReplaceTextbox, WM_GETTEXT, (WPARAM)MAX_NAME_LEN, (LPARAM)(new_text   ));

					validate_mode = ladder->IsValidNumber(search_text) ? VALIDATE_IS_NUMBER : VALIDATE_IS_VAR;

					if(h == SearchNextButton) {
						matches = FindAndReplace(search_text, "", eSearchAndReplaceMode_FindNext);
						if(!matches) {
							matches = FindAndReplace(search_text, "", eSearchAndReplaceMode_FindFirst);
							if(!matches)
								Error(_("Nao encontrado!"));
						}
					} else if(h == ReplaceButton) {
						if(ladder->IsValidNameAndType(ladder->getIdIO(search_text), new_text, ladder->getDetailsIO(new_text).type, _("Substituir por"),
								validate_mode | VALIDATE_DONT_ASK | VALIDATE_TYPES_MUST_MATCH | VALIDATE_ACCEPT_IO_PENDING, 0, 0)) {
							matches = FindAndReplace(search_text, new_text, eSearchAndReplaceMode_ReplaceNext);
							if(!matches) {
								matches = FindAndReplace(search_text, new_text, eSearchAndReplaceMode_ReplaceFirst);
								if(!matches) {
									Error(_("Nao encontrado!"));
								}
							}
						}
					} else if(h == ReplaceAllButton) {
						if(ladder->IsValidNameAndType(ladder->getIdIO(search_text), new_text, ladder->getDetailsIO(new_text).type, _("Substituir por"),
								validate_mode | VALIDATE_DONT_ASK | VALIDATE_TYPES_MUST_MATCH | VALIDATE_ACCEPT_IO_PENDING, 0, 0)) {
							matches = FindAndReplace(search_text, new_text, eSearchAndReplaceMode_ReplaceAll);
							if(matches) {
								sprintf(texto, _("Encontrada(s) %d ocorrência(s)"), matches);
								ShowTaskDialog(texto, NULL, TD_INFORMATION_ICON, TDCBF_OK_BUTTON);
							} else {
								Error(_("Nao encontrado!"));
							}
						}
					}

					if(matches)
						InvalidateRect(MainWindow, NULL, FALSE);
				}

				break;
			}
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Localizar:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 16, 90, 21, FARWindow, NULL, Instance, NULL);
    NiceFont(textLabel);

    SearchTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        100, 16, 191, 321, FARWindow, NULL, Instance, NULL);
    FixedFont(SearchTextbox);

	LoadIOListToComboBox(SearchTextbox, vector<eType>()); // Vetor vazio, todos os tipos...
	SendMessage(SearchTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Substituir por:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        5, 46, 90, 21, FARWindow, NULL, Instance, NULL);
    NiceFont(textLabel2);

    ReplaceTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        100, 46, 191, 321, FARWindow, NULL, Instance, NULL);
    FixedFont(ReplaceTextbox);

	LoadIOListToComboBox(ReplaceTextbox, vector<eType>()); // Vetor vazio, todos os tipos...
	SendMessage(ReplaceTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    FarOkButton = CreateWindowEx(0, WC_BUTTON, _("Fechar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        216, 75, 75, 53, FARWindow, NULL, Instance, NULL); 
    NiceFont(FarOkButton);

	SearchNextButton = CreateWindowEx(0, WC_BUTTON, _("Localizar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        5, 75, 75, 53, FARWindow, NULL, Instance, NULL); 
    NiceFont(SearchNextButton);

    ReplaceButton = CreateWindowEx(0, WC_BUTTON, _("Substituir"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        85, 75, 126, 23, FARWindow, NULL, Instance, NULL); 
    NiceFont(ReplaceButton);

    ReplaceAllButton = CreateWindowEx(0, WC_BUTTON, _("Substituir Todos"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        85, 105, 126, 23, FARWindow, NULL, Instance, NULL); 
    NiceFont(ReplaceAllButton);
}

void MoveFARWindow(RECT r)
{
	MoveWindow(FARWindow, r.left, r.top, r.right, r.bottom, TRUE);
}

void ShowFARWindow(void)
{
	// Se janela ja exibida, apenas retorna
	if(FARWindowDone == FALSE) {
		return;
	}

	WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)FARWindowProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "POPToolsFARWindow";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    FARWindow = CreateWindowClient(WS_EX_CLIENTEDGE, "POPToolsFARWindow",
        "", WS_CHILD | WS_CLIPSIBLINGS,
        100, 100, FarWidth, FarHeight, MainWindow, NULL, Instance, NULL);

    MakeControls();
   
	SendMessage(SearchTextbox , WM_SETTEXT, 0, (LPARAM)(POPSettings.last_search_text));
	SendMessage(ReplaceTextbox, WM_SETTEXT, 0, (LPARAM)(POPSettings.last_new_text   ));

	if(IoListHeight < FarHeight) {
		IoListHeight = FarHeight;
	}

	MainWindowResized();
    ShowWindow(FARWindow, TRUE);
	SetFocus(MainWindow);

    MSG msg;
    DWORD ret;
    FARWindowDone = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !FARWindowDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_ESCAPE) {
                break;
			} else if(msg.wParam == VK_RETURN) {
				FARWindowProc(FARWindow, WM_COMMAND, BN_CLICKED, (LPARAM)SearchNextButton);
			}
		}

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	FARWindowDone = TRUE;
    DestroyWindow(FARWindow);
    return;
}
