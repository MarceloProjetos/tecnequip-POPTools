#include "poptools.h"

//Classe LadderElem
void LadderElem::Init(void)
{
	isEndOfLine   = false;
	poweredAfter  = false;
	selectedState = SELECTED_NONE;
}

LadderElem::LadderElem(void)
{
	Init();
}

LadderElem::LadderElem(bool EOL, bool Comment, bool Formatted, bool Name, int elemWhich)
{
	Init();

	which       = elemWhich;
	isEndOfLine = EOL;
	isName      = Name;
	isComment   = Comment;
	isFormatted = Formatted;
}

// Classe LadderElemPlaceHolder
LadderElemPlaceHolder::LadderElemPlaceHolder(void) : LadderElem(false, false, false, true, ELEM_PLACEHOLDER)
{
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

// Classe LadderElemComment
LadderElemComment::LadderElemComment(void) : LadderElem(false, true, false, true, ELEM_COMMENT)
{
	str = _("--add comment here--");
}

pair<string, string> LadderElemComment::DrawTXT(void)
{
	string first, second = "";

	char tbuf[MAX_COMMENT_LEN];
	char tlbuf[MAX_COMMENT_LEN+8];

	strcpy(tbuf, str.c_str());
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

void LadderElemComment::setProperties(string newStr)
{
	str = newStr;
}

inline int LadderElemComment::getWidthTXT(void)
{
	char tbuf[MAX_COMMENT_LEN];

	strcpy(tbuf, str.c_str());
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

// Classe LadderElemContact
LadderElemContact::LadderElemContact(void) : LadderElem(false, false, false, true, ELEM_CONTACTS)
{
	name    = _("in");
	negated = false;
	type    = IO_TYPE_DIG_INPUT;
	bit     = 0;
}

pair<string, string> LadderElemContact::DrawTXT(void)
{
	char ch, buf[4], name_with_type[MAX_NAME_LEN+4];

	buf[0] = ']';
	buf[1] = negated ? '/' : ' ';
	buf[2] = '[';
	buf[3] = '\0';

	// TODO: Internacionalizar
	switch(type) {
	case IO_TYPE_DIG_INPUT:
		ch = 'E';
		break;

	case IO_TYPE_DIG_OUTPUT:
		ch = 'S';
		break;

	case IO_TYPE_INTERNAL_RELAY:
		ch = 'R';
		break;

	case IO_TYPE_INTERNAL_FLAG:
		ch = 'F';
		break;

	default:
		ch = '?';
		break;
	}
	sprintf(name_with_type, "%s (%c)", name.c_str(), ch);

	return pair<string, string>(name_with_type, buf);
}

bool LadderElemContact::GenerateIntCode(IntCode &ic)
{
	string buf;
	if(type == IO_TYPE_INTERNAL_FLAG) {
		buf = "$" + name;
	} else {
		buf = name;
	}

	ic.OpBit(negated ? INT_IF_BIT_SET : INT_IF_BIT_CLEAR, buf.c_str(), bit);
		ic.OpBit(INT_CLEAR_BIT, ic.getStateInOut(), bit);
	ic.Op   (INT_END_IF);

	return true;
}

bool LadderElemContact::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void LadderElemContact::getProperties(string &curName, bool &curNegated, unsigned int &curType, unsigned char &curBit)
{
	curName    = name;
	curNegated = negated;
	curType    = type;
	curBit     = bit;
}

void LadderElemContact::setProperties(string newName, bool newNegated, unsigned int newType, unsigned char newBit)
{
	name    = newName;
	negated = newNegated;
	type    = newType;
	bit     = newBit;
}

// Classe LadderElemCoil
LadderElemCoil::LadderElemCoil(void) : LadderElem(true, false, false, true, ELEM_COIL)
{
	name      = _("out");
	negated   = false;
	setOnly   = false;
	resetOnly = false;
	type      = IO_TYPE_DIG_OUTPUT;
	bit       = 0;
}

pair<string, string> LadderElemCoil::DrawTXT(void)
{
	char buf[4], name_with_type[MAX_NAME_LEN+4];

    buf[0] = '(';
    if(negated) {
        buf[1] = '/';
    } else if(setOnly) {
        buf[1] = 'S';
    } else if(resetOnly) {
        buf[1] = 'R';
    } else {
        buf[1] = ' ';
    }
    buf[2] = ')';
    buf[3] = '\0';

	// TODO: Internacionalizar
	sprintf(name_with_type, "%s (%c)", name.c_str(), type == IO_TYPE_DIG_OUTPUT ? 'S' : 'R');

	return pair<string, string>(name_with_type, buf);
}

bool LadderElemCoil::GenerateIntCode(IntCode &ic)
{
	const char *stateInOut = ic.getStateInOut();
	if(negated) {
		ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.OpBit(INT_CLEAR_BIT, name.c_str(), bit);
		ic.Op(INT_ELSE);
		ic.OpBit(INT_SET_BIT, name.c_str(), bit);
		ic.Op(INT_END_IF);
	} else if(setOnly) {
		ic.OpBit(INT_IF_BIT_SET, stateInOut, bit);
		ic.OpBit(INT_SET_BIT, name.c_str(), bit);
		ic.Op(INT_END_IF);
	} else if(resetOnly) {
		ic.OpBit(INT_IF_BIT_SET, stateInOut, bit);
		ic.OpBit(INT_CLEAR_BIT, name.c_str(), bit);
		ic.Op(INT_END_IF);
	} else {
		ic.OpBit(INT_COPY_BIT_TO_BIT, name.c_str(), stateInOut, bit);
	}

	return true;
}

bool LadderElemCoil::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemCoil::setProperties(string newName, bool newNegated, bool newSetOnly, bool newResetOnly,
	unsigned int newType, unsigned char newBit)
{
	name      = newName;
	negated   = newNegated;
	setOnly   = newSetOnly;
	resetOnly = newResetOnly;
	type      = newType;
	bit       = newBit;
}

// Classe LadderElemTimer
LadderElemTimer::LadderElemTimer(int which) : LadderElem(which == ELEM_RTO ? true : false, false, false, true, which)
{
	name  = _("new");
	delay = 100000;
}

//-----------------------------------------------------------------------------
// Calculate the period in scan units from the period in microseconds, and
// raise an error if the given period is unachievable.
//-----------------------------------------------------------------------------
int LadderElemTimer::TimerPeriod(void)
{
    unsigned int period = (delay / Prog.settings.cycleTime) - 1;

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

	if(delay >= 1000*1000) {
		sprintf(buf, "[\x01%s\x02 %.3f s]", s, delay/1000000.0);
	} else if(delay >= 100*1000) {
		sprintf(buf, "[\x01%s\x02 %.1f ms]", s, delay/1000.0);
	} else {
		sprintf(buf, "[\x01%s\x02 %.2f ms]", s, delay/1000.0);
	}

	return pair<string, string>(name, buf);
}

bool LadderElemTimer::GenerateIntCode(IntCode &ic)
{
	const char *stateInOut = ic.getStateInOut();

	switch(getWhich()) {
        case ELEM_RTO: {
            int period = TimerPeriod();

			ic.Op(INT_IF_VARIABLE_LES_LITERAL, name.c_str(), period);

            ic.Op(INT_IF_BIT_SET, stateInOut);
			ic.Op(INT_INCREMENT_VARIABLE, name.c_str());
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

            ic.Op(INT_IF_VARIABLE_LES_LITERAL, name.c_str(), period);

            ic.Op(INT_INCREMENT_VARIABLE, name.c_str());
            ic.Op(INT_CLEAR_BIT, stateInOut);
            ic.Op(INT_END_IF);

            ic.Op(INT_ELSE);

            ic.Op(INT_SET_VARIABLE_TO_LITERAL, name.c_str());

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
            sprintf(antiGlitchName, "$%s_antiglitch", name.c_str());
            ic.Op(INT_IF_BIT_CLEAR, antiGlitchName);
                ic.Op(INT_SET_VARIABLE_TO_LITERAL, name.c_str(), period);
            ic.Op(INT_END_IF);
            ic.Op(INT_SET_BIT, antiGlitchName);
            
            ic.Op(INT_IF_BIT_CLEAR, stateInOut);

            ic.Op(INT_IF_VARIABLE_LES_LITERAL, name.c_str(), period);

            ic.Op(INT_INCREMENT_VARIABLE, name.c_str());
            ic.Op(INT_SET_BIT, stateInOut);
            ic.Op(INT_END_IF);

            ic.Op(INT_ELSE);

            ic.Op(INT_SET_VARIABLE_TO_LITERAL, name.c_str());

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

void LadderElemTimer::setProperties(string newName, int newDelay)
{
	name  = newName;
	delay = newDelay;
}

// Classe LadderElemRTC
LadderElemRTC::LadderElemRTC(void) : LadderElem(false, false, false, true, ELEM_RTC)
{
	time_t rawtime;
	struct tm * t;

	time ( &rawtime );
	t = localtime ( &rawtime );

	t->tm_year += 1900;
	t->tm_mon++;
	t->tm_sec = t->tm_sec > 59 ? 59 : t->tm_sec;

	mode = ELEM_RTC_MODE_FIXED;
	wday = 255;

	start.tm_mday = 0;
	start.tm_mon  = 0;
	start.tm_year = 0;
	start.tm_hour = t->tm_hour;
	start.tm_min  = t->tm_min;
	start.tm_sec  = t->tm_sec;

	end.tm_mday   = 0;
	end.tm_mon    = 0;
	end.tm_year   = 0;
	end.tm_hour   = t->tm_hour;
	end.tm_min    = t->tm_min;
	end.tm_sec    = t->tm_sec;
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
		struct tm *ptm = linha ? &end : &start;

		if(wday & (1 << 7)) 
		{
			int i;

			sprintf(linha ? bufe : bufs, "[\x01%s %02d:%02d:%02d\x02]", _("SMTWTFS"),
				ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

			if(linha || mode == ELEM_RTC_MODE_FIXED) {
				for(i=0; i<7; i++) {
					if(!(wday & (1<<i))) {
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

	if(mode == ELEM_RTC_MODE_FIXED) {
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

void LadderElemRTC::setProperties(int newMode, unsigned char newWday, struct tm newStart, struct tm newEnd)
{
	mode  = newMode;
	wday  = newWday;
	start = newStart;
	end   = newEnd;
}

// Classe LadderElemCounter
LadderElemCounter::LadderElemCounter(int which) : LadderElem(which == ELEM_CTC ? true : false, false, false, true, which)
{
	name = _("new");
	max  = 0;
}

pair<string, string> LadderElemCounter::DrawTXT(void)
{
	int which = getWhich();
	char *s, op, buf[256];

	if(which == ELEM_CTC) {
		sprintf(buf, _("{\x01""CTC\x02 0:%d}"), max);
	} else {
		if(which == ELEM_CTU) {
			s = _("CTU");
			op = '>';
		} else if(which == ELEM_CTD) {
			s = _("CTD");
			op = '<';
		} else oops();

		sprintf(buf, "[\x01%s\x02 %c=%d]", s, op, max);
	}

	return pair<string, string>(name, buf);
}

bool LadderElemCounter::GenerateIntCode(IntCode &ic)
{
    string storeName;
	const char *stateInOut = ic.getStateInOut();
    storeName = ic.GenSymOneShot();

	switch(getWhich()) {
        case ELEM_CTU: {
            ic.Op(INT_IF_BIT_SET, stateInOut);
				ic.Op(INT_IF_BIT_CLEAR, storeName.c_str());
					ic.Op(INT_INCREMENT_VARIABLE, name.c_str());
                ic.Op(INT_END_IF);
            ic.Op(INT_END_IF);
			ic.Op(INT_COPY_BIT_TO_BIT, storeName.c_str(), stateInOut);

			ic.Op(INT_IF_VARIABLE_LES_LITERAL, name.c_str(), max);
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
					ic.Op(INT_SET_VARIABLE_SUBTRACT, name.c_str(), name.c_str(), "$scratch", 0, 0);
                ic.Op(INT_END_IF);
            ic.Op(INT_END_IF);
			ic.Op(INT_COPY_BIT_TO_BIT, storeName.c_str(), stateInOut);

			ic.Op(INT_IF_VARIABLE_GRT_VARIABLE, name.c_str(), max);
                ic.Op(INT_CLEAR_BIT, stateInOut);
            ic.Op(INT_ELSE);
                ic.Op(INT_SET_BIT, stateInOut);
            ic.Op(INT_END_IF);
            break;
        }
        case ELEM_CTC: {
            ic.Op(INT_IF_BIT_SET, stateInOut);
                ic.Op(INT_IF_BIT_CLEAR, storeName.c_str());
                    ic.Op(INT_INCREMENT_VARIABLE, name.c_str());
                    ic.Op(INT_IF_VARIABLE_LES_LITERAL, name.c_str(), max+1);
                    ic.Op(INT_ELSE);
                        ic.Op(INT_SET_VARIABLE_TO_LITERAL, name.c_str(), (SWORD)0);
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

void LadderElemCounter::setProperties(string newName, int newMax)
{
	name = newName;
	max  = newMax;
}

// Classe LadderElemReset
LadderElemReset::LadderElemReset(void) : LadderElem(true, false, false, true, ELEM_RES)
{
	name = _("new");
}

pair<string, string> LadderElemReset::DrawTXT(void)
{
	return pair<string, string>(name, _("{RES}"));
}

bool LadderElemReset::GenerateIntCode(IntCode &ic)
{
	const char *stateInOut = ic.getStateInOut();

	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_SET_VARIABLE_TO_LITERAL, name.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemReset::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemReset::setProperties(string newName)
{
	name = newName;
}

// Classe LadderElemOneShot
LadderElemOneShot::LadderElemOneShot(int which) : LadderElem(false, false, false, false, which)
{
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

// Classe LadderElemCmp
LadderElemCmp::LadderElemCmp(int which) : LadderElem(false, false, false, false, which)
{
	op1 = _("var");
	op2 = "1";
}

pair<string, string> LadderElemCmp::DrawTXT(void)
{
	char *s;

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

	l1 = 2 + 1 + strlen(s) + op1.size();
	l2 = 2 + 1 + op2.size();
	lmax = max(l1, l2);

	memset(s1, ' ', sizeof(s1));
	s1[0] = '[';
	s1[lmax-1] = ']';
	s1[lmax] = '\0';
	strcpy(s2, s1);

	memcpy(s1+1, op1.c_str(), op1.size());
	memcpy(s1+op1.size()+2, s, strlen(s));
	memcpy(s2+2, op2.c_str(), op2.size());

	return pair<string, string>(s1, s2);
}

bool LadderElemCmp::GenerateIntCode(IntCode &ic)
{
	const char *cop1 = ic.VarFromExpr(op1.c_str(), "$scratch");
	const char *cop2 = ic.VarFromExpr(op2.c_str(), "$scratch2");

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

void LadderElemCmp::setProperties(string newOp1, string newOp2)
{
	op1 = newOp1;
	op2 = newOp2;
}

// Classe LadderElemMath
LadderElemMath::LadderElemMath(int which) : LadderElem(true, false, false, true, which)
{
	dest = _("dest");
	op1  = _("src");
	op2 = "1";
}

pair<string, string> LadderElemMath::DrawTXT(void)
{
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
	memcpy(top+lt, dest.c_str(), dest.size());
	lt += dest.size() + 2;
	top[lt++] = ':';
	top[lt++] = '=';

	int lb = 2;
	memcpy(bot+lb, op1.c_str(), op1.size());
	lb += op1.size() + 1;
	bot[lb++] = op;
	lb++;
	memcpy(bot+lb, op2.c_str(), op2.size());
	lb += op2.size();

	int l = max(lb, lt - 2);
	top[l+2] = '}'; top[l+3] = '\0';
	bot[l] = '}'; bot[l+1] = '\0';

	return pair<string, string>(top, bot);
}

bool LadderElemMath::GenerateIntCode(IntCode &ic)
{
	if(IsNumber(dest.c_str())) {
		Error(_("Math instruction: '%s' not a valid destination."), dest.c_str());
		CompileError();
	}
	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	const char *cop1 = ic.VarFromExpr(op1.c_str(), "$scratch");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	const char *cop2 = ic.VarFromExpr(op2.c_str(), "$scratch2");

	int intOp;
	switch(getWhich()) {
		case ELEM_ADD: intOp = INT_SET_VARIABLE_ADD     ; break;
		case ELEM_SUB: intOp = INT_SET_VARIABLE_SUBTRACT; break;
		case ELEM_MUL: intOp = INT_SET_VARIABLE_MULTIPLY; break;
		case ELEM_DIV: intOp = INT_SET_VARIABLE_DIVIDE  ; break;
		case ELEM_MOD: intOp = INT_SET_VARIABLE_MODULO  ; break;
		default: oops();
	}

	ic.Op(intOp, dest.c_str(), cop1, cop2, 0, 0);

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemMath::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemMath::setProperties(string newOp1, string newOp2, string newDest)
{
	op1  = newOp1;
	op2  = newOp2;
	dest = newDest;
}

// Classe LadderElemSqrt
LadderElemSqrt::LadderElemSqrt(void) : LadderElem(true, false, false, false, ELEM_SQRT)
{
	dest = _("dest");
	src  = _("src");
}

pair<string, string> LadderElemSqrt::DrawTXT(void)
{
	char top[256];
	char bot[256];

	sprintf(top, "{%s :=}", dest.c_str());
	sprintf(bot, _("{%s SQRT}"), src.c_str());

	return pair<string, string>(top, bot);
}

bool LadderElemSqrt::GenerateIntCode(IntCode &ic)
{
	if(IsNumber(dest.c_str())) {
		Error(_("Sqrt instruction: '%s' not a valid destination."), dest.c_str());
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	if(IsNumber(src.c_str())) {
		ic.Op(INT_SET_VARIABLE_TO_LITERAL, dest.c_str(), CheckMakeNumber(src.c_str()));
	} else {
		ic.Op(INT_SET_VARIABLE_TO_VARIABLE, dest.c_str(), src.c_str(), 0);
	}

		ic.Op(INT_SQRT, dest.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemSqrt::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemSqrt::setProperties(string newSrc, string newDest)
{
	src  = newSrc;
	dest = newDest;
}

// Classe LadderElemRand
LadderElemRand::LadderElemRand(void) : LadderElem(true, false, false, true, ELEM_RAND)
{
	var = _("var");
	min = "0";
	max = "100";
}

pair<string, string> LadderElemRand::DrawTXT(void)
{
	char top[256];
	char bot[256];

	sprintf(top, "{ %s := }", var.c_str());
	sprintf(bot, "{%s <= ? <= %s}", min.c_str(), max.c_str());

	return pair<string, string>(top, bot);
}

bool LadderElemRand::GenerateIntCode(IntCode &ic)
{
	if(IsNumber(var.c_str())) {
		Error(_("Rand instruction: '%s' not a valid destination."), var);
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_RAND, var.c_str(), min.c_str(), max.c_str(), 0, 0);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemRand::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemRand::setProperties(string newVar, string newMin, string newMax)
{
	var = newVar;
	min = newMin;
	max = newMax;
}

// Classe LadderElemAbs
LadderElemAbs::LadderElemAbs(void) : LadderElem(true, false, false, false, ELEM_ABS)
{
	src  = _("src");
	dest = _("dest");
}

pair<string, string> LadderElemAbs::DrawTXT(void)
{
	char top[256];
	char bot[256];

	sprintf(top, "{ %s :=}", dest.c_str());
	sprintf(bot, "{ |%s| }", src.c_str());

	return pair<string, string>(top, bot);
}

bool LadderElemAbs::GenerateIntCode(IntCode &ic)
{
	if(IsNumber(dest.c_str())) {
		Error(_("Abs instruction: '%s' not a valid destination."), dest.c_str());
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_IF_VARIABLE_LES_LITERAL, src.c_str(), (SWORD)0);
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch_int", CheckMakeNumber("-1"));
			ic.Op(INT_SET_VARIABLE_MULTIPLY, dest.c_str(), src.c_str(), "$scratch_int", 0, 0);
		ic.Op(INT_ELSE);
			if(IsNumber(src.c_str())) {
				ic.Op(INT_SET_VARIABLE_TO_LITERAL, dest.c_str(), CheckMakeNumber(src.c_str()));
			} else {
				ic.Op(INT_SET_VARIABLE_TO_VARIABLE, dest.c_str(), src.c_str(), 0);
			}
		ic.Op(INT_END_IF);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemAbs::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemAbs::setProperties(string newSrc, string newDest)
{
	src  = newSrc;
	dest = newDest;
}

// Classe LadderElemMove
LadderElemMove::LadderElemMove(void) : LadderElem(true, false, false, false, ELEM_MOVE)
{
	src  = _("src");
	dest = _("dest");
}

pair<string, string> LadderElemMove::DrawTXT(void)
{
	char top[256];
	char bot[256];

	sprintf(top, "{%s :=}", dest.c_str());
	sprintf(bot, _("{%s MOV}"), src.c_str());

	return pair<string, string>(top, bot);
}

bool LadderElemMove::GenerateIntCode(IntCode &ic)
{
	if(IsNumber(dest.c_str())) {
		Error(_("Move instruction: '%s' not a valid destination."), dest.c_str());
		CompileError();
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	if(IsNumber(src.c_str())) {
		ic.Op(INT_SET_VARIABLE_TO_LITERAL, dest.c_str(), CheckMakeNumber(src.c_str()));
	} else {
		ic.Op(INT_SET_VARIABLE_TO_VARIABLE, dest.c_str(), src.c_str(), 0);
	}

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemMove::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemMove::setProperties(string newSrc, string newDest)
{
	src  = newSrc;
	dest = newDest;
}

// Classe LadderElemOpenShort
LadderElemOpenShort::LadderElemOpenShort(int which) : LadderElem(false, false, false, true, which)
{
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

// Classe LadderElemSetBit
LadderElemSetBit::LadderElemSetBit(void) : LadderElem(false, false, false, true, ELEM_PADDING)
{
	name = _("new");
	bit  = 0;
	set  = false;
}

pair<string, string> LadderElemSetBit::DrawTXT(void)
{
	char str[100];

	sprintf(str, _("{%s BIT:%d}"), set ? _("SET") : _("RST"), bit);

	return pair<string, string>(name, str);
}

bool LadderElemSetBit::GenerateIntCode(IntCode &ic)
{
	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	if(set) {
		ic.OpBit(INT_SET_SINGLE_BIT  , name.c_str(), bit);
	} else {
		ic.OpBit(INT_CLEAR_SINGLE_BIT, name.c_str(), bit);
	}

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemSetBit::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void LadderElemSetBit::setProperties(string newName, int newBit, bool newSet)
{
	name = newName;
	bit  = newBit;
	set  = newSet;
}

// Classe LadderElemCheckBit
LadderElemCheckBit::LadderElemCheckBit(void) : LadderElem(false, false, false, true, ELEM_CHECK_BIT)
{
	name = _("new");
	bit  = 0;
	set  = false;
}

pair<string, string> LadderElemCheckBit::DrawTXT(void)
{
	char str[100];

	sprintf(str, _("{CHECK %s:%d}"), set ? _("ON") : _("OFF"), bit);

	return pair<string, string>(name, str);
}

bool LadderElemCheckBit::GenerateIntCode(IntCode &ic)
{
	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	if(set) {
		ic.OpBit(INT_IF_BIT_CHECK_CLEAR, name.c_str(), bit);
	} else {
		ic.OpBit(INT_IF_BIT_CHECK_SET  , name.c_str(), bit);
	}

			ic.OpBit(INT_CLEAR_BIT, ic.getStateInOut(), set);
		ic.Op(INT_END_IF);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemCheckBit::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void LadderElemCheckBit::setProperties(string newName, int newBit, bool newSet)
{
	name = newName;
	bit  = newBit;
	set  = newSet;
}

// Classe LadderElemReadAdc
LadderElemReadAdc::LadderElemReadAdc(void) : LadderElem(true, false, false, true, ELEM_READ_ADC)
{
	name = _("new");
}

pair<string, string> LadderElemReadAdc::DrawTXT(void)
{
	char txt[50];

	sprintf(txt, _("{READ ADC %s }"), GetPinADC(name.c_str()));

	return pair<string, string>(name, txt);
}

bool LadderElemReadAdc::GenerateIntCode(IntCode &ic)
{
	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_READ_ADC, name.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemReadAdc::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemReadAdc::setProperties(string newName)
{
	name = newName;
}

// Classe LadderElemSetDa
LadderElemSetDa::LadderElemSetDa(void) : LadderElem(true, false, false, true, ELEM_SET_DA)
{
	name  = _("new");
	mode  = ELEM_SET_DA_MODE_RAW;
}

pair<string, string> LadderElemSetDa::DrawTXT(void)
{
	char cname[50];

	switch(mode) {
	case ELEM_SET_DA_MODE_RAW:
		strcpy(cname, name.c_str());
		break;

	case ELEM_SET_DA_MODE_MV:
		sprintf(cname, "%s mV", name.c_str());
		break;

	case ELEM_SET_DA_MODE_PCT:
		sprintf(cname, "%s %%", name.c_str());
		break;
	}

	return pair<string, string>(cname, _("{SET DA}"));
}

bool LadderElemSetDa::GenerateIntCode(IntCode &ic)
{
	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_SET_DA, name.c_str(), mode);
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemSetDa::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemSetDa::setProperties(string newName, int newMode)
{
	name  = newName;
	mode  = newMode;
}

// Classe LadderElemReadEnc
LadderElemReadEnc::LadderElemReadEnc(void) : LadderElem(true, false, false, true, ELEM_READ_ENC)
{
	name = _("new");
}

pair<string, string> LadderElemReadEnc::DrawTXT(void)
{
	return pair<string, string>(name, _("{READ ENC}"));
}

bool LadderElemReadEnc::GenerateIntCode(IntCode &ic)
{
	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_READ_ENC, name.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemReadEnc::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemReadEnc::setProperties(string newName)
{
	name = newName;
}

// Classe LadderElemResetEnc
LadderElemResetEnc::LadderElemResetEnc(void) : LadderElem(true, false, false, true, ELEM_RESET_ENC)
{
	name = _("new");
}

pair<string, string> LadderElemResetEnc::DrawTXT(void)
{
	return pair<string, string>(name, _("{WRITE ENC}"));
}

bool LadderElemResetEnc::GenerateIntCode(IntCode &ic)
{
	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_RESET_ENC, name.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemResetEnc::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemResetEnc::setProperties(string newName)
{
	name = newName;
}

// Classe LadderElemMultisetDA
LadderElemMultisetDA::LadderElemMultisetDA(void) : LadderElem(true, false, false, true, ELEM_MULTISET_DA)
{
    name                  = "600";
	name1                 = "2047";
	gaint                 = 10;
	gainr                 = 5;
	initval               = DA_RESOLUTION - 1;
	linear                = true; // true = linear, false = curva
	forward               = true; // true = avanço, false = recuo
	speedup               = false; // true = aceleração, false = desaceleração
	StartFromCurrentValue = false; // false = Iniciar ou ir para zero, conforme speedup. true = partir do valor atual até o valor configurado
}

pair<string, string> LadderElemMultisetDA::DrawTXT(void)
{
	return pair<string, string>(
		linear ? _("RAMPA LINEAR") : _("RAMPA CURVA"),
		StartFromCurrentValue ? "{COINCIDIR}": (speedup ? _("{ACELERACAO}") : _("{DESACELERACAO}"))
		);
}

bool LadderElemMultisetDA::GenerateIntCode(IntCode &ic)
{
	char cgaint[10];
	char cgainr[10];
	char ctype [10];
	char str_initval[MAX_NAME_LEN];
	string oneShot;
	oneShot = ic.GenSymOneShot();

	_itoa(gaint, cgaint, 10);
	_itoa(gainr, cgainr, 10);
	_itoa(type , ctype , 10);

	strcpy(str_initval, name1.c_str());
	if(IsNumber(str_initval)) {
		int initval = atoi(str_initval);

		if (type == 1)  // (mV)
			initval = static_cast<int>(DA_RESOLUTION * (initval / DA_VOLTAGE));
		else if (type == 2) // (%)
			initval = static_cast<int>(DA_RESOLUTION * (initval / 100.0f));

		_itoa(initval, str_initval, 10);
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), ic.getStateInOut());
			ic.Op(INT_MULTISET_DA, name.c_str(), str_initval, cgaint, cgainr, ctype, NULL, NULL, linear, StartFromCurrentValue ? 2 : speedup);
		ic.Op(INT_END_IF);
			ic.Op(INT_ELSE_IF); ic.Op(INT_IF_BIT_SET, oneShot.c_str());
		ic.Op(INT_MULTISET_DA, "", ramp_abort_mode == RAMP_ABORT_DEFAULT ? Prog.settings.ramp_abort_mode : ramp_abort_mode);
		ic.Op(INT_CLEAR_BIT, oneShot.c_str());
	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemMultisetDA::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemMultisetDA::setProperties(string newName, string newName1, int newGaint, int newGainr, int newInitval,
	bool newLinear, bool newForward, bool newSpeedup, bool newStartFromCurrentValue)
{
    name                  = newName;
	name1                 = newName1;
	gaint                 = newGaint;
	gainr                 = newGainr;
	initval               = newInitval;
	linear                = newLinear;
	forward               = newForward;
	speedup               = newSpeedup;
	StartFromCurrentValue = newStartFromCurrentValue;
}

// Classe LadderElemUSS
LadderElemUSS::LadderElemUSS(int which) : LadderElem(false, false, false, true, which)
{
	name          = _("new");
	id            = 0;
	parameter     = 0;
	parameter_set = 0;
	index         = 0;
}

pair<string, string> LadderElemUSS::DrawTXT(void)
{
	return pair<string, string>(name, (getWhich() == ELEM_READ_USS) ? _("{READ USS}") : _("{WRITE USS}"));
}

bool LadderElemUSS::GenerateIntCode(IntCode &ic)
{
	const char *stateInOut = ic.getStateInOut();

	char addr     [10];
	char param    [10];
	char param_set[10];
	char index    [10];

	sprintf(addr     , "%d", id);
	sprintf(param    , "%d", parameter);
	sprintf(param_set, "%d", parameter_set);
	sprintf(index    , "%d", index);

	// We want to respond to rising edges, so yes we need a one shot.
	string oneShot = ic.GenSymOneShot();
	string byPass  = ic.GenSymOneShot();

	// OneShot 
	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_IF_BIT_SET, "$SerialReady");
				ic.Op(getWhich() == ELEM_READ_USS ? INT_READ_USS : INT_WRITE_USS, name.c_str(), addr, param, param_set, (SWORD)atoi(index), 0);
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

void LadderElemUSS::setProperties(string newName, int newId, int newParameter, int newParameter_set, int newIndex)
{
	name          = newName;
	id            = newId;
	parameter     = newParameter;
	parameter_set = newParameter_set;
	index         = newIndex;
}

// Classe LadderElemModBUS
LadderElemModBUS::LadderElemModBUS(int which) : LadderElem(false, false, false, true, which)
{
    name         = _("new");
	retransmitir = true;
	elem         = MbNodeList_GetByIndex(0)->NodeID;

	MbNodeList_AddRef(elem);
}

pair<string, string> LadderElemModBUS::DrawTXT(void)
{
	char *bot;
	char buf[100];
	MbNodeList *l = MbNodeList_GetByNodeID(elem);

	sprintf(buf, "%s:%d", l->node.name, address);

	if(getWhich() == ELEM_READ_MODBUS) {
		bot = l->node.iface == MB_IFACE_RS485 ? _("{READ MB 485}")  : _("{READ MB ETH}");
	} else {
		bot = l->node.iface == MB_IFACE_RS485 ? _("{WRITE MB 485}") : _("{WRITE MB ETH}");
	}

	return pair<string, string>(buf, bot);
}

bool LadderElemModBUS::GenerateIntCode(IntCode &ic)
{
	char id[10];
	char addr[10];
	char *MbReady, *MbTimeout;
	const char *stateInOut = ic.getStateInOut();
	int intcode;
	MbNodeList *list;

	list    = MbNodeList_GetByNodeID(elem);
	intcode = getWhich() == ELEM_READ_MODBUS ? INT_READ_MODBUS : INT_WRITE_MODBUS;
	sprintf(id  , "%d", list->node.id);
	sprintf(addr, "%d", address);

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
				ic.Op(intcode, name.c_str(), id, addr, list->node.ip, (unsigned char)int32);
				ic.Op(INT_SET_BIT, MessageSent.c_str());
			ic.Op(INT_END_IF);
			ic.Op(INT_CLEAR_BIT, stateInOut);
			ic.Op(INT_CLEAR_BIT, ReplyReceived.c_str());
		ic.Op(INT_END_IF);
		ic.Op(INT_IF_BIT_CLEAR, ReplyReceived.c_str());
			ic.Op(INT_CLEAR_BIT, stateInOut);
			ic.Op(INT_IF_BIT_SET, MbReady);
				ic.Op(INT_SET_BIT, ReplyReceived.c_str());
		if(retransmitir) { // Retransmitir?
			ic.Op(INT_ELSE_IF); ic.Op(INT_IF_BIT_SET, MbTimeout);
				ic.Op(INT_CLEAR_BIT, MbTimeout);
				ic.Op(intcode, name.c_str(), id, addr, list->node.ip, (unsigned char)int32);
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

void LadderElemModBUS::setProperties()
{
}

// Classe LadderElemSetPWM
LadderElemSetPWM::LadderElemSetPWM(void) : LadderElem(true, false, false, true, ELEM_SET_PWM)
{
	name       = _("duty_cycle");
	targetFreq = 1000;
}

pair<string, string> LadderElemSetPWM::DrawTXT(void)
{
	char l[50];
	if(targetFreq >= 100000) {
		sprintf(l, _("{PWM %d kHz}"), (targetFreq+500)/1000);
	} else if(targetFreq >= 10000) {
		sprintf(l, _("{PWM %.1f kHz}"), targetFreq/1000.0);
	} else if(targetFreq >= 1000) {
		sprintf(l, _("{PWM %.2f kHz}"), targetFreq/1000.0);
	} else {
		sprintf(l, _("{PWM %d Hz}"), targetFreq);
	}

	return pair<string, string>(name, l);
}

bool LadderElemSetPWM::GenerateIntCode(IntCode &ic)
{
	char line[80];

	string oneShot = ic.GenSymOneShot();

	// ugh; need a >16 bit literal though, could be >64 kHz
	sprintf(line, "%d", targetFreq);

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());
		ic.Op(INT_SET_BIT, oneShot.c_str());
		ic.Op(INT_SET_PWM, name.c_str(), line);
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

void LadderElemSetPWM::setProperties()
{
}

// Classe LadderElemUART
LadderElemUART::LadderElemUART(int which) : LadderElem(false, false, false, true, which)
{
	name = _("char");
}

pair<string, string> LadderElemUART::DrawTXT(void)
{
	return pair<string, string>(name, (getWhich() == ELEM_UART_RECV) ? _("{UART RECV}") : _("{UART SEND}"));
}

bool LadderElemUART::GenerateIntCode(IntCode &ic)
{
	string oneShot = ic.GenSymOneShot();
	const char *stateInOut = ic.getStateInOut();

	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_COPY_BIT_TO_BIT, oneShot.c_str(), stateInOut);
			ic.Op(getWhich() == ELEM_UART_SEND ? INT_UART_SEND : INT_UART_RECV, name.c_str(), stateInOut);
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

void LadderElemUART::setProperties(string newName)
{
	name = newName;
}

// Classe LadderElemMasterRelay
LadderElemMasterRelay::LadderElemMasterRelay(void) : LadderElem(true, false, false, true, ELEM_MASTER_RELAY)
{
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

// Classe LadderElemShiftRegister
LadderElemShiftRegister::LadderElemShiftRegister(void) : LadderElem(true, false, false, false, ELEM_SHIFT_REGISTER)
{
	name   = _("reg");
	stages = 7;
}

pair<string, string> LadderElemShiftRegister::DrawTXT(void)
{
	char bot[MAX_NAME_LEN+20];
	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, "%s:%d", name.c_str(), stages-1);
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
			for(i = (stages-2); i >= 0; i--) {
				char dest[MAX_NAME_LEN+10], src[MAX_NAME_LEN+10];
				sprintf(src , "%s%d", name.c_str(), i);
				sprintf(dest, "%s%d", name.c_str(), i+1);
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

void LadderElemShiftRegister::setProperties(string newName, int newStages)
{
	name   = newName;
	stages = newStages;
}

// Classe LadderElemLUT
LadderElemLUT::LadderElemLUT(void) : LadderElem(true, false, false, false, ELEM_LOOK_UP_TABLE)
{
	dest         = _("dest");
	index        = _("index");
	count        = 0;
	editAsString = false;

	vals.fill(0);
}

pair<string, string> LadderElemLUT::DrawTXT(void)
{
	char top[MAX_NAME_LEN+20], bot[MAX_NAME_LEN+20];

	memset(top, ' ', sizeof(top));
	top[0] = '{';
	sprintf(top+2, "%s :=", dest.c_str());
	top[strlen(top)] = ' ';
	top[19] = '}';
	top[20] = '\0';

	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, " %s[%s]", "LUT", index.c_str());
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

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	for(i = 0; i < count; i++) {
		ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", i);
		ic.Op(INT_IF_VARIABLE_EQUALS_VARIABLE, index.c_str(), "$scratch");
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, dest.c_str(), vals[i]);
		ic.Op(INT_END_IF);
	}

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemLUT::CanInsert(LadderContext context)
{
	return context.canInsertEnd;
}

void LadderElemLUT::setProperties(string newDest, string newIndex, int newCount,
	bool newEditAsString, array<long, MAX_LOOK_UP_TABLE_LEN> newVals)
{
	dest         = newDest;
	index        = newIndex;
	count        = newCount;
	editAsString = newEditAsString;
	vals         = newVals;
}

// Classe LadderElemPiecewise
LadderElemPiecewise::LadderElemPiecewise(void) : LadderElem(false, false, false, false, ELEM_PADDING)
{
	dest  = _("yvar");
	index = _("xvar");
	count = 0;

	vals.fill(0);
}

pair<string, string> LadderElemPiecewise::DrawTXT(void)
{
	char top[MAX_NAME_LEN+20], bot[MAX_NAME_LEN+20];

	memset(top, ' ', sizeof(top));
	top[0] = '{';
	sprintf(top+2, "%s :=", dest.c_str());
	top[strlen(top)] = ' ';
	top[19] = '}';
	top[20] = '\0';

	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, " %s[%s]", "PWL", index.c_str());
	bot[strlen(bot)] = ' ';
	bot[19] = '}';
	bot[20] = '\0';

	return pair<string, string>(top, bot);
}

bool LadderElemPiecewise::GenerateIntCode(IntCode &ic)
{
	// This one is not so obvious; we have to decide how best to
	// perform the linear interpolation, using our 16-bit fixed
	// point math.
	if(count == 0) {
		Error(_("Piecewise linear lookup table with zero elements!"));
		CompileError();
	}

	int i;
	int xThis = vals[0];
	for(i = 1; i < count; i++) {
		if(vals[i*2] <= xThis) {
			Error(_("x values in piecewise linear table must be "
				"strictly increasing."));
			CompileError();
		}
		xThis = vals[i*2];
	}

	ic.Op(INT_IF_BIT_SET, ic.getStateInOut());

	for(i = count - 1; i >= 1; i--) {
		int thisDx = vals[i*2    ] - vals[(i-1)*2    ];
		int thisDy = vals[i*2 + 1] - vals[(i-1)*2 + 1];
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

		ic.Op(INT_IF_VARIABLE_LES_LITERAL, index.c_str(), vals[i*2]+1);

			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch"  , vals[(i-1)*2]);
			ic.Op(INT_SET_VARIABLE_SUBTRACT  , "$scratch"  , index.c_str(), "$scratch" , 0, 0);
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch2" , thisDx);
			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch3" , thisDy);
			ic.Op(INT_SET_VARIABLE_MULTIPLY  , dest.c_str(), "$scratch"   , "$scratch3", 0, 0);
			ic.Op(INT_SET_VARIABLE_DIVIDE    , dest.c_str(), dest.c_str() , "$scratch2", 0, 0);

			ic.Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch"  , vals[(i-1)*2 + 1]);
			ic.Op(INT_SET_VARIABLE_ADD       , dest.c_str(), dest.c_str() , "$scratch" , 0, 0);

		ic.Op(INT_END_IF);
	}

	ic.Op(INT_END_IF);

	return true;
}

bool LadderElemPiecewise::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

void LadderElemPiecewise::setProperties(string newDest, string newIndex, int newCount, array<long, MAX_LOOK_UP_TABLE_LEN> newVals)
{
	dest  = newDest;
	index = newIndex;
	count = newCount;
	vals  = newVals;
}

// Classe LadderElemFmtString
LadderElemFmtString::LadderElemFmtString(int which) : LadderElem(false, false, true, true, which)
{
	var = _("var");
	txt = _("value: %d\\r\\n");
}

pair<string, string> LadderElemFmtString::DrawTXT(void)
{
	char top[100], bot[100];

	sprintf(top, "%s: %s", getWhich() == ELEM_READ_FORMATTED_STRING ? _("READ") : _("WRITE"), var.c_str());
	sprintf(bot, "{\"%s\"}", txt.c_str());

	return pair<string, string>(top, bot);
}

bool LadderElemFmtString::GenerateIntCode(IntCode &ic)
{
	const char *stateInOut = ic.getStateInOut();
	// We want to respond to rising edges, so yes we need a one shot.
	string oneShot = ic.GenSymOneShot();
	string byPass  = ic.GenSymOneShot();

	// OneShot 
	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_IF_BIT_SET, "$SerialReady");
				if (getWhich() == ELEM_READ_FORMATTED_STRING)
					ic.Op(INT_READ_FORMATTED_STRING, var.c_str(), txt.c_str());
				else
					ic.Op(INT_WRITE_FORMATTED_STRING, var.c_str(), txt.c_str());
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

void LadderElemFmtString::setProperties(string newVar, string newTxt)
{
	var = newVar;
	txt = newTxt;
}

// Classe LadderElemYaskawa
LadderElemYaskawa::LadderElemYaskawa(int which) : LadderElem(false, false, true, true, which)
{
	id  = "0";
	var = "var";
	txt = _("0ZSET%d");
}

pair<string, string> LadderElemYaskawa::DrawTXT(void)
{
	char top[100], bot[100];

	sprintf(top, "%s: %s", getWhich() == ELEM_READ_SERVO_YASKAWA ? _("RX NS600") : _("TX NS600"), var.c_str());
	sprintf(bot, "{\"%s\"}", txt.c_str());

	return pair<string, string>(top, bot);
}

bool LadderElemYaskawa::GenerateIntCode(IntCode &ic)
{
	const char *stateInOut = ic.getStateInOut();

	// We want to respond to rising edges, so yes we need a one shot.
	string oneShot = ic.GenSymOneShot();
	string byPass  = ic.GenSymOneShot();

	// OneShot 
	ic.Op(INT_IF_BIT_SET, stateInOut);
		ic.Op(INT_IF_BIT_CLEAR, oneShot.c_str());
			ic.Op(INT_IF_BIT_SET, "$SerialReady");
				if (getWhich() == ELEM_READ_SERVO_YASKAWA)
					ic.Op(INT_READ_SERVO_YASKAWA , var.c_str(), txt.c_str(), id.c_str(), 0, 0);
				else
					ic.Op(INT_WRITE_SERVO_YASKAWA, var.c_str(), txt.c_str(), id.c_str(), 0, 0);
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

void LadderElemYaskawa::setProperties(string newId, string newVar, string newTxt)
{
	id  = newId;
	var = newVar;
	txt = newTxt;
}

// Classe LadderElemPersist
LadderElemPersist::LadderElemPersist(void) : LadderElem(true, false, false, true, ELEM_PERSIST)
{
	var = _("saved");
}

pair<string, string> LadderElemPersist::DrawTXT(void)
{
	return pair<string, string>(var, _("{PERSIST}"));
}

bool LadderElemPersist::GenerateIntCode(IntCode &ic)
{
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
			ic.Op(INT_EEPROM_READ, var.c_str(), isInit.c_str(), EepromAddrFree);
		ic.Op(INT_END_IF);
	ic.Op(INT_ELSE);
		// While running, continuously compare the EEPROM copy of
		// the variable against the RAM one; if they are different,
		// write the RAM one to EEPROM. 
		ic.Op(INT_CLEAR_BIT, "$scratch");
		ic.Op(INT_EEPROM_BUSY_CHECK, "$scratch");
		ic.Op(INT_IF_BIT_CLEAR, "$scratch");
			ic.Op(INT_EEPROM_READ, "$scratch2", "$scratch", EepromAddrFree);
			ic.Op(INT_IF_VARIABLE_EQUALS_VARIABLE, "$scratch2", var.c_str());
			ic.Op(INT_ELSE);
				ic.Op(INT_EEPROM_WRITE, var.c_str(), EepromAddrFree);
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

void LadderElemPersist::setProperties(string newVar)
{
	var = newVar;
}

// Classe LadderElemX
LadderElemX::LadderElemX(void) : LadderElem(false, false, false, true, ELEM_PADDING)
{
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

void LadderElemX::setProperties()
{
}

//Classe LadderCircuit
LadderCircuit::LadderCircuit(void)
{
	isSeries = true;
	vectorSubckt.clear();
}

LadderCircuit::LadderCircuit(bool newSeries)
{
	isSeries = newSeries;
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

		CenterWithSpaces    (*cx, *cy, (char *)txt.first .c_str(), poweredAfter , elem->IsName(), extra);
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

    /*NormText();

    if(elem == Selected && !InSimulationMode) {
        EmphText();
        ThisHighlighted = TRUE;
    } else {
        ThisHighlighted = FALSE;
    }*/

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
//				EmphText();
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

//    NormText();

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

void LadderCircuit::AddPlaceHolderIfNoEOL(void)
{
	vector<Subckt>::iterator it;

	if(!HasEOL()) {
		it = vectorSubckt.begin() + vectorSubckt.size();
		if(vectorSubckt.empty() || ((it-1)->elem == nullptr && !(it-1)->subckt->IsSeries())) {
			// Se o circuito estiver vazio ou se adicionamos um paralelo e for o ultimo elemento,
			// devemos adicionar o elemento de final de linha (PlaceHolder)
			Subckt s = { new LadderElemPlaceHolder, this };
			vectorSubckt.push_back(s);
		}
	}
}

bool LadderCircuit::AddElement(LadderElem *elem, LadderContext &context)
{
	int state;
	vector<Subckt>::iterator it;
	Subckt node = { elem, this };

	// Se elemento a ser adicionado for nulo, retorna
	if(elem == nullptr) return false;

	if(vectorSubckt.empty()) {
		vectorSubckt.push_back(node);

		return true;
	} else if(context.SelectedElem != nullptr) {
		for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
			if(context.SelectedElem == it->elem) {
				// Encontrada posicao de insercao
				if(it->elem->getWhich() == ELEM_PLACEHOLDER) {
					// Existe placeholder, removendo...
					delete it->elem;
					context.SelectedElem = nullptr;

					it->elem = elem;
				} else {
					// Agora criamos o subcircuito para insercao.
					state = context.SelectedElem->getSelectedState();
					if((isSeries && (state == SELECTED_LEFT || state == SELECTED_RIGHT)) ||
						(!isSeries && (state == SELECTED_ABOVE || state == SELECTED_BELOW))) {
							// Serie adicionando em serie ou Paralelo adicionando em paralelo.
							// Apenas adiciona um item a mais no vetor.

							// Se devemos inserir depois do elemento selecionado, avanca iterator.
							if(state == SELECTED_RIGHT || state == SELECTED_BELOW) it++;

							// Insere o elemento na posicao indicada
							vectorSubckt.insert(it, node);
					} else {
							// Inserindo paralelamente em um serie ou serie em um paralelo.
							// Devemos criar o serie / paralelo e entao incluir o novo elemento como um item
							// do circuito criado
							LadderCircuit *new_subckt = new LadderCircuit(!isSeries);

							// Aqui carregamos ponteiros para os objetos que serao inseridos.
							// Primeiro adicionamos o novo elemento e entao o subcircuito com os elementos existentes
							LadderElem *first = elem, *second = it->elem;
							if(state == SELECTED_RIGHT || state == SELECTED_BELOW) {
								// Se devemos inserir depois do elemento selecionado, inverte a ordem
								LadderElem *tmp = first;
								first  = second;
								second = tmp;
							}

							// Insere o primeiro elemento no subcircuito criado.
							new_subckt->AddElement(first , context);

							// Atualiza o contexto para que o segundo elemento seja adicionado corretamente
							context.SelectedElem = first;
							first->Select(isSeries ? SELECTED_BELOW : SELECTED_RIGHT);

							// Insere o segundo elemento no subcircuito criado.
							new_subckt->AddElement(second, context);

							// Restaura o contexto
							first->Select(SELECTED_NONE);
							context.SelectedElem = it->elem;

							// Atualiza o item atual para o subcircuito criado
							it->elem = nullptr;
							it->subckt = new_subckt;
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

	vector<Subckt>::iterator it;
	for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
		if(elem == it->elem) {
			// Encontrado elemento a ser removido
			delete it->elem;

			if(context.SelectedElem == elem) {
				context.SelectedElem = nullptr;
			}

			vectorSubckt.erase(it);
			AddPlaceHolderIfNoEOL();

			// Removido com sucesso, retorna true!
			return true;
		} else if(it->elem == nullptr && it->subckt->DelElement(elem, context)) { // Removido em subcircuito
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
		vectorSubckt.erase(it);
	}

	return ret;
}

void LadderCircuit::RemoveUnnecessarySubckts(void)
{
	vector<Subckt>::iterator it, itnext;

	for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
		if(it->elem == nullptr) {
			it->subckt->RemoveUnnecessarySubckts();
			if(it->subckt->getSize() == 1) {
				// Recupera o unico elemento do subcircuito
				Subckt s = it->subckt->getSubckt(0);

				// Desaloca o subcircuito
				delete it->subckt;

				// Adiciona o elemento ao circuito atual
				if(s.elem != nullptr) { // Se for um elemento
					it->elem   = s.elem;
					it->subckt = this; // Agora este elemento pertence ao circuito atual
				} else if(s.subckt->IsSeries() != isSeries) { // Se for um tipo diferente de circuito
					it->elem   = nullptr;
					it->subckt = s.subckt;
				} else { // Caso mais complicado: restou um circuito do mesmo tipo do nosso!
					Subckt ss;

					// Primeiro adiciona todos os itens do subcircuito na posicao atual deste circuito
					itnext = it;
					while(s.subckt->getSize() > 0) {
						ss = s.subckt->getSubckt(0);
						if(ss.elem != nullptr) {
							ss.subckt = this; // Este elemento passa a fazer pate do circuito atual
						}
						it = vectorSubckt.insert(itnext, ss);
						itnext = it + 1;
					}

					// Exclui o circuito vazio
					vectorSubckt.erase(itnext);
				}
			}
		}
	}
}

// Classe LadderDiagram
void LadderDiagram::Init(void)
{
	context.SelectedElem    = nullptr;
	context.SelectedCircuit = nullptr;

	NeedScrollSelectedIntoView = false;

	NewRung(false);
}

LadderDiagram::LadderDiagram(void)
{
	Init();
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
		if(context.SelectedElem != nullptr) {
			context.SelectedElem->Select(SELECTED_NONE);
		}

		context.SelectedElem    = elem;
		context.SelectedCircuit = getSubcktForElement(elem);
	}

	elem->Select(state);

	if(!IsSelectedVisible()) {
		NeedScrollSelectedIntoView = true;
	}

	updateContext();
}

bool LadderDiagram::EditSelectedElement(void)
{
	if(context.SelectedElem != nullptr) {
		return context.SelectedElem->ShowDialog();
	}

	return false;
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

void LadderDiagram::updateContext(void)
{
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

			if(context.SelectedElem->getSelectedState() == SELECTED_ABOVE ||
			   context.SelectedElem->getSelectedState() == SELECTED_BELOW)
			{
				context.canInsertEnd   = true;
				context.canInsertOther = false;
			} else if(context.SelectedElem->getSelectedState() == SELECTED_RIGHT) {
				context.canInsertEnd   = false;
				context.canInsertOther = false;
			}
		} else {
			if(context.SelectedElem->getWhich() == ELEM_PLACEHOLDER) {
				context.canDelete = false;
			}

			if(context.SelectedElem->getSelectedState() == SELECTED_RIGHT || 
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

//	SetMenusEnabled(context.canNegate, context.canNormal, context.canResetOnly, context.canSetOnly, context.canDelete,
//        CanInsertEnd, CanInsertOther, context.canPushDown, context.canPushUp, CanInsertComment);
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

	if(position < 0) { // Se posicao menor que zero, insere no final
		it = rungs.end();
		position = rungs.size();
	} else {
		it = rungs.begin() + position;
		if(it != rungs.end() && isAfter) it++;
	}

	// Adiciona a linha na posicao calculada
	it = rungs.insert(it, new LadderCircuit(true));

	// Adiciona o padding
	LadderElem *elem = new LadderElemPlaceHolder;
	(*it)->AddElement(elem, context);

	SelectElement(elem, SELECTED_RIGHT);
}

bool LadderDiagram::PushRung(int rung, bool up)
{
	int QtdRungs = rungs.size();

	if(rung < 0) {
		rung = RungContainingSelected();
	}

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

	updateContext();

	return true;
}

bool LadderDiagram::AddElement(LadderElem *elem)
{
	// Se elemento a ser adicionado for nulo ou nao existe um elemento selecionado, retorna
	if(elem == nullptr || context.SelectedElem == nullptr || context.SelectedCircuit == nullptr) return false;

	bool ret = elem->CanInsert(context) && context.SelectedCircuit->AddElement(elem, context);

	// Se adicionado com sucesso, atualiza o contexto
	if(ret == true) {
		int position = RungContainingSelected();
		if(position >= 0) { // Se posicao menor que zero, insere no final
			vector<LadderCircuit *>::iterator it = rungs.begin() + position;
			(*it)->AddPlaceHolderIfNoEOL();
		}

		SelectElement(elem, elem->IsEOL() ? SELECTED_LEFT : SELECTED_RIGHT);
		updateContext();
	} else {
		// Apos o recebimento de elem, a classe LadderDiagram passa a ser responsavel por ele.
		// Se por qualquer motivo houver uma falha ao inserir, a classe tem a responsabilidade
		// de desalocar este elemento.
		delete elem;
	}

	return ret;
}

bool LadderDiagram::DelElement(LadderElem *elem)
{
	if(elem == nullptr) {
		if(context.SelectedElem == nullptr) return false; // Sem elemento para remover
		elem = context.SelectedElem;
	}

	int rung = RungContainingElement(elem);

	if(rungs[rung]->DelElement(elem, context)) {
		rungs[rung]->RemoveUnnecessarySubckts();
		updateContext();
		return true;
	}

	return false;
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
