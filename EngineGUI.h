#ifndef ENGINEGUI_H
#define ENGINEGUI_H

#include <Windows.h>
#include "EngineRender.h"

#include <vector>

class EngineGUI
{
private:
	void Init(void);
	void FreeAll(void);
	void FreeRender(void);

	void CreateBackgroundBrush(void);

	void InvalidateTarget(void);

	EngineRender *pRender;

	std::vector<COLORREF> Brushes;
	unsigned int BrushOffset;

	POINT DrawOffset;

	HWND Target;
	RECT LastTargetRect;

	bool TargetInvalid;

public:
	EngineGUI(void);
	EngineGUI(EngineRender *pR);
	EngineGUI(EngineRender *pR, COLORREF BackgroundColor);

	~EngineGUI(void);

	HRESULT SetTarget(HWND hwnd);

	HRESULT SetRender(EngineRender  *pR);
	HRESULT GetRender(EngineRender **ppRender);

	HRESULT SetBackgroundColor(COLORREF rgb);
	unsigned int CreateBrush(COLORREF rgb);

	HRESULT StartDraw(void);
	HRESULT SetDrawOffset(POINT Offset);
	HRESULT EndDraw(void);

	HRESULT DrawRectangle(RECT r, unsigned int brush, bool filled = true, unsigned int radiusX = 0, unsigned radiusY = 0, unsigned int angle = 0);
	HRESULT DrawEllipse(RECT r, unsigned int brush, bool filled = true);
	HRESULT DrawEllipse(POINT center, float rx, float ry, unsigned int brush, bool filled = true);
	HRESULT DrawText(const char *txt, RECT r, unsigned int format, unsigned int brush);
	HRESULT DrawLine(POINT start, POINT end, unsigned int brush);
};

#endif