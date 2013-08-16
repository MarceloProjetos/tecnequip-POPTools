#include "poptools.h"

#include "EngineGUI.h"
#include "EngineRenderD2D.h"

// Variavel que indica se o cursor deve ser exibido. Liga e Desliga a cada 800 ms, conforme timer.
bool isCursorVisible = true;

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

// Estruturas que armazenam as cores do diagrama ladder
typedef struct {
	COLORREF Background;
	COLORREF Foreground;
	COLORREF Bus;
	COLORREF Wire;
	COLORREF WireOff;
	COLORREF Selection;
	COLORREF Breakpoint;
	COLORREF ConnectionDot;

	// Algumas cores padrao (branco, preto, etc...)
	COLORREF White;
	COLORREF Black;
} tLadderColors;

typedef struct {
	COLORREF Background;
	COLORREF Foreground;
	COLORREF Border;
	COLORREF BorderText;
} tLadderColorGroup;

// Definicao do ponteiro para os comandos dos itens expandidos
typedef bool (*tFncExpandedItem)(LadderElem *, unsigned int) ;

// Estruturas que definem os controles disponiveis para uso nos items expandidos
typedef struct {
	tFncExpandedItem fnc;
	vector<string>   items;
	int              selected;
} tControlList;

// Definicao que descreve um item expandido
typedef pair < string, unsigned int > tExpandedItem;

// Estrutura que armazena dados de comando em um controle de item expandido
typedef struct {
	tFncExpandedItem fnc;
	unsigned int     index;
} tCmdExpandedItem;

// Estrutura passada para a funcao que altera o nome de um I/O, cadastrada como um comando
// ao clicar no nome da variável em uma caixa de elemento
typedef struct {
	eType type;
	bool  reply;
} tCmdChangeNameData;

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

	// Variaveis que armazenam as cores utilizadas no diagrama ladder
	tLadderColors LadderColors;
	map<int, tLadderColorGroup> LadderColorGroups;

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

	vector<RECT> DrawExpandedItems(tLadderColorGroup cg, RECT r, POINT GridSize, unsigned int GridOffset, vector<tExpandedItem> items);

	// Funcoes para adicionar controles aos itens expandidos
	void addControlList(LadderElem *elem, RECT r, tControlList list);

	pair<POINT, POINT> DrawWire(POINT StartPoint, POINT EndPoint, bool StartHasConnectionDot);

	void        MouseClick(int x, int y, bool isDown, bool isDouble);
	LadderElem *SearchElement(LadderElem *ref, eMoveCursor moveTo);

	void AddCommand(tCommandSource source, RECT region, void (*fnc)(tCommandSource, void *), void *data, bool isDoubleClick = true);

	void CmdExpand(LadderElem *elem);

	// Funcoes para configurar as cores do diagrama ladder
	void              setLadderColors(tLadderColors colors);
	tLadderColors     getLadderColors(void);

	void              setLadderColorGroup(int elem, tLadderColorGroup colors);
	tLadderColorGroup getLadderColorGroup(int elem);
	void              delLadderColorGroup(int elem);
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
		if(source.elem->ShowDialog(ladder->getContext()) == true) {
			ladder->ProgramChanged();
			UpdateMainWindowTitleBar();
		}
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

void cmdExpandedItem(tCommandSource source, void *data)
{
	tCmdExpandedItem *cmd = (tCmdExpandedItem *)data;

	if(cmd->fnc != nullptr) {
		(cmd->fnc)(source.elem, cmd->index);
	}
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

	// Cria o grupo de cores padrao
	tLadderColorGroup group;

	group.Background = RGB(170, 170, 170);
	group.Foreground = RGB(  0,   0, 255);
	group.Border     = RGB( 85,  85,  85);
	group.BorderText = RGB(255, 255, 255);

	setLadderColorGroup(0, group);

	// Carrega as cores padrao da interface
	tLadderColors colors;

	colors.Background    = RGB(  0,   0,   0);
	colors.Foreground    = RGB(255, 255, 255);
	colors.Selection     = RGB(255,   0,   0);
	colors.Bus           = RGB(255, 255, 255);
	colors.ConnectionDot = RGB(255,   0,   0);
	colors.Wire          = RGB(255, 255, 255);
	colors.WireOff       = RGB(255, 255, 255);
	colors.Breakpoint    = RGB(255,   0,   0);

	colors.White         = RGB(255, 255, 255);
	colors.Black         = RGB(  0,   0,   0);

	setLadderColors(colors);
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
	SetBackgroundColor(LadderColors.Background);

	InterfaceColors[INTERF_COLOR_3DLIGHT ] = CreateBrush(GetSysColor(COLOR_3DLIGHT   ));
	InterfaceColors[INTERF_COLOR_3DFACE  ] = CreateBrush(GetSysColor(COLOR_3DFACE    ));
	InterfaceColors[INTERF_COLOR_3DSHADOW] = CreateBrush(GetSysColor(COLOR_3DDKSHADOW));

	/*** Grupos de Cores ***/
	tLadderColorGroup group;

	// Azul: I/O
	group.Background = RGB(170, 170, 255);
	group.Foreground = RGB(  0,   0, 255);
	group.Border     = RGB( 85,  85, 170);
	group.BorderText = RGB(255, 255, 255);

	setLadderColorGroup(ELEM_COIL            , group);
	setLadderColorGroup(ELEM_CONTACTS        , group);
	setLadderColorGroup(ELEM_ONE_SHOT_RISING , group);
	setLadderColorGroup(ELEM_ONE_SHOT_FALLING, group);
	setLadderColorGroup(ELEM_OPEN            , group);
	setLadderColorGroup(ELEM_MASTER_RELAY    , group);
	setLadderColorGroup(ELEM_SHORT           , group);

	// Verde: Temporizadores / Contadores
	group.Background = RGB(170, 255, 170);
	group.Foreground = RGB( 85, 170,  85);
	group.Border     = RGB( 85, 170,  85);
	group.BorderText = RGB(255, 255, 255);

	setLadderColorGroup(ELEM_TON, group);
	setLadderColorGroup(ELEM_TOF, group);
	setLadderColorGroup(ELEM_RTO, group);
	setLadderColorGroup(ELEM_CTU, group);
	setLadderColorGroup(ELEM_CTD, group);
	setLadderColorGroup(ELEM_CTC, group);
	setLadderColorGroup(ELEM_RES, group);
	setLadderColorGroup(ELEM_RTC, group);

	// Laranja: Variaveis / Condicionais / Matematicos
	group.Background = RGB(255, 200, 132);
	group.Foreground = RGB(  0,   0, 255);
	group.Border     = RGB(255, 140,   0);
	group.BorderText = RGB(255, 255, 255);

	setLadderColorGroup(ELEM_MOVE            , group);
	setLadderColorGroup(ELEM_SET_BIT         , group);
	setLadderColorGroup(ELEM_CHECK_BIT       , group);
	setLadderColorGroup(ELEM_EQU             , group);
	setLadderColorGroup(ELEM_NEQ             , group);
	setLadderColorGroup(ELEM_LES             , group);
	setLadderColorGroup(ELEM_LEQ             , group);
	setLadderColorGroup(ELEM_GRT             , group);
	setLadderColorGroup(ELEM_GEQ             , group);
	setLadderColorGroup(ELEM_PERSIST         , group);
	setLadderColorGroup(ELEM_ADD             , group);
	setLadderColorGroup(ELEM_SUB             , group);
	setLadderColorGroup(ELEM_DIV             , group);
	setLadderColorGroup(ELEM_MUL             , group);
	setLadderColorGroup(ELEM_SQRT            , group);
	setLadderColorGroup(ELEM_ABS             , group);
	setLadderColorGroup(ELEM_MOD             , group);
	setLadderColorGroup(ELEM_RAND            , group);
	setLadderColorGroup(ELEM_LOOK_UP_TABLE   , group);
	setLadderColorGroup(ELEM_PIECEWISE_LINEAR, group);
	setLadderColorGroup(ELEM_SHIFT_REGISTER  , group);

	// Amarelo: Analogicos / Motores / Encoders
	group.Background = RGB(255, 255, 170);
	group.Foreground = RGB(255, 255, 170);
	group.Border     = RGB(220, 220,   0);
	group.BorderText = RGB(255, 255, 255);

	setLadderColorGroup(ELEM_SET_DA     , group);
	setLadderColorGroup(ELEM_READ_ADC   , group);
	setLadderColorGroup(ELEM_READ_ENC   , group);
	setLadderColorGroup(ELEM_RESET_ENC  , group);
	setLadderColorGroup(ELEM_MULTISET_DA, group);
	setLadderColorGroup(ELEM_SET_PWM    , group);

	// Roxo: Comunicacao
	group.Background = RGB(255, 170, 255);
	group.Foreground = RGB(255,   0, 255);
	group.Border     = RGB(170,  85, 170);
	group.BorderText = RGB(255, 255, 255);

	setLadderColorGroup(ELEM_READ_MODBUS           , group);
	setLadderColorGroup(ELEM_WRITE_MODBUS          , group);
	setLadderColorGroup(ELEM_READ_FORMATTED_STRING , group);
	setLadderColorGroup(ELEM_WRITE_FORMATTED_STRING, group);
	setLadderColorGroup(ELEM_UART_RECV             , group);
	setLadderColorGroup(ELEM_UART_SEND             , group);
	setLadderColorGroup(ELEM_READ_USS              , group);
	setLadderColorGroup(ELEM_WRITE_USS             , group);
	setLadderColorGroup(ELEM_READ_SERVO_YASKAWA    , group);
	setLadderColorGroup(ELEM_WRITE_SERVO_YASKAWA   , group);
}

void LadderGUI::DrawStart(int OffsetX, int OffsetY)
{
	POINT OffsetXY = { -OffsetX, -OffsetY };

	SetDrawOffset(OffsetXY);
	StartDraw();

	MouseOverElement = nullptr;

	ClearCommandLists();

	vectorAreaElements.clear();
}

void LadderGUI::DrawEnd(void)
{
	POINT start = { 0, 0 }, size = { 1, 1 };
	RECT r = gui.getRECT(start, size);
	r.left  -= Grid1x1.x/2;

	vector<RECT>::iterator it;
	for(it = vectorConnectionDots.begin(); it != vectorConnectionDots.end(); it++) {
		if(it->left > r.left) {
			DrawEllipse(*it, LadderColors.ConnectionDot);
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

	tLadderColorGroup colorgroup = getLadderColorGroup(elem->getWhich());

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
			DrawRectangle(rCursor, LadderColors.Selection, true , 0, 0, 45);
		}

		if(SelectedState == SELECTED_BELOW) {
			rCursor.top    = r.bottom - 7;
			rCursor.bottom = r.bottom + 7;
			DrawRectangle(rCursor, LadderColors.Selection, true , 0, 0, 45);
		}
	}

	// Desenha a barra
	DrawRectangle(r, colorgroup.Background);

	// Desenha as linhas de seleção superior / inferior
	if(SelectedState != SELECTED_NONE) {
		POINT start = { r.left, r.top }, end = { r.right, r.top };
		DrawLine(start, end, LadderColors.Selection);

		start.y = r.bottom;
		end.y   = r.bottom;
		DrawLine(start, end, LadderColors.Selection);
	}

	return r;
}

RECT LadderGUI::DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, POINT GridSize, string ElemName, bool ShowExpand)
{
	unsigned int SizeZ = 30;

	bool isMouseOver;
	RECT r = getRECT(StartTopLeft, GridSize);
	r.bottom += 10; // Aumenta um pouco alem do grid para a linha de conexao entrar na caixa ao ines de ficar por baixo

	// Calcula a posicao da linha do barramento
	POINT MidPoint;
	POINT StartPoint = { r.left  - Grid1x1.x / 2, r.top + HeaderHeight + Grid1x1.y };
	POINT EndPoint   = { r.right + Grid1x1.x / 2, r.top + HeaderHeight + Grid1x1.y };

	// Centraliza o objeto, descontando metade da profundidade
	r.left  -= SizeZ/2;
	r.right -= SizeZ/2;

	if(elem == nullptr) return r;

	tLadderColorGroup colorgroup = getLadderColorGroup(elem->getWhich());

	isMouseOver = MouseOver(r);

	// Desenha o indicador de selecao
	RECT rSel, rCursor = { - 7, - 7, + 7, + 7 };

	// Estrutura com a origem do comando
	tCommandSource source = { nullptr, nullptr, elem };

	// Selecao Direita
	if(SelectedState == SELECTED_RIGHT) {
		rCursor.left   += r.right;
		rCursor.right  += r.right;
		rCursor.top    += r.top + HeaderHeight + Grid1x1.y;
		rCursor.bottom += r.top + HeaderHeight + Grid1x1.y;
	} else {
		rSel.left   = r.left;
		rSel.right  = r.right + Grid1x1.x/2;
		rSel.top    = r.top;
		rSel.bottom = r.bottom;
		AddCommand(source, rSel, CmdSelectRight, nullptr, false);
	}

	// Selecao Esquerda
	if(SelectedState == SELECTED_LEFT) {
		rCursor.left   += r.left;
		rCursor.right  += r.left;
		rCursor.top    += r.top + HeaderHeight + Grid1x1.y;
		rCursor.bottom += r.top + HeaderHeight + Grid1x1.y;
	} else {
		rSel.left   = r.left - Grid1x1.x/2;
		rSel.right  = r.left + Grid1x1.x/2;
		rSel.top    = r.top;
		rSel.bottom = r.bottom;
		AddCommand(source, rSel, CmdSelectLeft, nullptr, false);
	}

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

	if(SelectedState != SELECTED_NONE && isCursorVisible) {
		rCursor.left  += 30;
		rCursor.right += 30;
		DrawRectangle(rCursor, LadderColors.Selection, true , 0, 0, 45);
	}

	// Desenha a caixa 3D
	RECT r3D = r;
	if(!isMouseOver) {
		r3D.top += HeaderHeight;
	}

	COLORREF bgcolor = SelectedState != SELECTED_NONE ? LadderColors.Selection : colorgroup.Border;

	DrawRectangle3D(r3D, (float)SizeZ, colorgroup.Background, bgcolor, bgcolor, true, 10, 10);

	// Desenha a linha de barramento do elemento, ou seja, 1 para baixo no grid e 1/2 grid para cada lado
	MidPoint.x = r3D.left - 2; // Desconta a borda
	MidPoint.y = StartPoint.y;
	DrawLine(StartPoint, MidPoint, LadderColors.Wire);

	MidPoint.x = r3D.right + SizeZ/2;
	MidPoint.y = EndPoint.y;
	DrawLine(MidPoint  , EndPoint, LadderColors.Wire);

	// Desenha a caixa em si. Se mouse estiver sobre o elemento, desenha com borda.
	if(isMouseOver) {
		MouseOverElement = elem;

		RECT rHeader = r, rExpand = { 0, 0, 0, 0 };

		rHeader.bottom = rHeader.top + HeaderHeight;
		DrawRectangle(rHeader, colorgroup.Border, true, 10);

		rHeader.top += HeaderHeight/2;
		DrawRectangle(rHeader, colorgroup.Border, true);

		r.left   += 3;
		r.top    += 3;
		r.right  -= 3;
		r.bottom -= 3;

		if(ShowExpand) {
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
			DrawLine(StartPoint, EndPoint, colorgroup.BorderText);

			StartPoint.x += 10;
			DrawLine(StartPoint, EndPoint, colorgroup.BorderText);

			StartPoint.y -= 5;
			EndPoint.y -= 5;
			DrawLine(StartPoint, EndPoint, colorgroup.BorderText);

			StartPoint.x -= 10;
			DrawLine(StartPoint, EndPoint, colorgroup.BorderText);

			// Adiciona o comando do botao de expandir / retrair
			AddCommand(source, rExpand, ::CmdExpand, nullptr, false);
		}

		// Desenha o texto do nome do controle
		RECT rText = r;
		rText.right -= rExpand.right - rExpand.left;
		DrawText(ElemName.c_str(), rText, 0, colorgroup.BorderText, eAlignMode_Center, eAlignMode_TopLeft);

		r.top = rHeader.bottom;
	} else {
		r.top += HeaderHeight;

		r.left   += 3;
		r.right  -= 3;
		r.bottom -= 3;
	}

	if(SelectedState != SELECTED_NONE) {
		DrawRectangle(r3D, LadderColors.Selection, false, 10, 10);
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

	tLadderColorGroup colorgroup = getLadderColorGroup(elem->getWhich());

	size.x = 1 + (txt_size / Grid1x1.x);
	size.y = 1;

	r = DrawElementBox(elem, SelectedState, StartTopLeft, size);
	r.top += 2;
	r.left += (r.right - r.left - txt_size)/2;
	DrawText(txt.first.c_str(), r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	r.top += FONT_HEIGHT;
	DrawText(txt.second.c_str(), r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
}

vector<RECT> LadderGUI::DrawExpandedItems(tLadderColorGroup cg, RECT r, POINT GridSize, unsigned int GridOffset, vector<tExpandedItem> items)
{
	RECT rText, rExp;
	vector<RECT> vectorRectExp;

	r.top += Grid1x1.y * GridOffset;
	rExp = r;

	unsigned int i;
	for(i = 0; i < items.size(); i++) {
		rExp.left   = r.left  + 5;
		rExp.right  = r.right - 5;
		rExp.bottom = rExp.top + Grid1x1.y * items[i].second;
		gui.DrawRectangle(rExp, cg.Border, true, 5);

		rText       = rExp;
		rText.left += 10;
		gui.DrawText(items[i].first.c_str(), rText, 0, LadderColors.White, eAlignMode_TopLeft, eAlignMode_Center);

		rExp.top    += 5;
		rExp.left   += Grid1x1.x + 5;
		rExp.right  -= 5;
		rExp.bottom -= 5;
		gui.DrawRectangle(rExp, LadderColors.White, true, 5);
		vectorRectExp.push_back(rExp);

		rExp.top = rExp.bottom + 10;
	}

	return vectorRectExp;
}

void LadderGUI::addControlList(LadderElem *elem, RECT r, tControlList list)
 {
	unsigned int offsetText = 3, itemBorder = 1;
	unsigned int colWidth, colHeight, nCols, nLines;

	// Esta funcao adiciona um controle no modo de lista. Ele tenta identificar o numero de colunas utilizaveis
	// conforme a largura do retangulo e o tamanho dos textos
	unsigned int i, maxlen = 0;
	for(i = 0; i < list.items.size(); i++) {
		if(list.items[i].size() > maxlen) {
			maxlen = list.items[i].size();
		}
	}

	maxlen *= FONT_WIDTH; // Multiplica os caracteres pela largura de cadaum deles
	maxlen += offsetText + 2*itemBorder; // Cada elemento tambem deve considerar as bordas e o deslocamento do texto

	if(maxlen == 0) return; // Maior texto tem tamanho zero ??? Ou lista vazia ou items com strings vazias...

	r.top    += itemBorder;
	r.left   += itemBorder;
	r.bottom -= itemBorder;
	r.right  -= itemBorder;

	nCols  = (r.right - r.left) / maxlen;

	if(nCols == 0) return; // Textos muito longos ou area muito pequena, impossivel montar a lista!
	if(nCols > list.items.size()) {
		nCols = list.items.size();
	}

	nLines = (list.items.size() / nCols) + ((list.items.size() % nCols) != 0);

	colWidth  = (r.right - r.left) / nCols;
	colHeight = (r.bottom - r.top) / nLines;

	if(colHeight < FONT_HEIGHT + 2) return; // Sem espaco entre as linhas, ficaria tudo embaralhado... retorna!

	// Nesse ponto ja temos as dimensoes da lista e todos os valores ja foram checados e sao validos
	RECT rItem;
	unsigned int offsetX, offsetY;

	tLadderColorGroup cg = getLadderColorGroup(elem->getWhich());

	for(i = 0; i < list.items.size(); i++) {
		offsetX = (i % nCols) * colWidth;
		offsetY = (i / nCols) * colHeight;

		rItem = r;
		rItem.top    += offsetY;
		rItem.left   += offsetX;
		rItem.bottom += offsetY;
		rItem.right  += offsetX;

		rItem.right   = rItem.left + colWidth - itemBorder;
		rItem.bottom  = rItem.top + colHeight - itemBorder;
		rItem.left   += itemBorder;
		rItem.top    += itemBorder;

		if(i == list.selected) {
			gui.DrawRectangle(rItem, cg.Border, true, 3);
		} else {
			tCommandSource source = { nullptr, nullptr, nullptr };
			source.elem = elem;

			tCmdExpandedItem *cmd = new tCmdExpandedItem;
			cmd->index = i;
			cmd->fnc   = list.fnc;

			AddCommand(source, rItem, cmdExpandedItem, cmd, false);
		}

		gui.DrawText(list.items[i].c_str(), rItem, 0, i == list.selected ? LadderColors.White : cg.Border, eAlignMode_Center, eAlignMode_Center);
	}
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
	DrawLine(StartPoint, EndPoint, LadderColors.Wire);

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

// Funcoes para configurar as cores do diagrama ladder
void LadderGUI::setLadderColors(tLadderColors colors)
{
	LadderColors.Background    = CreateBrush(colors.Background   );
	LadderColors.Foreground    = CreateBrush(colors.Foreground   );
	LadderColors.Selection     = CreateBrush(colors.Selection    );
	LadderColors.Bus           = CreateBrush(colors.Bus          );
	LadderColors.ConnectionDot = CreateBrush(colors.ConnectionDot);
	LadderColors.Wire          = CreateBrush(colors.Wire         );
	LadderColors.WireOff       = CreateBrush(colors.WireOff      );
	LadderColors.Breakpoint    = CreateBrush(colors.Breakpoint   );
	LadderColors.White         = CreateBrush(colors.White        );
	LadderColors.Black         = CreateBrush(colors.Black        );
}

tLadderColors LadderGUI::getLadderColors(void)
{
	return LadderColors;
}

void LadderGUI::setLadderColorGroup(int elem, tLadderColorGroup colors)
{
	if(elem >= 0) {
		LadderColorGroups[elem].Background = CreateBrush(colors.Background);
		LadderColorGroups[elem].Foreground = CreateBrush(colors.Foreground);
		LadderColorGroups[elem].Border     = CreateBrush(colors.Border    );
		LadderColorGroups[elem].BorderText = CreateBrush(colors.BorderText);
	}
}

tLadderColorGroup LadderGUI::getLadderColorGroup(int elem)
{
	if(LadderColorGroups.count(elem) == 0) {
		elem = 0;
	}

	return LadderColorGroups[elem];
}

void LadderGUI::delLadderColorGroup(int elem)
{
	// Nao permite excluir o grupo padrao!
	if(elem > 0) {
		map<int, tLadderColorGroup>::iterator it;
		it = LadderColorGroups.find(elem);
		if(it != LadderColorGroups.end()) {
			LadderColorGroups.erase(it);
		}
	}
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

// Funcao que atualiza o Nome/Tipo de um I/O
bool cmdChangeName(LadderElem *elem, unsigned int index, pair<unsigned long, int> pin, eType type,
	vector<eType> types, char *title, char *field)
{
	bool ret    = false;
	string name = ladder->getNameIO(pin);

	if(ShowVarDialog(title, field, &name, types)) {
		if(elem->updateNameTypeIO(index, name, type)) {
			ret = true;
			UpdateMainWindowTitleBar();
		}
	}

	return ret;
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
		gui.DrawEllipse(rCursor, gui.getLadderColors().Selection);
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

	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich());

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

	gui.DrawText(txt.first .c_str(), r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	r.top  += GridSize.y;
	gui.DrawText(txt.second.c_str(), r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
}

// Classe LadderElemContact
void ContactCmdChangeName(tCommandSource source, void *data)
{
	bool                ret;
	eType               type;
	tCmdChangeNameData *dataChangeName = (tCmdChangeNameData *)(data);
	LadderElemContact  *contact        = dynamic_cast<LadderElemContact *>(source.elem);

	// Le os dados do I/O para ver se ainda esta marcado como sendo do tipo reservado.
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemContactProp *prop      = (LadderElemContactProp *)contact->getProperties();
	mapDetails             detailsIO = ladder->getDetailsIO(prop->idName.first);

	vector<eType> types;

	if(dataChangeName == nullptr) {
		type = detailsIO.type;
	} else {
		type = dataChangeName->type;
	}

	types.push_back(type);

	ret = cmdChangeName(source.elem, 0, prop->idName, type, types, _("Contacts"), _("Name:"));
	if(dataChangeName != nullptr) {
		dataChangeName->reply = ret;
	}

	// Aqui desalocamos as propriedades
	delete prop;
}

bool ContactCmdExpandedSource(LadderElem *elem, unsigned int selected)
{
	bool  ret;
	eType type;
	tCmdChangeNameData dataChangeName;
	LadderElemContact *contact = dynamic_cast<LadderElemContact *>(elem);

	switch(selected) {
		case 0: type = eType_DigInput     ; break;
		case 1: type = eType_DigOutput    ; break;
		case 2: type = eType_InternalRelay; break;
		case 3: type = eType_InternalFlag ; break;
	}

	// Le os dados do I/O para ver se ainda esta marcado como sendo do tipo reservado.
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemContactProp *prop      = (LadderElemContactProp *)contact->getProperties();
	mapDetails             detailsIO = ladder->getDetailsIO(prop->idName.first);
	string                 name      = ladder->getNameIO(prop->idName);

	// Aqui desalocamos as propriedades
	delete prop;

	ret = contact->updateNameTypeIO(0, name, type);

	if(!ret || detailsIO.type == eType_Reserved) {
		tCommandSource source = { nullptr, nullptr, elem };
		dataChangeName.type   = type;

		ContactCmdChangeName(source, &dataChangeName);
		ret = dataChangeName.reply;
	} else {
		UpdateMainWindowTitleBar();
	}

	return ret;
}

bool ContactCmdExpandedNegated(LadderElem *elem, unsigned int negated)
{
	bool ret = false;
	bool new_negated = negated ? true : false;
	LadderElemContact *contact = dynamic_cast<LadderElemContact *>(elem);
	LadderElemContactProp *prop = (LadderElemContactProp *)contact->getProperties();

	if(prop->negated != new_negated) {
		ladder->CheckpointBegin("Inverter Contato");

		prop->negated = new_negated;

		contact->setProperties(ladder->getContext(), prop);

		ladder->CheckpointEnd();
		ladder->updateContext();
		ladder->ProgramChanged();
		UpdateMainWindowTitleBar();

		ret = true;
	} else {
		delete prop;
	}

	return ret;
}

void ContactCmdToggleNegated(tCommandSource source, void *data)
{
	LadderElemContactProp *prop = (LadderElemContactProp *)source.elem->getProperties();

	ContactCmdExpandedNegated(source.elem, prop->negated ? 0 : 1); // Inverte o estado atual

	delete prop;
}

bool LadderElemContact::ShowDialog(LadderContext context)
{/*
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

	return changed;*/
	return false;
}

void LadderElemContact::DrawGUI(void *data)
{
	RECT rCmd;
	POINT start, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	if(ddg->expanded) {
		ddg->size.x = 5;
		ddg->size.y = 6;
	} else {
		ddg->size.x = 2;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich());

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
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(name, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, ContactCmdChangeName, nullptr);

	// Desenha o contato
	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 5;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, colors.Wire);

	start.x  = end.x;
	start.y -= 10;
	end.y   += 10;
	gui.DrawLine(start, end, colors.Wire);

	start.x += 10;
	end.x   += 10;
	gui.DrawLine(start, end, colors.Wire);

	rCmd.left    = start.x - 10;
	rCmd.right   = start.x;
	rCmd.top     = start.y;
	rCmd.bottom  = end.y;
	gui.AddCommand(source, rCmd, ContactCmdToggleNegated, nullptr);

	start.y = r.top + GridSize.y;
	end.x   = r.right;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, colors.Wire);

	// Exibe o texto que identifica o tipo de contato: Entrada, Saida, Rele Interno ou Flag interna
	char ch[] = { 0, 0 };
	int selected = 0;

	switch(detailsIO.type) {
	case eType_Reserved:
		selected = -1;
	case eType_DigInput:
		ch[0] = 'E';
		break;

	case eType_DigOutput:
		ch[0] = 'S';
		selected = 1;
		break;

	case eType_InternalRelay:
		ch[0] = 'R';
		selected = 2;
		break;

	case eType_InternalFlag:
		ch[0] = 'F';
		selected = 3;
		break;

	default:
		ch[0] = '?';
		selected = -1;
		break;
	}

	rText      = r;
	rText.top  = end.y + 5;
	rText.left = end.x - 15;
	rText.bottom -= 5;
	gui.DrawText(ch, rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	// Desenha o sinal de fechado, se ativado
	if(prop.negated) {
		start.y -= 10;
		end.x    = start.x - 10;
		end.y   += 10;
		gui.DrawLine(start, end, colors.Wire);
	}

	// Se expandido, desenha os itens do modo expandido
	if(ddg->expanded) {
		vector<tExpandedItem> items;
		items.push_back(tExpandedItem(_("Source"), 2));
		items.push_back(tExpandedItem(_("Negado"), 1));
		
		vector<RECT> rExp = gui.DrawExpandedItems(colorgroup, r, ddg->size, 2, items);

		// Caixas desenhadas. Agora criamos o conteudo
		tControlList list;

		list.items.push_back("Entrada");
		list.items.push_back("Saida");
		list.items.push_back("Rele Interno");
		list.items.push_back("Flag Interna");

		list.selected = selected;
		list.fnc      = ContactCmdExpandedSource;

		gui.addControlList(this, rExp[0], list);

		list.items.clear();
		list.items.push_back("Não");
		list.items.push_back("Sim");

		list.fnc      = ContactCmdExpandedNegated;
		list.selected = prop.negated ? 1 : 0;

		gui.addControlList(this, rExp[1], list);
	}
}

// Classe LadderElemCoil
void CoilCmdChangeName(tCommandSource source, void *data)
{
	bool                ret;
	eType               type;
	tCmdChangeNameData *dataChangeName = (tCmdChangeNameData *)(data);
	LadderElemCoil     *coil           = dynamic_cast<LadderElemCoil *>(source.elem);

	// Le os dados do I/O para ver se ainda esta marcado como sendo do tipo reservado.
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemCoilProp *prop      = (LadderElemCoilProp *)coil->getProperties();
	mapDetails          detailsIO = ladder->getDetailsIO(prop->idName.first);

	vector<eType> types;

	if(dataChangeName == nullptr) {
		type = detailsIO.type;
	} else {
		type = dataChangeName->type;
	}

	types.push_back(type);

	ret = cmdChangeName(source.elem, 0, prop->idName, type, types, _("Coil"), _("Name:"));
	if(dataChangeName != nullptr) {
		dataChangeName->reply = ret;
	}

	// Aqui desalocamos as propriedades
	delete prop;
}

bool CoilCmdExpandedSource(LadderElem *elem, unsigned int selected)
{
	bool  ret;
	eType type;
	tCmdChangeNameData dataChangeName;
	LadderElemCoil *coil = dynamic_cast<LadderElemCoil *>(elem);

	switch(selected) {
		case 0: type = eType_DigOutput    ; break;
		case 1: type = eType_InternalRelay; break;
	}

	// Le os dados do I/O para ver se ainda esta marcado como sendo do tipo reservado.
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemCoilProp *prop      = (LadderElemCoilProp *)coil->getProperties();
	mapDetails          detailsIO = ladder->getDetailsIO(prop->idName.first);
	string              name      = ladder->getNameIO(prop->idName);

	// Aqui desalocamos as propriedades
	delete prop;

	ret = coil->updateNameTypeIO(0, name, type);

	if(!ret || detailsIO.type == eType_Reserved) {
		tCommandSource source = { nullptr, nullptr, elem };
		dataChangeName.type   = type;

		CoilCmdChangeName(source, &dataChangeName);
		ret = dataChangeName.reply;
	}

	if(ret) {
		UpdateMainWindowTitleBar();
	}

	return ret;
}

bool CoilCmdExpandedType(LadderElem *elem, unsigned int type)
{
	bool ret = false;
	bool new_negated = false, new_setonly = false, new_resetonly = false;

	LadderElemCoil *coil = dynamic_cast<LadderElemCoil *>(elem);
	LadderElemCoilProp *prop = (LadderElemCoilProp *)coil->getProperties();

	switch(type) {
		case 1: new_negated   = true; break;
		case 2: new_setonly   = true; break;
		case 3: new_resetonly = true; break;
	}

	if(prop->negated != new_negated || prop->setOnly != new_setonly || prop->resetOnly != new_resetonly) {
		ladder->CheckpointBegin("Alterar Tipo de Bobina");

		prop->negated   = new_negated;
		prop->setOnly   = new_setonly;
		prop->resetOnly = new_resetonly;

		coil->setProperties(ladder->getContext(), prop);

		ladder->CheckpointEnd();
		ladder->updateContext();
		ladder->ProgramChanged();
		UpdateMainWindowTitleBar();

		ret = true;
	} else {
		delete prop;
	}

	return ret;
}

bool LadderElemCoil::ShowDialog(LadderContext context)
{/*
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

	return changed;*/
	return false;
}

void LadderElemCoil::DrawGUI(void *data)
{
	POINT start, size, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 4;
		ddg->size.y = 9;
	} else {
		ddg->size.x = 2;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich());

	string sname = ladder->getNameIO(prop.idName.first);
	const char *name = sname.c_str();

	DoEOL(ddg->start, size, ddg->size);
	start = ddg->start;

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, start, ddg->size, _("BOBINA"));
	ddg->region = r;

	// Escreve o nome da bobina
	RECT rText    = r;
	rText.left   += 5;
	rText.top    += 5;
	rText.bottom  = rText.top + FONT_HEIGHT;
	gui.DrawText(name, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, CoilCmdChangeName, nullptr);

	// Desenha a bobina
	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 9;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, colors.Wire);

	start.x  = end.x + 10;
	gui.DrawEllipse(start, 10, 10, colors.Wire, false);

	if(prop.setOnly || prop.resetOnly) {
		rText.top    = start.y - 10;
		rText.left   = start.x - 10;
		rText.bottom = start.y + 10;
		rText.right  = start.x + 10;

		gui.DrawText(prop.setOnly ? "S" : "R", rText, 0, colors.Wire, eAlignMode_Center, eAlignMode_Center);
	}

	start.x += 10;
	end.x    = r.right;
	gui.DrawLine(start, end, colors.Wire);

	// Exibe o texto que identifica o tipo de contato: Entrada, Saida, Rele Interno ou Flag interna
	char ch[] = { 0, 0 };
	unsigned int selected;
	mapDetails detailsIO = ladder->getDetailsIO(prop.idName.first);

	switch(detailsIO.type) {
	case eType_Reserved:
	case eType_DigOutput:
		ch[0] = 'S';
		selected = 0;
		break;

	case eType_InternalRelay:
		ch[0] = 'R';
		selected = 1;
		break;

	default:
		ch[0] = '?';
		selected = 0;
		break;
	}

	rText      = r;
	rText.top  = end.y + 5;
	rText.left = end.x - 15;
	rText.bottom -= 5;
	gui.DrawText(ch, rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	// Desenha o sinal de fechado, se ativado
	if(prop.negated) {
		start.x -= 3;
		start.y -= 7;
		end.x    = start.x - 14;
		end.y   += 7;
		gui.DrawLine(start, end, colors.Wire);
	}

	// Se expandido, desenha os itens do modo expandido
	if(ddg->expanded) {
		vector<tExpandedItem> items;
		items.push_back(tExpandedItem(_("Source"), 2));
		items.push_back(tExpandedItem(_("Type"  ), 4));

		vector<RECT> rExp = gui.DrawExpandedItems(colorgroup, r, ddg->size, 2, items);

		// Caixas desenhadas. Agora criamos o conteudo
		tControlList list;

		list.items.push_back(_("Pin on MCU"));
		list.items.push_back(_("Internal Relay"));

		list.selected = selected;
		list.fnc      = CoilCmdExpandedSource;

		gui.addControlList(this, rExp[0], list);

		list.items.clear();
		list.items.push_back(_("( ) Normal"    ));
		list.items.push_back(_("(/) Negated"   ));
		list.items.push_back(_("(S) Set-Only"  ));
		list.items.push_back(_("(R) Reset-Only"));

		list.fnc      = CoilCmdExpandedType;

		if(prop.negated) {
			list.selected = 1;
		} else if(prop.setOnly) {
			list.selected = 2;
		} else if(prop.resetOnly) {
			list.selected = 3;
		} else {
			list.selected = 0;
		}

		gui.addControlList(this, rExp[1], list);
	}
}

// Classe LadderElemTimer
void TimerCmdChangeName(tCommandSource source, void *data)
{
	int              which = source.elem->getWhich();
	eType            type  = getTimerTypeIO(which);
	LadderElemTimer *timer = dynamic_cast<LadderElemTimer *>(source.elem);

	char *s;
    switch(which) 
	{ 
        case ELEM_TON: s = _("Turn-On Delay"); break;
        case ELEM_TOF: s = _("Turn-Off Delay"); break;
        case ELEM_RTO: s = _("Retentive Turn-On Delay"); break;
        default: oops(); break;
    }

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemTimerProp *prop = (LadderElemTimerProp *)timer->getProperties();

	vector<eType> types;
	types.push_back(type);

	cmdChangeName(source.elem, 0, prop->idName, type, types, s, _("Name:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

void TimerCmdChangeTime(tCommandSource source, void *data)
{
	int              which = source.elem->getWhich();
	eType            type  = getTimerTypeIO(which);
	LadderElemTimer *timer = dynamic_cast<LadderElemTimer *>(source.elem);

	char *s;
    switch(which) 
	{ 
        case ELEM_TON: s = _("Turn-On Delay"); break;
        case ELEM_TOF: s = _("Turn-Off Delay"); break;
        case ELEM_RTO: s = _("Retentive Turn-On Delay"); break;
        default: oops(); break;
    }

	// Le os dados do Timer para atualizar o delay
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemTimerProp *prop = (LadderElemTimerProp *)timer->getProperties();

	char cname[100];
	sprintf(cname, "%d", prop->delay / 1000);
	string name = cname;

	vector<eType> types;
	types.push_back(eType_General);

	if(ShowVarDialog(s, _("Delay (ms):"), &name, types) &&
		ladder->IsValidNameAndType(0, name.c_str(), eType_Pending , _("Tempo"), VALIDATE_IS_NUMBER, 1, 2147483)) {
			prop->delay = 1000*atoi(name.c_str());

			LadderSettingsGeneral settings = ladder->getSettingsGeneral();
			if(prop->delay % settings.cycleTime) {
				prop->delay = (1 + prop->delay / settings.cycleTime) * settings.cycleTime;
				Error(_("Tempo de ciclo deve ser múltiplo de %d! Será utilizado %d."),
					settings.cycleTime/1000, prop->delay/1000);
			}

			source.elem->setProperties(ladder->getContext(), prop);
	} else {
		// Se foi cancelada a alteracao, devemos desalocar as propriedades
		delete prop;
	}
}

bool LadderElemTimer::ShowDialog(LadderContext context)
{/*
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

	return changed;*/
	return false;
}

void LadderElemTimer::DrawGUI(void *data)
{
	POINT start, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	if(ddg->expanded) {
		ddg->size.x = 2;
		ddg->size.y = 3;
	} else {
		ddg->size.x = 2;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich());

	string sname = ladder->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	mapDetails detailsIO = ladder->getDetailsIO(prop.idName.first);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("ATRASO"));
	ddg->region = r;

	// Flag que indica se o desenho deve iniciar com nivel alto ou baixo
	bool doHighStart = (getWhich() == ELEM_TOF) ? true : false;

	// Primeiro calculamos as coordenadas do relogio
	RECT rClock = r;
	if(!doHighStart) {
		rClock.top += GridSize.y - 10;
	}
	rClock.bottom = rClock.top + 20;
	rClock.left += 5;
	rClock.right = rClock.left + (rClock.bottom - rClock.top);

	// Desenha o a linha inicial do timer
	start.x = r.left;
	start.y = r.top + (doHighStart ? 10 : GridSize.y);
	end.x   = rClock.right + 10;
	end.y   = start.y;
	gui.DrawLine(start, end, colors.Wire);

	// Agora desenha o circulo do relogio
	gui.DrawEllipse(rClock, colorgroup.Background);
	gui.DrawEllipse(rClock, colorgroup.Foreground, false);

	if(ladder->getContext().inSimulationMode) {
	} else {
		// Em seguida, desenha os ponteiros
		POINT start, end;
		start.x = rClock.left + (rClock.right - rClock.left)/2;
		start.y = rClock.top + 5;

		end.x = start.x;
		end.y = rClock.top + (rClock.bottom - rClock.top)/2;

		gui.DrawLine(start, end, colorgroup.Foreground);

		start.x = end.x + 5;
		start.y = end.y + 5;

		gui.DrawLine(start, end, colorgroup.Foreground);
	}

	// Desenha a linha vertical
	start.x  = end.x;
	if(doHighStart) {
		start.y += GridSize.y - 10;
	} else {
		start.y -= GridSize.y - 10;
	}
	gui.DrawLine(start, end, colors.Wire);

	// Desenha a seta
	POINT StartArrow = start, EndArrow = end;

	if(doHighStart) {
		StartArrow.y -= 5;
		EndArrow.y    = StartArrow.y - 10;
	} else {
		StartArrow.y += 5;
		EndArrow.y    = StartArrow.y + 10;
	}
	EndArrow.x   -= 5;
	gui.DrawLine(StartArrow, EndArrow, colors.Wire);

	EndArrow.x   += 10;
	gui.DrawLine(StartArrow, EndArrow, colors.Wire);

	// Desenha o texto indicando o tempo do timer
	RECT rText = r;
	char buf[1024];

	LadderSettingsGeneral settings = ladder->getSettingsGeneral();
	DescribeForIoList(prop.delay / settings.cycleTime, getTimerTypeIO(getWhich()), buf);

	rText.top = (doHighStart ? end.y - 5 : start.y + 5);
	rText.bottom = (doHighStart ? start.y : end.y);
	rText.left = start.x + 10;
	rText.right = r.right;
	gui.DrawText(buf, rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, TimerCmdChangeTime, nullptr);

	// Desenha a linha final, ligando ao final do elemento
	end.x = r.right;
	end.y = start.y;
	gui.DrawLine(start, end, colors.Wire);

	// Se expandido, desenha o texto com o nome da variavel
	if(ddg->expanded) {
		string txt = _("Nome: ") + Diagram->getNameIO(prop.idName);
		rText.top = r.bottom - GridSize.y + 5;
		rText.bottom = r.bottom;
		rText.left = r.left + 10;
		rText.right = r.right;
		gui.DrawText(txt.c_str(), rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
		gui.AddCommand(source, rText, TimerCmdChangeName, nullptr);
	}
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
	POINT GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	ddg->size.x = 3;
	ddg->size.y = 2;

	if(ddg->DontDraw) return;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich());

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("RTC"), false);
	ddg->region = r;

	POINT start, size = { 42, 42 };

	start.x = r.left + 3;
	start.y = r.top + 5;
	gui.DrawPictureFromResource(IDB_LADDER_RTC, start, size);

	int linha;
	char bufs[256], bufe[256];

	char mday [10];
	char month[10];
	char year [10];

	memset(month, 0, sizeof(month));
	memset(year , 0, sizeof(year));

	for(linha=0; linha<2; linha++) {
		struct tm *ptm = linha ? &prop.end : &prop.start;

		if(prop.wday & (1 << 7)) 
		{
			int i;

			sprintf(linha ? bufe : bufs, "%s %02d:%02d:%02d", _("SMTWTFS"),
				ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

			if(linha || prop.mode == ELEM_RTC_MODE_FIXED) {
				for(i=0; i<7; i++) {
					if(!(prop.wday & (1<<i))) {
						(linha ? bufe : bufs)[i] = ' ';
					} else if(linha) {
						bufe[i] = '*';
					}
				}
			}
		} else {
			if (ptm->tm_mday)
				sprintf(mday, "%02d", ptm->tm_mday);
			else
				sprintf(mday, " *");

			if (ptm->tm_mon)
				sprintf(month, "%02d", ptm->tm_mon);
			else
				sprintf(month, " *");

			if (ptm->tm_year)
				sprintf(year, "%02d", ptm->tm_year);			
			else
				sprintf(year, " *  ");

			sprintf(linha ? bufe : bufs, "%s/%s/%s %02d:%02d:%02d", mday, month, year, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		} 
	}

	if(prop.mode == ELEM_RTC_MODE_FIXED) {
		strcpy(bufe, bufs);
		strcpy(bufs, _("Mode Fixed"));
	}

	r.top  += 10;
	r.left += size.x + 6;
	gui.DrawText(bufs, r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	r.top += FONT_HEIGHT + 5;
	gui.DrawText(bufe, r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
}

// Classe LadderElemCounter
void CounterCmdChangeName(tCommandSource source, void *data)
{
	int                which = source.elem->getWhich();
	LadderElemCounter *counter = dynamic_cast<LadderElemCounter *>(source.elem);

	char *title;
    switch(which) 
	{ 
        case ELEM_CTU: title = _("Count Up"        ); break;
        case ELEM_CTD: title = _("Count Down"      ); break;
        case ELEM_CTC: title = _("Circular Counter"); break;
        default: oops(); break;
    }

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemCounterProp *prop = (LadderElemCounterProp *)counter->getProperties();

	vector<eType> types;
	types.push_back(eType_Counter);

	cmdChangeName(source.elem, 0, prop->idName, eType_Counter, types, title, _("Name:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

void CounterCmdChangeValue(tCommandSource source, void *data)
{
	int                which = source.elem->getWhich();
	LadderElemCounter *counter = dynamic_cast<LadderElemCounter *>(source.elem);

	char *title, *desc;
    switch(which) 
	{ 
        case ELEM_CTU: desc = _("True if >= :"); title = _("Count Up"        ); break;
        case ELEM_CTD: desc = _("True if <= :"); title = _("Count Down"      ); break;
        case ELEM_CTC: desc = _("Max value:"  ); title = _("Circular Counter"); break;
        default: oops(); break;
    }

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemCounterProp *prop = (LadderElemCounterProp *)counter->getProperties();

	char cname[100];
	sprintf(cname, "%d", prop->max);
	string name = cname;

	vector<eType> types;
	types.push_back(eType_Counter);

	if(ShowVarDialog(title, desc, &name, types) &&
		ladder->IsValidNameAndType(0, name.c_str(), eType_Pending, _("Valor"), VALIDATE_IS_NUMBER, 0, 0)) {
			prop->max = atoi(name.c_str());

			source.elem->setProperties(ladder->getContext(), prop);
	} else {
		// Se foi cancelada a alteracao, devemos desalocar as propriedades
		delete prop;
	}
}

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

	tLadderColors colors = gui.getLadderColors    ();

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
	gui.DrawLine(start, end, colors.Wire);

	start.x  = end.x;
	start.y -= 20;
	gui.DrawLine(start, end, colors.Wire);

	if(isOSR) {
		POINT StartArrow = start, EndArrow = end;

		StartArrow.y += 5;
		EndArrow.x   -= 5;
		EndArrow.y    = StartArrow.y + 10;
		gui.DrawLine(StartArrow, EndArrow, colors.Wire);

		EndArrow.x   += 10;
		gui.DrawLine(StartArrow, EndArrow, colors.Wire);
	}

	end.x += 20;
	end.y  = start.y;
	gui.DrawLine(start, end, colors.Wire);

	start.x  = end.x;
	start.y += 20;
	gui.DrawLine(start, end, colors.Wire);

	if(!isOSR) {
		POINT StartArrow = start, EndArrow = end;

		StartArrow.y -= 5;
		EndArrow.x   -= 5;
		EndArrow.y    = StartArrow.y - 10;
		gui.DrawLine(StartArrow, EndArrow, colors.Wire);

		EndArrow.x   += 10;
		gui.DrawLine(StartArrow, EndArrow, colors.Wire);
	}

	end.x = r.right;
	end.y = start.y;
	gui.DrawLine(start, end, colors.Wire);
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
	char *s;
	string op1 = Diagram->getNameIO(prop.idOp1);
	string op2 = Diagram->getNameIO(prop.idOp2);

	switch(getWhich()) {
		case ELEM_EQU: s = "=="; break;
		case ELEM_NEQ: s = "<>"; break;
		case ELEM_GRT: s = ">" ; break;
		case ELEM_GEQ: s = ">="; break;
		case ELEM_LES: s = "<" ; break;
		case ELEM_LEQ: s = "<="; break;
		default: oops();
	}

	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 4;
		ddg->size.y = 4;
	} else {
		ddg->size.x = 3;
		ddg->size.y = 3;
	}

	if(ddg->DontDraw) return;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich());

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("COMPARAR"), true);
	ddg->region = r;

	op1 += " ";
	op1 += s;
	r.left += 10;
	r.top  += (GridSize.y - FONT_HEIGHT)/2;
	gui.DrawText(op1.c_str(), r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	r.top  += GridSize.y;
	gui.DrawText(op2.c_str(), r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
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

	tLadderColors colors = gui.getLadderColors();

	GetCursorPos(&xy);
	gui.setMousePosition(xy);

	gui.SetBackgroundColor(colors.Background);

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
		sprintf(num, "%3d", i + 1);
		gui.DrawText(num, r, 0, colors.Foreground, eAlignMode_TopLeft, eAlignMode_Center);

		// Agora desenha o breakpoint, se ativo
		if(rungs[i]->hasBreakpoint) {
			r.top    += FONT_HEIGHT + 5;
			r.bottom  = r.top + 10;
			r.left    = Grid1x1.x/2 - 5;
			r.right   = Grid1x1.x/2 + 5;
			gui.DrawEllipse(r, colors.Breakpoint);
		}

		RungDDG.start.y += RungDDG.size.y + 1;
	}

	POINT start = { 0, 0 };
	r = gui.getRECT(start, SizeMax);
	r.left  -= Grid1x1.x/2;
	r.right += Grid1x1.x/2;

	RECT rBus = { r.left - 5, 0, r.left, r.bottom };
	gui.DrawRectangle(rBus, colors.Bus);

	rBus.left  = r.right;
	rBus.right = r.right + 5;
	gui.DrawRectangle(rBus, colors.Bus);

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
