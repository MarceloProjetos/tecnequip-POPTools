#ifndef ENGINERENDERD2D_H
#define ENGINERENDERD2D_H

#include <vector>

using namespace std;

#include <string.h>

#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")

#include "resource.h"
#include "EngineRender.h"
#include "ResourceFontContext.h"

// Estrutura que contem informacoes de um formato de texto
typedef struct {
	unsigned int       height;
	unsigned int       width;
	IDWriteTextFormat *format;
} tTextFormat;

// Estrutura que contem informacoes de um pincel: pode ser solido, gradiente, etc...
typedef struct {
	eBrushType brushType;
	struct {
		ID2D1SolidColorBrush     *pBrush;
	} solid;

	struct {
		unsigned int                 angle;
		ID2D1LinearGradientBrush    *pBrush;
		ID2D1GradientStopCollection *pGradientStops;
	} linear;

	struct {
		eOffsetMode                  mode;
		D2D1_POINT_2F                offset;
		ID2D1RadialGradientBrush    *pBrush;
		ID2D1GradientStopCollection *pGradientStops;
	} radial;
} tBrushDataD2D;

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
	IDWriteFontCollection *pFontCollection;
    ResourceFontContext   *pFontContext;

	std::vector<tBrushDataD2D > Brushes;
	std::vector<tTextFormat> TextFormats;

	HRESULT DrawPicture(IWICBitmapDecoder *pDecoder, POINT start, POINT size);

	HRESULT Flush(void);

public:
	EngineRenderD2D(void);
	~EngineRenderD2D(void);

	HRESULT CreateRenderTarget  (HWND hwnd);
	HRESULT ResizeRenderTarget  (HWND hwnd);
	HRESULT CreateBrush         (unsigned int &index, unsigned int rgb, float alpha = 1.0f);
	HRESULT CreateLinearGradient(unsigned int &index, unsigned int brushStart, unsigned int brushEnd, unsigned int angle  = 0);
	HRESULT CreateRadialGradient(unsigned int &index, unsigned int brushStart, unsigned int brushEnd, POINT        offset, eOffsetMode mode = eOffsetMode_Units);
	HRESULT CreateTextFormat    (unsigned int rgb, unsigned int &index);

	POINT   getTextSize     (const char *txt, POINT maxSize, unsigned int format);

	ID2D1Brush               *getBrush              (unsigned int brush, D2D1_RECT_F rf);
	ID2D1SolidColorBrush     *getSolidColorBrush    (unsigned int brush);
	ID2D1LinearGradientBrush *getLinearGradientBrush(unsigned int brush, D2D1_RECT_F rf);
	ID2D1RadialGradientBrush *getRadialGradientBrush(unsigned int brush, D2D1_RECT_F rf);

	void    StartDraw(void);
	void    Clear(unsigned int brush);
	HRESULT EndDraw(void);

	eTranslatedError TranslateError(HRESULT hr);

	HRESULT DrawPolygon  (vector<POINT> points, unsigned int brush, bool filled = true, unsigned int angle = 0, float brushWidth = 2.0f);
	void    DrawRectangle(RECT r, unsigned int brush, bool filled = true, unsigned int radiusX = 0, unsigned int radiusY = 0, unsigned int angle = 0, float brushWidth = 2.0f);
	HRESULT DrawEllipse  (POINT center, float rx, float ry, unsigned int brush, bool filled = true, float brushWidth = 2.0f);
	HRESULT DrawArc      (POINT start, POINT end, float rx, float ry, float angle, bool isClockWise, unsigned int brush, float brushWidth = 2.0f);
	void    DrawText     (const char *txt, RECT r, unsigned int format, unsigned int brush, eAlignMode alignX, eAlignMode alignY, bool acceptMultiLine);
	HRESULT DrawLine     (POINT start, POINT end, unsigned int brush, unsigned int angle = 0, float brushWidth = 2.0f);

	HRESULT DrawPictureFromFile    (char *filename, POINT start, POINT size);
	HRESULT DrawPictureFromResource(int   id      , POINT start, POINT size);

	HRESULT DrawRectangle3D(RECT r, float sizeZ, unsigned int brushBG, unsigned int brushGradient, unsigned int brushIntBorder, unsigned int brushExtBorder,
		bool filled, float radiusX, float radiusY, float angle);

	// Funcao utilizada para fazer testes, evitando misturar codigo de teste em funcoes ja existentes e funcionando
	void DrawTest(void);
};

#endif