#include "poptools.h"

// If we encounter an error while compiling then it's convenient to break
// out of the possibly-deeply-recursed function we're in.
jmp_buf CompileErrorBuf;

#include <sstream>

using namespace std;

IntCode::IntCode(void)
{
	Clear();
}

void IntCode::ClearParallelCount(void)
{
	GenSymCountParThis = 0;
	GenSymCountParOut  = 0;
}

int IntCode::getEepromAddr(void)
{
	if(EepromAddrFree > EEPROM_SIZE) return -1;

	EepromAddrFree += 2;
	return EepromAddrFree - 2;
}

void IntCode::Clear(void)
{
	ClearParallelCount();

	GenSymCountOneShot         = 0;
	GenSymCountFormattedString = 0;

    // The EEPROM addresses for the `Make Persistent' op are assigned at
    // int code generation time.
	EepromAddrFree             = 0;

	arrayIntCode.clear();

	StateInOut = "";
}

const char *IntCode::getStateInOut(void)
{
	return StateInOut.c_str();
}

void  IntCode::setStateInOut(string s)
{
	StateInOut = s;
}

vector<IntOp> IntCode::getVectorIntCode(void)
{
	return arrayIntCode;
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
string IntCode::GenSymParThis(void)
{
	char buf[100];
	sprintf(buf, "$parThis_%d", GenSymCountParThis++);
	return string(buf);
//	ostringstream oss;
//    oss << "$parThis_" << GenSymCountParOut++;
//	return oss.str();
}

string IntCode::GenSymParOut(void)
{
	char buf[100];
	sprintf(buf, "$parOut_%d", GenSymCountParOut++);
	return string(buf);
//	ostringstream oss;
//    oss << "$parOut_" << GenSymCountParOut++;
//	return oss.str();
}

string IntCode::GenSymOneShot(void)
{
	char buf[100];
	sprintf(buf, "$oneShot_%d", GenSymCountOneShot++);
	return string(buf);
//	ostringstream oss;
//    oss << "$oneShot_" << GenSymCountParOut++;
//	return oss.str();
}

string IntCode::GenSymFormattedString(void)
{
	char buf[100];
	sprintf(buf, "$formattedString_%d", GenSymCountFormattedString++);
	return string(buf);
//	ostringstream oss;
//    oss << "formattedString_" << GenSymCountParOut++;
//	return oss.str();
}

//-----------------------------------------------------------------------------
// Compile an instruction to the program.
//-----------------------------------------------------------------------------
void IntCode::Op(int op, const char *name1, const char *name2, const char *name3, const char *name4, const char *name5, const char *name6, const char *name7, SWORD lit, unsigned char bit)
{
	IntOp iop;
	memset(&iop, 0, sizeof(iop));

    iop.op = op;

	if(name1) strcpy(iop.name1, name1);
    if(name2) strcpy(iop.name2, name2);
    if(name3) strcpy(iop.name3, name3);
    if(name4) strcpy(iop.name4, name4);
    if(name5) strcpy(iop.name5, name5);
    if(name6) strcpy(iop.name6, name6);
	if(name7) strcpy(iop.name7, name7);

	iop.literal = lit;
	iop.bit = bit;

	switch (op)
	{
	case INT_SET_BIT:
		strcpy(iop.desc, "SET_BIT");
		break;
	case INT_CHECK_BIT:
		strcpy(iop.desc, "CHECK_BIT");
		break;
	case INT_CLEAR_BIT:
		strcpy(iop.desc, "CLEAR_BIT");
		break;
	case INT_COPY_BIT_TO_BIT:
		strcpy(iop.desc, "COPY_BIT_TO_BIT");
		break;
	case INT_SET_VARIABLE_TO_LITERAL:
		strcpy(iop.desc, "SET_VARIABLE_TO_LITERAL");
		break;
	case INT_SET_VARIABLE_TO_VARIABLE:
		strcpy(iop.desc, "SET_VARIABLE_TO_VARIABLE");
		break;
	case INT_INCREMENT_VARIABLE:
		strcpy(iop.desc, "INCREMENT_VARIABLE");
		break;
	case INT_SET_VARIABLE_ADD:
		strcpy(iop.desc, "SET_VARIABLE_ADD");
		break;
	case INT_SET_VARIABLE_SUBTRACT:
		strcpy(iop.desc, "SET_VARIABLE_SUBTRACT");
		break;
	case INT_SET_VARIABLE_MULTIPLY:
		strcpy(iop.desc, "SET_VARIABLE_MULTIPLY");
		break;
	case INT_SET_VARIABLE_DIVIDE:
		strcpy(iop.desc, "SET_VARIABLE_DIVIDE");
		break;
	case INT_SET_VARIABLE_MODULO:
		strcpy(iop.desc, "SET_VARIABLE_MODULO");
		break;
	case INT_CREATE_STATIC_VARIABLE:
		strcpy(iop.desc, "CREATE_STATIC_VARIABLE");
		break;
	case INT_READ_ADC:
		strcpy(iop.desc, "READ_ADC");
		break;
	case INT_SET_PWM:
		strcpy(iop.desc, "SET_PWM");
		break;
	case INT_UART_SEND:
		strcpy(iop.desc, "UART_SEND");
		break;
	case INT_UART_RECV:
		strcpy(iop.desc, "UART_RECV");
		break;
	case INT_EEPROM_BUSY_CHECK:
		strcpy(iop.desc, "EEPROM_BUSY_CHECK");
		break;
	case INT_EEPROM_READ:
		strcpy(iop.desc, "EEPROM_READ");
		break;
	case INT_EEPROM_WRITE:
		strcpy(iop.desc, "EEPROM_WRITE");
		break;
	case INT_READ_FORMATTED_STRING:
		strcpy(iop.desc, "FORMATTED_STRING_READ");
		break;
	case INT_WRITE_FORMATTED_STRING:
		strcpy(iop.desc, "FORMATTED_STRING_WRITE");
		break;
	case INT_READ_SERVO_YASKAWA:
		strcpy(iop.desc, "FORMATTED_SERVO_YASKAWA_READ");
		break;
	case INT_WRITE_SERVO_YASKAWA:
		strcpy(iop.desc, "FORMATTED_SERVO_YASKAWA_WRITE");
		break;
	case INT_READ_ENC:
		strcpy(iop.desc, "READ_ENC");
		break;
	case INT_RESET_ENC:
		strcpy(iop.desc, "RESET_ENC");
		break;
	case INT_READ_USS:
		strcpy(iop.desc, "READ_USS");
		break;
	case INT_WRITE_USS:
		strcpy(iop.desc, "WRITE_USS");
		break;
	/*case INT_IF_GROUP(x) (((x) >= 50) && ((x) < 60))
		strcpy(IntCode[IntCodeLen].desc, "???");
		break;*/
	case INT_IF_BIT_SET:
		strcpy(iop.desc, "IF_BIT_SET");
		break;
	case INT_IF_BIT_CLEAR:
		strcpy(iop.desc, "IF_BIT_CLEAR");
		break;
	case INT_IF_VARIABLE_LES_LITERAL:
		strcpy(iop.desc, "IF_VARIABLE_LES_LITERAL");
		break;
	case INT_IF_VARIABLE_EQUALS_VARIABLE:
		strcpy(iop.desc, "IF_VARIABLE_EQUALS_VARIABLE");
		break;
	case INT_IF_VARIABLE_GRT_VARIABLE:
		strcpy(iop.desc, "IF_VARIABLE_GRT_VARIABLE");
		break;
	case INT_ELSE:
		strcpy(iop.desc, "ELSE");
		break;
	case INT_ELSE_IF:
		strcpy(iop.desc, "ELSE IF");
		break;
	case INT_END_IF:
		strcpy(iop.desc, "END_IF");
		break;
	case INT_SIMULATE_NODE_STATE:
		strcpy(iop.desc, "SIMULATE_NODE_STATE");
		break;
	case INT_COMMENT:
		strcpy(iop.desc, "COMMENT");
		break;
	// Only used for the interpretable code.
	//#define INT_END_OF_PROGRAM                     255
	default:
		strcpy(iop.desc, "???");
	}

	arrayIntCode.push_back(iop);
}

//-----------------------------------------------------------------------------
// Compile the instruction that the simulator uses to keep track of which
// nodes are energized (so that it can display which branches of the circuit
// are energized onscreen). The MCU code generators ignore this, of course.
//-----------------------------------------------------------------------------
void IntCode::SimState(bool *b, const char *name)
{
	IntOp iop;

	iop.op = INT_SIMULATE_NODE_STATE;
    strcpy(iop.name1, name);
    iop.poweredAfter = b;

	arrayIntCode.push_back(iop);
}

//-----------------------------------------------------------------------------
// printf-like comment function
//-----------------------------------------------------------------------------
void IntCode::Comment(const char *str, ...)
{
    va_list f;
    char buf[MAX_NAME_LEN];
    va_start(f, str);
    vsprintf(buf, str, f);
    Op(INT_COMMENT, buf);
}

//-----------------------------------------------------------------------------
// Try to turn a string into a 16-bit constant, and raise an error if
// something bad happens when we do so (e.g. out of range).
//-----------------------------------------------------------------------------
SWORD CheckMakeNumber(const char *str)
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
const char *IntCode::VarFromExpr(const char *expr, char *tempName)
{
    if(IsNumber(expr)) {
        Op(INT_SET_VARIABLE_TO_LITERAL, tempName, CheckMakeNumber(expr));
        return tempName;
    } else {
        return expr;
    }
}

//-----------------------------------------------------------------------------
// Generate intermediate code for the entire program. Return TRUE if it worked,
// else FALSE.
//-----------------------------------------------------------------------------
BOOL GenerateIntermediateCode(void)
{
    if(setjmp(CompileErrorBuf) != 0) {
        return FALSE;
    }

	ladder->GenerateIntCode();

    return TRUE;
}
