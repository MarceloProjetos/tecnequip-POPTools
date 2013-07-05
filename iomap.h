#ifndef IOMAP_H
#define IOMAP_H

#include "poptools.h"
#include <map>

// Definicao da classe que gerencia todos os I/Os definidos
class mapIO {
private:
	LadderDiagram *diagram;

	unsigned long countIO;
	unsigned long selectedIO;

	typedef map<string, pair<unsigned long, mapDetails> > tMapIO;
	tMapIO IO;

	vector<string> vectorInternalFlag;
	vector<string> vectorReservedName;

	bool Add(string name, eType type, bool isUndoRedo = false);

	// Estrutura de dados para acoes de Desfazer / Refazer
	enum UndoRedoActionsEnum { eCheckpoint = 0, eAssign, eRequest, eUpdate, eAdd, eDiscard };
	union UndoRedoData {
		struct {
			unsigned long  id;
			unsigned int   pin;
			unsigned int   bit;
		} Assign;
		struct {
			unsigned long  id;
			bool           isBit;
			bool           isDiscard;
		} Request;
		struct {
			unsigned long  id;
			char          *name;
			eType          type;
		} Update;
		struct {
			char          *name;
			eType          type;
		} Add;
		struct {
			unsigned long  id;
			char          *name;
			mapDetails     detailsIO;
		} Discard;
	};

public:
	mapIO(LadderDiagram *pDiagram);

	void Clear(void); // Descarrega a lista de I/Os

	unsigned long  getIndex     (unsigned long id, bool isTotal = true);
	string         getName      (unsigned long id);
	mapDetails     getDetails   (unsigned long id);
	unsigned long  getID        (string name);
	unsigned long  getID        (unsigned int  index);
	char          *getTypeString(eType type);
	unsigned int   getCount     (void);
	void           Select       (unsigned int index);

	unsigned long Request(string name, bool isBit, eType type);
	void          Discard(unsigned long id, bool isBit);

	bool Update(unsigned long id, string name, eType type, bool isUndoRedo = false);

	bool Assign(unsigned long   id, unsigned int pin, unsigned int bit, bool isUndoRedo = false);
	bool Assign(string        name, unsigned int pin, unsigned int bit, bool isUndoRedo = false);

	bool IsReserved    (string name);
	bool IsInternalFlag(string name);

	string          getPortName           (int index);
	string          getPinName            (int index);
	vector<string>  getVectorInternalFlags(void);

	// Funcao para ordenar a lista de I/O conforme o campo especificado
	void            Sort                  (eSortBy sortby);

	// Funcao para checar os I/Os, buscando por erros como variaveis do tipo temporizador
	// usadas em mais de 1 lugar ao mesmo tempo, I/Os nao associados a uma entrada / saida, etc.
	bool Validate(eValidateIO mode);

	// Funcoes para gravar / carregar o mapa de I/O do disco
	bool Load(FILE *f, unsigned int version);
	bool Save(FILE *f);

	// Funcoes relacionadas com a interface grafica
	void updateGUI      (void);
	void ShowIoMapDialog(int item);

	// Funcao que executa uma acao de desfazer / refazer
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

#endif
