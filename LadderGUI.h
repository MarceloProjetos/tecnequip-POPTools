#ifndef LADDER_GUI
#define LADDER_GUI

#include "poptools.h"

// A linha a seguir, quando habilitada, ativa codigo que exibe diversas informacoes de depuracao
// na tela como posicao do mouse. coordenada do ultimo clique / duplo-clique, ultimo erro do D2D, etc.
//#define SHOW_DEBUG_INFO

#include "EngineGUI.h"
#include "EngineRenderD2D.h"

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
	bool  isFullRedraw;

	bool  hasBreakpoint;

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

	// Cores utilizadas para desenhar caixas de dialogo
	COLORREF DialogBorder;
	COLORREF DialogBorderText;
	COLORREF DialogBackground;
	COLORREF DialogBackgroundLight;
	COLORREF DialogBackgroundDark;
	COLORREF DialogBackgroundText;
	COLORREF DialogDefButtonUp;
	COLORREF DialogDefButtonDown;
	COLORREF DialogDefButtonBorder;
	COLORREF DialogDefButtonText;
	COLORREF DialogButtonUp;
	COLORREF DialogButtonDown;
	COLORREF DialogButtonBorder;
	COLORREF DialogButtonText;

	// Cores com Transparencia
	COLORREF BlackHalfTransparency;
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
// ao clicar no nome da variável em uma caixa de elemento
typedef struct {
	eType type;
	bool  reply;
} tCmdChangeNameData;

// Estrutura e mapa utilizados para armazenar dados especificos e nao-volateis de diagramas
// Nao podemos usar as variaveis estaticas diretamente pois, como sao varios diagramas ladder,
// haveria conflito ao mudar de um diagrama para outro.
typedef struct {
	POINT                SizeMax;
	POINT                ScrollOffset;
	vector<tDataDrawGUI> rungData;
	bool                 needSelectRung;
} tDiagramData;

// Definicao de estrutura que contem os dados que representam uma janela de dialogo
typedef struct  {
	eDialogType  type;
	bool         hasCancel;
	eReply      *reply;
	string       title;
	string       message;
} tDialogData;

/*** Fim das estruturas auxiliares ***/

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

	int  HeaderHeight;
	bool needFullRedraw;

	array<unsigned int, 3>    InterfaceColors;

	typedef struct {
		RECT   r;
		bool (*fnc)(tCommandSource, void *);
		void  *data;
		bool   isSimMode;
	} tCommandItem;
	typedef pair<tCommandSource, tCommandItem>  tCommandListItem;
	typedef vector<tCommandListItem>            tCommandsList;

	// Estruturas e variaveis relacionadas com janela de dialogo
	typedef struct {
		eReply buttonReply;
		string buttonText;
	} tButtonData;

	bool                isDialogActive;
	int                 buttonActive;
	tDialogData         DialogData;
	vector<tButtonData> vectorButtonData;

	tCommandsList CommandsSingleClick;
	tCommandsList CommandsDoubleClick;

	void ClearCommandLists(void);

	bool MouseOver (RECT region);

	void PaintScrollAndSplitter(void);

	// Variaveis que armazenam as cores utilizadas no diagrama ladder
	array<tLadderColors, 2>     LadderColors;
	map<int, tLadderColorGroup> LadderColorGroups;
	vector<tLadderColorGroup>   LadderColorSimulation;

	// Gradientes utilizados na caixa de dialogo
	unsigned int gradDialogDefButtonNormal;
	unsigned int gradDialogDefButtonOver;
	unsigned int gradDialogButtonNormal;
	unsigned int gradDialogButtonOver;

#ifdef SHOW_DEBUG_INFO
	POINT   mouse_last_click;
	POINT   mouse_last_doubleclick;
	HRESULT d2d_last_error;
#endif

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

	void DrawInit  (void);
	void DrawStart (int OffsetX, int OffsetY);
	void DrawEnd   (void);
	void NeedRedraw(bool isRedrawNeeded);
	bool getNeedFullRedraw(void) { return needFullRedraw; }

	void registerElementArea(LadderElem *elem, POINT start, POINT size);

	void DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, bool poweredBefore);
	RECT DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, POINT GridSize, string ElemName, bool ShowExpand, bool poweredBefore);
	void DrawElementBox(LadderElem *elem, int SelectedState, POINT StartTopLeft, pair<string, string> txt, bool poweredBefore);

	RECT DrawElementBar(LadderElem *elem, int SelectedState, int StartGridY, int GridHeight);

	void DrawDialogBox(void);

	vector<RECT> DrawExpandedItems(tLadderColorGroup cg, RECT r, POINT GridSize, unsigned int GridOffset, vector<tExpandedItem> items);

	// Funcoes para adicionar controles aos itens expandidos
	void addControlList(LadderElem *elem, RECT r, tControlList list);

	eReply      ShowDialog    (eDialogType type, bool hasCancel, const char *title, const char *message);
	tDialogData getDialogData (void);
	bool IsDialogActive       (void);
	void setDialogActiveButton(bool isNext);
	void DialogConfirm        (void);
	void DialogCancel         (void);

	tWire DrawWire(POINT StartPoint, POINT EndPoint, bool poweredBefore);
	void  addConnectionDot(POINT p, bool isPowered, bool isForced);

	void        MouseClick(int x, int y, bool isDown, bool isDouble);
	LadderElem *SearchElement(LadderElem *ref, eMoveCursor moveTo);

	RECT getElemArea(LadderElem *elem);

	void AddCommand(tCommandSource source, RECT region, bool (*fnc)(tCommandSource, void *), void *data,
		bool isDoubleClick, bool isSimulationMode, bool isDialogCommand = false);

	void CmdExpand(LadderElem *elem);

	// Funcoes para configurar as cores do diagrama ladder
	void              setLadderColors(tLadderColors colors, bool isModeSimulation);
	tLadderColors     getLadderColors(void);

	void              setLadderColorGroup(int elem, tLadderColorGroup colors);
	tLadderColorGroup getLadderColorGroup(int elem, bool isActive);
	void              delLadderColorGroup(int elem);
};

// Criacao do objeto que desenha a interface
extern LadderGUI gui;

#endif