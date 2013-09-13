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

	if (SUCCEEDED(hr)) {
		// Nao pode inicializar ainda, chamamos CoInitialize no main e somos global
		pWICFactory = NULL;
	}
}

void EngineRenderD2D::FreeRenderTarget(void)
{
	unsigned int i;
	for(i=0; i<TextFormats.size(); i++) {
		SafeRelease(&TextFormats[i].format);
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
	SafeRelease(&pWICFactory);
	SafeRelease(&pWriteFactory);
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
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRT != NULL) {
		tTextFormat txt;
		txt.format = NULL;
		txt.height = 13;
		txt.width  = 8;

		// Create a DirectWrite text format object.
		hr = pWriteFactory->CreateTextFormat(
			L"Lucida Console",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			(float)txt.height,
			L"", //locale
			&txt.format
			);

		if(SUCCEEDED(hr)) {
			index = TextFormats.size();
			TextFormats.push_back(txt);
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

#include <cmath>

inline float DegreeToRadian(float angle)
{
    return 3.14159f * angle / 180.0f;
}

void EngineRenderD2D::Teste(void)
{/*
	RECT r = { 300, 300, 350, 400 };
	DrawRectangle3D(r, 30, 21, 23, true, 10, 10, 0);

	RECT r2 = { 200, 200, 250, 250 };
	DrawRectangle3D(r2, 30, 21, 23, false, 20, 20, 45);
*/}

HRESULT EngineRenderD2D::DrawRectangle3D(RECT r, float sizeZ, unsigned int brushBG, unsigned int brushIntBorder, unsigned int brushExtBorder,
	bool filled, float radiusX, float radiusY, float angle)
{
//	DrawRectangle(r, brushBG       , true , (unsigned int)radiusX, (unsigned int)radiusY, angle);
//	DrawRectangle(r, brushExtBorder, false, (unsigned int)radiusX, (unsigned int)radiusY, angle);
//	return S_OK;
	ID2D1PathGeometry *pGeometry;
	HRESULT hr = pD2DFactory->CreatePathGeometry(&pGeometry);

	if(SUCCEEDED(hr)) {
		// Pontos das linhas na curvatura do canto superior direito da caixa.
		// 0: Ponto inicial da linha 1
		// 1: Ponto final da linha 1
		// 2: Ponto inicial da linha 2
		// 3: Ponto final da linha 2
		D2D1_POINT_2F pointLines[4];

		D2D1_RECT_F rectRoundedRect;
		ID2D1GeometrySink *pSink = NULL;

		// Corrige as coordenadas para que o ponto inicial seja o canto superior esquerdo e nao onde
		// se inicia o arco que liga a borda superior com a lateral esquerda...
		r.left   += (long)radiusX;
		r.right  += (long)radiusX;

		float sizeX = (float)(r.right - r.left) - 2*radiusX, sizeY = (float)(r.bottom - r.top) - 2*radiusY;

		hr = pGeometry->Open(&pSink);
		if (SUCCEEDED(hr)) {
			static float
				cos30 = cos(DegreeToRadian(30.0f)), sin15 = sin(DegreeToRadian(15.0f)),
				sin30 = sin(DegreeToRadian(30.0f)), sin75 = sin(DegreeToRadian(75.0f));
			float cos60 = sin30, sin60 = cos30;

			pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

			// Iniciando o desenho da figura. Devido a forma que eu criei o teste, o desenho se inicia pelo canto
			// inferior direito do retangulo e segue em sentido anti-horario.
			// Agora que ja foi feito assim, nao sera mudado!

			D2D1_POINT_2F point = D2D1::Point2F(float(r.left) + sizeX, float(r.bottom));
			pSink->BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);

			rectRoundedRect.bottom = point.y; // Ponto inferior do poligono
			rectRoundedRect.right  = point.x + radiusX; // Acrescentando-se o raio temos a medida da lateral direita

			// Arco ligando a borda inferior a lateral direita
			point.x += (radiusX *         sin30);
			point.y -= (radiusY * sin15 * sin30) / sin75;
			pSink->AddArc(
				D2D1::ArcSegment(
					point,
					D2D1::SizeF(radiusX, radiusY),
					0.0f,
					D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);

			// Lateral direita inferior
			point.x += sizeZ * cos30;
			point.y -= sizeZ * sin30;
			pSink->AddLine(point);

			// Arco ligando a lateral direita inferior a lateral direita
			point.x += radiusX * cos60;
			point.y -= radiusY * sin60;
			pSink->AddArc(
				D2D1::ArcSegment(
					point,
					D2D1::SizeF(radiusX, radiusY),
					0.0f,
					D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);

			// Lateral direita
			point.y -= sizeY;
			pSink->AddLine(point);

			pointLines[3] = point; // Ponto final da segunda linha

			//Arco ligando a lateral direita a borda superior
			point.x -= radiusX;
			point.y -= radiusY;
			pSink->AddArc(
				D2D1::ArcSegment(
					point,
					D2D1::SizeF(radiusX, radiusY),
					0.0f,
					D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);

			pointLines[1] = point; // Ponto final da primeira linha

			// Borda superior
			point.x -= sizeX;
			pSink->AddLine(point);

			// Arco ligando a borda superior a borda superior esquerda
			point.x -= (radiusX *         sin30);
			point.y += (radiusY * sin15 * sin30) / sin75;
			pSink->AddArc(
				D2D1::ArcSegment(
					point,
					D2D1::SizeF(radiusX, radiusY),
					0.0f,
					D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);

			// Borda superior esquerda
			point.x -= sizeZ * cos30;
			point.y += sizeZ * sin30;
			pSink->AddLine(point);

			// Arco ligando a borda superior esquerda a lateral esquerda
			point.x -= radiusX * cos60;
			point.y += radiusY * sin60;
			pSink->AddArc(
				D2D1::ArcSegment(
					point,
					D2D1::SizeF(radiusX, radiusY),
					0.0f,
					D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);

			rectRoundedRect.left = point.x; // Neste ponto temos a lateral esquerda
			rectRoundedRect.top  = point.y - radiusY; // Ao removermos o raio, temos o topo do retangulo

			// Lateral esquerda
			point.y += sizeY;
			pSink->AddLine(point);

			// Arco ligando a lateral esquerda a borda inferior
			point.x += radiusX;
			point.y += radiusY;
			pSink->AddArc(
				D2D1::ArcSegment(
					point,
					D2D1::SizeF(radiusX, radiusY),
					0.0f,
					D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);

			// Figura finalizada. Fecha em modo CLOSED para que seja feita a linha da borda inferior
			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

			hr = pSink->Close();

			SafeRelease(&pSink);

			// Calcula o ponto de rotacao (Se usado)
			D2D1_POINT_2F xy;
			if(angle) {
				xy.x = rectRoundedRect.left + (rectRoundedRect.right  - rectRoundedRect.left)/2;
				xy.y = rectRoundedRect.top  + (rectRoundedRect.bottom - rectRoundedRect.top )/2;
				pRT->SetTransform(D2D1::Matrix3x2F::Rotation(angle, xy));
			}

			// Desenha o fundo
			if(filled) {
				pRT->FillGeometry(pGeometry, Brushes[brushBG]);
			}

			if(brushBG != brushIntBorder) {
				// Desenha a borda interna
				pRT->DrawRoundedRectangle(D2D1::RoundedRect(rectRoundedRect, radiusX, radiusY), Brushes[brushIntBorder], 4);

				// Faltam os pontos iniciais das linhas. Calculamos atraves de rectRoundedRect
				pointLines[0].x = rectRoundedRect.right - radiusX; // Ponto inicial da primeira linha
				pointLines[0].y = rectRoundedRect.top; // Ponto inicial da primeira linha
				pointLines[2].x = rectRoundedRect.right; // Ponto inicial da segunda linha
				pointLines[2].y = rectRoundedRect.top + radiusY; // Ponto inicial da segunda linha

				// Desenha as duas linhas superiores
				pRT->DrawLine(pointLines[0], pointLines[1], Brushes[brushIntBorder], 4);
				pRT->DrawLine(pointLines[2], pointLines[3], Brushes[brushIntBorder], 4);
			}

			// Desenha a borda externa
			pRT->DrawGeometry(pGeometry, Brushes[brushExtBorder], 4);

			// Se usando rotacao, volta ao normal
			if(angle) pRT->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f, xy));

			// Descarta a geometria criada
			SafeRelease(&pGeometry);
		}
	}

	return hr;
}

#include "resource.h"

HRESULT EngineRenderD2D::DrawPicture(IWICBitmapDecoder *pDecoder, POINT start, POINT size)
{
	HRESULT hr = S_OK;
    ID2D1Bitmap            *m_pD2DBitmap = NULL;
    IWICFormatConverter    *m_pConvertedSourceBitmap = NULL;

	// Retrieve the first frame of the image from the decoder
	IWICBitmapFrameDecode *pFrame = NULL;

	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pFrame);
	}

	// Format convert the frame to 32bppPBGRA
	if (SUCCEEDED(hr))
	{
		hr = pWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pConvertedSourceBitmap->Initialize(
			pFrame,                          // Input bitmap to convert
			GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
			WICBitmapDitherTypeNone,         // Specified dither pattern
			NULL,                            // Specify a particular palette 
			0.f,                             // Alpha threshold
			WICBitmapPaletteTypeCustom       // Palette translation type
			);
	}

	// D2DBitmap may have been released due to device loss. 
	// If so, re-create it from the source bitmap
	if (m_pConvertedSourceBitmap)
	{
		pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
		SafeRelease(&m_pConvertedSourceBitmap);
	}

	// Draws an image and scales it to the current window size
	if (m_pD2DBitmap)
	{
		if(size.x == 0 || size.y == 0) {
			D2D1_SIZE_F sizeBMP = m_pD2DBitmap->GetSize();
			size.x = size.x ? size.x : (long)sizeBMP.width;
			size.y = size.y ? size.y : (long)sizeBMP.height;
		}

		D2D1_RECT_F rectangle = { float(start.x), float(start.y),
			float(start.x + size.x), float(start.y + size.y) };

		pRT->DrawBitmap(m_pD2DBitmap, rectangle);

		SafeRelease(&m_pD2DBitmap);
	}

	return hr;
}

HRESULT EngineRenderD2D::DrawPictureFromResource(int id, POINT start, POINT size)
{
	HRESULT hr = S_OK;

	// WIC interface pointers.
	IWICStream *pIWICStream = NULL;
	IWICBitmapDecoder *pDecoder = NULL;

	// Resource management.
	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;

	if(pWICFactory == NULL) {
		// Create WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pWICFactory)
			);
	} else {
		hr = S_OK;
	}

	// Locate the resource in the application's executable.
	imageResHandle = FindResource(
	   NULL,             // This component.
	   MAKEINTRESOURCE(id),   // Resource name.
	   MAKEINTRESOURCE(RT_RCDATA));        // Resource type.

	hr = (imageResHandle ? S_OK : E_FAIL);

	// Load the resource to the HGLOBAL.
	if (SUCCEEDED(hr)){
	   imageResDataHandle = LoadResource(NULL, imageResHandle);
	   hr = (imageResDataHandle ? S_OK : E_FAIL);
	}

	// Lock the resource to retrieve memory pointer.
	if (SUCCEEDED(hr)){
	   pImageFile = LockResource(imageResDataHandle);
	   hr = (pImageFile ? S_OK : E_FAIL);
	}

	// Calculate the size.
	if (SUCCEEDED(hr)){
	   imageFileSize = SizeofResource(NULL, imageResHandle);
	   hr = (imageFileSize ? S_OK : E_FAIL);
	}

	// Create a WIC stream to map onto the memory.
	if (SUCCEEDED(hr)){
	   hr = pWICFactory->CreateStream(&pIWICStream);
	}

	// Initialize the stream with the memory pointer and size.
	if (SUCCEEDED(hr)){
	   hr = pIWICStream->InitializeFromMemory(
			 reinterpret_cast<BYTE*>(pImageFile),
			 imageFileSize);
	}

	// Create a decoder for the stream.
	if (SUCCEEDED(hr)){
	   hr = pWICFactory->CreateDecoderFromStream(
		  pIWICStream,                   // The stream to use to create the decoder
		  NULL,                          // Do not prefer a particular vendor
		  WICDecodeMetadataCacheOnLoad,  // Cache metadata when needed
		  &pDecoder);                   // Pointer to the decoder
	}

	if (SUCCEEDED(hr)){
		DrawPicture(pDecoder, start, size);

		SafeRelease(&pIWICStream);
		SafeRelease(&pDecoder);
	}

	return hr;
}

HRESULT EngineRenderD2D::DrawPictureFromFile(char *filename, POINT start, POINT size)
{
	HRESULT hr;
    
	IWICBitmapDecoder *pDecoder = NULL;

	if(pWICFactory == NULL) {
		// Create WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pWICFactory)
			);
	} else {
		hr = S_OK;
	}

	if (SUCCEEDED(hr)) {
		unsigned int len = strlen(filename) + 1;
		wchar_t *pWideChar = new wchar_t[len];

		memset(pWideChar,0,len);
		::MultiByteToWideChar(CP_ACP, NULL, filename, -1, pWideChar, len);

		hr = pWICFactory->CreateDecoderFromFilename(
			pWideChar,                      // Image to be decoded
			NULL,                            // Do not prefer a particular vendor
			GENERIC_READ,                    // Desired read access to the file
			WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
			&pDecoder                        // Pointer to the decoder
			);

		delete [] pWideChar;
	}

	if (SUCCEEDED(hr)){
		DrawPicture(pDecoder, start, size);

		SafeRelease(&pDecoder);
	}

	return hr;
}

HRESULT EngineRenderD2D::EndDraw(void)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	Teste();

	if(pRT != NULL) {
		hr = pRT->EndDraw();
	}

	return hr;
}

void EngineRenderD2D::DrawRectangle(RECT r, unsigned int brush, bool filled, unsigned int radiusX, unsigned int radiusY, unsigned int angle, float brushWidth)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRT != NULL && brush < Brushes.size()) {
		D2D1_ROUNDED_RECT rf;
		D2D1_POINT_2F xy;
		rf.rect = D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom);
		rf.radiusX = (float)radiusX;
		rf.radiusY = (float)(radiusY ? radiusY : radiusX);

		if(angle) {
			xy.x = rf.rect.left + (rf.rect.right  - rf.rect.left)/2;
			xy.y = rf.rect.top  + (rf.rect.bottom - rf.rect.top )/2;
			pRT->SetTransform(D2D1::Matrix3x2F::Rotation((float)angle, xy));
		}

		if(filled) {
			rf.radiusX == 0.0 ? pRT->FillRectangle(rf.rect, Brushes[brush]            ) : pRT->FillRoundedRectangle(rf, Brushes[brush]            );
		} else {
			rf.radiusX == 0.0 ? pRT->DrawRectangle(rf.rect, Brushes[brush], brushWidth) : pRT->DrawRoundedRectangle(rf, Brushes[brush], brushWidth);
		}

		if(angle) pRT->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f, xy));
	}
}

void EngineRenderD2D::DrawText(const char *txt, RECT r, unsigned int format, unsigned int brush, eAlignMode alignX, eAlignMode alignY)
{
	if(pRT != NULL && brush < Brushes.size() && format < TextFormats.size()) {
		// Calcula alinhamento horizontal
		if(alignX != eAlignMode_TopLeft) {
			unsigned int len = strlen(txt) * TextFormats[format].width;

			if(alignX == eAlignMode_Center) {
				r.left += (r.right - r.left - len)/2;
			} else if(alignX == eAlignMode_BottomRight) {
				r.left  =  r.right - len;
			}
		}

		// Calcula alinhamento vertical
		if(alignY == eAlignMode_Center) {
			r.top += (r.bottom - r.top - TextFormats[format].height)/2;
		} else if(alignY == eAlignMode_BottomRight) {
			r.top  =  r.bottom - TextFormats[format].height;
		}

		int size = strlen(txt) * 2 + 1;
		WCHAR *wtxt = new WCHAR[size];
		D2D1_RECT_F rf = D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom);

		swprintf(wtxt, size, L"%S", txt);
		pRT->DrawText(wtxt, static_cast<UINT>(wcslen(wtxt)), TextFormats[format].format, &rf, Brushes[brush]);

		delete [] wtxt;
	}
}

HRESULT EngineRenderD2D::DrawLine(POINT start, POINT end, unsigned int brush, float brushWidth)
{
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_HANDLE);

	if(pRT != NULL && brush < Brushes.size()) {
		pRT->DrawLine(D2D1::Point2F((float)start.x, (float)start.y),
					  D2D1::Point2F((float)  end.x, (float)  end.y), Brushes[brush], brushWidth);
		hr = S_OK;
	}

	return hr;
}

HRESULT EngineRenderD2D::DrawEllipse(POINT center, float rx, float ry, unsigned int brush, bool filled, float brushWidth)
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
			pRT->DrawEllipse(&ellipse, Brushes[brush], brushWidth);
		}

		hr = S_OK;
	}

	return hr;
}

HRESULT EngineRenderD2D::DrawArc(POINT start, POINT end, float rx, float ry, float angle, bool isClockWise, unsigned int brush, float brushWidth)
{
	ID2D1PathGeometry *pGeometry;
	HRESULT hr = pD2DFactory->CreatePathGeometry(&pGeometry);

	if(SUCCEEDED(hr)) {
		ID2D1GeometrySink *pSink = NULL;

		hr = pGeometry->Open(&pSink);
		if (SUCCEEDED(hr)) {
			pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

			// Iniciando o desenho da figura. Devido a forma que eu criei o teste, o desenho se inicia pelo canto
			// inferior direito do retangulo e segue em sentido anti-horario.
			// Agora que ja foi feito assim, nao sera mudado!

			D2D1_POINT_2F point = D2D1::Point2F(float(start.x), float(start.y));
			pSink->BeginFigure(point, D2D1_FIGURE_BEGIN_FILLED);

			// Arco para o ponto final, usando os dados fornecidos
			point = D2D1::Point2F(float(end.x), float(end.y));
			pSink->AddArc(
				D2D1::ArcSegment(
					point,
					D2D1::SizeF(rx, ry),
					0.0f,
					isClockWise ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);

			// Finaliza o arco em modo OPEN para que nao seja adicionada uma linha entre suas extremidades
			pSink->EndFigure(D2D1_FIGURE_END_OPEN);

			hr = pSink->Close();

			SafeRelease(&pSink);

			// Agora desenha o arco
			pRT->DrawGeometry(pGeometry, Brushes[brush], brushWidth);

			SafeRelease(&pGeometry);
		}
	}

	return hr;
}
