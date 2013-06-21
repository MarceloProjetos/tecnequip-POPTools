#include "poptools.h"

// Funcao apenas devido a compatibilidade com codigo antigo.
// Quando nao for mais necessaria, deve ser removida.
int RemoveParallelStart(int which, void *any)
{
	int ret = SUBCKT_STATUS_NOTFOUND;
	return ret;
}

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

bool LadderElemPlaceHolder::GenerateIntCode(IntCode &ic)
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

bool LadderElemComment::GenerateIntCode(IntCode &ic)
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

	prop.str    = newProp->str;
}

void *LadderElemComment::getProperties(void)
{
	LadderElemCommentProp *curProp = new LadderElemCommentProp;

	curProp->str    = prop.str;

	return curProp;
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

bool LadderElemContact::GenerateIntCode(IntCode &ic)
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

	curProp->idName  = prop.idName;
	curProp->negated = prop.negated;

	return curProp;
}

void LadderElemContact::internalSetProperties(void *data)
{
	LadderElemContactProp *newProp = (LadderElemContactProp *)data;

	prop.idName  = newProp->idName;
	prop.negated = newProp->negated;
}

LadderElem *LadderElemContact::Clone(void)
{
	LadderElemContact *clone = new LadderElemContact(Diagram);
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

bool LadderElemCoil::GenerateIntCode(IntCode &ic)
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

	prop.idName    = newProp->idName;
	prop.negated   = newProp->negated;
	prop.setOnly   = newProp->setOnly;
	prop.resetOnly = newProp->resetOnly;
}

void *LadderElemCoil::getProperties(void)
{
	LadderElemCoilProp *curProp = new LadderElemCoilProp;

	curProp->idName    = prop.idName;
	curProp->negated   = prop.negated;
	curProp->setOnly   = prop.setOnly;
	curProp->resetOnly = prop.resetOnly;

	return curProp;
}

LadderElem *LadderElemCoil::Clone(void)
{
	LadderElemCoil *clone = new LadderElemCoil(Diagram);
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
    unsigned int period = (prop.delay / Prog.settings.cycleTime) - 1;

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

bool LadderElemTimer::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
	prop.delay  = newProp->delay;
}

void *LadderElemTimer::getProperties(void)
{
	LadderElemTimerProp *curProp = new LadderElemTimerProp;

	curProp->idName = prop.idName;
	curProp->delay  = prop.delay;

	return curProp;
}

LadderElem *LadderElemTimer::Clone(void)
{
	LadderElemTimer *clone = new LadderElemTimer(Diagram, getWhich());
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

bool LadderElemRTC::GenerateIntCode(IntCode &ic)
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

	prop.mode  = newProp->mode;
	prop.wday  = newProp->wday;
	prop.start = newProp->start;
	prop.end   = newProp->end;
}

void *LadderElemRTC::getProperties(void)
{
	LadderElemRTCProp *curProp = new LadderElemRTCProp;

	curProp->mode  = prop.mode;
	curProp->wday  = prop.wday;
	curProp->start = prop.start;
	curProp->end   = prop.end;

	return curProp;
}

LadderElem *LadderElemRTC::Clone(void)
{
	LadderElemRTC *clone = new LadderElemRTC(Diagram);
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

bool LadderElemCounter::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
	prop.max    = newProp->max;
}

void *LadderElemCounter::getProperties(void)
{
	LadderElemCounterProp *curProp = new LadderElemCounterProp;

	curProp->idName = prop.idName;
	curProp->max    = prop.max;

	return curProp;
}

LadderElem *LadderElemCounter::Clone(void)
{
	LadderElemCounter *clone = new LadderElemCounter(Diagram, getWhich());
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

bool LadderElemReset::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
}

void *LadderElemReset::getProperties(void)
{
	LadderElemResetProp *curProp = new LadderElemResetProp;

	curProp->idName = prop.idName;

	return curProp;
}

LadderElem *LadderElemReset::Clone(void)
{
	LadderElemReset *clone = new LadderElemReset(Diagram);
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

bool LadderElemOneShot::GenerateIntCode(IntCode &ic)
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

bool LadderElemCmp::GenerateIntCode(IntCode &ic)
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

	prop.idOp1 = newProp->idOp1;
	prop.idOp2 = newProp->idOp2;
}

void *LadderElemCmp::getProperties(void)
{
	LadderElemCmpProp *curProp = new LadderElemCmpProp;

	curProp->idOp1 = prop.idOp1;
	curProp->idOp2 = prop.idOp2;

	return curProp;
}

LadderElem *LadderElemCmp::Clone(void)
{
	LadderElemCmp *clone = new LadderElemCmp(Diagram, getWhich());
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

bool LadderElemMath::GenerateIntCode(IntCode &ic)
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

	prop.idOp1  = newProp->idOp1;
	prop.idOp2  = newProp->idOp2;
	prop.idDest = newProp->idDest;
}

void *LadderElemMath::getProperties(void)
{
	LadderElemMathProp *curProp = new LadderElemMathProp;

	curProp->idOp1  = prop.idOp1;
	curProp->idOp2  = prop.idOp2;
	curProp->idDest = prop.idDest;

	return curProp;
}

LadderElem *LadderElemMath::Clone(void)
{
	LadderElemMath *clone = new LadderElemMath(Diagram, getWhich());
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

bool LadderElemSqrt::GenerateIntCode(IntCode &ic)
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

	prop.idSrc  = newProp->idSrc;
	prop.idDest = newProp->idDest;
}

void *LadderElemSqrt::getProperties(void)
{
	LadderElemSqrtProp *curProp = new LadderElemSqrtProp;

	curProp->idSrc  = prop.idSrc;
	curProp->idDest = prop.idDest;

	return curProp;
}

LadderElem *LadderElemSqrt::Clone(void)
{
	LadderElemSqrt *clone = new LadderElemSqrt(Diagram);
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

bool LadderElemRand::GenerateIntCode(IntCode &ic)
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

	prop.idVar = newProp->idVar;
	prop.idMin = newProp->idMin;
	prop.idMax = newProp->idMax;
}

void *LadderElemRand::getProperties(void)
{
	LadderElemRandProp *curProp = new LadderElemRandProp;

	curProp->idVar = prop.idVar;
	curProp->idMin = prop.idMin;
	curProp->idMax = prop.idMax;

	return curProp;
}

LadderElem *LadderElemRand::Clone(void)
{
	LadderElemRand *clone = new LadderElemRand(Diagram);
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

bool LadderElemAbs::GenerateIntCode(IntCode &ic)
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

	prop.idSrc  = newProp->idSrc;
	prop.idDest = newProp->idDest;
}

void *LadderElemAbs::getProperties(void)
{
	LadderElemAbsProp *curProp = new LadderElemAbsProp;

	curProp->idSrc  = prop.idSrc;
	curProp->idDest = prop.idDest;

	return curProp;
}

LadderElem *LadderElemAbs::Clone(void)
{
	LadderElemAbs *clone = new LadderElemAbs(Diagram);
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

bool LadderElemMove::GenerateIntCode(IntCode &ic)
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

	prop.idSrc  = newProp->idSrc;
	prop.idDest = newProp->idDest;
}

void *LadderElemMove::getProperties(void)
{
	LadderElemMoveProp *curProp = new LadderElemMoveProp;

	curProp->idSrc  = prop.idSrc;
	curProp->idDest = prop.idDest;

	return curProp;
}

LadderElem *LadderElemMove::Clone(void)
{
	LadderElemMove *clone = new LadderElemMove(Diagram);
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

bool LadderElemOpenShort::GenerateIntCode(IntCode &ic)
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

bool LadderElemSetBit::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
	prop.bit    = newProp->bit;
	prop.set    = newProp->set;
}

void *LadderElemSetBit::getProperties(void)
{
	LadderElemSetBitProp *curProp = new LadderElemSetBitProp;

	curProp->idName = prop.idName;
	curProp->bit    = prop.bit;
	curProp->set    = prop.set;

	return curProp;
}

LadderElem *LadderElemSetBit::Clone(void)
{
	LadderElemSetBit *clone = new LadderElemSetBit(Diagram);
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

bool LadderElemCheckBit::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
	prop.bit    = newProp->bit;
	prop.set    = newProp->set;
}

void *LadderElemCheckBit::getProperties(void)
{
	LadderElemCheckBitProp *curProp = new LadderElemCheckBitProp;

	curProp->idName = prop.idName;
	curProp->bit    = prop.bit;
	curProp->set    = prop.set;

	return curProp;
}

LadderElem *LadderElemCheckBit::Clone(void)
{
	LadderElemCheckBit *clone = new LadderElemCheckBit(Diagram);
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

pair<string, string> LadderElemReadAdc::DrawTXT(void)
{
	char txt[50];
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	sprintf(txt, _("{READ ADC %s }"), GetPinADC(name));

	return pair<string, string>(name, txt);
}

bool LadderElemReadAdc::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
}

void *LadderElemReadAdc::getProperties(void)
{
	LadderElemReadAdcProp *curProp = new LadderElemReadAdcProp;

	curProp->idName = prop.idName;

	return curProp;
}

LadderElem *LadderElemReadAdc::Clone(void)
{
	LadderElemReadAdc *clone = new LadderElemReadAdc(Diagram);
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

bool LadderElemSetDa::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
	prop.mode   = newProp->mode;
}

void *LadderElemSetDa::getProperties(void)
{
	LadderElemSetDaProp *curProp = new LadderElemSetDaProp;

	curProp->idName = prop.idName;
	curProp->mode   = prop.mode;

	return curProp;
}

LadderElem *LadderElemSetDa::Clone(void)
{
	LadderElemSetDa *clone = new LadderElemSetDa(Diagram);
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

bool LadderElemReadEnc::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
}

void *LadderElemReadEnc::getProperties(void)
{
	LadderElemReadEncProp *curProp = new LadderElemReadEncProp;

	curProp->idName = prop.idName;

	return curProp;
}

LadderElem *LadderElemReadEnc::Clone(void)
{
	LadderElemReadEnc *clone = new LadderElemReadEnc(Diagram);
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

bool LadderElemResetEnc::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
}

void *LadderElemResetEnc::getProperties(void)
{
	LadderElemResetEncProp *curProp = new LadderElemResetEncProp;

	curProp->idName = prop.idName;

	return curProp;
}

LadderElem *LadderElemResetEnc::Clone(void)
{
	LadderElemResetEnc *clone = new LadderElemResetEnc(Diagram);
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
	prop.forward               = true;  // true = avan�o, false = recuo
	prop.speedup               = false; // true = acelera��o, false = desacelera��o
	prop.StartFromCurrentValue = false; // false = Iniciar ou ir para zero, conforme speedup. true = partir do valor atual at� o valor configurado
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

bool LadderElemMultisetDA::GenerateIntCode(IntCode &ic)
{
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
		ic.Op(INT_MULTISET_DA, "", prop.ramp_abort_mode == RAMP_ABORT_DEFAULT ? Prog.settings.ramp_abort_mode : prop.ramp_abort_mode);
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

    prop.idTime                = newProp->idTime;
	prop.idDesl                = newProp->idDesl;
	prop.gaint                 = newProp->gaint;
	prop.gainr                 = newProp->gainr;
	prop.initval               = newProp->initval;
	prop.linear                = newProp->linear;
	prop.forward               = newProp->forward;
	prop.speedup               = newProp->speedup;
	prop.StartFromCurrentValue = newProp->StartFromCurrentValue;
}

void *LadderElemMultisetDA::getProperties(void)
{
	LadderElemMultisetDAProp *curProp = new LadderElemMultisetDAProp;

    curProp->idTime                = prop.idTime;
	curProp->idDesl                = prop.idDesl;
	curProp->gaint                 = prop.gaint;
	curProp->gainr                 = prop.gainr;
	curProp->initval               = prop.initval;
	curProp->linear                = prop.linear;
	curProp->forward               = prop.forward;
	curProp->speedup               = prop.speedup;
	curProp->StartFromCurrentValue = prop.StartFromCurrentValue;

	return curProp;
}

LadderElem *LadderElemMultisetDA::Clone(void)
{
	LadderElemMultisetDA *clone = new LadderElemMultisetDA(Diagram);
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

bool LadderElemUSS::GenerateIntCode(IntCode &ic)
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

	prop.idName        = newProp->idName;
	prop.id            = newProp->id;
	prop.parameter     = newProp->parameter;
	prop.parameter_set = newProp->parameter_set;
	prop.index         = newProp->index;
}

void *LadderElemUSS::getProperties(void)
{
	LadderElemUSSProp *curProp = new LadderElemUSSProp;

	curProp->idName        = prop.idName;
	curProp->id            = prop.id;
	curProp->parameter     = prop.parameter;
	curProp->parameter_set = prop.parameter_set;
	curProp->index         = prop.index;

	return curProp;
}

LadderElem *LadderElemUSS::Clone(void)
{
	LadderElemUSS *clone = new LadderElemUSS(Diagram, getWhich());
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

	prop.retransmitir = true;
	prop.elem         = MbNodeList_GetByIndex(0)->NodeID;
	prop.idName       = pair<unsigned long, int>(0, 0);

	Diagram->getIO(prop.idName, _("new"), false, (getWhich() == ELEM_READ_MODBUS) ? eType_ReadModbus : eType_WriteModbus);

	MbNodeList_AddRef(prop.elem);
}

pair<string, string> LadderElemModBUS::DrawTXT(void)
{
	char *bot;
	char buf[100];
	MbNodeList *l = MbNodeList_GetByNodeID(prop.elem);

	sprintf(buf, "%s:%d", l->node.name, prop.address);

	if(getWhich() == ELEM_READ_MODBUS) {
		bot = l->node.iface == MB_IFACE_RS485 ? _("{READ MB 485}")  : _("{READ MB ETH}");
	} else {
		bot = l->node.iface == MB_IFACE_RS485 ? _("{WRITE MB 485}") : _("{WRITE MB ETH}");
	}

	return pair<string, string>(buf, bot);
}

bool LadderElemModBUS::GenerateIntCode(IntCode &ic)
{
	string sname = Diagram->getNameIO(prop.idName);
	const char *name = sname.c_str();

	char id[10];
	char addr[10];
	char *MbReady, *MbTimeout;
	const char *stateInOut = ic.getStateInOut();
	int intcode;
	MbNodeList *list;

	list    = MbNodeList_GetByNodeID(prop.elem);
	intcode = getWhich() == ELEM_READ_MODBUS ? INT_READ_MODBUS : INT_WRITE_MODBUS;
	sprintf(id  , "%d", list->node.id);
	sprintf(addr, "%d", prop.address);

	if(list->node.iface == MB_IFACE_RS485) {
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
				ic.Op(intcode, name, id, addr, list->node.ip, (unsigned char)prop.int32);
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
				ic.Op(intcode, name, id, addr, list->node.ip, (unsigned char)prop.int32);
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
	return context.canInsertOther && Prog.settings.mb_list_size;
}

void LadderElemModBUS::internalSetProperties(void *data)
{
	LadderElemModBUSProp *newProp = (LadderElemModBUSProp *)data;

	prop.idName       = newProp->idName;
	prop.retransmitir = newProp->retransmitir;
	prop.elem         = newProp->elem;
}

void *LadderElemModBUS::getProperties(void)
{
	LadderElemModBUSProp *curProp = new LadderElemModBUSProp;

	curProp->idName       = prop.idName;
	curProp->retransmitir = prop.retransmitir;
	curProp->elem         = prop.elem;

	return curProp;
}

LadderElem *LadderElemModBUS::Clone(void)
{
	LadderElemModBUS *clone = new LadderElemModBUS(Diagram, getWhich());
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

bool LadderElemSetPWM::GenerateIntCode(IntCode &ic)
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

	prop.idName     = newProp->idName;
	prop.targetFreq = newProp->targetFreq;
}

void *LadderElemSetPWM::getProperties(void)
{
	LadderElemSetPWMProp *curProp = new LadderElemSetPWMProp;

	curProp->idName     = prop.idName;
	curProp->targetFreq = prop.targetFreq;

	return curProp;
}

LadderElem *LadderElemSetPWM::Clone(void)
{
	LadderElemSetPWM *clone = new LadderElemSetPWM(Diagram);
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

bool LadderElemUART::GenerateIntCode(IntCode &ic)
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

	prop.idName = newProp->idName;
}

void *LadderElemUART::getProperties(void)
{
	LadderElemUARTProp *curProp = new LadderElemUARTProp;

	curProp->idName = prop.idName;

	return curProp;
}

LadderElem *LadderElemUART::Clone(void)
{
	LadderElemUART *clone = new LadderElemUART(Diagram, getWhich());
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

bool LadderElemMasterRelay::GenerateIntCode(IntCode &ic)
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

bool LadderElemShiftRegister::GenerateIntCode(IntCode &ic)
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

LadderElem *LadderElemShiftRegister::Clone(void)
{
	LadderElemShiftRegister *clone = new LadderElemShiftRegister(Diagram);
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

bool LadderElemLUT::GenerateIntCode(IntCode &ic)
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

	prop.idDest       = newProp->idDest;
	prop.idIndex      = newProp->idIndex;
	prop.count        = newProp->count;
	prop.editAsString = newProp->editAsString;
	prop.vals         = newProp->vals;
}

void *LadderElemLUT::getProperties(void)
{
	LadderElemLUTProp *curProp = new LadderElemLUTProp;

	curProp->idDest       = prop.idDest;
	curProp->idIndex      = prop.idIndex;
	curProp->count        = prop.count;
	curProp->editAsString = prop.editAsString;
	curProp->vals         = prop.vals;

	return curProp;
}

LadderElem *LadderElemLUT::Clone(void)
{
	LadderElemLUT *clone = new LadderElemLUT(Diagram);
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

bool LadderElemPiecewise::GenerateIntCode(IntCode &ic)
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

	prop.idDest  = newProp->idDest;
	prop.idIndex = newProp->idIndex;
	prop.count   = newProp->count;
	prop.vals    = newProp->vals;
}

void *LadderElemPiecewise::getProperties(void)
{
	LadderElemPiecewiseProp *curProp = new LadderElemPiecewiseProp;

	curProp->idDest  = prop.idDest;
	curProp->idIndex = prop.idIndex;
	curProp->count   = prop.count;
	curProp->vals    = prop.vals;

	return curProp;
}

LadderElem *LadderElemPiecewise::Clone(void)
{
	LadderElemPiecewise *clone = new LadderElemPiecewise(Diagram);
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

bool LadderElemFmtString::GenerateIntCode(IntCode &ic)
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

	prop.idVar = newProp->idVar;
	prop.txt   = newProp->txt;
}

void *LadderElemFmtString::getProperties(void)
{
	LadderElemFmtStringProp *curProp = new LadderElemFmtStringProp;

	curProp->idVar = prop.idVar;
	curProp->txt   = prop.txt;

	return curProp;
}

LadderElem *LadderElemFmtString::Clone(void)
{
	LadderElemFmtString *clone = new LadderElemFmtString(Diagram, getWhich());
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

bool LadderElemYaskawa::GenerateIntCode(IntCode &ic)
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

	prop.id    = newProp->id;
	prop.idVar = newProp->idVar;
	prop.txt   = newProp->txt;
}

void *LadderElemYaskawa::getProperties(void)
{
	LadderElemYaskawaProp *curProp = new LadderElemYaskawaProp;

	curProp->id    = prop.id;
	curProp->idVar = prop.idVar;
	curProp->txt   = prop.txt;

	return curProp;
}

LadderElem *LadderElemYaskawa::Clone(void)
{
	LadderElemYaskawa *clone = new LadderElemYaskawa(Diagram, getWhich());
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

bool LadderElemPersist::GenerateIntCode(IntCode &ic)
{
	string svar = Diagram->getNameIO(prop.idVar);
	const char *var  = svar.c_str();

	int EepromAddrFree = ic.getEepromAddr();
	if(EepromAddrFree < 0) {
		Error(_("Muitas vari�veis persistentes!"));
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

	prop.idVar = newProp->idVar;
}

void *LadderElemPersist::getProperties(void)
{
	LadderElemPersistProp *curProp = new LadderElemPersistProp;

	curProp->idVar = prop.idVar;

	return curProp;
}

LadderElem *LadderElemPersist::Clone(void)
{
	LadderElemPersist *clone = new LadderElemPersist(Diagram);
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

bool LadderElemX::GenerateIntCode(IntCode &ic)
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
	context.Diagram         = this;
	context.ParallelStart   = nullptr;
	context.SelectedElem    = nullptr;
	context.SelectedCircuit = nullptr;

	context.SelectedState   = SELECTED_NONE;

	CheckPointLevels     =  0;
	CheckPointLevelsMax  = 30;
	CheckpointBeginCount =  0;
	CheckpointDoRollback = false;
	isCheckpointEmpty    = true;

	copiedElement = nullptr;
	copiedRung    = nullptr;

	NeedScrollSelectedIntoView = false;

	NewRung(false);

	updateContext();

	// As acoes executadas durante a inicializacao (criacao da linha e PlaceHolder) nao devem ser desfeitas
	UndoList.clear();
	CheckPointLevels = 0;
}

LadderDiagram::LadderDiagram(void)
{
	IO = new mapIO(this);

	Init();
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
		if(rungs[i]->IsComment()) continue;

		ic.Comment("");
        ic.Comment(_("start rung %d"), i+1);
		ic.ClearParallelCount();
        ic.Op(INT_COPY_BIT_TO_BIT, "$rung_top", "$mcr");
        ic.SimState(&(Prog.rungPowered[i]), "$rung_top");

		ret = rungs[i]->GenerateIntCode(ic);
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
	vector<LadderCircuit>::size_type i;

	for(i = 0; i < rungs.size() && subckt == nullptr; i++) {
		subckt = rungs[i]->getSubcktForElement(elem);
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
			if(context.SelectedElem->getWhich() == ELEM_PLACEHOLDER) {
				context.canDelete = false;
			}

			if(context.SelectedState == SELECTED_RIGHT || 
				context.SelectedElem->getWhich() == ELEM_PLACEHOLDER) {
					vector<LadderCircuit *>::iterator it;
					if(i >= 0) {
						it = rungs.begin() + i;
						context.canInsertEnd = (*it)->IsLast(context.SelectedElem);
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
		thisWidth = rungs[i]->getWidthTXT(0);
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
		height += rungs[i]->getHeightTXT() + 1;
	}

	return height;
}

int  LadderDiagram::RungContainingElement(LadderElem *elem)
{
	vector<LadderCircuit>::size_type i;

	if(elem != nullptr) {
		for(i = 0; i < rungs.size(); i++) {
			if(rungs[i]->getSubcktForElement(elem) != nullptr)
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
	return (n < rungs.size()) ? rungs[n]->IsEmpty() : true;
}

void LadderDiagram::NewRung(bool isAfter)
{
	vector<LadderCircuit *>::iterator it;
	int position = RungContainingSelected();

	CheckpointBegin(_("Inserir Linha"));

	// Registro da acao para desfazer / refazer
	UndoRedoAction action;
	UndoRedoData *data = new UndoRedoData;

	data->NewRung.rung = new LadderCircuit(true);

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
	(*it)->AddElement(elem, context);

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

	LadderCircuit *tmp = rungs[rung];
	rungs[rung] = rungs[rung + offset];
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

	vector<LadderCircuit *>::iterator it = rungs.begin() + rung;

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

	(*it)->updateIO(true);
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
		delete copiedRung;
	}

	// Copia o elemento
	copiedRung = rungs[pos]->Clone();

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
		copiedRung->updateIO(false);

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
		copiedRung = copiedRung->Clone();

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
			vector<LadderCircuit *>::iterator it = rungs.begin() + position;
			(*it)->AddPlaceHolderIfNoEOL(context);
		}

		SelectElement(elem, elem->IsEOL() ? SELECTED_LEFT : SELECTED_RIGHT);
		updateContext();
	} else {
		// Ocorreu um erro! Marca o checkpoint para remocao
		CheckpointRollback();

		// Se o elemento for descartado, deve descartar todos os I/Os que ele houver registrado.
		elem->updateIO(true);

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
	if(rungs[rung]->DelElement(elem, context)) {
		elem->updateIO(true);
		rungs[rung]->RemoveUnnecessarySubckts(context);
		rungs[rung]->AddPlaceHolderIfNoEOL(context);
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

	// Primeiro descarta um elemento previamente copiado
	if(copiedElement != nullptr) {
		delete copiedElement;
	}

	// Copia o elemento
	copiedElement = elem->Clone();

	// Retorna com sucesso!
	return true;
}

bool LadderDiagram::PasteElement(void)
{
	bool ret = false;

	if(copiedElement == nullptr) return false; // Nao existe elemento copiado

	LadderElem *newCopiedElement = copiedElement->Clone();

	CheckpointBegin(_("Colar Elemento"));

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

		rungs[it]->DrawTXT(DisplayMatrix, &cx, &cy, true, ColsAvailable);

		cx  = OffsetX;
		cy += (rungs[it]->getHeightTXT() + 1) * POS_HEIGHT;
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

// Tries to insert a parallel subcircuit between Prog.ParallelStart and currently selected object
bool LadderDiagram::InsertParallel(LadderElem *elem)
{
	bool ret = false;
	unsigned int i, CurrentRung, UseCurrentParallel = 1;
	InsertionPoint StartPoint = { 0, { context.ParallelStart, nullptr }, nullptr, nullptr};
	InsertionPoint EndPoint   = { 0, { context.SelectedElem , nullptr }, nullptr, nullptr};

	// First we should to remove last saved state because UndoRemember was called before to add this new elemment.
	// We don't want user going back to this state (with parallel start)!
	//UndoForget();

	// Phase 1: check if Prog.ParallelStart and currently selected object are in the same subcircuit.
	for(i=0; i < rungs.size(); i++) {
		rungs[i]->ElemInSubcktSeries(context, &StartPoint);
		if(StartPoint.series != nullptr) {
			// Start found, now we will search for End in the same rung and stop the search.
			rungs[i]->ElemInSubcktSeries(context, &EndPoint);
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
							rungs[CurrentRung]->ElemInSubcktSeries(context, &EndPoint);
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
	} else {
		Error(_("Imposs�vel criar paralelo entre os pontos selecionados!"));
	}

	// Phase 5: free Prog.ParallelStart and collapse.
	context.canDelete = true; // Forca a exclusao de ParallelStart
	DelElement(context.ParallelStart);
	context.ParallelStart = nullptr;
	rungs[CurrentRung]->RemoveUnnecessarySubckts(context);

	return ret;
}

bool LadderDiagram::acceptIO(unsigned long id, enum eType type)
{
	vector<LadderCircuit>::size_type it;

	// Se o tipo atual do I/O for reservado, o novo tipo deve ser reservado tambem.
	if(IO->getDetails(id).type == eType_Reserved) {
		type = eType_Reserved;
	}

	for(it = 0; it < rungs.size(); it++) {
		if(!rungs[it]->acceptIO(id, type)) {
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
				if(ShowDialog(false, "Nome Alterado", "Nome '%s' utilizado tamb�m em outros elementos.\n"
					"Alterar o nome em todos os elementos que o utilizam?", IO->getName(newpin.first).c_str()) == eReply_No) {
						newpin.first = 0;
				}

			}
		}

		// Verifica se o tipo do I/O eh aceito pelos outros elementos que ja o utilizam
		if(newpin.first && !acceptIO(newpin.first, type)) {
			// Alguem compartilhando desta variavel nao aceita a alteracao de tipo. Cancela a alteracao!
			ShowDialog(true, "Tipo Inv�lido", "Conflito entre tipos para '%s' ! Opera��o n�o permitida.", IO->getName(newpin.first).c_str());
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
		ShowDialog(true, "Nome Inv�lido", "Nome '%s' reservado para uso interno, favor escolher outro nome.", name.c_str());
		return false;
	} else  if(type == eType_InternalFlag && !name_is_internal_flag) {
		ShowDialog(true, "Nome Inv�lido", "Para o tipo 'Flag Interna' � obrigat�rio selecionar um item da lista.");
		return false;
	}

	// Check for Variable and Number restrictions
	if(!name_is_number && !IsValidVarName(name)) {
		ShowDialog(true, "Nome Inv�lido", "%s '%s' inv�lido!\n\nVari�vel: Apenas letras (A a Z), n�meros ou _ (underline) e n�o inicar com n�mero\nN�mero: Apenas n�meros, podendo iniciar por - (menos)", FieldName ? FieldName : "Nome", name.c_str());
		return false;
	} else if((Rules & VALIDATE_IS_VAR) && name_is_number) {
		ShowDialog(true, "Nome Inv�lido", "'%s' n�o pode ser n�mero!", FieldName ? FieldName : name.c_str());
		return false;
	} else if((Rules & VALIDATE_IS_NUMBER) && !name_is_number) {
		ShowDialog(true, "N�mero Inv�lido", "'%s' deve ser n�mero!", FieldName ? FieldName : name.c_str());
		return false;
	} else if(name_is_number && !IsValidNumber(name)) {
		ShowDialog(true, "N�mero Inv�lido", "N�mero '%s' inv�lido!", name.c_str());
		return false;
	}

	// Check for Number Limits
	if((MinVal || MaxVal) && (Rules & (VALIDATE_IS_NUMBER | VALIDATE_IS_VAR_OR_NUMBER)) && name_is_number) {
		val = atoi(name.c_str());
		if(val < MinVal || val > MaxVal) {
			ShowDialog(true, "N�mero Inv�lido", "'%s' fora dos limites! Deve estar entre %d e %d.", FieldName ? FieldName : name.c_str(), MinVal, MaxVal);
			return false;
		}
	}

	// Check for Type restrictions
	if(!name_is_number) {
		// If types must match and types are different or cannot accept io_pending type, generates an error
		if((Rules & VALIDATE_TYPES_MUST_MATCH) && current_type != type && !((current_type == eType_Pending || type == eType_Pending) && (Rules & VALIDATE_ACCEPT_IO_PENDING))) {
			ShowDialog(true, "Tipo Inv�lido", "Conflito entre tipos! Opera��o n�o permitida.");
		} else if(type == eType_DigInput || type == eType_DigOutput || type == eType_InternalRelay) {
			if(type == current_type) { // no type change, ok!
				ret = true;
			} else if(current_type == eType_Pending) { // Inexistent name, ok!
				ret = true;
			} else if(type == eType_DigInput && (current_type == eType_DigOutput || current_type == eType_InternalRelay) && !acceptIO(newid, type)) {
				ShowDialog(true, "Sa�da em uso", "N�o � poss�vel alterar para Entrada.");
			} else if(!_stricmp(old_name.c_str(), name.c_str())) { // name not changed, ok!
				ret = true;
			// name changed, check for type changes
			} else if(type == eType_InternalRelay && current_type == eType_DigOutput) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(true, "Tipo inv�lido", "Conflito de tipos: Rele Interno <-> Sa�da");
				} else if(ShowDialog(false, "Confirmar altera��o de Sa�da para Rele Interno", "J� existe uma Sa�da com este nome. Alterar para Rele Interno?") == eReply_Yes) {
					ret = true;
				}
			} else if(type == eType_DigOutput && current_type == eType_InternalRelay) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(true, "Tipo inv�lido", "Conflito de tipos: Rele Interno <-> Sa�da");
				} else if(ShowDialog(false, "Confirmar altera��o de Rele Interno para Sa�da", "J� existe um Rele Interno com este nome. Alterar para Sa�da?") == eReply_Yes) {
					ret = true;
				}
			} else if(type == eType_DigOutput && current_type == eType_DigInput) { // changing existent input to output, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(true, "Tipo inv�lido", "Conflito de tipos: Entrada <-> Sa�da");
				} else if(ShowDialog(false, "Confirmar altera��o de Entrada para Sa�da", "J� existe uma Entrada com este nome. Alterar para Sa�da?") == eReply_Yes) {
					ret = true;
				}
			} else if(type == eType_InternalRelay && current_type == eType_DigInput) { // changing existent input to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					ShowDialog(true, "Tipo inv�lido", "Conflito de tipos: Rele Interno <-> Entrada");
				} else if(ShowDialog(false, "Confirmar altera��o de Entrada para Rel� Interno", "J� existe uma Entrada com este nome. Alterar para Rel� Interno?") == eReply_Yes) {
					ret = true;
				}
			} else { // no wrong conditions, ok!
				ret = true;
			}
		} else if(current_type == eType_Counter || current_type == eType_TOF || current_type == eType_TON) {
			if(type == eType_Counter || type == eType_TOF || type == eType_TON) {
				ret = true;
			} else {
				ShowDialog(true, "Nome inv�lido", "'Nome' deve ser um contador ou timer!");
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

	// Se estamos adicionando um checkpoint em Desfazer, a lista de Refazer se torna invalida!
	// Apagando a lista de Refazer...
	while(RedoList.size() > 0) {
		DiscardCheckpoint(false);
	}

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
					ExecuteAction(true, true, action);
					UndoList.pop_back();
				} while(action.action != eCheckpoint);
			} else if(isCheckpointEmpty) {
				// Fechado um checkpoint sem registro de acoes.
				// Devemos remover o marcador de checkpoint e diminuir 1 nivel
				UndoList.pop_back();
				CheckPointLevels--;
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
			LadderCircuit *tmp = rungs[data->PushRung.pos];
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
				delete data->DelRung.rung;
			}
		} else if(IsUndo) {
			rungs.insert(rungs.begin() + data->DelRung.pos, data->DelRung.rung);
		} else {
			rungs.erase(rungs.begin() + data->DelRung.pos);
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
