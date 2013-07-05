#include "poptools.h"

// Funcao auxiliar que entrega um contexto "vazio"
LadderContext getEmptyContext(void)
{
	LadderContext context;

	context.canNegate        = false;
	context.canNormal        = false;
	context.canSetOnly       = false;
	context.canResetOnly     = false;
	context.canPushUp        = false;
	context.canPushDown      = false;
	context.canDelete        = false;
	context.canDeleteRung    = false;
	context.canInsertEnd     = false;
	context.canInsertOther   = false;
	context.canInsertComment = false;

	context.SelectedCircuit  = nullptr;
	context.SelectedElem     = nullptr;
	context.SelectedState    = SELECTED_NONE;

	context.Diagram          = nullptr;
	context.ParallelStart    = nullptr;

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

//Classe LadderElem
void LadderElem::Init(void)
{
	isEndOfLine   = false;
	poweredAfter  = false;
}

LadderElem::LadderElem(void)
{
	Init();
}

LadderElem::LadderElem(bool EOL, bool Comment, bool Formatted, int elemWhich)
{
	Init();

	which       = elemWhich;
	isEndOfLine = EOL;
	isComment   = Comment;
	isFormatted = Formatted;
}

void LadderElem::setProperties(LadderContext context, void *propData)
{
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
	internalSetProperties(propData);

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
	updateIO(true); // Descarta o I/O referenciado quando da criacao do objeto
	return internalLoad(f, version);
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
			if(ret) {
				delete data->SetProp.data;
			}
		} else {
			olddata = getProperties();
			internalSetProperties(data->SetProp.data);
			delete data->SetProp.data;
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
LadderElemPlaceHolder::LadderElemPlaceHolder(LadderDiagram *diagram) : LadderElem(false, false, false, ELEM_PLACEHOLDER)
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

LadderElem *LadderElemPlaceHolder::Clone(void)
{
	LadderElemPlaceHolder *clone = new LadderElemPlaceHolder(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados

	return clone;
}

bool LadderElemPlaceHolder::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemComment
LadderElemComment::LadderElemComment(LadderDiagram *diagram) : LadderElem(false, true, false, ELEM_COMMENT)
{
	Diagram  = diagram;
	prop.str = _("--add comment here--");
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

void LadderElemComment::internalSetProperties(void *data)
{
	LadderElemCommentProp *newProp = (LadderElemCommentProp *)data;

	prop = *newProp;
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

LadderElem *LadderElemComment::Clone(void)
{
	LadderElemComment *clone = new LadderElemComment(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemComment::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemContact
LadderElemContact::LadderElemContact(LadderDiagram *diagram) : LadderElem(false, false, false, ELEM_CONTACTS)
{
	Diagram      = diagram;

	prop.negated = false;
	prop.idName  = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("in"), true, eType_DigInput);
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

	// TODO: Internacionalizar
	switch(detailsIO.type) {
	case eType_DigInput:
		ch = 'E';
		break;

	case eType_DigOutput:
		ch = 'S';
		break;

	case eType_InternalRelay:
		ch = 'R';
		break;

	case eType_InternalFlag:
		ch = 'F';
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

void LadderElemContact::internalSetProperties(void *data)
{
	LadderElemContactProp *newProp = (LadderElemContactProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_bool (f, &prop.negated);
}

LadderElem *LadderElemContact::Clone(void)
{
	LadderElemContact *clone = new LadderElemContact(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

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

void LadderElemContact::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("in"), true, eType_DigInput, isDiscard);
}

bool LadderElemContact::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemCoil
LadderElemCoil::LadderElemCoil(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_COIL)
{
	Diagram        = diagram;

	prop.negated   = false;
	prop.setOnly   = false;
	prop.resetOnly = false;
	prop.idName    = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("out"), true, eType_DigOutput);
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

	// TODO: Internacionalizar
	sprintf(name_with_type, "%s (%c)", name.c_str(), detailsIO.type == eType_DigOutput ? 'S' : 'R');

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

void LadderElemCoil::internalSetProperties(void *data)
{
	LadderElemCoilProp *newProp = (LadderElemCoilProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_bool (f, &prop.negated) &&
		fread_bool (f, &prop.resetOnly) &&
		fread_bool (f, &prop.setOnly);
}

LadderElem *LadderElemCoil::Clone(void)
{
	LadderElemCoil *clone = new LadderElemCoil(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemCoil::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_DigOutput && type != eType_InternalRelay && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemCoil::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("out"), true, eType_DigOutput, isDiscard);
}

bool LadderElemCoil::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemTimer
LadderElemTimer::LadderElemTimer(LadderDiagram *diagram, int which) : LadderElem(which == ELEM_RTO ? true : false, false, false, which)
{
	Diagram     = diagram;

	prop.delay  = 100000;
	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, getTimerTypeIO(getWhich()));
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
        Error(_("Timer period too short (needs faster cycle time)."));
        CompileError();
    }
    if(period >= (((unsigned int)(1 << 31)) - 1)) {
        Error(_("Timer period too long (max 2147483647 times cycle time); use a "
            "slower cycle time."));
        CompileError();
    }

    return period;
}

pair<string, string> LadderElemTimer::DrawTXT(void)
{
	char *s;
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
	return getWhich() == ELEM_RTO ? context.canInsertEnd : context.canInsertOther;
}

void LadderElemTimer::internalSetProperties(void *data)
{
	LadderElemTimerProp *newProp = (LadderElemTimerProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_int  (f, &prop.delay);
}

LadderElem *LadderElemTimer::Clone(void)
{
	LadderElemTimer *clone = new LadderElemTimer(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemTimer::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != getTimerTypeIO(getWhich()) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemTimer::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, getTimerTypeIO(getWhich()), isDiscard);
}

bool LadderElemTimer::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemRTC
LadderElemRTC::LadderElemRTC(LadderDiagram *diagram) : LadderElem(false, false, false, ELEM_RTC)
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

			sprintf(linha ? bufe : bufs, "[\x01%s %02d:%02d:%02d\x02]", _("SMTWTFS"),
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
	return true;
}

bool LadderElemRTC::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void LadderElemRTC::internalSetProperties(void *data)
{
	LadderElemRTCProp *newProp = (LadderElemRTCProp *)data;

	prop = *newProp;
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

LadderElem *LadderElemRTC::Clone(void)
{
	LadderElemRTC *clone = new LadderElemRTC(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemRTC::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemCounter
LadderElemCounter::LadderElemCounter(LadderDiagram *diagram, int which) : LadderElem(which == ELEM_CTC ? true : false, false, false, which)
{
	Diagram   = diagram;
	prop.max  = 0;
	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, eType_Counter);
}

pair<string, string> LadderElemCounter::DrawTXT(void)
{
	int which = getWhich();
	char *s, op, buf[256];

	if(which == ELEM_CTC) {
		sprintf(buf, _("{\x01""CTC\x02 0:%d}"), prop.max);
	} else {
		if(which == ELEM_CTU) {
			s = _("CTU");
			op = '>';
		} else if(which == ELEM_CTD) {
			s = _("CTD");
			op = '<';
		} else oops();

		sprintf(buf, "[\x01%s\x02 %c=%d]", s, op, prop.max);
	}

	return pair<string, string>(Diagram->getNameIO(prop.idName.first), buf);
}

bool LadderElemCounter::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName.first);
	const char *name = sname.c_str();

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

			ic.Op(INT_IF_VARIABLE_LES_LITERAL, name, prop.max);
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

			ic.Op(INT_IF_VARIABLE_GRT_VARIABLE, name, prop.max);
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
                    ic.Op(INT_IF_VARIABLE_LES_LITERAL, name, prop.max+1);
                    ic.Op(INT_ELSE);
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

void LadderElemCounter::internalSetProperties(void *data)
{
	LadderElemCounterProp *newProp = (LadderElemCounterProp *)data;

	prop = *newProp;
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
		fwrite_ulong(f, prop.idName.first) &&
		fwrite_int  (f, prop.idName.second) &&
		fwrite_int  (f, prop.max);
}

bool LadderElemCounter::internalLoad(FILE *f, unsigned int version)
{
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_int  (f, &prop.max);
}

LadderElem *LadderElemCounter::Clone(void)
{
	LadderElemCounter *clone = new LadderElemCounter(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemCounter::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_Counter && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemCounter::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_Counter, isDiscard);
}

bool LadderElemCounter::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemReset
LadderElemReset::LadderElemReset(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_RES)
{
	Diagram   = diagram;

	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, eType_Pending);
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

void LadderElemReset::internalSetProperties(void *data)
{
	LadderElemResetProp *newProp = (LadderElemResetProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);
}

LadderElem *LadderElemReset::Clone(void)
{
	LadderElemReset *clone = new LadderElemReset(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

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

void LadderElemReset::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_Pending, isDiscard);
}

bool LadderElemReset::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemOneShot
LadderElemOneShot::LadderElemOneShot(LadderDiagram *diagram, int which) : LadderElem(false, false, false, which)
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

LadderElem *LadderElemOneShot::Clone(void)
{
	LadderElemOneShot *clone = new LadderElemOneShot(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados

	return clone;
}

bool LadderElemOneShot::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemCmp
LadderElemCmp::LadderElemCmp(LadderDiagram *diagram, int which) : LadderElem(false, false, false, which)
{
	Diagram  = diagram;

	prop.idOp1 = pair<unsigned long, int>(0, 0);
	prop.idOp2 = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idOp1, _("var"), false, eType_General);
	Diagram->getIO(prop.idOp2,   "1"   , false, eType_Pending);
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

	char s1[POS_WIDTH+10], s2[POS_WIDTH+10];
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

void LadderElemCmp::internalSetProperties(void *data)
{
	LadderElemCmpProp *newProp = (LadderElemCmpProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idOp1.first) &&
		fread_int  (f, &prop.idOp1.second) &&
		fread_ulong(f, &prop.idOp2.first) &&
		fread_int  (f, &prop.idOp2.second);
}

LadderElem *LadderElemCmp::Clone(void)
{
	LadderElemCmp *clone = new LadderElemCmp(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemCmp::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idOp1.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	if(id == prop.idOp2.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemCmp::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idOp1, _("var"), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idOp2,   "1"   , false, eType_Pending, isDiscard);
}

bool LadderElemCmp::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemMath
LadderElemMath::LadderElemMath(LadderDiagram *diagram, int which) : LadderElem(true, false, false, which)
{
	Diagram   = diagram;

	prop.idDest = pair<unsigned long, int>(0, 0);
	prop.idOp1  = pair<unsigned long, int>(0, 0);
	prop.idOp2  = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idDest, _("dest"), false, eType_General);
	Diagram->getIO(prop.idOp1 , _("src" ), false, eType_General);
	Diagram->getIO(prop.idOp2 ,   "1"    , false, eType_Pending);
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
		Error(_("Math instruction: '%s' not a valid destination."), dest);
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

void LadderElemMath::internalSetProperties(void *data)
{
	LadderElemMathProp *newProp = (LadderElemMathProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idOp1.first) &&
		fread_int  (f, &prop.idOp1.second) &&
		fread_ulong(f, &prop.idOp2.first) &&
		fread_int  (f, &prop.idOp2.second);
}

LadderElem *LadderElemMath::Clone(void)
{
	LadderElemMath *clone = new LadderElemMath(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemMath::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idOp1.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	if(id == prop.idOp2.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	if(id == prop.idDest.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemMath::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idDest, _("dest"), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idOp1 , _("src" ), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idOp2 ,   "1"    , false, eType_Pending, isDiscard);
}

bool LadderElemMath::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemSqrt
LadderElemSqrt::LadderElemSqrt(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_SQRT)
{
	Diagram   = diagram;

	prop.idDest = pair<unsigned long, int>(0, 0);
	prop.idSrc  = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idDest, _("dest"), false, eType_General);
	Diagram->getIO(prop.idSrc , _("src" ), false, eType_General);
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

void LadderElemSqrt::internalSetProperties(void *data)
{
	LadderElemSqrtProp *newProp = (LadderElemSqrtProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idSrc.first) &&
		fread_int  (f, &prop.idSrc.second);
}

LadderElem *LadderElemSqrt::Clone(void)
{
	LadderElemSqrt *clone = new LadderElemSqrt(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemSqrt::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idSrc.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	if(id == prop.idDest.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemSqrt::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idDest, _("dest"), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idSrc , _("src" ), false, eType_General, isDiscard);
}

bool LadderElemSqrt::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemRand
LadderElemRand::LadderElemRand(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_RAND)
{
	Diagram  = diagram;

	prop.idVar = pair<unsigned long, int>(0, 0);
	prop.idMin = pair<unsigned long, int>(0, 0);
	prop.idMax = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idVar, _("var"), false, eType_General);
	Diagram->getIO(prop.idMin,     "0" , false, eType_Pending);
	Diagram->getIO(prop.idMax,   "100" , false, eType_Pending);
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

void LadderElemRand::internalSetProperties(void *data)
{
	LadderElemRandProp *newProp = (LadderElemRandProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idMin.first) &&
		fread_int  (f, &prop.idMin.second) &&
		fread_ulong(f, &prop.idVar.first) &&
		fread_int  (f, &prop.idVar.second) &&
		fread_ulong(f, &prop.idMax.first) &&
		fread_int  (f, &prop.idMax.second);
}

LadderElem *LadderElemRand::Clone(void)
{
	LadderElemRand *clone = new LadderElemRand(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemRand::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idVar.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	if(id == prop.idMin.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	if(id == prop.idMax.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemRand::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idVar, _("dest"), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idMin,     "0"  , false, eType_Pending, isDiscard);
	Diagram->updateIO(prop.idMax,   "100"  , false, eType_Pending, isDiscard);
}

bool LadderElemRand::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemAbs
LadderElemAbs::LadderElemAbs(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_ABS)
{
	Diagram   = diagram;

	prop.idDest = pair<unsigned long, int>(0, 0);
	prop.idSrc  = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idDest, _("dest"), false, eType_General);
	Diagram->getIO(prop.idSrc , _("src" ), false, eType_General);
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

void LadderElemAbs::internalSetProperties(void *data)
{
	LadderElemAbsProp *newProp = (LadderElemAbsProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idSrc.first) &&
		fread_int  (f, &prop.idSrc.second);
}

LadderElem *LadderElemAbs::Clone(void)
{
	LadderElemAbs *clone = new LadderElemAbs(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemAbs::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idSrc.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	if(id == prop.idDest.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemAbs::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idDest, _("dest"), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idSrc , _("src" ), false, eType_General, isDiscard);
}

bool LadderElemAbs::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemMove
LadderElemMove::LadderElemMove(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_MOVE)
{
	Diagram   = diagram;

	prop.idDest = pair<unsigned long, int>(0, 0);
	prop.idSrc  = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idDest, _("dest"), false, eType_General);
	Diagram->getIO(prop.idSrc , _("src" ), false, eType_General);
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
		Error(_("Move instruction: '%s' not a valid destination."), src);
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

void LadderElemMove::internalSetProperties(void *data)
{
	LadderElemMoveProp *newProp = (LadderElemMoveProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idDest.first) &&
		fread_int  (f, &prop.idDest.second) &&
		fread_ulong(f, &prop.idSrc.first) &&
		fread_int  (f, &prop.idSrc.second);
}

LadderElem *LadderElemMove::Clone(void)
{
	LadderElemMove *clone = new LadderElemMove(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemMove::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idSrc.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	if(id == prop.idDest.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemMove::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idDest, _("dest"), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idSrc , _("src" ), false, eType_General, isDiscard);
}

bool LadderElemMove::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemOpenShort
LadderElemOpenShort::LadderElemOpenShort(LadderDiagram *diagram, int which) : LadderElem(false, false, false, which)
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

LadderElem *LadderElemOpenShort::Clone(void)
{
	LadderElemOpenShort *clone = new LadderElemOpenShort(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados

	return clone;
}

bool LadderElemOpenShort::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemSetBit
LadderElemSetBit::LadderElemSetBit(LadderDiagram *diagram) : LadderElem(false, false, false, ELEM_PADDING)
{
	Diagram   = diagram;

	prop.bit    = 0;
	prop.set    = false;
	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, eType_General);
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

void LadderElemSetBit::internalSetProperties(void *data)
{
	LadderElemSetBitProp *newProp = (LadderElemSetBitProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_bool (f, &prop.set) &&
		fread_int  (f, &prop.bit);
}

LadderElem *LadderElemSetBit::Clone(void)
{
	LadderElemSetBit *clone = new LadderElemSetBit(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemSetBit::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemSetBit::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_General, isDiscard);
}

bool LadderElemSetBit::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemCheckBit
LadderElemCheckBit::LadderElemCheckBit(LadderDiagram *diagram) : LadderElem(false, false, false, ELEM_CHECK_BIT)
{
	Diagram   = diagram;

	prop.bit    = 0;
	prop.set    = false;
	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, eType_General);
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

void LadderElemCheckBit::internalSetProperties(void *data)
{
	LadderElemCheckBitProp *newProp = (LadderElemCheckBitProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_bool (f, &prop.set) &&
		fread_int  (f, &prop.bit);
}

LadderElem *LadderElemCheckBit::Clone(void)
{
	LadderElemCheckBit *clone = new LadderElemCheckBit(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemCheckBit::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemCheckBit::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_General, isDiscard);
}

bool LadderElemCheckBit::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemReadAdc
LadderElemReadAdc::LadderElemReadAdc(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_READ_ADC)
{
	Diagram   = diagram;

	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, eType_ReadADC);
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

	sprintf(txt, _("{READ ADC %s }"), GetNameADC());

	return pair<string, string>(Diagram->getNameIO(prop.idName), txt);
}

bool LadderElemReadAdc::internalGenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_READ_ADC, name);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemReadAdc::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemReadAdc::internalSetProperties(void *data)
{
	LadderElemReadAdcProp *newProp = (LadderElemReadAdcProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);
}

LadderElem *LadderElemReadAdc::Clone(void)
{
	LadderElemReadAdc *clone = new LadderElemReadAdc(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemReadAdc::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_ReadADC && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemReadAdc::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_ReadADC, isDiscard);
}

bool LadderElemReadAdc::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemSetDa
LadderElemSetDa::LadderElemSetDa(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_SET_DA)
{
	Diagram    = diagram;

	prop.mode   = ELEM_SET_DA_MODE_RAW;
	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, eType_SetDAC);
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

void LadderElemSetDa::internalSetProperties(void *data)
{
	LadderElemSetDaProp *newProp = (LadderElemSetDaProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second) &&
		fread_int  (f, &prop.mode);
}

LadderElem *LadderElemSetDa::Clone(void)
{
	LadderElemSetDa *clone = new LadderElemSetDa(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemSetDa::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_SetDAC && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemSetDa::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_SetDAC, isDiscard);
}

bool LadderElemSetDa::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemReadEnc
LadderElemReadEnc::LadderElemReadEnc(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_READ_ENC)
{
	Diagram   = diagram;

	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, eType_ReadEnc);
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

void LadderElemReadEnc::internalSetProperties(void *data)
{
	LadderElemReadEncProp *newProp = (LadderElemReadEncProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);
}

LadderElem *LadderElemReadEnc::Clone(void)
{
	LadderElemReadEnc *clone = new LadderElemReadEnc(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemReadEnc::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_ReadEnc && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemReadEnc::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_ReadEnc, isDiscard);
}

bool LadderElemReadEnc::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemResetEnc
LadderElemResetEnc::LadderElemResetEnc(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_RESET_ENC)
{
	Diagram   = diagram;

	prop.idName = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, eType_ResetEnc);
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

void LadderElemResetEnc::internalSetProperties(void *data)
{
	LadderElemResetEncProp *newProp = (LadderElemResetEncProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);
}

LadderElem *LadderElemResetEnc::Clone(void)
{
	LadderElemResetEnc *clone = new LadderElemResetEnc(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemResetEnc::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_ResetEnc && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemResetEnc::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_ResetEnc, isDiscard);
}

bool LadderElemResetEnc::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemMultisetDA
LadderElemMultisetDA::LadderElemMultisetDA(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_MULTISET_DA)
{
	Diagram                    = diagram;

	prop.gaint                 = 10;
	prop.gainr                 = 5;
	prop.initval               = DA_RESOLUTION - 1;
	prop.linear                = true;  // true = linear, false = curva
	prop.forward               = true;  // true = avanço, false = recuo
	prop.speedup               = false; // true = aceleração, false = desaceleração
	prop.StartFromCurrentValue = false; // false = Iniciar ou ir para zero, conforme speedup. true = partir do valor atual até o valor configurado
	prop.idTime                = pair<unsigned long, int>(0, 0);
	prop.idDesl                = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idTime,  "600", false, eType_Pending);
	Diagram->getIO(prop.idDesl, "2047", false, eType_Pending);
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

	strcpy(str_initval, desl);
	if(IsNumber(str_initval)) {
		int initval = atoi(str_initval);

		if (prop.type == 1)  // (mV)
			initval = static_cast<int>(DA_RESOLUTION * (initval / DA_VOLTAGE));
		else if (prop.type == 2) // (%)
			initval = static_cast<int>(DA_RESOLUTION * (initval / 100.0f));

		_itoa(initval, str_initval, 10);
	}

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

void LadderElemMultisetDA::internalSetProperties(void *data)
{
	LadderElemMultisetDAProp *newProp = (LadderElemMultisetDAProp *)data;

	prop = *newProp;
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
		fwrite_bool (f, prop.linear) &&
		fwrite_bool (f, prop.forward) &&
		fwrite_bool (f, prop.speedup) &&
		fwrite_bool (f, prop.StartFromCurrentValue) &&
		fwrite_ulong(f, prop.idDesl.first) &&
		fwrite_int  (f, prop.idDesl.second) &&
		fwrite_ulong(f, prop.idTime.first) &&
		fwrite_int  (f, prop.idTime.second);
}

bool LadderElemMultisetDA::internalLoad(FILE *f, unsigned int version)
{
	return
		fread_int  (f, &prop.gainr) &&
		fread_int  (f, &prop.gaint) &&
		fread_int  (f, &prop.initval) &&
		fread_bool (f, &prop.linear) &&
		fread_bool (f, &prop.forward) &&
		fread_bool (f, &prop.speedup) &&
		fread_bool (f, &prop.StartFromCurrentValue) &&
		fread_ulong(f, &prop.idDesl.first) &&
		fread_int  (f, &prop.idDesl.second) &&
		fread_ulong(f, &prop.idTime.first) &&
		fread_int  (f, &prop.idTime.second);
}

LadderElem *LadderElemMultisetDA::Clone(void)
{
	LadderElemMultisetDA *clone = new LadderElemMultisetDA(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemMultisetDA::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idTime.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	if(id == prop.idDesl.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemMultisetDA::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idTime,  "600", false, eType_Pending, isDiscard);
	Diagram->updateIO(prop.idDesl, "2047", false, eType_Pending, isDiscard);
}

bool LadderElemMultisetDA::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemUSS
LadderElemUSS::LadderElemUSS(LadderDiagram *diagram, int which) : LadderElem(false, false, false, which)
{
	Diagram            = diagram;

	prop.id            = 0;
	prop.parameter     = 0;
	prop.parameter_set = 0;
	prop.index         = 0;
	prop.idName        = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, (getWhich() == ELEM_READ_USS) ? eType_ReadUSS : eType_WriteUSS);
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

void LadderElemUSS::internalSetProperties(void *data)
{
	LadderElemUSSProp *newProp = (LadderElemUSSProp *)data;

	prop = *newProp;
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
	return
		fread_int  (f, &prop.id) &&
		fread_int  (f, &prop.index) &&
		fread_int  (f, &prop.parameter) &&
		fread_int  (f, &prop.parameter_set) &&
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);
}

LadderElem *LadderElemUSS::Clone(void)
{
	LadderElemUSS *clone = new LadderElemUSS(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemUSS::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != ((getWhich() == ELEM_READ_USS) ? eType_ReadUSS : eType_WriteUSS) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemUSS::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, (getWhich() == ELEM_READ_USS) ? eType_ReadUSS : eType_WriteUSS, isDiscard);
}

bool LadderElemUSS::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemModBUS
LadderElemModBUS::LadderElemModBUS(LadderDiagram *diagram, int which) : LadderElem(false, false, false, which)
{
	Diagram           = diagram;

	int NodeID = Diagram->mbGetNodeIDByIndex(0);
	if(NodeID < 0) {
		NodeID = Diagram->mbCreateNode("Default");
	}

	prop.address      = 0;
	prop.retransmitir = true;
	prop.int32        = false;
	prop.elem         = NodeID;
	prop.idName       = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, (getWhich() == ELEM_READ_MODBUS) ? eType_ReadModbus : eType_WriteModbus);
}

pair<string, string> LadderElemModBUS::DrawTXT(void)
{
	char *bot;
	char buf[100];
	LadderMbNode node = Diagram->mbGetNodeByNodeID(prop.elem);

	sprintf(buf, "%s:%d", node.name, prop.address);

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
	Diagram->mbAddRef(prop.address);
}

void LadderElemModBUS::doPostRemove(void)
{
	Diagram->mbDelRef(prop.address);
}

void LadderElemModBUS::internalSetProperties(void *data)
{
	LadderElemModBUSProp *newProp = (LadderElemModBUSProp *)data;

	prop = *newProp;
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
	return
		fread_int  (f, &prop.address) &&
		fread_int  (f, &prop.elem) &&
		fread_bool (f, &prop.int32) &&
		fread_bool (f, &prop.retransmitir) &&
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);
}

LadderElem *LadderElemModBUS::Clone(void)
{
	LadderElemModBUS *clone = new LadderElemModBUS(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemModBUS::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != ((getWhich() == ELEM_READ_MODBUS) ? eType_ReadModbus : eType_WriteModbus) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemModBUS::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, (getWhich() == ELEM_READ_MODBUS) ? eType_ReadModbus : eType_WriteModbus, isDiscard);
}

bool LadderElemModBUS::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemSetPWM
LadderElemSetPWM::LadderElemSetPWM(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_SET_PWM)
{
	Diagram         = diagram;

	prop.targetFreq = 1000;
	prop.idName       = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("duty_cycle"), false, eType_PWM);
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

void LadderElemSetPWM::internalSetProperties(void *data)
{
	LadderElemSetPWMProp *newProp = (LadderElemSetPWMProp *)data;

	prop = *newProp;
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
	return
		fread_int  (f, &prop.targetFreq) &&
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);
}

LadderElem *LadderElemSetPWM::Clone(void)
{
	LadderElemSetPWM *clone = new LadderElemSetPWM(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemSetPWM::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != eType_PWM && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemSetPWM::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, eType_PWM, isDiscard);
}

bool LadderElemSetPWM::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemUART
LadderElemUART::LadderElemUART(LadderDiagram *diagram, int which) : LadderElem(false, false, false, which)
{
	Diagram   = diagram;

	prop.idName        = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("char"), false, (getWhich() == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART);
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
			ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), stateInOut);
			ic.Op(getWhich() == ELEM_UART_SEND ? INT_UART_SEND : INT_UART_RECV, name, stateInOut);
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

void LadderElemUART::internalSetProperties(void *data)
{
	LadderElemUARTProp *newProp = (LadderElemUARTProp *)data;

	prop = *newProp;
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
	return
		fread_ulong(f, &prop.idName.first) &&
		fread_int  (f, &prop.idName.second);
}

LadderElem *LadderElemUART::Clone(void)
{
	LadderElemUART *clone = new LadderElemUART(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemUART::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idName.first && type != ((getWhich() == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemUART::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idName, _("new"), false, (getWhich() == ELEM_UART_RECV) ? eType_RxUART : eType_TxUART, isDiscard);
}

bool LadderElemUART::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemMasterRelay
LadderElemMasterRelay::LadderElemMasterRelay(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_MASTER_RELAY)
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

	return true;
}

bool LadderElemMasterRelay::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

LadderElem *LadderElemMasterRelay::Clone(void)
{
	LadderElemMasterRelay *clone = new LadderElemMasterRelay(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados

	return clone;
}

bool LadderElemMasterRelay::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemShiftRegister
LadderElemShiftRegister::LadderElemShiftRegister(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_SHIFT_REGISTER)
{
	Diagram     = diagram;

	prop.stages  = 7;
	prop.nameReg = _("reg");

	createRegs();
}

void LadderElemShiftRegister::createRegs(void)
{
	int i;
	vector< pair<unsigned long, int> >::iterator it;

	// Primeiro descarta os I/Os anteriores, solicitando I/O para o valor zero.
	for(it = prop.vectorIdRegs.begin(); it != prop.vectorIdRegs.end(); it++) {
		Diagram->getIO(*it, "0", false, eType_General);
	}

	// Limpa o vetor anterior
	prop.vectorIdRegs.clear();

	// Registra os novos valores
	char cname[MAX_NAME_LEN + 10];
	pair<unsigned long, int> pin;
	for(i = 0; i < prop.stages; i++) {
		pin = pair<unsigned long, int>(0, 0);
		sprintf(cname, "%s%d", prop.nameReg.c_str(), i);
		if(!Diagram->getIO(pin, _(cname), false, eType_General)) {
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

void LadderElemShiftRegister::internalSetProperties(void *data)
{
	LadderElemShiftRegisterProp *newProp = (LadderElemShiftRegisterProp *)data;

	prop = *newProp;

	createRegs();
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

		for(i = 0; i < prop.stages; i++) {
			ret = fread_ulong(f, &pin.first) && fread_int(f, &pin.second);
			if(ret == true) {
				prop.vectorIdRegs.push_back(pin);
			} else {
				break;
			}
		}
	}

	return ret;
}

LadderElem *LadderElemShiftRegister::Clone(void)
{
	LadderElemShiftRegister *clone = new LadderElemShiftRegister(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemShiftRegister::acceptIO(unsigned long id, eType type)
{
	vector< pair<unsigned long, int> >::iterator it;

	for(it = prop.vectorIdRegs.begin(); it != prop.vectorIdRegs.end(); it++) {
		if(id == it->first && type != eType_General && type != eType_Reserved) {
			return false;
		}
	}

	return true;
}

void LadderElemShiftRegister::updateIO(bool isDiscard)
{
	int i;
	char cname[MAX_NAME_LEN + 10];

	for(i = 0; i < prop.stages; i++) {
		sprintf(cname, "reg%d", i);
		Diagram->updateIO(prop.vectorIdRegs[i], cname, false, eType_General, isDiscard);
	}
}

bool LadderElemShiftRegister::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemLUT
LadderElemLUT::LadderElemLUT(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_LOOK_UP_TABLE)
{
	Diagram           = diagram;

	prop.count        = 0;
	prop.editAsString = false;
	prop.idDest       = pair<unsigned long, int>(0, 0);
	prop.idIndex      = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idDest , _("dest" ), false, eType_General);
	Diagram->getIO(prop.idIndex, _("index"), false, eType_General);

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

void LadderElemLUT::internalSetProperties(void *data)
{
	LadderElemLUTProp *newProp = (LadderElemLUTProp *)data;

	prop = *newProp;
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

	return ret;
}

LadderElem *LadderElemLUT::Clone(void)
{
	LadderElemLUT *clone = new LadderElemLUT(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemLUT::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idDest.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	if(id == prop.idIndex.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemLUT::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idDest , _("dest" ), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idIndex, _("index"), false, eType_General, isDiscard);
}

bool LadderElemLUT::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemPiecewise
LadderElemPiecewise::LadderElemPiecewise(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_PIECEWISE_LINEAR)
{
	Diagram      = diagram;

	prop.count   = 0;
	prop.idDest  = pair<unsigned long, int>(0, 0);
	prop.idIndex = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idDest , _("yvar"), false, eType_General);
	Diagram->getIO(prop.idIndex, _("xvar"), false, eType_General);

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
		Error(_("Piecewise linear lookup table with zero elements!"));
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

void LadderElemPiecewise::internalSetProperties(void *data)
{
	LadderElemPiecewiseProp *newProp = (LadderElemPiecewiseProp *)data;

	prop = *newProp;
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

	return ret;
}

LadderElem *LadderElemPiecewise::Clone(void)
{
	LadderElemPiecewise *clone = new LadderElemPiecewise(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemPiecewise::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idDest.first && type != eType_General && type != eType_Reserved) {
		return false;
	}

	if(id == prop.idIndex.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemPiecewise::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idDest , _("yvar"), false, eType_General, isDiscard);
	Diagram->updateIO(prop.idIndex, _("xvar"), false, eType_General, isDiscard);
}

bool LadderElemPiecewise::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemFmtString
LadderElemFmtString::LadderElemFmtString(LadderDiagram *diagram, int which) : LadderElem(false, false, true, which)
{
	Diagram  = diagram;

	prop.txt = _("value: %d\\r\\n");
	prop.idVar = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idVar , _("var"), false, (which == ELEM_READ_FORMATTED_STRING) ? eType_RxUART : eType_TxUART);
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
	string byPass  = ic.GenSymOneShot();

	// OneShot 
	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_IF_BIT_SET, "$SerialReady");
				if (getWhich() == ELEM_READ_FORMATTED_STRING)
					ic.Op(INT_READ_FORMATTED_STRING, var, prop.txt.c_str());
				else
					ic.Op(INT_WRITE_FORMATTED_STRING, var, prop.txt.c_str());
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

bool LadderElemFmtString::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void LadderElemFmtString::internalSetProperties(void *data)
{
	LadderElemFmtStringProp *newProp = (LadderElemFmtStringProp *)data;

	prop = *newProp;
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
	return
		fread_string(f, &prop.txt) &&
		fread_ulong (f, &prop.idVar.first) &&
		fread_int   (f, &prop.idVar.second);
}

LadderElem *LadderElemFmtString::Clone(void)
{
	LadderElemFmtString *clone = new LadderElemFmtString(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemFmtString::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idVar.first && type != ((getWhich() == ELEM_READ_FORMATTED_STRING) ? eType_RxUART : eType_TxUART) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemFmtString::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idVar, _("var"), false, (getWhich() == ELEM_READ_FORMATTED_STRING) ? eType_RxUART : eType_TxUART, isDiscard);
}

bool LadderElemFmtString::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemYaskawa
LadderElemYaskawa::LadderElemYaskawa(LadderDiagram *diagram, int which) : LadderElem(false, false, true, which)
{
	Diagram  = diagram;

	prop.id  = 0;
	prop.txt = _("0ZSET%d");
	prop.idVar = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idVar , _("var"), false, (which == ELEM_READ_SERVO_YASKAWA) ? eType_ReadYaskawa : eType_WriteYaskawa);
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
				if (getWhich() == ELEM_READ_SERVO_YASKAWA)
					ic.Op(INT_READ_SERVO_YASKAWA , var, prop.txt.c_str(), idtxt, 0, 0);
				else
					ic.Op(INT_WRITE_SERVO_YASKAWA, var, prop.txt.c_str(), idtxt, 0, 0);
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

bool LadderElemYaskawa::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void LadderElemYaskawa::internalSetProperties(void *data)
{
	LadderElemYaskawaProp *newProp = (LadderElemYaskawaProp *)data;

	prop = *newProp;
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
	return
		fread_int   (f, &prop.id) &&
		fread_string(f, &prop.txt) &&
		fread_ulong (f, &prop.idVar.first) &&
		fread_int   (f, &prop.idVar.second);
}

LadderElem *LadderElemYaskawa::Clone(void)
{
	LadderElemYaskawa *clone = new LadderElemYaskawa(Diagram, getWhich());
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemYaskawa::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idVar.first && type != ((getWhich() == ELEM_READ_SERVO_YASKAWA) ? eType_ReadYaskawa : eType_WriteYaskawa) && type != eType_Reserved) {
		return false;
	}

	return true;
}

void LadderElemYaskawa::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idVar, _("var"), false, (getWhich() == ELEM_READ_SERVO_YASKAWA) ? eType_ReadYaskawa : eType_WriteYaskawa, isDiscard);
}

bool LadderElemYaskawa::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemPersist
LadderElemPersist::LadderElemPersist(LadderDiagram *diagram) : LadderElem(true, false, false, ELEM_PERSIST)
{
	Diagram  = diagram;

	prop.idVar = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idVar , _("var"), false, eType_General);
}

pair<string, string> LadderElemPersist::DrawTXT(void)
{
	return pair<string, string>(Diagram->getNameIO(prop.idVar), _("{PERSIST}"));
}

bool LadderElemPersist::internalGenerateIntCode(IntCode &ic)
{
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

	ic.Op(INT_IF_BIT_CLEAR, isInit.c_str());
		ic.Op(INT_CLEAR_BIT, "$scratch");
		ic.Op(INT_EEPROM_BUSY_CHECK, "$scratch");
		ic.Op(INT_IF_BIT_CLEAR, "$scratch");
			ic.Op(INT_EEPROM_READ, var, isInit.c_str(), EepromAddrFree);
		ic.Op(INT_END_IF);
	ic.Op(INT_ELSE);
		// While running, continuously compare the EEPROM copy of
		// the variable against the RAM one; if they are different,
		// write the RAM one to EEPROM. 
		ic.Op(INT_CLEAR_BIT, "$scratch");
		ic.Op(INT_EEPROM_BUSY_CHECK, "$scratch");
		ic.Op(INT_IF_BIT_CLEAR, "$scratch");
			ic.Op(INT_EEPROM_READ, "$scratch2", "$scratch", EepromAddrFree);
			ic.Op(INT_IF_VARIABLE_EQUALS_VARIABLE, "$scratch2", var);
			ic.Op(INT_ELSE);
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

void LadderElemPersist::internalSetProperties(void *data)
{
	LadderElemPersistProp *newProp = (LadderElemPersistProp *)data;

	prop = *newProp;
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
	return
		fread_ulong (f, &prop.idVar.first) &&
		fread_int   (f, &prop.idVar.second);
}

LadderElem *LadderElemPersist::Clone(void)
{
	LadderElemPersist *clone = new LadderElemPersist(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemPersist::acceptIO(unsigned long id, eType type)
{
	if(id == prop.idVar.first && !Diagram->IsGenericTypeIO(type)) {
		return false;
	}

	return true;
}

void LadderElemPersist::updateIO(bool isDiscard)
{
	Diagram->updateIO(prop.idVar, _("var"), false, eType_General, isDiscard);
}

bool LadderElemPersist::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
}

// Classe LadderElemX
LadderElemX::LadderElemX(LadderDiagram *diagram) : LadderElem(false, false, false, ELEM_PADDING)
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

LadderElem *LadderElemX::Clone(void)
{
	LadderElemX *clone = new LadderElemX(Diagram);
	clone->updateIO(true); // Descarta os I/Os registrados
	clone->internalSetProperties(&prop);

	return clone;
}

bool LadderElemX::internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	return true; // Nada a fazer
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
			delete it->elem;
		} else {
			delete it->subckt;
		}
	}

	vectorSubckt.clear();
}

bool LadderCircuit::DrawElementTXT(vector< vector<int> > &DisplayMatrix, LadderElem *elem, int *cx, int *cy, bool poweredBefore)
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

//				if(CheckBoundsUndoIfFails(gx, gy)) return FALSE;

//				DM_BOUNDS(gx, gy);
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
void LadderCircuit::VerticalWireTXT(int cx, int cy)
{
    int j;
    for(j = 1; j < POS_HEIGHT; j++) {
        DrawChars(cx, cy + (POS_HEIGHT/2 - j), "|");
    }
    DrawChars(cx, cy + (POS_HEIGHT/2), "+");
    DrawChars(cx, cy + (POS_HEIGHT/2 - POS_HEIGHT), "+");
}

bool LadderCircuit::DrawTXT(vector< vector<int> > &DisplayMatrix, int *cx, int *cy, bool poweredBefore, int ColsAvailable)
{
    int cx0 = *cx, cy0 = *cy;

	bool poweredAfter = poweredBefore;
	vector<Subckt>::iterator it;

	if(isSeries) {
		for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
			if(it->elem != nullptr) {
				poweredAfter = DrawElementTXT(DisplayMatrix, it->elem, cx, cy, poweredAfter);
			} else {
				poweredAfter = it->subckt->DrawTXT(DisplayMatrix, cx, cy, poweredAfter, ColsAvailable);
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
				poweredThis = DrawElementTXT(DisplayMatrix, it->elem, cx, cy, poweredBefore);
				justDrewHeight = 1;
			} else {
				poweredThis = it->subckt->DrawTXT(DisplayMatrix, cx, cy, poweredBefore, ColsAvailable);
				justDrewHeight = it->subckt->getHeightTXT();
			}

			if(InSimulationMode) {
				if(poweredThis) poweredAfter = true;
			}

			while((*cx - cx0) < widthMax*POS_WIDTH) {
				int gx = *cx/POS_WIDTH;
				int gy = *cy/POS_HEIGHT;

//				if(CheckBoundsUndoIfFails(gx, gy)) return FALSE;

//				DM_BOUNDS(gx, gy);
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
				if(needWire) VerticalWireTXT(*cx - 1, *cy + j*POS_HEIGHT);
			}
			// stupid special case
			if(lowestPowered == 0 && InSimulationMode) {
				DrawChars(*cx - 1, *cy + (POS_HEIGHT/2), "+");
			}
		}

		needWire = false;
		for(j = heightMax - 1; j >= 1; j--) {
			if(DisplayMatrix[cx0/POS_WIDTH][*cy/POS_HEIGHT + j]) {
				needWire = true;
			}
			if(needWire) VerticalWireTXT(cx0 - 1, *cy + j*POS_HEIGHT);
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

bool LadderCircuit::HasEOL(void)
{
	vector<Subckt>::iterator it;

	for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
		if(it->elem != nullptr) {
			if(it->elem->IsEOL()) {
				return true;
			}
		} else if(it->subckt->HasEOL()) {
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
		ic.Comment(_("start series ["));
		for(i = 0; i < vectorSubckt.size() && ret; i++) {
			if(vectorSubckt[i].elem != nullptr) {
				ret = vectorSubckt[i].elem->GenerateIntCode(ic);
			} else {
				ret = vectorSubckt[i].subckt->GenerateIntCode(ic);
			}
		}
		ic.Comment(_("finish series ["));
	} else {
		string parThis, parOut;
		parThis = ic.GenSymParThis();
		parOut  = ic.GenSymParOut ();

		const char *stateInOut = ic.getStateInOut();
		ic.setStateInOut(parThis);

		ic.Comment(_("start parallel ["));
		ic.Op(INT_CLEAR_BIT, parOut.c_str());

        for(i = 0; i < vectorSubckt.size() && ret; i++) {
			ic.Op(INT_COPY_BIT_TO_BIT, parThis.c_str(), stateInOut);

			if(vectorSubckt[i].elem != nullptr) {
				ret = vectorSubckt[i].elem->GenerateIntCode(ic);
			} else {
				ret = vectorSubckt[i].subckt->GenerateIntCode(ic);
			}

			ic.Op(INT_IF_BIT_SET, parThis.c_str());
			ic.Op(INT_SET_BIT, parOut.c_str());
			ic.Op(INT_END_IF);
        }

		ic.Op(INT_COPY_BIT_TO_BIT, stateInOut, parOut.c_str());
		ic.setStateInOut(stateInOut);

		ic.Comment(_("finish parallel ["));
	}

	return ret;
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

void LadderCircuit::AddPlaceHolderIfNoEOL(LadderContext context)
{
	vector<Subckt>::iterator it;

	if(!HasEOL()) {
		it = vectorSubckt.begin() + vectorSubckt.size();
		if(vectorSubckt.empty() || ((it-1)->elem == nullptr && !(it-1)->subckt->IsSeries())) {
			// Se o circuito estiver vazio ou se adicionamos um paralelo e for o ultimo elemento,
			// devemos adicionar o elemento de final de linha (PlaceHolder)
			Subckt s = { new LadderElemPlaceHolder(context.Diagram), this };
			InsertSubckt(context, getSize(), s);
		}
	}
}

bool LadderCircuit::InsertSubckt(LadderContext context, unsigned int pos, Subckt s,
	bool isMove, bool isUndoRedo)
{
	bool ret = false;

	if(s.elem != nullptr) {
		s.subckt = this;
	}

	if(pos == getSize()) {
		ret = true;
		vectorSubckt.push_back(s);
	} else if(pos < getSize()) {
		ret = true;
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

	return ret;
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
			start = getSize();
		}

		// Inserindo em paralelo
		InsertSubckt(context, start, s);

		ret = true;
	}

	return ret;
}

int LadderCircuit::ElemInSubcktSeries(LadderContext &context, InsertionPoint *point)
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
				(point->subckt.elem != context.SelectedElem ? 1 : context.SelectedState == SELECTED_RIGHT)) {
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
					series = vectorSubckt[i].subckt;
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

void LadderCircuit::RemoveUnnecessarySubckts(LadderContext context)
{
	unsigned int i;
	for(i = 0; i < vectorSubckt.size(); i++) {
		if(vectorSubckt[i].elem == nullptr) {
			vectorSubckt[i].subckt->RemoveUnnecessarySubckts(context);
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

				// Desaloca o subcircuito
				DeleteSubckt(context, i);
			}
		}
	}
}

LadderCircuit *LadderCircuit::Clone(void)
{
	vector<Subckt>::size_type i;
	LadderCircuit *clone = new LadderCircuit(isSeries);

	for(i = 0; i < vectorSubckt.size(); i++) {
		Subckt s;
		if(vectorSubckt[i].elem != nullptr) {
			s.elem   = vectorSubckt[i].elem->Clone();
			s.subckt = this;
		} else {
			s.elem = nullptr;
			s.subckt = vectorSubckt[i].subckt->Clone();
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

bool LadderCircuit::Save(LadderDiagram *diagram, FILE *f)
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
				if(!fwrite_bool(f, false) || !it->subckt->Save(diagram, f)) {
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
						default: break; // Elemento nao suportado. Novo elemento nao cadastrado aqui???
						}

						s.elem->Load(f, version);
						s.subckt = this;
					} else {
						break; // erro carregando elemento, interrompe carregamento
					}
				} else {
					s.elem   = nullptr;
					s.subckt = new LadderCircuit;
					s.subckt->Load(diagram, f, version);
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

bool LadderCircuit::acceptIO(unsigned long id, enum eType type)
{
	bool ret = true;
	vector<Subckt>::size_type it;

	for(it = 0; it < vectorSubckt.size(); it++) {
		if(vectorSubckt[it].elem != nullptr) {
			ret = vectorSubckt[it].elem  ->acceptIO(id, type);
		} else {
			ret = vectorSubckt[it].subckt->acceptIO(id, type);
		}

		if(ret == false) {
			break;
		}
	}

	return ret;
}

void LadderCircuit::updateIO(bool isDiscard)
{
	vector<Subckt>::size_type it;

	for(it = 0; it < vectorSubckt.size(); it++) {
		if(vectorSubckt[it].elem != nullptr) {
			vectorSubckt[it].elem  ->updateIO(isDiscard);
		} else {
			vectorSubckt[it].subckt->updateIO(isDiscard);
		}
	}
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
					delete data->DeleteSubckt.subckt.elem;
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
					delete data->DeleteSubckt.subckt.elem;
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
	context.Diagram            = this;
	context.ParallelStart      = nullptr;
	context.SelectedElem       = nullptr;
	context.SelectedCircuit    = nullptr;

	context.SelectedState      = SELECTED_NONE;

	context.inSimulationMode     = false;

	context.currentFilename    = "";

	CheckPointLevels           =  0;
	CheckPointLevelsMax        = 30;
	CheckpointBeginCount       =  0;
	CheckpointDoRollback       = false;
	isCheckpointEmpty          = true;

	copiedElement              = nullptr;
	copiedRung                 = nullptr;

	NeedScrollSelectedIntoView = false;

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

	LadderSettings.Dac.ramp_abort_mode = RAMP_ABORT_LEAVE; // Modo padrao de abandono: mantem o valor atual

	LadderSettings.MbSlave.ModBUSID    = 0; // ID padrao do ModBUS escravo: 0

	LadderSettings.Info.Name           = ""; // Nome do Programa
	LadderSettings.Info.Developer      = ""; // Nome do Desenvolvedor
	LadderSettings.Info.Description    = ""; // Descricao do programa
	LadderSettings.Info.FWVersion      = "1.5"; // Versao do Firmware atual do POPTools
	LadderSettings.Info.BuildNumber    =  0; // Numero de compilacao atual
	LadderSettings.Info.CompileDate    =  0; // Data da ultima compilacao. Zero: nunca
	LadderSettings.Info.ProgramDate    =  0; // Data da ultima gravacao. Zero: nunca

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
	if(copiedElement != nullptr) {
		delete copiedElement;
	}

	if(copiedRung != nullptr) {
		delete copiedRung->rung;
		delete copiedRung;
	}

	while(rungs.size() > 0) {
		DeleteRung(0);
	}

	while(UndoList.size() > 0) {
		DiscardCheckpoint(true);
	}

	while(RedoList.size() > 0) {
		DiscardCheckpoint(false);
	}

	ic.Clear();
	IO->Clear();
}

void LadderDiagram::NewDiagram(void)
{
	ClearDiagram();

	NewRung(false);

	// As acoes executadas durante a inicializacao (criacao da linha e PlaceHolder) nao devem ser desfeitas
	UndoList.clear();
	CheckPointLevels = 0;

	updateContext();
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
        ic.Comment(_("start rung %d"), i+1);
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

	if(context.SelectedElem != nullptr) {
		CheckpointBegin("Editar Elemento");
		ret = context.SelectedElem->ShowDialog(context);
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
	context.canNegate        = false;
	context.canNormal        = false;
	context.canSetOnly       = false;
	context.canResetOnly     = false;
	context.canPushUp        = true;
	context.canPushDown      = true;
	context.canDelete        = true;
	context.canDeleteRung    = false;
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

	if(rungs.size() > 1) {
		context.canDeleteRung = true;
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
					vector<LadderRung *>::iterator it;
					if(i >= 0) {
						it = rungs.begin() + i;
						context.canInsertEnd = (*it)->rung->IsLast(context.SelectedElem);
					} else {
						context.canInsertEnd = false;
					}
			}

			if(context.SelectedElem->getWhich() == ELEM_CONTACTS) {
				context.canNegate = true;
				context.canNormal = true;
			}
		}

		if(context.SelectedCircuit->IsEmpty()) {
			// a comment must be the only element in its rung, and it will fill
			// the rung entirely
			context.canInsertComment = true;
		}

		if(context.SelectedElem->IsComment()) {
			// if there's a comment there already then don't let anything else
			// into the rung
			context.canInsertEnd   = false;
			context.canInsertOther = false;
		}
	}

	SetMenusEnabled(&context);

	updateUndoContextAfter();
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

bool LadderDiagram::IsRungEmpty(unsigned int n)
{
	return (n < rungs.size()) ? rungs[n]->rung->IsEmpty() : true;
}

void LadderDiagram::NewRung(bool isAfter)
{
	vector<LadderRung *>::iterator it;
	int position = RungContainingSelected();

	CheckpointBegin(_("Inserir Linha"));

	// Registro da acao para desfazer / refazer
	UndoRedoAction action;
	UndoRedoData *data = new UndoRedoData;

	data->NewRung.rung                = new LadderRung;

	data->NewRung.rung->hasBreakpoint = false;
	data->NewRung.rung->isPowered     = false;
	data->NewRung.rung->rung          = new LadderCircuit(true);

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
}

bool LadderDiagram::PushRung(int rung, bool up)
{
	int QtdRungs = rungs.size();

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

	if(!IsSelectedVisible()) {
		NeedScrollSelectedIntoView = true;
	}

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

	// Atualiza o contexto
	updateContext();

	return true;
}

bool LadderDiagram::DeleteRung(int rung)
{
	if(rung < 0) {
		rung = RungContainingSelected();
	}

	if(rung < 0) return false;

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
		context.SelectedCircuit = nullptr;
	}

	(*it)->rung->doPostRemove();
	(*it)->rung->updateIO(true);
	rungs.erase(it);

	CheckpointEnd();

	updateContext();

	return true;
}

bool LadderDiagram::CopyRung(LadderElem *elem)
{
	int pos;
	if(elem == nullptr) {
		elem = context.SelectedElem;
		if(elem == nullptr) return false; // Sem linha para copiar
	}

	pos = RungContainingElement(elem);

	// Primeiro descarta uma linha previamente copiada
	if(copiedRung != nullptr) {
		delete copiedRung->rung;
		delete copiedRung;
	}

	// Copia a linha
	copiedRung                = new LadderRung;

	copiedRung->hasBreakpoint = false;
	copiedRung->isPowered     = false;
	copiedRung->rung          = rungs[pos]->rung->Clone();

	// Retorna com sucesso!
	return true;
}

bool LadderDiagram::PasteRung(bool isAfter)
{
	int pos;
	bool ret = false;

	// Nao existe linha copiada ou nao existe ponto de insercao definido
	if(copiedRung == nullptr || context.SelectedElem == nullptr) return false;

	CheckpointBegin(_("Colar Linha"));

	pos = RungContainingSelected();

	if(pos >= 0) {
		if(isAfter) {
			pos++;
		}

		rungs.insert(rungs.begin() + pos, copiedRung);
		copiedRung->rung->doPostInsert();
		copiedRung->rung->updateIO(false);

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

		// Cria uma copia de si mesma para que seja possivel inserir uma nova copia
		copiedRung                = new LadderRung;

		copiedRung->hasBreakpoint = false;
		copiedRung->isPowered     = false;
		copiedRung->rung          = rungs[pos]->rung->Clone();

		ret = true;
	} else {
		CheckpointRollback();
	}

	CheckpointEnd();

	updateContext();

	return ret;
}

bool LadderDiagram::AddElement(LadderElem *elem)
{
	// Se elemento a ser adicionado for nulo, retorna
	if(elem == nullptr) return false;
	// Se nao existe um elemento selecionado, descarta o elemento que nos foi passado e retorna
	if(context.SelectedElem == nullptr || context.SelectedCircuit == nullptr) {
		// Se o elemento for descartado, deve descartar todos os I/Os que ele houver registrado.
		elem->updateIO(true);

		// Apos o recebimento de elem, a classe LadderDiagram passa a ser responsavel por ele.
		// Se por qualquer motivo houver uma falha ao inserir, a classe tem a responsabilidade
		// de desalocar este elemento.
		delete elem;

		return false;
	}

	CheckpointBegin(_("Adicionar Elemento"));

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
		int position = RungContainingSelected();
		if(position >= 0) { // Se posicao menor que zero, insere no final
			vector<LadderRung *>::iterator it = rungs.begin() + position;
			(*it)->rung->AddPlaceHolderIfNoEOL(context);
		}

		// Elemento adicionado, chama a funcao para que o elemento realize qualquer etapa
		// adicional apos a sua inclusao no circuito.
		// Especificamente para o caso do ModBUS, que precisa referenciar o node que ele usa.
		elem->doPostInsert();

		SelectElement(elem, elem->IsEOL() ? SELECTED_LEFT : SELECTED_RIGHT);
		updateContext();
	} else {
		// Ocorreu um erro! Marca o checkpoint para remocao
		CheckpointRollback();

		// Apos o recebimento de elem, a classe LadderDiagram passa a ser responsavel por ele.
		// Se por qualquer motivo houver uma falha ao inserir, a classe tem a responsabilidade
		// de desalocar este elemento.
		delete elem;
	}

	CheckpointEnd();

	return ret;
}

bool LadderDiagram::DelElement(LadderElem *elem)
{
	if(elem == nullptr) {
		if(context.SelectedElem == nullptr) return false; // Sem elemento para remover
		elem = context.SelectedElem;
	}

	int rung = RungContainingElement(elem);

	CheckpointBegin(_("Remover Elemento"));
	if(rungs[rung]->rung->DelElement(elem, context)) {
		// Elemento adicionado, chama a funcao para que o elemento realize qualquer etapa
		// adicional apos a sua inclusao no circuito.
		// Especificamente para o caso do ModBUS, que precisa referenciar o node que ele usa.
		elem->doPostRemove();

		elem->updateIO(true);
		rungs[rung]->rung->RemoveUnnecessarySubckts(context);
		rungs[rung]->rung->AddPlaceHolderIfNoEOL(context);

		if(elem == context.ParallelStart) {
			context.ParallelStart = nullptr;
		}

		updateContext();
		CheckpointEnd();
		return true;
	}

	CheckpointRollback();
	CheckpointEnd();

	return false;
}

bool LadderDiagram::CopyElement(LadderElem *elem)
{
	if(elem == nullptr) {
		elem = context.SelectedElem;
		if(elem == nullptr) return false; // Sem elemento para copiar
	}

	// As acoes de copia do objeto nao devem ser registradas pois o CTRL+Z nao desfaz a copia.
	// Assim precisamos criar um checkpoint para que possamos descartar as acoes inscritas
	CheckpointBegin(_("Copiar Elemento"));

	// Primeiro descarta um elemento previamente copiado
	if(copiedElement != nullptr) {
		delete copiedElement;
	}

	// Copia o elemento
	copiedElement = elem->Clone();

	// Copia finalizada. Executa o rollback e fecha o checkpoint e assim todas as acoes registradas
	// desde a criacao do checkpoint serao descartadas
	CheckpointRollback();
	CheckpointEnd();

	// Retorna com sucesso!
	return true;
}

bool LadderDiagram::PasteElement(void)
{
	bool ret = false;

	if(copiedElement == nullptr) return false; // Nao existe elemento copiado

	CheckpointBegin(_("Colar Elemento"));

	LadderElem *newCopiedElement = copiedElement->Clone();

	if(AddElement(copiedElement)) {
		copiedElement->updateIO(false);
		ret = true;
	} else {
		CheckpointRollback();
	}

	CheckpointEnd();

	copiedElement = newCopiedElement;

	updateContext();

	return ret;
}

void LadderDiagram::DrawTXT(int OffsetX)
{
    int i;
	int cx = OffsetX, cy = 0;
	int ColsAvailable = getWidthTXT();
	int RowsAvailable = getHeightTXT();

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
		DrawChars(0, cy, ch);

		rungs[it]->rung->DrawTXT(DisplayMatrix, &cx, &cy, true, ColsAvailable);

		cx  = OffsetX;
		cy += (rungs[it]->rung->getHeightTXT() + 1) * POS_HEIGHT;
	}

	// Desenha a linha final
    char *str = _("[END]");
    int lead = (ColsAvailable*POS_WIDTH - strlen(str))/2;
    for(i = 0; i < lead; i++) {
        DrawChars(cx + i, cy + (POS_HEIGHT/2), "-");
    }
    DrawChars(cx + i, cy + (POS_HEIGHT/2), str);
    i += strlen(str);
    for(; i < ColsAvailable*POS_WIDTH; i++) {
        DrawChars(cx + i, cy + (POS_HEIGHT/2), "-");
    }
}

bool LadderDiagram::AddParallelStart(void)
{
	bool ret = false;

	if(context.ParallelStart == nullptr) {
		if(context.SelectedElem != nullptr) {
			switch(context.SelectedState) {
			case SELECTED_NONE:
			case SELECTED_LEFT:
			case SELECTED_RIGHT:
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

	// First we should to remove last saved state because UndoRemember was called before to add this new elemment.
	// We don't want user going back to this state (with parallel start)!
	//UndoForget();

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

			if(context.SelectedState == SELECTED_RIGHT) {
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
							EndPoint.series        = nullptr;
							EndPoint.parallel      = nullptr;
							rungs[CurrentRung]->rung->ElemInSubcktSeries(context, &EndPoint);
						}
					} while(LastIsEndParallel);
					Previous = EndPoint.series->getSubckt(EndPoint.point);
				}
			} else if(context.SelectedState == SELECTED_LEFT) {
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
			// Special condition: selectedstate is SELECTED_RIGHT
			if(context.SelectedState == SELECTED_RIGHT) {
				EndPoint.point++;
				StartPoint.point--;
				UseCurrentParallel = 0;
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
			ret = StartPoint.parallel->InsertParallel(elem, StartPoint.point, EndPoint.point, context);
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
static const unsigned long int LADDER_BINFMT_MAGIC      = 0x0f5a0100;
static const unsigned long int LADDER_BINFMT_MAGIC_MASK = 0xffff0000;
#define LADDER_BINFMT_GET_VERSION(m)  (m & 0x00ff)
#define LADDER_BINFMT_GET_FLAGS(m)   ((m & 0xff00) >> 8)
#define LADDER_FILE_MAX_SIZE         20971520 // 20 MB

bool LadderDiagram::Save(string filename, bool dontSaveFilename)
{
	bool hasParallel = context.ParallelStart != nullptr;
	bool failed = true; // Desmarca ao chegar no final da logica. Se nao chegou, ocorreu uma falha

	if(filename.size() == 0) { // Nome vazio! Tentamos usar o nome do ultimo salvamento
		filename = context.currentFilename;
		if(filename.size() == 0) return false; // Continua vazio! Retorna erro...
	}

	FILE *f = fopen(filename.c_str(), "wb+");
    if(!f) return failed;

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

		fwrite_int   (f, LadderSettings.MbSlave.ModBUSID   ) &&

		fwrite_string(f, LadderSettings.Info.Name          ) &&
		fwrite_string(f, LadderSettings.Info.Developer     ) &&
		fwrite_string(f, LadderSettings.Info.Description   ) &&
		fwrite_string(f, LadderSettings.Info.FWVersion     ) &&
		fwrite_long  (f, LadderSettings.Info.BuildNumber   ) &&
		fwrite_time_t(f, LadderSettings.Info.CompileDate   ) &&
		fwrite_time_t(f, LadderSettings.Info.ProgramDate   )
		) {
			// Configuracoes OK, agora devemos gravar o mapa de I/O
			ret = IO->Save(f);

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
						if(!fwrite_bool(f, (*it)->hasBreakpoint) || !(*it)->rung->Save(this, f)) {
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
					if(dontSaveFilename == false) {
						context.currentFilename = filename;
					}
				}
			}
	}

	fclose(f);

	if(failed) {
		_unlink(filename.c_str());
	}

	// Agora executamos o rollback e encerramos o checkpoint.
	// Dessa forma as acoes serao desfeitas e as listas de desfazer/refazer nao serao perdidas.
	CheckpointRollback();
	CheckpointEnd();

	return !failed;
}

bool LadderDiagram::Load(string filename)
{
	unsigned long int magic;
	bool failed = true; // Desmarca ao chegar no final da logica. Se nao chegou, ocorreu uma falha

	FILE *f = fopen(filename.c_str(), "rb");
    if(!f) return failed;

	// Start with the magic number
	fread(&magic, sizeof(magic), 1, f);
	if(((magic ^ LADDER_BINFMT_MAGIC) & LADDER_BINFMT_MAGIC_MASK) == 0) {
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
				// volta para o comeco, depois do numero magico
				fseek(f, sizeof(LADDER_BINFMT_MAGIC), SEEK_SET);

				// Limpa e inicializa o diagrama com os valores padrao antes de comecar o carregamento
				ClearDiagram();

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

					fread_int   (f, &LadderSettings.MbSlave.ModBUSID   ) &&

					fread_string(f, &LadderSettings.Info.Name          ) &&
					fread_string(f, &LadderSettings.Info.Developer     ) &&
					fread_string(f, &LadderSettings.Info.Description   ) &&
					fread_string(f, &LadderSettings.Info.FWVersion     ) &&
					fread_long  (f, &LadderSettings.Info.BuildNumber   ) &&
					fread_time_t(f, &LadderSettings.Info.CompileDate   ) &&
					fread_time_t(f, &LadderSettings.Info.ProgramDate   )
					) {
						// Configuracoes OK, agora devemos ler o mapa de I/O
						bool ret = IO->Load(f, LADDER_BINFMT_GET_VERSION(magic));

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
									rung->isPowered = false;
									if(fread_bool(f, &rung->hasBreakpoint)) {
										rung->rung = new LadderCircuit;
										if(!rung->rung->Load(this, f, LADDER_BINFMT_GET_VERSION(magic))) {
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
							context.currentFilename = filename;
						}
				}
			}
		}
	}

	fclose(f);

	if(failed) {
		// Erro durante o carregamento
		NewDiagram();
	} else {
		// Carregmento finalizado com sucesso!
		// As acoes executadas durante o carregamento nao devem ser desfeitas
		UndoList.clear();
		CheckPointLevels = 0;

		// Garante que o contexto esta OK
		updateContext();

		// Forca desenhar a lista de I/Os e a tela
		IO->updateGUI();
		DrawGUI();
	}

	return !failed;
}

string LadderDiagram::getCurrentFilename(void)
{
	return context.currentFilename;
}

void LadderDiagram::ToggleBreakPoint(unsigned int rung)
{
	// Se linha inexistente ou comentario, retorna.
	if(rung > rungs.size() || rungs[rung]->rung->IsComment()) return;

	rungs[rung]->hasBreakpoint = !rungs[rung]->hasBreakpoint;
}


/*** Funcoes para gravar a configuracao do ladder ***/

// Funcao que Registra no Undo/Redo a configuracao atual para que possa ser desfeito
void LadderDiagram::RegisterSettingsChanged(void)
{
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
	unsigned int index = mbGetIndexByNodeID(NodeID);

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
	unsigned int index = mbGetIndexByNodeID(NodeID);

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

/*** Funcoes relacionadas com I/O ***/

bool LadderDiagram::acceptIO(unsigned long id, enum eType type)
{
	vector<LadderCircuit>::size_type it;

	// Se o tipo atual do I/O for reservado, o novo tipo deve ser reservado tambem.
	if(IO->getDetails(id).type == eType_Reserved) {
		type = eType_Reserved;
	}

	for(it = 0; it < rungs.size(); it++) {
		if(!rungs[it]->rung->acceptIO(id, type)) {
			return false;
		}
	}

	return true;
}

void LadderDiagram::updateIO(pair<unsigned long, int> &pin, string defaultName, bool isBit, eType defaultType, bool isDiscard)
{
	if(pin.first == 0) return; // id nao esta em uso, indica que eh um numero e nao um I/O

	if(isDiscard) {
		IO->Discard(pin.first, isBit);
	} else {
		// Se nao for descarte, indica que devemos atualizar o I/O conforme os passos abaixo:
		// 1) Verifica se id eh valido
		// 1.1) Se for valido, verifica se o tipo ainda eh valido e entao faz um request
		// 1.2) Se nao for valido, devemos criar um novo I/O e atualizar o id
		if(IO->getName(pin.first).size() > 0) { // id valido!
			mapDetails DetailsIO = IO->getDetails(pin.first);
			if(acceptIO(pin.first, DetailsIO.type)) {
				IO->Request(IO->getName(pin.first), isBit, DetailsIO.type);
			} else {
				pin.first = 0;
			}
		} else {
			pin.first = 0;
		}

		if(pin.first == 0) { // id invalido! Cria o novo I/O
			getIO(pin, defaultName, isBit, defaultType);
		}
	}
}

bool LadderDiagram::getIO(pair<unsigned long, int> &pin, string name, bool isBit, eType type)
{
	bool ret = true;
	pair<unsigned long, int> newpin;

	// O objetivo dessa funcao eh simplificar o gerenciamento de IO para os elementos do ladder
	// Ela deve por si propria executar todas as acoes para cadastramento / atualizacao / remocao de IOs
	// Assim sendo, essas sao as acoes que devem ser executados por essa funcao:
	// 1 - Realizar Request / Discard quando necessario
	// 2 - Converter para valor numerico quando for o caso
	// 3 - Carregar os valores no par de retorno conforme o tipo (variavel / numerico)
	// 4 - Atualizar o IO se houve alteracao de tipo

	// O valor solicitado eh um numero, carrega no par de retorno.
	if(IsNumber(name.c_str())) {
		newpin.first  = 0;
		newpin.second = atoi(name.c_str());
	} else {
		mapDetails detailsIO = IO->getDetails(pin.first);
		newpin.first  = IO->getID(name);
		newpin.second = 0;

		if(!newpin.first && !IO->IsReserved(IO->getName(pin.first))) { // variavel inexistente
			newpin.first = pin.first; // O novo pino vai utilizar o mesmo id, ou seja, nao cria nova variavel

			// Se houver mais de 1 elemento usando a variavel, precisamos perguntar ao usuario se ele
			// deseja criar uma nova variavel ou se ele quer atualizar o nome da variavel para todos
			// os elementos
			if((detailsIO.countRequestBit + detailsIO.countRequestInt) > 1) {
				// Precisamos saber do usuario se devemos atualizar o IO ou se vamos criar uma nova variavel
				if(ShowDialog(false, "Nome Alterado", "Nome '%s' utilizado também em outros elementos.\n"
					"Alterar o nome em todos os elementos que o utilizam?", IO->getName(newpin.first).c_str()) == eReply_No) {
						newpin.first = 0;
				}

			}
		}

		// Verifica se o tipo do I/O eh aceito pelos outros elementos que ja o utilizam
		if(newpin.first && !acceptIO(newpin.first, type)) {
			// Alguem compartilhando desta variavel nao aceita a alteracao de tipo. Cancela a alteracao!
			ShowDialog(true, "Tipo Inválido", "Conflito entre tipos para '%s' ! Operação não permitida.", IO->getName(newpin.first).c_str());
			return false; // retorna pino invalido
		}

		// I/O diferente do anterior ou inexistente, fazer o request
		if(newpin.first != pin.first || newpin.first == 0) {
			newpin.first = IO->Request(name, isBit, type);
			if(newpin.first == 0) { // Erro no request
				ret = false;
			}
		// Mesmo que a variavel nao mudou, o tipo pode ter sido alterado. Tenta atualizar
		} else if(!IO->Update(newpin.first, name, type)) {
			// Houve erro na atualizacao! Retorna falso para informar que houve o erro.
			// O procedimento correto nesse caso eh que o objeto nao seja atualizado devido
			// ao erro, mantendo o pino antigo. Dessa forma nao podemos descartar o id antigo.
			ret = false;
		}
	}

	// Se estiver sem erro, descarta o id antigo e atualiza o pino
	if(ret == true) {
		// Se id era valido, o novo pino for valido e id mudou, entao a variavel foi atualizada
		if(pin.first && newpin.first != pin.first) {
			// id mudou, devemos descartar o antigo
			IO->Discard(pin.first, isBit);
		}

		// Tudo ok, atualiza o pino atual com o novo
		pin = newpin;
	}

	return ret;
}

bool LadderDiagram::getIO(vector<tGetIO> &vectorGetIO)
{
	bool ret = true;
	vector<tGetIO>::iterator it;

	for(it = vectorGetIO.begin(); it != vectorGetIO.end(); it++) {
		ret &= getIO(it->pin, it->name, it->isBit, it->type);
	}

	return ret;
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
	return IO->getName(IO->getID(index));
}

mapDetails LadderDiagram::getDetailsIO(unsigned long id)
{
	return IO->getDetails(id);
}

mapDetails LadderDiagram::getDetailsIO(string name)
{
	return getDetailsIO(IO->getID(name));
}

char *LadderDiagram::getStringTypeIO(unsigned int index)
{
	return IO->getTypeString(IO->getDetails(IO->getID(index)).type);
}

unsigned int LadderDiagram::getCountIO(void)
{
	return IO->getCount();
}

void LadderDiagram::selectIO(unsigned int index)
{
	IO->Select(index);
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

bool LadderDiagram::IsValidNameAndType(unsigned long id, string name, eType type, const char *FieldName, unsigned int Rules, int MinVal, int MaxVal, eReply canUpdate)
{
	string old_name = IO->getName(id);
	if(id &&old_name.size() == 0) return false; // id invalido!

	int val;
	bool ret = false;
	eType current_type;
	bool  name_is_internal_flag = IO->IsInternalFlag(name);
	bool  name_is_reserved      = IO->IsReserved(name);
	bool  name_is_number        = IsNumber(name.c_str());

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
		ShowDialog(true, "Nome Inválido", "Nome '%s' reservado para uso interno, favor escolher outro nome.", name.c_str());
		return false;
	} else  if(type == eType_InternalFlag && !name_is_internal_flag) {
		ShowDialog(true, "Nome Inválido", "Para o tipo 'Flag Interna' é obrigatório selecionar um item da lista.");
		return false;
	}

	// Check for Variable and Number restrictions
	if(!name_is_number && !IsValidVarName(name)) {
		ShowDialog(true, "Nome Inválido", "%s '%s' inválido!\n\nVariável: Apenas letras (A a Z), números ou _ (underline) e não inicar com número\nNúmero: Apenas números, podendo iniciar por - (menos)", FieldName ? FieldName : "Nome", name.c_str());
		return false;
	} else if((Rules & VALIDATE_IS_VAR) && name_is_number) {
		ShowDialog(true, "Nome Inválido", "'%s' não pode ser número!", FieldName ? FieldName : name.c_str());
		return false;
	} else if((Rules & VALIDATE_IS_NUMBER) && !name_is_number) {
		ShowDialog(true, "Número Inválido", "'%s' deve ser número!", FieldName ? FieldName : name.c_str());
		return false;
	} else if(name_is_number && !IsValidNumber(name)) {
		ShowDialog(true, "Número Inválido", "Número '%s' inválido!", name.c_str());
		return false;
	}

	// Check for Number Limits
	if((MinVal || MaxVal) && (Rules & (VALIDATE_IS_NUMBER | VALIDATE_IS_VAR_OR_NUMBER)) && name_is_number) {
		val = atoi(name.c_str());
		if(val < MinVal || val > MaxVal) {
			ShowDialog(true, "Número Inválido", "'%s' fora dos limites! Deve estar entre %d e %d.", FieldName ? FieldName : name.c_str(), MinVal, MaxVal);
			return false;
		}
	}

	// Check for Type restrictions
	if(!name_is_number) {
		// If types must match and types are different or cannot accept io_pending type, generates an error
		if((Rules & VALIDATE_TYPES_MUST_MATCH) && current_type != type && !((current_type == eType_Pending || type == eType_Pending) && (Rules & VALIDATE_ACCEPT_IO_PENDING))) {
			ShowDialog(true, "Tipo Inválido", "Conflito entre tipos! Operação não permitida.");
		} else if(type == eType_DigInput || type == eType_DigOutput || type == eType_InternalRelay) {
			if(type == current_type) { // no type change, ok!
				ret = true;
			} else if(current_type == eType_Pending) { // Inexistent name, ok!
				ret = true;
			} else if(type == eType_DigInput && (current_type == eType_DigOutput || current_type == eType_InternalRelay) && !acceptIO(newid, type)) {
				ShowDialog(true, "Saída em uso", "Não é possível alterar para Entrada.");
			} else if(!_stricmp(old_name.c_str(), name.c_str())) { // name not changed, ok!
				ret = true;
			// name changed, check for type changes
			} else if(type == eType_InternalRelay && current_type == eType_DigOutput) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(true, "Tipo inválido", "Conflito de tipos: Rele Interno <-> Saída");
				} else if(ShowDialog(false, "Confirmar alteração de Saída para Rele Interno", "Já existe uma Saída com este nome. Alterar para Rele Interno?") == eReply_Yes) {
					ret = true;
				}
			} else if(type == eType_DigOutput && current_type == eType_InternalRelay) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(true, "Tipo inválido", "Conflito de tipos: Rele Interno <-> Saída");
				} else if(ShowDialog(false, "Confirmar alteração de Rele Interno para Saída", "Já existe um Rele Interno com este nome. Alterar para Saída?") == eReply_Yes) {
					ret = true;
				}
			} else if(type == eType_DigOutput && current_type == eType_DigInput) { // changing existent input to output, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(true, "Tipo inválido", "Conflito de tipos: Entrada <-> Saída");
				} else if(ShowDialog(false, "Confirmar alteração de Entrada para Saída", "Já existe uma Entrada com este nome. Alterar para Saída?") == eReply_Yes) {
					ret = true;
				}
			} else if(type == eType_InternalRelay && current_type == eType_DigInput) { // changing existent input to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(true, "Tipo inválido", "Conflito de tipos: Rele Interno <-> Entrada");
				} else if(ShowDialog(false, "Confirmar alteração de Entrada para Relé Interno", "Já existe uma Entrada com este nome. Alterar para Relé Interno?") == eReply_Yes) {
					ret = true;
				}
			} else { // no wrong conditions, ok!
				ret = true;
			}
		} else if(current_type == eType_Counter || current_type == eType_TOF || current_type == eType_TON) {
			if(type == eType_Counter || type == eType_TOF || type == eType_TON) {
				ret = true;
			} else {
				ShowDialog(true, "Nome inválido", "'Nome' deve ser um contador ou timer!");
			}
		} else {
			ret = true;
		}
	} else {
		ret = true;
	}

	return ret;
}

bool LadderDiagram::IsValidNameAndType(unsigned long id, string name, eType type, eReply canUpdate)
{
	return IsValidNameAndType(id, name, type, NULL, VALIDATE_IS_VAR, 0, 0, canUpdate);
}

eValidateResult LadderDiagram::Validate(void)
{
	unsigned int i;
	char msg_error[1024] = "", msg_warning[1024] = "";
	int  WarningPersist = 0, CountPWM = 0;
	eValidateResult ret = eValidateResult_OK;

	// Validate I/O Pin Assignment
	// Only if not in Simulation Mode
	if(!context.inSimulationMode) {
		mapDetails detailsIO;
		unsigned int count = IO->getCount();
		for(i = 0; i < count; i++) {
			unsigned long id = IO->getID(i);
			detailsIO = IO->getDetails(id);
			switch(detailsIO.type) {
			case eType_ReadADC:
				if(detailsIO.pin == 0) {
					Error(_("Variável A/D '%s' deve ser associado a um canal válido!"), IO->getName(id).c_str());
					ret = eValidateResult_Error;
				}
				break;
			case eType_ReadEnc:
				if(detailsIO.pin == 0) {
					Error(_("Leitura de Encoder '%s' deve ser associada a um canal válido!"), IO->getName(id).c_str());
					ret = eValidateResult_Error;
				}
			case eType_ResetEnc:
				if(detailsIO.pin == 0) {
					Error(_("Escrita de Encoder '%s' deve ser associada a um canal válido!"), IO->getName(id).c_str());
					ret = eValidateResult_Error;
				}
			}
		}
	}

	// Validate Generated IntCode
	if(ret != eValidateResult_Error) {
		vector<IntOp> vectorIntCode = ladder->getVectorIntCode();
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
				ShowDialog(true, _("Erro ao validar diagrama!"), _(msg_error));
				ret = eValidateResult_Error;
				break;
			}

			if(msg_warning[0]) {
				ShowDialog(true, _("Atenção"), _(msg_warning));
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

vector<string> LadderDiagram::getVectorInternalFlagsIO(void)
{
	return IO->getVectorInternalFlags();
}

// Funcoes relacionadas aos comandos de Desfazer / Refazer
void LadderDiagram::RegisterAction(UndoRedoAction Action, bool isUndo)
{
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

				if(UndoList.size() == 0) return; // Lista vazia

				do {
					action = UndoList.back();
					// Primeiro desfaz a acao
					ExecuteAction(true , false, action);
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
