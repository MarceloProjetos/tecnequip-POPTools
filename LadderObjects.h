#ifndef LADDEROBJECTS_H
#define LADDEROBJECTS_H

#include <deque>
#include <array>
#include <vector>
#include <string>

#include "intcode.h"
#include "iomap.h"

using namespace std;

/*** Estruturas auxiliares ***/
class LadderElem;
class LadderCircuit;
class LadderDiagram;

// Estrutura auxiliar que representa um subcircuito
typedef struct {
	LadderElem    *elem;
	LadderCircuit *subckt;
} Subckt;

#define SUBCKT_STATUS_NOTFOUND 0
#define SUBCKT_STATUS_FIRST    1
#define SUBCKT_STATUS_INSIDE   2
#define SUBCKT_STATUS_LAST     3

typedef struct {
	unsigned int   point;
	Subckt         subckt;
	LadderCircuit *series;
	LadderCircuit *parallel;
} InsertionPoint;

typedef struct {
	LadderElem    *SelectedElem;
	LadderElem    *ParallelStart;
	LadderCircuit *SelectedCircuit;
	LadderDiagram *Diagram;

	int SelectedState;

	bool canNegate;
	bool canNormal;
	bool canSetOnly;
	bool canResetOnly;
	bool canPushUp;
	bool canPushDown;
	bool canDelete;
	bool canDeleteRung;
	bool canInsertEnd;
	bool canInsertOther;
	bool canInsertComment;
} LadderContext;

// Estrutura auxiliar que representa uma acao de desfazer / refazer
typedef struct {
	// Texto que descreve a acao de forma amigavel para o usuario
	string         Description;

	// Objeto responsavel por executar a acao
	LadderElem    *elem;
	LadderCircuit *subckt;

	// Contexto Antes e Depois de executar a acao
	LadderContext  contextAfter;
	LadderContext  contextBefore;

	// Codigo da acao e ponteiro para dados especificos da acao
	unsigned int   action;
	void          *data;
} UndoRedoAction;

// Funcao auxiliar que retorna o tipo de I/O do timer conforme o seu tipo
eType getTimerTypeIO(int which);

/*** Classes representando os elementos do Ladder ***/

// Classe base de elementos - Abstrata, todos os elementos derivam dessa classe base
class LadderElem {
private:
	bool isEndOfLine;
	bool isComment;
	bool isFormatted;
	int  which;

	virtual void internalSetProperties(void *data) = 0;

	// Estrutura de dados para acoes de Desfazer / Refazer
	enum UndoRedoActionsEnum { eCheckpoint = 0, eSetProp, eActionsEnd };
	union UndoRedoData {
		struct {
			void *data;
		} SetProp;
	};

	void Init(void);

	// Funcao que executa uma acao de desfazer / refazer nas classes derivadas
	virtual bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action) = 0;

protected:
	bool poweredAfter;

	LadderDiagram *Diagram;

public:
	LadderElem(void);
	LadderElem(bool EOL, bool Comment, bool Formatted, int elemWhich);

	virtual pair<string, string> DrawTXT(void) = 0;
	virtual void DrawGUI(void *data) = 0;

	virtual bool ShowDialog(LadderContext context) = 0;

	virtual bool GenerateIntCode(IntCode &ic) = 0;

	inline bool IsComment     (void) { return isComment;    }
	inline bool IsEOL         (void) { return isEndOfLine;  }
	inline bool IsPoweredAfter(void) { return poweredAfter; }
	inline bool IsFormatted   (void) { return isFormatted;  }
	inline int  getWhich      (void) { return which;        }

	virtual bool CanInsert(LadderContext context) = 0;

	virtual inline int getWidthTXT(void) = 0;

	void          setProperties(LadderContext context, void *propData);
	virtual void *getProperties(void) = 0;

	virtual LadderElem *Clone(void) = 0;

	// Funcoes relacionadas com I/O
	virtual bool acceptIO(unsigned long id, eType type) = 0;
	virtual void updateIO(bool isDiscard) = 0;

	// Funcao que executa uma acao de desfazer / refazer
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento PlaceHolder
class LadderElemPlaceHolder : public LadderElem {
	// Sem propriedades privadas...
	void internalSetProperties(void *data) { }

public:
	LadderElemPlaceHolder(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	void *getProperties(void) { return nullptr; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Comment
struct LadderElemCommentProp {
	string str;
};

class LadderElemComment : public LadderElem {
private:
	LadderElemCommentProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemComment(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void);

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Contato
struct LadderElemContactProp {
    bool                     negated;
	pair<unsigned long, int> idName;
};

class LadderElemContact : public LadderElem {
private:
	LadderElemContactProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemContact(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type);
	void updateIO(bool isDiscard);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Bobina
struct LadderElemCoilProp {
    bool                     negated;
    bool                     setOnly;
    bool                     resetOnly;
	pair<unsigned long, int> idName;
};

class LadderElemCoil : public LadderElem {
private:
	LadderElemCoilProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemCoil(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type);
	void updateIO(bool isDiscard);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Timer
struct LadderElemTimerProp {
	pair<unsigned long, int> idName;
    int                      delay;
};

class LadderElemTimer : public LadderElem {
private:
	LadderElemTimerProp prop;

	int TimerPeriod(void);

	void internalSetProperties(void *data);

public:
	LadderElemTimer(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type);
	void updateIO(bool isDiscard);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento RTC
struct LadderElemRTCProp {
	int           mode;  // Continuous or Intermittent (when using date/hour range) or only a fixed date/hour.
    unsigned char wday;  // [0:0] Sum, [0:1] Mon, [0:2] Tue, [0:3] Wed, [0:4] Thu, [0:5] Fri, [0:6] Sat, [0:7] WDay 1=YES, 0=No
	struct tm     start; // Start date/time of the range or the fixed one
	struct tm     end;   // End date/time when using range.
};

class LadderElemRTC : public LadderElem {
private:
	LadderElemRTCProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemRTC(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Counter
struct LadderElemCounterProp {
	pair<unsigned long, int> idName;
    int                      max;
};

class LadderElemCounter : public LadderElem {
private:
	LadderElemCounterProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemCounter(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type);
	void updateIO(bool isDiscard);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Reset Timer / Counter
struct LadderElemResetProp {
	pair<unsigned long, int> idName;
};

class LadderElemReset : public LadderElem {
private:
	LadderElemResetProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemReset(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type);
	void updateIO(bool isDiscard);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento One Shot
class LadderElemOneShot : public LadderElem {
	// Sem propriedades privadas...
	void internalSetProperties(void *data) { }

public:
	LadderElemOneShot(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void) { return nullptr; }

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Cmp
struct LadderElemCmpProp {
	pair<unsigned long, int> idOp1;
	pair<unsigned long, int> idOp2;
};

class LadderElemCmp : public LadderElem {
private:
	LadderElemCmpProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemCmp(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type);
	void updateIO(bool isDiscard);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Math
struct LadderElemMathProp {
	string op1;
	string op2;
	string dest;
};

class LadderElemMath : public LadderElem {
private:
	LadderElemMathProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemMath(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 2; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Sqrt
struct LadderElemSqrtProp {
	string src;
	string dest;
};

class LadderElemSqrt : public LadderElem {
private:
	LadderElemSqrtProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemSqrt(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Rand
struct LadderElemRandProp {
	string var;
	string min;
	string max;
};

class LadderElemRand : public LadderElem {
private:
	LadderElemRandProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemRand(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 2; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Abs
struct LadderElemAbsProp {
	string src;
	string dest;
};

class LadderElemAbs : public LadderElem {
private:
	LadderElemAbsProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemAbs(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Move
struct LadderElemMoveProp {
	string src;
	string dest;
};

class LadderElemMove : public LadderElem {
private:
	LadderElemMoveProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemMove(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Open & Short
class LadderElemOpenShort : public LadderElem {
	// Sem propriedades privadas...
	void internalSetProperties(void *data) { }

public:
	LadderElemOpenShort(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void) { return nullptr; }

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Set Bit
struct LadderElemSetBitProp {
	string name;
	int    bit;
	bool   set;
};

class LadderElemSetBit : public LadderElem {
private:
	LadderElemSetBitProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemSetBit(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Check Bit
struct LadderElemCheckBitProp {
	string name;
	int    bit;
	bool   set;
};

class LadderElemCheckBit : public LadderElem {
private:
	LadderElemCheckBitProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemCheckBit(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Read A/D
struct LadderElemReadAdcProp {
	string name;
};

class LadderElemReadAdc : public LadderElem {
private:
	LadderElemReadAdcProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemReadAdc(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Set D/A
struct LadderElemSetDaProp {
	string name;
	int    mode;
};

class LadderElemSetDa : public LadderElem {
private:
	LadderElemSetDaProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemSetDa(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Read Encoder
struct LadderElemReadEncProp {
	string name;
};

class LadderElemReadEnc : public LadderElem {
private:
	LadderElemReadEncProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemReadEnc(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Reset Encoder
struct LadderElemResetEncProp {
	string name;
};

class LadderElemResetEnc : public LadderElem {
private:
	LadderElemResetEncProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemResetEnc(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Multiset D/A
struct LadderElemMultisetDAProp {
	string	name;					// Tempo
	string	name1;					// Deslocamento
	bool	linear;					// true = Linear, false = Curva Ascendente/Descendente
	bool	forward;				// true = Avança, false = Recua
	bool	speedup;				// true = Aceleração, false = Desaceleração
	int		initval;				// valor inicial do DA na rampa
	int		type;					// 0 = Valor saida do DA (2048 ~ -2048), 1 = milivolt (mV) (10V ~-10V), 2 = percentual (%)
	int		gaint;					// tempo da curva de ganho em %
	int		gainr;					// resolução da curva de ganho em %
	bool	StartFromCurrentValue;	// false = Iniciar ou ir para zero, conforme speedup. true = partir do valor atual até o valor configurado
	int		ramp_abort_mode;
};

class LadderElemMultisetDA : public LadderElem {
private:
	LadderElemMultisetDAProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemMultisetDA(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Read / Write USS
struct LadderElemUSSProp {
    string  name;
	int		id;
	int		parameter;
	int		parameter_set;
	int		index;
};

class LadderElemUSS : public LadderElem {
private:
	LadderElemUSSProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemUSS(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento ModBUS
struct LadderElemModBUSProp {
    string  name;
	int		elem;
	int		address;
	bool	int32;
	bool	retransmitir;
};

class LadderElemModBUS : public LadderElem {
private:
	LadderElemModBUSProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemModBUS(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Set PWM
struct LadderElemSetPWMProp {
    string  name;
    int     targetFreq;
};

class LadderElemSetPWM : public LadderElem {
private:
	LadderElemSetPWMProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemSetPWM(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	bool CanInsert(LadderContext context);

	void *getProperties(void);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Send / Receive UART
struct LadderElemUARTProp {
	string name;
};

class LadderElemUART : public LadderElem {
private:
	LadderElemUARTProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemUART(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Master Relay
class LadderElemMasterRelay : public LadderElem {
	// Sem propriedades privadas...
	void internalSetProperties(void *data) { }

public:
	LadderElemMasterRelay(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void) { return nullptr; }

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Shift Register
struct LadderElemShiftRegisterProp {
    string name;
    int    stages;
};

class LadderElemShiftRegister : public LadderElem {
private:
	LadderElemShiftRegisterProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemShiftRegister(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Look Up Table
struct LadderElemLUTProp {
    string                             dest;
    string                             index;
    int                                count;
    bool                               editAsString;
    array<long, MAX_LOOK_UP_TABLE_LEN> vals;
};

class LadderElemLUT : public LadderElem {
private:
	LadderElemLUTProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemLUT(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Piecewise Linear
struct LadderElemPiecewiseProp {
    string                             dest;
    string                             index;
    int                                count;
    array<long, MAX_LOOK_UP_TABLE_LEN> vals;
};

class LadderElemPiecewise : public LadderElem {
private:
	LadderElemPiecewiseProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemPiecewise(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Formatted String
struct LadderElemFmtStringProp {
    string var;
    string txt;
};

class LadderElemFmtString : public LadderElem {
private:
	LadderElemFmtStringProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemFmtString(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 2; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Read / Write Yaskawa
struct LadderElemYaskawaProp {
    string id;
    string var;
    string txt;
};

class LadderElemYaskawa : public LadderElem {
private:
	LadderElemYaskawaProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemYaskawa(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 2; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento Persist
struct LadderElemPersistProp {
	string var;
};

class LadderElemPersist : public LadderElem {
private:
	LadderElemPersistProp prop;

	void internalSetProperties(void *data);

public:
	LadderElemPersist(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

// Classe do elemento X
struct LadderElemXProp {
};

class LadderElemX : public LadderElem {
private:
	// Sem propriedades privadas...
	LadderElemXProp prop;

	void internalSetProperties(void *data) { }

public:
	LadderElemX(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(LadderContext context);

	bool GenerateIntCode(IntCode &ic);

	void *getProperties(void) { return nullptr; }

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(void);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool acceptIO(unsigned long id, eType type) { return true; }
	void updateIO(bool isDiscard) { }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/*** Classe representando os circuitos (Serie / Paralelo) ***/

class LadderCircuit {
private:
	bool isSeries;

	vector<Subckt> vectorSubckt;

	bool DrawElementTXT(vector< vector<int> > &DisplayMatrix, LadderElem *elem, int *cx, int *cy, bool poweredBefore);
	void VerticalWireTXT(int cx, int cy);

	// Estrutura de dados para acoes de Desfazer / Refazer
	enum UndoRedoActionsEnum { eCheckpoint = 0, eInsertSubckt, eDeleteSubckt, eMoveSubckt };
	union UndoRedoData {
		struct {
			unsigned int pos;
			Subckt       subckt;
		} InsertSubckt;
		struct {
			unsigned int pos;
			Subckt       subckt;
		} DeleteSubckt;
		struct {
			unsigned int   pos;
			unsigned int   fromPos;
			LadderCircuit *circuit;
		} MoveSubckt;
	};

protected:
	bool InsertSubckt(LadderContext context, unsigned int pos, Subckt s, bool isMove = false,
		bool isUndoRedo = false);
	bool DeleteSubckt(LadderContext context, unsigned int pos, bool isMove = false,	bool isUndoRedo = false);
	bool MoveSubckt  (LadderContext context, unsigned int pos, LadderCircuit *fromCircuit,
		unsigned int fromPos, bool isUndoRedo = false);

public:
	LadderCircuit(void);
	LadderCircuit(bool newSeries);

	~LadderCircuit(void);

	bool DrawTXT(vector< vector<int> > &DisplayMatrix, int *cx, int *cy, bool poweredBefore, int ColsAvailable);
	void DrawGUI(void *data);

	bool IsComment(void);
	bool IsEmpty(void);
	bool IsLast(LadderElem *elem);
	bool IsSeries(void) { return isSeries; }
	bool HasEOL(void);

	bool GenerateIntCode(IntCode &ic);

	unsigned int   getSize(void) { return vectorSubckt.size(); }
	LadderCircuit *getSubcktForElement(LadderElem *elem);
	Subckt         getSubckt(unsigned int pos);

	int            getWidthTXT (int ColsAvailable);
	int            getHeightTXT(void);

	void AddPlaceHolderIfNoEOL(LadderContext context);
	bool AddElement(LadderElem *elem, LadderContext &context);
	bool InsertParallel(LadderElem *elem, unsigned int start, unsigned int end, LadderContext &context);
	bool DelElement(LadderElem *elem, LadderContext &context);

	void RemoveUnnecessarySubckts(LadderContext context);

	int SearchMatch(LadderCircuit *series, int direction);
	int ElemInSubcktSeries(LadderContext &context, InsertionPoint *point);

	LadderCircuit *Clone(void);

	// Funcoes relacionadas com I/O
	bool acceptIO(unsigned long id, eType type);
	void updateIO(bool isDiscard);

	// Funcao que executa uma acao de desfazer / refazer
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/*** Classe representando o Diagrama Ladder ***/
class LadderDiagram {
private:
	// Undo / Redo Action Lists
	unsigned int CheckPointLevels;
	unsigned int CheckPointLevelsMax;
	unsigned int CheckpointBeginCount;

	bool CheckpointDoRollback;

	deque<UndoRedoAction> UndoList;
	deque<UndoRedoAction> RedoList;

	void DiscardCheckpoint(bool isUndo = true);
	bool ExecuteAction(bool isUndo, bool isDiscard, UndoRedoAction Action);

	// Estrutura de dados para acoes de Desfazer / Refazer
	enum UndoRedoActionsEnum { eCheckpoint = 0, ePushRung, eNewRung, eDelRung };
	union UndoRedoData {
		struct {
			int  pos;
			bool isUp;
		} PushRung;
		struct {
			int  pos;
			LadderCircuit *rung;
		} NewRung;
		struct {
			int  pos;
			LadderCircuit *rung;
		} DelRung;
	};

	// Context properties
	LadderContext context;

	// list of rungs
	vector<LadderCircuit *> rungs;

	// Objeto que contem todos os I/Os
	mapIO IO;

	bool NeedScrollSelectedIntoView;

	IntCode ic;

	LadderElem    *copiedElement;
	LadderCircuit *copiedRung;

	void Init(void);

	LadderCircuit *getSubcktForElement(LadderElem *elem);

	void updateUndoContextAfter(bool forceNotNull = false);

	bool IsSelectedVisible(void);

	bool InsertParallel(LadderElem *elem);

public:
	LadderDiagram(void);

	vector<IntOp> getVectorIntCode(void);
	bool GenerateIntCode(void);

	int getWidthTXT (void);
	int getHeightTXT(void);

	void updateContext(void);
	LadderContext getContext(void) { return context; }

	void ClearDiagram(void);

	void SelectElement(LadderElem *elem, int state);

	bool AddElement         (LadderElem *elem);
	bool DelElement         (LadderElem *elem = nullptr);
	bool CopyElement        (LadderElem *elem = nullptr);
	bool PasteElement       (void);
	bool EditSelectedElement(void);

	int  RungContainingElement (LadderElem *elem);
	int  RungContainingSelected(void);

	void NewRung    (bool isAfter);
	bool PushRung   (int rung, bool up);
	bool DeleteRung (int rung);
	bool IsRungEmpty(unsigned int n);
	bool CopyRung   (LadderElem *elem = nullptr);
	bool PasteRung  (bool isAfter);

	void DrawTXT(int OffsetX);
	void DrawGUI(void);

	void MouseClick(int x, int y, bool isDown, bool isDouble);

	bool AddParallelStart(void);

	// Funcoes relacionadas aos comandos de Desfazer / Refazer
	void RegisterAction    (UndoRedoAction Action, bool isUndo = true);
	void CheckpointBegin   (string description);
	void CheckpointRollback(void) { CheckpointDoRollback = true; }
	void CheckpointEnd     (void);
	void UndoRedo          (bool isUndo);
	void Undo              (void) { UndoRedo( true); }
	void Redo              (void) { UndoRedo(false); }

	// Funcoes relacionadas com I/O
	bool          acceptIO        (unsigned long  id, eType type);
	void          updateIO        (pair<unsigned long, int> &pin, string defaultName, bool isBit, eType defaultType, bool isDiscard);
	bool          getIO           (pair<unsigned long, int> &pin, string name, bool isBit, eType type);
	string        getNameIO       (unsigned long  id);
	string        getNameIObyIndex(unsigned int index);
	mapDetails    getDetailsIO    (unsigned long  id);
	mapDetails    getDetailsIO    (string name) { return getDetailsIO(IO.getID(name)); }
	char         *getStringTypeIO (unsigned int   index);
	unsigned int  getCountIO      (void);
	void          selectIO        (unsigned int index);

	bool equalsNameIO             (string name1, string name2);

	bool IsValidNumber     (string varnumber);
	bool IsValidVarName    (string varname);
	bool IsValidNameAndType(unsigned long id, string name, eType type, const char *FieldName, unsigned int Rules, int MinVal, int MaxVal, eReply canUpdate = eReply_Ask);
	bool IsValidNameAndType(unsigned long id, string name, eType type, eReply canUpdate = eReply_Ask);

	// Funcao que exibe uma janela de dialogo para o usuario. Dependente de implementacao da interface
	eReply ShowDialog(bool isMessage, char *title, char *message, ...);

	// Funcao que executa uma acao de desfazer / refazer
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

#endif
