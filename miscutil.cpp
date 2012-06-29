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
// Miscellaneous utility functions that don't fit anywhere else. IHEX writing,
// verified memory allocator, other junk.
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldmicro.h"

// We should display messages to the user differently if we are running
// interactively vs. in batch (command-line) mode.
BOOL RunningInBatchMode = FALSE;

// Allocate memory on a local heap
HANDLE MainHeap;

// Running checksum as we build up IHEX records.
static int IhexChecksum;

// Try to common a bit of stuff between the dialog boxes, since only one
// can be open at any time.
HWND OkButton;
HWND CancelButton;
BOOL DialogDone;
BOOL DialogCancel;

HFONT MyNiceFont;
HFONT MyFixedFont;

//-----------------------------------------------------------------------------
// printf-like debug function, to the Windows debug log.
//-----------------------------------------------------------------------------
void dbp(char *str, ...)
{
    va_list f;
    char buf[1024];
    va_start(f, str);
    vsprintf(buf, str, f);
    OutputDebugString(buf);
    OutputDebugString("\n");
}

//-----------------------------------------------------------------------------
// Wrapper for AttachConsole that does nothing running under <WinXP, so that
// we still run (except for the console stuff) in earlier versions.
//-----------------------------------------------------------------------------
#define ATTACH_PARENT_PROCESS ((DWORD)-1) // defined in WinCon.h, but only if
                                          // _WIN32_WINNT >= 0x500
BOOL AttachConsoleDynamic(DWORD base)
{
    typedef BOOL WINAPI fptr_acd(DWORD base);
    fptr_acd *fp;

    HMODULE hm = LoadLibrary("kernel32.dll");
    if(!hm) return FALSE;

    fp = (fptr_acd *)GetProcAddress(hm, "AttachConsole");
    if(!fp) return FALSE;

    return fp(base);
}

//-----------------------------------------------------------------------------
// For error messages to the user; printf-like, to a message box.
//-----------------------------------------------------------------------------
void Error(char *str, ...)
{
    va_list f;
    char buf[1024];
    va_start(f, str);
    vsprintf(buf, str, f);
    if(RunningInBatchMode) {
        AttachConsoleDynamic(ATTACH_PARENT_PROCESS);
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written;

        // Indicate that it's an error, plus the output filename
        char str[MAX_PATH+100];
        sprintf(str, "compile error ('%s'): ", CurrentCompileFile);
        WriteFile(h, str, strlen(str), &written, NULL);
        // The error message itself
        WriteFile(h, buf, strlen(buf), &written, NULL);
        // And an extra newline to be safe.
        strcpy(str, "\n");
        WriteFile(h, str, strlen(str), &written, NULL);
    } else {
        HWND h = GetForegroundWindow();
        MessageBox(h, buf, _("LDmicro Error"), MB_OK | MB_ICONERROR);
    }
}

//-----------------------------------------------------------------------------
// A standard format for showing a message that indicates that a compile
// was successful.
//-----------------------------------------------------------------------------
void CompileSuccessfulMessage(char *str)
{
    if(RunningInBatchMode) {
        char str[MAX_PATH+100];
        sprintf(str, "compiled okay, wrote '%s'\n", CurrentCompileFile);

        AttachConsoleDynamic(ATTACH_PARENT_PROCESS);
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written;
        WriteFile(h, str, strlen(str), &written, NULL);
    } else {
        MessageBox(MainWindow, str, _("Compile"),
            MB_OK | MB_ICONINFORMATION);
    }
}

//-----------------------------------------------------------------------------
// A standard format for showing a message that indicates that a compile
// was successful.
//-----------------------------------------------------------------------------
void ProgramSuccessfulMessage(char *str)
{
    if(RunningInBatchMode) {
        char str[MAX_PATH+100];
        sprintf(str, "compiled okay, wrote '%s'\n", CurrentCompileFile);

        AttachConsoleDynamic(ATTACH_PARENT_PROCESS);
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written;
        WriteFile(h, str, strlen(str), &written, NULL);
    } else {
        MessageBox(MainWindow, str, _("Compile Successful"),
            MB_OK | MB_ICONINFORMATION);
    }
}

//-----------------------------------------------------------------------------
// Check the consistency of the heap on which all the PLC program stuff is
// stored.
//-----------------------------------------------------------------------------
void CheckHeap(char *file, int line)
{
    static unsigned int SkippedCalls;
    static SDWORD LastCallTime;
    SDWORD now = GetTickCount();

    // It slows us down too much to do the check every time we are called;
    // but let's still do the check periodically; let's do it every 70
    // calls or every 20 ms, whichever is sooner.
    if(SkippedCalls < 70 && (now - LastCallTime) < 20) {
        SkippedCalls++;
        return;
    }

    SkippedCalls = 0;
    LastCallTime = now;

    if(!HeapValidate(MainHeap, 0, NULL)) {
        dbp("file %s line %d", file, line);
        Error("Noticed memory corruption at file '%s' line %d.", file, line);
        oops();
    }
}

//-----------------------------------------------------------------------------
// Like malloc/free, but memsets memory allocated to all zeros. Also TODO some
// checking and something sensible if it fails.
//-----------------------------------------------------------------------------
void *CheckMalloc(size_t n)
{
    ok();
    void *p = HeapAlloc(MainHeap, HEAP_ZERO_MEMORY, n);
    return p;
}
void CheckFree(void *p)
{
    ok();
    HeapFree(MainHeap, 0, p);
}


//-----------------------------------------------------------------------------
// Clear the checksum and write the : that starts an IHEX record.
//-----------------------------------------------------------------------------
void StartIhex(FILE *f)
{
    fprintf(f, ":");
    IhexChecksum = 0;
}

//-----------------------------------------------------------------------------
// Write an octet in hex format to the given stream, and update the checksum
// for the IHEX file.
//-----------------------------------------------------------------------------
void WriteIhex(FILE *f, BYTE b)
{
    fprintf(f, "%02X", b);
    IhexChecksum += b;
}

//-----------------------------------------------------------------------------
// Write the finished checksum to the IHEX file from the running sum
// calculated by WriteIhex.
//-----------------------------------------------------------------------------
void FinishIhex(FILE *f)
{
    IhexChecksum = ~IhexChecksum + 1;
    IhexChecksum = IhexChecksum & 0xff;
    fprintf(f, "%02X\n", IhexChecksum);
}

//-----------------------------------------------------------------------------
// Create a window with a given client area.
//-----------------------------------------------------------------------------
HWND CreateWindowClient(DWORD exStyle, char *className, char *windowName,
    DWORD style, int x, int y, int width, int height, HWND parent,
    HMENU menu, HINSTANCE instance, void *param)
{
    HWND h = CreateWindowEx(exStyle, className, windowName, style, x, y,
        width, height, parent, menu, instance, param);

    RECT r;
    GetClientRect(h, &r);
    width = width - (r.right - width);
    height = height - (r.bottom - height);
    
    SetWindowPos(h, HWND_TOP, x, y, width, height, 0);

    return h;
}

//-----------------------------------------------------------------------------
// Window proc for the dialog boxes. This Ok/Cancel stuff is common to a lot
// of places, and there are no other callbacks from the children.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
        case WM_NOTIFY:
            break;

		case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == OkButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
            } else if(h == CancelButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
            }
            break;
			}

        case WM_CLOSE:
        case WM_DESTROY:
            DialogDone = TRUE;
            DialogCancel = TRUE;
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
	
}

//-----------------------------------------------------------------------------
// Set the font of a control to a pretty proportional font (typ. Tahoma).
//-----------------------------------------------------------------------------
void NiceFont(HWND h)
{
    SendMessage(h, WM_SETFONT, (WPARAM)MyNiceFont, TRUE);
}

//-----------------------------------------------------------------------------
// Set the font of a control to a pretty fixed-width font (typ. Lucida
// Console).
//-----------------------------------------------------------------------------
void FixedFont(HWND h)
{
    SendMessage(h, WM_SETFONT, (WPARAM)MyFixedFont, TRUE);
}

//-----------------------------------------------------------------------------
// Create our dialog box class, used for most of the popup dialogs.
//-----------------------------------------------------------------------------
void MakeDialogBoxClass(void)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                          CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)DialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "LDmicroDialog";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 32, 32, 0);
    wc.hIconSm          = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 16, 16, 0);

    RegisterClassEx(&wc);

    MyNiceFont = CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Tahoma");
    if(!MyNiceFont)
        MyNiceFont = (HFONT)GetStockObject(SYSTEM_FONT);

    MyFixedFont = CreateFont(14, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Lucida Console");
    if(!MyFixedFont)
        MyFixedFont = (HFONT)GetStockObject(SYSTEM_FONT);
}

//-----------------------------------------------------------------------------
// Map an I/O type to a string describing it. Used both in the on-screen
// list and when we write a text file to describe it.
//-----------------------------------------------------------------------------
char *IoTypeToString(int ioType)
{
    switch(ioType) {
        case IO_TYPE_DIG_INPUT:         return _("digital in"); 
        case IO_TYPE_DIG_OUTPUT:        return _("digital out"); 
        case IO_TYPE_INTERNAL_RELAY:    return _("int. relay"); 
        case IO_TYPE_UART_TX:           return _("UART tx"); 
        case IO_TYPE_UART_RX:           return _("UART rx"); 
        case IO_TYPE_PWM_OUTPUT:        return _("PWM out"); 
        case IO_TYPE_TON:               return _("turn-on delay"); 
        case IO_TYPE_TOF:               return _("turn-off delay"); 
        case IO_TYPE_RTO:               return _("retentive timer"); 
        case IO_TYPE_COUNTER:           return _("counter"); 
        case IO_TYPE_GENERAL:           return _("general var"); 
        case IO_TYPE_READ_ADC:          return _("adc input"); 
        case IO_TYPE_READ_ENC:          return _("entrada encoder"); 
        case IO_TYPE_RESET_ENC:         return _("reset encoder"); 
        case IO_TYPE_READ_USS:         return _("read USS"); 
        case IO_TYPE_WRITE_USS:         return _("write USS"); 
		case IO_TYPE_SET_DA:			return _("set DA"); 
		case IO_TYPE_READ_MODBUS:       return _("leitura modbus"); 
		case IO_TYPE_WRITE_MODBUS:      return _("escrita modbus"); 
		case IO_TYPE_READ_MODBUS_ETH:   return _("leitura modbus eth"); 
		case IO_TYPE_WRITE_MODBUS_ETH:  return _("escrita modbus eth"); 
		case IO_TYPE_READ_YASKAWA:		return _("leitura NS600"); 
		case IO_TYPE_WRITE_YASKAWA:		return _("escrita NS600"); 
		case IO_TYPE_INTERNAL_FLAG:		return _("flag interna"); 
        default:                        return _("");
    }
}

//-----------------------------------------------------------------------------
// Get a pin number for a given I/O; for digital ins and outs and analog ins,
// this is easy, but for PWM and UART this is forced (by what peripherals
// are available) so we look at the characteristics of the MCU that is in
// use.
//-----------------------------------------------------------------------------
void PinNumberForIo(char *dest, PlcProgramSingleIo *io)
{
    if(!dest) return;

    if(!io) {
        strcpy(dest, "");
        return;
    }
        
    int type = io->type;
    if(type == IO_TYPE_DIG_INPUT || type == IO_TYPE_DIG_OUTPUT || type == IO_TYPE_GENERAL
        || type == IO_TYPE_READ_ADC || type == IO_TYPE_READ_ENC || type == IO_TYPE_RESET_ENC)
    {
        int pin = io->pin;
        if(pin == NO_PIN_ASSIGNED) {
			if(type == IO_TYPE_GENERAL) {
		        strcpy(dest, "");
			} else {
	            strcpy(dest, _("(not assigned)"));
			}
        } else {
            sprintf(dest, "%d", pin);
        }
    } else if(type == IO_TYPE_UART_TX && Prog.mcu) {
        if(Prog.mcu->uartNeeds.txPin == 0) {
            strcpy(dest, _("<no UART!>"));
        } else {
            sprintf(dest, "%d", Prog.mcu->uartNeeds.txPin);
        }
    } else if(type == IO_TYPE_UART_RX && Prog.mcu) {
        if(Prog.mcu->uartNeeds.rxPin == 0) {
            strcpy(dest, _("<no UART!>"));
        } else {
            sprintf(dest, "%d", Prog.mcu->uartNeeds.rxPin);
        }
    } else if(type == IO_TYPE_PWM_OUTPUT && Prog.mcu) {
        if(Prog.mcu->pwmNeedsPin == 0) {
            strcpy(dest, _("<no PWM!>"));
        } else {
            sprintf(dest, "%d", Prog.mcu->pwmNeedsPin);
        }
    } else {
        strcpy(dest, "");
    }
}

//-----------------------------------------------------------------------------
// Is an expression that could be either a variable name or a number a number?
//-----------------------------------------------------------------------------
bool IsNumber(char *str)
{
    if(*str == '-' || isdigit(*str)) {
        return TRUE;
    } else if(*str == '\'') {
        // special case--literal single character
        return TRUE;
    } else {
        return FALSE;
    }
}

void LoadIOListToComboBox(HWND ComboBox, unsigned int mask)
{
	int i, idx = 0;

	SendMessage(ComboBox, CB_RESETCONTENT, 0, 0);
	if(mask & IO_TYPE_INTERNAL_FLAG) {
		mask &= ~IO_TYPE_INTERNAL_FLAG;
		for(i = 0; *InternalFlags[i]; i++) {
			SendMessage(ComboBox, CB_INSERTSTRING, idx++, (LPARAM)((LPCTSTR)InternalFlags[i]));
		}
	}

	for(i = 0; i < Prog.io.count; i++) {
		if(Prog.io.assignment[i].type & mask) {
			SendMessage(ComboBox, CB_INSERTSTRING, idx++, (LPARAM)((LPCTSTR)Prog.io.assignment[i].name));
		}
    }
}

#include <setupapi.h>

#ifndef GUID_DEVINTERFACE_COMPORT
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
#endif
  
typedef HKEY (__stdcall SETUPDIOPENDEVREGKEY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, DWORD, DWORD, REGSAM);
typedef BOOL (__stdcall SETUPDICLASSGUIDSFROMNAME)(LPCTSTR, LPGUID, DWORD, PDWORD);
typedef BOOL (__stdcall SETUPDIDESTROYDEVICEINFOLIST)(HDEVINFO);
typedef BOOL (__stdcall SETUPDIENUMDEVICEINFO)(HDEVINFO, DWORD, PSP_DEVINFO_DATA);
typedef HDEVINFO (__stdcall SETUPDIGETCLASSDEVS)(LPGUID, LPCTSTR, HWND, DWORD);
typedef BOOL (__stdcall SETUPDIGETDEVICEREGISTRYPROPERTY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, PDWORD, PBYTE, DWORD, PDWORD);

int LoadCOMPorts(HWND ComboBox, unsigned int iDefaultPort, bool bHasAuto)
{
  //Get the various function pointers we require from setupapi.dll
  HINSTANCE hSetupAPI = LoadLibrary(_T("SETUPAPI.DLL"));
  if (hSetupAPI == NULL)
    return FALSE;

  SETUPDIOPENDEVREGKEY* lpfnLPSETUPDIOPENDEVREGKEY = reinterpret_cast<SETUPDIOPENDEVREGKEY*>(GetProcAddress(hSetupAPI, "SetupDiOpenDevRegKey"));
  SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(hSetupAPI, "SetupDiGetClassDevsA"));
  SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(hSetupAPI, "SetupDiGetDeviceRegistryPropertyA"));
  SETUPDIDESTROYDEVICEINFOLIST* lpfnSETUPDIDESTROYDEVICEINFOLIST = reinterpret_cast<SETUPDIDESTROYDEVICEINFOLIST*>(GetProcAddress(hSetupAPI, "SetupDiDestroyDeviceInfoList"));
  SETUPDIENUMDEVICEINFO* lpfnSETUPDIENUMDEVICEINFO = reinterpret_cast<SETUPDIENUMDEVICEINFO*>(GetProcAddress(hSetupAPI, "SetupDiEnumDeviceInfo"));

  if ((lpfnLPSETUPDIOPENDEVREGKEY == NULL) || (lpfnSETUPDIDESTROYDEVICEINFOLIST == NULL) ||
      (lpfnSETUPDIENUMDEVICEINFO == NULL) || (lpfnSETUPDIGETCLASSDEVS == NULL) || (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY == NULL))
  {
    //Unload the setup dll
    FreeLibrary(hSetupAPI);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
  }
  
  //Now create a "device information set" which is required to enumerate all the ports
  GUID guid = GUID_DEVINTERFACE_COMPORT;
  HDEVINFO hDevInfoSet = lpfnSETUPDIGETCLASSDEVS(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if (hDevInfoSet == INVALID_HANDLE_VALUE)
  {
		DWORD dwLastError = GetLastError();
  
    //Unload the setup dll
    FreeLibrary(hSetupAPI);
    
    SetLastError(dwLastError);

    return FALSE;
  }

  //Finally do the enumeration
  BOOL bMoreItems = TRUE, iPortMatch = FALSE;
  int nIndex = 0, nPort = 0, iFoundCount = 0;
  SP_DEVINFO_DATA devInfo;

  if(ComboBox) {
	ComboBox_ResetContent(ComboBox);
	if(bHasAuto) SendMessage(ComboBox, CB_INSERTSTRING, iFoundCount++, (LPARAM)("<auto>"));
  }

  while (bMoreItems)
  {
    //Enumerate the current device
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    bMoreItems = lpfnSETUPDIENUMDEVICEINFO(hDevInfoSet, nIndex, &devInfo);
    if (bMoreItems)
    {
      //Did we find a serial port for this device
      BOOL bAdded = FALSE;

      //Get the registry key which stores the ports settings
      HKEY hDeviceKey = lpfnLPSETUPDIOPENDEVREGKEY(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
      if (hDeviceKey)
      {
        //Read in the name of the port
        TCHAR szPortName[256];
        szPortName[0] = _T('\0');
        DWORD dwSize = sizeof(szPortName);
        DWORD dwType = 0;
  	    if ((RegQueryValueEx(hDeviceKey, _T("PortName"), NULL, &dwType, reinterpret_cast<LPBYTE>(szPortName), &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
        {
          //If it looks like "COMX" then
          //add it to the array which will be returned
          size_t nLen = _tcslen(szPortName);
          if (nLen > 3)
          {
            if ((_tcsnicmp(szPortName, _T("COM"), 3) == 0) && IsNumber(&(szPortName[3])))
            {
              //Work out the port number
              nPort = _ttoi(&(szPortName[3]));
              bAdded = TRUE;
            }
          }
        }

        //Close the key now that we are finished with it
        RegCloseKey(hDeviceKey);
      }

      //If the port was a serial port, then also try to get its friendly name
      if (bAdded)
      {
        TCHAR szFriendlyName[256], COMPort[256+10]; // 256+10 = Friendly name + "COMx - " string
        szFriendlyName[0] = _T('\0');
        DWORD dwSize = sizeof(szFriendlyName);
        DWORD dwType = 0;
        if (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, reinterpret_cast<PBYTE>(szFriendlyName), dwSize, &dwSize) && (dwType == REG_SZ))
        {
			if(!strcmp(szFriendlyName, "Silicon Labs CP210x USB to UART Bridge")) {
				if(!ComboBox) // Just searching for POP-7 device
					break;

				strcpy(szFriendlyName, "POP-7");
			}
			sprintf(COMPort, "COM%d - %s", nPort, szFriendlyName);
        }
        else
        {
			Error(COMPort, "COM%d", nPort);
        }

		if(ComboBox) {
			SendMessage(ComboBox, CB_INSERTSTRING, iFoundCount, (LPARAM)(COMPort));

			if(nPort == iDefaultPort) {
				iPortMatch = TRUE;
				SendMessage(ComboBox, CB_SETCURSEL, iFoundCount, 0);
			}

			iFoundCount++;
		}
      }
    }

    ++nIndex;
  }

  if((!iPortMatch || !iDefaultPort) && ComboBox) {
	SendMessage(ComboBox, CB_SETCURSEL, iFoundCount ? 0 : -1, 0);
  }

  //Free up the "device information set" now that we are finished with it
  lpfnSETUPDIDESTROYDEVICEINFOLIST(hDevInfoSet);

  //Unload the setup dll
  FreeLibrary(hSetupAPI);

  //Return the success indicator or POP-7 port number
  return !ComboBox ? nPort : TRUE;
}

unsigned int GetTypeFromName(char *name)
{
	int i;

	for(i=0; i<Prog.io.count; i++) {
		if(!_stricmp(name, Prog.io.assignment[i].name))
			return Prog.io.assignment[i].type;
	}

	return IO_TYPE_PENDING;
}

bool IsInternalFlag(char *name)
{
	unsigned int i;

	for(i=0; *InternalFlags[i]; i++) {
		if(!_stricmp(name, InternalFlags[i])) {
			return TRUE;
		}
	}

	return FALSE;	
}

bool IsValidNumber(char *number)
{
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

bool IsValidVarName(char *name)
{
	bool first = true;

	while(*name) {
		// If char isn't letter nor '_' or is number in the first position, returns false
		if((toupper(*name) < 'A' || toupper(*name) > 'Z') && *name != '_' && !(!first && (*name >= '0' && *name <= '9')))
			return false;

		name++;
		first = false;
	}

	return true;
}

bool IsValidNameAndType(char *old_name, char *name, char *FieldName, unsigned int Rules, unsigned int new_type, int MinVal, int MaxVal)
{
	int val;
	bool ret = FALSE;
	char msg[MAX_NAME_LEN+50];
	unsigned int current_type = GetTypeFromName(name);
	bool name_is_internal_flag = IsInternalFlag(name);
	bool name_is_number = IsNumber(name);

	// Check for Internal Flags restrictions
	if(new_type != IO_TYPE_INTERNAL_FLAG && name_is_internal_flag) {
		sprintf(msg, "Nome '%s' reservado para uso interno, favor escolher outro nome.", name);
		Error(_(msg));
		return FALSE;
	} else  if(new_type == IO_TYPE_INTERNAL_FLAG && !name_is_internal_flag) {
		Error(_("Nome Inválido! Para o tipo 'Flag Interna' é obrigatório selecionar um item da lista."));
		return FALSE;
	}

	// Check for Variable and Number restrictions
	if(!name_is_number && !IsValidVarName(name)) {
		sprintf(msg, "%s '%s' inválido!\n\nVariável: Apenas letras (A a Z), números ou _ (underline) e não inicar com número\nNúmero: Apenas números, podendo iniciar por - (menos)", FieldName ? FieldName : "Nome", name);
		Error(_(msg));
		return FALSE;
	} else if((Rules & VALIDATE_IS_VAR) && name_is_number) {
		sprintf(msg, "'%s' não pode ser número!", FieldName ? FieldName : name);
		Error(_(msg));
		return FALSE;
	} else if((Rules & VALIDATE_IS_NUMBER) && !name_is_number) {
		sprintf(msg, "'%s' deve ser número!", FieldName ? FieldName : name);
		Error(_(msg));
		return FALSE;
	} else if(name_is_number && !IsValidNumber(name)) {
		sprintf(msg, "Número '%s' inválido!", name);
		Error(_(msg));
		return FALSE;
	}

	// Check for Number Limits
	if((MinVal || MaxVal) && (Rules & (VALIDATE_IS_NUMBER | VALIDATE_IS_VAR_OR_NUMBER)) && name_is_number) {
		val = atoi(name);
		if(val < MinVal || val > MaxVal) {
			sprintf(msg, "'%s' fora dos limites! Deve estar entre %d e %d.", FieldName ? FieldName : name, MinVal, MaxVal);
			Error(_(msg));
			return FALSE;
		}
	}

	// Check for Type restrictions
	if(!name_is_number) {
		// If types must match and types are different or cannot accept io_pending type, generates an error
		if((Rules & VALIDATE_TYPES_MUST_MATCH) && current_type != new_type && !((current_type == IO_TYPE_PENDING || new_type == IO_TYPE_PENDING) && (Rules & VALIDATE_ACCEPT_IO_PENDING))) {
			Error(_("Conflito entre tipos! Operação não permitida."));
		} else if(new_type == IO_TYPE_DIG_INPUT || new_type == IO_TYPE_DIG_OUTPUT || new_type == IO_TYPE_INTERNAL_RELAY) {
			if(new_type == current_type) { // no type change, ok!
				ret = TRUE;
			} else if(current_type == IO_TYPE_PENDING) { // Inexistent name, ok!
				ret = TRUE;
			} else if(new_type == IO_TYPE_DIG_INPUT && (current_type == IO_TYPE_DIG_OUTPUT || current_type == IO_TYPE_INTERNAL_RELAY) && ExistsCoilWithName(name)) {
				Error(_("Saída em uso! Não é possível alterar para Entrada."));
			} else if(!_stricmp(old_name, name)) { // name not changed, ok!
				ret = TRUE;
			// name changed, check for type changes
			} else if(new_type == IO_TYPE_INTERNAL_RELAY && current_type == IO_TYPE_DIG_OUTPUT) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					Error("Valor inválido! Conflito de tipos: Rele Interno <-> Saída");
				} else if(MessageBox(MainWindow, _("Já existe uma Saída com este nome. Alterar para Rele Interno?"), "Confirmar alteração de Saída para Rele Interno", MB_YESNO | MB_ICONQUESTION) == IDYES) {
					ret = TRUE;
				}
			} else if(new_type == IO_TYPE_DIG_OUTPUT && current_type == IO_TYPE_INTERNAL_RELAY) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					Error("Valor inválido! Conflito de tipos: Rele Interno <-> Saída");
				} else if(MessageBox(MainWindow, _("Já existe um Rele Interno com este nome. Alterar para Saída?"), "Confirmar alteração de Rele Interno para Saída", MB_YESNO | MB_ICONQUESTION) == IDYES) {
					ret = TRUE;
				}
			} else if(new_type == IO_TYPE_DIG_OUTPUT && current_type == IO_TYPE_DIG_INPUT) { // changing existent input to output, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					Error("Valor inválido! Conflito de tipos: Entrada <-> Saída");
				} else if(MessageBox(MainWindow, _("Já existe uma Entrada com este nome. Alterar para Saída?"), "Confirmar alteração de Entrada para Saída", MB_YESNO | MB_ICONQUESTION) == IDYES) {
					ret = TRUE;
				}
			} else if(new_type == IO_TYPE_INTERNAL_RELAY && current_type == IO_TYPE_DIG_INPUT) { // changing existent input to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					Error("Valor inválido! Conflito de tipos: Rele Interno <-> Entrada");
				} else if(MessageBox(MainWindow, _("Já existe uma Entrada com este nome. Alterar para Relé Interno?"), "Confirmar alteração de Entrada para Relé Interno", MB_YESNO | MB_ICONQUESTION) == IDYES) {
					ret = TRUE;
				}
			} else { // no wrong conditions, ok!
				ret = TRUE;
			}
		} else if(current_type == IO_TYPE_COUNTER || current_type == IO_TYPE_TOF || current_type == IO_TYPE_TON) {
			if(new_type == IO_TYPE_COUNTER || new_type == IO_TYPE_TOF || new_type == IO_TYPE_TON) {
				ret = TRUE;
			} else {
				Error(_("'Nome' deve ser um contador ou timer!"));
			}
		} else {
			ret = TRUE;
		}
	} else {
		ret = TRUE;
	}

	return ret;
}

bool IsValidNameAndType(char *old_name, char *name, unsigned int new_type)
{
	return IsValidNameAndType(old_name, name, NULL, VALIDATE_IS_VAR, new_type, 0, 0);
}
