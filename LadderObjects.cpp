#include "poptools.h"

// Funcao auxiliar que entrega um contexto "vazio"
LadderContext getEmptyContext(void)
{
	LadderContext context;

	context.canNegate              = false;
	context.canNormal              = false;
	context.canSetOnly             = false;
	context.canResetOnly           = false;
	context.canPushUp              = false;
	context.canPushDown            = false;
	context.canDelete              = false;
	context.canInsertEnd           = false;
	context.canInsertOther         = false;
	context.canInsertComment       = false;

	context.SelectedCircuit        = nullptr;
	context.SelectedElem           = nullptr;
	context.SelectedState          = SELECTED_NONE;

	context.programChangedNotSaved = false;

	context.inSimulationMode       = false;
	context.isLoadingFile          = false;
	context.isDrawBlocked          = false;

	context.Diagram                = nullptr;
	context.ParallelStart          = nullptr;

	return context;
}

// Funcao auxiliar que retorna o tipo de I/O do timer conforme o seu tipo
eType getTimerTypeIO(int which)
{
	eType type;

	switch(which) {
	case ELEM_TON: type = eType_TON; break;
	case ELEM_TOF: type = eType_TOF; break;
	case ELEM_RTO: type = eType_RTO; break;
	default: oops();
	}

	return type;
}

// Funcao auxiliar que converte um ponteiro da classe base (LadderElem) para uma classe
// derivada (LadderElemCoil, LadderElemContact, etc) e entao o desaloca da memoria
void UnallocElem(LadderElem *elem)
{
	// Elemento nulo, apenas retorna.
	if(elem == nullptr) return;

	switch(elem->getWhich()) {
		case ELEM_PLACEHOLDER           : delete dynamic_cast<LadderElemPlaceHolder   *>(elem); break;
		case ELEM_COMMENT               : delete dynamic_cast<LadderElemComment       *>(elem); break;
		case ELEM_CONTACTS              : delete dynamic_cast<LadderElemContact       *>(elem); break;
		case ELEM_COIL                  : delete dynamic_cast<LadderElemCoil          *>(elem); break;
		case ELEM_TOF                   :
		case ELEM_TON                   :
		case ELEM_RTO                   : delete dynamic_cast<LadderElemTimer         *>(elem); break;
		case ELEM_RTC                   : delete dynamic_cast<LadderElemRTC           *>(elem);        break;
		case ELEM_CTU                   :
		case ELEM_CTD                   :
		case ELEM_CTC                   : delete dynamic_cast<LadderElemCounter       *>(elem); break;
		case ELEM_RES                   : delete dynamic_cast<LadderElemReset         *>(elem); break;
		case ELEM_ONE_SHOT_RISING       :
		case ELEM_ONE_SHOT_FALLING      : delete dynamic_cast<LadderElemOneShot       *>(elem); break;
		case ELEM_GRT                   :
		case ELEM_GEQ                   :
		case ELEM_LES                   :
		case ELEM_LEQ                   :
		case ELEM_NEQ                   :
		case ELEM_EQU                   : delete dynamic_cast<LadderElemCmp           *>(elem); break;
		case ELEM_MOD                   :
		case ELEM_ADD                   :
		case ELEM_SUB                   :
		case ELEM_MUL                   :
		case ELEM_DIV                   : delete dynamic_cast<LadderElemMath          *>(elem); break;
		case ELEM_SQRT                  : delete dynamic_cast<LadderElemSqrt          *>(elem); break;
		case ELEM_RAND                  : delete dynamic_cast<LadderElemRand          *>(elem); break;
		case ELEM_ABS                   : delete dynamic_cast<LadderElemAbs           *>(elem); break;
		case ELEM_MOVE                  : delete dynamic_cast<LadderElemMove          *>(elem); break;
		case ELEM_OPEN                  :
		case ELEM_SHORT                 : delete dynamic_cast<LadderElemOpenShort     *>(elem); break;
		case ELEM_SET_BIT               : delete dynamic_cast<LadderElemSetBit        *>(elem); break;
		case ELEM_CHECK_BIT             : delete dynamic_cast<LadderElemCheckBit      *>(elem); break;
		case ELEM_READ_ADC              : delete dynamic_cast<LadderElemReadAdc       *>(elem); break;
		case ELEM_SET_DA                : delete dynamic_cast<LadderElemSetDa         *>(elem); break;
		case ELEM_READ_ENC              : delete dynamic_cast<LadderElemReadEnc       *>(elem); break;
		case ELEM_RESET_ENC             : delete dynamic_cast<LadderElemResetEnc      *>(elem); break;
		case ELEM_MULTISET_DA           : delete dynamic_cast<LadderElemMultisetDA    *>(elem); break;
		case ELEM_READ_USS              :
		case ELEM_WRITE_USS             : delete dynamic_cast<LadderElemUSS           *>(elem); break;
		case ELEM_READ_MODBUS           :
		case ELEM_WRITE_MODBUS          : delete dynamic_cast<LadderElemModBUS        *>(elem); break;
		case ELEM_SET_PWM               : delete dynamic_cast<LadderElemSetPWM        *>(elem); break;
		case ELEM_UART_RECV             :
		case ELEM_UART_SEND             : delete dynamic_cast<LadderElemUART          *>(elem); break;
		case ELEM_MASTER_RELAY          : delete dynamic_cast<LadderElemMasterRelay   *>(elem); break;
		case ELEM_SHIFT_REGISTER        : delete dynamic_cast<LadderElemShiftRegister *>(elem); break;
		case ELEM_LOOK_UP_TABLE         : delete dynamic_cast<LadderElemLUT           *>(elem); break;
		case ELEM_PIECEWISE_LINEAR      : delete dynamic_cast<LadderElemPiecewise     *>(elem); break;
		case ELEM_READ_FORMATTED_STRING :
		case ELEM_WRITE_FORMATTED_STRING: delete dynamic_cast<LadderElemFmtString     *>(elem); break;
		case ELEM_READ_SERVO_YASKAWA    :
		case ELEM_WRITE_SERVO_YASKAWA   : delete dynamic_cast<LadderElemYaskawa       *>(elem); break;
		case ELEM_PERSIST               : delete dynamic_cast<LadderElemPersist       *>(elem); break;
		case ELEM_PID                   : delete dynamic_cast<LadderElemPID           *>(elem); break;
		default                         : delete elem;
	}
}

// Classe LadderExpansion
LadderExpansion::LadderExpansion(void)
{
	currentId = 0;
}

void LadderExpansion::Clear(void)
{
	vecExpansionBoardList.clear();
}

bool LadderExpansion::Add(string name, eExpansionBoard type)
{
	vector<sExpansionBoardItem>::iterator it;
	for(it = vecExpansionBoardList.begin(); it != vecExpansionBoardList.end(); it++) {
		if(it->name == name) {
			return false; // Placa ja existe!
		}
	}

	sExpansionBoardItem item;
	item.id   = currentId++;
	item.name = name;
	item.type = type;

	vecExpansionBoardList.push_back(item);

	return true; // Placa adicionada com sucesso!
}

bool LadderExpansion::Remove(string name)
{
	vector<sExpansionBoardItem>::iterator it;
	for(it = vecExpansionBoardList.begin(); it != vecExpansionBoardList.end(); it++) {
		if(it->name == name) {
			vecExpansionBoardList.erase(it);
			return false; // Placa removida!
		}
	}

	return false; // Placa nao encontrada!
}

sExpansionBoardItem LadderExpansion::getById(unsigned int id)
{
	sExpansionBoardItem item = { 0, "", eExpansionBoard_None };

	vector<sExpansionBoardItem>::iterator it;
	for(it = vecExpansionBoardList.begin(); it != vecExpansionBoardList.end(); it++) {
		if(it->id == id) {
			item = *it;
			break;
		}
	}

	return item; // Se placa nao encontrada, retorna valores iniciais
}

unsigned int LadderExpansion::getQuantityIO(eExpansionBoard typeBoard, eType typeIO)
{
	switch(typeBoard) {
	case eExpansionBoard_DigitalInput:
		if(typeIO == eType_DigInput) return 16; // Possui 16 entradas digitais
	case eExpansionBoard_DigitalOutput:
		if(typeIO == eType_DigOutput) return 8; // Possui 8 saidas digitais
	case eExpansionBoard_AnalogInput:
		if(typeIO == eType_ReadADC) return 2; // Possui 2 entradas analogicas
	}

	return 0; // Se chegou aqui, a placa nao possui este tipo de I/O.
}

bool LadderExpansion::Save(FILE *f)
{
	// Primeiro grava a quantidade de placas na lista e o id atual
	if(fwrite_uint(f, vecExpansionBoardList.size()) && fwrite_ulong(f, currentId)) {
		vector<sExpansionBoardItem>::iterator it;

		for(it = vecExpansionBoardList.begin(); it != vecExpansionBoardList.end(); it++) {
			if(
				!fwrite_ulong (f, it->id) ||
				!fwrite_string(f, it->name) || 
				!fwrite_uint  (f, it->type)) {
					break; // Erro na gravacao!
			}
		}

		if(it == vecExpansionBoardList.end()) {
			return true; // alcancou final, significa que gravou com sucesso!
		}
	}

	return false;
}

bool LadderExpansion::Load(FILE *f, unsigned int version)
{
	unsigned int size;
	const unsigned int FirstLoadVersion = 2;

	Clear();

	// Se for versao antiga, simplesmente retorna
	if(version < FirstLoadVersion) {
		return true;
	}

	// Primeiro carrega a quantidade de placas na lista e o id atual
	if(fread_uint(f, &size) && fread_ulong(f, &currentId)) {
		unsigned int  i, iType;
		sExpansionBoardItem item;

		for(i = 0; i < size; i++) {
			if(
				!fread_ulong (f, &item.id) ||
				!fread_string(f, &(item.name)) || 
				!fread_uint  (f, &(iType))) {
					break; // Erro na leitura!
			} else {
				switch(iType) {
					default:
					case eExpansionBoard_None         : item.type = eExpansionBoard_None         ; break;
					case eExpansionBoard_DigitalInput : item.type = eExpansionBoard_DigitalInput ; break;
					case eExpansionBoard_DigitalOutput: item.type = eExpansionBoard_DigitalOutput; break;
					case eExpansionBoard_AnalogInput  : item.type = eExpansionBoard_AnalogInput  ; break;
				}

				vecExpansionBoardList.push_back(item);
			}
		}

		if(i == size) {
			return true; // alcancou final, significa que gravou com sucesso!
		}
	}

	// Ocorreu erro! Limpa a lista que estava sendo carregada
	Clear();

	return false;
}

//Classe LadderElem
void LadderElem::Init(void)
{
	isEndOfLine   = false;
	isComment     = false;
	isFormatted   = false;
	isUART        = false;
	poweredAfter  = false;
}

LadderElem::LadderElem(void)
{
	Init();
}

LadderElem::LadderElem(bool EOL, bool Comment, bool Formatted, bool UART, int elemWhich)
{
	Init();

	which       = elemWhich;
	isEndOfLine = EOL;
	isComment   = Comment;
	isFormatted = Formatted;
	isUART      = UART;
}

void LadderElem::setProperties(LadderContext context, void *propData)
{
	if(Diagram->IsLocked()) return; // Nao permite alterar se estiver em simulacao

	// Registro da acao para desfazer / refazer
	UndoRedoAction action;
	UndoRedoData *data = new UndoRedoData;

	data->SetProp.data = getProperties();

	action.action        = eSetProp;
	action.contextAfter  = getEmptyContext();
	action.contextBefore = context;
	action.data          = data;
	action.io            = nullptr;
	action.elem          = this;
	action.subckt        = nullptr;

	context.Diagram->CheckpointBegin(_("Alterado Elemento"));
	context.Diagram->RegisterAction(action);

	// Altera as propriedades do elemento
	if(internalSetProperties(propData, false) == false) { // Retornou erro, cancela!
		context.Diagram->CheckpointRollback();
	} else {
		context.Diagram->ProgramChanged();
	}

	// Finaliza a operacao
	context.Diagram->CheckpointEnd();
}

bool LadderElem::GenerateIntCode(IntCode &ic)
{
	bool ret = internalGenerateIntCode(ic);

	if(ret == true) {
		ic.SimState(&poweredAfter, ic.getStateInOut());
	}

	return ret;
}

bool LadderElem::Save(FILE *f)
{
	return internalSave(f);
}

bool LadderElem::Load(FILE *f, unsigned int version)
{
//	updateIO(Diagram, true); // Descarta o I/O referenciado quando da criacao do objeto
	return internalLoad(f, version);
}

bool LadderElem::updateNameTypeIO(unsigned int index, string name, eType type)
{
	Diagram->CheckpointBegin(_("Alterar Nome/Tipo do I/O"));

	if(internalUpdateNameTypeIO(index, name, type)) {
		Diagram->CheckpointEnd();
		Diagram->updateContext();
		Diagram->ProgramChanged();

		return true;
	} else {
		Diagram->CheckpointRollback();
		Diagram->CheckpointEnd();
	}

	return false;
}

// Funcao que executa uma acao de desfazer / refazer
bool LadderElem::DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	void *olddata;
	bool ret = true;
	UndoRedoData *data = (UndoRedoData *)action.data;

	switch(action.action) {
	case eSetProp: {
		if(isDiscard) {
			ret = internalDoUndoRedo(IsUndo, isDiscard, action);
		} else {
			olddata = getProperties();
			internalSetProperties(data->SetProp.data, true);
			data->SetProp.data = olddata;
		}
		break;
	}

	default: ret = internalDoUndoRedo(IsUndo, isDiscard, action);
	}

	// Se estamos descartando, desaloca a estrutura que representa a acao
	if(isDiscard && ret) {
		delete data;
	}

	return ret; // Nada mais a fazer
}

// Classe LadderElemPlaceHolder
LadderElemPlaceHolder::LadderElemPlaceHolder(LadderDiagram *diagram) : LadderElem(false, false, false, false, ELEM_PLACEHOLDER)
{
	Diagram = diagram;
}

pair<string, string> LadderElemPlaceHolder::DrawTXT(void)
{
	return pair<string, string>("", "--");
}

bool LadderElemPlaceHolder::internalGenerateIntCode(IntCode &ic)
{
	return true;
}

bool LadderElemPlaceHolder::CanInsert(LadderContext context)
{
	return context.canInsertOther &&
		(context.SelectedElem == nullptr || context.SelectedElem->getWhich() != ELEM_PLACEHOLDER);
}

LadderElem *LadderElemPlaceHolder::Clone(LadderDiagram *diagram)
{
	LadderElemPlaceHolder *clone = new LadderElemPlaceHolder(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados

	return clone;
}

bool LadderElemPlaceHolder::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemComment
LadderElemComment::LadderElemComment(LadderDiagram *diagram) : LadderElem(false, true, false, false, ELEM_COMMENT)
{
	Diagram  = diagram;
	prop.str = _("--Adicione Comentários Aqui--");
}

pair<string, string> LadderElemComment::DrawTXT(void)
{
	string first, second = "";

	char tbuf[MAX_COMMENT_LEN];
	char tlbuf[MAX_COMMENT_LEN+8];

	strcpy(tbuf, prop.str.c_str());
	char *b = strchr(tbuf, '\n');

	if(b) {
		if(b[-1] == '\r') b[-1] = '\0';
		*b = '\0';

		sprintf(tlbuf, "\x03 ; %s\x02", b+1);
		second = tlbuf;
	}

	sprintf(tlbuf, "\x03 ; %s\x02", tbuf);
	first = tlbuf;

	return pair<string, string>(first, second);
}

bool LadderElemComment::internalGenerateIntCode(IntCode &ic)
{
	return true;
}

bool LadderElemComment::CanInsert(LadderContext context)
{
	return context.canInsertComment;
}

bool LadderElemComment::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemCommentProp *newProp = (LadderElemCommentProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemComment::getProperties(void)
{
	LadderElemCommentProp *curProp = new LadderElemCommentProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemComment::internalSave(FILE *f)
{
	return fwrite_string(f, prop.str);
}

bool LadderElemComment::internalLoad(FILE *f, unsigned int version)
{
	return fread_string(f, &prop.str);
}

inline int LadderElemComment::getWidthTXT(void)
{
	char tbuf[MAX_COMMENT_LEN];

	strcpy(tbuf, prop.str.c_str());
	char *b = strchr(tbuf, '\n');

	int len;
	if(b) {
		*b = '\0';
		len = max(strlen(tbuf)-1, strlen(b+1));
	} else {
		len = strlen(tbuf);
	}
	// round up, and allow space for lead-in
	return  (len + 7 + (POS_WIDTH-1)) / POS_WIDTH;
}

LadderElem *LadderElemComment::Clone(LadderDiagram *diagram)
{
	LadderElemComment *clone = new LadderElemComment(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemComment::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemCommentProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemContact
LadderElemContact::LadderElemContact(LadderDiagram *diagram) : LadderElem(false, false, false, false, ELEM_CONTACTS)
{
	Diagram      = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = Diagram->getReservedNameIO(eType_DigInput);
	infoIO_Name.isBit         = true;
	infoIO_Name.type          = eType_DigInput;
	infoIO_Name.access        = eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.negated = false;
	prop.idName  = infoIO_Name.pin;
}

pair<string, string> LadderElemContact::DrawTXT(void)
{
	string sname = Diagram->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	mapDetails detailsIO = Diagram->getDetailsIO(prop.idName.first);
	char ch, buf[4], name_with_type[MAX_NAME_LEN+4];

	buf[0] = ']';
	buf[1] = prop.negated ? '/' : ' ';
	buf[2] = '[';
	buf[3] = '\0';

	const char *typeLetters = _("ESRF");
	switch(detailsIO.type) {
	case eType_DigInput:
		ch = typeLetters[0];
		break;

	case eType_DigOutput:
		ch = typeLetters[1];
		break;

	case eType_InternalRelay:
		ch = typeLetters[2];
		break;

	case eType_InternalFlag:
		ch = typeLetters[3];
		break;

	default:
		ch = '?';
		break;
	}
	sprintf(name_with_type, "%s (%c)", name, ch);

	return pair<string, string>(name_with_type, buf);
}

bool LadderElemContact::internalGenerateIntCode(IntCode &ic)
{
	string name = Diagram->getNameIO(prop.idName.first);
	mapDetails detailsIO = Diagram->getDetailsIO(prop.idName.first);

	string buf;
	if(detailsIO.type == eType_InternalFlag) {
		buf = "$" + name;
	} else {
		buf = name;
	}

	ic.OpBit(prop.negated ? INT_IF_BIT_SET : INT_IF_BIT_CLEAR, buf.c_str(), detailsIO.bit);
		ic.OpBit(INT_CLEAR_BIT, ic.getStateInOut(), detailsIO.bit);
	ic.Op   (INT_END_IF);

	return true;
}

bool LadderElemContact::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void *LadderElemContact::getProperties(void)
{
	LadderElemContactProp *curProp = new LadderElemContactProp;

	*curProp = prop;

	return curProp;
}

bool LadderElemContact::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemContactProp *newProp = (LadderElemContactProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemContact::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second) &&
		fwrite_bool (f, prop.negated);
}

bool LadderElemContact::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_bool (f, &prop.negated);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemContact::Clone(LadderDiagram *diagram)
{
	LadderElemContact *clone = new LadderElemContact(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemContact::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_DigInput && type != eType_DigOutput &&
		type != eType_InternalRelay && type != eType_InternalFlag && type != eType_Reserved) {
			return false;
	}

	return true;
}

void LadderElemContact::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemContact::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return Diagram->getDetailsIO(prop.idName.first).type;
	}

	return eType_Pending;
}

int LadderElemContact::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(prop.idName.first).type;
			if(type == eType_Reserved) {
				type = eType_DigInput;
			}

			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, type, infoIO_Name)) {
				LadderElemContactProp *data = (LadderElemContactProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemContact::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	if(type == eType_Reserved) {
		type = Diagram->getDetailsIO(name).type;
	}

	if(type != eType_DigInput &&
		type != eType_DigOutput &&
		type != eType_InternalRelay &&
		type != eType_InternalFlag) {
			type = eType_DigInput;
	}

	eReply reply;
	bool ret = Diagram->IsValidNameAndType(pin.first, name.c_str(), type, &reply);
	if(ret) {
		if(reply == eReply_No || reply == eReply_Ok) {
			mapDetails detailsIO = Diagram->getDetailsIO(name);
			if(detailsIO.type != eType_Pending) {
				type = detailsIO.type;
			}
		}

		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemContactProp *data = (LadderElemContactProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemContact::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemContactProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemCoil
LadderElemCoil::LadderElemCoil(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_COIL)
{
	Diagram        = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = Diagram->getReservedNameIO(eType_DigOutput);
	infoIO_Name.isBit         = true;
	infoIO_Name.type          = eType_DigOutput;
	infoIO_Name.access        = eRequestAccessType_Write;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.negated   = false;
	prop.setOnly   = false;
	prop.resetOnly = false;
	prop.idName    = infoIO_Name.pin;
}

pair<string, string> LadderElemCoil::DrawTXT(void)
{
	string name = Diagram->getNameIO(prop.idName.first);
	char buf[4], name_with_type[MAX_NAME_LEN+4];
	mapDetails detailsIO = Diagram->getDetailsIO(prop.idName.first);

    buf[0] = '(';
    if(prop.negated) {
        buf[1] = '/';
    } else if(prop.setOnly) {
        buf[1] = 'S';
    } else if(prop.resetOnly) {
        buf[1] = 'R';
    } else {
        buf[1] = ' ';
    }
    buf[2] = ')';
    buf[3] = '\0';

	const char *typeLetters = _("ESRF");
	sprintf(name_with_type, "%s (%c)", name.c_str(), detailsIO.type == eType_DigOutput ? typeLetters[1] : typeLetters[2]);

	return pair<string, string>(name_with_type, buf);
}

bool LadderElemCoil::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	mapDetails detailsIO = Diagram->getDetailsIO(prop.idName.first);
	const char *stateInOut = ic.getStateInOut();

	if(prop.negated) {
		ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.OpBit(INT_CLEAR_BIT, name, detailsIO.bit);
		ic.Op(INT_ELSE);
		ic.OpBit(INT_SET_BIT, name, detailsIO.bit);
		ic.Op(INT_END_IF);
	} else if(prop.setOnly) {
		ic.OpBit(INT_IF_BIT_SET, stateInOut, detailsIO.bit);
		ic.OpBit(INT_SET_BIT, name, detailsIO.bit);
		ic.Op(INT_END_IF);
	} else if(prop.resetOnly) {
		ic.OpBit(INT_IF_BIT_SET, stateInOut, detailsIO.bit);
		ic.OpBit(INT_CLEAR_BIT, name, detailsIO.bit);
		ic.Op(INT_END_IF);
	} else {
		ic.OpBit(INT_COPY_BIT_TO_BIT, name, stateInOut, detailsIO.bit);
	}

	return true;
}

bool LadderElemCoil::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemCoil::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemCoilProp *newProp = (LadderElemCoilProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemCoil::getProperties(void)
{
	LadderElemCoilProp *curProp = new LadderElemCoilProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemCoil::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second) &&
		fwrite_bool (f, prop.negated) &&
		fwrite_bool (f, prop.resetOnly) &&
		fwrite_bool (f, prop.setOnly);
}

bool LadderElemCoil::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_bool (f, &prop.negated) &&
		fread_bool (f, &prop.resetOnly) &&
		fread_bool (f, &prop.setOnly);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemCoil::Clone(LadderDiagram *diagram)
{
	LadderElemCoil *clone = new LadderElemCoil(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemCoil::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_DigOutput && type != eType_InternalRelay && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemCoil::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemCoil::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return Diagram->getDetailsIO(prop.idName.first).type;
	}

	return eType_Pending;
}

int LadderElemCoil::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(prop.idName.first).type;
			if(type == eType_Reserved) {
				type = eType_DigOutput;
			}

			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, type, infoIO_Name)) {
				LadderElemCoilProp *data = (LadderElemCoilProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemCoil::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	if(type == eType_Reserved) {
		type = Diagram->getDetailsIO(name).type;
	}

	if(type != eType_DigOutput && type != eType_InternalRelay) {
		type = eType_DigOutput;
	}

	eReply reply;
	bool ret = Diagram->IsValidNameAndType(pin.first, name.c_str(), type, &reply);
	if(ret) {
		if(reply == eReply_No) {
			mapDetails detailsIO = Diagram->getDetailsIO(name);
			if(detailsIO.type != eType_DigOutput &&
				detailsIO.type != eType_InternalRelay &&
				detailsIO.type != eType_Reserved) {
					Diagram->ShowDialog(eDialogType_Message, false, _("Tipo Inválido"), _("Conflito entre tipos! Operação não permitida."));
					return false;
			} else {
				type = detailsIO.type;
			}
		}

		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemCoilProp *data = (LadderElemCoilProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemCoil::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemCoilProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemTimer
LadderElemTimer::LadderElemTimer(LadderDiagram *diagram, int which) : LadderElem(false, false, false, false, which)
{
	Diagram     = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = ""; // nome vazio, recebera uma variavel com sequencia numerica
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = getTimerTypeIO(getWhich());
	infoIO_Name.access        = which == ELEM_RTO ? eRequestAccessType_Read : eRequestAccessType_ReadWrite;
	infoIO_Name.isUniqueRead  = true;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.delay  = 100000;
	prop.idName = infoIO_Name.pin;
}

//-----------------------------------------------------------------------------
// Calculate the period in scan units from the period in microseconds, and
// raise an error if the given period is unachievable.
//-----------------------------------------------------------------------------
int LadderElemTimer::TimerPeriod(void)
{
	LadderSettingsGeneral settings = Diagram->getSettingsGeneral();
    unsigned int period = (prop.delay / settings.cycleTime) - 1;

    if(period < 1)  {
        Error(_("Período de Tempo muito curto (necessita de um tempo de ciclo menor)."));
        CompileError();
    }
    if(period >= (((unsigned int)(1 << 31)) - 1)) {
        Error(_("Tempo do Temporizador muito grande(max. 2147483647 tempo de ciclo); use um tempo de ciclo maior."));
        CompileError();
    }

    return period;
}

pair<string, string> LadderElemTimer::DrawTXT(void)
{
	const char *s;
	int which = getWhich();

	if(which == ELEM_TON)
		s = _("TON");
	else if(which == ELEM_TOF)
		s = _("TOF");
	else if(which == ELEM_RTO)
		s = _("RTO");
	else oops();

	char buf[256];

	if(prop.delay >= 1000*1000) {
		sprintf(buf, "[\x01%s\x02 %.3f s]" , s, prop.delay/1000000.0);
	} else if(prop.delay >= 100*1000) {
		sprintf(buf, "[\x01%s\x02 %.1f ms]", s, prop.delay/1000.0);
	} else {
		sprintf(buf, "[\x01%s\x02 %.2f ms]", s, prop.delay/1000.0);
	}

	return pair<string, string>(Diagram->getNameIO(prop.idName.first), buf);
}

bool LadderElemTimer::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName.first);
	const char *name = sname.c_str();
	const char *stateInOut = ic.getStateInOut();

	switch(getWhich()) {
        case ELEM_RTO: {
            int period = TimerPeriod();

			ic.Op(INT_IF_VARIABLE_LES_LITERAL, name, period);

            ic.Op(INT_IF_BIT_SET, stateInOut);
			ic.Op(INT_INCREMENT_VARIABLE, name);
            ic.Op(INT_END_IF);
            ic.Op(INT_CLEAR_BIT, stateInOut);

            ic.Op(INT_ELSE);

            ic.Op(INT_SET_BIT, stateInOut);

            ic.Op(INT_END_IF);

            break;
        }
        case ELEM_TON: {
            int period = TimerPeriod();

            ic.Op(INT_IF_BIT_SET, stateInOut);

            ic.Op(INT_IF_VARIABLE_LES_LITERAL, name, period);

            ic.Op(INT_INCREMENT_VARIABLE, name);
            ic.Op(INT_CLEAR_BIT, stateInOut);
            ic.Op(INT_END_IF);

            ic.Op(INT_ELSE);

            ic.Op(INT_SET_VARIABLE_TO_LITERAL, name);

            ic.Op(INT_END_IF);

            break;
        }
        case ELEM_TOF: {
            int period = TimerPeriod();

            // All variables start at zero by default, so by default the
            // TOF timer would start out with its output forced HIGH, until
            // it finishes counting up. This does not seem to be what
            // people expect, so add a special case to fix that up.
            char antiGlitchName[MAX_NAME_LEN];
            sprintf(antiGlitchName, "$%s_antiglitch", name);
            ic.Op(INT_IF_BIT_CLEAR, antiGlitchName);
                ic.Op(INT_SET_VARIABLE_TO_LITERAL, name, period);
            ic.Op(INT_END_IF);
            ic.Op(INT_SET_BIT, antiGlitchName);
            
            ic.Op(INT_IF_BIT_CLEAR, stateInOut);

            ic.Op(INT_IF_VARIABLE_LES_LITERAL, name, period);

            ic.Op(INT_INCREMENT_VARIABLE, name);
            ic.Op(INT_SET_BIT, stateInOut);
            ic.Op(INT_END_IF);

            ic.Op(INT_ELSE);

            ic.Op(INT_SET_VARIABLE_TO_LITERAL, name);

            ic.Op(INT_END_IF);
            break;
        }
	}

	return true;
}

bool LadderElemTimer::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemTimer::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemTimerProp *newProp = (LadderElemTimerProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemTimer::getProperties(void)
{
	LadderElemTimerProp *curProp = new LadderElemTimerProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemTimer::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second) &&
		fwrite_int  (f, prop.delay);
}

bool LadderElemTimer::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_int  (f, &prop.delay);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemTimer::Clone(LadderDiagram *diagram)
{
	LadderElemTimer *clone = new LadderElemTimer(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemTimer::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != getTimerTypeIO(getWhich()) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemTimer::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemTimer::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return getTimerTypeIO(getWhich());
	}

	return eType_Pending;
}

int LadderElemTimer::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, getTimerTypeIO(getWhich()), infoIO_Name)) {
				LadderElemTimerProp *data = (LadderElemTimerProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemTimer::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = getTimerTypeIO(getWhich());

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Nome" ), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemTimerProp *data = (LadderElemTimerProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemTimer::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemTimerProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemRTC
LadderElemRTC::LadderElemRTC(LadderDiagram *diagram) : LadderElem(false, false, false, false, ELEM_RTC)
{
	time_t rawtime;
	struct tm * t;

	Diagram = diagram;

	time ( &rawtime );
	t = localtime ( &rawtime );

	t->tm_year += 1900;
	t->tm_mon++;
	t->tm_sec = t->tm_sec > 59 ? 59 : t->tm_sec;

	prop.mode = ELEM_RTC_MODE_FIXED;
	prop.wday = 255;

	prop.start.tm_mday = 0;
	prop.start.tm_mon  = 0;
	prop.start.tm_year = 0;
	prop.start.tm_hour = t->tm_hour;
	prop.start.tm_min  = t->tm_min;
	prop.start.tm_sec  = t->tm_sec;

	prop.end.tm_mday   = 0;
	prop.end.tm_mon    = 0;
	prop.end.tm_year   = 0;
	prop.end.tm_hour   = t->tm_hour;
	prop.end.tm_min    = t->tm_min;
	prop.end.tm_sec    = t->tm_sec;
}

pair<string, string> LadderElemRTC::DrawTXT(void)
{
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

			sprintf(linha ? bufe : bufs, "[\x01%s %02d:%02d:%02d\x02]", _("DSTQQSS"),
				ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

			if(linha || prop.mode == ELEM_RTC_MODE_FIXED) {
				for(i=0; i<7; i++) {
					if(!(prop.wday & (1<<i))) {
						(linha ? bufe : bufs)[i+2] = ' ';
					} else if(linha) {
						bufe[i+2] = '*';
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

			sprintf(linha ? bufe : bufs, "[\x01%s/%s/%s %02d:%02d:%02d\x02]", mday, month, year, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		} 
	}

	if(prop.mode == ELEM_RTC_MODE_FIXED) {
		strcpy(bufe, bufs);
		strcpy(bufs, _("RTC - Mode Fixed"));
	}

	return pair<string, string>(bufs, bufe);
}

bool LadderElemRTC::internalGenerateIntCode(IntCode &ic)
{
	char sd[10], sm[10], sy[10], sh[10], smm[10], ss[10];
	char ed[10], em[10], ey[10], eh[10], emm[10], es[10];
	char mode[10];

	_itoa(prop.start.tm_mday, sd  , 10);
	_itoa(prop.start.tm_mon , sm  , 10);
	_itoa(prop.start.tm_year, sy  , 10);
	_itoa(prop.start.tm_hour, sh  , 10);
	_itoa(prop.start.tm_min , smm , 10);
	_itoa(prop.start.tm_sec , ss  , 10);

	_itoa(prop.end.tm_mday  , ed  , 10);
	_itoa(prop.end.tm_mon   , em  , 10);
	_itoa(prop.end.tm_year  , ey  , 10);
	_itoa(prop.end.tm_hour  , eh  , 10);
	_itoa(prop.end.tm_min   , emm , 10);
	_itoa(prop.end.tm_sec   , es  , 10);

	_itoa(prop.mode         , mode, 10);

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
			ic.Op(INT_SET_RTC, sd, sm, sy, sh, smm, ss, NULL, 0, 0);
			ic.Op(INT_SET_RTC, ed, em, ey, eh, emm, es, NULL, 0, 1);
			ic.Op(INT_CHECK_RTC, ic.getStateInOut(), mode, prop.wday);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemRTC::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemRTC::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemRTCProp *newProp = (LadderElemRTCProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemRTC::getProperties(void)
{
	LadderElemRTCProp *curProp = new LadderElemRTCProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemRTC::internalSave(FILE *f)
{
	return
		fwrite_pointer(f, &prop.start, sizeof(prop.start)) &&
		fwrite_pointer(f, &prop.end  , sizeof(prop.end)) &&
		fwrite_int    (f, prop.mode) &&
		fwrite_uchar  (f, prop.wday);
}

bool LadderElemRTC::internalLoad(FILE *f, unsigned int version)
{
	return
		fread_pointer(f, &prop.start, sizeof(prop.start)) &&
		fread_pointer(f, &prop.end  , sizeof(prop.end)) &&
		fread_int    (f, &prop.mode) &&
		fread_uchar  (f, &prop.wday);
}

LadderElem *LadderElemRTC::Clone(LadderDiagram *diagram)
{
	LadderElemRTC *clone = new LadderElemRTC(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemRTC::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemRTCProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemCounter
LadderElemCounter::LadderElemCounter(LadderDiagram *diagram, int which) : LadderElem(which == ELEM_CTC ? true : false, false, false, false, which)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_Counter;
	infoIO_Name.access        = which == ELEM_CTC ? eRequestAccessType_ReadWrite : eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	// Caracteristicas do I/O Max
	infoIO_Max.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Max.name          = _("novo");
	infoIO_Max.isBit         = false;
	infoIO_Max.type          = eType_General;
	infoIO_Max.access        = eRequestAccessType_Read;
	infoIO_Max.isUniqueRead  = false;
	infoIO_Max.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Max);

	prop.idMax  = infoIO_Max .pin;
	prop.idName = infoIO_Name.pin;
}

pair<string, string> LadderElemCounter::DrawTXT(void)
{
	int which = getWhich();
	const char *s;
	char op, buf[256];

	if(which == ELEM_CTC) {
		sprintf(buf, _("{\x01""CTC\x02 0:%s}"), Diagram->getNameIO(prop.idMax).c_str());
	} else {
		if(which == ELEM_CTU) {
			s = _("CTU");
			op = '>';
		} else if(which == ELEM_CTD) {
			s = _("CTD");
			op = '<';
		} else oops();

		sprintf(buf, "[\x01%s\x02 %c=%s]", s, op, Diagram->getNameIO(prop.idMax).c_str());
	}

	return pair<string, string>(Diagram->getNameIO(prop.idName.first), buf);
}

bool LadderElemCounter::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName.first);
	const char *name = sname.c_str();

	string smax = Diagram->getNameIO(prop.idMax);
	const char *cmax = ic.VarFromExpr(smax.c_str(), "$scratch2");

	const char *stateInOut = ic.getStateInOut();
    string storeName = ic.GenSymOneShot();

	switch(getWhich()) {
        case ELEM_CTU: {
            ic.Op(INT_IF_BIT_SET, stateInOut);
				ic.Op(INT_IF_BIT_CLEAR, storeName.c_str());
					ic.Op(INT_INCREMENT_VARIABLE, name);
                ic.Op(INT_END_IF);
            ic.Op(INT_END_IF);
			ic.Op(INT_COPY_BIT_TO_BIT, storeName.c_str(), stateInOut);

			ic.Op(INT_IF_VARIABLE_GRT_VARIABLE, cmax, name);
                ic.Op(INT_CLEAR_BIT, stateInOut);
            ic.Op(INT_ELSE);
                ic.Op(INT_SET_BIT, stateInOut);
            ic.Op(INT_END_IF);
            break;
        }
        case ELEM_CTD: {
            ic.Op(INT_IF_BIT_SET, stateInOut);
				ic.Op(INT_IF_BIT_CLEAR, storeName.c_str());
                    ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", 1);
					ic.Op(INT_SET_VARIABLE_SUBTRACT, name, name, "$scratch", 0, 0);
                ic.Op(INT_END_IF);
            ic.Op(INT_END_IF);
			ic.Op(INT_COPY_BIT_TO_BIT, storeName.c_str(), stateInOut);

			ic.Op(INT_IF_VARIABLE_GRT_VARIABLE, name, cmax);
                ic.Op(INT_CLEAR_BIT, stateInOut);
            ic.Op(INT_ELSE);
                ic.Op(INT_SET_BIT, stateInOut);
            ic.Op(INT_END_IF);
            break;
        }
        case ELEM_CTC: {
            ic.Op(INT_IF_BIT_SET, stateInOut);
                ic.Op(INT_IF_BIT_CLEAR, storeName.c_str());
                    ic.Op(INT_INCREMENT_VARIABLE, name);
//                    ic.Op(INT_IF_VARIABLE_LES_LITERAL, name, prop.max+1);
//                    ic.Op(INT_ELSE);
					ic.Op(INT_IF_VARIABLE_GRT_VARIABLE, name, cmax);
                        ic.Op(INT_SET_VARIABLE_TO_LITERAL, name, (SWORD)0);
                    ic.Op(INT_END_IF);
                ic.Op(INT_END_IF);
            ic.Op(INT_END_IF);
            ic.Op(INT_COPY_BIT_TO_BIT, storeName.c_str(), stateInOut);
            break;
        }
	}

	return true;
}

bool LadderElemCounter::CanInsert(LadderContext context)
{
	return getWhich() == ELEM_CTC ? context.canInsertEnd : context.canInsertOther;
}

bool LadderElemCounter::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemCounterProp *newProp = (LadderElemCounterProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemCounter::getProperties(void)
{
	LadderElemCounterProp *curProp = new LadderElemCounterProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemCounter::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first ) &&
		fwrite_int  (f, prop.idName.second) &&
		fwrite_ulong(f, prop.idMax .first ) &&
		fwrite_int  (f, prop.idMax .second);
}

bool LadderElemCounter::internalLoad(FILE *f, unsigned int version)
{
	bool ret;

	if(version < 1) {
		ret =
			fread_ulong(f, &prop.idName.first) &&
			fread_int  (f, &prop.idName.second) &&
			fread_int  (f, &prop.idMax .second);

		prop.idMax.first = 0;
	} else {
		ret =
			fread_ulong(f, &prop.idName.first ) &&
			fread_int  (f, &prop.idName.second) &&
			fread_ulong(f, &prop.idMax .first ) &&
			fread_int  (f, &prop.idMax .second);
	}

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
		Diagram->updateTypeIO(prop.idMax.first );
	}

	return ret;
}

LadderElem *LadderElemCounter::Clone(LadderDiagram *diagram)
{
	LadderElemCounter *clone = new LadderElemCounter(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemCounter::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_Counter && type != eType_Reserved) {
		return false;
	}

	if(id == prop.idMax.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemCounter::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;

	infoIO_Max.pin = prop.idMax;
	Diagram->updateIO(owner, this, infoIO_Max, isDiscard);
	prop.idMax = infoIO_Max.pin;
}

eType LadderElemCounter::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return eType_Counter;
	}

	if(prop.idMax.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemCounter::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idName.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, eType_Counter, infoIO_Name)) {
				LadderElemCounterProp *data = (LadderElemCounterProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				n++;
			}
		} else {
			return 1;
		}
	}

	if(prop.idMax.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idMax;
			if(Diagram->getIO(this, pin, sNewText, eType_General, infoIO_Max)) {
				LadderElemCounterProp *data = (LadderElemCounterProp *)getProperties();

				data->idMax  = pin;

				setProperties(Diagram->getContext(), data);

				n++;
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemCounter::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	bool isValid, tryGeneralTypeFirst;
	pair<unsigned long, int> pin = (index == 0) ? prop.idName : prop.idMax;

	if(index == 0) {
		type = eType_Counter;
		tryGeneralTypeFirst = false;

		isValid = Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Nome"), VALIDATE_IS_VAR_OR_NUMBER, 0, 0);
	} else {
		// Se variavel sem tipo, usa tipo geral.
		type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
		if(type == eType_Reserved) {
			type = eType_General;
		}
		tryGeneralTypeFirst = true;

		isValid = Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Valor"), VALIDATE_IS_VAR_OR_NUMBER, 0, 0);
	}

	if(isValid) {
		if(Diagram->getIO(this, pin, name, type, (index == 0) ? infoIO_Name : infoIO_Max, tryGeneralTypeFirst)) {
			LadderElemCounterProp *data = (LadderElemCounterProp *)getProperties();

			if(index == 0) {
				data->idName = pin;
			} else {
				data->idMax  = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemCounter::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemCounterProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemReset
LadderElemReset::LadderElemReset(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_RES)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_Pending;
	infoIO_Name.access        = eRequestAccessType_Write;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.idName = infoIO_Name.pin;
}

pair<string, string> LadderElemReset::DrawTXT(void)
{
	return pair<string, string>(Diagram->getNameIO(prop.idName.first), _("{RES}"));
}

bool LadderElemReset::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName.first);
	const char *name = sname.c_str();

	const char *stateInOut = ic.getStateInOut();

	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_SET_VARIABLE_TO_LITERAL, name);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemReset::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemReset::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemResetProp *newProp = (LadderElemResetProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemReset::getProperties(void)
{
	LadderElemResetProp *curProp = new LadderElemResetProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemReset::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second);
}

bool LadderElemReset::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemReset::Clone(LadderDiagram *diagram)
{
	LadderElemReset *clone = new LadderElemReset(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemReset::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_Counter && type != eType_TON && type != eType_TOF &&
		type != eType_RTO && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemReset::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemReset::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return Diagram->getDetailsIO(prop.idName.first).type;
	}

	return eType_Pending;
}

int LadderElemReset::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type == eType_Counter) {
				pair<unsigned long, int> pin = prop.idName;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Name)) {
					LadderElemResetProp *data = (LadderElemResetProp *)getProperties();

					data->idName  = pin;

					setProperties(Diagram->getContext(), data);

					return 1;
				}
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemReset::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = Diagram->getDetailsIO(name).type;

	if(!acceptIO(prop.idName.first, type)) {
		Diagram->ShowDialog(eDialogType_Message, false, _("Contador/Temporizador inválido"), _("Selecione um contador/temporizador válido!"));
	} else if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Destino"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemResetProp *data = (LadderElemResetProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemReset::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemResetProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemOneShot
LadderElemOneShot::LadderElemOneShot(LadderDiagram *diagram, int which) : LadderElem(false, false, false, false, which)
{
	Diagram = diagram;
}

pair<string, string> LadderElemOneShot::DrawTXT(void)
{
	int which = getWhich();

	if(which == ELEM_ONE_SHOT_RISING) {
		return pair<string, string>("      _ ", _("[\x01OSR\x02_/ ]"));
	} else if(which == ELEM_ONE_SHOT_FALLING) {
		return pair<string, string>("    _   ", _("[\x01OSF\x02 \\_]"));
	} else oops();

	return pair<string, string>("", "");
}

bool LadderElemOneShot::internalGenerateIntCode(IntCode &ic)
{
    string storeName;
	const char *stateInOut = ic.getStateInOut();
    storeName = ic.GenSymOneShot();

	ic.Op(INT_COPY_BIT_TO_BIT, "$scratch", stateInOut);

	switch(getWhich()) {
        case ELEM_ONE_SHOT_RISING: {
			ic.Op(INT_IF_BIT_SET, storeName.c_str());
	            ic.Op(INT_CLEAR_BIT, stateInOut);
            ic.Op(INT_END_IF);

			break;
        }
        case ELEM_ONE_SHOT_FALLING: {
			ic.Op(INT_IF_BIT_CLEAR, stateInOut);
				ic.Op(INT_IF_BIT_SET, storeName.c_str());
					ic.Op(INT_SET_BIT, stateInOut);
				ic.Op(INT_END_IF);
			ic.Op(INT_ELSE);
				ic.Op(INT_CLEAR_BIT, stateInOut);
			ic.Op(INT_END_IF);

			break;
        }

		default: oops();
	}

	ic.Op(INT_COPY_BIT_TO_BIT, storeName.c_str(), "$scratch");

	return true;
}

bool LadderElemOneShot::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

LadderElem *LadderElemOneShot::Clone(LadderDiagram *diagram)
{
	LadderElemOneShot *clone = new LadderElemOneShot(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados

	return clone;
}

bool LadderElemOneShot::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemCmp
LadderElemCmp::LadderElemCmp(LadderDiagram *diagram, int which) : LadderElem(false, false, false, false, which)
{
	Diagram  = diagram;

	// Caracteristicas do I/O Op1
	infoIO_Op1.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Op1.name          = _("var");
	infoIO_Op1.isBit         = false;
	infoIO_Op1.type          = eType_General;
	infoIO_Op1.access        = eRequestAccessType_Read;
	infoIO_Op1.isUniqueRead  = false;
	infoIO_Op1.isUniqueWrite = false;

	// Caracteristicas do I/O Op2
	infoIO_Op2.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Op2.name          = "1";
	infoIO_Op2.isBit         = false;
	infoIO_Op2.type          = eType_Pending;
	infoIO_Op2.access        = eRequestAccessType_Read;
	infoIO_Op2.isUniqueRead  = false;
	infoIO_Op2.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Op1);
	Diagram->getIO(this, infoIO_Op2);

	prop.idOp1 = infoIO_Op1.pin;
	prop.idOp2 = infoIO_Op2.pin;
}

pair<string, string> LadderElemCmp::DrawTXT(void)
{
	char *s;
	string sop1 = Diagram->getNameIO(prop.idOp1);
	string sop2 = Diagram->getNameIO(prop.idOp2);
	const char *op1 = sop1.c_str();
	const char *op2 = sop2.c_str();

	switch(getWhich()) {
		case ELEM_EQU: s = "=="; break;
		case ELEM_NEQ: s = "<>"; break;
		case ELEM_GRT: s = ">" ; break;
		case ELEM_GEQ: s = ">="; break;
		case ELEM_LES: s = "<" ; break;
		case ELEM_LEQ: s = "<="; break;
		default: oops();
	}

	char s1[POS_WIDTH+20], s2[POS_WIDTH+20];
	int l1, l2, lmax;

	l1 = 2 + 1 + strlen(s) + sop1.size();
	l2 = 2 + 1 + sop2.size();
	lmax = max(l1, l2);

	memset(s1, ' ', sizeof(s1));
	s1[0] = '[';
	s1[lmax-1] = ']';
	s1[lmax] = '\0';
	strcpy(s2, s1);

	memcpy(s1+1, op1, sop1.size());
	memcpy(s1+sop1.size()+2, s, strlen(s));
	memcpy(s2+2, op2, sop2.size());

	return pair<string, string>(s1, s2);
}

bool LadderElemCmp::internalGenerateIntCode(IntCode &ic)
{
	string sop1 = Diagram->getNameIO(prop.idOp1);
	string sop2 = Diagram->getNameIO(prop.idOp2);
	const char *op1 = sop1.c_str();
	const char *op2 = sop2.c_str();

	const char *cop1 = ic.VarFromExpr(op1, "$scratch");
	const char *cop2 = ic.VarFromExpr(op2, "$scratch2");

	switch(getWhich()) {
	case ELEM_GRT:
		ic.Op(INT_IF_VARIABLE_GRT_VARIABLE   , cop1, cop2);
		ic.Op(INT_ELSE);
		break;
	case ELEM_GEQ:
		ic.Op(INT_IF_VARIABLE_GRT_VARIABLE   , cop2, cop1);
		break;
	case ELEM_LES:
		ic.Op(INT_IF_VARIABLE_GRT_VARIABLE   , cop2, cop1);
		ic.Op(INT_ELSE);
		break;
	case ELEM_LEQ:
		ic.Op(INT_IF_VARIABLE_GRT_VARIABLE   , cop1, cop2);
		break;
	case ELEM_EQU:
		ic.Op(INT_IF_VARIABLE_EQUALS_VARIABLE, cop1, cop2);
		ic.Op(INT_ELSE);
		break;
	case ELEM_NEQ:
		ic.Op(INT_IF_VARIABLE_EQUALS_VARIABLE, cop1, cop2);
		break;
	default: oops();
	}

		ic.Op(INT_CLEAR_BIT, ic.getStateInOut());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemCmp::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemCmp::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemCmpProp *newProp = (LadderElemCmpProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemCmp::getProperties(void)
{
	LadderElemCmpProp *curProp = new LadderElemCmpProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemCmp::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idOp1.first) &&
		fwrite_int  (f, prop.idOp1.second) &&
		fwrite_ulong(f, prop.idOp2.first) &&
		fwrite_int  (f, prop.idOp2.second);
}

bool LadderElemCmp::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idOp1.first) &&
		fread_int  (f, &prop.idOp1.second) &&
		fread_ulong(f, &prop.idOp2.first) &&
		fread_int  (f, &prop.idOp2.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idOp1.first);
		Diagram->updateTypeIO(prop.idOp2.first);
	}

	return ret;
}

LadderElem *LadderElemCmp::Clone(LadderDiagram *diagram)
{
	LadderElemCmp *clone = new LadderElemCmp(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemCmp::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idOp1.first || id == prop.idOp2.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemCmp::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Op1.pin = prop.idOp1;
	Diagram->updateIO(owner, this, infoIO_Op1, isDiscard);
	prop.idOp1 = infoIO_Op1.pin;

	infoIO_Op2.pin = prop.idOp2;
	Diagram->updateIO(owner, this, infoIO_Op2, isDiscard);
	prop.idOp2 = infoIO_Op2.pin;
}

eType LadderElemCmp::getAllowedTypeIO(unsigned long id)
{
	if(prop.idOp1.first == id || prop.idOp2.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemCmp::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idOp1.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idOp1;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Op1)) {
					LadderElemCmpProp *data = (LadderElemCmpProp *)getProperties();

					data->idOp1  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idOp2.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idOp2;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Op2)) {
					LadderElemCmpProp *data = (LadderElemCmpProp *)getProperties();

					data->idOp2  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemCmp::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	const char *field = (index == 0) ? _("Operador 1") : _("Operador 2");
	pair<unsigned long, int> pin = (index == 0) ? prop.idOp1 : prop.idOp2;

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field, VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, (index == 0) ? infoIO_Op1 : infoIO_Op2, true)) {
			LadderElemCmpProp *data = (LadderElemCmpProp *)getProperties();

			if(index == 0) {
				data->idOp1 = pin;
			} else {
				data->idOp2 = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemCmp::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemCmpProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemMath
LadderElemMath::LadderElemMath(LadderDiagram *diagram, int which) : LadderElem(true, false, false, false, which)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Dest
	infoIO_Dest.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Dest.name          = _("dest");
	infoIO_Dest.isBit         = false;
	infoIO_Dest.type          = eType_General;
	infoIO_Dest.access        = eRequestAccessType_Write;
	infoIO_Dest.isUniqueRead  = false;
	infoIO_Dest.isUniqueWrite = false;

	// Caracteristicas do I/O Op1
	infoIO_Op1.pin            = pair<unsigned long, int>(0, 0);
	infoIO_Op1.name           = _("src");
	infoIO_Op1.isBit          = false;
	infoIO_Op1.type           = eType_General;
	infoIO_Op1.access         = eRequestAccessType_Read;
	infoIO_Op1.isUniqueRead   = false;
	infoIO_Op1.isUniqueWrite  = false;

	// Caracteristicas do I/O Op2
	infoIO_Op2.pin            = pair<unsigned long, int>(0, 0);
	infoIO_Op2.name           = "1";
	infoIO_Op2.isBit          = false;
	infoIO_Op2.type           = eType_Pending;
	infoIO_Op2.access         = eRequestAccessType_Read;
	infoIO_Op2.isUniqueRead   = false;
	infoIO_Op2.isUniqueWrite  = false;

	Diagram->getIO(this, infoIO_Dest);
	Diagram->getIO(this, infoIO_Op1 );
	Diagram->getIO(this, infoIO_Op2 );

	prop.idDest = infoIO_Dest.pin;
	prop.idOp1  = infoIO_Op1 .pin;
	prop.idOp2  = infoIO_Op2 .pin;
}

pair<string, string> LadderElemMath::DrawTXT(void)
{
	string sop1  = Diagram->getNameIO(prop.idOp1 );
	string sop2  = Diagram->getNameIO(prop.idOp2 );
	string sdest = Diagram->getNameIO(prop.idDest);
	const char *op1  = sop1 .c_str();
	const char *op2  = sop2 .c_str();
	const char *dest = sdest.c_str();

	char op;
	char top[POS_WIDTH*2-3+2+10];
	char bot[POS_WIDTH*2-3+10];

	memset(top, ' ', sizeof(top)-1);
	top[0] = '{';
	top[1] = '\x01';

	memset(bot, ' ', sizeof(bot)-1);
	bot[0] = '{';

	int lt = 2;
	switch(getWhich()) {
		case ELEM_ADD: memcpy(top+lt, _("ADD\x02"), 4); op = '+'; break;
		case ELEM_SUB: memcpy(top+lt, _("SUB\x02"), 4); op = '-'; break;
		case ELEM_MUL: memcpy(top+lt, _("MUL\x02"), 4); op = '*'; break;
		case ELEM_DIV: memcpy(top+lt, _("DIV\x02"), 4); op = '/'; break;
		case ELEM_MOD: memcpy(top+lt, _("MOD\x02"), 4); op = '%'; break;
		default: oops();
	}

	lt += 6;
	memcpy(top+lt, dest, sdest.size());
	lt += sdest.size() + 2;
	top[lt++] = ':';
	top[lt++] = '=';

	int lb = 2;
	memcpy(bot+lb, op1, sop1.size());
	lb += sop1.size() + 1;
	bot[lb++] = op;
	lb++;
	memcpy(bot+lb, op2, sop2.size());
	lb += sop2.size();

	int l = max(lb, lt - 2);
	top[l+2] = '}'; top[l+3] = '\0';
	bot[l] = '}'; bot[l+1] = '\0';

	return pair<string, string>(top, bot);
}

bool LadderElemMath::internalGenerateIntCode(IntCode &ic)
{
	string sop1  = Diagram->getNameIO(prop.idOp1 );
	string sop2  = Diagram->getNameIO(prop.idOp2 );
	string sdest = Diagram->getNameIO(prop.idDest);
	const char *op1  = sop1 .c_str();
	const char *op2  = sop2 .c_str();
	const char *dest = sdest.c_str();

	if(IsNumber(dest)) {
		Error(_("Instruções Math: o destino '%s' não é válido."), dest);
		CompileError();
	}
	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	const char *cop1 = ic.VarFromExpr(op1, "$scratch");
	const char *cop2 = ic.VarFromExpr(op2, "$scratch2");

	int intOp;
	switch(getWhich()) {
		case ELEM_ADD: intOp = INT_SET_VARIABLE_ADD     ; break;
		case ELEM_SUB: intOp = INT_SET_VARIABLE_SUBTRACT; break;
		case ELEM_MUL: intOp = INT_SET_VARIABLE_MULTIPLY; break;
		case ELEM_DIV: intOp = INT_SET_VARIABLE_DIVIDE  ; break;
		case ELEM_MOD: intOp = INT_SET_VARIABLE_MODULO  ; break;
		default: oops();
	}

	ic.Op(intOp, dest, cop1, cop2, 0, 0);

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemMath::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemMath::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemMathProp *newProp = (LadderElemMathProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemMath::getProperties(void)
{
	LadderElemMathProp *curProp = new LadderElemMathProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemMath::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idDest.first) &&
		fwrite_int  (f, prop.idDest.second) &&
		fwrite_ulong(f, prop.idOp1.first) &&
		fwrite_int  (f, prop.idOp1.second) &&
		fwrite_ulong(f, prop.idOp2.first) &&
		fwrite_int  (f, prop.idOp2.second);
}

bool LadderElemMath::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idOp1.first) &&
		fread_int  (f, &prop.idOp1.second) &&
		fread_ulong(f, &prop.idOp2.first) &&
		fread_int  (f, &prop.idOp2.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idDest.first);
		Diagram->updateTypeIO(prop.idOp1 .first);
		Diagram->updateTypeIO(prop.idOp2 .first);
	}

	return ret;
}

LadderElem *LadderElemMath::Clone(LadderDiagram *diagram)
{
	LadderElemMath *clone = new LadderElemMath(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemMath::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idOp1.first || id == prop.idOp2.first || id == prop.idDest.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemMath::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Dest.pin = prop.idDest;
	Diagram->updateIO(owner, this, infoIO_Dest, isDiscard);
	prop.idDest = infoIO_Dest.pin;

	infoIO_Op1.pin = prop.idOp1;
	Diagram->updateIO(owner, this, infoIO_Op1, isDiscard);
	prop.idOp1 = infoIO_Op1.pin;

	infoIO_Op2.pin = prop.idOp2;
	Diagram->updateIO(owner, this, infoIO_Op2, isDiscard);
	prop.idOp2 = infoIO_Op2.pin;
}

eType LadderElemMath::getAllowedTypeIO(unsigned long id)
{
	if(prop.idDest.first == id || prop.idOp1.first == id || prop.idOp2.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemMath::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idDest.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idDest;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Dest)) {
					LadderElemMathProp *data = (LadderElemMathProp *)getProperties();

					data->idDest  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idOp1.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idOp1;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Op1)) {
					LadderElemMathProp *data = (LadderElemMathProp *)getProperties();

					data->idOp1  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idOp2.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idOp2;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Op2)) {
					LadderElemMathProp *data = (LadderElemMathProp *)getProperties();

					data->idOp2  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemMath::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	const char *field;
	tRequestIO request;
	pair<unsigned long, int> pin;

	if(index == 0) {
		request = infoIO_Op1;
		pin     = prop.idOp1;
		field   = _("Primeiro Parâmetro");
	} else if(index == 1) {
		request = infoIO_Op2;
		pin     = prop.idOp2;
		field   = _("Segundo Parâmetro");
	} else {
		request = infoIO_Dest;
		pin     = prop.idDest;
		field   = _("Destino");
	}

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field,
			(index == 2) ? VALIDATE_IS_VAR : VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, request, true)) {
			LadderElemMathProp *data = (LadderElemMathProp *)getProperties();

			if(index == 0) {
				data->idOp1  = pin;
			} else if(index == 1) {
				data->idOp2  = pin;
			} else {
				data->idDest = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemMath::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemMathProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemSqrt
LadderElemSqrt::LadderElemSqrt(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_SQRT)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Dest
	infoIO_Dest.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Dest.name          = _("dest");
	infoIO_Dest.isBit         = false;
	infoIO_Dest.type          = eType_General;
	infoIO_Dest.access        = eRequestAccessType_Write;
	infoIO_Dest.isUniqueRead  = false;
	infoIO_Dest.isUniqueWrite = false;

	// Caracteristicas do I/O Src
	infoIO_Src.pin            = pair<unsigned long, int>(0, 0);
	infoIO_Src.name           = _("src");
	infoIO_Src.isBit          = false;
	infoIO_Src.type           = eType_General;
	infoIO_Src.access         = eRequestAccessType_Read;
	infoIO_Src.isUniqueRead   = false;
	infoIO_Src.isUniqueWrite  = false;

	Diagram->getIO(this, infoIO_Dest);
	Diagram->getIO(this, infoIO_Src );

	prop.idDest = infoIO_Dest.pin;
	prop.idSrc  = infoIO_Src .pin;
}

pair<string, string> LadderElemSqrt::DrawTXT(void)
{
	string ssrc  = Diagram->getNameIO(prop.idSrc );
	string sdest = Diagram->getNameIO(prop.idDest);
	const char *src  = ssrc .c_str();
	const char *dest = sdest.c_str();

	char top[256];
	char bot[256];

	sprintf(top, "{%s :=}", dest);
	sprintf(bot, _("{%s SQRT}"), src);

	return pair<string, string>(top, bot);
}

bool LadderElemSqrt::internalGenerateIntCode(IntCode &ic)
{
	string ssrc  = Diagram->getNameIO(prop.idSrc );
	string sdest = Diagram->getNameIO(prop.idDest);
	const char *src  = ssrc .c_str();
	const char *dest = sdest.c_str();

	if(IsNumber(dest)) {
		Error(_("Sqrt instruction: '%s' not a valid destination."), dest);
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	if(IsNumber(src)) {
		ic.Op(INT_SET_VARIABLE_TO_LITERAL, dest, CheckMakeNumber(src));
	} else {
		ic.Op(INT_SET_VARIABLE_TO_VARIABLE, dest, src, 0);
	}

		ic.Op(INT_SQRT, dest);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemSqrt::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemSqrt::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemSqrtProp *newProp = (LadderElemSqrtProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemSqrt::getProperties(void)
{
	LadderElemSqrtProp *curProp = new LadderElemSqrtProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemSqrt::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idDest.first) &&
		fwrite_int  (f, prop.idDest.second) &&
		fwrite_ulong(f, prop.idSrc.first) &&
		fwrite_int  (f, prop.idSrc.second);
}

bool LadderElemSqrt::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idSrc.first) &&
		fread_int  (f, &prop.idSrc.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idDest.first);
		Diagram->updateTypeIO(prop.idSrc .first);
	}

	return ret;
}

LadderElem *LadderElemSqrt::Clone(LadderDiagram *diagram)
{
	LadderElemSqrt *clone = new LadderElemSqrt(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemSqrt::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idSrc.first || id == prop.idDest.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemSqrt::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Dest.pin = prop.idDest;
	Diagram->updateIO(owner, this, infoIO_Dest, isDiscard);
	prop.idDest = infoIO_Dest.pin;

	infoIO_Src.pin = prop.idSrc;
	Diagram->updateIO(owner, this, infoIO_Src, isDiscard);
	prop.idSrc = infoIO_Src.pin;
}

eType LadderElemSqrt::getAllowedTypeIO(unsigned long id)
{
	if(prop.idDest.first == id || prop.idSrc.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemSqrt::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idDest.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idDest;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Dest)) {
					LadderElemSqrtProp *data = (LadderElemSqrtProp *)getProperties();

					data->idDest  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idSrc.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idSrc;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Src)) {
					LadderElemSqrtProp *data = (LadderElemSqrtProp *)getProperties();

					data->idSrc  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemSqrt::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin;

	const char *field;
	unsigned int rules;
	if(index == 0) {
		pin   = prop.idSrc;
		field = _("Origem");
		rules = VALIDATE_IS_VAR_OR_NUMBER;
	} else {
		pin   = prop.idDest;
		field = _("Destino");
		rules = VALIDATE_IS_VAR;
	}

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field, rules, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, (index == 0) ? infoIO_Src : infoIO_Dest, true)) {
			LadderElemSqrtProp *data = (LadderElemSqrtProp *)getProperties();

			if(index == 0) {
				data->idSrc  = pin;
			} else {
				data->idDest = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemSqrt::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemSqrtProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemRand
LadderElemRand::LadderElemRand(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_RAND)
{
	Diagram  = diagram;

	// Caracteristicas do I/O Var
	infoIO_Var.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Var.name          = _("var");
	infoIO_Var.isBit         = false;
	infoIO_Var.type          = eType_General;
	infoIO_Var.access        = eRequestAccessType_Write;
	infoIO_Var.isUniqueRead  = false;
	infoIO_Var.isUniqueWrite = false;

	// Caracteristicas do I/O Min
	infoIO_Min.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Min.name          = "0";
	infoIO_Min.isBit         = false;
	infoIO_Min.type          = eType_Pending;
	infoIO_Min.access        = eRequestAccessType_Read;
	infoIO_Min.isUniqueRead  = false;
	infoIO_Min.isUniqueWrite = false;

	// Caracteristicas do I/O Max
	infoIO_Max.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Max.name          = "100";
	infoIO_Max.isBit         = false;
	infoIO_Max.type          = eType_Pending;
	infoIO_Max.access        = eRequestAccessType_Read;
	infoIO_Max.isUniqueRead  = false;
	infoIO_Max.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Var);
	Diagram->getIO(this, infoIO_Min);
	Diagram->getIO(this, infoIO_Max);

	prop.idVar = infoIO_Var.pin;
	prop.idMin = infoIO_Min.pin;
	prop.idMax = infoIO_Max.pin;
}

pair<string, string> LadderElemRand::DrawTXT(void)
{
	string svar = Diagram->getNameIO(prop.idVar);
	string smin = Diagram->getNameIO(prop.idMin);
	string smax = Diagram->getNameIO(prop.idMax);
	const char *var = svar.c_str();
	const char *min = smin.c_str();
	const char *max = smax.c_str();

	char top[256];
	char bot[256];

	sprintf(top, "{ %s := }", var);
	sprintf(bot, "{%s <= ? <= %s}", min, max);

	return pair<string, string>(top, bot);
}

bool LadderElemRand::internalGenerateIntCode(IntCode &ic)
{
	string svar = Diagram->getNameIO(prop.idVar);
	string smin = Diagram->getNameIO(prop.idMin);
	string smax = Diagram->getNameIO(prop.idMax);
	const char *var = svar.c_str();
	const char *min = smin.c_str();
	const char *max = smax.c_str();

	if(IsNumber(var)) {
		Error(_("Rand instruction: '%s' not a valid destination."), var);
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_RAND, var, min, max, 0, 0);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemRand::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemRand::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemRandProp *newProp = (LadderElemRandProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemRand::getProperties(void)
{
	LadderElemRandProp *curProp = new LadderElemRandProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemRand::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idMin.first) &&
		fwrite_int  (f, prop.idMin.second) &&
		fwrite_ulong(f, prop.idVar.first) &&
		fwrite_int  (f, prop.idVar.second) &&
		fwrite_ulong(f, prop.idMax.first) &&
		fwrite_int  (f, prop.idMax.second);
}

bool LadderElemRand::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idMin.first) &&
		fread_int  (f, &prop.idMin.second) &&
		fread_ulong(f, &prop.idVar.first) &&
		fread_int  (f, &prop.idVar.second) &&
		fread_ulong(f, &prop.idMax.first) &&
		fread_int  (f, &prop.idMax.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idMin.first);
		Diagram->updateTypeIO(prop.idVar.first);
		Diagram->updateTypeIO(prop.idMax.first);
	}

	return ret;
}

LadderElem *LadderElemRand::Clone(LadderDiagram *diagram)
{
	LadderElemRand *clone = new LadderElemRand(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemRand::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idVar.first || id == prop.idMin.first || id == prop.idMax.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemRand::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Var.pin = prop.idVar;
	Diagram->updateIO(owner, this, infoIO_Var, isDiscard);
	prop.idVar = infoIO_Var.pin;

	infoIO_Min.pin = prop.idMin;
	Diagram->updateIO(owner, this, infoIO_Min, isDiscard);
	prop.idMin = infoIO_Min.pin;

	infoIO_Max.pin = prop.idMax;
	Diagram->updateIO(owner, this, infoIO_Max, isDiscard);
	prop.idMax = infoIO_Max.pin;
}

eType LadderElemRand::getAllowedTypeIO(unsigned long id)
{
	if(prop.idVar.first == id || prop.idMax.first == id || prop.idMin.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemRand::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idVar.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idVar;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Var)) {
					LadderElemRandProp *data = (LadderElemRandProp *)getProperties();

					data->idVar  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idMin.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idMin;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Min)) {
					LadderElemRandProp *data = (LadderElemRandProp *)getProperties();

					data->idMin  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idMax.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idMax;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Max)) {
					LadderElemRandProp *data = (LadderElemRandProp *)getProperties();

					data->idMax  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemRand::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	tRequestIO request;
	pair<unsigned long, int> pin;

	const char *field;
	unsigned int rules;
	if(index == 0) {
		request = infoIO_Min;
		pin     = prop.idMin;
		field   = _("Mínimo");
		rules   = VALIDATE_IS_VAR_OR_NUMBER;
	} else if(index == 1) {
		request = infoIO_Var;
		pin     = prop.idVar;
		field   = _("Destino");
		rules   = VALIDATE_IS_VAR;
	} else {
		request = infoIO_Max;
		pin     = prop.idMax;
		field   = _("Máximo");
		rules   = VALIDATE_IS_VAR_OR_NUMBER;
	}

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field, rules, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, request, true)) {
			LadderElemRandProp *data = (LadderElemRandProp *)getProperties();

			if(index == 0) {
				data->idMin  = pin;
			} else if(index == 1) {
				data->idVar  = pin;
			} else {
				data->idMax = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemRand::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemRandProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemAbs
LadderElemAbs::LadderElemAbs(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_ABS)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Dest
	infoIO_Dest.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Dest.name          = _("dest");
	infoIO_Dest.isBit         = false;
	infoIO_Dest.type          = eType_General;
	infoIO_Dest.access        = eRequestAccessType_Write;
	infoIO_Dest.isUniqueRead  = false;
	infoIO_Dest.isUniqueWrite = false;

	// Caracteristicas do I/O Src
	infoIO_Src.pin            = pair<unsigned long, int>(0, 0);
	infoIO_Src.name           = _("src");
	infoIO_Src.isBit          = false;
	infoIO_Src.type           = eType_General;
	infoIO_Src.access         = eRequestAccessType_Read;
	infoIO_Src.isUniqueRead   = false;
	infoIO_Src.isUniqueWrite  = false;

	Diagram->getIO(this, infoIO_Dest);
	Diagram->getIO(this, infoIO_Src );

	prop.idDest = infoIO_Dest.pin;
	prop.idSrc  = infoIO_Src .pin;
}

pair<string, string> LadderElemAbs::DrawTXT(void)
{
	string ssrc  = Diagram->getNameIO(prop.idSrc );
	string sdest = Diagram->getNameIO(prop.idDest);
	const char *src  = ssrc .c_str();
	const char *dest = sdest.c_str();

	char top[256];
	char bot[256];

	sprintf(top, "{ %s :=}", dest);
	sprintf(bot, "{ |%s| }", src);

	return pair<string, string>(top, bot);
}

bool LadderElemAbs::internalGenerateIntCode(IntCode &ic)
{
	string ssrc  = Diagram->getNameIO(prop.idSrc );
	string sdest = Diagram->getNameIO(prop.idDest);
	const char *src  = ssrc .c_str();
	const char *dest = sdest.c_str();

	if(IsNumber(dest)) {
		Error(_("Abs instruction: '%s' not a valid destination."), dest);
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_IF_VARIABLE_LES_LITERAL, src, (SWORD)0);
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch_int", CheckMakeNumber("-1"));
			ic.Op(INT_SET_VARIABLE_MULTIPLY, dest, src, "$scratch_int", 0, 0);
		ic.Op(INT_ELSE);
			if(IsNumber(src)) {
				ic.Op(INT_SET_VARIABLE_TO_LITERAL, dest, CheckMakeNumber(src));
			} else {
				ic.Op(INT_SET_VARIABLE_TO_VARIABLE, dest, src, 0);
			}
		ic.Op(INT_END_IF);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemAbs::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemAbs::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemAbsProp *newProp = (LadderElemAbsProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemAbs::getProperties(void)
{
	LadderElemAbsProp *curProp = new LadderElemAbsProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemAbs::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idDest.first) &&
		fwrite_int  (f, prop.idDest.second) &&
		fwrite_ulong(f, prop.idSrc.first) &&
		fwrite_int  (f, prop.idSrc.second);
}

bool LadderElemAbs::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idSrc.first) &&
		fread_int  (f, &prop.idSrc.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idDest.first);
		Diagram->updateTypeIO(prop.idSrc .first);
	}

	return ret;
}

LadderElem *LadderElemAbs::Clone(LadderDiagram *diagram)
{
	LadderElemAbs *clone = new LadderElemAbs(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemAbs::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idSrc.first || id == prop.idDest.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemAbs::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Dest.pin = prop.idDest;
	Diagram->updateIO(owner, this, infoIO_Dest, isDiscard);
	prop.idDest = infoIO_Dest.pin;

	infoIO_Src.pin = prop.idSrc;
	Diagram->updateIO(owner, this, infoIO_Src, isDiscard);
	prop.idSrc = infoIO_Src.pin;
}

eType LadderElemAbs::getAllowedTypeIO(unsigned long id)
{
	if(prop.idDest.first == id || prop.idSrc.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemAbs::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idDest.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idDest;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Dest)) {
					LadderElemAbsProp *data = (LadderElemAbsProp *)getProperties();

					data->idDest  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idSrc.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idSrc;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Src)) {
					LadderElemAbsProp *data = (LadderElemAbsProp *)getProperties();

					data->idSrc  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemAbs::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin;

	const char *field;
	unsigned int rules;
	if(index == 0) {
		pin   = prop.idSrc;
		field = _("Origem");
		rules = VALIDATE_IS_VAR_OR_NUMBER;
	} else {
		pin   = prop.idDest;
		field = _("Destino");
		rules = VALIDATE_IS_VAR;
	}

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field, rules, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, (index == 0) ? infoIO_Src : infoIO_Dest, true)) {
			LadderElemAbsProp *data = (LadderElemAbsProp *)getProperties();

			if(index == 0) {
				data->idSrc  = pin;
			} else {
				data->idDest = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemAbs::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemAbsProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemMove
LadderElemMove::LadderElemMove(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_MOVE)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Dest
	infoIO_Dest.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Dest.name          = _("dest");
	infoIO_Dest.isBit         = false;
	infoIO_Dest.type          = eType_General;
	infoIO_Dest.access        = eRequestAccessType_Write;
	infoIO_Dest.isUniqueRead  = false;
	infoIO_Dest.isUniqueWrite = false;

	// Caracteristicas do I/O Src
	infoIO_Src.pin            = pair<unsigned long, int>(0, 0);
	infoIO_Src.name           = _("src");
	infoIO_Src.isBit          = false;
	infoIO_Src.type           = eType_General;
	infoIO_Src.access         = eRequestAccessType_Read;
	infoIO_Src.isUniqueRead   = false;
	infoIO_Src.isUniqueWrite  = false;

	Diagram->getIO(this, infoIO_Dest);
	Diagram->getIO(this, infoIO_Src );

	prop.idDest = infoIO_Dest.pin;
	prop.idSrc  = infoIO_Src .pin;
}

pair<string, string> LadderElemMove::DrawTXT(void)
{
	string ssrc  = Diagram->getNameIO(prop.idSrc );
	string sdest = Diagram->getNameIO(prop.idDest);
	const char *src  = ssrc .c_str();
	const char *dest = sdest.c_str();

	char top[256];
	char bot[256];

	sprintf(top, "{%s :=}", dest);
	sprintf(bot, _("{%s MOV}"), src);

	return pair<string, string>(top, bot);
}

bool LadderElemMove::internalGenerateIntCode(IntCode &ic)
{
	string ssrc  = Diagram->getNameIO(prop.idSrc );
	string sdest = Diagram->getNameIO(prop.idDest);
	const char *src  = ssrc .c_str();
	const char *dest = sdest.c_str();

	if(IsNumber(dest)) {
		Error(_("Instrução Mover: o destino '%s' não é válido."), src);
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	if(IsNumber(src)) {
		ic.Op(INT_SET_VARIABLE_TO_LITERAL, dest, CheckMakeNumber(src));
	} else {
		ic.Op(INT_SET_VARIABLE_TO_VARIABLE, dest, src, 0);
	}

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemMove::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemMove::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemMoveProp *newProp = (LadderElemMoveProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemMove::getProperties(void)
{
	LadderElemMoveProp *curProp = new LadderElemMoveProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemMove::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idDest.first) &&
		fwrite_int  (f, prop.idDest.second) &&
		fwrite_ulong(f, prop.idSrc.first) &&
		fwrite_int  (f, prop.idSrc.second);
}

bool LadderElemMove::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idSrc.first) &&
		fread_int  (f, &prop.idSrc.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idDest.first);
		Diagram->updateTypeIO(prop.idSrc .first);
	}

	return ret;
}

LadderElem *LadderElemMove::Clone(LadderDiagram *diagram)
{
	LadderElemMove *clone = new LadderElemMove(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemMove::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idSrc.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	if(id == prop.idDest.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemMove::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Dest.pin = prop.idDest;
	Diagram->updateIO(owner, this, infoIO_Dest, isDiscard);
	prop.idDest = infoIO_Dest.pin;

	infoIO_Src.pin = prop.idSrc;
	Diagram->updateIO(owner, this, infoIO_Src, isDiscard);
	prop.idSrc = infoIO_Src.pin;
}

eType LadderElemMove::getAllowedTypeIO(unsigned long id)
{
	if(prop.idDest.first == id || prop.idSrc.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemMove::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idDest.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idDest;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Dest)) {
					LadderElemMoveProp *data = (LadderElemMoveProp *)getProperties();

					data->idDest  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idSrc.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idSrc;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Src)) {
					LadderElemMoveProp *data = (LadderElemMoveProp *)getProperties();

					data->idSrc  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemMove::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	bool isValid;
	pair<unsigned long, int> pin = (index == 0) ? prop.idSrc : prop.idDest;

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(index == 0) {
		isValid = Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Origem"), VALIDATE_IS_VAR_OR_NUMBER, 0, 0);
	} else {
		isValid = Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Destino"), VALIDATE_IS_VAR, 0, 0);
	}

	if(isValid) {
		if(Diagram->getIO(this, pin, name, type, (index == 0) ? infoIO_Src : infoIO_Dest, true)) {
			LadderElemMoveProp *data = (LadderElemMoveProp *)getProperties();

			if(index == 0) {
				data->idSrc  = pin;
			} else {
				data->idDest = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemMove::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemMoveProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemOpenShort
LadderElemOpenShort::LadderElemOpenShort(LadderDiagram *diagram, int which) : LadderElem(false, false, false, false, which)
{
	Diagram = diagram;
}

pair<string, string> LadderElemOpenShort::DrawTXT(void)
{
	switch(getWhich()) {
		case ELEM_OPEN : return pair<string, string>("", "+      +");
		case ELEM_SHORT: return pair<string, string>("", "+------+");
		default: oops();
	}

	return pair<string, string>("", "");
}

bool LadderElemOpenShort::internalGenerateIntCode(IntCode &ic)
{
	if(getWhich() == ELEM_OPEN) {
		ic.Op(INT_CLEAR_BIT, ic.getStateInOut());
	}

	return true;
}

bool LadderElemOpenShort::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

LadderElem *LadderElemOpenShort::Clone(LadderDiagram *diagram)
{
	LadderElemOpenShort *clone = new LadderElemOpenShort(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados

	return clone;
}

bool LadderElemOpenShort::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Sempre retorna sucesso...
}

// Classe LadderElemSetBit
LadderElemSetBit::LadderElemSetBit(LadderDiagram *diagram) : LadderElem(false, false, false, false, ELEM_SET_BIT)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_General;
	infoIO_Name.access        = eRequestAccessType_Write;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.bit    = 0;
	prop.set    = false;
	prop.idName = infoIO_Name.pin;
}

pair<string, string> LadderElemSetBit::DrawTXT(void)
{
	char str[100];

	sprintf(str, _("{%s BIT:%d}"), prop.set ? _("SET") : _("RST"), prop.bit);

	return pair<string, string>(Diagram->getNameIO(prop.idName), str);
}

bool LadderElemSetBit::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name  = sname.c_str();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	if(prop.set) {
		ic.OpBit(INT_SET_SINGLE_BIT  , name, prop.bit);
	} else {
		ic.OpBit(INT_CLEAR_SINGLE_BIT, name, prop.bit);
	}

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemSetBit::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemSetBit::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemSetBitProp *newProp = (LadderElemSetBitProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemSetBit::getProperties(void)
{
	LadderElemSetBitProp *curProp = new LadderElemSetBitProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemSetBit::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second) &&
		fwrite_bool (f, prop.set) &&
		fwrite_int  (f, prop.bit);
}

bool LadderElemSetBit::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_bool (f, &prop.set) &&
		fread_int  (f, &prop.bit);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemSetBit::Clone(LadderDiagram *diagram)
{
	LadderElemSetBit *clone = new LadderElemSetBit(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemSetBit::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemSetBit::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemSetBit::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemSetBit::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}

				pair<unsigned long, int> pin = prop.idName;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Name)) {
					LadderElemSetBitProp *data = (LadderElemSetBitProp *)getProperties();

					data->idName  = pin;

					setProperties(Diagram->getContext(), data);

					return 1;
				}
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemSetBit::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Nome"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name, true)) {
			LadderElemSetBitProp *data = (LadderElemSetBitProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemSetBit::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemSetBitProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemCheckBit
LadderElemCheckBit::LadderElemCheckBit(LadderDiagram *diagram) : LadderElem(false, false, false, false, ELEM_CHECK_BIT)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_General;
	infoIO_Name.access        = eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.bit    = 0;
	prop.set    = false;
	prop.idName = infoIO_Name.pin;
}

pair<string, string> LadderElemCheckBit::DrawTXT(void)
{
	char str[100];

	sprintf(str, _("{CHECK %s:%d}"), prop.set ? _("ON") : _("OFF"), prop.bit);

	return pair<string, string>(Diagram->getNameIO(prop.idName), str);
}

bool LadderElemCheckBit::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name  = sname.c_str();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	if(prop.set) {
		ic.OpBit(INT_IF_BIT_CHECK_CLEAR, name, prop.bit);
	} else {
		ic.OpBit(INT_IF_BIT_CHECK_SET  , name, prop.bit);
	}

			ic.OpBit(INT_CLEAR_BIT, ic.getStateInOut(), prop.set);
		ic.Op(INT_END_IF);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemCheckBit::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemCheckBit::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemCheckBitProp *newProp = (LadderElemCheckBitProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemCheckBit::getProperties(void)
{
	LadderElemCheckBitProp *curProp = new LadderElemCheckBitProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemCheckBit::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second) &&
		fwrite_bool (f, prop.set) &&
		fwrite_int  (f, prop.bit);
}

bool LadderElemCheckBit::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_bool (f, &prop.set) &&
		fread_int  (f, &prop.bit);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemCheckBit::Clone(LadderDiagram *diagram)
{
	LadderElemCheckBit *clone = new LadderElemCheckBit(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemCheckBit::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemCheckBit::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemCheckBit::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemCheckBit::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}

				pair<unsigned long, int> pin = prop.idName;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Name)) {
					LadderElemCheckBitProp *data = (LadderElemCheckBitProp *)getProperties();

					data->idName  = pin;

					setProperties(Diagram->getContext(), data);

					return 1;
				}
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemCheckBit::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Nome"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name, true)) {
			LadderElemCheckBitProp *data = (LadderElemCheckBitProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemCheckBit::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemCheckBitProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemReadAdc
LadderElemReadAdc::LadderElemReadAdc(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_READ_ADC)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_ReadADC;
	infoIO_Name.access        = eRequestAccessType_Write;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.idName        = infoIO_Name.pin;
}

string LadderElemReadAdc::GetNameADC(void)
{
	switch(Diagram->getDetailsIO(prop.idName.first).pin) {
	case 1:
		return "AD1";
		break;
	case 2:
		return "AD2";
		break;
	case 3:
		return "AD3";
		break;
	case 4:
		return "AD4";
		break;
	case 5:
		return "AD5";
		break;
	case 6:
		return "TEMP";
		break;
	}

	return "?";
}

pair<string, string> LadderElemReadAdc::DrawTXT(void)
{
	char txt[50];

	sprintf(txt, _("{READ ADC %s }"), GetNameADC().c_str());

	return pair<string, string>(Diagram->getNameIO(prop.idName), txt);
}

bool LadderElemReadAdc::internalGenerateIntCode(IntCode &ic)
{
	LadderSettingsADC settings = Diagram->getSettingsADC();
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_READ_ADC, name);
		if(!settings.isCelsius) {
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", 9);
			ic.Op(INT_SET_VARIABLE_MULTIPLY, name, name, "$scratch", 0, 0);

			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", 5);
			ic.Op(INT_SET_VARIABLE_DIVIDE, name, name, "$scratch", 0, 0);

			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", 32);
			ic.Op(INT_SET_VARIABLE_ADD, name, name, "$scratch", 0, 0);
		}
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemReadAdc::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemReadAdc::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemReadAdcProp *newProp = (LadderElemReadAdcProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemReadAdc::getProperties(void)
{
	LadderElemReadAdcProp *curProp = new LadderElemReadAdcProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemReadAdc::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second);
}

bool LadderElemReadAdc::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemReadAdc::Clone(LadderDiagram *diagram)
{
	LadderElemReadAdc *clone = new LadderElemReadAdc(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemReadAdc::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_ReadADC && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemReadAdc::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemReadAdc::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return eType_ReadADC;
	}

	return eType_Pending;
}

int LadderElemReadAdc::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, eType_ReadADC, infoIO_Name)) {
				LadderElemReadAdcProp *data = (LadderElemReadAdcProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemReadAdc::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = eType_ReadADC;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Nome" ), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemReadAdcProp *data = (LadderElemReadAdcProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemReadAdc::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemReadAdcProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemSetDa
LadderElemSetDa::LadderElemSetDa(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_SET_DA)
{
	Diagram    = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_SetDAC;
	infoIO_Name.access        = eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.mode   = ELEM_SET_DA_MODE_RAW;
	prop.idName = infoIO_Name.pin;
}

pair<string, string> LadderElemSetDa::DrawTXT(void)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	char cname[50];

	switch(prop.mode) {
	case ELEM_SET_DA_MODE_RAW:
		strcpy(cname, name);
		break;

	case ELEM_SET_DA_MODE_MV:
		sprintf(cname, "%s mV", name);
		break;

	case ELEM_SET_DA_MODE_PCT:
		sprintf(cname, "%s %%", name);
		break;
	}

	return pair<string, string>(cname, _("{SET DA}"));
}

bool LadderElemSetDa::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_SET_DA, name, prop.mode);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemSetDa::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemSetDa::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemSetDaProp *newProp = (LadderElemSetDaProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemSetDa::getProperties(void)
{
	LadderElemSetDaProp *curProp = new LadderElemSetDaProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemSetDa::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second) &&
		fwrite_int  (f, prop.mode);
}

bool LadderElemSetDa::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_int  (f, &prop.mode);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemSetDa::Clone(LadderDiagram *diagram)
{
	LadderElemSetDa *clone = new LadderElemSetDa(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemSetDa::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_SetDAC && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemSetDa::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemSetDa::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return eType_SetDAC;
	}

	return eType_Pending;
}

int LadderElemSetDa::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, eType_SetDAC, infoIO_Name)) {
				LadderElemSetDaProp *data = (LadderElemSetDaProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemSetDa::isValidDaValue(string new_name, int new_mode)
{
	int min, max;

	switch(new_mode) {
	case ELEM_SET_DA_MODE_RAW:
		min = -DA_RESOLUTION;
		max =  DA_RESOLUTION - 1;
		break;

	case ELEM_SET_DA_MODE_MV:
		min = -10000;
		max = +10000;
		break;

	case ELEM_SET_DA_MODE_PCT:
		min = -100;
		max = +100;
	}

	return Diagram->IsValidNameAndType(prop.idName.first, new_name, eType_SetDAC, _("Origem"), VALIDATE_IS_VAR_OR_NUMBER, min, max);
}

bool LadderElemSetDa::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = eType_SetDAC;

	if(isValidDaValue(name, prop.mode)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemSetDaProp *data = (LadderElemSetDaProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemSetDa::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemSetDaProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemReadEnc
LadderElemReadEnc::LadderElemReadEnc(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_READ_ENC)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_ReadEnc;
	infoIO_Name.access        = eRequestAccessType_Write;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.idName = infoIO_Name.pin;

	// Caracteristicas do I/O IncPerimRoda
	infoIO_IncPerimRoda.pin           = pair<unsigned long, int>(0, 0);
	infoIO_IncPerimRoda.name          = "IncPerimRoda";
	infoIO_IncPerimRoda.isBit         = false;
	infoIO_IncPerimRoda.type          = eType_General;
	infoIO_IncPerimRoda.access        = eRequestAccessType_ReadWrite;
	infoIO_IncPerimRoda.isUniqueRead  = false;
	infoIO_IncPerimRoda.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_IncPerimRoda);

	// Caracteristicas do I/O IncPulsosVolta
	infoIO_IncPulsosVolta.pin           = pair<unsigned long, int>(0, 0);
	infoIO_IncPulsosVolta.name          = "IncPulsosVolta";
	infoIO_IncPulsosVolta.isBit         = false;
	infoIO_IncPulsosVolta.type          = eType_General;
	infoIO_IncPulsosVolta.access        = eRequestAccessType_ReadWrite;
	infoIO_IncPulsosVolta.isUniqueRead  = false;
	infoIO_IncPulsosVolta.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_IncPulsosVolta);

	// Caracteristicas do I/O IncFatorCorr
	infoIO_IncFatorCorr.pin           = pair<unsigned long, int>(0, 0);
	infoIO_IncFatorCorr.name          = "IncFatorCorr";
	infoIO_IncFatorCorr.isBit         = false;
	infoIO_IncFatorCorr.type          = eType_General;
	infoIO_IncFatorCorr.access        = eRequestAccessType_ReadWrite;
	infoIO_IncFatorCorr.isUniqueRead  = false;
	infoIO_IncFatorCorr.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_IncFatorCorr);

	// Caracteristicas do I/O AbsPerimRoda
	infoIO_AbsPerimRoda.pin           = pair<unsigned long, int>(0, 0);
	infoIO_AbsPerimRoda.name          = "AbsPerimRoda";
	infoIO_AbsPerimRoda.isBit         = false;
	infoIO_AbsPerimRoda.type          = eType_General;
	infoIO_AbsPerimRoda.access        = eRequestAccessType_ReadWrite;
	infoIO_AbsPerimRoda.isUniqueRead  = false;
	infoIO_AbsPerimRoda.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_AbsPerimRoda);

	// Caracteristicas do I/O AbsFatorCorr
	infoIO_AbsFatorCorr.pin           = pair<unsigned long, int>(0, 0);
	infoIO_AbsFatorCorr.name          = "AbsFatorCorr";
	infoIO_AbsFatorCorr.isBit         = false;
	infoIO_AbsFatorCorr.type          = eType_General;
	infoIO_AbsFatorCorr.access        = eRequestAccessType_ReadWrite;
	infoIO_AbsFatorCorr.isUniqueRead  = false;
	infoIO_AbsFatorCorr.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_AbsFatorCorr);
}

pair<string, string> LadderElemReadEnc::DrawTXT(void)
{
	return pair<string, string>(Diagram->getNameIO(prop.idName), _("{READ ENC}"));
}

bool LadderElemReadEnc::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_READ_ENC, name);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemReadEnc::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemReadEnc::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemReadEncProp *newProp = (LadderElemReadEncProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemReadEnc::getProperties(void)
{
	LadderElemReadEncProp *curProp = new LadderElemReadEncProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemReadEnc::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second);
}

bool LadderElemReadEnc::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);

		// As variaveis internas nao tem seus IDs salvos no arquivo
		// Assim precisam ser carregadas a partir da lista de I/O
		infoIO_AbsFatorCorr  .pin.first = Diagram->getIdIO(infoIO_AbsFatorCorr  .name);
		infoIO_AbsPerimRoda  .pin.first = Diagram->getIdIO(infoIO_AbsPerimRoda  .name);
		infoIO_IncFatorCorr  .pin.first = Diagram->getIdIO(infoIO_IncFatorCorr  .name);
		infoIO_IncPerimRoda  .pin.first = Diagram->getIdIO(infoIO_IncPerimRoda  .name);
		infoIO_IncPulsosVolta.pin.first = Diagram->getIdIO(infoIO_IncPulsosVolta.name);
	}

	return ret;
}

LadderElem *LadderElemReadEnc::Clone(LadderDiagram *diagram)
{
	LadderElemReadEnc *clone = new LadderElemReadEnc(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemReadEnc::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_ReadEnc && type != eType_Reserved) {
		return false;
	}

	if(id == infoIO_IncPerimRoda.pin.first && type != eType_General) {
		return false;
	}

	if(id == infoIO_IncPulsosVolta.pin.first && type != eType_General) {
		return false;
	}

	if(id == infoIO_IncFatorCorr.pin.first && type != eType_General) {
		return false;
	}

	if(id == infoIO_AbsPerimRoda.pin.first && type != eType_General) {
		return false;
	}

	if(id == infoIO_AbsFatorCorr.pin.first && type != eType_General) {
		return false;
	}

	return true;
}

void LadderElemReadEnc::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;

	// Atualizacao de variaveis internas
	Diagram->updateIO(owner, this, infoIO_IncPerimRoda  , isDiscard);
	Diagram->updateIO(owner, this, infoIO_IncPulsosVolta, isDiscard);
	Diagram->updateIO(owner, this, infoIO_IncFatorCorr  , isDiscard);
	Diagram->updateIO(owner, this, infoIO_AbsPerimRoda  , isDiscard);
	Diagram->updateIO(owner, this, infoIO_AbsFatorCorr  , isDiscard);
}

eType LadderElemReadEnc::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return eType_ReadEnc;
	} else if(infoIO_IncPerimRoda.pin.first == id ||
		infoIO_IncPulsosVolta.pin.first == id ||
		infoIO_IncFatorCorr  .pin.first == id ||
		infoIO_AbsPerimRoda  .pin.first == id ||
		infoIO_AbsFatorCorr  .pin.first == id) {
			return eType_General;
	}

	return eType_Pending;
}

int LadderElemReadEnc::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, eType_ReadEnc, infoIO_Name)) {
				LadderElemReadEncProp *data = (LadderElemReadEncProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemReadEnc::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = eType_ReadEnc;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Destino"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemReadEncProp *data = (LadderElemReadEncProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemReadEnc::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemReadEncProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemResetEnc
LadderElemResetEnc::LadderElemResetEnc(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_RESET_ENC)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_ResetEnc;
	infoIO_Name.access        = eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.idName = infoIO_Name.pin;

	// Caracteristicas do I/O IncPerimRoda
	infoIO_IncPerimRoda.pin           = pair<unsigned long, int>(0, 0);
	infoIO_IncPerimRoda.name          = "IncPerimRoda";
	infoIO_IncPerimRoda.isBit         = false;
	infoIO_IncPerimRoda.type          = eType_General;
	infoIO_IncPerimRoda.access        = eRequestAccessType_ReadWrite;
	infoIO_IncPerimRoda.isUniqueRead  = false;
	infoIO_IncPerimRoda.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_IncPerimRoda);

	// Caracteristicas do I/O IncPulsosVolta
	infoIO_IncPulsosVolta.pin           = pair<unsigned long, int>(0, 0);
	infoIO_IncPulsosVolta.name          = "IncPulsosVolta";
	infoIO_IncPulsosVolta.isBit         = false;
	infoIO_IncPulsosVolta.type          = eType_General;
	infoIO_IncPulsosVolta.access        = eRequestAccessType_ReadWrite;
	infoIO_IncPulsosVolta.isUniqueRead  = false;
	infoIO_IncPulsosVolta.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_IncPulsosVolta);

	// Caracteristicas do I/O IncFatorCorr
	infoIO_IncFatorCorr.pin           = pair<unsigned long, int>(0, 0);
	infoIO_IncFatorCorr.name          = "IncFatorCorr";
	infoIO_IncFatorCorr.isBit         = false;
	infoIO_IncFatorCorr.type          = eType_General;
	infoIO_IncFatorCorr.access        = eRequestAccessType_ReadWrite;
	infoIO_IncFatorCorr.isUniqueRead  = false;
	infoIO_IncFatorCorr.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_IncFatorCorr);

	// Caracteristicas do I/O AbsPerimRoda
	infoIO_AbsPerimRoda.pin           = pair<unsigned long, int>(0, 0);
	infoIO_AbsPerimRoda.name          = "AbsPerimRoda";
	infoIO_AbsPerimRoda.isBit         = false;
	infoIO_AbsPerimRoda.type          = eType_General;
	infoIO_AbsPerimRoda.access        = eRequestAccessType_ReadWrite;
	infoIO_AbsPerimRoda.isUniqueRead  = false;
	infoIO_AbsPerimRoda.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_AbsPerimRoda);

	// Caracteristicas do I/O AbsFatorCorr
	infoIO_AbsFatorCorr.pin           = pair<unsigned long, int>(0, 0);
	infoIO_AbsFatorCorr.name          = "AbsFatorCorr";
	infoIO_AbsFatorCorr.isBit         = false;
	infoIO_AbsFatorCorr.type          = eType_General;
	infoIO_AbsFatorCorr.access        = eRequestAccessType_ReadWrite;
	infoIO_AbsFatorCorr.isUniqueRead  = false;
	infoIO_AbsFatorCorr.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_AbsFatorCorr);
}

pair<string, string> LadderElemResetEnc::DrawTXT(void)
{
	return pair<string, string>(Diagram->getNameIO(prop.idName), _("{WRITE ENC}"));
}

bool LadderElemResetEnc::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_RESET_ENC, name);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemResetEnc::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemResetEnc::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemResetEncProp *newProp = (LadderElemResetEncProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemResetEnc::getProperties(void)
{
	LadderElemResetEncProp *curProp = new LadderElemResetEncProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemResetEnc::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second);
}

bool LadderElemResetEnc::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);

		// As variaveis internas nao tem seus IDs salvos no arquivo
		// Assim precisam ser carregadas a partir da lista de I/O
		infoIO_AbsFatorCorr  .pin.first = Diagram->getIdIO(infoIO_AbsFatorCorr  .name);
		infoIO_AbsPerimRoda  .pin.first = Diagram->getIdIO(infoIO_AbsPerimRoda  .name);
		infoIO_IncFatorCorr  .pin.first = Diagram->getIdIO(infoIO_IncFatorCorr  .name);
		infoIO_IncPerimRoda  .pin.first = Diagram->getIdIO(infoIO_IncPerimRoda  .name);
		infoIO_IncPulsosVolta.pin.first = Diagram->getIdIO(infoIO_IncPulsosVolta.name);
	}

	return ret;
}

LadderElem *LadderElemResetEnc::Clone(LadderDiagram *diagram)
{
	LadderElemResetEnc *clone = new LadderElemResetEnc(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemResetEnc::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_ResetEnc && type != eType_Reserved) {
		return false;
	}

	if(id == infoIO_IncPerimRoda.pin.first && type != eType_General) {
		return false;
	}

	if(id == infoIO_IncPulsosVolta.pin.first && type != eType_General) {
		return false;
	}

	if(id == infoIO_IncFatorCorr.pin.first && type != eType_General) {
		return false;
	}

	if(id == infoIO_AbsPerimRoda.pin.first && type != eType_General) {
		return false;
	}

	if(id == infoIO_AbsFatorCorr.pin.first && type != eType_General) {
		return false;
	}

	return true;
}

void LadderElemResetEnc::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;

	// Atualizacao de variaveis internas
	Diagram->updateIO(owner, this, infoIO_IncPerimRoda  , isDiscard);
	Diagram->updateIO(owner, this, infoIO_IncPulsosVolta, isDiscard);
	Diagram->updateIO(owner, this, infoIO_IncFatorCorr  , isDiscard);
	Diagram->updateIO(owner, this, infoIO_AbsPerimRoda  , isDiscard);
	Diagram->updateIO(owner, this, infoIO_AbsFatorCorr  , isDiscard);
}

eType LadderElemResetEnc::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return eType_ResetEnc;
	} else if(infoIO_IncPerimRoda.pin.first == id ||
		infoIO_IncPulsosVolta.pin.first == id ||
		infoIO_IncFatorCorr  .pin.first == id ||
		infoIO_AbsPerimRoda  .pin.first == id ||
		infoIO_AbsFatorCorr  .pin.first == id) {
			return eType_General;
	}

	return eType_Pending;
}

int LadderElemResetEnc::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, eType_ResetEnc, infoIO_Name)) {
				LadderElemResetEncProp *data = (LadderElemResetEncProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemResetEnc::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = eType_ResetEnc;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Destino"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemResetEncProp *data = (LadderElemResetEncProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemResetEnc::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemResetEncProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemMultisetDA
LadderElemMultisetDA::LadderElemMultisetDA(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_MULTISET_DA)
{
	Diagram                    = diagram;

	// Caracteristicas do I/O Time
	infoIO_Time.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Time.name          = "600";
	infoIO_Time.isBit         = false;
	infoIO_Time.type          = eType_Pending;
	infoIO_Time.access        = eRequestAccessType_Read;
	infoIO_Time.isUniqueRead  = false;
	infoIO_Time.isUniqueWrite = false;

	// Caracteristicas do I/O Desl
	infoIO_Desl.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Desl.name          = "2047";
	infoIO_Desl.isBit         = false;
	infoIO_Desl.type          = eType_Pending;
	infoIO_Desl.access        = eRequestAccessType_Read;
	infoIO_Desl.isUniqueRead  = false;
	infoIO_Desl.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Time);
	Diagram->getIO(this, infoIO_Desl);

	prop.gaint                 = 10;
	prop.gainr                 = 5;
	prop.initval               = DA_RESOLUTION - 1;
	prop.type                  = 0;
	prop.linear                = true;  // true = linear, false = curva
	prop.forward               = true;  // true = avanço, false = recuo
	prop.speedup               = true;  // true = aceleração, false = desaceleração
	prop.StartFromCurrentValue = false; // false = Iniciar ou ir para zero, conforme speedup. true = partir do valor atual até o valor configurado
	prop.ramp_abort_mode       = 0;
	prop.idTime                = infoIO_Time.pin;
	prop.idDesl                = infoIO_Desl.pin;
}

pair<string, string> LadderElemMultisetDA::DrawTXT(void)
{
	return pair<string, string>(
		prop.linear ? _("RAMPA LINEAR") : _("RAMPA CURVA"),
		prop.StartFromCurrentValue ? "{COINCIDIR}": (prop.speedup ? _("{ACELERACAO}") : _("{DESACELERACAO}"))
		);
}

bool LadderElemMultisetDA::internalGenerateIntCode(IntCode &ic)
{
	LadderSettingsDAC settings = Diagram->getSettingsDAC();

	string stime = Diagram->getNameIO(prop.idTime);
	string sdesl = Diagram->getNameIO(prop.idDesl);
	const char *time = stime.c_str();
	const char *desl = sdesl.c_str();

	char cgaint[10];
	char cgainr[10];
	char ctype [10];
	char str_initval[MAX_NAME_LEN];
	string oneShot;
	oneShot = ic.GenSymOneShot();

	_itoa(prop.gaint, cgaint, 10);
	_itoa(prop.gainr, cgainr, 10);
	_itoa(prop.type , ctype , 10);

	// Apenas carrega o valor, nao precisa converter pois o firmware ja faz isso
	strcpy(str_initval, desl);

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), ic.getStateInOut());
			ic.Op(INT_MULTISET_DA, time, str_initval, cgaint, cgainr, ctype, NULL, NULL, prop.linear, prop.StartFromCurrentValue ? 2 : prop.speedup);
		ic.Op(INT_END_IF);
	ic.Op(INT_ELSE_IF); ic.Op(INT_IF_BIT_SET, oneShot.c_str());
		ic.Op(INT_MULTISET_DA, "", prop.ramp_abort_mode == RAMP_ABORT_DEFAULT ? settings.ramp_abort_mode : prop.ramp_abort_mode);
		ic.Op(INT_CLEAR_BIT, oneShot.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemMultisetDA::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemMultisetDA::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemMultisetDAProp *newProp = (LadderElemMultisetDAProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemMultisetDA::getProperties(void)
{
	LadderElemMultisetDAProp *curProp = new LadderElemMultisetDAProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemMultisetDA::internalSave(FILE *f)
{
	return
		fwrite_int  (f, prop.gainr) &&
		fwrite_int  (f, prop.gaint) &&
		fwrite_int  (f, prop.initval) &&
		fwrite_int  (f, prop.type) &&
		fwrite_bool (f, prop.linear) &&
		fwrite_bool (f, prop.forward) &&
		fwrite_bool (f, prop.speedup) &&
		fwrite_bool (f, prop.StartFromCurrentValue) &&
		fwrite_int  (f, prop.ramp_abort_mode) &&
		fwrite_ulong(f, prop.idDesl.first) &&
		fwrite_int  (f, prop.idDesl.second) &&
		fwrite_ulong(f, prop.idTime.first) &&
		fwrite_int  (f, prop.idTime.second);
}

bool LadderElemMultisetDA::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_int  (f, &prop.gainr) &&
		fread_int  (f, &prop.gaint) &&
		fread_int  (f, &prop.initval) &&
		fread_int  (f, &prop.type) &&
		fread_bool (f, &prop.linear) &&
		fread_bool (f, &prop.forward) &&
		fread_bool (f, &prop.speedup) &&
		fread_bool (f, &prop.StartFromCurrentValue) &&
		fread_int  (f, &prop.ramp_abort_mode) &&
		fread_ulong(f, &prop.idDesl.first) &&
		fread_int  (f, &prop.idDesl.second) &&
		fread_ulong(f, &prop.idTime.first) &&
		fread_int  (f, &prop.idTime.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idDesl.first);
		Diagram->updateTypeIO(prop.idTime.first);
	}

	return ret;
}

LadderElem *LadderElemMultisetDA::Clone(LadderDiagram *diagram)
{
	LadderElemMultisetDA *clone = new LadderElemMultisetDA(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemMultisetDA::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idTime.first || id == prop.idDesl.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemMultisetDA::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Time.pin = prop.idTime;
	Diagram->updateIO(owner, this, infoIO_Time, isDiscard);
	prop.idTime = infoIO_Time.pin;

	infoIO_Desl.pin = prop.idDesl;
	Diagram->updateIO(owner, this, infoIO_Desl, isDiscard);
	prop.idDesl = infoIO_Desl.pin;
}

eType LadderElemMultisetDA::getAllowedTypeIO(unsigned long id)
{
	if(prop.idDesl.first == id || prop.idTime.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemMultisetDA::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idDesl.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idDesl;
			if(Diagram->getIO(this, pin, sNewText, eType_General, infoIO_Desl)) {
				LadderElemMultisetDAProp *data = (LadderElemMultisetDAProp *)getProperties();

				data->idDesl  = pin;

				setProperties(Diagram->getContext(), data);

				n++;
			}
		} else {
			return 1;
		}
	}

	if(prop.idTime.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idTime;
			if(Diagram->getIO(this, pin, sNewText, eType_General, infoIO_Time)) {
				LadderElemMultisetDAProp *data = (LadderElemMultisetDAProp *)getProperties();

				data->idTime  = pin;

				setProperties(Diagram->getContext(), data);

				n++;
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemMultisetDA::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin;

	const char *field;
	if(index == 0) {
		pin   = prop.idTime;
		field = _("Tempo");
	} else {
		pin   = prop.idDesl;
		field = _("Valor");
	}

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field, VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, (index == 0) ? infoIO_Time : infoIO_Desl, true)) {
			LadderElemMultisetDAProp *data = (LadderElemMultisetDAProp *)getProperties();

			if(index == 0) {
				data->idTime = pin;
			} else {
				data->idDesl = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemMultisetDA::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemMultisetDAProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemUSS
LadderElemUSS::LadderElemUSS(LadderDiagram *diagram, int which) : LadderElem(false, false, false, true, which)
{
	Diagram            = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = (getWhich() == ELEM_READ_USS) ? eType_ReadUSS : eType_WriteUSS;
	infoIO_Name.access        = (getWhich() == ELEM_READ_USS) ? eRequestAccessType_Write : eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.id            = 0;
	prop.parameter     = 0;
	prop.parameter_set = 0;
	prop.index         = 0;
	prop.idName        = infoIO_Name.pin;
}

pair<string, string> LadderElemUSS::DrawTXT(void)
{
	return pair<string, string>(Diagram->getNameIO(prop.idName), (getWhich() == ELEM_READ_USS) ? _("{READ USS}") : _("{WRITE USS}"));
}

bool LadderElemUSS::internalGenerateIntCode(IntCode &ic)
{
	const char *stateInOut = ic.getStateInOut();
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	char addr     [10];
	char param    [10];
	char param_set[10];
	char index    [10];

	sprintf(addr     , "%d", prop.id);
	sprintf(param    , "%d", prop.parameter);
	sprintf(param_set, "%d", prop.parameter_set);
	sprintf(index    , "%d", prop.index);

	// We want to respond to rising edges, so yes we need a one shot.
	string oneShot = ic.GenSymOneShot();
	string byPass  = ic.GenSymOneShot();

	// OneShot 
	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_IF_BIT_SET, "$SerialReady");
				ic.Op(getWhich() == ELEM_READ_USS ? INT_READ_USS : INT_WRITE_USS, name, addr, param, param_set, (SWORD)atoi(index), 0);
				ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), stateInOut);
			ic.Op(INT_END_IF);
			ic.Op(INT_CLEAR_BIT, stateInOut);
			ic.Op(INT_COPY_BIT_TO_BIT, byPass.c_str(), stateInOut);
		ic.Op(INT_END_IF);
		ic.Op(INT_IF_BIT_CLEAR, byPass.c_str());
			ic.Op(INT_IF_BIT_SET, "$SerialReady");
				ic.Op(INT_SET_BIT, byPass.c_str());
			ic.Op(INT_ELSE);
				ic.Op(INT_CLEAR_BIT, stateInOut);
			ic.Op(INT_END_IF);
		ic.Op(INT_END_IF);
	ic.Op(INT_ELSE);
		ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), stateInOut);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemUSS::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemUSS::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemUSSProp *newProp = (LadderElemUSSProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemUSS::getProperties(void)
{
	LadderElemUSSProp *curProp = new LadderElemUSSProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemUSS::internalSave(FILE *f)
{
	return
		fwrite_int  (f, prop.id) &&
		fwrite_int  (f, prop.index) &&
		fwrite_int  (f, prop.parameter) &&
		fwrite_int  (f, prop.parameter_set) &&
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second);
}

bool LadderElemUSS::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_int  (f, &prop.id) &&
		fread_int  (f, &prop.index) &&
		fread_int  (f, &prop.parameter) &&
		fread_int  (f, &prop.parameter_set) &&
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemUSS::Clone(LadderDiagram *diagram)
{
	LadderElemUSS *clone = new LadderElemUSS(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemUSS::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_ReadUSS && type != eType_WriteUSS && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemUSS::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemUSS::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return (getWhich() == ELEM_READ_USS) ? eType_ReadUSS : eType_WriteUSS;
	}

	return eType_Pending;
}

int LadderElemUSS::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			eType type = (getWhich() == ELEM_READ_USS) ? eType_ReadUSS : eType_WriteUSS;
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, type, infoIO_Name)) {
				LadderElemUSSProp *data = (LadderElemUSSProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemUSS::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = (getWhich() == ELEM_READ_USS) ? eType_ReadUSS : eType_WriteUSS;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Nome" ), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemReadAdcProp *data = (LadderElemReadAdcProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemUSS::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemUSSProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemModBUS
LadderElemModBUS::LadderElemModBUS(LadderDiagram *diagram, int which) : LadderElem(false, false, false, true, which)
{
	Diagram           = diagram;

	int NodeID = Diagram->mbGetNodeIDByIndex(0);
	if(NodeID < 0) {
		NodeID = Diagram->mbCreateNode("Default");
	}

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("novo");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = (getWhich() == ELEM_READ_MODBUS) ? eType_ReadModbus : eType_WriteModbus;
	infoIO_Name.access        = (getWhich() == ELEM_READ_MODBUS) ? eRequestAccessType_Write : eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.address      = 0;
	prop.retransmitir = true;
	prop.int32        = false;
	prop.elem         = NodeID;
	prop.idName       = infoIO_Name.pin;
}

pair<string, string> LadderElemModBUS::DrawTXT(void)
{
	const char *bot;
	char buf[100];
	LadderMbNode node = Diagram->mbGetNodeByNodeID(prop.elem);

	sprintf(buf, "%s:%d", node.name.c_str(), prop.address);

	if(getWhich() == ELEM_READ_MODBUS) {
		bot = (node.iface == eMbTypeNode_RS485) ? _("{READ MB 485}")  : _("{READ MB ETH}");
	} else {
		bot = (node.iface == eMbTypeNode_RS485) ? _("{WRITE MB 485}") : _("{WRITE MB ETH}");
	}

	return pair<string, string>(buf, bot);
}

bool LadderElemModBUS::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	char id[10];
	char addr[10];
	char *MbReady, *MbTimeout;
	const char *stateInOut = ic.getStateInOut();
	int intcode;
	LadderMbNode node;

	node    = Diagram->mbGetNodeByNodeID(prop.elem);
	intcode = getWhich() == ELEM_READ_MODBUS ? INT_READ_MODBUS : INT_WRITE_MODBUS;
	sprintf(id  , "%d", node.id);
	sprintf(addr, "%d", prop.address);

	if(node.iface == eMbTypeNode_RS485) {
		MbReady   = "$SerialReady";
		MbTimeout = "$SerialTimeout";
	} else {
		MbReady   = "$TcpReady";
		MbTimeout = "$TcpTimeout";
	}

	// We want to respond to rising edges, so yes we need a one shot.
	string MessageSent   = ic.GenSymOneShot();
	string ReplyReceived = ic.GenSymOneShot();

	// OneShot 
	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, MessageSent.c_str());
			ic.Op(INT_IF_BIT_SET, MbReady);
				ic.Op(intcode, name, id, addr, node.ip, (unsigned char)prop.int32);
				ic.Op(INT_SET_BIT, MessageSent.c_str());
			ic.Op(INT_END_IF);
			ic.Op(INT_CLEAR_BIT, stateInOut);
			ic.Op(INT_CLEAR_BIT, ReplyReceived.c_str());
		ic.Op(INT_END_IF);
		ic.Op(INT_IF_BIT_CLEAR, ReplyReceived.c_str());
			ic.Op(INT_CLEAR_BIT, stateInOut);
			ic.Op(INT_IF_BIT_SET, MbReady);
				ic.Op(INT_SET_BIT, ReplyReceived.c_str());
		if(prop.retransmitir) { // Retransmitir?
			ic.Op(INT_ELSE_IF); ic.Op(INT_IF_BIT_SET, MbTimeout);
				ic.Op(INT_CLEAR_BIT, MbTimeout);
				ic.Op(intcode, name, id, addr, node.ip, (unsigned char)prop.int32);
		}
			ic.Op(INT_END_IF);
		ic.Op(INT_END_IF);
	ic.Op(INT_ELSE);
		ic.Op(INT_CLEAR_BIT, MessageSent.c_str());
		ic.Op(INT_CLEAR_BIT, ReplyReceived.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemModBUS::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void LadderElemModBUS::doPostInsert(void)
{
	Diagram->mbAddRef(prop.elem);
}

void LadderElemModBUS::doPostRemove(void)
{
	Diagram->mbDelRef(prop.elem);
}

bool LadderElemModBUS::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemModBUSProp *newProp = (LadderElemModBUSProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemModBUS::getProperties(void)
{
	LadderElemModBUSProp *curProp = new LadderElemModBUSProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemModBUS::internalSave(FILE *f)
{
	return
		fwrite_int  (f, prop.address) &&
		fwrite_int  (f, prop.elem) &&
		fwrite_bool (f, prop.int32) &&
		fwrite_bool (f, prop.retransmitir) &&
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second);
}

bool LadderElemModBUS::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_int  (f, &prop.address) &&
		fread_int  (f, &prop.elem) &&
		fread_bool (f, &prop.int32) &&
		fread_bool (f, &prop.retransmitir) &&
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemModBUS::Clone(LadderDiagram *diagram)
{
	LadderElemModBUS *clone = new LadderElemModBUS(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemModBUS::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_ReadModbus && type != eType_WriteModbus && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemModBUS::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemModBUS::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return (getWhich() == ELEM_READ_MODBUS) ? eType_ReadModbus : eType_WriteModbus;
	}

	return eType_Pending;
}

int LadderElemModBUS::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			eType type = (getWhich() == ELEM_READ_MODBUS) ? eType_ReadModbus : eType_WriteModbus;
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, type, infoIO_Name)) {
				LadderElemModBUSProp *data = (LadderElemModBUSProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemModBUS::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = (getWhich() == ELEM_WRITE_MODBUS) ? eType_WriteModbus : eType_ReadModbus;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Variável"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemModBUSProp *data = (LadderElemModBUSProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemModBUS::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemModBUSProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemSetPWM
LadderElemSetPWM::LadderElemSetPWM(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_SET_PWM)
{
	Diagram         = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("duty_cycle");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = eType_PWM;
	infoIO_Name.access        = eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.targetFreq = 1000;
	prop.idName     = infoIO_Name.pin;
}

pair<string, string> LadderElemSetPWM::DrawTXT(void)
{
	char l[50];
	if(prop.targetFreq >= 100000) {
		sprintf(l, _("{PWM %d kHz}"), (prop.targetFreq+500)/1000);
	} else if(prop.targetFreq >= 10000) {
		sprintf(l, _("{PWM %.1f kHz}"), prop.targetFreq/1000.0);
	} else if(prop.targetFreq >= 1000) {
		sprintf(l, _("{PWM %.2f kHz}"), prop.targetFreq/1000.0);
	} else {
		sprintf(l, _("{PWM %d Hz}"), prop.targetFreq);
	}

	return pair<string, string>(Diagram->getNameIO(prop.idName), l);
}

bool LadderElemSetPWM::internalGenerateIntCode(IntCode &ic)
{
	char line[80];
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	string oneShot = ic.GenSymOneShot();

	// ugh; need a >16 bit literal though, could be >64 kHz
	sprintf(line, "%d", prop.targetFreq);

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_SET_BIT, oneShot.c_str());
		ic.Op(INT_SET_PWM, name, line);
	ic.Op(INT_ELSE_IF); ic.Op(INT_IF_BIT_SET, oneShot.c_str());
		ic.Op(INT_CLEAR_BIT, oneShot.c_str());
		ic.Op(INT_SET_PWM, "0", "0"); // Disable PWM Output
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemSetPWM::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemSetPWM::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemSetPWMProp *newProp = (LadderElemSetPWMProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemSetPWM::getProperties(void)
{
	LadderElemSetPWMProp *curProp = new LadderElemSetPWMProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemSetPWM::internalSave(FILE *f)
{
	return
		fwrite_int  (f, prop.targetFreq) &&
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second);
}

bool LadderElemSetPWM::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_int  (f, &prop.targetFreq) &&
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemSetPWM::Clone(LadderDiagram *diagram)
{
	LadderElemSetPWM *clone = new LadderElemSetPWM(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemSetPWM::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_PWM && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemSetPWM::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemSetPWM::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return eType_PWM;
	}

	return eType_Pending;
}

int LadderElemSetPWM::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, eType_PWM, infoIO_Name)) {
				LadderElemSetPWMProp *data = (LadderElemSetPWMProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemSetPWM::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idName;

	type = eType_PWM;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Variável"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
			LadderElemSetPWMProp *data = (LadderElemSetPWMProp *)getProperties();

			data->idName  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemSetPWM::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemSetPWMProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemUART
LadderElemUART::LadderElemUART(LadderDiagram *diagram, int which) : LadderElem(false, false, false, true, which)
{
	Diagram   = diagram;

	// Caracteristicas do I/O Name
	infoIO_Name.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Name.name          = _("char");
	infoIO_Name.isBit         = false;
	infoIO_Name.type          = (getWhich() == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART;
	infoIO_Name.access        = (getWhich() == ELEM_UART_RECV) ? eRequestAccessType_Write : eRequestAccessType_Read;
	infoIO_Name.isUniqueRead  = false;
	infoIO_Name.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Name);

	prop.idName     = infoIO_Name.pin;
}

pair<string, string> LadderElemUART::DrawTXT(void)
{
	return pair<string, string>(Diagram->getNameIO(prop.idName), (getWhich() == ELEM_UART_RECV) ? _("{UART RECV}") : _("{UART SEND}"));
}

bool LadderElemUART::internalGenerateIntCode(IntCode &ic)
{
	string oneShot = ic.GenSymOneShot();
	const char *stateInOut = ic.getStateInOut();
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(getWhich() == ELEM_UART_SEND ? INT_UART_SEND : INT_UART_RECV, name, stateInOut);
			ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), stateInOut);
		ic.Op(INT_END_IF);
	ic.Op(INT_ELSE);
		ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), stateInOut);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemUART::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemUART::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemUARTProp *newProp = (LadderElemUARTProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemUART::getProperties(void)
{
	LadderElemUARTProp *curProp = new LadderElemUARTProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemUART::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second);
}

bool LadderElemUART::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idName.first);
	}

	return ret;
}

LadderElem *LadderElemUART::Clone(LadderDiagram *diagram)
{
	LadderElemUART *clone = new LadderElemUART(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemUART::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != ((getWhich() == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemUART::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Name.pin = prop.idName;
	Diagram->updateIO(owner, this, infoIO_Name, isDiscard);
	prop.idName = infoIO_Name.pin;
}

eType LadderElemUART::getAllowedTypeIO(unsigned long id)
{
	if(prop.idName.first == id) {
		return (getWhich() == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART;
	}

	return eType_Pending;
}

int LadderElemUART::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idName.first == idSearch) {
		if(isReplace) {
			eType type = (getWhich() == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART;
			pair<unsigned long, int> pin = prop.idName;
			if(Diagram->getIO(this, pin, sNewText, type, infoIO_Name)) {
				LadderElemUARTProp *data = (LadderElemUARTProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemUART::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	int which = getWhich();
	pair<unsigned long, int> pin = prop.idName;

	type = (which == ELEM_UART_SEND) ? eType_TxUART : eType_RxUART;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, (which == ELEM_UART_RECV) ? _("Destino") : _("Origem"),
		(which == ELEM_UART_RECV) ? VALIDATE_IS_VAR : VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
			if(Diagram->getIO(this, pin, name, type, infoIO_Name)) {
				LadderElemUARTProp *data = (LadderElemUARTProp *)getProperties();

				data->idName  = pin;

				setProperties(Diagram->getContext(), data);

				return true;
			}
	}

	return false;
}

bool LadderElemUART::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemUARTProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemMasterRelay
LadderElemMasterRelay::LadderElemMasterRelay(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_MASTER_RELAY)
{
	Diagram = diagram;
}

pair<string, string> LadderElemMasterRelay::DrawTXT(void)
{
	return pair<string, string>("", _("{MASTER RLY}"));
}

bool LadderElemMasterRelay::internalGenerateIntCode(IntCode &ic)
{
	// Tricky: must set the master control relay if we reach this
	// instruction while the master control relay is cleared, because
	// otherwise there is no good way for it to ever become set
	// again.
	ic.Op(INT_IF_BIT_CLEAR, "$mcr");
		ic.Op(INT_SET_BIT, "$mcr");
	ic.Op(INT_ELSE);
		ic.Op(INT_COPY_BIT_TO_BIT, "$mcr", ic.getStateInOut());
	ic.Op(INT_END_IF);

	ic.SimState(&isMasterRelayActive, "$mcr");

	return true;
}

bool LadderElemMasterRelay::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

LadderElem *LadderElemMasterRelay::Clone(LadderDiagram *diagram)
{
	LadderElemMasterRelay *clone = new LadderElemMasterRelay(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados

	return clone;
}

bool LadderElemMasterRelay::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Sempre retorna sucesso...
}

// Classe LadderElemShiftRegister
LadderElemShiftRegister::LadderElemShiftRegister(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_SHIFT_REGISTER)
{
	Diagram     = diagram;

	prop.stages  = 7;
	prop.nameReg = _("reg");

	// Caracteristicas dos I/Os
	InfoIO_Regs.pin           = pair<unsigned long, int>(0, 0);
	InfoIO_Regs.name          = prop.nameReg;
	InfoIO_Regs.isBit         = false;
	InfoIO_Regs.type          = eType_General;
	InfoIO_Regs.access        = eRequestAccessType_ReadWrite;
	InfoIO_Regs.isUniqueRead  = false;
	InfoIO_Regs.isUniqueWrite = false;

	createRegs();
}

void LadderElemShiftRegister::createRegs(void)
{
	int i;
	eType type;
	vector< pair<unsigned long, int> >::iterator it;

	// Primeiro descarta os I/Os anteriores, solicitando I/O para o valor zero.
	for(it = prop.vectorIdRegs.begin(); it != prop.vectorIdRegs.end(); it++) {
		Diagram->getIO(this, *it, "0", eType_General, InfoIO_Regs);
	}

	// Limpa o vetor anterior
	prop.vectorIdRegs.clear();

	// Registra os novos valores
	char cname[MAX_NAME_LEN + 10];
	pair<unsigned long, int> pin;
	for(i = 0; i < prop.stages; i++) {
		pin = pair<unsigned long, int>(0, 0);
		strcpy(cname, prop.nameReg.c_str());
		if(i < 10 && prop.stages > 10) {
			strcat(cname, "0");
		}
		sprintf(cname, "%s%d", cname, i);

		type = eType_General;
		if(i == 0) {
			mapDetails detailsIO = Diagram->getDetailsIO(cname);
			if(Diagram->IsGenericTypeIO(detailsIO.type)) {
				type = detailsIO.type;
			}
		}

		if(!Diagram->getIO(this, pin, cname, type, InfoIO_Regs)) {
			// Ocorreu algum erro ao registrar os I/Os. Solicita novamente com valor padrao
			prop.nameReg = _("reg");
			createRegs();
			break;
		}
		prop.vectorIdRegs.push_back(pin);
	}
}

pair<string, string> LadderElemShiftRegister::DrawTXT(void)
{
	char bot[MAX_NAME_LEN+20];
	char name[MAX_NAME_LEN];

	strcpy(name, prop.nameReg.c_str());

	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, "%s:%d", name, prop.stages-1);
	bot[strlen(bot)] = ' ';
	bot[19] = '}';
	bot[20] = '\0';

	return pair<string, string>(_("{\x01SHIFT REG\x02   }"), bot);
}

bool LadderElemShiftRegister::internalGenerateIntCode(IntCode &ic)
{
	string storeName = ic.GenSymOneShot();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_IF_BIT_CLEAR, storeName.c_str());
			int i;
			for(i = (prop.stages-2); i >= 0; i--) {
				char dest[MAX_NAME_LEN+10], src[MAX_NAME_LEN+10];
				strcpy(src , Diagram->getNameIO(prop.vectorIdRegs[i    ].first).c_str());
				strcpy(dest, Diagram->getNameIO(prop.vectorIdRegs[i + 1].first).c_str());
				ic.Op(INT_SET_VARIABLE_TO_VARIABLE, dest, src);
			}
		ic.Op(INT_END_IF);
	ic.Op(INT_END_IF);
	ic.Op(INT_COPY_BIT_TO_BIT, storeName.c_str(), ic.getStateInOut());

	return true;
}

bool LadderElemShiftRegister::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemShiftRegister::internalSetProperties(void *data, bool isUndoRedo, bool isCloning)
{
	LadderElemShiftRegisterProp *newProp = (LadderElemShiftRegisterProp *)data;

	prop = *newProp;
	delete newProp;

	if(isUndoRedo == false && !isCloning) {
		createRegs();
	}

	return true;
}

void *LadderElemShiftRegister::getProperties(void)
{
	LadderElemShiftRegisterProp *curProp = new LadderElemShiftRegisterProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemShiftRegister::internalSave(FILE *f)
{
	vector< pair<unsigned long, int> >::iterator it = prop.vectorIdRegs.begin();

	bool ret =
		fwrite_string(f, prop.nameReg) &&
		fwrite_int   (f, prop.stages);

	while(ret == true && it != prop.vectorIdRegs.end()) {
		ret = fwrite_ulong(f, it->first) && fwrite_int(f, it->second);
		it++;
	}

	return ret;
}

bool LadderElemShiftRegister::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_string(f, &prop.nameReg) &&
		fread_int   (f, &prop.stages);

	if(ret == true) {
		int i;
		pair<unsigned long, int> pin;

		prop.vectorIdRegs.clear();

		for(i = 0; i < prop.stages; i++) {
			ret = fread_ulong(f, &pin.first) && fread_int(f, &pin.second);
			if(ret == true) {
				Diagram->updateTypeIO(pin.first);
				prop.vectorIdRegs.push_back(pin);
			} else {
				break;
			}
		}
	}

	return ret;
}

LadderElem *LadderElemShiftRegister::Clone(LadderDiagram *diagram)
{
	LadderElemShiftRegister *clone = new LadderElemShiftRegister(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties(), false, true);

	return clone;
}

bool LadderElemShiftRegister::acceptIO(unsigned long id, eType type)
{
	vector< pair<unsigned long, int> >::iterator it;

	for(it = prop.vectorIdRegs.begin(); it != prop.vectorIdRegs.end(); it++) {
		if(id == it->first && type != eType_General && type != eType_Reserved) {
			// Se for o primeiro elemento e o tipo for generico, aceita. Caso contrario, nao aceita...
			return (it == prop.vectorIdRegs.begin() && Diagram->IsGenericTypeIO(type));
		}
	}

	return true;
}

void LadderElemShiftRegister::updateIO(LadderDiagram *owner, bool isDiscard)
{
	int i;
	char cname[MAX_NAME_LEN + 10];
	string previous_name = InfoIO_Regs.name;

	for(i = 0; i < prop.stages; i++) {
		sprintf(cname, "%s%d", InfoIO_Regs.name.c_str(), i);

		InfoIO_Regs.name = cname;
		InfoIO_Regs.pin  = prop.vectorIdRegs[i];

		Diagram->updateIO(owner, this, InfoIO_Regs, isDiscard);
		prop.vectorIdRegs[i] = InfoIO_Regs.pin;
	}

	InfoIO_Regs.name = previous_name;
}

eType LadderElemShiftRegister::getAllowedTypeIO(unsigned long id)
{
	vector< pair<unsigned long, int> >::iterator it;

	for(it = prop.vectorIdRegs.begin(); it != prop.vectorIdRegs.end(); it++) {
		if(id == it->first) {
			return eType_General;
		}
	}

	return eType_Pending;
}

int LadderElemShiftRegister::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(!isReplace) {
		unsigned int i;
		for(i = 0; i < prop.vectorIdRegs.size(); i++) {
			if(prop.vectorIdRegs[i].first == idSearch) {
				return 1;
			}
		}
	}

	return 0;
}

bool LadderElemShiftRegister::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemShiftRegisterProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemLUT
LadderElemLUT::LadderElemLUT(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_LOOK_UP_TABLE)
{
	Diagram           = diagram;

	// Caracteristicas do I/O Dest
	infoIO_Dest.pin            = pair<unsigned long, int>(0, 0);
	infoIO_Dest.name           = _("dest");
	infoIO_Dest.isBit          = false;
	infoIO_Dest.type           = eType_General;
	infoIO_Dest.access         = eRequestAccessType_Write;
	infoIO_Dest.isUniqueRead   = false;
	infoIO_Dest.isUniqueWrite  = false;

	// Caracteristicas do I/O Index
	infoIO_Index.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Index.name          = _("indice");
	infoIO_Index.isBit         = false;
	infoIO_Index.type          = eType_General;
	infoIO_Index.access        = eRequestAccessType_Read;
	infoIO_Index.isUniqueRead  = false;
	infoIO_Index.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Dest );
	Diagram->getIO(this, infoIO_Index);

	prop.count        = 0;
	prop.editAsString = false;
	prop.idDest       = infoIO_Dest .pin;
	prop.idIndex      = infoIO_Index.pin;

	prop.vals.fill(0);
}

pair<string, string> LadderElemLUT::DrawTXT(void)
{
	char top[MAX_NAME_LEN+20], bot[MAX_NAME_LEN+20];

	string sdest  = Diagram->getNameIO(prop.idDest);
	string sindex = Diagram->getNameIO(prop.idIndex);
	const char *dest  = sdest .c_str();
	const char *index = sindex.c_str();

	memset(top, ' ', sizeof(top));
	top[0] = '{';
	sprintf(top+2, "%s :=", dest);
	top[strlen(top)] = ' ';
	top[19] = '}';
	top[20] = '\0';

	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, " %s[%s]", "LUT", index);
	bot[strlen(bot)] = ' ';
	bot[19] = '}';
	bot[20] = '\0';

	return pair<string, string>(top, bot);
}

bool LadderElemLUT::internalGenerateIntCode(IntCode &ic)
{
	// God this is stupid; but it will have to do, at least until I
	// add new int code instructions for this.
	int i;

	string sdest  = Diagram->getNameIO(prop.idDest);
	string sindex = Diagram->getNameIO(prop.idIndex);
	const char *dest  = sdest .c_str();
	const char *index = sindex.c_str();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	for(i = 0; i < prop.count; i++) {
		ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", i);
		ic.Op(INT_IF_VARIABLE_EQUALS_VARIABLE, index, "$scratch");
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, dest, prop.vals[i]);
		ic.Op(INT_END_IF);
	}

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemLUT::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemLUT::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemLUTProp *newProp = (LadderElemLUTProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemLUT::getProperties(void)
{
	LadderElemLUTProp *curProp = new LadderElemLUTProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemLUT::internalSave(FILE *f)
{
	bool ret =
		fwrite_int  (f, prop.count) &&
		fwrite_bool (f, prop.editAsString) &&
		fwrite_ulong(f, prop.idDest.first) &&
		fwrite_int  (f, prop.idDest.second) &&
		fwrite_ulong(f, prop.idIndex.first) &&
		fwrite_int  (f, prop.idIndex.second);

	if(ret == true) {
		int i;
		for(i = 0; i < prop.count; i++) {
			ret = fwrite_long(f, prop.vals[i]);
			if(ret != true) {
				break;
			}
		}
	}

	return ret;
}

bool LadderElemLUT::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_int  (f, &prop.count) &&
		fread_bool (f, &prop.editAsString) &&
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idIndex.first) &&
		fread_int  (f, &prop.idIndex.second);

	if(ret == true) {
		int i;
		for(i = 0; i < prop.count; i++) {
			ret = fread_long(f, &prop.vals[i]);
			if(ret != true) {
				break;
			}
		}
	}

	if(ret == true) {
		Diagram->updateTypeIO(prop.idDest .first);
		Diagram->updateTypeIO(prop.idIndex.first);
	}

	return ret;
}

LadderElem *LadderElemLUT::Clone(LadderDiagram *diagram)
{
	LadderElemLUT *clone = new LadderElemLUT(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemLUT::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idDest.first || id == prop.idIndex.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemLUT::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Dest.pin = prop.idDest;
	Diagram->updateIO(owner, this, infoIO_Dest, isDiscard);
	prop.idDest = infoIO_Dest.pin;

	infoIO_Index.pin = prop.idIndex;
	Diagram->updateIO(owner, this, infoIO_Index, isDiscard);
	prop.idIndex = infoIO_Index.pin;
}

eType LadderElemLUT::getAllowedTypeIO(unsigned long id)
{
	if(prop.idDest.first == id || prop.idIndex.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemLUT::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idDest.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idDest;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Dest)) {
					LadderElemLUTProp *data = (LadderElemLUTProp *)getProperties();

					data->idDest  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idIndex.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idIndex;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Index)) {
					LadderElemLUTProp *data = (LadderElemLUTProp *)getProperties();

					data->idIndex  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemLUT::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin;

	const char *field;
	if(index == 0) {
		pin   = prop.idIndex;
		field = _("Índice");
	} else {
		pin   = prop.idDest;
		field = _("Destino");
	}

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field, VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, (index == 0) ? infoIO_Index : infoIO_Dest, true)) {
			LadderElemLUTProp *data = (LadderElemLUTProp *)getProperties();

			if(index == 0) {
				data->idIndex = pin;
			} else {
				data->idDest  = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemLUT::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemLUTProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemPiecewise
LadderElemPiecewise::LadderElemPiecewise(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_PIECEWISE_LINEAR)
{
	Diagram      = diagram;

	// Caracteristicas do I/O Dest
	infoIO_Dest.pin            = pair<unsigned long, int>(0, 0);
	infoIO_Dest.name           = _("yvar");
	infoIO_Dest.isBit          = false;
	infoIO_Dest.type           = eType_General;
	infoIO_Dest.access         = eRequestAccessType_Write;
	infoIO_Dest.isUniqueRead   = false;
	infoIO_Dest.isUniqueWrite  = false;

	// Caracteristicas do I/O Index
	infoIO_Index.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Index.name          = _("xvar");
	infoIO_Index.isBit         = false;
	infoIO_Index.type          = eType_General;
	infoIO_Index.access        = eRequestAccessType_Read;
	infoIO_Index.isUniqueRead  = false;
	infoIO_Index.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Dest );
	Diagram->getIO(this, infoIO_Index);

	prop.count   = 0;
	prop.idDest  = infoIO_Dest .pin;
	prop.idIndex = infoIO_Index.pin;

	prop.vals.fill(0);
}

pair<string, string> LadderElemPiecewise::DrawTXT(void)
{
	char top[MAX_NAME_LEN+20], bot[MAX_NAME_LEN+20];

	string sdest  = Diagram->getNameIO(prop.idDest);
	string sindex = Diagram->getNameIO(prop.idIndex);
	const char *dest  = sdest .c_str();
	const char *index = sindex.c_str();

	memset(top, ' ', sizeof(top));
	top[0] = '{';
	sprintf(top+2, "%s :=", dest);
	top[strlen(top)] = ' ';
	top[19] = '}';
	top[20] = '\0';

	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, " %s[%s]", "PWL", index);
	bot[strlen(bot)] = ' ';
	bot[19] = '}';
	bot[20] = '\0';

	return pair<string, string>(top, bot);
}

bool LadderElemPiecewise::internalGenerateIntCode(IntCode &ic)
{
	string sdest  = Diagram->getNameIO(prop.idDest);
	string sindex = Diagram->getNameIO(prop.idIndex);
	const char *dest  = sdest .c_str();
	const char *index = sindex.c_str();

	// This one is not so obvious; we have to decide how best to
	// perform the linear interpolation, using our 16-bit fixed
	// point math.
	if(prop.count == 0) {
		Error(_("Tabela de Linearização por Segmentos sem elementos!"));
		CompileError();
	}

	int i;
	int xThis = prop.vals[0];
	for(i = 1; i < prop.count; i++) {
		if(prop.vals[i*2] <= xThis) {
			Error(_("x values in piecewise linear table must be "
				"strictly increasing."));
			CompileError();
		}
		xThis = prop.vals[i*2];
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	for(i = prop.count - 1; i >= 1; i--) {
		int thisDx = prop.vals[i*2    ] - prop.vals[(i-1)*2    ];
		int thisDy = prop.vals[i*2 + 1] - prop.vals[(i-1)*2 + 1];
		// The output point is given by
		//    yout = y[i-1] + (xin - x[i-1])*dy/dx
		// and this is the best form in which to keep it, numerically
		// speaking, because you can always fix numerical problems
		// by moving the PWL points closer together.
               
		// Check for numerical problems, and fail if we have them.
		if((thisDx*thisDy) >= (int)2147483647 || 
			(thisDx*thisDy) <= (int)(-2147483647)) {
			Error(_("Numerical problem with piecewise linear lookup "
				"table. Either make the table entries smaller, "
				"or space the points together more closely.\r\n\r\n"
				"See the help file for details."));
			CompileError();
		}

		ic.Op(INT_IF_VARIABLE_LES_LITERAL, index, prop.vals[i*2]+1);

			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch" , prop.vals[(i-1)*2]);
			ic.Op(INT_SET_VARIABLE_SUBTRACT  , "$scratch" , index     , "$scratch" , 0, 0);
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch2", thisDx);
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch3", thisDy);
			ic.Op(INT_SET_VARIABLE_MULTIPLY  , dest       , "$scratch", "$scratch3", 0, 0);
			ic.Op(INT_SET_VARIABLE_DIVIDE    , dest       , dest      , "$scratch2", 0, 0);

			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch" , prop.vals[(i-1)*2 + 1]);
			ic.Op(INT_SET_VARIABLE_ADD       , dest       , dest      , "$scratch" , 0, 0);

		ic.Op(INT_END_IF);
	}

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemPiecewise::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemPiecewise::internalSetProperties(void *data, bool isUndoRedo)
{
	bool ret = true;
	LadderElemPiecewiseProp *newProp = (LadderElemPiecewiseProp *)data;

	int i;
	int xThis = newProp->vals[0];
	for(i = 1; i < newProp->count; i++) {
		if(newProp->vals[i*2] <= xThis) {
			Error(_("Os valores X na Tabela de Linearização por Segmentos deve ser estritamente incrementais."));
			ret = false;
			break;
		}
		xThis = newProp->vals[i*2];
	}

	if(ret) {
		prop = *newProp;
	}
	delete newProp;

	return ret;
}

void *LadderElemPiecewise::getProperties(void)
{
	LadderElemPiecewiseProp *curProp = new LadderElemPiecewiseProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemPiecewise::internalSave(FILE *f)
{
	bool ret =
		fwrite_int  (f, prop.count) &&
		fwrite_ulong(f, prop.idDest.first) &&
		fwrite_int  (f, prop.idDest.second) &&
		fwrite_ulong(f, prop.idIndex.first) &&
		fwrite_int  (f, prop.idIndex.second);

	if(ret == true) {
		int i;
		for(i = 0; i < prop.count * 2; i++) {
			ret = fwrite_long(f, prop.vals[i]);
			if(ret != true) {
				break;
			}
		}
	}

	return ret;
}

bool LadderElemPiecewise::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_int  (f, &prop.count) &&
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idIndex.first) &&
		fread_int  (f, &prop.idIndex.second);

	if(ret == true) {
		int i;
		for(i = 0; i < prop.count * 2; i++) {
			ret = fread_long(f, &prop.vals[i]);
			if(ret != true) {
				break;
			}
		}
	}

	if(ret == true) {
		Diagram->updateTypeIO(prop.idDest .first);
		Diagram->updateTypeIO(prop.idIndex.first);
	}

	return ret;
}

LadderElem *LadderElemPiecewise::Clone(LadderDiagram *diagram)
{
	LadderElemPiecewise *clone = new LadderElemPiecewise(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemPiecewise::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idDest.first || id == prop.idIndex.first) {
		return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemPiecewise::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Dest.pin = prop.idDest;
	Diagram->updateIO(owner, this, infoIO_Dest, isDiscard);
	prop.idDest = infoIO_Dest.pin;

	infoIO_Index.pin = prop.idIndex;
	Diagram->updateIO(owner, this, infoIO_Index, isDiscard);
	prop.idIndex = infoIO_Index.pin;
}

eType LadderElemPiecewise::getAllowedTypeIO(unsigned long id)
{
	if(prop.idDest.first == id || prop.idIndex.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemPiecewise::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idDest.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idDest;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Dest)) {
					LadderElemPiecewiseProp *data = (LadderElemPiecewiseProp *)getProperties();

					data->idDest  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idIndex.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idIndex;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Index)) {
					LadderElemPiecewiseProp *data = (LadderElemPiecewiseProp *)getProperties();

					data->idIndex  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemPiecewise::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin;

	const char *field;
	if(index == 0) {
		pin   = prop.idIndex;
		field = _("Índice");
	} else {
		pin   = prop.idDest;
		field = _("Destino");
	}

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field, VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, (index == 0) ? infoIO_Index : infoIO_Dest, true)) {
			LadderElemPiecewiseProp *data = (LadderElemPiecewiseProp *)getProperties();

			if(index == 0) {
				data->idIndex = pin;
			} else {
				data->idDest  = pin;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemPiecewise::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemPiecewiseProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemFmtString
LadderElemFmtString::LadderElemFmtString(LadderDiagram *diagram, int which) : LadderElem(false, false, true, true, which)
{
	Diagram  = diagram;

	// Caracteristicas do I/O Var
	infoIO_Var.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Var.name          = _("var");
	infoIO_Var.isBit         = false;
	infoIO_Var.type          = (which == ELEM_READ_FORMATTED_STRING) ? eType_RxUART : eType_TxUART;
	infoIO_Var.access        = (which == ELEM_READ_FORMATTED_STRING) ? eRequestAccessType_Write : eRequestAccessType_Read;
	infoIO_Var.isUniqueRead  = false;
	infoIO_Var.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Var);

	prop.txt   = _("valor: %d\\r\\n");
	prop.idVar = infoIO_Var.pin;
}

pair<string, string> LadderElemFmtString::DrawTXT(void)
{
	char top[100], bot[100];
	string svar = Diagram->getNameIO(prop.idVar);
	const char *var  = svar.c_str();

	sprintf(top, "%s: %s", getWhich() == ELEM_READ_FORMATTED_STRING ? _("READ") : _("WRITE"), var);
	sprintf(bot, "{\"%s\"}", prop.txt.c_str());

	return pair<string, string>(top, bot);
}

bool LadderElemFmtString::internalGenerateIntCode(IntCode &ic)
{
	string svar = Diagram->getNameIO(prop.idVar);
	const char *var  = svar.c_str();

	const char *stateInOut = ic.getStateInOut();
	// We want to respond to rising edges, so yes we need a one shot.
	string oneShot = ic.GenSymOneShot();

	// OneShot 
	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_IF_BIT_SET, "$SerialReady");
				if (getWhich() == ELEM_READ_FORMATTED_STRING)
					ic.Op(INT_READ_FORMATTED_STRING , var, prop.txt.c_str(), "$scratchInt", 0, 0);
				else
					ic.Op(INT_WRITE_FORMATTED_STRING, var, prop.txt.c_str(), "$scratchInt", 0, 0);
				ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratchZero", (SWORD)0);
				ic.Op(INT_IF_VARIABLE_GRT_VARIABLE, "$scratchInt", "$scratchZero");
					ic.Op(INT_SET_BIT, oneShot.c_str());
				ic.Op(INT_END_IF);
			ic.Op(INT_END_IF);
		ic.Op(INT_END_IF);
		ic.Op(INT_COPY_BIT_TO_BIT, stateInOut, oneShot.c_str());
	ic.Op(INT_ELSE);
		ic.Op(INT_CLEAR_BIT, oneShot.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemFmtString::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemFmtString::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemFmtStringProp *newProp = (LadderElemFmtStringProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemFmtString::getProperties(void)
{
	LadderElemFmtStringProp *curProp = new LadderElemFmtStringProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemFmtString::internalSave(FILE *f)
{
	return
		fwrite_string(f, prop.txt) &&
		fwrite_ulong (f, prop.idVar.first) &&
		fwrite_int   (f, prop.idVar.second);
}

bool LadderElemFmtString::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_string(f, &prop.txt) &&
		fread_ulong (f, &prop.idVar.first) &&
		fread_int   (f, &prop.idVar.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idVar.first);
	}

	return ret;
}

LadderElem *LadderElemFmtString::Clone(LadderDiagram *diagram)
{
	LadderElemFmtString *clone = new LadderElemFmtString(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemFmtString::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idVar.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemFmtString::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Var.pin = prop.idVar;
	Diagram->updateIO(owner, this, infoIO_Var, isDiscard);
	prop.idVar = infoIO_Var.pin;
}

eType LadderElemFmtString::getAllowedTypeIO(unsigned long id)
{
	if(prop.idVar.first == id) {
		return (getWhich() == ELEM_READ_FORMATTED_STRING) ? eType_RxUART : eType_TxUART;
	}

	return eType_Pending;
}

int LadderElemFmtString::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idVar.first == idSearch) {
		if(isReplace) {
			eType type = (getWhich() == ELEM_READ_FORMATTED_STRING) ? eType_RxUART : eType_TxUART;
			pair<unsigned long, int> pin = prop.idVar;
			if(Diagram->getIO(this, pin, sNewText, type, infoIO_Var)) {
				LadderElemFmtStringProp *data = (LadderElemFmtStringProp *)getProperties();

				data->idVar  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemFmtString::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	bool mode_read = (getWhich() == ELEM_READ_FORMATTED_STRING);
	pair<unsigned long, int> pin = prop.idVar;

	type = mode_read ? eType_RxUART : eType_TxUART;
	// Se variavel sem tipo, usa tipo da UART.
	eType NewType = Diagram->getTypeIO(Diagram->getNameIO(pin), name, type, true);
	if(NewType != eType_Reserved && NewType != eType_General) {
		type = NewType;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Variável"), mode_read ? VALIDATE_IS_VAR : VALIDATE_IS_VAR_OR_NUMBER, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Var)) {
			LadderElemFmtStringProp *data = (LadderElemFmtStringProp *)getProperties();

			data->idVar  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemFmtString::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemFmtStringProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemYaskawa
LadderElemYaskawa::LadderElemYaskawa(LadderDiagram *diagram, int which) : LadderElem(false, false, true, true, which)
{
	Diagram  = diagram;

	// Caracteristicas do I/O Var
	infoIO_Var.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Var.name          = _("var");
	infoIO_Var.isBit         = false;
	infoIO_Var.type          = (which == ELEM_READ_SERVO_YASKAWA) ? eType_ReadYaskawa : eType_WriteYaskawa;
	infoIO_Var.access        = (which == ELEM_READ_SERVO_YASKAWA) ? eRequestAccessType_Write : eRequestAccessType_Read;
	infoIO_Var.isUniqueRead  = false;
	infoIO_Var.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Var);

	prop.id    = 0;
	prop.txt   = "ZSET%d";
	prop.idVar = infoIO_Var.pin;
}

pair<string, string> LadderElemYaskawa::DrawTXT(void)
{
	char top[100], bot[100];

	string svar = Diagram->getNameIO(prop.idVar);
	const char *var  = svar.c_str();

	sprintf(top, "%s: %s", getWhich() == ELEM_READ_SERVO_YASKAWA ? _("RX NS600") : _("TX NS600"), var);
	sprintf(bot, "{\"%s\"}", prop.txt.c_str());

	return pair<string, string>(top, bot);
}

bool LadderElemYaskawa::internalGenerateIntCode(IntCode &ic)
{
	string svar = Diagram->getNameIO(prop.idVar);
	const char *var  = svar.c_str();

	char idtxt[MAX_NAME_LEN];
	const char *stateInOut = ic.getStateInOut();

	sprintf(idtxt, "%d", prop.id);

	// We want to respond to rising edges, so yes we need a one shot.
	string oneShot = ic.GenSymOneShot();
	string byPass  = ic.GenSymOneShot();

	// OneShot 
    ic.Op(INT_IF_BIT_SET, stateInOut);
        ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
            ic.Op(INT_IF_BIT_SET, "$SerialReady");
				ic.Op((getWhich() == ELEM_READ_SERVO_YASKAWA) ? INT_READ_SERVO_YASKAWA : INT_WRITE_SERVO_YASKAWA,
					var, prop.txt.c_str(), idtxt, 0, 0);
                ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), stateInOut);
            ic.Op(INT_END_IF);
            ic.Op(INT_CLEAR_BIT, stateInOut);
            ic.Op(INT_COPY_BIT_TO_BIT, byPass.c_str(), stateInOut);
        ic.Op(INT_END_IF);
        ic.Op(INT_IF_BIT_CLEAR, byPass.c_str());
            ic.Op(INT_IF_BIT_SET, "$SerialReady");
                ic.Op(INT_SET_BIT, byPass.c_str());
            ic.Op(INT_ELSE);
                ic.Op(INT_CLEAR_BIT, stateInOut);
            ic.Op(INT_END_IF);
        ic.Op(INT_END_IF);
    ic.Op(INT_ELSE);
		ic.Op(INT_CLEAR_BIT, oneShot.c_str());
    ic.Op(INT_END_IF);

	return true;
}

bool LadderElemYaskawa::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

bool LadderElemYaskawa::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemYaskawaProp *newProp = (LadderElemYaskawaProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemYaskawa::getProperties(void)
{
	LadderElemYaskawaProp *curProp = new LadderElemYaskawaProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemYaskawa::internalSave(FILE *f)
{
	return
		fwrite_int   (f, prop.id) &&
		fwrite_string(f, prop.txt) &&
		fwrite_ulong (f, prop.idVar.first) &&
		fwrite_int   (f, prop.idVar.second);
}

bool LadderElemYaskawa::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_int   (f, &prop.id) &&
		fread_string(f, &prop.txt) &&
		fread_ulong (f, &prop.idVar.first) &&
		fread_int   (f, &prop.idVar.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idVar.first);
	}

	return ret;
}

LadderElem *LadderElemYaskawa::Clone(LadderDiagram *diagram)
{
	LadderElemYaskawa *clone = new LadderElemYaskawa(diagram, getWhich());
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemYaskawa::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idVar.first && type != ((getWhich() == ELEM_READ_SERVO_YASKAWA) ? eType_ReadYaskawa : eType_WriteYaskawa) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemYaskawa::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Var.pin = prop.idVar;
	Diagram->updateIO(owner, this, infoIO_Var, isDiscard);
	prop.idVar = infoIO_Var.pin;
}

eType LadderElemYaskawa::getAllowedTypeIO(unsigned long id)
{
	if(prop.idVar.first == id) {
		return (getWhich() == ELEM_READ_SERVO_YASKAWA) ? eType_ReadYaskawa : eType_WriteYaskawa;
	}

	return eType_Pending;
}

int LadderElemYaskawa::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idVar.first == idSearch) {
		if(isReplace) {
			eType type = (getWhich() == ELEM_READ_SERVO_YASKAWA) ? eType_ReadYaskawa : eType_WriteYaskawa;
			pair<unsigned long, int> pin = prop.idVar;
			if(Diagram->getIO(this, pin, sNewText, type, infoIO_Var)) {
				LadderElemYaskawaProp *data = (LadderElemYaskawaProp *)getProperties();

				data->idVar  = pin;

				setProperties(Diagram->getContext(), data);

				return 1;
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemYaskawa::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idVar;

	type = (getWhich() == ELEM_READ_SERVO_YASKAWA) ? eType_ReadYaskawa : eType_WriteYaskawa;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Variável"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Var)) {
			LadderElemYaskawaProp *data = (LadderElemYaskawaProp *)getProperties();

			data->idVar  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemYaskawa::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemYaskawaProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemPersist
LadderElemPersist::LadderElemPersist(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_PERSIST)
{
	Diagram  = diagram;

	// Caracteristicas do I/O Var
	infoIO_Var.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Var.name          = _("var");
	infoIO_Var.isBit         = false;
	infoIO_Var.type          = eType_General;
	infoIO_Var.access        = eRequestAccessType_ReadWrite;
	infoIO_Var.isUniqueRead  = false;
	infoIO_Var.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Var);

	prop.idVar = infoIO_Var.pin;
}

pair<string, string> LadderElemPersist::DrawTXT(void)
{
	return pair<string, string>(Diagram->getNameIO(prop.idVar), _("{PERSIST}"));
}

bool LadderElemPersist::internalGenerateIntCode(IntCode &ic)
{
	const char *savedE2P = "$S_SavedE2P";
	string svar = Diagram->getNameIO(prop.idVar);
	const char *var  = svar.c_str();

	int EepromAddrFree = ic.getEepromAddr();
	if(EepromAddrFree < 0) {
		Error(_("Muitas variáveis persistentes!"));
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	// At startup, get the persistent variable from flash.
	string isInit = ic.GenSymOneShot();

	ic.Op(INT_CREATE_STATIC_VARIABLE, savedE2P);

	ic.Op(INT_IF_BIT_CLEAR, isInit.c_str());
		ic.Op(INT_CLEAR_BIT, "$scratch");
		ic.Op(INT_EEPROM_BUSY_CHECK, "$scratch");
		ic.Op(INT_IF_BIT_CLEAR, "$scratch");
			ic.Op(INT_EEPROM_READ, var, isInit.c_str(), EepromAddrFree);
			ic.Op(INT_SET_VARIABLE_TO_VARIABLE, savedE2P, var, 0);
		ic.Op(INT_END_IF);
	ic.Op(INT_ELSE);
		// While running, continuously compare the EEPROM copy of
		// the variable against the RAM one; if they are different,
		// write the RAM one to EEPROM. 
		ic.Op(INT_IF_VARIABLE_EQUALS_VARIABLE, savedE2P, var);
		ic.Op(INT_ELSE);
			ic.Op(INT_CLEAR_BIT, "$scratch");
			ic.Op(INT_EEPROM_BUSY_CHECK, "$scratch");
			ic.Op(INT_IF_BIT_CLEAR, "$scratch");
				ic.Op(INT_EEPROM_WRITE, var, EepromAddrFree);
			ic.Op(INT_END_IF);
		ic.Op(INT_END_IF);
	ic.Op(INT_END_IF);

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemPersist::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemPersist::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemPersistProp *newProp = (LadderElemPersistProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemPersist::getProperties(void)
{
	LadderElemPersistProp *curProp = new LadderElemPersistProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemPersist::internalSave(FILE *f)
{
	return
		fwrite_ulong (f, prop.idVar.first) &&
		fwrite_int   (f, prop.idVar.second);
}

bool LadderElemPersist::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong (f, &prop.idVar.first) &&
		fread_int   (f, &prop.idVar.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idVar.first);
	}

	return ret;
}

LadderElem *LadderElemPersist::Clone(LadderDiagram *diagram)
{
	LadderElemPersist *clone = new LadderElemPersist(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemPersist::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idVar.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemPersist::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Var.pin = prop.idVar;
	Diagram->updateIO(owner, this, infoIO_Var, isDiscard);
	prop.idVar = infoIO_Var.pin;
}

eType LadderElemPersist::getAllowedTypeIO(unsigned long id)
{
	if(prop.idVar.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemPersist::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	if(prop.idVar.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}

				pair<unsigned long, int> pin = prop.idVar;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Var)) {
					LadderElemPersistProp *data = (LadderElemPersistProp *)getProperties();

					data->idVar  = pin;

					setProperties(Diagram->getContext(), data);

					return 1;
				}
			}
		} else {
			return 1;
		}
	}

	return 0;
}

bool LadderElemPersist::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	pair<unsigned long, int> pin = prop.idVar;

	type = eType_General;

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, _("Variável"), VALIDATE_IS_VAR, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, infoIO_Var)) {
			LadderElemPersistProp *data = (LadderElemPersistProp *)getProperties();

			data->idVar  = pin;

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemPersist::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemPersistProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemPID
LadderElemPID::LadderElemPID(LadderDiagram *diagram) : LadderElem(true, false, false, false, ELEM_PID)
{
	Diagram = diagram;

	// Caracteristicas do I/O Setpoint
	infoIO_Setpoint.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Setpoint.name          = _("novo");
	infoIO_Setpoint.isBit         = false;
	infoIO_Setpoint.type          = eType_General;
	infoIO_Setpoint.access        = eRequestAccessType_Read;
	infoIO_Setpoint.isUniqueRead  = false;
	infoIO_Setpoint.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Setpoint);
	prop.idSetpoint = infoIO_Setpoint.pin;

	// Caracteristicas do I/O ProcessValue
	infoIO_ProcessValue.pin           = pair<unsigned long, int>(0, 0);
	infoIO_ProcessValue.name          = _("novo");
	infoIO_ProcessValue.isBit         = false;
	infoIO_ProcessValue.type          = eType_General;
	infoIO_ProcessValue.access        = eRequestAccessType_Read;
	infoIO_ProcessValue.isUniqueRead  = false;
	infoIO_ProcessValue.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_ProcessValue);
	prop.idProcessValue = infoIO_ProcessValue.pin;

	// Caracteristicas do I/O GainP
	infoIO_GainP.pin           = pair<unsigned long, int>(0, 0);
	infoIO_GainP.name          = _("novo");
	infoIO_GainP.isBit         = false;
	infoIO_GainP.type          = eType_General;
	infoIO_GainP.access        = eRequestAccessType_Read;
	infoIO_GainP.isUniqueRead  = false;
	infoIO_GainP.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_GainP);
	prop.idGainP = infoIO_GainP.pin;

	// Caracteristicas do I/O GainI
	infoIO_GainI.pin           = pair<unsigned long, int>(0, 0);
	infoIO_GainI.name          = _("novo");
	infoIO_GainI.isBit         = false;
	infoIO_GainI.type          = eType_General;
	infoIO_GainI.access        = eRequestAccessType_Read;
	infoIO_GainI.isUniqueRead  = false;
	infoIO_GainI.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_GainI);
	prop.idGainI = infoIO_GainI.pin;

	// Caracteristicas do I/O GainD
	infoIO_GainD.pin           = pair<unsigned long, int>(0, 0);
	infoIO_GainD.name          = _("novo");
	infoIO_GainD.isBit         = false;
	infoIO_GainD.type          = eType_General;
	infoIO_GainD.access        = eRequestAccessType_Read;
	infoIO_GainD.isUniqueRead  = false;
	infoIO_GainD.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_GainD);
	prop.idGainD = infoIO_GainD.pin;

	// Caracteristicas do I/O Output
	infoIO_Output.pin           = pair<unsigned long, int>(0, 0);
	infoIO_Output.name          = _("novo");
	infoIO_Output.isBit         = false;
	infoIO_Output.type          = eType_General;
	infoIO_Output.access        = eRequestAccessType_Write;
	infoIO_Output.isUniqueRead  = false;
	infoIO_Output.isUniqueWrite = false;

	Diagram->getIO(this, infoIO_Output);
	prop.idOutput = infoIO_Output.pin;
}

pair<string, string> LadderElemPID::DrawTXT(void)
{
	return pair<string, string>(Diagram->getNameIO(prop.idOutput), "{ PID }");
}

bool LadderElemPID::internalGenerateIntCode(IntCode &ic)
{
	string sSetpoint     = Diagram->getNameIO(prop.idSetpoint    );
	string sProcessValue = Diagram->getNameIO(prop.idProcessValue);
	string sGainP        = Diagram->getNameIO(prop.idGainP       );
	string sGainI        = Diagram->getNameIO(prop.idGainI       );
	string sGainD        = Diagram->getNameIO(prop.idGainD       );
	string sOutput       = Diagram->getNameIO(prop.idOutput      );

	const char *Setpoint     = ic.VarFromExpr(sSetpoint    .c_str(), "$scratch_sp");
	const char *ProcessValue = ic.VarFromExpr(sProcessValue.c_str(), "$scratch_pv");
	const char *GainP        = ic.VarFromExpr(sGainP       .c_str(), "$scratch_p");
	const char *GainI        = ic.VarFromExpr(sGainI       .c_str(), "$scratch_i");
	const char *GainD        = ic.VarFromExpr(sGainD       .c_str(), "$scratch_d");
	const char *Output       = ic.VarFromExpr(sOutput      .c_str(), "$scratch_op");

	const char *error        = "$scratch_error";
	const char *errorI       = "$scratch_errorI";
	const char *parcelGainP  = "$scratch_gainp";
	const char *parcelGainI  = ic.VarFromExpr("0", "$scratch_gaini");
	const char *parcelGainD  = "$scratch_gaind";
	const char *valZero      = ic.VarFromExpr(    "0", "$scratch_zero" );
	const char *val10K       = ic.VarFromExpr("10000", "$scratch_10k"  );
	const char *val100       = ic.VarFromExpr(  "100", "$scratch_100"  );
	const char *lastI        = "$S_LastI";
	const char *lastError    = "$S_LastErr";

	const char *stateInOut   = ic.getStateInOut();

	ic.Op(INT_IF_BIT_SET, stateInOut);
		// Criacao de variaveis estaticas
		ic.Op(INT_CREATE_STATIC_VARIABLE, lastError);
		ic.Op(INT_CREATE_STATIC_VARIABLE, lastI    );

		// Calculo da parcela P
		ic.Op(INT_SET_VARIABLE_SUBTRACT, error      , Setpoint, ProcessValue, 0, 0);
		ic.Op(INT_SET_VARIABLE_MULTIPLY, parcelGainP, GainP   , error       , 0, 0);

		// Calculo da parcela I
		ic.Op(INT_IF_VARIABLE_EQUALS_VARIABLE, GainI, valZero);
		ic.Op(INT_ELSE);
			ic.Op(INT_SET_VARIABLE_MULTIPLY, errorI     , error      , val10K, 0, 0);
			ic.Op(INT_SET_VARIABLE_DIVIDE  , parcelGainI, errorI     , GainI , 0, 0);
			ic.Op(INT_SET_VARIABLE_ADD     , parcelGainI, parcelGainI, lastI , 0, 0);
		ic.Op(INT_END_IF);

		// Calculo da parcela D
		ic.Op(INT_SET_VARIABLE_SUBTRACT, parcelGainD, error      , lastError  , 0, 0);
		ic.Op(INT_SET_VARIABLE_MULTIPLY, parcelGainD, parcelGainD, GainD      , 0, 0);

		// Soma as parcelas, gerando Output
		ic.Op(INT_SET_VARIABLE_ADD     , Output     , parcelGainP, parcelGainD, 0, 0);
		ic.Op(INT_SET_VARIABLE_ADD     , Output     , Output     , parcelGainI, 0, 0);
		ic.Op(INT_SET_VARIABLE_DIVIDE  , Output     , Output     , val100     , 0, 0);

		// Salva os valores calculados para uso no calculo seguinte
		ic.Op(INT_SET_VARIABLE_TO_VARIABLE, lastError, error      , 0);
		ic.Op(INT_SET_VARIABLE_TO_VARIABLE, lastI    , parcelGainI, 0);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemPID::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

bool LadderElemPID::internalSetProperties(void *data, bool isUndoRedo)
{
	LadderElemPIDProp *newProp = (LadderElemPIDProp *)data;

	prop = *newProp;
	delete newProp;

	return true;
}

void *LadderElemPID::getProperties(void)
{
	LadderElemPIDProp *curProp = new LadderElemPIDProp;

	*curProp = prop;

	return curProp;
}

// Funcoes para ler / gravar dados especificos do elemento no disco
bool LadderElemPID::internalSave(FILE *f)
{
	return
		fwrite_ulong(f, prop.idSetpoint.first) &&
		fwrite_int  (f, prop.idSetpoint.second) &&
		fwrite_ulong(f, prop.idProcessValue.first) &&
		fwrite_int  (f, prop.idProcessValue.second) &&
		fwrite_ulong(f, prop.idGainP.first) &&
		fwrite_int  (f, prop.idGainP.second) &&
		fwrite_ulong(f, prop.idGainI.first) &&
		fwrite_int  (f, prop.idGainI.second) &&
		fwrite_ulong(f, prop.idGainD.first) &&
		fwrite_int  (f, prop.idGainD.second) &&
		fwrite_ulong(f, prop.idOutput.first) &&
		fwrite_int  (f, prop.idOutput.second);
}

bool LadderElemPID::internalLoad(FILE *f, unsigned int version)
{
	bool ret =
		fread_ulong(f, &prop.idSetpoint.first) &&
		fread_int  (f, &prop.idSetpoint.second) &&
		fread_ulong(f, &prop.idProcessValue.first) &&
		fread_int  (f, &prop.idProcessValue.second) &&
		fread_ulong(f, &prop.idGainP.first) &&
		fread_int  (f, &prop.idGainP.second) &&
		fread_ulong(f, &prop.idGainI.first) &&
		fread_int  (f, &prop.idGainI.second) &&
		fread_ulong(f, &prop.idGainD.first) &&
		fread_int  (f, &prop.idGainD.second) &&
		fread_ulong(f, &prop.idOutput.first) &&
		fread_int  (f, &prop.idOutput.second);

	if(ret == true) {
		Diagram->updateTypeIO(prop.idSetpoint    .first);
		Diagram->updateTypeIO(prop.idProcessValue.first);
		Diagram->updateTypeIO(prop.idGainP       .first);
		Diagram->updateTypeIO(prop.idGainI       .first);
		Diagram->updateTypeIO(prop.idGainD       .first);
		Diagram->updateTypeIO(prop.idOutput      .first);
	}

	return ret;
}

LadderElem *LadderElemPID::Clone(LadderDiagram *diagram)
{
	LadderElemPID *clone = new LadderElemPID(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemPID::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idSetpoint.first ||
		id == prop.idProcessValue.first ||
		id == prop.idGainP.first ||
		id == prop.idGainI.first ||
		id == prop.idGainD.first ||
		id == prop.idOutput.first) {
			return Diagram->IsGenericTypeIO(type);
	}

	return true;
}

void LadderElemPID::updateIO(LadderDiagram *owner, bool isDiscard)
{
	infoIO_Setpoint.pin = prop.idSetpoint;
	Diagram->updateIO(owner, this, infoIO_Setpoint, isDiscard);
	prop.idSetpoint = infoIO_Setpoint.pin;

	infoIO_ProcessValue.pin = prop.idProcessValue;
	Diagram->updateIO(owner, this, infoIO_ProcessValue, isDiscard);
	prop.idProcessValue = infoIO_ProcessValue.pin;

	infoIO_GainP.pin = prop.idGainP;
	Diagram->updateIO(owner, this, infoIO_GainP, isDiscard);
	prop.idGainP = infoIO_GainP.pin;

	infoIO_GainI.pin = prop.idGainI;
	Diagram->updateIO(owner, this, infoIO_GainI, isDiscard);
	prop.idGainI = infoIO_GainI.pin;

	infoIO_GainD.pin = prop.idGainD;
	Diagram->updateIO(owner, this, infoIO_GainD, isDiscard);
	prop.idGainD = infoIO_GainD.pin;

	infoIO_Output.pin = prop.idOutput;
	Diagram->updateIO(owner, this, infoIO_Output, isDiscard);
	prop.idOutput = infoIO_Output.pin;
}

eType LadderElemPID::getAllowedTypeIO(unsigned long id)
{
	if(prop.idSetpoint.first == id ||
		prop.idProcessValue.first == id ||
		prop.idGainP.first == id ||
		prop.idGainI.first == id ||
		prop.idGainD.first == id ||
		prop.idOutput.first == id) {
		return eType_General;
	}

	return eType_Pending;
}

int LadderElemPID::SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace)
{
	unsigned int n = 0;

	if(prop.idSetpoint.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idSetpoint;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Setpoint)) {
					LadderElemPIDProp *data = (LadderElemPIDProp *)getProperties();

					data->idSetpoint  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idProcessValue.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idProcessValue;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_ProcessValue)) {
					LadderElemPIDProp *data = (LadderElemPIDProp *)getProperties();

					data->idProcessValue  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idGainP.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idGainP;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_GainP)) {
					LadderElemPIDProp *data = (LadderElemPIDProp *)getProperties();

					data->idGainP  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idGainI.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idGainI;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_GainI)) {
					LadderElemPIDProp *data = (LadderElemPIDProp *)getProperties();

					data->idGainI  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idGainD.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idGainD;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_GainD)) {
					LadderElemPIDProp *data = (LadderElemPIDProp *)getProperties();

					data->idGainD  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	if(prop.idOutput.first == idSearch) {
		if(isReplace) {
			eType type = Diagram->getDetailsIO(sNewText).type;
			if(type != eType_Reserved) {
				if(type == eType_Pending) {
					type = eType_General;
				}
				pair<unsigned long, int> pin = prop.idOutput;
				if(Diagram->getIO(this, pin, sNewText, type, infoIO_Output)) {
					LadderElemPIDProp *data = (LadderElemPIDProp *)getProperties();

					data->idOutput  = pin;

					setProperties(Diagram->getContext(), data);

					n++;
				}
			}
		} else {
			return 1;
		}
	}

	return n;
}

bool LadderElemPID::internalUpdateNameTypeIO(unsigned int index, string name, eType type)
{
	char *field = NULL;
	tRequestIO request;
	pair<unsigned long, int> pin;
	unsigned int rules = VALIDATE_IS_VAR_OR_NUMBER;

	// Se variavel sem tipo, usa tipo geral.
	type = Diagram->getTypeIO(Diagram->getNameIO(pin), name, eType_General, true);
	if(type == eType_Reserved) {
		type = eType_General;
	}

	switch(index) {
	case 0:
		request = infoIO_Setpoint;
		pin     = prop.idSetpoint;
		break;

	case 1:
		request = infoIO_GainP;
		pin     = prop.idGainP;
		break;

	case 2:
		request = infoIO_GainI;
		pin     = prop.idGainI;
		break;

	case 3:
		request = infoIO_GainD;
		pin     = prop.idGainD;
		break;

	case 4:
		rules   = VALIDATE_IS_VAR;
		request = infoIO_Output;
		pin     = prop.idOutput;
		break;

	case 5:
		request = infoIO_ProcessValue;
		pin     = prop.idProcessValue;
		break;
	}

	if(Diagram->IsValidNameAndType(pin.first, name.c_str(), type, field, rules, 0, 0)) {
		if(Diagram->getIO(this, pin, name, type, request, true)) {
			LadderElemPIDProp *data = (LadderElemPIDProp *)getProperties();

			switch(index) {
			case 0:
				data->idSetpoint  = pin;
				break;

			case 1:
				data->idGainP  = pin;
				break;

			case 2:
				data->idGainI  = pin;
				break;

			case 3:
				data->idGainD  = pin;
				break;

			case 4:
				data->idOutput  = pin;
				break;

			case 5:
				data->idProcessValue  = pin;
				break;
			}

			setProperties(Diagram->getContext(), data);

			return true;
		}
	}

	return false;
}

bool LadderElemPID::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemPIDProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

// Classe LadderElemX
LadderElemX::LadderElemX(LadderDiagram *diagram) : LadderElem(false, false, false, false, ELEM_PADDING)
{
	Diagram = diagram;
}

pair<string, string> LadderElemX::DrawTXT(void)
{
	return pair<string, string>("", "");
}

bool LadderElemX::internalGenerateIntCode(IntCode &ic)
{
	return true;
}

bool LadderElemX::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

LadderElem *LadderElemX::Clone(LadderDiagram *diagram)
{
	LadderElemX *clone = new LadderElemX(diagram);
	clone->updateIO(diagram, true); // Descarta os I/Os registrados
	clone->internalSetProperties(getProperties());

	return clone;
}

bool LadderElemX::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	if(isDiscard && action.action == eSetProp) {
		delete (LadderElemXProp *)(((UndoRedoData *)action.data)->SetProp.data);
	}

	return true; // Sempre retorna sucesso...
}

//Classe LadderCircuit
LadderCircuit::LadderCircuit(void)
{
	isSeries = true;
}

LadderCircuit::LadderCircuit(bool newSeries)
{
	isSeries = newSeries;
}

LadderCircuit::~LadderCircuit(void)
{
	vector<Subckt>::iterator it;
	for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
		if(it->elem != nullptr) {
			UnallocElem(it->elem);
		} else {
			delete it->subckt;
		}
	}

	vectorSubckt.clear();
}

bool LadderCircuit::DrawElementTXT(tFncDrawChars DrawChars, vector< vector<int> > &DisplayMatrix, LadderElem *elem, int *cx, int *cy, bool poweredBefore)
{
	if(elem == nullptr) return false;

	int width                = elem->getWidthTXT();
	bool poweredAfter        = elem->IsPoweredAfter();
	bool isPlaceHolder       = elem->getWhich() == ELEM_PLACEHOLDER;
	pair<string, string> txt = elem->DrawTXT();

	int extra = width*POS_WIDTH - (elem->IsFormatted() ? FormattedStrlen(txt.first.c_str()) : txt.first.size());

	if(elem->IsComment()) {
		DrawChars(*cx, *cy + (POS_HEIGHT/2)    , (char *)txt.first .c_str());
		DrawChars(*cx, *cy + (POS_HEIGHT/2) - 1, (char *)txt.second.c_str());
	} else {
		if(elem->IsEOL()) {
			while(*cx < (ColsAvailable - width)*POS_WIDTH) {
				int gx = *cx/POS_WIDTH;
				int gy = *cy/POS_HEIGHT;

				DisplayMatrix[gx][gy] = ELEM_PADDING;

				char buf[256];
				int j;
				for(j = 0; j < POS_WIDTH; j++) {
					buf[j] = '-';
				}
				buf[j] = '\0';
				DrawChars(*cx, *cy + (POS_HEIGHT/2), buf);
				*cx += POS_WIDTH;
			}
		}

		CenterWithSpaces    (*cx, *cy, (char *)txt.first .c_str(), poweredAfter , false, extra);
		CenterWithWiresWidth(*cx, *cy, (char *)txt.second.c_str(), poweredBefore, poweredAfter  ,
			isPlaceHolder ? 2 : width*POS_WIDTH);
	}

	*cx += width*POS_WIDTH;

    // And now we can enter the element into the display matrix so that the
    // UI routines know what element is at position (gx, gy) when the user
    // clicks there, and so that we know where to put the cursor if this
    // element is selected.

    int gx = *cx/POS_WIDTH;
    int gy = *cy/POS_HEIGHT;
//    if(CheckBoundsUndoIfFails(gx, gy)) 
//		return FALSE;
//    DM_BOUNDS(gx, gy);

	for(int i = width; i > 0; i--) {
		DisplayMatrix[gx - i][gy] = elem->getWhich();
	}

	return poweredAfter;
}

//-----------------------------------------------------------------------------
// Draw a vertical wire one leaf element unit high up from (cx, cy), where cx
// and cy are in charcter units.
//-----------------------------------------------------------------------------
void LadderCircuit::VerticalWireTXT(tFncDrawChars DrawChars, int cx, int cy)
{
    int j;
    for(j = 1; j < POS_HEIGHT; j++) {
        DrawChars(cx, cy + (POS_HEIGHT/2 - j), "|");
    }
    DrawChars(cx, cy + (POS_HEIGHT/2), "+");
    DrawChars(cx, cy + (POS_HEIGHT/2 - POS_HEIGHT), "+");
}

bool LadderCircuit::DrawTXT(tFncDrawChars DrawChars, vector< vector<int> > &DisplayMatrix, int *cx, int *cy, bool poweredBefore, int ColsAvailable)
{
    int cx0 = *cx, cy0 = *cy;

	bool poweredAfter = poweredBefore;
	vector<Subckt>::iterator it;

	if(isSeries) {
		for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
			if(it->elem != nullptr) {
				poweredAfter = DrawElementTXT(DrawChars, DisplayMatrix, it->elem, cx, cy, poweredAfter);
			} else {
				poweredAfter = it->subckt->DrawTXT(DrawChars, DisplayMatrix, cx, cy, poweredAfter, ColsAvailable);
			}
		}
	} else {
		int widthMax = getWidthTXT(ColsAvailable - (*cx / POS_WIDTH));
		int heightMax = getHeightTXT();
		int lowestPowered = -1;
		int justDrewHeight;
		int downBy = 0;

		poweredAfter = false;

		for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
			bool poweredThis;

			if(it->elem != nullptr) {
				poweredThis = DrawElementTXT(DrawChars, DisplayMatrix, it->elem, cx, cy, poweredBefore);
				justDrewHeight = 1;
			} else {
				poweredThis = it->subckt->DrawTXT(DrawChars, DisplayMatrix, cx, cy, poweredBefore, ColsAvailable);
				justDrewHeight = it->subckt->getHeightTXT();
			}

			while((*cx - cx0) < widthMax*POS_WIDTH) {
				int gx = *cx/POS_WIDTH;
				int gy = *cy/POS_HEIGHT;

				if(gx>0 && DisplayMatrix[gx-1][gy] == ELEM_PLACEHOLDER)
					break;
				DisplayMatrix[gx][gy] = ELEM_PADDING;

				char buf[256];
				int j;
				for(j = 0; j < POS_WIDTH; j++) {
					buf[j] = '-';
				}
				buf[j] = '\0';
				DrawChars(*cx, *cy + (POS_HEIGHT/2), buf);
				*cx += POS_WIDTH;
			}

			*cx = cx0;
			*cy += POS_HEIGHT*justDrewHeight;

			downBy += justDrewHeight;
			if(poweredThis) {
				lowestPowered = downBy - 1;
			}
		}
		*cx = cx0 + POS_WIDTH*widthMax;
		*cy = cy0;

		int j;
		bool needWire;

		if(*cx/POS_WIDTH != ColsAvailable) {
			int gx = *cx/POS_WIDTH;
			int gy = *cy/POS_HEIGHT;

			needWire = false;
			for(j = heightMax - 1; j >= 1; j--) {
				if(DisplayMatrix[gx - 1][gy + j] != ELEM_PLACEHOLDER) {
					needWire = true;
				}
				if(needWire) VerticalWireTXT(DrawChars, *cx - 1, *cy + j*POS_HEIGHT);
			}
		}

		needWire = false;
		for(j = heightMax - 1; j >= 1; j--) {
			if(DisplayMatrix[cx0/POS_WIDTH][*cy/POS_HEIGHT + j]) {
				needWire = true;
			}
			if(needWire) VerticalWireTXT(DrawChars, cx0 - 1, *cy + j*POS_HEIGHT);
		}
	}

	return poweredAfter;
}

bool LadderCircuit::IsComment(void)
{
	return vectorSubckt.size() == 1 && vectorSubckt[0].elem != nullptr && vectorSubckt[0].elem->IsComment();
}

bool LadderCircuit::IsEmpty(void)
{
	return vectorSubckt.empty() || (vectorSubckt.size() == 1 && vectorSubckt[0].elem != nullptr &&
		vectorSubckt[0].elem->getWhich() == ELEM_PLACEHOLDER);
}

bool LadderCircuit::IsLast(LadderElem *elem)
{
	return !vectorSubckt.empty() && vectorSubckt[vectorSubckt.size() - 1].elem == elem;
}

bool LadderCircuit::HasEOL(bool checkActive)
{
	vector<Subckt>::iterator it;

	for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
		if(it->elem != nullptr) {
			if(checkActive ? it->elem->IsActiveEOL() : it->elem->IsEOL()) {
				return true;
			}
		} else if(it->subckt->HasEOL(checkActive)) {
			return true;
		}
	}

	return false;
}

bool LadderCircuit::UartFunctionUsed(void)
{
	vector<Subckt>::iterator it;

	for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
		if(it->elem != nullptr) {
			if(it->elem->IsUART()) {
				return true;
			}
		} else if(it->subckt->UartFunctionUsed()) {
			return true;
		}
	}

	return false;
}

bool LadderCircuit::GenerateIntCode(IntCode &ic)
{
	bool ret = true;
	unsigned int i;

	if(isSeries) {
		ic.Comment(_("iniciando serie ["));
		for(i = 0; i < vectorSubckt.size() && ret; i++) {
			if(vectorSubckt[i].elem != nullptr) {
				ret = vectorSubckt[i].elem->GenerateIntCode(ic);
			} else {
				ret = vectorSubckt[i].subckt->GenerateIntCode(ic);
			}
		}
		ic.Comment(_("terminando serie ["));
	} else {
		string parThis, parOut;
		parThis = ic.GenSymParThis();
		parOut  = ic.GenSymParOut ();

		string stateInOut = ic.getStateInOut();
		ic.setStateInOut(parThis);

		ic.Comment(_("iniciando paralelo ["));
		ic.Op(INT_CLEAR_BIT, parOut.c_str());

        for(i = 0; i < vectorSubckt.size() && ret; i++) {
			ic.Op(INT_COPY_BIT_TO_BIT, parThis.c_str(), stateInOut.c_str());

			if(vectorSubckt[i].elem != nullptr) {
				ret = vectorSubckt[i].elem->GenerateIntCode(ic);
			} else {
				ret = vectorSubckt[i].subckt->GenerateIntCode(ic);
			}

			ic.Op(INT_IF_BIT_SET, parThis.c_str());
			ic.Op(INT_SET_BIT, parOut.c_str());
			ic.Op(INT_END_IF);
        }

		ic.Op(INT_COPY_BIT_TO_BIT, stateInOut.c_str(), parOut.c_str());
		ic.setStateInOut(stateInOut);

		ic.Comment(_("terminando paralelo ["));
	}

	return ret;
}

unsigned int LadderCircuit::getSubcktIndex(Subckt s)
{
	unsigned int i;

	for(i = 0; i < vectorSubckt.size(); i++) {
		if(vectorSubckt[i].elem != nullptr) {
			if(s.elem == vectorSubckt[i].elem) {
				break; // elemento coincide! interrompe busca
			}
		} else if(s.subckt == vectorSubckt[i].subckt) {
			break; // subcircuito coincide! interrompe busca
		}
	}

	return i;
}

LadderCircuit *LadderCircuit::getSubcktForElement(LadderElem *elem)
{
	LadderCircuit *subckt = nullptr;
	vector<Subckt>::size_type i;

	if(elem != nullptr) {
		for(i = 0; i < vectorSubckt.size() && subckt == nullptr; i++) {
			if(vectorSubckt[i].elem != nullptr) {
				if(elem == vectorSubckt[i].elem) {
					subckt = this;
					break;
				}
			} else {
				subckt = vectorSubckt[i].subckt->getSubcktForElement(elem);
			}
		}
	}

	return subckt;
}

Subckt LadderCircuit::getPrevious(Subckt previous)
{
	Subckt ret = { nullptr, nullptr };
	vector<Subckt>::size_type i;

	if(previous.subckt != nullptr) {
		if(previous.elem != nullptr && previous.subckt != this) {
			return previous.subckt->getPrevious(previous);
		} else if(previous.elem != nullptr || previous.subckt != this) {
			if(previous.elem != nullptr) {
				for(i = 0; i < vectorSubckt.size(); i++) {
					if(vectorSubckt[i].elem == previous.elem) {
						break;
					}
				}
			} else {
				for(i = 0; i < vectorSubckt.size(); i++) {
					if(vectorSubckt[i].elem == nullptr) {
						if(vectorSubckt[i].subckt == previous.subckt) {
							break;
						} else {
							ret = vectorSubckt[i].subckt->getPrevious(previous);
							if(ret.subckt != nullptr) {
								return ret;
							}
						}
					}
				}
			}

			// Recua para o item anterior e verifica se nao alcancou o inicio do vetor
			if(i > 0) {
				i--;
				ret.elem = vectorSubckt[i].elem;
				if(ret.elem == nullptr) { // Elemento eh um subcircuito
					ret.elem   = vectorSubckt[i].subckt->getLastElement();
					ret.subckt = vectorSubckt[i].subckt->getSubcktForElement(previous.elem);
				} else { // Elemento nao eh subcircuito, nos somos o subcircuito do elemento
					ret.subckt = this;
				}
			}
		}
	}

	return ret;
}

Subckt LadderCircuit::getNext(Subckt next)
{
	Subckt ret = { nullptr, nullptr };
	vector<Subckt>::size_type i;

	if(next.subckt != nullptr) {
		if(next.elem != nullptr && next.subckt != this) {
			return next.subckt->getNext(next);
		} else if(next.elem != nullptr || next.subckt != this) {
			if(next.elem != nullptr) {
				for(i = 0; i < vectorSubckt.size(); i++) {
					if(vectorSubckt[i].elem == next.elem) {
						break;
					}
				}
			} else {
				for(i = 0; i < vectorSubckt.size(); i++) {
					if(vectorSubckt[i].elem == nullptr) {
						if(vectorSubckt[i].subckt == next.subckt) {
							break;
						} else {
							ret = vectorSubckt[i].subckt->getNext(next);
							if(ret.subckt != nullptr) {
								return ret;
							}
						}
					}
				}
			}

			// Avanca para o proximo item e verifica se nao alcancou o final do vetor
			i++;
			if(i < vectorSubckt.size()) {
				ret.elem = vectorSubckt[i].elem;
				if(ret.elem == nullptr) { // Elemento eh um subcircuito
					ret.elem   = vectorSubckt[i].subckt->getFirstElement();
					ret.subckt = vectorSubckt[i].subckt->getSubcktForElement(next.elem);
				} else { // Elemento nao eh subcircuito, nos somos o subcircuito do elemento
					ret.subckt = this;
				}
			}
		}
	}

	return ret;
}

LadderCircuit *LadderCircuit::getParentSubckt(LadderCircuit *subckt)
{
	if(this == subckt) return nullptr;

	LadderCircuit *parent = nullptr;
	vector<Subckt>::size_type i;

	if(subckt != nullptr) {
		for(i = 0; i < vectorSubckt.size() && parent == nullptr; i++) {
			if(vectorSubckt[i].elem == nullptr) {
				if(vectorSubckt[i].subckt == subckt) {
					parent = this;
				} else {
					parent = vectorSubckt[i].subckt->getParentSubckt(subckt);
				}
			}
		}
	}

	return parent;
}

LadderElem *LadderCircuit::getFirstElement(void)
{
	if(vectorSubckt.size() > 0) {
		if(vectorSubckt[0].elem != nullptr) {
			return vectorSubckt[0].elem;
		} else if(vectorSubckt[0].subckt != nullptr) {
			return vectorSubckt[0].subckt->getFirstElement();
		}
	}

	return nullptr;
}

LadderElem *LadderCircuit::getLastElement(void)
{
	unsigned int index = vectorSubckt.size();
	if(index > 0) {
		index--;
		if(vectorSubckt[index].elem != nullptr) {
			return vectorSubckt[index].elem;
		} else if(vectorSubckt[index].subckt != nullptr) {
			return vectorSubckt[index].subckt->getLastElement();
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// Determine the height, in leaf element units, of a particular subcircuit.
// The height of a leaf is 1, the height of a parallel circuit is the sum of
// of the heights of its members, and the height of a series circuit is the
// maximum of the heights of its members. (This is the dual of the width
// case.)
//-----------------------------------------------------------------------------
int LadderCircuit::getHeightTXT(void)
{
	int thisHeight, height = 0;

	vector<Subckt>::size_type i;

	for(i = 0; i < vectorSubckt.size(); i++) {
		if(isSeries) {
			if(vectorSubckt[i].elem != nullptr) {
				thisHeight = 1;
			} else {
				thisHeight = vectorSubckt[i].subckt->getHeightTXT();
			}

			if(thisHeight > height) {
				height = thisHeight;
			}
		} else {
			height += vectorSubckt[i].elem != nullptr ? 1 : vectorSubckt[i].subckt->getHeightTXT();
		}
	}

	return height;
}

//-----------------------------------------------------------------------------
// Determine the width, in leaf element units, of a particular subcircuit.
// The width of a leaf is 1, the width of a series circuit is the sum of
// of the widths of its members, and the width of a parallel circuit is
// the maximum of the widths of its members.
//-----------------------------------------------------------------------------
int LadderCircuit::getWidthTXT(int ColsAvailable)
{
	int thisWidth, width = 0;

	vector<Subckt>::size_type i;

	for(i = 0; i < vectorSubckt.size(); i++) {
		if(vectorSubckt[i].elem != nullptr) {
			thisWidth = vectorSubckt[i].elem->getWidthTXT();
			if(vectorSubckt[i].elem->IsEOL()) {
				thisWidth = max(ColsAvailable - (isSeries ? width : 0), thisWidth);
			}
		} else {
			thisWidth = vectorSubckt[i].subckt->getWidthTXT(ColsAvailable - (isSeries ? width : 0));
		}

		if(isSeries) {
			width += thisWidth;
		} else {
			if(thisWidth > width) {
				width = thisWidth;
			}
		}
	}

	return width;
}

void LadderCircuit::setDiagram(LadderDiagram *newDiagram)
{
	vector<Subckt>::size_type i;

	for(i = 0; i < vectorSubckt.size(); i++) {
		if(vectorSubckt[i].elem != nullptr) {
			vectorSubckt[i].elem  ->setDiagram(newDiagram);
		} else {
			vectorSubckt[i].subckt->setDiagram(newDiagram);
		}
	}
}

void LadderCircuit::DeleteEndPlaceHolder(LadderContext context, LadderElem *elem)
{
	vector<Subckt>::iterator it = vectorSubckt.begin() + vectorSubckt.size();

	if(!vectorSubckt.empty()) {
		if((it-1)->elem == nullptr) {
			(it-1)->subckt->DeleteEndPlaceHolder(context, elem);
		} else if((it-1)->elem->getWhich() == ELEM_PLACEHOLDER && vectorSubckt.size() > 1 &&
			(it-1)->elem != context.ParallelStart && (it-1)->elem != elem) {
				DelElement((it-1)->elem, context);
		}
	}
}

void LadderCircuit::AddPlaceHolderIfNoEOL(LadderContext context, LadderElem *elem)
{
	vector<Subckt>::iterator it;

	if(IsSeries()) {
		if(!HasEOL()) {
			it = vectorSubckt.begin() + vectorSubckt.size();
			if(vectorSubckt.empty() || ((it-1)->elem == nullptr && !(it-1)->subckt->IsSeries())) {
				// Ao adicionar um elemento de final de linha, existe a possibilidade de ja existir algum no
				// circuito paralelo que era o ultimo elemento.
				// Dessa forma devemos percorrer este circuito para remover um eventual PlaceHolder existente.
				if(!vectorSubckt.empty()) {
					(it-1)->subckt->DeleteEndPlaceHolder(context, elem);
				}

				// Se o circuito estiver vazio ou se adicionamos um paralelo e for o ultimo elemento,
				// devemos adicionar o elemento de final de linha (PlaceHolder)
				Subckt s = { new LadderElemPlaceHolder(context.Diagram), this };
				InsertSubckt(context, getSize(), s);
			}
		} else if(!vectorSubckt.empty()) {
			it = (vectorSubckt.end() - 1);
			if(it->elem == nullptr) {
				it->subckt->AddPlaceHolderIfNoEOL(context, elem);
			}
		}
	} else {
		for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
			if(it->elem == nullptr) {
				it->subckt->AddPlaceHolderIfNoEOL(context, elem);
			}
		}
	}
}

bool LadderCircuit::InsertSubckt(LadderContext context, unsigned int pos, Subckt s,
	bool isMove, bool isUndoRedo)
{
	if(s.elem != nullptr) {
		s.subckt = this;
	}

	if(pos >= getSize()) {
		vectorSubckt.push_back(s);
	} else {
		vectorSubckt.insert(vectorSubckt.begin() + pos, s);
	}

	// Registro da acao para desfazer / refazer
	if(!isMove && !isUndoRedo) {
		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->InsertSubckt.pos    = pos;
		data->InsertSubckt.subckt = s;

		action.action        = eInsertSubckt;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = context;
		action.data          = data;
		action.io            = nullptr;
		action.elem          = nullptr;
		action.subckt        = this;

		context.Diagram->RegisterAction(action);
	}

	return true;
}

bool LadderCircuit::DeleteSubckt(LadderContext context, unsigned int pos,
	bool isMove, bool isUndoRedo)
{
	bool ret = false;

	if(pos < getSize()) {
		vector<Subckt>::iterator it = vectorSubckt.begin() + pos;

	// Registro da acao para desfazer / refazer
		if(!isMove && !isUndoRedo) {
			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			data->DeleteSubckt.pos    = pos;
			data->DeleteSubckt.subckt = *it;

			action.action        = eDeleteSubckt;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = context;
			action.data          = data;
			action.io            = nullptr;
			action.elem          = nullptr;
			action.subckt        = this;

			context.Diagram->RegisterAction(action);
		}

		vectorSubckt.erase(it);

		ret = true;
	}

	return ret;
}

bool LadderCircuit::MoveSubckt(LadderContext context, unsigned int pos, LadderCircuit *fromCircuit,
	unsigned int fromPos, bool isUndoRedo)
{
	bool ret = false;
	Subckt s = fromCircuit->getSubckt(fromPos);

	if(s.elem != nullptr || s.subckt != nullptr) {
		// Se recebeu o elemento solicitado, remove-o do circuito antigo e o adiciona ao novo.
		fromCircuit->DeleteSubckt(context, fromPos, true, isUndoRedo);
		InsertSubckt(context, pos, s, true, isUndoRedo);

		// Registro da acao para desfazer / refazer
		if(!isUndoRedo) {
			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			data->MoveSubckt.pos     = pos;
			data->MoveSubckt.fromPos = fromPos;
			data->MoveSubckt.circuit = fromCircuit;

			action.action        = eMoveSubckt;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = context;
			action.data          = data;
			action.io            = nullptr;
			action.elem          = nullptr;
			action.subckt        = this;

			context.Diagram->RegisterAction(action);
		}

		ret = true;
	}

	return ret;
}

bool LadderCircuit::InsertParallel(LadderElem *elem, unsigned int start, unsigned int end, LadderContext &context)
{
	bool ret = false;

	if(isSeries) {
		// Inserindo em serie
		unsigned int i, qty = end - start + 1;

		// Cria um novo circuito serie que vai conter os elementos do intervalo selecionado
		LadderCircuit *new_series = new LadderCircuit(true);

		for(i=0; i < qty; i++) {
			new_series->MoveSubckt(context, new_series->getSize(), this, start);
		}

		// Phase 4: creates a new parallel subcircuit which will hold the previously created series
		//          subcircuit and the new element.
		LadderCircuit *new_parallel = new LadderCircuit(false);
		Subckt series = { nullptr, new_series }, new_elem = { elem, new_parallel };

		if(context.SelectedState == SELECTED_ABOVE) {
			new_parallel->InsertSubckt(context, new_parallel->getSize(), new_elem);
			new_parallel->InsertSubckt(context, new_parallel->getSize(), series  );
		} else {
			new_parallel->InsertSubckt(context, new_parallel->getSize(), series  );
			new_parallel->InsertSubckt(context, new_parallel->getSize(), new_elem);
		}

		new_elem.elem   = nullptr;
		new_elem.subckt = new_parallel;
		InsertSubckt(context, start, new_elem);

		return true;
	} else {
		Subckt s = { elem, this };

		if(context.SelectedState != SELECTED_ABOVE) {
			start++;
		}

		// Inserindo em paralelo
		InsertSubckt(context, start, s);

		ret = true;
	}

	return ret;
}

int LadderCircuit::ElemInSubcktSeries(LadderContext context, InsertionPoint *point)
{
	unsigned int i, status = SUBCKT_STATUS_NOTFOUND;

    if(isSeries) {
		for(i = 0; i < vectorSubckt.size(); i++) {
			if(vectorSubckt[i].elem != nullptr) {
				if(vectorSubckt[i].elem == point->subckt.elem) {
					point->point  = i;
					point->series = this;
					break;
				}
			} else if(point->subckt.elem == nullptr && vectorSubckt[i].subckt == point->subckt.subckt) {
				point->point  = i;
				point->series = this;
				break;
			} else {
				status = vectorSubckt[i].subckt->ElemInSubcktSeries(context, point);
				if(status != SUBCKT_STATUS_NOTFOUND) {
					if(point->series == nullptr) {
						point->point  = i;
						point->series = this;
					}
					break;
				}
            }
        }
		if(point->series != nullptr && status != SUBCKT_STATUS_INSIDE) {
			if(!i && (status != SUBCKT_STATUS_LAST) &&
				(point->subckt.elem != context.SelectedElem ? 1 : context.SelectedState == SELECTED_LEFT)) {
				status = SUBCKT_STATUS_FIRST;
			} else if(i == vectorSubckt.size() - 1 && (status != SUBCKT_STATUS_FIRST) &&
				(point->subckt.elem != context.SelectedElem ? 1 : context.SelectedState != SELECTED_LEFT)) {
				status = SUBCKT_STATUS_LAST;
			} else {
				status = SUBCKT_STATUS_INSIDE;
			}
		}
	} else {
		for(i=0; i < vectorSubckt.size(); i++) {
			if(vectorSubckt[i].elem != nullptr) {
				if(vectorSubckt[i].elem == point->subckt.elem) {
					status = SUBCKT_STATUS_FIRST;
			        break;
				}
			} else if(point->subckt.elem == nullptr && vectorSubckt[i].subckt == point->subckt.subckt) {
				status = SUBCKT_STATUS_FIRST;
				break;
			} else {
                status = vectorSubckt[i].subckt->ElemInSubcktSeries(context, point);
				if(point->series != nullptr) {
	                break;
				}
			}
		}
		if(i < vectorSubckt.size()) {
			if(status != SUBCKT_STATUS_INSIDE) {
				point->series   = nullptr;
				point->parallel = this;
			} else if(point->parallel == nullptr) {
				point->parallel = this;
			}
		}
	}

	return status;
}

int LadderCircuit::SearchMatch(LadderCircuit *series, int direction)
{
	unsigned int i, status = SUBCKT_STATUS_NOTFOUND;

	if(!isSeries) {
		for(i=0; i < vectorSubckt.size(); i++) {
			if(vectorSubckt[i].elem == nullptr && vectorSubckt[i].subckt->IsSeries()) {
				int index = (direction == SUBCKT_STATUS_FIRST ? 0 : vectorSubckt[i].subckt->getSize() - 1);
				Subckt s = vectorSubckt[i].subckt->getSubckt(index);

				if(series == vectorSubckt[i].subckt) {
					status = SUBCKT_STATUS_INSIDE;
				} else if(s.elem == nullptr && !s.subckt->IsSeries()) {
					status = s.subckt->SearchMatch(series, direction);
				}

				if(status == SUBCKT_STATUS_INSIDE) break;
			}
		}
	}

	return status;
}

bool LadderCircuit::AddElement(LadderElem *elem, LadderContext &context)
{
	int state;
	unsigned int i;
	Subckt node = { elem, this };

	// Se elemento a ser adicionado for nulo, retorna
	if(elem == nullptr) return false;

	if(vectorSubckt.empty()) {
		InsertSubckt(context, 0, node);

		return true;
	} else if(context.SelectedElem != nullptr) {
		for(i = 0; i < vectorSubckt.size(); i++) {
			if(context.SelectedElem == vectorSubckt[i].elem) {
				// Encontrada posicao de insercao
				if(vectorSubckt[i].elem->getWhich() == ELEM_PLACEHOLDER) {
					// Existe placeholder, removendo...
					DeleteSubckt(context, i);
					context.SelectedElem = nullptr;

					InsertSubckt(context, i, node);
				} else {
					// Agora criamos o subcircuito para insercao.
					state = context.SelectedState;
					if((isSeries && (state == SELECTED_LEFT || state == SELECTED_RIGHT)) ||
						(!isSeries && (state == SELECTED_ABOVE || state == SELECTED_BELOW))) {
							// Serie adicionando em serie ou Paralelo adicionando em paralelo.
							// Apenas adiciona um item a mais no vetor.

							// Se devemos inserir depois do elemento selecionado, avanca iterator.
							if(state == SELECTED_RIGHT || state == SELECTED_BELOW) i++;

							// Insere o elemento na posicao indicada
							InsertSubckt(context, i, node);
					} else {
							// Inserindo paralelamente em um serie ou serie em um paralelo.
							// Devemos criar o serie / paralelo e entao incluir o novo elemento como um item
							// do circuito criado
							LadderCircuit *new_subckt = new LadderCircuit(!isSeries);

							// Aqui carregamos ponteiros para os objetos que serao inseridos.
							node.elem   = elem;
							node.subckt = new_subckt;

							// Primeiro adicionamos o novo elemento e entao o subcircuito com os elementos existentes
							if(state == SELECTED_RIGHT || state == SELECTED_BELOW) {
								// Se devemos depois antes do elemento selecionado, inverte a ordem
								new_subckt->  MoveSubckt(context, new_subckt->getSize(), this, i);
								new_subckt->InsertSubckt(context, new_subckt->getSize(), node);
							} else {
								new_subckt->InsertSubckt(context, new_subckt->getSize(), node);
								new_subckt->  MoveSubckt(context, new_subckt->getSize(), this, i);
							}

							// Atualiza o item atual para o subcircuito criado
							node.elem   = nullptr;
							node.subckt = new_subckt;
							InsertSubckt(context, i, node);
					}
				}

				// Inserido com sucesso, retorna true!
				return true;
			}
		}
	}

	return false;
}

bool LadderCircuit::DelElement(LadderElem *elem, LadderContext &context)
{
	// Se elemento a ser removido for nulo, o circuito for vazio ou nao puder excluir, retorna false
	if(elem == nullptr || vectorSubckt.empty() || !context.canDelete) return false;

	unsigned int i;
	for(i = 0; i < vectorSubckt.size(); i++) {
		if(elem == vectorSubckt[i].elem) {
			// Encontrado elemento a ser removido
			DeleteSubckt(context, i);

			if(context.SelectedElem == elem) {
				context.SelectedElem = nullptr;
			}

			// Removido com sucesso, retorna true!
			return true;
		} else if(vectorSubckt[i].elem == nullptr && vectorSubckt[i].subckt->DelElement(elem, context)) {
			// Removido em subcircuito, retorna indicando sucesso na remocao!
			return true;
		}
	}

	return false;
}

Subckt LadderCircuit::getSubckt(unsigned int pos)
{
	Subckt ret = { nullptr, nullptr };

	if(pos < vectorSubckt.size()) {
		vector<Subckt>::iterator it = vectorSubckt.begin() + pos;
		ret = *it;
	}

	return ret;
}

void LadderCircuit::RemoveUnnecessarySubckts(LadderContext &context)
{
	unsigned int i;
	for(i = 0; i < vectorSubckt.size(); i++) {
		if(vectorSubckt[i].elem == nullptr) {
			vectorSubckt[i].subckt->RemoveUnnecessarySubckts(context);
			if(vectorSubckt[i].subckt->getSize() < 2) {
				if(vectorSubckt[i].subckt->getSize() == 1) {
					// Recupera o unico elemento do subcircuito
					Subckt s = vectorSubckt[i].subckt->getSubckt(0);

					// Adiciona o elemento ao circuito atual
					if(s.elem != nullptr || s.subckt->IsSeries() != isSeries) {
						// Se for um elemento ou um tipo diferente de circuito,
						// apenas move para o circuito atual na posicao do subcircuito
						MoveSubckt(context, i, vectorSubckt[i].subckt, 0);
						// Avanca i para corresponder a nova posicao do circuito vazio
						i++;
					} else { // Caso mais complicado: restou um circuito do mesmo tipo do nosso!
						while(s.subckt->getSize() > 0) {
							MoveSubckt(context, i++, s.subckt, 0);
						}
					}

					if(s.subckt == context.SelectedCircuit) {
						// Se o circuito selecionado estiver sendo excluido, devemos atualizar para o atual pois
						// indica que os elementos que estavam nele foram transferidos para o atual.
						context.SelectedCircuit = this;
					}
				} else if(vectorSubckt[i].subckt == context.SelectedCircuit) {
					// Se o circuito selecionado estiver sendo excluido, devemos atualizar para o atual pois
					// indica que os elementos que estavam nele foram transferidos para o atual.
					context.SelectedCircuit = this;
				}

				// Desaloca o subcircuito
				DeleteSubckt(context, i);
			}
		}
	}

	// Agora verifica se restou no circuito um placeholder no final e um elemento antes dele
	unsigned int size = vectorSubckt.size();
	if(isSeries && size > 1 && vectorSubckt[size-1].elem != nullptr &&
		vectorSubckt[size-1].elem->getWhich() == ELEM_PLACEHOLDER && vectorSubckt[size-2].elem != nullptr) {
			DelElement(vectorSubckt[size-1].elem, context);
	}
}

LadderCircuit *LadderCircuit::Clone(LadderDiagram *diagram)
{
	vector<Subckt>::size_type i;
	LadderCircuit *clone = new LadderCircuit(isSeries);

	for(i = 0; i < vectorSubckt.size(); i++) {
		Subckt s;
		if(vectorSubckt[i].elem != nullptr) {
			s.elem   = vectorSubckt[i].elem->Clone(diagram);
			s.subckt = this;
		} else {
			s.elem = nullptr;
			s.subckt = vectorSubckt[i].subckt->Clone(diagram);
		}

		// Simula ser Move para que nao registre a acao
		clone->InsertSubckt(getEmptyContext(), i, s, true);
	}

	return clone;
}

void LadderCircuit::doPostInsert(void)
{
	vector<Subckt>::size_type i;

	for(i = 0; i < vectorSubckt.size(); i++) {
		if(vectorSubckt[i].elem != nullptr) {
			vectorSubckt[i].elem->doPostInsert();
		} else {
			vectorSubckt[i].subckt->doPostInsert();
		}
	}
}

void LadderCircuit::doPostRemove(void)
{
	vector<Subckt>::size_type i;

	for(i = 0; i < vectorSubckt.size(); i++) {
		if(vectorSubckt[i].elem != nullptr) {
			vectorSubckt[i].elem->doPostRemove();
		} else {
			vectorSubckt[i].subckt->doPostRemove();
		}
	}
}

bool LadderCircuit::Save(FILE *f)
{
	bool ret = false;

	if(fwrite_bool(f, isSeries) && fwrite_uint(f, vectorSubckt.size())) {
		vector<Subckt>::iterator it;
		for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
			if(it->elem != nullptr) {
				// Salva flag indicando que este eh um elemento, id para criar o novo elemento
				// durante o carregamento e os dados em si.
				if(!fwrite_bool(f, true) || !fwrite_int(f, it->elem->getWhich()) ||
					!it->elem->Save(f)) {
						break; // erro durante a gravacao
				}
			} else {
				if(!fwrite_bool(f, false) || !it->subckt->Save(f)) {
					break; // erro durante a gravacao
				}
			}
		}

		if(it == vectorSubckt.end()) {
			ret = true; // gravacao finalizada com sucesso!
		}
	}

	return ret;
}

bool LadderCircuit::Load(LadderDiagram *diagram, FILE *f, unsigned int version)
{
	bool ret = false;
	unsigned int size;

	if(fread_bool(f, &isSeries) && fread_uint(f, &size)) {
		bool isElem;
		unsigned int i;

		for(i=0; i < size; i++) {
			if(fread_bool(f, &isElem)) {
				Subckt s;
				if(isElem) {
					int which;
					if(fread_int(f, &which)) {
						switch(which) {
						case ELEM_PLACEHOLDER           : s.elem = new LadderElemPlaceHolder  (diagram);        break;
						case ELEM_COMMENT               : s.elem = new LadderElemComment      (diagram);        break;
						case ELEM_CONTACTS              : s.elem = new LadderElemContact      (diagram);        break;
						case ELEM_COIL                  : s.elem = new LadderElemCoil         (diagram);        break;
						case ELEM_TOF                   :
						case ELEM_TON                   :
						case ELEM_RTO                   : s.elem = new LadderElemTimer        (diagram, which); break;
						case ELEM_RTC                   : s.elem = new LadderElemRTC          (diagram);        break;
						case ELEM_CTU                   :
						case ELEM_CTD                   :
						case ELEM_CTC                   : s.elem = new LadderElemCounter      (diagram, which); break;
						case ELEM_RES                   : s.elem = new LadderElemReset        (diagram);        break;
						case ELEM_ONE_SHOT_RISING       :
						case ELEM_ONE_SHOT_FALLING      : s.elem = new LadderElemOneShot      (diagram, which); break;
						case ELEM_GRT                   :
						case ELEM_GEQ                   :
						case ELEM_LES                   :
						case ELEM_LEQ                   :
						case ELEM_NEQ                   :
						case ELEM_EQU                   : s.elem = new LadderElemCmp          (diagram, which); break;
						case ELEM_MOD                   :
						case ELEM_ADD                   :
						case ELEM_SUB                   :
						case ELEM_MUL                   :
						case ELEM_DIV                   : s.elem = new LadderElemMath         (diagram, which); break;
						case ELEM_SQRT                  : s.elem = new LadderElemSqrt         (diagram);        break;
						case ELEM_RAND                  : s.elem = new LadderElemRand         (diagram);        break;
						case ELEM_ABS                   : s.elem = new LadderElemAbs          (diagram);        break;
						case ELEM_MOVE                  : s.elem = new LadderElemMove         (diagram);        break;
						case ELEM_OPEN                  :
						case ELEM_SHORT                 : s.elem = new LadderElemOpenShort    (diagram, which); break;
						case ELEM_SET_BIT               : s.elem = new LadderElemSetBit       (diagram);        break;
						case ELEM_CHECK_BIT             : s.elem = new LadderElemCheckBit     (diagram);        break;
						case ELEM_READ_ADC              : s.elem = new LadderElemReadAdc      (diagram);        break;
						case ELEM_SET_DA                : s.elem = new LadderElemSetDa        (diagram);        break;
						case ELEM_READ_ENC              : s.elem = new LadderElemReadEnc      (diagram);        break;
						case ELEM_RESET_ENC             : s.elem = new LadderElemResetEnc     (diagram);        break;
						case ELEM_MULTISET_DA           : s.elem = new LadderElemMultisetDA   (diagram);        break;
						case ELEM_READ_USS              :
						case ELEM_WRITE_USS             : s.elem = new LadderElemUSS          (diagram, which); break;
						case ELEM_READ_MODBUS           :
						case ELEM_WRITE_MODBUS          : s.elem = new LadderElemModBUS       (diagram, which); break;
						case ELEM_SET_PWM               : s.elem = new LadderElemSetPWM       (diagram);        break;
						case ELEM_UART_RECV             :
						case ELEM_UART_SEND             : s.elem = new LadderElemUART         (diagram, which); break;
						case ELEM_MASTER_RELAY          : s.elem = new LadderElemMasterRelay  (diagram);        break;
						case ELEM_SHIFT_REGISTER        : s.elem = new LadderElemShiftRegister(diagram);        break;
						case ELEM_LOOK_UP_TABLE         : s.elem = new LadderElemLUT          (diagram);        break;
						case ELEM_PIECEWISE_LINEAR      : s.elem = new LadderElemPiecewise    (diagram);        break;
						case ELEM_READ_FORMATTED_STRING :
						case ELEM_WRITE_FORMATTED_STRING: s.elem = new LadderElemFmtString    (diagram, which); break;
						case ELEM_READ_SERVO_YASKAWA    :
						case ELEM_WRITE_SERVO_YASKAWA   : s.elem = new LadderElemYaskawa      (diagram, which); break;
						case ELEM_PERSIST               : s.elem = new LadderElemPersist      (diagram);        break;
						case ELEM_PID                   : s.elem = new LadderElemPID          (diagram);        break;
						// Elemento nao suportado. Novo elemento nao cadastrado aqui???
						default                         : s.elem = nullptr; break;
						}

						if(s.elem == nullptr) { // Erro carregando elemento! Sai do loop...
							break;
						} else {
							s.elem->Load(f, version);
							s.subckt = this;
						}
					} else {
						break; // erro carregando elemento, interrompe carregamento
					}
				} else {
					s.elem   = nullptr;
					s.subckt = new LadderCircuit;
					if(s.subckt->Load(diagram, f, version) == false) break; // Se deu erro no carregamento, aborta...
				}

				vectorSubckt.push_back(s);
			} else {
				ret = false;
			}
		}

		if(i == size) {
			ret = true; // terminou de carregar os itens!
		}
	}

	return ret;
}

bool LadderCircuit::acceptIO(LadderElem *elem, unsigned long id, enum eType type)
{
	bool ret = true;
	vector<Subckt>::size_type it;

	for(it = 0; it < vectorSubckt.size(); it++) {
		if(vectorSubckt[it].elem != nullptr) {
			// Devemos ignorar o elemento passado como parametro pois, no caso de estarmos descartando seu I/O,
			// nao devemos verificar se ele aceita o novo tipo
			if(elem == vectorSubckt[it].elem) continue;

			ret = vectorSubckt[it].elem  ->acceptIO(id, type);
		} else {
			ret = vectorSubckt[it].subckt->acceptIO(elem, id, type);
		}

		if(ret == false) {
			break;
		}
	}

	return ret;
}

void LadderCircuit::updateIO(LadderDiagram *owner, bool isDiscard)
{
	vector<Subckt>::size_type it;

	for(it = 0; it < vectorSubckt.size(); it++) {
		if(vectorSubckt[it].elem != nullptr) {
			vectorSubckt[it].elem  ->updateIO(owner, isDiscard);
		} else {
			vectorSubckt[it].subckt->updateIO(owner, isDiscard);
		}
	}
}

void LadderCircuit::getAllowedTypes(LadderElem *elem, unsigned long id, vector<eType> *types)
{
	vector<Subckt>::size_type it;

	for(it = 0; it < vectorSubckt.size(); it++) {
		if(vectorSubckt[it].elem != nullptr) {
			// Devemos ignorar o elemento passado como parametro pois, no caso de estarmos descartando seu I/O,
			// o seu tipo nao devera ser considerado como uma das opcoes permitidas.
			if(elem == vectorSubckt[it].elem) continue;

			eType type = vectorSubckt[it].elem  ->getAllowedTypeIO(id);
			if((find(types->begin(), types->end(), type) == types->end()) && type != eType_Pending) {
				types->push_back(type);
			}
		} else {
			vectorSubckt[it].subckt->getAllowedTypes (elem, id, types);
		}
	}
}

int LadderCircuit::SearchAndReplace(LadderElem **refElem, unsigned long idSearch, string sNewText, eSearchAndReplaceMode mode)
{
	int matches = 0;
	// Sem o que buscar ou sem ponteiro para objeto de referencia, retorna erro!
	if(refElem == nullptr || idSearch == 0) return -1;

	vector<Subckt>::size_type it = 0;

	if(*refElem != nullptr) { // Procurando o objeto de referencia para a busca
		for(it = 0; it < vectorSubckt.size(); it++) {
			if(vectorSubckt[it].elem != nullptr) {
				if(vectorSubckt[it].elem == *refElem) { //Achou o elemento de referencia!
					*refElem = nullptr; // Limpa o ponteiro para indicar que nao estamos mais procurando
					it++; // Se este era o elemento de referencia, avancamos pois devemos iniciar a busca no proximo elemento
					break;
				}
			} else {
				matches = vectorSubckt[it].subckt->SearchAndReplace(refElem, idSearch, sNewText, mode);
				if(matches < 0) {
					// Ocorreu algum erro, retornando...
					return matches;
				}

				if(*refElem == nullptr) { // Elemento de referencia encontrado no subcircuito!
					it++; // Se encontrou o elemento de referencia, avancamos pois devemos iniciar a busca no proximo elemento
					break;
				}
			}
		}
	}

	if(*refElem == nullptr) { // Procurando o objeto de referencia para a busca
		// Nao inicializamos it. Ou ele sera 0 (inicializacao da variavel) ou sera o elemento
		for(; it < vectorSubckt.size(); it++) {
			// Se encontrou correspondencia e nao for para substituir todos, interrompe busca.
			if(matches > 0 && mode != eSearchAndReplaceMode_ReplaceAll) break;

			if(vectorSubckt[it].elem != nullptr) {
				bool isReplace = !(mode == eSearchAndReplaceMode_FindFirst || mode == eSearchAndReplaceMode_FindNext);
				matches += vectorSubckt[it].elem  ->SearchAndReplace(idSearch, sNewText, isReplace);
				if(matches > 0) { // Encontrou correspondencia nesse elemento!
					*refElem = vectorSubckt[it].elem;
				}
			} else {
				matches += vectorSubckt[it].subckt->SearchAndReplace(refElem, idSearch, sNewText, mode);
			}
		}
	}

	return matches;
}

bool LadderCircuit::DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	UndoRedoData *data = (UndoRedoData *)action.data;

	switch(action.action) {
	case eInsertSubckt: {
		if(isDiscard) {
			// Se estamos descartando uma acao que representava a inclusao de um elemento, devemos
			// desalocar a memoria ocupada pelo elemento se estiver descartando da lista de Refazer. Isso indica
			// que a operacao de inclusao foi desfeita e nao podera ser refeita. Se estiver descartando a lista
			// de Desfazer, significa que a inclusao foi feita e nao podera ser desfeita, ou seja, o objeto esta
			// no circuito e portanto nao deve ser desalocado.
			if(!IsUndo) {
				if(data->DeleteSubckt.subckt.elem != nullptr) {
					UnallocElem(data->DeleteSubckt.subckt.elem);
				} else if(data->DeleteSubckt.subckt.subckt != nullptr) { // Nunca subckt deveria ser nulo aqui!
					delete data->DeleteSubckt.subckt.subckt;
				}
			}
		} else {
			if(IsUndo) {
				DeleteSubckt(action.contextBefore, data->InsertSubckt.pos, false, true);
			} else {
				InsertSubckt(action.contextBefore, data->InsertSubckt.pos, data->InsertSubckt.subckt, false, true);
			}
		}
		break;
	}

	case eDeleteSubckt: {
		if(isDiscard) {
			// Se estamos descartando uma acao que representava a exclusao de um elemento, devemos
			// desalocar a memoria ocupada pelo elemento pois ele ja foi excluido do circuito.
			// Porem isso deve ser feito apenas se estiver descartando da lista de Desfazer, o que indica
			// que a operacao de exclusao nao podera ser desfeita. Se estiver descartando a lista de Refazer,
			// significa que a exclusao foi desfeita, ou seja, o objeto nao podera mais ser excluido e portanto
			// nao deve ser desalocado.
			if(IsUndo) {
				if(data->DeleteSubckt.subckt.elem != nullptr) {
					UnallocElem(data->DeleteSubckt.subckt.elem);
				} else if(data->DeleteSubckt.subckt.subckt != nullptr) { // Nunca subckt deveria ser nulo aqui!
					delete data->DeleteSubckt.subckt.subckt;
				}
			}
		} else {
			if(IsUndo) {
				InsertSubckt(action.contextBefore, data->DeleteSubckt.pos, data->DeleteSubckt.subckt, false, true);
			} else {
				DeleteSubckt(action.contextBefore, data->DeleteSubckt.pos, false, true);
			}
		}
		break;
	}

	case eMoveSubckt: {
		if(isDiscard) {
			// Nada a fazer...
		} else {
			if(IsUndo) {
				data->MoveSubckt.circuit->MoveSubckt(action.contextBefore, data->MoveSubckt.fromPos, this, data->MoveSubckt.pos, true);
			} else {
				MoveSubckt(action.contextBefore, data->MoveSubckt.pos, data->MoveSubckt.circuit, data->MoveSubckt.fromPos, true);
			}
		}
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

// Classe LadderDiagram
void LadderDiagram::Init(void)
{
	isLocked                       = false;
	isDialogShown                  = false;

	context.Diagram                = this;
	context.ParallelStart          = nullptr;
	context.SelectedElem           = nullptr;
	context.SelectedCircuit        = nullptr;

	context.SelectedState          = SELECTED_NONE;

	context.inSimulationMode       = false;
	context.isLoadingFile          = false;
	context.isDrawBlocked          = false;

	context.programChangedNotSaved = false;

	CheckPointLevels               =  0;
	CheckPointLevelsMax            = 30;
	CheckpointBeginCount           =  0;
	CheckpointDoRollback           = false;
	isCheckpointEmpty              = true;

	currentFilename                = "";

	NeedScrollSelectedIntoView     = false;

	// Inicializacao das configuracoes
	LadderSettings.General.canSave     = true; // Permite salvar o arquivo (sobreescrita)
	LadderSettings.General.cycleTime   = 10000; // Ciclo de 10 ms
	LadderSettings.General.mcuClock    = 100000000; // Frequencia da cpu: 100 MHz

	LadderSettings.Uart.UART           = 0; // Modo da UART: 8 bits de dados, sem paridade, 1 bit de parada
	LadderSettings.Uart.baudRate       = 9600; // Velocidade da UART: 9600 bps

	LadderSettings.Network.ip          = MAKEIPADDRESS(192, 168, 0, 254); // IP da POP-7: 192.168.0.254
	LadderSettings.Network.mask        = MAKEIPADDRESS(255, 255, 255, 0); // Mascara de rede: 255.255.255.0
	LadderSettings.Network.gw          = MAKEIPADDRESS(192, 168, 0,   1); // IP do Gateway: 192.168.0.1
	LadderSettings.Network.dns         = MAKEIPADDRESS(192, 168, 0,   1); // IP do servidor DNS: 192.168.0.1

	LadderSettings.Sntp.sntp_enable    = false; // Sincronismo com servidor de hora desativado por padrao
	LadderSettings.Sntp.dailysave      = false; // Horario de verao desativado por padrao
	LadderSettings.Sntp.sntp_server    = "br.pool.ntp.org"; // Usa servidor brasileiro de NTP.org
	LadderSettings.Sntp.gmt            = 9; // Fuso horario do Brasil: GMT-3, posicao 9 na lista

	LadderSettings.EncInc.conv_mode    = ENCODER_MODE_RAW; // Usa o valor lido do encoder, sem conversao
	LadderSettings.EncInc.factor       = 1.0; // Valor padrao do fator de correcao do encoder
	LadderSettings.EncInc.perimeter    = 400; // Normalmente usamos rodas com 400 mm de diametro
	LadderSettings.EncInc.pulses       = 1024; // Numero de pulsos por volta do encoder
	LadderSettings.EncInc.x4           = true; // Indica que devemos usar o modo x4 (ler as 4 bordas)

	LadderSettings.EncSSI.conv_mode    = ENCODER_MODE_RAW; // Usa o valor lido do encoder, sem conversao
	LadderSettings.EncSSI.factor       = 1.0; // Valor padrao do fator de correcao do encoder
	LadderSettings.EncSSI.perimeter    = 400; // Normalmente usamos rodas com 400 mm de diametro
	LadderSettings.EncSSI.mode         = 0; // Modo 0 = Leitura em Gray
	LadderSettings.EncSSI.size         = 24; // Numero de bits de resolucao do encoder
	LadderSettings.EncSSI.size_bpr     = 12; // Numero de bits que representam voltas

	LadderSettings.Adc.isCelsius       = true; // Modo padrao de leitura do A/D de temperatura: escala Celsius

	LadderSettings.Dac.ramp_abort_mode = RAMP_ABORT_LEAVE; // Modo padrao de abandono: mantem o valor atual

	LadderSettings.MbSlave.ModBUSID    = 0; // ID padrao do ModBUS escravo: 0

	LadderSettings.Info.Name           = "POPTools"; // Nome do Programa
	LadderSettings.Info.Developer      = ""; // Nome do Desenvolvedor
	LadderSettings.Info.Description    = ""; // Descricao do programa

	LadderSettings.Details.FWVersion      = "1.5"; // Versao do Firmware atual do POPTools
	LadderSettings.Details.BuildNumber    =  0; // Numero de compilacao atual
	LadderSettings.Details.CompileDate    =  0; // Data da ultima compilacao. Zero: nunca
	LadderSettings.Details.ProgramDate    =  0; // Data da ultima gravacao. Zero: nunca

	updateContext();
}

LadderDiagram::LadderDiagram(void)
{
	int i;

	IO = new mapIO(this);

	mcu = nullptr;
	for(i = 0; i < NUM_SUPPORTED_MCUS; i++) 
	{
		if(_stricmp(SupportedMcus[i].mcuName, DEFAULT_CPU)==0) 
		{
			mcu = &SupportedMcus[i];
			break;
		}
	}

	// Somente precisamos inicializar a variavel aqui e nao para cada vez que inicializarmos o diagrama
	// pois a propria funcao DrawTXT se encarregara de limpa-la ao terminar
	fncDrawChars = nullptr;

	Init();

	NewDiagram();
}

LadderDiagram::~LadderDiagram(void)
{
	FreeDiagram();

	delete IO;
}

void LadderDiagram::ClearDiagram(void)
{
	FreeDiagram();
	Init();
}

void LadderDiagram::FreeDiagram(void)
{
	unsigned int i;

	while(rungs.size() > 0) {
		DeleteRung(0, true);
	}

	while(UndoList.size() > 0) {
		DiscardCheckpoint(true);
	}

	while(RedoList.size() > 0) {
		DiscardCheckpoint(false);
	}

	ic.Clear();
	IO->Clear();

	for(i = 0; i < vectorMbNodeList.size(); i++) {
		delete vectorMbNodeList[i];
	}
	vectorMbNodeList.clear();
}

void LadderDiagram::Activate(bool isActive)
{
	if(isActive) {
		IO->updateGUI();
	}
}

void LadderDiagram::NewDiagram(void)
{
	ClearDiagram();

	NewRung(false);

	// As acoes executadas durante a inicializacao (criacao da linha e PlaceHolder) nao devem ser desfeitas
	// Porem as acoes registradas devem ser desalocadas. Assim executamos o discard para todas as acoes
	while(UndoList.size() > 0) {
		DiscardCheckpoint(true);
	}

	updateContext();

	// Limpa a flag que indica que o arquivo precisa ser salvo.
	context.programChangedNotSaved = false;
}

vector<IntOp> LadderDiagram::getVectorIntCode(void)
{
	return ic.getVectorIntCode();
}

bool LadderDiagram::GenerateIntCode(void)
{
	bool ret = true;
	vector<LadderCircuit>::size_type i;

	ic.Clear();
	ic.setStateInOut("$rung_top");
    ic.Op(INT_SET_BIT, "$mcr");

	for(i = 0; i < rungs.size() && ret; i++) {
		if(rungs[i]->rung->IsComment()) continue;

		ic.Comment("");
        ic.Comment(_("iniciando linha %d"), i+1);
		ic.ClearParallelCount();
        ic.Op(INT_COPY_BIT_TO_BIT, "$rung_top", "$mcr");
		ic.SimState(&rungs[i]->isPowered, "$rung_top");

		ret = rungs[i]->rung->GenerateIntCode(ic);
	}

	return ret;
}

void LadderDiagram::SelectElement(LadderElem *elem, int state)
{
	if(elem == nullptr) return;

	// Comentarios somente devem ter selecao para cima ou para baixo
	if(elem->IsComment() && (state == SELECTED_LEFT || state == SELECTED_RIGHT)) {
		state = SELECTED_BELOW;
	}

	// Objetos de fim de linha nunca devem ter sua selecao marcada no lado direito
	if(elem->IsEOL() && state == SELECTED_RIGHT) {
		state = SELECTED_BELOW;
	}

	if(elem != context.SelectedElem) {
		context.SelectedElem    = elem;
		context.SelectedCircuit = getSubcktForElement(elem);
	}

	context.SelectedState = state;

	if(!IsSelectedVisible()) {
		NeedScrollSelectedIntoView = true;
	}

	updateContext();
}

bool LadderDiagram::EditSelectedElement(void)
{
	bool ret = false;

	if(context.SelectedElem != nullptr && !IsLocked()) {
		CheckpointBegin(_("Editar Elemento"));
		ret = context.SelectedElem->ShowDialog(context);
		if(ret) {
			ProgramChanged();
		}
		CheckpointEnd();
		updateContext();
	}

	return ret;
}

LadderCircuit *LadderDiagram::getSubcktForElement(LadderElem *elem)
{
	LadderCircuit *subckt = nullptr;
	vector<LadderRung *>::size_type i;

	for(i = 0; i < rungs.size() && subckt == nullptr; i++) {
		subckt = rungs[i]->rung->getSubcktForElement(elem);
	}

	return subckt;
}

LadderCircuit *LadderDiagram::getParentSubckt(LadderCircuit *subckt)
{
	LadderCircuit *parent = nullptr;
	vector<LadderCircuit>::size_type i;

	if(subckt != nullptr) {
		for(i = 0; i < rungs.size(); i++) {
			if(rungs[i]->rung == subckt) {
				break;
			} else {
				parent = rungs[i]->rung->getParentSubckt(subckt);
				if(parent != nullptr) {
					break;
				}
			}
		}
	}

	return parent;
}

void LadderDiagram::MoveCursor(eMoveCursor moveTo)
{
	LadderElem *elem = nullptr;

	switch(moveTo) {
	case eMoveCursor_DiagramHome:
		elem = rungs[0]->rung->getFirstElement();
		break;

	case eMoveCursor_RungHome: {
		int index = this->RungContainingSelected();
		if(index >= 0) {
			elem = rungs[index]->rung->getFirstElement();
		}
		break;
		}

	case eMoveCursor_DiagramEnd:
		elem = rungs[rungs.size() - 1]->rung->getLastElement();
		break;

	case eMoveCursor_RungEnd: {
		int index = this->RungContainingSelected();
		if(index >= 0) {
			elem = rungs[index]->rung->getLastElement();
		}
		break;
		}

	case eMoveCursor_Up:
		if(context.SelectedState != SELECTED_ABOVE) {
			elem = context.SelectedElem;
		} else {
			elem = SearchElement(moveTo);
			// se encontrar um elemento, deve ser selecionado na extremidade inferior
			moveTo = eMoveCursor_Down;
		}
		break;

	case eMoveCursor_Down:
		if(context.SelectedState != SELECTED_BELOW) {
			elem = context.SelectedElem;
		} else {
			elem = SearchElement(moveTo);
			// se encontrar um elemento, deve ser selecionado na extremidade superior
			moveTo = eMoveCursor_Up;
		}
		break;

	case eMoveCursor_Left:
		if(context.SelectedState != SELECTED_LEFT) {
			elem = context.SelectedElem;
		} else {
			elem = SearchElement(moveTo);
			// se encontrar um elemento, deve ser selecionado na extremidade direita
			moveTo = eMoveCursor_Right;
		}
		break;

	case eMoveCursor_Right:
		if(context.SelectedState != SELECTED_RIGHT) {
			elem = context.SelectedElem;
		} else {
			elem = SearchElement(moveTo);
			// se encontrar um elemento, deve ser selecionado na extremidade esquerda
			moveTo = eMoveCursor_Left;
		}
		break;

	default: elem = SearchElement(moveTo);
	}

	if(elem != nullptr && getSubcktForElement(elem) != nullptr) {
		int state = elem->IsEOL() ? SELECTED_LEFT : SELECTED_RIGHT;

		switch(moveTo) {
			case eMoveCursor_Up   : state = SELECTED_ABOVE; break;
			case eMoveCursor_Down : state = SELECTED_BELOW; break;
			case eMoveCursor_Left : state = SELECTED_LEFT ; break;
			case eMoveCursor_Right: state = SELECTED_RIGHT; break;
		}

		SelectElement(elem, state);

		updateContext();
	}
}

void LadderDiagram::updateUndoContextAfter(bool forceNotNull)
{
	deque<UndoRedoAction>::iterator it;

	if(UndoList.size() == 0) return; // Lista vazia...

	it = UndoList.end();
	do{
		it--;

		if(it->contextAfter.Diagram == nullptr || forceNotNull) {
			it->contextAfter = context;
			// Se estiver no modo para forcar atualizacao,
			// deve sair no primeiro checkpoint encontrado.
			if(forceNotNull && it->action == eCheckpoint) {
				break;
			}
		} else {
			break;
		}
	} while (it != UndoList.begin());
}

void LadderDiagram::updateContext(void)
{
	if(IsLocked()) {
		context.canNegate        = false;
		context.canNormal        = false;
		context.canSetOnly       = false;
		context.canResetOnly     = false;
		context.canPushUp        = false;
		context.canPushDown      = false;
		context.canDelete        = false;
		context.canInsertEnd     = false;
		context.canInsertOther   = false;
		context.canInsertComment = false;
	} else {
		context.canNegate        = false;
		context.canNormal        = false;
		context.canSetOnly       = false;
		context.canResetOnly     = false;
		context.canPushUp        = true;
		context.canPushDown      = true;
		context.canDelete        = true;
		context.canInsertEnd     = false;
		context.canInsertOther   = true;
		context.canInsertComment = false;

		int i = RungContainingSelected();
		if(i >= 0) {
			if(i == 0) context.canPushUp = false;
			if(i == rungs.size() - 1) context.canPushDown = false;

			if(context.SelectedCircuit->IsEmpty())
			{
				context.canDelete = false;
			}
		}

		if(context.SelectedElem == nullptr) {
			context.canDelete      = false;
			context.canInsertEnd   = false;
			context.canInsertOther = false;
		} else {
			if(context.SelectedElem->IsEOL()) {
				if(context.SelectedElem->getWhich() == ELEM_COIL) {
					context.canNegate    = true;
					context.canNormal    = true;
					context.canResetOnly = true;
					context.canSetOnly   = true;
				}

				if(context.SelectedState == SELECTED_ABOVE ||
				   context.SelectedState == SELECTED_BELOW)
				{
					context.canInsertEnd   = true;
					context.canInsertOther = false;
				} else if(context.SelectedState == SELECTED_RIGHT) {
					context.canInsertEnd   = false;
					context.canInsertOther = false;
				}
			} else {
				if(context.SelectedElem->getWhich() == ELEM_PLACEHOLDER &&
					context.SelectedElem != context.ParallelStart) {
						context.canDelete = false;
				}

				if(context.SelectedState == SELECTED_RIGHT || 
					context.SelectedElem->getWhich() == ELEM_PLACEHOLDER) {
						if(i >= 0) {
//							vector<LadderRung *>::iterator it = rungs.begin() + i;
//							context.canInsertEnd = (*it)->rung->IsLast(context.SelectedElem);
							// context.canInsertEnd = context.SelectedCircuit->IsLast(context.SelectedElem);
							bool canInsertEnd     = true;
							LadderElem    *elem   = context.SelectedElem;
							LadderCircuit *subckt = context.SelectedCircuit;
							LadderCircuit *rung   = rungs[i]->rung;
							Subckt         next   = { elem, subckt };

							do {
								next = subckt->getNext(next);
								if(next.elem == nullptr || !subckt->IsSeries()) {
									next.elem   = nullptr;
									next.subckt = subckt;
									subckt      = rung->getParentSubckt(next.subckt);
								} else {
									canInsertEnd = false;
									break;
								}
							} while(subckt != nullptr);

							context.canInsertEnd = canInsertEnd;
						} else {
							context.canInsertEnd = false;
						}
				}

				if(context.SelectedElem->getWhich() == ELEM_CONTACTS) {
					context.canNegate = true;
					context.canNormal = true;
				}
			}

			if(context.SelectedCircuit->IsEmpty() || context.SelectedElem->IsComment() ||
				context.SelectedState == SELECTED_BELOW || context.SelectedState == SELECTED_ABOVE) {
					// a comment must be the only element in its rung, and it will fill
					// the rung entirely
					// Se o elemento selecionado for um comentario, permite a insercao tambem.
					// Nesse caso, ao se inserir o novo comentario, uma nova linha sera adicio-
					// nada abaixo da atual e o comentario sera adicionado na nova linha.
					context.canInsertComment = true;
			}

			// Se o elemento atualmente selecionado for um comentario, permite adicionar qualquer objeto pois
			// nesse caso adicionaremos uma linha automaticamente.
			if(context.SelectedElem->IsComment()) {
				context.canInsertEnd   = true;
				context.canInsertOther = true;
			}
		}
	}

	SetMenusEnabled(&context);

	updateUndoContextAfter();
}

void LadderDiagram::SetLock(bool state)
{
	isLocked = state;
	updateContext();
}

void LadderDiagram::SetDialogShown(bool state)
{
	isDialogShown = state;
}

int LadderDiagram::getWidthTXT(void)
{
	int thisWidth, width = 0;
	vector<LadderCircuit>::size_type i;

	for(i = 0; i < rungs.size(); i++) {
		thisWidth = rungs[i]->rung->getWidthTXT(0);
		if(thisWidth > width) {
			width = thisWidth;
		}
	}

	return width;
}

int LadderDiagram::getHeightTXT(void)
{
	int height = 0;
	vector<LadderCircuit>::size_type i;

	for(i = 0; i < rungs.size(); i++) {
		height += rungs[i]->rung->getHeightTXT() + 1;
	}

	return height;
}

int  LadderDiagram::RungContainingElement(LadderElem *elem)
{
	vector<LadderCircuit>::size_type i;

	if(elem != nullptr) {
		for(i = 0; i < rungs.size(); i++) {
			if(rungs[i]->rung->getSubcktForElement(elem) != nullptr)
				return i;
		}
	}

	return -1;
}

int LadderDiagram::RungContainingSelected(void)
{
	return RungContainingElement(context.SelectedElem);
}

bool LadderDiagram::IsRungEmpty(unsigned int rung)
{
	return (rung < rungs.size()) ? rungs[rung]->rung->IsEmpty() : true;
}

bool LadderDiagram::NewRung(bool isAfter)
{
	// Se diagrama bloqueado, retorna
	if(IsLocked()) return false;

	vector<LadderRung *>::iterator it;
	int position = RungContainingSelected();

	CheckpointBegin(_("Inserir Linha"));

	// Registro da acao para desfazer / refazer
	UndoRedoAction action;
	UndoRedoData *data = new UndoRedoData;

	data->NewRung.rung                 = new LadderRung;

	data->NewRung.rung->hasBreakpoint  = false;
	data->NewRung.rung->isPowered      = false;
	data->NewRung.rung->isBeingDeleted = false;
	data->NewRung.rung->rung           = new LadderCircuit(true);

	if(position < 0) { // Se posicao menor que zero, insere no final
		it = rungs.end();
		data->NewRung.pos = rungs.size();
	} else {
		data->NewRung.pos = min((unsigned int)position + (isAfter ? 1 : 0), rungs.size());
		it = rungs.begin() + data->NewRung.pos;
	}

	action.action        = eNewRung;
	action.contextAfter  = getEmptyContext();
	action.contextBefore = context;
	action.data          = data;
	action.io            = nullptr;
	action.elem          = nullptr;
	action.subckt        = nullptr;

	RegisterAction(action);

	// Adiciona a linha na posicao calculada
	it = rungs.insert(it, data->NewRung.rung);

	// Adiciona o padding
	LadderElem *elem = new LadderElemPlaceHolder(this);
	(*it)->rung->AddElement(elem, context);

	SelectElement(elem, SELECTED_RIGHT);

	CheckpointEnd();

	// Indica que houve alteracao no programa
	ProgramChanged();

	return true;
}

bool LadderDiagram::PushRung(int rung, bool up)
{
	int QtdRungs = rungs.size();

	// Se diagrama bloqueado, retorna
	if(IsLocked()) return false;

	if(rung < 0) {
		rung = RungContainingSelected();
	}

	if(rung < 0) return false;

	if(QtdRungs == 1                  || // Existe apenas 1 linha! Impossivel movimenta-la
		(rung >= QtdRungs)            || // Linha inexistente!
		(rung == (QtdRungs-1) && !up) || // Tentando mover ultima linha para baixo...
		(rung == 0 && up)) {             // Tentando mover primeira linha para cima...
			return false; // Impossivel executar a operacao solicitada
	}

	int offset;
	if(up) {
		offset = -1;
	} else {
		offset = +1;
	}

	LadderRung *tmp      = rungs[rung];
	rungs[rung]          = rungs[rung + offset];
	rungs[rung + offset] = tmp;

	// Sempre forca redesenhar a tela pois nesse momento ainda nao sabemos se isso sera necessario ou nao...
	NeedScrollSelectedIntoView = true;

	// Registro da acao para desfazer / refazer
	UndoRedoAction action;
	UndoRedoData *data = new UndoRedoData;

	data->PushRung.pos  = rung;
	data->PushRung.isUp = up;

	action.action        = ePushRung;
	action.contextAfter  = getEmptyContext();
	action.contextBefore = context;
	action.data          = data;
	action.io            = nullptr;
	action.elem          = nullptr;
	action.subckt        = nullptr;

	CheckpointBegin(_("Mover Linha"));
	RegisterAction(action);
	CheckpointEnd();

	// Indica que houve alteracao no programa
	ProgramChanged();

	// Atualiza o contexto
	updateContext();

	return true;
}

bool LadderDiagram::DeleteRung(int rung, bool isFreeDiagram)
{
	// Se diagrama bloqueado, retorna
	if(IsLocked()) return false;

	if(rung < 0) {
		rung = RungContainingSelected();
	}

	if(rung < 0 || (!isFreeDiagram && rungs.size() == 1 && rungs[0]->rung->IsEmpty())) return false;

	CheckpointBegin(_("Remover Linha"));

	vector<LadderRung *>::iterator it = rungs.begin() + rung;

	// Registro da acao para desfazer / refazer
	UndoRedoAction action;
	UndoRedoData *data = new UndoRedoData;

	data->DelRung.pos  = rung;
	data->DelRung.rung = *it;

	action.action        = eDelRung;
	action.contextAfter  = getEmptyContext();
	action.contextBefore = context;
	action.data          = data;
	action.io            = nullptr;
	action.elem          = nullptr;
	action.subckt        = nullptr;

	RegisterAction(action);

	if(rung == RungContainingSelected()) {
		context.SelectedElem    = nullptr;

		if(rung < (int)rungs.size() - 1) {
			context.SelectedElem = rungs[rung + 1]->rung->getFirstElement();
		} else if(rung > 0) {
			context.SelectedElem = rungs[rung - 1]->rung->getFirstElement();
		}

		context.SelectedCircuit = getSubcktForElement(context.SelectedElem);
	}

	if(rung == RungContainingElement(context.ParallelStart)) {
		context.ParallelStart = nullptr;
	}

	(*it)->isBeingDeleted = true;
	(*it)->rung->doPostRemove();
	(*it)->rung->updateIO(this, true);
	rungs.erase(it);

	if(!isFreeDiagram && rungs.size() == 0) {
		NewRung(false);
	}

	CheckpointEnd();

	// Indica que houve alteracao no programa
	ProgramChanged();

	updateContext();

	return true;
}

bool LadderDiagram::CopyRung(LadderClipboard *clipboard, LadderElem *elem)
{
	// Se nao recebemos a estrutura que contem as informacoes de copiar/colar, retorna.
	if(clipboard == nullptr) return false;

	int pos;
	if(elem == nullptr) {
		elem = context.SelectedElem;
		if(elem == nullptr) return false; // Sem linha para copiar
	}

	// Durante esta operacao, nao devemos desenhar a tela para evitar a lista de I/Os sendo recarregada
	context.isDrawBlocked = true;

	pos = RungContainingElement(elem);

	// Primeiro descarta uma linha previamente copiada
	if(clipboard->rungCopy != nullptr) {
		delete clipboard->rungCopy->rung;
		delete clipboard->rungCopy;
	}

	// Copia a linha
	clipboard->rungCopy                 = new LadderRung;
	clipboard->rungOwner                = this;

	clipboard->rungCopy->hasBreakpoint  = false;
	clipboard->rungCopy->isPowered      = false;
	clipboard->rungCopy->isBeingDeleted = false;
	clipboard->rungCopy->rung           = rungs[pos]->rung->Clone(this);

	// Operacao finalizada, a tela ja pode ser desbloqueada
	context.isDrawBlocked = false;

	// Retorna com sucesso!
	return true;
}

bool LadderDiagram::PasteRung(LadderClipboard clipboard, bool isAfter)
{
	int pos;
	bool ret = false;

	// Se diagrama bloqueado, retorna
	if(IsLocked()) return false;

	// Nao existe linha copiada ou nao existe ponto de insercao definido
	if(clipboard.rungCopy == nullptr || context.SelectedElem == nullptr) return false;

	// Durante esta operacao, nao devemos desenhar a tela para evitar a lista de I/Os sendo recarregada
	context.isDrawBlocked = true;

	CheckpointBegin(_("Colar Linha"));

	pos = RungContainingSelected();

	if(pos >= 0) {
		if(isAfter) {
			pos++;
		}

		// Cria uma copia de si mesma para que seja possivel inserir uma nova copia
		LadderRung *copiedRung     = new LadderRung;

		copiedRung->hasBreakpoint  = false;
		copiedRung->isPowered      = false;
		copiedRung->isBeingDeleted = false;
		copiedRung->rung           = clipboard.rungCopy->rung->Clone(this);

		rungs.insert(rungs.begin() + pos, copiedRung);
		copiedRung->rung->doPostInsert();
		copiedRung->rung->updateIO(clipboard.rungOwner, false);

		// Registro da acao para desfazer / refazer
		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->NewRung.rung = copiedRung;
		data->NewRung.pos  = pos;

		action.action        = eNewRung;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = context;
		action.data          = data;
		action.io            = nullptr;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		RegisterAction(action);

		// Indica que houve alteracao no programa
		ProgramChanged();

		ret = true;
	} else {
		CheckpointRollback();
	}

	CheckpointEnd();

	updateContext();

	// Operacao finalizada, a tela ja pode ser desbloqueada
	context.isDrawBlocked = false;

	return ret;
}

bool LadderDiagram::AddElement(LadderElem *elem)
{
	// Se diagrama bloqueado, retorna
	if(IsLocked()) return false;

	// Se elemento a ser adicionado for nulo, retorna
	if(elem == nullptr) return false;
	// Se nao existe um elemento selecionado, descarta o elemento que nos foi passado e retorna
	if(context.SelectedElem == nullptr || context.SelectedCircuit == nullptr) {
		// Se o elemento for descartado, deve descartar todos os I/Os que ele houver registrado.
		elem->updateIO(this, true);

		// Apos o recebimento de elem, a classe LadderDiagram passa a ser responsavel por ele.
		// Se por qualquer motivo houver uma falha ao inserir, a classe tem a responsabilidade
		// de desalocar este elemento.
		UnallocElem(elem);

		return false;
	}

	CheckpointBegin(_("Adicionar Elemento"));

	// Se estiver adicionando um comentario e a linha nao estiver vazia,
	// deve ser adicionada uma nova linha
	if((elem->IsComment() && !rungs[RungContainingSelected()]->rung->IsEmpty()) ||
		context.SelectedElem->IsComment()){
			NewRung((context.SelectedState == SELECTED_ABOVE) ? false : true);
	}

	bool ret = elem->CanInsert(context);
	if(ret) {
		if(context.ParallelStart == nullptr) {
			ret = context.SelectedCircuit->AddElement(elem, context);
		} else {
			ret = InsertParallel(elem);
		}
	}

	// Se adicionado com sucesso, atualiza o contexto
	if(ret == true) {
		// Indica que houve alteracao no programa
		ProgramChanged();

		int position = RungContainingSelected();
		if(position >= 0) { // Se posicao menor que zero, insere no final
			vector<LadderRung *>::iterator it = rungs.begin() + position;
			(*it)->rung->AddPlaceHolderIfNoEOL(context, elem);
		}

		// Elemento adicionado, chama a funcao para que o elemento realize qualquer etapa
		// adicional apos a sua inclusao no circuito.
		// Especificamente para o caso do ModBUS, que precisa referenciar o node que ele usa.
		elem->doPostInsert();

		SelectElement(elem, elem->IsEOL() ?
			(context.SelectedState == SELECTED_BELOW ? SELECTED_BELOW : SELECTED_LEFT) :
			SELECTED_RIGHT);

		updateContext();
	} else {
		// Ocorreu um erro! Marca o checkpoint para remocao
		CheckpointRollback();

		// Apos o recebimento de elem, a classe LadderDiagram passa a ser responsavel por ele.
		// Se por qualquer motivo houver uma falha ao inserir, a classe tem a responsabilidade
		// de desalocar este elemento.
		UnallocElem(elem);
	}

	CheckpointEnd();

	return ret;
}

bool LadderDiagram::DelElement(LadderElem *elem)
{
	// Se diagrama bloqueado, retorna
	if(IsLocked()) return false;

	if(elem == nullptr) {
		if(context.SelectedElem == nullptr) return false; // Sem elemento para remover
		elem = context.SelectedElem;
	}

	int rung = RungContainingElement(elem);

	CheckpointBegin(_("Remover Elemento"));
	if(rungs[rung]->rung->IsComment()) {
		bool ret = DeleteRung(rung);
		CheckpointEnd();
		return ret;
	} else {
		// Seleciona o elemento a ser excluido e movimenta o cursor para descobrir para onde devemos deslocar o cursor apos a exclusao
		SelectElement(elem, SELECTED_LEFT);
		MoveCursor(eMoveCursor_Any); // busca o elemento mais proximo em qualquer direcao

		// Salva o ponteiro para o novo elemento e posicao do cursor
		LadderElem *newElem = context.SelectedElem;
		int newState = context.SelectedState;
		if(newElem == elem) { // novo eh o mesmo que o que esta sendo excluido! desconsidera...
			newElem = nullptr;
		}

		// Ja sabemos para onde movimentar, seleciona o elemento original novamente
		// Seleciona em qualquer sentido pois sera excluido de qualquer forma
		SelectElement(elem, SELECTED_LEFT);

		if(rungs[rung]->rung->DelElement(elem, context)) {
			// Elemento removido, chama a funcao para que o elemento realize qualquer etapa
			// adicional apos a sua exclusao no circuito.
			// Especificamente para o caso do ModBUS, que precisa referenciar o node que ele usa.
			elem->doPostRemove();

			elem->updateIO(this, true);
			rungs[rung]->rung->RemoveUnnecessarySubckts(context);
			rungs[rung]->rung->AddPlaceHolderIfNoEOL(context, nullptr);

			if(elem == context.ParallelStart) {
				context.ParallelStart = nullptr;
			}

			if(newElem != nullptr) {
				SelectElement(newElem, newState);
			}

			// Se nao houver elemento selecionado, seleciona o primeiro elemento do circuito
			if(context.SelectedElem == nullptr) {
				elem = rungs[rung]->rung->getFirstElement();
				SelectElement(elem, elem->IsEOL() ? SELECTED_LEFT : SELECTED_RIGHT);
			}

			CheckpointEnd();

			// Indica que houve alteracao no programa
			ProgramChanged();

			updateContext();

			return true;
		}
	}

	CheckpointRollback();
	CheckpointEnd();

	return false;
}

bool LadderDiagram::CopyElement(LadderClipboard *clipboard, LadderElem *elem)
{
	// Se nao recebemos a estrutura que contem as informacoes de copiar/colar, retorna.
	if(clipboard == nullptr) return false;

	if(elem == nullptr) {
		elem = context.SelectedElem;
		if(elem == nullptr) return false; // Sem elemento para copiar
	}

	if(elem->getWhich() == ELEM_PLACEHOLDER) return false; // Nao pode copiar PlaceHolder...

	// Durante esta operacao, nao devemos desenhar a tela para evitar a lista de I/Os sendo recarregada
	context.isDrawBlocked = true;

	// As acoes de copia do objeto nao devem ser registradas pois o CTRL+Z nao desfaz a copia.
	// Assim precisamos criar um checkpoint para que possamos descartar as acoes inscritas
	CheckpointBegin(_("Copiar Elemento"));

	// Primeiro descarta um elemento previamente copiado
	if(clipboard->elemCopy != nullptr) {
		UnallocElem(clipboard->elemCopy);
	}

	// Copia o elemento
	clipboard->elemCopy  = elem->Clone(this);
	clipboard->elemOwner = this;

	// Copia finalizada. Executa o rollback e fecha o checkpoint e assim todas as acoes registradas
	// desde a criacao do checkpoint serao descartadas
	CheckpointRollback();
	CheckpointEnd();

	// Operacao finalizada, a tela ja pode ser desbloqueada
	context.isDrawBlocked = false;

	// Retorna com sucesso!
	return true;
}

bool LadderDiagram::PasteElement(LadderClipboard clipboard)
{
	bool ret = false;

	// Se diagrama bloqueado, retorna
	if(IsLocked()) return false;

	if(clipboard.elemCopy == nullptr) return false; // Nao existe elemento copiado

	// Durante esta operacao, nao devemos desenhar a tela para evitar a lista de I/Os sendo recarregada
	context.isDrawBlocked = true;

	CheckpointBegin(_("Colar Elemento"));

	LadderElem *newCopiedElement = clipboard.elemCopy->Clone(this);
	newCopiedElement->setDiagram(this);

	if(AddElement(newCopiedElement)) {
		newCopiedElement->updateIO(clipboard.elemOwner, false);
		ret = true;

		// Indica que houve alteracao no programa
		ProgramChanged();
	} else {
		CheckpointRollback();
	}

	CheckpointEnd();

	updateContext();

	// Operacao finalizada, a tela ja pode ser desbloqueada
	context.isDrawBlocked = false;

	return ret;
}

void LadderDiagram::DrawTXT(tFncDrawChars fnc)
{
    int i;
	const int OffsetX = 6;
	int cx = OffsetX, cy = 0;
	int ColsAvailable = getWidthTXT();
	int RowsAvailable = getHeightTXT();

	// Verifica se recebemos um ponteiro para funcao de desenhar caracteres
	if(fnc == nullptr) return;

	fncDrawChars = fnc;

	vector< vector<int> > DisplayMatrix(ColsAvailable, vector<int>(RowsAvailable, 0));

	vector<LadderCircuit>::size_type it;

	for(it = 0; it < rungs.size(); it++) {
		char ch[4] = { ' ', ' ', ' ', 0 };
        if(it < 10) {
            ch[2] = it + '0';
		} else if(it < 100) {
            ch[1] = (it / 10) + '0';
			ch[2] = (it % 10) + '0';
        } else {
			_itoa(it, ch, 10);
        }
		fncDrawChars(0, cy, ch);

		rungs[it]->rung->DrawTXT(fncDrawChars, DisplayMatrix, &cx, &cy, true, ColsAvailable);

		cx  = OffsetX;
		cy += (rungs[it]->rung->getHeightTXT() + 1) * POS_HEIGHT;
	}

	// Desenha a linha final
    const char *str = _("[END]");
    int lead = (ColsAvailable*POS_WIDTH - strlen(str))/2;
    for(i = 0; i < lead; i++) {
        fncDrawChars(cx + i, cy + (POS_HEIGHT/2), "-");
    }
    fncDrawChars(cx + i, cy + (POS_HEIGHT/2), str);
    i += strlen(str);
    for(; i < ColsAvailable*POS_WIDTH; i++) {
        fncDrawChars(cx + i, cy + (POS_HEIGHT/2), "-");
    }

	fncDrawChars = nullptr;
}

bool LadderDiagram::AddParallelStart(void)
{
	bool ret = false;

	// Se diagrama bloqueado, retorna
	if(IsLocked()) return false;

	if(context.ParallelStart == nullptr) {
		if(context.SelectedElem != nullptr) {
			switch(context.SelectedState) {
			case SELECTED_RIGHT: {
				LadderElem    *elem   = context.SelectedElem;
				LadderCircuit *subckt = context.SelectedCircuit;
				LadderCircuit *rung   = rungs[RungContainingElement(elem)]->rung;
				Subckt         next   = { elem, subckt };

				do {
					next = subckt->getNext(next);
					if(next.elem == nullptr) {
						next.elem   = nullptr;
						next.subckt = subckt;
						subckt      = rung->getParentSubckt(next.subckt);
					} else {
						elem = next.elem;
						break;
					}
				} while(subckt != nullptr);

				SelectElement(elem, SELECTED_BELOW);
			}

			case SELECTED_NONE:
			case SELECTED_LEFT:
				context.SelectedState = SELECTED_BELOW;
			}
		}

		LadderElem *ps = new LadderElemPlaceHolder(this);
		ret = AddElement(ps);

		if(ret == true) {
			context.ParallelStart = ps;
			updateContext();
			updateUndoContextAfter(true);
		}
	}

	return ret;
}

// Tries to insert a parallel subcircuit between context.ParallelStart and currently selected object
bool LadderDiagram::InsertParallel(LadderElem *elem)
{
	bool ret = false;
	unsigned int i, CurrentRung, UseCurrentParallel = 1;
	InsertionPoint StartPoint = { 0, { context.ParallelStart, nullptr }, nullptr, nullptr};
	InsertionPoint EndPoint   = { 0, { context.SelectedElem , nullptr }, nullptr, nullptr};

	// Phase 1: check if context.ParallelStart and currently selected object are in the same subcircuit.
	for(i=0; i < rungs.size(); i++) {
		rungs[i]->rung->ElemInSubcktSeries(context, &StartPoint);
		if(StartPoint.series != nullptr) {
			// Start found, now we will search for End in the same rung and stop the search.
			rungs[i]->rung->ElemInSubcktSeries(context, &EndPoint);
			break;
		}
	}

	CurrentRung = i;

	// Phase 2: check if we can make a parallel between two selected points.
	if(StartPoint.series != nullptr && EndPoint.series != nullptr && StartPoint.series != EndPoint.series) {
		UseCurrentParallel = 0;
		LadderElem *EndElem = nullptr;

		if(EndPoint.parallel != nullptr && EndPoint.parallel->getSize() == 2) {
			EndElem = EndPoint.parallel->getSubckt(1).elem;
		}

		if(StartPoint.parallel == EndPoint.parallel) {
			StartPoint.point  = 0;
			StartPoint.series = EndPoint.series;
		} else if(EndElem != nullptr && EndElem->getWhich() == ELEM_PLACEHOLDER) { // Special condition
			StartPoint.point  = 0;
			StartPoint.series = EndPoint.series;
		} else {
			Subckt End, Previous = { nullptr, nullptr }, Next = { nullptr, nullptr };

			if(context.SelectedState != SELECTED_LEFT) {
				End = EndPoint.series->getSubckt(EndPoint.point);
				// Next
				if(End.elem == EndPoint.subckt.elem && EndPoint.point < (EndPoint.series->getSize()-1)) {
					Next = EndPoint.series->getSubckt(EndPoint.point + 1);
				} else if(End.elem == nullptr && !End.subckt->IsSeries()) {
					bool LastIsEndParallel;
					do {
						End = EndPoint.series->getSubckt(EndPoint.series->getSize()-1);
						LastIsEndParallel = (End.elem == nullptr) && End.subckt == EndPoint.parallel;

						if(LastIsEndParallel) {
							EndPoint.subckt.subckt = EndPoint.parallel;
							EndPoint.subckt.elem   = nullptr;
							EndPoint.series        = nullptr;
							EndPoint.parallel      = nullptr;
							rungs[CurrentRung]->rung->ElemInSubcktSeries(context, &EndPoint);
						}
					} while(LastIsEndParallel);
					Previous = EndPoint.series->getSubckt(EndPoint.point);
				}
			} else {
				End = EndPoint.series->getSubckt(EndPoint.point);
				// Next
				if(End.elem == nullptr && !End.subckt->IsSeries()) {
					Next = End;
				}
				// Previous
				if(EndPoint.point) {
					Previous = EndPoint.series->getSubckt(EndPoint.point - 1);
				}
			}

			if((Next.elem == nullptr && Next.subckt != nullptr) &&
					Next.subckt->SearchMatch(StartPoint.series, SUBCKT_STATUS_FIRST)) {
				if(context.SelectedState == SELECTED_RIGHT && StartPoint.point) {
					StartPoint.point--;
				}
				EndPoint.point  = 0;
				EndPoint.series = StartPoint.series;
			} else if((Previous.elem == nullptr && Previous.subckt != nullptr) &&
					Previous.subckt->SearchMatch(StartPoint.series, SUBCKT_STATUS_LAST)) {

				EndPoint.series = StartPoint.series;
				EndPoint.point  = StartPoint.series->getSize();
				if(context.SelectedState == SELECTED_RIGHT) {
					EndPoint.point--;
				}

			}
		}
	}

	// Phase 3: creates a series subcircuit containing all elements between start and end points.
	if(StartPoint.series != nullptr && StartPoint.series == EndPoint.series && context.SelectedElem->getWhich() != ELEM_PLACEHOLDER) {
		if(StartPoint.point > EndPoint.point) {
			UseCurrentParallel = 0;

			if(context.SelectedState != SELECTED_LEFT) {
				StartPoint.point--;
			}

			// Special condition: selectedstate is SELECTED_RIGHT
			if(context.SelectedState == SELECTED_RIGHT) {
				EndPoint.point++;
			}

			i = StartPoint.point;
			StartPoint.point = EndPoint.point;
			EndPoint.point = i;
		}
		if(!UseCurrentParallel || StartPoint.point != EndPoint.point) {
			if(context.SelectedState == SELECTED_LEFT && EndPoint.point)
				EndPoint.point--;

			ret = StartPoint.series  ->InsertParallel(elem, StartPoint.point, EndPoint.point, context);
		} else {
			Subckt s = { context.SelectedElem, context.SelectedCircuit };
			LadderCircuit *circuit =  context.SelectedCircuit;

			while(circuit != nullptr && circuit != StartPoint.parallel) {
				s.elem = nullptr;
				s.subckt = circuit;

				circuit = StartPoint.parallel->getParentSubckt(circuit);
			}

			unsigned int index = StartPoint.parallel->getSubcktIndex(s);
			ret = StartPoint.parallel->InsertParallel(elem, index           , index         , context);
		}

		// Phase 5: free context.ParallelStart and collapse.
		context.canDelete = true; // Forca a exclusao de ParallelStart
		DelElement(context.ParallelStart);
		context.ParallelStart = nullptr;
		rungs[CurrentRung]->rung->RemoveUnnecessarySubckts(context);
	} else {
		Error(_("Impossível criar paralelo entre os pontos selecionados!"));
	}

	return ret;
}

/*** Funcoes para Ler / Gravar o diagrama ladder no disco ***/

// Magic number to help us to identify if the file is valid.
// Format: xxxxyyzz
// Where:
// xxxx = always 0f5a
// yy   = flags.
// yy.0 = 0 for old binfmt. 1 for v2 and others.
// yy.1 - yy.15: reserved for future use.
// zz   = format version
static const unsigned long int LADDER_BINFMT_MAGIC      = 0x0f5a0102;
static const unsigned long int LADDER_BINFMT_MAGIC_MASK = 0xffff0000;
#define LADDER_BINFMT_GET_VERSION(m)  (m & 0x00ff)
#define LADDER_BINFMT_GET_FLAGS(m)   ((m & 0xff00) >> 8)
#define LADDER_BINFMT_FLAG_NEWBINFMT 0x01
#define LADDER_FILE_MAX_SIZE         20971520 // 20 MB

bool LadderDiagram::Save(string filename, bool isBackup)
{
	bool hasParallel = context.ParallelStart != nullptr;
	bool failed = true; // Desmarca ao chegar no final da logica. Se nao chegou, ocorreu uma falha

	// Se a contagem de checkpoint nao for zero, indica que alguma alteracao esta ocorrendo no diagrama.
	// Dessa forma nao podemos realizar o salvamento pois nesse momento estamos em um estado inconsistente!
	if(CheckpointBeginCount > 0) {
		return false;
	}

	if(filename.size() == 0) { // Nome vazio! Tentamos usar o nome do ultimo salvamento
		filename = currentFilename;
		if(filename.size() == 0) return false; // Continua vazio! Retorna erro...
	}

	// A seguir verificamos se o arquivo esta sendo sobrescrito e se esta acao eh permitida.
	if(filename == currentFilename && !LadderSettings.General.canSave) {
		return false; // Nome do arquivo carregado e flag nao permite sobrescrever! Retorna erro.
	}

	FILE *f = fopen(filename.c_str(), "wb+");
    if(!f) return failed;

	// Desmarca a flag que indica que nao pode salvar pois esse novo arquivo podera ser salvo.
	LadderSettings.General.canSave = true;

	// Alguns arquivos (como exemplos) nao podem ser sobrescritos. Dessa forma existe uma flag
	// interna nas configuracoes do projeto que inibe esse salvamento. A linha a seguir usa
	// essa flag para que o arquivo salvo neste momento tenha essa protecao.
	// Desative a linha abaixo apenas quando desejar impedir que o arquivo seja sobrescrito.
	//LadderSettings.General.canSave = false;

	// Ao salvar, se houver o inicio de um paralelo, ele deve ser removido
	// A acao de exclusao constara na lista de desfazer.
	// Para que o usuario nao perca o paralelo e nem consiga desfazer, devemos cancelar essa operacao
	// Para isso criamos um checkpoint ao iniciar o salvamento e executando um rollback no final
	// Dessa forma mesmo a lista de refazer (se existir) nao sera perdida...
	CheckpointBegin("Salvar");

	// Agora excluimos o inicio do paralelo, se ele existir...
	if(context.ParallelStart != nullptr) {
		DelElement(context.ParallelStart);
	}

	// Start with the magic number
	bool ret = fwrite_ulong (f, LADDER_BINFMT_MAGIC);

	/*** A seguir iniciamos o salvamento dos dados em si ***/

	// Salvando as configuracoes
	if(ret == true &&
		fwrite_bool  (f, LadderSettings.General.canSave    ) &&
		fwrite_int   (f, LadderSettings.General.cycleTime  ) &&
		fwrite_int   (f, LadderSettings.General.mcuClock   ) &&

		fwrite_int   (f, LadderSettings.Uart.UART          ) &&
		fwrite_int   (f, LadderSettings.Uart.baudRate      ) &&

		fwrite_ulong (f, LadderSettings.Network.ip         ) &&
		fwrite_ulong (f, LadderSettings.Network.mask       ) &&
		fwrite_ulong (f, LadderSettings.Network.gw         ) &&
		fwrite_ulong (f, LadderSettings.Network.dns        ) &&

		fwrite_bool  (f, LadderSettings.Sntp.sntp_enable   ) &&
		fwrite_bool  (f, LadderSettings.Sntp.dailysave     ) &&
		fwrite_string(f, LadderSettings.Sntp.sntp_server   ) &&
		fwrite_int   (f, LadderSettings.Sntp.gmt           ) &&

		fwrite_int   (f, LadderSettings.EncInc.conv_mode   ) &&
		fwrite_float (f, LadderSettings.EncInc.factor      ) &&
		fwrite_int   (f, LadderSettings.EncInc.perimeter   ) &&
		fwrite_int   (f, LadderSettings.EncInc.pulses      ) &&
		fwrite_bool  (f, LadderSettings.EncInc.x4          ) &&

		fwrite_int   (f, LadderSettings.EncSSI.conv_mode   ) &&
		fwrite_float (f, LadderSettings.EncSSI.factor      ) &&
		fwrite_int   (f, LadderSettings.EncSSI.perimeter   ) &&
		fwrite_int   (f, LadderSettings.EncSSI.mode        ) &&
		fwrite_int   (f, LadderSettings.EncSSI.size        ) &&
		fwrite_int   (f, LadderSettings.EncSSI.size_bpr    ) &&

		fwrite_int   (f, LadderSettings.Dac.ramp_abort_mode) &&

		fwrite_bool  (f, LadderSettings.Adc.isCelsius      ) &&

		fwrite_int   (f, LadderSettings.MbSlave.ModBUSID   ) &&

		fwrite_string(f, LadderSettings.Info.Name          ) &&
		fwrite_string(f, LadderSettings.Info.Developer     ) &&
		fwrite_string(f, LadderSettings.Info.Description   ) &&

		fwrite_string(f, LadderSettings.Details.FWVersion  ) &&
		fwrite_long  (f, LadderSettings.Details.BuildNumber) &&
		fwrite_time_t(f, LadderSettings.Details.CompileDate) &&
		fwrite_time_t(f, LadderSettings.Details.ProgramDate)
		) {
			// Configuracoes OK, agora devemos gravar a lista de placas de expansao
			ret = expansionBoards.Save(f) && IO->Save(f);

			// Se OK, agora devemos gravar o mapa de I/O
			if(ret == true) {
				ret = IO->Save(f);
			}

			// Se OK, agora gravamos a lista de nodes do ModBUS.
			if(ret == true) {
				if(fwrite_uint(f, vectorMbNodeList.size())) {
					vector<LadderMbNodeList *>::iterator it;

					for(it = vectorMbNodeList.begin(); it != vectorMbNodeList.end(); it++) {
						fwrite_int   (f, (*it)->NodeID);
						fwrite_uint  (f, (*it)->NodeCount);
						fwrite_string(f, (*it)->node.name);
						fwrite_int   (f, (*it)->node.id);
						fwrite_ulong (f, (*it)->node.ip);
						fwrite_uint  (f, (*it)->node.iface);
					}

					if(it != vectorMbNodeList.end()) {
						ret = false; // erro durante a gravacao da lista
					}
				} else {
					ret = false;
				}
			}

			// Gravacao do diagrama ladder
			if(ret == true) {
				vector<LadderRung *>::iterator it;

				if(fwrite_uint(f, rungs.size())) {
					for(it = rungs.begin(); it != rungs.end(); it++) {
						if(!fwrite_bool(f, (*it)->hasBreakpoint) || !(*it)->rung->Save(f)) {
							break;
						}
					}

					if(it != rungs.end()) {
						ret = false; // erro durante a gravacao do diagrama
					}
				} else {
					ret = false;
				}
			}

			// Se nao houve erros ate aqui, calcula e grava o checksum
			if(ret == true) {
				unsigned short int crc = 0;
				long size;

				fseek(f, 0, SEEK_END);
				size = ftell(f) - sizeof(LADDER_BINFMT_MAGIC);
				if(size <= LADDER_FILE_MAX_SIZE) {
					unsigned char *buffer = new unsigned char[size];

					fseek(f, sizeof(LADDER_BINFMT_MAGIC), SEEK_SET);
					fread(buffer, size, 1, f);

					crc = CRC16((unsigned char *)buffer, size);

					fseek(f, 0, SEEK_END);
					fwrite(&crc, sizeof(crc), 1, f);

					delete [] buffer;

					failed = false;

					// Atualizamos o nome do arquivo atual, permitindo a acao salvar
					// utilizar este nome de arquivo ao receber um nome vazio.
					// Quando salvando um backup nao devemos salvar o nome do arquivo.
					if(!isBackup) {
						currentFilename = filename;
					}
				}
			}
	}

	fclose(f);

	// Agora executamos o rollback e encerramos o checkpoint.
	// Dessa forma as acoes serao desfeitas e as listas de desfazer/refazer nao serao perdidas.
	CheckpointRollback();
	CheckpointEnd();

	if(failed) {
		_unlink(filename.c_str());
	} else if(!isBackup) { // Se for backup, nao devemos atualizar as flags que indicam que o diagrama possui alteracoes
		context.programChangedNotSaved = false;

		// Atualiza a flag que indica se precisa salvar em todos os itens das listas de Desfazer/Refazer
		deque<UndoRedoAction>::iterator it;

		if(UndoList.size() > 0) {
			for(it = UndoList.begin(); it != UndoList.end(); it++) {
				it->contextAfter .programChangedNotSaved = true;
				it->contextBefore.programChangedNotSaved = true;
			}

			it = UndoList.end() - 1;
			while(it->action != eCheckpoint) {
				it->contextAfter.programChangedNotSaved = false;

				// Verifica se chegou ao final da lista
				if(it == UndoList.begin()) {
					break;
				}

				it--;
			}
		}

		if(RedoList.size() > 0) {
			for(it = RedoList.begin(); it != RedoList.end(); it++) {
				it->contextAfter .programChangedNotSaved = true;
				it->contextBefore.programChangedNotSaved = true;
			}

			it = RedoList.end() - 2; // Removemos 2 pois o primeiro eh a marcacao do checkpoint
			while(it->action != eCheckpoint) {
				it->contextBefore.programChangedNotSaved = false;

				// Verifica se chegou ao final da lista
				if(it == RedoList.begin()) {
					break;
				}

				it--;
			}
		}
	}

	return !failed;
}

bool LadderDiagram::Load(string filename)
{
	unsigned long int magic;
	bool failed = true; // Desmarca ao chegar no final da logica. Se nao chegou, ocorreu uma falha

	FILE *f = fopen(filename.c_str(), "rb");
    if(!f) return !failed;

	// Start with the magic number
	fread(&magic, sizeof(magic), 1, f);
	if(((magic ^ LADDER_BINFMT_MAGIC) & LADDER_BINFMT_MAGIC_MASK) == 0 &&
		(LADDER_BINFMT_GET_FLAGS(magic) & LADDER_BINFMT_FLAG_NEWBINFMT)) {
		long size;
		unsigned char *buffer;
		unsigned short int crc_calc, crc_read;

		fseek(f, 0, SEEK_END);
		size   = ftell(f) - sizeof(LADDER_BINFMT_MAGIC) - sizeof(crc_read);
		if(size <= LADDER_FILE_MAX_SIZE) {
			buffer = new unsigned char[size];

			fseek(f, sizeof(LADDER_BINFMT_MAGIC), SEEK_SET);
			fread(buffer, size, 1, f);

			crc_calc = CRC16((unsigned char *)buffer, size);

			fread(&crc_read, sizeof(crc_read), 1, f);
			delete [] buffer;

			if(crc_calc == crc_read) {
				int fileVersion = LADDER_BINFMT_GET_VERSION(magic);
				// volta para o comeco, depois do numero magico
				fseek(f, sizeof(LADDER_BINFMT_MAGIC), SEEK_SET);

				// Limpa e inicializa o diagrama com os valores padrao antes de comecar o carregamento
				ClearDiagram();

				// Marca o contexto indicando que um arquivo esta sendo carregado
				context.isLoadingFile = true;

				// A tela tambem deve ser bloqueada
				context.isDrawBlocked = true;

				/*** A seguir iniciamos o carregamento dos dados em si ***/

				// Carregando as configuracoes
				if(
					fread_bool  (f, &LadderSettings.General.canSave    ) &&
					fread_int   (f, &LadderSettings.General.cycleTime  ) &&
					fread_int   (f, &LadderSettings.General.mcuClock   ) &&

					fread_int   (f, &LadderSettings.Uart.UART          ) &&
					fread_int   (f, &LadderSettings.Uart.baudRate      ) &&

					fread_ulong (f, &LadderSettings.Network.ip         ) &&
					fread_ulong (f, &LadderSettings.Network.mask       ) &&
					fread_ulong (f, &LadderSettings.Network.gw         ) &&
					fread_ulong (f, &LadderSettings.Network.dns        ) &&

					fread_bool  (f, &LadderSettings.Sntp.sntp_enable   ) &&
					fread_bool  (f, &LadderSettings.Sntp.dailysave     ) &&
					fread_string(f, &LadderSettings.Sntp.sntp_server   ) &&
					fread_int   (f, &LadderSettings.Sntp.gmt           ) &&

					fread_int   (f, &LadderSettings.EncInc.conv_mode   ) &&
					fread_float (f, &LadderSettings.EncInc.factor      ) &&
					fread_int   (f, &LadderSettings.EncInc.perimeter   ) &&
					fread_int   (f, &LadderSettings.EncInc.pulses      ) &&
					fread_bool  (f, &LadderSettings.EncInc.x4          ) &&

					fread_int   (f, &LadderSettings.EncSSI.conv_mode   ) &&
					fread_float (f, &LadderSettings.EncSSI.factor      ) &&
					fread_int   (f, &LadderSettings.EncSSI.perimeter   ) &&
					fread_int   (f, &LadderSettings.EncSSI.mode        ) &&
					fread_int   (f, &LadderSettings.EncSSI.size        ) &&
					fread_int   (f, &LadderSettings.EncSSI.size_bpr    ) &&

					fread_int   (f, &LadderSettings.Dac.ramp_abort_mode) &&

					fread_bool  (f, &LadderSettings.Adc.isCelsius      ) &&

					fread_int   (f, &LadderSettings.MbSlave.ModBUSID   ) &&

					fread_string(f, &LadderSettings.Info.Name          ) &&
					fread_string(f, &LadderSettings.Info.Developer     ) &&
					fread_string(f, &LadderSettings.Info.Description   ) &&

					fread_string(f, &LadderSettings.Details.FWVersion  ) &&
					fread_long  (f, &LadderSettings.Details.BuildNumber) &&
					fread_time_t(f, &LadderSettings.Details.CompileDate) &&
					fread_time_t(f, &LadderSettings.Details.ProgramDate)
					) {
						// Configuracoes OK, agora devemos ler a lista de placas de expansao
						bool ret = expansionBoards.Load(f, fileVersion);

						// Configuracoes OK, agora devemos ler o mapa de I/O
						if(ret == true) {
							ret = IO->Load(f, fileVersion);
						}

						// Se OK, agora carregamos a lista de nodes do ModBUS.
						if(ret == true) {
							unsigned int i, list_size, iIface;
							LadderMbNodeList nl;

							if(fread_uint(f, &list_size)) {

								for(i = 0; i < list_size; i++) {
									if(
										!fread_int   (f, &nl.NodeID) ||
										!fread_uint  (f, &nl.NodeCount) ||
										!fread_string(f, &nl.node.name) ||
										!fread_int   (f, &nl.node.id) ||
										!fread_ulong (f, &nl.node.ip) ||
										!fread_uint  (f, &iIface)
										) {
											break;
									} else {
										LadderMbNodeList *pnl = new LadderMbNodeList;

										switch(iIface) {
											default:
											case eMbTypeNode_RS485   : nl.node.iface = eMbTypeNode_RS485   ; break;
											case eMbTypeNode_Ethernet: nl.node.iface = eMbTypeNode_Ethernet; break;
										}

										*pnl = nl;
										vectorMbNodeList.push_back(pnl);
									}
								}

								if(i != list_size) {
									ret = false; // erro durante a gravacao da lista
								}
							} else {
								ret = false;
							}
						}

						// Se OK, agora carregamos o diagrama ladder.
						if(ret == true) {
							unsigned int num_rungs;

							if(fread_uint(f, &num_rungs)) {
								unsigned int i;
								for(i = 0; i < num_rungs; i++) {
									LadderRung *rung = new LadderRung;
									rung->isPowered      = false;
									rung->isBeingDeleted = false;
									if(fread_bool(f, &rung->hasBreakpoint)) {
										rung->rung = new LadderCircuit;
										if(!rung->rung->Load(this, f, fileVersion)) {
											// Erro ao carregar o circuito. Desaloca memoria e cancela carregamento
											delete rung->rung;
											delete rung;
											break;
										} else {
											rungs.push_back(rung);
										}
									} else {
										delete rung;
										break;
									}
								}

								if(i != num_rungs) {
									ret = false; // algum erro ocorreu ao carregar o diagrama
								}
							} else {
								ret = false;
							}
						}

						if(ret == true) {
							// Fim do carregamento, nenhum erro ate aqui!
							// Desmarca a flag de erro pois o carregamento finalizou sem erro.
							failed = false;

							// Atualizamos o nome do arquivo atual, permitindo a acao salvar
							// utilizar este nome de arquivo ao receber um nome vazio.
							currentFilename = filename;
						}
				}
			}
		}
	}

	fclose(f);

	NeedRedraw(true);

	context.isLoadingFile = false;
	context.isDrawBlocked = false;

	if(failed) {
		// Erro durante o carregamento
		NewDiagram();
		IO->updateGUI();
	} else {
		// Carregmento finalizado com sucesso!
		// As acoes executadas durante o carregamento nao devem ser desfeitas, apenas descartadas
		while(UndoList.size() > 0) {
			DiscardCheckpoint(true);
		}

		// Garante que o contexto esta OK
		updateContext();

		// Forca desenhar a lista de I/Os e a tela
		IO->updateGUI();
		DrawGUI();
	}

	// Se houve erro ou nao, sempre estaremos em uma situacao em que nao existem alteracoes...
	context.programChangedNotSaved = false;

	return !failed;
}

string LadderDiagram::getCurrentFilename(void)
{
	return currentFilename;
}

void LadderDiagram::ToggleBreakPoint(unsigned int rung)
{
	// Se linha inexistente ou comentario, retorna.
	if(rung > rungs.size() || rungs[rung]->rung->IsComment()) return;

	rungs[rung]->hasBreakpoint = !rungs[rung]->hasBreakpoint;
}

unsigned int LadderDiagram::getBreakPointActiveAtRung(void)
{
	unsigned int i;

	for(i = 0; i < rungs.size(); i++) {
		if(rungs[i]->hasBreakpoint && rungs[i]->rung->HasActiveEOL())
			return i + 1;
	}

	return 0;
}

void LadderDiagram::ProgramChanged(void)
{
	NeedRedraw(true);
	context.programChangedNotSaved = true;
}

/*** Funcoes para gravar a configuracao do ladder ***/

// Funcao que Registra no Undo/Redo a configuracao atual para que possa ser desfeito
void LadderDiagram::RegisterSettingsChanged(void)
{
	// Cria um checkpoint para evitar registrar uma ação fora de um checkpoint
	CheckpointBegin(_("Alterar Configurações"));

	// Registro da acao para desfazer / refazer
	UndoRedoAction action;
	UndoRedoData *data = new UndoRedoData;

	data->SettingsChanged.settings = new tLadderSettings;
	*(data->SettingsChanged.settings) = LadderSettings;

	action.action        = eSettingsChanged;
	action.contextAfter  = getEmptyContext();
	action.contextBefore = context;
	action.data          = data;
	action.io            = nullptr;
	action.elem          = nullptr;
	action.subckt        = nullptr;

	RegisterAction(action);

	// Essa funcao somente eh chamada quando alguma configuracao estiver sendo alterada.
	// Desta forma, indica que houve alteracao no programa
	ProgramChanged();

	CheckpointEnd();
}

void LadderDiagram::setSettingsGeneral(LadderSettingsGeneral setGeneral)
{
	RegisterSettingsChanged();
	LadderSettings.General = setGeneral;
}

void LadderDiagram::setSettingsUART(LadderSettingsUART setUart)
{
	RegisterSettingsChanged();
	LadderSettings.Uart = setUart;
}

void LadderDiagram::setSettingsNetwork(LadderSettingsNetwork setNetwork)
{
	RegisterSettingsChanged();
	LadderSettings.Network = setNetwork;
}

void LadderDiagram::setSettingsSNTP(LadderSettingsSNTP setSntp)
{
	RegisterSettingsChanged();
	LadderSettings.Sntp = setSntp;
}

void LadderDiagram::setSettingsEncoderIncremental(LadderSettingsEncoderIncremental setEncInc)
{
	RegisterSettingsChanged();
	LadderSettings.EncInc = setEncInc;
}

void LadderDiagram::setSettingsEncoderSSI(LadderSettingsEncoderSSI setEncSSI)
{
	RegisterSettingsChanged();
	LadderSettings.EncSSI = setEncSSI;
}

void LadderDiagram::setSettingsADC(LadderSettingsADC setAdc)
{
	RegisterSettingsChanged();
	LadderSettings.Adc = setAdc;
}

void LadderDiagram::setSettingsDAC(LadderSettingsDAC setDac)
{
	RegisterSettingsChanged();
	LadderSettings.Dac = setDac;
}

void LadderDiagram::setSettingsModbusSlave(LadderSettingsModbusSlave setMbSlave)
{
	RegisterSettingsChanged();
	LadderSettings.MbSlave = setMbSlave;
}

void LadderDiagram::setSettingsInformation(LadderSettingsInformation setInfo)
{
	RegisterSettingsChanged();
	LadderSettings.Info = setInfo;
}

void LadderDiagram::setSettingsDetails(LadderSettingsDetails setDetails)
{
	// Aqui nao registramos as alteracoes pois elas nao devem ser desfeitas
	LadderSettings.Details = setDetails;
}

// Funcao para configurar o modo de simulacao
void LadderDiagram::setSimulationState(bool state)
{
	SetLock(state); // Em simulacao, o diagrama deve ser travado para impedir alteracoes
	context.inSimulationMode = state;
	updateContext();
}

/*** Funcoes relacionadas com lista de nos do ModBUS ***/

// Clears a node to its default values
void LadderDiagram::mbClearNode(LadderMbNode *node)
{
	if(node == nullptr) return;

	node->id    = 0;
	node->iface = eMbTypeNode_RS485;
	node->ip    = 0;
	node->name  = "";
}

// Create a new node in the list
int LadderDiagram::mbCreateNode(string NodeName)
{
	if(NodeName.size() == 0) return -1;

	LadderMbNode node;
	mbClearNode(&node);
	node.name = NodeName;

	return mbCreateNode(node);
}

// Create a new node in the list
int LadderDiagram::mbCreateNode(LadderMbNode node)
{
	if(node.name.size() == 0) return -1;

	int elem = -1, NodeID = mbGetNodeIDByName(node.name);

	if(NodeID >= 0) {
		elem = mbUpdateNode(NodeID, node);
	} else {
		LadderMbNodeList *nl = new LadderMbNodeList;
		elem = vectorMbNodeList.size();

		nl->NodeID    = elem ? vectorMbNodeList[elem - 1]->NodeID + 1 : 0;
		nl->NodeCount = 0;
		nl->node      = node;

		// Registro da acao para desfazer / refazer
		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->MbNodeCreate.nl = nl;

		action.action        = eMbNodeCreate;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = context;
		action.data          = data;
		action.io            = nullptr;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		RegisterAction(action);

		vectorMbNodeList.push_back(nl);

		// Indica que houve alteracao no programa
		ProgramChanged();
	}

	return elem;
}

// Update data of node in the list
int LadderDiagram::mbUpdateNode(int NodeID, LadderMbNode node)
{
	if(node.name.size() == 0) return -1;

	int elem = mbGetIndexByNodeID(NodeID);

	if(elem >= 0) {
		unsigned int index = elem;
		if(index < vectorMbNodeList.size()) {
			// Registro da acao para desfazer / refazer
			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			  data->MbNodeUpdate.index = index;
			  data->MbNodeUpdate.node  = new LadderMbNode;
			*(data->MbNodeUpdate.node) = vectorMbNodeList[index]->node;

			action.action        = eMbNodeUpdate;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = context;
			action.data          = data;
			action.io            = nullptr;
			action.elem          = nullptr;
			action.subckt        = nullptr;

			RegisterAction(action);

			// Atualiza o no
			vectorMbNodeList[index]->node = node;

			// Indica que houve alteracao no programa
			ProgramChanged();
		} else {
			elem = -1;
		}
	} else {
		elem = mbCreateNode(node);
	}

	return elem;
}

// Delete a node from the list
void LadderDiagram::mbDeleteNode(int NodeID)
{
	unsigned int index = mbGetIndexByNodeID(NodeID);
	if(index >= 0 && index < vectorMbNodeList.size()) {
		// Registro da acao para desfazer / refazer
		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->MbNodeDelete.index = index;
		data->MbNodeDelete.nl    = vectorMbNodeList[index];

		action.action        = eMbNodeDelete;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = context;
		action.data          = data;
		action.io            = nullptr;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		RegisterAction(action);

		// Remove o no
		vectorMbNodeList.erase(vectorMbNodeList.begin() + index);

		// Indica que houve alteracao no programa
		ProgramChanged();
	}
}

// Get MbNodeList by Index (Position in the list)
LadderMbNode LadderDiagram::mbGetNodeByIndex(int elem)
{
	if(elem >= 0) {
		unsigned int index = elem;
		if(index < vectorMbNodeList.size()) {
			return vectorMbNodeList[index]->node;
		}
	}

	// Se chegou aqui, index invalido! Cria um no vazio
	LadderMbNode node;
	mbClearNode(&node);

	return node;
}

// Get MbNode by NodeID
LadderMbNode LadderDiagram::mbGetNodeByNodeID(int NodeID)
{
	return mbGetNodeByIndex(mbGetIndexByNodeID(NodeID));
}

// Get NodeID by Index (Position in the list)
int LadderDiagram::mbGetNodeIDByIndex(int elem)
{
	if(elem >= 0) {
		unsigned int index = elem;
		if(index < vectorMbNodeList.size()) {
			return vectorMbNodeList[index]->NodeID;
		}
	}

	return -1;
}

// Get NodeID by node name
int LadderDiagram::mbGetNodeIDByName(string Name)
{
	unsigned int index;

	for(index = 0; index < vectorMbNodeList.size(); index++) {
		if(vectorMbNodeList[index]->node.name == Name) {
			return vectorMbNodeList[index]->NodeID;
		}
	}

	return -1;
}

// Get Index (Position in the list) by NodeID
int LadderDiagram::mbGetIndexByNodeID(int NodeID)
{
	unsigned int index;

	for(index = 0; index < vectorMbNodeList.size(); index++) {
		if(vectorMbNodeList[index]->NodeID == NodeID) {
			return index;
		}
	}

	return -1;
}

// Get Node Count by NodeID
unsigned int LadderDiagram::mbGetNodeCount(int NodeID)
{
	unsigned int index;

	for(index = 0; index < vectorMbNodeList.size(); index++) {
		if(vectorMbNodeList[index]->NodeID == NodeID) {
			return vectorMbNodeList[index]->NodeCount;
		}
	}

	return 0;
}

// Increment Reference Count for given NodeID
void LadderDiagram::mbAddRef(int NodeID)
{
	int index = mbGetIndexByNodeID(NodeID);

	if(index >= 0) {
		// Registro da acao para desfazer / refazer
		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->MbNodeRef.index    = index;
		data->MbNodeRef.isAddRef = true;

		action.action        = eMbNodeRef;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = context;
		action.data          = data;
		action.io            = nullptr;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		RegisterAction(action);

		// Adiciona a referencia
		vectorMbNodeList[index]->NodeCount++;
	}
}

// Decrement Reference Count for given NodeID
void LadderDiagram::mbDelRef(int NodeID)
{
	int index = mbGetIndexByNodeID(NodeID);

	if(index >= 0 && vectorMbNodeList[index]->NodeCount > 0) {
		// Registro da acao para desfazer / refazer
		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->MbNodeRef.index    = index;
		data->MbNodeRef.isAddRef = false;

		action.action        = eMbNodeRef;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = context;
		action.data          = data;
		action.io            = nullptr;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		RegisterAction(action);

		// Remove a referencia
		vectorMbNodeList[index]->NodeCount--;
	}
}

/*** Funcoes relacionadas com Placas de Expansao ***/

vector<sExpansionBoardItem> LadderDiagram::getExpansionBoardList(void)
{
	return expansionBoards.getExpansionBoardList();
}

/*** Funcoes relacionadas com I/O ***/

bool LadderDiagram::acceptIO(unsigned long id, enum eType type)
{
	vector<LadderCircuit>::size_type it;

	// Se o tipo atual do I/O for reservado, o novo tipo deve ser reservado tambem.
	if(IO->getDetails(id).type == eType_Reserved) {
		type = eType_Reserved;
	}

	for(it = 0; it < rungs.size(); it++) {
		if(!rungs[it]->rung->acceptIO(nullptr, id, type)) {
			return false;
		}
	}

	return true;
}

eType LadderDiagram::getAllowedType(LadderElem *elem, unsigned long id, eType default_type)
{
	eType ret = eType_Pending;
	vector<eType> allowedTypes;
	vector<LadderCircuit>::size_type i;

	// Inicialmente montamos a lista com os tipos permitidos para o I/O
	for(i = 0; i < rungs.size(); i++) {
		rungs[i]->rung->getAllowedTypes(elem, id, &allowedTypes);
	}

	// Depois verificamos se entre eles existe o tipo atual.
	// Se existir, nao faz nada pois significa que podemos continuar com ele
	// Se nao existir, vamos passando a lista 1 a 1 ate encontrarmos um tipo
	// aceito por todos os elementos.
	// O tipo Geral deve ser a ultima opcao. Devemos dar preferencia a tipos
	// especificos.
	if(find(allowedTypes.begin(), allowedTypes.end(), getDetailsIO(id).type) == allowedTypes.end()) {
		vector<eType>::iterator it;
		// Nao existe mais o tipo atual. Devemos buscar 1 a 1 ate chegar no tipo aceito por todos
		for(it = allowedTypes.begin(); it != allowedTypes.end(); it++) {
			for(i = 0; i < rungs.size(); i++) {
				if(!rungs[i]->rung->acceptIO(elem, id, *it)) {
					break;
				}
			}

			// Se foi ate o final, significa que foi aceito por todos. atualiza o tipo!
			if(i == rungs.size()) {
				ret = *it;
				if(ret != eType_General) {
					// Se for tipo Geral, continuamos a busca para verificar se existe outra opcao.
					// Fazemos isso pois devemos dar preferencia por tipos especificos.
					// Assim, caso nao for encontrado outro tipo aceito, usaremos o
					// tipo Geral.
					break;
				}
			}
		}
	} else {
		ret = getDetailsIO(id).type;
	}

	if(ret == eType_Pending) {
		ret = default_type;
	}

	return ret;
}

void LadderDiagram::updateIO(LadderDiagram *owner, LadderElem *elem, tRequestIO &infoIO, bool isDiscard)
{
	if(infoIO.pin.first == 0) return; // id nao esta em uso, indica que eh um numero e nao um I/O

	if(isDiscard) {
		string name;
		bool useGeneralType = false;
		unsigned long id = infoIO.pin.first;

		IO->Discard(infoIO);
		name = getNameIO(id);
		if(name.size() > 0 && !IO->IsReserved(name)) {
			// I/O ainda existente e nao eh reservado!! Devemos atualizar o tipo.
			vector<eType> allowedTypes;
			vector<LadderCircuit>::size_type i;

			// Inicialmente montamos a lista com os tipos permitidos para o I/O
			for(i = 0; i < rungs.size(); i++) {
				// Se a linha estiver sendo excluida, nao considera os tipos retornados por ela.
				if(rungs[i]->isBeingDeleted) continue;

				rungs[i]->rung->getAllowedTypes(elem, id, &allowedTypes);
			}

			// Depois verificamos se entre eles existe o tipo atual.
			// Se existir, nao faz nada pois significa que podemos continuar com ele
			// Se nao existir, vamos passando a lista 1 a 1 ate encontrarmos um tipo
			// aceito por todos os elementos.
			// O tipo Geral deve ser a ultima opcao. Devemos dar preferencia a tipos
			// especificos.
			if(find(allowedTypes.begin(), allowedTypes.end(), getDetailsIO(id).type) == allowedTypes.end()) {
				vector<eType>::iterator it;
				// Nao existe mais o tipo atual. Devemos buscar 1 a 1 ate chegar no tipo aceito por todos
				for(it = allowedTypes.begin(); it != allowedTypes.end(); it++) {
					for(i = 0; i < rungs.size(); i++) {
						// Se a linha estiver sendo excluida, nao verifica se o tipo eh aceito por ela.
						if(rungs[i]->isBeingDeleted) continue;

						if(!rungs[i]->rung->acceptIO(elem, id, *it)) {
							break;
						}
					}

					// Se foi ate o final, significa que foi aceito por todos. atualiza o tipo!
					if(i == rungs.size()) {
						if(*it == eType_General) {
							// Se for tipo Geral, apenas indicamos que ele pode ser usado.
							// Fazemos isso pois devemos dar preferencia por tipos especificos.
							// Assim, caso nao for encontrado outro tipo aceito, usaremos o
							// tipo Geral.
							useGeneralType = true;
						} else {
							useGeneralType = false;
							IO->Update(id, getNameIO(id), *it);
							break;
						}
					}
				}

				// Se devemos usar o tipo geral significa que ele eh aceito mas nao foi utilizado
				// pois a logica estava procurando por outro tipo compativel. Se chegamos aqui com
				// a flag useGeneralType marcada significa que nenhum outro tipo aceito foi encontrado.
				if(useGeneralType) {
					IO->Update(id, getNameIO(id), eType_General);
				}
			}
		}
	} else {
		string name;
		// Tipo a ser usado pelo I/O
		eType type = eType_Pending;

		// Se nao for descarte, indica que devemos atualizar o I/O conforme os passos abaixo:
		// 1) Se o I/O tem como origem outro diagrama ladder, verificar se existe um compativel
		// no diagrama atual
		// 2) Verifica se id eh valido
		// 2.1) Se for valido, verifica se o tipo ainda eh valido e entao faz um request
		// 2.2) Se nao for valido, devemos criar um novo I/O e atualizar o id

		// Etapa 1
		if(owner != this) { // I/O veio de outro diagrama!!!
			if(owner != nullptr) {
				name = owner->getNameIO   (infoIO.pin);
				eType  ownerType;

				// Se o nome estiver vazio, o I/O deve ter sido excluido. Recupera do cache!
				if(name.size() == 0) {
					pair<string, eType> cachedIO = owner->getCachedIO(infoIO.pin.first);
					name = cachedIO.first;
					ownerType = cachedIO.second;
				} else {
					ownerType = owner->getDetailsIO(infoIO.pin.first).type;
				}

				long oldID = infoIO.pin.first; // Salva o ID para se for necessario checar o tipo
				infoIO.pin.first = 0;

				pair <unsigned long, int> pin = pair<unsigned long, int>(0, 0);
				pin.first = getIdIO(name);

				if(name.size() > 0) {
					if(pin.first == 0) {
						// Se pin.first for 0, significa que nao existe no diagrama atual um
						// I/O com o mesmo nome que no diagrama de onde foi copiado este elemento.
						// Assim, para tentar manter a logica o mais proxima da original possivel,
						// vamos atualizar a estrutura que solicita o I/O para usar o nome do I/O
						// original. Nao utilizaremos o tipo do I/O no diagrama de origem pois pode
						// nao ser valido. Ex.: se estivermos copiando um move que usava variavel de
						// um A/D, o tipo sera Leitura de A/D mas neste diagrama deve ser tipo geral.
						infoIO.name = name;

						// Excecao: se o elemento for um contato ou bobina, podemos aceitar o tipo.
						if(elem->getWhich() == ELEM_COIL || elem->getWhich() == ELEM_CONTACTS) {
							infoIO.type = ownerType;
						}
					} else if(getDetailsIO(pin.first).type == ownerType) { // Mesmo tipo!
						infoIO.pin = pin;
						type       = ownerType;
					} else if(acceptIO(pin.first, ownerType)) { // Compativel!
						infoIO.pin = pin;
						type = getAllowedType(nullptr, pin.first, ownerType); // nullptr: Nao estamos descartando I/O aqui, aceitar qualquer elemento.
						if(type != ownerType && !elem->acceptIO(oldID, type)) {
							type = ownerType;
						}
					}
				}
			} else {
				infoIO.pin.first = 0;
			}
		}

		// Etapa 2
		name = IO->getName(infoIO.pin.first);

		// Se o nome estiver vazio, o I/O deve ter sido excluido. Recupera do cache!
		if(name.size() == 0 && infoIO.pin.first != 0) {
			pair<string, eType> cachedIO = getCachedIO(infoIO.pin.first);
			name = cachedIO.first;
			type = cachedIO.second;

			// Pode ser que ja exista um novo I/O com o mesmo nome do I/O recuperado do cache.
			// Assim devemos atualizar o id para o novo mas podemos tentar usar o I/O em cache
			// pois sera verificado posteriormente a compatibilidade entre os tipos.
			unsigned long id = IO->getID(name);
			if(id != 0) {
				infoIO.pin.first = id;
			}
		}

		if(name.size() > 0) { // id valido!
			if(type == eType_Pending) {
				type = IO->getDetails(infoIO.pin.first).type;
			}

			if(acceptIO(infoIO.pin.first, type)) {
				tRequestIO copyInfoIO = infoIO;
				copyInfoIO.name  = name;
				copyInfoIO.type  = type;
				infoIO.pin.first = IO->Request(copyInfoIO);
			} else {
				infoIO.pin.first = 0;
			}
		} else {
			infoIO.pin.first = 0;
		}

		if(infoIO.pin.first == 0) { // id invalido! Cria o novo I/O
			getIO(elem, infoIO);
		}
	}
}

void LadderDiagram::updateTypeIO(unsigned long id)
{
	IO->updateType(id);
}

bool LadderDiagram::getIO(LadderElem *elem, tRequestIO &infoIO, bool tryGeneralTypeFirst)
{
	// Ao carregar um arquivo, os I/Os sao lidos do disco e nao precisam ser solicitados aqui.
	if(context.isLoadingFile) {
		return true;
	}

	bool ret = true;
	pair<unsigned long, int> newpin;

	// O objetivo dessa funcao eh simplificar o gerenciamento de IO para os elementos do ladder
	// Ela deve por si propria executar todas as acoes para cadastramento / atualizacao / remocao de IOs
	// Assim sendo, essas sao as acoes que devem ser executados por essa funcao:
	// 1 - Realizar Request / Discard quando necessario
	// 2 - Converter para valor numerico quando for o caso
	// 3 - Carregar os valores no par de retorno conforme o tipo (variavel / numerico)
	// 4 - Atualizar o IO se houve alteracao de tipo

	// Se nome estiver vazio, buscamos um nome que nao esteja em uso.
	if(infoIO.name.size() == 0) {
		do {
			infoIO.name = IO->getNextVar();
		} while(getIdIO(infoIO.name) != 0);
	}

	// O valor solicitado eh um numero, carrega no par de retorno.
	if(IsNumber(infoIO.name.c_str())) {
		newpin.first  = 0;
		newpin.second = atoi(infoIO.name.c_str());
	} else {
		mapDetails detailsIO = IO->getDetails(infoIO.pin.first);
		newpin.first  = IO->getID(infoIO.name);
		newpin.second = 0;

		if(!newpin.first && !IO->IsReserved(IO->getName(infoIO.pin.first))) { // variavel inexistente
			newpin.first = infoIO.pin.first; // O novo pino vai utilizar o mesmo id, ou seja, nao cria nova variavel

			// Se houver mais de 1 elemento usando a variavel, precisamos perguntar ao usuario se ele
			// deseja criar uma nova variavel ou se ele quer atualizar o nome da variavel para todos
			// os elementos
			if((detailsIO.countRequestBit + detailsIO.countRequestInt) > 1) {
				// Precisamos saber do usuario se devemos atualizar o IO ou se vamos criar uma nova variavel
				if(ShowDialog(eDialogType_Question, false, _("Nome Alterado"), _("Nome '%s' utilizado também em outros elementos.\n"
					"Alterar o nome em todos os elementos que o utilizam?"), IO->getName(newpin.first).c_str()) == eReply_No) {
						newpin.first = 0;
				}
			}
		}

		// Se devemos primeiro tentar o tipo geral, checamos se ele eh aceito pelos outros I/Os.
		// Se for aceito, podemos atualizar para o tipo geral pois vai funcionar!
		if(tryGeneralTypeFirst && acceptIO(newpin.first, eType_General)) {
			infoIO.type = eType_General;
		}

		// Verifica se o tipo do I/O eh aceito pelos outros elementos que ja o utilizam
		if(newpin.first && !acceptIO(newpin.first, infoIO.type)) {
			// Alguem compartilhando desta variavel nao aceita a alteracao de tipo. Cancela a alteracao!
			ShowDialog(eDialogType_Message, false, _("Tipo Inválido"), _("Conflito entre tipos para '%s' ! Operação não permitida."), IO->getName(newpin.first).c_str());
			return false; // retorna pino invalido
		}

		// I/O diferente do anterior ou inexistente, fazer o request
		if(newpin.first != infoIO.pin.first || newpin.first == 0) {
			newpin.first = IO->Request(infoIO);
			if(newpin.first == 0) { // Erro no request
				ret = false;
			}
		// Mesmo que a variavel nao mudou, o tipo pode ter sido alterado. Tenta atualizar
		} else if(!IO->Update(newpin.first, infoIO.name, infoIO.type)) {
			// Houve erro na atualizacao! Retorna falso para informar que houve o erro.
			// O procedimento correto nesse caso eh que o objeto nao seja atualizado devido
			// ao erro, mantendo o pino antigo. Dessa forma nao podemos descartar o id antigo.
			ret = false;
		}
	}

	// Se estiver sem erro, descarta o id antigo e atualiza o pino
	if(ret == true) {
		// Se id era valido, o novo pino for valido e id mudou, entao a variavel foi atualizada
		if(infoIO.pin.first && newpin.first != infoIO.pin.first) {
			// id mudou, devemos descartar o antigo
			updateIO(this, elem, infoIO, true);
		}

		// Tudo ok, atualiza o pino atual com o novo
		infoIO.pin = newpin;
	}

	return ret;
}

bool LadderDiagram::getIO(LadderElem *elem, pair<unsigned long, int> &pin, string name, eType type, tRequestIO infoIO, bool tryGeneralTypeFirst)
{
	infoIO.pin  = pin;
	infoIO.name = name;
	infoIO.type = type;

	bool ret = getIO(elem, infoIO, tryGeneralTypeFirst);
	
	pin = infoIO.pin;

	return ret;
}

bool LadderDiagram::getIO(LadderElem *elem, vector<tRequestIO> &vectorGetIO)
{
	bool ret = true;
	vector<tRequestIO>::iterator it;

	for(it = vectorGetIO.begin(); it != vectorGetIO.end(); it++) {
		ret &= getIO(elem, *it);
	}

	return ret;
}

unsigned long LadderDiagram::getIdIO(string name)
{
	return IO->getID(name);
}

eType LadderDiagram::getTypeIO(string previousName, string newName, eType default_type, bool isGeneric)
{
	unsigned long newId      = IO->getID(newName);
	unsigned long previousId = IO->getID(previousName);

	// Se variavel sem tipo, usa tipo default.
	eType type = default_type;

	mapDetails detailsIO = getDetailsIO(newId);
	if(detailsIO.type != eType_Pending) {
		type = detailsIO.type;
	} else {
		detailsIO = getDetailsIO(previousId);
		if(detailsIO.type != eType_Pending) {
			type = detailsIO.type;
		}
	}

	if(isGeneric && !IsGenericTypeIO(type)) {
		type = eType_General;
	}

	return type;
}

string LadderDiagram::getNameIO(pair<unsigned long, int> pin)
{
	if(pin.first) { // id valido!
		return IO->getName(pin.first);
	} else {
		char buf[1024];
		sprintf(buf, "%d", pin.second);
		return string(buf);
	}
}

string LadderDiagram::getNameIO(unsigned long id)
{
	return getNameIO(pair<unsigned long, int>(id, 0));
}

string LadderDiagram::getNameIObyIndex(unsigned int index)
{
	return IO->getName(IO->getID(index, true));
}

mapDetails LadderDiagram::getDetailsIO(unsigned long id)
{
	return IO->getDetails(id);
}

mapDetails LadderDiagram::getDetailsIO(string name)
{
	return getDetailsIO(IO->getID(name));
}

const char *LadderDiagram::getStringTypeIO(unsigned int index)
{
	return IO->getTypeString(IO->getDetails(IO->getID(index, true)).type);
}

unsigned int LadderDiagram::getCountIO(void)
{
	return IO->getCount();
}

void LadderDiagram::selectIO(unsigned int index)
{
	IO->Select(index);
}

bool LadderDiagram::IsInternalVarIO(string name)
{
	return IO->IsInternalVar(name);
}

bool LadderDiagram::IsInternalFlagIO(string name)
{
	return IO->IsInternalFlag(name);
}

string LadderDiagram::getInternalVarNameIO(string name)
{
	return IO->getInternalVarName(name);
}

vector< pair<string, string> > LadderDiagram::getInternalVarVectorIO(void)
{
	return IO->getInternalVarVector();
}

bool LadderDiagram::equalsNameIO(string name1, string name2)
{
	return name1 == name2;
}

bool LadderDiagram::IsValidNumber(string varnumber)
{
	const char *number = varnumber.c_str();

	if(!IsNumber(number))
		return false;

	number++;
	while(*number) {
		if(*number < '0' || *number > '9')
			return false;

		number++;
	}

	return true;
}

void LadderDiagram::sortIO(eSortBy sortby)
{
	IO->Sort(sortby);
	IO->updateGUI();
}

bool LadderDiagram::UartFunctionUsed(void)
{
	vector<LadderCircuit>::size_type i;

	for(i = 0; i < rungs.size(); i++) {
		if(rungs[i]->rung->UartFunctionUsed())
			return true;
	}

	return IO->UartFunctionUsed();
}

bool LadderDiagram::PwmFunctionUsed (void)
{
	return IO->PwmFunctionUsed ();
}

int LadderDiagram::SearchAndReplace(string sSearchText, string sNewText, eSearchAndReplaceMode mode)
{
	int rungMatches, matches = 0;
	unsigned long idSearch = IO->getID(sSearchText);
	LadderElem *firstElem = rungs[0]->rung->getFirstElement();

	if(idSearch == IO->getID(sNewText)) {
		// Textos equivalentes ou invalidos! De qualquer forma, nao podemos continuar
		return -1;
	}

	if(context.SelectedElem == nullptr) {
		SelectElement(firstElem, SELECTED_RIGHT);

		if(context.SelectedElem == nullptr) { // Ainda nulo? Nao deveria...
			return -1; // Retorna erro pois nao podemos iniciar a busca sem um elemento selecionado
		}
	}

	// Se existe I/O sendo pesquisado, continua
	if(idSearch > 0) {
		LadderElem *refElem = nullptr;
		unsigned int i, startRung = 0, count = rungs.size();
		bool doSearchAndReplace = true, firstPass = true, refElemNotFirstNorNull = false;

		// Iniciando a busca. Aqui criamos um checkpoint para que operacoes de substituicao possam ser desfeitas
		// Nao precisamos verificar se estamos apenas buscando. Se nada for registrado, o checkpoint sera descartado.
		CheckpointBegin(_("Localizar e Substituir"));

		// A busca pode partir de um elemento de referencia ou do inicio.
		// O ponteiro para o elemento de referencia deve ser nulo se deve iniciar a busca do primeiro elemento
		// Quando devemos buscar o proximo elemento, iniciamos a busca a partir da linha contendo o elemento atual
		// pois nao faz sentido iniciar a logica de uma linha anterior ao elemento de referencia
		if(mode == eSearchAndReplaceMode_FindNext || mode == eSearchAndReplaceMode_ReplaceNext) {
			refElem = context.SelectedElem;
			// Podemos seguramente converter para unsigned int aqui pois se nao houvesse elemento selecionado,
			// ja teriamos retornado no inicio desta funcao
			startRung = (unsigned int)RungContainingSelected();
		}

		if(refElem != nullptr && refElem != firstElem) {
			refElemNotFirstNorNull = true;
		}

		do {
			// Agora fazemos a busca, partindo da linha e elemento de referencia carregados no codigo acima.
			for(i = startRung; i < count; i++) {
				rungMatches = rungs[i]->rung->SearchAndReplace(&refElem, idSearch, sNewText, mode);
				if(rungMatches < 0) { // Ocorreu um erro ao realizar a busca! Aborta...
					CheckpointRollback();
					matches = -1;
					break;
				}

				matches += rungMatches;

				if(mode != eSearchAndReplaceMode_ReplaceAll && matches > 0) {
					// Estava buscando por uma correspondencia e encontrou! interrompe a busca.
					SelectElement(refElem, SELECTED_RIGHT);

					// Operacao finalizada, encerra o breakpoint
					CheckpointEnd();

					// Atualiza o contexto
					updateContext();

					// Retorna o numero de correspondencias encontrada
					return matches;
				}
			}

			// Terminamos uma busca, devemos checar se devemos reiniciar a busca do comeco.
			// Se o elemento de referencia nao for nulo e o modo seja Substituir Tudo ou se
			// nao houve correspondencias e for a primeira passagem, reinicia da primeira linha
			if(refElemNotFirstNorNull && firstPass && matches >= 0 &&
				(mode == eSearchAndReplaceMode_ReplaceAll || matches == 0)) {
					count              = startRung + 1;
					startRung          = 0;
					firstPass          = false;
					doSearchAndReplace = true;
			} else {
					doSearchAndReplace = false;
			}
		} while(doSearchAndReplace);

		// Operacao finalizada, encerra o breakpoint
		CheckpointEnd();

		// Atualiza o contexto
		updateContext();
	}

	return matches;
}

bool LadderDiagram::IsValidVarName(string varname)
{
	bool first = true;
	const char *name = varname.c_str();

	if(varname.size() == 0) return false; // If text in white return false

	while(*name) {
		// If char isn't letter nor '_' or is number in the first position, returns false
		if((toupper(*name) < 'A' || toupper(*name) > 'Z') && *name != '_' && !(!first && (*name >= '0' && *name <= '9')))
			return false;

		name++;
		first = false;
	}

	return true;
}

bool LadderDiagram::IsValidNameAndType(unsigned long id, string name, eType type, const char *FieldName, unsigned int Rules, int MinVal, int MaxVal, eReply *reply)
{
	// Se reply nao tiver sido fornecido, usa variavel local apenas para nao ficar com ponteiro nulo.
	eReply dummy;
	if(reply == nullptr) {
		reply = &dummy;
	}

	*reply = eReply_Cancel;

	string old_name = IO->getName(id);
	if(id &&old_name.size() == 0) return false; // id invalido!

	int val;
	bool ret = false;
	eType current_type;
	bool  name_is_internal_var  = IO->IsInternalVar (name);
	bool  name_is_internal_flag = IO->IsInternalFlag(name);
	bool  name_is_reserved      = IO->IsReserved(name);
	bool  name_is_number        = IsNumber(name.c_str());

	// As variaveis internas sao variaveis que podem ser utilizadas pela logica mas que precisam
	// ja existir previamente. Sao criadas por certos elementos como leitura de encoder, por exemplo.
	// Dessa forma, se a variavel ainda nao existir, devera ser considerada como de uso reservado.
	if(name_is_internal_var && getDetailsIO(name).countRequestInt == 0) {
		name_is_reserved = true; // Variavel interna sem request para int indica variavel nao utilizada.
	}

	// Para variaveis internas, o tipo deve ser obrigatoriamente Geral.
	if(name_is_internal_var && type != eType_General) {
		ShowDialog(eDialogType_Message, false, _("Tipo Inválido"), _("Conflito entre tipos! Operação não permitida."));
	}

	unsigned long newid = IO->getID(name);
	mapDetails newdetailsIO = IO->getDetails(newid);
	mapDetails oldDetailsIO = IO->getDetails(id);

	if(name == old_name && type == oldDetailsIO.type) { // Nenhuma mudanca! retorna OK...
		return true;
	}

	if(newid > 0) {
		current_type = newdetailsIO.type;
	} else {
		current_type = eType_Pending;
	}

	// Check for Internal Flags and Reserved Names restrictions
	if((type != eType_InternalFlag && name_is_internal_flag) || name_is_reserved) {
		ShowDialog(eDialogType_Message, false, _("Nome Inválido"), _("Nome '%s' reservado para uso interno, favor escolher outro nome."), name.c_str());
		return false;
	} else  if(type == eType_InternalFlag && !name_is_internal_flag) {
		ShowDialog(eDialogType_Message, false, _("Nome Inválido"), _("Para o tipo 'Flag Interna' é obrigatório selecionar um item da lista."));
		return false;
	}

	// Check for Variable and Number restrictions
	if(!name_is_number && !IsValidVarName(name)) {
		ShowDialog(eDialogType_Message, false, _("Nome Inválido"), _("%s '%s' inválido!\n\nVariável: Apenas letras (A a Z), números ou _ (underline) e não iniciar com número\nNúmero: Apenas números, podendo iniciar por - (menos)"), FieldName ? FieldName : _("Nome"), name.c_str());
		return false;
	} else if((Rules & VALIDATE_IS_VAR) && name_is_number) {
		ShowDialog(eDialogType_Message, false, _("Nome Inválido"), _("'%s' não pode ser número!"), FieldName ? FieldName : name.c_str());
		return false;
	} else if((Rules & VALIDATE_IS_NUMBER) && !name_is_number) {
		ShowDialog(eDialogType_Message, false, _("Número Inválido"), _("'%s' deve ser número!"), FieldName ? FieldName : name.c_str());
		return false;
	} else if(name_is_number && !IsValidNumber(name)) {
		ShowDialog(eDialogType_Message, false, _("Número Inválido"), _("Número '%s' inválido!"), name.c_str());
		return false;
	}

	// Check for Number Limits
	if((MinVal || MaxVal) && (Rules & (VALIDATE_IS_NUMBER | VALIDATE_IS_VAR_OR_NUMBER)) && name_is_number) {
		val = atoi(name.c_str());
		if(val < MinVal || val > MaxVal) {
			ShowDialog(eDialogType_Message, false, _("Número Inválido"), _("'%s' fora dos limites! Deve estar entre %d e %d."), FieldName ? FieldName : name.c_str(), MinVal, MaxVal);
			return false;
		}
	}

	// Check for Type restrictions
	if(!name_is_number) {
		// If types must match and types are different or cannot accept io_pending type, generates an error
		if((Rules & VALIDATE_TYPES_MUST_MATCH) && current_type != type && !((current_type == eType_Pending || type == eType_Pending) && (Rules & VALIDATE_ACCEPT_IO_PENDING))) {
			ShowDialog(eDialogType_Message, false, _("Tipo Inválido"), _("Conflito entre tipos! Operação não permitida."));
		} else if(type == eType_DigInput || type == eType_DigOutput || type == eType_InternalRelay) {
			if(type == current_type) { // no type change, ok!
				ret = true;
			} else if(current_type == eType_Pending) { // Inexistent name, ok!
				ret = true;
			} else if(type == eType_DigInput && (current_type == eType_DigOutput || current_type == eType_InternalRelay) && !acceptIO(newid, type)) {
				*reply = ShowDialog(eDialogType_Message, true, _("Saída em uso"), _("Não é possível alterar para Entrada.\r\nSerá utilizado o tipo atual."));
				if(*reply != eReply_Cancel) {
					ret = true;
				}
			} else if(!_stricmp(old_name.c_str(), name.c_str())) { // name not changed, ok!
				ret = true;
			// name changed, check for type changes
			} else if(type == eType_InternalRelay && current_type == eType_DigOutput) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(eDialogType_Message, false, _("Tipo inválido"), _("Conflito de tipos: Rele Interno <-> Saída"));
				} else {
					*reply = ShowDialog(eDialogType_Question, true, _("Confirmar alteração de Saída para Rele Interno"), _("Já existe uma Saída com este nome. Alterar para Rele Interno?"));
					if(*reply != eReply_Cancel) {
						ret = true;
					}
				}
			} else if(type == eType_DigOutput && current_type == eType_InternalRelay) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(eDialogType_Message, false, _("Tipo inválido"), _("Conflito de tipos: Rele Interno <-> Saída"));
				} else {
					*reply = ShowDialog(eDialogType_Question, true, _("Confirmar alteração de Rele Interno para Saída"), _("Já existe um Rele Interno com este nome. Alterar para Saída?"));
					if(*reply != eReply_Cancel) {
						ret = true;
					}
				}
			} else if(type == eType_DigOutput && current_type == eType_DigInput) { // changing existent input to output, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(eDialogType_Message, false, _("Tipo inválido"), _("Conflito de tipos: Entrada <-> Saída"));
				} else {
					*reply = ShowDialog(eDialogType_Question, true, _("Confirmar alteração de Entrada para Saída"), _("Já existe uma Entrada com este nome. Alterar para Saída?"));
					if(*reply != eReply_Cancel) {
						ret = true;
					}
				}
			} else if(type == eType_InternalRelay && current_type == eType_DigInput) { // changing existent input to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(eDialogType_Message, false, _("Tipo inválido"), _("Conflito de tipos: Rele Interno <-> Entrada"));
				} else {
					*reply = ShowDialog(eDialogType_Question, true, _("Confirmar alteração de Entrada para Relé Interno"), _("Já existe uma Entrada com este nome. Alterar para Relé Interno?"));
					if(*reply != eReply_Cancel) {
						ret = true;
					}
				}
			} else { // no wrong conditions, ok!
				ret = true;
			}
		} else if(current_type == eType_Counter || current_type == eType_TOF || current_type == eType_TON) {
			if(type == eType_Counter || type == eType_TOF || type == eType_TON) {
				ret = true;
			} else {
				ShowDialog(eDialogType_Message, false, _("Nome Inválido"), _("'%s' deve ser um contador ou timer!"), name.c_str());
			}
		} else {
			ret = true;
		}
	} else {
		ret = true;
	}

	return ret;
}

bool LadderDiagram::IsValidNameAndType(unsigned long id, string name, eType type, eReply *reply)
{
	return IsValidNameAndType(id, name, type, NULL, VALIDATE_IS_VAR, 0, 0, reply);
}

eReply LadderDiagram::ShowValidateDialog(bool isError, const char *msg)
{
	eReply reply;
	if(isError) {
		reply = ShowDialog(eDialogType_Message, false, _("Erro ao validar diagrama!"), msg);
	} else {
		reply = ShowDialog(eDialogType_Message, true , _("Atenção"                  ), msg);
	}

	return reply;
}

eValidateResult LadderDiagram::Validate(void)
{
	char msg_error[1024] = "", msg_warning[1024] = "";
	int  WarningPersist = 0, CountPWM = 0;
	eValidateResult ret = eValidateResult_OK;

	// Valida o mapa de I/Os
	// Na simulacao valida apenas os nomes, nao as atribuicoes
	if(IO->Validate(context.inSimulationMode ? eValidateIO_OnlyNames : eValidateIO_Full) == false) {
		ret = eValidateResult_Error;
	}

	// Valida diagrama
	if(ret != eValidateResult_Error) {
		vector<LadderRung *>::size_type len = rungs.size(), i;

		for(i = 0; i < len; i++) {
			if(rungs[i]->rung->IsEmpty()) {
				SelectElement(rungs[i]->rung->getFirstElement(), SELECTED_RIGHT);
				Error(_("Linha Vazia; apague ou adicione instruções antes de simular/compilar."));
				ret = eValidateResult_Error;
				break;
			}
		}
	}

	// Validate Generated IntCode
	if(ret != eValidateResult_Error) {
		vector<IntOp> vectorIntCode = getVectorIntCode();
		vector<IntOp>::size_type IntCodeLen = vectorIntCode.size(), i;

		for(i = 0; i < IntCodeLen; i++) {
			if(vectorIntCode[i].op == INT_READ_ADC && IO->getDetails(IO->getID(vectorIntCode[i].name1)).type != eType_ReadADC) {
				sprintf(msg_error, _("Variável A/D '%s' usada em lógica incompatível!"), vectorIntCode[i].name1);
			}

			if(vectorIntCode[i].op == INT_EEPROM_READ && !WarningPersist) {
				WarningPersist = 1;
				sprintf(msg_warning, _("Variáveis persistentes devem ser usadas cautelosamente. Excesso no uso pode interferir no desempenho da execução do diagrama ladder e reduzir a vida útil do CLP.\nA memória interna possui um limite no número de gravações."));
			}

			if(vectorIntCode[i].op == INT_SET_PWM) {
				CountPWM++;
				if(CountPWM == 4) // Each ELEM_PWM generates two INT_SET_PWM
					sprintf(msg_warning, _("Cuidado ao utilizar mais de uma instrução PWM em sua lógica.\nSe duas instruções PWM forem ativadas ao mesmo tempo, o PWM não funcionará corretamente."));
			}

			if(msg_error[0]) {
				ShowValidateDialog(true, msg_error);
				ret = eValidateResult_Error;
				break;
			}

			if(msg_warning[0]) {
				eReply reply = ShowValidateDialog(false, msg_warning);
				if(reply == eReply_Cancel) { // Se usuario escolheu cancelar, interrompe validacao e retorna erro
					ret = eValidateResult_Error;
					break;
				}
				ret = eValidateResult_Warning;
				msg_warning[0] = '\0';
			}
		}
	}

	return ret;
}

// Funcao que checa se um tipo de I/O pode ser usado em objeto de tipo Geral
bool LadderDiagram::IsGenericTypeIO(eType type)
{
	switch(type) {
		case eType_Reserved:
		case eType_General:
		case eType_Counter:
		case eType_ReadADC:
		case eType_SetDAC:
		case eType_ReadEnc:
		case eType_ResetEnc:
		case eType_ReadUSS:
		case eType_WriteUSS:
		case eType_ReadModbus:
		case eType_WriteModbus:
		case eType_PWM:
		case eType_RxUART:
		case eType_TxUART:
		case eType_ReadYaskawa:
		case eType_WriteYaskawa:
			return true;
	}

	return false;
}

string LadderDiagram::getPortNameIO(int index)
{
	return IO->getPortName(index);
}

string LadderDiagram::getPinNameIO(int index)
{
	return IO->getPinName(index);
}

int LadderDiagram::getPinFromDialogListIO(string name)
{
	return IO->getPinFromDialogList(name);
}

unsigned int LadderDiagram::LoadUnusedBitsIO(string name, int pin)
{
	return IO->LoadUnusedBits(name, pin);
}

vector<string> LadderDiagram::getVectorInternalFlagsIO(void)
{
	return IO->getVectorInternalFlags();
}

vector<eType> LadderDiagram::getGeneralTypes(void)
{
	vector<eType> types;

	types.push_back(eType_General     );
	types.push_back(eType_Counter     );
	types.push_back(eType_ReadADC     );
	types.push_back(eType_SetDAC      );
	types.push_back(eType_ReadEnc     );
	types.push_back(eType_ResetEnc    );
	types.push_back(eType_ReadUSS     );
	types.push_back(eType_WriteUSS    );
	types.push_back(eType_ReadModbus  );
	types.push_back(eType_WriteModbus );
	types.push_back(eType_PWM         );
	types.push_back(eType_RxUART      );
	types.push_back(eType_TxUART      );
	types.push_back(eType_ReadYaskawa );
	types.push_back(eType_WriteYaskawa);

	return types;
}

vector<string> LadderDiagram::getListIO(void)
{
	return IO->getList();
}

string LadderDiagram::getReservedNameIO(eType type)
{
	return IO->getReservedName(type);
}

pair<string, eType> LadderDiagram::getCachedIO(unsigned long id)
{
	return IO->getCachedIO(id);
}

// Funcoes relacionadas aos comandos de Desfazer / Refazer
void LadderDiagram::RegisterAction(UndoRedoAction Action, bool isUndo)
{
	// Nunca registra uma acao com o contexto marcado como tela bloqueada ou ao desfazer pode ser que a tela fique travada
	Action.contextBefore.isDrawBlocked = false;
	Action.contextAfter .isDrawBlocked = false;

	if(isUndo) {
		if(Action.action != eCheckpoint) {
			isCheckpointEmpty = false;
		}

		UndoList.push_back(Action);
	} else {
		RedoList.push_back(Action);
	}
}

bool LadderDiagram::ExecuteAction(bool isUndo, bool isDiscard, UndoRedoAction Action)
{
	// Se checkpoint, nada a fazer. Retorna como operacao executada com sucesso
	if(Action.action == eCheckpoint) return true;

	if(Action.io != nullptr) {
		return Action.io->DoUndoRedo(isUndo, isDiscard, Action);
	} else if(Action.elem != nullptr) {
		return Action.elem->DoUndoRedo(isUndo, isDiscard, Action);
	} else if(Action.subckt != nullptr) {
		return Action.subckt->DoUndoRedo(isUndo, isDiscard, Action);
	} else {
		return DoUndoRedo(isUndo, isDiscard, Action);
	}
}

void LadderDiagram::CheckpointBegin(string description)
{
	UndoRedoAction cp;

	// Se ja registrando acoes, descarta chekpoint
	if(CheckpointBeginCount++ > 0) return;

	CheckpointDoRollback = false;

	cp.Description = description;

	cp.action  = eCheckpoint; // 0 -> Checkpoint!
	cp.data    = nullptr;

	cp.io      = nullptr;
	cp.elem    = nullptr;
	cp.subckt  = nullptr;

	cp.contextBefore = cp.contextAfter = context;

	RegisterAction(cp);

	if(++CheckPointLevels > CheckPointLevelsMax) {
		DiscardCheckpoint();
	}
}

void LadderDiagram::CheckpointEnd(void)
{
	if(CheckpointBeginCount > 0) { // Nunca deveria ser zero aqui...
		CheckpointBeginCount--;

		if(CheckpointBeginCount == 0) {
			if(CheckpointDoRollback) {
				UndoRedoAction action;

				CheckpointDoRollback = false;
				CheckPointLevels--;

				if(UndoList.size() == 0) return; // Lista vazia

				do {
					action = UndoList.back();

					// Primeiro desfaz a acao
					ExecuteAction(true , false, action);

					// Restaura o contexto para o que era valido antes da acao desfeita
					context = action.contextBefore;

					// Agora descarta como Refazer pois a acao foi desfeita mas nao sera refeita...
					ExecuteAction(false, true , action);

					UndoList.pop_back();
				} while(action.action != eCheckpoint);
			} else if(isCheckpointEmpty) {
				// Fechado um checkpoint sem registro de acoes.
				// Devemos remover o marcador de checkpoint e diminuir 1 nivel
				UndoList.pop_back();
				CheckPointLevels--;
			} else {
				// Tudo ok, checkpoint valido!
				// Se estamos adicionando um checkpoint em Desfazer, a lista de Refazer se torna invalida!
				// Apagando a lista de Refazer...
				while(RedoList.size() > 0) {
					DiscardCheckpoint(false);
				}
			}

			isCheckpointEmpty = true;
		}
	}
}

void LadderDiagram::DiscardCheckpoint(bool isUndo)
{
	if(isUndo) {
		// Sempre a lista inicia por um checkpoint. Assim devemos remove-lo antes de iniciar o loop
		// Isso precisa ser feito pois o loop termina ao encontrar um checkpoint
		UndoList.pop_front();
		while(!UndoList.empty()) { // Executa as acoes ate achar um checkpoint ou a lista estiver vazia
			UndoRedoAction action = UndoList.front();

			if(action.action == eCheckpoint) { // Checkpoint atingido
				// Diminui a contagem de niveis de checkpoint
				CheckPointLevels--;

				break; // sai do loop!
			}

			// Descarta a acao
			ExecuteAction(true, true, action);

			// Exclui o elemento atual da lista apos executar sua acao
			UndoList.pop_front();
		}
	} else {
		while(RedoList.size() > 0) { // Executa as acoes enquanto a lista estiver alem do ponto inicial
			bool isCheckpoint = (RedoList[0].action == eCheckpoint);

			// Descarta a acao
			ExecuteAction(false, true, RedoList[0]);

			// Exclui o elemento atual da lista apos executar sua acao
			RedoList.erase(RedoList.begin());

			// Atingiu o checkpoint! Sai do loop...
			if(isCheckpoint) break;
		}

	}
}

void LadderDiagram::UndoRedo(bool isUndo)
{
	bool ignoreFirstCheckpoint;
	deque<UndoRedoAction> *List;

	// Durante esta operacao, nao devemos desenhar a tela para evitar a lista de I/Os sendo recarregada
	context.isDrawBlocked = true;

	// Se houver checkpoint em aberto, fecha agora
	while(CheckpointBeginCount > 0) {
		CheckpointEnd();
	}

	if(isUndo) {
		ignoreFirstCheckpoint = false;
		List = &UndoList;
	} else {
		ignoreFirstCheckpoint = true;
		List = &RedoList;
	}

	// Ao desfazer / refazer alguma acao, o diagrama sera alterado.
	// Dessa forma devemos forcar redesenhar a tela por completo
	NeedRedraw(true);

	while(!List->empty()) { // Executa as acoes ate achar um checkpoint ou a lista estiver vazia
		UndoRedoAction action = List->back();

		if(action.action == eCheckpoint) { // Checkpoint atingido
			RegisterAction(action, !isUndo);

			// Se devemos ignorar o primeiro checkpoint, apenas o remove e vai para o proximo
			if(ignoreFirstCheckpoint) {
				List->pop_back();
				ignoreFirstCheckpoint = false;
				continue;
			}

			// Se for Desfazer, remove o checkpoint e diminui a contagem de niveis de checkpoint
			if(isUndo) {
				List->pop_back();
				CheckPointLevels--;
			} else {
				CheckPointLevels++;
				// Registramos a acao mas ela nao sera executada! Cancelamos o registro...
				UndoList.pop_back();
			}

			break; // sai do loop!
		}

		// Executa a acao
		ExecuteAction(isUndo, false, action);

		// Registra a acao na lista contraria a que estamos trabalhando
		RegisterAction(action, !isUndo);

		// Atualiza o contexto para o que era valido antes/depois da operacao
		if(isUndo) {
			context = action.contextBefore;
		} else {
			context = action.contextAfter;
		}

		// Exclui o elemento atual da lista apos executar sua acao
		List->pop_back();
	}

	if(!IsSelectedVisible()) {
		NeedScrollSelectedIntoView = true;
	}

	SetMenusEnabled(&context);

	// Ao registrarmos acoes, a funcao altera a flag isCheckpointEmpty para indicar que houve um
	// registro, de forma que CheckpointEnd nao remova o checkpoint por pensar que ele esta vazio.
	// Porem isso nao vale para Undo/Redo entao limpamos a flag agora.
	isCheckpointEmpty = true;

	// Operacao finalizada, a tela ja pode ser desbloqueada
	context.isDrawBlocked = false;
}

bool LadderDiagram::DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	UndoRedoData *data = (UndoRedoData *)action.data;
	switch(action.action) {
	case ePushRung: {
		if(isDiscard) {
			// Nada a fazer...
		} else {
			int offset = data->PushRung.isUp ? -1 : +1;
			LadderRung *tmp                    = rungs[data->PushRung.pos];
			rungs[data->PushRung.pos         ] = rungs[data->PushRung.pos + offset];
			rungs[data->PushRung.pos + offset] = tmp;
		}
		break;
	}

	case eNewRung: {
		if(isDiscard) {
			// Se estiver descartando a acao para refazer a inclusao da linha, significa que ela havia sido
			// removida do circuito e nesse momento a acao para sua inclusao esta sendo descartada.
			// Assim essa linha nao sera mais incluida no circuito e deve ser desalocada da memoria.
			if(!IsUndo) {
				delete data->NewRung.rung->rung;
				delete data->NewRung.rung;
			}
		} else if(IsUndo) {
			rungs.erase(rungs.begin() + data->NewRung.pos);
		} else {
			rungs.insert(rungs.begin() + data->NewRung.pos, data->NewRung.rung);
		}

		break;
	}

	case eDelRung: {
		if(isDiscard) {
			// Se estiver descartando a acao para desfazer a exclusao da linha, significa que ela havia sido
			// removida do circuito e nesse momento a acao para sua exclusao esta sendo descartada.
			// Assim essa linha nao sera mais incluida no circuito e deve ser desalocada da memoria.
			if(IsUndo) {
				delete data->DelRung.rung->rung;
				delete data->DelRung.rung;
			}
		} else if(IsUndo) {
			rungs.insert(rungs.begin() + data->DelRung.pos, data->DelRung.rung);
		} else {
			rungs.erase(rungs.begin() + data->DelRung.pos);
		}

		break;
	}

	case eSettingsChanged: {
		if(isDiscard) {
			delete data->SettingsChanged.settings; // Descarrega a estrutura de configuracoes
		} else {
			tLadderSettings oldsettings       = LadderSettings;
			LadderSettings                    = *(data->SettingsChanged.settings);
			*(data->SettingsChanged.settings) = oldsettings;
		}
		break;
	}

	case eMbNodeCreate: {
		if(isDiscard) {
			// Se estiver descartando a acao para refazer a inclusao do node, significa que ele havia sido
			// removido do circuito e nesse momento a acao para sua inclusao esta sendo descartada.
			// Assim esse node nao sera mais cadastrado e deve ser desalocado da memoria.
			if(!IsUndo) {
				delete data->MbNodeCreate.nl;
			}
		} else if(IsUndo) {
			vectorMbNodeList.pop_back();
		} else {
			vectorMbNodeList.push_back(data->MbNodeCreate.nl);
		}

		break;
	}

	case eMbNodeUpdate: {
		if(isDiscard) {
			delete data->MbNodeUpdate.node;
		} else {
			LadderMbNode oldnode                             = vectorMbNodeList[data->MbNodeUpdate.index]->node;
			vectorMbNodeList[data->MbNodeUpdate.index]->node = *(data->MbNodeUpdate.node);
			*(data->MbNodeUpdate.node)                       = oldnode;
		}

		break;
	}

	case eMbNodeDelete: {
		if(isDiscard) {
			// Se estiver descartando a acao para desfazer a exclusao do node, significa que ele havia sido
			// removido do circuito e nesse momento a acao para sua exclusao esta sendo descartada.
			// Assim esse node nao sera mais cadastrado e deve ser desalocado da memoria.
			if(IsUndo) {
				delete data->MbNodeDelete.nl;
			}
		} else if(IsUndo) {
			vectorMbNodeList.insert(vectorMbNodeList.begin() + data->MbNodeDelete.index, data->MbNodeDelete.nl);
		} else {
			vectorMbNodeList.erase(vectorMbNodeList.begin() + data->MbNodeDelete.index);
		}

		break;
	}

	case eMbNodeRef: {
		if(isDiscard) {
			// Nada a fazer...
		} else {
			int offset = data->MbNodeRef.isAddRef ? +1 : -1;
			if(IsUndo) {
				offset *= -1;
			}

			vectorMbNodeList[data->MbNodeRef.index]->NodeCount += offset;
		}

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
