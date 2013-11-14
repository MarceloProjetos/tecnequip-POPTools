#ifndef __POPTOOLS_H
#define __POPTOOLS_H

// Visual Leak Detector
#ifdef _DEBUG
#include <vld.h>
#endif

#pragma warning(push)
#pragma warning(disable : 4995) //Disable warning related to deprecated functions (unsecure): strcpy, sprintf...
#include <windows.h>
#include <windowsx.h>
#include "mmsystem.h"
#include <comutil.h>
#include <ShlObj.h>
#include <ShellAPI.h>
#include <tchar.h>
#include <wchar.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <commdlg.h>
#include <setjmp.h>
#include <richedit.h>
#include <atlbase.h>
#include <limits.h>
#include <atlcom.h>
#include <initguid.h>
#include <winspool.h>

// Direct2D Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <assert.h>

// String Safe Header File
#include <strsafe.h>

#include "resource.h"
#include "splash.h"
#include "modbus_master.h"
#include "MainRibbon.h"
#include <uiribbon.h>
#include <UIRibbonPropertyHelpers.h>
#include "PropertySet.h"
#include "KeyboardHandlers.h"

/****************************************
****                                 ****
***       MRU Classes Definition      ***
****                                 ****
****************************************/

#include <shellapi.h>
#include <strsafe.h>
#include <OleAuto.h>
#pragma warning(pop) //Enable warning related to deprecated functions (unsecure)

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "WindowsCodecs.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "comsuppw.lib")

// Enable Visual Style
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma endregion

using namespace std;
using namespace D2D1;

template<class Interface>
inline void
SafeRelease(
    Interface **ppInterfaceToRelease
    )
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}

// Acquires an additional reference, if non-null.
template <typename InterfaceType>
inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
    if (newObject != NULL)
        newObject->AddRef();

    return newObject;
}


// Sets a new COM object, releasing the old one.
template <typename InterfaceType>
inline void SafeSet(InterfaceType** currentObject, InterfaceType* newObject)
{
    SafeAcquire(newObject);
    SafeRelease(&currentObject);
    currentObject = newObject;
}


// Maps exceptions to equivalent HRESULTs,
inline HRESULT ExceptionToHResult() throw()
{
    try
    {
        throw;  // Rethrow previous exception.
    }
    catch(std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_FAIL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

typedef signed long SDWORD;
//typedef signed short SWORD;
typedef SDWORD SWORD;
//-----------------------------------------------
// `Configuration options.'

// The library that I use to do registry stuff.
#define FREEZE_SUBKEY "POPTools"
#include "freeze.h"

#define DEFAULT_CPU "NXP LPC1768 LQFP100"

// Color Visual Style
#define CORES_CONSOLE 1

// Cycle Time
#define PLC_CYCLE 10 // miliseconds

// Size of the font that we will use to draw the ladder diagrams, in pixels
#define FONT_WIDTH  8
#define FONT_HEIGHT 13

#define DA_RESOLUTION		2048		// DA resolution 12 bits (4096 / 2) 
#define DA_VOLTAGE			10000.0f	// voltagem DA em mV (-10V +10V)
#define DA_CYCLE_INTERVAL	10			// miliseconds

#define ENCODER_MODE_RAW    0 // No Conversion
#define ENCODER_MODE_M      1 // Convert to Meter
#define ENCODER_MODE_MM     2 // Convert to Milimeter
#define ENCODER_MODE_TMM    3 // Convert to Milimeter / 10

#define RAMP_ABORT_DEFAULT 0
#define RAMP_ABORT_LEAVE   1
#define RAMP_ABORT_STOP    2
#define RAMP_ABORT_DESACEL 3

#define EEPROM_SIZE 0x7E

// Identifier for the message sent to MainWindow when SimSocket receives something
#define WM_SOCKET 104

//-----------------------------------------------
// Constants for the GUI. We have drop-down menus, a listview for the I/Os,
// etc.

struct strSerialConfig {
	BYTE  bByteSize;
	BYTE  bParity;
	BYTE  bStopBits;
	char *ConfigName;
};

// Menu IDs

#define MNU_NEW                 0x01
#define MNU_OPEN                0x02
#define MNU_CLOSE               0x0d
#define MNU_SAVE                0x03
#define MNU_SAVE_AS             0x04
#define MNU_SAVE_AS_C           0x08
#define MNU_EXPORT              0x05
#define MNU_EXIT                0x06
#define MNU_PRINT               0x0c
#define MNU_RECENT_CLEAR        0x07
#define MNU_RECENT_START        0x100

#define MNU_COPY_RUNG           0x09
#define MNU_PASTE_RUNG_BEFORE   0x0a
#define MNU_PASTE_RUNG_AFTER    0x0b

#define MNU_UNDO                0x10
#define MNU_REDO                0x11
#define MNU_PUSH_RUNG_UP        0x12
#define MNU_PUSH_RUNG_DOWN      0x13
#define MNU_INSERT_RUNG_BEFORE  0x14
#define MNU_INSERT_RUNG_AFTER   0x15
#define MNU_COPY_ELEMENT        0x1a
#define MNU_PASTE_ELEMENT       0x1b
#define MNU_CUT_ELEMENT         0x1e
#define MNU_GO_HOME             0x1c
#define MNU_GO_END              0x1d
#define MNU_DELETE_ELEMENT      0x16
#define MNU_DELETE_RUNG         0x17
#define MNU_FIND_AND_REPLACE    0x18
#define MNU_FIND_AND_REPLACE_NEXT 0x19

#define MNU_INSERT_COMMENT      0x20
#define MNU_INSERT_CONTACTS     0x21
#define MNU_INSERT_COIL         0x22
#define MNU_INSERT_TON          0x23
#define MNU_INSERT_TOF          0x24
#define MNU_INSERT_RTO          0x25
#define MNU_INSERT_RES          0x26
#define MNU_INSERT_OSR          0x27
#define MNU_INSERT_OSF          0x28
#define MNU_INSERT_CTU          0x29
#define MNU_INSERT_CTD          0x2a
#define MNU_INSERT_CTC          0x2b
#define MNU_INSERT_ADD          0x2c
#define MNU_INSERT_SUB          0x2d
#define MNU_INSERT_MUL          0x2e
#define MNU_INSERT_DIV          0x2f
#define MNU_INSERT_MOD          0x40
#define MNU_INSERT_SQRT         0x5c
#define MNU_INSERT_RAND         0x5d
#define MNU_INSERT_ABS          0x5e
#define MNU_INSERT_MOV          0x30
#define MNU_INSERT_READ_ADC     0x31
#define MNU_INSERT_SET_PWM      0x32
#define MNU_INSERT_UART_SEND    0x33
#define MNU_INSERT_UART_RECV    0x34
#define MNU_INSERT_EQU          0x35
#define MNU_INSERT_NEQ          0x36
#define MNU_INSERT_GRT          0x37
#define MNU_INSERT_GEQ          0x38
#define MNU_INSERT_LES          0x39
#define MNU_INSERT_LEQ          0x3a
#define MNU_INSERT_OPEN         0x3b
#define MNU_INSERT_SHORT        0x3c
#define MNU_INSERT_MASTER_RLY   0x3d
#define MNU_INSERT_SHIFT_REG    0x3e
#define MNU_INSERT_LUT          0x3f
#define MNU_INSERT_PERSIST      0x41
#define MNU_MAKE_NORMAL         0x42
#define MNU_NEGATE              0x43
#define MNU_MAKE_SET_ONLY       0x44
#define MNU_MAKE_RESET_ONLY     0x45
#define MNU_INSERT_PWL          0x46
#define MNU_INSERT_READ_ENC     0x47
#define MNU_INSERT_RESET_ENC    0x48
#define MNU_INSERT_READ_USS     0x49
#define MNU_INSERT_WRITE_USS    0x4a
#define MNU_CONDITIONAL_MENU	0x4b
#define MNU_INSERT_SET_DA       0x4c
#define MNU_INSERT_READ_MODBUS  0x4d
#define MNU_INSERT_WRITE_MODBUS 0x4e
#define MNU_INSERT_SET_BIT		0x4f
#define MNU_INSERT_CHECK_BIT	0x52
#define MNU_READ_FMTD_STR		0x53
#define MNU_WRITE_FMTD_STR		0x54
#define MNU_READ_SERVO_YASKAWA	0x55
#define MNU_WRITE_SERVO_YASKAWA	0x56
#define MNU_INSERT_RTC			0x57
#define MNU_INSERT_MULTISET_DA  0x58
#define MNU_INSERT_PID          0x5f
#define MNU_INSERT_PARALLEL     0x5b

#define MNU_MCU_SETTINGS        0x59
#define MNU_MCU_PREFERENCES     0x5a
#define MNU_PROCESSOR_0         0xa0

#define MNU_SIMULATION_MODE      0x60
#define MNU_START_SIMULATION     0x61
#define MNU_PAUSE_SIMULATION     0x63
#define MNU_STOP_SIMULATION      0x64
#define MNU_SINGLE_CYCLE         0x65
#define MNU_START_LOG_SIMULATION 0x66
#define MNU_STOP_LOG_SIMULATION  0x67

#define MNU_COMPILE             0x70
#define MNU_PROGRAM				0x72
#define MNU_DEBUG               0x74

#define MNU_MANUAL              0x80
#define MNU_KEYBOARD_MANUAL     0x81
#define MNU_ABOUT               0x82

#define MNU_EXAMPLE_COMMENT      0x120
#define MNU_EXAMPLE_CONTACTS     0x121
#define MNU_EXAMPLE_COIL         0x122
#define MNU_EXAMPLE_TON          0x123
#define MNU_EXAMPLE_TOF          0x124
#define MNU_EXAMPLE_RTO          0x125
#define MNU_EXAMPLE_RES          0x126
#define MNU_EXAMPLE_OSR          0x127
#define MNU_EXAMPLE_OSF          0x128
#define MNU_EXAMPLE_CTU          0x129
#define MNU_EXAMPLE_CTD          0x12a
#define MNU_EXAMPLE_CTC          0x12b
#define MNU_EXAMPLE_ADD          0x12c
#define MNU_EXAMPLE_SUB          0x12d
#define MNU_EXAMPLE_MUL          0x12e
#define MNU_EXAMPLE_DIV          0x12f
#define MNU_EXAMPLE_MOD          0x140
#define MNU_EXAMPLE_SQRT         0x15c
#define MNU_EXAMPLE_RAND         0x15d
#define MNU_EXAMPLE_ABS          0x15e
#define MNU_EXAMPLE_MOV          0x130
#define MNU_EXAMPLE_READ_ADC     0x131
#define MNU_EXAMPLE_SET_PWM      0x132
#define MNU_EXAMPLE_UART_SEND    0x133
#define MNU_EXAMPLE_UART_RECV    0x134
#define MNU_EXAMPLE_EQU          0x135
#define MNU_EXAMPLE_NEQ          0x136
#define MNU_EXAMPLE_GRT          0x137
#define MNU_EXAMPLE_GEQ          0x138
#define MNU_EXAMPLE_LES          0x139
#define MNU_EXAMPLE_LEQ          0x13a
#define MNU_EXAMPLE_OPEN         0x13b
#define MNU_EXAMPLE_SHORT        0x13c
#define MNU_EXAMPLE_MASTER_RLY   0x13d
#define MNU_EXAMPLE_SHIFT_REG    0x13e
#define MNU_EXAMPLE_LUT          0x13f
#define MNU_EXAMPLE_FMTD_STR     0x140
#define MNU_EXAMPLE_PERSIST      0x141
#define MNU_EXAMPLE_PWL          0x146
#define MNU_EXAMPLE_READ_ENC     0x147
#define MNU_EXAMPLE_RESET_ENC    0x148
#define MNU_EXAMPLE_READ_USS     0x149
#define MNU_EXAMPLE_WRITE_USS    0x14a
#define MNU_EXAMPLE_SET_DA       0x14c
#define MNU_EXAMPLE_READ_MODBUS  0x14d
#define MNU_EXAMPLE_WRITE_MODBUS 0x14e
#define MNU_EXAMPLE_SET_BIT		0x14f
#define MNU_EXAMPLE_CHECK_BIT	0x152
#define MNU_EXAMPLE_READ_FMTD_STR		0x153
#define MNU_EXAMPLE_WRITE_FMTD_STR		0x154
#define MNU_EXAMPLE_READ_SERVO_YASK		0x155
#define MNU_EXAMPLE_WRITE_SERVO_YASK	0x156
#define MNU_EXAMPLE_RTC			0x157
#define MNU_EXAMPLE_MULTISET_DA  0x158
#define MNU_EXAMPLE_PID          0x15f

#define MNU_EXAMPLE_ADC_LED    0x200
#define MNU_EXAMPLE_SEMAPHORE  0x201
#define MNU_EXAMPLE_TESTER     0x202

// Columns within the I/O etc. listview.
#define LV_IO_NAME              0x00
#define LV_IO_TYPE              0x01
#define LV_IO_STATE             0x02
#define LV_IO_PIN               0x03
#define LV_IO_PORT              0x04

// Timer IDs associated with the main window.
#define TIMER_BLINK_CURSOR      1
#define TIMER_SIMULATE          2
#define TIMER_AUTOSAVE          3
#define TIMER_DEBUG_INFO        4

//-----------------------------------------------
// Data structures for the actual ladder logic. A rung on the ladder
// is a series subcircuit. A series subcircuit contains elements or
// parallel subcircuits. A parallel subcircuit contains elements or series
// subcircuits. An element is a set of contacts (possibly negated) or a coil.

#define ELEM_PLACEHOLDER        0x01
#define ELEM_PADDING            0x04
#define ELEM_COMMENT            0x05

#define ELEM_CONTACTS           0x10
#define ELEM_COIL               0x11
#define ELEM_TON                0x12
#define ELEM_TOF                0x13
#define ELEM_RTO                0x14
#define ELEM_RES                0x15
#define ELEM_ONE_SHOT_RISING    0x16
#define ELEM_ONE_SHOT_FALLING   0x17
#define ELEM_MOVE               0x18
#define ELEM_ADD                0x19
#define ELEM_SUB                0x1a
#define ELEM_MUL                0x1b
#define ELEM_DIV                0x1c
#define ELEM_MOD                0x3a
#define ELEM_SQRT               0x43
#define ELEM_RAND               0x44
#define ELEM_ABS                0x45
#define ELEM_EQU                0x1d
#define ELEM_NEQ                0x1e
#define ELEM_GRT                0x1f
#define ELEM_GEQ                0x20
#define ELEM_LES                0x21
#define ELEM_LEQ                0x22
#define ELEM_CTU                0x23
#define ELEM_CTD                0x24
#define ELEM_CTC                0x25
#define ELEM_SHORT              0x26
#define ELEM_OPEN               0x27
#define ELEM_READ_ADC           0x28
#define ELEM_SET_PWM            0x29
#define ELEM_UART_RECV          0x2a
#define ELEM_UART_SEND          0x2b
#define ELEM_MASTER_RELAY       0x2c
#define ELEM_SHIFT_REGISTER     0x2d
#define ELEM_LOOK_UP_TABLE      0x2e
#define ELEM_FORMATTED_STRING   0x2f
#define ELEM_PERSIST            0x30
#define ELEM_PIECEWISE_LINEAR   0x31
#define ELEM_READ_ENC			0x32
#define ELEM_RESET_ENC			0x33
#define ELEM_READ_USS			0x34
#define ELEM_WRITE_USS			0x35
#define ELEM_SET_DA				0x36
#define ELEM_READ_MODBUS		0x37
#define ELEM_WRITE_MODBUS   	0x38
#define ELEM_SET_BIT			0x39
#define ELEM_CHECK_BIT			0x3c
#define ELEM_READ_FORMATTED_STRING 0x3d
#define ELEM_WRITE_FORMATTED_STRING 0x3e
#define ELEM_READ_SERVO_YASKAWA 0x3f
#define ELEM_WRITE_SERVO_YASKAWA 0x40
#define ELEM_RTC				0x41
#define ELEM_MULTISET_DA		0x42
#define ELEM_PID                0x46

#define MAX_NAME_LEN                128
#define MAX_COMMENT_LEN             384
#define MAX_LOOK_UP_TABLE_LEN        60

#define ELEM_RTC_MODE_FIXED        0
#define ELEM_RTC_MODE_CONTINUOUS   1
#define ELEM_RTC_MODE_INTERMITTENT 2

#define ELEM_SET_DA_MODE_RAW 0
#define ELEM_SET_DA_MODE_MV  1
#define ELEM_SET_DA_MODE_PCT 2

#define SELECTED_NONE       0
#define SELECTED_ABOVE      1
#define SELECTED_BELOW      2
#define SELECTED_RIGHT      3
#define SELECTED_LEFT       4

#include "intcode.h"

typedef struct McuIoInfoTag McuIoInfo;

#define MAX_IO  4096

#include "LadderObjects.h"
#include "iomap.h"

//-----------------------------------------------
// For actually drawing the ladder logic on screen; constants that determine
// how the boxes are laid out in the window, need to know that lots of
// places for figuring out if a mouse click is in a box etc.

// dimensions, in characters, of the area reserved for 1 leaf element
#define POS_WIDTH   22
#define POS_HEIGHT  3

#define MAX_RECENT_ITEMS     10
#define MAX_RECENT_MENU_LEN 100

// Settings structure
typedef struct SettingsTag {
	// Idioma da interface
	int idLanguage;
	// Simulation settings
	bool ShowSimulationWarnings;
	// Find And Replace settings
	char last_search_text[MAX_NAME_LEN];
	char last_new_text[MAX_NAME_LEN];
	// Recent List
	char recent_list[MAX_RECENT_ITEMS][MAX_PATH];
	// COM Port Settings
	int COMPortFlash;
	int COMPortDebug;
	// Interval in minutes between autosave calls
	int	AutoSaveInterval;
} Settings;

//-----------------------------------------------
// Processor definitions. These tables tell us where to find the I/Os on
// a processor, what bit in what register goes with what pin, etc. There
// is one master SupportedMcus table, which contains entries for each
// supported microcontroller.

typedef struct McuIoPinInfoTag {
    char    port;
    int     pin;
    int     bit;
	int     coil;
} McuIoPinInfo;

typedef struct McuAdcPinInfoTag {
    int     pin;
    BYTE    muxRegValue;
} McuAdcPinInfo;

typedef struct McuEncPinInfoTag {
    int     pin;
    BYTE    muxRegValue;
} McuEncPinInfo;

#define ISA_AVR             0x00
#define ISA_PIC16           0x01
#define ISA_ANSIC           0x02
#define ISA_INTERPRETED     0x03
#define ISA_LPC17	        0x04

#define MAX_IO_PORTS        256
#define MAX_RAM_SECTIONS    5

typedef struct McuIoInfoTag {
    char            *mcuName;
    char             portPrefix;
    DWORD            inputRegs[MAX_IO_PORTS];         // a is 0, j is 9
    DWORD            outputRegs[MAX_IO_PORTS];
    DWORD            dirRegs[MAX_IO_PORTS];
    DWORD            flashWords;
    struct {
        DWORD            start;
        int              len;
    }                ram[MAX_RAM_SECTIONS];
    McuIoPinInfo    *pinInfo;
    int              pinCount;
    McuAdcPinInfo   *adcInfo;
    int              adcCount;
    int              adcMax;
    McuEncPinInfo   *encInfo;
    int              encCount;
    int              encMax;
    struct {
        int             rxPin;
        int             txPin;
    }                uartNeeds;
    int              pwmNeedsPin;
    int              whichIsa;
    BOOL             avrUseIjmp;
    DWORD            configurationWord;
} McuIoInfo;

#define NUM_SUPPORTED_MCUS 16

enum eConfSection {
	eConfSection_Default = 0,
	eConfSection_Network,
	eConfSection_EncInc,
	eConfSection_EncSSI
};

//-----------------------------------------------
// Function prototypes

// poptools.cpp
bool DoEvents(void);
void ProcessMenu(int code);
void SetMenusEnabled(LadderContext *context);
void RefreshScrollbars(void);
bool CheckSaveUserCancels(void);

extern HINSTANCE Instance;
extern HWND MainWindow;
extern HDC Hdc;
extern LadderDiagram *ladder;
extern LadderClipboard clipboard;
extern vector<LadderDiagram *> ladderList;
extern Settings POPSettings;
extern char CurrentCompileFile[MAX_PATH];
extern McuIoInfo SupportedMcus[NUM_SUPPORTED_MCUS];

#define INTVAR_INC_PERIMRODA 0
#define INTVAR_INC_PULSOS    1
#define INTVAR_INC_FATOR     2
#define INTVAR_ABS_PERIMRODA 3
#define INTVAR_ABS_FATOR     4

#define PROGRESS_MODE_COMPILE 0
#define PROGRESS_MODE_PROGRAM 1

#define PROGRESS_STAGE_COMPILING 0
#define PROGRESS_STAGE_ERASING   1
#define PROGRESS_STAGE_WRITING   2
#define PROGRESS_STAGE_VERIFYING 3
#define PROGRESS_STAGE_FINISHING 4
#define PROGRESS_STAGE_FINISHED  5

#define PROGRESS_STAGE_STATE_OK     0
#define PROGRESS_STAGE_STATE_FAILED 1

#define PROGRESS_STATUS_OK     0
#define PROGRESS_STATUS_CANCEL 1
#define PROGRESS_STATUS_DONE   2

typedef struct {
	int   iCurrentStage;
	int   iStagePercent;
	int   bStageState;
	char *szMsg;
} ProgressStatus;

void HideProgressWindow  (void);
void ShowProgressWindow  (int mode);
int  UpdateProgressWindow(ProgressStatus *ps);

/*** ShowTaskDialog prototypes ***/

// Full version
int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
	TASKDIALOG_BUTTON *pButtons, TASKDIALOG_BUTTON *pRadioButtons, PCWSTR pszVerificationText, PCWSTR pszExpandedInformation,
	int *pnRadioButton, BOOL *pfVerificationFlagChecked);

// MessageBox replacement
int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons);

// MessageBox replacement with CheckBox
int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
	PCWSTR pszVerificationText, BOOL *pfVerificationFlagChecked);
int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
	PCWSTR pszVerificationText, bool *pfVerificationFlagChecked);

// With custom buttons
int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR *pszButtons);

// With radio buttons
int ShowTaskDialog(PCWSTR pszMainInstruction, PCWSTR pszContent, PCWSTR pszMainIcon, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons,
	PCWSTR *pszRadioButtons, int *pnRadioButton);

/*** End of ShowTaskDialog prototypes ***/

// memory debugging, because I often get careless; ok() will check that the
// heap used for all the program storage is not yet corrupt, and oops() if
// it is
void CheckHeap(char *file, int line);
#ifdef _DEBUG
#define ok() CheckHeap(__FILE__, __LINE__)
#else
#define ok()
#endif

// maincontrols.cpp
void MakeMainWindowControls(void);
void VscrollProc(WPARAM wParam);
void HscrollProc(WPARAM wParam);
void RefreshControlsToSettings(void);
void MainWindowResized(void);
void ToggleSimulationMode(void);
void StopSimulation(void);
void PauseSimulation(void);
void StartSimulation(void);
void UpdateMainWindowTitleBar(void);
void StatusBarSetText(int bar, char * text);
void RefreshDrawWindow();
void ShowTabCtrl(bool visible);
extern int TabHeight;
extern int ScrollWidth;
extern int ScrollHeight;
extern BOOL NeedHoriz;
extern HWND IoList;
extern HWND DrawWindow;
extern HWND TabCtrl;
extern int IoListTop;
extern int IoListHeight;
extern HWND UartSimulationWindow;
extern HWND UartSimulationTextControl;
extern BOOL RealTimeSimulationRunning;

// draw.cpp
extern int ColsAvailable;

int FormattedStrlen(const char *str);
void CenterWithSpaces(int cx, int cy, char *str, BOOL powered, BOOL isName);
void CenterWithSpaces(int cx, int cy, char *str, BOOL powered, BOOL isName, int extra);
void CenterWithWiresWidth(int cx, int cy, char *str, BOOL before, BOOL after, int totalWidth);
void CenterWithWires(int cx, int cy, char *str, BOOL before, BOOL after);

// draw_outputdev.cpp
extern void (*DrawChars)(int, int, char *);
void CALLBACK BlinkCursor(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);
void PaintWindow(void);
void ExportDrawingAsText(char *file);
void SetUpScrollbars(BOOL *horizShown, SCROLLINFO *horiz, SCROLLINFO *vert);
extern int ScrollXOffset;
extern int ScrollYOffset;
extern int ScrollXOffsetMax;
extern int ScrollYOffsetMax;

// schematic.cpp
void MoveCursorKeyboard(int keyCode, int shiftPressed);
bool EditSelectedElement(void);
bool MakeResetOnlySelected(void);
bool MakeSetOnlySelected(void);
bool MakeNormalSelected(void);
bool NegateSelected(void);

unsigned int FindAndReplace(string search_text, string new_text, eSearchAndReplaceMode mode);

// circuit.cpp
bool AddTimer(int which);
bool AddRTC(int which);
bool AddCoil(void);
bool AddContact(void);
bool AddEmpty(int which);
bool AddMove(void);
bool AddMath(int which);
bool AddSqrt(void);
bool AddRand(void);
bool AddAbs(void);
bool AddCmp(int which);
bool AddReset(void);
bool AddCounter(int which);
bool AddReadAdc(void);
bool AddSetDA(void);
bool AddReadEnc(void);
bool AddResetEnc(void);
bool AddMultisetDA(void);
bool AddReadFormatString(void);
bool AddWriteFormatString(void);
bool AddReadServoYaskawa(void);
bool AddWriteServoYaskawa(void);
bool AddReadUSS(void);
bool AddWriteUSS(void);
bool AddReadModbus(void);
bool AddWriteModbus(void);
bool AddReadModbusEth(void);
bool AddWriteModbusEth(void);
bool AddSetPwm(void);
bool AddUart(int which);
bool AddPersist(void);
bool AddPID(void);
bool AddComment(char *text);
bool AddSetBit(void);
bool AddCheckBit(void);
bool AddShiftRegister(void);
bool AddMasterRelay(void);
bool AddLookUpTable(void);
bool AddPiecewiseLinear(void);
bool AddFormattedString(void);
bool AddParallelStart(void);
bool DeleteSelectedFromProgram(void);
bool DeleteSelectedRung(void);
bool InsertRung(bool afterCursor);
bool PushRung(bool up);
bool NewProgram(void);
void SwitchProgram(LadderDiagram *newladder);
bool CloseProgram(LadderDiagram *diagram, bool isPOPToolsExiting = false);
bool CloseAllPrograms(bool isPOPToolsExiting = false);

// loadsave.cpp
bool fwrite_uint   (FILE *f, unsigned int   var);
bool fwrite_int    (FILE *f,          int   var);
bool fwrite_ulong  (FILE *f, unsigned long  var);
bool fwrite_long   (FILE *f,          long  var);
bool fwrite_float  (FILE *f, float          var);
bool fwrite_uchar  (FILE *f, unsigned char  var);
bool fwrite_bool   (FILE *f, bool           var);
bool fwrite_string (FILE *f, string         var);
bool fwrite_time_t (FILE *f, time_t         var);
bool fwrite_pointer(FILE *f, void          *var, unsigned int size);

bool fread_uint   (FILE *f, unsigned int  *var);
bool fread_int    (FILE *f,          int  *var);
bool fread_ulong  (FILE *f, unsigned long *var);
bool fread_long   (FILE *f,          long *var);
bool fread_float  (FILE *f, float         *var);
bool fread_uchar  (FILE *f, unsigned char *var);
bool fread_bool   (FILE *f, bool          *var);
bool fread_string (FILE *f, string        *var);
bool fread_time_t (FILE *f, time_t        *var);
bool fread_pointer(FILE *f, void          *var, unsigned int size);

BOOL LoadProjectFromFile(char *filename);
BOOL SaveProjectToFile(const char *filename, bool isBackup = false);

void SetAutoSaveInterval(int interval);
void CALLBACK AutoSaveNow(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);

// iolist.cpp
void ShowIoDialog(int item);
void IoListProc(NMHDR *h);
void IoMapListProc(NMHDR *h);
void ShowAnalogSliderPopup (const char *name);
void ShowEncoderSliderPopup(const char *name);

// LadderGUI.cpp
extern bool isDiagramChangedSinceLastDraw;

// commentdialog.cpp
bool ShowCommentDialog(char *comment, POINT ElemStart, POINT ElemSize, POINT GridSize);
// multisetdadialog.cpp
bool ShowMultisetDADialog(LadderElemMultisetDAProp *l, string *time, string *desl);
// simpledialog.cpp
bool ShowVarDialog(char *title, char *varname, string *name, POINT start, POINT size, POINT GridSize, vector<eType> types, bool isTxt = false);
bool ShowRTCDialog(int *mode, unsigned char *wday, struct tm *start, struct tm *end, POINT ElemStart, POINT ElemSize, POINT GridSize);
bool ShowVarBitDialog(char *title, char *varname, string *name, int * bit, POINT ElemStart, POINT ElemSize, POINT GridSize, vector<eType> types);
bool ShowModbusDialog(int mode_write, string *name, int *id, int *address, POINT ElemStart, POINT ElemSize, POINT GridSize);
void ShowSimulationVarSetDialog(const char *name, char *val);
bool ShowLookUpTableDialog(LadderElemLUTProp *t, POINT ElemStart, POINT ElemSize, POINT GridSize);
bool ShowPiecewiseLinearDialog(LadderElemPiecewiseProp *t, POINT ElemStart, POINT ElemSize, POINT GridSize);
// confdialog.cpp
extern struct strSerialConfig SerialConfig[];
extern char *SerialParityString[];
bool ShowConfDialog(eConfSection confSection);
extern char *EncAbsConfig[];
extern char *EncoderConvModes[];
// modbusdialog.cpp
void PopulateModBUSMasterCombobox(HWND h, bool has_new);
// helpdialog.cpp
void ShowHelpDialog(BOOL about);
void OpenCHM(void);
void OpenCHM(unsigned int hID);
// FARdialog.cpp
extern HWND FARWindow;
extern const LONG FarWidth;
bool ShowFARWindow(void);
void MoveFARWindow(RECT r);
// prefdialog.cpp
void ShowPrefDialog(void);

// iomap.cpp
unsigned int IoMap_GetIndex(mapDetails detailsIO);
bool         IoMap_IsModBUS(mapDetails detailsIO);

// miscutil.cpp
#define oops() { \
        dbp("bad at %d %s\n", __LINE__, __FILE__); \
        Error("Internal error at line %d file '%s'\n", __LINE__, __FILE__); \
        exit(1); \
    }
void dbp(char *str, ...);
void Error(char *str, ...);
void Error(char *title, char *str, va_list f);
void Warning(char *title, char *str, ...);
void *CheckMalloc(size_t n);
void CheckFree(void *p);
extern HANDLE MainHeap;
HWND CreateWindowClient(DWORD exStyle, char *className, char *windowName,
    DWORD style, int x, int y, int width, int height, HWND parent,
    HMENU menu, HINSTANCE instance, void *param);
void MakeDialogBoxClass(void);
void NiceFont(HWND h);
void FixedFont(HWND h);
string getAppDirectory(void);
bool IsNumber(const char *str);
void LoadIOListToComboBox(HWND ComboBox, vector<eType> allowedTypes);
int LoadCOMPorts(HWND ComboBox, unsigned int iDefaultPort, bool bHasAuto);
void ChangeFileExtension(char *name, char *ext);
unsigned short int CRC16(unsigned char *puchMsg, unsigned int usDataLen);
extern HFONT MyNiceFont;
extern HFONT MyFixedFont;
extern HWND OkButton;
extern HWND CancelButton;
extern BOOL DialogDone;
extern BOOL DialogCancel;

typedef struct {
	bool     CharToWideChar;
	char    *pChar;
	wchar_t *pWideChar;
} tConvString;

void ConvString_Free(tConvString *pStr, bool both);
void ConvString_Convert(tConvString *pStr);
char *ConvString_Convert(char *pc, wchar_t *pwc);
wchar_t *ConvString_Convert(wchar_t *pwc, char *pc);

// multisetdadialog.cpp
void PopulateAbortModeCombobox(HWND AbortModeCombobox, bool IncludeDefault);

// Regras de validação de variáveis
#define VALIDATE_IS_VAR				0x00000001
#define VALIDATE_IS_NUMBER			0x00000002
#define VALIDATE_IS_VAR_OR_NUMBER	0x00000004
#define VALIDATE_DONT_ASK           0x00000008
#define VALIDATE_TYPES_MUST_MATCH   0x00000010
#define VALIDATE_ACCEPT_IO_PENDING  0x00000020

int iscontrol(WPARAM wParam);

// lang.cpp
enum eExampleGalleryHeader {
	eExampleGalleryHeader_IO = 0,
	eExampleGalleryHeader_Timers,
	eExampleGalleryHeader_Counters,
	eExampleGalleryHeader_Variables,
	eExampleGalleryHeader_Conditionals,
	eExampleGalleryHeader_Mathematics,
	eExampleGalleryHeader_Analogs,
	eExampleGalleryHeader_Motors,
	eExampleGalleryHeader_ModBUS,
	eExampleGalleryHeader_RS485_Text,
	eExampleGalleryHeader_RS485_Others,
	eExampleGalleryHeader_Applications,
};

char *_(char *in);
void setLanguage(unsigned int id);
PCWSTR getRibbonLocalizedLabel(UINT nCmdID);
PCWSTR getRibbonLocalizedTooltipTitle(UINT nCmdID);
PCWSTR getRibbonLocalizedLabelDescription(UINT nCmdID);
PCWSTR getRibbonLocalizedTooltipDescription(UINT nCmdID);
PCWSTR getRibbonLocalizedExampleGalleryHeader(eExampleGalleryHeader header);

// Ribbon.cpp
extern unsigned int RibbonHeight;

extern HRESULT InitRibbon        (HWND hWindowFrame);
extern HRESULT RibbonSetCmdState (UINT uCmdID, BOOL bEnabled);
extern void    DestroyRibbon     (void);
extern void    SetApplicationMode(void);
extern void    EnableInterfaceItem(int item, BOOL enabled);

// simulate.cpp
void LogSimulation(bool isStart);
void SimulateOneCycle(BOOL forceRefresh);
void CALLBACK PlcCycleTimer(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);
void StartSimulationTimer(void);
void ClearSimulationData(void);
void DescribeForIoList(int val, eType type, char *out);
void DescribeForIoList(const char *name, eType type, char *out);
void SimulationToggleContact(const char *name);
void  SetAdcShadow(const char *name, SWORD val);
SWORD GetAdcShadow(const char *name);
void  SetEncShadow(const char *name, SWORD val);
SWORD GetEncShadow(const char *name);
void SetSimulationVariable(const char *name, SWORD val);
SWORD GetSimulationVariable(const char *name);
void DestroyUartSimulationWindow(void);
void ShowUartSimulationWindow(void);
#define MAX_SCROLLBACK 16384

// WatchPoint declarations
void  AddWP      (char *name, int val);
void  RemoveWP   (char *name);
int  *GetValWP   (const char *name, int *val);
void  ClearListWP(void);

// Hardware Registers
#define HWR_MODBUS_SIZE 32

#define HWR_REG_INPUT  0
#define HWR_REG_OUTPUT 1
#define HWR_REG_MODBUS 2

typedef struct strHardwareRegisters HardwareRegisters;
struct strHardwareRegisters {
	unsigned int  DigitalInput;
	unsigned int  DigitalOutput;
	         int  ModBUS[HWR_MODBUS_SIZE];
			 bool Syncing;
			 bool NeedUpdate;
};

extern HardwareRegisters hwreg;

void HardwareRegisters_Sync(HardwareRegisters *hwr);

// compilecommon.cpp
#define CompileError() longjmp(CompileErrorBuf, 1)
extern jmp_buf CompileErrorBuf;

#define DIAGRAM_VALIDATION_OK      0
#define DIAGRAM_VALIDATION_WARNING 1
#define DIAGRAM_VALIDATION_ERROR   2

// intcode.cpp
void IntDumpListing(char *outFile);
BOOL GenerateIntermediateCode(void);
SWORD CheckMakeNumber(const char *str);
// gcc.cpp
extern char OutputLog[MAX_PATH + MAX_NAME_LEN + 1];
void GenerateDeclarations(FILE *f);
void CompileAnsiC(char *outFile);
DWORD CompileAnsiCToGCC(BOOL ShowSuccessMessage);
DWORD GenerateCFile(char *filename);

BOOL FlashProgram(char * hexFile, int ComPort, long BaudRate);

// debugdialog.cpp
void ShowDebugDialog(void);

// modbus_slave.cpp
void Init_MBDev_Slave        (void);
int  SimulationServer_Start  (void);
void SimulationServer_Stop   (void);
void SimulationServer_Message(WPARAM wParam, LPARAM lParam);

// modbus_usb.cpp
bool          USB_SetDateTime(struct tm *t);
bool          USB_GetDateTime(struct tm *t);
unsigned long USB_GetInput   (void);
unsigned int  USB_GetOutput  (void);

// CommonFileDialog.cpp
enum FDS_Mode { LoadLadder = 0, SaveLadder, SaveText, SaveC, SaveCSV };
HRESULT FileDialogShow(enum FDS_Mode mode, char *DefExt, char *FileName);

#endif
