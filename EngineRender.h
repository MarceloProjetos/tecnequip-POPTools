#ifndef ENGINERENDER_H
#define ENGINERENDER_H

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

	virtual void DrawRectangle(RECT r, unsigned int brush, bool filled = true, unsigned int radiusX = 0, unsigned int radiusY = 0, unsigned int angle = 0)=0;
	virtual HRESULT DrawEllipse(POINT center, float rx, float ry, unsigned int brush, bool filled = true)=0;
	virtual void DrawText(const char *txt, RECT r, unsigned int format, unsigned int brush)=0;
	virtual HRESULT DrawLine(POINT start, POINT end, unsigned int brush)=0;
};

#endif