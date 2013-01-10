#include "poptools.h"

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
#ifdef _DEBUG
	va_list f;
    char buf[1024];
    va_start(f, str);
    vsprintf(buf, str, f);
    OutputDebugString(buf);
    OutputDebugString("\n");
#endif
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

	HWND h = GetForegroundWindow();
    MessageBox(h, buf, _("POPTools Error"), MB_OK | MB_ICONERROR);
}

//-----------------------------------------------------------------------------
// A standard format for showing a message that indicates that a compile
// was successful.
//-----------------------------------------------------------------------------
void CompileSuccessfulMessage(char *str)
{
    MessageBox(MainWindow, str, _("Compile"),
        MB_OK | MB_ICONINFORMATION);
}

//-----------------------------------------------------------------------------
// A standard format for showing a message that indicates that a compile
// was successful.
//-----------------------------------------------------------------------------
void ProgramSuccessfulMessage(char *str)
{
    MessageBox(MainWindow, str, _("Compile Successful"),
        MB_OK | MB_ICONINFORMATION);
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
    wc.lpszClassName    = "POPToolsDialog";
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
        case IO_TYPE_RESET_ENC:         return _("write encoder"); 
        case IO_TYPE_READ_USS:         return _("read USS"); 
        case IO_TYPE_WRITE_USS:         return _("write USS"); 
		case IO_TYPE_SET_DA:			return _("Set D/A"); 
		case IO_TYPE_READ_MODBUS:       return _("leitura modbus"); 
		case IO_TYPE_WRITE_MODBUS:      return _("escrita modbus"); 
		case IO_TYPE_READ_MODBUS_ETH:   return _("leitura modbus eth"); 
		case IO_TYPE_WRITE_MODBUS_ETH:  return _("escrita modbus eth"); 
		case IO_TYPE_READ_YASKAWA:		return _("leitura NS600"); 
		case IO_TYPE_WRITE_YASKAWA:		return _("escrita NS600"); 
		case IO_TYPE_INTERNAL_FLAG:		return _("Flag Interna"); 
        default:                        return "";
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

bool IsReservedName(char *name)
{
	int i;
	char *reserved[] = { _("in"), _("out"), _("new"), NULL };

	for(i=0; reserved[i] != NULL; i++) {
		if(!_stricmp(name, reserved[i]))
			return true;
	}

	return false;
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

	if(name == NULL || !strlen(name)) return false; // If text in white return false

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
	unsigned int current_type = GetTypeFromName(name);
	bool name_is_internal_flag = IsInternalFlag(name);
	bool name_is_reserved = IsReservedName(name);
	bool name_is_number = IsNumber(name);

	// Check for Internal Flags and Reserved Names restrictions
	if((new_type != IO_TYPE_INTERNAL_FLAG && name_is_internal_flag) || name_is_reserved) {
		Error(_("Nome '%s' reservado para uso interno, favor escolher outro nome."), name);
		return FALSE;
	} else  if(new_type == IO_TYPE_INTERNAL_FLAG && !name_is_internal_flag) {
		Error(_("Nome Inválido! Para o tipo 'Flag Interna' é obrigatório selecionar um item da lista."));
		return FALSE;
	}

	// Check for Variable and Number restrictions
	if(!name_is_number && !IsValidVarName(name)) {
		Error(_("%s '%s' inválido!\n\nVariável: Apenas letras (A a Z), números ou _ (underline) e não inicar com número\nNúmero: Apenas números, podendo iniciar por - (menos)"), FieldName ? FieldName : "Nome", name);
		return FALSE;
	} else if((Rules & VALIDATE_IS_VAR) && name_is_number) {
		Error(_("'%s' não pode ser número!"), FieldName ? FieldName : name);
		return FALSE;
	} else if((Rules & VALIDATE_IS_NUMBER) && !name_is_number) {
		Error(_("'%s' deve ser número!"), FieldName ? FieldName : name);
		return FALSE;
	} else if(name_is_number && !IsValidNumber(name)) {
		Error(_("Número '%s' inválido!"), name);
		return FALSE;
	}

	// Check for Number Limits
	if((MinVal || MaxVal) && (Rules & (VALIDATE_IS_NUMBER | VALIDATE_IS_VAR_OR_NUMBER)) && name_is_number) {
		val = atoi(name);
		if(val < MinVal || val > MaxVal) {
			Error(_("'%s' fora dos limites! Deve estar entre %d e %d."), FieldName ? FieldName : name, MinVal, MaxVal);
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
					Error(_("Valor inválido! Conflito de tipos: Rele Interno <-> Saída"));
				} else if(MessageBox(MainWindow, _("Já existe uma Saída com este nome. Alterar para Rele Interno?"), _("Confirmar alteração de Saída para Rele Interno"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
					ret = TRUE;
				}
			} else if(new_type == IO_TYPE_DIG_OUTPUT && current_type == IO_TYPE_INTERNAL_RELAY) { // changing existent output to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					Error(_("Valor inválido! Conflito de tipos: Rele Interno <-> Saída"));
				} else if(MessageBox(MainWindow, _("Já existe um Rele Interno com este nome. Alterar para Saída?"), _("Confirmar alteração de Rele Interno para Saída"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
					ret = TRUE;
				}
			} else if(new_type == IO_TYPE_DIG_OUTPUT && current_type == IO_TYPE_DIG_INPUT) { // changing existent input to output, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					Error(_("Valor inválido! Conflito de tipos: Entrada <-> Saída"));
				} else if(MessageBox(MainWindow, _("Já existe uma Entrada com este nome. Alterar para Saída?"), _("Confirmar alteração de Entrada para Saída"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
					ret = TRUE;
				}
			} else if(new_type == IO_TYPE_INTERNAL_RELAY && current_type == IO_TYPE_DIG_INPUT) { // changing existent input to internal relay, needs confirmation
				if(Rules & VALIDATE_DONT_ASK) {
					Error(_("Valor inválido! Conflito de tipos: Rele Interno <-> Entrada"));
				} else if(MessageBox(MainWindow, _("Já existe uma Entrada com este nome. Alterar para Relé Interno?"), _("Confirmar alteração de Entrada para Relé Interno"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
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

#define KEY_CONTROL_A  1
#define KEY_CONTROL_C  3
#define KEY_CONTROL_V 22
#define KEY_CONTROL_X 24

int iscontrol(WPARAM wParam)
{
	switch(wParam) {
		case KEY_CONTROL_A:
		case KEY_CONTROL_C:
		case KEY_CONTROL_V:
		case KEY_CONTROL_X:
		return 1;
	}

	return 0;
}

void ChangeFileExtension(char *name, char *ext)
{
	unsigned int pos;

	pos = strlen(name) - 1;
	while(name[pos] != '.' && pos) pos--;
	if(pos) {
		name[pos] = 0;
	}

	sprintf(name, "%s.%s", name, ext);
}

char *GetPinADC(char *name)
{
	int i;
	for(i=0; i < Prog.io.count; i++) {
		if(!strcmp(name, Prog.io.assignment[i].name))
			break;
	}

	switch(Prog.io.assignment[i].pin) {
	case 1:
		return "AD1";
		break;
	case 2:
		return "AD2";
		break;
	case 3:
		return "AD3";
		break;
	case 4:
		return "AD4";
		break;
	case 5:
		return "AD5";
		break;
	case 6:
		return "TEMP";
		break;
	}

	return "?";
}

int GetPinEnc(char *name)
{
	int i;
	for(i=0; i < Prog.io.count; i++) {
		if((Prog.io.assignment[i].type == IO_TYPE_READ_ENC || Prog.io.assignment[i].type == IO_TYPE_RESET_ENC)
				&& !strcmp(name, Prog.io.assignment[i].name))
			return Prog.io.assignment[i].pin;
	}

	return NO_PIN_ASSIGNED;
}

// CRC calculation //

/* Table of CRC values for high-order byte */
static unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

/* Table of CRC values for low-order byte */
static unsigned char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

unsigned short int CRC16(unsigned char *puchMsg, unsigned int usDataLen)
{
  unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
  unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
  unsigned int uIndex ; /* will index into CRC lookup table */

  while (usDataLen--) /* pass through message buffer */
    {
    uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
    uchCRCLo = auchCRCLo[uIndex] ;
    }

  //return (uchCRCHi << 8 | uchCRCLo) ;
  return (uchCRCLo << 8 | uchCRCHi) ;
}

// XOR calculation //
