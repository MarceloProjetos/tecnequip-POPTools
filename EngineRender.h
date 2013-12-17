#ifndef ENGINERENDER_H
#define ENGINERENDER_H

// Enumeracao com os tipos de alinhamento suportados
enum eAlignMode { eAlignMode_TopLeft = 0, eAlignMode_Center, eAlignMode_BottomRight };

// Enumeracao com os tipos de pincel disponiveis.
enum eBrushType { eBrushType_SolidColor, eBrushType_GradientLinear };

class EngineRender
{
private:
	virtual void Init(void)=0;
	virtual void FreeAll(void)=0;

public:
	virtual ~EngineRender(void) { };

	virtual HRESULT CreateRenderTarget(HWND hwnd)=0;
	virtual HRESULT ResizeRenderTarget(HWND hwnd)=0;
	virtual HRESULT CreateBrush   (unsigned int &index, unsigned int rgb, float alpha = 1.0f)=0;
	virtual HRESULT CreateGradient(unsigned int &index, unsigned int brushStart, unsigned int brushEnd, unsigned int angle = 0)=0;

	virtual POINT getTextSize(const char *txt, POINT maxSize, unsigned int format)=0;

	virtual void    StartDraw(void)=0;
	virtual void    Clear(unsigned int brush)=0;
	virtual HRESULT EndDraw(void)=0;

	virtual HRESULT DrawPolygon  (vector<POINT> points, unsigned int brush, bool filled = true, unsigned int angle = 0, float brushWidth = 2.0f) = 0;
	virtual void    DrawRectangle(RECT r, unsigned int brush, bool filled = true, unsigned int radiusX = 0, unsigned int radiusY = 0, unsigned int angle = 0, float brushWidth = 2.0f)=0;
	virtual HRESULT DrawEllipse  (POINT center, float rx, float ry, unsigned int brush, bool filled = true, float brushWidth = 2.0f)=0;
	virtual HRESULT DrawArc      (POINT start, POINT end, float rx, float ry, float angle, bool isClockWise, unsigned int brush, float brushWidth = 2.0f)=0;
	virtual void    DrawText     (const char *txt, RECT r, unsigned int format, unsigned int brush, eAlignMode alignX, eAlignMode alignY, bool acceptMultiLine = false)=0;
	virtual HRESULT DrawLine     (POINT start, POINT end, unsigned int brush, unsigned int angle = 0, float brushWidth = 2.0f)=0;

	virtual HRESULT DrawPictureFromFile    (char *filename, POINT start, POINT size)=0;
	virtual HRESULT DrawPictureFromResource(int   id      , POINT start, POINT size)=0;

	virtual HRESULT DrawRectangle3D(RECT r, float sizeZ, unsigned int brushBG, unsigned int brushGradient, unsigned int brushIntBorder, unsigned int brushExtBorder,
		bool filled = true, float radiusX = 20.0f, float radiusY = 20.0f, float angle = 0.0f)=0;
};

#endif