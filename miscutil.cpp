#include "poptools.h"

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
// For error messages to the user; printf-like, to a message box.
//-----------------------------------------------------------------------------
void Error(const char *title, const char *str, va_list f)
{
    char buf[1024];
    vsprintf(buf, str, f);

	ladder->ShowDialog(eDialogType_Error, false, title, buf);
}

void Error(const char *str, ...)
{
    va_list f;
    va_start(f, str);

	Error(_("POPTools Erro"), str, f);
}

void Warning(const char *title, const char *str, ...)
{
    char buf[1024];

	va_list f;
    va_start(f, str);

    vsprintf(buf, str, f);

	ladder->ShowDialog(eDialogType_Message, false, title, buf);
}

//-----------------------------------------------------------------------------
// Create a window with a given client area.
//-----------------------------------------------------------------------------
HWND CreateWindowClient(DWORD exStyle, const char *className, const char *windowName,
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
// Is an expression that could be either a variable name or a number a number?
//-----------------------------------------------------------------------------
bool IsNumber(const char *str)
{
    if(*str == '-' || isdigit((unsigned char)(*str))) {
        return TRUE;
    } else if(*str == '\'') {
        // special case--literal single character
        return TRUE;
    } else {
        return FALSE;
    }
}

void LoadIOListToComboBox(HWND ComboBox, vector<eType> allowedTypes)
{
	int idx = 0;
	unsigned int i;

	SendMessage(ComboBox, CB_RESETCONTENT, 0, 0);
	if(find(allowedTypes.begin(), allowedTypes.end(), eType_InternalFlag) != allowedTypes.end()) {
		vector<string>::iterator it;
		vector<string> flags = ladder->getVectorInternalFlagsIO();
		for(it = flags.begin(); it != flags.end(); it++) {
			SendMessage(ComboBox, CB_INSERTSTRING, idx++, (LPARAM)((LPCTSTR)it->c_str()));
		}
	} else {
		mapDetails detailsIO;
		unsigned int count = ladder->getCountIO();
		for(i = 0; i < count; i++) {
			string name = ladder->getNameIObyIndex(i);
			detailsIO = ladder->getDetailsIO(name);
			if(allowedTypes.size() == 0 ||
				find(allowedTypes.begin(), allowedTypes.end(), detailsIO.type) != allowedTypes.end()) {
					SendMessage(ComboBox, CB_INSERTSTRING, idx++, (LPARAM)((LPCTSTR)name.c_str()));
			}
		}
    }
}

string getAppDirectory(void)
{
	char szAppPath[MAX_PATH]      = "";
	char szAppDirectory[MAX_PATH] = "";

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	// Extract directory
	strncpy(szAppDirectory, szAppPath, strrchr(szAppPath, '\\') - szAppPath);

	return string(szAppDirectory);
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
	nPort = 0;
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

// Functions to convert wchar <=> char
void ConvString_Free(tConvString *pStr, bool both)
{
	if(both || pStr->CharToWideChar) {
		delete pStr->pWideChar;
		pStr->pWideChar = NULL;
	}

	if(both || !pStr->CharToWideChar) {
		delete pStr->pChar;
		pStr->pChar = NULL;
	}
}

void ConvString_Convert(tConvString *pStr)
{
	if(pStr->CharToWideChar) {
		int len = strlen(pStr->pChar) + 1;
		pStr->pWideChar = new wchar_t[len];

	  memset(pStr->pWideChar,0,len);
	  ::MultiByteToWideChar(CP_ACP, NULL, pStr->pChar, -1, pStr->pWideChar, len);
	} else {
		pStr->pChar = _com_util::ConvertBSTRToString(pStr->pWideChar);
	}
}

char *ConvString_Convert(char *pc, wchar_t *pwc)
{
	tConvString pStr;

	pStr.pWideChar = pwc;
	pStr.CharToWideChar = false;

	ConvString_Convert(&pStr);

	if(pc) {
		strcpy(pc, pStr.pChar);
		ConvString_Free(&pStr, false);
	} else {
		pc = pStr.pChar;
	}

	return pc;
}

wchar_t *ConvString_Convert(wchar_t *pwc, const char *pc)
{
	tConvString pStr;

	pStr.pChar = (char *)pc;
	pStr.CharToWideChar = true;

	ConvString_Convert(&pStr);

	if(pwc) {
		wcscpy(pwc, pStr.pWideChar);
		ConvString_Free(&pStr, false);
	} else {
		pwc = pStr.pWideChar;
	}

	return pwc;
}

long long ElapsedTime(bool ShowDialog, void (*fnc)(void *), void *data)
{
	FILETIME start, end;
	GetSystemTimeAsFileTime(&start);

	if(fnc != nullptr) {
		(*fnc)(data);
	}

	GetSystemTimeAsFileTime(&end);

	long long elapsed = ((long long)(end.dwHighDateTime - start.dwHighDateTime) << 32) + (end.dwLowDateTime - start.dwLowDateTime);
	if(ShowDialog) {
		Warning("Tempo medido", "O teste executou em %d milissegundos", elapsed/10000);
	}

	return elapsed;
}

POINT getWindowStart(POINT size)
{
	RECT r;
	POINT start;

	GetWindowRect(MainWindow, &r);

	start.x = r.left + (r.right  - r.left - size.x)/2;
	start.y = r.top  + (r.bottom - r.top  - size.y)/2;

	if(start.x < r.left) {
		start.x = r.left;
	}

	if(start.y < r.top) {
		start.y = r.top;
	}

	return start;
}
