#include "EngineGUI.h"
#include "EngineUtils.h"

EngineGUI::EngineGUI(void)
{
	Init();
}

EngineGUI::EngineGUI(EngineRender *pR)
{
	Init();
	SetRender(pR);
}

EngineGUI::EngineGUI(EngineRender *pR, COLORREF rgb)
{
	Init();
	SetRender(pR);
	SetBackgroundColor(rgb);
}

EngineGUI::~EngineGUI(void)
{
	FreeAll();
}

void EngineGUI::Init(void)
{
	pRender = NULL;
	Target = NULL;

	BrushOffset = 0;
	Brushes.push_back(RGB(0, 0, 0));

	DrawOffset.x = DrawOffset.y = 0;
}

void EngineGUI::FreeRender(void)
{
	if(pRender != NULL) {
		delete pRender;
		pRender = NULL;
	}
}

void EngineGUI::FreeAll(void)
{
	FreeRender();
	Brushes.clear();
	Target = NULL;
}

HRESULT EngineGUI::SetTarget(HWND hwnd)
{
	Target = hwnd;

	return S_OK;
}

HRESULT EngineGUI::SetRender(EngineRender *pR)
{
	FreeRender();

	pRender = pR;

	return S_OK;
}

HRESULT EngineGUI::GetRender(EngineRender **ppRender)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(ppRender != NULL) {
		*ppRender = pRender;
		hr = S_OK;
	}

	return hr;
}

HRESULT EngineGUI::StartDraw(void)
{
	unsigned int i, index;
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL && Target != NULL) {
		pRender->CreateRenderTarget(Target);

		for(i=0; i < Brushes.size(); i++) {
			pRender->CreateBrush(Brushes[i], index);
			if(!i) {
				BrushOffset = index;
			}
		}

		pRender->StartDraw();
		pRender->Clear(Brushes[0]);
		hr = S_OK;
	}

	return hr;
}

HRESULT EngineGUI::SetDrawOffset(POINT Offset)
{
	DrawOffset = Offset;
	return S_OK;
}

HRESULT EngineGUI::EndDraw(void)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		hr = pRender->EndDraw();
	}

	return hr;
}

HRESULT EngineGUI::SetBackgroundColor(COLORREF rgb)
{
	Brushes[0] = rgb;

	return S_OK;
}

unsigned int EngineGUI::CreateBrush(COLORREF rgb)
{
	Brushes.push_back(rgb);

	return Brushes.size() - 1;
}

HRESULT EngineGUI::DrawRectangle(RECT r, unsigned int brush, bool filled)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		r.left   += DrawOffset.x;
		r.right  += DrawOffset.x;
		r.top    += DrawOffset.y;
		r.bottom += DrawOffset.y;
		pRender->DrawRectangle(r, brush + BrushOffset, filled);
		hr = S_OK;
	}

	return hr;
}

HRESULT EngineGUI::DrawText(char *txt, RECT r, unsigned int format, unsigned int brush)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		r.left   += DrawOffset.x;
		r.right  += DrawOffset.x;
		r.top    += DrawOffset.y;
		r.bottom += DrawOffset.y;
		pRender->DrawText(txt, r, format, brush + BrushOffset);
		hr = S_OK;
	}

	return hr;
}

HRESULT EngineGUI::DrawLine(POINT start, POINT end, unsigned int brush)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		start.x += DrawOffset.x;
		end.x   += DrawOffset.x;
		start.y += DrawOffset.y;
		end.y   += DrawOffset.y;
		hr = pRender->DrawLine(start, end, brush + BrushOffset);
	}

	return hr;
}

HRESULT EngineGUI::DrawEllipse(RECT r, unsigned int brush, bool filled)
{
	POINT center;
	float rx, ry;

	center.x = r.left + (r.right  - r.left)/2;
	center.y = r.top  + (r.bottom - r.top )/2;

	rx = abs((float)(r.right  - r.left))/2;
	ry = abs((float)(r.bottom - r.top ))/2;

	return DrawEllipse(center, rx, ry, brush, filled);
}

HRESULT EngineGUI::DrawEllipse(POINT center, float rx, float ry, unsigned int brush, bool filled)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		center.x += DrawOffset.x;
		center.y += DrawOffset.y;
		hr = pRender->DrawEllipse(center, rx, ry, brush + BrushOffset, filled);
	}

	return hr;
}
