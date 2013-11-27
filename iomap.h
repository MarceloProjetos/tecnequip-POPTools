#ifndef IOMAP_H
#define IOMAP_H

#include "poptools.h"
#include <map>

// Definicao da classe que gerencia todos os I/Os definidos
class mapIO {
private:
	LadderDiagram *diagram;

	unsigned int  seqNo;
	unsigned long countIO;
	unsigned long selectedIO;

	eSortBy currentSortBy;

	typedef map <string, pair<unsigned long, mapDetails> > tMapIO;
	tMapIO IO;

	typedef pair<string, pair<unsigned long, mapDetails> > tVectorIO;
	vector<tVectorIO> vectorIO;

	map<unsigned long, pair<string, eType> > mapCachedIO;

	unsigned int maxNameSize;

	vector<string>                 vectorInternalFlag;
	vector<string>                 vectorReservedName;
	vector< pair<string, string> > vectorInternalVar;

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
			unsigned long      id;
			bool               isBit;
			bool               isDiscard;
			eRequestAccessType accessType;
			bool               isUniqueRead;
			bool               isUniqueWrite;
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

	// Funcao que verifica o nome do I/O e retorna uma string valida
	string getValidName(string name);

	// Funcao que sincroniza o mapa de I/O com o vetor ordenado
	void SyncVectorIO(void);

public:
	mapIO(LadderDiagram *pDiagram);

	void Clear(void); // Descarrega a lista de I/Os

	unsigned long   getIndex          (unsigned long id, bool isTotal = true);
	string          getName           (unsigned long id);
	string          getInternalVarName(string name);
	string          getReservedName   (eType type);
	mapDetails      getDetails        (unsigned long id);
	unsigned long   getID             (string name);
	unsigned long   getID             (unsigned int  index, bool fromVectorIO = false);
	char           *getTypeString     (eType type);
	unsigned int    getCount          (void);
	vector<string>  getList           (void);
	void            Select            (unsigned int index);

	// Funcao que retorna um I/O do cache
	pair<string, eType> getCachedIO   (unsigned long id);
	 
	// Funcao que retorna string para a proxima variavel da sequencia
	string         getNextVar        (string prefix = "seq");

	unsigned long Request(tRequestIO infoIO);
	void          Discard(tRequestIO infoIO);

	bool Update(unsigned long id, string name, eType type, bool isUndoRedo = false);

	bool Assign(unsigned long   id, unsigned int pin, unsigned int bit, bool isUndoRedo = false);
	bool Assign(string        name, unsigned int pin, unsigned int bit, bool isUndoRedo = false);

	bool IsReserved    (string name);
	bool IsInternalFlag(string name);
	bool IsInternalVar (string name);

	// Funcoes que indicam se existe algum I/O que use a UART e o PWM, respectivamente
	bool UartFunctionUsed(void);
	bool PwmFunctionUsed (void);

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
