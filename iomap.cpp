#include "poptools.h"

static LONG_PTR PrevBoardProc;

static bool gSortAscending = true;

// Funcoes para ordenacao
template<class T>
	struct CompareIoName : std::binary_function<T,T,bool>
	{
		inline bool operator ()(const T& _left, const T& _right)
		{
			return gSortAscending ?
				_stricmp(_left .first.c_str(), _right.first.c_str()) < 0 :
				_stricmp(_right.first.c_str(), _left .first.c_str()) < 0;
		}
	};

template<class T>
	struct CompareIoType : std::binary_function<T,T,bool>
	{
		inline bool operator ()(const T& _left, const T& _right)
		{
			return gSortAscending ?
				_left .second.second.type < _right.second.second.type :
				_right.second.second.type < _left .second.second.type;
		}
	};

template<class T>
	struct CompareIoPin : std::binary_function<T,T,bool>
	{
		inline bool operator ()(const T& _left, const T& _right)
		{
			// Se algum dos pinos nao tiver associacao, o que tem deve sempre ser considerado menor
			if(!_left .second.second.pin &&  _right.second.second.pin) return false;
			if( _left .second.second.pin && !_right.second.second.pin) return true;

			return gSortAscending ?
				_left .second.second.pin < _right.second.second.pin :
				_right.second.second.pin < _left .second.second.pin;
		}
	};

template<class T>
	struct CompareIoPort : std::binary_function<T,T,bool>
	{
		inline bool operator ()(const T& _left, const T& _right)
		{
			unsigned int iLeft, iRight;

			// Se algum dos pinos nao tiver associacao, o que tem deve sempre ser considerado menor
			if(!_left .second.second.pin &&  _right.second.second.pin) return false;
			if( _left .second.second.pin && !_right.second.second.pin) return true;

			// Calcula o valor conforme o peso de cada variavel
			eType type       = _left .second.second.type;
			unsigned int bit = _left .second.second.bit;

			if(IoMap_IsModBUS(_left .second.second)) {
				if(type != eType_General) {
					bit++;
				}
				type = eType_Last;
			}
			iLeft  = (int(type)*10000) + (_left .second.second.pin*100) + bit;

			type = _right.second.second.type;
			bit  = _right.second.second.bit;

			if(IoMap_IsModBUS(_right.second.second)) {
				if(type != eType_General) {
					bit++;
				}
				type = eType_Last;
			}
			iRight = (int(type)*10000) + (_right.second.second.pin*100) + bit;

			return gSortAscending ?
				iLeft  < iRight :
				iRight < iLeft;
		}
	};

mapIO::mapIO(LadderDiagram *pDiagram)
{
	seqNo      = 0;
	countIO    = 0;
	selectedIO = 0;

	// Cada placa de expansao inicia 100 pinos depois da anterior.
	// Assim, E2 da placa 3 sera o pino 302 para o diagrama
	pinOffset  = 100;

	currentSortBy        = eSortBy_Name;
	currentSortAscending = true;

	maxNameSize = 16;

	diagram = pDiagram;

	// Adicionando itens que sao flags internas
	vectorInternalFlag.push_back("SerialReady"  );
	vectorInternalFlag.push_back("SerialTimeout");
	vectorInternalFlag.push_back("SerialAborted");
	vectorInternalFlag.push_back("RampActive"   );
	vectorInternalFlag.push_back("TcpReady"     );
	vectorInternalFlag.push_back("TcpTimeout"   );

	// Adicionando itens que sao variaveis internas
	vectorInternalVar.push_back(pair<string, string>("IncPerimRoda"  ,"INC_Perimeter"));
	vectorInternalVar.push_back(pair<string, string>("IncPulsosVolta","INC_PPR"      ));
	vectorInternalVar.push_back(pair<string, string>("IncFatorCorr"  ,"INC_Factor10k"));
	vectorInternalVar.push_back(pair<string, string>("AbsPerimRoda"  ,"ABS_Perimeter"));
	vectorInternalVar.push_back(pair<string, string>("AbsFatorCorr"  ,"ABS_Factor10k"));

	// Adicionando variaveis reservadas
	vectorReservedName.push_back(_("entrada"  ));
	vectorReservedName.push_back(_("saida" ));
	vectorReservedName.push_back(_("novo" ));
	vectorReservedName.push_back(_("var" ));
	vectorReservedName.push_back(_("char"));
	vectorReservedName.push_back(_("dest"));
	vectorReservedName.push_back(_("src" ));
	vectorReservedName.push_back(_("xvar" ));
	vectorReservedName.push_back(_("yvar" ));
	vectorReservedName.push_back(_("indice" ));
}

void mapIO::Clear(void)
{
	IO.clear();
	mapCachedIO.clear();

	countIO = 0;
	selectedIO = 0;
	SyncVectorIO();
}

bool mapIO::IsReserved(string name)
{
	vector<string>::size_type i;
	for(i = 0; i < vectorReservedName.size(); i++) {
		if(name == vectorReservedName[i]) return true; // Encontrado!
	}

	// Caso especial para Shift Register:
	// Nome reservado se iniciar por reg e terminar em numero
	char cname[MAX_NAME_LEN];
	strcpy(cname, name.c_str());
	if(tolower(cname[0]) == 'r' && tolower(cname[1]) == 'e' && tolower(cname[2]) == 'g') {
		// inicia com reg, agora verifica se termina em numero
		int val = atoi(cname + 3);
		sprintf(cname + 3, "%d", val);
		if(name == cname) {
			return true;
		}
	}

	return false;
}

bool mapIO::IsInternalFlag(string name)
{
	vector<string>::size_type i;
	for(i = 0; i < vectorInternalFlag.size(); i++) {
		if(name == vectorInternalFlag[i]) return true; // Encontrado!
	}

	return false;
}

bool mapIO::IsInternalVar(string name)
{
	vector<string>::size_type i;
	for(i = 0; i < vectorInternalVar.size(); i++) {
		if(name == vectorInternalVar[i].first) return true; // Encontrado!
	}

	return false;
}

// Funcoes que indicam se existe algum I/O que use a UART e o PWM, respectivamente
bool mapIO::UartFunctionUsed(void)
{
	tMapIO::iterator it;

	for(it = IO.begin(); it != IO.end(); it++) {
		if(it->second.second.type == eType_RxUART || it->second.second.type == eType_TxUART)
			return true; // Encontrado!
	}

	return false;
}

bool mapIO::PwmFunctionUsed (void)
{
	tMapIO::iterator it;

	for(it = IO.begin(); it != IO.end(); it++) {
		if(it->second.second.type == eType_PWM)
			return true; // Encontrado!
	}

	return false;
}

void mapIO::SyncVectorIO(void)
{
	vectorIO.clear();
	vectorIO = vector<tVectorIO>(IO.begin(), IO.end());
	Sort(eSortBy_Current);
}

string mapIO::getValidName(string name)
{
	if(name.size() > maxNameSize) {
		name.resize(maxNameSize);
	}

	tMapIO::iterator it;

	for(it = IO.begin(); it != IO.end(); it++) {
		if(!_stricmp(it->first.c_str(), name.c_str())) {
			name = it->first;
		}
	}

	return name;
}

bool mapIO::Add(string name, eType type, bool isUndoRedo)
{
	bool ret = false;

	if(IO.count(name) == 0) {
		if(IsReserved(name)) {
			type = eType_Reserved;
		}

		IO[name].first       = 1 + countIO++; // ID a partir de 1. Zero indica id invalido
		IO[name].second.pin  = 0;
		IO[name].second.bit  = 0;
		IO[name].second.type = type;

		// Inicializa a contagem de requisicoes de tipo de dado
		IO[name].second.countRequestBit   = 0;
		IO[name].second.countRequestInt   = 0;

		// Inicializa a contagem de requisicoes de tipo de acesso
		IO[name].second.countRequestRead  = 0;
		IO[name].second.countRequestWrite = 0;

		// Inicializa as flags que indicam se a variavel pode ser utilizada mais de 1 vez
		// para cada acesso, ou seja, se nao pode ser utilizada por dois elementos ao mesmo tempo
		IO[name].second.isUniqueRead      = false;
		IO[name].second.isUniqueWrite     = false;

		if(!isUndoRedo) {
			// Registro da acao para desfazer / refazer
			diagram->CheckpointBegin(_("Adicionar I/O"));

			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			data->Add.name = AllocCharFromString(name);
			data->Add.type = type;

			action.action        = eAdd;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = diagram->getContext();
			action.data          = data;
			action.io            = this;
			action.elem          = nullptr;
			action.subckt        = nullptr;

			diagram->RegisterAction(action);

			diagram->CheckpointEnd();
		}

		updateGUI();

		ret = true;
	}

	return true;
}

bool mapIO::Update(unsigned long id, string name, eType type, bool isUndoRedo)
{
	string old_name = getName(id);
	if(old_name.size() == 0) return false; // id invalido!

	name = getValidName(name);

	if(IsReserved(name)) {
		type = eType_Reserved;
	}

	// Check for Internal Flags and Reserved Names restrictions
	if((type != eType_InternalFlag && IsInternalFlag(name)) ||
		(type == eType_InternalFlag && !IsInternalFlag(name)) ||
		(IsReserved(name) && name != old_name)) {
			return false;
	}

	if(!isUndoRedo) {
		// Registro da acao para desfazer / refazer
		diagram->CheckpointBegin(_("Atualizar I/O"));

		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->Update.id   = id;
		data->Update.name = AllocCharFromString(old_name);
		data->Update.type = IO[old_name].second.type;
		data->Update.pin  = IO[old_name].second.pin;

		action.action        = eUpdate;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = diagram->getContext();
		action.data          = data;
		action.io            = this;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		diagram->RegisterAction(action);

		diagram->CheckpointEnd();
	}

	// Se estiver alterando o tipo do I/O, devemos remover a associacao do pino pois nao sera valido
	if(IO[old_name].second.type != type) {
		IO[old_name].second.pin = 0;
	}

	if(name == old_name) { // Nome inalterado, atualiza apenas o tipo!
		IO[name].second.type = type;
	} else {
		tMapIO::iterator it = IO.find(old_name);

		// Atualizando o nome do I/O
		if(IO.count(name) > 0) {
			// Nesta condicao esta mesclando dois I/Os existentes.
			// Situacao possivel porem que nao ocorre atualmente devido a tratamento em LadderDiagram::getIO
			// Para garantir que nao vai mesmo ocorrer, vamos gerar erro aqui!
			// Se vier a ser alterado o tratamento em LadderDiagram::getIO, vai aparecer o erro e sera entao
			// possivel corrigi-lo ao inves de prejudicar toda a logica
			// Precisa de uma tarefa especifica de undo/redo para esta situacao e tambem mesclar as contagens de
			// requests int/bit, read/write e flags (o que ja esta sendo feito abaixo porem precisa de verificacao)
			oops();

			IO[name].second.countRequestBit   += it->second.second.countRequestBit;
			IO[name].second.countRequestInt   += it->second.second.countRequestInt;

			IO[name].second.countRequestRead  += it->second.second.countRequestRead;
			IO[name].second.countRequestWrite += it->second.second.countRequestWrite;

			IO[name].second.isUniqueRead      &= it->second.second.isUniqueRead;
			IO[name].second.isUniqueWrite     &= it->second.second.isUniqueWrite;
		} else {
			IO[name] = it->second;
		}

		// Atualizando o tipo do I/O
		IO[name].second.type = type;

		// Exclui o I/O antigo
		IO.erase(it);
	}

	updateGUI();

	return true;
}

unsigned long mapIO::Request(tRequestIO infoIO)
{
	bool result;
	unsigned long id = 0;

	infoIO.name = getValidName(infoIO.name);

	if(IsReserved(infoIO.name)) {
		infoIO.access        = eRequestAccessType_ReadWrite;
		infoIO.isUniqueRead  = false;
		infoIO.isUniqueWrite = false;
	}

	bool isRequestRead  = (infoIO.access == eRequestAccessType_Read ) || (infoIO.access == eRequestAccessType_ReadWrite);
	bool isRequestWrite = (infoIO.access == eRequestAccessType_Write) || (infoIO.access == eRequestAccessType_ReadWrite);

	// Se for variavel interna, o tipo obrigatoriamente deve ser Geral.
	if(IsInternalVar(infoIO.name) && infoIO.type != eType_General) {
		return 0;
	}

	// Verifica se podemos fazer o request conforme isUniqueRead e isUniqueWrite
	if(IO.count(infoIO.name) > 0 &&
		((isRequestRead  && (infoIO.isUniqueRead  || IO[infoIO.name].second.isUniqueRead ) && IO[infoIO.name].second.countRequestRead  > 0) ||
		 (isRequestWrite && (infoIO.isUniqueWrite || IO[infoIO.name].second.isUniqueWrite) && IO[infoIO.name].second.countRequestWrite > 0))) {
			return 0;
	}

	// Registro da acao para desfazer / refazer
	diagram->CheckpointBegin(_("Solicitar I/O"));

	if(IO.count(infoIO.name) > 0) {
		result = Update(getID(infoIO.name), infoIO.name, infoIO.type);
	} else {
		result = Add(infoIO.name, infoIO.type);
	}

	if(result) {
		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		id = getID(infoIO.name);

		data->Request.id            = id;
		data->Request.isBit         = infoIO.isBit;
		data->Request.isDiscard     = false;
		data->Request.accessType    = infoIO.access;
		// Para as flags, devemos salvar o estado atual para que possamos recupera-lo
		data->Request.isUniqueRead  = IO[infoIO.name].second.isUniqueRead;
		data->Request.isUniqueWrite = IO[infoIO.name].second.isUniqueWrite;

		action.action        = eRequest;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = diagram->getContext();
		action.data          = data;
		action.io            = this;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		diagram->RegisterAction(action);

		// Registra o tipo de acesso solicitado
		if(isRequestRead) {
			IO[infoIO.name].second.countRequestRead++;
		}
		if(isRequestWrite) {
			IO[infoIO.name].second.countRequestWrite++;
		}

		// Configura as flags indicando se o tipo de acesso eh unico
		if(infoIO.isUniqueRead) {
			IO[infoIO.name].second.isUniqueRead  = infoIO.isUniqueRead;
		}
		if(infoIO.isUniqueWrite) {
			IO[infoIO.name].second.isUniqueWrite = infoIO.isUniqueWrite;
		}

		// Registra o tipo de dado solicitado
		if(infoIO.isBit) {
			IO[infoIO.name].second.countRequestBit++;
		} else {
			IO[infoIO.name].second.countRequestInt++;
		}
	}

	diagram->CheckpointEnd();

	return id;
}

void mapIO::Discard(tRequestIO infoIO)
{
	string name = getName(infoIO.pin.first);
	if(name.size() > 0) {
		if(infoIO.isBit) {
			IO[name].second.countRequestBit--;
		} else {
			IO[name].second.countRequestInt--;
		}

		if(IO[name].second.type == eType_Reserved) {
			infoIO.access        = eRequestAccessType_ReadWrite;
			infoIO.isUniqueRead  = false;
			infoIO.isUniqueWrite = false;
		}

		// Decrementa a contagem do tipo de acesso solicitado
		bool isRequestRead  = (infoIO.access == eRequestAccessType_Read ) || (infoIO.access == eRequestAccessType_ReadWrite);
		bool isRequestWrite = (infoIO.access == eRequestAccessType_Write) || (infoIO.access == eRequestAccessType_ReadWrite);

		if(isRequestRead) {
			IO[name].second.countRequestRead--;
		}
		if(isRequestWrite) {
			IO[name].second.countRequestWrite--;
		}

		// Configura as flags indicando se o tipo de acesso eh unico
		if(infoIO.isUniqueRead) { // Descartando I/O que exigia acesso unico. Assim remove essa exigencia
			IO[name].second.isUniqueRead  = false;
		}
		if(infoIO.isUniqueWrite) { // Descartando I/O que exigia acesso unico. Assim remove essa exigencia
			IO[name].second.isUniqueWrite = false;
		}

		// Registro da acao para desfazer / refazer
		diagram->CheckpointBegin(_("Descartar I/O"));

		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->Request.id            = infoIO.pin.first;
		data->Request.isBit         = infoIO.isBit;
		data->Request.isDiscard     = true;
		data->Request.accessType    = infoIO.access;
		data->Request.isUniqueRead  = infoIO.isUniqueRead;
		data->Request.isUniqueWrite = infoIO.isUniqueWrite;

		action.action        = eRequest;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = diagram->getContext();
		action.data          = data;
		action.io            = this;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		diagram->RegisterAction(action);

		// Se Variavel nao usada, remove do mapa
		if(IO[name].second.countRequestBit == 0 && IO[name].second.countRequestInt == 0) {
			// Antes de descartar, salva no cache
			mapCachedIO[IO[name].first] = pair<string, eType>(name, IO[name].second.type);

			// Registra acao para Desfazer / Refazer a acao
			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			data->Discard.id        = infoIO.pin.first;
			data->Discard.name      = AllocCharFromString(name);
			data->Discard.detailsIO = getDetails(infoIO.pin.first);

			action.action        = eDiscard;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = diagram->getContext();
			action.data          = data;
			action.io            = this;
			action.elem          = nullptr;
			action.subckt        = nullptr;

			diagram->RegisterAction(action);

			tMapIO::iterator it;
			it = IO.find(name);
			IO.erase(it);

			updateGUI();
		}

		diagram->CheckpointEnd();
	}
}

unsigned long mapIO::getIndex(unsigned long id)
{
	unsigned long idx;
	vector<tVectorIO>::iterator it;

	for(idx = 1, it = vectorIO.begin(); it != vectorIO.end(); it++, idx++) {
		// Se encontrou, retorna o indice conforme modo do IO
		if(it->second.first == id) {
			return idx;
		}
	}

	return 0;
}

string mapIO::getName(unsigned long id)
{
	tMapIO::iterator it;

	for(it = IO.begin(); it != IO.end(); it++) {
		if(it->second.first == id) {
			return it->first;
		}
	}

	return "";
}

string mapIO::getInternalVarName(string name)
{
	vector<string>::size_type i;
	for(i = 0; i < vectorInternalVar.size(); i++) {
		if(name == vectorInternalVar[i].first) {
			return vectorInternalVar[i].second; // Encontrado!
		}
	}

	return "";
}

vector< pair<string, string> > mapIO::getInternalVarVector(void)
{
	return vectorInternalVar;
}

string mapIO::getReservedName(eType type)
{
	switch(type) {
		case eType_DigInput:			return _("entrada");
		case eType_InternalRelay:
        case eType_DigOutput:			return _("saida");
        default:                        return _("novo");
	}
}

mapDetails mapIO::getDetails(unsigned long id)
{
	string name = getName(id);

	if(name.size() > 0) {
		return IO[name].second;
	}

	mapDetails m = { 0, 0, 0, 0, false, false, eType_Pending, 0, 0 };
	return m;
}

unsigned long mapIO::getID(string name)
{
	name = getValidName(name);

	// Retorna zero se o nome nao estiver no mapa
	if(IO.count(name) == 0) {
		return 0;
	}

	return IO[name].first;
}

unsigned long mapIO::getID(unsigned int index, bool fromVectorIO)
{
	// Retorna zero se o indice for maior que o numero de elementos do mapa
	if(index >= IO.size()) {
		return 0;
	}

	if(fromVectorIO) {
		return vectorIO[index].second.first;
	} else {
		tMapIO::iterator it = IO.begin();

		while(index--) {
			it++;
		}

		return it->second.first;
	}
}

unsigned int mapIO::getCount(void)
{
	return IO.size();
}

vector<string> mapIO::getList(void)
{
	vector<string> listIO;

	tMapIO::iterator it;
	for(it = IO.begin(); it != IO.end(); it++) {
		listIO.push_back(it->first);
	}

	return listIO;
}

void mapIO::Select(unsigned int index)
{
	selectedIO = getID(index, true);
}

pair<string, eType> mapIO::getCachedIO(unsigned long id)
{
	if(mapCachedIO.count(id) > 0) {
		return mapCachedIO[id];
	}

	return pair<string, eType>("", eType_Pending);
}

string mapIO::getNextVar(string prefix)
{
	char buf[1024];
	sprintf(buf, "%s%d", prefix.c_str(), seqNo++);

	return string(buf);
}

bool mapIO::Assign(unsigned long id, unsigned int pin, unsigned int bit, bool isUndoRedo)
{
	string name = getName(id);

	if(name.size() > 0) {
		return Assign(name, pin, bit, isUndoRedo);
	}

	return false;
}

bool mapIO::Assign(string name, unsigned int pin, unsigned int bit, bool isUndoRedo)
{
	name = getValidName(name);

	if(IO.count(name) > 0) {
		// Registro da acao para desfazer / refazer
		if(!isUndoRedo) {
			diagram->CheckpointBegin(_("Associar I/O"));

			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			data->Assign.id  = IO[name].first;
			data->Assign.pin = IO[name].second.pin;
			data->Assign.bit = IO[name].second.bit;

			action.action        = eAssign;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = diagram->getContext();
			action.data          = data;
			action.io            = this;
			action.elem          = nullptr;
			action.subckt        = nullptr;

			diagram->RegisterAction(action);

			diagram->CheckpointEnd();
		}

		// Associa o I/O
		IO[name].second.pin = pin;
		IO[name].second.bit = bit;

		// Indica que houve alteracao no programa
		diagram->ProgramChanged();

		return true;
	}

	return false;
}

string mapIO::getPortName(int index)
{
	char buf[1024];
	McuIoInfo *mcu = diagram->getMCU();
	mapDetails detailsIO = getDetails(getID(index, true));

	if((detailsIO.type != eType_DigInput && detailsIO.type != eType_DigOutput && detailsIO.type != eType_General
		&& detailsIO.type != eType_ReadADC && detailsIO.type != eType_ReadEnc && detailsIO.type != eType_ResetEnc
		&& detailsIO.type != eType_ReadUSS && detailsIO.type != eType_WriteUSS) || detailsIO.pin == 0 || mcu == nullptr)
	{
		return string("");
	}

	if(UartFunctionUsed() && mcu != nullptr) {
		if((mcu->uartNeeds.rxPin == detailsIO.pin) ||
			(mcu->uartNeeds.txPin == detailsIO.pin))
		{
			return string(_("<UART necessária!>"));
		}
	}

	if(PwmFunctionUsed() && mcu != nullptr) {
		if(mcu->pwmNeedsPin == detailsIO.pin && detailsIO.type == eType_DigOutput) {
			return string(_("<PWM necessário!>"));
		}
	}

	if(detailsIO.pin < pinOffset) {
		if((detailsIO.type == eType_DigOutput || detailsIO.type == eType_DigInput) && detailsIO.pin > 19)
			sprintf(buf, "M%d.%d", detailsIO.pin - 20, detailsIO.bit);
		else if (detailsIO.type == eType_DigOutput)
			sprintf(buf, "S%d", detailsIO.pin);
		else if (detailsIO.type == eType_General)
			sprintf(buf, "M%d", detailsIO.pin - 20);
		else if (detailsIO.type == eType_DigInput)
			sprintf(buf, "E%d", detailsIO.pin);
		else if (detailsIO.type == eType_ReadADC)
			if (detailsIO.pin == 6)
				strcpy(buf, "TEMP");
			else
				sprintf(buf, "AD%d", detailsIO.pin);
		else if (detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc)
			strcpy(buf, detailsIO.pin == 1 ? _("Enc. Inc.") : _("Enc. Abs."));
		else
			strcpy(buf, _("<Não é uma E/S!>"));
	} else {
		unsigned int boardId = detailsIO.pin / pinOffset;
		string boardName = diagram->getBoardById(boardId).name;

		detailsIO.pin -= (boardId * pinOffset);

		if(detailsIO.type == eType_DigOutput)
			sprintf(buf, "%s.S%d", boardName.c_str(), detailsIO.pin);
		else if (detailsIO.type == eType_DigInput)
			sprintf(buf, "%s.E%d", boardName.c_str(), detailsIO.pin);
		else if (detailsIO.type == eType_ReadADC)
			sprintf(buf, "%s.AD%d", boardName.c_str(), detailsIO.pin);
		else if (detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc)
			sprintf(buf, "%s.ENC%d", boardName.c_str(), detailsIO.pin);
		else
			strcpy(buf, _("<Não é uma E/S!>"));
	}

	return string(buf);
}

string mapIO::getPinName(int index)
{
	char buf[1024];
	mapDetails detailsIO = getDetails(getID(index, true));

	if(detailsIO.pin != 0) {
		sprintf(buf, "%d", detailsIO.pin);
	} else if(detailsIO.type == eType_DigInput || detailsIO.type == eType_DigOutput ||
		detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc ||
		detailsIO.type == eType_ReadADC) {
			strcpy(buf, _("(sem atribuição)"));
	} else {
			strcpy(buf, "");
	}

	return string(buf);
}

int mapIO::getPinFromDialogList(string name)
{
	unsigned int idx;

	for (idx = 0; idx < vecDialogItemList.size(); idx++) {
		if(name == vecDialogItemList[idx].first) {
			return vecDialogItemList[idx].second;
		}
	}

	return 0;
}

vector<string> mapIO::getVectorInternalFlags(void)
{
	return vectorInternalFlag;
}

void mapIO::Sort(eSortBy sortby)
{
	if(sortby != eSortBy_Current) {
		// get new sort parameters. If NewSortColumn < 0, use previous settings
		if (currentSortBy == sortby) {
			currentSortAscending = !currentSortAscending;
		} else {
			currentSortBy        = sortby;
			currentSortAscending = true;
		}
	}

	gSortAscending = currentSortAscending;

	switch(currentSortBy) {
		case eSortBy_Name: sort(vectorIO.begin(), vectorIO.end(), CompareIoName<tVectorIO>()); break;
		case eSortBy_Type: sort(vectorIO.begin(), vectorIO.end(), CompareIoType<tVectorIO>()); break;
		case eSortBy_Pin : sort(vectorIO.begin(), vectorIO.end(), CompareIoPin <tVectorIO>()); break;
		case eSortBy_Port: sort(vectorIO.begin(), vectorIO.end(), CompareIoPort<tVectorIO>()); break;
	}
}

bool mapIO::Validate(eValidateIO mode)
{
	char buf[1024];
	bool ret = true, isError = false;
	tMapIO::iterator it;

	for(it = IO.begin(); it != IO.end(); it++) {
		buf[0] = 0; // Limpa o buffer
		isError = false;

		if(it->second.second.pin == 0 && mode == eValidateIO_Full) {
			isError = true; // Marca como se estivesse com erro para nao ter que fazer isso em cada case. No default arruma!
			switch(it->second.second.type) {
			case eType_ReadADC:
				sprintf(buf, _("Variável A/D '%s' deve ser associado a um canal válido!"), it->first.c_str());
				break;
			case eType_ReadEnc:
				sprintf(buf, _("Leitura de Encoder '%s' deve ser associada a um canal válido!"), it->first.c_str());
				break;
			case eType_ResetEnc:
				sprintf(buf, _("Escrita de Encoder '%s' deve ser associada a um canal válido!"), it->first.c_str());
				break;
			case eType_DigInput:
			case eType_DigOutput:
				sprintf(buf, _("Deve associar pinos a todas E/S.\r\n\r\n'%s' não está associado."), it->first.c_str());
				break;
			default:
				// Se chegamos aqui, nao existe erro. Limpa a flag!
				isError = false;
			}
		}

		if(!strlen(buf) && IsReserved(it->first) && mode == eValidateIO_Full) {
			isError = true; // Marca como erro pois nao podemos aceitar compilar com variaveis reservadas!
			sprintf(buf, _("Variável '%s' reservada! Favor alterar para um nome válido"), it->first.c_str());
		}

		if(!isError && it->second.second.type != eType_Reserved && it->second.second.type != eType_InternalFlag) {
			if(it->second.second.countRequestRead  == 0 && it->second.second.type != eType_DigOutput && !it->second.second.pin) { // Nao tem leitura!
				sprintf(buf, _("Variável '%s' não é lida, apenas escrita!"), it->first.c_str());
			} else if(it->second.second.countRequestWrite == 0 && it->second.second.type != eType_DigInput && !it->second.second.pin) { // Nao tem escrita!
				sprintf(buf, _("Variável '%s' não é escrita, apenas lida!"), it->first.c_str());
			}
		}

		if(strlen(buf)) {
			if(diagram->ShowValidateDialog(isError, buf) == eReply_Cancel || isError) {
				ret = false; // Se foi cancelado ou foi erro, marca a flag para indicar o erro na saida
				break; // Usuario interrompeu!
			}
		}
	}

	return ret;
}

void mapIO::updateType(unsigned long id)
{
	string name = getName(id);
	if(IsReserved(name)) {
		IO[name].second.type = eType_Reserved;
	}
}

void mapIO::updateAssignments(void)
{
	tMapIO::iterator it;

	diagram->CheckpointBegin(_("Atualizar atribuições"));

	for(it = IO.begin(); it != IO.end(); it++) {
		unsigned long id = it->second.second.pin / 100;
		if(id > 0) {
			// id associado a placa de expansao, devemos checar se ela existe
			tExpansionBoardItem board = diagram->getBoardById(id);
			if(!board.id) {
				// A placa nao existe, id retornado foi zero!
				Assign(it->second.first, 0, 0); // Remove a associacao
			}
		}
	}

	diagram->CheckpointEnd();
}

bool mapIO::Save(FILE *f)
{
	// Primeiro grava a variavel que controla os IDs dos I/Os
	if(fwrite_ulong(f, countIO) && fwrite_uint(f, IO.size())) {
		tMapIO::iterator it;

		for(it = IO.begin(); it != IO.end(); it++) {
			if(
				!fwrite_string(f, it->first) ||
				!fwrite_ulong (f, it->second.first) || 
				!fwrite_uint  (f, it->second.second.countRequestBit) || 
				!fwrite_uint  (f, it->second.second.countRequestInt) || 
				!fwrite_uint  (f, it->second.second.countRequestRead) || 
				!fwrite_uint  (f, it->second.second.countRequestWrite) || 
				!fwrite_bool  (f, it->second.second.isUniqueRead) || 
				!fwrite_bool  (f, it->second.second.isUniqueWrite) || 
				!fwrite_uint  (f, it->second.second.type) || 
				!fwrite_uint  (f, it->second.second.pin) || 
				!fwrite_uint  (f, it->second.second.bit)) {
					break; // Erro na gravacao!
			}
		}

		if(it == IO.end()) {
			return true; // alcancou IO.end(), significa que gravou com sucesso!
		}
	}

	return false;
}

bool mapIO::Load(FILE *f, unsigned int version)
{
	unsigned int size;

	Clear();

	// Primeiro grava a variavel que controla os IDs dos I/Os
	if(fread_ulong(f, &countIO) && fread_uint(f, &size)) {
		unsigned int  i;
		string        name;
		unsigned int  iType;
		unsigned long pinID;
		mapDetails    detailsIO;

		for(i = 0; i < size; i++) {
			if(
				!fread_string(f, &name) ||
				!fread_ulong (f, &(pinID)) || 
				!fread_uint  (f, &(detailsIO.countRequestBit)) || 
				!fread_uint  (f, &(detailsIO.countRequestInt)) || 
				!fread_uint  (f, &(detailsIO.countRequestRead)) || 
				!fread_uint  (f, &(detailsIO.countRequestWrite)) || 
				!fread_bool  (f, &(detailsIO.isUniqueRead)) || 
				!fread_bool  (f, &(detailsIO.isUniqueWrite)) || 
				!fread_uint  (f, &iType) || 
				!fread_uint  (f, &(detailsIO.pin)) || 
				!fread_uint  (f, &(detailsIO.bit))) {
					break; // Erro na leitura!
			} else {
				switch(iType) {
					default:
					case eType_Pending      : detailsIO.type = eType_Pending      ; break;
					case eType_Reserved     : detailsIO.type = eType_Reserved     ; break;
					case eType_General      : detailsIO.type = eType_General      ; break;
					case eType_DigInput     : detailsIO.type = eType_DigInput     ; break;
					case eType_InternalRelay: detailsIO.type = eType_InternalRelay; break;
					case eType_DigOutput    : detailsIO.type = eType_DigOutput    ; break;
					case eType_InternalFlag : detailsIO.type = eType_InternalFlag ; break;
					case eType_Counter      : detailsIO.type = eType_Counter      ; break;
					case eType_TOF          : detailsIO.type = eType_TOF          ; break;
					case eType_TON          : detailsIO.type = eType_TON          ; break;
					case eType_RTO          : detailsIO.type = eType_RTO          ; break;
					case eType_ReadADC      : detailsIO.type = eType_ReadADC      ; break;
					case eType_SetDAC       : detailsIO.type = eType_SetDAC       ; break;
					case eType_ReadEnc      : detailsIO.type = eType_ReadEnc      ; break;
					case eType_ResetEnc     : detailsIO.type = eType_ResetEnc     ; break;
					case eType_ReadUSS      : detailsIO.type = eType_ReadUSS      ; break;
					case eType_WriteUSS     : detailsIO.type = eType_WriteUSS     ; break;
					case eType_ReadModbus   : detailsIO.type = eType_ReadModbus   ; break;
					case eType_WriteModbus  : detailsIO.type = eType_WriteModbus  ; break;
					case eType_PWM          : detailsIO.type = eType_PWM          ; break;
					case eType_RxUART       : detailsIO.type = eType_RxUART       ; break;
					case eType_TxUART       : detailsIO.type = eType_TxUART       ; break;
					case eType_ReadYaskawa  : detailsIO.type = eType_ReadYaskawa  ; break;
					case eType_WriteYaskawa : detailsIO.type = eType_WriteYaskawa ; break;
				}

				IO[name].first  = pinID;
				IO[name].second = detailsIO;
			}
		}

		if(i == size) {
			return true; // alcancou IO.end(), significa que gravou com sucesso!
		}
	}

	// Ocorreu erro! Limpa a lista que estava sendo carregada
	Clear();

	return false;
}

//-----------------------------------------------------------------------------
// Map an I/O type to a string describing it. Used both in the on-screen
// list and when we write a text file to describe it.
//-----------------------------------------------------------------------------
const char *mapIO::getTypeString(eType type)
{
    switch(type) {
        case eType_Reserved:			return _("reservado");
		case eType_DigInput:			return _("entrada digital");
        case eType_DigOutput:			return _("saída digital");
		case eType_InternalRelay:		return _("rele interno");
        case eType_TxUART:				return _("UART tx");
		case eType_RxUART:				return _("UART rx");
        case eType_PWM:					return _("saída PWM");
		case eType_TON:					return _("atraso p/ ativar");
		case eType_TOF:					return _("atraso p/ desativar");
        case eType_RTO:					return _("atraso com memória");
        case eType_Counter:				return _("contador");
		case eType_General:				return _("var geral");
        case eType_ReadADC:				return _("entrada adc");
        case eType_ReadEnc:				return _("entrada encoder");
        case eType_ResetEnc:			return _("escrever encoder");
        case eType_ReadUSS:				return _("ler USS");
		case eType_WriteUSS:			return _("escrever USS");
		case eType_SetDAC:				return _("Set D/A");
		case eType_ReadModbus:			return _("leitura modbus");
		case eType_WriteModbus:			return _("escrita modbus");
		case eType_ReadYaskawa:			return _("leitura NS600");
		case eType_WriteYaskawa:		return _("escrita NS600");
		case eType_InternalFlag:		return _("flag interna");
		case eType_Pending:				return _("Sem tipo");
        default:                        return "";
    }
}

// Funcao que executa uma acao de desfazer / refazer
bool mapIO::DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	UndoRedoData *data = (UndoRedoData *)action.data;
	switch(action.action) {
	case eAssign: {
		if(isDiscard) {
			// Nada a fazer...
		} else {
			mapDetails detailsIO = getDetails(data->Assign.id);
			Assign(data->Assign.id, data->Assign.pin, data->Assign.bit, true);

			// Atualiza a estrutura para se desfazer essa acao
			data->Assign.pin = detailsIO.pin;
			data->Assign.bit = detailsIO.bit;

			// Atualiza a tela para que se exiba corretamente a lista de I/Os
			updateGUI();
		}
		break;
	}

	case eRequest: {
		if(isDiscard) {
			// Nada a fazer...
		} else {
			// Calcula o offset conforme o tipo de operacao
			int offset = data->Request.isDiscard ? -1 : +1;
			if(IsUndo) {
				offset *= -1;
			}

			// Atualiza contagem de solicitacao de tipo de dado
			if(data->Request.isBit) {
				IO[getName(data->Request.id)].second.countRequestBit += offset;
			} else {
				IO[getName(data->Request.id)].second.countRequestInt += offset;
			}

			// Atualiza contagem de solicitacao de tipo de acesso - leitura
			if(data->Request.accessType == eRequestAccessType_Read ||
				data->Request.accessType == eRequestAccessType_ReadWrite) {
					IO[getName(data->Request.id)].second.countRequestRead += offset;
			}

			// Atualiza contagem de solicitacao de tipo de acesso - escrita
			if(data->Request.accessType == eRequestAccessType_Write ||
				data->Request.accessType == eRequestAccessType_ReadWrite) {
					IO[getName(data->Request.id)].second.countRequestWrite += offset;
			}

			// Atualiza flags de permissao de acesso
			IO[getName(data->Request.id)].second.isUniqueRead  = data->Request.isUniqueRead;
			IO[getName(data->Request.id)].second.isUniqueWrite = data->Request.isUniqueWrite;
		}
		break;
	}

	case eAdd: {
		if(isDiscard) {
			delete [] data->Add.name; // Descarta o buffer com o nome do I/O
		} else if(IsUndo) {
			// Ao desfazer a criacao de um I/O tambem devemos copia-lo para o cache pois um objeto
			// na area de transferencia pode ter referencia a ele
			mapCachedIO[IO[data->Add.name].first] = pair<string, eType>(data->Add.name, data->Add.type);

			IO.erase(IO.find(data->Add.name));
			countIO--;
		} else {
			Add(data->Add.name, data->Add.type, true);
		}

		// Atualiza a tela para que se exiba corretamente a lista de I/Os
		updateGUI();

		break;
	}

	case eUpdate: {
		if(isDiscard) {
			delete [] data->Update.name; // Descarta o buffer com o nome do I/O
		} else {
			char       *name      = AllocCharFromString(getName(data->Update.id));
			mapDetails  detailsIO = getDetails(data->Update.id);

			Update(data->Update.id, data->Update.name, data->Update.type, true);
			IO[data->Update.name].second.pin = data->Update.pin;

			if(name != nullptr) {
				delete [] data->Update.name; // Descarta o buffer com o nome (agora atual) do I/O
				data->Update.name    = name; // Salva o nome anterior para a operacao de Refazer
				data->Update.type    = detailsIO.type; // Atualiza o tipo para o anterior
				data->Update.pin     = detailsIO.pin; // Atualiza o pino pois se houve alteracao de tipo, ele foi zerado
			}
		}

		// Atualiza a tela para que se exiba corretamente a lista de I/Os
		updateGUI();

		break;
	}

	case eDiscard: {
		if(isDiscard) {
			delete [] data->Discard.name; // Descarta o buffer com o nome do I/O
		} else if(IsUndo) {
			IO[data->Discard.name].first  = data->Discard.id;
			IO[data->Discard.name].second = data->Discard.detailsIO;
		} else {
			tMapIO::iterator it;
			it = IO.find(data->Discard.name);
			IO.erase(it);
		}

		// Atualiza a tela para que se exiba corretamente a lista de I/Os
		updateGUI();

		break;
	}

	default: return false;
	}

	// Se estamos descartando, desaloca a estrutura que representa a acao
	if(isDiscard) {
		delete data;
	}

	return true; // Nada mais a fazer
}

// stuff for the dialog box that lets you choose pin assignments
static BOOL DialogDone;
static BOOL DialogCancel;

static HWND IoDialog;

static HWND PinList;
static HWND lblBit;
static HWND OkButton;
static HWND CancelButton;
static HWND BitCombobox;
static HWND BoardCombobox;
static HWND textLabelName;

// stuff for the popup that lets you set the simulated value of an analog in
static BOOL SliderDone;
static BOOL SliderCancel;

const LPCTSTR ComboboxBitItens[] = { "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", "10",
									"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
									"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
									"31"};

// Funcao para adicionar um item para a lista de I/O da janela de atribuicao
void mapIO::AddDialogItem(string name, int pin)
{
	// Ao adicionar um pino zerado, indica que estamos iniciando uma nova lista
	// e assim devemos excluir os itens ja existentes.
	if(!pin) {
		vecDialogItemList.clear();
		SendMessage(PinList, LB_RESETCONTENT, 0, 0);
	}

	const unsigned int centeredTextLength = 12;
	char centeredText[centeredTextLength + 1];
	memset(centeredText, ' ', centeredTextLength);
	centeredText[centeredTextLength] = 0;

	if(name.size() > centeredTextLength) {
		name.resize(centeredTextLength);
	}

	strncpy(centeredText + (centeredTextLength - name.size())/2, name.c_str(), name.size());

	vecDialogItemList.push_back(pair<string, int>(centeredText, pin));
	SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)centeredText);
}

void mapIO::LoadPinList(void)
{
	LoadPinList(0);
}

// Funcao que carrega uma lista de I/Os conforme placa selecionada
void mapIO::LoadPinList(unsigned long id)
{
	static unsigned long lastId = 0;
	if(id == 0) {
		id = lastId;
	} else {
		lastId = id;
	}

	unsigned int currentBoard;
	McuIoInfo *mcu = diagram->getMCU();
	mapDetails detailsIO = getDetails(id);
	string name = getName(id);

	if(mcu == nullptr) {
		ladder->ShowDialog(eDialogType_Error, false, _("Atribuição de Pinos de E/S"), _("Nenhum dispositivo selecionado!"));
        return;
    }

    char buf[40];
	int i = 0;
	bool canUseIO;

	AddDialogItem(_("(sem pino)"), 0);
	currentBoard = SendMessage(BoardCombobox, CB_GETCURSEL, 0, 0);

	// Carrega bits disponiveis no combobox de bits.
	SendMessage(BitCombobox, CB_SETCURSEL, LoadUnusedBits(name, detailsIO.pin), 0);

	if(currentBoard == 0) { // CLP
		if(detailsIO.pin >= 20 && ComboBox_GetCount(BitCombobox) > 0) {
			EnableWindow(BitCombobox, TRUE);
		}

		// Apenas verificamos se os pinos de entrada / saida ja sao utilizados
		// A/D, Encoder, etc. podem ser atribuidos a mais de 1 objeto
		if(detailsIO.type == eType_ReadADC) {
			for(i = 0; i < mcu->adcCount; i++) 
			{
				if (i == 4) continue;
				if (i == mcu->adcCount - 1)
					strcpy(buf, "TEMP");
				else
					sprintf(buf, "ADC%d", mcu->adcInfo[i].muxRegValue);

				AddDialogItem(buf, mcu->adcInfo[i].pin);
			}
		} else if(detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc) {
			for(i = 0; i < mcu->encCount; i++) {
				const char *strEnc[] = { _("Enc. Inc."), _("Enc. Abs.") };
				AddDialogItem(strEnc[i], mcu->encInfo[i].pin);
			}
		} else if (detailsIO.type  == eType_DigInput  ||
					detailsIO.type == eType_DigOutput ||
					detailsIO.type == eType_General) {

			tMapIO::iterator it;
			for(i = 0; i < mcu->pinCount; i++) {
				canUseIO = true;

				// Primeiro identificamos se o pino ja esta sendo utilizado
				for(it = IO.begin(); it != IO.end(); it++) 
				{
					if(it->second.first == id) continue;
					if(it->second.second.pin == mcu->pinInfo[i].pin && it->second.second.type == detailsIO.type && 
						!((it->second.second.type == eType_DigInput && i > 18) || (it->second.second.type == eType_DigOutput && i > 66)) )
					{
						canUseIO = false;
						break;
					}
				}

				if(canUseIO && (detailsIO.type == eType_DigInput  && i < 51) ||
								(detailsIO.type == eType_DigOutput && i > 50) ||
								(detailsIO.type == eType_General   && i > 66)) {
					sprintf(buf, "%c%d", mcu->pinInfo[i].port, mcu->pinInfo[i].bit);
					AddDialogItem(buf, mcu->pinInfo[i].pin);
				}
			}
		}
	} else {
		vector<tExpansionBoardItem>::iterator itBoard;
		vector<tExpansionBoardItem> boardList = diagram->getBoardList();

		char nameBoard[1024];
        SendMessage(BoardCombobox, WM_GETTEXT, (WPARAM)sizeof(nameBoard), (LPARAM)(nameBoard));
		string sNameBoard = string(nameBoard);

		for(itBoard = boardList.begin(); itBoard != boardList.end(); itBoard++) {
			if(itBoard->name == sNameBoard) break; // Placa encontrada!
		}

		if(itBoard != boardList.end()) {
			unsigned int i, qtdIO = diagram->getBoardQtyIO(itBoard->type, detailsIO.type);
			unsigned int pinStart = itBoard->id * pinOffset;
			unsigned int pin;

			tMapIO::iterator it;
			for(i = 0; i < qtdIO; i++) {
				canUseIO = true;

				// Calculo do numero do pino conforme a placa de expansao
				pin = pinStart + i + 1;

				if (detailsIO.type == eType_DigInput || detailsIO.type == eType_DigOutput) {
					for(it = IO.begin(); it != IO.end(); it++) 
					{
						if(it->second.first == id) continue;
						if(it->second.second.pin == pin && it->second.second.type == detailsIO.type) {
							canUseIO = false;
						}
					}

					// Se chegou aqui, pino atual nao utilizado
					char expPort = (detailsIO.type == eType_DigInput) ? 'E' : 'S';
					if(canUseIO) {
						sprintf(buf, "%c%d", expPort, i+1);
						AddDialogItem(buf, pin);
					}
				}
			}
		}
	}

	SetFocus(PinList);

	unsigned int idx;
	for (idx = 0; idx < vecDialogItemList.size(); idx++)
	{
		if (detailsIO.pin == vecDialogItemList[idx].second)
			SendMessage(PinList, LB_SETCURSEL, idx, 0);
	}

	if(idx == 1) { // Lista de pinos vazia. Sempre tera pelo menos 1, o item (Sem Pino)
		if(detailsIO.type == eType_ReadADC) {
			Error(_("O dispositivo selecionado não possui ADC ou não suporta."));
		} else if(detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc) {
			Error(_("No Encoder or Encoder not supported for selected micro."));
		} else {
			Error(_("Não existem pinos disponíveis"));
		}
	}
}

// Funcao para carregar no combobox de bits da janela de atribuicao apenas os bits nao utilizados,
// impedindo que sejam usado por mais de um I/O ao mesmo tempo.
unsigned int mapIO::LoadUnusedBits(string name, int pin)
{
	unsigned int idx = 0, i = 0, mask = 0, posList = 0;
	mapDetails detailsIO = getDetails(getID(name));

	ComboBox_ResetContent(BitCombobox);

	for(i = 0; i < vectorIO.size(); i++) {
		if(vectorIO[i].second.second.pin == pin && vectorIO[i].second.second.type != eType_General &&
			(vectorIO[i].second.second.pin != detailsIO.pin || vectorIO[i].second.second.bit != detailsIO.bit)) {
			mask |= 1UL << vectorIO[i].second.second.bit;
		}
	}

	for(i = 0; i < sizeof(ComboboxBitItens) / sizeof(ComboboxBitItens[0]); i++) {
		if(!((mask >> i) & 1)) {
			SendMessage(BitCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBitItens[i]));
			if(i == detailsIO.bit) {
				posList = idx;
			}
			idx++;
		}
	}

	return posList;
}

void SortList(int NewSortColumn)
{
	// sort list
	eSortBy sortby = eSortBy_Nothing;

	switch(NewSortColumn) {
		case 0: sortby = eSortBy_Name; break;
		case 1: sortby = eSortBy_Type; break;
		case 3: sortby = eSortBy_Pin ; break;
		case 4: sortby = eSortBy_Port; break;
	}

	if(sortby != eSortBy_Nothing) {
		ladder->sortIO(sortby);
		RefreshControlsToSettings();
	}
}

//-----------------------------------------------------------------------------
// Window proc for the contacts dialog box
//-----------------------------------------------------------------------------
static LRESULT CALLBACK IoDialogProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
        case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == OkButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
            } else if(h == CancelButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
            } else if(h == PinList && HIWORD(wParam) == LBN_DBLCLK) {
                DialogDone = TRUE;
			} else if (h == PinList && HIWORD(wParam) == LBN_SELCHANGE) {
				bool sem_bit;
				char pin_name[MAX_NAME_LEN], name[MAX_NAME_LEN];
				SendMessage(textLabelName, WM_GETTEXT, (WPARAM)MAX_NAME_LEN, (LPARAM)(name));

				SendMessage(PinList, LB_GETTEXT,
					(WPARAM)SendMessage(PinList, LB_GETCURSEL, 0, 0), (LPARAM)pin_name);

				int pin = ladder->getPinFromDialogListIO(pin_name);
				unsigned int bit = ladder->LoadUnusedBitsIO(name, pin);
				sem_bit = (pin < 20) || (ComboBox_GetCount(BitCombobox) == 0);
				SendMessage(BitCombobox, CB_SETCURSEL, sem_bit ? 0 : bit, 0);
				EnableWindow(BitCombobox, sem_bit ? FALSE : TRUE);
			}
            break;
        }

        case WM_CLOSE:
        case WM_DESTROY:
            DialogDone = TRUE;
            DialogCancel = TRUE;
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

//-----------------------------------------------------------------------------
// Create our window class; nothing exciting.
//-----------------------------------------------------------------------------
static BOOL MakeWindowClass()
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)IoDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "POPToolsIo";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 32, 32, 0);
    wc.hIconSm          = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 16, 16, 0);

    return RegisterClassEx(&wc);
}

static void MakeControls(void)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Atribua:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        6, 1, 50, 17, IoDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabelName = CreateWindowEx(0, WC_STATIC, "?",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        57, 1, 64, 17, IoDialog, NULL, Instance, NULL);
    NiceFont(textLabelName);
	
    lblBit = CreateWindowEx(0, WC_STATIC, _("Placa:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        6, 25, 50, 21, IoDialog, NULL, Instance, NULL);
    NiceFont(lblBit);

	BoardCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | WS_DISABLED,
        55, 23, 66, 140, IoDialog, NULL, Instance, NULL);
    NiceFont(BoardCombobox);

    PinList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTBOX, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | WS_VSCROLL |
        LBS_NOTIFY, 6, 53, 115, 280, IoDialog, NULL, Instance, NULL);
    FixedFont(PinList);
	
    lblBit = CreateWindowEx(0, WC_STATIC, _("Bit:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        6, 332, 35, 21, IoDialog, NULL, Instance, NULL);
    NiceFont(lblBit);

	BitCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | WS_DISABLED,
        55, 330, 66, 140, IoDialog, NULL, Instance, NULL);
    NiceFont(BitCombobox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        10, 365, 107, 23, IoDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        10, 396, 107, 23, IoDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

static LRESULT CALLBACK BoardProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) 
	{
		case WM_COMMAND:
			if ( HIWORD(wParam) == CBN_SELCHANGE) {
				ladder->LoadIoPinList();
			}
			break;
	}

	return CallWindowProc((WNDPROC)PrevBoardProc, hwnd, msg, wParam, lParam);
}

void mapIO::ShowIoMapDialog(int item)
{
	McuIoInfo *mcu = diagram->getMCU();
	unsigned long id = getID(item, true);
	mapDetails detailsIO = getDetails(id);

	if(mcu == nullptr) {
		ladder->ShowDialog(eDialogType_Error, false, _("Atribuição de Pinos de E/S"), _("Nenhum dispositivo selecionado!"));
        return;
    }

	string name = getName(id);
	if(IsReserved(name)) {
		diagram->SearchAndReplace(name, "", eSearchAndReplaceMode_FindFirst);
        Error(_("Renomear as E/S para um nome diferente do padrão ('%s') antes de atribuir um pino."), getName(id).c_str());
        return;
    }

	if(IsInternalFlag(getName(id)) || IsInternalVar(getName(id))) {
        Error(_("Variáveis internas não podem ser atribuídas."));
        return;
    }

    if(detailsIO.type != eType_DigInput && 
       detailsIO.type != eType_DigOutput &&
       detailsIO.type != eType_General  &&
	   detailsIO.type != eType_ReadADC &&
       detailsIO.type != eType_ReadEnc &&
	   detailsIO.type != eType_ResetEnc)
    {
        Error(_("Somente pode atribuir números dos pinos aos pinos de Entrada/Saída ou Variável de Uso Geral."));
        return;
    }

    MakeWindowClass();

	POINT size  = { 127, 430 };
	POINT start = getWindowStart(size);

    // We need the TOOLWINDOW style, or else the window will be forced to
    // a minimum width greater than our current width. And without the
    // APPWINDOW style, it becomes impossible to get the window back (by
    // Alt+Tab or taskbar).
    IoDialog = CreateWindowClient(WS_EX_TOOLWINDOW | WS_EX_APPWINDOW,
        "POPToolsIo", _("Pino E/S"),
        WS_OVERLAPPED | WS_SYSMENU,
        start.x, start.y, size.x, size.y, MainWindow, NULL, Instance, NULL);

    MakeControls();

	SendMessage(BitCombobox, CB_SETCURSEL, LoadUnusedBits(name, detailsIO.pin), 0);
	if(detailsIO.pin >= 20 && ComboBox_GetCount(BitCombobox) > 0) {
		EnableWindow(BitCombobox, TRUE);
	}

	if(detailsIO.type != eType_DigInput && detailsIO.type != eType_DigOutput) {
		ShowWindow(BitCombobox, SW_HIDE);
		ShowWindow(lblBit     , SW_HIDE);

		RECT r, rOK;
		GetWindowRect(PinList , &r  );
		GetWindowRect(OkButton, &rOK);

		r.bottom = rOK.top - 6;

		SetWindowPos(PinList, NULL, 6, 18, r.right - r.left, r.bottom - r.top, SWP_NOZORDER);
	}

	SendMessage(textLabelName, WM_SETTEXT, 0, (LPARAM)(getName(id).c_str()));

	unsigned int i = 0, idx = 0, currentId = detailsIO.pin / pinOffset;
	vector<tExpansionBoardItem>::iterator itBoard;
	vector<tExpansionBoardItem> boardList = diagram->getBoardList();

	SendMessage(BoardCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)"POP-7"));
	for(itBoard = boardList.begin(); itBoard != boardList.end(); itBoard++) {
		unsigned int qtyIO = diagram->getBoardQtyIO(itBoard->type, detailsIO.type);
		if(qtyIO > 0) {
			i++;
			SendMessage(BoardCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)itBoard->name.c_str()));
			if(itBoard->id == currentId) {
				idx = i;
			}
		}
	}
	SendMessage (BoardCombobox, CB_SETCURSEL, idx, 0);
	EnableWindow(BoardCombobox, TRUE);

	LoadPinList(id);

	EnableWindow(MainWindow, FALSE);
    ShowWindow(IoDialog, TRUE);
    SetFocus(PinList);

	//SendMessage(PinList, LB_SETCURSEL, item, 0);
	for (idx = 0; idx < vecDialogItemList.size(); idx++)
	{
		if (detailsIO.pin == vecDialogItemList[idx].second)
			SendMessage(PinList, LB_SETCURSEL, idx, 0);
	}

	PrevBoardProc = SetWindowLongPtr(BoardCombobox, GWLP_WNDPROC, (LONG_PTR)BoardProc);

	MSG msg;
    DWORD ret;
    DialogDone = FALSE;
    DialogCancel = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                DialogDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
                break;
            }
        } else if(msg.message == WM_COMMAND) {
			if (HIWORD(msg.wParam) == CBN_SELCHANGE) {
				LoadPinList(id);
			}
		}

        if(IsDialogMessage(IoDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
		int currentBoard;
        int sel = SendMessage(PinList, LB_GETCURSEL, 0, 0);
		int  pin_number = 0;
        char pin_name[16];
		char buf[16];

		SendMessage(PinList    , LB_GETTEXT, (WPARAM)sel        , (LPARAM)pin_name);
		SendMessage(BitCombobox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));
		currentBoard = SendMessage(BoardCombobox, CB_GETCURSEL, 0, 0);

		for (idx = 0; idx < vecDialogItemList.size(); idx++)
		{
			if(!strcmp(pin_name, vecDialogItemList[idx].first.c_str())) {
				pin_number = vecDialogItemList[idx].second;
				break;
			}
		}

		tMapIO::iterator it = IO.end();
		if(pin_number != 0) {
			for(it = IO.begin(); it != IO.end(); it++) {
				if(it->second.second.pin == pin_number && it->second.second.bit == atoi(buf)) {
					break;
				}
			}
		}

		if(it == IO.end()) {
			Assign(id, pin_number, atoi(buf));

			updateGUI();
		} else {
			Error(_("I/O já utilizado"));
		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(IoDialog);
	SetFocus(IoList);
}

// ShowWatchPoint Dialog functions
HWND CheckON;
HWND CheckOFF;
HWND CheckVal;
HWND EnabledWP;

static LONG_PTR PrevWPDialogProc;

void SetWPControlsEnabled()
{
	BOOL enabled = (Button_GetCheck(EnabledWP) == BST_CHECKED ? TRUE : FALSE);

	if(CheckON  != NULL)
		Button_Enable(CheckON , enabled);
	if(CheckOFF != NULL)
		Button_Enable(CheckOFF, enabled);
	if(CheckVal != NULL)
		Edit_Enable  (CheckVal, enabled);
}

static LRESULT CALLBACK WPDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == EnabledWP && wParam == BN_CLICKED) {
				SetWPControlsEnabled();
            }
            break;
        }
    }

	return CallWindowProc((WNDPROC)PrevWPDialogProc, hwnd, msg, wParam, lParam);
}

int *ShowWatchPointDialog(char *name, eType type, int *curval, int *newval)
{
	int *ret = NULL;
	char buf[20];

	// Create window
    MakeWindowClass();

	HWND WPDialog = CreateWindowClient(0, "POPToolsIo",
        _("Set WatchPoint"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 230, 85, MainWindow, NULL, Instance, NULL);

	PrevWPDialogProc = SetWindowLongPtr(WPDialog, GWLP_WNDPROC, (LONG_PTR)WPDialogProc);

	// Create controls
	CheckON  = NULL;
	CheckOFF = NULL;
	CheckVal = NULL;

    EnabledWP = CreateWindowEx(0, WC_BUTTON, _("WatchPoint Enabled"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 10, 140, 21, WPDialog, NULL, Instance, NULL); 
    NiceFont(EnabledWP);

	if(type == eType_DigInput || type == eType_DigOutput ||
		type == eType_InternalRelay || type == eType_InternalFlag) {
		CheckON = CreateWindowEx(0, WC_BUTTON, _("Ligado"),
			WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
			10, 35, 100, 20, WPDialog, NULL, Instance, NULL);
		NiceFont(CheckON);

		CheckOFF = CreateWindowEx(0, WC_BUTTON, _("Desligado"),
			WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
			10, 55, 100, 20, WPDialog, NULL, Instance, NULL);
		NiceFont(CheckOFF);

		Button_SetCheck(CheckON , BST_CHECKED  );
		Button_SetCheck(CheckOFF, BST_UNCHECKED);
	} else {
		HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Nome:"),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
			10, 45, 40, 21, WPDialog, NULL, Instance, NULL);
		NiceFont(textLabel);

		CheckVal = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
			WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
			55, 45, 85, 21, WPDialog, NULL, Instance, NULL);
		FixedFont(CheckVal);
	}

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        150, 10, 70, 30, WPDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        150, 45, 70, 30, WPDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

	// Set initial values
	if(curval != NULL) {
		Button_SetCheck(EnabledWP , BM_SETCHECK);
		if(CheckVal == NULL) {
			if(*curval == 0) {
				Button_SetCheck(CheckON , BST_UNCHECKED);
				Button_SetCheck(CheckOFF, BST_CHECKED  );
			}
		} else {
			int val = *curval;
			LadderSettingsGeneral settings = ladder->getSettingsGeneral();
			if(type == eType_RTO || type == eType_TON || type == eType_TOF)
				val = (val * settings.cycleTime) / 1000;

			sprintf(buf, "%d", val);
			Edit_SetText(CheckVal, buf);
		}
	}

	SetWPControlsEnabled();

	// Dialog Loop
    EnableWindow(MainWindow, FALSE);
    ShowWindow(WPDialog, TRUE);
    SetFocus(EnabledWP);

    MSG msg;
    DialogDone = FALSE;
    DialogCancel = FALSE;
    while(GetMessage(&msg, NULL, 0, 0) && !DialogDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                DialogDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
                break;
            }
        }

        if(IsDialogMessage(WPDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	// Check return
    if(!DialogCancel) {
		if(Button_GetCheck(EnabledWP) == BST_CHECKED && newval != NULL) {
			if(CheckVal == NULL) {
				*newval = (Button_GetCheck(CheckON) == BST_CHECKED);
			} else {
				Edit_GetText(CheckVal, buf, sizeof(buf)-1);
				*newval = atoi(buf);
				LadderSettingsGeneral settings = ladder->getSettingsGeneral();
				if(type == eType_RTO || type == eType_TON || type == eType_TOF)
					*newval = (*newval * 1000) / settings.cycleTime;
			}

			ret = newval;
		}
    } else {
		ret = curval;
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(WPDialog);
	return ret;
}

void IoMapListProc(NMHDR *h)
{
	char val[20];
	int c = LVN_COLUMNCLICK;
	LPNMLISTVIEW pNM = (LPNMLISTVIEW)h;

	switch(h->code) {
		case LVN_ITEMCHANGED:
			if(pNM->uNewState & LVIS_SELECTED) {
				ladder->selectIO(pNM->iItem);
			} else if(pNM->uOldState & LVIS_SELECTED) {
				ladder->selectIO(0);
			}
			break;

		case LVN_COLUMNCLICK:
			{
				LPNMLISTVIEW pLVInfo = (LPNMLISTVIEW)h;
				SortList(pLVInfo->iSubItem);
			}
			break;
        case LVN_GETDISPINFO: {
            NMLVDISPINFO *i = (NMLVDISPINFO *)h;
            int item = i->item.iItem;
            switch(i->item.iSubItem) {
                case LV_IO_PIN: {
					string name = ladder->getNameIObyIndex(item);
					mapDetails detailsIO = ladder->getDetailsIO(name);

					if(ladder->getContext().inSimulationMode) {
						int val;
						if(GetValWP(name.c_str(), &val) != NULL) {
							DescribeForIoList(val, detailsIO.type, i->item.pszText);
						} else {
							strcpy(i->item.pszText, "");
						}
					} else {
						strcpy(i->item.pszText, ladder->getPinNameIO(item).c_str());
					}
                    break;
				}

                case LV_IO_TYPE: {
                    strcpy(i->item.pszText, ladder->getStringTypeIO(item));
                    break;
                }
                case LV_IO_NAME: {
					string name = ladder->getNameIObyIndex(item);
					strcpy(i->item.pszText, name.c_str());
                    break;
				}

                case LV_IO_PORT: {
					strcpy(i->item.pszText, ladder->getPortNameIO(item).c_str());
					break;
                }

                case LV_IO_STATE: {
                    if(ladder->getContext().inSimulationMode) {
						string name = ladder->getNameIObyIndex(item);
						mapDetails detailsIO = ladder->getDetailsIO(name);

						DescribeForIoList(name.c_str(), detailsIO.type, i->item.pszText);
                    } else {
                        strcpy(i->item.pszText, "");
                    }
                    break;
                }

            }
            break;
        }
        case LVN_ITEMACTIVATE: {
			if(ladder->IsDialogShown()) break;

			NMITEMACTIVATE *i = (NMITEMACTIVATE *)h;
			if(ladder->getContext().inSimulationMode && i->iSubItem != LV_IO_NAME) {
				char name[1024];
				strcpy(name, ladder->getNameIObyIndex(i->iItem).c_str());
				mapDetails detailsIO = ladder->getDetailsIO(name);

				if(i->iSubItem != LV_IO_PIN) {
					if(detailsIO.type == eType_DigInput || (detailsIO.type == eType_Reserved &&
						!strcmp(name, ladder->getReservedNameIO(eType_DigInput).c_str()))) {
							SimulationToggleContact(name);
					} else if(detailsIO.type == eType_ReadADC) {
						ShowAnalogSliderPopup(name);
					} else if(detailsIO.type == eType_ReadEnc) {
						ShowEncoderSliderPopup(name);
					} else if(detailsIO.type == eType_Counter ||
							detailsIO.type == eType_TOF ||
							detailsIO.type == eType_TON ||
							detailsIO.type == eType_General) {
						sprintf(val, "%d", GetSimulationVariable(name));
						ShowSimulationVarSetDialog(name, val);
						SetSimulationVariable(name, atoi(val));
					}
				} else {
					int wpval;
					if(ShowWatchPointDialog(name, detailsIO.type, GetValWP(name, &wpval), &wpval) != NULL) {
						AddWP(name, wpval);
					} else {
						RemoveWP(name);
					}
					InvalidateRect(IoList, NULL, FALSE);
				}
            } else {
				if(i->iSubItem == LV_IO_NAME) {
					FindAndReplace(ladder->getNameIObyIndex(i->iItem), "", eSearchAndReplaceMode_FindNext);
				} else {
					ladder->ShowIoMapDialog(i->iItem);
					UpdateMainWindowTitleBar();
				}

				InvalidateRect(MainWindow, NULL, FALSE);
            }
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Dialog proc for the popup that lets you set the value of an analog input for
// simulation.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK SliderDialogProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
		case WM_COMMAND: {
            HWND h = (HWND)lParam;
			if(wParam == BN_CLICKED) {
				if(h == OkButton) {
					SliderDone = TRUE;
					return 0;
				} else if(h == CancelButton) {
					SliderDone = TRUE;
					SliderCancel = TRUE;
					return 0;
				}
			}

			break;
		 }

        case WM_CLOSE:
        case WM_DESTROY:
            SliderDone = TRUE;
            SliderCancel = TRUE;
            return 1;
    }

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// A little toolbar-style window that pops up to allow the user to set the
// simulated value of an ADC pin.
//-----------------------------------------------------------------------------
SWORD ShowSliderPopup(const char *name, SWORD currentVal, SWORD minVal, SWORD maxVal)
{
	bool isFirst = true;

	WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)SliderDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "POPToolsSlider";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    POINT pt;
    GetCursorPos(&pt);

    int left = pt.x - 10;
    // try to put the slider directly under the cursor (though later we might
    // realize that that would put the popup off the screen)
    int top = pt.y - 55;

    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    if(top + 110 >= r.bottom) {
        top = r.bottom - 110;
    }
    if(top < 0) top = 0;

    HWND SliderMain = CreateWindowClient(0, "POPToolsSlider", _("Pino E/S"),
        WS_VISIBLE | WS_POPUP | WS_DLGFRAME,
        left, top, 195, 100, MainWindow, NULL, Instance, NULL);
	HWND SliderTrackbar = CreateWindowEx(0, TRACKBAR_CLASS, "", WS_CHILD |
        TBS_AUTOTICKS | TBS_VERT | TBS_TOOLTIPS | WS_CLIPSIBLINGS | WS_VISIBLE, 
        0, 0, 30, 100, SliderMain, NULL, Instance, NULL);

	char buf[1024];
	sprintf(buf, "Valor de %s:", name);
    HWND textLabel = CreateWindowEx(0, WC_STATIC, buf,
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        40, 5, 150, 21, SliderMain, NULL, Instance, NULL);
    NiceFont(textLabel);

    HWND ValTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | CBS_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        40, 30, 150, 21, SliderMain, NULL, Instance, NULL);
    FixedFont(ValTextbox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        40, 65, 70, 23, SliderMain, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        120, 65, 70, 23, SliderMain, NULL, Instance, NULL); 
    NiceFont(CancelButton);

    SendMessage(SliderTrackbar, TBM_SETRANGEMIN, FALSE, minVal);
    SendMessage(SliderTrackbar, TBM_SETRANGEMAX, FALSE, maxVal);
    SendMessage(SliderTrackbar, TBM_SETTICFREQ, (maxVal - minVal)/8, 0);
    SendMessage(SliderTrackbar, TBM_SETPOS, TRUE, currentVal);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(SliderMain, TRUE);
    SetFocus(ValTextbox);
    SendMessage(ValTextbox, EM_SETSEL, 0, -1);

    DWORD ret;
    MSG msg;
    SliderDone = FALSE;
    SliderCancel = FALSE;

	SWORD newVal, previousVal = currentVal;
    while(!SliderDone && (ret = GetMessage(&msg, NULL, 0, 0))) {
		newVal = (SWORD)SendMessage(SliderTrackbar, TBM_GETPOS, 0, 0);
		if(newVal != previousVal || isFirst) {
			isFirst = false;
			sprintf(buf, "%d", newVal);
			SendMessage(ValTextbox, WM_SETTEXT, 0, (LPARAM)buf);
		    SendMessage(ValTextbox, EM_SETSEL , 0, -1);
			previousVal = newVal;
		}

        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                SliderDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                SliderDone = TRUE;
                SliderCancel = TRUE;
                break;
            }
		}
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!SliderCancel) {
		SendMessage(ValTextbox, WM_GETTEXT, sizeof(buf), (LPARAM)buf);
        currentVal = (SWORD)atoi(buf);
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(SliderMain);

	// A logica a seguir garante que retornamos um valor dentro do range permitido
	return min(maxVal, max(currentVal, minVal));
}

//-----------------------------------------------------------------------------
// A little toolbar-style window that pops up to allow the user to set the
// simulated value of an ADC pin.
//-----------------------------------------------------------------------------
void ShowAnalogSliderPopup(const char *name)
{
	SWORD minVal = 0, maxVal;

	if(ladder->getDetailsIO(name).pin == 6) {
		LadderSettingsADC settings = ladder->getSettingsADC();

		minVal = -20;
		maxVal = 100;

		if(!settings.isCelsius) {
			minVal = (minVal * 9)/5 + 32;
			maxVal = (maxVal * 9)/5 + 32;
		}
	} else {
		McuIoInfo *mcu = ladder->getMCU();

		if(mcu != nullptr) {
			maxVal = mcu->adcMax;
		} else {
			maxVal = 1023;
		}
	}

	if(maxVal == 0) {
        Error(_("O dispositivo selecionado não possui ADC ou não suporta."));
        return;
    }

	SetAdcShadow(name, ShowSliderPopup(name, GetAdcShadow(name), minVal, maxVal));

	ListView_RedrawItems(IoList, 0, ladder->getCountIO() - 1);
}

//-----------------------------------------------------------------------------
// A little toolbar-style window that pops up to allow the user to set the
// simulated value of an ADC pin.
//-----------------------------------------------------------------------------
void ShowEncoderSliderPopup(const char *name)
{
	SWORD minVal = 0, maxVal;

	McuIoInfo *mcu = ladder->getMCU();

	if(mcu != nullptr) {
		maxVal = mcu->encMax;
	} else {
		maxVal = 0x7FFFFFFF;
	}

	mapDetails DetailsIO = ladder->getDetailsIO(name);
	if(DetailsIO.pin == 1) { // Encoder Incremental
		LadderSettingsEncoderIncremental settings = ladder->getSettingsEncoderIncremental();

		if(settings.conv_mode) {
			float fator = (float)(settings.perimeter * 10)/(settings.pulses * (settings.x4 ? 4 : 2));
			switch(settings.conv_mode) {
			case 1: // Metros
				fator /= 1000;
				break;
			case 2: // Milimetros
				fator /= 10;
				break;
			case 3: // Decimos de milimetro
				// Nao precisa fazer nada pois o fator ja foi calculado para essa situacao
				break;
			}

			maxVal = (SWORD)(((float)(maxVal)/2.0f) * fator);
			minVal = -maxVal;
		}
	} else if(DetailsIO.pin == 2) { // Encoder Absoluto
		LadderSettingsEncoderSSI settings = ladder->getSettingsEncoderSSI();

		maxVal = (1UL << settings.size) - 1;
		if(settings.conv_mode) {
			float fator = (float)(settings.perimeter * 10)/(1UL << settings.size_bpr);
			switch(settings.conv_mode) {
			case 1: // Metros
				fator /= 1000;
				break;
			case 2: // Milimetros
				fator /= 10;
				break;
			case 3: // Decimos de milimetro
				// Nao precisa fazer nada pois o fator ja foi calculado para essa situacao
				break;
			}

			maxVal = (SWORD)((float)(maxVal) * fator)/2;
			minVal = -maxVal;
		}
	}

	if(maxVal == 0) {
        Error(_("No Encoder or Encoder not supported for selected micro."));
        return;
    }

	SetEncShadow(name, ShowSliderPopup(name, GetEncShadow(name), minVal, maxVal));

	ListView_RedrawItems(IoList, 0, ladder->getCountIO() - 1);
}

// Returns if io is associated to a ModBUS register or not.
bool IoMap_IsModBUS(mapDetails detailsIO)
{
	return detailsIO.pin >= 20;
}

// Returns index of register associated to pin. Examples:
// Pin associated to Digital Input 3: returns 2
// Pin associated to ModBUS register 5: returns 4
// Please note that there is no way to know if the index is for a Digital
// Input or ModBUS register. To know this, please call IoMap_IsModBUS()
unsigned int IoMap_GetIndex(mapDetails detailsIO)
{
	unsigned int ret = 0;

	if(detailsIO.pin > 0) {
		switch(detailsIO.type) {
			case eType_DigInput:
			case eType_DigOutput:
				ret = IoMap_IsModBUS(detailsIO) ? detailsIO.pin - 20 : detailsIO.pin - 1;
				break;

			case eType_General:
				ret = detailsIO.pin - 20;
				break;
		}
	}

	return ret;
}
