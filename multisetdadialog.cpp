#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>

#include "ldmicro.h"

#define MARGIN		20.0f
#define FONT_NAME	L"Verdana"
#define FONT_SIZE	20.0f

#define SHOW_GRAPH	0

extern ID2D1Factory*		pD2DFactory;
extern IDWriteFactory*		pWriteFactory;
extern IWICImagingFactory *	pWICFactory;

static HWND MultisetDADialog;
static ID2D1HwndRenderTarget*	pRenderTarget;
static ID2D1PathGeometry*		pLinePathGeometry;
static ID2D1PathGeometry*		pLineFillPathGeometry;
static IDWriteTextFormat*		pTextFormat;
static ID2D1SolidColorBrush*	pBackgroundBrush;
static ID2D1SolidColorBrush*	pAxisBrush;
static ID2D1SolidColorBrush*	pLineBrush;
static ID2D1SolidColorBrush*	pLineFillBrush;
static ID2D1SolidColorBrush*	pPointBrush;
static ID2D1SolidColorBrush*	pFontBrush;

static HWND ForwardRadio;
static HWND BackwardRadio;
static HWND SpeedUpRadio;
static HWND SpeedDownRadio;
static HWND LinearRadio;
static HWND CustomRadio;
static HWND TimeTextbox;
static HWND DeslTextbox;
static HWND ResTimeTextbox;
static HWND ResDeslTextbox;

static LONG_PTR PrevTimeNameProc;
static LONG_PTR PrevDeslNameProc;

#if SHOW_GRAPH
static HWND GraphBox;
static LONG_PTR PrevGraphBoxProc;
#endif

static ElemMultisetDA *current;

static vector<D2D1_POINT_2F>	DAPoints;

const LPCTSTR ComboboxTimeResolItens[] = { _("11"), _("12"), _("13"), _("14"), _("15"), _("16"), _("17"), _("18"), _("19"), _("20"), 
										   _("21"), _("22"), _("23"), _("24"), _("25"), _("26"), _("27"), _("28"), _("29"), _("30"),
										   _("31"), _("32"), _("33"), _("34"), _("35"), _("36"), _("37"), _("38"), _("39"), _("40"),
										   _("41"), _("42"), _("43"), _("44"), _("45"), _("46"), _("47"), _("48"), _("49"), _("50"),
										   _("51"), _("52"), _("53"), _("54"), _("55"), _("56"), _("57"), _("58"), _("59"), _("60"),
										   _("61"), _("62"), _("63"), _("64"), _("65"), _("66"), _("67"), _("68"), _("69"), _("70"),
										   _("71"), _("72"), _("73"), _("74"), _("75"), _("76"), _("77"), _("78"), _("79"), _("80"),
										   _("81"), _("82"), _("83"), _("84"), _("85"), _("86"), _("87"), _("88"), _("89"), _("90"),
										   _("91"), _("92"), _("93"), _("94"), _("95"), _("96"), _("97"), _("98"), _("99"), _("100") };

const LPCTSTR ComboboxDeslResolItens[] = { _("11"), _("12"), _("13"), _("14"), _("15"), _("16"), _("17"), _("18"), _("19"), _("20"), 
										   _("21"), _("22"), _("23"), _("24"), _("25"), _("26"), _("27"), _("28"), _("29"), _("30"),
										   _("31"), _("32"), _("33"), _("34"), _("35"), _("36"), _("37"), _("38"), _("39"), _("40"),
										   _("41"), _("42"), _("43"), _("44"), _("45"), _("46"), _("47"), _("48"), _("49"), _("50"),
										   _("51"), _("52"), _("53"), _("54"), _("55"), _("56"), _("57"), _("58"), _("59"), _("60"),
										   _("61"), _("62"), _("63"), _("64"), _("65"), _("66"), _("67"), _("68"), _("69"), _("70"),
										   _("71"), _("72"), _("73"), _("74"), _("75"), _("76"), _("77"), _("78"), _("79"), _("80"),
										   _("81"), _("82"), _("83"), _("84"), _("85"), _("86"), _("87"), _("88"), _("89"), _("90"),
										   _("91"), _("92"), _("93"), _("94"), _("95"), _("96"), _("97"), _("98"), _("99"), _("100") };


#define MAX_IO_SEEN_PREVIOUSLY 512
extern struct {
    char    name[MAX_NAME_LEN];
    int     type;
    int     pin;
	int		bit;
} IoSeenPreviously[MAX_IO_SEEN_PREVIOUSLY];
extern int IoSeenPreviouslyCount;

void CalculateDAPoints(ElemMultisetDA * l)
{
	int tm = 0;
	float factor = 0;

	for (int i = 0; i < l->desloc; i++)
	{
		tm = i * l->resolt;
		if (tm < l->time)
		{
			if (tm < l->time / 2)
			{
				factor = pow((tm * 1000.0f) / l->time, 2);
			}
			else
			{
				factor = 500000.0f - pow(1000.0f - (tm * 1000.0f) / l->time, 2);	
			}
			factor /= 1000.0f;
		}
		else
		{
			factor = 500.0f;
		}
		l->points[i] = l->desloc + (l->desloc * 2 * factor) / 1000.0f;
	}
}

#if SHOW_GRAPH
D2D1_POINT_2F PixelToDIP(LPARAM lParam)
{
    static D2D1_POINT_2F dpi = {96, 96}; // The default DPI

    if (&pD2DFactory)
    {
        pD2DFactory->GetDesktopDpi(&dpi.x, &dpi.y);
    }

    return D2D1::Point2F(
        static_cast<int>(static_cast<short>(LOWORD(lParam))) * 96 / dpi.x,
        static_cast<int>(static_cast<short>(HIWORD(lParam))) * 96 / dpi.y);
}

HRESULT CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (pRenderTarget == nullptr)
	{
		RECT rc;
		GetClientRect(GraphBox, &rc);

		D2D1_SIZE_U SizeInPixel = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);

		// Create a Direct2D render target.
		hr = pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(GraphBox, SizeInPixel),
			&pRenderTarget
			);

		if (SUCCEEDED(hr))
		{
			hr = pD2DFactory->CreatePathGeometry(&pLinePathGeometry);
		}

		if (SUCCEEDED(hr))
		{
			hr = pD2DFactory->CreatePathGeometry(&pLineFillPathGeometry);
		}

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::White),
				&pBackgroundBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&pAxisBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightBlue),
				&pLineBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSkyBlue),
				&pLineFillBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightBlue),
				&pPointBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			// Create a DirectWrite text format object.
			hr = pWriteFactory->CreateTextFormat(
				FONT_NAME,
				NULL,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				FONT_SIZE,
				L"", //locale
				&pTextFormat
				);
		}

		if (SUCCEEDED(hr))
		{
			// Center the text horizontally and vertically.
			pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

			pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}

	return hr;
}

void DiscardDeviceResources()
{
	SafeRelease(&pBackgroundBrush);
	SafeRelease(&pRenderTarget);
}

void MoveGraphPoint(D2D1_POINT_2F p)
{
}

void CapturePoint(D2D1_POINT_2F p)
{
}

void ReleasePoint(D2D1_POINT_2F p)
{
}

void Render(D2D1_RECT_F Rect)
{
    HRESULT hr;

    hr = CreateDeviceResources();

    if (SUCCEEDED(hr) && !(pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
    {
        // Retrieve the size of the render target.
        D2D1_SIZE_F sz = pRenderTarget->GetSize();

		float margin = 50.0f;

		//float scaleX = (sz.width) / ((current->time / current->resolt) + 5.0f); // (DA_RESOLUTION * current->resolt);
		//float scaleY = (sz.height) / (current->desloc + 50.0f); // (DA_RESOLUTION * current->resold);
		float scaleX = sz.width / (current->time / current->resolt); // (DA_RESOLUTION * current->resolt);
		float scaleY = sz.height / current->desloc; // (DA_RESOLUTION * current->resold);

        pRenderTarget->BeginDraw();	

        pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		D2D1_MATRIX_3X2_F m = D2D1::Matrix3x2F::Scale(scaleX, scaleY, D2D1::Point2F(0.0f, 0.0f));
		//D2D1::Matrix3x2F::Translation(scaleX, scaleY);

		pRenderTarget->SetTransform(&m);

		Rect.right /= scaleX; 
		Rect.bottom /= scaleY;

		D2D1_SIZE_F size = sz;

		size.width *= scaleX; 
		size.height *= scaleY;

		//float intervalX = (sz.width - 5.0f)  / (current->time / current->resolt);
		//float intervalY = (sz.height - 50.0f) / current->desloc;
		float intervalX = (sz.width - 5.0f)  / (current->time / current->resolt);
		float intervalY = (sz.height - 50.0f) / current->desloc;

		//pRenderTarget->PushAxisAlignedClip(Rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		// Draw background
		pRenderTarget->FillRectangle(&Rect, pBackgroundBrush);

		// Draw X, Y Axis
		//pRenderTarget->DrawLine(D2D1::Point2F(intervalX, intervalY), 
		//						D2D1::Point2F(intervalX, size.height - intervalY), pAxisBrush);
		//pRenderTarget->DrawLine(D2D1::Point2F(intervalX, size.height - intervalY), 
		//						D2D1::Point2F(size.width - intervalX, size.height - intervalY), pAxisBrush);

		//WCHAR Num[24];

		// Draw X Scale
		//for (int i = 1; i < size.width / current->resolt; i++)
		//{
		//	pRenderTarget->DrawLine(D2D1::Point2F(intervalX + ((( size.width - (intervalX * 2)) / current->resolt) * i), size.height - intervalY - 5.0f), 
		//							D2D1::Point2F(intervalX + ((( size.width - (intervalX * 2)) / current->resolt) * i), size.height - intervalY + 5.0f), pAxisBrush);

		//	D2D1_RECT_F r = D2D1::RectF(intervalX + ((( size.width - (intervalX * 2)) / current->resolt) * i) - FONT_SIZE,
		//								size.height - FONT_SIZE - 5.0f, 
		//								intervalX + ((( size.width - (intervalX * 2)) / current->resolt) * i) + FONT_SIZE, 
		//								size.height - 5.0f);

		//	StringCchPrintfW(Num, ARRAYSIZE(Num), L"%d", i);

		//	pRenderTarget->DrawText(Num, static_cast<UINT>(wcslen(Num)), pTextFormat, &r, pAxisBrush);
		//}

		// Draw Y Scale
		//for (int i = DA_RESOLUTION / current->resold; i; i--)
		//{
		//	pRenderTarget->DrawLine(D2D1::Point2F(intervalX - 5.0f, intervalY + (intervalY * i)), 
		//							D2D1::Point2F(intervalX + 5.0f, intervalY + (intervalY * i)), pAxisBrush);

		//	D2D1_RECT_F r = D2D1::RectF(5.0f, 
		//								intervalY + (intervalY * i) - FONT_SIZE, 
		//								FONT_SIZE + 5.0f, 
		//								intervalY + (intervalY * i) + FONT_SIZE);

		//	StringCchPrintfW(Num, ARRAYSIZE(Num), L"%d", i);

		//	pRenderTarget->DrawText(Num, static_cast<UINT>(wcslen(Num)), pTextFormat, &r, pAxisBrush);
		//}

		int i;
		ID2D1GeometrySink *pSink = NULL;

		hr = pLineFillPathGeometry->Open(&pSink);

		D2D1_POINT_2F point = D2D1::Point2F(intervalX, size.height - (current->points[0] - size.height));

		pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

		pSink->BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);

		for (i = 0; i < current->time / current->resolt - 1; i++)
		{
			pSink->AddLine(D2D1::Point2F(intervalX + (intervalX * i+1), size.height - (current->points[i] - size.height) - intervalY));
		}
		pSink->AddLine(D2D1::Point2F(intervalX + (intervalX * i), size.height));

        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		pRenderTarget->FillGeometry(pLineFillPathGeometry, pLineFillBrush);

		hr = pSink->Close();
		SafeRelease(&pSink);

		//hr = pLinePathGeometry->Open(&pSink);

		//pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

		//pSink->BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);

		//for (i = 0; i < current->time / current->resolt - 1; i++)
		//{
		//	pSink->AddLine(D2D1::Point2F(intervalX + (intervalX * i+1), size.height - (current->points[i] - size.height) - intervalY));
		//}
		//pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		//pRenderTarget->DrawGeometry(pLinePathGeometry, pLineBrush, 1.0f);

		//hr = pSink->Close();
		//SafeRelease(&pSink);

		//pRenderTarget->PopAxisAlignedClip();

		hr = pRenderTarget->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
            DiscardDeviceResources();
	}
}

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK GraphBoxProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
        case WM_NOTIFY:
            break;

		case WM_MOUSEMOVE:
			{
				D2D1_POINT_2F p = PixelToDIP(lParam);

				MoveGraphPoint(p);
			}
			break;

		case WM_LBUTTONDOWN:
			{
				D2D1_POINT_2F p = PixelToDIP(lParam);

				CapturePoint(p);
			}
			break;

		case WM_LBUTTONUP:
			{
				D2D1_POINT_2F p = PixelToDIP(lParam);

				ReleasePoint(p);
			}
			break;

		case WM_PAINT:
			{
				RECT r;
				GetClientRect(hwnd, &r);

				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);

				if (r.right == 0 && r.bottom == 0)
					r = ps.rcPaint;

				D2D1_RECT_F Rect = RectF(static_cast<FLOAT>(r.left), static_cast<FLOAT>(r.top), static_cast<FLOAT>(r.right), static_cast<FLOAT>(r.bottom));

				Render(Rect);

				EndPaint(hwnd, &ps); 
				//ValidateRect(hwnd, NULL);
			}
			break;
        case WM_CLOSE:
        case WM_DESTROY:
            DiscardDeviceResources();
            break;

        default:
			return CallWindowProc((WNDPROC)PrevGraphBoxProc, hwnd, msg, wParam, lParam);
    }

    return 1;
}
#endif

//-----------------------------------------------------------------------------
// Don't allow any characters other than A-Za-z0-9_ in the name.
//-----------------------------------------------------------------------------

static LRESULT CALLBACK TimeNameProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isalpha(wParam) || isdigit(wParam) || wParam == '_' || wParam == '-' ||
            wParam == '\b'))
        {
            return 0;
        }
    }

    return CallWindowProc((WNDPROC)PrevTimeNameProc, hwnd, msg, wParam, lParam);
}
static LRESULT CALLBACK DeslNameProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isalpha(wParam) || isdigit(wParam) || wParam == '_' || wParam == '-' ||
            wParam == '\b'))
        {
            return 0;
        }
    }

    return CallWindowProc((WNDPROC)PrevDeslNameProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    /*HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Tipo de Rampa"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 140, 85, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper);*/

    /*LinearRadio = CreateWindowEx(0, WC_BUTTON, _("Linear"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 18, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(LinearRadio);*/

    /*CustomRadio = CreateWindowEx(0, WC_BUTTON, _("Curva Ascendente"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 38, 125, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(CustomRadio);*/

    /*CustomRadio = CreateWindowEx(0, WC_BUTTON, _("Personalizada"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 58, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(CustomRadio);*/

    /*HWND grouper2 = CreateWindowEx(0, WC_BUTTON, _("Direção do Movimento"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        150, 3, 145, 85, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper2);*/

    /*ForwardRadio = CreateWindowEx(0, WC_BUTTON, _("Avançar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        159, 18, 100, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(ForwardRadio);*/

    /*BackwardRadio = CreateWindowEx(0, WC_BUTTON, _("Recuar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        159, 38, 100, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(BackwardRadio); */

    HWND grouper3 = CreateWindowEx(0, WC_BUTTON, _("Variação Velocidade"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 3/*300, 3*/, 140, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper3);

    SpeedUpRadio = CreateWindowEx(0, WC_BUTTON, _("Aceleração"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        16, 18/*309, 18*/, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedUpRadio);

    SpeedDownRadio = CreateWindowEx(0, WC_BUTTON, _("Desaceleração"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        16, 40/*309, 38*/, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedDownRadio); 

    HWND timeLabel = CreateWindowEx(0, WC_STATIC, _("Tempo:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        150, 20/*450, 5*/, 100, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(timeLabel);

    TimeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        250, 20/*550, 5*/, 50, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(TimeTextbox);

    HWND deslLabel = CreateWindowEx(0, WC_STATIC, _("Deslocamento:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        150, 45/*450, 25*/, 100, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(deslLabel);

    DeslTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        250, 45/*550, 25*/, 50, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(DeslTextbox);

    /*HWND restimeLabel = CreateWindowEx(0, WC_STATIC, _("Escala T (ms):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        450, 45, 100, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(restimeLabel);

    ResTimeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        550, 45, 50, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(ResTimeTextbox);

    HWND resdeslLabel = CreateWindowEx(0, WC_STATIC, _("Escala D (mm):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        450, 65, 100, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(resdeslLabel);

    ResDeslTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        550, 65, 50, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(ResDeslTextbox);*/

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        340, 15/*720, 10*/, 70, 23, MultisetDADialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        340, 45/*720, 40*/, 70, 23, MultisetDADialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    PrevTimeNameProc = SetWindowLongPtr(TimeTextbox, GWLP_WNDPROC, 
        (LONG_PTR)TimeNameProc);

	PrevDeslNameProc = SetWindowLongPtr(DeslTextbox, GWLP_WNDPROC, 
        (LONG_PTR)DeslNameProc);

#if SHOW_GRAPH
    GraphBox = CreateWindowEx(0, WC_STATIC, NULL,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        7, 100, /*984, 690*/784, 490, MultisetDADialog, NULL, Instance, NULL);
	PrevGraphBoxProc = SetWindowLongPtr(GraphBox, GWLP_WNDPROC, 
        (LONG_PTR)GraphBoxProc);
#endif


}

void ShowMultisetDADialog(ElemMultisetDA *l)
{
	current = l;

    MultisetDADialog = CreateWindowClient(0, "LDmicroDialog",
        _("MultisetDA"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, /*800, 600*/420, 80, MainWindow, NULL, Instance, NULL);
    RECT r;
    GetClientRect(MultisetDADialog, &r);

    MakeControls();

	SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(l->name));
	SendMessage(DeslTextbox, WM_SETTEXT, 0, (LPARAM)(l->name1));

	/*_itoa(l->time, num, 10);
    SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(l->desloc, num, 10);
    SendMessage(DeslTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(l->resolt, num, 10);
    SendMessage(ResTimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(l->resold, num, 10);
    SendMessage(ResDeslTextbox, WM_SETTEXT, 0, (LPARAM)(num));*/

    /*if(l->linear) {
        SendMessage(LinearRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(CustomRadio, BM_SETCHECK, BST_CHECKED, 0);
    } 
	if(l->forward) {
        SendMessage(ForwardRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(BackwardRadio, BM_SETCHECK, BST_CHECKED, 0);
    }*/ 
    if(l->speedup) {
        SendMessage(SpeedUpRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(SpeedDownRadio, BM_SETCHECK, BST_CHECKED, 0);
    }     

    EnableWindow(MainWindow, FALSE);
    ShowWindow(MultisetDADialog, TRUE);
    SetFocus(TimeTextbox);
    SendMessage(TimeTextbox, EM_SETSEL, 0, -1);

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
		if(IsDialogMessage(MultisetDADialog, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

    if(!DialogCancel) {

        /*if(SendMessage(LinearRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->linear = TRUE;
        } else if(SendMessage(CustomRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->linear = FALSE;
        } 
        if(SendMessage(ForwardRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->forward = TRUE;
        } else if(SendMessage(BackwardRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->forward = FALSE;
        }*/          
		if(SendMessage(SpeedUpRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->speedup = TRUE;
        } else if(SendMessage(SpeedDownRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->speedup = FALSE;
        }


        /*SendMessage(TimeTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(num));
		l->time = atoi(num);
        SendMessage(DeslTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(num));
		l->desloc = atoi(num);*/

		SendMessage(TimeTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(l->name));
		SendMessage(DeslTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(l->name1));

		//CalculateDAPoints(l);

		//int i;

		/**bit = 0;
		for (i = 0; i < MAX_IO_SEEN_PREVIOUSLY; i++)
		{
			if (_stricmp(IoSeenPreviously[i].name, name)==0)
				*bit = IoSeenPreviously[i].bit;
		}*/
		//char cbit[10];
		//_itoa(*bit, cbit, 10);
		//SetWindowText(BitTextbox, cbit );

    }
	//DiscardDeviceResources();
    EnableWindow(MainWindow, TRUE);
    DestroyWindow(MultisetDADialog);
    return;
}
