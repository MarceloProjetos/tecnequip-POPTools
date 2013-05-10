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

LadderElem::LadderElem(bool EOL, bool Comment, int elemWhich)
{
	Init();

	which       = elemWhich;
	isEndOfLine = EOL;
	isComment   = Comment;
}

// Classe LadderElemPlaceHolder
LadderElemPlaceHolder::LadderElemPlaceHolder(void) : LadderElem(false, false, ELEM_PLACEHOLDER)
{
}

pair<string, string> LadderElemPlaceHolder::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
    CenterWithWiresWidth(*cx, *cy, "--", FALSE, FALSE, 2);
    *cx += POS_WIDTH;

	return pair<string, string>("", "--");
}

void LadderElemPlaceHolder::DrawGUI(void)
{
}

bool LadderElemPlaceHolder::GenerateIntCode(IntCode &ic)
{
	return true;
}

bool LadderElemPlaceHolder::CanInsert(LadderContext context)
{
	return context.canInsertOther;
}

// Classe LadderElemComment
LadderElemComment::LadderElemComment(void) : LadderElem(false, true, ELEM_COMMENT)
{
	str = _("--add comment here--");
}

pair<string, string> LadderElemComment::DrawTXT(int *cx, int *cy, bool poweredBefore)
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
		DrawChars(*cx, *cy + (POS_HEIGHT/2), tlbuf);
		second = tlbuf;
	}

	sprintf(tlbuf, "\x03 ; %s\x02", tbuf);
	DrawChars(*cx, *cy + (POS_HEIGHT/2) - 1, tlbuf);
	first = tlbuf;

	*cx += ColsAvailable*POS_WIDTH;

	return pair<string, string>(first, second);
}

void LadderElemComment::DrawGUI(void)
{
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

// Classe LadderElemContact
LadderElemContact::LadderElemContact(void) : LadderElem(false, false, ELEM_CONTACTS)
{
	name    = _("in");
	negated = false;
	type    = IO_TYPE_DIG_INPUT;
	bit     = 0;
}

pair<string, string> LadderElemContact::DrawTXT(int *cx, int *cy, bool poweredBefore)
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
	sprintf(name_with_type, "%s (%c)", name, ch);

	CenterWithSpaces(*cx, *cy, name_with_type, poweredAfter, TRUE);
	CenterWithWires (*cx, *cy, buf, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(buf, name_with_type);
}

void LadderElemContact::DrawGUI(void)
{
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

void LadderElemContact::setProperties(string newName, bool newNegated, unsigned int newType, unsigned char newBit)
{
	name    = newName;
	negated = newNegated;
	type    = newType;
	bit     = newBit;
}

// Classe LadderElemCoil
LadderElemCoil::LadderElemCoil(void) : LadderElem(true, false, ELEM_COIL)
{
	name      = _("out");
	negated   = false;
	setOnly   = false;
	resetOnly = false;
	type      = IO_TYPE_DIG_OUTPUT;
	bit       = 0;
}

pair<string, string> LadderElemCoil::DrawTXT(int *cx, int *cy, bool poweredBefore)
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
	sprintf(name_with_type, "%s (%c)", name, type == IO_TYPE_DIG_OUTPUT ? 'S' : 'R');

	CenterWithSpaces(*cx, *cy, name_with_type, poweredAfter, TRUE);
    CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);

	return pair<string, string>(buf, name_with_type);
}

void LadderElemCoil::DrawGUI(void)
{
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
LadderElemTimer::LadderElemTimer(int which) : LadderElem(which == ELEM_RTO ? true : false, false, which)
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

pair<string, string> LadderElemTimer::DrawTXT(int *cx, int *cy, bool poweredBefore)
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

	char buf[256], buf_name[256];

	strcpy(buf_name, name.c_str());

	if(delay >= 1000*1000) {
		sprintf(buf, "[\x01%s\x02 %.3f s]", s, delay/1000000.0);
	} else if(delay >= 100*1000) {
		sprintf(buf, "[\x01%s\x02 %.1f ms]", s, delay/1000.0);
	} else {
		sprintf(buf, "[\x01%s\x02 %.2f ms]", s, delay/1000.0);
	}

	CenterWithSpaces(*cx, *cy, buf_name, poweredAfter, TRUE);
	CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(name, buf);
}

void LadderElemTimer::DrawGUI(void)
{
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
LadderElemRTC::LadderElemRTC(void) : LadderElem(false, false, ELEM_RTC)
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

pair<string, string> LadderElemRTC::DrawTXT(int *cx, int *cy, bool poweredBefore)
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

	CenterWithSpaces(*cx, *cy, bufs, poweredAfter , TRUE);
	CenterWithWires (*cx, *cy, bufe, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(bufs, bufe);
}

void LadderElemRTC::DrawGUI(void)
{
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
LadderElemCounter::LadderElemCounter(int which) : LadderElem(which == ELEM_CTC ? true : false, false, which)
{
	name = _("new");
	max  = 0;
}

pair<string, string> LadderElemCounter::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	int which = getWhich();
	char *s, op, buf[256];

	strcpy(buf, name.c_str());
	CenterWithSpaces(*cx, *cy, buf, poweredAfter, TRUE);

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

	CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(name, buf);
}

void LadderElemCounter::DrawGUI(void)
{
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
LadderElemReset::LadderElemReset(void) : LadderElem(true, false, ELEM_RES)
{
	name = _("new");
}

pair<string, string> LadderElemReset::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char buf[256];

	strcpy(buf, name.c_str());
	CenterWithSpaces(*cx, *cy, buf, poweredAfter, TRUE);

	strcpy(buf, name.c_str());
	CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);

	return pair<string, string>(name, buf);
}

void LadderElemReset::DrawGUI(void)
{
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
LadderElemOneShot::LadderElemOneShot(int which) : LadderElem(false, false, which)
{
}

pair<string, string> LadderElemOneShot::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char *s1, *s2;
	int which = getWhich();

	if(which == ELEM_ONE_SHOT_RISING) {
		s1 = "      _ ";
		s2 = _("[\x01OSR\x02_/ ]");
	} else if(which == ELEM_ONE_SHOT_FALLING) {
		s1 = "    _   ";
		s2 = _("[\x01OSF\x02 \\_]");
	} else oops();

	CenterWithSpaces(*cx, *cy, s1, poweredAfter , FALSE);
	CenterWithWires (*cx, *cy, s2, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(s1, s2);
}

void LadderElemOneShot::DrawGUI(void)
{
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
LadderElemCmp::LadderElemCmp(int which) : LadderElem(false, false, which)
{
	op1 = _("var");
	op2 = "1";
}

pair<string, string> LadderElemCmp::DrawTXT(int *cx, int *cy, bool poweredBefore)
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

	CenterWithSpaces(*cx, *cy, s1, poweredAfter , FALSE);
	CenterWithWires (*cx, *cy, s2, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(s1, s2);
}

void LadderElemCmp::DrawGUI(void)
{
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
LadderElemMath::LadderElemMath(int which) : LadderElem(true, false, which)
{
	dest = _("dest");
	op1  = _("src");
	op2 = "1";
}

pair<string, string> LadderElemMath::DrawTXT(int *cx, int *cy, bool poweredBefore)
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

	int extra = 2*POS_WIDTH - FormattedStrlen(top);
	PoweredText(poweredAfter);
	DrawChars(*cx + (extra/2), *cy + (POS_HEIGHT/2) - 1, top);
	CenterWithWiresWidth(*cx, *cy, bot, poweredBefore, poweredAfter,
	2*POS_WIDTH);

	*cx += POS_WIDTH;

	return pair<string, string>(top, bot);
}

void LadderElemMath::DrawGUI(void)
{
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
LadderElemSqrt::LadderElemSqrt(void) : LadderElem(true, false, ELEM_SQRT)
{
	dest = _("dest");
	src  = _("src");
}

pair<string, string> LadderElemSqrt::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[256];
	char bot[256];

	sprintf(top, "{%s :=}", dest);
	sprintf(bot, _("{%s SQRT}"), src);

	CenterWithSpaces(*cx, *cy, top, poweredAfter , FALSE);
	CenterWithWires (*cx, *cy, bot, poweredBefore, poweredAfter);

	return pair<string, string>(top, bot);
}

void LadderElemSqrt::DrawGUI(void)
{
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
LadderElemRand::LadderElemRand(void) : LadderElem(true, false, ELEM_RAND)
{
	var = _("var");
	min = "0";
	max = "100";
}

pair<string, string> LadderElemRand::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[256];
	char bot[256];

	sprintf(top, "{ %s := }", var);
	sprintf(bot, "{%s <= ? <= %s}", min, max);

	int extra = 2*POS_WIDTH - FormattedStrlen(top);
	DrawChars(*cx + (extra/2), *cy + (POS_HEIGHT/2) - 1, top);
	CenterWithWiresWidth(*cx, *cy, bot, poweredBefore, poweredAfter, 2*POS_WIDTH);
	*cx += POS_WIDTH;

	return pair<string, string>(top, bot);
}

void LadderElemRand::DrawGUI(void)
{
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
LadderElemAbs::LadderElemAbs(void) : LadderElem(true, false, ELEM_ABS)
{
	src  = _("src");
	dest = _("dest");
}

pair<string, string> LadderElemAbs::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
    char top[256];
    char bot[256];

	sprintf(top, "{ %s :=}", dest.c_str());
	sprintf(bot, "{ |%s| }", src.c_str());

    CenterWithSpaces(*cx, *cy, top, poweredAfter , FALSE);
    CenterWithWires (*cx, *cy, bot, poweredBefore, poweredAfter);

	return pair<string, string>(top, bot);
}

void LadderElemAbs::DrawGUI(void)
{
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
LadderElemMove::LadderElemMove(void) : LadderElem(true, false, ELEM_MOVE)
{
	src  = _("src");
	dest = _("dest");
}

pair<string, string> LadderElemMove::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[256];
	char bot[256];

	sprintf(top, "{%s :=}", dest.c_str());
	sprintf(bot, _("{%s MOV}"), src.c_str());

	CenterWithSpaces(*cx, *cy, top, poweredAfter , FALSE);
	CenterWithWires (*cx, *cy, bot, poweredBefore, poweredAfter);

	return pair<string, string>(top, bot);
}

void LadderElemMove::DrawGUI(void)
{
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
LadderElemOpenShort::LadderElemOpenShort(int which) : LadderElem(false, false, which)
{
}

pair<string, string> LadderElemOpenShort::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char *s;

	switch(getWhich()) {
		case ELEM_OPEN : s = "+      +"; break;
		case ELEM_SHORT: s = "+------+"; break;
		default: oops();
	}

	CenterWithWires(*cx, *cy, s, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>("", s);
}

void LadderElemOpenShort::DrawGUI(void)
{
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
LadderElemSetBit::LadderElemSetBit(void) : LadderElem(false, false, ELEM_PADDING)
{
	name = _("new");
	bit  = 0;
	set  = false;
}

pair<string, string> LadderElemSetBit::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char str[100];

	strcpy(str, name.c_str());
	CenterWithSpaces(*cx, *cy, str, poweredAfter, TRUE);

	sprintf(str, _("{%s BIT:%d}"), set ? _("SET") : _("RST"), bit);
	CenterWithWires(*cx, *cy, str, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(name, str);
}

void LadderElemSetBit::DrawGUI(void)
{
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
LadderElemCheckBit::LadderElemCheckBit(void) : LadderElem(false, false, ELEM_CHECK_BIT)
{
	name = _("new");
	bit  = 0;
	set  = false;
}

pair<string, string> LadderElemCheckBit::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char str[100];

	strcpy(str, name.c_str());
	CenterWithSpaces(*cx, *cy, str, poweredAfter, TRUE);

	sprintf(str, _("{CHECK %s:%d}"), set ? _("ON") : _("OFF"), bit);
	CenterWithWires(*cx, *cy, str, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(name, str);
}

void LadderElemCheckBit::DrawGUI(void)
{
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
LadderElemReadAdc::LadderElemReadAdc(void) : LadderElem(true, false, ELEM_READ_ADC)
{
	name = _("new");
}

pair<string, string> LadderElemReadAdc::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char txt[50];

	strcpy(txt, name.c_str());
	CenterWithSpaces(*cx, *cy, txt, poweredAfter, TRUE);

	sprintf(txt, _("{READ ADC %s }"), GetPinADC(name.c_str()));
	CenterWithWires(*cx, *cy, txt, poweredBefore, poweredAfter);

	return pair<string, string>(name, txt);
}

void LadderElemReadAdc::DrawGUI(void)
{
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
LadderElemSetDa::LadderElemSetDa(void) : LadderElem(true, false, ELEM_SET_DA)
{
	name  = _("new");
	mode  = ELEM_SET_DA_MODE_RAW;
}

pair<string, string> LadderElemSetDa::DrawTXT(int *cx, int *cy, bool poweredBefore)
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

	CenterWithSpaces(*cx, *cy, cname        , poweredAfter , TRUE);
	CenterWithWires (*cx, *cy, _("{SET DA}"), poweredBefore, poweredAfter);

	return pair<string, string>(cname, _("{SET DA}"));
}

void LadderElemSetDa::DrawGUI(void)
{
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
LadderElemReadEnc::LadderElemReadEnc(void) : LadderElem(true, false, ELEM_READ_ENC)
{
	name = _("new");
}

pair<string, string> LadderElemReadEnc::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char cname[MAX_NAME_LEN];

	strcpy(cname, name.c_str());

	CenterWithSpaces(*cx, *cy, cname          , poweredAfter , TRUE);
	CenterWithWires (*cx, *cy, _("{READ ENC}"), poweredBefore, poweredAfter);

	return pair<string, string>(name, _("{READ ENC}"));
}

void LadderElemReadEnc::DrawGUI(void)
{
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
LadderElemResetEnc::LadderElemResetEnc(void) : LadderElem(true, false, ELEM_RESET_ENC)
{
	name = _("new");
}

pair<string, string> LadderElemResetEnc::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char cname[MAX_NAME_LEN];

	strcpy(cname, name.c_str());

	CenterWithSpaces(*cx, *cy, cname           , poweredAfter , TRUE);
	CenterWithWires (*cx, *cy, _("{WRITE ENC}"), poweredBefore, poweredAfter);

	return pair<string, string>(name, _("{WRITE ENC}"));
}

void LadderElemResetEnc::DrawGUI(void)
{
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
LadderElemMultisetDA::LadderElemMultisetDA(void) : LadderElem(true, false, ELEM_MULTISET_DA)
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

pair<string, string> LadderElemMultisetDA::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char *top, *bot;

	top = linear ? _("RAMPA LINEAR") : _("RAMPA CURVA");
	bot = StartFromCurrentValue ? "{COINCIDIR}": (speedup ? _("{ACELERACAO}") : _("{DESACELERACAO}"));

	CenterWithSpaces(*cx, *cy, top, poweredAfter , TRUE);
	CenterWithWires (*cx, *cy, bot, poweredBefore, poweredAfter);

	return pair<string, string>(top, bot);
}

void LadderElemMultisetDA::DrawGUI(void)
{
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
LadderElemUSS::LadderElemUSS(int which) : LadderElem(false, false, which)
{
	name          = _("new");
	id            = 0;
	parameter     = 0;
	parameter_set = 0;
	index         = 0;
}

pair<string, string> LadderElemUSS::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[MAX_NAME_LEN];

	strcpy(top, name.c_str());
	CenterWithSpaces(*cx, *cy, top, poweredAfter, TRUE);

	char *bot = (getWhich() == ELEM_READ_USS) ? _("{READ USS}") : _("{WRITE USS}");
	CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(name, bot);
}

void LadderElemUSS::DrawGUI(void)
{
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
LadderElemModBUS::LadderElemModBUS(int which) : LadderElem(false, false, which)
{
    name         = _("new");
	retransmitir = true;
	elem         = MbNodeList_GetByIndex(0)->NodeID;

	MbNodeList_AddRef(elem);
}

pair<string, string> LadderElemModBUS::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char *bot;
	char buf[100];
	MbNodeList *l = MbNodeList_GetByNodeID(elem);

	sprintf(buf, "%s:%d", l->node.name, address);
	CenterWithSpaces(*cx, *cy, buf, poweredAfter, TRUE);

	if(getWhich() == ELEM_READ_MODBUS) {
		bot = l->node.iface == MB_IFACE_RS485 ? _("{READ MB 485}")  : _("{READ MB ETH}");
	} else {
		bot = l->node.iface == MB_IFACE_RS485 ? _("{WRITE MB 485}") : _("{WRITE MB ETH}");
	}

	CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(buf, bot);
}

void LadderElemModBUS::DrawGUI(void)
{
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
	return context.canInsertOther;
}

void LadderElemModBUS::setProperties()
{
}

// Classe LadderElemSetPWM
LadderElemSetPWM::LadderElemSetPWM(void) : LadderElem(true, false, ELEM_SET_PWM)
{
	name       = _("duty_cycle");
	targetFreq = 1000;
}

pair<string, string> LadderElemSetPWM::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[MAX_NAME_LEN];
	strcpy(top, name.c_str());
	CenterWithSpaces(*cx, *cy, top, poweredAfter, TRUE);

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
	CenterWithWires(*cx, *cy, l, poweredBefore, poweredAfter);

	return pair<string, string>(name, l);
}

void LadderElemSetPWM::DrawGUI(void)
{
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
LadderElemUART::LadderElemUART(int which) : LadderElem(false, false, which)
{
	name = _("char");
}

pair<string, string> LadderElemUART::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char buf[MAX_NAME_LEN];

	strcpy(buf, name.c_str());
	CenterWithSpaces(*cx, *cy, buf, poweredAfter, TRUE);

	strcpy(buf, (getWhich() == ELEM_UART_RECV) ? _("{UART RECV}") : _("{UART SEND}"));
	CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);

	*cx += POS_WIDTH;

	return pair<string, string>(name, buf);
}

void LadderElemUART::DrawGUI(void)
{
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
LadderElemMasterRelay::LadderElemMasterRelay(void) : LadderElem(true, false, ELEM_MASTER_RELAY)
{
}

pair<string, string> LadderElemMasterRelay::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	CenterWithWires(*cx, *cy, _("{MASTER RLY}"), poweredBefore, poweredAfter);

	return pair<string, string>("", _("{MASTER RLY}"));
}

void LadderElemMasterRelay::DrawGUI(void)
{
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
LadderElemShiftRegister::LadderElemShiftRegister(void) : LadderElem(true, false, ELEM_SHIFT_REGISTER)
{
	name   = _("reg");
	stages = 7;
}

pair<string, string> LadderElemShiftRegister::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char bot[MAX_NAME_LEN+20];
	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, "%s:%d", name.c_str(), stages-1);
	bot[strlen(bot)] = ' ';
	bot[19] = '}';
	bot[20] = '\0';

	CenterWithSpaces(*cx, *cy, _("{\x01SHIFT REG\x02   }"), poweredAfter, FALSE);
	CenterWithWires (*cx, *cy, bot                        , poweredBefore, poweredAfter);

	return pair<string, string>(_("{\x01SHIFT REG\x02   }"), bot);
}

void LadderElemShiftRegister::DrawGUI(void)
{
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
LadderElemLUT::LadderElemLUT(void) : LadderElem(true, false, ELEM_LOOK_UP_TABLE)
{
	dest         = _("dest");
	index        = _("index");
	count        = 0;
	editAsString = false;

	vals.fill(0);
}

pair<string, string> LadderElemLUT::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[MAX_NAME_LEN+20], bot[MAX_NAME_LEN+20];

	memset(top, ' ', sizeof(top));
	top[0] = '{';
	sprintf(top+2, "%s :=", dest.c_str());
	top[strlen(top)] = ' ';
	top[19] = '}';
	top[20] = '\0';
	CenterWithSpaces(*cx, *cy, top, poweredAfter, FALSE);

	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, " %s[%s]", "LUT", index.c_str());
	bot[strlen(bot)] = ' ';
	bot[19] = '}';
	bot[20] = '\0';
	CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);

	return pair<string, string>(top, bot);
}

void LadderElemLUT::DrawGUI(void)
{
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
LadderElemPiecewise::LadderElemPiecewise(void) : LadderElem(false, false, ELEM_PADDING)
{
	dest  = _("yvar");
	index = _("xvar");
	count = 0;

	vals.fill(0);
}

pair<string, string> LadderElemPiecewise::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[MAX_NAME_LEN+20], bot[MAX_NAME_LEN+20];

	memset(top, ' ', sizeof(top));
	top[0] = '{';
	sprintf(top+2, "%s :=", dest.c_str());
	top[strlen(top)] = ' ';
	top[19] = '}';
	top[20] = '\0';
	CenterWithSpaces(*cx, *cy, top, poweredAfter, FALSE);

	memset(bot, ' ', sizeof(bot));
	bot[0] = '{';
	sprintf(bot+2, " %s[%s]", "PWL", index.c_str());
	bot[strlen(bot)] = ' ';
	bot[19] = '}';
	bot[20] = '\0';
	CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);

	return pair<string, string>(top, bot);
}

void LadderElemPiecewise::DrawGUI(void)
{
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
LadderElemFmtString::LadderElemFmtString(int which) : LadderElem(false, false, which)
{
	var = _("var");
	txt = _("value: %d\\r\\n");
}

pair<string, string> LadderElemFmtString::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[100], bot[100];
	sprintf(top, "%s: %s", getWhich() == ELEM_READ_FORMATTED_STRING ? _("READ") : _("WRITE"), var.c_str());

	int extra = 2*POS_WIDTH - strlen(bot);
	PoweredText(poweredAfter);
//	NameText();
	DrawChars(*cx + (extra/2), *cy + (POS_HEIGHT/2) - 1, top);
//	BodyText();

	sprintf(bot, "{\"%s\"}", txt.c_str());
	CenterWithWiresWidth(*cx, *cy, bot, poweredBefore, poweredAfter, 2*POS_WIDTH);

	*cx += 2*POS_WIDTH;

	return pair<string, string>(top, bot);
}

void LadderElemFmtString::DrawGUI(void)
{
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
LadderElemYaskawa::LadderElemYaskawa(int which) : LadderElem(false, false, which)
{
	id  = "0";
	var = "var";
	txt = _("0ZSET%d");
}

pair<string, string> LadderElemYaskawa::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[100], bot[100];
	sprintf(top, "%s: %s", getWhich() == ELEM_READ_SERVO_YASKAWA ? _("RX NS600") : _("TX NS600"), var.c_str());

	int extra = 2*POS_WIDTH - strlen(top);
	PoweredText(poweredAfter);
//	NameText();
	DrawChars(*cx + (extra/2), *cy + (POS_HEIGHT/2) - 1, top);
//	BodyText();

	sprintf(bot, "{\"%s\"}", txt.c_str());
	CenterWithWiresWidth(*cx, *cy, bot, poweredBefore, poweredAfter, 2*POS_WIDTH);

	*cx += 2*POS_WIDTH;

	return pair<string, string>(top, bot);
}

void LadderElemYaskawa::DrawGUI(void)
{
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
LadderElemPersist::LadderElemPersist(void) : LadderElem(true, false, ELEM_PERSIST)
{
	var = _("saved");
}

pair<string, string> LadderElemPersist::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	char top[MAX_NAME_LEN];

	strcpy(top, var.c_str());

	CenterWithSpaces(*cx, *cy, top           , poweredAfter , TRUE);
	CenterWithWires (*cx, *cy, _("{PERSIST}"), poweredBefore, poweredAfter);

	return pair<string, string>(top, _("{PERSIST}"));
}

void LadderElemPersist::DrawGUI(void)
{
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
LadderElemX::LadderElemX(void) : LadderElem(false, false, ELEM_PADDING)
{
}

pair<string, string> LadderElemX::DrawTXT(int *cx, int *cy, bool poweredBefore)
{
	return pair<string, string>("", "");
}

void LadderElemX::DrawGUI(void)
{
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

bool LadderCircuit::DrawTXT(int *cx, int *cy, bool poweredBefore)
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
				it->elem->DrawTXT(cx, cy, poweredAfter);
				poweredAfter = it->elem->IsPoweredAfter();
			} else {
				poweredAfter = it->subckt->DrawTXT(cx, cy, poweredAfter);
			}
		}
	} else {
		int widthMax = 1;//CountWidthOfElement(which, elem, (*cx)/POS_WIDTH);
		int heightMax = 1;//CountHeightOfElement(which, elem);
		int lowestPowered = -1;
		int downBy = 0;

		poweredAfter = false;

		for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
			bool poweredThis;

			if(it->elem != nullptr) {
				it->elem->DrawTXT(cx, cy, poweredBefore);
				poweredThis = it->elem->IsPoweredAfter();
			} else {
				poweredThis = it->subckt->DrawTXT(cx, cy, poweredBefore);
			}

			if(InSimulationMode) {
				if(poweredThis) poweredAfter = true;
				PoweredText(poweredThis);
			}

			while((*cx - cx0) < widthMax*POS_WIDTH) {
				int gx = *cx/POS_WIDTH;
				int gy = *cy/POS_HEIGHT;

//				if(CheckBoundsUndoIfFails(gx, gy)) return FALSE;

//				DM_BOUNDS(gx, gy);
				if(gx>0 && DisplayMatrixWhich[gx-1][gy] == ELEM_PLACEHOLDER)
					break;
				DisplayMatrix     [gx][gy] = PADDING_IN_DISPLAY_MATRIX;
				DisplayMatrixWhich[gx][gy] = ELEM_PADDING;

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
			int justDrewHeight = 1;/*CountHeightOfElement(it->elem->getWhich(),
				p->contents[i].d.any);*/
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
				if(j <= lowestPowered) PoweredText(poweredAfter);
				if(DisplayMatrix[gx - 1][gy + j] && DisplayMatrixWhich[gx - 1][gy + j] != ELEM_PLACEHOLDER) {
					needWire = true;
				}
//				if(needWire) VerticalWire(*cx - 1, *cy + j*POS_HEIGHT);
			}
			// stupid special case
			if(lowestPowered == 0 && InSimulationMode) {
//				EmphText();
				DrawChars(*cx - 1, *cy + (POS_HEIGHT/2), "+");
			}
		}

		PoweredText(poweredBefore);
		needWire = false;
		for(j = heightMax - 1; j >= 1; j--) {
			if(DisplayMatrix[cx0/POS_WIDTH][*cy/POS_HEIGHT + j]) {
				needWire = true;
			}
//			if(needWire) VerticalWire(cx0 - 1, *cy + j*POS_HEIGHT);
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
		if((vectorSubckt.size() == 1 && vectorSubckt[0].elem->getWhich() == ELEM_PLACEHOLDER)) {
			// Existe placeholder, removendo...
			delete vectorSubckt[0].elem;
			context.SelectedElem = nullptr;

			vectorSubckt[0].elem = elem;

			return true;
		} else {
			for(it = vectorSubckt.begin(); it != vectorSubckt.end(); it++) {
				if(context.SelectedElem == it->elem) {
					// Encontrada posicao de insercao, cria o subcircuito para insercao.
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

					// Inserido com sucesso, retorna true!
					return true;
				}
			}
		}
	}

	return false;
}

// Classe LadderDiagram
void LadderDiagram::Init(void)
{
	context.SelectedElem    = nullptr;
	context.SelectedCircuit = nullptr;

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
		if(rungs[i].IsComment()) continue;

		ic.Comment("");
        ic.Comment(_("start rung %d"), i+1);
		ic.ClearParallelCount();
        ic.Op(INT_COPY_BIT_TO_BIT, "$rung_top", "$mcr");
        ic.SimState(&(Prog.rungPowered[i]), "$rung_top");

		ret = rungs[i].GenerateIntCode(ic);
	}

	return ret;
}

void LadderDiagram::SelectElement(LadderElem *elem, int state)
{
	if(elem) {
		if(context.SelectedElem != nullptr) {
			context.SelectedElem->Select(SELECTED_NONE);
		}

		context.SelectedElem    = elem;
		context.SelectedCircuit = getSubcktForElement(elem);

		elem->Select(state);

		updateContext();
	}
}

LadderCircuit *LadderDiagram::getSubcktForElement(LadderElem *elem)
{
	LadderCircuit *subckt = nullptr;
	vector<LadderCircuit>::size_type i;

	for(i = 0; i < rungs.size() && subckt == nullptr; i++) {
		subckt = rungs[i].getSubcktForElement(elem);
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
			if(context.SelectedElem->getSelectedState() == SELECTED_RIGHT || 
				context.SelectedElem->getWhich() == ELEM_PLACEHOLDER) {
					context.canInsertEnd = context.SelectedCircuit->IsLast(context.SelectedElem);
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

int LadderDiagram::RungContainingSelected(void)
{
	vector<LadderCircuit>::size_type i;

	if(context.SelectedElem != nullptr) {
		for(i = 0; i < rungs.size(); i++) {
			if(rungs[i].getSubcktForElement(context.SelectedElem) != nullptr)
				return i;
		}
	}

	return -1;
}

void LadderDiagram::NewRung(bool isAfter)
{
	vector<LadderCircuit>::iterator it;
	int position = RungContainingSelected();

	if(position < 0) { // Se posicao menor que zero, insere no final
		it = rungs.end();
		position = rungs.size();
	} else {
		it = rungs.begin() + position;
		if(it != rungs.end() && isAfter) it++;
	}

	// Adiciona a linha na posicao calculada
	it = rungs.insert(it, LadderCircuit(true));

	// Adiciona o padding
	LadderElem *elem = new LadderElemPlaceHolder;
	it->AddElement(elem, context);

	SelectElement(elem, SELECTED_RIGHT);
}

bool LadderDiagram::AddElement(LadderElem *elem)
{
	// Se elemento a ser adicionado for nulo ou nao existe um elemento selecionado, retorna
	if(elem == nullptr || context.SelectedElem == nullptr || context.SelectedCircuit == nullptr) return false;

	bool ret = elem->CanInsert(context) && context.SelectedCircuit->AddElement(elem, context);

	// Se adicionado com sucesso, atualiza o contexto
	if(ret == true) {
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
