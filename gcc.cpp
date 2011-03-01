//-----------------------------------------------------------------------------
// Copyright 2007 Jonathan Westhues
//
// This file is part of LDmicro.
// 
// LDmicro is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// LDmicro is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with LDmicro.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// Write the program as ANSI C source. This is very simple, because the
// intermediate code structure is really a lot like C. Someone else will be
// responsible for calling us with appropriate timing.
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <Shlobj.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

#include "ldmicro.h"
#include "intcode.h"

static char SeenVariables[MAX_IO][MAX_NAME_LEN];
static int SeenVariablesCount;

static int MODBUS_MASTER = 0;

//#define INT_UNSIGNED	

//-----------------------------------------------------------------------------
// Have we seen a variable before? If not then no need to generate code for
// it, otherwise we will have to make a declaration, and mark it as seen.
//-----------------------------------------------------------------------------
static BOOL SeenVariable(char *name)
{
    int i;
    for(i = 0; i < SeenVariablesCount; i++) {
        if(strcmp(SeenVariables[i], name)==0) {
            return TRUE;
        }
    }
    if(i >= MAX_IO) oops();
    strcpy(SeenVariables[i], name);
    SeenVariablesCount++;
    return FALSE;
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
		if (strcmp(Prog.io.assignment[i].name, str) == 0)
		{
			int pin = Prog.io.assignment[i].pin;

			if (Prog.io.assignment[i].type == IO_TYPE_DIG_OUTPUT) 
			{
				if (pin > 18)
				{
					sprintf(ret, "M[%d]", pin - 20);
					return ret;
				}
				else
				{
					sprintf(ret, "U%d", pin);
					return ret;
				}
			} 
			if (Prog.io.assignment[i].type == IO_TYPE_READ_ADC) 
			{
				sprintf(ret, "A%d", pin);
				return ret;
			} 
			/*if (Prog.io.assignment[i].type == IO_TYPE_READ_ENC || Prog.io.assignment[i].type == IO_TYPE_RESET_ENC) 
			{
				sprintf(ret, "ENC%d", pin);
				return ret;
			}*/
			else if (Prog.io.assignment[i].type == IO_TYPE_DIG_INPUT) 
			{
				if (pin > 19)
				{
					sprintf(ret, "M[%d]", pin - 20);
					return ret;
				}
				else
				{
					sprintf(ret, "I%d", pin);
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
static void DeclareInt(FILE *f, char *str)
{
	if (strncmp(MapSym(str), "E", 1) == 0 || strncmp(MapSym(str), "Z", 1) == 0 || strncmp(MapSym(str), "M", 1) == 0 || isdigit(*str) || *str == '-')
	{
		return;
	}

	if (strcmp(str, "I_SerialReady") == 0) return;

    //fprintf(f, "STATIC SWORD %s;\n", MapSym(str));
#ifdef INT_UNSIGNED
	fprintf(f, "volatile unsigned int %s = 0;\n", MapSym(str));
#else
	fprintf(f, "volatile int %s = 0;\n", MapSym(str));
#endif
}

//-----------------------------------------------------------------------------
// Generate a declaration for a bit var; three cases, input, output, and
// internal relay. An internal relay is just a BOOL variable, but for an
// input or an output someone else must provide read/write functions.
//-----------------------------------------------------------------------------
static void DeclareBit(FILE *f, char *rawStr)
{
    char *str = MapSym(rawStr);
	if (strncmp(str, "M", 1) == 0)
	{
		return;
	} else if(*rawStr == 'X') {
        //fprintf(f, "\n");
        //fprintf(f, "/* You provide this function. */\n");
        //fprintf(f, "PROTO(extern BOOL Read_%s(void);)\n", str);
        //fprintf(f, "\n");
    } else if(*rawStr == 'Y') {
        //fprintf(f, "\n");
        //fprintf(f, "/* You provide these functions. */\n");
        //fprintf(f, "PROTO(BOOL Read_%s(void);)\n", str);
        //fprintf(f, "PROTO(void Write_%s(BOOL v);)\n", str);
        //fprintf(f, "\n");
    } else if(*rawStr == 'P' || *rawStr == 'M') {
		//fprintf(f, "\n");
		//fprintf(f, "volatile unsigned int %s = 0;\n", str);
		//fprintf(f, "#define Read_%s(x) %s = ReadParameter(atoi(%s[1]))\n", str, str);
		//fprintf(f, "#define Write_%s(x) WriteParameter(atoi(%s[1]), %s)\n", str, str);
		return;
    } else if(*rawStr == 'W' || *rawStr == 'G') {
		return;
    } else if (strcmp(str, "I_SerialReady") == 0) 
		return;

	fprintf(f, "\n");
#ifdef INT_UNSIGNED
	fprintf(f, "volatile unsigned int %s = 0;\n", str);
#else
	fprintf(f, "volatile int %s = 0;\n", str);
#endif

	//fprintf(f, "#define Read_%s() %s\n", str, str);
	//fprintf(f, "#define Write_%s(x) %s = x\n", str, str);
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
			case INT_SET_SINGLE_BIT:
			case INT_CLEAR_SINGLE_BIT:
                bitVar1 = IntCode[i].name1;
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
            case INT_READ_MODBUS_ETH:
            case INT_WRITE_MODBUS_ETH:
            case INT_SET_PWM:
                intVar1 = IntCode[i].name1;
                break;

            case INT_READ_MODBUS:
            case INT_WRITE_MODBUS:
				MODBUS_MASTER = 1;
                intVar1 = IntCode[i].name1;
                break;

            case INT_UART_RECV:
            case INT_UART_SEND:
                intVar1 = IntCode[i].name1;
                bitVar1 = IntCode[i].name2;
                break;

            case INT_IF_BIT_SET:
            case INT_IF_BIT_CLEAR:
			case INT_IF_BIT_CHECK_SET:
			case INT_IF_BIT_CHECK_CLEAR:
                bitVar1 = IntCode[i].name1;
                break;

            case INT_IF_VARIABLE_LES_LITERAL:
                intVar1 = IntCode[i].name1;
                break;

            case INT_IF_VARIABLE_EQUALS_VARIABLE:
            case INT_IF_VARIABLE_GRT_VARIABLE:
                intVar1 = IntCode[i].name1;
                intVar2 = IntCode[i].name2;
                break;

            case INT_END_IF:
            case INT_ELSE:
            case INT_COMMENT:
            case INT_SIMULATE_NODE_STATE:
            case INT_EEPROM_BUSY_CHECK:
            case INT_EEPROM_READ:
            case INT_EEPROM_WRITE:
                break;

            default:
                oops();
        }
        if(bitVar1 && !SeenVariable(bitVar1)) DeclareBit(f, bitVar1);
        if(bitVar2 && !SeenVariable(bitVar2)) DeclareBit(f, bitVar2);

        if(intVar1 && !SeenVariable(intVar1)) DeclareInt(f, intVar1);
        if(intVar2 && !SeenVariable(intVar2)) DeclareInt(f, intVar2);
        if(intVar3 && !SeenVariable(intVar3)) DeclareInt(f, intVar3);
    }
}

//-----------------------------------------------------------------------------
// Actually generate the C source for the program.
//-----------------------------------------------------------------------------
static void GenerateAnsiC(FILE *f)
{
    int i;
    int indent = 1;
    for(i = 0; i < IntCodeLen; i++) {

        if(IntCode[i].op == INT_END_IF) indent--;
        if(IntCode[i].op == INT_ELSE) indent--;

        int j;
        for(j = 0; j < indent; j++) fprintf(f, "    ");

		char *str = MapSym(IntCode[i].name1);

        switch(IntCode[i].op) {
            case INT_SET_BIT:
				if (strncmp(str, "M[", 2) == 0)
					fprintf(f, "%s |= (1 << %d);  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
	                fprintf(f, "%s = 1;\n", MapSym(IntCode[i].name1));
                break;

            case INT_CLEAR_BIT:
				if (strncmp(str, "M[", 2) == 0)
					fprintf(f, "%s &= ~(1 << %d);  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
	                fprintf(f, "%s = 0;\n", MapSym(IntCode[i].name1));
                break;

			case INT_SET_SINGLE_BIT:
                fprintf(f, "%s |= 1 << %d;\n", MapSym(IntCode[i].name1), IntCode[i].bit, MapSym(IntCode[i].name1), IntCode[i].bit);
                break;

            case INT_CLEAR_SINGLE_BIT:
                fprintf(f, "%s &= ~(1 << %d);\n", MapSym(IntCode[i].name1), IntCode[i].bit);
                break;

            case INT_COPY_BIT_TO_BIT:
				if (strncmp(str, "M[", 2) == 0)
	                fprintf(f, "%s &= ~(1 << %d); %s |= %s << %d;  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, MapSym(IntCode[i].name1), MapSym(IntCode[i].name2), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "%s = %s;\n", MapSym(IntCode[i].name1), MapSym(IntCode[i].name2));
                break;

            case INT_SET_VARIABLE_TO_LITERAL:
                fprintf(f, "%s = %d;\n", MapSym(IntCode[i].name1),
                    IntCode[i].literal);
                break;

            case INT_SET_VARIABLE_TO_VARIABLE:
                fprintf(f, "%s = %s;\n", MapSym(IntCode[i].name1),
                    MapSym(IntCode[i].name2));
                break;

            {
                char op;
                case INT_SET_VARIABLE_ADD: op = '+'; goto arith;
                case INT_SET_VARIABLE_SUBTRACT: op = '-'; goto arith;
                case INT_SET_VARIABLE_MULTIPLY: op = '*'; goto arith;
                case INT_SET_VARIABLE_DIVIDE: op = '/'; goto arith;
                arith:
                    fprintf(f, "%s = %s %c %s;\n",
                        MapSym(IntCode[i].name1),
                        MapSym(IntCode[i].name2),
                        op,
                        MapSym(IntCode[i].name3) );
                    break;
            }

            case INT_INCREMENT_VARIABLE:
                fprintf(f, "%s++;\n", MapSym(IntCode[i].name1));
                break;

            case INT_IF_BIT_SET:
				if (strncmp(str, "M[", 2) == 0)
	                fprintf(f, "if (%s & (1 << %d)) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "if (%s) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CLEAR:
				if (strncmp(str, "M[", 2) == 0)
	                fprintf(f, "if (!(%s & (1 << %d))) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "if (!%s) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CHECK_SET:
				if (strncmp(str, "M[", 2) == 0)
	                fprintf(f, "if (%s & (1 << %d)) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "if (%s & (1 << %d)) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
                indent++;
                break;

            case INT_IF_BIT_CHECK_CLEAR:
				if (strncmp(str, "M[", 2) == 0)
	                fprintf(f, "if (!(%s & (1 << %d))) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "if (!(%s & (1 << %d))) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
                indent++;
                break;

            case INT_IF_VARIABLE_LES_LITERAL:
                fprintf(f, "if (%s < %d) {\n", MapSym(IntCode[i].name1), 
                    IntCode[i].literal);
                indent++;
                break;

            case INT_IF_VARIABLE_EQUALS_VARIABLE:
                fprintf(f, "if (%s == %s) {\n", MapSym(IntCode[i].name1),
                    MapSym(IntCode[i].name2));
                indent++;
                break;

            case INT_IF_VARIABLE_GRT_VARIABLE:
                fprintf(f, "if (%s > %s) {\n", MapSym(IntCode[i].name1),
                    MapSym(IntCode[i].name2));
                indent++;
                break;

            case INT_END_IF:
                fprintf(f, "}\n");
                break;

            case INT_ELSE:
                fprintf(f, "} else {\n"); indent++;
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
				break;
            case INT_EEPROM_READ:
				break;
            case INT_EEPROM_WRITE:
				break;
            case INT_READ_ADC:
				fprintf(f, "%s = ADCRead(%d);\n", MapSym(IntCode[i].name1), atoi(MapSym(IntCode[i].name1) + 1));
				break;
            case INT_SET_DA:
				fprintf(f, "DAC_Write(%s);\n", MapSym(IntCode[i].name1));
				break;
            case INT_READ_ENC:
				fprintf(f, "%s = ENCRead();\n", MapSym(IntCode[i].name1));
				break;
            case INT_RESET_ENC:
				fprintf(f, "ENCReset();\n");
				break;
			case INT_READ_FORMATTED_STRING:
				fprintf(f, "read_formatted_string(\"%s\", &%s);\n", IntCode[i].name2, MapSym(IntCode[i].name1));
				break;
			case INT_WRITE_FORMATTED_STRING:
				fprintf(f, "write_formatted_string(\"%s\", &%s);\n", IntCode[i].name2, MapSym(IntCode[i].name1));
				break;
			case INT_READ_SERVO_YASKAWA:
				fprintf(f, "read_servo_yaskawa(\"%s\", \"%s\", &%s);\n", IntCode[i].name3, IntCode[i].name2, MapSym(IntCode[i].name1));
				break;
			case INT_WRITE_SERVO_YASKAWA:
				fprintf(f, "write_servo_yaskawa(\"%s\", \"%s\", &%s);\n", IntCode[i].name3, IntCode[i].name2, MapSym(IntCode[i].name1));
				break;
            case INT_READ_USS:
				fprintf(f, "uss_get_param(%d, %d, %d, %d, &%s);\n", atoi(IntCode[i].name2), atoi(IntCode[i].name3), atoi(IntCode[i].name4), IntCode[i].literal, MapSym(IntCode[i].name1));
				break;
            case INT_WRITE_USS:
				fprintf(f, "uss_set_param(%d, %d, %d, %d, &%s);\n", atoi(IntCode[i].name2), atoi(IntCode[i].name3), atoi(IntCode[i].name4), IntCode[i].literal, MapSym(IntCode[i].name1));
				break;
            case INT_READ_MODBUS:
				fprintf(f, "modbus_send(%d, MB_FC_READ_HOLDING_REGISTERS, %d, %d, &%s);\n", atoi(IntCode[i].name2), atoi(IntCode[i].name3), IntCode[i].bit + 1, MapSym(IntCode[i].name1));
				break;
            case INT_WRITE_MODBUS:
				fprintf(f, "modbus_send(%d, MB_FC_WRITE_MULTIPLE_REGISTERS, %d, %d, &%s);\n", atoi(IntCode[i].name2), atoi(IntCode[i].name3), IntCode[i].bit + 1, MapSym(IntCode[i].name1));
				break;
            case INT_READ_MODBUS_ETH:
				fprintf(f, "modbus_tcp_send(%d, MB_FC_READ_HOLDING_REGISTERS, %d, %d, &%s);\n", atoi(IntCode[i].name2), atoi(IntCode[i].name3), IntCode[i].bit + 1, MapSym(IntCode[i].name1));
				break;
            case INT_WRITE_MODBUS_ETH:
				fprintf(f, "modbus_tcp_send(%d, MB_FC_WRITE_MULTIPLE_REGISTERS, %d, %d, &%s);\n", atoi(IntCode[i].name2), atoi(IntCode[i].name3), IntCode[i].bit + 1, MapSym(IntCode[i].name1));
				break;
            case INT_SET_PWM:
				break;
            case INT_UART_RECV:
				break;
            case INT_UART_SEND:
				fprintf(f, "%s = RS232Write(%s);\n", MapSym(IntCode[i].name2), MapSym(IntCode[i].name1));
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

	for (i = strlen(dest); i; i--)
	{
		if (dest[i] == '\\')
			dest[i] = '/';
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
	
	char szAppProgramFiles[MAX_PATH]   = "";
	char szToolsProgramFiles[MAX_PATH]   = "";
	char szAppPath[MAX_PATH]      = "";
	char szTmpPath[MAX_PATH]		= "";
	char szTempPath[MAX_PATH]		= "";
	char szAppDirectory[MAX_PATH] = "";
	char szAppDestPath[MAX_PATH]  = "";
	char szAppOutputDir[MAX_PATH] = "";
	char * fileName;

	if(!SUCCEEDED(SHGetSpecialFolderPath(0, szAppProgramFiles, CSIDL_PROGRAM_FILES, FALSE))) 
		return GetLastError();

	strcpy(szToolsProgramFiles, szAppProgramFiles);

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	GetTempPath(sizeof(szTmpPath), szTmpPath);

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

	// "/C" option - Do the command then terminate the command window
	// "/K" option - Wait exit 
	strcat(szArgs, " /C \""); 

	//the application you would like to run from the command window
	strcat(szArgs, "\"");
	strcat(szArgs, szAppDirectory);
#ifdef _DEBUG
	strcat(szArgs, "\\src\\gcc\\bin\\make.exe\" -f makefile -C \"");
#else
	strcat(szArgs, "\\src\\gcc\\bin\\make.exe\" -f compile -C \"");
#endif
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

	strcat(szArgs, ">> \"");
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

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	if(!GetExitCodeProcess(ProcessInfo.hProcess, &exitCode))
		exitCode = 0;

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);

	return exitCode;
}

DWORD CompileAnsiCToGCC(char *dest)
{
    SeenVariablesCount = 0;

	char szAppPath[MAX_PATH]		= "";
	char szTempPath[MAX_PATH]		= "";
	char szAppHeader[MAX_PATH]		= "";
	char szAppSourceFile[MAX_PATH]	= "";

	MODBUS_MASTER = 0;

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

	// Extract directory
	//strncpy(szAppHeader, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
	strncpy(szAppHeader, szTempPath, strrchr(szTempPath, '\\') - szTempPath);
	szAppHeader[strlen(szAppHeader)] = '\0';
	//strcat(szAppHeader, "\\src\\ld.h");
	strcat(szAppHeader, "\\ld.h");

	FILE *h = fopen(szAppHeader, "w");
    if(!h) {
        Error(_("Couldn't open file '%s'"), szAppHeader);
        return 1;
    }

	fprintf(h, "/*=========================================================================*/\n");
	fprintf(h, "/*  DEFINE: All code exported                                              */\n");
	fprintf(h, "/*=========================================================================*/\n\n");

	fprintf(h, "volatile unsigned int I1 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I2 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I3 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I4 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I5 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I6 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I7 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I8 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I9 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I10 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I11 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I12 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I13 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I14 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I15 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I16 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I17 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I18 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int I19 __attribute__((weak)) = 0;\n\n");

	fprintf(h, "volatile unsigned int U1 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U2 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U3 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U4 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U5 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U6 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U7 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U8 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U9 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U10 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U11 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U12 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U13 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U14 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U15 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U16 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U17 __attribute__((weak)) = 0;\n");
	fprintf(h, "volatile unsigned int U18 __attribute__((weak)) = 0;\n\n");

#ifdef INT_UNSIGNED
	fprintf(h, "volatile unsigned int M[32] __attribute__((weak));\n");
#else
	fprintf(h, "volatile int M[32] __attribute__((weak));\n");
#endif
	fprintf(h, "volatile int ENC1 __attribute__((weak)) = 0;\n\n");

	fprintf(h, "volatile unsigned char MODBUS_MASTER __attribute__((weak)) = 0;\n\n");

	fprintf(h, "extern void PlcCycle(void);\n");
	fprintf(h, "extern volatile unsigned int TIME_INTERVAL;\n");
	fprintf(h, "void ld_Init(void);\n");

	fclose(h);

	//strncpy(szAppSourceFile, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
	strncpy(szAppSourceFile, szTempPath, strrchr(szTempPath, '\\') - szTempPath);
	szAppSourceFile[strlen(szAppSourceFile)] = '\0';
	//strcat(szAppSourceFile, "\\src\\ld.c");
	strcat(szAppSourceFile, "\\ld.c");

    FILE *f = fopen(szAppSourceFile, "w");
    if(!f) {
        Error(_("Couldn't open file '%s'"), szAppSourceFile);
        return 1;
    }

    if(setjmp(CompileErrorBuf) != 0) {
        fclose(f);
        return 1;
    }

    // Set up the TRISx registers (direction). 1 means tri-stated (input).
    BYTE isInput[MAX_IO_PORTS], isOutput[MAX_IO_PORTS];
    BuildDirectionRegisters(isInput, isOutput);

    fprintf(f,
"/*****************************************************************************\n"
" * Tecnequip Tecnologia em Equipamentos Ltda                                 *\n"
" *****************************************************************************/\n"
"#include <string.h>\n"
"#include \"uss.h\"\n"
"#include \"modbus.h\"\n\n"
"extern void DAC_Write(unsigned int val);\n"
"extern void modbus_send(unsigned char id,\n"
"                        int fc,\n"
"                        unsigned short int address,\n"
"                        unsigned short int size,\n"
"                        volatile int * value);\n\n"
"extern void modbus_tcp_send(unsigned char id,\n"
"                        int fc,\n"
"                        unsigned short int address,\n"
"                        unsigned short int size,\n"
"                        volatile int * value);\n\n"
"volatile unsigned int CYCLE_TIME = %d;\n"
"volatile unsigned int TIME_INTERVAL = ((25000000/1000) * %d) - 1;\n"
"volatile int M[32];\n"
"volatile int ENC1;\n\n"
	, Prog.cycleTime / 1000, Prog.cycleTime / 1000);

	fprintf(f, "volatile unsigned char IP_ADDR[4] = { %d, %d, %d, %d };\n", Prog.ip[0], Prog.ip[1], Prog.ip[2], Prog.ip[3]);
	fprintf(f, "volatile unsigned char IP_MASK[4] = { %d, %d, %d, %d };\n", Prog.mask[0], Prog.mask[1], Prog.mask[2], Prog.mask[3]);
	fprintf(f, "volatile unsigned char IP_GW[4] = { %d, %d, %d, %d };\n\n", Prog.gw[0], Prog.gw[1], Prog.gw[2], Prog.gw[3]);

	//int j;

	//for(int i = 0; i < DISPLAY_MATRIX_X_SIZE; i++) 
	//{
	//	for(j = 0; j < DISPLAY_MATRIX_Y_SIZE; j++) 
	//	{
	//		ElemLeaf *l = DisplayMatrix[i][j];
	//		if ((l && DisplayMatrixWhich[i][j] == ELEM_READ_MODBUS)
	//			|| (l && DisplayMatrixWhich[i][j] == ELEM_WRITE_MODBUS)
	//			|| (l && DisplayMatrixWhich[i][j] == ELEM_READ_MODBUS_ETH)
	//			|| (l && DisplayMatrixWhich[i][j] == ELEM_WRITE_MODBUS_ETH)) 
	//		{
	//			fprintf(f, "unsigned char MODBUS_MASTER = 1; // 0=Slave, 1=Master\n\n");
	//			break;
	//		}
	//	}
	//	if (j < DISPLAY_MATRIX_Y_SIZE)
	//		break;
	//}

	fprintf(f, "extern struct MB_Device modbus_master;\n");
	fprintf(f, "extern unsigned int RS232Write(char c);\n");
	fprintf(f, "extern void RS485Config(int baudrate, int bits, int parity, int stopbit);\n");
	fprintf(f, "extern unsigned int ADCRead(unsigned int i);\n");
	fprintf(f, "extern unsigned int ENCRead(void);\n");
	fprintf(f, "extern unsigned int ENCReset(void);\n");
	fprintf(f, "extern volatile unsigned int I_SerialReady;\n\n");

    // now generate declarations for all variables
    GenerateDeclarations(f);

	struct serialtag
	{
		int bits;
		int parity;
		int stopbit;
	} serial;

	switch(Prog.UART)
	{
	case 1: // 8-E-1
		serial.bits = 8;  // 8 bits
		serial.parity = 2; // even
		serial.stopbit = 1; // 1 stopbit
		break;
	case 2: // 8-O-1
		serial.bits = 8;  // 8 bits
		serial.parity = 1; // odd
		serial.stopbit = 1; // 1 stopbit
		break;
	case 3: // 7-N-1
		serial.bits = 7;  // 8 bits
		serial.parity = 0; // even
		serial.stopbit = 1; // 1 stopbit
		break;
	case 4: // 7-E-1
		serial.bits = 7;  // 8 bits
		serial.parity = 2; // even
		serial.stopbit = 1; // 1 stopbit
		break;
	case 5: // 7-O-1
		serial.bits = 7;  // 8 bits
		serial.parity = 1; // odd
		serial.stopbit = 1; // 1 stopbit
		break;
	default: // 8-N-1
		serial.bits = 8;  // 8 bits
		serial.parity = 0; // none
		serial.stopbit = 1; // 1 stopbit
	}
	fprintf(f, "volatile unsigned char MODBUS_MASTER = %d; // 0 = Slave, 1 = Master \n", MODBUS_MASTER);

    fprintf(f,
"\n"
"\n"
"void ld_Init(void)\n"
"{\n"
"  RS485Config(%d, %d, %d, %d);\n"
"}\n\n"
"/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */\n"
"void PlcCycle(void)\n"
"{\n", Prog.baudRate, serial.bits, serial.parity, serial.stopbit);

    GenerateAnsiC(f);

    fprintf(f, "}\n");
    fclose(f);

    char str[MAX_PATH+500];
	DWORD err = 0;

	StatusBarSetText(0, "Compilando... aguarde !");

	if ((err = InvokeGCC(dest)))
	{
		sprintf(str, _("A compilação retornou erro. O código do erro é %d. O arquivo com o log do erro esta na pasta \"C:\\Users\\<User>\\AppData\\Temp\\POPTools\\output.log\"\n"), err);	}
	else
	{
		sprintf(str, _("Compilado com sucesso. O arquivo binário foi criado em '%s'.\r\n\r\n"), dest);
	}

	CompileSuccessfulMessage(str);

	StatusBarSetText(0, "");

	return err;
}
