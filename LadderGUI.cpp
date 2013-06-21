#include "poptools.h"

#include "EngineGUI.h"
#include "EngineRenderD2D.h"

#include <map>

// Variavel que indica se o cursor deve ser exibido. Liga e Desliga a cada 800 ms, conforme timer.
bool isCursorVisible = true;

// Colours with which to do syntax highlighting, configurable
extern SyntaxHighlightingColours HighlightColours;

/*** Estruturas auxiliares ***/

// Dados passados para as funcoes DrawGUI
typedef struct {
	POINT start;
	POINT size;
	RECT  region;
	RECT  regionSelected;
	bool  expanded;
	bool  DontDraw;
	LadderContext *context;
} tDataDrawGUI;

/*** Fim das estruturas auxiliares ***/

/*** Funcoes auxiliares ***/

inline bool PointInsideRegion(POINT p, RECT region)
{
	return (region.left < p.x) && (region.right > p.x) &&
		(region.top < p.y) && (region.bottom > p.y);
}

inline bool IsRegionZero(RECT region)
{
	return (region.left == 0) && (region.right == 0) && (region.top == 0) && (region.bottom == 0);
}

/*** Fim das funcoes auxiliares ***/

/*** Definicao da classe LaderGUI ***/

static const int INTERF_COLOR_3DLIGHT  = 0;
static const int INTERF_COLOR_3DFACE   = 1;
static const int INTERF_COLOR_3DSHADOW = 2;

class LadderGUI : public EngineGUI
{
private:
	POINT Grid1x1;
	POINT MousePosition;
	POINT DiagramSize;

	LadderElem *MouseOverElement;

	vector<RECT>         vectorConnectionDots;
	vector<LadderElem *> vectorExpandedElements;

	int HeaderHeight;

	array<unsigned int, 3>    InterfaceColors;
	SyntaxHighlightingColours HighlightColoursBrush;

	multimap<LadderElem*, pair<RECT, void(*)(LadderElem*)> > CommandsSingleClick;
	multimap<LadderElem*, pair<RECT, void(*)(LadderElem*)> > CommandsDoubleClick;

	bool MouseOver (RECT region);

	void PaintScrollAndSplitter(void);

public:
	LadderGUI(void);

	inline RECT getRECT(POINT Start, POINT Size);
	POINT       getGridSize(void) { return Grid1x1; }

	bool IsExpanded(LadderElem *elem);

	void setMousePosition(POINT xy);

	void  setDiagramSize(POINT size) { DiagramSize = size; }
	POINT getDiagramSize(void)       { return DiagramSize; }

	void DrawInit (void);
	void DrawStart(int OffsetX, int OffsetY);
	void DrawEnd  (void);

	void DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft);
	RECT DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, POINT GridSize, string ElemName = "teste", bool ShowExpand = true);
	void DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, pair<string, string> txt);

	RECT DrawElementBar(LadderElem *elem, int SelectedState, int StartGridY, int GridHeight);

	pair<POINT, POINT> DrawWire(POINT StartPoint, POINT EndPoint, bool StartHasConnectionDot);

	void MouseClick(int x, int y, bool isDown, bool isDouble);

	void AddCommand(LadderElem *elem, RECT region, void (*fnc)(LadderElem*), bool isDoubleClick = true);

	void CmdExpand(LadderElem *elem);
};

// Criacao do objeto que desenha a interface
static LadderGUI gui;

// Funcoes usadas nos comandos genericos
void CmdExpand(LadderElem *elem)
{
	gui.CmdExpand(elem);
}

void CmdShowDialog(LadderElem *elem)
{
	if(elem != nullptr) {
		ladder->CheckpointBegin("Editar Elemento");
		elem->ShowDialog(ladder->getContext());
		ladder->CheckpointEnd();
		ladder->updateContext();
	}
}

void CmdSelect(LadderElem *elem, int SelectedState)
{
	ladder->SelectElement(elem, SelectedState);
}

void CmdSelectLeft(LadderElem *elem)
{
	CmdSelect(elem, SELECTED_LEFT);
}

void CmdSelectRight(LadderElem *elem)
{
	CmdSelect(elem, SELECTED_RIGHT);
}

void CmdSelectAbove(LadderElem *elem)
{
	CmdSelect(elem, SELECTED_ABOVE);
}

void CmdSelectBelow(LadderElem *elem)
{
	CmdSelect(elem, SELECTED_BELOW);
}

// Construtores
LadderGUI::LadderGUI(void) : EngineGUI(new EngineRenderD2D)
{
	MousePosition.x = 0;
	MousePosition.y = 0;

	MouseOverElement = nullptr;

	Grid1x1.x = 60;
	Grid1x1.y = 30;

	HeaderHeight = FONT_HEIGHT + 5;
}

// Metodos privados
bool LadderGUI::MouseOver(RECT region)
{
	POINT p = {  MousePosition.x + ScrollXOffset, MousePosition.y + ScrollYOffset*Grid1x1.y };
	return PointInsideRegion(p, region);
}

void LadderGUI::PaintScrollAndSplitter(void)
{
	RECT r;
	// Fill around the scroll bars
	if(NeedHoriz) {
		r.top = IoListTop - ScrollHeight - 2 + ScrollYOffset*Grid1x1.y;
		r.bottom = IoListTop - 2 + ScrollYOffset*Grid1x1.y;
		DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DFACE]);
	}
	GetClientRect(MainWindow, &r);
	r.bottom += ScrollYOffset*Grid1x1.y;
	r.right += ScrollXOffset;
	r.left = r.right - ScrollWidth - 2;
	DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DFACE]);

	// Draw the splitter thing to grab to resize the I/O listview.
	GetClientRect(DrawWindow, &r);
	r.left  += ScrollXOffset;
	r.right += ScrollXOffset;

	r.top = IoListTop - 2 - RibbonHeight + ScrollYOffset*Grid1x1.y;
	r.bottom = IoListTop - RibbonHeight + ScrollYOffset*Grid1x1.y;
	DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DFACE  ]);
	r.top = IoListTop - 2 - RibbonHeight + ScrollYOffset*Grid1x1.y;
	r.bottom = IoListTop - 1 - RibbonHeight + ScrollYOffset*Grid1x1.y;
	DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DLIGHT ]);
	r.top = IoListTop - RibbonHeight + ScrollYOffset*Grid1x1.y;
	r.bottom = IoListTop + 1 - RibbonHeight + ScrollYOffset*Grid1x1.y;
	DrawRectangle(r, InterfaceColors[INTERF_COLOR_3DSHADOW]);
}

// Metodos Publicos
void LadderGUI::CmdExpand(LadderElem *elem)
{
	vector<LadderElem *>::iterator it;
	for(it = vectorExpandedElements.begin(); it != vectorExpandedElements.end(); it++) {
		if(elem == *it) {
			// Encontrado, portanto expandido. Remover da lista!
			vectorExpandedElements.erase(it);
			return;
		}
	}

	// Se nao encontrou, nao esta expandido. Adicionar na lista!
	vectorExpandedElements.push_back(elem);
}

inline RECT LadderGUI::getRECT(POINT Start, POINT Size)
{
	RECT r;

	r.left  = Grid1x1.x/2 + (Start.x + 1) * Grid1x1.x;
	r.right = r.left      + (Size.x     ) * Grid1x1.x;

	r.top    = Start.y         * Grid1x1.y + Grid1x1.y/2;
	r.bottom = r.top + (Size.y * Grid1x1.y);

	return r;
}

bool LadderGUI::IsExpanded(LadderElem *elem)
{
	vector<LadderElem *>::iterator it;
	for(it = vectorExpandedElements.begin(); it != vectorExpandedElements.end(); it++) {
		if(elem == *it) return true;
	}

	return false;
}

void LadderGUI::setMousePosition(POINT xy)
{
	RECT r;
	GetWindowRect(DrawWindow, &r);

	xy.x -= r.left;
	xy.y -= r.top;

	MousePosition = xy;
}

void LadderGUI::DrawInit(void)
{
	SetTarget(DrawWindow);
	SetBackgroundColor(HighlightColours.bg);

	HighlightColoursBrush.breakpoint       = CreateBrush(HighlightColours.breakpoint );
	HighlightColoursBrush.bus              = CreateBrush(HighlightColours.bus        );
	HighlightColoursBrush.comment          = CreateBrush(HighlightColours.comment    );
	HighlightColoursBrush.def              = CreateBrush(HighlightColours.def        );
	HighlightColoursBrush.lit              = CreateBrush(HighlightColours.lit        );
	HighlightColoursBrush.name             = CreateBrush(HighlightColours.name       );
	HighlightColoursBrush.op               = CreateBrush(HighlightColours.op         );
	HighlightColoursBrush.punct            = CreateBrush(HighlightColours.punct      );
	HighlightColoursBrush.rungNum          = CreateBrush(HighlightColours.rungNum    );
	HighlightColoursBrush.selected         = CreateBrush(HighlightColours.selected   );
	HighlightColoursBrush.simBusLeft       = CreateBrush(HighlightColours.simBusLeft );
	HighlightColoursBrush.simBusRight      = CreateBrush(HighlightColours.simBusRight);
	HighlightColoursBrush.simOff           = CreateBrush(HighlightColours.simOff     );
	HighlightColoursBrush.simOn            = CreateBrush(HighlightColours.simOn      );
	HighlightColoursBrush.simRungNum       = CreateBrush(HighlightColours.simRungNum );

	InterfaceColors[INTERF_COLOR_3DLIGHT ] = CreateBrush(GetSysColor(COLOR_3DLIGHT   ));
	InterfaceColors[INTERF_COLOR_3DFACE  ] = CreateBrush(GetSysColor(COLOR_3DFACE    ));
	InterfaceColors[INTERF_COLOR_3DSHADOW] = CreateBrush(GetSysColor(COLOR_3DDKSHADOW));
}

void LadderGUI::DrawStart(int OffsetX, int OffsetY)
{
	POINT OffsetXY = { -OffsetX, -OffsetY };

	SetDrawOffset(OffsetXY);
	StartDraw();

	MouseOverElement = nullptr;

	CommandsSingleClick.clear();
	CommandsDoubleClick.clear();
/*
	RECT r;
	GetClientRect(DrawWindow, &r);
	r.top += 100;

	DrawText("breakpoint", r, 0, HighlightColoursBrush.breakpoint);
	r.top += 20;

	DrawText("bus", r, 0, HighlightColoursBrush.bus);
	r.top += 20;

	DrawText("comment", r, 0, HighlightColoursBrush.comment);
	r.top += 20;

	DrawText("def", r, 0, HighlightColoursBrush.def);
	r.top += 20;

	DrawText("lit", r, 0, HighlightColoursBrush.lit);
	r.top += 20;

	DrawText("name", r, 0, HighlightColoursBrush.name);
	r.top += 20;

	DrawText("op", r, 0, HighlightColoursBrush.op);
	r.top += 20;

	DrawText("punct", r, 0, HighlightColoursBrush.punct);
	r.top += 20;

	DrawText("rungNum", r, 0, HighlightColoursBrush.rungNum);
	r.top += 20;

	DrawText("selected", r, 0, HighlightColoursBrush.selected);
	r.top += 20;

	DrawText("simBusLeft", r, 0, HighlightColoursBrush.simBusLeft);
	r.top += 20;

	DrawText("simBusRight", r, 0, HighlightColoursBrush.simBusRight);
	r.top += 20;

	DrawText("simOff", r, 0, HighlightColoursBrush.simOff);
	r.top += 20;

	DrawText("simOn", r, 0, HighlightColoursBrush.simOn);
	r.top += 20;

	DrawText("simRungNum", r, 0, HighlightColoursBrush.simRungNum);
	r.top += 20;*/
}

void LadderGUI::DrawEnd(void)
{
	vector<RECT>::iterator it;
	for(it = vectorConnectionDots.begin(); it != vectorConnectionDots.end(); it++) {
		DrawEllipse(*it, HighlightColoursBrush.breakpoint);
	}

	vectorConnectionDots.clear();

	PaintScrollAndSplitter();

	EndDraw();
}

RECT LadderGUI::DrawElementBar(LadderElem *elem, int SelectedState, int StartGridY, int GridHeight)
{
	RECT r, rCursor;
	POINT start, size = DiagramSize;

	start.x = 0;
	start.y = StartGridY;

	size.y = GridHeight;

	r = getRECT(start, size);
	r.left  -= Grid1x1.x/2;
	r.right += Grid1x1.x/2;

	// Desenha os cursores
	if(SelectedState != SELECTED_NONE) {
		rCursor.left   = r.left + (r.right - r.left)/2 - 7;
		rCursor.right  = r.left + (r.right - r.left)/2 + 7;
		rCursor.top    = r.top - 7;
		rCursor.bottom = r.top + 7;
		DrawRectangle(rCursor, HighlightColoursBrush.breakpoint, true , 0, 0, 45);

		rCursor.top    = r.bottom - 7;
		rCursor.bottom = r.bottom + 7;
		DrawRectangle(rCursor, HighlightColoursBrush.breakpoint, true , 0, 0, 45);
	}

	// Desenha a barra
	DrawRectangle(r, HighlightColoursBrush.simBusRight);

	// Desenha as linhas de seleção superior / inferior
	if(SelectedState != SELECTED_NONE) {
		POINT start = { r.left, r.top }, end = { r.right, r.top };
		DrawLine(start, end, HighlightColoursBrush.breakpoint);

		start.y = r.bottom;
		end.y   = r.bottom;
		DrawLine(start, end, HighlightColoursBrush.breakpoint);
	}

	return r;
}

RECT LadderGUI::DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, POINT GridSize, string ElemName, bool ShowExpand)
{
	bool isMouseOver;
	RECT r = getRECT(StartTopLeft, GridSize);

	isMouseOver = MouseOver(r);

	// Desenha a linha de barramento do elemento, ou seja, 1 para baixo no grid e 1/2 grid para cada lado
	POINT StartPoint = { r.left  - Grid1x1.x / 2, r.top + HeaderHeight + Grid1x1.y };
	POINT EndPoint   = { r.right + Grid1x1.x / 2, r.top + HeaderHeight + Grid1x1.y };
	DrawLine(StartPoint, EndPoint, HighlightColoursBrush.bus);

	// Desenha os losangos que conectam a caixa do elemento ao barramento
//	RECT rLosangoLeft  = { r.left  - 5, r.top + HeaderHeight + Grid1x1.y - 5, r.left  + 5, r.top + HeaderHeight + Grid1x1.y + 5 };
//	RECT rLosangoRight = { r.right - 5, r.top + HeaderHeight + Grid1x1.y - 5, r.right + 5, r.top + HeaderHeight + Grid1x1.y + 5 };
//	DrawRectangle(rLosangoLeft , HighlightColoursBrush.bus, true , 0, 0, 45);
//	DrawRectangle(rLosangoRight, HighlightColoursBrush.bus, true , 0, 0, 45);

	// Desenha o indicador de selecao
	RECT rSel, rCursor = { - 7, - 7, + 7, + 7 };

	// Selecao Acima
	if(SelectedState == SELECTED_ABOVE) {
		rCursor.left   += r.left + (r.right - r.left)/2;
		rCursor.right  += r.left + (r.right - r.left)/2;
		rCursor.top    += r.top + (isMouseOver ? 0 : HeaderHeight);
		rCursor.bottom += r.top + (isMouseOver ? 0 : HeaderHeight);
	} else {
		rSel.top    = r.top                - Grid1x1.y/2;
		rSel.bottom = r.top + HeaderHeight + Grid1x1.y/2;
		rSel.left   = r.left;
		rSel.right  = r.right;
		AddCommand(elem, rSel, CmdSelectAbove, false);
	}

	// Selecao Abaixo
	if(SelectedState == SELECTED_BELOW) {
		rCursor.left   += r.left + (r.right - r.left)/2;
		rCursor.right  += r.left + (r.right - r.left)/2;
		rCursor.top    += r.bottom;
		rCursor.bottom += r.bottom;
	} else {
		rSel.top    = r.bottom - Grid1x1.y/2;
		rSel.bottom = r.bottom + Grid1x1.y/2;
		rSel.left   = r.left;
		rSel.right  = r.right;
		AddCommand(elem, rSel, CmdSelectBelow, false);
	}

	// Selecao Esquerda
	if(SelectedState == SELECTED_LEFT) {
		rCursor.left   += r.left;
		rCursor.right  += r.left;
		rCursor.top    += r.top + HeaderHeight + Grid1x1.y;
		rCursor.bottom += r.top + HeaderHeight + Grid1x1.y;
//		rCursor.top    += r.top + (r.bottom - r.top)/2;
//		rCursor.bottom += r.top + (r.bottom - r.top)/2;
	} else {
		rSel.left   = r.left - Grid1x1.x/2;
		rSel.right  = r.left + Grid1x1.x/2;
		rSel.top    = r.top;
		rSel.bottom = r.bottom;
		AddCommand(elem, rSel, CmdSelectLeft, false);
	}

	// Selecao Direita
	if(SelectedState == SELECTED_RIGHT) {
		rCursor.left   += r.right;
		rCursor.right  += r.right;
		rCursor.top    += r.top + HeaderHeight + Grid1x1.y;
		rCursor.bottom += r.top + HeaderHeight + Grid1x1.y;
//		rCursor.top    += r.top + (r.bottom - r.top)/2;
//		rCursor.bottom += r.top + (r.bottom - r.top)/2;
	} else {
		rSel.left   = r.right - Grid1x1.x/2;
		rSel.right  = r.right + Grid1x1.x/2;
		rSel.top    = r.top;
		rSel.bottom = r.bottom;
		AddCommand(elem, rSel, CmdSelectRight, false);
	}

	if(SelectedState != SELECTED_NONE && isCursorVisible) {
		DrawRectangle(rCursor, HighlightColoursBrush.breakpoint, true , 0, 0, 45);
	}

	// Desenha a caixa em si. Se mouse estiver sobre o elemento, desenha com borda.
	if(isMouseOver) {
		MouseOverElement = elem;

		DrawRectangle(r, HighlightColoursBrush.simBusRight, true , 5, 5);

		RECT rHeader = r;
		rHeader.bottom = rHeader.top + HeaderHeight;
		DrawRectangle(rHeader, HighlightColoursBrush.simOff, true);

		if(SelectedState != SELECTED_NONE) {
			DrawRectangle(r, HighlightColoursBrush.breakpoint, false, 5, 5);
		} else {
			DrawRectangle(r, HighlightColoursBrush.simOff, false, 5, 5);
		}

		r.left   += 3;
		r.top    += 3;
		r.right  -= 3;
		r.bottom -= 3;

		if(ShowExpand) {
			RECT rExpand;
			rExpand.left   = r.right - 15;
			rExpand.right  = r.right;
			rExpand.top    = r.top;
			rExpand.bottom = r.top + HeaderHeight - 2;

			// Desenha o icone de expandir / retrair
			bool expanded = IsExpanded(MouseOverElement);
			StartPoint.x = r.right - 12;
			StartPoint.y = r.top + (expanded ? 10 : 5);
			EndPoint.x = StartPoint.x + 5;
			EndPoint.y = StartPoint.y + (expanded ? -5 : +5);
			DrawLine(StartPoint, EndPoint, HighlightColoursBrush.name);

			StartPoint.x += 10;
			DrawLine(StartPoint, EndPoint, HighlightColoursBrush.name);

			StartPoint.y -= 5;
			EndPoint.y -= 5;
			DrawLine(StartPoint, EndPoint, HighlightColoursBrush.name);

			StartPoint.x -= 10;
			DrawLine(StartPoint, EndPoint, HighlightColoursBrush.name);

			// Adiciona o comando do botao de expandir / retrair
			AddCommand(elem, rExpand, ::CmdExpand, false);
		}

		// Desenha o texto do nome do controle
		DrawText(ElemName.c_str(), r, 0, HighlightColoursBrush.name);

		r.top = rHeader.bottom;
	} else {
		r.top += HeaderHeight;
		DrawRectangle(r, HighlightColoursBrush.simBusRight, true , 5, 5);

		if(SelectedState != SELECTED_NONE) {
			DrawRectangle(r, HighlightColoursBrush.breakpoint , false, 5, 5);

			r.left   += 3;
			r.right  -= 3;
		}
	}

	AddCommand(elem, r, CmdShowDialog);

	return r;
}

void LadderGUI::DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft)
{
	POINT size = { 1, 1 };

	DrawElementBox(elem, SelectedState, StartTopLeft, size);
}

void LadderGUI::DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, pair<string, string> txt)
{
	RECT r;
	POINT size;
	int txt_size = max(txt.first.size(), txt.second.size()) * FONT_WIDTH;

	size.x = 1 + (txt_size / Grid1x1.x);
	size.y = 1;

	r = DrawElementBox(elem, SelectedState, StartTopLeft, size);
	r.top += 2;
	r.left += (r.right - r.left - txt_size)/2;
	DrawText(txt.first.c_str(), r, 0, HighlightColoursBrush.name);

	r.top += FONT_HEIGHT;
	DrawText(txt.second.c_str(), r, 0, HighlightColoursBrush.name);
}

pair<POINT, POINT> LadderGUI::DrawWire(POINT StartPoint, POINT EndPoint, bool StartHasConnectionDot)
{
	POINT Size = { EndPoint.x - StartPoint.x, EndPoint.y - StartPoint.y };
	RECT r = getRECT(StartPoint, Size);

	StartPoint.x = r.left;
	EndPoint.x   = r.right;
	StartPoint.y = r.top    + Grid1x1.y + HeaderHeight;
	EndPoint.y   = r.bottom + Grid1x1.y + HeaderHeight;

	if(StartPoint.y != EndPoint.y) { // Linha vertical
		StartPoint.x -= Grid1x1.x/2; // Desenha a linha vertical centralizada ao grid
		EndPoint.x    = StartPoint.x;
	} else {
		StartPoint.x += Grid1x1.x/2;
		EndPoint  .x += Grid1x1.x/2;
	}

	// Desenha a linha (Horizontal ou Vertical)
	DrawLine(StartPoint, EndPoint, HighlightColoursBrush.bus);

	// Se solicitado, desenha o ponto de conexao
	if(StartHasConnectionDot) {
		RECT ConnectionDot;

		ConnectionDot.left   = StartPoint.x - 5;
		ConnectionDot.right  = StartPoint.x + 5;
		ConnectionDot.top    = StartPoint.y - 5;
		ConnectionDot.bottom = StartPoint.y + 5;

		vectorConnectionDots.push_back(ConnectionDot);
	}

	return pair<POINT, POINT>(StartPoint, EndPoint);
}

void LadderGUI::MouseClick(int x, int y, bool isDown, bool isDouble)
{
	POINT p = { x + ScrollXOffset, y + ScrollYOffset*Grid1x1.y };
	multimap<LadderElem*, pair<RECT, void(*)(LadderElem*)> >::iterator it;
	pair<multimap<LadderElem*, pair<RECT, void(*)(LadderElem*)> >::iterator,
		multimap<LadderElem*, pair<RECT, void(*)(LadderElem*)> >::iterator> range;

	if(isDouble || isDown) {
		multimap<LadderElem*, pair<RECT, void(*)(LadderElem*)> > *Commands;
		Commands = isDouble ? &CommandsDoubleClick : &CommandsSingleClick;

		if(Commands->size() <= 0) return; // Lista de comandos vazia! retorna...

		if(MouseOverElement != nullptr && Commands->count(MouseOverElement) > 0) {
			range = Commands->equal_range(MouseOverElement);
		} else {
			range.first  = Commands->begin();
			range.second = Commands->end  ();
		}

		it = range.second;
		do {
			it--;
			if(it->second.second != nullptr && PointInsideRegion(p, it->second.first)) {
				it->second.second(it->first);
				break;
			}
		} while(it != range.first);
	}
}

void LadderGUI::AddCommand(LadderElem *elem, RECT region, void (*fnc)(LadderElem*), bool isDoubleClick)
{
	multimap<LadderElem*, pair<RECT, void(*)(LadderElem*)> > *Commands;
	Commands = isDoubleClick ? &CommandsDoubleClick : &CommandsSingleClick;

	Commands->insert(pair<LadderElem*, pair<RECT, void(*)(LadderElem*)> >(elem, pair<RECT, void(*)(LadderElem*)>(region, fnc)));
}

/*** Fim da definicao da classe LaderGUI ***/

// Funcao para fazer o trabalho extra de um elemento de fim de linha:
// Calcular a posicao a direita e desenhar a ligacao entre este elemento e o anterior
void DoEOL(POINT &Start, POINT &DiagramSize, POINT &MySize)
{
	POINT OldStart = Start;

	OldStart.x--;
	Start.x = DiagramSize.x - MySize.x;

	gui.DrawWire(Start, OldStart, false);
}

// Funcao para desenhar um elemento na tela, apenas para testes...
void DrawGenericGUI(LadderElem *elem, void *data)
{
	POINT start, size;
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	start = ddg->start;
	size  = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 3;
		ddg->size.y = 3;
	} else {
		ddg->size.x = 1;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return;

	if(elem->IsEOL()) {
		DoEOL(start, size, ddg->size);
	}

	int SelectedState = ddg->context->SelectedElem == elem ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(elem, SelectedState, start, ddg->size, _("???"));
	ddg->region = r;
}

// Classe LadderElemPlaceHolder
bool LadderElemPlaceHolder::ShowDialog(LadderContext context) { return false; }

void LadderElemPlaceHolder::DrawGUI(void *data)
{
	RECT rCursor;
	int SelectedState;
	POINT start, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;
	pair<POINT, POINT> WireCoords;

	ddg->size.x = 1;
	ddg->size.y = 2;

	if(ddg->DontDraw) return;

	SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;

	start = ddg->start;
	end.x = start.x - 1;
	end.y = start.y;

	// Desenha o PlaceHolder
	WireCoords = gui.DrawWire(start, end, true);

	// Selecao Direita
	rCursor.left   = WireCoords.first.x - 10;
	rCursor.right  = WireCoords.first.x + 10;
	rCursor.top    = WireCoords.first.y - 10;
	rCursor.bottom = WireCoords.first.y + 10;

	if(SelectedState == SELECTED_NONE) {
		gui.AddCommand(this, rCursor, CmdSelectRight, false);
	} else if(isCursorVisible) {
		gui.DrawEllipse(rCursor, 1);
	}
}

// Classe LadderElemComment
bool LadderElemComment::ShowDialog(LadderContext context)
{
	char txt[MAX_COMMENT_LEN];

	strcpy(txt, prop.str.c_str());
	bool changed = ShowCommentDialog(txt);

	if(changed) {
		LadderElemCommentProp *data = (LadderElemCommentProp *)getProperties();

		data->str = txt;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemComment::DrawGUI(void *data)
{
	pair<string, string> txt = DrawTXT();
	POINT GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;
	int size = 1 + (max(txt.first.size(), txt.second.size()) * FONT_WIDTH) / GridSize.x;

	ddg->size.x = max(size, ddg->size.x);
	ddg->size.y = txt.second.size() ? 2 : 1;

	if(ddg->DontDraw) return;

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = ddg->region = gui.DrawElementBar(this, SelectedState, ddg->start.y, ddg->size.y);
	gui.AddCommand(this, r, CmdSelectBelow, false);

	RECT rDialog = r;
	rDialog.left   += GridSize.x;
	rDialog.right  -= GridSize.x;
	rDialog.top    += 10;
	rDialog.bottom -= 10;
	gui.AddCommand(this, rDialog, CmdShowDialog);

	r.left += GridSize.x;
	r.top  += (GridSize.y - FONT_HEIGHT)/2;

	gui.DrawText(txt.first .c_str(), r, 0, 1);

	r.top  += GridSize.y;
	gui.DrawText(txt.second.c_str(), r, 0, 1);
}

// Classe LadderElemContact
void ContactCmdToggleNegated(LadderElem *elem)
{
	LadderElemContact *contact = dynamic_cast<LadderElemContact *>(elem);
	LadderElemContactProp *data = (LadderElemContactProp *)contact->getProperties();

	ladder->CheckpointBegin("Inverter Contato");

	data->negated = !data->negated;

	contact->setProperties(ladder->getContext(), data);

	ladder->CheckpointEnd();
	ladder->updateContext();
}

bool LadderElemContact::ShowDialog(LadderContext context)
{
	bool NewNegated         = prop.negated;
	unsigned long NewIdName = prop.idName.first;

	bool changed = ShowContactsDialog(&NewNegated, &NewIdName);

	if(changed) {
		LadderElemContactProp *data = (LadderElemContactProp *)getProperties();

		data->negated      = NewNegated;
		data->idName.first = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemContact::DrawGUI(void *data)
{
	RECT rCmd;
	POINT start, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	if(ddg->expanded) {
		ddg->size.x = 3;
		ddg->size.y = 5;
	} else {
		ddg->size.x = 2;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return;

	string sname = ladder->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	mapDetails detailsIO = ladder->getDetailsIO(prop.idName.first);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("CONTATO"));
	ddg->region = r;

	// Escreve o nome do contato
	RECT rText = r;
	rText.left += 5;
	rText.top  += 5;
	gui.DrawText(name, rText, 0, 1);

	// Desenha o contato
	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 5;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, 2);

	start.x  = end.x;
	start.y -= 10;
	end.y   += 10;
	gui.DrawLine(start, end, 2);

	start.x += 10;
	end.x   += 10;
	gui.DrawLine(start, end, 2);

	rCmd.left    = start.x - 10;
	rCmd.right   = start.x;
	rCmd.top     = start.y;
	rCmd.bottom  = end.y;
	gui.AddCommand(this, rCmd, ContactCmdToggleNegated);

	start.y = r.top + GridSize.y;
	end.x   = r.right;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, 2);

	// Desenha o sinal de fechado, se ativado
	if(prop.negated) {
		start.y -= 10;
		end.x    = start.x - 10;
		end.y   += 10;
		gui.DrawLine(start, end, 2);
	}
}

// Classe LadderElemCoil
bool LadderElemCoil::ShowDialog(LadderContext context)
{
	bool          NewNegated   = prop.negated;
	bool          NewSetOnly   = prop.setOnly;
	bool          NewResetOnly = prop.resetOnly;
	unsigned long NewIdName    = prop.idName.first;

	bool changed = ShowCoilDialog(&NewNegated, &NewSetOnly, &NewResetOnly, &NewIdName);

	if(changed) {
		LadderElemCoilProp *data = (LadderElemCoilProp *)getProperties();

		data->setOnly      = NewSetOnly;
		data->resetOnly    = NewResetOnly;
		data->negated      = NewNegated;
		data->idName.first = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemCoil::DrawGUI(void *data)
{
	POINT start, size, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	start = ddg->start;
	size  = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 2;
		ddg->size.y = 5;
	} else {
		ddg->size.x = 2;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return;

	string sname = ladder->getNameIO(prop.idName.first);
	const char *name = sname.c_str();

	DoEOL(start, size, ddg->size);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, start, ddg->size, _("BOBINA"));
	ddg->region = r;

	// Escreve o nome da bobina
	RECT rText = r;
	rText.left += 5;
	rText.top  += 5;
	gui.DrawText(name, rText, 0, 1);

	// Desenha a bobina
	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 9;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, 2);

	start.x  = end.x + 10;
	gui.DrawEllipse(start, 10, 10, 2, false);

	start.x += 10;
	end.x    = r.right;
	gui.DrawLine(start, end, 2);

	// Desenha o sinal de fechado, se ativado
	if(prop.negated) {
		start.x -= 3;
		start.y -= 7;
		end.x    = start.x - 14;
		end.y   += 7;
		gui.DrawLine(start, end, 2);
	}
}

// Classe LadderElemTimer
bool LadderElemTimer::ShowDialog(LadderContext context)
{
	int NewDelay = prop.delay;
	unsigned long NewIdName = prop.idName.first;

	bool changed = ShowTimerDialog(getWhich(), &NewDelay, &NewIdName);

	if(changed) {
		LadderElemTimerProp *data = (LadderElemTimerProp *)getProperties();

		data->delay        = NewDelay;
		data->idName.first = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemTimer::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemRTC
bool LadderElemRTC::ShowDialog(LadderContext context)
{
	int NewMode           = prop.mode;
	unsigned char NewWday = prop.wday;
	struct tm NewStart    = prop.start;
	struct tm NewEnd      = prop.end;

	bool changed = ShowRTCDialog(&NewMode, &NewWday, &NewStart, &NewEnd);

	if(changed) {
		LadderElemRTCProp *data = (LadderElemRTCProp *)getProperties();

		data->mode  = NewMode;
		data->wday  = NewWday;
		data->start = NewStart;
		data->end   = NewEnd;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemRTC::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemCounter
bool LadderElemCounter::ShowDialog(LadderContext context)
{
	int NewMax = prop.max;
	unsigned long NewIdName = prop.idName.first;

	bool changed = ShowCounterDialog(getWhich(), &NewMax, &NewIdName);

	if(changed) {
		LadderElemCounterProp *data = (LadderElemCounterProp *)getProperties();

		data->max  = NewMax;
		data->idName.first = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemCounter::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemReset
bool LadderElemReset::ShowDialog(LadderContext context)
{
	unsigned long NewIdName = prop.idName.first;

	bool changed = ShowResetDialog(this, &NewIdName);

	if(changed) {
		LadderElemResetProp *data = (LadderElemResetProp *)getProperties();

		data->idName.first = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemReset::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemOneShot
bool LadderElemOneShot::ShowDialog(LadderContext context) { return false; }

void LadderElemOneShot::DrawGUI(void *data)
{
	bool isOSR = getWhich() == ELEM_ONE_SHOT_RISING;

	POINT start, size, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	start = ddg->start;
	size  = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 3;
		ddg->size.y = 2;
	} else {
		ddg->size.x = 1;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return;

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, start, ddg->size, ddg->expanded ? (isOSR ? _("BORDA DE SUBIDA") : _("BORDA DE DESCIDA")) : _("BORDA"), true);
	ddg->region = r;

	//                                                   _
	// Faz o desenho do OneShot, algo parecido a isso: _| |_
	// Se for OSR, desenha seta na borda de subida. Se for OSF, na de descida...

	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 10;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, 2);

	start.x  = end.x;
	start.y -= 20;
	gui.DrawLine(start, end, 2);

	if(isOSR) {
		POINT StartArrow = start, EndArrow = end;

		StartArrow.y += 5;
		EndArrow.x   -= 5;
		EndArrow.y    = StartArrow.y + 10;
		gui.DrawLine(StartArrow, EndArrow, 2);

		EndArrow.x   += 10;
		gui.DrawLine(StartArrow, EndArrow, 2);
	}

	end.x += 20;
	end.y  = start.y;
	gui.DrawLine(start, end, 2);

	start.x  = end.x;
	start.y += 20;
	gui.DrawLine(start, end, 2);

	if(!isOSR) {
		POINT StartArrow = start, EndArrow = end;

		StartArrow.y -= 5;
		EndArrow.x   -= 5;
		EndArrow.y    = StartArrow.y - 10;
		gui.DrawLine(StartArrow, EndArrow, 2);

		EndArrow.x   += 10;
		gui.DrawLine(StartArrow, EndArrow, 2);
	}

	end.x = r.right;
	end.y = start.y;
	gui.DrawLine(start, end, 2);
}

// Classe LadderElemCmp
bool LadderElemCmp::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdOp1 = prop.idOp1;
	pair<unsigned long, int> NewIdOp2 = prop.idOp2;

	bool changed = ShowCmpDialog(getWhich(), &NewIdOp1, &NewIdOp2);

	if(changed) {
		LadderElemCmpProp *data = (LadderElemCmpProp *)getProperties();

		data->idOp1 = NewIdOp1;
		data->idOp2 = NewIdOp2;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemCmp::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemMath
bool LadderElemMath::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdOp1  = prop.idOp1;
	pair<unsigned long, int> NewIdOp2  = prop.idOp2;
	pair<unsigned long, int> NewIdDest = prop.idDest;

	bool changed = ShowMathDialog(getWhich(), &NewIdDest, &NewIdOp1, &NewIdOp2);

	if(changed) {
		LadderElemMathProp *data = (LadderElemMathProp *)getProperties();

		data->idDest = NewIdDest;
		data->idOp1  = NewIdOp1;
		data->idOp2  = NewIdOp2;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemMath::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemSqrt
bool LadderElemSqrt::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdDest = prop.idDest;
	pair<unsigned long, int> NewIdSrc  = prop.idSrc;

	bool changed = ShowSqrtDialog(&NewIdDest, &NewIdSrc);

	if(changed) {
		LadderElemSqrtProp *data = (LadderElemSqrtProp *)getProperties();

		data->idDest = NewIdDest;
		data->idSrc  = NewIdSrc;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemSqrt::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemRand
bool LadderElemRand::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdVar = prop.idVar;
	pair<unsigned long, int> NewIdMin = prop.idMin;
	pair<unsigned long, int> NewIdMax = prop.idMax;

	bool changed = ShowRandDialog(&NewIdVar, &NewIdMin, &NewIdMax);

	if(changed) {
		LadderElemRandProp *data = (LadderElemRandProp *)getProperties();

		data->idVar = NewIdVar;
		data->idMin = NewIdMin;
		data->idMax = NewIdMax;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemRand::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemAbs
bool LadderElemAbs::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdDest = prop.idDest;
	pair<unsigned long, int> NewIdSrc  = prop.idSrc;

	bool changed = ShowAbsDialog(&NewIdDest, &NewIdSrc);

	if(changed) {
		LadderElemAbsProp *data = (LadderElemAbsProp *)getProperties();

		data->idDest = NewIdDest;
		data->idSrc  = NewIdSrc;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemAbs::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemMove
bool LadderElemMove::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdDest = prop.idDest;
	pair<unsigned long, int> NewIdSrc  = prop.idSrc;

	bool changed = ShowMathDialog(getWhich(), &NewIdDest, &NewIdSrc, nullptr);

	if(changed) {
		LadderElemMoveProp *data = (LadderElemMoveProp *)getProperties();

		data->idDest = NewIdDest;
		data->idSrc  = NewIdSrc;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemMove::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemOpenShort
bool LadderElemOpenShort::ShowDialog(LadderContext context) { return false; }

void LadderElemOpenShort::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemSetBit
bool LadderElemSetBit::ShowDialog(LadderContext context)
{
	bool NewSet = prop.set;
	int  NewBit = prop.bit;
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowSetBitDialog(&NewIdName, &NewSet, &NewBit);

	if(changed) {
		LadderElemSetBitProp *data = (LadderElemSetBitProp *)getProperties();

		data->idName = NewIdName;
		data->set    = NewSet;
		data->bit    = NewBit;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemSetBit::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemCheckBit
bool LadderElemCheckBit::ShowDialog(LadderContext context)
{
	bool NewSet = prop.set;
	int  NewBit = prop.bit;
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowCheckBitDialog(&NewIdName, &NewSet, &NewBit);

	if(changed) {
		LadderElemCheckBitProp *data = (LadderElemCheckBitProp *)getProperties();

		data->idName = NewIdName;
		data->set    = NewSet;
		data->bit    = NewBit;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemCheckBit::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemReadAdc
bool LadderElemReadAdc::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowReadAdcDialog(&NewIdName);

	if(changed) {
		LadderElemReadAdcProp *data = (LadderElemReadAdcProp *)getProperties();

		data->idName = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemReadAdc::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemSetDa
bool LadderElemSetDa::ShowDialog(LadderContext context)
{
	int NewMode = prop.mode;
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowSetDADialog(&NewIdName, &NewMode);

	if(changed) {
		LadderElemSetDaProp *data = (LadderElemSetDaProp *)getProperties();

		data->idName = NewIdName;
		data->mode   = NewMode;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemSetDa::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemReadEnc
bool LadderElemReadEnc::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowReadEncDialog(&NewIdName);

	if(changed) {
		LadderElemReadEncProp *data = (LadderElemReadEncProp *)getProperties();

		data->idName = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemReadEnc::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemResetEnc
bool LadderElemResetEnc::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowResetEncDialog(&NewIdName);

	if(changed) {
		LadderElemResetEncProp *data = (LadderElemResetEncProp *)getProperties();

		data->idName = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemResetEnc::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemMultisetDA
bool LadderElemMultisetDA::ShowDialog(LadderContext context)
{
	LadderElemMultisetDAProp dialogData = prop;
	bool changed = ShowMultisetDADialog(&dialogData);

	if(changed) {
		LadderElemMultisetDAProp *data = (LadderElemMultisetDAProp *)getProperties();

		*data = dialogData;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemMultisetDA::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemUSS
bool LadderElemUSS::ShowDialog(LadderContext context)
{
	int NewId                          = prop.id;
	int NewIndex                       = prop.index;                                                                                                                                                                                                                                                                                                                                                                                         
	int NewParameter                   = prop.parameter;
	int NewParameterSet                = prop.parameter_set;
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowUSSDialog(getWhich(), &NewIdName, &NewId, &NewParameter, &NewParameterSet, &NewIndex);

	if(changed) {
		LadderElemUSSProp *data = (LadderElemUSSProp *)getProperties();

		data->idName        = NewIdName;
		data->id            = NewId;
		data->parameter     = NewParameter;
		data->parameter_set = NewParameterSet;
		data->index         = NewIndex;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemUSS::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemModBUS
bool LadderElemModBUS::ShowDialog(LadderContext context)
{
	int  NewElem         = prop.elem;
	int  NewAddress      = prop.address;
	bool NewInt32        = prop.int32;
	bool NewRetransmitir = prop.retransmitir;
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowModbusDialog(getWhich() == ELEM_WRITE_MODBUS ? 1 : 0,
					&NewIdName, &NewElem, &NewAddress, &NewInt32, &NewRetransmitir);

	if(changed) {
		LadderElemModBUSProp *data = (LadderElemModBUSProp *)getProperties();

		data->idName       = NewIdName;
		data->elem         = NewElem;
		data->address      = NewAddress;
		data->int32        = NewInt32;
		data->retransmitir = NewRetransmitir;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemModBUS::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemSetPWM
bool LadderElemSetPWM::ShowDialog(LadderContext context)
{
	int  NewTargetFreq = prop.targetFreq;
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowSetPwmDialog(&NewIdName, &NewTargetFreq);

	if(changed) {
		LadderElemSetPWMProp *data = (LadderElemSetPWMProp *)getProperties();

		data->idName       = NewIdName;
		data->targetFreq = NewTargetFreq;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemSetPWM::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemUART
bool LadderElemUART::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdName = prop.idName;

	bool changed = ShowUartDialog(getWhich(), &NewIdName);

	if(changed) {
		LadderElemUARTProp *data = (LadderElemUARTProp *)getProperties();

		data->idName = NewIdName;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemUART::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemMasterRelay
bool LadderElemMasterRelay::ShowDialog(LadderContext context) { return false; }

void LadderElemMasterRelay::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemShiftRegister
bool LadderElemShiftRegister::ShowDialog(LadderContext context)
{
	int  NewStages = prop.stages;
	char NewName[MAX_NAME_LEN];

	strcpy(NewName, prop.nameReg.c_str());

	bool changed = ShowShiftRegisterDialog(NewName, &NewStages);

	if(changed && ladder->IsValidNameAndType(prop.vectorIdRegs[0].first, NewName, eType_General, _("Nome"), VALIDATE_IS_VAR, 0,   0)) {
			LadderElemShiftRegisterProp *data = (LadderElemShiftRegisterProp *)getProperties();

			data->stages  = NewStages;
			data->nameReg = NewName;

			setProperties(context, data);
	}

	return changed;
}

void LadderElemShiftRegister::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemLUT
bool LadderElemLUT::ShowDialog(LadderContext context)
{
	LadderElemLUTProp Dialogdata = prop;

	bool changed = ShowLookUpTableDialog(&Dialogdata);

	if(changed) {
		LadderElemLUTProp *data = (LadderElemLUTProp *)getProperties();

		*data = Dialogdata;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemLUT::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemPiecewise
bool LadderElemPiecewise::ShowDialog(LadderContext context)
{
	LadderElemPiecewiseProp Dialogdata = prop;
	bool changed = ShowPiecewiseLinearDialog(&Dialogdata);

	if(changed) {
		LadderElemPiecewiseProp *data = (LadderElemPiecewiseProp *)getProperties();

		*data = Dialogdata;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemPiecewise::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemFmtString
bool LadderElemFmtString::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdVar = prop.idVar;

	char NewTxt[MAX_NAME_LEN];
	strcpy(NewTxt, prop.txt.c_str());

	bool changed = ShowFormattedStringDialog(getWhich() == ELEM_WRITE_FORMATTED_STRING ? 1 : 0, &NewIdVar, NewTxt);

	if(changed) {
		LadderElemFmtStringProp *data = (LadderElemFmtStringProp *)getProperties();

		data->idVar = NewIdVar;
		data->txt = NewTxt;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemFmtString::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemYaskawa
bool LadderElemYaskawa::ShowDialog(LadderContext context)
{
	int NewId = prop.id;
	pair<unsigned long, int> NewIdVar = prop.idVar;

	char NewTxt[MAX_NAME_LEN];
	strcpy(NewTxt, prop.txt.c_str());

	bool changed = ShowServoYaskawaDialog(getWhich() == ELEM_WRITE_SERVO_YASKAWA ? 1 : 0, &NewId, &NewIdVar, NewTxt);

	if(changed) {
		LadderElemYaskawaProp *data = (LadderElemYaskawaProp *)getProperties();

		data->id    = NewId;
		data->idVar = NewIdVar;
		data->txt   = NewTxt;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemYaskawa::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemPersist
bool LadderElemPersist::ShowDialog(LadderContext context)
{
	pair<unsigned long, int> NewIdVar = prop.idVar;
	bool changed = ShowPersistDialog(&NewIdVar);

	if(changed) {
		LadderElemPersistProp *data = (LadderElemPersistProp *)getProperties();

		data->idVar = NewIdVar;

		setProperties(context, data);
	}

	return changed;
}

void LadderElemPersist::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

// Classe LadderElemX
bool LadderElemX::ShowDialog(LadderContext context) { return false; }

void LadderElemX::DrawGUI(void *data)
{
	DrawGenericGUI(this, data);
}

//Classe LadderCircuit
void LadderCircuit::DrawGUI(void *data)
{
	int max = 0;
	bool FirstIsParallelStart = false;
	POINT StartWire, EndWire, PreviousWire;
	bool isFirstElement = true, HasEOL = false;
	vector<Subckt>::iterator it;
	vector<POINT> EndPoints;

	RECT RegionZero = { 0, 0, 0, 0 };

	tDataDrawGUI ElemDDG, *ddg = (tDataDrawGUI*)data;

	ElemDDG.start    = ddg->start;
	ElemDDG.DontDraw = ddg->DontDraw;

	for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
		ElemDDG.size           = ddg->size;
		ElemDDG.context        = ddg->context;
		ElemDDG.region         = RegionZero;
		ElemDDG.regionSelected = RegionZero;

		if(it->elem != nullptr) {
			HasEOL |= it->elem->IsEOL();
			ElemDDG.expanded = gui.IsExpanded(it->elem);
			it->elem->DrawGUI(&ElemDDG);

			if(it->elem == ddg->context->SelectedElem) {
				ddg->regionSelected = gui.getRECT(ElemDDG.start, ElemDDG.size);
			}
		} else {
			HasEOL |= it->subckt->HasEOL();
			it->subckt->DrawGUI(&ElemDDG);
			if(!IsRegionZero(ElemDDG.regionSelected)) {
				ddg->regionSelected = ElemDDG.regionSelected;
			}
		}

		if(isSeries) {
			ElemDDG.start.x += ElemDDG.size.x + 1;
			if(ElemDDG.size.y > max) {
				max = ElemDDG.size.y;
			}
		} else {
			if(ddg->DontDraw == false) {
				bool isParallelStart = ddg->context->ParallelStart != nullptr &&
					it->elem == ddg->context->ParallelStart;

				// Salva o ponto final do elemento atual para poder fazer a ligacao no final
				if(isFirstElement || !isParallelStart) {
					if(isParallelStart) {
						FirstIsParallelStart = isFirstElement;
					}
					POINT xy = { ElemDDG.start.x + ElemDDG.size.x, ElemDDG.start.y };
					EndPoints.push_back(xy);
				}

				// Desenha a linha de ligacao
				if(isFirstElement) {
					isFirstElement = false;
					EndWire        = ddg->start;
				} else {
					StartWire = EndWire;
					EndWire   = ElemDDG.start;
					gui.DrawWire(StartWire, EndWire, true);
				}
			}

			// Atualiza as coordenadas para o proximo elemento
			ElemDDG.start.y += ElemDDG.size.y + 1;
			if(ElemDDG.size.x > max) {
				max = ElemDDG.size.x;
			}
		}
	}

	if(isSeries) {
		ddg->size.x = ElemDDG.start.x - ddg->start.x - 1;
		ddg->size.y = max;
	} else {
		if(ddg->DontDraw == false && !HasEOL) {
			vector<POINT>::iterator itPoint;

			isFirstElement = true;

			// Agora desenhamos as linhas de ligacao do final do paralelo
			for(itPoint = EndPoints.begin(); itPoint != EndPoints.end(); itPoint++) {
				// Desenha a linha horizontal
				StartWire = EndWire = *itPoint;
				EndWire.x = ddg->start.x + max;
				if(StartWire.x < EndWire.x && (itPoint != EndPoints.begin() || !FirstIsParallelStart)) {
					gui.DrawWire(StartWire, EndWire, false);
				}

				// Em seguida, a linha vertical
				if(isFirstElement) {
					isFirstElement = false;
				} else {
					EndWire.x++; // Avanca 1 posicao no grid pois a linha vertical se desenha a esquerda
					StartWire.x = EndWire.x;
					StartWire.y = PreviousWire.y;
					gui.DrawWire(StartWire, EndWire, itPoint != (EndPoints.begin() + 1) || !FirstIsParallelStart);
				}

				PreviousWire.y = EndWire.y;
			}
		}

		ddg->size.x = max;
		ddg->size.y = ElemDDG.start.y - ddg->start.y - 1;
	}
}

// Classe LadderDiagram
void LadderDiagram::DrawGUI(void)
{
	static bool InitOK = false;

	if(!InitOK) {
		InitOK = true;
		gui.DrawInit();
	}

	RECT  r;
	POINT xy, Grid1x1 = gui.getGridSize();

	GetCursorPos(&xy);
	gui.setMousePosition(xy);

	gui.SetBackgroundColor(InSimulationMode ? HighlightColours.simBg : HighlightColours.bg);

	char num[20];
	POINT SizeMax = { 0, 0 };

	vector<LadderCircuit>::size_type i;
	tDataDrawGUI RungDDG = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, true, true, &context };

	for(i = 0; i < rungs.size(); i++) {
		rungs[i]->DrawGUI(&RungDDG);

		RungDDG.start.y += RungDDG.size.y + 1;
		if(SizeMax.x < RungDDG.size.x) {
			SizeMax.x = RungDDG.size.x;
		}
	}

	RECT rWindow;
	GetClientRect(DrawWindow, &rWindow);

	SizeMax.y = RungDDG.start.y;
	SizeMax.x = max(SizeMax.x, (rWindow.right)/Grid1x1.x - 3);

	gui.setDiagramSize(SizeMax);
	RefreshScrollbars();

	if(NeedScrollSelectedIntoView) {
		RECT rSelected;
		rSelected.left   = RungDDG.regionSelected.left   - ScrollXOffset;
		rSelected.right  = RungDDG.regionSelected.right  - ScrollXOffset;
		rSelected.top    = RungDDG.regionSelected.top    - ScrollYOffset * Grid1x1.y;
		rSelected.bottom = RungDDG.regionSelected.bottom - ScrollYOffset * Grid1x1.y;

		NeedScrollSelectedIntoView = false;

		// Verifica se precisa movimentar no eixo X
		if(rSelected.left < 0 || rWindow.right < rSelected.right) {
			RECT r = RungDDG.regionSelected;
			int offset = ((rWindow.right - rWindow.left) - (r.right - r.left)) / 2;
			ScrollXOffset = max(0, min(ScrollXOffsetMax, RungDDG.regionSelected.left - offset));
		}

		// Verifica se precisa movimentar no eixo Y
		if(rSelected.top < 0 || rWindow.bottom < rSelected.bottom) {
			RECT r = RungDDG.regionSelected;
			int offset = ((rWindow.bottom - rWindow.top) - (r.bottom - r.top)) / 2;
			ScrollYOffset = max(0, min(ScrollYOffsetMax, (RungDDG.regionSelected.top - offset) / Grid1x1.y));
		}

		RefreshScrollbars();
	}

	gui.DrawStart(ScrollXOffset, ScrollYOffset*Grid1x1.y);

	RungDDG.DontDraw = false;
	RungDDG.start.y = 0;
	for(i = 0; i < rungs.size(); i++) {
		RungDDG.size = SizeMax;
		rungs[i]->DrawGUI(&RungDDG);

		RECT r = gui.getRECT(RungDDG.start, RungDDG.size);
		r.right = r.left;
		r.left  = 5;
		r.top += (r.bottom - r.top - FONT_HEIGHT)/2;
		sprintf(num, "%3d", i + 1);
		gui.DrawText(num, r, 0, 2);

		RungDDG.start.y += RungDDG.size.y + 1;
	}

	POINT start = { 0, 0 };
	r = gui.getRECT(start, SizeMax);
	r.left  -= Grid1x1.x/2;
	r.right += Grid1x1.x/2;

	RECT rBus = { r.left - 5, 0, r.left, r.bottom };
	gui.DrawRectangle(rBus, 2);

	rBus.left  = r.right;
	rBus.right = r.right + 5;
	gui.DrawRectangle(rBus, 2);

	gui.DrawEnd();
}

bool LadderDiagram::IsSelectedVisible(void)
{
	if(context.SelectedElem == nullptr) return false;

	vector<LadderCircuit>::size_type i;
	tDataDrawGUI RungDDG = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, true, true, &context };

	for(i = 0; i < rungs.size(); i++) {
		rungs[i]->DrawGUI(&RungDDG);

		if(!IsRegionZero(RungDDG.regionSelected)) { // Selecionado encontrado.
			RECT rWindow = { 0, 0, 0, 0 };
			GetClientRect(DrawWindow, &rWindow);
			if(IsRegionZero(rWindow)) {
				// Nao foi possivel receber as coordenadas da janela. Talvez ainda nao
				// foi criada ou esta minimizada.
				// De qualquer forma, considere que o objeto esteja visivel...
				return true;
			}

			POINT Grid1x1 = gui.getGridSize();
			POINT TopLeft = { RungDDG.regionSelected.left - ScrollXOffset,
				RungDDG.regionSelected.top - ScrollYOffset*Grid1x1.y };
			POINT BottomRight = { RungDDG.regionSelected.right - ScrollXOffset,
				RungDDG.regionSelected.bottom - ScrollYOffset*Grid1x1.y };

			return PointInsideRegion(TopLeft, rWindow) && PointInsideRegion(BottomRight, rWindow);
		}

		RungDDG.start.y += RungDDG.size.y + 1;
	}

	return false;
}

void LadderDiagram::MouseClick(int x, int y, bool isDown, bool isDouble)
{
	gui.MouseClick(x, y, isDown, isDouble);
}

// Funcao que exibe uma janela de dialogo para o usuario. Dependente de implementacao da interface
eReply LadderDiagram::ShowDialog(bool isMessage, char *title, char *message, ...)
{
	eReply reply = eReply_Yes;
	message = _(message);

	va_list f;
	va_start(f, message);

	if(isMessage) {
		Error(title, message, f);
	} else {
		char buf[1024];
		vsprintf(buf, message, f);
		if(MessageBox(MainWindow, buf, _(title), MB_YESNO | MB_ICONQUESTION) != IDYES) {
			reply = eReply_No;
		}
	}

	return reply;
}

HRESULT UpdateRibbonHeight(void);

//-----------------------------------------------------------------------------
// Paint the ladder logic program to the screen. Also figure out where the
// cursor should go and fill in coordinates for BlinkCursor. Not allowed to
// draw deeper than IoListTop, as we would run in to the I/O listbox.
//-----------------------------------------------------------------------------
void PaintWindow(void)
{
	static bool first = true;

    ok();

	// Update Ribbon Height
	if(!RibbonHeight || first) {
		first = false;
		UpdateRibbonHeight();
	}

	ladder->DrawGUI();

	ok();
}

//-----------------------------------------------------------------------------
// Blink the cursor on the schematic; called by a Windows timer. We XOR
// draw it so just draw the same rectangle every time to show/erase the
// cursor. Cursor may be in one of four places in the selected leaf (top,
// bottom, left, right) but we don't care; just go from the coordinates
// computed when we drew the schematic in the paint procedure.
//-----------------------------------------------------------------------------
void CALLBACK BlinkCursor(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
	isCursorVisible = !isCursorVisible;
	InvalidateRect(DrawWindow, NULL, FALSE);
}

//-----------------------------------------------------------------------------
// Determine the settings of the vertical and (if needed) horizontal
// scrollbars used to scroll our view of the program.
//-----------------------------------------------------------------------------
void SetUpScrollbars(BOOL *horizShown, SCROLLINFO *horiz, SCROLLINFO *vert)
{
	POINT Grid1x1 = gui.getGridSize();
	POINT start = { 0, 0 }, size = gui.getDiagramSize();
	RECT rDiagram = gui.getRECT(start, size);

	rDiagram.right += (3*Grid1x1.x)/2;

	RECT rScreen;
	GetClientRect(DrawWindow, &rScreen);

	if(rDiagram.right <= rScreen.right) {
        *horizShown = FALSE;
        ScrollXOffset = 0;
        ScrollXOffsetMax = 0;
    } else {
        *horizShown = TRUE;
        memset(horiz, 0, sizeof(*horiz));
        horiz->cbSize = sizeof(*horiz);
        horiz->fMask = SIF_DISABLENOSCROLL | SIF_ALL;
        horiz->nMin = 0;
        horiz->nMax = rDiagram.right;
        RECT r;
        GetClientRect(DrawWindow, &r);
        horiz->nPage = r.right;
        horiz->nPos = ScrollXOffset;

        ScrollXOffsetMax = horiz->nMax - horiz->nPage + 1;
        if(ScrollXOffset > ScrollXOffsetMax) ScrollXOffset = ScrollXOffsetMax;
        if(ScrollXOffset < 0) ScrollXOffset = 0;
    }

    vert->cbSize = sizeof(*vert);
    vert->fMask  = SIF_DISABLENOSCROLL | SIF_ALL;
    vert->nMin   = 0;
	vert->nMax   = rDiagram.bottom/Grid1x1.y - 1;
	vert->nPos   = ScrollYOffset;
    vert->nPage  = 1;

    ScrollYOffsetMax = vert->nMax - vert->nPage + 1;

    if(ScrollYOffset > ScrollYOffsetMax) ScrollYOffset = ScrollYOffsetMax;
    if(ScrollYOffset < 0) ScrollYOffset = 0;
}

// Classe mapIO
void mapIO::updateGUI(void)
{
    int i, iocount = ladder->getCountIO();

    ListView_DeleteAllItems(IoList);
    for(i = 0; i < iocount; i++) {
        LVITEM lvi;
        lvi.mask        = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
        lvi.state       = lvi.stateMask = 0;
        lvi.iItem       = i;
        lvi.iSubItem    = 0;
        lvi.pszText     = LPSTR_TEXTCALLBACK;
        lvi.lParam      = i;

        if(ListView_InsertItem(IoList, &lvi) < 0) oops();
    }

	for(i = 0; i < iocount; i++) {
		ListView_SetItemState(IoList, i, 0, LVIS_SELECTED);
	}

	if(selectedIO) {
		ListView_SetItemState (IoList, getIndex(selectedIO, true) - 1, LVIS_SELECTED, LVIS_SELECTED);
		ListView_EnsureVisible(IoList, getIndex(selectedIO, true) - 1, FALSE);
	}
}

void LadderDiagram::ShowIoMapDialog(int item)
{
	IO->ShowIoMapDialog(item);
	updateContext();
}
