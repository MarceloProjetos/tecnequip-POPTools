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
	COLORREF BusOff;
	COLORREF Wire;
	COLORREF WireOff;
	COLORREF Selection;
	COLORREF Breakpoint;
	COLORREF ConnectionDot;
	COLORREF ConnectionDotOff;

	// Algumas cores padrao (branco, preto, etc...)
	COLORREF White;
	COLORREF Black;
	COLORREF Red;
	COLORREF Green;
	COLORREF Blue;
} tLadderColors;

typedef struct {
	COLORREF Background;
	COLORREF Foreground;
	COLORREF Border;
	COLORREF BorderText;
} tLadderColorGroup;

// Estrutura que define um ponto de conexao entre linhas
typedef struct {
	POINT        dot; // Posicao do ponto
	unsigned int count; // Numero de cruzamentos. Se apenas 1 significa apenas juncao de duas linhas
	bool         isPowered; // Indica se o ponto esta em um linha energizada ou nao
} tConnectionDot;

// Estrutura que define uma linha do ladder
typedef struct {
	POINT start;     // Ponto inicial da linha
	POINT end;       // Ponto ifnal da linha
	bool  isPowered; // Indica se a linha esta energizada ou nao
} tWire;

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
// ao clicar no nome da vari�vel em uma caixa de elemento
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

// Funcao que identifica se um ponto pertence a uma linha (somente horizontal ou vertical)
bool isPointOverLine(POINT p, POINT LineStart, POINT LineEnd)
{
	int point, start, end;
	// Primeiro identifica se a linha eh vertical ou horizontal
	if(LineStart.x == LineEnd.x) { // vertical
		if(p.x != LineStart.x) return false; // Ponto nao esta sobre a linha
		point = p        .y;
		start = LineStart.y;
		end   = LineEnd  .y;
	} else if(LineStart.y == LineEnd.y) { // horizontal
		if(p.y != LineStart.y) return false; // Ponto nao esta sobre a linha
		point = p        .x;
		start = LineStart.x;
		end   = LineEnd  .x;
	} else { // nem vertical nem horizontal. Retorna falso!
		return false;
	}

	return (point > start && point < end);
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

	vector<tConnectionDot>             vectorConnectionDots;
	vector<tWire>                      vectorWires;
	vector<LadderElem *>               vectorExpandedElements;
	vector< pair<LadderElem *, RECT> > vectorAreaElements;

	int HeaderHeight;

	array<unsigned int, 3>    InterfaceColors;

	typedef struct {
		RECT r;
		void(*fnc)(tCommandSource, void *);
		void *data;
		bool isSimMode;
	} tCommandItem;
	typedef pair<tCommandSource, tCommandItem>  tCommandListItem;
	typedef vector<tCommandListItem>            tCommandsList;

	tCommandsList CommandsSingleClick;
	tCommandsList CommandsDoubleClick;

	void ClearCommandLists(void);

	bool MouseOver (RECT region);

	void PaintScrollAndSplitter(void);

	// Variaveis que armazenam as cores utilizadas no diagrama ladder
	array<tLadderColors, 2>       LadderColors;
	map<int, tLadderColorGroup> LadderColorGroups;
	vector<tLadderColorGroup>   LadderColorSimulation;

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

	void DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, bool poweredBefore);
	RECT DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, POINT GridSize, string ElemName, bool ShowExpand, bool poweredBefore);
	void DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, pair<string, string> txt, bool poweredBefore);

	RECT DrawElementBar(LadderElem *elem, int SelectedState, int StartGridY, int GridHeight);

	vector<RECT> DrawExpandedItems(tLadderColorGroup cg, RECT r, POINT GridSize, unsigned int GridOffset, vector<tExpandedItem> items);

	// Funcoes para adicionar controles aos itens expandidos
	void addControlList(LadderElem *elem, RECT r, tControlList list);

	tWire DrawWire(POINT StartPoint, POINT EndPoint, bool poweredBefore);
	void  addConnectionDot(POINT p, bool isPowered, bool isForced);

	void        MouseClick(int x, int y, bool isDown, bool isDouble);
	LadderElem *SearchElement(LadderElem *ref, eMoveCursor moveTo);

	RECT getElemArea(LadderElem *elem);

	void AddCommand(tCommandSource source, RECT region, void (*fnc)(tCommandSource, void *), void *data, bool isDoubleClick, bool isSimulationMode);

	void CmdExpand(LadderElem *elem);

	// Funcoes para configurar as cores do diagrama ladder
	void              setLadderColors(tLadderColors colors, bool isModeSimulation);
	tLadderColors     getLadderColors(void);

	void              setLadderColorGroup(int elem, tLadderColorGroup colors);
	tLadderColorGroup getLadderColorGroup(int elem, bool isActive);
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
	if(source.elem != nullptr && ladder->getContext().inSimulationMode == false) {
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

	group.Background = RGB(200, 200, 200);
	group.Foreground = RGB( 55,  55,  55);
	group.Border     = RGB( 85,  85,  85);
	group.BorderText = RGB(255, 255, 255);

	setLadderColorGroup(0, group);

	// Cria o grupo de cores de simulacao - inativo
	group.Background = CreateBrush(RGB(100, 130, 130));
	group.Foreground = CreateBrush(RGB(255, 255, 255));
	group.Border     = CreateBrush(RGB( 64,  80,  80));
	group.BorderText = CreateBrush(RGB(255, 255, 255));
	LadderColorSimulation.push_back(group);

	// Cria o grupo de cores de simulacao - ativo
	group.Background = CreateBrush(RGB(255, 100, 100));
	group.Foreground = CreateBrush(RGB(255, 255, 255));
	group.Border     = CreateBrush(RGB(163,  73, 163));
	group.BorderText = CreateBrush(RGB(255, 255, 255));
	LadderColorSimulation.push_back(group);

	// Carrega as cores padrao da interface - modo normal
	tLadderColors colors;

	colors.Background       = RGB(  0,   0,   0);
	colors.Foreground       = RGB(255, 255, 255);
	colors.Selection        = RGB(255,   0,   0);
	colors.Bus              = RGB(255, 255, 255);
	colors.BusOff           = RGB(255, 255, 255);
	colors.ConnectionDot    = RGB(255,   0,   0);
	colors.ConnectionDotOff = RGB(255,   0,   0);
	colors.Wire             = RGB(255, 255, 255);
	colors.WireOff          = RGB(255, 255, 255);
	colors.Breakpoint       = RGB(255,   0,   0);

	colors.White            = RGB(255, 255, 255);
	colors.Black            = RGB(  0,   0,   0);
	colors.Red              = RGB(255,   0,   0);
	colors.Green            = RGB(  0, 255,   0);
	colors.Blue             = RGB(  0,   0, 255);

	setLadderColors(colors, false);

	// Carrega as cores padrao da interface - modo de simulacao
	colors.Background       = RGB(  0,   0,   0);
	colors.Foreground       = RGB(255, 255, 255);
	colors.Selection        = RGB(255,   0,   0);
	colors.Bus              = RGB(255,   0,   0);
	colors.BusOff           = RGB(  0,   0, 255);
	colors.ConnectionDot    = RGB(255,   0,   0);
	colors.ConnectionDotOff = RGB( 51, 181, 229);
	colors.Wire             = RGB(255,   0,   0);
	colors.WireOff          = RGB( 51, 181, 229);
	colors.Breakpoint       = RGB(255,   0,   0);

	colors.White            = RGB(255, 255, 255);
	colors.Black            = RGB(  0,   0,   0);
	colors.Red              = RGB(255,   0,   0);
	colors.Green            = RGB(  0, 255,   0);
	colors.Blue             = RGB(  0,   0, 255);

	setLadderColors(colors, true);
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
	tLadderColors colors = getLadderColors();
	SetTarget(DrawWindow);
	SetBackgroundColor(colors.Background);

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
	group.Background = RGB(150, 235, 150);
	group.Foreground = RGB( 65, 150,  65);
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
	group.Background = RGB(235, 235, 150);
	group.Foreground = RGB(150, 150,  65);
	group.Border     = RGB(180, 180,   0);
	group.BorderText = RGB(255, 255, 255);

	setLadderColorGroup(ELEM_SET_DA     , group);
	setLadderColorGroup(ELEM_READ_ADC   , group);
	setLadderColorGroup(ELEM_READ_ENC   , group);
	setLadderColorGroup(ELEM_RESET_ENC  , group);
	setLadderColorGroup(ELEM_MULTISET_DA, group);
	setLadderColorGroup(ELEM_SET_PWM    , group);

	// Roxo: Comunicacao
	group.Background = RGB(235, 150, 235);
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
	vector<tConnectionDot>::iterator it;
	for(it = vectorConnectionDots.begin(); it != vectorConnectionDots.end(); it++) {
		bool draw = false;
		if(it->count > 2) {
			draw = true;
		} else {
			unsigned int i, size = vectorWires.size();

			for(i=0; i < size; i++) {
				if(isPointOverLine(it->dot, vectorWires[i].start, vectorWires[i].end)) {
					draw = true;
					it->isPowered = it->isPowered || vectorWires[i].isPowered;
					break;
				}
			}
		}

		if(draw) { // Apenas desenha conexoes com mais de duas ligacoes senao eh apenas juncao de duas linhas
			DrawEllipse(it->dot, 5.0f, 5.0f, it->isPowered ? getLadderColors().ConnectionDot : getLadderColors().ConnectionDotOff);
		}
	}

	vectorConnectionDots.clear();
	vectorWires.clear();

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

	tLadderColors     colors     = getLadderColors();
	tLadderColorGroup colorgroup = getLadderColorGroup(elem->getWhich(), elem->IsPoweredAfter());

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
			DrawRectangle(rCursor, colors.Selection, true , 0, 0, 45);
		}

		if(SelectedState == SELECTED_BELOW) {
			rCursor.top    = r.bottom - 7;
			rCursor.bottom = r.bottom + 7;
			DrawRectangle(rCursor, colors.Selection, true , 0, 0, 45);
		}
	}

	// Desenha a barra
	DrawRectangle(r, colorgroup.Background);

	// Desenha as linhas de sele��o superior / inferior
	if(SelectedState != SELECTED_NONE) {
		POINT start = { r.left, r.top }, end = { r.right, r.top };
		DrawLine(start, end, colors.Selection);

		start.y = r.bottom;
		end.y   = r.bottom;
		DrawLine(start, end, colors.Selection);
	}

	return r;
}

RECT LadderGUI::DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, POINT GridSize, string ElemName, bool ShowExpand, bool poweredBefore)
{
	unsigned int SizeZ = 30;
	bool inSimulationMode = ladder->getContext().inSimulationMode;

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

	tLadderColors     colors     = getLadderColors();
	tLadderColorGroup colorgroup = getLadderColorGroup(elem->getWhich(), elem->IsPoweredAfter());

	isMouseOver = MouseOver(r);

	// Desenha o indicador de selecao
	RECT rSel, rCursor = { - 7, - 7, + 7, + 7 };

	// Estrutura com a origem do comando
	tCommandSource source = { nullptr, nullptr, elem };

	if(inSimulationMode == false) {
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
			AddCommand(source, rSel, CmdSelectRight, nullptr, false, false);
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
			AddCommand(source, rSel, CmdSelectLeft, nullptr, false, false);
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
			AddCommand(source, rSel, CmdSelectAbove, nullptr, false, false);
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
			AddCommand(source, rSel, CmdSelectBelow, nullptr, false, false);
		}
	}

	// Desenha a caixa 3D
	RECT r3D = r;
	if(!isMouseOver) {
		r3D.top += HeaderHeight;
	}

	COLORREF bgcolor = (SelectedState != SELECTED_NONE && inSimulationMode == false) ? colors.Selection : colorgroup.Border;

	DrawRectangle3D(r3D, (float)SizeZ, colorgroup.Background, bgcolor, bgcolor, true, 10, 10);

	// Desenha a linha de barramento do elemento, ou seja, 1 para baixo no grid e 1/2 grid para cada lado
	MidPoint.x = r3D.left - 2; // Desconta a borda
	MidPoint.y = StartPoint.y;
	DrawLine(StartPoint, MidPoint, poweredBefore ? colors.Wire : colors.WireOff);

	MidPoint.x = r3D.right + SizeZ/2;
	MidPoint.y = EndPoint.y;
	DrawLine(MidPoint  , EndPoint, elem->IsPoweredAfter() ? colors.Wire : colors.WireOff);

	addConnectionDot(StartPoint, poweredBefore, false);
	addConnectionDot(EndPoint, elem->IsPoweredAfter(), false);

	if(SelectedState != SELECTED_NONE && isCursorVisible && inSimulationMode == false) {
		DrawRectangle(rCursor, colors.Selection, true , 0, 0, 45);
		DrawRectangle(r3D    , colorgroup.Background , true, 10, 10);
	}

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
			AddCommand(source, rExpand, ::CmdExpand, nullptr, false, false);
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

	if(SelectedState != SELECTED_NONE && inSimulationMode == false) {
		float width = gui.getBrushWidth();
		gui.setBrushWidth(5.0f);
		DrawRectangle(r3D, colors.Selection, false, 10, 10);
		gui.setBrushWidth(width);
	}

	AddCommand(source, r, CmdShowDialog, nullptr, true, false);

	return r;
}

void LadderGUI::DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, bool poweredBefore)
{
	POINT size = { 1, 1 };

	DrawElementBox(elem, SelectedState, StartTopLeft, size, "???", false, poweredBefore);
}

void LadderGUI::DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, pair<string, string> txt, bool poweredBefore)
{
	RECT r;
	POINT size;
	int txt_size = max(txt.first.size(), txt.second.size()) * FONT_WIDTH;

	tLadderColorGroup colorgroup = getLadderColorGroup(elem->getWhich(), elem->IsPoweredAfter());

	size.x = 1 + (txt_size / Grid1x1.x);
	size.y = 1;

	r = DrawElementBox(elem, SelectedState, StartTopLeft, size, "???", false, poweredBefore);
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

	tLadderColors colors = getLadderColors();

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
		gui.DrawText(items[i].first.c_str(), rText, 0, colors.White, eAlignMode_TopLeft, eAlignMode_Center);

		rExp.top    += 5;
		rExp.left   += Grid1x1.x + 5;
		rExp.right  -= 5;
		rExp.bottom -= 5;
		gui.DrawRectangle(rExp, colors.White, true, 5);
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

	tLadderColorGroup cg = getLadderColorGroup(elem->getWhich(), elem->IsPoweredAfter());

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

			AddCommand(source, rItem, cmdExpandedItem, cmd, false, false);
		}

		gui.DrawText(list.items[i].c_str(), rItem, 0, i == list.selected ? getLadderColors().White : cg.Border, eAlignMode_Center, eAlignMode_Center);
	}
}

tWire LadderGUI::DrawWire(POINT StartPoint, POINT EndPoint, bool isPowered)
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

	tWire wire = { StartPoint, EndPoint, isPowered };

	// Desenha a linha (Horizontal ou Vertical)
	DrawLine(StartPoint, EndPoint, isPowered ? getLadderColors().Wire : getLadderColors().WireOff);

	// Registra os pontos de conexao da linha
	addConnectionDot(StartPoint, isPowered, false);
	addConnectionDot(EndPoint  , isPowered, false);

	vectorWires.push_back(wire);

	return wire;
}

void LadderGUI::addConnectionDot(POINT p, bool isPowered, bool isForced)
{
	// Calculamos um retangulo para start.x = 0 pois assim teremos a coordenada do barramento.
	// Somente adiciona um ponto apos o inicio do barramento...
	POINT startR  = { 0, 0 }, sizeR = { 1, 1 };
	RECT r        = getRECT(startR, sizeR);
	r.left       -= Grid1x1.x/2;

	if(p.x <= r.left) {
		return;
	}

	unsigned int i, size = vectorConnectionDots.size();
	for(i=0; i < size; i++) {
		if(p.x == vectorConnectionDots[i].dot.x && p.y == vectorConnectionDots[i].dot.y) { // Ja existe!
			vectorConnectionDots[i].isPowered = vectorConnectionDots[i].isPowered || isPowered;
			vectorConnectionDots[i].count++;

			if(isForced && vectorConnectionDots[i].count < 3) {
				vectorConnectionDots[i].count = 3;
			}

			return;
		}
	}

	// Ponto nao encontrado! cria um novo
	tConnectionDot dot;

	dot.dot       = p;
	dot.count     = isForced ? 3 : 1;
	dot.isPowered = isPowered;

	vectorConnectionDots.push_back(dot);
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
			if(it->second.fnc != nullptr && ladder->getContext().inSimulationMode == it->second.isSimMode &&
				PointInsideRegion(p, it->second.r)) {
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

RECT LadderGUI::getElemArea(LadderElem *elem)
{
	unsigned int i;

	for(i = 0; i < vectorAreaElements.size(); i++) {
		if(vectorAreaElements[i].first == elem) { // Encontrado!
			return vectorAreaElements[i].second;
		}
	}

	// Se chegou aqui significa que o elemento nao foi encontrado...
	// Criamos entao um retangulo com as coordenadas zeradas para indicar o erro
	RECT r = { 0, 0, 0, 0 };

	return r;
}

void LadderGUI::AddCommand(tCommandSource source, RECT region, void (*fnc)(tCommandSource, void *), void *data, bool isDoubleClick, bool isSimulationMode)
{
	tCommandsList *Commands;
	Commands = isDoubleClick ? &CommandsDoubleClick : &CommandsSingleClick;

	tCommandListItem item;
	item.first            = source;
	item.second.r         = region;
	item.second.fnc       = fnc;
	item.second.data      = data;
	item.second.isSimMode = isSimulationMode;

	Commands->push_back(item);
}

// Funcoes para configurar as cores do diagrama ladder
void LadderGUI::setLadderColors(tLadderColors colors, bool isModeSimulation)
{
	unsigned int index = isModeSimulation ? 1 : 0;
	LadderColors[index].Background       = CreateBrush(colors.Background      );
	LadderColors[index].Foreground       = CreateBrush(colors.Foreground      );
	LadderColors[index].Selection        = CreateBrush(colors.Selection       );
	LadderColors[index].Bus              = CreateBrush(colors.Bus             );
	LadderColors[index].BusOff           = CreateBrush(colors.BusOff          );
	LadderColors[index].ConnectionDot    = CreateBrush(colors.ConnectionDot   );
	LadderColors[index].ConnectionDotOff = CreateBrush(colors.ConnectionDotOff);
	LadderColors[index].Wire             = CreateBrush(colors.Wire            );
	LadderColors[index].WireOff          = CreateBrush(colors.WireOff         );
	LadderColors[index].Breakpoint       = CreateBrush(colors.Breakpoint      );
	LadderColors[index].White            = CreateBrush(colors.White           );
	LadderColors[index].Black            = CreateBrush(colors.Black           );
	LadderColors[index].Red              = CreateBrush(colors.Red             );
	LadderColors[index].Green            = CreateBrush(colors.Green           );
	LadderColors[index].Blue             = CreateBrush(colors.Blue            );
}

tLadderColors LadderGUI::getLadderColors(void)
{
	return ladder->getContext().inSimulationMode ? LadderColors[1] : LadderColors[0];
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

tLadderColorGroup LadderGUI::getLadderColorGroup(int elem, bool isActive)
{
	if(LadderColorGroups.count(elem) == 0) {
		elem = 0;
	}

	if(ladder->getContext().inSimulationMode) {
		return LadderColorSimulation[isActive ? 1 : 0];
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
static void DoEOL(POINT &Start, POINT &DiagramSize, POINT &MySize, bool poweredBefore)
{
	POINT OldStart = Start;

	Start.x = DiagramSize.x - MySize.x;

	if(Start.x != OldStart.x) {
		OldStart.x--;
		gui.DrawWire(Start, OldStart, poweredBefore);
	}
}

// Funcao para desenhar um elemento na tela, apenas para testes...
static void DrawGenericGUI(LadderElem *elem, void *data, bool poweredBefore, string name = "???")
{
	POINT size;
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 2;
	ddg->size.y = 2;

	if(ddg->DontDraw) return;

	if(elem->IsEOL()) {
		DoEOL(ddg->start, size, ddg->size, poweredBefore);
	}

	int SelectedState = ddg->context->SelectedElem == elem ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(elem, SelectedState, ddg->start, ddg->size, name.c_str(), false, poweredBefore);
	ddg->region = r;

	r.top   += 5;
	r.left  += 5;
	r.right -= 5;
	gui.DrawText("Duplo clique para editar:", r, 0,
		gui.getLadderColorGroup(elem->getWhich(), elem->IsPoweredAfter()).Foreground,
		eAlignMode_TopLeft, eAlignMode_TopLeft);

	r.top += 15 + FONT_HEIGHT;
	gui.DrawText(name.c_str(), r, 0, gui.getLadderColorGroup(elem->getWhich(), elem->IsPoweredAfter()).Foreground,
		eAlignMode_Center, eAlignMode_TopLeft);
}

// Funcao que atualiza o Nome/Tipo de um I/O
bool cmdChangeName(LadderElem *elem, unsigned int index, pair<unsigned long, int> pin, eType type,
	vector<eType> types, char *title, char *field)
{
	bool ret    = false;
	string name = ladder->getNameIO(pin);

	POINT start, size, GridSize = gui.getGridSize();
	RECT rArea = gui.getElemArea(elem);

	RECT rWindow;
	GetWindowRect(DrawWindow, &rWindow);

	start.x = rArea.left   ;
	start.y = rArea.top    ;
	size .x = rArea.right  - rArea.left;
	size .y = rArea.bottom - rArea.top;

	if(ShowVarDialog(title, field, &name, start, size, GridSize, types)) {
		if(elem->updateNameTypeIO(index, name, type)) {
			ret = true;
			UpdateMainWindowTitleBar();
		}
	}

	return ret;
}

// Classe LadderElemPlaceHolder
bool LadderElemPlaceHolder::ShowDialog(LadderContext context) { return false; }

bool LadderElemPlaceHolder::DrawGUI(bool poweredBefore, void *data)
{
	RECT rCursor;
	int SelectedState;
	POINT start, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;
	tWire WireCoords;

	ddg->size.x = 1;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;

	start = ddg->start;
	end.x = start.x - 1;
	end.y = start.y;

	// Desenha o PlaceHolder
	WireCoords = gui.DrawWire(start, end, poweredBefore);
	gui.addConnectionDot(WireCoords.start, poweredBefore, true);

	// Selecao Direita
	rCursor.left   = WireCoords.start.x - 10;
	rCursor.right  = WireCoords.start.x + 10;
	rCursor.top    = WireCoords.start.y - 10;
	rCursor.bottom = WireCoords.start.y + 10;

	if(SelectedState == SELECTED_NONE) {
		tCommandSource source = { nullptr, nullptr, this };
		gui.AddCommand(source, rCursor, CmdSelectRight, nullptr, false, false);
	} else if(isCursorVisible) {
		gui.DrawEllipse(rCursor, gui.getLadderColors().Selection);
	}

	return poweredAfter;
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

bool LadderElemComment::DrawGUI(bool poweredBefore, void *data)
{
	pair<string, string> txt = DrawTXT();
	POINT GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;
	int size = 1 + (max(txt.first.size(), txt.second.size()) * FONT_WIDTH) / GridSize.x;

	ddg->size.x = max(size, ddg->size.x);
	ddg->size.y = txt.second.size() ? 2 : 1;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = ddg->region = gui.DrawElementBar(this, SelectedState, ddg->start.y, ddg->size.y);
	tCommandSource source = { nullptr, nullptr, this };
	gui.AddCommand(source, r, CmdSelectBelow, nullptr, false, false);

	RECT rDialog = r;
	rDialog.left   += GridSize.x;
	rDialog.right  -= GridSize.x;
	rDialog.top    += 10;
	rDialog.bottom -= 10;
	gui.AddCommand(source, rDialog, CmdShowDialog, nullptr, true, false);

	r.left += GridSize.x;
	r.top  += (GridSize.y - FONT_HEIGHT)/2;

	gui.DrawText(txt.first .c_str(), r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	r.top  += GridSize.y;
	gui.DrawText(txt.second.c_str(), r, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	return poweredAfter;
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

	types.push_back((type == eType_Reserved) ? eType_DigInput : type);

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
{
	tCommandSource source = { nullptr, nullptr, this };
	tCmdChangeNameData dataChangeName;

	dataChangeName.reply = false;
	dataChangeName.type  = Diagram->getDetailsIO(prop.idName.first).type;

	ContactCmdChangeName(source, &dataChangeName);

	return dataChangeName.reply;
}

bool LadderElemContact::DrawGUI(bool poweredBefore, void *data)
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

	if(ddg->DontDraw) return poweredAfter;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string sname = Diagram->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	mapDetails detailsIO = Diagram->getDetailsIO(prop.idName.first);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("CONTATO"), true, poweredBefore);
	ddg->region = r;

	// Escreve o nome do contato
	RECT rText = r;
	rText.left += 5;
	rText.top  += 5;
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(name, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, ContactCmdChangeName, nullptr, true, false);

	// Desenha o contato
	COLORREF colorWire = poweredAfter ? colors.Wire : colors.WireOff;

	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 5;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, colorWire);

	start.x  = end.x;
	start.y -= 10;
	end.y   += 10;
	gui.DrawLine(start, end, colorWire);

	start.x += 10;
	end.x   += 10;
	gui.DrawLine(start, end, colorWire);

	rCmd.left    = start.x - 10;
	rCmd.right   = start.x;
	rCmd.top     = start.y;
	rCmd.bottom  = end.y;
	gui.AddCommand(source, rCmd, ContactCmdToggleNegated, nullptr, true, false);

	start.y = r.top + GridSize.y;
	end.x   = r.right;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, colorWire);

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
		gui.DrawLine(start, end, colorWire);
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
		list.items.push_back("N�o");
		list.items.push_back("Sim");

		list.fnc      = ContactCmdExpandedNegated;
		list.selected = prop.negated ? 1 : 0;

		gui.addControlList(this, rExp[1], list);
	}

	return poweredAfter;
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

	types.push_back((type == eType_Reserved) ? eType_DigOutput : type);

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
{
	tCommandSource source = { nullptr, nullptr, this };
	tCmdChangeNameData dataChangeName;

	dataChangeName.reply = false;
	dataChangeName.type  = Diagram->getDetailsIO(prop.idName.first).type;

	CoilCmdChangeName(source, &dataChangeName);

	return dataChangeName.reply;
}

bool LadderElemCoil::DrawGUI(bool poweredBefore, void *data)
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

	if(ddg->DontDraw) return poweredAfter;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string sname = ladder->getNameIO(prop.idName.first);
	const char *name = sname.c_str();

	DoEOL(ddg->start, size, ddg->size, poweredBefore);
	start = ddg->start;

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, start, ddg->size, _("BOBINA"), true, poweredBefore);
	ddg->region = r;

	// Escreve o nome da bobina
	RECT rText    = r;
	rText.left   += 5;
	rText.top    += 5;
	rText.bottom  = rText.top + FONT_HEIGHT;
	gui.DrawText(name, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, CoilCmdChangeName, nullptr, true, false);

	// Desenha a bobina
	COLORREF colorWire = poweredAfter ? colors.Wire : colors.WireOff;

	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 9;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, colorWire);

	start.x  = end.x + 10;
	gui.DrawEllipse(start, 10, 10, colorWire, false);

	if(prop.setOnly || prop.resetOnly) {
		rText.top    = start.y - 10;
		rText.left   = start.x - 10;
		rText.bottom = start.y + 10;
		rText.right  = start.x + 10;

		gui.DrawText(prop.setOnly ? "S" : "R", rText, 0, colorWire, eAlignMode_Center, eAlignMode_Center);
	}

	start.x += 10;
	end.x    = r.right;
	gui.DrawLine(start, end, colorWire);

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
		gui.DrawLine(start, end, colorWire);
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

	return poweredAfter;
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
	int                 which = source.elem->getWhich();
	eType               type  = getTimerTypeIO(which);
	tCmdChangeNameData *dataChangeName = (tCmdChangeNameData *)(data);
	LadderElemTimer    *timer = dynamic_cast<LadderElemTimer *>(source.elem);

	char *s;
    switch(which) 
	{ 
        case ELEM_TON: s = _("Turn-On Delay"); break;
        case ELEM_TOF: s = _("Turn-Off Delay"); break;
        case ELEM_RTO: s = _("Retentive Turn-On Delay"); break;
        default: oops(); break;
    }

	if(dataChangeName != nullptr) {
		dataChangeName->reply = false;
	}

	// Le os dados do Timer para atualizar o delay
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemTimerProp *prop = (LadderElemTimerProp *)timer->getProperties();

	char cname[100];
	sprintf(cname, "%d", prop->delay / 1000);
	string name = cname;

	vector<eType> types;
	types.push_back(eType_General);

	// Passa a posicao do objeto para a janela para que seja exibida centralizada ao elemento
	POINT start, size, GridSize = gui.getGridSize();
	RECT rArea = gui.getElemArea(source.elem);

	RECT rWindow;
	GetWindowRect(DrawWindow, &rWindow);

	start.x = rArea.left   ;
	start.y = rArea.top    ;
	size .x = rArea.right  - rArea.left;
	size .y = rArea.bottom - rArea.top;

	if(ShowVarDialog(s, _("Delay (ms):"), &name, start, size, GridSize, types) &&
		ladder->IsValidNameAndType(0, name.c_str(), eType_Pending , _("Tempo"), VALIDATE_IS_NUMBER, 1, 2147483)) {
			prop->delay = 1000*atoi(name.c_str());

			LadderSettingsGeneral settings = ladder->getSettingsGeneral();
			if(prop->delay % settings.cycleTime) {
				prop->delay = (1 + prop->delay / settings.cycleTime) * settings.cycleTime;
				Error(_("Tempo de ciclo deve ser m�ltiplo de %d! Ser� utilizado %d."),
					settings.cycleTime/1000, prop->delay/1000);
			}

			source.elem->setProperties(ladder->getContext(), prop);

			if(dataChangeName != nullptr) {
				dataChangeName->reply = true;
			}
	} else {
		// Se foi cancelada a alteracao, devemos desalocar as propriedades
		delete prop;
	}
}

bool LadderElemTimer::ShowDialog(LadderContext context)
{
	tCommandSource source = { nullptr, nullptr, this };
	tCmdChangeNameData dataChangeName;

	dataChangeName.reply = false;
	dataChangeName.type  = Diagram->getDetailsIO(prop.idName.first).type;

	TimerCmdChangeTime(source, &dataChangeName);

	return dataChangeName.reply;
}

bool LadderElemTimer::DrawGUI(bool poweredBefore, void *data)
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

	if(ddg->DontDraw) return poweredAfter;

	int which = getWhich();
	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string sname = ladder->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	mapDetails detailsIO = ladder->getDetailsIO(prop.idName.first);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("ATRASO"), true, poweredBefore);
	ddg->region = r;

	// Flag que indica se o desenho deve iniciar com nivel alto ou baixo
	bool doHighStart = (which == ELEM_TOF) ? true : false;

	// Primeiro calculamos as coordenadas do relogio
	RECT rClock = r;
	if(!doHighStart) {
		rClock.top += GridSize.y - 10;
	} else {
		rClock.top += 3;
	}
	rClock.bottom = rClock.top + 20;
	rClock.left += 5;
	rClock.right = rClock.left + (rClock.bottom - rClock.top);

	// Desenha o a linha inicial do timer
	COLORREF colorWire = poweredAfter ? colors.Wire : colors.WireOff;

	start.x = r.left;
	start.y = r.top + (doHighStart ? 10 : GridSize.y);
	end.x   = rClock.right + 10;
	end.y   = start.y;
	gui.DrawLine(start, end, colorWire);

	if(which != ELEM_RTO) {
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
	} else {
		POINT start, size = { 20, 20 };

		start.x = r.left + 3;
		start.y = r.top + 5;
		gui.DrawPictureFromResource(IDB_LADDER_HOURGLASS, start, size);
	}

	// Desenha a linha vertical
	start.x  = end.x;
	if(doHighStart) {
		start.y += GridSize.y - 10;
	} else {
		start.y -= GridSize.y - 10;
	}
	gui.DrawLine(start, end, colorWire);

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
	gui.DrawLine(StartArrow, EndArrow, colorWire);

	EndArrow.x   += 10;
	gui.DrawLine(StartArrow, EndArrow, colorWire);

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
	gui.AddCommand(source, rText, TimerCmdChangeTime, nullptr, true, false);

	// Desenha a linha final, ligando ao final do elemento
	end.x = r.right;
	end.y = start.y;
	gui.DrawLine(start, end, colorWire);

	// Se expandido, desenha o texto com o nome da variavel
	if(ddg->expanded) {
		string txt = _("Nome: ") + Diagram->getNameIO(prop.idName);
		rText.top = r.bottom - GridSize.y + 5;
		rText.bottom = r.bottom;
		rText.left = r.left + 10;
		rText.right = r.right;
		gui.DrawText(txt.c_str(), rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
		gui.AddCommand(source, rText, TimerCmdChangeName, nullptr, true, false);
	}

	return poweredAfter;
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

bool LadderElemRTC::DrawGUI(bool poweredBefore, void *data)
{
	POINT GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	ddg->size.x = 3;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("RTC"), false, poweredBefore);
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

	return poweredAfter;
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

	// Passa a posicao do objeto para a janela para que seja exibida centralizada ao elemento
	POINT start, size, GridSize = gui.getGridSize();
	RECT rArea = gui.getElemArea(source.elem);

	RECT rWindow;
	GetWindowRect(DrawWindow, &rWindow);

	start.x = rArea.left   ;
	start.y = rArea.top    ;
	size .x = rArea.right  - rArea.left;
	size .y = rArea.bottom - rArea.top;

	if(ShowVarDialog(title, desc, &name, start, size, GridSize, types) &&
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
	return false;
}

bool LadderElemCounter::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 5;
		ddg->size.y = 2;
	} else {
		ddg->size.x = 3;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string name = Diagram->getNameIO(prop.idName);

	int which = getWhich();

	char *title;
    switch(which) {
        case ELEM_CTU:  title = _("Count Up"); break;
        case ELEM_CTD:  title = _("Count Down"); break;
        case ELEM_CTC:  title = _("Circular Counter"); break;

        default: oops();
    }

	if(which == ELEM_CTC) {
		DoEOL(ddg->start, size, ddg->size, poweredBefore);
	}

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, ddg->expanded ? title : _("CONTADOR"), true, poweredBefore);
	ddg->region = r;

	tCommandSource source = { nullptr, nullptr, this };

	// Desenha o simbolo conforme o tipo de contador
	char *txt;
	POINT start, end;
	if(which == ELEM_CTC) {
		txt = ddg->expanded ? _("Max value:") : _("Max:");

		POINT start, size = { 38, 32 };

		start.x = r.left + 5;
		start.y = r.top  + 10;
		gui.DrawPictureFromResource(IDB_LADDER_CYCLE, start, size);

		// Carrega a extremidade direita do circulo em end para que os textos sejam exibidos apos o circulo
		end.x = start.x + size.x;

		/*
		// Desenha o circulo
		RECT rCircle = r;
		rCircle.top    += 10;
		rCircle.left   += 10;
		rCircle.bottom  = rCircle.top  + GridSize.y;
		rCircle.right   = rCircle.left + GridSize.y;

		gui.DrawEllipse(rCircle, colorgroup.Border, false);

		// Desenha a seta
		start.x = rCircle.right;
		start.y = rCircle.top + (rCircle.bottom - rCircle.top)/2;
		end = start;
		end.x -= 6;
		end.y -= 5;
		gui.DrawLine(start, end, colorgroup.Border);

		end.x += 10;
		gui.DrawLine(start, end, colorgroup.Border);

		// Carrega a extremidade direita do circulo em end para que os textos sejam exibidos apos o circulo
		end.x = rCircle.right;
		*/
	} else {
		txt = (which == ELEM_CTU ?
			(ddg->expanded ? _("True if >= :") : ">= :") : 
			(ddg->expanded ? _("True if <= :") : "<= :"));

		RECT rSignal;
		end.x = r.left + 10;
		end.y = r.top + (which == ELEM_CTU ? GridSize.y : 0) + 5;

		// Canto superior/inferior esquerdo
		rSignal.top  = end.y;
		rSignal.left = end.x;

		// Desenha os degraus
		int offset = 8;
		unsigned int i;
		for(i = 0; i < 3; i++) {
			start  = end;
			end.y += (which == ELEM_CTU ? -offset : +offset);
			gui.DrawLine(start, end, colorgroup.Border);

			start  = end;
			end.x += offset;
			gui.DrawLine(start, end, colorgroup.Border);
		}

		// Canto superior/inferior ireito
		rSignal.bottom = end.y;
		rSignal.right  = end.x;
		if(rSignal.bottom < rSignal.top) {
			LONG tmp = rSignal.bottom;
			rSignal.bottom = rSignal.top;
			rSignal.top    = tmp;
		}

		// Desenha o sinal
		if(which == ELEM_CTU) {
			rSignal.top  -= 2;
			rSignal.left -= 2;
			// Este � o contador incremental, desenha o sinal no lado superior esquerdo
			gui.DrawText("+", rSignal, 0, colors.Red, eAlignMode_TopLeft    , eAlignMode_TopLeft);
		} else {
			// Este � o contador decremental, desenha o sinal no lado superior direito
			gui.DrawText("-", rSignal, 0, colors.Red, eAlignMode_BottomRight, eAlignMode_TopLeft);
		}
	}

	// Desenha o nome da variavel
	r.top  += 10;
	r.left  = end.x + 5;
	r.right  -= 5;
	gui.DrawText(name.c_str(), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, r, CounterCmdChangeName, nullptr, true, false);

	// Desenha o limite do contador
	char val[1024];
	sprintf(val, "%s %d", txt, prop.max);

	r.top += FONT_HEIGHT + 5;
	gui.DrawText(val, r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, r, CounterCmdChangeValue, nullptr, true, false);

	return poweredAfter;
}

// Classe LadderElemReset
void ResetCmdChangeName(tCommandSource source, void *data)
{
	tCmdChangeNameData *dataChangeName = (tCmdChangeNameData *)(data);
	LadderElemReset *move = dynamic_cast<LadderElemReset *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemResetProp *prop = (LadderElemResetProp *)move->getProperties();

	mapDetails detailsIO = ladder->getDetailsIO(prop->idName.first);

	vector<eType> types;
	types.push_back(eType_TOF);
	types.push_back(eType_TON);
	types.push_back(eType_RTO);
	types.push_back(eType_Counter);

	if(dataChangeName != nullptr) {
		dataChangeName->reply = false;
	}

	bool ret = cmdChangeName(source.elem, 0, prop->idName, detailsIO.type,	types, _("Reset"), _("Name:"));

	if(dataChangeName != nullptr) {
		dataChangeName->reply = ret;
	}

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemReset::ShowDialog(LadderContext context)
{
	tCommandSource source = { nullptr, nullptr, this };
	tCmdChangeNameData dataChangeName;

	dataChangeName.reply = false;

	ResetCmdChangeName(source, &dataChangeName);

	return dataChangeName.reply;
}

bool LadderElemReset::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 2;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string name = Diagram->getNameIO(prop.idName);

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, "ZERAR", false, poweredBefore);
	ddg->region = r;

	tCommandSource source = { nullptr, nullptr, this };

	// Desenha o relogio ou zerar do contador, conforme tipo de variavel
	POINT startPicture, sizePicture = { 20, 20 };
	startPicture.x = r.left + (r.right - r.left - sizePicture.x)/2;
	startPicture.y = r.top + 5;

	if(Diagram->getDetailsIO(prop.idName.first).type == eType_Counter) {
		gui.DrawPictureFromResource(IDB_LADDER_COUNTER, startPicture, sizePicture);
	} else {
		gui.DrawPictureFromResource(IDB_LADDER_CLOCK  , startPicture, sizePicture);
	}

	// Desenha o nome da variavel
	r.top = startPicture.y + sizePicture.y + 5;
	gui.DrawText(name.c_str(), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	// Adiciona 10 a cada extremidade para que o comando a seguir nao sobreponha o comando de selecao do objeto
	r.top    += 10;
	r.left   += 10;
	r.bottom -= 10;
	r.right  -= 10;
	gui.AddCommand(source, r, ResetCmdChangeName, nullptr, true, false);

	return poweredAfter;
}

// Classe LadderElemOneShot
bool LadderElemOneShot::ShowDialog(LadderContext context) { return false; }

bool LadderElemOneShot::DrawGUI(bool poweredBefore, void *data)
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

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors colors = gui.getLadderColors    ();

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, start, ddg->size, ddg->expanded ? (isOSR ? _("BORDA DE SUBIDA") : _("BORDA DE DESCIDA")) : _("BRD"), true, poweredBefore);
	ddg->region = r;

	//                                                   _
	// Faz o desenho do OneShot, algo parecido a isso: _| |_
	// Se for OSR, desenha seta na borda de subida. Se for OSF, na de descida...
	COLORREF colorWire = poweredAfter ? colors.Wire : colors.WireOff;

	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 10;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, colorWire);

	start.x  = end.x;
	start.y -= 20;
	gui.DrawLine(start, end, colorWire);

	if(isOSR) {
		POINT StartArrow = start, EndArrow = end;

		StartArrow.y += 5;
		EndArrow.x   -= 5;
		EndArrow.y    = StartArrow.y + 10;
		gui.DrawLine(StartArrow, EndArrow, colorWire);

		EndArrow.x   += 10;
		gui.DrawLine(StartArrow, EndArrow, colorWire);
	}

	end.x += 20;
	end.y  = start.y;
	gui.DrawLine(start, end, colorWire);

	start.x  = end.x;
	start.y += 20;
	gui.DrawLine(start, end, colorWire);

	if(!isOSR) {
		POINT StartArrow = start, EndArrow = end;

		StartArrow.y -= 5;
		EndArrow.x   -= 5;
		EndArrow.y    = StartArrow.y - 10;
		gui.DrawLine(StartArrow, EndArrow, colorWire);

		EndArrow.x   += 10;
		gui.DrawLine(StartArrow, EndArrow, colorWire);
	}

	end.x = r.right;
	end.y = start.y;
	gui.DrawLine(start, end, colorWire);

	return poweredAfter;
}

// Classe LadderElemCmp
void CmpCmdChangeOp(tCommandSource source, void *data)
{
	int            nOp   = *(int *)data;
	int            which = source.elem->getWhich();
	LadderElemCmp *cmp   = dynamic_cast<LadderElemCmp *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemCmpProp *prop = (LadderElemCmpProp *)cmp->getProperties();

	mapDetails detailsIO = ladder->getDetailsIO(nOp == 0 ? prop->idOp1.first : prop->idOp2.first);

	char *s;
	switch(which) {
		case ELEM_EQU: s = _("If Equals"                  ); break;
		case ELEM_NEQ: s = _("If Not Equals"              ); break;
		case ELEM_GRT: s = _("If Greater Than"            ); break;
		case ELEM_GEQ: s = _("If Greater Than or Equal To"); break;
		case ELEM_LES: s = _("If Less Than"               ); break;
		case ELEM_LEQ: s = _("If Less Than or Equal To"   ); break;
		default: oops();
	}

	cmdChangeName(source.elem, nOp, nOp == 0 ? prop->idOp1 : prop->idOp2, detailsIO.type,
		ladder->getGeneralTypes(), s, _("Name:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemCmp::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemCmp::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 2;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("COMPARAR"), false, poweredBefore);
	ddg->region = r;

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

	int *pInt1 = new int, *pInt2 = new int;
	tCommandSource source = { nullptr, nullptr, this };

	*pInt1 = 0;
	*pInt2 = 1;

	r.left   += 5;
	r.top    += 5;
	r.bottom  = r.top + FONT_HEIGHT;
	gui.DrawText(op1.c_str(), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	gui.AddCommand(source, r, CmpCmdChangeOp, pInt1, true, false);

	r.top    += FONT_HEIGHT;
	r.bottom += FONT_HEIGHT;
	gui.DrawText(s          , r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	r.top    += FONT_HEIGHT;
	r.bottom += FONT_HEIGHT;
	gui.DrawText(op2.c_str(), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	gui.AddCommand(source, r, CmpCmdChangeOp, pInt2, true, false);

	return poweredAfter;
}

// Classe LadderElemMath
void MathCmdChangeName(tCommandSource source, void *data)
{
	int             nVar  = *(int *)data;
	int             which = source.elem->getWhich();
	LadderElemMath *math  = dynamic_cast<LadderElemMath *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemMathProp *prop = (LadderElemMathProp *)math->getProperties();

	pair<unsigned long, int> pin;
	switch(nVar) {
	case 0: pin = prop->idOp1 ; break;
	case 1: pin = prop->idOp2 ; break;
	case 2: pin = prop->idDest; break;
	}

	mapDetails detailsIO = ladder->getDetailsIO(pin.first);

	char *title;
	switch(which) {
		case ELEM_ADD: title = _("Add"     ); break;
		case ELEM_SUB: title = _("Subtract"); break;
		case ELEM_DIV: title = _("Divide"  ); break;
		case ELEM_MUL: title = _("Multiply"); break;
		case ELEM_MOD: title = _("Modulo"  ); break;
		default: oops();
	}

	cmdChangeName(source.elem, nVar, pin, detailsIO.type, ladder->getGeneralTypes(),
		title, nVar == 0 ? _("Operator 1") : (nVar == 1 ? _("Operator 2") : _("Destination:")));

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemMath::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemMath::DrawGUI(bool poweredBefore, void *data)
{
	bool canExpand = true;
	int which = getWhich();
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	char *s, *title;
	switch(which) {
		case ELEM_ADD: s = "+"; title = _("SOMAR"      ); break;
		case ELEM_SUB: s = "-"; title = _("SUBTRAIR"   ); break;
		case ELEM_DIV: s = "" ; title = _("DIVIDIR"    ); break; // Sem sinal! Divisao tem um formato diferente
		case ELEM_MUL: s = "*"; title = _("MULTIPLICAR"); break;
		case ELEM_MOD: s = "" ; title = _("RESTO"      ); break; // Sem sinal! Divisao tem um formato diferente
		default: oops();
	}

	size  = ddg->size;

	if(*s) {
		if(ddg->expanded) {
			ddg->size.x = 3;
			ddg->size.y = 3;
		} else {
			ddg->size.x = 2;
			ddg->size.y = 3;
		}
	} else {
		canExpand = false;
		ddg->size.x = 4;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(which, poweredAfter);

	string op1  = Diagram->getNameIO(prop.idOp1 );
	string op2  = Diagram->getNameIO(prop.idOp2 );
	string dest = Diagram->getNameIO(prop.idDest);

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, title, canExpand, poweredBefore);
	ddg->region = r;

	int *pIntOp1 = new int, *pIntOp2 = new int, *pIntDest = new int;
	tCommandSource source = { nullptr, nullptr, this };

	*pIntOp1  = 0;
	*pIntOp2  = 1;
	*pIntDest = 2;

	if(*s) { // Existe sinal, desenha no modo padrao
		// Desenha o primeiro operador
		r.right -= 5;
		r.top   += (GridSize.y - FONT_HEIGHT)/2;
		gui.DrawText(op1.c_str(), r, 0, colorgroup.Foreground, eAlignMode_BottomRight, eAlignMode_TopLeft);
		gui.AddCommand(source, r, MathCmdChangeName, pIntOp1, true, false);

		// Cria o retangulo para desenhar o sinal, salvando o retangulo que identifica o texto superior
		RECT rSinal = r;

		// Desenha o segundo operador
		r.top  += FONT_HEIGHT + 10;
		gui.DrawText(op2.c_str(), r, 0, colorgroup.Foreground, eAlignMode_BottomRight, eAlignMode_TopLeft);
		gui.AddCommand(source, r, MathCmdChangeName, pIntOp2, true, false);

		// Desenha o sinal centralizado entre a parte de baixo do texto inferior e o topo do texto superior
		rSinal.left   += 5;
		rSinal.bottom  = r.top + FONT_HEIGHT;
		gui.DrawText(s, rSinal, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_Center);

		// Desenha a linha inferior que separa os operadores do resultado
		POINT StartPoint = { r.left + 10, r.top + FONT_HEIGHT + 5 }, EndPoint = { r.right, r.top + FONT_HEIGHT + 5 };
		gui.DrawLine(StartPoint, EndPoint, colorgroup.Foreground);

		// Desenha o destino
		r.top  += FONT_HEIGHT + 10;
		gui.DrawText(dest.c_str(), r, 0, colorgroup.Foreground, eAlignMode_BottomRight, eAlignMode_TopLeft);
		gui.AddCommand(source, r, MathCmdChangeName, pIntDest, true, false);
	} else { // Sem sinal! Divisao, desenha o seu simbolo...
		RECT rText = r;
		rText.top   += (GridSize.y - FONT_HEIGHT)/2;
		rText.left  += 5;
		rText.right -= 5;

		// Desenha as linhas que compoem o simbolo da divisao
		POINT StartPoint = { rText.left + (rText.right - rText.left)/2, rText.top };
		POINT EndPoint   = { StartPoint.x, rText.top + FONT_HEIGHT + 5 };
		gui.DrawLine(StartPoint, EndPoint, colorgroup.Foreground);

		StartPoint.x = rText.right;
		StartPoint.y = EndPoint.y;
		gui.DrawLine(StartPoint, EndPoint, colorgroup.Foreground);

		// Desenha os operadores (Dividendo e Divisor)
		RECT rTextLeft = rText;
		rTextLeft.right = EndPoint.x;
		gui.DrawText(op1.c_str(), rTextLeft, 0, colorgroup.Foreground, eAlignMode_TopLeft    , eAlignMode_TopLeft);
		gui.AddCommand(source, rTextLeft, MathCmdChangeName, pIntOp1, true, false);

		rText.left = rTextLeft.right;
		gui.DrawText(op2.c_str(), rText    , 0, colorgroup.Foreground, eAlignMode_BottomRight, eAlignMode_TopLeft);
		gui.AddCommand(source, rText, MathCmdChangeName, pIntOp2, true, false);

		// Desenha o destino
		enum eAlignMode align = (which == ELEM_MOD) ? eAlignMode_TopLeft : eAlignMode_BottomRight;
		RECT rDest            = (which == ELEM_MOD) ? rTextLeft : rText;
		rDest.top  += FONT_HEIGHT + 10;
		gui.DrawText(dest.c_str(), rDest, 0, colorgroup.Foreground, align, eAlignMode_TopLeft);
		gui.AddCommand(source, rDest, MathCmdChangeName, pIntDest, true, false);
	}

	return poweredAfter;
}

// Classe LadderElemSqrt
void SqrtCmdChangeName(tCommandSource source, void *data)
{
	int             nVar = *(int *)data;
	LadderElemSqrt *sqrt = dynamic_cast<LadderElemSqrt *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemSqrtProp *prop = (LadderElemSqrtProp *)sqrt->getProperties();

	mapDetails detailsIO = ladder->getDetailsIO(nVar == 0 ? prop->idSrc.first : prop->idDest.first);

	cmdChangeName(source.elem, nVar, nVar == 0 ? prop->idSrc : prop->idDest, detailsIO.type,
		ladder->getGeneralTypes(), _("Square Root"), nVar == 0 ? _("Destination:") : _("Source:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemSqrt::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemSqrt::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 4;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string src  = Diagram->getNameIO(prop.idSrc );
	string dest = Diagram->getNameIO(prop.idDest);

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, "RAIZ", false, poweredBefore);
	ddg->region = r;

	int *pInt1 = new int, *pInt2 = new int;
	tCommandSource source = { nullptr, nullptr, this };

	*pInt1 = 0;
	*pInt2 = 1;

	r.top   += (r.bottom - r.top - FONT_HEIGHT)/2;
	r.left  += 2;
	r.right -= 2;

	// Desenha os operadores (Origem e Destino)
	RECT rText = r;
	rText.right -= (r.right - r.left)/2;

	RECT rDest = rText;
	rDest.right -= 7;
	gui.DrawText(dest.c_str(), rDest, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rDest, SqrtCmdChangeName, pInt2, true, false);

	rDest.left  = rDest.right - 2;
	rDest.right = rDest.left + FONT_WIDTH;
	gui.DrawText("=", rDest, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	rText.left  = rText.right + 10;
	rText.right = r.right;
	gui.DrawText(src .c_str(), rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, SqrtCmdChangeName, pInt1, true, false);

	// Desenha as linhas que compoem o simbolo da raiz
	POINT StartPoint = { rText.left - 2, r.top - 5 };
	POINT EndPoint   = { StartPoint.x, r.top + FONT_HEIGHT + 5 };
	gui.DrawLine(StartPoint, EndPoint, colorgroup.Foreground);

	POINT angleStart, angleEnd = EndPoint;
	angleStart.x = EndPoint.x - (EndPoint.y - StartPoint.y)/3;
	angleStart.y = EndPoint.y - (EndPoint.y - StartPoint.y)/2;
	gui.DrawLine(angleStart, angleEnd, colorgroup.Foreground);

	StartPoint.x = r.right;
	EndPoint  .y = StartPoint.y;
	gui.DrawLine(StartPoint, EndPoint, colorgroup.Foreground);

	EndPoint.x  = StartPoint.x;
	EndPoint.y += 5;
	gui.DrawLine(StartPoint, EndPoint, colorgroup.Foreground);

	return poweredAfter;
}

// Classe LadderElemRand
void RandCmdChangeVar(tCommandSource source, void *data)
{
	int             nVar = *(int *)data;
	LadderElemRand *rand = dynamic_cast<LadderElemRand *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemRandProp *prop = (LadderElemRandProp *)rand->getProperties();

	char * field;
	pair<unsigned long, int> pin;
	switch(nVar) {
	case 0:
		pin = prop->idMin;
		field = _("M�nimo:");
		break;
	case 1:
		pin = prop->idVar;
		field = _("Destination:");
		break;
	case 2:
		pin = prop->idMax;
		field = _("M�ximo:");
		break;
	}

	mapDetails detailsIO = ladder->getDetailsIO(pin.first);

	cmdChangeName(source.elem, nVar, pin, detailsIO.type, ladder->getGeneralTypes(), _("Rand"), field);

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemRand::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemRand::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 3;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("ALEATORIO"), false, poweredBefore);
	ddg->region = r;

	int *pInt1 = new int, *pInt2 = new int, *pInt3 = new int;
	tCommandSource source = { nullptr, nullptr, this };

	*pInt1 = 0;
	*pInt2 = 1;
	*pInt3 = 2;

	RECT rGlobe = r;
	RECT rText  = r;

	// Calcula tamanho do globo
	rGlobe.top    +=  5;
	rGlobe.left   += 10;
	rGlobe.right   = rGlobe.left + GridSize.y;
	rGlobe.bottom  = rGlobe.top  + GridSize.y;

	POINT start, end;

	// Desenha o bocal do globo
	RECT rGlobeOut;
	rGlobeOut.top    = rGlobe.top + (rGlobe.bottom - rGlobe.top)/2;
	rGlobeOut.bottom = rGlobe.bottom + 8;
	rGlobeOut.left   = rGlobe.left + (rGlobe.right - rGlobe.left)/2 - 3;
	rGlobeOut.right  = rGlobeOut.left + 6;
	gui.DrawRectangle(rGlobeOut, colorgroup.Border);

	// Limpa o excendete do bocal, preenchendo o circulo do globo
	gui.DrawEllipse(rGlobe, colorgroup.Background, true);

	// Desenha a linha horizontal
	start.x = rGlobe.left;
	start.y = rGlobe.top + (rGlobe.bottom - rGlobe.top)/2;
	end  .x = rGlobe.right + 5; // passa do limite para fazer a alavanca de giro
	end  .y = start.y;
	gui.DrawLine(start, end, colorgroup.Border);

	// Desenha a alavanca
	start.x = end.x;
	start.y = end.y + 5;
	gui.DrawLine(start, end, colorgroup.Border);

	end.x = start.x + 5;
	end.y = start.y;
	gui.DrawLine(start, end, colorgroup.Border);

	rText.left = end.x + 5; // Esse eh o ponto maximo do globo em X, salva como inicio do texto...

	// Desenha a linha vertical
	start.x = rGlobe.left + (rGlobe.right - rGlobe.left)/2;
	start.y = rGlobe.top;
	end  .x = start.x;
	end  .y = rGlobe.bottom;
	gui.DrawLine(start, end, colorgroup.Border);

	// Desenha o globo, sendo o circulo e duas elipses internas, dando o efeito de grade...
	gui.DrawEllipse(rGlobe, colorgroup.Border, false);

	rGlobe.left  += 5;
	rGlobe.right -= 5;
	gui.DrawEllipse(rGlobe, colorgroup.Border, false);

	rGlobe.left  += 5;
	rGlobe.right -= 5;
	gui.DrawEllipse(rGlobe, colorgroup.Border, false);

	/*** Desenha os textos ***/

	// Primeiro desenha o limite inferior
	rText.top    += 5;
	rText.bottom  = rText.top + FONT_HEIGHT;
	gui.DrawText(Diagram->getNameIO(prop.idMin).c_str(), rText, 0, colorgroup.Foreground,
		eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, RandCmdChangeVar, pInt1, true, false);

	// A seguir desenha a variavel de destino
	rText.top    = rText.bottom + 3;
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(Diagram->getNameIO(prop.idVar).c_str(), rText, 0, colorgroup.Foreground,
		eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, RandCmdChangeVar, pInt2, true, false);

	// Por ultimo desenha o limite superior
	rText.top    = rText.bottom + 3;
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(Diagram->getNameIO(prop.idMax).c_str(), rText, 0, colorgroup.Foreground,
		eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, RandCmdChangeVar, pInt3, true, false);

	return poweredAfter;
}

// Classe LadderElemAbs
void AbsCmdChangeName(tCommandSource source, void *data)
{
	int             nVar = *(int *)data;
	LadderElemAbs *abs = dynamic_cast<LadderElemAbs *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemAbsProp *prop = (LadderElemAbsProp *)abs->getProperties();

	mapDetails detailsIO = ladder->getDetailsIO(nVar == 0 ? prop->idSrc.first : prop->idDest.first);

	cmdChangeName(source.elem, nVar, nVar == 0 ? prop->idSrc : prop->idDest, detailsIO.type,
		ladder->getGeneralTypes(), _("Abs"), nVar == 0 ? _("Destination:") : _("Source:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemAbs::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemAbs::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 4;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string src  = Diagram->getNameIO(prop.idSrc );
	string dest = Diagram->getNameIO(prop.idDest);

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, "RAIZ", false, poweredBefore);
	ddg->region = r;

	int *pInt1 = new int, *pInt2 = new int;
	tCommandSource source = { nullptr, nullptr, this };

	*pInt1 = 0;
	*pInt2 = 1;

	r.top   += (r.bottom - r.top - FONT_HEIGHT)/2;
	r.left  += 2;
	r.right -= 2;

	// Desenha os operadores (Origem e Destino)
	RECT rText = r;
	rText.right -= (r.right - r.left)/2;

	RECT rDest = rText;
	rDest.right -= 7;
	gui.DrawText(dest.c_str(), rDest, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rDest, AbsCmdChangeName, pInt2, true, false);

	rDest.left  = rDest.right - 2;
	rDest.right = rDest.left + FONT_WIDTH;
	gui.DrawText("=", rDest, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	rText.left  = rText.right + 5;
	rText.right = r.right - 3;
	gui.DrawText(src .c_str(), rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, AbsCmdChangeName, pInt1, true, false);

	// Desenha as linhas que compoem o simbolo do absoluto
	POINT StartPoint = { rText.left - 2, r.top - 5 };
	POINT EndPoint   = { StartPoint.x, r.top + FONT_HEIGHT + 5 };
	gui.DrawLine(StartPoint, EndPoint, colorgroup.Foreground);

	StartPoint.x = r.right - 3;
	EndPoint  .x = r.right - 3;
	gui.DrawLine(StartPoint, EndPoint, colorgroup.Foreground);

	return poweredAfter;
}

// Classe LadderElemMove
void MoveCmdChangeName(tCommandSource source, void *data)
{
	int             nVar = *(int *)data;
	LadderElemMove *move = dynamic_cast<LadderElemMove *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemMoveProp *prop = (LadderElemMoveProp *)move->getProperties();

	mapDetails detailsIO = ladder->getDetailsIO(nVar == 0 ? prop->idSrc.first : prop->idDest.first);

	cmdChangeName(source.elem, nVar, nVar == 0 ? prop->idSrc : prop->idDest, detailsIO.type,
		ladder->getGeneralTypes(), _("Move"), nVar == 0 ? _("Destination:") : _("Source:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemMove::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemMove::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 4;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string src  = Diagram->getNameIO(prop.idSrc );
	string dest = Diagram->getNameIO(prop.idDest);

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, "MOVER", false, poweredBefore);
	ddg->region = r;

	int *pInt1 = new int, *pInt2 = new int;
	tCommandSource source = { nullptr, nullptr, this };

	*pInt1 = 0;
	*pInt2 = 1;

	r.top   += (r.bottom - r.top - FONT_HEIGHT)/2;
	r.left  += 2;
	r.right -= 2;

	// Desenha os operadores (Origem e Destino)
	RECT rText = r;
	rText.right -= (r.right - r.left)/2;

	RECT rDest = rText;
	rDest.right -= FONT_WIDTH/2 + 2;
	gui.DrawText(dest.c_str(), rDest, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rDest, MoveCmdChangeName, pInt2, true, false);

	rDest.left  = rDest.right + 2;
	rDest.right = rDest.left + FONT_WIDTH;
	gui.DrawText("=", rDest, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

	rText.left  = rDest.right + 2;
	rText.right = r.right;
	gui.DrawText(src .c_str(), rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, MoveCmdChangeName, pInt1, true, false);

	return poweredAfter;
}

// Classe LadderElemOpenShort
bool LadderElemOpenShort::ShowDialog(LadderContext context) { return false; }

bool LadderElemOpenShort::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 2;
		ddg->size.y = 2;
	} else {
		ddg->size.x = 1;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("COMPARAR"), true, poweredBefore);
	ddg->region = r;

	// Desenha a linha esquerda
	COLORREF colorWire = poweredAfter ? colors.Wire : colors.WireOff;

	POINT start, end;
	start.x = r.left;
	start.y = r.top + GridSize.y;
	end.x   = r.left + (r.right - r.left)/2 - 10;
	end.y   = r.top + GridSize.y;
	gui.DrawLine(start, end, colorWire);

	// Desenha o arco central
	start.x = end.x + 20;
	gui.DrawArc(end, start, 10, 10, 180, true, colorWire);

	// Se aberto, desenha um retangulo no centro do arco para criar o efeito de corte
	if(getWhich() == ELEM_OPEN) {
		RECT rCut = { start.x - 3, start.y + 2, end.x + 3, end.y - 12 };
		gui.DrawRectangle(rCut, colorgroup.Background);
	}

	// Desenha a linha direita
	end.x = r.right;
	gui.DrawLine(start, end, colorWire);

	return poweredAfter;
}

// Classe LadderElemSetBit
bool SetBitCmdExpandedChangeMode(LadderElem *elem, unsigned int set)
{
	bool ret = false, new_set = set == 0 ? true : false;
	LadderElemSetBit *setbit = dynamic_cast<LadderElemSetBit *>(elem);
	LadderElemSetBitProp *prop = (LadderElemSetBitProp *)setbit->getProperties();

	if(prop->set != new_set) {
		ladder->CheckpointBegin("Configurar Modo SetBit");

		prop->set = new_set;

		setbit->setProperties(ladder->getContext(), prop);

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

bool LadderElemSetBit::ShowDialog(LadderContext context)
{
	int  NewBit = prop.bit;
	string NewName = Diagram->getNameIO(prop.idName);

	bool changed = ShowVarBitDialog(_("Set Bit"), _("Name:"), &NewName, &NewBit, Diagram->getGeneralTypes());

	if(changed) {
		if(Diagram->IsValidNameAndType(prop.idName.first, NewName, eType_General, _("Nome"), VALIDATE_IS_VAR, 0, 0)) {
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(pin, NewName, eType_General, infoIO_Name)) {
				LadderElemSetBitProp *data = (LadderElemSetBitProp *)getProperties();

				data->bit    = NewBit;
				data->idName = pin;

				setProperties(context, data);
			} else {
				changed = false;
			}
		} else {
			changed = false;
		}
	}

	return changed;
}

bool LadderElemSetBit::DrawGUI(bool poweredBefore, void *data)
{
	POINT GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	if(ddg->expanded) {
		ddg->size.x = 4;
		ddg->size.y = 4;
	} else {
		ddg->size.x = 3;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return poweredAfter;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string sname = ladder->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	mapDetails detailsIO = ladder->getDetailsIO(prop.idName.first);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("ESCREVER BIT"), true, poweredBefore);
	ddg->region = r;

	// Escreve o nome da variavel
	RECT rText  = r;
	rText.left += 5;
	rText.top  += 5;
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(name, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	// Escreve o modo (liga ou desliga) e o bit
	char buf[1024];
	sprintf(buf, _("%s bit %d"), prop.set ? _("Ligar") : _("Desligar"), prop.bit);
	rText.top    = rText.bottom + 5;
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(buf, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	// Se expandido, desenha os itens do modo expandido
	if(ddg->expanded) {
		vector<tExpandedItem> items;
		items.push_back(tExpandedItem(_("Modo"), 1));
		
		vector<RECT> rExp = gui.DrawExpandedItems(colorgroup, r, ddg->size, 2, items);

		// Caixas desenhadas. Agora criamos o conteudo
		tControlList list;

		list.items.push_back(_("Ligar"));
		list.items.push_back(_("Desligar"));

		list.selected = prop.set ? 0 : 1;
		list.fnc      = SetBitCmdExpandedChangeMode;

		gui.addControlList(this, rExp[0], list);
	}

	return poweredAfter;
}

// Classe LadderElemCheckBit
bool CheckBitCmdExpandedChangeMode(LadderElem *elem, unsigned int set)
{
	bool ret = false, new_set = set == 0 ? true : false;
	LadderElemCheckBit *checkbit = dynamic_cast<LadderElemCheckBit *>(elem);
	LadderElemCheckBitProp *prop = (LadderElemCheckBitProp *)checkbit->getProperties();

	if(prop->set != new_set) {
		ladder->CheckpointBegin("Configurar Modo CheckBit");

		prop->set = new_set;

		checkbit->setProperties(ladder->getContext(), prop);

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

bool LadderElemCheckBit::ShowDialog(LadderContext context)
{
	bool NewSet = prop.set;
	int  NewBit = prop.bit;
	string CurrName = Diagram->getNameIO(prop.idName);
	string NewName  = CurrName;

	bool changed = ShowVarBitDialog(_("Check Bit"), _("Name:"), &NewName, &NewBit, Diagram->getGeneralTypes());

	if(changed) {
		// Se variavel sem tipo, usa tipo geral.
		eType name_type = Diagram->getTypeIO(CurrName, NewName, eType_General, true);

		if(Diagram->IsValidNameAndType(prop.idName.first, NewName, name_type, _("Nome"), VALIDATE_IS_VAR, 0, 0)) {
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
		} else {
			changed = false;
		}
	}

	return changed;
}

bool LadderElemCheckBit::DrawGUI(bool poweredBefore, void *data)
{
	POINT GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	if(ddg->expanded) {
		ddg->size.x = 5;
		ddg->size.y = 4;
	} else {
		ddg->size.x = 3;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return poweredAfter;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string sname = ladder->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	mapDetails detailsIO = ladder->getDetailsIO(prop.idName.first);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("CHECAR BIT"), true, poweredBefore);
	ddg->region = r;

	// Escreve o nome da variavel
	RECT rText  = r;
	rText.left += 5;
	rText.top  += 5;
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(name, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	// Escreve o modo (liga ou desliga) e o bit
	char buf[1024];
	sprintf(buf, _("bit %d %s?"), prop.bit, prop.set ? _("Ligado") : _("Desligado"));
	rText.top    = rText.bottom + 5;
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(buf, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	// Se expandido, desenha os itens do modo expandido
	if(ddg->expanded) {
		vector<tExpandedItem> items;
		items.push_back(tExpandedItem(_("Modo"), 1));
		
		vector<RECT> rExp = gui.DrawExpandedItems(colorgroup, r, ddg->size, 2, items);

		// Caixas desenhadas. Agora criamos o conteudo
		tControlList list;

		list.items.push_back(_("Ligado"));
		list.items.push_back(_("Desligado"));

		list.selected = prop.set ? 0 : 1;
		list.fnc      = CheckBitCmdExpandedChangeMode;

		gui.addControlList(this, rExp[0], list);
	}

	return poweredAfter;
}

// Classe LadderElemReadAdc
void ReadAdcCmdChangeName(tCommandSource source, void *data)
{
	tCmdChangeNameData *dataChangeName = (tCmdChangeNameData *)(data);
	int                which = source.elem->getWhich();
	LadderElemReadAdc *readadc = dynamic_cast<LadderElemReadAdc *>(source.elem);

	if(dataChangeName != nullptr) {
		dataChangeName->reply = false;
	}

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemReadAdcProp *prop = (LadderElemReadAdcProp *)readadc->getProperties();

	vector<eType> types;
	types.push_back(eType_ReadADC);

	bool ret = cmdChangeName(source.elem, 0, prop->idName, eType_ReadADC, types, _("Read A/D Converter"), _("Destination:"));
	if(dataChangeName != nullptr) {
		dataChangeName->reply = ret;
	}

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemReadAdc::ShowDialog(LadderContext context)
{
	tCommandSource source = { nullptr, nullptr, this };
	tCmdChangeNameData dataChangeName;

	dataChangeName.reply = false;
	dataChangeName.type  = eType_ReadADC;

	ReadAdcCmdChangeName(source, &dataChangeName);

	return dataChangeName.reply;
}

bool LadderElemReadAdc::DrawGUI(bool poweredBefore, void *data)
{
	POINT start, size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size = ddg->size;

	ddg->size.x = 3;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("LER A/D"), false, poweredBefore);
	ddg->region = r;

	tCommandSource source = { nullptr, nullptr, this };
	mapDetails detailsIO = Diagram->getDetailsIO(prop.idName.first);

	if(detailsIO.pin == 6) { // 6: canal do A/D que recebe o sensor de temperatura
		start.x = r.left + 3;
		start.y = r.top + 5;
		size .y = r.bottom - r.top - 8;
		size .x = size.y;

		gui.DrawPictureFromResource(IDB_LADDER_TERMOMETHER, start, size);
	} else {
		start.x = r.left + 3;
		start.y = r.top + 5;
		size .y = r.bottom - r.top - 8 - FONT_HEIGHT - 5; // FONT_HEIGHT - 5: espaco para o texto com o nome do A/D
		size .x = size.y;
		gui.DrawPictureFromResource(IDB_LADDER_AD, start, size);

		if(detailsIO.pin > 0) {
			char buf[100];
			sprintf(buf, "AD%d", detailsIO.pin);

			RECT rText;
			rText.left = start.x - 5;
			rText.top  = start.y + size.y + 5;
			rText.right = start.x + size.x + 10;
			rText.bottom = rText.top + FONT_HEIGHT;
			gui.DrawText(buf, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_Center);
		}
	}

	// Desenha o nome do I/O
	r.left  += size.x + 5;
	r.right -= 5;
	gui.DrawText(Diagram->getNameIO(prop.idName).c_str(), r, 0, colorgroup.Foreground,
		eAlignMode_Center, eAlignMode_Center);
	gui.AddCommand(source, r, ReadAdcCmdChangeName, nullptr, true, false);

	return poweredAfter;
}

// Classe LadderElemSetDa
void SetDaCmdChangeName(tCommandSource source, void *data)
{
	int                which = source.elem->getWhich();
	LadderElemSetDa *setda = dynamic_cast<LadderElemSetDa *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemSetDaProp *prop = (LadderElemSetDaProp *)setda->getProperties();

	vector<eType> types;
	types.push_back(eType_SetDAC);

	cmdChangeName(source.elem, 0, prop->idName, eType_SetDAC, types, _("Set D/A"), _("Source:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

bool SetDaCmdExpandedChangeMode(LadderElem *elem, unsigned int mode)
{
	bool ret = false;
	LadderElemSetDa *setda = dynamic_cast<LadderElemSetDa *>(elem);
	LadderElemSetDaProp *prop = (LadderElemSetDaProp *)setda->getProperties();

	if(prop->mode != mode && setda->isValidDaValue(ladder->getNameIO(prop->idName), mode)) {
		ladder->CheckpointBegin("Configurar Modo D/A");

		prop->mode = mode;

		setda->setProperties(ladder->getContext(), prop);

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

bool LadderElemSetDa::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemSetDa::DrawGUI(bool poweredBefore, void *data)
{
	POINT start, size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 7;
		ddg->size.y = 6;
	} else {
		ddg->size.x = 2;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return poweredAfter;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();
	mapDetails detailsIO = Diagram->getDetailsIO(prop.idName.first);

	DoEOL(ddg->start, size, ddg->size, poweredBefore);
	start = ddg->start;

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("ESCREVER D/A"), true, poweredBefore);
	ddg->region = r;

	// Escreve o nome do I/O
	RECT rText = r;
	rText.left += 5;
	rText.top  += 5;
	rText.bottom = rText.top + FONT_HEIGHT;
	gui.DrawText(name, rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, rText, SetDaCmdChangeName, nullptr, true, false);

	// Se expandido, desenha os itens do modo expandido
	if(ddg->expanded) {
		vector<tExpandedItem> items;
		items.push_back(tExpandedItem(_("Modo"), 3));
		
		vector<RECT> rExp = gui.DrawExpandedItems(colorgroup, r, ddg->size, 2, items);

		// Caixas desenhadas. Agora criamos o conteudo
		tControlList list;

		list.items.push_back(_("Resolu��o DA (12 bits)[-2048 ~ 2047]"));
		list.items.push_back(_("Resolu��o DA (mV)[-10000 ~ 10000]"));
		list.items.push_back(_("Resolu��o DA (%)[-100 ~ 100]"));

		list.selected = prop.mode;
		list.fnc      = SetDaCmdExpandedChangeMode;

		gui.addControlList(this, rExp[0], list);
	}

	return poweredAfter;
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

bool LadderElemReadEnc::DrawGUI(bool poweredBefore, void *data)
{
	DrawGenericGUI(this, data, poweredBefore, _("LER ENCODER"));

	return poweredAfter;
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

bool LadderElemResetEnc::DrawGUI(bool poweredBefore, void *data)
{
	DrawGenericGUI(this, data, poweredBefore, _("ZERAR ENCODER"));

	return poweredAfter;
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

bool LadderElemMultisetDA::DrawGUI(bool poweredBefore, void *data)
{
	POINT offsetGraph = { 0, 0 };
	POINT sizeGraph   = { 2, 4 };
	POINT start, size, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size = ddg->size;

	if(ddg->expanded) {
		sizeGraph.x = 3;
		sizeGraph.y = 5;

		ddg->size.x = 3;
		ddg->size.y = 7;

		// Calcula a diferenca do tamanho do elemento para o tamanho do grafico
		offsetGraph.x = (ddg->size.x - sizeGraph.x) * GridSize.x;
		offsetGraph.y = (ddg->size.y - sizeGraph.y) * GridSize.y;
	} else {
		ddg->size = sizeGraph;
	}

	if(ddg->DontDraw) return poweredAfter;

	DoEOL(ddg->start, size, ddg->size, poweredBefore);
	start = ddg->start;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string sdesl = ladder->getNameIO(prop.idDesl.first);
	const char *name = sdesl.c_str();
	mapDetails detailsIO = ladder->getDetailsIO(prop.idTime.first);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("RAMPA D/A"), true, poweredBefore);
	ddg->region = r;
	RECT rGraph = r;

	/*** Agora desenhamos o grafico ***/

	// Desconta o offset que temos que considerar
	rGraph.bottom -= offsetGraph.y;
	rGraph.right  -= offsetGraph.x;

	// Primeiro identificamos se a rampa sera de baixo para cima ou de cima para baixo.
	bool isUpDown;
	if((prop.forward && prop.speedup) || (!prop.forward && !prop.speedup)) { // Curva/Rampa de baixo para cima
		isUpDown = false;
	} else { // Curva/Rampa de cima para baixo
		isUpDown = true;
	}

	// Desenha o eixo Y
	start.x = rGraph.left   + 10;
	start.y = rGraph.top    + (isUpDown ? 5 : 10);
	end  .x = start.x;
	end  .y = rGraph.bottom - (isUpDown ? 10 : 5);
	gui.DrawLine(start, end, colors.Black);

	if(isUpDown) {
		start = end;
		end.y = start.y - 10;
	} else {
		end.y = start.y + 10;
	}

	end.x -= 5;
	gui.DrawLine(start, end, colors.Black);

	end.x += 10;
	gui.DrawLine(start, end, colors.Black);

	// Desenha o eixo X
	start.x = rGraph.left   +  5;
	start.y = (isUpDown ? rGraph.top + 10 : rGraph.bottom - 10);
	end  .x = rGraph.right  - 10;
	end  .y = start.y;
	gui.DrawLine(start, end, colors.Black);

	start.x  = end.x - 10;
	start.y -= 5;
	gui.DrawLine(start, end, colors.Black);

	start.y += 10;
	gui.DrawLine(start, end, colors.Black);

	// Devemos descontar a area usada pelos eixos da area do grafico
	rGraph.top    += 10;
	rGraph.left   += 10;
	rGraph.bottom -= 10;
	rGraph.right  -= 10;

	// Desconta tambem o espaco ocupado pelas setas, conforme a posicao em que foi desenhada (acima ou abaixo)
	if(isUpDown) {
		rGraph.bottom -= 12;
	} else {
		rGraph.top    += 12;
	}

	// A seguir desenhamos a linha do gr�fico conforme o tipo escolhido: curva/linear, acelerar/desacelerar, etc.

	// Agora calculamos os pontos inicial/final da rampa
	if(isUpDown) { // Curva/Rampa de cima para baixo
		start.x = rGraph.left;
		start.y = rGraph.top;
		end  .x = rGraph.right;
		end  .y = rGraph.bottom;
	} else { // Curva/Rampa de baixo para cima
		start.x = rGraph.left;
		start.y = rGraph.bottom;
		end  .x = rGraph.right;
		end  .y = rGraph.top;
	}

	if(prop.linear) {
		// Calcula o valor do amortecimento da rampa (ganhos)
		POINT startGain = start, endGain = end, diff = { rGraph.right - rGraph.left, rGraph.bottom - rGraph.top };

		diff.x = (diff.x * prop.gaint) / 100;
		diff.y = (diff.y * prop.gainr) / 100;

		if(diff.x) { // amortecimento ativo
			// Calcula os pontos do amortecimento da rampa conforme os valores previamente calculados (ganhos)
			startGain.x += diff.x;
			endGain  .x -= diff.x;

			if(isUpDown) {
				startGain.y += diff.y;
				endGain  .y -= diff.y;
			} else {
				startGain.y -= diff.y;
				endGain  .y += diff.y;
			}

			// Desenha as linhas que representam o amortecimento
			gui.DrawLine(start  , startGain, colorgroup.Border);
			gui.DrawLine(endGain, end      , colorgroup.Border);
		}

		// Desenha a linha que representa a rampa entre as linhas que representam o amortecimento
		gui.DrawLine(startGain, endGain, colorgroup.Border);
	} else {
		// Calcula o ponto central
		POINT midGraph = { rGraph.left + (rGraph.right - rGraph.left)/2,
			rGraph.top + (rGraph.bottom - rGraph.top)/2 };

		// Calcula o raio da curva. Nao usa o raio exato para suavizar a rampa
		// Se o raio fosse exato, o ponto central estaria a 90 graus, o que signigica uma aceleracao instantanea!
		float radiusX = float(midGraph.x - rGraph.left)*1.5f;
		float radiusY = float(midGraph.y - rGraph.top )*1.5f;

		// Desenha as linhas
		gui.DrawArc(start   , midGraph, radiusX, radiusY, 90, isUpDown , colorgroup.Border);
		gui.DrawArc(midGraph, end     , radiusX, radiusY, 90, !isUpDown, colorgroup.Border);
	}

	// Se em modo expandido, exibe informacoes adicionais como variaveis, numero de passos, etc.
	if(ddg->expanded) {
		char buf[1024];
		RECT rText = { r.left + 5, rGraph.bottom + 30, r.right - 5, 0 };

		sprintf(buf, "%s %s", _("Tempo:"), Diagram->getNameIO(prop.idTime).c_str());

		rText.bottom = rText.top + FONT_HEIGHT;
		gui.DrawText(buf, rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);

		sprintf(buf, "%s %s", _("Valor:"), Diagram->getNameIO(prop.idDesl).c_str());

		rText.top    = rText.bottom + 10;
		rText.bottom = rText.top    + FONT_HEIGHT;
		gui.DrawText(buf, rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
	}

	return poweredAfter;
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

bool LadderElemUSS::DrawGUI(bool poweredBefore, void *data)
{
	DrawGenericGUI(this, data, poweredBefore, (getWhich() == ELEM_READ_USS) ? _("LER USS") : _("ESCREVER USS"));

	return poweredAfter;
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

bool LadderElemModBUS::DrawGUI(bool poweredBefore, void *data)
{
	DrawGenericGUI(this, data, poweredBefore, (getWhich() == ELEM_READ_MODBUS) ? _("LER MB") : _("ESCREVER MB"));

	return poweredAfter;
}

// Classe LadderElemSetPWM
void SetPWMCmdChangeName(tCommandSource source, void *data)
{
	int                which = source.elem->getWhich();
	LadderElemSetPWM *elem = dynamic_cast<LadderElemSetPWM *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemSetPWMProp *prop = (LadderElemSetPWMProp *)elem->getProperties();

	vector<eType> types;
	types.push_back(eType_PWM);
	types.push_back(eType_General);

	cmdChangeName(source.elem, 0, prop->idName, eType_PWM, types, _("Set PWM Duty Cycle"), _("Duty cycle var:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

void SetPWMCmdChangeValue(tCommandSource source, void *data)
{
	int                which = source.elem->getWhich();
	LadderElemSetPWM *elem = dynamic_cast<LadderElemSetPWM *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemSetPWMProp *prop = (LadderElemSetPWMProp *)elem->getProperties();

	char cname[100];
	sprintf(cname, "%d", prop->targetFreq);
	string name = cname;

	vector<eType> types;
	types.push_back(eType_Pending);

	// Passa a posicao do objeto para a janela para que seja exibida centralizada ao elemento
	POINT start, size, GridSize = gui.getGridSize();
	RECT rArea = gui.getElemArea(source.elem);

	RECT rWindow;
	GetWindowRect(DrawWindow, &rWindow);

	start.x = rArea.left   ;
	start.y = rArea.top    ;
	size .x = rArea.right  - rArea.left;
	size .y = rArea.bottom - rArea.top;

	if(ShowVarDialog(_("Set PWM Duty Cycle"), _("Frequency (Hz):"), &name, start, size, GridSize, types) &&
		ladder->IsValidNameAndType(0, name.c_str(), eType_Pending, _("Frequ�ncia"), VALIDATE_IS_NUMBER, 1, 2000)) {
			prop->targetFreq = atoi(name.c_str());

			source.elem->setProperties(ladder->getContext(), prop);
	} else {
		// Se foi cancelada a alteracao, devemos desalocar as propriedades
		delete prop;
	}
}

bool LadderElemSetPWM::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemSetPWM::DrawGUI(bool poweredBefore, void *data)
{
	POINT start, end, size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 3;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string name = Diagram->getNameIO(prop.idName);

	int which = getWhich();

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("PWM"), true, poweredBefore);
	ddg->region = r;

	tCommandSource source = { nullptr, nullptr, this };

	// Desenha o PWM
	int offsetX = GridSize.x / 6, offsetY = GridSize.y / 2;
	unsigned int i;

	end.x = r.left + 10;
	end.y = r.bottom - 5;

	for(i = 0; i < 4; i++) {
		start  = end;
		end.y += (!(i%2) ? -offsetY : +offsetY);
		gui.DrawLine(start, end, colorgroup.Border);

		start  = end;
		end.x += offsetX;
		gui.DrawLine(start, end, colorgroup.Border);
	}

	// Desenha o texto sobre a forma do pulso
	RECT rText = r;

	rText.left  += 10;
	rText.right  = end.x;
	rText.top  += 10;
	gui.DrawText(_("PWM"), rText, 0, colors.Black, eAlignMode_Center, eAlignMode_TopLeft);

	char val[1024];

	// Desenha o nome da variavel
	sprintf(val, "(%%) %s", name.c_str());

	r.top  += 10;
	r.left  = end.x;
	gui.DrawText(val, r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, r, SetPWMCmdChangeName, nullptr, true, false);

	// Desenha o limite do contador
	sprintf(val, "(Hz) %d", prop.targetFreq);

	r.top += FONT_HEIGHT + 5;
	gui.DrawText(val, r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, r, SetPWMCmdChangeValue, nullptr, true, false);

	return poweredAfter;
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

bool LadderElemUART::DrawGUI(bool poweredBefore, void *data)
{
	DrawGenericGUI(this, data, poweredBefore, (getWhich() == ELEM_UART_RECV) ? _("LER SERIAL") : _("ESCREVER SERIAL"));

	return poweredAfter;
}

// Classe LadderElemMasterRelay
bool LadderElemMasterRelay::ShowDialog(LadderContext context) { return false; }

bool LadderElemMasterRelay::DrawGUI(bool poweredBefore, void *data)
{
	static bool isTurnOFF = true;

	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 2;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("CHAVE MESTRE"), false, poweredBefore);
	ddg->region = r;

	// Primeiro desenha o contorno da chave
	r.top    += 10;
	r.left   += 10;
	r.bottom -= 10;
	r.right  -= 10;
	gui.DrawRectangle(r, colorgroup.Border, true, 5);

	// Agora calcula as coordenadas internas e das partes esquerda e direita
	r.top    += 5;
	r.left   += 5;
	r.bottom -= 5;
	r.right  -= 5;

	RECT rLeft = r, rRight = r;

	rLeft.right -= (rLeft.right - rLeft.left)/2;
	rRight.left  = rLeft.right;

	// Desenha o lado ativo
	gui.DrawRectangle(isTurnOFF ? rRight : rLeft, colorgroup.Background, true, 5);

	// Desenha os textos ON/OFF
	gui.DrawText("ON" , rLeft , 0, isTurnOFF ? colorgroup.Background : colors.White, eAlignMode_Center, eAlignMode_Center);
	gui.DrawText("OFF", rRight, 0, isTurnOFF ? colors.White : colorgroup.Background, eAlignMode_Center, eAlignMode_Center);

	return poweredAfter;
}

// Classe LadderElemShiftRegister
void ShiftRegisterCmdChangeName(tCommandSource source, void *data)
{
	LadderElemShiftRegister *elem = dynamic_cast<LadderElemShiftRegister *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemShiftRegisterProp *prop = (LadderElemShiftRegisterProp *)elem->getProperties();

	string CurrName = prop->nameReg;

	// Passa a posicao do objeto para a janela para que seja exibida centralizada ao elemento
	POINT start, size, GridSize = gui.getGridSize();
	RECT rArea = gui.getElemArea(source.elem);

	RECT rWindow;
	GetWindowRect(DrawWindow, &rWindow);

	start.x = rArea.left   ;
	start.y = rArea.top    ;
	size .x = rArea.right  - rArea.left;
	size .y = rArea.bottom - rArea.top;

	if(ShowVarDialog(_("Shift Register"), _("Name:"), &prop->nameReg, start, size, GridSize, ladder->getGeneralTypes())) {
		if(prop->nameReg.size() > 11) prop->nameReg.erase(11);

		if(CurrName != prop->nameReg) {
			elem->setProperties(ladder->getContext(), prop);
			UpdateMainWindowTitleBar();
			return;
		}
	}

	// Se foi cancelada a alteracao, devemos desalocar as propriedades
	delete prop;
}

void ShiftRegisterCmdChangeStages(tCommandSource source, void *data)
{
	LadderElemShiftRegister *elem = dynamic_cast<LadderElemShiftRegister *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemShiftRegisterProp *prop = (LadderElemShiftRegisterProp *)elem->getProperties();

	vector<eType> types;
	types.push_back(eType_General);

	char cname[100];
	sprintf(cname, "%d", prop->stages);
	string name = cname;

	// Passa a posicao do objeto para a janela para que seja exibida centralizada ao elemento
	POINT start, size, GridSize = gui.getGridSize();
	RECT rArea = gui.getElemArea(source.elem);

	RECT rWindow;
	GetWindowRect(DrawWindow, &rWindow);

	start.x = rArea.left   ;
	start.y = rArea.top    ;
	size .x = rArea.right  - rArea.left;
	size .y = rArea.bottom - rArea.top;

	if(ShowVarDialog(_("Shift Register"), _("Stages:"), &name, start, size, GridSize, types) &&
		ladder->IsValidNameAndType(0, name.c_str(), eType_Pending , _("Est�gios"), VALIDATE_IS_NUMBER, 1, 99)) {
			prop->stages = atol(name.c_str());
			source.elem->setProperties(ladder->getContext(), prop);
			UpdateMainWindowTitleBar();
	} else {
		// Se foi cancelada a alteracao, devemos desalocar as propriedades
		delete prop;
	}
}

bool LadderElemShiftRegister::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemShiftRegister::DrawGUI(bool poweredBefore, void *data)
{
	int items, limit = 10;
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	if(prop.stages > limit) {
		items = limit;
	} else {
		items = prop.stages;
	}

	size  = ddg->size;

	if(ddg->expanded) {
		ddg->size.x = 3;
		ddg->size.y = 2 + items + 1;
	} else {
		ddg->size.x = 2;
		ddg->size.y = 2;
	}

	if(ddg->DontDraw) return poweredAfter;

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("LISTA"), true, poweredBefore);
	ddg->region = r;

	tCommandSource source = { nullptr, nullptr, this };

	r.left   += 5;
	r.right  += 5;
	r.top    += 10;
	r.bottom  = r.top + FONT_HEIGHT;
	gui.DrawText(prop.nameReg.c_str(), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, r, ShiftRegisterCmdChangeName, nullptr, true, false);

	char buf[1024];
	sprintf(buf, "%s %d", _("Stages:"), prop.stages);

	r.top    += FONT_HEIGHT + 5;
	r.bottom += FONT_HEIGHT + 5;
	gui.DrawText(buf, r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, r, ShiftRegisterCmdChangeStages, nullptr, true, false);

	if(ddg->expanded) {
		string s;
		RECT rText = r, rLastText;
		int i;

		rText.left   += 10;
		rText.right  -= 10;
		rText.top     = r.bottom + 5;
		rText.bottom  = rText.top + FONT_HEIGHT;

		gui.DrawText(_("Registradores:"), rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
		rText.top    += FONT_HEIGHT + 5; // Cria um espacamento antes do inicio dos registradores
		rText.bottom += FONT_HEIGHT + 5; // Cria um espacamento antes do inicio dos registradores

		POINT start, end;

		for(i = 0; i < items; i++) {
			if(prop.stages > limit &&
				(i >= (limit/2 - 1)) &&
				(i <=  limit/2)) { // Estamos no meio do elemento e existem muitos items, exibir ". . ."
					s = ". . .";
			} else if(prop.stages > limit && i > limit/2) {
				s = Diagram->getNameIO(prop.vectorIdRegs[prop.stages - limit + i]);
			} else {
				s = Diagram->getNameIO(prop.vectorIdRegs[i]);
			}

			gui.DrawText(s.c_str(), rText, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

			rLastText     = rText;
			rText.top     = rText.top + GridSize.y;
			rText.bottom  = rText.top + FONT_HEIGHT;

			start.x = rText.left  + (rText.right - rText.left)/2;
			end  .x = start.x;

			start.y = rLastText.bottom + 2;
			end  .y = rText    .top    - 2;

			gui.DrawLine(start, end, colorgroup.Foreground);

			start.x -= 5;
			start.y  = end.y - 5;
			gui.DrawLine(start, end, colorgroup.Foreground);

			start.x += 10;
			gui.DrawLine(start, end, colorgroup.Foreground);
		}

		// Terminou de exibir os registradores.
		// Devemos agora exibir o desenho que indica o descarte do ultimo elemento
		start.y = end.y   +  5;
		end  .y = start.y + 10;
		end  .x = start.x - 10;
		gui.DrawLine(start, end, colors.Red);

		POINT tmp = start;

		start.x = end.x;
		end  .x = tmp.x;
		gui.DrawLine(start, end, colors.Red);
	}

	return poweredAfter;
}

// Classe LadderElemLUT
void LUTCmdChangeName(tCommandSource source, void *data)
{
	int             nVar = *(int *)data;
	LadderElemLUT *lut = dynamic_cast<LadderElemLUT *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemLUTProp *prop = (LadderElemLUTProp *)lut->getProperties();

	mapDetails detailsIO = ladder->getDetailsIO(nVar == 0 ? prop->idIndex.first : prop->idDest.first);

	cmdChangeName(source.elem, nVar, nVar == 0 ? prop->idIndex : prop->idDest, detailsIO.type,
		ladder->getGeneralTypes(), _("Look-Up Table"), nVar == 0 ? _("Index:") : _("Dest:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

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

bool LadderElemLUT::DrawGUI(bool poweredBefore, void *data)
{
	POINT size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 3;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("TABELA DE BUSCA"), false, poweredBefore);
	ddg->region = r;

	int *pInt1 = new int, *pInt2 = new int;
	tCommandSource source = { nullptr, nullptr, this };

	*pInt1 = 0;
	*pInt2 = 1;

	char buf[1024];

	sprintf(buf, "%s %s", _("Destination:"), Diagram->getNameIO(prop.idDest).c_str());

	r.left   += 3;
	r.top    += 5;
	r.bottom  = r.top + FONT_HEIGHT;
	gui.DrawText(buf, r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, r, LUTCmdChangeName, pInt2, true, false);

	sprintf(buf, "%s %s", _("Index:"), Diagram->getNameIO(prop.idIndex).c_str());

	r.top    += FONT_HEIGHT;
	r.bottom += FONT_HEIGHT;
	gui.DrawText(buf, r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);
	gui.AddCommand(source, r, LUTCmdChangeName, pInt1, true, false);

	sprintf(buf, "%s %d", _("Count:"), prop.count);

	r.top    += FONT_HEIGHT;
	r.bottom += FONT_HEIGHT;
	gui.DrawText(buf, r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

	return poweredAfter;
}

// Classe LadderElemPiecewise
void PiecewiseCmdChangeName(tCommandSource source, void *data)
{
	int             nVar = *(int *)data;
	LadderElemPiecewise *piecewise = dynamic_cast<LadderElemPiecewise *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemPiecewiseProp *prop = (LadderElemPiecewiseProp *)piecewise->getProperties();

	mapDetails detailsIO = ladder->getDetailsIO(nVar == 0 ? prop->idIndex.first : prop->idDest.first);

	cmdChangeName(source.elem, nVar, nVar == 0 ? prop->idIndex : prop->idDest, detailsIO.type,
		ladder->getGeneralTypes(), _("Piecewise Linear Table"), nVar == 0 ? _("Index:") : _("Destination:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

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

bool LadderElemPiecewise::DrawGUI(bool poweredBefore, void *data)
{
	POINT offsetGraph = { 0, 0 };
	POINT sizeGraph   = { 2, 4 };
	POINT start, size, end, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size = ddg->size;

	if(ddg->expanded) {
		sizeGraph.x = 3;
		sizeGraph.y = 5;

		ddg->size.x = 3;
		ddg->size.y = 7;

		// Calcula a diferenca do tamanho do elemento para o tamanho do grafico
		offsetGraph.x = (ddg->size.x - sizeGraph.x) * GridSize.x;
		offsetGraph.y = (ddg->size.y - sizeGraph.y) * GridSize.y;
	} else {
		ddg->size = sizeGraph;
	}

	if(ddg->DontDraw) return poweredAfter;

	DoEOL(ddg->start, size, ddg->size, poweredBefore);
	start = ddg->start;

	tCommandSource source = { nullptr, nullptr, this };

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, _("LINEARIZACAO"), true, poweredBefore);
	ddg->region = r;
	RECT rGraph = r;

	/*** Agora desenhamos o grafico ***/

	if(prop.count < 2) { // Sem pontos, nao desenha o grafico!
		// Exibir apenas um texto informando "Dados insuficientes para exibicao de grafico"
		r.top += GridSize.y / 2;
		gui.DrawText(_("Dados"        ), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

		r.top += FONT_HEIGHT + 5;
		gui.DrawText(_("insuficientes"), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

		r.top += FONT_HEIGHT + 5;
		gui.DrawText(_("para exibi��o"), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

		r.top += FONT_HEIGHT + 5;
		gui.DrawText(_("do gr�fico"   ), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_TopLeft);

		return poweredAfter;
	}

	// Desconta o offset que temos que considerar
	rGraph.bottom -= offsetGraph.y;
	rGraph.right  -= offsetGraph.x;

	// Primeiro calculamos a posicao do eixo horizontal no eixo vertical
	bool isUpDown = false;

	int i;
	unsigned int ySize, xSize = abs(prop.vals[(prop.count - 1) * 2] - prop.vals[0]);
	int yMax = prop.vals[1], yMin = prop.vals[1];

	// Identifica os extremos em X e Y
	for(i = 1; i < prop.count*2; i += 2) {
		if(prop.vals[i] > yMax) {
			yMax = prop.vals[i];
		}

		if(prop.vals[i] < yMin) {
			yMin = prop.vals[i];
		}
	}

	ySize = abs(yMax - yMin);

	if(yMax < 0) { // A posicao mais alta em Y ainda esta abaixo de zero. O eixo X deve ser desenhado acima
		isUpDown = true;
	}

	// Devemos descontar a area usada pelos eixos da area do grafico
	rGraph.top    += 10;
	rGraph.left   += 10;
	rGraph.bottom -= 10;
	rGraph.right  -= 10;

	// Desconta tambem o espaco ocupado pelas setas, conforme a posicao em que foi desenhada (acima ou abaixo)
	if(isUpDown) {
		rGraph.bottom -= 12;
	} else {
		rGraph.top    += 12;
	}

	// Agora calculamos os fatores de multiplica��o dos pontos nos eixos X e Y para que caibam na area do grafico
	float scaleX = float(rGraph.right  - rGraph.left)/float(xSize);
	float scaleY = float(rGraph.bottom - rGraph.top )/float(ySize);

	// Desenha o eixo X
	start.x = rGraph.left  -  5;
	start.y = rGraph.top   + (LONG)((ySize + yMin) * scaleY);
	end  .x = rGraph.right + 5;
	end  .y = start.y;

	if(start.y >= rGraph.top && start.y <= rGraph.bottom) {
		gui.DrawLine(start, end, colors.Black);

		start.x  = end.x - 10;
		start.y -= 5;
		gui.DrawLine(start, end, colors.Black);

		start.y += 10;
		gui.DrawLine(start, end, colors.Black);
	}

	// Desenha o eixo Y
	start.x = rGraph.left - (LONG)(prop.vals[0] * scaleX);
	start.y = rGraph.top  - (isUpDown ? 5 : 15);
	end  .x = start.x;
	end  .y = rGraph.bottom + (isUpDown ? 15 : 5);

	if(start.x >= rGraph.left && start.x <= rGraph.right) {
		gui.DrawLine(start, end, colors.Black);

		if(isUpDown) {
			start = end;
			end.y = start.y - 10;
		} else {
			end.y = start.y + 10;
		}

		end.x -= 5;
		gui.DrawLine(start, end, colors.Black);

		end.x += 10;
		gui.DrawLine(start, end, colors.Black);
	}

	// A seguir desenhamos as linhas que representam os pontos cadastrados
	for(i = 0; i < prop.count*2; i += 2) {
		start = end;
		end.x = rGraph.left + (LONG)(         (prop.vals[i    ] - prop.vals[0])  * scaleX);
		end.y = rGraph.top  + (LONG)((ySize - (prop.vals[i + 1] - yMin        )) * scaleY);
		if(i == 0) {
			// Este eh o primeiro ponto sendo calculado. Ele sera o ponto inicial do grafico.
			// Nao devemos desenhar a linha ainda!
			continue;
		}

		gui.DrawLine(start, end, colorgroup.Border);
	}

	// Se em modo expandido, exibe informacoes adicionais como variaveis, numero de passos, etc.
	if(ddg->expanded) {
		char buf[1024];
		RECT rText = { r.left + 3, rGraph.bottom + 20, r.right - 3, 0 };

		int *pInt1 = new int, *pInt2 = new int;

		*pInt1 = 0;
		*pInt2 = 1;

		sprintf(buf, _("Eixo X: %s"), Diagram->getNameIO(prop.idIndex).c_str());

		rText.bottom = rText.top + FONT_HEIGHT;
		gui.DrawText(buf, rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
		gui.AddCommand(source, rText, PiecewiseCmdChangeName, pInt1, true, false);

		sprintf(buf, _("Eixo Y: %s"), Diagram->getNameIO(prop.idDest).c_str());

		rText.top    = rText.bottom + 5;
		rText.bottom = rText.top    + FONT_HEIGHT;
		gui.DrawText(buf, rText, 0, colorgroup.Foreground, eAlignMode_TopLeft, eAlignMode_TopLeft);
		gui.AddCommand(source, rText, PiecewiseCmdChangeName, pInt2, true, false);
	}

	return poweredAfter;
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

bool LadderElemFmtString::DrawGUI(bool poweredBefore, void *data)
{
	DrawGenericGUI(this, data, poweredBefore, (getWhich() == ELEM_READ_FORMATTED_STRING) ? _("LER STRING") : _("ESCREVER STRING"));

	return poweredAfter;
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

bool LadderElemYaskawa::DrawGUI(bool poweredBefore, void *data)
{
	DrawGenericGUI(this, data, poweredBefore, (getWhich() == ELEM_READ_SERVO_YASKAWA) ? _("LER NS-600") : _("ESCREVER NS-600"));

	return poweredAfter;
}

// Classe LadderElemPersist
void PersistCmdChangeVar(tCommandSource source, void *data)
{
	int                which = source.elem->getWhich();
	LadderElemPersist *elem = dynamic_cast<LadderElemPersist *>(source.elem);

	// Le os dados do I/O para ter a referencia do I/O atual
	// Para isso precisamos carregar as propriedades do elemento, precisamos descarregar depois do uso...
	LadderElemPersistProp *prop = (LadderElemPersistProp *)elem->getProperties();

	cmdChangeName(source.elem, 0, prop->idVar, eType_General, ladder->getGeneralTypes(),
		_("Make Persistent"), _("Variable:"));

	// Aqui desalocamos as propriedades
	delete prop;
}

bool LadderElemPersist::ShowDialog(LadderContext context)
{
	return false;
}

bool LadderElemPersist::DrawGUI(bool poweredBefore, void *data)
{
	POINT start, end, size, GridSize = gui.getGridSize();
	tDataDrawGUI *ddg = (tDataDrawGUI*)data;

	size  = ddg->size;

	ddg->size.x = 3;
	ddg->size.y = 2;

	if(ddg->DontDraw) return poweredAfter;

	tLadderColors     colors     = gui.getLadderColors    ();
	tLadderColorGroup colorgroup = gui.getLadderColorGroup(getWhich(), poweredAfter);

	string name = Diagram->getNameIO(prop.idVar);

	int which = getWhich();

	DoEOL(ddg->start, size, ddg->size, poweredBefore);

	int SelectedState = ddg->context->SelectedElem == this ? ddg->context->SelectedState : SELECTED_NONE;
	RECT r = gui.DrawElementBox(this, SelectedState, ddg->start, ddg->size, "PERSISTENTE", false, poweredBefore);
	ddg->region = r;

	tCommandSource source = { nullptr, nullptr, this };

	// Desenha a imagem
	RECT rPenDrive = r;

	// Desenha o corpo do pendrive
	rPenDrive.left   += 10;
	rPenDrive.right   = rPenDrive.left + 25;
	rPenDrive.top    += 17;
	rPenDrive.bottom -= 5;

	start.x = rPenDrive.left;
	start.y = rPenDrive.top;
	end  .x = start.x;
	end  .y = rPenDrive.bottom - 5;
	gui.DrawLine(start, end, colorgroup.Border);

	start.x = rPenDrive.left + 5;
	start.y = rPenDrive.bottom;
	gui.DrawArc(start, end, 5.0f, 5.0f, 90, true, colorgroup.Border);

	end  .x = rPenDrive.right - 5;
	end  .y = start.y;
	gui.DrawLine(start, end, colorgroup.Border);

	start.x = rPenDrive.right;
	start.y = rPenDrive.bottom - 5;
	gui.DrawArc(start, end, 5.0f, 5.0f, 90, true, colorgroup.Border);

	end  .x = rPenDrive.right;
	end  .y = rPenDrive.top;
	gui.DrawLine(start, end, colorgroup.Border);

	start.x = rPenDrive.left;
	start.y = rPenDrive.top;
	gui.DrawLine(start, end, colorgroup.Border);

	start.y = rPenDrive.bottom - 5; // Salva o valor em y da parte inferior do pendrive para desenhar a linha

	// Desenha o conector do pendrive
	rPenDrive.left   +=  5;
	rPenDrive.right  -=  5;
	rPenDrive.bottom  =  rPenDrive.top;
	rPenDrive.top    -= 10;
	gui.DrawRectangle(rPenDrive, colorgroup.Border, false); 

	// Desenha a linha inferior
	start.x = rPenDrive.left;
	end  .x = rPenDrive.right;
	end  .y = start.y;
	gui.DrawLine(start, end, colorgroup.Border);

	// Desenha as duas linhas do conector
	start.x = rPenDrive.left + (rPenDrive.right - rPenDrive.left)/2 - 3;
	start.y = rPenDrive.top + 3;
	end  .x = start.x;
	end  .y = start.y + 3;
	gui.DrawLine(start, end, colorgroup.Border);

	start.x += 6;
	end  .x  = start.x;
	gui.DrawLine(start, end, colorgroup.Border);

	// Desenha o nome da variavel
	r.left  += GridSize.x + 5;
	r.right -= 5;
	gui.DrawText(name.c_str(), r, 0, colorgroup.Foreground, eAlignMode_Center, eAlignMode_Center);
	gui.AddCommand(source, r, PersistCmdChangeVar, nullptr, true, false);

	return poweredAfter;
}

// Classe LadderElemX
bool LadderElemX::ShowDialog(LadderContext context) { return false; }

bool LadderElemX::DrawGUI(bool poweredBefore, void *data)
{
	DrawGenericGUI(this, data, poweredBefore);

	return poweredAfter;
}

//Classe LadderCircuit
bool LadderCircuit::DrawGUI(bool poweredBefore, void *data)
{
	int max = 0;
	bool FirstIsParallelStart = false;
	POINT StartWire, EndWire, PreviousWire, elemStart;
	bool isFirstElement = true, HasEOL = false;
	bool poweredThis, poweredAfter = isSeries ? poweredBefore : false;
	vector<Subckt>::iterator it;
	vector<POINT> EndPoints;

	// Vetor para armazenar o estado de ligado/desligado dos elementos no paralelo
	// first: indica se a linha horizontal esta energizada
	// second: indica se a linha vertical esta energizada
	vector< pair<bool, bool> > vectorEndParallelPowered;

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
			poweredThis = it->elem->DrawGUI(isSeries ? poweredAfter : poweredBefore, &ElemDDG);

			if(ElemDDG.DontDraw == false) {
				gui.registerElementArea(it->elem, ElemDDG.start, ElemDDG.size);
			}

			if(it->elem == ddg->context->SelectedElem) {
				ddg->regionSelected = gui.getRECT(ElemDDG.start, ElemDDG.size);
			}

			ElemDDG.start = elemStart;
		} else {
			HasEOL |= it->subckt->HasEOL();
			poweredThis = it->subckt->DrawGUI(isSeries ? poweredAfter : poweredBefore, &ElemDDG);
			if(!IsRegionZero(ElemDDG.regionSelected)) {
				ddg->regionSelected = ElemDDG.regionSelected;
			}
		}

		if(isSeries) {
			poweredAfter = poweredThis;

			ElemDDG.start.x += ElemDDG.size.x + 1;
			if(ElemDDG.size.y > max) {
				max = ElemDDG.size.y;
			}
		} else {
			pair<bool, bool> poweredElem = pair<bool, bool>(poweredThis, poweredThis);

			if(poweredThis) {
				poweredAfter = true;

				unsigned int i;
				for(i = 0; i < vectorEndParallelPowered.size(); i++) {
					vectorEndParallelPowered[i].second = true;
				}
			}

			vectorEndParallelPowered.push_back(poweredElem);

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
					gui.DrawWire(StartWire, EndWire, poweredBefore);
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
			isFirstElement = true;

			// Agora desenhamos as linhas de ligacao do final do paralelo
			unsigned int i;
			for(i = 0; i < EndPoints.size(); i++) {
				// Desenha a linha horizontal
				StartWire = EndWire = EndPoints[i];
				EndWire.x = ddg->start.x + max;
				if(StartWire.x < EndWire.x && (i != 0 || !FirstIsParallelStart)) {
					gui.DrawWire(StartWire, EndWire, vectorEndParallelPowered[i].first);
				}

				// Em seguida, a linha vertical
				if(isFirstElement) {
					isFirstElement = false;
				} else {
					EndWire.x++; // Avanca 1 posicao no grid pois a linha vertical se desenha a esquerda
					StartWire.x = EndWire.x;
					StartWire.y = PreviousWire.y;
					gui.DrawWire(StartWire, EndWire, vectorEndParallelPowered[i].second);
				}

				PreviousWire.y = EndWire.y;
			}
		}

		ddg->size.x = max;
		ddg->size.y = ElemDDG.start.y - ddg->start.y - 1;
	}

	return poweredAfter;
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

	if(context.isLoadingFile) return; // Durante o carregamento de um arquivo nao devemos desenhar a tela...

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
		rungs[i]->rung->DrawGUI(rungs[i]->isPowered, &RungDDG);

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
		rungs[i]->rung->DrawGUI(rungs[i]->isPowered, &RungDDG);

		RECT r = gui.getRECT(RungDDG.start, RungDDG.size);
		r.right = r.left;
		r.left  = 0;

		// Adicionando comando para o breakpoint.
		// Passa a flag do modo atual para indicar se o comando deve funcionar em modo de simula��o ou modo normal
		// Dessa forma o comando sempre estar� ativo pois ao entrar/sair do modo de simula��o a tela � redesenhada
		tCommandSource source = { this, nullptr, nullptr };
		int *rung = new int;

		*rung = i;
		gui.AddCommand(source, r, cmdToggleBreakpoint, rung, true, context.inSimulationMode);

		// Agora desenha o numero da linha
		r.left  = 5;
		sprintf(num, "%3d", i + 1);
		gui.DrawText(num, r, 0, colors.Foreground, eAlignMode_TopLeft, eAlignMode_Center);

		// Agora desenha o breakpoint, se ativo
		if(rungs[i]->hasBreakpoint) {
			r.top     = r.top + (r.bottom - r.top)/2 + FONT_HEIGHT + 5;
			r.bottom  = r.top + 14;
			r.left    = Grid1x1.x/2 - 7;
			r.right   = Grid1x1.x/2 + 7;
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
	gui.DrawRectangle(rBus, colors.BusOff);

	gui.DrawEnd();
}

bool LadderDiagram::IsSelectedVisible(void)
{
	if(context.SelectedElem == nullptr) return false;

	RECT r = gui.getElemArea(context.SelectedElem);
	if(!IsRegionZero(r)) { // Selecionado encontrado.
		RECT rWindow = { 0, 0, 0, 0 };
		GetClientRect(DrawWindow, &rWindow);
		if(IsRegionZero(rWindow)) {
			// Nao foi possivel receber as coordenadas da janela. Talvez ainda nao
			// foi criada ou esta minimizada.
			// De qualquer forma, considere que o objeto esteja visivel...
			return true;
		}

		POINT Grid1x1 = gui.getGridSize();
		POINT TopLeft = { r.left - ScrollXOffset, r.top - ScrollYOffset*Grid1x1.y };
		POINT BottomRight = { r.right - ScrollXOffset, r.bottom - ScrollYOffset*Grid1x1.y };

		return PointInsideRegion(TopLeft, rWindow) && PointInsideRegion(BottomRight, rWindow);
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
	// Nao atualiza a lista de I/Os enquanto carregando o arquivo
	if(diagram->getContext().isLoadingFile == true) return;

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
