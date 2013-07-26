#include "poptools.h"

#include "EngineGUI.h"
#include "EngineRenderD2D.h"

// Variavel que indica se o cursor deve ser exibido. Liga e Desliga a cada 800 ms, conforme timer.
bool isCursorVisible = true;

// Colours with which to do syntax highlighting, configurable
extern SyntaxHighlightingColours HighlightColours;

/*** Estruturas auxiliares ***/

// Estrutura que contem a origem do comando, permitindo que tanto um elemento, um circuito ou o
// proprio diagrama possa cadastrar acoes
typedef struct {
	LadderDiagram *diagram;
	LadderCircuit *circuit;
	LadderElem    *elem;
} tCommandSource;

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

	vector<RECT>                       vectorConnectionDots;
	vector<LadderElem *>               vectorExpandedElements;
	vector< pair<LadderElem *, RECT> > vectorAreaElements;

	int HeaderHeight;

	array<unsigned int, 3>    InterfaceColors;
	SyntaxHighlightingColours HighlightColoursBrush;

	typedef struct {
		RECT r;
		void(*fnc)(tCommandSource, void *);
		void *data;
	} tCommandItem;
	typedef pair<tCommandSource, tCommandItem>  tCommandListItem;
	typedef vector<tCommandListItem>            tCommandsList;

	tCommandsList CommandsSingleClick;
	tCommandsList CommandsDoubleClick;

	void ClearCommandLists(void);

	bool MouseOver (RECT region);

	void PaintScrollAndSplitter(void);

public:
	LadderGUI(void);
	~LadderGUI(void) { ClearCommandLists(); }

	inline RECT getRECT(POINT Start, POINT Size);
	POINT       getGridSize(void) { return Grid1x1; }
	POINT       getCenter(RECT r);

	bool IsExpanded(LadderElem *elem);

	void setMousePosition(POINT xy);

	void  setDiagramSize(POINT size) { DiagramSize = size; }
	POINT getDiagramSize(void)       { return DiagramSize; }

	void DrawInit (void);
	void DrawStart(int OffsetX, int OffsetY);
	void DrawEnd  (void);

	void registerElementArea(LadderElem *elem, POINT start, POINT size);

	void DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft);
	RECT DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, POINT GridSize, string ElemName = "teste", bool ShowExpand = true);
	void DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, pair<string, string> txt);

	RECT DrawElementBar(LadderElem *elem, int SelectedState, int StartGridY, int GridHeight);

	pair<POINT, POINT> DrawWire(POINT StartPoint, POINT EndPoint, bool StartHasConnectionDot);

	void        MouseClick(int x, int y, bool isDown, bool isDouble);
	LadderElem *SearchElement(LadderElem *ref, eMoveCursor moveTo);

	void AddCommand(tCommandSource source, RECT region, void (*fnc)(tCommandSource, void *), void *data, bool isDoubleClick = true);

	void CmdExpand(LadderElem *elem);
};

// Criacao do objeto que desenha a interface
static LadderGUI gui;

// Funcoes usadas nos comandos genericos
void CmdExpand(tCommandSource source, void *data)
{
	gui.CmdExpand(source.elem);
}

void CmdShowDialog(tCommandSource source, void *data)
{
	if(source.elem != nullptr) {
		ladder->CheckpointBegin("Editar Elemento");
		source.elem->ShowDialog(ladder->getContext());
		ladder->CheckpointEnd();
		ladder->updateContext();
	}
}

void CmdSelect(tCommandSource source, int SelectedState)
{
	ladder->SelectElement(source.elem, SelectedState);
}

void CmdSelectLeft(tCommandSource source, void *data)
{
	CmdSelect(source, SELECTED_LEFT);
}

void CmdSelectRight(tCommandSource source, void *data)
{
	CmdSelect(source, SELECTED_RIGHT);
}

void CmdSelectAbove(tCommandSource source, void *data)
{
	CmdSelect(source, SELECTED_ABOVE);
}

void CmdSelectBelow(tCommandSource source, void *data)
{
	CmdSelect(source, SELECTED_BELOW);
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
void LadderGUI::ClearCommandLists(void)
{
	unsigned int i, count = CommandsSingleClick.size();

	for(i = 0; i < count; i++) {
		if(CommandsSingleClick[i].second.data != nullptr) {
			delete CommandsSingleClick[i].second.data;
		}
	}

	count = CommandsDoubleClick.size();
	for(i = 0; i < count; i++) {
		if(CommandsDoubleClick[i].second.data != nullptr) {
			delete CommandsDoubleClick[i].second.data;
		}
	}

	CommandsSingleClick.clear();
	CommandsDoubleClick.clear();
}

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

inline POINT LadderGUI::getCenter(RECT r)
{
	POINT p;

	p.x = r.left + (r.right  - r.left)/2;
	p.y = r.top  + (r.bottom - r.top )/2;

	return p;
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

	ClearCommandLists();

	vectorAreaElements.clear();

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
	POINT start = { 0, 0 }, size = { 1, 1 };
	RECT r = gui.getRECT(start, size);
	r.left  -= Grid1x1.x/2;

	vector<RECT>::iterator it;
	for(it = vectorConnectionDots.begin(); it != vectorConnectionDots.end(); it++) {
		if(it->left > r.left) {
			DrawEllipse(*it, HighlightColoursBrush.breakpoint);
		}
	}

	vectorConnectionDots.clear();

	PaintScrollAndSplitter();

	EndDraw();
}

void LadderGUI::registerElementArea(LadderElem *elem, POINT start, POINT size)
{
	vectorAreaElements.push_back(pair<LadderElem *, RECT>(elem, getRECT(start, size)));
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
	if(SelectedState != SELECTED_NONE && isCursorVisible) {
		rCursor.left   = r.left + (r.right - r.left)/2 - 7;
		rCursor.right  = r.left + (r.right - r.left)/2 + 7;

		if(SelectedState == SELECTED_ABOVE) {
			rCursor.top    = r.top - 7;
			rCursor.bottom = r.top + 7;
			DrawRectangle(rCursor, HighlightColoursBrush.breakpoint, true , 0, 0, 45);
		}

		if(SelectedState == SELECTED_BELOW) {
			rCursor.top    = r.bottom - 7;
			rCursor.bottom = r.bottom + 7;
			DrawRectangle(rCursor, HighlightColoursBrush.breakpoint, true , 0, 0, 45);
		}
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

	// Estrutura com a origem do comando
	tCommandSource source = { nullptr, nullptr, elem };

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
		AddCommand(source, rSel, CmdSelectAbove, nullptr, false);
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
		AddCommand(source, rSel, CmdSelectBelow, nullptr, false);
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
		AddCommand(source, rSel, CmdSelectLeft, nullptr, false);
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
		AddCommand(source, rSel, CmdSelectRight, nullptr, false);
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
			AddCommand(source, rExpand, ::CmdExpand, nullptr, false);
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

	AddCommand(source, r, CmdShowDialog, nullptr);

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
	tCommandsList::iterator it;

	if(isDouble || isDown) {
		tCommandsList *Commands;
		Commands = isDouble ? &CommandsDoubleClick : &CommandsSingleClick;

		if(Commands->size() <= 0) return; // Lista de comandos vazia! retorna...

		it = Commands->end();
		do {
			it--;
			if(it->second.fnc != nullptr && PointInsideRegion(p, it->second.r)) {
				it->second.fnc(it->first, it->second.data);
				break;
			}
		} while(it != Commands->begin());
	}
}

LadderElem *LadderGUI::SearchElement(LadderElem *ref, eMoveCursor moveTo)
{
	LadderElem *elem = nullptr;

	if(ref != nullptr) {
		unsigned int i;
		LONG newxy, minxy = 0;
		POINT refCenter, elemCenter;
		RECT refArea = { 0, 0, 0, 0 }, elemArea;

		// Primeiro buscamos a area do elemento de referencia
		for(i = 0; i < vectorAreaElements.size(); i++) {
			if(vectorAreaElements[i].first == ref) { // Encontrado!
				refArea = vectorAreaElements[i].second;
				break;
			}
		}

		// Aqui criamos ponteiros para a coordenada que devemos comparar para decidir se o elemento deve ser considerado
		// Exemplo: Se buscando para baixo, elementos com top <= ao elemento de referencia devem ser desconsiderados...
		LONG *pMin, *pMax;
		switch(moveTo) {
			default                     :
			case eMoveCursor_RungHome   :
			case eMoveCursor_DiagramHome:
			// As situacoes acima nao deveriam ocorrer, estao aqui apenas por seguranca
			case eMoveCursor_Up         : pMin = &elemArea.top   ; pMax = &refArea .top   ; break;

			case eMoveCursor_RungEnd    :
			case eMoveCursor_DiagramEnd :
			// As situacoes acima nao deveriam ocorrer, estao aqui apenas por seguranca
			case eMoveCursor_Down       : pMin = &refArea .bottom; pMax = &elemArea.bottom; break;

			case eMoveCursor_Left       : pMin = &elemArea.left  ; pMax = &refArea .left  ; break;

			case eMoveCursor_Right      : pMin = &refArea .right ; pMax = &elemArea.right ; break;
		}

		// Agora comecamos o loop entre todos os elementos cadastrados, calculando a diagonal entre seus centros
		for(i = 0; i < vectorAreaElements.size(); i++) {
			if(vectorAreaElements[i].first != ref) { // Elemento nao eh o de referencia, calcula diagonal
				elemArea   = vectorAreaElements[i].second;
				if(*pMin < *pMax) {
					refCenter  = getCenter(refArea);
					elemCenter = getCenter(elemArea);

					LONG x = abs(refCenter.x - elemCenter.x), y = abs(refCenter.y - elemCenter.y);
					if(vectorAreaElements[i].first->IsComment()) {
						if(moveTo == eMoveCursor_Up || moveTo == eMoveCursor_Down) {
							// Comentario ocupa toda a tela, nao calcular a distancia ate seu centro
							x = 0;
						} else {
							// Ao mover lateralmente nao devemos mudar o cursor para o comentario.
							// Se o usuario quisesse ir para o comentario, teria feito o deslocamento
							// para cima ou para baixo...
							continue;
						}
					}
					newxy = x + y;

					if(!minxy || minxy > newxy) {
						minxy = newxy;
						elem  = vectorAreaElements[i].first;
					}
				}
			}
		}
	}

	return elem;
}

void LadderGUI::AddCommand(tCommandSource source, RECT region, void (*fnc)(tCommandSource, void *), void *data, bool isDoubleClick)
{
	tCommandsList *Commands;
	Commands = isDoubleClick ? &CommandsDoubleClick : &CommandsSingleClick;

	tCommandListItem item;
	item.first       = source;
	item.second.r    = region;
	item.second.fnc  = fnc;
	item.second.data = data;

	Commands->push_back(item);
}

/*** Fim da definicao da classe LaderGUI ***/

// Funcao para fazer o trabalho extra de um elemento de fim de linha:
// Calcular a posicao a direita e desenhar a ligacao entre este elemento e o anterior
static void DoEOL(POINT &Start, POINT &DiagramSize, POINT &MySize)
{
	POINT OldStart = Start;

	OldStart.x--;
	Start.x = DiagramSize.x - MySize.x;

	gui.DrawWire(Start, OldStart, false);
}

// Funcao para desenhar um elemento na tela, apenas para testes...
static void DrawGenericGUI(LadderElem *elem, void *data)
{
	POINT size;
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

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
		DoEOL(ddg->start, size, ddg->size);
	}

	int SelectedState = ddg->context->SelectedElem == elem ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(elem, SelectedState, ddg->start, ddg->size, _("???"));
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
		tCommandSource source = { nullptr, nullptr, this };
		gui.AddCommand(source, rCursor, CmdSelectRight, nullptr, false);
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
	tCommandSource source = { nullptr, nullptr, this };
	gui.AddCommand(source, r, CmdSelectBelow, nullptr, false);

	RECT rDialog = r;
	rDialog.left   += GridSize.x;
	rDialog.right  -= GridSize.x;
	rDialog.top    += 10;
	rDialog.bottom -= 10;
	gui.AddCommand(source, rDialog, CmdShowDialog, nullptr);

	r.left += GridSize.x;
	r.top  += (GridSize.y - FONT_HEIGHT)/2;

	gui.DrawText(txt.first .c_str(), r, 0, 1);

	r.top  += GridSize.y;
	gui.DrawText(txt.second.c_str(), r, 0, 1);
}

// Classe LadderElemContact
void ContactCmdToggleNegated(tCommandSource source, void *data)
{
	LadderElemContact *contact = dynamic_cast<LadderElemContact *>(source.elem);
	LadderElemContactProp *prop = (LadderElemContactProp *)contact->getProperties();

	ladder->CheckpointBegin("Inverter Contato");

	prop->negated = !prop->negated;

	contact->setProperties(ladder->getContext(), prop);

	ladder->CheckpointEnd();
	ladder->updateContext();
}

bool LadderElemContact::ShowDialog(LadderContext context)
{
	eType  type       = Diagram->getDetailsIO(prop.idName.first).type;
	bool   NewNegated = prop.negated;
	string NewName    = Diagram->getNameIO(prop.idName);

	bool changed = ShowContactsDialog(&NewNegated, &NewName, &type);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, type, infoIO_Name)) {
			LadderElemContactProp *data = (LadderElemContactProp *)getProperties();

			data->negated = NewNegated;
			data->idName  = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	tCommandSource source = { nullptr, nullptr, this };
	gui.AddCommand(source, rCmd, ContactCmdToggleNegated, nullptr);

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
	eType  type         = Diagram->getDetailsIO(prop.idName.first).type;
	bool   NewNegated   = prop.negated;
	bool   NewSetOnly   = prop.setOnly;
	bool   NewResetOnly = prop.resetOnly;
	string NewName      = Diagram->getNameIO(prop.idName);

	bool changed = ShowCoilDialog(&NewNegated, &NewSetOnly, &NewResetOnly, &NewName, &type);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, type, infoIO_Name)) {
			LadderElemCoilProp *data = (LadderElemCoilProp *)getProperties();

			data->setOnly   = NewSetOnly;
			data->resetOnly = NewResetOnly;
			data->negated   = NewNegated;
			data->idName    = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
	}

	return changed;
}

void LadderElemCoil::DrawGUI(void *data)
{
	POINT start, size, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

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

	DoEOL(ddg->start, size, ddg->size);
	start = ddg->start;

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
	int NewDelay   = prop.delay;
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowTimerDialog(getWhich(), &NewDelay, &NewName);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, getTimerTypeIO(getWhich()), infoIO_Name)) {
			LadderElemTimerProp *data = (LadderElemTimerProp *)getProperties();

			data->delay  = NewDelay;
			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowCounterDialog(getWhich(), &NewMax, &NewName);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, eType_Counter, infoIO_Name)) {
			LadderElemCounterProp *data = (LadderElemCounterProp *)getProperties();

			data->max    = NewMax;
			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowResetDialog(&NewName);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		eType type = Diagram->getDetailsIO(NewName).type;
		// No caso do reset, ele deve ser associado a uma variavel ja existente visto que ele deve
		// zerar um contador / timer que existe! Assim checa diretamente com ele se aceita a variavel.
		if(!acceptIO(prop.idName.first, type)) {
			changed = false;
			Diagram->ShowDialog(true, "Contador/Temporizador inválido", "Selecione um contador/temporizador válido!");
		} else if(Diagram->getIO(pin, NewName, type, infoIO_Name)) {// Variavel valida!
			LadderElemResetProp *data = (LadderElemResetProp *)getProperties();

			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string CurrOp1 = Diagram->getNameIO(prop.idOp1);
	string CurrOp2 = Diagram->getNameIO(prop.idOp2);
	string NewOp1  = CurrOp1;
	string NewOp2  = CurrOp2;

	bool changed = ShowCmpDialog(getWhich(), &NewOp1, &NewOp2);

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType op1_type = ladder->getTypeIO(CurrOp1, NewOp1, eType_General, true);
		eType op2_type = ladder->getTypeIO(CurrOp2, NewOp2, eType_General, true);

		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Op1;
		pin.pin   = prop.idOp1;
		pin.name  = NewOp1;
		pin.type  = op1_type;
		pins.push_back(pin);

		pin       = infoIO_Op2;
		pin.pin   = prop.idOp2;
		pin.name  = NewOp2;
		pin.type  = op2_type;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(Diagram->getIO(pins)) {
			LadderElemCmpProp *data = (LadderElemCmpProp *)getProperties();

			data->idOp1 = pins[0].pin;
			data->idOp2 = pins[1].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string CurrOp1  = Diagram->getNameIO(prop.idOp1 );
	string CurrOp2  = Diagram->getNameIO(prop.idOp2 );
	string CurrDest = Diagram->getNameIO(prop.idDest);
	string NewOp1   = CurrOp1;
	string NewOp2   = CurrOp2;
	string NewDest  = CurrDest;

	bool changed = ShowMathDialog(getWhich(), &NewDest, &NewOp1, &NewOp2);

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType op1_type = ladder->getTypeIO(CurrOp1, NewOp1, eType_General, true);
		eType op2_type = ladder->getTypeIO(CurrOp2, NewOp2, eType_General, true);

		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Dest;
		pin.pin   = prop.idDest;
		pin.name  = NewDest;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Op1;
		pin.pin   = prop.idOp1;
		pin.name  = NewOp1;
		pin.type  = op1_type;
		pins.push_back(pin);

		pin       = infoIO_Op2;
		pin.pin   = prop.idOp2;
		pin.name  = NewOp2;
		pin.type  = op2_type;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(ladder->getIO(pins)) {
			LadderElemMathProp *data = (LadderElemMathProp *)getProperties();

			data->idDest = pins[0].pin;
			data->idOp1  = pins[1].pin;
			data->idOp2  = pins[2].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewDest = Diagram->getNameIO(prop.idDest);
	string CurrSrc = Diagram->getNameIO(prop.idSrc);
	string NewSrc  = CurrSrc;

	bool changed = ShowSqrtDialog(&NewDest, &NewSrc);

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType src_type = ladder->getTypeIO(CurrSrc, NewSrc, eType_General, true);

		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Dest;
		pin.pin   = prop.idDest;
		pin.name  = NewDest;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Src;
		pin.pin   = prop.idSrc;
		pin.name  = NewSrc;
		pin.type  = src_type;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(ladder->getIO(pins)) {
			LadderElemSqrtProp *data = (LadderElemSqrtProp *)getProperties();

			data->idDest = pins[0].pin;
			data->idSrc  = pins[1].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewVar = Diagram->getNameIO(prop.idVar);
	string NewMin = Diagram->getNameIO(prop.idMin);
	string NewMax = Diagram->getNameIO(prop.idMax);

	bool changed = ShowRandDialog(&NewVar, &NewMin, &NewMax);

	if(changed) {
		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Var;
		pin.pin   = prop.idVar;
		pin.name  = NewVar;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Min;
		pin.pin   = prop.idMin;
		pin.name  = NewMin;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Max;
		pin.pin   = prop.idMax;
		pin.name  = NewMax;
		pin.type  = eType_General;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(ladder->getIO(pins)) {
			LadderElemRandProp *data = (LadderElemRandProp *)getProperties();

			data->idVar = pins[0].pin;
			data->idMin = pins[1].pin;
			data->idMax = pins[2].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewDest = Diagram->getNameIO(prop.idDest);
	string CurrSrc = Diagram->getNameIO(prop.idSrc);
	string NewSrc  = CurrSrc;

	bool changed = ShowAbsDialog(&NewDest, &NewSrc);

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType src_type = ladder->getTypeIO(CurrSrc, NewSrc, eType_General, true);

		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Dest;
		pin.pin   = prop.idDest;
		pin.name  = NewDest;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Src;
		pin.pin   = prop.idSrc;
		pin.name  = NewSrc;
		pin.type  = src_type;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(ladder->getIO(pins)) {
			LadderElemAbsProp *data = (LadderElemAbsProp *)getProperties();

			data->idDest = pins[0].pin;
			data->idSrc  = pins[1].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewDest = Diagram->getNameIO(prop.idDest);
	string CurrSrc = Diagram->getNameIO(prop.idSrc);
	string NewSrc  = CurrSrc;

	bool changed = ShowMathDialog(getWhich(), &NewDest, &NewSrc, nullptr);

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType src_type = ladder->getTypeIO(CurrSrc, NewSrc, eType_General, true);

		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Dest;
		pin.pin   = prop.idDest;
		pin.name  = NewDest;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Src;
		pin.pin   = prop.idSrc;
		pin.name  = NewSrc;
		pin.type  = src_type;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(ladder->getIO(pins)) {
			LadderElemMoveProp *data = (LadderElemMoveProp *)getProperties();

			data->idDest = pins[0].pin;
			data->idSrc  = pins[1].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowSetBitDialog(&NewName, &NewSet, &NewBit);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, eType_General, infoIO_Name)) {
			LadderElemSetBitProp *data = (LadderElemSetBitProp *)getProperties();

			data->set    = NewSet;
			data->bit    = NewBit;
			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string CurrName = Diagram->getNameIO(prop.idName);
	string NewName  = CurrName;

	bool changed = ShowCheckBitDialog(&NewName, &NewSet, &NewBit);

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType name_type = ladder->getTypeIO(CurrName, NewName, eType_General, true);

		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, name_type, infoIO_Name)) {
			LadderElemCheckBitProp *data = (LadderElemCheckBitProp *)getProperties();

			data->set    = NewSet;
			data->bit    = NewBit;
			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowReadAdcDialog(&NewName);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, eType_ReadADC, infoIO_Name)) {
			LadderElemReadAdcProp *data = (LadderElemReadAdcProp *)getProperties();

			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowSetDADialog(&NewName, &NewMode);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, eType_SetDAC, infoIO_Name)) {
			LadderElemSetDaProp *data = (LadderElemSetDaProp *)getProperties();

			data->mode   = NewMode;
			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowReadEncDialog(&NewName);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, eType_ReadEnc, infoIO_Name)) {
			LadderElemReadEncProp *data = (LadderElemReadEncProp *)getProperties();

			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowResetEncDialog(&NewName);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, eType_ResetEnc, infoIO_Name)) {
			LadderElemResetEncProp *data = (LadderElemResetEncProp *)getProperties();

			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewTime = Diagram->getNameIO(prop.idTime);
	string NewDesl = Diagram->getNameIO(prop.idDesl);

	LadderElemMultisetDAProp dialogData = prop;
	bool changed = ShowMultisetDADialog(&dialogData, &NewTime, &NewDesl);

	if(changed) {
		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Time;
		pin.pin   = prop.idTime;
		pin.name  = NewTime;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Desl;
		pin.pin   = prop.idDesl;
		pin.name  = NewDesl;
		pin.type  = eType_General;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(ladder->getIO(pins)) {
			LadderElemMultisetDAProp *data = (LadderElemMultisetDAProp *)getProperties();

			*data = dialogData;
			data->idTime = pins[0].pin;
			data->idDesl = pins[1].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	int NewId           = prop.id;
	int NewIndex        = prop.index;                                                                                                                                                                                                                                                                                                                                                                                         
	int NewParameter    = prop.parameter;
	int NewParameterSet = prop.parameter_set;
	string NewName      = Diagram->getNameIO(prop.idName);

	bool changed = ShowUSSDialog(getWhich(), &NewName, &NewId, &NewParameter, &NewParameterSet, &NewIndex);

	if(changed) {
		eType type;
		if(getWhich() == ELEM_READ_USS) {
			type = eType_ReadUSS;
		} else {
			type = eType_WriteUSS;
		}

		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, type, infoIO_Name)) {
			LadderElemUSSProp *data = (LadderElemUSSProp *)getProperties();

			data->idName        = pin;
			data->id            = NewId;
			data->parameter     = NewParameter;
			data->parameter_set = NewParameterSet;
			data->index         = NewIndex;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName       = Diagram->getNameIO(prop.idName);

	bool changed = ShowModbusDialog(getWhich() == ELEM_WRITE_MODBUS ? 1 : 0,
					&NewName, &NewElem, &NewAddress, &NewInt32, &NewRetransmitir);

	if(changed) {
		eType type;
		if(getWhich() == ELEM_READ_MODBUS) {
			type = eType_ReadModbus;
		} else {
			type = eType_WriteModbus;
		}

		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, type, infoIO_Name)) {
			LadderElemModBUSProp *data = (LadderElemModBUSProp *)getProperties();

			data->idName       = pin;
			data->elem         = NewElem;
			data->address      = NewAddress;
			data->int32        = NewInt32;
			data->retransmitir = NewRetransmitir;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowSetPwmDialog(&NewName, &NewTargetFreq);

	if(changed) {
		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, eType_PWM, infoIO_Name)) {
			LadderElemSetPWMProp *data = (LadderElemSetPWMProp *)getProperties();

			data->idName     = pin;
			data->targetFreq = NewTargetFreq;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowUartDialog(getWhich(), &NewName);

	if(changed) {
		eType type;
		if(getWhich() == ELEM_UART_RECV) {
			type = eType_RxUART;
		} else {
			type = eType_TxUART;
		}

		pair<unsigned long, int> pin = prop.idName;
		if(Diagram->getIO(pin, NewName, type, infoIO_Name)) {
			LadderElemUARTProp *data = (LadderElemUARTProp *)getProperties();

			data->idName = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	} else {
		changed = false;
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
	string NewDest   = Diagram->getNameIO(prop.idDest);
	string CurrIndex = Diagram->getNameIO(prop.idIndex);
	string NewIndex  = CurrIndex;

	LadderElemLUTProp Dialogdata = prop;

	bool changed = ShowLookUpTableDialog(&Dialogdata, &NewDest, &NewIndex);

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType index_type = ladder->getTypeIO(CurrIndex, NewIndex, eType_General, true);

		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Dest;
		pin.pin   = prop.idDest;
		pin.name  = NewDest;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Index;
		pin.pin   = prop.idIndex;
		pin.name  = NewIndex;
		pin.type  = index_type;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(ladder->getIO(pins)) {
			LadderElemLUTProp *data = (LadderElemLUTProp *)getProperties();

			*data = Dialogdata;
			data->idDest  = pins[0].pin;
			data->idIndex = pins[0].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewDest   = Diagram->getNameIO(prop.idDest);
	string CurrIndex = Diagram->getNameIO(prop.idIndex);
	string NewIndex  = CurrIndex;

	LadderElemPiecewiseProp Dialogdata = prop;
	bool changed = ShowPiecewiseLinearDialog(&Dialogdata, &NewDest, &NewIndex);

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType index_type = ladder->getTypeIO(CurrIndex, NewIndex, eType_General, true);

		tRequestIO pin;
		vector<tRequestIO> pins;

		pin       = infoIO_Dest;
		pin.pin   = prop.idDest;
		pin.name  = NewDest;
		pin.type  = eType_General;
		pins.push_back(pin);

		pin       = infoIO_Index;
		pin.pin   = prop.idIndex;
		pin.name  = NewIndex;
		pin.type  = index_type;
		pins.push_back(pin);

		// Se variavel alterada e valida, atualiza o pino
		if(ladder->getIO(pins)) {
			LadderElemPiecewiseProp *data = (LadderElemPiecewiseProp *)getProperties();

			*data = Dialogdata;
			data->idDest  = pins[0].pin;
			data->idIndex = pins[0].pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewVar = Diagram->getNameIO(prop.idVar);

	char NewTxt[MAX_NAME_LEN];
	strcpy(NewTxt, prop.txt.c_str());

	bool changed = ShowFormattedStringDialog(getWhich() == ELEM_WRITE_FORMATTED_STRING ? 1 : 0, &NewVar, NewTxt);

	if(changed) {
		eType type;
		if(getWhich() == ELEM_READ_FORMATTED_STRING) {
			type = eType_RxUART;
		} else {
			type = eType_TxUART;
		}

		pair<unsigned long, int> pin = prop.idVar;
		if(Diagram->getIO(pin, NewVar, type, infoIO_Var)) {
			LadderElemFmtStringProp *data = (LadderElemFmtStringProp *)getProperties();

			data->idVar = pin;
			data->txt = NewTxt;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewVar = Diagram->getNameIO(prop.idVar);

	char NewTxt[MAX_NAME_LEN];
	strcpy(NewTxt, prop.txt.c_str());

	bool changed = ShowServoYaskawaDialog(getWhich() == ELEM_WRITE_SERVO_YASKAWA ? 1 : 0, &NewId, &NewVar, NewTxt);

	if(changed) {
		eType type;
		if(getWhich() == ELEM_READ_SERVO_YASKAWA) {
			type = eType_ReadYaskawa;
		} else {
			type = eType_WriteYaskawa;
		}

		pair<unsigned long, int> pin = prop.idVar;
		if(Diagram->getIO(pin, NewVar, type, infoIO_Var)) {
			LadderElemYaskawaProp *data = (LadderElemYaskawaProp *)getProperties();

			data->id    = NewId;
			data->idVar = pin;
			data->txt = NewTxt;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	string NewVar = Diagram->getNameIO(prop.idVar);
	bool changed = ShowPersistDialog(&NewVar);

	if(changed) {
		pair<unsigned long, int> pin = prop.idVar;
		if(Diagram->getIO(pin, NewVar, eType_General, infoIO_Var)) {
			LadderElemPersistProp *data = (LadderElemPersistProp *)getProperties();

			data->idVar = pin;

			setProperties(context, data);
		} else {
			changed = false;
		}
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
	POINT StartWire, EndWire, PreviousWire, elemStart;
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
			elemStart = ElemDDG.start;
			it->elem->DrawGUI(&ElemDDG);

			if(ElemDDG.DontDraw == false) {
				gui.registerElementArea(it->elem, ElemDDG.start, ElemDDG.size);
			}

			if(it->elem == ddg->context->SelectedElem) {
				ddg->regionSelected = gui.getRECT(ElemDDG.start, ElemDDG.size);
			}

			ElemDDG.start = elemStart;
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

void cmdToggleBreakpoint(tCommandSource source, void *data)
{
	if(data != nullptr) {
		ladder->ToggleBreakPoint(*(int *)data);
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

	gui.SetBackgroundColor(context.inSimulationMode ? HighlightColours.simBg : HighlightColours.bg);

	char num[20];
	POINT SizeMax = { 0, 0 };

	vector<LadderCircuit>::size_type i;
	tDataDrawGUI RungDDG = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, true, true, &context };

	for(i = 0; i < rungs.size(); i++) {
		rungs[i]->rung->DrawGUI(&RungDDG);

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
		rungs[i]->rung->DrawGUI(&RungDDG);

		RECT r = gui.getRECT(RungDDG.start, RungDDG.size);
		r.right = r.left;
		r.left  = 0;

		// Adicionando comando para o breakpoint
		int *rung = new int;
		*rung = i;
		tCommandSource source = { this, nullptr, nullptr };
		gui.AddCommand(source, r, cmdToggleBreakpoint, rung);

		// Agora desenha o numero da linha
		r.left  = 5;
		r.top += (r.bottom - r.top - FONT_HEIGHT)/2;
		sprintf(num, "%3d", i + 1);
		gui.DrawText(num, r, 0, 2);

		// Agora desenha o breakpoint, se ativo
		if(rungs[i]->hasBreakpoint) {
			r.top    += FONT_HEIGHT + 5;
			r.bottom  = r.top + 10;
			r.left    = Grid1x1.x/2 - 5;
			r.right   = Grid1x1.x/2 + 5;
			gui.DrawEllipse(r, 2);
		}

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
		rungs[i]->rung->DrawGUI(&RungDDG);

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

LadderElem *LadderDiagram::SearchElement(eMoveCursor moveTo)
{
	return gui.SearchElement(context.SelectedElem, moveTo);
}

void LadderDiagram::ShowIoMapDialog(int item)
{
	IO->ShowIoMapDialog(item);
	updateContext();
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
