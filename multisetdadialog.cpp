#include "poptools.h"

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
static ID2D1PathGeometry*		pCoincFillPathGeometry;
static IDWriteTextFormat*		pTextFormat;
static ID2D1SolidColorBrush*	pBackgroundBrush;
static ID2D1SolidColorBrush*	pAxisBrush;
static ID2D1SolidColorBrush*	pLineBrush;
static ID2D1SolidColorBrush*	pGainBrush;
static ID2D1SolidColorBrush*	pLineFillBrush;
static ID2D1SolidColorBrush*	pCoincFillBrush;
static ID2D1SolidColorBrush*	pCoincBrush;
static ID2D1SolidColorBrush*	pPointBrush;
static ID2D1SolidColorBrush*	pFontBrush;

static HWND ForwardRadio;
static HWND BackwardRadio;
static HWND SpeedUpRadio;
static HWND SpeedDownRadio;
static HWND SpeedCoincRadio;
static HWND LinearRadio;
static HWND CurveRadio;
static HWND TimeTextbox;
static HWND InitValTextbox;
static HWND GainInitValTextbox;
static HWND GainTimeTextbox;
static HWND UpdateButton;
static HWND ResolTypeCombobox;
static HWND GraphBox;
static HWND AbortModeCombobox;

static LONG_PTR PrevMultisetDADialogProc;
static LONG_PTR PrevNumAlphaProc;
static LONG_PTR PrevNumProc;
static LONG_PTR PrevResolTypeProc;
static LONG_PTR PrevGraphBoxProc;

static int		tempo;
static int		initval;
static int		gainr;
static int		gaint;
static int		gain_idx;
static float	points[DA_RESOLUTION];
static float	gains[DA_RESOLUTION];

static ElemMultisetDA current;

static vector<D2D1_POINT_2F>	DAPoints;

const LPCTSTR ResollTypeItens[] = { _("(12 bits)[2047 ~ -2048]"), _("(mV)[10000 ~ -10000]"), _("(%)[0 ~ 100]") };
const LPCTSTR RampAbortModes [] = { _("Default"), _("Leave"), _("Stop"), _("Desaceleration") };

void CalcLinearUp(int calc_tempo, int calc_initval)
{
	int tm = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	float t = static_cast<float>(calc_tempo);
	float d = static_cast<float>(calc_initval);

	memset(points, 0, sizeof(points));

	float a = d / t; // coeficiente angular
	float l = 0.0f; // coeficiente linear

	for (int i = 0; i < calc_tempo / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;
		points[i] = DA_RESOLUTION + ((tm * a) + l); // y = a.x + b
	}
}

void CalcLinearDown(int calc_tempo, int calc_initval)
{
	int tm = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	float t = static_cast<float>(calc_tempo);
	float d = static_cast<float>(calc_initval);

	memset(points, 0, sizeof(points));

	float a = abs(d) / t; // coeficiente angular
	float l = static_cast<float>(calc_initval); // coeficiente linear

	if (d < 0)
	{
		for (int i = 0; i < calc_tempo / DA_CYCLE_INTERVAL; i++)
		{
			tm = i * DA_CYCLE_INTERVAL;
			points[i] = ((tm * a) + l); // y = a.x + b
		}
	}
	else
	{
		a *= -1.0f;
		for (int i = 0; i < calc_tempo / DA_CYCLE_INTERVAL; i++)
		{
			tm = i * DA_CYCLE_INTERVAL;
			points[i] = ((tm * a) + l); // y = a.x + b
		}
	}

}

void CalcGainDown(int calc_tempo, int calc_initval, int calc_gainx, int calc_gainy = 5)
{
	int tm = 0;
	int i = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	float gx = static_cast<float>(calc_gainx);
	float gy = static_cast<float>(calc_gainy);
	float t = static_cast<float>(calc_tempo);
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

void CalcGainUp(int calc_tempo, int calc_initval, int calc_gainx, int calc_gainy = 5)
{
	int tm = 0;
	int i = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	memset(gains, 0, sizeof(gains));

	float VarDA;

	float DeltaXGanho  = (float)(calc_tempo    * calc_gainx) / 100;
	float DeltaYGanho  = (float)(calc_initval * calc_gainy) / 100;

	float DeltaXLinear = calc_tempo    - 2*DeltaXGanho;
	float DeltaYLinear = calc_initval - 2*DeltaYGanho;

	for (tm=0; tm < calc_tempo; tm+=DA_CYCLE_INTERVAL)
	{
		i = tm/DA_CYCLE_INTERVAL;

		if (tm < DeltaXGanho) {
			VarDA = (DeltaYGanho * tm) / DeltaXGanho;
		} else if(tm < (calc_tempo - DeltaXGanho)) {
			VarDA = DeltaYGanho + (DeltaYLinear * (tm - DeltaXGanho)) / DeltaXLinear;
		} else {
			VarDA = calc_initval - (DeltaYGanho * (calc_tempo - tm)) / DeltaXGanho;
		}

		gains[i] = (float)(VarDA+2048);
	}
}

void CalcCurveUp(int calc_tempo, int calc_initval)
{
	int tm = 0;
	float factor = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	memset(points, 0, sizeof(points));

	for (int i = 0; i < calc_tempo / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;
		if (tm < calc_tempo)
		{
			if (tm < calc_tempo / 2)
			{
				factor = pow((tm * 1000.0f) / calc_tempo, 2);
			}
			else
			{
				factor = 500000.0f - pow(1000.0f - (tm * 1000.0f) / calc_tempo, 2);	
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

void CalcCurveDown(int calc_tempo, int calc_initval)
{
	int tm = 0;
	float factor = 0;

	if (calc_initval > DA_RESOLUTION || 
		calc_initval < (DA_RESOLUTION * -1))
		return;

	memset(points, 0, sizeof(points));

	for (int i = 0; i < calc_tempo / DA_CYCLE_INTERVAL; i++)
	{
		tm = i * DA_CYCLE_INTERVAL;
		if (tm < calc_tempo)
		{
			if (tm < calc_tempo / 2)
			{
				factor = 500000.0f - pow((tm * 1000.0f) / calc_tempo, 2);
			}
			else
			{
				factor = pow(1000.0f - (tm * 1000.0f) / calc_tempo, 2);
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
			hr = pD2DFactory->CreatePathGeometry(&pCoincFillPathGeometry);
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
			//D2D1_COLOR_F c = D2D1::ColorF(0xc2d1f0);
			//c.a = 0.6f;
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightGreen, 0.5f),
				&pCoincFillBrush
				);
		}

		if (SUCCEEDED(hr))
		{
			//D2D1_COLOR_F c = D2D1::ColorF(0xc2d1f0);
			//c.a = 0.6f;
			hr = pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Green, 0.5f),
				&pCoincBrush
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
			properties.dashStyle = D2D1_DASH_STYLE_DASH_DOT;

			/*float dashes[] =
			{
			2.0f, 1.0f,
			3.0f, 1.0f,
			};

			pD2DFactory->CreateStrokeStyle(properties, dashes, _countof(dashes), &pDashStrokeStyle);*/
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
	SafeRelease(&pCoincFillPathGeometry);
	SafeRelease(&pTextFormat);
	SafeRelease(&pAxisBrush);
	SafeRelease(&pLineBrush);
	SafeRelease(&pLineFillBrush);
	SafeRelease(&pCoincFillBrush);
	SafeRelease(&pCoincBrush);
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

// Function to Normalize D/A value. It returns an absolute offset from 0V to p point.
inline float NormalizeDA(float p, float GraphResolution)
{
	return abs(abs(p) - (current.speedup ? DA_RESOLUTION : GraphResolution));
}

void Render()
{
    HRESULT hr;
	float OffsetX = 0, OffsetY = 0, GraphResolution = DA_RESOLUTION;

    hr = CreateDeviceResources();

    if (SUCCEEDED(hr) && !(pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
    {
		// If final D/A value is below 10% of max value, we should to scale graph to be easier to see the result
		if(abs(initval) < GraphResolution*0.1f) {
			GraphResolution /= 10;
		}

		// If we should to coincide the ramp with current D/A value, show a green area before
		// the ramp so the user will understand it.
		if(current.StartFromCurrentValue) {
			// Offset for X and Y axis from ramp start
			OffsetX = abs(tempo   ) * 0.2f;
			OffsetY = abs(initval) * 0.2f;
			// If the final value for D/A is low, shows a inverse graph
			if(abs(initval) < GraphResolution/2) {
				OffsetY = (GraphResolution/2)*0.8f;
			}
		}

		// Retrieve the size of the render target.
        D2D1_SIZE_F size = pRenderTarget->GetSize();

		// Scale to draw graph. They will convert values from time and D/A values to graph coordinates.
		float scaleX = (size.width  - (MARGIN * 3)) / abs(tempo);
		float scaleY = (size.height - (MARGIN * 2)) / GraphResolution; // abs(initval);

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
		float interval = (size.width - MARGIN * 3 - OffsetX*scaleX) / DA_CYCLE_INTERVAL;
		for (int i = 0; i < size.width / DA_CYCLE_INTERVAL; i++)
		{
			pRenderTarget->DrawLine(D2D1::Point2F(MARGIN*2 - 1.0f + (interval * i) + OffsetX*scaleX, size.height - MARGIN + 1.0f - 5.0f*(i>0)), 
									D2D1::Point2F(MARGIN*2 - 1.0f + (interval * i) + OffsetX*scaleX, size.height - MARGIN + 1.0f + 5.0f), pAxisBrush);

			D2D1_RECT_F r = D2D1::RectF(MARGIN*2 - 1.0f + (interval * i) - (FONT_SIZE * 2.0f) + OffsetX*scaleX,
										size.height - FONT_SIZE - 5.0f, 
										MARGIN*2 - 1.0f + (interval * i) + (FONT_SIZE * 2.0f) + OffsetX*scaleX, 
										size.height - 5.0f);

			StringCchPrintfW(Num, ARRAYSIZE(Num), L"%d", (abs(tempo) / DA_CYCLE_INTERVAL) * i );

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

			StringCchPrintfW(Num, ARRAYSIZE(Num), L"%d", static_cast<int>(initval < 0 ? GraphResolution * (0.25f * i) * -1.0f: GraphResolution * (0.25f * (4 - i))));

			pRenderTarget->DrawText(Num, static_cast<UINT>(wcslen(Num)), pTextFormat, &r, pAxisBrush);
		}

		// Load size and height with ramp limits
		size.width  = (float)abs(tempo);
		size.height = GraphResolution;

		// How much should we advance in X coordinate for each D/A cycle interval
		float intervalX = (size.width - OffsetX) / (abs(tempo) / DA_CYCLE_INTERVAL);

		D2D1_MATRIX_3X2_F translate = D2D1::Matrix3x2F::Translation(MARGIN * 2, MARGIN);

        pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		pRenderTarget->SetTransform(translate);

		// Create graph geometry
		ID2D1GeometrySink *pSink = NULL, *pSinkCoinc = NULL;

		hr = pLineFillPathGeometry->Open(&pSink);
		if(!SUCCEEDED(hr)) {
			Error(_("Ocorreu um erro ao desenhar o gráfico do D/A."));
			DiscardDeviceResources();
			return;
		}

		int i, pass, end = (int)ceil(static_cast<float>(abs(tempo) / DA_CYCLE_INTERVAL));
		float desl, p, x, y, prevx, prevy, startx, starty, offset = 0, fator_graph, fator_ramp, FigureOffsetY = 0;

		pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

		// Point where to start to draw the graph in X axis.
		startx = prevx = OffsetX * scaleX;

		// Point where to start to draw the graph in Y axis.
		starty = NormalizeDA(points[0], GraphResolution);
		if((!current.speedup && current.forward) || (current.speedup && !current.forward)) {
			starty = GraphResolution - starty;
		}
		starty = prevy = (size.height - starty)*scaleY;

		// If we are drawing the inverse graph, we should to offset figure points in Y axis
		if(current.StartFromCurrentValue && abs(initval) < GraphResolution/2) {
			FigureOffsetY = initval*scaleY;
		}

		// Calculate graphic points
		pSink->BeginFigure(D2D1::Point2F(startx, starty + FigureOffsetY), D2D1_FIGURE_BEGIN_FILLED);
		if(current.StartFromCurrentValue) { // If we are using coincide mode...
			// We have to use offset in Y axis
			starty += (initval<0 ? +OffsetY : -OffsetY)*scaleY;

			hr = pCoincFillPathGeometry->Open(&pSinkCoinc);
			if(!SUCCEEDED(hr)) {
				Error(_("Ocorreu um erro ao desenhar o gráfico do D/A."));
				DiscardDeviceResources();
				return;
			}

			// Draw Green Area
			pSinkCoinc->BeginFigure(D2D1::Point2F(startx, prevy  + FigureOffsetY), D2D1_FIGURE_BEGIN_FILLED);

			pSinkCoinc->AddLine    (D2D1::Point2F(0.0f  , prevy  + FigureOffsetY));
			pSinkCoinc->AddLine    (D2D1::Point2F(0.0f  , starty                ));
			pSinkCoinc->AddLine    (D2D1::Point2F(startx, starty                ));

			pSinkCoinc->EndFigure  (D2D1_FIGURE_END_CLOSED); // D2D1_FIGURE_END_OPEN D2D1_FIGURE_END_CLOSED

			hr = pSinkCoinc->Close();
			SafeRelease(&pSinkCoinc);

			// Draw graph area
			pRenderTarget->FillGeometry(pCoincFillPathGeometry, pCoincFillBrush);

			// Draw Green Line
			pRenderTarget->DrawLine(D2D1::Point2F(0.0f  , starty),
									D2D1::Point2F(startx, starty),
									pCoincBrush, 4.0f, pDashStrokeStyle);

			// Continue to draw graphic
			pSink->AddLine    (D2D1::Point2F(startx, starty));

			// Line to separate green area from graph area
			pRenderTarget->DrawLine(D2D1::Point2F(startx, 0.0f), 
									D2D1::Point2F(startx, GraphResolution*scaleY),
									pAxisBrush, 1.0f, pDashStrokeStyle);
		}

		// Calculate total variation of Y axis, in D/A values.
		desl = abs(NormalizeDA(current.linear ? gains[      0] : points[0      ], GraphResolution) -
				   NormalizeDA(current.linear ? gains[end - 1] : points[end - 1], GraphResolution));

		// We have to go through graph points two times.
		// The first one is to draw the blue area
		// The second one is to draw line graph over borders of blue area.
		for(pass = 0; pass<2; pass++) {
			// initalize previous point with start point.
			prevx = startx;
			prevy = starty;

			// Loop to calculate graph points
			for (i = 1; i <= end; i++) {
				// Calculate X axis position
				x = intervalX * i + OffsetX;
				if(i<end) { // If we didn't reach end point, calculate next one...
					// Normalized point. Now we have in p a value from 0 to GraphResolution, indicating
					// the "height" of D/A in Y axis
					p = NormalizeDA(current.linear ? gains[i] : points[i], GraphResolution);

					// Now we calculate a value from 0 to 1, indicanting the position of p related to
					// GraphResolution in Y axis
					fator_graph = p/GraphResolution;
					if((current.speedup && current.forward) || (!current.speedup && !current.forward)) {
						fator_graph = 1.0f - fator_graph;
					}

					if(abs(initval) < GraphResolution/2) { // if drawing inverse graph...
						// Invers graph, we have to consider inverse p value.
						p = GraphResolution - p;
						// Value from 0 to 1 indicating p position related to total variation of the ramp
						fator_ramp = p/desl;

						// Offset for Y axis to draw graph
						offset = OffsetY - OffsetY*(1.0f - fator_ramp) + abs(initval)*(1.0f - fator_ramp);
					} else {
						// Value from 0 to 1 indicating p position related to total variation of the ramp
						fator_ramp = p/desl;

						// Offset for Y axis to draw graph
						offset = OffsetY - OffsetY*fator_ramp;
					}

					// If initval (D/A value) is negative, invert offset.
					if(initval > 0) {
						offset *= -1;
					}

					// Calculate Y axis position
					y = size.height*fator_graph + (current.StartFromCurrentValue ? offset : 0);
				} else if(current.speedup) {
					// Last point! If accelerating, final point should to be final D/A value
					y = initval < 0 ? abs(initval) : size.height - initval;
				} else if(current.StartFromCurrentValue) {
					// Last point! We are decelerating and using coincide mode. Special case!
					// Final point should to be final D/A value.
					y = (initval < 0 ? 0.0f : GraphResolution) - initval;
				} else {
					// Last point! We are decelerating and not using coincide mode.
					// Final point should to be 0V
					y = initval < 0 ? 0.0f : GraphResolution;
				}

				// Convert calculated X and Y values to graph coordinates
				x *= scaleX;
				y *= scaleY;

				if(pass == 0) {
					// First pass, draw the point in figure representing blue graph area.
					pSink->AddLine(D2D1::Point2F(x, y));
				} else {
					// Second pass. Draw a line from previous calculated point to new one.
					// This way we will make a continuous line, following D/A calculated values
					pRenderTarget->DrawLine(D2D1::Point2F(prevx, prevy),
											D2D1::Point2F(    x,     y), 
											i > (end*(1.0f-(float)(current.gaint)/100)) && current.linear ? pGainBrush : pLineBrush,
											4.0f, pLineStrokeStyle);
				}

				if(current.linear && pass == 1 && i == end/2) {
					// We reached mid point when drawing graph line. We have to insert the graph dot.
					pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x,y), 4.0f, 4.0f), pLineBrush);
				}

				// Keep current X and Y axis values for future use.
				prevx = x;
				prevy = y;
			}

			// At this point, the current objet is almost finished. We just have to close figures, etc.
			if(pass == 0) { // First pass, We have to close the figure representing blue area.
				pSink->AddLine(D2D1::Point2F(current.speedup || current.StartFromCurrentValue ? x : 0.0f, initval < 0 ? 0.0f : GraphResolution * scaleY));

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED); // D2D1_FIGURE_END_OPEN D2D1_FIGURE_END_CLOSED
		
				hr = pSink->Close();
				SafeRelease(&pSink);

				// Draw graph area
				pRenderTarget->FillGeometry(pLineFillPathGeometry, pLineFillBrush);

				// When using linear mode, we have to draw a dotted line from start to end point so it is eays
				// for the user to see variation of ramp because of gain factors.
				if (current.linear) {
					pRenderTarget->DrawLine(D2D1::Point2F(startx, starty),
											D2D1::Point2F(     x,      y),
											pLineBrush, 2.0f, pDashStrokeStyle);
				}
			}
		}

		// Render finished! After all hard work, just end draw... :-)
		hr = pRenderTarget->EndDraw();

		// Discard all objects allocated to render graph
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

	if(SendMessage(SpeedCoincRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
		current.StartFromCurrentValue = TRUE;
		current.speedup = TRUE;
	} else if(SendMessage(SpeedUpRadio, BM_GETSTATE, 0, 0) & BST_CHECKED) {
		current.StartFromCurrentValue = FALSE;
		current.speedup = TRUE;
	} else {
		current.StartFromCurrentValue = FALSE;
		current.speedup = FALSE;
	}

	SendMessage(GainTimeTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));

	if (atoi(num) > MAX_GAIN_TIME_VAL)
	{
		_itoa(MAX_GAIN_TIME_VAL, num, 10);
		SendMessage(GainTimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
		StringCchPrintf(msg, sizeof(msg), _("O valor máximo permitido para o campo Tempo (%%) da Curva de Ganho é %d."), MAX_GAIN_TIME_VAL);
		MessageBox(MultisetDADialog, msg, _("Valor inválido no campo Tempo (%) !"), MB_OK | MB_ICONEXCLAMATION);
	}

	current.gaint = max(0, min(MAX_GAIN_TIME_VAL, abs(atoi(num))));
	gaint = current.gaint;

	SendMessage(GainInitValTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));

	if (atoi(num) > MAX_GAIN_RESOLUTION_VAL || atoi(num) > current.gaint)
	{
		_itoa(min(current.gaint, MAX_GAIN_RESOLUTION_VAL), num, 10);
		SendMessage(GainInitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));
		StringCchPrintf(msg, sizeof(msg), _("O valor máximo permitido para o campo Resolução (%%) da Curva de Ganho é %s."), num);
		MessageBox(MultisetDADialog, msg, _("Valor inválido no campo Resolução (%) !"), MB_OK | MB_ICONEXCLAMATION);
	}

	current.gainr = max(0, min(MAX_GAIN_RESOLUTION_VAL, abs(atoi(num))));
	current.gainr = min(current.gaint, current.gainr);
	gainr = current.gainr;

	SendMessage(TimeTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));

	if(IsNumber(num)) {
		if (atoi(num) > MAX_TIME_VAL || atoi(num) < MIN_TIME_VAL)
		{
			_itoa(max(MIN_TIME_VAL, min(MAX_TIME_VAL, abs(atoi(num)))), num, 10);
			SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
			StringCchPrintf(msg, sizeof(msg), _("O valor permitido para o campo Tempo (ms) no Tamanho da Rampa esta entre %d e %d."), MIN_TIME_VAL, MAX_TIME_VAL);
			MessageBox(MultisetDADialog, msg, _("Valor inválido no campo Tempo (ms) !"), MB_OK | MB_ICONEXCLAMATION);
		}

		current.time = max(MIN_TIME_VAL, min(MAX_TIME_VAL, abs(atoi(num))));
		_itoa(current.time, num, 10);
		_itoa(current.time, current.name, 10);
		SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	} else {
		current.time = 600;
		strcpy(current.name, num);
	}

	tempo = current.time;

	current.type            = SendMessage(ResolTypeCombobox, CB_GETCURSEL, 0, 0);
	current.ramp_abort_mode = SendMessage(AbortModeCombobox, CB_GETCURSEL, 0, 0);

	SendMessage(InitValTextbox, WM_GETTEXT, (WPARAM)sizeof(num) - 1, (LPARAM)(num));

	if(IsNumber(num)) {
		if (current.type == 1)  // (mV)
		{
			current.initval = min(MAX_MILIVOLT_VAL, abs(atoi(num)));
		}
		else if (current.type == 2) // (%)
		{
			current.initval = min(100, abs(atoi(num)));
		}
		else // DA 
		{
			current.initval = min(DA_RESOLUTION, abs(atoi(num)));
			if (current.forward && current.initval >= DA_RESOLUTION)
				current.initval = DA_RESOLUTION - 1;
		}
	
		_itoa(current.initval, num, 10);
		_itoa(current.forward ? current.initval : current.initval * -1, current.name1, 10);
		SendMessage(InitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));

		if ((current.type == 0 && (((current.forward && atoi(num) > DA_RESOLUTION - 1) || (!current.forward && atoi(num) > DA_RESOLUTION)))) ||
			(current.type == 1 && (atoi(num) > MAX_MILIVOLT_VAL)) || 
			(current.type == 2 && (atoi(num) > 100)))
		{
			StringCchPrintf(msg, sizeof(msg), _("O valor máximo permitido para o campo Resolução DA no Tamanho da Rampa é %d."), current.type == 0 ? DA_RESOLUTION - 1 : (current.type == 1 ? MAX_MILIVOLT_VAL : 100));
			MessageBox(MultisetDADialog, msg, _("Valor inválido no campo Resolução DA !"), MB_OK | MB_ICONEXCLAMATION);
		}
	} else {
		switch(current.type) {
		case 1:
			current.initval = 10000;
			break;
		case 2:
			current.initval = 100;
			break;
		default:
			current.initval = 2047;
		}
		strcpy(current.name1, num);
	}

	if (current.type == 1)  // (mV)
		initval = static_cast<int>(DA_RESOLUTION * (current.initval / DA_VOLTAGE));
	else if (current.type == 2) // (%)
		initval = static_cast<int>(DA_RESOLUTION * (current.initval / 100.0f));
	else // DA 
		initval = current.initval;

	if(current.StartFromCurrentValue && abs(initval) < (abs(initval) < DA_RESOLUTION*0.1f ? DA_RESOLUTION*0.1f : DA_RESOLUTION)/2) {
		current.speedup = !current.speedup;
	}

	if(!current.forward) {
		initval *= -1;
	}

	_itoa(current.gainr, num, 10);
	SendMessage(GainInitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));
	_itoa(current.gaint, num, 10);
	SendMessage(GainTimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));

	if(!initval) initval = current.forward ? +1 : -1;

	if (current.linear)
	{
		if (current.speedup) {
			CalcLinearUp(tempo, initval);
			CalcGainUp  (tempo, initval, gaint, gainr);
		} else {
			CalcLinearDown(tempo, initval);
			CalcGainDown(tempo, initval, gaint, gainr);
		}
	}
	else
	{
		if (current.speedup)
			CalcCurveUp(tempo, initval);
		else
			CalcCurveDown(tempo, initval);
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

	if(IsNumber(num)) {
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
	}

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
			if((h == TimeTextbox || h == InitValTextbox) && HIWORD(wParam) == CBN_KILLFOCUS) {
				UpdateWindow();
			} else if (wParam == BN_CLICKED) 
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

//-----------------------------------------------------------------------------
// Don't allow any characters other than 0-9 and a-z in the name.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK NumberAlphaProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
			if(!(isdigit(wParam) || isalpha(wParam) || /*wParam == '-' ||*/ wParam == '\b')) 
		        return 0;
			break;
    }

    return CallWindowProc((WNDPROC)PrevNumAlphaProc, hwnd, msg, wParam, lParam);
}

static void MakeControls(void)
{
    HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Tipo de Rampa"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE | WS_TABSTOP,
        7, 3, 110, 67, MultisetDADialog, NULL, Instance, NULL);
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
        120, 3, 145, 67, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper2);

    ForwardRadio = CreateWindowEx(0, WC_BUTTON, _("Avançar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        129, 20, 100, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(ForwardRadio);

    BackwardRadio = CreateWindowEx(0, WC_BUTTON, _("Recuar"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        129, 42, 100, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(BackwardRadio); 

    HWND Label = CreateWindowEx(0, WC_STATIC, _("Modo de Abandono:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        7, 74, 120, 20, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(Label);

	AbortModeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        130, 72, 135, 100, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(AbortModeCombobox);

    HWND grouper3 = CreateWindowEx(0, WC_BUTTON, _("Variação Velocidade"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        268, 3, 140, 90, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper3);

    SpeedUpRadio = CreateWindowEx(0, WC_BUTTON, _("Aceleração"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
        279, 20, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedUpRadio);

    SpeedDownRadio = CreateWindowEx(0, WC_BUTTON, _("Desaceleração"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        279, 42, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedDownRadio); 

    SpeedCoincRadio = CreateWindowEx(0, WC_BUTTON, _("Coincidir"),
        WS_CHILD | BS_AUTORADIOBUTTON | WS_VISIBLE | WS_TABSTOP,
        279, 64, 120, 25, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(SpeedCoincRadio); 

    HWND GainTimeLabel = CreateWindowEx(0, WC_STATIC, _("Tempo:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        380, 28, 120, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(GainTimeLabel);

    HWND GainTimeLabel2 = CreateWindowEx(0, WC_STATIC, "%",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        535, 28, 15, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(GainTimeLabel2);

	GainTimeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        505, 28, 25, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(GainTimeTextbox);

    HWND GainResolLabel = CreateWindowEx(0, WC_STATIC, _("Resolução DA:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        380, 60, 120, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(GainResolLabel);

    HWND GainResolLabel2 = CreateWindowEx(0, WC_STATIC, "%",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        535, 60, 15, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(GainResolLabel2);

    GainInitValTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        505, 60, 25, 21, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(GainInitValTextbox);
    
    HWND grouper4 = CreateWindowEx(0, WC_BUTTON, _("Curva de Ganho"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        411, 3, 145, 90, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(grouper4);

    HWND timeLabel = CreateWindowEx(0, WC_STATIC, _("Tempo:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        560, 28, 55, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(timeLabel);

    HWND timeLabel2 = CreateWindowEx(0, WC_STATIC, "ms",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        765, 28, 30, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(timeLabel2);

    TimeTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        620, 28, 140, 321, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(TimeTextbox);

	LoadIOListToComboBox(TimeTextbox, IO_TYPE_VAR);
	SendMessage(TimeTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND ResolTypeLabel = CreateWindowEx(0, WC_STATIC, _("Valor:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        560, 63, 55, 21, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(ResolTypeLabel);

	ResolTypeCombobox = CreateWindowEx(0, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        725, 60, 60, 100, MultisetDADialog, NULL, Instance, NULL);
    NiceFont(ResolTypeCombobox);

    InitValTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN | WS_VSCROLL,
        620, 61, 100, 321, MultisetDADialog, NULL, Instance, NULL);
    FixedFont(InitValTextbox);

	LoadIOListToComboBox(InitValTextbox, IO_TYPE_GENERAL);
	SendMessage(InitValTextbox, CB_SETDROPPEDWIDTH, 300, 0);

    HWND grouper5 = CreateWindowEx(0, WC_BUTTON, _("Tamanho da Rampa"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        560, 3, 235, 90, MultisetDADialog, NULL, Instance, NULL);
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
        7, 100, 864, 490, MultisetDADialog, NULL, Instance, NULL);

	PrevResolTypeProc = SetWindowLongPtr(ResolTypeCombobox, GWLP_WNDPROC, 
        (LONG_PTR)ResolTypeProc);

	PrevNumProc = SetWindowLongPtr(GainInitValTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevNumProc = SetWindowLongPtr(GainTimeTextbox, GWLP_WNDPROC, 
        (LONG_PTR)NumberProc);

	PrevGraphBoxProc = SetWindowLongPtr(GraphBox, GWLP_WNDPROC, 
        (LONG_PTR)GraphBoxProc);

}

void PopulateAbortModeCombobox(HWND AbortModeCombobox, bool IncludeDefault)
{
	for (int i = IncludeDefault ? 0 : 1; i < sizeof(RampAbortModes) / sizeof(RampAbortModes[0]); i++)
		SendMessage(AbortModeCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)RampAbortModes[i]));
}

void ShowMultisetDADialog(ElemMultisetDA *l)
{
	char num[12];
	char time_tmp   [MAX_NAME_LEN];
	char initval_tmp[MAX_NAME_LEN];

	current = *l;
	tempo = current.time;
	//initval = current.initval;

	strcpy(time_tmp   , l->name );
	strcpy(initval_tmp, l->name1);

	MultisetDADialog = CreateWindowClient(0, "POPToolsDialog",
        _("Rampa de Aceleração/Desaceleração"), WS_OVERLAPPED | WS_SYSMENU | WS_EX_CONTROLPARENT,
        100, 100, 880, 600, MainWindow, NULL, Instance, NULL);

	PrevMultisetDADialogProc = SetWindowLongPtr(MultisetDADialog, GWLP_WNDPROC, 
        (LONG_PTR)MultisetDADialogProc);

    RECT r;
    GetClientRect(MultisetDADialog, &r);

    MakeControls();

	PopulateAbortModeCombobox(AbortModeCombobox, true);
	SendMessage(AbortModeCombobox, CB_SETCURSEL, current.ramp_abort_mode, 0);

	for (int i = 0; i < sizeof(ResollTypeItens) / sizeof(ResollTypeItens[0]); i++)
		SendMessage(ResolTypeCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ResollTypeItens[i]));

	SendMessage(ResolTypeCombobox, CB_SETCURSEL, current.type, 0);
	SendMessage(ResolTypeCombobox, CB_SETDROPPEDWIDTH, 150, 0);

	_itoa(current.gaint, num, 10);
    SendMessage(GainTimeTextbox, WM_SETTEXT, 0, (LPARAM)(num));

	_itoa(current.gainr, num, 10);
    SendMessage(GainInitValTextbox, WM_SETTEXT, 0, (LPARAM)(num));

	SendMessage(TimeTextbox, WM_SETTEXT, 0, (LPARAM)(current.name));
    SendMessage(InitValTextbox, WM_SETTEXT, 0, (LPARAM)(current.name1));

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

	if(current.StartFromCurrentValue) {
        SendMessage(SpeedCoincRadio, BM_SETCHECK, BST_CHECKED, 0);
	} else if(current.speedup) {
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

    if(!DialogCancel &&
			IsValidNameAndType(time_tmp   , current.name , _("Tempo"), VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(current.name), 0, 0) &&
			IsValidNameAndType(initval_tmp, current.name1, _("Valor"), VALIDATE_IS_VAR_OR_NUMBER, GetTypeFromName(current.name), 0, 0)) {
		*l = current;
	}

	DiscardDeviceResources();
    EnableWindow(MainWindow, TRUE);
    DestroyWindow(MultisetDADialog);

    return;
}
