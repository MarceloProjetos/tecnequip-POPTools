#include "EngineRenderD2D.h"
#include "EngineUtils.h"

EngineRenderD2D::EngineRenderD2D(void)
{
	Init();
}

EngineRenderD2D::~EngineRenderD2D(void)
{
	FreeAll();
}

void EngineRenderD2D::Init(void)
{
	pRT = NULL;
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

	if (SUCCEEDED(hr)) {
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&pWriteFactory));
	}
}

void EngineRenderD2D::FreeRenderTarget(void)
{
	unsigned int i;
	for(i=0; i<TextFormats.size(); i++) {
		SafeRelease(&TextFormats[i]);
	}
	TextFormats.clear();

	for(i=0; i<Brushes.size(); i++) {
		SafeRelease(&Brushes[i]);
	}
	Brushes.clear();

	SafeRelease(&pRT);
}

void EngineRenderD2D::FreeAll(void)
{
	FreeRenderTarget();
	SafeRelease(&pD2DFactory);
}

HRESULT EngineRenderD2D::CreateRenderTarget(HWND hwnd)
{
	// Obtain the size of the drawing area.
	RECT rc;
	GetClientRect(hwnd, &rc);

	if(pRT != NULL) {
		FreeRenderTarget();
	}

	// Create a Direct2D render target			
	HRESULT hr = pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			hwnd,
			D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top)
		),
		&pRT
	);

	if (SUCCEEDED(hr)) {
		unsigned int dummy;
		CreateTextFormat(0, dummy);
	}

	if (SUCCEEDED(hr)) {
		unsigned int dummy;
		CreateBrush(RGB(0,0,0), dummy);
	}

	return hr;
}

HRESULT EngineRenderD2D::CreateBrush(unsigned int rgb, unsigned int &index)
{
	ID2D1SolidColorBrush *pBrush = NULL;
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRT != NULL) {
		rgb = RGB((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
		hr = pRT->CreateSolidColorBrush(D2D1::ColorF(rgb, 1.0), &pBrush);

		if(SUCCEEDED(hr)) {
			index = Brushes.size();
			Brushes.push_back(pBrush);
		}
	}

	return hr;
}

HRESULT EngineRenderD2D::CreateTextFormat(unsigned int rgb, unsigned int &index)
{
	IDWriteTextFormat *pTextFormat = NULL;
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRT != NULL) {
		// Create a DirectWrite text format object.
		hr = pWriteFactory->CreateTextFormat(
			L"Lucida Console",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			13,
			L"", //locale
			&pTextFormat
			);

		if(SUCCEEDED(hr)) {
			index = TextFormats.size();
			TextFormats.push_back(pTextFormat);
		}
	}

	return hr;
}

void EngineRenderD2D::StartDraw(void)
{
	if(pRT != NULL) {
		pRT->BeginDraw();
		pRT->SetTransform(D2D1::Matrix3x2F::Identity());
	}
}

void EngineRenderD2D::Clear(unsigned int brush)
{
	if(pRT != NULL && brush < Brushes.size()) {
		pRT->Clear(Brushes[brush]->GetColor());
	}
}

HRESULT EngineRenderD2D::EndDraw(void)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRT != NULL) {
		hr = pRT->EndDraw();
	}

	return hr;
}

void EngineRenderD2D::DrawRectangle(RECT r, unsigned int brush, bool filled)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRT != NULL && brush < Brushes.size()) {
		if(filled) {
			pRT->FillRectangle(D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom), Brushes[brush]);
		} else {
			pRT->DrawRectangle(D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom), Brushes[brush]);
		}
	}
}

void EngineRenderD2D::DrawText(char *txt, RECT r, unsigned int format, unsigned int brush)
{
	if(pRT != NULL && brush < Brushes.size() && format < TextFormats.size()) {
		WCHAR wtxt[100];
		D2D1_RECT_F rf = D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom);

		swprintf(wtxt, wcslen(wtxt), L"%S", txt);
		pRT->DrawText(wtxt, static_cast<UINT>(wcslen(wtxt)), TextFormats[format], &rf, Brushes[brush]);
	}
}

HRESULT EngineRenderD2D::DrawLine(POINT start, POINT end, unsigned int brush)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRT != NULL && brush < Brushes.size()) {
		pRT->DrawLine(D2D1::Point2F((float)start.x, (float)start.y),
					  D2D1::Point2F((float)  end.x, (float)  end.y), Brushes[brush]);
		hr = S_OK;
	}

	return hr;
}

HRESULT EngineRenderD2D::DrawEllipse(POINT center, float rx, float ry, unsigned int brush, bool filled)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
	D2D1_ELLIPSE ellipse;

	ellipse.point = D2D1::Point2F((float)center.x, (float)center.y);
	ellipse.radiusX = rx;
	ellipse.radiusY = ry;

	if(pRT != NULL && brush < Brushes.size()) {
		if(filled) {
			pRT->FillEllipse(&ellipse, Brushes[brush]);
		} else {
			pRT->DrawEllipse(&ellipse, Brushes[brush]);
		}

		hr = S_OK;
	}

	return hr;
}
