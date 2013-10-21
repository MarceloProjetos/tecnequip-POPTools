#include "poptools.h"

HBITMAP hBitmap;
// SPLASH.cpp: implementation of the SPLASH class.
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE:
        hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SPLASH));
        if(hBitmap == NULL)
            Error(_("Could not load splash image!"));
		break;
	case WM_PAINT: {
			BITMAP bm;
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd, &ps);

			HDC hdcMem = CreateCompatibleDC(hdc);
			HGDIOBJ hbmOld = SelectObject(hdcMem, hBitmap);

			GetObject(hBitmap, sizeof(bm), &bm);

			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);

			EndPaint(hwnd, &ps);
        }
        break;
	case WM_LBUTTONDOWN:
		EnableWindow(GetParent(hwnd), TRUE);
		DestroyWindow(hwnd);
		break;
    case WM_CLOSE:
    case WM_DESTROY:
		EnableWindow(GetParent(hwnd), TRUE);
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

SPLASH::SPLASH()
{

}

SPLASH::~SPLASH()
{
 DestroyWindow(hSplashWnd);
}

void SPLASH::Init(HWND hWnd)
{
	hParentWindow=hWnd;
}

void SPLASH::Show()
{
	char* c_szSplashClass = "SplashWindow";

	WNDCLASS wc = { 0 };
    wc.lpfnWndProc = SplashWndProc;
	wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    wc.hInstance = GetModuleHandle(NULL);
    //wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_SPLASHICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = c_szSplashClass;
    RegisterClass(&wc);

	hSplashWnd=CreateWindowEx(WS_EX_CLIENTEDGE,c_szSplashClass,_("Sobre..."),
		WS_POPUPWINDOW|WS_EX_TOPMOST|WS_EX_TOOLWINDOW | WS_DLGFRAME| SS_BITMAP,440,300,550,450,hParentWindow,NULL,GetModuleHandle(NULL),NULL);

	HBITMAP hBitmap;
	hBitmap = LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_SPLASH));
	//SendMessage(hSplashWnd,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)LoadBitmap(hInst,MAKEINTRESOURCE(resid)));

	this->SHOWING = FALSE;

	//get size of hSplashWnd (width and height)
	int x,y;

	HDWP windefer;
	RECT rect,prect;
	GetClientRect(hSplashWnd,&rect);
	x=rect.right;y=rect.bottom;
	//get parent screen coordinates
	GetWindowRect(this->hParentWindow,&prect);

	//center splash window on parent window
	pwidth=prect.right-prect.left;
	pheight=prect.bottom - prect.top;

	tx=(pwidth/2) - (x/2);
	ty=(pheight/2) - (y/2);

	tx+=prect.left;
	ty+=prect.top;


	windefer=BeginDeferWindowPos(1);
	DeferWindowPos(windefer,hSplashWnd,HWND_NOTOPMOST,tx,ty,50,50,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOZORDER);
	EndDeferWindowPos(windefer);

	EnableWindow(hParentWindow, FALSE);
	ShowWindow(hSplashWnd,SW_SHOWNORMAL);
	UpdateWindow(hSplashWnd);
}

void SPLASH::Hide()
{
  ShowWindow(hSplashWnd,SW_HIDE);
}

