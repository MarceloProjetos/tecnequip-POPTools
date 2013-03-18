#include "poptools.h"

#define SEENVAR_LISTS 4

// Lists indexes
#define SEENVAR_MODE_USERBIT 0
#define SEENVAR_MODE_SYSTEM  1
#define SEENVAR_MODE_USERINT 2
#define SEENVAR_MODE_OTHERS  3

// There are 4 lists: bit, system, int and others.
static char SeenVariables[SEENVAR_LISTS][MAX_IO][MAX_NAME_LEN];
static int SeenVariablesCount[SEENVAR_LISTS];
static char *NameVarArray[] = { "ArrayBitUser", "ArrayBitSystem", "ArrayIntUser" };
static int MODBUS_RS485_MASTER = 0, MODBUS_TCP_MASTER = 0;

// Control variables
int HasPWM = 0;

//#define INT_UNSIGNED	

//-----------------------------------------------------------------------------
// Have we seen a variable before? If not then no need to generate code for
// it, otherwise we will have to make a declaration, and mark it as seen.
//-----------------------------------------------------------------------------
static BOOL SeenVariable(char *name, int mode)
{
    int i, index;
	bool found = false;

	for(index = 0; index < SEENVAR_LISTS && !found; index++) {
		for(i = 0; i < SeenVariablesCount[index]; i++) {
			if(_stricmp(SeenVariables[index][i], name)==0) {
				found = true;
				break;
			}
		}
    }

	if(found) {
		// We have to decrement the index because it was incremented after the item has been found.
		index--;

		// If it was declared as bit but now it is being used as integer, we have to change it.
		if(index == SEENVAR_MODE_USERBIT && mode == SEENVAR_MODE_USERINT) {
			// Move it to USERINT list
			strcpy(SeenVariables[SEENVAR_MODE_USERINT][SeenVariablesCount[SEENVAR_MODE_USERINT]], SeenVariables[SEENVAR_MODE_USERBIT][i]);

			// Move all items after the changed item one position back
			memcpy((char *)SeenVariables[SEENVAR_MODE_USERBIT][i], (char *)SeenVariables[SEENVAR_MODE_USERBIT][i+1], SeenVariablesCount[SEENVAR_MODE_USERBIT] - i);

			// Update counters
			SeenVariablesCount[SEENVAR_MODE_USERINT]++;
			SeenVariablesCount[SEENVAR_MODE_USERBIT]--;
		}

		return TRUE;
	} else {
		if(i >= MAX_IO) oops();

		strcpy(SeenVariables[mode][SeenVariablesCount[mode]], name);
		SeenVariablesCount[mode]++;

		return FALSE;
	}
}

//-----------------------------------------------------------------------------
// Turn an internal symbol into a C name; only trick is that internal symbols
// use $ for symbols that the int code generator needed for itself, so map
// that into something okay for C.
//-----------------------------------------------------------------------------
static char *MapSym(char *str)
{
    static char AllRets[4][MAX_NAME_LEN+30];
    static int RetCnt;

    RetCnt = (RetCnt + 1) & 3;

    char *ret = AllRets[RetCnt];
    
	int i;
    for(i = 0; i < Prog.io.count; i++) 
	{
		if (_stricmp(Prog.io.assignment[i].name, str) == 0)
		{
			int pin = Prog.io.assignment[i].pin;

			if (Prog.io.assignment[i].type == IO_TYPE_DIG_OUTPUT) 
			{
				if (pin > 18)
				{
					sprintf(ret, "MODBUS_REGISTER[%d]", pin - 20);
					return ret;
				}
				else
				{
					sprintf(ret, "GPIO_OUTPUT_PORT%d", pin);
					return ret;
				}
			} 
			if (Prog.io.assignment[i].type == IO_TYPE_READ_ADC) 
			{
				sprintf(ret, "A%d", pin);
				return ret;
			} 
			if (Prog.io.assignment[i].type == IO_TYPE_GENERAL && Prog.io.assignment[i].pin) 
			{
				sprintf(ret, "MODBUS_REGISTER[%d]", Prog.io.assignment[i].pin - 20);
				return ret;
			} 
			if (Prog.io.assignment[i].type == IO_TYPE_DIG_INPUT) 
			{
				if (pin > 19)
				{
					sprintf(ret, "MODBUS_REGISTER[%d]", pin - 20);
					return ret;
				}
				else
				{
					sprintf(ret, "GPIO_INPUT_PORT%d", pin);
					return ret;
				}
			}
		}
	}

	if (strlen(str) == 0)
		sprintf(ret, "VAR_NULL");
    else if(*str == '$') {
        sprintf(ret, "I_%s", str+1);
    } else {
        sprintf(ret, "U_%s", str);
    }
    return ret;
}

//-----------------------------------------------------------------------------
// Generate a declaration for an integer var; easy, a static 32-bit qty.
//-----------------------------------------------------------------------------
static void DeclareInt(FILE *f, char *rawstr)
{
	int mode;
	char *str = MapSym(rawstr);
	if (isdigit(*rawstr) || *rawstr == '-')
	{
		return;
	}

	// The following line is really necessary? Apparently not, but ...
	if (_stricmp(rawstr, "I_SerialReady") == 0) return;

	mode = (!strncmp(str, "U_", 2) || !strncmp(str, "I_", 2)) ? SEENVAR_MODE_USERINT : SEENVAR_MODE_OTHERS;
	if(!SeenVariable(str, mode) && mode == SEENVAR_MODE_OTHERS && strncmp(str, "M", 1)) {
#ifdef INT_UNSIGNED
		fprintf(f, "volatile unsigned int %s = 0;\n", str);
#else
		fprintf(f, "volatile int %s = 0;\n", str);
#endif
	}
}

//-----------------------------------------------------------------------------
// Generate a declaration for a bit var; three cases, input, output, and
// internal relay. An internal relay is just a BOOL variable, but for an
// input or an output someone else must provide read/write functions.
//-----------------------------------------------------------------------------
static void DeclareBit(FILE *f, char *rawStr)
{
	int mode, intflag;
    char *str = MapSym(rawStr);
	if (!strncmp(str, "M", 1)) {
		return;
	}

	intflag = (!strncmp(str, "I_", 2) && IsInternalFlag(str+2));

	mode = (*rawStr == '$'  && !intflag) ? SEENVAR_MODE_SYSTEM : (!strncmp(str, "U_", 2) ? SEENVAR_MODE_USERBIT : SEENVAR_MODE_OTHERS);
	if(!SeenVariable(str, mode) && mode == SEENVAR_MODE_OTHERS && !intflag) {
#ifdef INT_UNSIGNED
		fprintf(f, "volatile unsigned int %s = 0;\n", str);
#else
		fprintf(f, "volatile unsigned char %s = 0;\n", str);
#endif
	}
}

//-----------------------------------------------------------------------------
// Generate declarations for all the 16-bit/single bit variables in the ladder
// program.
//-----------------------------------------------------------------------------
static void GenerateDeclarations(FILE *f)
{
    int i;
    for(i = 0; i < IntCodeLen; i++) {
        char *bitVar1 = NULL, *bitVar2 = NULL;
        char *intVar1 = NULL, *intVar2 = NULL, *intVar3 = NULL;

        switch(IntCode[i].op) {
            case INT_SET_BIT:
            case INT_CLEAR_BIT:
                bitVar1 = IntCode[i].name1;
                break;

			case INT_SET_SINGLE_BIT:
			case INT_CLEAR_SINGLE_BIT:
                intVar1 = IntCode[i].name1;
                break;

            case INT_COPY_BIT_TO_BIT:
                bitVar1 = IntCode[i].name1;
                bitVar2 = IntCode[i].name2;
                break;

            case INT_SET_VARIABLE_TO_LITERAL:
                intVar1 = IntCode[i].name1;
                break;

            case INT_SET_VARIABLE_TO_VARIABLE:
                intVar1 = IntCode[i].name1;
                intVar2 = IntCode[i].name2;
                break;

            case INT_SET_VARIABLE_DIVIDE:
            case INT_SET_VARIABLE_MULTIPLY:
            case INT_SET_VARIABLE_SUBTRACT:
            case INT_SET_VARIABLE_ADD:
                intVar1 = IntCode[i].name1;
                intVar2 = IntCode[i].name2;
                intVar3 = IntCode[i].name3;
                break;

            case INT_INCREMENT_VARIABLE:
            case INT_READ_ADC:
            case INT_SET_DA:
            case INT_READ_ENC:
            case INT_RESET_ENC:
			case INT_READ_FORMATTED_STRING:
			case INT_WRITE_FORMATTED_STRING:
			case INT_READ_SERVO_YASKAWA:
			case INT_WRITE_SERVO_YASKAWA:
            case INT_READ_USS:
            case INT_WRITE_USS:
            case INT_SET_PWM:
                intVar1 = IntCode[i].name1;
                break;

            case INT_READ_MODBUS:
            case INT_WRITE_MODBUS:
                intVar1 = IntCode[i].name1;
                break;

            case INT_UART_RECV:
            case INT_UART_SEND:
                intVar1 = IntCode[i].name1;
                bitVar1 = IntCode[i].name2;
                break;

            case INT_IF_BIT_SET:
            case INT_IF_BIT_CLEAR:
                bitVar1 = IntCode[i].name1;
                break;

			case INT_IF_BIT_CHECK_SET:
			case INT_IF_BIT_CHECK_CLEAR:
                intVar1 = IntCode[i].name1;
                break;

            case INT_IF_VARIABLE_LES_LITERAL:
                intVar1 = IntCode[i].name1;
                break;

            case INT_IF_VARIABLE_EQUALS_VARIABLE:
            case INT_IF_VARIABLE_GRT_VARIABLE:
                intVar1 = IntCode[i].name1;
                intVar2 = IntCode[i].name2;
                break;

			case INT_MULTISET_DA:
				if(strlen(IntCode[i].name1)) {
					intVar1 = IntCode[i].name1;
					intVar2 = IntCode[i].name2;
				}
                break;

            case INT_EEPROM_READ:
            case INT_EEPROM_WRITE:
                intVar1 = IntCode[i].name1;
                break;

            case INT_RAND:
                intVar1 = IntCode[i].name1;
                intVar2 = IntCode[i].name2;
                intVar3 = IntCode[i].name3;
                break;

			case INT_SQRT:
            case INT_END_IF:
            case INT_ELSE_IF:
            case INT_ELSE:
            case INT_COMMENT:
            case INT_SIMULATE_NODE_STATE:
            case INT_EEPROM_BUSY_CHECK:
			case INT_SET_RTC:
			case INT_CHECK_RTC:
                break;

            default:
                oops();
        }
        if(bitVar1) DeclareBit(f, bitVar1);
        if(bitVar2) DeclareBit(f, bitVar2);

        if(intVar1) DeclareInt(f, intVar1);
        if(intVar2) DeclareInt(f, intVar2);
        if(intVar3) DeclareInt(f, intVar3);
    }

	for(i=0; i != SEENVAR_MODE_OTHERS; i++) {
		int count = SeenVariablesCount[i];

		if(i != SEENVAR_MODE_USERINT) {
			count = count/32 + (count%32 > 0);
		}

		fprintf(f, "volatile int %s_Count = %d;\n", NameVarArray[i], count);
		fprintf(f, "volatile int %s[%d];\n", NameVarArray[i], count ? count : 1);
	}
}

#define GENVARCODE_MODE_READ  0
#define GENVARCODE_MODE_WRITE 1

char *GenVarCode(char *dest, char *name, char *val, int mode)
{
	int list, i, bit;
	char buf[MAX_NAME_LEN], true_str[MAX_NAME_LEN], false_str[MAX_NAME_LEN];

	for(list = 0; list < SEENVAR_LISTS; list++) {
		for(i = 0; i < SeenVariablesCount[list]; i++) {
			if(!strcmp(name, SeenVariables[list][i])) {
				switch(list) {
					case SEENVAR_MODE_USERBIT:
					case SEENVAR_MODE_SYSTEM: {
						bit = i%32;
						i /= 32;
						sprintf(buf, "%s[%d]", NameVarArray[list], i);

						if(mode == GENVARCODE_MODE_READ) {
							sprintf(dest, "((%s >> %d) & 1)", buf, bit);
						} else {
							sprintf(true_str , "%s |= 1UL << %d;"   , buf, bit);
							sprintf(false_str, "%s &= ~(1UL << %d);", buf, bit);
							if(!strcmp(val, "0")) {
								strcpy(dest, false_str);
							} else if(!strcmp(val, "1")) {
								strcpy(dest, true_str);
							} else {
								sprintf(dest, "if(%s) %s else %s", val, true_str, false_str);
							}
						}

						break;
					}

					case SEENVAR_MODE_USERINT:
						if(mode == GENVARCODE_MODE_READ) {
							sprintf(dest, "%s[%d]", NameVarArray[list], i);
						} else {
							sprintf(dest, "%s[%d] = %s;", NameVarArray[list], i, val);
						}

						break;

					case SEENVAR_MODE_OTHERS:
						if(mode == GENVARCODE_MODE_READ) {
							sprintf(dest, "%s", SeenVariables[list][i]);
						} else {
							sprintf(dest, "%s = %s;", SeenVariables[list][i], val);
						}

						break;
				}

				return dest;
			}
		}
	}

	oops();
	return NULL;
}

//-----------------------------------------------------------------------------
// Actually generate the C source for the program.
//-----------------------------------------------------------------------------
static void GenerateAnsiC(FILE *f, unsigned int &ad_mask)
{
    int i;
    int indent = 1;
	char buf[1024], buf2[1024];

	ad_mask = 0;
	
	for(i = 0; i < IntCodeLen; i++) {

        if(IntCode[i].op == INT_ELSE_IF) indent--;
        if(IntCode[i].op == INT_END_IF) indent--;
        if(IntCode[i].op == INT_ELSE) indent--;

		if(!i || IntCode[i-1].op != INT_ELSE_IF) {
	        int j;
		    for(j = 0; j < indent; j++) fprintf(f, "    ");
		}

		char *str = MapSym(IntCode[i].name1);

        switch(IntCode[i].op) {
            case INT_SET_BIT:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
					fprintf(f, "%s |= (1 << %d);  // %s = 1\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "%s // %s = 1\n", GenVarCode(buf, MapSym(IntCode[i].name1), "1", GENVARCODE_MODE_WRITE), IntCode[i].name1);
                break;

            case INT_CLEAR_BIT:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
					fprintf(f, "%s &= ~(1 << %d);  // %s = 0\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
	                fprintf(f, "%s // %s = 0\n", GenVarCode(buf, MapSym(IntCode[i].name1), "0", GENVARCODE_MODE_WRITE), IntCode[i].name1);
                break;

			case INT_SET_SINGLE_BIT:
                fprintf(f, "%s |= 1 << %d; // %s.%d\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].bit, IntCode[i].name1, IntCode[i].bit);
                break;

            case INT_CLEAR_SINGLE_BIT:
                fprintf(f, "%s &= ~(1 << %d); // %s.%d\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].bit, IntCode[i].name1, IntCode[i].bit);
                break;

            case INT_COPY_BIT_TO_BIT:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
	                fprintf(f, "%s &= ~(1 << %d); %s |= %s << %d;  // %s = %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, MapSym(IntCode[i].name1), GenVarCode(buf2, MapSym(IntCode[i].name2), NULL, GENVARCODE_MODE_READ), IntCode[i].bit, IntCode[i].name1, IntCode[i].name2);
				else
					fprintf(f, "%s // %s = %s\n", GenVarCode(buf, MapSym(IntCode[i].name1),  GenVarCode(buf2, MapSym(IntCode[i].name2), NULL, GENVARCODE_MODE_READ), GENVARCODE_MODE_WRITE), IntCode[i].name1, IntCode[i].name2);
                break;

            case INT_SET_VARIABLE_TO_LITERAL:
				sprintf(buf2, "%d", IntCode[i].literal);
                fprintf(f, "%s // %s\n", GenVarCode(buf, MapSym(IntCode[i].name1), buf2, GENVARCODE_MODE_WRITE), IntCode[i].name1);
                break;

            case INT_SET_VARIABLE_TO_VARIABLE:
                fprintf(f, "%s // %s = %s\n", GenVarCode(buf, MapSym(IntCode[i].name1), GenVarCode(buf2, MapSym(IntCode[i].name2), NULL, GENVARCODE_MODE_READ), GENVARCODE_MODE_WRITE),
                    IntCode[i].name1, IntCode[i].name2);
                break;

            {
                char op;
                case INT_SET_VARIABLE_ADD: op = '+'; goto arith;
                case INT_SET_VARIABLE_SUBTRACT: op = '-'; goto arith;
                case INT_SET_VARIABLE_MULTIPLY: op = '*'; goto arith;
                case INT_SET_VARIABLE_DIVIDE: op = '/'; fprintf(f, "if(%s) ", GenVarCode(buf , MapSym(IntCode[i].name3), NULL, GENVARCODE_MODE_READ)); goto arith;
                arith:
					char mathstr[1024];
					sprintf(mathstr, "%s %c %s",
						GenVarCode(buf , MapSym(IntCode[i].name2), NULL, GENVARCODE_MODE_READ),
						op,
						GenVarCode(buf2, MapSym(IntCode[i].name3), NULL, GENVARCODE_MODE_READ));
					fprintf(f, "%s // %s = %s %c %s\n",
						GenVarCode(buf, MapSym(IntCode[i].name1), mathstr, GENVARCODE_MODE_WRITE),
                        IntCode[i].name1,
                        IntCode[i].name2,
                        op,
                        IntCode[i].name3);
                    break;
            }

            case INT_INCREMENT_VARIABLE:
                fprintf(f, "%s++;\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
                break;

            case INT_IF_BIT_SET:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
	                fprintf(f, "if (%s & (1 << %d)) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "if (%s) {  // %s\n", GenVarCode(buf , MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CLEAR:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
	                fprintf(f, "if (!(%s & (1 << %d))) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "if (!%s) {  // %s\n", GenVarCode(buf , MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CHECK_SET:
				fprintf(f, "if (%s & (1 << %d)) {  // %s\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].bit, IntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CHECK_CLEAR:
				fprintf(f, "if (!(%s & (1 << %d))) {  // %s\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].bit, IntCode[i].name1);
                indent++;
                break;

            case INT_IF_VARIABLE_LES_LITERAL:
                fprintf(f, "if (%s < %d) {\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), 
                    IntCode[i].literal);
                indent++;
                break;

            case INT_IF_VARIABLE_EQUALS_VARIABLE:
                fprintf(f, "if (%s == %s) {\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ),
                    GenVarCode(buf2, MapSym(IntCode[i].name2), NULL, GENVARCODE_MODE_READ));
                indent++;
                break;

            case INT_IF_VARIABLE_GRT_VARIABLE:
                fprintf(f, "if (%s > %s) {\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ),
                    GenVarCode(buf2, MapSym(IntCode[i].name2), NULL, GENVARCODE_MODE_READ));
                indent++;
                break;

            case INT_END_IF:
                fprintf(f, "}\n");
                break;

            case INT_ELSE:
                fprintf(f, "} else {\n"); indent++;
                break;

            case INT_ELSE_IF:
                fprintf(f, "} else ");
                break;

            case INT_SIMULATE_NODE_STATE:
                fprintf(f, "\n");
                break;

            case INT_COMMENT:
                if(IntCode[i].name1[0]) {
                    fprintf(f, "/* %s */\n", IntCode[i].name1);
                } else {
                    fprintf(f, "\n");
                }
                break;

            case INT_EEPROM_BUSY_CHECK:
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(IntCode[i].name1), "E2P_Busy()", GENVARCODE_MODE_WRITE));
				break;
            case INT_EEPROM_READ:
				sprintf(buf2, "E2P_Read((void *)&%s, %d, 2)", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].literal*2);
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(IntCode[i].name2), buf2, GENVARCODE_MODE_WRITE));
				break;
            case INT_EEPROM_WRITE:
				fprintf(f, "E2P_Write((void *)&%s, %d, 2);\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].literal);
				break;
            case INT_READ_ADC:
				ad_mask |= 1 << (atoi(MapSym(IntCode[i].name1)+1) - 1);
				sprintf(buf2, "ADC_Read(%d)", atoi(MapSym(IntCode[i].name1) + 1));
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(IntCode[i].name1), buf2, GENVARCODE_MODE_WRITE));
				break;
            case INT_SET_DA:
				fprintf(f, "DAC_Write(DAC_Conv(%s, %d));\n", IsNumber(IntCode[i].name1) ? IntCode[i].name1 : GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].literal);
				break;
            case INT_SQRT:
				fprintf(f, "srint(&%s);\n", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_RAND: {
				char *min, *max;
				char randbuf[1024];
				min = IsNumber(IntCode[i].name2) ? IntCode[i].name2 : GenVarCode(buf , MapSym(IntCode[i].name2), NULL, GENVARCODE_MODE_READ);
				max = IsNumber(IntCode[i].name3) ? IntCode[i].name3 : GenVarCode(buf2, MapSym(IntCode[i].name3), NULL, GENVARCODE_MODE_READ);
				sprintf(randbuf, "%s + (rand() %% (%s - %s + 1))", min, max, min);
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(IntCode[i].name1), randbuf, GENVARCODE_MODE_WRITE));
				break;
			}
			case INT_MULTISET_DA:
				if(strlen(IntCode[i].name1)) {
					fprintf(f, "DAC_Start(%d, %d, %s, %s, %s, DAC_Conv(%s, %s));\n", IntCode[i].bit, IntCode[i].literal, IntCode[i].name3, IntCode[i].name4,
						IsNumber(IntCode[i].name1) ? IntCode[i].name1 : GenVarCode(buf , MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ),
						IsNumber(IntCode[i].name2) ? IntCode[i].name2 : GenVarCode(buf2, MapSym(IntCode[i].name2), NULL, GENVARCODE_MODE_READ), IntCode[i].name5);
				} else {
					fprintf(f, "DAC_Abort(%d);\n", IntCode[i].literal-1);
				}
				break;

			case INT_SET_RTC: {
				char *RtcName[] = { "RTC_StartTM", "RTC_EndTM" };
				fprintf(f, "%s.tm_year = %s; %s.tm_mon = %s; %s.tm_mday = %s; %s.tm_hour = %s; %s.tm_min = %s; %s.tm_sec = %s;\n",
					RtcName[IntCode[i].bit], IntCode[i].name3, RtcName[IntCode[i].bit], IntCode[i].name2, RtcName[IntCode[i].bit], IntCode[i].name1,
					RtcName[IntCode[i].bit], IntCode[i].name4, RtcName[IntCode[i].bit], IntCode[i].name5, RtcName[IntCode[i].bit], IntCode[i].name6);
				break;
			}

			case INT_CHECK_RTC:
				{
					char *RTC_Mode;
					char rtcbuf[10*1024];
					int mode = atoi(IntCode[i].name2), j;
					int WeekDays = IntCode[i].literal & 0x7F, UseWeekDays = (IntCode[i].literal >> 7) & 1;

					if(mode == ELEM_RTC_MODE_FIXED) {
						if (UseWeekDays) {
							sprintf(rtcbuf, "(%d & (1 << RTC_NowTM.tm_wday)) && ", WeekDays);
						} else {
							sprintf(rtcbuf, "(!RTC_StartTM.tm_mday || RTC_NowTM.tm_mday == RTC_StartTM.tm_mday       ) && "
											"(!RTC_StartTM.tm_mon  || RTC_NowTM.tm_mon  == RTC_StartTM.tm_mon  -    1) && "
											"(!RTC_StartTM.tm_year || RTC_NowTM.tm_year == RTC_StartTM.tm_year - 1900) && ");
						}

						sprintf(rtcbuf, "%s(RTC_NowTM.tm_hour == RTC_StartTM.tm_hour) && "
										  "(RTC_NowTM.tm_min  == RTC_StartTM.tm_min ) && "
										  "(RTC_NowTM.tm_sec  == RTC_StartTM.tm_sec )", rtcbuf);
					} else {
						if(mode == ELEM_RTC_MODE_CONTINUOUS) {
							RTC_Mode = UseWeekDays ? "RTC_MODE_WEEKDAY_CONTINUOUS"   : "RTC_MODE_DATE_CONTINUOUS";
						} else {
							RTC_Mode = UseWeekDays ? "RTC_MODE_WEEKDAY_INTERMITTENT" : "RTC_MODE_DATE_INTERMITTENT";
						}

						fprintf(f, "RTC_StartTM = *AdjustDate(RTC_StartTM, RTC_GETDATE_MODE_START);\n");
					    for(j = 0; j < indent; j++) fprintf(f, "    ");

						fprintf(f, "RTC_EndTM   = *AdjustDate(RTC_EndTM  , RTC_GETDATE_MODE_END  );\n");
					    for(j = 0; j < indent; j++) fprintf(f, "    ");

						sprintf(rtcbuf, "RTC_OutputState(RTC_StartTM, RTC_EndTM, RTC_NowTM, %s, %d)", RTC_Mode, WeekDays);
					}

					fprintf(f, "%s\n", GenVarCode(buf, MapSym(IntCode[i].name1), rtcbuf, GENVARCODE_MODE_WRITE));
				}
				break;

            case INT_READ_ENC: {
				int ch = GetPinEnc(IntCode[i].name1);
				if(ch != NO_PIN_ASSIGNED) {
					sprintf(buf2, "ENC_Read(%d)", ch - 1);
					fprintf(f, "%s\n", GenVarCode(buf, MapSym(IntCode[i].name1), buf2, GENVARCODE_MODE_WRITE));
				}
				break;
				}
            case INT_RESET_ENC: {
				int ch = GetPinEnc(IntCode[i].name1);
				char *name = GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ);
				if(ch != NO_PIN_ASSIGNED) {
					fprintf(f, "ENC_Reset(%d, %s);\n", ch-1, name);
				}
				break;
				}
			case INT_READ_FORMATTED_STRING:
				fprintf(f, "Format_String_Read(\"%s\", &%s);\n", IntCode[i].name2, GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
			case INT_WRITE_FORMATTED_STRING:
				fprintf(f, "Format_String_Write(\"%s\", &%s);\n", IntCode[i].name2, GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
			case INT_READ_SERVO_YASKAWA:
				fprintf(f, "Yaskawa_Read(\"%s\", \"%s\", &%s);\n", IntCode[i].name3, IntCode[i].name2, GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
			case INT_WRITE_SERVO_YASKAWA:
				fprintf(f, "Yaskawa_Write(\"%s\", \"%s\", &%s);\n", IntCode[i].name3, IntCode[i].name2, GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_READ_USS:
				fprintf(f, "USS_Get_Param(%d, %d, %d, %d, &%s);\n", atoi(IntCode[i].name2), atoi(IntCode[i].name3), atoi(IntCode[i].name4), IntCode[i].literal, GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break; 
            case INT_WRITE_USS:
				fprintf(f, "USS_Set_Param(%d, %d, %d, %d, &%s);\n", atoi(IntCode[i].name2), atoi(IntCode[i].name3), atoi(IntCode[i].name4), IntCode[i].literal, GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_READ_MODBUS:
				if(IntCode[i].literal) MODBUS_TCP_MASTER = 1; else MODBUS_RS485_MASTER = 1;
				fprintf(f, "Modbus_Send(%d, %uUL, MODBUS_FC_READ_HOLDING_REGISTERS, %d, %d, &%s);\n", atoi(IntCode[i].name2), IntCode[i].literal, atoi(IntCode[i].name3), IntCode[i].bit + 1, GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_WRITE_MODBUS:
				if(IntCode[i].literal) MODBUS_TCP_MASTER = 1; else MODBUS_RS485_MASTER = 1;
				fprintf(f, "Modbus_Send(%d, %uUL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, %d, %d, &%s);\n", atoi(IntCode[i].name2), IntCode[i].literal, atoi(IntCode[i].name3), IntCode[i].bit + 1, GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_SET_PWM:
				fprintf(f, "PWM_Set(%s, %s);\n", IsNumber(IntCode[i].name1) ? IntCode[i].name1 : GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ), IntCode[i].name2);
				HasPWM = 1;
				break;
            case INT_UART_RECV:
				sprintf(buf2, "RS485_Read((unsigned char *)&%s, 1)", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(IntCode[i].name2), buf2, GENVARCODE_MODE_WRITE));
				break;
            case INT_UART_SEND:
				sprintf(buf2, "RS485_Write((unsigned char *)&%s, 1)", GenVarCode(buf, MapSym(IntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(IntCode[i].name2), buf2, GENVARCODE_MODE_WRITE));
                break;

            default:
                oops();
        }
    }
}

//-----------------------------------------------------------------------------
// Split file name with a full path into 3 string and return offset of start file name string
//-----------------------------------------------------------------------------
char * SplitPathFileNameAndExt(char * dest)
{
	unsigned int i = 0;
	char * fileName = NULL;

	for (i = strlen(dest); i; i--)
	{
		if (dest[i] == '.')
		{
			dest[i] = '\0';
			break;
		}
	}

	for (i = strlen(dest); i; i--)
	{
		if (dest[i] == '\\')
		{
			if (fileName == NULL)
			{
				fileName = dest + (strlen(dest) - (strlen(dest) - i)) + 1;
				dest[i] = '\0';
			}
			else
				dest[i] = '/';
		}
	}

	return fileName;
}

//-----------------------------------------------------------------------------
// Split file name with a full path into 3 string and return offset of start file name string
//-----------------------------------------------------------------------------
char * ConvertToUNIXPath(char * dest)
{
	unsigned int i = 0;
	unsigned int x = 0;

	for (i = strlen(dest); i; i--)
	{
		if (dest[i] == '\\' && dest[i + 1] != ' ')
			dest[i] = '/';
	}

	for (i = strlen(dest); i; i--)
	{
		if (dest[i] == ' ' && i > 0)
		{
			if (dest[i - 1] != '\\')
			{
				for (x = strlen(dest)+1; x > i; x--)
					dest[x+1] = dest[x];
				dest[i] = '\\';
				dest[i + 1] = ' ';
			}
		}
	}
	return dest;
}

void DumpInvokeCmd(char * szCmd, char * szArgs)
{
	char szAppPath[MAX_PATH]      = "";
	char szAppDirectory[MAX_PATH] = "";

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	// Extract directory
	strncpy(szAppDirectory, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
	szAppDirectory[strlen(szAppDirectory)] = '\0';

	strcat(szAppDirectory, "\\src\\cmd.bat");

    FILE *f = fopen(szAppDirectory, "w");
    if(!f)
        return;

    fprintf(f,"%s %s\n", szCmd, szArgs);

	fclose(f);
}

DWORD InvokeGCC(char* dest)
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	char szCmd[MAX_PATH];
	char szArgs[4096];
	char *pEnvCMD = NULL;
	char *pDefaultCMD = "CMD.EXE";
	ULONG exitCode;
	
	char szAppProgramFilesLong[MAX_PATH]   = "";
	char szAppProgramFiles[MAX_PATH]   = "";
	char szToolsProgramFiles[MAX_PATH]   = "";
	char szAppPathLong[MAX_PATH]      = "";
	char szAppPath[MAX_PATH]      = "";
	char szTmpPathLong[MAX_PATH]		= "";
	char szTmpPath[MAX_PATH]		= "";
	char szTempPath[MAX_PATH]		= "";
	//char szAppDirectoryLong[MAX_PATH] = "";
	char szAppDirectory[MAX_PATH] = "";
	char szAppDestPath[MAX_PATH]  = "";
	char szAppOutputDir[MAX_PATH] = "";
	char * fileName;

	if (!SUCCEEDED(SHGetSpecialFolderPath(0, szAppProgramFilesLong, CSIDL_PROGRAM_FILES, FALSE))) 
		return GetLastError();

	if (!SUCCEEDED(GetShortPathName(szAppProgramFilesLong, szAppProgramFiles, sizeof(szAppProgramFiles))))
		return GetLastError();

	strcpy(szToolsProgramFiles, szAppProgramFiles);

	::GetModuleFileName(0, szAppPathLong, sizeof(szAppPathLong) - 1);

	if (!SUCCEEDED(GetShortPathName(szAppPathLong, szAppPath, sizeof(szAppPath))))
		return GetLastError();

	GetTempPath(sizeof(szTmpPathLong), szTmpPathLong);

	if (!SUCCEEDED(GetShortPathName(szTmpPathLong, szTmpPath, sizeof(szTmpPath))))
		return GetLastError();

	strcat(szTmpPath, "POPTools\\");
	if (!CreateDirectory(szTmpPath, NULL))
	{
		DWORD err = GetLastError();	
		if (err != ERROR_ALREADY_EXISTS)
		{
			LPTSTR s;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				err,
				0,
				(LPTSTR)&s,
				0,
				NULL);
			return err;
		}
	}

	strncpy(szTempPath, szTmpPath, strrchr(szTmpPath, '\\') - szTmpPath);
	szTempPath[strlen(szTempPath)] = '\0';

	// Extract directory
	strncpy(szAppDirectory, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
	szAppDirectory[strlen(szAppDirectory)] = '\0';

	strncpy(szAppOutputDir, szTempPath, strrchr(szTempPath, '\\') - szTempPath);
	szAppOutputDir[strlen(szAppOutputDir)] = '\0';

	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	strcpy(szAppDestPath, dest);
	fileName = SplitPathFileNameAndExt(szAppDestPath);

	strcpy(szCmd, "");
	strcpy(szArgs, "");

	pEnvCMD = getenv("COMSPEC");

	if(pEnvCMD)
		strcat(szCmd, pEnvCMD);
	else
		strcat(szCmd, pDefaultCMD);

	strcat(szArgs, " /C \"\""); 
	strcat(szArgs, szAppDirectory); // C:\Users\ADMINI~1\DOCUME~1\VISUAL~3\Projects\POPTools\bin
	strcat(szArgs, "\\gcc\\bin\\cs-make.exe\" -f compile -C \"");
	strcat(szArgs, ConvertToUNIXPath(szAppDirectory));
	strcat(szArgs, "/src/\"");
	strcat(szArgs, " HEX_FILE=\""); 
	strcat(szArgs, fileName);
	strcat(szArgs, "\" ");
	strcat(szArgs, " HEX_PATH=\""); 
	strcat(szArgs, ConvertToUNIXPath(szAppDestPath));
	strcat(szArgs, "\" ");
	strcat(szArgs, " TEMP_PATH=\""); 
	strcat(szArgs, ConvertToUNIXPath(szTempPath));
	strcat(szArgs, "\" ");
	strcat(szArgs, " TOOLS_PATH=\"");
	strcat(szArgs, ConvertToUNIXPath(szAppDirectory));
	strcat(szArgs, "/src/gcc\" clean release "); 

	//output the application being run from the command window to the log file.

	strcat(szArgs, "> \"");
	strcat(szArgs, szAppOutputDir);
	strcat(szArgs, "\\POPTools\\output.log\" 2>&1");

	strcat(szArgs, "\"");

	if (!CreateProcess(szCmd, szArgs, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, 
		NULL, 
		NULL,
		&StartupInfo,
		&ProcessInfo))
	{
		DWORD err = GetLastError();	
		LPTSTR s;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            err,
            0,
            (LPTSTR)&s,
            0,
            NULL);
		return err;
	}

#ifdef _DEBUG
	//DumpInvokeCmd(szCmd, szArgs);
#endif

	while(WaitForSingleObject(ProcessInfo.hProcess, 25) == WAIT_TIMEOUT)
		UpdateProgressWindow(NULL);

	if(!GetExitCodeProcess(ProcessInfo.hProcess, &exitCode))
		exitCode = 0;

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);

	return exitCode;
}

void CheckPinAssignments(void)
{
	int i;
	unsigned int type;
	for(i=0; i < Prog.io.count; i++) {
		type = Prog.io.assignment[i].type;
		if(Prog.io.assignment[i].pin == NO_PIN_ASSIGNED &&
			(type == IO_TYPE_READ_ADC || type == IO_TYPE_READ_ENC || type == IO_TYPE_RESET_ENC)) {
				Error(_("Must assign pins for all I/O.\r\n\r\n'%s' is not assigned."), Prog.io.assignment[i].name);
				CompileError();
		}
	}
}

DWORD GenerateCFile(char *filename)
{
	unsigned int i, ad_mask;

	FILE *f = fopen(filename, "w");
    if(!f) {
        Error(_("Couldn't open file '%s'"), filename);
        return 0;
    }

    if(setjmp(CompileErrorBuf) != 0) {
        fclose(f);
		remove(filename);
        return 0;
    }

	HasPWM = 0;

	// Set up the TRISx registers (direction). 1 means tri-stated (input).
    BYTE isInput[MAX_IO_PORTS], isOutput[MAX_IO_PORTS];
    BuildDirectionRegisters(isInput, isOutput);

	CheckPinAssignments();

	for(i = 0; i < SEENVAR_LISTS; i++) {
	    SeenVariablesCount[i] = 0;
	}

	MODBUS_RS485_MASTER = 0;
	MODBUS_TCP_MASTER = 0;

	fprintf(f, "/*****************************************************************************\n");
	fprintf(f, " * Tecnequip Tecnologia em Equipamentos Ltda                                 *\n");
	fprintf(f, " *****************************************************************************/\n");
	/*fprintf(f, "#include <string.h>\n");
	fprintf(f, "#include \"lwip/opt.h\"\n");
	fprintf(f, "#include \"lwip/arch.h\"\n");
	fprintf(f, "#include \"lwip/api.h\"\n");
	fprintf(f, "#include \"coos.h\"\n");

	fprintf(f, "\n");
	fprintf(f, "#include \"rtc.h\"\n");
	fprintf(f, "#include \"gpio.h\"\n");
	fprintf(f, "#include \"rtc.h\"\n");
	fprintf(f, "#include \"dac.h\"\n");
	fprintf(f, "#include \"adc.h\"\n");
	fprintf(f, "#include \"qei.h\"\n");
	fprintf(f, "#include \"rs485.h\"\n");
	fprintf(f, "#include \"rs232.h\"\n");
	fprintf(f, "#include \"modbus.h\"\n");
	fprintf(f, "#include \"uss.h\"\n");
	fprintf(f, "#include \"yaskawa.h\"\n");
	fprintf(f, "#include \"format_str.h\"\n");*/

	fprintf(f, "#include \"ld.h\"\n");

	fprintf(f, "\n");
	//fprintf(f, "const volatile unsigned int 	CYCLE_TIME = %d;\n", Prog.settings.cycleTime / 1000);
	//fprintf(f, "const volatile unsigned int		TIME_INTERVAL = ((25000000/1000) * %d) - 1;\n", Prog.settings.cycleTime / 1000);
	
	fprintf(f, "\n");
	fprintf(f, "extern volatile unsigned char 	MODBUS_RS485_MASTER; // 0 = Slave, 1 = Master\n");
	fprintf(f, "extern volatile unsigned char 	MODBUS_TCP_MASTER; // 0 = Slave, 1 = Master\n");
	fprintf(f, "extern volatile int 			MODBUS_REGISTER[32];\n");
	//fprintf(f, "extern volatile int 			ENCODER1;\n");
	fprintf(f, "extern struct tm 				RTC_NowTM;\n");
	fprintf(f, "struct tm 						RTC_StartTM;\n");
	fprintf(f, "struct tm 						RTC_EndTM;\n");
	for(i=0; *InternalFlags[i]; i++) {
		fprintf(f, "extern volatile unsigned int 	I_%s;\n", InternalFlags[i]);
	}

	fprintf(f, "\n");
	fprintf(f, "extern struct ip_addr 			IP_ADDRESS;\n");
	fprintf(f, "extern struct ip_addr 			IP_NETMASK;\n");
	fprintf(f, "extern struct ip_addr 			IP_GATEWAY;\n");
	fprintf(f, "extern struct ip_addr			IP_DNS;\n");

	fprintf(f, "\n");
	fprintf(f, "char 							SNTP_SERVER_ADDRESS[] = \"%s\";\n", Prog.settings.sntp);
	fprintf(f, "int								SNTP_GMT = %d;\n", Prog.settings.gmt > 12 ? Prog.settings.gmt - 12 : Prog.settings.gmt - 12);
	fprintf(f, "int								SNTP_DAILY_SAVE = %d;\n", Prog.settings.dailysave);

	fprintf(f, "\n");
	fprintf(f, "// Variaveis PLC\n");

	// now generate declarations for all variables
    GenerateDeclarations(f);

	fprintf(f, "\n");

    fprintf(f,"void PLC_Run(void)\n{\n");

    GenerateAnsiC(f, ad_mask);

    fprintf(f, "}\n");

	fprintf(f, "\n");

	fprintf(f, "void PLC_Init(void)\n");
	fprintf(f, "{\n");
	fprintf(f, "	I_SerialReady = 1;\n");
	fprintf(f, "	MODBUS_RS485_MASTER = %d;\n", MODBUS_RS485_MASTER);
	fprintf(f, "	MODBUS_TCP_MASTER = %d;\n", MODBUS_TCP_MASTER);
	fprintf(f, "	ModBUS_SetID(%d);\n", Prog.settings.ModBUSID);
	fprintf(f, "\n");
	fprintf(f, "	RS485_Config(%d, %d, %d, %d);\n", Prog.settings.baudRate, SerialConfig[Prog.settings.UART].bByteSize,
		SerialConfig[Prog.settings.UART].bParity, SerialConfig[Prog.settings.UART].bStopBits == ONESTOPBIT ? 1 : 2);
	fprintf(f, "\n");
	fprintf(f, "	IP4_ADDR(&IP_ADDRESS, %d,%d,%d,%d);\n", Prog.settings.ip[0], Prog.settings.ip[1], Prog.settings.ip[2], Prog.settings.ip[3]);
	fprintf(f, "	IP4_ADDR(&IP_NETMASK, %d,%d,%d,%d);\n", Prog.settings.mask[0], Prog.settings.mask[1], Prog.settings.mask[2], Prog.settings.mask[3]);
	fprintf(f, "	IP4_ADDR(&IP_GATEWAY, %d,%d,%d,%d);\n", Prog.settings.gw[0], Prog.settings.gw[1], Prog.settings.gw[2], Prog.settings.gw[3]);
	fprintf(f, "	IP4_ADDR(&IP_DNS, %d,%d,%d,%d);\n", Prog.settings.dns[0], Prog.settings.dns[1], Prog.settings.dns[2], Prog.settings.dns[3]);
	fprintf(f, "\n");
	if(Prog.settings.x4) {
		fprintf(f, "    QEIConfig.CaptureMode = QEI_CAPMODE_4X;\n");
	} else {
		fprintf(f, "    QEIConfig.CaptureMode = QEI_CAPMODE_2X;\n");
	}

	fprintf(f, "    ADC_SetMask(%d);\n", ad_mask);
	fprintf(f, "    SSI_Init(%d, %d);\n", Prog.settings.ssi_size, Prog.settings.ssi_mode);
	if(HasPWM) {
		fprintf(f, "    PWM_Init();\n");
	}

	fprintf(f, "\n");
	for(i=0; i != SEENVAR_MODE_OTHERS; i++) {
		if(SeenVariablesCount[i]) {
			fprintf(f, "    memset((void*)%s, 0, sizeof(%s));\n", NameVarArray[i], NameVarArray[i]);
		}
	}

	fprintf(f, "}\n");

	fclose(f);

	return 1;
}

DWORD CompileAnsiCToGCC(BOOL ShowSuccessMessage)
{
	char szAppPath[MAX_PATH]		= "";
	char szTempPath[MAX_PATH]		= "";
	char szAppHeader[MAX_PATH]		= "";
	char szAppSourceFile[MAX_PATH]	= "";

	if(ValidateDiagram() == DIAGRAM_VALIDATION_ERROR) {
		return 1;
	}

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	GetTempPath(sizeof(szTempPath), szTempPath);

	strcat(szTempPath, "POPTools\\");
	if (!CreateDirectory(szTempPath, NULL))
	{
		DWORD err = GetLastError();	
		if (err != ERROR_ALREADY_EXISTS)
		{
			LPTSTR s;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				err,
				0,
				(LPTSTR)&s,
				0,
				NULL);
			return err;
		}
	}

	strncpy(szAppSourceFile, szTempPath, strrchr(szTempPath, '\\') - szTempPath);
	sprintf(CurrentCompileFile, "%s\\ld.hex", szAppSourceFile);
	strcat(szAppSourceFile, "\\ld.c");

	if(!GenerateCFile(szAppSourceFile))
		return 1;
	
	char str[MAX_PATH+500];
	DWORD err = 0;

	if ((err = InvokeGCC(CurrentCompileFile))) {
		sprintf(str, _("A compilação retornou erro. O código do erro é %d. O arquivo com o log do erro esta na pasta \"C:\\Users\\<User>\\AppData\\Temp\\POPTools\\output.log\"\n"), err);
//		CompileSuccessfulMessage(str);
	} else if(ShowSuccessMessage) {
		sprintf(str, _("Compilado com sucesso. O arquivo binário foi criado em '%s'.\r\n\r\n"), CurrentCompileFile);
//		CompileSuccessfulMessage(str);
	}

	return err;
}
