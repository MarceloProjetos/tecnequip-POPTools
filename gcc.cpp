#include "poptools.h"
#include "intreg.h"

#include <vector>

static vector<IntOp>::size_type IntCodeLen;
static vector<IntOp> vectorIntCode;
static map<unsigned int, unsigned int> mapExpansionIO;

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
int HasADC = 0;
int HasDAC = 0;
int HasQEI = 0;
int HasSSI = 0;

// Output log
char OutputLog[MAX_PATH + MAX_NAME_LEN + 1];

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
	unsigned int offset = ladder->getPinOffsetIO();
	vector<tExpansionBoardItem> boards;
    static char AllRets[4][MAX_NAME_LEN+30];
    static int RetCnt;

    RetCnt = (RetCnt + 1) & 3;

    char *ret = AllRets[RetCnt];
    
	// Tenta receber o nome da variavel interna. Se retornar string vazia, indica
	// que nao eh uma variavel interna. Caso contrario, usa o nome e retorna.
	string name = ladder->getInternalVarNameIO(str);
	if(name.size() > 0) {
		strcpy(ret, name.c_str());
		return ret;
	}

	mapDetails detailsIO = ladder->getDetailsIO(str);
	boards = ladder->getBoardList();

	if (detailsIO.type == eType_DigOutput) {
		if(IoMap_IsModBUS(detailsIO)) {
			sprintf(ret, "MODBUS_REGISTER[%d]", IoMap_GetIndex(detailsIO));
			return ret;
		} else if(detailsIO.pin >= offset) {
			sprintf(ret, "expansionBoards[%d].value.bits.bit%d", mapExpansionIO[detailsIO.pin / offset], (detailsIO.pin % offset) - 1);
			return ret;
		} else {
			sprintf(ret, "GPIO_OUTPUT_PORT%d", detailsIO.pin);
			return ret;
		}
	} else if(detailsIO.type == eType_General && detailsIO.pin) {
		sprintf(ret, "MODBUS_REGISTER[%d]", IoMap_GetIndex(detailsIO));
		return ret;
	} else if (detailsIO.type == eType_DigInput) {
		if(IoMap_IsModBUS(detailsIO)) {
			sprintf(ret, "MODBUS_REGISTER[%d]", IoMap_GetIndex(detailsIO));
			return ret;
		} else if(detailsIO.pin >= offset) {
			sprintf(ret, "expansionBoards[%d].value.bits.bit%d", mapExpansionIO[detailsIO.pin / offset], (detailsIO.pin % offset) - 1);
			return ret;
		} else {
			sprintf(ret, "GPIO_INPUT_PORT%d", detailsIO.pin);
			return ret;
		}
	}

	if (strlen(str) == 0) {
		sprintf(ret, "VAR_NULL");
	} else if(!strncmp(str, "$S_", 3)) {
		strcpy(ret, str+3);
	} else if(*str == '$') {
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
	if(rawstr == nullptr || strlen(rawstr) == 0) {
		rawstr = "$dummyInt";
	}

	int mode;
	bool intvar;
	char *str = MapSym(rawstr);
	if (isdigit(*rawstr) || *rawstr == '-')
	{
		return;
	}

	intvar = ladder->IsInternalVarIO(rawstr);
	mode = ((!strncmp(str, "U_", 2) || !strncmp(str, "I_", 2)) && !intvar) ? SEENVAR_MODE_USERINT : SEENVAR_MODE_OTHERS;
	if(!SeenVariable(str, mode) && mode == SEENVAR_MODE_OTHERS && strncmp(str, "M", 1)) {
#ifdef INT_UNSIGNED
		fprintf(f, "volatile unsigned int %s = 0;\n", str);
#else
		if(intvar) {
			fprintf(f, "extern volatile int %s;\n", str);
		} else if(strstr(str, "expansionBoards[") == NULL) {
			fprintf(f, "volatile int %s = 0;\n", str);
		}
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
	if(rawStr == nullptr || strlen(rawStr) == 0) {
		rawStr = "$dummyChar";
	}

	int mode, intflag;
    char *str = MapSym(rawStr);
	if (!strncmp(str, "M", 1)) {
		return;
	}

	intflag = (!strncmp(str, "I_", 2) && ladder->IsInternalFlagIO(str+2));

	mode = (*rawStr == '$'  && !intflag) ? SEENVAR_MODE_SYSTEM : (!strncmp(str, "U_", 2) ? SEENVAR_MODE_USERBIT : SEENVAR_MODE_OTHERS);
	if(!SeenVariable(str, mode) && mode == SEENVAR_MODE_OTHERS && !intflag) {
#ifdef INT_UNSIGNED
		fprintf(f, "volatile unsigned int %s = 0;\n", str);
#else
		if(strstr(str, "expansionBoards[") == NULL) {
			fprintf(f, "volatile unsigned char %s = 0;\n", str);
		}
#endif
	}
}

//-----------------------------------------------------------------------------
// Generate declarations for all the 16-bit/single bit variables in the ladder
// program.
//-----------------------------------------------------------------------------
static void GenerateDeclarations(FILE *f)
{
	vector<IntOp>::size_type i;
    for(i = 0; i < IntCodeLen; i++) {
        char *bitVar1 = NULL, *bitVar2 = NULL;
        char *intVar1 = NULL, *intVar2 = NULL, *intVar3 = NULL;

        switch(vectorIntCode[i].op) {
            case INT_SET_BIT:
            case INT_CLEAR_BIT:
                bitVar1 = vectorIntCode[i].name1;
                break;

			case INT_SET_SINGLE_BIT:
			case INT_CLEAR_SINGLE_BIT:
                intVar1 = vectorIntCode[i].name1;
                break;

            case INT_COPY_BIT_TO_BIT:
                bitVar1 = vectorIntCode[i].name1;
                bitVar2 = vectorIntCode[i].name2;
                break;

            case INT_SET_VARIABLE_TO_LITERAL:
                intVar1 = vectorIntCode[i].name1;
                break;

            case INT_SET_VARIABLE_TO_VARIABLE:
                intVar1 = vectorIntCode[i].name1;
                intVar2 = vectorIntCode[i].name2;
                break;

            case INT_SET_VARIABLE_MODULO:
            case INT_SET_VARIABLE_DIVIDE:
            case INT_SET_VARIABLE_MULTIPLY:
            case INT_SET_VARIABLE_SUBTRACT:
            case INT_SET_VARIABLE_ADD:
                intVar1 = vectorIntCode[i].name1;
                intVar2 = vectorIntCode[i].name2;
                intVar3 = vectorIntCode[i].name3;
                break;

			case INT_READ_FORMATTED_STRING:
			case INT_WRITE_FORMATTED_STRING:
                intVar1 = vectorIntCode[i].name1;
                intVar2 = vectorIntCode[i].name3;
                break;

            case INT_INCREMENT_VARIABLE:
            case INT_READ_ADC:
            case INT_SET_DA:
            case INT_READ_ENC:
            case INT_RESET_ENC:
			case INT_READ_SERVO_YASKAWA:
			case INT_WRITE_SERVO_YASKAWA:
            case INT_READ_USS:
            case INT_WRITE_USS:
            case INT_SET_PWM:
                intVar1 = vectorIntCode[i].name1;
                break;

            case INT_READ_MODBUS:
            case INT_WRITE_MODBUS:
                intVar1 = vectorIntCode[i].name1;
                break;

            case INT_UART_RECV:
            case INT_UART_SEND:
                intVar1 = vectorIntCode[i].name1;
                bitVar1 = vectorIntCode[i].name2;
                break;

            case INT_IF_BIT_SET:
            case INT_IF_BIT_CLEAR:
                bitVar1 = vectorIntCode[i].name1;
                break;

			case INT_IF_BIT_CHECK_SET:
			case INT_IF_BIT_CHECK_CLEAR:
                intVar1 = vectorIntCode[i].name1;
                break;

            case INT_IF_VARIABLE_LES_LITERAL:
                intVar1 = vectorIntCode[i].name1;
                break;

            case INT_IF_VARIABLE_EQUALS_VARIABLE:
            case INT_IF_VARIABLE_GRT_VARIABLE:
                intVar1 = vectorIntCode[i].name1;
                intVar2 = vectorIntCode[i].name2;
                break;

			case INT_MULTISET_DA:
				if(strlen(vectorIntCode[i].name1)) {
					intVar1 = vectorIntCode[i].name1;
					intVar2 = vectorIntCode[i].name2;
				}
                break;

            case INT_EEPROM_READ:
            case INT_EEPROM_WRITE:
                intVar1 = vectorIntCode[i].name1;
                break;

            case INT_RAND:
                intVar1 = vectorIntCode[i].name1;
                intVar2 = vectorIntCode[i].name2;
                intVar3 = vectorIntCode[i].name3;
                break;

            case INT_LCD:
                intVar1 = vectorIntCode[i].name1;
                intVar2 = vectorIntCode[i].name2;
                intVar3 = vectorIntCode[i].name3;
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
            case INT_CREATE_STATIC_VARIABLE: // Variavel estatica sera local, nao deve ser declarada globalmente
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
    vector<IntOp>::size_type i;
    int indent = 1;
	char buf[1024], buf2[1024];

	ad_mask = 0;

	fprintf(f, "    Cycle_Init();\n\n");

	for(i = 0; i < IntCodeLen; i++) {

        if(vectorIntCode[i].op == INT_ELSE_IF) indent--;
        if(vectorIntCode[i].op == INT_END_IF) indent--;
        if(vectorIntCode[i].op == INT_ELSE) indent--;

		if(!i || vectorIntCode[i-1].op != INT_ELSE_IF) {
	        int j;
		    for(j = 0; j < indent; j++) fprintf(f, "    ");
		}

		char *str = MapSym(vectorIntCode[i].name1);
        char op = 0;

        switch(vectorIntCode[i].op) {
            case INT_SET_BIT:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
					fprintf(f, "%s |= (1 << %d);  // %s = 1\n", MapSym(vectorIntCode[i].name1), vectorIntCode[i].bit, vectorIntCode[i].name1);
				else
					fprintf(f, "%s // %s = 1\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), "1", GENVARCODE_MODE_WRITE), vectorIntCode[i].name1);
                break;

            case INT_CLEAR_BIT:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
					fprintf(f, "%s &= ~(1 << %d);  // %s = 0\n", MapSym(vectorIntCode[i].name1), vectorIntCode[i].bit, vectorIntCode[i].name1);
				else
	                fprintf(f, "%s // %s = 0\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), "0", GENVARCODE_MODE_WRITE), vectorIntCode[i].name1);
                break;

			case INT_SET_SINGLE_BIT:
                fprintf(f, "%s |= 1 << %d; // %s.%d\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].bit, vectorIntCode[i].name1, vectorIntCode[i].bit);
                break;

            case INT_CLEAR_SINGLE_BIT:
                fprintf(f, "%s &= ~(1 << %d); // %s.%d\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].bit, vectorIntCode[i].name1, vectorIntCode[i].bit);
                break;

            case INT_COPY_BIT_TO_BIT:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
	                fprintf(f, "%s &= ~(1 << %d); %s |= %s << %d;  // %s = %s\n", MapSym(vectorIntCode[i].name1), vectorIntCode[i].bit, MapSym(vectorIntCode[i].name1), GenVarCode(buf2, MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].bit, vectorIntCode[i].name1, vectorIntCode[i].name2);
				else
					fprintf(f, "%s // %s = %s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1),  GenVarCode(buf2, MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ), GENVARCODE_MODE_WRITE), vectorIntCode[i].name1, vectorIntCode[i].name2);
                break;

            case INT_SET_VARIABLE_TO_LITERAL:
				sprintf(buf2, "%d", vectorIntCode[i].literal);
                fprintf(f, "%s // %s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), buf2, GENVARCODE_MODE_WRITE), vectorIntCode[i].name1);
                break;

            case INT_SET_VARIABLE_TO_VARIABLE:
                fprintf(f, "%s // %s = %s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), GenVarCode(buf2, MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ), GENVARCODE_MODE_WRITE),
                    vectorIntCode[i].name1, vectorIntCode[i].name2);
                break;

            {
                case INT_SET_VARIABLE_ADD: op = '+'; goto arith;
                case INT_SET_VARIABLE_SUBTRACT: op = '-'; goto arith;
                case INT_SET_VARIABLE_MULTIPLY: op = '*'; goto arith;
                case INT_SET_VARIABLE_MODULO: op = '%';
				case INT_SET_VARIABLE_DIVIDE: op = op ? op : '/'; fprintf(f, "if(%s) ", GenVarCode(buf , MapSym(vectorIntCode[i].name3), NULL, GENVARCODE_MODE_READ)); goto arith;
                arith:
					char mathstr[1024];
					sprintf(mathstr, "%s %c %s",
						GenVarCode(buf , MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ),
						op,
						GenVarCode(buf2, MapSym(vectorIntCode[i].name3), NULL, GENVARCODE_MODE_READ));
					fprintf(f, "%s // %s = %s %c %s\n",
						GenVarCode(buf, MapSym(vectorIntCode[i].name1), mathstr, GENVARCODE_MODE_WRITE),
                        vectorIntCode[i].name1,
                        vectorIntCode[i].name2,
                        op,
                        vectorIntCode[i].name3);
                    break;
            }

            case INT_CREATE_STATIC_VARIABLE:
                fprintf(f, "static int %s = 0;\n", MapSym(vectorIntCode[i].name1));
                break;

            case INT_INCREMENT_VARIABLE:
                fprintf(f, "%s++;\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
                break;

            case INT_IF_BIT_SET:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
	                fprintf(f, "if (%s & (1 << %d)) {  // %s\n", MapSym(vectorIntCode[i].name1), vectorIntCode[i].bit, vectorIntCode[i].name1);
				else
					fprintf(f, "if (%s) {  // %s\n", GenVarCode(buf , MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CLEAR:
				if (strncmp(str, "MODBUS_REGISTER[", 2) == 0)
	                fprintf(f, "if (!(%s & (1 << %d))) {  // %s\n", MapSym(vectorIntCode[i].name1), vectorIntCode[i].bit, vectorIntCode[i].name1);
				else
					fprintf(f, "if (!%s) {  // %s\n", GenVarCode(buf , MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CHECK_SET:
				fprintf(f, "if (%s & (1 << %d)) {  // %s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].bit, vectorIntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CHECK_CLEAR:
				fprintf(f, "if (!(%s & (1 << %d))) {  // %s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].bit, vectorIntCode[i].name1);
                indent++;
                break;

            case INT_IF_VARIABLE_LES_LITERAL:
                fprintf(f, "if (%s < %d) {\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), 
                    vectorIntCode[i].literal);
                indent++;
                break;

            case INT_IF_VARIABLE_EQUALS_VARIABLE:
                fprintf(f, "if (%s == %s) {\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ),
                    GenVarCode(buf2, MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ));
                indent++;
                break;

            case INT_IF_VARIABLE_GRT_VARIABLE:
                fprintf(f, "if (%s > %s) { // %s > %s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ),
                    GenVarCode(buf2, MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].name1, vectorIntCode[i].name2);
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
                if(vectorIntCode[i].name1[0]) {
                    fprintf(f, "/* %s */\n", vectorIntCode[i].name1);
                } else {
                    fprintf(f, "\n");
                }
                break;

            case INT_EEPROM_BUSY_CHECK:
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), "E2P_Busy()", GENVARCODE_MODE_WRITE));
				break;
            case INT_EEPROM_READ:
				sprintf(buf2, "E2P_Read((void *)&%s, %d, 4)", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].literal);
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name2), buf2, GENVARCODE_MODE_WRITE));
				break;
            case INT_EEPROM_WRITE:
				fprintf(f, "E2P_Write((void *)&%s, %d, 4);\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].literal);
				break;
            case INT_READ_ADC: {
				HasADC = 1;
				unsigned int ad_channel = ladder->getDetailsIO(vectorIntCode[i].name1).pin;

				ad_mask |= 1 << (ad_channel - 1); // Canal do A/D comeca em zero, devemos descontar 1
				sprintf(buf2, "ADC_Read(%d)", ad_channel);
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), buf2, GENVARCODE_MODE_WRITE));
				break;
			}
            case INT_SET_DA:
				HasDAC = 1;

				fprintf(f, "DAC_Write(DAC_Conv(%s, %d));\n", IsNumber(vectorIntCode[i].name1) ? vectorIntCode[i].name1 : GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].literal);
				break;
            case INT_SQRT:
				fprintf(f, "srint(&%s);\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_RAND: {
				char *min, *max;
				char randbuf[1024];
				min = IsNumber(vectorIntCode[i].name2) ? vectorIntCode[i].name2 : GenVarCode(buf , MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ);
				max = IsNumber(vectorIntCode[i].name3) ? vectorIntCode[i].name3 : GenVarCode(buf2, MapSym(vectorIntCode[i].name3), NULL, GENVARCODE_MODE_READ);
				sprintf(randbuf, "(%s <= %s ? %s + (rand() %% (%s - %s + 1)) : %s + (rand() %% (%s - %s + 1)))", min, max, min, max, min, max, min, max);
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), randbuf, GENVARCODE_MODE_WRITE));
				break;
			}
			case INT_MULTISET_DA:
				HasDAC = 1;

				if(strlen(vectorIntCode[i].name1)) {
					fprintf(f, "DAC_Start(%d, %d, %s, %s, %s, DAC_Conv(%s, %s));\n", vectorIntCode[i].bit, vectorIntCode[i].literal, vectorIntCode[i].name3, vectorIntCode[i].name4,
						IsNumber(vectorIntCode[i].name1) ? vectorIntCode[i].name1 : GenVarCode(buf , MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ),
						IsNumber(vectorIntCode[i].name2) ? vectorIntCode[i].name2 : GenVarCode(buf2, MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].name5);
				} else {
					fprintf(f, "DAC_Abort(%d);\n", vectorIntCode[i].literal-1);
				}
				break;

			case INT_SET_RTC: {
				char *RtcName[] = { "RTC_StartTM", "RTC_EndTM" };
				fprintf(f, "%s.tm_year = %s; %s.tm_mon = %s; %s.tm_mday = %s; %s.tm_hour = %s; %s.tm_min = %s; %s.tm_sec = %s;\n",
					RtcName[vectorIntCode[i].bit], vectorIntCode[i].name3, RtcName[vectorIntCode[i].bit], vectorIntCode[i].name2, RtcName[vectorIntCode[i].bit], vectorIntCode[i].name1,
					RtcName[vectorIntCode[i].bit], vectorIntCode[i].name4, RtcName[vectorIntCode[i].bit], vectorIntCode[i].name5, RtcName[vectorIntCode[i].bit], vectorIntCode[i].name6);
				break;
			}

			case INT_CHECK_RTC:
				{
					char *RTC_Mode;
					char rtcbuf[10*1024];
					int mode = atoi(vectorIntCode[i].name2), j;
					int WeekDays = vectorIntCode[i].literal & 0x7F, UseWeekDays = (vectorIntCode[i].literal >> 7) & 1;

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

					fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), rtcbuf, GENVARCODE_MODE_WRITE));
				}
				break;

            case INT_READ_ENC: {
				mapDetails detailsIO = ladder->getDetailsIO(vectorIntCode[i].name1);
				if((detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc) && detailsIO.pin) {
					if(detailsIO.pin == 1) {
						HasQEI = 1;
					} else {
						HasSSI = 1;
					}

					sprintf(buf2, "ENC_Read(%d)", detailsIO.pin - 1);
					fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name1), buf2, GENVARCODE_MODE_WRITE));
				}
				break;
				}
            case INT_RESET_ENC: {
				char *name = GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ);
				mapDetails detailsIO = ladder->getDetailsIO(vectorIntCode[i].name1);
				if((detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc) && detailsIO.pin) {
					if(detailsIO.pin == 1) {
						HasQEI = 1;
					} else {
						HasSSI = 1;
					}

					fprintf(f, "ENC_Reset(%d, %s);\n", detailsIO.pin-1, name);
				}
				break;
				}
			case INT_READ_FORMATTED_STRING:
				sprintf(buf2, "Format_String_Read(\"%s\", &%s)", vectorIntCode[i].name2, GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name3), buf2, GENVARCODE_MODE_WRITE));
				break;
			case INT_WRITE_FORMATTED_STRING:
				sprintf(buf2, "Format_String_Write(\"%s\", &%s)", vectorIntCode[i].name2, GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name3), buf2, GENVARCODE_MODE_WRITE));
				break;
			case INT_READ_SERVO_YASKAWA:
				fprintf(f, "Yaskawa_Read(\"%s\", \"%s\", &%s);\n", vectorIntCode[i].name3, vectorIntCode[i].name2, GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
			case INT_WRITE_SERVO_YASKAWA:
				fprintf(f, "Yaskawa_Write(\"%s\", \"%s\", &%s);\n", vectorIntCode[i].name3, vectorIntCode[i].name2, GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_READ_USS:
				fprintf(f, "USS_Get_Param(%d, %d, %d, %d, &%s);\n", atoi(vectorIntCode[i].name2), atoi(vectorIntCode[i].name3), atoi(vectorIntCode[i].name4), vectorIntCode[i].literal, GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break; 
            case INT_WRITE_USS:
				fprintf(f, "USS_Set_Param(%d, %d, %d, %d, &%s);\n", atoi(vectorIntCode[i].name2), atoi(vectorIntCode[i].name3), atoi(vectorIntCode[i].name4), vectorIntCode[i].literal, GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_READ_MODBUS:
				if(vectorIntCode[i].literal) MODBUS_TCP_MASTER = 1; else MODBUS_RS485_MASTER = 1;
				fprintf(f, "Modbus_Send(%d, %uUL, MODBUS_FC_READ_HOLDING_REGISTERS, %d, %d, &%s);\n", atoi(vectorIntCode[i].name2), vectorIntCode[i].literal, atoi(vectorIntCode[i].name3), vectorIntCode[i].bit + 1, GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_WRITE_MODBUS:
				if(vectorIntCode[i].literal) MODBUS_TCP_MASTER = 1; else MODBUS_RS485_MASTER = 1;
				fprintf(f, "Modbus_Send(%d, %uUL, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, %d, %d, &%s);\n", atoi(vectorIntCode[i].name2), vectorIntCode[i].literal, atoi(vectorIntCode[i].name3), vectorIntCode[i].bit + 1, GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				break;
            case INT_SET_PWM:
				fprintf(f, "PWM_Set(%s, %s);\n", IsNumber(vectorIntCode[i].name1) ? vectorIntCode[i].name1 : GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ), vectorIntCode[i].name2);
				HasPWM = 1;
				break;
            case INT_UART_RECV:
				sprintf(buf2, "RS485_ReadChar((unsigned char *)&%s)", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name2), buf2, GENVARCODE_MODE_WRITE));
				break;
            case INT_UART_SEND:
				sprintf(buf2, "RS485_Write((unsigned char *)&%s, 1)", GenVarCode(buf, MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ));
				fprintf(f, "%s\n", GenVarCode(buf, MapSym(vectorIntCode[i].name2), buf2, GENVARCODE_MODE_WRITE));
                break;

            case INT_LCD: {
				switch(vectorIntCode[i].literal) {
				case eCommandLCD_Clear:
					fprintf(f, "XP_lcd_Clear();\n");
					break;
				case eCommandLCD_Write:
					GenVarCode(buf2, MapSym(vectorIntCode[i].name2), NULL, GENVARCODE_MODE_READ);
					GenVarCode(buf , MapSym(vectorIntCode[i].name1), NULL, GENVARCODE_MODE_READ);
					fprintf(f, "XP_lcd_MoveCursor(%s,%s); ", buf, buf2);

					GenVarCode(buf , MapSym(vectorIntCode[i].name3), NULL, GENVARCODE_MODE_READ);
					fprintf(f, "XP_lcd_WriteText(\"%s\", &%s);\n", vectorIntCode[i].name4, buf);

					break;
				case eCommandLCD_BackLight:
					fprintf(f, "XP_lcd_setBL(%s);\n", GenVarCode(buf, MapSym(vectorIntCode[i].name3), NULL, GENVARCODE_MODE_READ));
					break;
				}
                break;
			}

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

static void ExportVarsAsCSV(string filename)
{
    char exportFile[MAX_PATH];

	if(filename.size() == 0) { // Nome nao definido, retorna...
		return;
	}

	strcpy(exportFile, filename.c_str());
	ChangeFileExtension(exportFile, "var.csv");

	if(!strlen(exportFile))
		return;

    FILE *f = fopen(exportFile, "w");
    if(!f) {
        return;
    }

	// Exporta o mapa de registradores
	char buf[100];
	unsigned int list, i;
	char reg[MAX_NAME_LEN], bit[MAX_NAME_LEN];

	csvSaveField(f, _("Registradores de Data/Hora"));
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Dia"));
	sprintf(buf, "%d", INTREG_DATETIME_START + 0);
	csvSaveField(f, buf);
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Mês"));
	sprintf(buf, "%d", INTREG_DATETIME_START + 1);
	csvSaveField(f, buf);
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Ano"));
	sprintf(buf, "%d", INTREG_DATETIME_START + 2);
	csvSaveField(f, buf);
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Hora"));
	sprintf(buf, "%d", INTREG_DATETIME_START + 4);
	csvSaveField(f, buf);
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Minuto"));
	sprintf(buf, "%d", INTREG_DATETIME_START + 5);
	csvSaveField(f, buf);
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Segundo"));
	sprintf(buf, "%d", INTREG_DATETIME_START + 6);
	csvSaveField(f, buf);
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Dia da Semana"));
	sprintf(buf, "%d", INTREG_DATETIME_START + 7);
	csvSaveField(f, buf);
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Dia do Ano"));
	sprintf(buf, "%d", INTREG_DATETIME_START + 8);
	csvSaveField(f, buf);
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Obs.: A hora somente é atualizada quando é realizada a leitura do registrador Dia"));
	fwrite("\n", 1, 1, f);

	// Quebra de linha para separar as secoes
	csvSaveField(f, "");
	fwrite("\n", 1, 1, f);

	// Salva as entradas
	csvSaveField(f, _("Entradas Digitais"));
	fwrite("\n", 1, 1, f);

	vector<string> listIO = ladder->getListIO();
	for(i = 0; i < listIO.size(); i++) {
		mapDetails detailsIO = ladder->getDetailsIO(listIO[i]);
		if(detailsIO.type == eType_DigInput && !IoMap_IsModBUS(detailsIO)) {
			sprintf(buf, "%d", detailsIO.pin);
			csvSaveField(f, listIO[i]);
			csvSaveField(f, buf);
			fwrite("\n", 1, 1, f);
		}
	}

	// Quebra de linha para separar as secoes
	csvSaveField(f, "");
	fwrite("\n", 1, 1, f);

	// Salva as entradas
	csvSaveField(f, _("Saídas Digitais"));
	fwrite("\n", 1, 1, f);

	listIO = ladder->getListIO();
	for(i = 0; i < listIO.size(); i++) {
		mapDetails detailsIO = ladder->getDetailsIO(listIO[i]);
		if(detailsIO.type == eType_DigOutput && !IoMap_IsModBUS(detailsIO)) {
			sprintf(buf, "%d", detailsIO.pin);
			csvSaveField(f, listIO[i]);
			csvSaveField(f, buf);
			fwrite("\n", 1, 1, f);
		}
	}

	// Quebra de linha para separar as secoes
	csvSaveField(f, "");
	fwrite("\n", 1, 1, f);

	// Salva os registradores
	csvSaveField(f, _("Registradores:"));
	fwrite("\n", 1, 1, f);

	csvSaveField(f, _("Nome"));
	csvSaveField(f, _("Registrador"));
	csvSaveField(f, _("Bit"));
	fwrite("\n", 1, 1, f);

	// A seguir fazemos o loop entre todas as variaveis cadastradas, salvando no CSV...
	for(list = 0; list < SEENVAR_LISTS; list++) {
		// Apenas sao acessiveis as variaveis de usuario
		if(list != SEENVAR_MODE_USERBIT && list != SEENVAR_MODE_USERINT) continue;

		for(i = 0; i < (unsigned int)SeenVariablesCount[list]; i++) {
			switch(list) {
				case SEENVAR_MODE_USERBIT:
					sprintf(reg, "%d", INTREG_USERBIT_START + (i/32));
					sprintf(bit, "%d", i%32);
					break;

				case SEENVAR_MODE_USERINT:
					sprintf(reg, "%d", INTREG_USERINT_START + i);
					strcpy (bit, "-");
					break;
			}

			csvSaveField(f, (char *)(SeenVariables[list][i]) + 2);
			csvSaveField(f, reg);
			csvSaveField(f, bit);
			fwrite("\n", 1, 1, f);
		}
	}

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
	char szMakefile[MAX_PATH]      = "";
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

	eModelPLC plcModel = ladder->getSettingsGeneral().model;

	if(plcModel == eModelPLC_POP7) {
		strcpy(szMakefile, "/src/pop7/\"");
	} else {
		strcpy(szMakefile, "/src/pop8/\"");
	}

	strcat(szArgs, " /C \"\""); 
	strcat(szArgs, szAppDirectory); // C:\Users\ADMINI~1\DOCUME~1\VISUAL~3\Projects\POPTools\bin
	strcat(szArgs, "\\gcc\\bin\\cs-make.exe\" -f compile -C \""); 
	strcat(szArgs, ConvertToUNIXPath(szAppDirectory));
	strcat(szArgs, ConvertToUNIXPath(szMakefile));
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

	sprintf(OutputLog, "%s\\POPTools\\output.log", szAppOutputDir);
	strcat(szArgs, "> \"");
	strcat(szArgs, OutputLog);
	strcat(szArgs, "\" 2>&1");

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

DWORD GenerateCFile(char *filename)
{
	unsigned int i, ad_mask, boardIndex;

	vectorIntCode = ladder->getVectorIntCode();
	IntCodeLen = vectorIntCode.size();

	if(ladder->Validate() == eValidateResult_Error) {
		return 0;
	}

	FILE *f = fopen(filename, "w");
    if(!f) {
        Error(_("Não pode abrir o arquivo '%s'\n"), filename);
        return 0;
    }

	LadderSettingsUART               settingsUart    = ladder->getSettingsUART              ();
	LadderSettingsNetwork            settingsNetwork = ladder->getSettingsNetwork           ();
	LadderSettingsSNTP               settingsSntp    = ladder->getSettingsSNTP              ();
	LadderSettingsEncoderIncremental settingsEncInc  = ladder->getSettingsEncoderIncremental();
	LadderSettingsEncoderSSI         settingsEncSSI  = ladder->getSettingsEncoderSSI        ();
	LadderSettingsModbusSlave        settingsMbSlave = ladder->getSettingsModbusSlave       ();
	LadderSettingsInformation        settingsInfo    = ladder->getSettingsInformation       ();

    if(setjmp(CompileErrorBuf) != 0) {
        fclose(f);
		remove(filename);
        return 0;
    }

	HasPWM = 0;
	HasADC = 0;
	HasDAC = 0;
	HasQEI = 0;
	HasSSI = 0;

	mapExpansionIO.clear();
	for(i = 0; i < SEENVAR_LISTS; i++) {
	    SeenVariablesCount[i] = 0;
	}

	MODBUS_RS485_MASTER = 0;
	MODBUS_TCP_MASTER = 0;

	fprintf(f, "/*****************************************************************************\n");
	fprintf(f, " * Tecnequip Tecnologia em Equipamentos Ltda                                 *\n");
	fprintf(f, " *****************************************************************************/\n");

	fprintf(f, "#include \"poptools.h\"\n");

	fprintf(f, "\n");
	fprintf(f, "// Prototipo para a funcao que inicializa um ciclo de execucao\n");
	fprintf(f, "void Cycle_Init(void);\n");

	fprintf(f, "\n");
	fprintf(f, "// Flag que indica se o ModBUS na interface RS-485 funcionara como master (true) ou slave (false)\n");
	fprintf(f, "extern volatile unsigned char 	MODBUS_RS485_MASTER; // 0 = Slave, 1 = Master\n");
	fprintf(f, "// Flag que indica se o ModBUS na interface Ethernet funcionara como master (true) ou slave (false)\n");
	fprintf(f, "extern volatile unsigned char 	MODBUS_TCP_MASTER; // 0 = Slave, 1 = Master\n");
	fprintf(f, "// Vetor com os registradores acessiveis pelo ModBUS (Holding Registers)\n");
	fprintf(f, "extern volatile int 			MODBUS_REGISTER[32];\n");
	fprintf(f, "// Ultima hora lida do RTC, atualizada ao iniciar um ciclo de execucao\n");
	fprintf(f, "extern struct tm 				RTC_NowTM;\n");
	fprintf(f, "// As Variaveis a seguir armazenam a data/hora inicial e final (respectivamente) de um elemento de\n");
	fprintf(f, "// agendamento de tarefa, compartilhado com todos os elementos para economizar memoria.\n");
	fprintf(f, "struct tm 						RTC_StartTM;\n");
	fprintf(f, "struct tm 						RTC_EndTM;\n\n");

	vector<string> vInternalFlags = ladder->getVectorInternalFlagsIO();
	for(i=0; i < vInternalFlags.size(); i++) {
		fprintf(f, "extern volatile unsigned int 	I_%s;\n", vInternalFlags[i].c_str());
	}

	fprintf(f, "\n");
	fprintf(f, "extern struct ip_addr 			IP_ADDRESS;\n");
	fprintf(f, "extern struct ip_addr 			IP_NETMASK;\n");
	fprintf(f, "extern struct ip_addr 			IP_GATEWAY;\n");
	fprintf(f, "extern struct ip_addr			IP_DNS;\n");

	fprintf(f, "\n");
	fprintf(f, "char 							SNTP_SERVER_ADDRESS[] = \"%s\";\n", settingsSntp.sntp_server.c_str());
	fprintf(f, "int								SNTP_GMT = %d;\n", settingsSntp.gmt - 12);
	fprintf(f, "int								SNTP_DAILY_SAVE = %d;\n", settingsSntp.dailysave);

	fprintf(f, "\n");

	// then generate the expansion boards list
	char *boardName;
	vector<tExpansionBoardItem> boards = ladder->getBoardList();

	fprintf(f, "struct strExpansionBoard expansionBoards[] = {\n");

	for(i=0, boardIndex=0; i < boards.size(); i++) {
		switch(boards[i].type) {
			case eExpansionBoard_DigitalInput : boardName = "eBoardType_Input" ; break;
			case eExpansionBoard_DigitalOutput: boardName = "eBoardType_Output"; break;
			case eExpansionBoard_AnalogInput  : boardName = "eBoardType_AD"    ; break;
			default: boardName = NULL;
		}

		if(boardName != NULL) {
			mapExpansionIO[boards[i].id] = boardIndex++;
			fprintf(f, "    { %s, %d, { { 0 } }, %d, %d },\n", boardName, (boards[i].address >> 1) & 0x7F, 0 /* Canal !?? */, boards[i].useIRQ);
		}
	}

	fprintf(f, "    { eBoardType_None, 0, { { 0 } }, 0, 0 }\n};\n");

	fprintf(f, "\n");

	// now generate declarations for all variables
	fprintf(f, "// Variaveis PLC\n");
    GenerateDeclarations(f);

	fprintf(f, "\n");

	fprintf(f, "// Funcao que executa um ciclo de processamento da logica criada pelo usuario\n");
    fprintf(f,"void PLC_Run(void)\n{\n");

    GenerateAnsiC(f, ad_mask);

    fprintf(f, "}\n");

	fprintf(f, "\n");

	fprintf(f, "// Funcao que contem os passos adicionais para inicializacao da Ethernet, quando necessario\n");
	fprintf(f, "// Ex.: Configuracao de servidores utilizados para sincronismo de data/hora via SNTP (Quando em uso)\n");
	fprintf(f, "void PLC_Net_Init(void)\n");
	fprintf(f, "{\n");

	if(settingsSntp.sntp_enable) {
		fprintf(f, "    SNTP_Init();\n");
	}

	fprintf(f, "}\n");

	fprintf(f, "\n");

	fprintf(f, "// Funcao que inicializa o controlador, executado apenas ao ligar o equipamento\n");
	fprintf(f, "void PLC_Init(void)\n");
	fprintf(f, "{\n");
	fprintf(f, "	MODBUS_RS485_MASTER = %d;\n", MODBUS_RS485_MASTER);
	fprintf(f, "	MODBUS_TCP_MASTER = %d;\n", MODBUS_TCP_MASTER);
	fprintf(f, "	ModBUS_SetID(%d);\n", settingsMbSlave.ModBUSID);
	fprintf(f, "	ModBUS_SetAppName(\"%s\");\n", settingsInfo.Name.c_str());
	fprintf(f, "\n");
	fprintf(f, "	RS485_Config(%d, %d, %d, %d);\n", settingsUart.baudRate, SerialConfig[settingsUart.UART].bByteSize,
		SerialConfig[settingsUart.UART].bParity, SerialConfig[settingsUart.UART].bStopBits == ONESTOPBIT ? 1 : 2);
	fprintf(f, "\n");
	fprintf(f, "	IP4_ADDR(&IP_ADDRESS, %d,%d,%d,%d);\n", FIRST_IPADDRESS(settingsNetwork.ip), SECOND_IPADDRESS(settingsNetwork.ip),
		THIRD_IPADDRESS(settingsNetwork.ip), FOURTH_IPADDRESS(settingsNetwork.ip));
	fprintf(f, "	IP4_ADDR(&IP_NETMASK, %d,%d,%d,%d);\n", FIRST_IPADDRESS(settingsNetwork.mask), SECOND_IPADDRESS(settingsNetwork.mask),
		THIRD_IPADDRESS(settingsNetwork.mask), FOURTH_IPADDRESS(settingsNetwork.mask));
	fprintf(f, "	IP4_ADDR(&IP_GATEWAY, %d,%d,%d,%d);\n", FIRST_IPADDRESS(settingsNetwork.gw), SECOND_IPADDRESS(settingsNetwork.gw),
		THIRD_IPADDRESS(settingsNetwork.gw), FOURTH_IPADDRESS(settingsNetwork.gw));
	fprintf(f, "	IP4_ADDR(&IP_DNS, %d,%d,%d,%d);\n", FIRST_IPADDRESS(settingsNetwork.dns), SECOND_IPADDRESS(settingsNetwork.dns),
		THIRD_IPADDRESS(settingsNetwork.dns), FOURTH_IPADDRESS(settingsNetwork.dns));

	if(HasQEI) {
		fprintf(f, "\n");
		// Incremental Encoder
		fprintf(f, "    ENC_Config(%d, %d, %d, %f, %d);\n", 0, settingsEncInc.conv_mode,
			settingsEncInc.perimeter, settingsEncInc.factor, settingsEncInc.pulses * (settingsEncInc.x4 ? 4 : 2));

		if(settingsEncInc.x4) {
			fprintf(f, "    QEIConfig.CaptureMode = QEI_CAPMODE_4X;\n");
		} else {
			fprintf(f, "    QEIConfig.CaptureMode = QEI_CAPMODE_2X;\n");
		}

		fprintf(f, "\n    QEI_Init();\n");
	}

	if(HasSSI) {
		fprintf(f, "\n");
		// Absolute Encoder
		fprintf(f, "    ENC_Config(%d, %d, %d, %f, %d);\n", 1, settingsEncSSI.conv_mode,
			settingsEncSSI.perimeter, settingsEncSSI.factor, settingsEncSSI.size_bpr);

		fprintf(f, "    SSI_Init(%d, %d);\n", settingsEncSSI.size, settingsEncSSI.mode);
	}

	if(HasADC) {
		fprintf(f, "\n    ADC_Init();\n");
		fprintf(f, "\n    ADC_SetMask(%d);\n", ad_mask);
	}

	if(HasDAC) {
		fprintf(f, "\n    DAC_Init();\n");
	}

	if(HasPWM) {
		fprintf(f, "\n    PWM_Init();\n");
	}

	for(i=0; i < boards.size(); i++) {
		switch(boards[i].type) {
		case eExpansionBoard_LCD:
			fprintf(f, "\n    XP_lcd_Init(%d, %d);\n", boards[i].address, boards[i].version);
			break;
		}
	}

	fprintf(f, "\n");
	for(i=0; i != SEENVAR_MODE_OTHERS; i++) {
		if(SeenVariablesCount[i]) {
			fprintf(f, "    memset((void*)%s, 0, sizeof(%s));\n", NameVarArray[i], NameVarArray[i]);
		}
	}

	fprintf(f, "}\n");
	fprintf(f, "\n");

	fprintf(f, "// Funcao que contem os passos adicionais para inicializacao de um ciclo de execucao, quando necessario\n");
	fprintf(f, "// Ex.: Atualizacao da leitura dos canais A/D (Quando em uso)\n");
	fprintf(f, "void Cycle_Init(void)\n");
	fprintf(f, "{\n");
	if(HasADC) {
		fprintf(f, "	ADC_Update();\n");
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
	sprintf(CurrentCompileFile, "%s\\poptools.hex", szAppSourceFile);
	strcat(szAppSourceFile, "\\poptools.c");

	if(!GenerateCFile(szAppSourceFile))
		return 1;

	if(POPSettings.GenerateMemoryMap == true) {
		ExportVarsAsCSV(ladder->getCurrentFilename());
	}

	DWORD err = 0;

	if (!(err = InvokeGCC(CurrentCompileFile))) {
		LadderSettingsDetails settings = ladder->getSettingsDetails();
		settings.BuildNumber++;
		settings.CompileDate = time(NULL);
		ladder->setSettingsDetails(settings);
	}

	return err;
}
