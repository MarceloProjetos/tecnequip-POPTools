#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <math.h>

#include "ldmicro.h"

#define MARGIN		25.0f
#define FONT_NAME	L"Verdana"
#define FONT_SIZE	12.0f

#define SHOW_GRAPH	1

extern ID2D1Factory*		pD2DFactory;
extern IDWriteFactory*		pWriteFactory;
extern IWICImagingFactory *	pWICFactory;

static HWND MultisetDADialog;
static ID2D1HwndRenderTarget*	pRenderTarget;
static ID2D1StrokeStyle *		pLineStrokeStyle;
static ID2D1TransformedGeometry * pTransformedGeometry;
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
static HWND AdjustTextbox;
static HWND ResTimeTextbox;
static HWND ResDeslTextbox;
static HWND UpdateButton;
static HWND DeslTypeCombobox;

static LONG_PTR PrevNumProc;
static LONG_PTR PrevUpdateButtonProc;
static LONG_PTR PrevDeslTypeProc;

static HWND		GraphBox;
static LONG_PTR PrevGraphBoxProc;

static int		time;
static int		desloc;
static float	points[DA_RESOLUTION];

static ElemMultisetDA *current;

static vector<D2D1_POINT_2F>	DAPoints;

//const LPCTSTR ComboboxTimeResolItens[] = { _("11"), _("12"), _("13"), _("14"), _("15"), _("16"), _("17"), _("18"), _("19"), _("20"), 
//										   _("21"), _("22"), _("23"), _("24"), _("25"), _("26"), _("27"), _("28"), _("29"), _("30"),
//										   _("31"), _("32"), _("33"), _("34"), _("35"), _("36"), _("37"), _("38"), _("39"), _("40"),
//										   _("41"), _("42"), _("43"), _("44"), _("45"), _("46"), _("47"), _("48"), _("49"), _("50"),
//										   _("51"), _("52"), _("53"), _("54"), _("55"), _("56"), _("57"), _("58"), _("59"), _("60"),
//										   _("61"), _("62"), _("63"), _("64"), _("65"), _("66"), _("67"), _("68"), _("69"), _("70"),
//										   _("71"), _("72"), _("73"), _("74"), _("75"), _("76"), _("77"), _("78"), _("79"), _("80"),
//										   _("81"), _("82"), _("83"), _("84"), _("85"), _("86"), _("87"), _("88"), _("89"), _("90"),
//										   _("91"), _("92"), _("93"), _("94"), _("95"), _("96"), _("97"), _("98"), _("99"), _("100") };

//const LPCTSTR ComboboxDeslResolItens[] = { _("11"), _("12"), _("13"), _("14"), _("15"), _("16"), _("17"), _("18"), _("19"), _("20"), 
//										   _("21"), _("22"), _("23"), _("24"), _("25"), _("26"), _("27"), _("28"), _("29"), _("30"),
//										   _("31"), _("32"), _("33"), _("34"), _("35"), _("36"), _("37"), _("38"), _("39"), _("40"),
//										   _("41"), _("42"), _("43"), _("44"), _("45"), _("46"), _("47"), _("48"), _("49"), _("50"),
//										   _("51"), _("52"), _("53"), _("54"), _("55"), _("56"), _("57"), _("58"), _("59"), _("60"),
//										   _("61"), _("62"), _("63"), _("64"), _("65"), _("66"), _("67"), _("68"), _("69"), _("70"),
//										   _("71"), _("72"), _("73"), _("74"), _("75"), _("76"), _("77"), _("78"), _("79"), _("80"),
//										   _("81"), _("82"), _("83"), _("84"), _("85"), _("86"), _("87"), _("88"), _("89"), _("90"),
//										   _("91"), _("92"), _("93"), _("94"), _("95"), _("96"), _("97"), _("98"), _("99"), _("100") };

const LPCTSTR DeslTypeItens[] = { _("Deslocamento (DA):"), _("Deslocamento (mV)"), _("Deslocamento (%)") };

#define MAX_IO_SEEN_PREVIOUSLY 512
extern struct {
    char    name[MAX_NAME_LEN];
    int     type;
    int     pin;
	int		bit;
} IoSeenPreviously[MAX_IO_SEEN_PREVIOUSLY];
extern int IoSeenPreviouslyCount;

void CalcUp(int calc_time, int calc_desloc)
{
	int tm = 0;
	float factor = 0;

	if (calc_desloc > DA_RESOLUTION || 
		calc_desloc < (DA_RESOLUTION * -1))
		return;

	memset(points, 0, sizeof(points));

	for (int i = 0; i < calc_time / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;
		if (tm < calc_time)
		{
			if (tm < calc_time / 2)
			{
				factor = pow((tm * 1000.0f) / calc_time, 2);
			}
			else
			{
				factor = 500000.0f - pow(1000.0f - (tm * 1000.0f) / calc_time, 2);	
			}
			factor /= 1000.0f;
		}
		else
		{
			factor = 500.0f;
		}
		points[i] = DA_RESOLUTION + (calc_desloc * 2 * factor) / 1000.0f;
	}
}

void CalcDown(int calc_time, int calc_desloc)
{
	int tm = 0;
	float factor = 0;

	if (calc_desloc > DA_RESOLUTION || 
		calc_desloc < (DA_RESOLUTION * -1))
		return;

	memset(points, 0, sizeof(points));

	for (int i = 0; i < calc_time / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;
		if (tm < calc_time)
		{
			if (tm < calc_time / 2)
			{
				factor = 500000.0f - pow((tm * 1000.0f) / calc_time, 2);
			}
			else
			{
				factor = pow(1000.0f - (tm * 1000.0f) / calc_time, 2);
			}
			factor /= 1000.0f;
		}
		else
		{
			factor = 0.0f;
		}
		points[i] = /*DA_RESOLUTION +*/ (calc_desloc * 2 * factor) / 1000.0f;
	}

	
}

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
			hr = pD2DFactory->CreateStrokeStyle(
				D2D1::StrokeStyleProperties(
					D2D1_CAP_STYLE_ROUND,
					D2D1_CAP_STYLE_ROUND,
					D2D1_CAP_STYLE_FLAT,
					D2D1_LINE_JOIN_ROUND,
					2.0f,
					D2D1_DASH_STYLE_SOLID,
					0.0f),
				NULL,
				0,
				&pLineStrokeStyle
				);
		}

		if (SUCCEEDED(hr))
		{
			//D2D1_COLOR_F c = D2D1::ColorF(0xf0ffff);
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::White),
				&pBackgroundBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Gray),
				&pAxisBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			D2D1_COLOR_F c = D2D1::ColorF(0x3366cc);
			hr = pRenderTarget->CreateSolidColorBrush(
				c /*D2D1::ColorF(D2D1::ColorF::LightBlue)*/,
				&pLineBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			//D2D1_COLOR_F c = D2D1::ColorF(0xc2d1f0);
			//c.a = 0.6f;
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSkyBlue, 0.5f),
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

void Render()
{
    HRESULT hr;

    hr = CreateDeviceResources();

    if (SUCCEEDED(hr) && !(pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
    {
        // Retrieve the size of the render target.
        D2D1_SIZE_F size = pRenderTarget->GetSize();

        pRenderTarget->BeginDraw();	

		// Draw Background
		pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, size.width, size.height), pBackgroundBrush);

		// Draw X Axis
		pRenderTarget->DrawLine(D2D1::Point2F(MARGIN * 2, size.height - MARGIN + 1.0f), 
								D2D1::Point2F(size.width - MARGIN, size.height - MARGIN + 1.0f), pAxisBrush);

		// Draw Y Axis
		pRenderTarget->DrawLine(D2D1::Point2F((MARGIN * 2) - 1.0f, MARGIN - 1.0f), 
								D2D1::Point2F((MARGIN * 2) - 1.0f, size.height - MARGIN + 1.0f), pAxisBrush);

		/*pRenderTarget->DrawLine(D2D1::Point2F(size.width - MARGIN, MARGIN - 1.0f), 
								D2D1::Point2F(size.width - MARGIN, size.height - MARGIN + 1.0f), pAxisBrush);*/

		WCHAR Num[24];

		// Draw X Scale
		float interval = (size.width - MARGIN * 2) / DA_CYCLE_INTERVAL;
		for (int i = 1; i < size.width / DA_CYCLE_INTERVAL; i++)
		{
			/*pRenderTarget->DrawLine(D2D1::Point2F(MARGIN - 1.0f + (interval * i), size.height - MARGIN - 1.0f - 5.0f), 
									D2D1::Point2F(MARGIN - 1.0f + (interval * i), size.height - MARGIN - 1.0f + 5.0f), pAxisBrush);*/

			D2D1_RECT_F r = D2D1::RectF(MARGIN - 1.0f + (interval * i) - (FONT_SIZE * 1.5f),
										size.height - FONT_SIZE - 5.0f, 
										MARGIN - 1.0f + (interval * i) + (FONT_SIZE * 1.5f), 
										size.height - 5.0f);

			StringCchPrintfW(Num, ARRAYSIZE(Num), L"%d", (abs(time) / DA_CYCLE_INTERVAL) * i );

			pRenderTarget->DrawText(Num, static_cast<UINT>(wcslen(Num)), pTextFormat, &r, pAxisBrush);
		}

		// Draw Y Scale
		interval = (size.height - MARGIN * 2) / 4;
		for (int i = 0; i < 5; i++)
		{
			if (i < 4)
				pRenderTarget->DrawLine(//D2D1::Point2F((MARGIN * 2) - 1.0f - 5.0f, MARGIN - 1.0f + (interval * i)), 
										//D2D1::Point2F((MARGIN * 2) - 1.0f + 5.0f, MARGIN - 1.0f + (interval * i)), 
										D2D1::Point2F((MARGIN * 2) - 1.0f, MARGIN - 1.0f + (interval * i)),
										D2D1::Point2F(size.width - MARGIN, MARGIN - 1.0f + (interval * i)), 
										pAxisBrush);

			D2D1_RECT_F r = D2D1::RectF(5.0f, 
										MARGIN - 1.0f + (interval * i) - FONT_SIZE, 
										(MARGIN * 2) - 5.0f, 
										MARGIN - 1.0f + (interval * i) + FONT_SIZE);

			StringCchPrintfW(Num, ARRAYSIZE(Num), L"%d", static_cast<int>(desloc < 0 ? DA_RESOLUTION * (0.25f * i) * -1.0f: DA_RESOLUTION * (0.25f * (4 - i))));

			pRenderTarget->DrawText(Num, static_cast<UINT>(wcslen(Num)), pTextFormat, &r, pAxisBrush);
		}

		// Scale to draw graph
		float scaleX = (size.width - (MARGIN * 3)) / abs(time);
		float scaleY = (size.height - (MARGIN * 2)) / DA_RESOLUTION; // abs(desloc);

		size.width = ceil((size.width - (MARGIN * 3)) / scaleX); 
		size.height = ceil((size.height - (MARGIN * 2)) / scaleY);

		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(scaleX, scaleY, D2D1::Point2F(0.0f, 0.0f));
		
		float intervalX = size.width / (abs(time) / DA_CYCLE_INTERVAL);
		float intervalY = (size.height - (MARGIN * 2)) / DA_RESOLUTION; // abs(desloc);

		D2D1_MATRIX_3X2_F translate = D2D1::Matrix3x2F::Translation(MARGIN * 2, MARGIN);

        pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		pRenderTarget->SetTransform(scale * translate);

		// Create graph geometry
		ID2D1GeometrySink *pSink = NULL;

		hr = pLineFillPathGeometry->Open(&pSink);

		pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

		// Calculate graphic points
		pSink->BeginFigure(D2D1::Point2F(0.0f, size.height - (current->speedup ? (desloc < 0 ? points[0] : points[0] - DA_RESOLUTION) : (desloc < 0 ? points[0] + DA_RESOLUTION : points[0]))), D2D1_FIGURE_BEGIN_FILLED);

		int i;
		for (i = 1; i < ceil(static_cast<float>(abs(time) / DA_CYCLE_INTERVAL)); i++)
		{
			pSink->AddLine(D2D1::Point2F(intervalX * i, size.height - (current->speedup ? (desloc < 0 ? points[i] : points[i] - DA_RESOLUTION) : (desloc < 0 ? points[i] + DA_RESOLUTION : points[i]))));
		}

		if (current->speedup)
		{
			pSink->AddLine(D2D1::Point2F(intervalX * i, desloc < 0 ? abs(desloc) : size.height - desloc));
			pSink->AddLine(D2D1::Point2F(intervalX * i, desloc < 0 ? 0.0f : DA_RESOLUTION));
		}
		else
		{
			pSink->AddLine(D2D1::Point2F(intervalX * i, desloc < 0 ? 0.0f : DA_RESOLUTION));
			pSink->AddLine(D2D1::Point2F(desloc < 0 ? 0.0f : 0.0f, desloc < 0 ? 0.0f : DA_RESOLUTION));
		}

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED); // D2D1_FIGURE_END_OPEN D2D1_FIGURE_END_CLOSED
		
		hr = pSink->Close();
		SafeRelease(&pSink);

		// Draw graph area
		pRenderTarget->FillGeometry(pLineFillPathGeometry, pLineFillBrush);
		//pRenderTarget->DrawGeometry(pLineFillPathGeometry, pLineFillBrush);

		// Draw graphic line
		pRenderTarget->SetTransform(translate);
		size = pRenderTarget->GetSize();

		intervalX = (size.width - (MARGIN * 3)) / (abs(time) / DA_CYCLE_INTERVAL);
		intervalY = (size.height - (MARGIN * 2)) / abs(desloc);

		float point1, point2 = 0.0f;
		for (i = 1; i < ceil(static_cast<float>(abs(time) / DA_CYCLE_INTERVAL)); i++)
		{
			if (current->speedup)
			{
				point1 = (points[i - 1] - DA_RESOLUTION) * scaleY;
				point2 = (points[i] - DA_RESOLUTION) * scaleY;

				pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), desloc < 0 ? abs(point1) : size.height - point1 - (MARGIN * 2)),
										D2D1::Point2F(intervalX * i, desloc < 0 ? abs(point2) : size.height - point2 - (MARGIN * 2)), 
										pLineBrush, 4.0f, pLineStrokeStyle);
			}
			else
			{
				point1 = points[i - 1] * scaleY;
				point2 = points[i] * scaleY;

				pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), desloc < 0 ? abs(point1) : size.height - point1 - (MARGIN * 2)),
										D2D1::Point2F(intervalX * i, desloc < 0 ? abs(point2) : size.height - point2 - (MARGIN * 2)), 
										pLineBrush, 4.0f, pLineStrokeStyle);
			}

		}

		if (current->speedup)
		{
			point1 = (points[i - 1] - DA_RESOLUTION) * scaleY;

			pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), desloc < 0 ? abs(point1) : size.height - point1 - (MARGIN * 2)),
									D2D1::Point2F(size.width - (MARGIN * 3) - 1.0f, desloc < 0 ? abs(desloc * scaleY) : size.height - (desloc * scaleY) - (MARGIN * 2)), 
									pLineBrush, 4.0f, pLineStrokeStyle);
		}
		else
		{
			point1 = points[i - 1] * scaleY;

			pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), desloc < 0 ?  abs(point1) : size.height - point1 - (MARGIN * 2)),
									D2D1::Point2F(size.width - (MARGIN * 3) - 1.0f, desloc < 0 ? 0.0f : size.height - (MARGIN * 2)), 
									pLineBrush, 4.0f, pLineStrokeStyle);
		}

		// Translate geometry
		/*D2D1_MATRIX_3X2_F transform = scale * translate;

		hr = pD2DFactory->CreateTransformedGeometry(
			 pLineFillPathGeometry,
			 &transform,
			 &pTransformedGeometry
			 );

		pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		pRenderTarget->FillGeometry(pTransformedGeometry, pLineFillBrush);

		SafeRelease(&pTransformedGeometry);*/

		/*for (i = 0; i < (time / DA_CYCLE_INTERVAL) - 1; i++)
		{
			pRenderTarget->DrawLine(D2D1::Point2F(intervalX * i, size.height - (points[i] - size.height) - intervalY),
									D2D1::Point2F(intervalX * i, size.height - (points[i + 1] - size.height) - intervalY), 
									pLineBrush, 5, pLineStrokeStyle);
		}*/

		// Draw points
		/*for (i = 1; i < time / DA_CYCLE_INTERVAL; i++)
		{
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(intervalX * i, size.height - (points[i] - size.height) - intervalY), 5.0f, 5.0f), pPointBrush);
		}*/

		//hr = pLinePathGeometry->Open(&pSink);

		//pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

		//pSink->BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);

		//for (i = 0; i < time / DA_CYCLE_INTERVAL - 1; i++)
		//{
		//	pSink->AddLine(D2D1::Point2F(intervalX + (intervalX * i+1), size.height - (points[i] - size.height) - intervalY));
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

				//D2D1_RECT_F Rect = RectF(static_cast<FLOAT>(r.left), static_cast<FLOAT>(r.top), static_cast<FLOAT>(r.right), static_cast<FLOAT>(r.bottom));

				Render(/*Rect*/);

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

//-----------------------------------------------------------------------------
// Don't allow any characters other than A-Za-z0-9_ in the name.
//-----------------------------------------------------------------------------

static LRESULT CALLBACK NumberProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    if(msg == WM_CHAR) {
        if(!(isdigit(wParam) || wParam == '-' || wParam == '\b')) {
            return 0;
        }
    }

	/*LONG_PTR t;
    if(hwnd == TimeTextbox)
        t = PrevTimeNameProc;
    else if(hwnd == DeslTextbox)
        t = PrevDeslNameProc;
    else
        oops();*/

    return CallWindowProc((WNDPROC)PrevNumProc, hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK UpdateButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char num[12];

	switch(msg)
	{
	case WM_COMMAND:
		{
		SendMessage(TimeTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(num));
		time = atoi(num);

		SendMessage(DeslTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(num));
		desloc = atoi(num);

		if(SendMessage(LinearRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			current->linear = TRUE;
		} else if(SendMessage(CustomRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			current->linear = FALSE;
		} 
		if(SendMessage(ForwardRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			current->forward = TRUE;
		} else if(SendMessage(BackwardRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			current->forward = FALSE;
		}          
		if(SendMessage(SpeedUpRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			current->speedup = TRUE;
		} else if(SendMessage(SpeedDownRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
			current->speedup = FALSE;
		}

		if (current->speedup)
			CalcUp(time, desloc);
		else
			CalcDown(time, desloc);

		InvalidateRect(GraphBox, NULL, FALSE);

		return 1;
		}
	};

    return CallWindowProc((WNDPROC)PrevUpdateButtonProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Tipo de Rampa"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 140, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper);

    LinearRadio = CreateWindowEx(0, WC_BUTTON, _("Linear"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 20, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(LinearRadio);

    CustomRadio = CreateWindowEx(0, WC_BUTTON, _("Curva"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 42, 125, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(CustomRadio);

    /*CustomRadio = CreateWindowEx(0, WC_BUTTON, _("Personalizada"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 58, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(CustomRadio);*/

    HWND grouper2 = CreateWindowEx(0, WC_BUTTON, _("Direção do Movimento"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        150, 3, 145, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper2);

    ForwardRadio = CreateWindowEx(0, WC_BUTTON, _("Avançar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        159, 20, 100, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(ForwardRadio);

    BackwardRadio = CreateWindowEx(0, WC_BUTTON, _("Recuar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        159, 42, 100, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(BackwardRadio); 

    HWND grouper3 = CreateWindowEx(0, WC_BUTTON, _("Variação Velocidade"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        /*7, 3*/300, 3, 140, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper3);

    SpeedUpRadio = CreateWindowEx(0, WC_BUTTON, _("Aceleração"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        /*16, 18*/309, 20, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedUpRadio);

    SpeedDownRadio = CreateWindowEx(0, WC_BUTTON, _("Desaceleração"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        /*16, 40*/309, 42, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedDownRadio); 

    HWND timeLabel = CreateWindowEx(0, WC_STATIC, _("Tempo (ms):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        /*150, 20*/445, 10, 150, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(timeLabel);

    TimeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        /*250, 20*/600, 10, 50, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(TimeTextbox);

	DeslTypeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        445, 33, 150, 100, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(DeslTypeCombobox);

    HWND adjustLabel = CreateWindowEx(0, WC_STATIC, _("(Fator Ajuste ???):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        445, 62, 150, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(adjustLabel);

    AdjustTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        600, 60, 50, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(AdjustTextbox);

    //HWND deslLabel = CreateWindowEx(0, WC_STATIC, _("Deslocamento:"),
    //    WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
    //    /*150, 45*/450, 35, 100, 21, MultisetDADialog, NULL, Instance, NULL);
    //NiceFont(deslLabel);

    DeslTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        /*250, 45*/600, 35, 50, 21, MultisetDADialog, NULL, Instance, NULL);
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
        /*340, 15*/720, 10, 70, 23, MultisetDADialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        /*340, 45*/720, 40, 70, 23, MultisetDADialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    UpdateButton = CreateWindowEx(0, WC_BUTTON, _("Atualizar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        /*340, 45*/720, 70, 70, 23, MultisetDADialog, NULL, Instance, NULL); 
    NiceFont(UpdateButton);

    GraphBox = CreateWindowEx(0, WC_STATIC, NULL,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        7, 100, /*984, 690*/784, 490, MultisetDADialog, NULL, Instance, NULL);

    PrevUpdateButtonProc = SetWindowLongPtr(UpdateButton, GWLP_WNDPROC, 
        (LONG_PTR)UpdateButtonProc);

    PrevNumProc = SetWindowLongPtr(TimeTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevNumProc = SetWindowLongPtr(DeslTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevNumProc = SetWindowLongPtr(AdjustTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevGraphBoxProc = SetWindowLongPtr(GraphBox, GWLP_WNDPROC, 
        (LONG_PTR)GraphBoxProc);

}

void ShowMultisetDADialog(ElemMultisetDA *l)
{
	char num[12];
	current = l;

    MultisetDADialog = CreateWindowClient(0, "LDmicroDialog",
        _("Rampa de Aceleração/Desaceleração"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 800, 600/*420, 80*/, MainWindow, NULL, Instance, NULL);
    RECT r;
    GetClientRect(MultisetDADialog, &r);

    MakeControls();

	for (int i = 0; i < sizeof(DeslTypeItens) / sizeof(DeslTypeItens[0]); i++)
		SendMessage(DeslTypeCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)DeslTypeItens[i]));

	SendMessage(DeslTypeCombobox, CB_SETCURSEL, l->type, 0);
	SendMessage(DeslTypeCombobox, CB_SETDROPPEDWIDTH, 150, 0);

	SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(l->name));
	SendMessage(DeslTextbox, WM_SETTEXT, 0, (LPARAM)(l->name1));

	time = l->time;
	desloc = l->desloc;

	if (l->type == 1)  // (mV)
		desloc = static_cast<int>(DA_RESOLUTION * (l->desloc / DA_VOLTAGE));
	else if (l->type == 2) // (%)
		desloc = static_cast<int>(DA_RESOLUTION * (l->desloc / 100.0f));
	else // DA 
		desloc = l->desloc;

	if (l->speedup)
		CalcUp(time, desloc);
	else
		CalcDown(time, desloc);

	_itoa(l->time, num, 10);
    SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(l->desloc, num, 10);
    SendMessage(DeslTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(l->resolt, num, 10);

    SendMessage(ResTimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(l->resold, num, 10);
    SendMessage(ResDeslTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(l->adjust, num, 10);
    SendMessage(AdjustTextbox, WM_SETTEXT, 0, (LPARAM)(num));

    if(l->linear) {
        SendMessage(LinearRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(CustomRadio, BM_SETCHECK, BST_CHECKED, 0);
    } 
	if(l->forward) {
        SendMessage(ForwardRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(BackwardRadio, BM_SETCHECK, BST_CHECKED, 0);
    } 
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

        if(SendMessage(LinearRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->linear = TRUE;
        } else if(SendMessage(CustomRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->linear = FALSE;
        } 
        if(SendMessage(ForwardRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->forward = TRUE;
        } else if(SendMessage(BackwardRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->forward = FALSE;
        }          
		if(SendMessage(SpeedUpRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->speedup = TRUE;
        } else if(SendMessage(SpeedDownRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
            l->speedup = FALSE;
        }

        SendMessage(TimeTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(num));
		l->time = max(30, min(2000, abs(atoi(num))));

		l->type = SendMessage(DeslTypeCombobox, CB_GETCURSEL, 0, 0);

		SendMessage(DeslTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(num));

		if (l->type == 1)  // (mV)
			l->desloc = min(10000, abs(atoi(num)));
		else if (l->type == 2) // (%)
			l->desloc = min(100, abs(atoi(num)));
		else // DA 
			l->desloc = min(DA_RESOLUTION, abs(atoi(num)));

		l->desloc = l->forward ?  l->desloc : l->desloc * -1;

		SendMessage(AdjustTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(num));
		l->adjust = abs(atoi(num));

		_itoa(l->time, l->name, 10);
		_itoa(l->desloc, l->name1, 10);

		//SendMessage(TimeTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(l->name));
		//SendMessage(DeslTextbox, WM_GETTEXT, (WPARAM)16, (LPARAM)(l->name1));

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
