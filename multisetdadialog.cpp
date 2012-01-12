#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <math.h>

#include "ldmicro.h"

#define MARGIN		25.0f
#define FONT_NAME	L"Verdana"
#define FONT_SIZE	12.0f

#define MAX_GAIN_TIME_VAL			49	// Max gain in %
#define MAX_GAIN_RESOLUTION_VAL	MAX_GAIN_TIME_VAL
#define MAX_TIME_VAL				10000
#define MAX_MILIVOLT_VAL			10000
#define MIN_TIME_VAL				30

extern ID2D1Factory*		pD2DFactory;
extern IDWriteFactory*		pWriteFactory;
extern IWICImagingFactory *	pWICFactory;

static HWND MultisetDADialog;
static ID2D1HwndRenderTarget*	pRenderTarget;
static ID2D1StrokeStyle *		pLineStrokeStyle;
static ID2D1StrokeStyle *		pDashStrokeStyle;
static ID2D1TransformedGeometry * pTransformedGeometry;
static ID2D1PathGeometry*		pLinePathGeometry;
static ID2D1PathGeometry*		pLineFillPathGeometry;
static IDWriteTextFormat*		pTextFormat;
static ID2D1SolidColorBrush*	pBackgroundBrush;
static ID2D1SolidColorBrush*	pAxisBrush;
static ID2D1SolidColorBrush*	pLineBrush;
static ID2D1SolidColorBrush*	pGainBrush;
static ID2D1SolidColorBrush*	pLineFillBrush;
static ID2D1SolidColorBrush*	pPointBrush;
static ID2D1SolidColorBrush*	pFontBrush;

static HWND ForwardRadio;
static HWND BackwardRadio;
static HWND SpeedUpRadio;
static HWND SpeedDownRadio;
static HWND LinearRadio;
static HWND CurveRadio;
static HWND TimeTextbox;
static HWND InitValTextbox;
static HWND GainInitValTextbox;
static HWND GainTimeTextbox;
static HWND UpdateButton;
static HWND ResolTypeCombobox;
static HWND GraphBox;

static LONG_PTR PrevMultisetDADialogProc;
static LONG_PTR PrevNumProc;
static LONG_PTR PrevResolTypeProc;
static LONG_PTR PrevGraphBoxProc;

static int		time;
static int		initval;
static int		gainr;
static int		gaint;
static int		gain_idx;
static float	points[DA_RESOLUTION];
static float	gains[DA_RESOLUTION];

static ElemMultisetDA current;

static vector<D2D1_POINT_2F>	DAPoints;

const LPCTSTR ResollTypeItens[] = { _("Resolução DA (12 bits)[2047 ~ -2048]"), _("Resolução DA (mV)[10000 ~ -10000]"), _("Resolução DA (%)[0 ~ 100]") };

#define MAX_IO_SEEN_PREVIOUSLY 512
extern struct {
    char    name[MAX_NAME_LEN];
    int     type;
    int     pin;
	int		bit;
} IoSeenPreviously[MAX_IO_SEEN_PREVIOUSLY];
extern int IoSeenPreviouslyCount;

void CalcLinearUp(int calc_time, int calc_initval)
{
	int tm = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	float t = static_cast<float>(calc_time);
	float d = static_cast<float>(calc_initval);

	memset(points, 0, sizeof(points));

	float a = d / t; // coeficiente angular
	float l = 0.0f; // coeficiente linear

	for (int i = 0; i < calc_time / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;
		points[i] = DA_RESOLUTION + ((tm * a) + l); // y = a.x + b
	}
}

void CalcLinearDown(int calc_time, int calc_initval)
{
	int tm = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	float t = static_cast<float>(calc_time);
	float d = static_cast<float>(calc_initval);

	memset(points, 0, sizeof(points));

	float a = abs(d) / t; // coeficiente angular
	float l = static_cast<float>(calc_initval); // coeficiente linear

	if (d < 0)
	{
		for (int i = 0; i < calc_time / DA_CYCLE_INTERVAL; i++)
		{
			tm = i * DA_CYCLE_INTERVAL;
			points[i] = ((tm * a) + l); // y = a.x + b
		}
	}
	else
	{
		a *= -1.0f;
		for (int i = 0; i < calc_time / DA_CYCLE_INTERVAL; i++)
		{
			tm = i * DA_CYCLE_INTERVAL;
			points[i] = ((tm * a) + l); // y = a.x + b
		}
	}

}

void CalcGainDown(int calc_time, int calc_initval, int calc_gainx, int calc_gainy = 5)
{
	int tm = 0;
	int i = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	float gx = static_cast<float>(calc_gainx);
	float gy = static_cast<float>(calc_gainy);
	float t = static_cast<float>(calc_time);
	float d = static_cast<float>(calc_initval);

	memset(gains, 0, sizeof(gains));

	// calcula os pontos da linha da curva de ganho proporcional
	float gd = d * (gy / 100.0f);	// % altura (y) da curva de ganho
	float gt = t * (gx / 100.0f);	// % largura (x) do tempo
	float gdeltay = (d - gd) - d;	// DeltaY = (Yb - Ya)
	float gdeltax = gt;				// DeltaX = (Xb - Xa)
	float ga = gdeltay / gdeltax;	// coeficiente angular (a = DeltaY / DeltaX)
	float gb = d;					// coeficiente linear (b = y - ax)

	for (i = 0; i < gt / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;
		gains[i] = ((tm * ga) + gb); // y = a.x + b
	}

	// calcula os pontos da linha de aceleração/desaceleração
	float deltax = (t * (1.0f - (gx / 100.0f))) - (t * (gx / 100.0f));	// DeltaY = (Yb - Ya)
	float deltay = gd - (d - gd);										// DeltaX = (Xb - Xa)
	float a = deltay / deltax;											// coeficiente angular (a = DeltaY / DeltaX)
	float y = (d - gd);													// um ponto qualquer no eixo y
	float ax = (a * (t * (gx / 100.0f)));								// a * x
	float b = y - ax;													// coeficiente linear (b = y - ax)

	for (; i < (t - gt) / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;

		gains[i] = ((tm * a) + b); // y = a.x + b
	}

	// calcula os pontos da linha da curva de ganho
	gdeltay = gd;					// DeltaY = (Yb - Ya)
	gdeltax = (t - gt) - t;			// DeltaX = (Xb - Xa)
	ga = gdeltay / gdeltax;			// coeficiente angular (a = DeltaY / DeltaX)
	gb = gd - (ga * (t - gt));		// coeficiente linear (b = y - ax)

	gain_idx = i;

	for (; i < t / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;
		gains[i] = ((tm * ga) + gb); // y = a.x + b
	}

}

void CalcCurveUp(int calc_time, int calc_initval)
{
	int tm = 0;
	float factor = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
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
		points[i] = DA_RESOLUTION + (calc_initval * 2 * factor) / 1000.0f;
	}
}

void CalcCurveDown(int calc_time, int calc_initval)
{
	int tm = 0;
	float factor = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
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
		points[i] = /*DA_RESOLUTION +*/ (calc_initval * 2 * factor) / 1000.0f;
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
				D2D1::ColorF(D2D1::ColorF::Red),
				&pGainBrush
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
			
			D2D1_STROKE_STYLE_PROPERTIES properties = D2D1::StrokeStyleProperties();
			properties.dashStyle = D2D1_DASH_STYLE_DASH;

			/*float dashes[] =
			{
			2.0f, 1.0f,
			3.0f, 1.0f,
			};*/

			//hr = m_pDrawArea->Factory->CreateStrokeStyle(properties, dashes, _countof(dashes), &m_strokeStyle);
			pD2DFactory->CreateStrokeStyle(properties, NULL, NULL, &pDashStrokeStyle);
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
	SafeRelease(&pLineStrokeStyle);
	SafeRelease(&pTransformedGeometry);
	SafeRelease(&pLinePathGeometry);
	SafeRelease(&pLineFillPathGeometry);
	SafeRelease(&pTextFormat);
	SafeRelease(&pBackgroundBrush);
	SafeRelease(&pAxisBrush);
	SafeRelease(&pLineBrush);
	SafeRelease(&pLineFillBrush);
	SafeRelease(&pPointBrush);
	SafeRelease(&pFontBrush);
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

		// Draw Text X Scale
		float interval = (size.width - MARGIN * 2) / DA_CYCLE_INTERVAL;
		for (int i = 1; i < size.width / DA_CYCLE_INTERVAL; i++)
		{
			pRenderTarget->DrawLine(D2D1::Point2F(MARGIN - 1.0f + (interval * i), size.height - MARGIN + 1.0f - 5.0f), 
									D2D1::Point2F(MARGIN - 1.0f + (interval * i), size.height - MARGIN + 1.0f + 5.0f), pAxisBrush);

			D2D1_RECT_F r = D2D1::RectF(MARGIN - 1.0f + (interval * i) - (FONT_SIZE * 2.0f),
										size.height - FONT_SIZE - 5.0f, 
										MARGIN - 1.0f + (interval * i) + (FONT_SIZE * 2.0f), 
										size.height - 5.0f);

			StringCchPrintfW(Num, ARRAYSIZE(Num), L"%d", (abs(time) / DA_CYCLE_INTERVAL) * i );

			pRenderTarget->DrawText(Num, static_cast<UINT>(wcslen(Num)), pTextFormat, &r, pAxisBrush);
		}

		// Draw Y Scale
		interval = (size.height - MARGIN * 2) / 4;
		for (int i = 0; i < 5; i++)
		{
			if (i < 4)
				pRenderTarget->DrawLine(D2D1::Point2F((MARGIN * 2) - 1.0f, MARGIN - 1.0f + (interval * i)),
										D2D1::Point2F(size.width - MARGIN, MARGIN - 1.0f + (interval * i)), 
										pAxisBrush);

			D2D1_RECT_F r = D2D1::RectF(5.0f, 
										MARGIN - 1.0f + (interval * i) - FONT_SIZE, 
										(MARGIN * 2) - 5.0f, 
										MARGIN - 1.0f + (interval * i) + FONT_SIZE);

			StringCchPrintfW(Num, ARRAYSIZE(Num), L"%d", static_cast<int>(initval < 0 ? DA_RESOLUTION * (0.25f * i) * -1.0f: DA_RESOLUTION * (0.25f * (4 - i))));

			pRenderTarget->DrawText(Num, static_cast<UINT>(wcslen(Num)), pTextFormat, &r, pAxisBrush);
		}

		// Scale to draw graph
		float scaleX = (size.width - (MARGIN * 3)) / abs(time);
		float scaleY = (size.height - (MARGIN * 2)) / DA_RESOLUTION; // abs(initval);

		size.width = ceil((size.width - (MARGIN * 3)) / scaleX); 
		size.height = ceil((size.height - (MARGIN * 2)) / scaleY);

		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(scaleX, scaleY, D2D1::Point2F(0.0f, 0.0f));
		
		float intervalX = size.width / (abs(time) / DA_CYCLE_INTERVAL);
		float intervalY = (size.height - (MARGIN * 2)) / DA_RESOLUTION; // abs(initval);

		D2D1_MATRIX_3X2_F translate = D2D1::Matrix3x2F::Translation(MARGIN * 2, MARGIN);

        pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		pRenderTarget->SetTransform(scale * translate);

		// Create graph geometry
		ID2D1GeometrySink *pSink = NULL;

		hr = pLineFillPathGeometry->Open(&pSink);

		pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

		// Calculate graphic points
		pSink->BeginFigure(D2D1::Point2F(0.0f, size.height - (current.speedup ? (initval < 0 ? points[0] : points[0] - DA_RESOLUTION) : (initval < 0 ? points[0] + DA_RESOLUTION : points[0]))), D2D1_FIGURE_BEGIN_FILLED);

		int i;
		float p;
		for (i = 1; i < ceil(static_cast<float>(abs(time) / DA_CYCLE_INTERVAL)); i++)
		{
			p = current.linear && !current.speedup ? gains[i] : points[i];
			pSink->AddLine(D2D1::Point2F(intervalX * i, size.height - (current.speedup ? (initval < 0 ? p : p - DA_RESOLUTION) : (initval < 0 ? p + DA_RESOLUTION : p))));
		}

		if (current.speedup)
		{
			pSink->AddLine(D2D1::Point2F(intervalX * i, initval < 0 ? abs(initval) : size.height - initval));
			pSink->AddLine(D2D1::Point2F(intervalX * i, initval < 0 ? 0.0f : DA_RESOLUTION));
		}
		else
		{
			pSink->AddLine(D2D1::Point2F(intervalX * i, initval < 0 ? 0.0f : DA_RESOLUTION));
			pSink->AddLine(D2D1::Point2F(initval < 0 ? 0.0f : 0.0f, initval < 0 ? 0.0f : DA_RESOLUTION));
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
		intervalY = (size.height - (MARGIN * 2)) / abs(initval);

		float point1, point2 = 0.0f;
		for (i = 1; i < ceil(static_cast<float>(abs(time) / DA_CYCLE_INTERVAL)); i++)
		{
			if (current.speedup)
			{
				point1 = (points[i - 1] - DA_RESOLUTION) * scaleY;
				point2 = (points[i] - DA_RESOLUTION) * scaleY;

				pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), initval < 0 ? abs(point1) : size.height - point1 - (MARGIN * 2)),
										D2D1::Point2F(intervalX * i, initval < 0 ? abs(point2) : size.height - point2 - (MARGIN * 2)), 
										pLineBrush, current.linear & !current.speedup ? 2.0f : 4.0f, current.linear & !current.speedup ? pDashStrokeStyle : pLineStrokeStyle);
			}
			else
			{
				point1 = points[i - 1] * scaleY;
				point2 = points[i] * scaleY;

				pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), initval < 0 ? abs(point1) : size.height - point1 - (MARGIN * 2)),
										D2D1::Point2F(intervalX * i, initval < 0 ? abs(point2) : size.height - point2 - (MARGIN * 2)), 
										pLineBrush, current.linear & !current.speedup ? 2.0f : 4.0f, current.linear & !current.speedup ? pDashStrokeStyle : pLineStrokeStyle);
			}

		}

		if (current.speedup)
		{
			point1 = (points[i - 1] - DA_RESOLUTION) * scaleY;

			pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), initval < 0 ? abs(point1) : size.height - point1 - (MARGIN * 2)),
									D2D1::Point2F(size.width - (MARGIN * 3) - 1.0f, initval < 0 ? abs(initval * scaleY) : size.height - (initval * scaleY) - (MARGIN * 2)), 
									pLineBrush, current.linear & !current.speedup ? 2.0f : 4.0f, current.linear & !current.speedup ? pDashStrokeStyle : pLineStrokeStyle);
		}
		else
		{
			point1 = points[i - 1] * scaleY;

			pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), initval < 0 ?  abs(point1) : size.height - point1 - (MARGIN * 2)),
									D2D1::Point2F(size.width - (MARGIN * 3) - 1.0f, initval < 0 ? 0.0f : size.height - (MARGIN * 2)), 
									pLineBrush, current.linear & !current.speedup ? 2.0f : 4.0f, current.linear & !current.speedup ? pDashStrokeStyle : pLineStrokeStyle);
		}

		if (current.linear)
		{

			for (i = 1; i < ceil(static_cast<float>(abs(time) / DA_CYCLE_INTERVAL)); i++)
			{
				if (!current.speedup)
				{
					point1 = gains[i - 1] * scaleY;
					point2 = gains[i] * scaleY;
					
					pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), initval < 0 ? abs(point1) : size.height - point1 - (MARGIN * 2)),
											D2D1::Point2F(intervalX * i, initval < 0 ? abs(point2) : size.height - point2 - (MARGIN * 2)), 
											i > gain_idx ? pGainBrush : pLineBrush, 4.0f, pLineStrokeStyle);
				}

			}

			if (!current.speedup)
			{
				point1 = gains[i - 1] * scaleY;

				pRenderTarget->DrawLine(D2D1::Point2F(intervalX * (i - 1), initval < 0 ?  abs(point1) : size.height - point1 - (MARGIN * 2)),
										D2D1::Point2F(size.width - (MARGIN * 3) - 1.0f, initval < 0 ? 0.0f : size.height - (MARGIN * 2)), 
										pGainBrush, 4.0f, pLineStrokeStyle);

				if (current.type == 1)  // (mV)
					point1 = (current.initval < 0 ? (DA_RESOLUTION * (abs(current.initval) / DA_VOLTAGE)) / 2 : DA_RESOLUTION - ((DA_RESOLUTION * (abs(current.initval) / DA_VOLTAGE)) / 2)) * scaleY;
				else if (current.type == 2) // (%)
					point1 = (current.initval < 0 ? (DA_RESOLUTION * (abs(current.initval) / 100.0f)) / 2 : DA_RESOLUTION - ((DA_RESOLUTION * (abs(current.initval) / 100.0f)) / 2)) * scaleY;
				else // DA 
					point1 = (current.initval < 0 ? abs(current.initval) / 2 : DA_RESOLUTION - (abs(current.initval) / 2)) * scaleY;

				pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F((current.time / 2) * scaleX, point1), 4.0f, 4.0f),
										pLineBrush);
			}
		}

		hr = pRenderTarget->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
            DiscardDeviceResources();
	}
}

void UpdateWindow(void)
{
	char msg[512];
	char num[12];

	if(SendMessage(LinearRadio, BM_GETCHECK, 0, 0) & BST_CHECKED && !SendMessage(CurveRadio, BM_GETCHECK, 0, 0) & BST_CHECKED)
		current.linear = TRUE;
	else 
		current.linear = FALSE;

	if(SendMessage(ForwardRadio, BM_GETSTATE, 0, 0) & BST_CHECKED && !SendMessage(BackwardRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) 
		current.forward = TRUE;
	else
		current.forward = FALSE;

	if(SendMessage(SpeedUpRadio, BM_GETSTATE, 0, 0) & BST_CHECKED && !SendMessage(SpeedDownRadio, BM_GETSTATE, 0, 0) & BST_CHECKED)
		current.speedup = TRUE;
	else
		current.speedup = FALSE;

	SendMessage(GainTimeTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));

	if (atoi(num) > MAX_GAIN_TIME_VAL)
	{
		_itoa(MAX_GAIN_TIME_VAL, num, 10);
		SendMessage(GainTimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
		StringCchPrintf(msg, sizeof(msg), _("O valor máximo permitido para o campo Tempo (%%) da Curva de Ganho é %d."), MAX_GAIN_TIME_VAL);
		MessageBox(MultisetDADialog, msg, "Valor inválido no campo Tempo (%) !", MB_OK | MB_ICONEXCLAMATION);
	}

	current.gaint = max(0, min(MAX_GAIN_TIME_VAL, abs(atoi(num))));
	gaint = current.gaint;

	SendMessage(GainInitValTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));

	if (atoi(num) > MAX_GAIN_RESOLUTION_VAL || atoi(num) > current.gaint)
	{
		_itoa(min(current.gaint, MAX_GAIN_RESOLUTION_VAL), num, 10);
		SendMessage(GainInitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));
		StringCchPrintf(msg, sizeof(msg), _("O valor máximo permitido para o campo Resolução (%%) da Curva de Ganho é %s."), num);
		MessageBox(MultisetDADialog, msg, "Valor inválido no campo Resolução (%) !", MB_OK | MB_ICONEXCLAMATION);
	}

	current.gainr = max(0, min(MAX_GAIN_RESOLUTION_VAL, abs(atoi(num))));
	current.gainr = min(current.gaint, current.gainr);
	gainr = current.gainr;

	SendMessage(TimeTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));

	if (atoi(num) > MAX_TIME_VAL || atoi(num) < MIN_TIME_VAL)
	{
		_itoa(max(MIN_TIME_VAL, min(MAX_TIME_VAL, abs(atoi(num)))), num, 10);
		SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
		StringCchPrintf(msg, sizeof(msg), _("O valor permitido para o campo Tempo (ms) no Tamanho da Rampa esta entre %d e %d."), MIN_TIME_VAL, MAX_TIME_VAL);
		MessageBox(MultisetDADialog, msg, "Valor inválido no campo Tempo (ms) !", MB_OK | MB_ICONEXCLAMATION);
	}

	current.time = max(MIN_TIME_VAL, min(MAX_TIME_VAL, abs(atoi(num))));
	time = current.time;

	current.type = SendMessage(ResolTypeCombobox, CB_GETCURSEL, 0, 0);

	SendMessage(InitValTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));

	if (current.type == 1)  // (mV)
	{
		current.initval = max(static_cast<int>(MAX_MILIVOLT_VAL * 0.1f), min(MAX_MILIVOLT_VAL, abs(atoi(num))));
	}
	else if (current.type == 2) // (%)
	{
		current.initval = max(10, min(100, abs(atoi(num))));
	}
	else // DA 
	{
		current.initval = max(static_cast<int>(DA_RESOLUTION * 0.1f), min(DA_RESOLUTION, abs(atoi(num))));
		if (current.forward && current.initval >= DA_RESOLUTION)
			current.initval = DA_RESOLUTION - 1;
	}
	
	if ((current.type == 0 && (((current.forward && atoi(num) > DA_RESOLUTION - 1) || (!current.forward && atoi(num) > DA_RESOLUTION)) || atoi(num) < static_cast<int>(DA_RESOLUTION * 0.1f))) ||
		(current.type == 1 && (atoi(num) > MAX_MILIVOLT_VAL || atoi(num) < static_cast<int>(MAX_MILIVOLT_VAL * 0.1f))) || 
		(current.type == 2 && (atoi(num) > 100 || atoi(num) < 10)))
	{
		_itoa(current.initval, num, 10);
		SendMessage(InitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));
		StringCchPrintf(msg, sizeof(msg), _("O valor permitido para o campo Resolução DA no Tamanho da Rampa esta entre %d e %d."), current.type == 0 ? static_cast<int>(DA_RESOLUTION * 0.1f) : (current.type == 1 ? static_cast<int>(MAX_MILIVOLT_VAL * 0.1f)  : 10), current.type == 0 ? DA_RESOLUTION - 1 : (current.type == 1 ? MAX_MILIVOLT_VAL : 100));
		MessageBox(MultisetDADialog, msg, "Valor inválido no campo Resolução DA !", MB_OK | MB_ICONEXCLAMATION);
	}

	if (current.type == 1)  // (mV)
		initval = static_cast<int>(DA_RESOLUTION * (current.initval / DA_VOLTAGE));
	else if (current.type == 2) // (%)
		initval = static_cast<int>(DA_RESOLUTION * (current.initval / 100.0f));
	else // DA 
		initval = current.initval;
		
	initval = current.forward ? initval : initval * -1;

	_itoa(current.time, num, 10);
	SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(current.initval, num, 10);
	SendMessage(InitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));

	_itoa(current.gainr, num, 10);
	SendMessage(GainInitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(current.gaint, num, 10);
	SendMessage(GainTimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	
	_itoa(current.time, current.name, 10);
	_itoa(current.initval, current.name1, 10);

	if (current.linear)
	{
		if (current.speedup)
			CalcLinearUp(time, initval);
		else
		{
			CalcLinearDown(time, initval);
			CalcGainDown(time, initval, gaint, gainr);
		}
	}
	else
	{
		if (current.speedup)
			CalcCurveUp(time, initval);
		else
			CalcCurveDown(time, initval);
	}
		 
	DiscardDeviceResources();

	InvalidateRect(GraphBox, NULL, FALSE);

}

void ConvertResolType(int r)
{
	char num[12];
	int i;
	float p;
	float c; // convert

	SendMessage(InitValTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));
	i = atoi(num);

	if (current.type == 1)  // (mV)
		c = static_cast<float>(DA_VOLTAGE);
	else if (current.type == 2) // (%)
		c = 100.0f;
	else // DA 
		c = static_cast<float>(DA_RESOLUTION);

	p = static_cast<float>(i / c);

	// value round down version							// value round up version
	if (r == 1)  // (mV)
		i = static_cast<int>(floor(DA_VOLTAGE * p));	// static_cast<int>(ceil(DA_VOLTAGE * p))
	else if (r == 2) // (%)
		i = static_cast<int>(floor(100.f * p));			// static_cast<int>(ceil(100.f * p))
	else // DA 
		i = static_cast<int>(floor(DA_RESOLUTION * p));	// static_cast<int>(ceil(DA_RESOLUTION * p))

	_itoa(i, num, 10);

	SendMessage(InitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));

	UpdateWindow();
}

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK MultisetDADialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND h;

	switch (msg) 
	{
        case WM_NOTIFY:
            break;
		case WM_COMMAND:
			h = (HWND)lParam;
			if (wParam == BN_CLICKED) 
			{
				UpdateWindow();
			}
			break;
		case WM_NCPAINT:
		case WM_PAINT:
			UpdateWindow();
			break;
	}

	return CallWindowProc((WNDPROC)PrevMultisetDADialogProc, hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK ResolTypeProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT r;

    switch (msg) 
	{
		case WM_COMMAND:
			if ( HIWORD(wParam) == CBN_SELCHANGE) 
			{               
                r = SendMessage(ResolTypeCombobox, CB_GETCURSEL, 0, 0);
                ConvertResolType(r);
			}
			break;
	}

	return CallWindowProc((WNDPROC)PrevResolTypeProc, hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK GraphBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	D2D1_POINT_2F p;

    switch (msg) {
        case WM_NOTIFY:
            break;
		case WM_MOUSEMOVE:
			p = PixelToDIP(lParam);
			MoveGraphPoint(p);
			break;
		case WM_LBUTTONDOWN:
			p = PixelToDIP(lParam);
			CapturePoint(p);
			break;
		case WM_LBUTTONUP:
			p = PixelToDIP(lParam);
			ReleasePoint(p);
			break;
		case WM_DRAWITEM:
		case WM_ERASEBKGND:
		case WM_NCPAINT:
		case WM_PAINT:
			RECT r;
			GetClientRect(hwnd, &r);

			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);

			Render();
			ValidateRect(hwnd, NULL);

			EndPaint(hwnd, &ps); 
			ValidateRect(hwnd, NULL);
			break;
        case WM_CLOSE:
        case WM_DESTROY:
            DiscardDeviceResources();
            break;
    }

    return CallWindowProc((WNDPROC)PrevGraphBoxProc, hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Don't allow any characters other than 0-9 in the name.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK NumberProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_GETDLGCODE:
			{
				MSG * m = (MSG *)lParam;
			
				if(m && m->message == WM_KEYDOWN && m->wParam == VK_RETURN)
				{
					UpdateWindow();
					return 0;
				}
				//return DLGC_WANTALLKEYS;
			}
			break;
		case WM_KILLFOCUS:	
			UpdateWindow();
			break;
		case WM_CHAR:
			if(!(isdigit(wParam) || /*wParam == '-' ||*/ wParam == '\b')) 
		        return 0;
			break;
    }

    return CallWindowProc((WNDPROC)PrevNumProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Tipo de Rampa"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 110, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper);

    LinearRadio = CreateWindowEx(0, WC_BUTTON, _("Linear"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
        16, 20, 80, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(LinearRadio);

    CurveRadio = CreateWindowEx(0, WC_BUTTON, _("Curva"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
        16, 42, 80, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(CurveRadio);

    HWND grouper2 = CreateWindowEx(0, WC_BUTTON, _("Direção do Movimento"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        120, 3, 145, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper2);

    ForwardRadio = CreateWindowEx(0, WC_BUTTON, _("Avançar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        129, 20, 100, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(ForwardRadio);

    BackwardRadio = CreateWindowEx(0, WC_BUTTON, _("Recuar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        129, 42, 100, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(BackwardRadio); 

    HWND grouper3 = CreateWindowEx(0, WC_BUTTON, _("Variação Velocidade"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        268, 3, 140, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper3);

    SpeedUpRadio = CreateWindowEx(0, WC_BUTTON, _("Aceleração"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        279, 20, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedUpRadio);

    SpeedDownRadio = CreateWindowEx(0, WC_BUTTON, _("Desaceleração"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        279, 42, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedDownRadio); 

    HWND GainTimeLabel = CreateWindowEx(0, WC_STATIC, _("Tempo (%):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        410, 20, 120, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(GainTimeLabel);

	GainTimeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        535, 20, 25, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(GainTimeTextbox);

    HWND GainResolLabel = CreateWindowEx(0, WC_STATIC, _("Resolução DA (%):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        410, 45, 120, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(GainResolLabel);

    GainInitValTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        535, 45, 25, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(GainInitValTextbox);
    
    HWND grouper4 = CreateWindowEx(0, WC_BUTTON, _("Curva de Ganho"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        411, 3, 155, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper4);

    HWND timeLabel = CreateWindowEx(0, WC_STATIC, _("Tempo (ms):"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        580, 20, 150, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(timeLabel);

    TimeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        735, 20, 50, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(TimeTextbox);

	ResolTypeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        580, 43, 150, 100, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(ResolTypeCombobox);

    InitValTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        735, 45, 50, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(InitValTextbox);

    HWND grouper5 = CreateWindowEx(0, WC_BUTTON, _("Tamanho da Rampa"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        570, 3, 225, 68, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper5);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        800, 10, 70, 23, MultisetDADialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        800, 40, 70, 23, MultisetDADialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    GraphBox = CreateWindowEx(0, WC_STATIC, NULL,
        WS_CHILD | SS_OWNERDRAW | WS_VISIBLE | CS_VREDRAW | WS_CLIPSIBLINGS | CS_HREDRAW | CS_PARENTDC,
        7, 80, 864, 490, MultisetDADialog, NULL, Instance, NULL);

	PrevResolTypeProc = SetWindowLongPtr(ResolTypeCombobox, GWLP_WNDPROC, 
        (LONG_PTR)ResolTypeProc);

    PrevNumProc = SetWindowLongPtr(TimeTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevNumProc = SetWindowLongPtr(InitValTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevNumProc = SetWindowLongPtr(GainInitValTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevNumProc = SetWindowLongPtr(GainTimeTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevGraphBoxProc = SetWindowLongPtr(GraphBox, GWLP_WNDPROC, 
        (LONG_PTR)GraphBoxProc);

}

void ShowMultisetDADialog(ElemMultisetDA *l)
{
	char num[12];

	current = *l;
	time = current.time;
	//initval = current.initval;

	MultisetDADialog = CreateWindowClient(0, "LDmicroDialog",
        _("Rampa de Aceleração/Desaceleração"), WS_OVERLAPPED | WS_SYSMENU | WS_EX_CONTROLPARENT,
        100, 100, 880, 600, MainWindow, NULL, Instance, NULL);

	PrevMultisetDADialogProc = SetWindowLongPtr(MultisetDADialog, GWLP_WNDPROC, 
        (LONG_PTR)MultisetDADialogProc);

    RECT r;
    GetClientRect(MultisetDADialog, &r);

    MakeControls();

	for (int i = 0; i < sizeof(ResollTypeItens) / sizeof(ResollTypeItens[0]); i++)
		SendMessage(ResolTypeCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ResollTypeItens[i]));

	SendMessage(ResolTypeCombobox, CB_SETCURSEL, current.type, 0);
	SendMessage(ResolTypeCombobox, CB_SETDROPPEDWIDTH, 230, 0);

	_itoa(current.gaint, num, 10);
    SendMessage(GainTimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));

	_itoa(current.gainr, num, 10);
    SendMessage(GainInitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));

	_itoa(current.time, num, 10);
    SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));

	_itoa(current.initval, num, 10);
    SendMessage(InitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));

    if(current.linear) {
        SendMessage(LinearRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(CurveRadio, BM_SETCHECK, BST_CHECKED, 0);
    } 
	if(current.forward) {
        SendMessage(ForwardRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(BackwardRadio, BM_SETCHECK, BST_CHECKED, 0);
    } 
    if(current.speedup) {
        SendMessage(SpeedUpRadio, BM_SETCHECK, BST_CHECKED, 0);
    } else {
        SendMessage(SpeedDownRadio, BM_SETCHECK, BST_CHECKED, 0);
    }

    EnableWindow(MainWindow, FALSE);
    ShowWindow(MultisetDADialog, TRUE);

	UpdateWindow();

	SetFocus(TimeTextbox);
    SendMessage(TimeTextbox, EM_SETSEL, 0, -1);

    MSG msg;
    DWORD ret;
    DialogDone = FALSE;
    DialogCancel = FALSE;

    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) 
	{
        if(msg.message == WM_KEYDOWN) 
		{
            if(msg.wParam == VK_RETURN && msg.hwnd == MultisetDADialog) 
			{
                DialogDone = TRUE;
                break;
            } 
			else if(msg.wParam == VK_ESCAPE) 
			{
                DialogDone = TRUE;
                DialogCancel = TRUE;
                break;
            }
        }

		if (IsDialogMessage(MultisetDADialog, &msg)) 
			continue;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

    if(!DialogCancel) 
		*l = current;

	DiscardDeviceResources();
    EnableWindow(MainWindow, TRUE);
    DestroyWindow(MultisetDADialog);

    return;
}
