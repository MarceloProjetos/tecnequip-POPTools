#include "poptools.h"

static HWND CommentDialog;

static HWND CommentTextbox;

static void MakeControls(void)
{
    CommentTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE |
        ES_MULTILINE | ES_WANTRETURN,
        7, 10, 600, 38, CommentDialog, NULL, Instance, NULL);
    FixedFont(CommentTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        620,  6, 70, 23, CommentDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        620, 36, 70, 23, CommentDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

bool ShowCommentDialog(char *comment, POINT ElemStart, POINT ElemSize, POINT GridSize)
{
	bool changed = false;
	POINT start = { 100, 100 }, size = { 700, 65 };

	// Se tamanho for definido, devemos posicionar a janela
	if(ElemSize.x > 0 && ElemSize.y > 0) {
		// Primeiro corrige as coordenadas para representar um valorabsoluto com relacao
		// ao canto superior esquerdo da tela ao inves do canto de DrawWindow
		RECT rWindow;

		GetWindowRect(DrawWindow, &rWindow);

		ElemStart.y += rWindow.top - ScrollYOffset * GridSize.y;

		// A janela do comentario aparece centralizada verticalmente com o comentario
		start.y = ElemStart.y + (ElemSize.y - size.y)/2;

		// Sempre centraliza com a tela pois o comentario ocupa a tela inteira
		start.x = rWindow.left + (rWindow.right - rWindow.left - size.x)/2;
	}

    CommentDialog = CreateWindowClient(0, "POPToolsDialog",
        _("Comment"), WS_OVERLAPPED | WS_SYSMENU,
        start.x, start.y, size.x, size.y, MainWindow, NULL, Instance, NULL);

    MakeControls();
   
    SendMessage(CommentTextbox, WM_SETTEXT, 0, (LPARAM)comment);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(CommentDialog, TRUE);
    SetFocus(CommentTextbox);
    SendMessage(CommentTextbox, EM_SETSEL, 0, -1);

    MSG msg;
    DWORD ret;
    DialogDone = FALSE;
    DialogCancel = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_TAB && GetFocus() == CommentTextbox) {
                SetFocus(OkButton);
                continue;
            } else if(msg.wParam == VK_ESCAPE) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
                break;
            }
        }

        if(IsDialogMessage(CommentDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
	   changed = true;

        SendMessage(CommentTextbox, WM_GETTEXT, (WPARAM)(MAX_COMMENT_LEN-1),
            (LPARAM)comment);
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(CommentDialog);

	return changed;
}
