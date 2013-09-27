#ifndef ENGINERENDERD2D_H
#define ENGINERENDERD2D_H

#include <vector>

#include <string.h>

#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")

#include "EngineRender.h"

using namespace std;

// Estrutura que contem informacoes de um formato de texto
typedef struct {
	unsigned int       height;
	unsigned int       width;
	IDWriteTextFormat *format;
} tTextFormat;

class EngineRenderD2D : public EngineRender
{
private:
	void Init(void);
	void FreeAll(void);
	void FreeRenderTarget(void);

	ID2D1Factory          *pD2DFactory;
	IWICImagingFactory    *pWICFactory;
	IDWriteFactory        *pWriteFactory;
	ID2D1HwndRenderTarget *pRT;

	std::vector<ID2D1SolidColorBrush *> Brushes;
	std::vector<tTextFormat           > TextFormats;

	HRESULT DrawPicture(IWICBitmapDecoder *pDecoder, POINT start, POINT size);

	HRESULT Flush(void);

public:
	EngineRenderD2D(void);
	~EngineRenderD2D(void);

	HRESULT CreateRenderTarget(HWND hwnd);
	HRESULT ResizeRenderTarget(HWND hwnd);
	HRESULT CreateBrush(unsigned int rgb, unsigned int &index);
	HRESULT CreateTextFormat(unsigned int rgb, unsigned int &index);

	void    StartDraw(void);
	void    Clear(unsigned int brush);
	HRESULT EndDraw(void);

	void    DrawRectangle(RECT r, unsigned int brush, bool filled = true, unsigned int radiusX = 0, unsigned int radiusY = 0, unsigned int angle = 0, float brushWidth = 2.0f);
	HRESULT DrawEllipse  (POINT center, float rx, float ry, unsigned int brush, bool filled = true, float brushWidth = 2.0f);
	HRESULT DrawArc      (POINT start, POINT end, float rx, float ry, float angle, bool isClockWise, unsigned int brush, float brushWidth = 2.0f);
	void    DrawText     (const char *txt, RECT r, unsigned int format, unsigned int brush, eAlignMode alignX, eAlignMode alignY);
	HRESULT DrawLine     (POINT start, POINT end, unsigned int brush, float brushWidth = 2.0f);

	HRESULT DrawPictureFromFile    (char *filename, POINT start, POINT size);
	HRESULT DrawPictureFromResource(int   id      , POINT start, POINT size);

	HRESULT DrawRectangle3D(RECT r, float sizeZ, unsigned int brushBG, unsigned int brushIntBorder, unsigned int brushExtBorder,
		bool filled, float radiusX, float radiusY, float angle);

	void Teste(void);
};

#endif