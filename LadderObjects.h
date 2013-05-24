#ifndef LADDEROBJECTS_H
#define LADDEROBJECTS_H

#include <array>
#include <vector>
#include <string>

#include "intcode.h"

using namespace std;

/*** Estruturas auxiliares ***/
class LadderElem;
class LadderCircuit;

typedef struct {
	LadderElem    *SelectedElem;
	LadderCircuit *SelectedCircuit;

	bool canNegate;
	bool canNormal;
	bool canSetOnly;
	bool canResetOnly;
	bool canPushUp;
	bool canPushDown;
	bool canDelete;
	bool canInsertEnd;
	bool canInsertOther;
	bool canInsertComment;
} LadderContext;

/*** Classes representando os elementos do Ladder ***/

// Classe base de elementos - Abstrata, todos os elementos derivam dessa classe base
class LadderElem {
private:
	bool isName;
	bool isEndOfLine;
	bool isComment;
	bool isFormatted;
	int  selectedState;
	int  which;

	void Init(void);

protected:
	bool poweredAfter;

public:
	LadderElem(void);
	LadderElem(bool EOL, bool Comment, bool Formatted, bool Name, int elemWhich);

	virtual pair<string, string> DrawTXT(void) = 0;
	virtual void DrawGUI(void *data) = 0;

	virtual bool ShowDialog(void) = 0;

	virtual bool GenerateIntCode(IntCode &ic) = 0;

	inline bool IsComment     (void) { return isComment;    }
	inline bool IsEOL         (void) { return isEndOfLine;  }
	inline bool IsPoweredAfter(void) { return poweredAfter; }
	inline bool IsFormatted   (void) { return isFormatted;  }
	inline bool IsName        (void) { return isName;       }
	inline int  getWhich      (void) { return which;        }

	void Select          (int state) { selectedState = state; }
	int  getSelectedState(void)      { return selectedState; }

	virtual bool CanInsert(LadderContext context) = 0;

	virtual inline int getWidthTXT(void) = 0;
};

// Classe do elemento PlaceHolder
class LadderElemPlaceHolder : public LadderElem {
	// Sem propriedades privadas...

public:
	LadderElemPlaceHolder(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Comment
class LadderElemComment : public LadderElem {
private:
	string str;

public:
	LadderElemComment(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newStr);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void);
};

// Classe do elemento Contato
class LadderElemContact : public LadderElem {
private:
    string        name;
    bool          negated;
	unsigned int  type;
	unsigned char bit;

public:
	LadderElemContact(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void getProperties(string &curName, bool &curNegated, unsigned int &curType, unsigned char &curBit);
	void setProperties(string  newName, bool  newNegated, unsigned int  newType, unsigned char  newBit);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Bobina
class LadderElemCoil : public LadderElem {
private:
    string        name;
    bool          negated;
    bool          setOnly;
    bool          resetOnly;
	unsigned int  type;
	unsigned char bit;

public:
	LadderElemCoil(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, bool newNegated, bool newSetOnly, bool newResetOnly,
		unsigned int newType, unsigned char newBit);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Timer
class LadderElemTimer : public LadderElem {
private:
    string name;
    int    delay;

	int TimerPeriod(void);

public:
	LadderElemTimer(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, int newDelay);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento RTC
class LadderElemRTC : public LadderElem {
private:
	int           mode;  // Continuous or Intermittent (when using date/hour range) or only a fixed date/hour.
    unsigned char wday;  // [0:0] Sum, [0:1] Mon, [0:2] Tue, [0:3] Wed, [0:4] Thu, [0:5] Fri, [0:6] Sat, [0:7] WDay 1=YES, 0=No
	struct tm     start; // Start date/time of the range or the fixed one
	struct tm     end;   // End date/time when using range.

public:
	LadderElemRTC(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(int newMode, unsigned char newWday, struct tm newStart, struct tm newEnd);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Counter
class LadderElemCounter : public LadderElem {
private:
    string  name;
    int     max;

public:
	LadderElemCounter(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, int newMax);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Reset Timer / Counter
class LadderElemReset : public LadderElem {
private:
	string name;

public:
	LadderElemReset(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento One Shot
class LadderElemOneShot : public LadderElem {
	// Sem propriedades privadas...

public:
	LadderElemOneShot(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Cmp
class LadderElemCmp : public LadderElem {
private:
	string op1;
	string op2;

public:
	LadderElemCmp(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newOp1, string newOp2);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Math
class LadderElemMath : public LadderElem {
private:
	string op1;
	string op2;
	string dest;

public:
	LadderElemMath(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newOp1, string newOp2, string newDest);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 2; }
};

// Classe do elemento Sqrt
class LadderElemSqrt : public LadderElem {
private:
	string src;
	string dest;

public:
	LadderElemSqrt(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newSrc, string newDest);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Rand
class LadderElemRand : public LadderElem {
private:
	string var;
	string min;
	string max;

public:
	LadderElemRand(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newVar, string newMin, string newMax);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 2; }
};

// Classe do elemento Abs
class LadderElemAbs : public LadderElem {
private:
	string src;
	string dest;

public:
	LadderElemAbs(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newSrc, string newDest);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Move
class LadderElemMove : public LadderElem {
private:
	string src;
	string dest;

public:
	LadderElemMove(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newSrc, string newDest);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Open & Short
class LadderElemOpenShort : public LadderElem {
	// Sem propriedades privadas...

public:
	LadderElemOpenShort(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Set Bit
class LadderElemSetBit : public LadderElem {
private:
	string name;
	int    bit;
	bool   set;

public:
	LadderElemSetBit(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, int newBit, bool newSet);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Check Bit
class LadderElemCheckBit : public LadderElem {
private:
	string name;
	int    bit;
	bool   set;

public:
	LadderElemCheckBit(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, int newBit, bool newSet);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Read A/D
class LadderElemReadAdc : public LadderElem {
private:
	string name;

public:
	LadderElemReadAdc(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Set D/A
class LadderElemSetDa : public LadderElem {
private:
	string name;
	int    mode;

public:
	LadderElemSetDa(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, int newMode);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Read Encoder
class LadderElemReadEnc : public LadderElem {
private:
	string name;

public:
	LadderElemReadEnc(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Reset Encoder
class LadderElemResetEnc : public LadderElem {
private:
	string name;

public:
	LadderElemResetEnc(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Multiset D/A
class LadderElemMultisetDA : public LadderElem {
private:
	string	name;					// Tempo
	string	name1;					// Deslocamento
	bool	linear;					// true = Linear, false = Curva Ascendente/Descendente
	bool	forward;				// true = Avan�a, false = Recua
	bool	speedup;				// true = Acelera��o, false = Desacelera��o
	int		initval;				// valor inicial do DA na rampa
	int		type;					// 0 = Valor saida do DA (2048 ~ -2048), 1 = milivolt (mV) (10V ~-10V), 2 = percentual (%)
	int		gaint;					// tempo da curva de ganho em %
	int		gainr;					// resolu��o da curva de ganho em %
	bool	StartFromCurrentValue;	// false = Iniciar ou ir para zero, conforme speedup. true = partir do valor atual at� o valor configurado
	int		ramp_abort_mode;

public:
	LadderElemMultisetDA(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, string newName1, int newGaint, int newGainr, int newInitval,
		bool newLinear, bool newForward, bool newSpeedup, bool newStartFromCurrentValue);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Read / Write USS
class LadderElemUSS : public LadderElem {
private:
    string  name;
	int		id;
	int		parameter;
	int		parameter_set;
	int		index;

public:
	LadderElemUSS(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, int newId, int newParameter, int newParameter_set, int newIndex);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento ModBUS
class LadderElemModBUS : public LadderElem {
private:
    string  name;
	int		elem;
	int		address;
	bool	int32;
	bool	retransmitir;

public:
	LadderElemModBUS(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties();

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Set PWM
class LadderElemSetPWM : public LadderElem {
private:
    string  name;
    int     targetFreq;

public:
	LadderElemSetPWM(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties();

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Send / Receive UART
class LadderElemUART : public LadderElem {
private:
	string name;

public:
	LadderElemUART(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Master Relay
class LadderElemMasterRelay : public LadderElem {
	// Sem propriedades privadas...

public:
	LadderElemMasterRelay(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Shift Register
class LadderElemShiftRegister : public LadderElem {
private:
    string name;
    int    stages;

public:
	LadderElemShiftRegister(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newName, int newStages);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Look Up Table
class LadderElemLUT : public LadderElem {
private:
    string                             dest;
    string                             index;
    int                                count;
    bool                               editAsString;
    array<long, MAX_LOOK_UP_TABLE_LEN> vals;

public:
	LadderElemLUT(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newDest, string newIndex, int newCount,
		bool newEditAsString, array<long, MAX_LOOK_UP_TABLE_LEN> newVals);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Piecewise Linear
class LadderElemPiecewise : public LadderElem {
private:
    string                             dest;
    string                             index;
    int                                count;
    array<long, MAX_LOOK_UP_TABLE_LEN> vals;

public:
	LadderElemPiecewise(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newDest, string newIndex, int newCount, array<long, MAX_LOOK_UP_TABLE_LEN> newVals);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento Formatted String
class LadderElemFmtString : public LadderElem {
private:
    string var;
    string txt;

public:
	LadderElemFmtString(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newVar, string newTxt);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 2; }
};

// Classe do elemento Read / Write Yaskawa
class LadderElemYaskawa : public LadderElem {
private:
    string id;
    string var;
    string txt;

public:
	LadderElemYaskawa(int which);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newId, string newVar, string newTxt);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 2; }
};

// Classe do elemento Persist
class LadderElemPersist : public LadderElem {
private:
	string var;

public:
	LadderElemPersist(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties(string newVar);

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

// Classe do elemento X
class LadderElemX : public LadderElem {
private:

public:
	LadderElemX(void);

	pair<string, string> DrawTXT(void);
	void DrawGUI(void *data);

	bool ShowDialog(void);

	bool GenerateIntCode(IntCode &ic);

	void setProperties();

	bool CanInsert(LadderContext context);

	inline int getWidthTXT(void) { return 1; }
};

/*** Classe representando os circuitos (Serie / Paralelo) ***/

// Estrutura auxiliar que representa um subcircuito
typedef struct {
	LadderElem    *elem;
	LadderCircuit *subckt;
} Subckt;

class LadderCircuit {
private:
	bool isSeries;

	vector<Subckt> vectorSubckt;

	bool DrawElementTXT(vector< vector<int> > &DisplayMatrix, LadderElem *elem, int *cx, int *cy, bool poweredBefore);
	void VerticalWireTXT(int cx, int cy);

public:
	LadderCircuit(void);
	LadderCircuit(bool newSeries);

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

	void AddPlaceHolderIfNoEOL(void);
	bool AddElement(LadderElem *elem, LadderContext &context);
	bool DelElement(LadderElem *elem, LadderContext &context);

	void RemoveUnnecessarySubckts(void);
};

/*** Classe representando o Diagrama Ladder ***/
class LadderDiagram {
private:
	// Context properties
	LadderContext context;

	// list of rungs
	vector<LadderCircuit *> rungs;

	bool NeedScrollSelectedIntoView;

	IntCode ic;

	void Init(void);

	LadderCircuit *getSubcktForElement(LadderElem *elem);

	void updateContext(void);

	bool IsSelectedVisible(void);

public:
	LadderDiagram(void);

	vector<IntOp> getVectorIntCode(void);
	bool GenerateIntCode(void);

	int getWidthTXT (void);
	int getHeightTXT(void);

	void SelectElement(LadderElem *elem, int state);

	bool AddElement   (LadderElem *elem);
	bool DelElement   (LadderElem *elem = nullptr);
	bool EditSelectedElement(void);

	int  RungContainingElement (LadderElem *elem);
	int  RungContainingSelected(void);

	bool IsRungEmpty(unsigned int n);
	void NewRung(bool isAfter);
	bool PushRung(int rung, bool up);

	void DrawTXT(int OffsetX);
	void DrawGUI(void);

	void MouseClick(int x, int y, bool isDown, bool isDouble);
};

#endif
