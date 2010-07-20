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
				if (pin > 19)
				{
					sprintf(ret, "U_M[%d]", pin - 20);
					return ret;
				}
				else
				{
					sprintf(ret, "U_S%d", pin);
					return ret;
				}
			} 
			if (Prog.io.assignment[i].type == IO_TYPE_READ_ADC) 
			{
				sprintf(ret, "U_A%d", pin);
				return ret;
			} 
			if (Prog.io.assignment[i].type == IO_TYPE_READ_ENC || Prog.io.assignment[i].type == IO_TYPE_RESET_ENC) 
			{
				sprintf(ret, "U_ENC%d", pin);
				return ret;
			}
			else if (Prog.io.assignment[i].type == IO_TYPE_DIG_INPUT) 
			{
				if (pin > 19)
				{
					sprintf(ret, "U_M[%d]", pin - 20);
					return ret;
				}
				else
				{
					sprintf(ret, "I_E%d", pin);
					return ret;
				}
			}
		}
	}

    if(*str == '$') {
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
	if (strncmp(MapSym(str), "U_E", 3) == 0)
	{
		return;
	}
    //fprintf(f, "STATIC SWORD %s;\n", MapSym(str));
	fprintf(f, "volatile unsigned int %s = 0;\n", MapSym(str));
}

//-----------------------------------------------------------------------------
// Generate a declaration for a bit var; three cases, input, output, and
// internal relay. An internal relay is just a BOOL variable, but for an
// input or an output someone else must provide read/write functions.
//-----------------------------------------------------------------------------
static void DeclareBit(FILE *f, char *rawStr)
{
    char *str = MapSym(rawStr);
	if (strncmp(str, "U_M[", 4) == 0)
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
    }
	fprintf(f, "\n");
	fprintf(f, "volatile unsigned int %s = 0;\n", str);
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
            case INT_READ_ENC:
            case INT_RESET_ENC:
            case INT_SET_PWM:
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
				if (strncmp(str, "U_M[", 4) == 0)
					fprintf(f, "%s |= (1 << %d);  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
	                fprintf(f, "%s = 1;\n", MapSym(IntCode[i].name1));
                break;

            case INT_CLEAR_BIT:
				if (strncmp(str, "U_M[", 4) == 0)
					fprintf(f, "%s &= ~(1 << %d);  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
	                fprintf(f, "%s = 0;\n", MapSym(IntCode[i].name1));
                break;

            case INT_COPY_BIT_TO_BIT:
				if (strncmp(str, "U_M[", 4) == 0)
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
				if (strncmp(str, "U_M[", 4) == 0)
	                fprintf(f, "if (%s & (1 << %d)) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "if (%s) {\n", MapSym(IntCode[i].name1));
                indent++;
                break;

            case INT_IF_BIT_CLEAR:
				if (strncmp(str, "U_M[", 4) == 0)
	                fprintf(f, "if (!(%s & (1 << %d))) {  // %s\n", MapSym(IntCode[i].name1), IntCode[i].bit, IntCode[i].name1);
				else
					fprintf(f, "if (!%s) {\n", MapSym(IntCode[i].name1));
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
				fprintf(f, "%s = ADCRead(%d);\n", MapSym(IntCode[i].name1), atoi(MapSym(IntCode[i].name1) + 3));
				break;
            case INT_READ_ENC:
				fprintf(f, "%s = ENCRead(%d);\n", MapSym(IntCode[i].name1), atoi(MapSym(IntCode[i].name1)));
				break;
            case INT_RESET_ENC:
				fprintf(f, "ENCReset(%d);\n", atoi(MapSym(IntCode[i].name1)));
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
	char szAppDirectory[MAX_PATH] = "";
	char szAppDestPath[MAX_PATH]  = "";
	char szAppOutputDir[MAX_PATH]    = "";
	char * fileName;

	if(!SUCCEEDED(SHGetSpecialFolderPath(0, szAppProgramFiles, CSIDL_PROGRAM_FILES, FALSE))) 
		return GetLastError();

	strcpy(szToolsProgramFiles, szAppProgramFiles);

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	// Extract directory
	strncpy(szAppDirectory, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
	szAppDirectory[strlen(szAppDirectory)] = '\0';
	strcpy(szAppOutputDir, szAppDirectory);

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
	strcat(szArgs, szAppProgramFiles);
	strcat(szArgs, "\\yagarto\\bin\\make.exe\" -C \"");
	strcat(szArgs, ConvertToUNIXPath(szAppDirectory));
	strcat(szArgs, "/src/\"");
	strcat(szArgs, " HEX_FILE=\""); 
	strcat(szArgs, fileName);
	strcat(szArgs, "\" ");
	strcat(szArgs, " HEX_PATH=\""); 
	strcat(szArgs, szAppDestPath);
	strcat(szArgs, "/\" ");
	strcat(szArgs, " TOOLS_PATH=\"");
	strcat(szArgs, ConvertToUNIXPath(szToolsProgramFiles));
	strcat(szArgs, "/yagarto\" clean debug "); 

	//output the application being run from the command window to the log file.
	strcat(szArgs, ">> \"");
	strcat(szArgs, szAppOutputDir);
	strcat(szArgs, "\\src\\output.log\" 2>&1\"");

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

	DumpInvokeCmd(szCmd, szArgs);

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	if(!GetExitCodeProcess(ProcessInfo.hProcess, &exitCode))
		exitCode = 0;

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);

	return exitCode;
}

void CompileAnsiCToGCC(char *dest)
{
    SeenVariablesCount = 0;

	char szAppPath[MAX_PATH]      = "";
	char szAppDirectory[MAX_PATH] = "";

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	// Extract directory
	strncpy(szAppDirectory, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
	szAppDirectory[strlen(szAppDirectory)] = '\0';

	strcat(szAppDirectory, "\\src\\ld.c");

    FILE *f = fopen(szAppDirectory, "w");
    if(!f) {
        Error(_("Couldn't open file '%s'"), szAppDirectory);
        return;
    }

    if(setjmp(CompileErrorBuf) != 0) {
        fclose(f);
        return;
    }

    // Set up the TRISx registers (direction). 1 means tri-stated (input).
    BYTE isInput[MAX_IO_PORTS], isOutput[MAX_IO_PORTS];
    BuildDirectionRegisters(isInput, isOutput);

    fprintf(f,
"/*****************************************************************************\n"
" * Tecnequip Tecnologia em Equipamentos Ltda                                 *\n"
" *****************************************************************************/\n"
"volatile unsigned int TIME_INTERVAL = ((25000000/1000) * %d) - 1;\n"
"volatile unsigned int U_M[32];\n"
"volatile int U_ENC1;\n\n"
		, Prog.cycleTime / 1000);

	fprintf(f, "extern unsigned int RS232Write(char c);\n");
	fprintf(f, "extern unsigned int ADCRead(unsigned int i);\n");
	fprintf(f, "extern unsigned int ENCRead(unsigned int i);\n");
	fprintf(f, "extern unsigned int ENCReset(unsigned int i);\n");

    // now generate declarations for all variables
    GenerateDeclarations(f);

    fprintf(f,
"\n"
"\n"
"/* Esta rotina deve ser chamada a cada ciclo para executar o diagrama ladder */\n"
"void PlcCycle(void)\n"
"{\n"
        );

    GenerateAnsiC(f);

    fprintf(f, "}\n");
    fclose(f);

    char str[MAX_PATH+500];
	DWORD err = 0;
	if ((err = InvokeGCC(dest)))
	{
		sprintf(str, _("A compilação retornou erro. O código do erro é %d. O arquivo com o log do erro esta na pasta \"<Program Files>\\<POPTools>\\src\\output.log\"\n"), err);
	}
	else
	{
		sprintf(str, _("Compilado com sucesso. O arquivo binário foi criado em '%s'.\r\n\r\n"), dest);
	}
    CompileSuccessfulMessage(str);
}
