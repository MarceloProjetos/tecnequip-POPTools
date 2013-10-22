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
	RECT rZero = { 0, 0, 0, 0 };
	LastTargetRect = rZero;

	pRender = NULL;
	Target = NULL;

	BrushOffset = 0;

	tBrushData data;
	data.type        = eBrushType_SolidColor;
	data.solid.rgb   = RGB(0, 0, 0);
	data.solid.alpha = 1.0f;
	Brushes.push_back(data);

	brushWidth = 2.0f;

	DrawOffset.x = DrawOffset.y = 0;

	TargetInvalid = true;
}

void EngineGUI::InvalidateTarget(void)
{
	TargetInvalid = true;
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
	if(Target != hwnd) {
		Target = hwnd;
		InvalidateTarget();
	}

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
		RECT r;
		GetClientRect(Target, &r);
		if(TargetInvalid) {
			TargetInvalid  = false;

			pRender->CreateRenderTarget(Target);

			for(i=0; i < Brushes.size(); i++) {
				if(Brushes[i].type == eBrushType_SolidColor) {
					pRender->CreateBrush(index, Brushes[i].solid.rgb, Brushes[i].solid.alpha);
				} else {
					pRender->CreateGradient(index, Brushes[i].linear.brushStart + BrushOffset,
						Brushes[i].linear.brushEnd + BrushOffset, Brushes[i].linear.angle);
				}

				if(!i) {
					BrushOffset = index;
				}
			}
		} else if(r.right != LastTargetRect.right || r.bottom != LastTargetRect.bottom) {
			LastTargetRect = r;
			pRender->ResizeRenderTarget(Target);
		}

		pRender->StartDraw();
		pRender->Clear(BrushOffset);
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
	if(Brushes[0].solid.rgb != rgb) {
		Brushes[0].solid.rgb = rgb;
		InvalidateTarget();
	}

	return S_OK;
}

unsigned int EngineGUI::CreateBrush(COLORREF rgb, float alpha)
{
	tBrushData data;
	data.type        = eBrushType_SolidColor;
	data.solid.rgb   = rgb;
	data.solid.alpha = alpha;
	Brushes.push_back(data);

	InvalidateTarget();

	return Brushes.size() - 1;
}

unsigned int EngineGUI::CreateGradient(unsigned int brushStart, unsigned int brushEnd, unsigned int angle)
{
	tBrushData data;
	data.type        = eBrushType_GradientLinear;
	data.linear.brushStart = brushStart;
	data.linear.brushEnd   = brushEnd;
	data.linear.angle      = angle;
	Brushes.push_back(data);

	InvalidateTarget();

	return Brushes.size() - 1;
}

HRESULT EngineGUI::DrawPolygon(vector<POINT> points, unsigned int brush, bool filled, unsigned int angle)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL && points.size() > 1) {
		vector<POINT>::iterator it;
		for(it = points.begin(); it != points.end(); it++) {
			it->x += DrawOffset.x;
			it->y += DrawOffset.y;
		}

		hr = pRender->DrawPolygon(points, brush + BrushOffset, filled, angle, brushWidth);
	}

	return hr;
}

HRESULT EngineGUI::DrawRectangle(RECT r, unsigned int brush, bool filled, unsigned int radiusX, unsigned int radiusY, unsigned int angle)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		r.left   += DrawOffset.x;
		r.right  += DrawOffset.x;
		r.top    += DrawOffset.y;
		r.bottom += DrawOffset.y;
		pRender->DrawRectangle(r, brush + BrushOffset, filled, radiusX, radiusY, angle, brushWidth);
		hr = S_OK;
	}

	return hr;
}

HRESULT EngineGUI::DrawText(const char *txt, RECT r, unsigned int format, unsigned int brush, eAlignMode alignX, eAlignMode alignY, bool acceptMultiLine)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		r.left   += DrawOffset.x;
		r.right  += DrawOffset.x;
		r.top    += DrawOffset.y;
		r.bottom += DrawOffset.y;
		pRender->DrawText(txt, r, format, brush + BrushOffset, alignX, alignY, acceptMultiLine);
		hr = S_OK;
	}

	return hr;
}

POINT EngineGUI::getTextSize(const char *txt, POINT maxSize, unsigned int format)
{
	POINT size = { 0, 0 };

	if(pRender != NULL) {
		size = pRender->getTextSize(txt, maxSize, format);
	}

	return size;
}

HRESULT EngineGUI::DrawLine(POINT start, POINT end, unsigned int brush)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		start.x += DrawOffset.x;
		end.x   += DrawOffset.x;
		start.y += DrawOffset.y;
		end.y   += DrawOffset.y;
		hr = pRender->DrawLine(start, end, brush + BrushOffset, brushWidth);
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
		hr = pRender->DrawEllipse(center, rx, ry, brush + BrushOffset, filled, brushWidth);
	}

	return hr;
}

HRESULT EngineGUI::DrawArc(POINT start, POINT end, float rx, float ry, float angle, bool isClockWise, unsigned int brush)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		start.x += DrawOffset.x;
		end.x   += DrawOffset.x;
		start.y += DrawOffset.y;
		end.y   += DrawOffset.y;
		hr = pRender->DrawArc(start, end, rx, ry, angle, isClockWise, brush + BrushOffset, brushWidth);
	}

	return hr;
}

HRESULT EngineGUI::DrawPictureFromFile(char *filename, POINT start, POINT size)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		start.x += DrawOffset.x;
		start.y += DrawOffset.y;
		hr = pRender->DrawPictureFromFile(filename, start, size);
	}

	return hr;
}

HRESULT EngineGUI::DrawPictureFromResource(int id, POINT start, POINT size)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRender != NULL) {
		start.x += DrawOffset.x;
		start.y += DrawOffset.y;
		hr = pRender->DrawPictureFromResource(id, start, size);
	}

	return hr;
}

HRESULT EngineGUI::DrawRectangle3D(RECT r, float sizeZ, unsigned int brushBG, unsigned int brushIntBorder, unsigned int brushExtBorder,
	bool filled, float radiusX, float radiusY, float angle)
{
	if(pRender != NULL) {
		r.left   += DrawOffset.x;
		r.right  += DrawOffset.x;
		r.top    += DrawOffset.y;
		r.bottom += DrawOffset.y;
		return pRender->DrawRectangle3D(r, sizeZ, brushBG + BrushOffset, brushIntBorder + BrushOffset, brushExtBorder + BrushOffset,
			filled, radiusX, radiusY, angle);
	}

	return HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);
}
