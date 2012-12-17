#include "poptools.h"

IntOp IntCode[MAX_INT_OPS];
int IntCodeLen;

static DWORD GenSymCountParThis;
static DWORD GenSymCountParOut;
static DWORD GenSymCountOneShot;
static DWORD GenSymCountFormattedString;

static WORD EepromAddrFree;

//-----------------------------------------------------------------------------
// Pretty-print the intermediate code to a file, for debugging purposes.
//-----------------------------------------------------------------------------
void IntDumpListing(char *outFile)
{
    FILE *f = fopen(outFile, "w");
    if(!f) {
        Error("Couldn't dump intermediate code to '%s'.", outFile);
    }

    int i;
    int indent = 0;
    for(i = 0; i < IntCodeLen; i++) {

        if(IntCode[i].op == INT_END_IF) indent--;
        if(IntCode[i].op == INT_ELSE) indent--;
    
        fprintf(f, "%3d:", i);
        int j;
        for(j = 0; j < indent; j++) fprintf(f, "    ");

        switch(IntCode[i].op) {
            case INT_SET_BIT:
                fprintf(f, "set bit '%s'", IntCode[i].name1);
                break;

            case INT_CLEAR_BIT:
                fprintf(f, "clear bit '%s'", IntCode[i].name1);
                break;

            case INT_CHECK_BIT:
                fprintf(f, "check bit '%s'", IntCode[i].name1);
                break;

            case INT_COPY_BIT_TO_BIT:
                fprintf(f, "let bit '%s' := '%s'", IntCode[i].name1,
                    IntCode[i].name2);
                break;

            case INT_SET_VARIABLE_TO_LITERAL:
                fprintf(f, "let var '%s' := %d", IntCode[i].name1,
                    IntCode[i].literal);
                break;

            case INT_SET_VARIABLE_TO_VARIABLE:
                fprintf(f, "let var '%s' := '%s'", IntCode[i].name1,
                    IntCode[i].name2);
                break;

            case INT_SET_VARIABLE_ADD:
                fprintf(f, "let var '%s' := '%s' + '%s'", IntCode[i].name1,
                    IntCode[i].name2, IntCode[i].name3);
                break;

            case INT_SET_VARIABLE_SUBTRACT:
                fprintf(f, "let var '%s' := '%s' - '%s'", IntCode[i].name1,
                    IntCode[i].name2, IntCode[i].name3);
                break;

            case INT_SET_VARIABLE_MULTIPLY:
                fprintf(f, "let var '%s' := '%s' * '%s'", IntCode[i].name1,
                    IntCode[i].name2, IntCode[i].name3);
                break;

            case INT_SET_VARIABLE_DIVIDE:
                fprintf(f, "let var '%s' := '%s' / '%s'", IntCode[i].name1,
                    IntCode[i].name2, IntCode[i].name3);
                break;

            case INT_INCREMENT_VARIABLE:
                fprintf(f, "increment '%s'", IntCode[i].name1);
                break;

            case INT_READ_ADC:
                fprintf(f, "read adc '%s'", IntCode[i].name1);
                break;

            case INT_READ_ENC:
                fprintf(f, "read enc '%s'", IntCode[i].name1);
                break;

			case INT_READ_FORMATTED_STRING:
                fprintf(f, "read format string '%s'", IntCode[i].name1);
                break;

			case INT_WRITE_FORMATTED_STRING:
                fprintf(f, "write format string '%s'", IntCode[i].name1);
                break;

			case INT_READ_SERVO_YASKAWA:
                fprintf(f, "read servo yaskawa '%s'", IntCode[i].name1);
                break;

			case INT_WRITE_SERVO_YASKAWA:
                fprintf(f, "write servo yaskawa '%s'", IntCode[i].name1);
                break;

            case INT_READ_USS:
                fprintf(f, "read uss '%s'", IntCode[i].name1);
                break;

            case INT_WRITE_USS:
                fprintf(f, "write uss '%s'", IntCode[i].name1);
                break;

            case INT_SET_PWM:
                fprintf(f, "set pwm '%s' %s Hz", IntCode[i].name1,
                    IntCode[i].name2);
                break;

            case INT_EEPROM_BUSY_CHECK:
                fprintf(f, "set bit '%s' if EEPROM busy", IntCode[i].name1);
                break;
            
            case INT_EEPROM_READ:
                fprintf(f, "read EEPROM[%d,%d+1] into '%s'",
                    IntCode[i].literal, IntCode[i].literal, IntCode[i].name1);
                break;

            case INT_EEPROM_WRITE:
                fprintf(f, "write '%s' into EEPROM[%d,%d+1]",
                    IntCode[i].name1, IntCode[i].literal, IntCode[i].literal);
                break;

            case INT_UART_SEND:
                fprintf(f, "uart send from '%s', done? into '%s'",
                    IntCode[i].name1, IntCode[i].name2);
                break;

            case INT_UART_RECV:
                fprintf(f, "uart recv int '%s', have? into '%s'", 
                    IntCode[i].name1, IntCode[i].name2);
                break;

            case INT_IF_BIT_SET:
                fprintf(f, "if '%s' {", IntCode[i].name1); indent++;
                break;

            case INT_IF_BIT_CLEAR:
                fprintf(f, "if not '%s' {", IntCode[i].name1); indent++;
                break;

            case INT_IF_VARIABLE_LES_LITERAL:
                fprintf(f, "if '%s' < %d {", IntCode[i].name1,
                    IntCode[i].literal); indent++;
                break;

            case INT_IF_VARIABLE_EQUALS_VARIABLE:
                fprintf(f, "if '%s' == '%s' {", IntCode[i].name1,
                    IntCode[i].name2); indent++;
                break;

            case INT_IF_VARIABLE_GRT_VARIABLE:
                fprintf(f, "if '%s' > '%s' {", IntCode[i].name1,
                    IntCode[i].name2); indent++;
                break;

            case INT_END_IF:
                fprintf(f, "}");
                break;

            case INT_ELSE:
                fprintf(f, "} else {"); indent++;
                break;

            case INT_SIMULATE_NODE_STATE:
                // simulation-only; the real code generators don't care
                break;

            case INT_COMMENT:
                fprintf(f, "# %s", IntCode[i].name1);
                break;

            default:
                oops();
        }
        fprintf(f, "\n");
        fflush(f);
    }
    fclose(f);
}

//-----------------------------------------------------------------------------
// Convert a hex digit (0-9a-fA-F) to its hex value, or return -1 if the
// character is not a hex digit.
//-----------------------------------------------------------------------------
int HexDigit(int c)
{
    if((c >= '0') && (c <= '9')) {
        return c - '0';
    } else if((c >= 'a') && (c <= 'f')) {
        return 10 + (c - 'a');
    } else if((c >= 'A') && (c <= 'F')) {
        return 10 + (c - 'A');
    }
    return -1;
}

//-----------------------------------------------------------------------------
// Generate a unique symbol (unique with each call) having the given prefix
// guaranteed not to conflict with any user symbols.
//-----------------------------------------------------------------------------
static void GenSymParThis(char *dest)
{
    sprintf(dest, "$parThis_%04x", GenSymCountParThis);
    GenSymCountParThis++;
}
static void GenSymParOut(char *dest)
{
    sprintf(dest, "$parOut_%04x", GenSymCountParOut);
    GenSymCountParOut++;
}
static void GenSymOneShot(char *dest)
{
    sprintf(dest, "$oneShot_%04x", GenSymCountOneShot);
    GenSymCountOneShot++;
}
static void GenSymFormattedString(char *dest)
{
    sprintf(dest, "$formattedString_%04x", GenSymCountFormattedString);
    GenSymCountFormattedString++;
}

//-----------------------------------------------------------------------------
// Compile an instruction to the program.
//-----------------------------------------------------------------------------
static void Op(int op, char *name1, char *name2, char *name3, char *name4, char *name5, char *name6, char *name7, SWORD lit, unsigned char bit)
{
    IntCode[IntCodeLen].op = op;
    if(name1) strcpy(IntCode[IntCodeLen].name1, name1);
    if(name2) strcpy(IntCode[IntCodeLen].name2, name2);
    if(name3) strcpy(IntCode[IntCodeLen].name3, name3);
    if(name4) strcpy(IntCode[IntCodeLen].name4, name4);
    if(name5) strcpy(IntCode[IntCodeLen].name5, name5);
    if(name6) strcpy(IntCode[IntCodeLen].name6, name6);
	if(name7) strcpy(IntCode[IntCodeLen].name7, name7);
    IntCode[IntCodeLen].literal = lit;
	IntCode[IntCodeLen].bit = bit;

	switch (op)
	{
	case INT_SET_BIT:
		strcpy(IntCode[IntCodeLen].desc, "SET_BIT");
		break;
	case INT_CHECK_BIT:
		strcpy(IntCode[IntCodeLen].desc, "CHECK_BIT");
		break;
	case INT_CLEAR_BIT:
		strcpy(IntCode[IntCodeLen].desc, "CLEAR_BIT");
		break;
	case INT_COPY_BIT_TO_BIT:
		strcpy(IntCode[IntCodeLen].desc, "COPY_BIT_TO_BIT");
		break;
	case INT_SET_VARIABLE_TO_LITERAL:
		strcpy(IntCode[IntCodeLen].desc, "SET_VARIABLE_TO_LITERAL");
		break;
	case INT_SET_VARIABLE_TO_VARIABLE:
		strcpy(IntCode[IntCodeLen].desc, "SET_VARIABLE_TO_VARIABLE");
		break;
	case INT_INCREMENT_VARIABLE:
		strcpy(IntCode[IntCodeLen].desc, "INCREMENT_VARIABLE");
		break;
	case INT_SET_VARIABLE_ADD:
		strcpy(IntCode[IntCodeLen].desc, "SET_VARIABLE_ADD");
		break;
	case INT_SET_VARIABLE_SUBTRACT:
		strcpy(IntCode[IntCodeLen].desc, "SET_VARIABLE_SUBTRACT");
		break;
	case INT_SET_VARIABLE_MULTIPLY:
		strcpy(IntCode[IntCodeLen].desc, "SET_VARIABLE_MULTIPLY");
		break;
	case INT_SET_VARIABLE_DIVIDE:
		strcpy(IntCode[IntCodeLen].desc, "SET_VARIABLE_DIVIDE");
		break;
	case INT_READ_ADC:
		strcpy(IntCode[IntCodeLen].desc, "READ_ADC");
		break;
	case INT_SET_PWM:
		strcpy(IntCode[IntCodeLen].desc, "SET_PWM");
		break;
	case INT_UART_SEND:
		strcpy(IntCode[IntCodeLen].desc, "UART_SEND");
		break;
	case INT_UART_RECV:
		strcpy(IntCode[IntCodeLen].desc, "UART_RECV");
		break;
	case INT_EEPROM_BUSY_CHECK:
		strcpy(IntCode[IntCodeLen].desc, "EEPROM_BUSY_CHECK");
		break;
	case INT_EEPROM_READ:
		strcpy(IntCode[IntCodeLen].desc, "EEPROM_READ");
		break;
	case INT_EEPROM_WRITE:
		strcpy(IntCode[IntCodeLen].desc, "EEPROM_WRITE");
		break;
	case INT_READ_FORMATTED_STRING:
		strcpy(IntCode[IntCodeLen].desc, "FORMATTED_STRING_READ");
		break;
	case INT_WRITE_FORMATTED_STRING:
		strcpy(IntCode[IntCodeLen].desc, "FORMATTED_STRING_WRITE");
		break;
	case INT_READ_SERVO_YASKAWA:
		strcpy(IntCode[IntCodeLen].desc, "FORMATTED_SERVO_YASKAWA_READ");
		break;
	case INT_WRITE_SERVO_YASKAWA:
		strcpy(IntCode[IntCodeLen].desc, "FORMATTED_SERVO_YASKAWA_WRITE");
		break;
	case INT_READ_ENC:
		strcpy(IntCode[IntCodeLen].desc, "READ_ENC");
		break;
	case INT_RESET_ENC:
		strcpy(IntCode[IntCodeLen].desc, "RESET_ENC");
		break;
	case INT_READ_USS:
		strcpy(IntCode[IntCodeLen].desc, "READ_USS");
		break;
	case INT_WRITE_USS:
		strcpy(IntCode[IntCodeLen].desc, "WRITE_USS");
		break;
	/*case INT_IF_GROUP(x) (((x) >= 50) && ((x) < 60))
		strcpy(IntCode[IntCodeLen].desc, "???");
		break;*/
	case INT_IF_BIT_SET:
		strcpy(IntCode[IntCodeLen].desc, "IF_BIT_SET");
		break;
	case INT_IF_BIT_CLEAR:
		strcpy(IntCode[IntCodeLen].desc, "IF_BIT_CLEAR");
		break;
	case INT_IF_VARIABLE_LES_LITERAL:
		strcpy(IntCode[IntCodeLen].desc, "IF_VARIABLE_LES_LITERAL");
		break;
	case INT_IF_VARIABLE_EQUALS_VARIABLE:
		strcpy(IntCode[IntCodeLen].desc, "IF_VARIABLE_EQUALS_VARIABLE");
		break;
	case INT_IF_VARIABLE_GRT_VARIABLE:
		strcpy(IntCode[IntCodeLen].desc, "IF_VARIABLE_GRT_VARIABLE");
		break;
	case INT_ELSE:
		strcpy(IntCode[IntCodeLen].desc, "ELSE");
		break;
	case INT_ELSE_IF:
		strcpy(IntCode[IntCodeLen].desc, "ELSE IF");
		break;
	case INT_END_IF:
		strcpy(IntCode[IntCodeLen].desc, "END_IF");
		break;
	case INT_SIMULATE_NODE_STATE:
		strcpy(IntCode[IntCodeLen].desc, "SIMULATE_NODE_STATE");
		break;
	case INT_COMMENT:
		strcpy(IntCode[IntCodeLen].desc, "COMMENT");
		break;
	// Only used for the interpretable code.
	//#define INT_END_OF_PROGRAM                     255
	default:
		strcpy(IntCode[IntCodeLen].desc, "???");
	}

    IntCodeLen++;
}
static void Op(int op, char *name1, char *name2, char *name3, char *name4, SWORD lit, unsigned char bit)
{
	Op(op, name1, name2, name3, name4, NULL, NULL, NULL, lit, bit);
}
static void Op(int op, char *name1, char *name2, char *name3, SWORD lit, unsigned char bit)
{
	Op(op, name1, name2, name3, NULL, lit, bit);
}
static void Op(int op, char *name1, char *name2, SWORD lit)
{
    Op(op, name1, name2, NULL, lit, 0);
}
static void Op(int op, char *name1, SWORD lit)
{
    Op(op, name1, NULL, NULL, lit, 0);
}
static void Op(int op, char *name1, char *name2)
{
    Op(op, name1, name2, NULL, 0, 0);
}
static void Op(int op, char *name1)
{
    Op(op, name1, NULL, NULL, 0, 0);
}
static void OpBit(int op, char *name1, unsigned char bit)
{
    Op(op, name1, NULL, NULL, 0, bit);
}
static void OpBit(int op, char *name1, char *name2, unsigned char bit)
{
    Op(op, name1, name2, NULL, 0, bit);
}
static void Op(int op)
{
    Op(op, NULL, NULL, NULL, 0, 0);
}

//-----------------------------------------------------------------------------
// Compile the instruction that the simulator uses to keep track of which
// nodes are energized (so that it can display which branches of the circuit
// are energized onscreen). The MCU code generators ignore this, of course.
//-----------------------------------------------------------------------------
static void SimState(BOOL *b, char *name)
{
    IntCode[IntCodeLen].op = INT_SIMULATE_NODE_STATE;
    strcpy(IntCode[IntCodeLen].name1, name);
    IntCode[IntCodeLen].poweredAfter = b;
    IntCodeLen++;
}

//-----------------------------------------------------------------------------
// printf-like comment function
//-----------------------------------------------------------------------------
void Comment(char *str, ...)
{
    va_list f;
    char buf[MAX_NAME_LEN];
    va_start(f, str);
    vsprintf(buf, str, f);
    Op(INT_COMMENT, buf);
}

//-----------------------------------------------------------------------------
// Calculate the period in scan units from the period in microseconds, and
// raise an error if the given period is unachievable.
//-----------------------------------------------------------------------------
static int TimerPeriod(ElemLeaf *l)
{
    unsigned int period = (l->d.timer.delay / Prog.settings.cycleTime) - 1;

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

//-----------------------------------------------------------------------------
// Try to turn a string into a 16-bit constant, and raise an error if
// something bad happens when we do so (e.g. out of range).
//-----------------------------------------------------------------------------
SWORD CheckMakeNumber(char *str)
{
    int val;

    if(*str == '\'') {
        val = str[1];
    } else {
        val = atoi(str);
    }

    if(val < (int)-2147483647 || val > (int)2147483647) {   
        Error(_("Constant %d out of range: -2147483648 to 2147483647 inclusive."), val);
        CompileError();
    }

    return (SWORD)val;
}

//-----------------------------------------------------------------------------
// Return an integer power of ten.
//-----------------------------------------------------------------------------
static int TenToThe(int x)
{
    int i;
    int r = 1;
    for(i = 0; i < x; i++) {
        r *= 10;
    }
    return r;
}

//-----------------------------------------------------------------------------
// Compile code to evaluate the given bit of ladder logic. The rung input
// state is in stateInOut before calling and will be in stateInOut after
// calling.
//-----------------------------------------------------------------------------
static char *VarFromExpr(char *expr, char *tempName)
{
    if(IsNumber(expr)) {
        Op(INT_SET_VARIABLE_TO_LITERAL, tempName, CheckMakeNumber(expr));
        return tempName;
    } else {
        return expr;
    }
}
static void IntCodeFromCircuit(int which, void *any, char *stateInOut)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            int i;
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            
            Comment(_("start series ["));
            for(i = 0; i < s->count; i++) {
                IntCodeFromCircuit(s->contents[i].which, s->contents[i].d.any,
                    stateInOut);
            }
            Comment(_("] finish series"));
            break;
        }
        case ELEM_PARALLEL_SUBCKT: {
            char parThis[MAX_NAME_LEN];
            GenSymParThis(parThis);

            char parOut[MAX_NAME_LEN];
            GenSymParOut(parOut);

            Comment(_("start parallel ["));

            Op(INT_CLEAR_BIT, parOut);

            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                Op(INT_COPY_BIT_TO_BIT, parThis, stateInOut);

                IntCodeFromCircuit(p->contents[i].which, p->contents[i].d.any,
                    parThis);

                Op(INT_IF_BIT_SET, parThis);
                Op(INT_SET_BIT, parOut);
                Op(INT_END_IF);
            }
            Op(INT_COPY_BIT_TO_BIT, stateInOut, parOut);
            Comment(_("] finish parallel"));
            
            break;
        }
        case ELEM_CONTACTS: {
			char name[MAX_NAME_LEN+2];
			if(l->d.contacts.type == IO_TYPE_INTERNAL_FLAG) {
				sprintf(name, "$%s", l->d.contacts.name);
			} else {
				strcpy(name, l->d.contacts.name);
			}
            if(l->d.contacts.negated) {
                OpBit(INT_IF_BIT_SET, name, l->d.contacts.bit);
            } else {
                OpBit(INT_IF_BIT_CLEAR, name, l->d.contacts.bit);
            }
			OpBit(INT_CLEAR_BIT, stateInOut, l->d.contacts.bit);
            Op(INT_END_IF);
            break;
        }
        case ELEM_COIL: {
            if(l->d.coil.negated) {
                Op(INT_IF_BIT_SET, stateInOut);
				OpBit(INT_CLEAR_BIT, l->d.contacts.name, l->d.coil.bit);
                Op(INT_ELSE);
                OpBit(INT_SET_BIT, l->d.contacts.name, l->d.coil.bit);
                Op(INT_END_IF);
            } else if(l->d.coil.setOnly) {
                OpBit(INT_IF_BIT_SET, stateInOut, l->d.coil.bit);
                OpBit(INT_SET_BIT, l->d.contacts.name, l->d.coil.bit);
                Op(INT_END_IF);
            } else if(l->d.coil.resetOnly) {
                OpBit(INT_IF_BIT_SET, stateInOut, l->d.coil.bit);
                OpBit(INT_CLEAR_BIT, l->d.contacts.name, l->d.coil.bit);
                Op(INT_END_IF);
            } else {
                OpBit(INT_COPY_BIT_TO_BIT, l->d.contacts.name, stateInOut, l->d.coil.bit);
            }
            break;
        }
        case ELEM_RTO: {
            int period = TimerPeriod(l);

            Op(INT_IF_VARIABLE_LES_LITERAL, l->d.timer.name, period);

            Op(INT_IF_BIT_SET, stateInOut);
            Op(INT_INCREMENT_VARIABLE, l->d.timer.name);
            Op(INT_END_IF);
            Op(INT_CLEAR_BIT, stateInOut);

            Op(INT_ELSE);

            Op(INT_SET_BIT, stateInOut);

            Op(INT_END_IF);

            break;
        }
         case ELEM_RTC: {
			char sd[10],sm[10],sy[10],sh[10],smm[10],ss[10];
			char ed[10],em[10],ey[10],eh[10],emm[10],es[10];
			char mode[10];

			_itoa(l->d.rtc.start.tm_mday, sd  , 10);
			_itoa(l->d.rtc.start.tm_mon , sm  , 10);
            _itoa(l->d.rtc.start.tm_year, sy  , 10);
			_itoa(l->d.rtc.start.tm_hour, sh  , 10);
			_itoa(l->d.rtc.start.tm_min , smm , 10);
			_itoa(l->d.rtc.start.tm_sec , ss  , 10);

			_itoa(l->d.rtc.end.tm_mday  , ed  , 10);
			_itoa(l->d.rtc.end.tm_mon   , em  , 10);
            _itoa(l->d.rtc.end.tm_year  , ey  , 10);
			_itoa(l->d.rtc.end.tm_hour  , eh  , 10);
			_itoa(l->d.rtc.end.tm_min   , emm , 10);
			_itoa(l->d.rtc.end.tm_sec   , es  , 10);

			_itoa(l->d.rtc.mode         , mode, 10);

            Op(INT_IF_BIT_SET, stateInOut);
				Op(INT_SET_RTC, sd, sm, sy, sh, smm, ss, NULL, 0, 0);
				Op(INT_SET_RTC, ed, em, ey, eh, emm, es, NULL, 0, 1);
				Op(INT_CHECK_RTC, stateInOut, mode, l->d.rtc.wday);
            Op(INT_END_IF);

            break;
        }       
		 case ELEM_RES:
            Op(INT_IF_BIT_SET, stateInOut);
            Op(INT_SET_VARIABLE_TO_LITERAL, l->d.reset.name);
            Op(INT_END_IF);
            break;

        case ELEM_TON: {
            int period = TimerPeriod(l);

            Op(INT_IF_BIT_SET, stateInOut);

            Op(INT_IF_VARIABLE_LES_LITERAL, l->d.timer.name, period);

            Op(INT_INCREMENT_VARIABLE, l->d.timer.name);
            Op(INT_CLEAR_BIT, stateInOut);
            Op(INT_END_IF);

            Op(INT_ELSE);

            Op(INT_SET_VARIABLE_TO_LITERAL, l->d.timer.name);

            Op(INT_END_IF);

            break;
        }
        case ELEM_TOF: {
            int period = TimerPeriod(l);

            // All variables start at zero by default, so by default the
            // TOF timer would start out with its output forced HIGH, until
            // it finishes counting up. This does not seem to be what
            // people expect, so add a special case to fix that up.
            char antiGlitchName[MAX_NAME_LEN];
            sprintf(antiGlitchName, "$%s_antiglitch", l->d.timer.name);
            Op(INT_IF_BIT_CLEAR, antiGlitchName);
                Op(INT_SET_VARIABLE_TO_LITERAL, l->d.timer.name, period);
            Op(INT_END_IF);
            Op(INT_SET_BIT, antiGlitchName);
            
            Op(INT_IF_BIT_CLEAR, stateInOut);

            Op(INT_IF_VARIABLE_LES_LITERAL, l->d.timer.name, period);

            Op(INT_INCREMENT_VARIABLE, l->d.timer.name);
            Op(INT_SET_BIT, stateInOut);
            Op(INT_END_IF);

            Op(INT_ELSE);

            Op(INT_SET_VARIABLE_TO_LITERAL, l->d.timer.name);

            Op(INT_END_IF);
            break;
        }
        case ELEM_CTU: {
            char storeName[MAX_NAME_LEN];
            GenSymOneShot(storeName);

            Op(INT_IF_BIT_SET, stateInOut);
                Op(INT_IF_BIT_CLEAR, storeName);
                    Op(INT_INCREMENT_VARIABLE, l->d.counter.name);
                Op(INT_END_IF);
            Op(INT_END_IF);
            Op(INT_COPY_BIT_TO_BIT, storeName, stateInOut);

            Op(INT_IF_VARIABLE_LES_LITERAL, l->d.counter.name,
                l->d.counter.max);
                Op(INT_CLEAR_BIT, stateInOut);
            Op(INT_ELSE);
                Op(INT_SET_BIT, stateInOut);
            Op(INT_END_IF);
            break;
        }
        case ELEM_CTD: {
            char storeName[MAX_NAME_LEN];
            GenSymOneShot(storeName);

            Op(INT_IF_BIT_SET, stateInOut);
                Op(INT_IF_BIT_CLEAR, storeName);
                    Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", 1);
                    Op(INT_SET_VARIABLE_SUBTRACT, l->d.counter.name,
                        l->d.counter.name, "$scratch", 0, 0);
                Op(INT_END_IF);
            Op(INT_END_IF);
            Op(INT_COPY_BIT_TO_BIT, storeName, stateInOut);

			Op(INT_IF_VARIABLE_GRT_VARIABLE, l->d.counter.name,
                l->d.counter.max);
                Op(INT_CLEAR_BIT, stateInOut);
            Op(INT_ELSE);
                Op(INT_SET_BIT, stateInOut);
            Op(INT_END_IF);
            break;
        }
        case ELEM_CTC: {
            char storeName[MAX_NAME_LEN];
            GenSymOneShot(storeName);

            Op(INT_IF_BIT_SET, stateInOut);
                Op(INT_IF_BIT_CLEAR, storeName);
                    Op(INT_INCREMENT_VARIABLE, l->d.counter.name);
                    Op(INT_IF_VARIABLE_LES_LITERAL, l->d.counter.name,
                        l->d.counter.max+1);
                    Op(INT_ELSE);
                        Op(INT_SET_VARIABLE_TO_LITERAL, l->d.counter.name,
                            (SWORD)0);
                    Op(INT_END_IF);
                Op(INT_END_IF);
            Op(INT_END_IF);
            Op(INT_COPY_BIT_TO_BIT, storeName, stateInOut);
            break;
        }
        case ELEM_GRT:
        case ELEM_GEQ:
        case ELEM_LES:
        case ELEM_LEQ:
        case ELEM_NEQ:
        case ELEM_EQU: {
            char *op1 = VarFromExpr(l->d.cmp.op1, "$scratch");
            char *op2 = VarFromExpr(l->d.cmp.op2, "$scratch2");
            
            if(which == ELEM_GRT) {
                Op(INT_IF_VARIABLE_GRT_VARIABLE, op1, op2);
                Op(INT_ELSE);
            } else if(which == ELEM_GEQ) {
                Op(INT_IF_VARIABLE_GRT_VARIABLE, op2, op1);
            } else if(which == ELEM_LES) {
                Op(INT_IF_VARIABLE_GRT_VARIABLE, op2, op1);
                Op(INT_ELSE);
            } else if(which == ELEM_LEQ) {
                Op(INT_IF_VARIABLE_GRT_VARIABLE, op1, op2);
            } else if(which == ELEM_EQU) {
                Op(INT_IF_VARIABLE_EQUALS_VARIABLE, op1, op2);
                Op(INT_ELSE);
            } else if(which == ELEM_NEQ) {
                Op(INT_IF_VARIABLE_EQUALS_VARIABLE, op1, op2);
            } else oops();

            Op(INT_CLEAR_BIT, stateInOut);
            Op(INT_END_IF);
            break;
        }
        case ELEM_ONE_SHOT_RISING: {
            char storeName[MAX_NAME_LEN];
            GenSymOneShot(storeName);

            Op(INT_COPY_BIT_TO_BIT, "$scratch", stateInOut);
            Op(INT_IF_BIT_SET, storeName);
            Op(INT_CLEAR_BIT, stateInOut);
            Op(INT_END_IF);
            Op(INT_COPY_BIT_TO_BIT, storeName, "$scratch");
            break;
        }
        case ELEM_ONE_SHOT_FALLING: {
            char storeName[MAX_NAME_LEN];
            GenSymOneShot(storeName);
        
            Op(INT_COPY_BIT_TO_BIT, "$scratch", stateInOut);

            Op(INT_IF_BIT_CLEAR, stateInOut);
            Op(INT_IF_BIT_SET, storeName);
            Op(INT_SET_BIT, stateInOut);
            Op(INT_END_IF);
            Op(INT_ELSE);
            Op(INT_CLEAR_BIT, stateInOut);
            Op(INT_END_IF);

            Op(INT_COPY_BIT_TO_BIT, storeName, "$scratch");
            break;
        }
        case ELEM_MOVE: {
            if(IsNumber(l->d.move.dest)) {
                Error(_("Move instruction: '%s' not a valid destination."),
                    l->d.move.dest);
                CompileError();
            }
            Op(INT_IF_BIT_SET, stateInOut);
            if(IsNumber(l->d.move.src)) {
                Op(INT_SET_VARIABLE_TO_LITERAL, l->d.move.dest,
                    CheckMakeNumber(l->d.move.src));
            } else {
                Op(INT_SET_VARIABLE_TO_VARIABLE, l->d.move.dest, l->d.move.src,
                    0);
            }
            Op(INT_END_IF);
            break;
        }

        case ELEM_SQRT: {
            if(IsNumber(l->d.sqrt.dest)) {
                Error(_("Sqrt instruction: '%s' not a valid destination."),
                    l->d.sqrt.dest);
                CompileError();
            }
            Op(INT_IF_BIT_SET, stateInOut);
            if(IsNumber(l->d.sqrt.src)) {
                Op(INT_SET_VARIABLE_TO_LITERAL, l->d.sqrt.dest,
                    CheckMakeNumber(l->d.sqrt.src));
            } else {
                Op(INT_SET_VARIABLE_TO_VARIABLE, l->d.sqrt.dest, l->d.sqrt.src,
                    0);
            }
			Op(INT_SQRT, l->d.sqrt.dest);
			Op(INT_END_IF);
            break;
        }

        case ELEM_RAND: {
			if(IsNumber(l->d.rand.var)) {
                Error(_("Rand instruction: '%s' not a valid destination."),
					l->d.rand.var);
                CompileError();
            }

			Op(INT_IF_BIT_SET, stateInOut);
				Op(INT_RAND, l->d.rand.var, l->d.rand.min, l->d.rand.max, 0, 0);
			Op(INT_END_IF);

			break;
        }

        case ELEM_ABS: {
			if(IsNumber(l->d.abs.dest)) {
                Error(_("Abs instruction: '%s' not a valid destination."),
                    l->d.abs.dest);
                CompileError();
            }

			Op(INT_IF_BIT_SET, stateInOut);
				Op(INT_IF_VARIABLE_LES_LITERAL, l->d.abs.src, (SWORD)0);
			        Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch_int", CheckMakeNumber("-1"));
					Op(INT_SET_VARIABLE_MULTIPLY, l->d.abs.dest, l->d.abs.src, "$scratch_int", 0, 0);
				Op(INT_ELSE);
					if(IsNumber(l->d.abs.src)) {
						Op(INT_SET_VARIABLE_TO_LITERAL, l->d.abs.dest,
							CheckMakeNumber(l->d.abs.src));
					} else {
						Op(INT_SET_VARIABLE_TO_VARIABLE, l->d.abs.dest, l->d.abs.src,
							0);
					}
	            Op(INT_END_IF);
            Op(INT_END_IF);

			break;
        }

        // These four are highly processor-dependent; the int code op does
        // most of the highly specific work
        {
            case ELEM_READ_ADC:
                Op(INT_IF_BIT_SET, stateInOut);
                Op(INT_READ_ADC, l->d.readAdc.name);
                Op(INT_END_IF);
                break;

            case ELEM_SET_DA:
                Op(INT_IF_BIT_SET, stateInOut);
                Op(INT_SET_DA, l->d.setDA.name, l->d.setDA.mode);
                Op(INT_END_IF);
                break;

            case ELEM_READ_ENC:
				Op(INT_IF_BIT_SET, stateInOut);
                Op(INT_READ_ENC, l->d.readEnc.name);
                Op(INT_END_IF);
                break;

            case ELEM_RESET_ENC:
				Op(INT_IF_BIT_SET, stateInOut);
				Op(INT_RESET_ENC);
                Op(INT_END_IF);
                break;

			case ELEM_MULTISET_DA:
				{
					char gaint[10];
					char gainr[10];
					char type [10];
					char str_initval[MAX_NAME_LEN];
					char oneShot[MAX_NAME_LEN];
					GenSymOneShot(oneShot);

					_itoa(l->d.multisetDA.gaint, gaint, 10);
					_itoa(l->d.multisetDA.gainr, gainr, 10);
					_itoa(l->d.multisetDA.type , type , 10);

					strcpy(str_initval, l->d.multisetDA.name1);
					if(IsNumber(str_initval)) {
						int initval = atoi(str_initval);

						if (l->d.multisetDA.type == 1)  // (mV)
							initval = static_cast<int>(DA_RESOLUTION * (initval / DA_VOLTAGE));
						else if (l->d.multisetDA.type == 2) // (%)
							initval = static_cast<int>(DA_RESOLUTION * (initval / 100.0f));

						_itoa(initval, str_initval, 10);
					}

					Op(INT_IF_BIT_SET, stateInOut);
						Op(INT_IF_BIT_CLEAR, oneShot);
							Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
							Op(INT_MULTISET_DA, l->d.multisetDA.name, str_initval, gaint, gainr, type, NULL, NULL, l->d.multisetDA.linear, l->d.multisetDA.StartFromCurrentValue ? 2 : l->d.multisetDA.speedup);
						Op(INT_END_IF);
					Op(INT_ELSE);
						Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
					Op(INT_END_IF);
				}
				break;

	        case ELEM_READ_FORMATTED_STRING: 
			case ELEM_WRITE_FORMATTED_STRING:
				{
				char var[128];
				char string[128];

				sprintf(var, "%s", l->d.fmtdStr.var);
				sprintf(string, "%s", l->d.fmtdStr.string);

				// We want to respond to rising edges, so yes we need a one shot.
				char oneShot[MAX_NAME_LEN];
				GenSymOneShot(oneShot);
				char byPass[MAX_NAME_LEN];
				GenSymOneShot(byPass);

				// OneShot 
				Op(INT_IF_BIT_SET, stateInOut);
					Op(INT_IF_BIT_CLEAR, oneShot);
						Op(INT_IF_BIT_SET, "$SerialReady");
							if (which == ELEM_READ_FORMATTED_STRING)
								Op(INT_READ_FORMATTED_STRING, var, string);
							else
								Op(INT_WRITE_FORMATTED_STRING, var, string);
							Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
						Op(INT_END_IF);
						Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_COPY_BIT_TO_BIT, byPass, stateInOut);
					Op(INT_END_IF);
					Op(INT_IF_BIT_CLEAR, byPass);
						Op(INT_IF_BIT_SET, "$SerialReady");
							Op(INT_SET_BIT, byPass);
						Op(INT_ELSE);
							Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_END_IF);
					Op(INT_END_IF);
				Op(INT_ELSE);
					Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
				Op(INT_END_IF);

				}
                break;

	        case ELEM_READ_SERVO_YASKAWA: 
			case ELEM_WRITE_SERVO_YASKAWA:
				{
				char id[128];
				char var_yaskawa[128];
				char string[128];

				sprintf(id, "%s", l->d.servoYaskawa.id);
				sprintf(var_yaskawa, "%s", l->d.servoYaskawa.var);
				sprintf(string, "%s", l->d.servoYaskawa.string);

				// We want to respond to rising edges, so yes we need a one shot.
				char oneShot[MAX_NAME_LEN];
				GenSymOneShot(oneShot);
				char byPass[MAX_NAME_LEN];
				GenSymOneShot(byPass);

				// OneShot 
				Op(INT_IF_BIT_SET, stateInOut);
					Op(INT_IF_BIT_CLEAR, oneShot);
						Op(INT_IF_BIT_SET, "$SerialReady");
							if (which == ELEM_READ_SERVO_YASKAWA)
								Op(INT_READ_SERVO_YASKAWA, var_yaskawa, string, id, 0, 0);
							else
								Op(INT_WRITE_SERVO_YASKAWA, var_yaskawa, string, id, 0, 0);
							Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
						Op(INT_END_IF);
						Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_COPY_BIT_TO_BIT, byPass, stateInOut);
					Op(INT_END_IF);
					Op(INT_IF_BIT_CLEAR, byPass);
						Op(INT_IF_BIT_SET, "$SerialReady");
							Op(INT_SET_BIT, byPass);
						Op(INT_ELSE);
							Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_END_IF);
					Op(INT_END_IF);
				Op(INT_ELSE);
					Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
				Op(INT_END_IF);

				}
                break;

            case ELEM_READ_USS:
            case ELEM_WRITE_USS:
				{
				char addr[10];
				char param[10];
				char param_set[10];
				char index[10];

				sprintf(addr, "%d", l->d.writeUSS.id);
				sprintf(param, "%d", l->d.writeUSS.parameter);
				sprintf(param_set, "%d", l->d.writeUSS.parameter_set);
				sprintf(index, "%d", l->d.writeUSS.index);

				// We want to respond to rising edges, so yes we need a one shot.
				char oneShot[MAX_NAME_LEN];
				GenSymOneShot(oneShot);
				char byPass[MAX_NAME_LEN];
				GenSymOneShot(byPass);

				// OneShot 
				Op(INT_IF_BIT_SET, stateInOut);
					Op(INT_IF_BIT_CLEAR, oneShot);
						Op(INT_IF_BIT_SET, "$SerialReady");
							if (which == ELEM_READ_USS)
								Op(INT_READ_USS, l->d.readUSS.name, addr, param, param_set, (SWORD)atoi(index), 0);
							else
								Op(INT_WRITE_USS, l->d.writeUSS.name, addr, param, param_set, (SWORD)atoi(index), 0);
							Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
						Op(INT_END_IF);
						Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_COPY_BIT_TO_BIT, byPass, stateInOut);
					Op(INT_END_IF);
					Op(INT_IF_BIT_CLEAR, byPass);
						Op(INT_IF_BIT_SET, "$SerialReady");
							Op(INT_SET_BIT, byPass);
						Op(INT_ELSE);
							Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_END_IF);
					Op(INT_END_IF);
				Op(INT_ELSE);
					Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
				Op(INT_END_IF);

				}
                break;
				
            case ELEM_READ_MODBUS:
            case ELEM_WRITE_MODBUS:
				{
				char id[10];
				char addr[10];

				sprintf(id, "%d", l->d.writeModbus.id);
				sprintf(addr, "%d", l->d.writeModbus.address);

				// We want to respond to rising edges, so yes we need a one shot.
				char oneShot[MAX_NAME_LEN];
				GenSymOneShot(oneShot);
				char byPass[MAX_NAME_LEN];
				GenSymOneShot(byPass);

				// OneShot 
				Op(INT_IF_BIT_SET, stateInOut);
					Op(INT_IF_BIT_CLEAR, oneShot);
						Op(INT_IF_BIT_SET, "$SerialReady");
							if (which == ELEM_READ_MODBUS)
								Op(INT_READ_MODBUS, l->d.readModbus.name, id, addr, 0, (unsigned char)l->d.readModbus.int32);
							else
								Op(INT_WRITE_MODBUS, l->d.writeModbus.name, id, addr, 0, (unsigned char)l->d.writeModbus.int32);
							Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
						Op(INT_END_IF);
						Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_COPY_BIT_TO_BIT, byPass, stateInOut);
					Op(INT_END_IF);
					Op(INT_IF_BIT_CLEAR, byPass);
						Op(INT_IF_BIT_SET, "$SerialReady");
							Op(INT_SET_BIT, byPass);
						if(which == ELEM_READ_MODBUS ? l->d.readModbus.retransmitir : l->d.writeModbus.retransmitir) { // Retransmitir?
							Op(INT_ELSE_IF); Op(INT_IF_BIT_SET, "$SerialTimeout");
								if (which == ELEM_READ_MODBUS)
									Op(INT_READ_MODBUS, l->d.readModbus.name, id, addr, 0, (unsigned char)l->d.readModbus.int32);
								else
									Op(INT_WRITE_MODBUS, l->d.writeModbus.name, id, addr, 0, (unsigned char)l->d.writeModbus.int32);
								Op(INT_CLEAR_BIT, "$SerialTimeout");
						}
						Op(INT_ELSE);
							Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_END_IF);
					Op(INT_END_IF);
				Op(INT_ELSE);
					Op(INT_IF_BIT_SET, oneShot);
						Op(INT_SET_BIT, "$SerialReady");
					Op(INT_END_IF);
					Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
				Op(INT_END_IF);

				}
                break;
				
            case ELEM_READ_MODBUS_ETH:
            case ELEM_WRITE_MODBUS_ETH:
				{
				char id[10];
				char addr[10];

				sprintf(id, "%d", l->d.writeModbusEth.id);
				sprintf(addr, "%d", l->d.writeModbusEth.address);

				// We want to respond to rising edges, so yes we need a one shot.
				char oneShot[MAX_NAME_LEN];
				GenSymOneShot(oneShot);
				char byPass[MAX_NAME_LEN];
				GenSymOneShot(byPass);

				// OneShot 
				Op(INT_IF_BIT_SET, stateInOut);
					Op(INT_IF_BIT_CLEAR, oneShot);
						Op(INT_IF_BIT_SET, "$SerialReady");
							if (which == ELEM_READ_MODBUS_ETH)
								Op(INT_READ_MODBUS_ETH, l->d.readModbusEth.name, id, addr, 0, (unsigned char)l->d.readModbusEth.int32);
							else
								Op(INT_WRITE_MODBUS_ETH, l->d.writeModbusEth.name, id, addr, 0, (unsigned char)l->d.writeModbusEth.int32);
							Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
						Op(INT_END_IF);
						Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_COPY_BIT_TO_BIT, byPass, stateInOut);
					Op(INT_END_IF);
					Op(INT_IF_BIT_CLEAR, byPass);
						Op(INT_IF_BIT_SET, "$SerialReady");
							Op(INT_SET_BIT, byPass);
						Op(INT_ELSE);
							Op(INT_CLEAR_BIT, stateInOut);
						Op(INT_END_IF);
					Op(INT_END_IF);
				Op(INT_ELSE);
					Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
				Op(INT_END_IF);

				}
                break;
				
            case ELEM_SET_PWM: {
                Op(INT_IF_BIT_SET, stateInOut);
                char line[80];
                // ugh; need a >16 bit literal though, could be >64 kHz
                sprintf(line, "%d", l->d.setPwm.targetFreq);
                Op(INT_SET_PWM, l->d.setPwm.name, line);
                Op(INT_END_IF);
                break;
            }
            case ELEM_PERSIST: {
				if(EepromAddrFree > EEPROM_SIZE) {
					Error(_("Muitas variáveis persistentes!"));
					CompileError();
				}
                Op(INT_IF_BIT_SET, stateInOut);

                // At startup, get the persistent variable from flash.
                char isInit[MAX_NAME_LEN];
                GenSymOneShot(isInit);
                Op(INT_IF_BIT_CLEAR, isInit);
                    Op(INT_CLEAR_BIT, "$scratch");
                    Op(INT_EEPROM_BUSY_CHECK, "$scratch");
                    Op(INT_IF_BIT_CLEAR, "$scratch");
                        Op(INT_EEPROM_READ, l->d.persist.var, isInit, EepromAddrFree);
                    Op(INT_END_IF);
				Op(INT_ELSE);
					// While running, continuously compare the EEPROM copy of
					// the variable against the RAM one; if they are different,
					// write the RAM one to EEPROM. 
					Op(INT_CLEAR_BIT, "$scratch");
					Op(INT_EEPROM_BUSY_CHECK, "$scratch");
					Op(INT_IF_BIT_CLEAR, "$scratch");
						Op(INT_EEPROM_READ, "$scratch2", "$scratch", EepromAddrFree);
						Op(INT_IF_VARIABLE_EQUALS_VARIABLE, "$scratch2",
							l->d.persist.var);
						Op(INT_ELSE);
							Op(INT_EEPROM_WRITE, l->d.persist.var, EepromAddrFree);
						Op(INT_END_IF);
					Op(INT_END_IF);
                Op(INT_END_IF);

                Op(INT_END_IF);

                EepromAddrFree += 2;
                break;
            }
            case ELEM_UART_SEND: {
				char oneShot[MAX_NAME_LEN];
				GenSymOneShot(oneShot);

				Op(INT_IF_BIT_SET, stateInOut);
					Op(INT_IF_BIT_CLEAR, oneShot);
						Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
			            Op(INT_UART_SEND, l->d.uart.name, stateInOut);
					Op(INT_END_IF);
				Op(INT_ELSE);
					Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
				Op(INT_END_IF);
                break;
			}

            case ELEM_UART_RECV: {
				char oneShot[MAX_NAME_LEN];
				GenSymOneShot(oneShot);

				Op(INT_IF_BIT_SET, stateInOut);
					Op(INT_IF_BIT_CLEAR, oneShot);
						Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
		                Op(INT_UART_RECV, l->d.uart.name, stateInOut);
					Op(INT_END_IF);
				Op(INT_ELSE);
					Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
				Op(INT_END_IF);
                break;
			}
        }

        case ELEM_DIV: {
			if(IsNumber(l->d.math.op2) && !atol(l->d.math.op2)) {
                Error(_("Math instruction: '%s' divided by zero!"),
                    l->d.math.op1);
                CompileError();
            }
        case ELEM_ADD:
        case ELEM_SUB:
        case ELEM_MUL:
            if(IsNumber(l->d.math.dest)) {
                Error(_("Math instruction: '%s' not a valid destination."),
                    l->d.math.dest);
                CompileError();
            }
            Op(INT_IF_BIT_SET, stateInOut);

            char *op1 = VarFromExpr(l->d.math.op1, "$scratch");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
            char *op2 = VarFromExpr(l->d.math.op2, "$scratch2");

            int intOp;
            if(which == ELEM_ADD) {
                intOp = INT_SET_VARIABLE_ADD;
            } else if(which == ELEM_SUB) {
                intOp = INT_SET_VARIABLE_SUBTRACT;
            } else if(which == ELEM_MUL) {
                intOp = INT_SET_VARIABLE_MULTIPLY;
            } else if(which == ELEM_DIV) {
                intOp = INT_SET_VARIABLE_DIVIDE;
            } else oops();

            Op(intOp, l->d.math.dest, op1, op2, 0, 0);

            Op(INT_END_IF);
            break;
        }
        case ELEM_MASTER_RELAY:
            // Tricky: must set the master control relay if we reach this
            // instruction while the master control relay is cleared, because
            // otherwise there is no good way for it to ever become set
            // again.
            Op(INT_IF_BIT_CLEAR, "$mcr");
            Op(INT_SET_BIT, "$mcr");
            Op(INT_ELSE);
            Op(INT_COPY_BIT_TO_BIT, "$mcr", stateInOut);
            Op(INT_END_IF);
            break;

        case ELEM_SET_BIT: {
            Op(INT_IF_BIT_SET, stateInOut);
            if(l->d.setBit.set) {
                OpBit(INT_SET_SINGLE_BIT, l->d.setBit.name, l->d.setBit.bit);
            } else {
				OpBit(INT_CLEAR_SINGLE_BIT, l->d.setBit.name, l->d.setBit.bit);
            }
            Op(INT_END_IF);
            break;
        }
        case ELEM_CHECK_BIT: {
			Op(INT_IF_BIT_SET, stateInOut);
            if(l->d.checkBit.set) {
                OpBit(INT_IF_BIT_CHECK_CLEAR, l->d.checkBit.name, l->d.checkBit.bit);
            } else {
                OpBit(INT_IF_BIT_CHECK_SET, l->d.checkBit.name, l->d.checkBit.bit);
            }
			OpBit(INT_CLEAR_BIT, stateInOut, l->d.checkBit.set);
            Op(INT_END_IF);
			Op(INT_END_IF);
            break;
        }
        case ELEM_SHIFT_REGISTER: {
            char storeName[MAX_NAME_LEN];
            GenSymOneShot(storeName);
            Op(INT_IF_BIT_SET, stateInOut);
                Op(INT_IF_BIT_CLEAR, storeName);
                    int i;
                    for(i = (l->d.shiftRegister.stages-2); i >= 0; i--) {
                        char dest[MAX_NAME_LEN+10], src[MAX_NAME_LEN+10];
                        sprintf(src, "%s%d", l->d.shiftRegister.name, i);
                        sprintf(dest, "%s%d", l->d.shiftRegister.name, i+1);
                        Op(INT_SET_VARIABLE_TO_VARIABLE, dest, src);
                    }
                Op(INT_END_IF);
            Op(INT_END_IF);
            Op(INT_COPY_BIT_TO_BIT, storeName, stateInOut);
            break;
        }
        case ELEM_LOOK_UP_TABLE: {
            // God this is stupid; but it will have to do, at least until I
            // add new int code instructions for this.
            int i;
            Op(INT_IF_BIT_SET, stateInOut);
            ElemLookUpTable *t = &(l->d.lookUpTable);
            for(i = 0; i < t->count; i++) {
                Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", i);
                Op(INT_IF_VARIABLE_EQUALS_VARIABLE, t->index, "$scratch");
                    Op(INT_SET_VARIABLE_TO_LITERAL, t->dest, t->vals[i]);
                Op(INT_END_IF);
            }
            Op(INT_END_IF);
            break;
        }
        case ELEM_PIECEWISE_LINEAR: {
            // This one is not so obvious; we have to decide how best to
            // perform the linear interpolation, using our 16-bit fixed
            // point math.
            ElemPiecewiseLinear *t = &(l->d.piecewiseLinear);
            if(t->count == 0) {
                Error(_("Piecewise linear lookup table with zero elements!"));
                CompileError();
            }
            int i;
            int xThis = t->vals[0];
            for(i = 1; i < t->count; i++) {
                if(t->vals[i*2] <= xThis) {
                    Error(_("x values in piecewise linear table must be "
                        "strictly increasing."));
                    CompileError();
                }
                xThis = t->vals[i*2];
            }
            Op(INT_IF_BIT_SET, stateInOut);
            for(i = t->count - 1; i >= 1; i--) {
                Op(INT_IF_VARIABLE_LES_LITERAL, t->index, t->vals[i*2]+1);
                int thisDx = t->vals[i*2] - t->vals[(i-1)*2];
                int thisDy = t->vals[i*2 + 1] - t->vals[(i-1)*2 + 1];
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

                Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", t->vals[(i-1)*2]);
                Op(INT_SET_VARIABLE_SUBTRACT, "$scratch", t->index,
                    "$scratch", 0, 0);
                Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch2", thisDx);
                Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch3", thisDy);
                Op(INT_SET_VARIABLE_MULTIPLY, t->dest, "$scratch", "$scratch3",
                    0, 0);
                Op(INT_SET_VARIABLE_DIVIDE, t->dest, t->dest, "$scratch2", 0, 0);

                Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch",
                    t->vals[(i-1)*2 + 1]);
                Op(INT_SET_VARIABLE_ADD, t->dest, t->dest, "$scratch", 0, 0);
                Op(INT_END_IF);
            }
            Op(INT_END_IF);
            break;
        }
        case ELEM_FORMATTED_STRING: {
            // Okay, this one is terrible and ineffcient, and it's a huge pain
            // to implement, but people want it a lot. The hard part is that
            // we have to let the PLC keep cycling, of course, and also that
            // we must do the integer to ASCII conversion sensisbly, with
            // only one divide per PLC cycle.

            // This variable is basically our sequencer: it is a counter that
            // increments every time we send a character.
            char seq[MAX_NAME_LEN];
            GenSymFormattedString(seq);

            // The variable whose value we might interpolate.
            char *var = l->d.fmtdStr.var;

            // This is the state variable for our integer-to-string conversion.
            // It contains the absolute value of var, possibly with some
            // of the higher powers of ten missing.
            char convertState[MAX_NAME_LEN];
            GenSymFormattedString(convertState);

            // We might need to suppress some leading zeros.
            char isLeadingZero[MAX_NAME_LEN];
            GenSymFormattedString(isLeadingZero);

            // This is a table of characters to transmit, as a function of the
            // sequencer position (though we might have a hole in the middle
            // for the variable output)
            char outputChars[MAX_LOOK_UP_TABLE_LEN];

            BOOL mustDoMinus = FALSE;

            // The total number of characters that we transmit, including
            // those from the interpolated variable.
            int steps;

            // The total number of digits to convert.
            int digits = -1;

            // So count that now, and build up our table of fixed things to
            // send.
            steps = 0;
            char *p = l->d.fmtdStr.string;
            while(*p) {
                if(*p == '\\' && (isdigit(p[1]) || p[1] == '-')) {
                    if(digits >= 0) {
                        Error(_("Multiple escapes (\\0-9) present in format "
                            "string, not allowed."));
                        CompileError();
                    }
                    p++;
                    if(*p == '-') {
                        mustDoMinus = TRUE;
                        outputChars[steps++] = 1;
                        p++;
                    }
                    if(!isdigit(*p) || (*p - '0') > 5 || *p == '0') {
                        Error(_("Bad escape sequence following \\; for a "
                            "literal backslash, use \\\\"));
                        CompileError();
                    }
                    digits = (*p - '0');
                    int i;
                    for(i = 0; i < digits; i++) {
                        outputChars[steps++] = 0;
                    }
                } else if(*p == '\\') {
                    p++;
                    switch(*p) {
                        case 'r': outputChars[steps++] = '\r'; break;
                        case 'n': outputChars[steps++] = '\n'; break;
                        case 'b': outputChars[steps++] = '\b'; break;
                        case 'f': outputChars[steps++] = '\f'; break;
                        case '\\': outputChars[steps++] = '\\'; break;
                        case 'x': {
                            int h, l;
                            p++;
                            h = HexDigit(*p);
                            if(h >= 0) {
                                p++;
                                l = HexDigit(*p);
                                if(l >= 0) {
                                    outputChars[steps++] = (h << 4) | l;
                                    break;
                                }
                            }
                            Error(_("Bad escape: correct form is \\xAB."));
                            CompileError();
                            break;
                        }
                        default:
                            Error(_("Bad escape '\\%c'"), *p);
                            CompileError();
                            break;
                    }
                } else {
                    outputChars[steps++] = *p;
                }
                if(*p) p++;
            }

            if(digits >= 0 && (strlen(var) == 0)) {
                Error(_("Variable is interpolated into formatted string, but "
                    "none is specified."));
                CompileError();
            } else if(digits < 0 && (strlen(var) > 0)) {
                Error(_("No variable is interpolated into formatted string, "
                    "but a variable name is specified. Include a string like "
                    "'\\-3', or leave variable name blank."));
                CompileError();
            }

            // We want to respond to rising edges, so yes we need a one shot.
			char oneShot[MAX_NAME_LEN];
			GenSymOneShot(oneShot);
			char byPass[MAX_NAME_LEN];
			GenSymOneShot(byPass);

            // Everything that involves seqScratch is a terrible hack to
            // avoid an if statement with a big body, which is the risk
            // factor for blowing up on PIC16 page boundaries.

            char *seqScratch = "$scratch3";

            //Op(INT_SET_VARIABLE_TO_VARIABLE, seqScratch, seq);

            Op(INT_IF_BIT_SET, stateInOut);
                Op(INT_IF_BIT_CLEAR, oneShot);
                    Op(INT_SET_VARIABLE_TO_LITERAL, seq, (SWORD)0);
					Op(INT_SET_VARIABLE_TO_VARIABLE, seqScratch, seq);
					Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
                Op(INT_END_IF);
			Op(INT_ELSE);
				Op(INT_SET_VARIABLE_TO_LITERAL, seqScratch, -1);
            Op(INT_END_IF);

			// No point doing any math unless we'll get to transmit this
            // cycle, so check that first.

            Op(INT_IF_VARIABLE_LES_LITERAL, seq, steps);
            Op(INT_ELSE);
                Op(INT_SET_VARIABLE_TO_LITERAL, seqScratch, -1);
            Op(INT_END_IF);

            //Op(INT_CLEAR_BIT, "$scratch");
            //Op(INT_UART_SEND, "$scratch", "$scratch");
            //Op(INT_IF_BIT_SET, "$scratch");
            //    Op(INT_SET_VARIABLE_TO_LITERAL, seqScratch, -1);
            //Op(INT_END_IF);

            // So we transmit this cycle, so check out which character.
            int i;
            int digit = 0;
            for(i = 0; i < steps; i++) {
                if(outputChars[i] == 0) {
                    // Note gross hack to work around limit of range for
                    // AVR brne op, which is +/- 64 instructions.
                    Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", i);
                    Op(INT_CLEAR_BIT, "$scratch");
                    Op(INT_IF_VARIABLE_EQUALS_VARIABLE, "$scratch", seqScratch);
                        Op(INT_SET_BIT, "$scratch");
                    Op(INT_END_IF);

                    Op(INT_IF_BIT_SET, "$scratch");

                    // Start the integer-to-string

                    // If there's no minus, then we have to load up
                    // convertState ourselves the first time.
                    if(digit == 0 && !mustDoMinus) {
                        Op(INT_SET_VARIABLE_TO_VARIABLE, convertState, var);
                    }
                    if(digit == 0) {
                        Op(INT_SET_BIT, isLeadingZero);
                    }
                    
                    Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch",
                        TenToThe((digits-digit)-1));
                    Op(INT_SET_VARIABLE_DIVIDE, "$scratch2", convertState,
                        "$scratch", 0, 0);
                    Op(INT_SET_VARIABLE_MULTIPLY, "$scratch", "$scratch",
                        "$scratch2", 0, 0);
                    Op(INT_SET_VARIABLE_SUBTRACT, convertState,
                        convertState, "$scratch", 0, 0);
                    Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", '0');
                    Op(INT_SET_VARIABLE_ADD, "$scratch2", "$scratch2",
                        "$scratch", 0, 0);

                    // Suppress all but the last leading zero.
                    if(digit != (digits - 1)) {
                        Op(INT_IF_VARIABLE_EQUALS_VARIABLE, "$scratch",
                            "$scratch2");
                            Op(INT_IF_BIT_SET, isLeadingZero);
                                Op(INT_SET_VARIABLE_TO_LITERAL,
                                    "$scratch2", ' ');
                            Op(INT_END_IF);
                        Op(INT_ELSE);
                            Op(INT_CLEAR_BIT, isLeadingZero);
                        Op(INT_END_IF);
                    }

                    Op(INT_END_IF);

                    digit++;
                } else if(outputChars[i] == 1) {
                    // do the minus; ugliness to get around the BRNE jump
                    // size limit, though
                    Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", i);
                    Op(INT_CLEAR_BIT, "$scratch");
                    Op(INT_IF_VARIABLE_EQUALS_VARIABLE, "$scratch", seqScratch);
                        Op(INT_SET_BIT, "$scratch");
                    Op(INT_END_IF);
                    Op(INT_IF_BIT_SET, "$scratch");
                       
                        // Also do the `absolute value' calculation while
                        // we're at it.
                        Op(INT_SET_VARIABLE_TO_VARIABLE, convertState, var);
                        Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch2", ' ');
                        Op(INT_IF_VARIABLE_LES_LITERAL, var, (SWORD)0);
                            Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch2", '-');
                            Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", (SWORD)0);
                            Op(INT_SET_VARIABLE_SUBTRACT, convertState, "$scratch", var, 0, 0);
                        Op(INT_END_IF);

                    Op(INT_END_IF);
                } else {
                    // just another character
                    Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch", i);
                    Op(INT_IF_VARIABLE_EQUALS_VARIABLE, "$scratch", seqScratch);
                        Op(INT_SET_VARIABLE_TO_LITERAL, "$scratch2", outputChars[i]);
                    Op(INT_END_IF);
                }
            }

			// OneShot 
			Op(INT_IF_BIT_SET, stateInOut);
				Op(INT_IF_BIT_CLEAR, oneShot);
					Op(INT_IF_BIT_SET, "$SerialReady");
		                Op(INT_UART_SEND, "$scratch2", "$scratch");
						Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
						Op(INT_INCREMENT_VARIABLE, seq);
					Op(INT_END_IF);
					Op(INT_CLEAR_BIT, stateInOut);
					Op(INT_COPY_BIT_TO_BIT, byPass, stateInOut);
				Op(INT_END_IF);
				Op(INT_IF_BIT_CLEAR, byPass);
					Op(INT_IF_BIT_SET, "$SerialReady");
						Op(INT_SET_BIT, byPass);
					Op(INT_ELSE);
						Op(INT_CLEAR_BIT, stateInOut);
					Op(INT_END_IF);
				Op(INT_END_IF);
			Op(INT_ELSE);
				Op(INT_COPY_BIT_TO_BIT, oneShot, stateInOut);
			Op(INT_END_IF);

            /*Op(INT_IF_VARIABLE_LES_LITERAL, seqScratch, (SWORD)0);
            Op(INT_ELSE);
                Op(INT_SET_BIT, "$scratch");
                Op(INT_UART_SEND, "$scratch2", "$scratch");
                Op(INT_INCREMENT_VARIABLE, seq);
            Op(INT_END_IF);*/
    
            // Rung-out state: true if we're still running, else false
            /*Op(INT_IF_BIT_SET, stateInOut);
				Op(INT_IF_VARIABLE_LES_LITERAL, seq, steps);
					Op(INT_CLEAR_BIT, stateInOut);
				Op(INT_ELSE);
					Op(INT_CLEAR_BIT, oneShot);
				Op(INT_END_IF);
			Op(INT_END_IF);*/
            break;
        }
        case ELEM_OPEN:
            Op(INT_CLEAR_BIT, stateInOut);
            break;

        case ELEM_SHORT:
            // goes straight through
            break;

        case ELEM_PLACEHOLDER:
            Error(
              _("Empty row; delete it or add instructions before compiling."));
            CompileError();
            break;

        default:
            oops();
            break;
    }

    if(which != ELEM_SERIES_SUBCKT && which != ELEM_PARALLEL_SUBCKT) {
        // then it is a leaf; let the simulator know which leaf it
        // should be updating for display purposes
        SimState(&(l->poweredAfter), stateInOut);
    }
}

//-----------------------------------------------------------------------------
// Generate intermediate code for the entire program. Return TRUE if it worked,
// else FALSE.
//-----------------------------------------------------------------------------
BOOL GenerateIntermediateCode(void)
{
    GenSymCountParThis = 0;
    GenSymCountParOut = 0;
    GenSymCountOneShot = 0;
    GenSymCountFormattedString = 0;

    // The EEPROM addresses for the `Make Persistent' op are assigned at
    // int code generation time.
    EepromAddrFree = 0;
    
    IntCodeLen = 0;
    memset(IntCode, 0, sizeof(IntCode));

    if(setjmp(CompileErrorBuf) != 0) {
        return FALSE;
    }

    Op(INT_SET_BIT, "$mcr");

    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        if(Prog.rungs[i]->count == 1 && 
            Prog.rungs[i]->contents[0].which == ELEM_COMMENT)
        {
            // nothing to do for this one
            continue;
        }
        Comment("");
        Comment(_("start rung %d"), i+1);
		GenSymCountParThis = 0;
		GenSymCountParOut  = 0;
        Op(INT_COPY_BIT_TO_BIT, "$rung_top", "$mcr");
        SimState(&(Prog.rungPowered[i]), "$rung_top");
        IntCodeFromCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i], "$rung_top");
    }

    return TRUE;
}
