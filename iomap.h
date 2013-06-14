#ifndef IOMAP_H
#define IOMAP_H

#include "poptools.h"
#include <map>

// Estruturas auxiliares
enum eReply { eReply_No = 0, eReply_Yes, eReply_Ask };
enum eType  { eType_Pending = 0, eType_General, eType_DigInput, eType_InternalRelay, eType_DigOutput,
	eType_InternalFlag, eType_Counter, eType_TOF, eType_TON, eType_RTO, eType_Reserved };

typedef struct {
	unsigned int countRequestBit;
	unsigned int countRequestInt;

	eType        type;
	unsigned int pin;
	unsigned int bit;
} mapDetails;

// Definicao da classe que gerencia todos os I/Os definidos
class LadderDiagram;

class mapIO {
private:
	LadderDiagram *diagram;

	unsigned long countIO;
	unsigned long selectedIO;
	map<string, pair<unsigned long, mapDetails> > IO;

	vector<string> vectorInternalFlag;
	vector<string> vectorReservedName;

	bool Add(string name, eType type);

	eReply AskUser(string txt);

public:
	mapIO(LadderDiagram *pDiagram);

	unsigned long  getIndex     (unsigned long id, bool isTotal = true);
	string         getName      (unsigned long id);
	mapDetails     getDetails   (unsigned long id);
	unsigned long  getID        (string name);
	unsigned long  getID        (unsigned int  index);
	char          *getTypeString(eType type);
	unsigned int   getCount     (void);
	void           Select       (unsigned int index);

	unsigned long Request(string name, bool isBit, eType type);
	void Discard(unsigned long id, bool isBit);

	bool Update(unsigned long id, string name, eType type);

	bool Assign(unsigned long   id, unsigned int pin, unsigned int bit);
	bool Assign(string        name, unsigned int pin, unsigned int bit);

	bool IsReserved    (string name);
	bool IsInternalFlag(string name);

	// Funcoes relacionadas com a interface grafica
	void updateGUI(void);
};

#endif
