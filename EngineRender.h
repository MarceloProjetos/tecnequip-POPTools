#ifndef ENGINERENDER_H
#define ENGINERENDER_H

// Enumeracao com os tipos de alinhamento suportados
enum eAlignMode { eAlignMode_TopLeft = 0, eAlignMode_Center, eAlignMode_BottomRight };

class EngineRender
{
private:
	virtual void Init(void)=0;
	virtual void FreeAll(void)=0;

public:
	virtual ~EngineRender(void) { };

	virtual HRESULT CreateRenderTarget(HWND hwnd)=0;
	virtual HRESULT CreateBrush(unsigned int rgb, unsigned int &index)=0;

	virtual void    StartDraw(void)=0;
	virtual void    Clear(unsigned int brush)=0;
	virtual HRESULT EndDraw(void)=0;

	virtual void    DrawRectangle(RECT r, unsigned int brush, bool filled = true, unsigned int radiusX = 0, unsigned int radiusY = 0, unsigned int angle = 0, float brushWidth = 2.0f)=0;
	virtual HRESULT DrawEllipse  (POINT center, float rx, float ry, unsigned int brush, bool filled = true, float brushWidth = 2.0f)=0;
	virtual HRESULT DrawArc      (POINT start, POINT end, float rx, float ry, float angle, bool isClockWise, unsigned int brush, float brushWidth = 2.0f)=0;
	virtual void    DrawText     (const char *txt, RECT r, unsigned int format, unsigned int brush, eAlignMode alignX, eAlignMode alignY)=0;
	virtual HRESULT DrawLine     (POINT start, POINT end, unsigned int brush, float brushWidth = 2.0f)=0;

	virtual HRESULT DrawPictureFromFile    (char *filename, POINT start, POINT size)=0;
	virtual HRESULT DrawPictureFromResource(int   id      , POINT start, POINT size)=0;

	virtual HRESULT DrawRectangle3D(RECT r, float sizeZ, unsigned int brushBG, unsigned int brushIntBorder, unsigned int brushExtBorder,
		bool filled = true, float radiusX = 20.0f, float radiusY = 20.0f, float angle = 0.0f)=0;
};

#endif