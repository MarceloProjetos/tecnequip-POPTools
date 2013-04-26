#ifndef ENGINERENDERD2D_H
#define ENGINERENDERD2D_H

#include <vector>

#include <d2d1.h>
#include <dwrite.h>

#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")

#include "EngineRender.h"

class EngineRenderD2D : public EngineRender
{
private:
	void Init(void);
	void FreeAll(void);
	void FreeRenderTarget(void);

	ID2D1Factory          *pD2DFactory;
	IDWriteFactory        *pWriteFactory;
	ID2D1HwndRenderTarget *pRT;

	std::vector<ID2D1SolidColorBrush *> Brushes;
	std::vector<IDWriteTextFormat    *> TextFormats;

public:
	EngineRenderD2D(void);
	~EngineRenderD2D(void);

	HRESULT CreateRenderTarget(HWND hwnd);
	HRESULT CreateBrush(unsigned int rgb, unsigned int &index);
	HRESULT CreateTextFormat(unsigned int rgb, unsigned int &index);

	void    StartDraw(void);
	void    Clear(unsigned int brush);
	HRESULT EndDraw(void);

	void DrawRectangle(RECT r, unsigned int brush, bool filled = true);
	HRESULT DrawEllipse(POINT center, float rx, float ry, unsigned int brush, bool filled = true);
	void DrawText(char *txt, RECT r, unsigned int format, unsigned int brush);
	HRESULT DrawLine(POINT start, POINT end, unsigned int brush);
};

#endif