#ifndef __LDMICRO_H
#define __LDMICRO_H

#include <windows.h>
#include <windowsx.h>
#include "mmsystem.h"
#include <comutil.h>
#include <ShlObj.h>
#include <ShellAPI.h>
#include <tchar.h>
#include <wchar.h>
#include <math.h>

#include <vector>

// Direct2D Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include <assert.h>
// String Safe Header File
#include <strsafe.h>

#pragma warning(disable : 4995)

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "WindowsCodecs.lib")
#pragma comment(lib, "dwrite.lib")

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

#include <setjmp.h>
typedef signed long SDWORD;
//typedef signed short SWORD;
typedef SDWORD SWORD;
//-----------------------------------------------
// `Configuration options.'

// The library that I use to do registry stuff.
#define FREEZE_SUBKEY "POPTools"

#define DEFAULT_CPU "NXP LPC1768 LQFP100"

// Color Visual Style
#define CORES_CONSOLE 0

// Size of the font that we will use to draw the ladder diagrams, in pixels
#define FONT_WIDTH   7
#define FONT_HEIGHT 13

#define DA_RESOLUTION		2048		// DA resolution 12 bits (4096 / 2) 
#define DA_VOLTAGE			10000.0f	// voltagem DA em mV (-10V +10V)
#define DA_CYCLE_INTERVAL	10			// miliseconds

// Identifier for the message sent to MainWindow when SimSocket receives something
#define WM_SOCKET 104

// Activating the next line will disable Ribbon in Main Window.
//#define POPTOOLS_DISABLE_RIBBON

#include "resource.h"
#include "splash.h"
#include "XMLWrapper.h"
#include "modbus_master.h"
#include "MainRibbon.h"

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
#define MNU_SAVE                0x03
#define MNU_SAVE_AS             0x04
#define MNU_SAVE_AS_C           0x08
#define MNU_EXPORT              0x05
#define MNU_EXIT                0x06
#define MNU_RECENT_CLEAR        0x07
#define MNU_RECENT_START        0x100

#define MNU_UNDO                0x10
#define MNU_REDO                0x11
#define MNU_PUSH_RUNG_UP        0x12
#define MNU_PUSH_RUNG_DOWN      0x13
#define MNU_INSERT_RUNG_BEFORE  0x14
#define MNU_INSERT_RUNG_AFTER   0x15
#define MNU_COPY_ELEMENT        0x1a
#define MNU_PASTE_ELEMENT       0x1b
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
#define MNU_INSERT_SQRT         0x5c
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
#define MNU_INSERT_READ_MODBUS_ETH	0x50
#define MNU_INSERT_WRITE_MODBUS_ETH	0x51
#define MNU_INSERT_CHECK_BIT	0x52
#define MNU_READ_FMTD_STR		0x53
#define MNU_WRITE_FMTD_STR		0x54
#define MNU_READ_SERVO_YASKAWA		0x55
#define MNU_WRITE_SERVO_YASKAWA		0x56
#define MNU_INSERT_RTC			0x57
#define MNU_INSERT_MULTISET_DA  0x58
#define MNU_INSERT_PARALLEL     0x5b

#define MNU_MCU_SETTINGS        0x59
#define MNU_MCU_PREFERENCES     0x5a
#define MNU_PROCESSOR_0         0xa0

#define MNU_SIMULATION_MODE     0x60
#define MNU_START_SIMULATION    0x61
#define MNU_PAUSE_SIMULATION    0x63
#define MNU_STOP_SIMULATION     0x64
#define MNU_SINGLE_CYCLE        0x65

#define MNU_COMPILE             0x70
#define MNU_COMPILE_AS          0x71
#define MNU_PROGRAM				0x72
#define MNU_PROGRAM_AS			0x73
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
#define MNU_EXAMPLE_SQRT         0x15c
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
#define MNU_EXAMPLE_READ_MODBUS_ETH	0x150
#define MNU_EXAMPLE_WRITE_MODBUS_ETH	0x151
#define MNU_EXAMPLE_CHECK_BIT	0x152
#define MNU_EXAMPLE_READ_FMTD_STR		0x153
#define MNU_EXAMPLE_WRITE_FMTD_STR		0x154
#define MNU_EXAMPLE_READ_SERVO_YASK		0x155
#define MNU_EXAMPLE_WRITE_SERVO_YASK	0x156
#define MNU_EXAMPLE_RTC			0x157
#define MNU_EXAMPLE_MULTISET_DA  0x158

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

//-----------------------------------------------
// Data structures for the actual ladder logic. A rung on the ladder
// is a series subcircuit. A series subcircuit contains elements or
// parallel subcircuits. A parallel subcircuit contains elements or series
// subcircuits. An element is a set of contacts (possibly negated) or a coil.

#define MAX_ELEMENTS_IN_SUBCKT  50

#define ELEM_PLACEHOLDER        0x01
#define ELEM_SERIES_SUBCKT      0x02
#define ELEM_PARALLEL_SUBCKT    0x03
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
#define ELEM_SQRT               0x43
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
#define ELEM_READ_MODBUS_ETH	0x3a
#define ELEM_WRITE_MODBUS_ETH  	0x3b
#define ELEM_CHECK_BIT			0x3c
#define ELEM_READ_FORMATTED_STRING 0x3d
#define ELEM_WRITE_FORMATTED_STRING 0x3e
#define ELEM_READ_SERVO_YASKAWA 0x3f
#define ELEM_WRITE_SERVO_YASKAWA 0x40
#define ELEM_RTC				0x41
#define ELEM_MULTISET_DA		0x42

#define CASE_LEAF \
        case ELEM_PLACEHOLDER: \
        case ELEM_COMMENT: \
        case ELEM_COIL: \
        case ELEM_CONTACTS: \
        case ELEM_TON: \
        case ELEM_TOF: \
        case ELEM_RTO: \
		case ELEM_RTC: \
        case ELEM_CTD: \
        case ELEM_CTU: \
        case ELEM_CTC: \
        case ELEM_RES: \
        case ELEM_ONE_SHOT_RISING: \
        case ELEM_ONE_SHOT_FALLING: \
        case ELEM_EQU: \
        case ELEM_NEQ: \
        case ELEM_GRT: \
        case ELEM_GEQ: \
        case ELEM_LES: \
        case ELEM_LEQ: \
        case ELEM_ADD: \
        case ELEM_SUB: \
        case ELEM_MUL: \
        case ELEM_DIV: \
        case ELEM_SQRT: \
        case ELEM_MOVE: \
        case ELEM_SHORT: \
        case ELEM_OPEN: \
		case ELEM_SET_BIT: \
		case ELEM_CHECK_BIT: \
        case ELEM_READ_ADC: \
		case ELEM_SET_DA: \
        case ELEM_READ_ENC: \
        case ELEM_RESET_ENC: \
		case ELEM_MULTISET_DA: \
        case ELEM_READ_USS: \
        case ELEM_WRITE_USS: \
        case ELEM_READ_MODBUS: \
        case ELEM_WRITE_MODBUS: \
        case ELEM_READ_MODBUS_ETH: \
        case ELEM_WRITE_MODBUS_ETH: \
        case ELEM_SET_PWM: \
        case ELEM_UART_SEND: \
        case ELEM_UART_RECV: \
        case ELEM_MASTER_RELAY: \
        case ELEM_SHIFT_REGISTER: \
        case ELEM_LOOK_UP_TABLE: \
        case ELEM_PIECEWISE_LINEAR: \
		case ELEM_READ_FORMATTED_STRING: \
		case ELEM_WRITE_FORMATTED_STRING: \
		case ELEM_READ_SERVO_YASKAWA: \
		case ELEM_WRITE_SERVO_YASKAWA: \
        case ELEM_FORMATTED_STRING: \
        case ELEM_PERSIST:

#define MAX_NAME_LEN                128
#define MAX_COMMENT_LEN             384
#define MAX_LOOK_UP_TABLE_LEN        60

typedef struct ElemSubckParallelTag ElemSubcktParallel;

typedef struct ElemCommentTag {
    char    str[MAX_COMMENT_LEN];
} ElemComment;

typedef struct ElemContactsTag {
    char    name[MAX_NAME_LEN];
    BOOL    negated;
	unsigned int type;
	unsigned char bit;
} ElemContacts;

typedef struct ElemCoilTag {
    char    name[MAX_NAME_LEN];
    BOOL    negated;
    BOOL    setOnly;
    BOOL    resetOnly;
	unsigned int  type;
	unsigned char bit;
} ElemCoil;

typedef struct ElemTimeTag {
    char    name[MAX_NAME_LEN];
    int     delay;
} ElemTimer;

typedef struct ElemResetTag {
    char    name[MAX_NAME_LEN];
} ElemReset;

typedef struct ElemRTCTag {
	//unsigned char select; // 0 -> Week Day, 1 -> Month Day
    unsigned char wday;  // [0:0] Sum, [0:1] Mon, [0:2] Tue, [0:3] Wed, [0:4] Thu, [0:5] Fri, [0:6] Sat, [0:7] WDay 1=YES, 0=No
	unsigned char mday;
	unsigned char month;
	unsigned int year;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
} ElemRTC;

typedef struct ElemMoveTag {
    char    src[MAX_NAME_LEN];
    char    dest[MAX_NAME_LEN];
} ElemMove;

typedef struct ElemMathTag {
    char    op1[MAX_NAME_LEN];
    char    op2[MAX_NAME_LEN];
    char    dest[MAX_NAME_LEN];
} ElemMath;

typedef struct ElemSqrtTag {
    char    src[MAX_NAME_LEN];
    char    dest[MAX_NAME_LEN];
} ElemSqrt;

typedef struct ElemCmpTag {
    char    op1[MAX_NAME_LEN];
    char    op2[MAX_NAME_LEN];
} ElemCmp;

typedef struct ElemCounterTag {
    char    name[MAX_NAME_LEN];
    int     max;
} ElemCounter;

typedef struct ElemReadAdcTag {
    char    name[MAX_NAME_LEN];
} ElemReadAdc;

#define ELEM_SET_DA_MODE_RAW 0
#define ELEM_SET_DA_MODE_MV  1
#define ELEM_SET_DA_MODE_PCT 2

typedef struct ElemSetDATag {
    char    name[MAX_NAME_LEN];
	int		value;
	int		mode;
} ElemSetDA;

typedef struct ElemReadEncTag {
    char    name[MAX_NAME_LEN];
} ElemReadEnc;

typedef struct ElemMultisetDATag {
	char    name[MAX_NAME_LEN];		// Tempo
	char    name1[MAX_NAME_LEN];	// Deslocamento
	BOOL	linear;					// 1 = Linear, 0 = Curva Ascendente/Descendente
	BOOL	forward;				// 1 = Avança, 0 = Recua
	BOOL	speedup;				// 1 = Aceleração, 0 = Desaceleração
	int		time;					// tempo total da rampa
	int		initval;				// valor inicial do DA na rampa
	int		type;					// 0 = Valor saida do DA (2048 ~ -2048), 1 = milivolt (mV) (10V ~-10V), 2 = percentual (%)
	int		gaint;					// tempo da curva de ganho em %
	int		gainr;					// resolução da curva de ganho em %
	BOOL	StartFromCurrentValue;	// 0 = Iniciar ou ir para zero, conforme speedup. 1 = partir do valor atual até o valor configurado
} ElemMultisetDA;

typedef struct ElemReadUSSTag {
    char    name[MAX_NAME_LEN];
	int		id;
	int		parameter;
	int		parameter_set;
	int		index;
} ElemReadUSS;

typedef struct ElemWriteUSSTag {
    char    name[MAX_NAME_LEN];
	int		id;
	int		parameter;
	int		parameter_set;
	int		index;
} ElemWriteUSS;

typedef struct ElemReadModbusTag {
    char    name[MAX_NAME_LEN];
	int		id;
	int		address;
	bool	int32;
	bool	retransmitir;
	int		value;
} ElemReadModbus;

typedef struct ElemWriteModbusTag {
    char    name[MAX_NAME_LEN];
	int		id;
	int		address;
	bool	int32;
	bool	retransmitir;
	int		value;
} ElemWriteModbus;

typedef struct ElemReadModbusEthTag {
    char    name[MAX_NAME_LEN];
	int		id;
	int		address;
	bool	int32;
	int		value;
} ElemReadModbusEth;

typedef struct ElemWriteModbusEthTag {
    char    name[MAX_NAME_LEN];
	int		id;
	int		address;
	bool	int32;
	int		value;
} ElemWriteModbusEth;

typedef struct ElemSetPwmTag {
    char    name[MAX_NAME_LEN];
    int     targetFreq;
} ElemSetPwm;

typedef struct ElemUartTag {
    char    name[MAX_NAME_LEN];
} ElemUart;

typedef struct ElemSetBitTag {
    char    name[MAX_NAME_LEN];
    int		bit;
	int		set;
} ElemSetBit;

typedef struct ElemCheckBitTag {
    char    name[MAX_NAME_LEN];
    int		bit;
	int		set;
} ElemCheckBit;

typedef struct ElemShiftRegisterTag {
    char    name[MAX_NAME_LEN];
    int     stages;
} ElemShiftRegister;

typedef struct ElemLookUpTableTag {
    char    dest[MAX_NAME_LEN];
    char    index[MAX_NAME_LEN];
    int     count;
    BOOL    editAsString;
    SWORD   vals[MAX_LOOK_UP_TABLE_LEN];
} ElemLookUpTable;

typedef struct ElemPiecewiseLinearTag {
    char    dest[MAX_NAME_LEN];
    char    index[MAX_NAME_LEN];
    int     count;
    SWORD   vals[MAX_LOOK_UP_TABLE_LEN];
} ElemPiecewiseLinear;

typedef struct ElemFormattedStringTag {
    char    var[MAX_NAME_LEN];
    char    string[MAX_LOOK_UP_TABLE_LEN];
} ElemFormattedString;

typedef struct ElemServoYaskawaTag {
    char    id[MAX_NAME_LEN];
    char    var[MAX_NAME_LEN];
    char    string[MAX_LOOK_UP_TABLE_LEN];
} ElemServoYaskawa;

typedef struct ElemPerisistTag {
    char    var[MAX_NAME_LEN];
} ElemPersist;

#define SELECTED_NONE       0
#define SELECTED_ABOVE      1
#define SELECTED_BELOW      2
#define SELECTED_RIGHT      3
#define SELECTED_LEFT       4
typedef struct ElemLeafTag {
    int     selectedState;
    BOOL    poweredAfter;
    union {
        ElemComment         comment;
        ElemContacts        contacts;
        ElemCoil            coil;
        ElemTimer           timer;
		ElemRTC				rtc;
        ElemReset           reset;
        ElemMove            move;
        ElemMath            math;
        ElemSqrt            sqrt;
        ElemCmp             cmp;
        ElemCounter         counter;
        ElemReadAdc         readAdc;
		ElemSetDA			setDA;
        ElemReadEnc         readEnc;
		ElemMultisetDA      multisetDA;
        ElemReadUSS         readUSS;
        ElemWriteUSS        writeUSS;
        ElemReadModbus      readModbus;
        ElemWriteModbus     writeModbus;
        ElemReadModbusEth   readModbusEth;
        ElemWriteModbusEth  writeModbusEth;
        ElemSetPwmTag       setPwm;
        ElemUart            uart;
		ElemSetBit			setBit;
		ElemCheckBit		checkBit;
        ElemShiftRegister   shiftRegister;
        ElemFormattedString fmtdStr;
		ElemServoYaskawa	servoYaskawa;
        ElemLookUpTable     lookUpTable;
        ElemPiecewiseLinear piecewiseLinear;
        ElemPersist         persist;
    }       d;
} ElemLeaf;

typedef struct ElemSubcktSeriesTag {
    struct {
        int     which;
        union {
            void                   *any;
            ElemSubcktParallel     *parallel;
            ElemLeaf               *leaf;
        } d;
    } contents[MAX_ELEMENTS_IN_SUBCKT];
    int count;
} ElemSubcktSeries;

typedef struct ElemSubckParallelTag {
    struct {
        int     which;
        union {
            void                   *any;
            ElemSubcktSeries       *series;
            ElemLeaf               *leaf;
        } d;
    } contents[MAX_ELEMENTS_IN_SUBCKT];
    int count;
} ElemSubcktParallel;

typedef struct McuIoInfoTag McuIoInfo;

typedef struct PlcProgramSingleIoTag {
    char        name[MAX_NAME_LEN];
#define IO_TYPE_PENDING         0x00000000
#define IO_TYPE_ALL             0x7FFFFFFF

#define IO_TYPE_DIG_INPUT        0x00000001
#define IO_TYPE_DIG_OUTPUT       0x00000002
#define IO_TYPE_READ_ADC         0x00000004
#define IO_TYPE_UART_TX          0x00000008
#define IO_TYPE_UART_RX          0x00000010
#define IO_TYPE_PWM_OUTPUT       0x00000020
#define IO_TYPE_INTERNAL_RELAY   0x00000040
#define IO_TYPE_TON              0x00000080
#define IO_TYPE_TOF              0x00000100
#define IO_TYPE_RTO              0x00000200
#define IO_TYPE_COUNTER          0x00000400
#define IO_TYPE_GENERAL          0x00000800
#define IO_TYPE_READ_ENC         0x00001000
#define IO_TYPE_RESET_ENC        0x00002000
#define IO_TYPE_READ_USS         0x00004000
#define IO_TYPE_WRITE_USS        0x00008000
#define IO_TYPE_SET_DA			 0x00010000
#define IO_TYPE_READ_MODBUS      0x00020000
#define IO_TYPE_WRITE_MODBUS     0x00040000
#define IO_TYPE_READ_MODBUS_ETH  0x00080000
#define IO_TYPE_WRITE_MODBUS_ETH 0x00100000
#define IO_TYPE_READ_YASKAWA	 0x00200000
#define IO_TYPE_WRITE_YASKAWA	 0x00400000
#define IO_TYPE_INTERNAL_FLAG	 0x80000000

    unsigned int  type;
#define NO_PIN_ASSIGNED         0
    int         pin;
	unsigned char bit;
} PlcProgramSingleIo;

#define MAX_IO  4096
typedef struct PlcProgramTag {
    struct {
        PlcProgramSingleIo  assignment[MAX_IO];
        int                 count;
    }           io;
    McuIoInfo  *mcu;
	ElemLeaf   *ParallelStart;
	struct {
		int         cycleTime;
		int         mcuClock;
		int         baudRate;	// RS485 baudrate
		int			UART;
		int			ModBUSID;
		unsigned char ip[4];
		unsigned char mask[4];
		unsigned char gw[4];
		unsigned char dns[4];
		char		sntp[126];
		int			gmt;
		unsigned char dailysave;
		int			diameter;
		int			pulses;
		float		factor;
		unsigned char x4;
		BOOL canSave;
	} settings;

#define MAX_RUNGS 999
    ElemSubcktSeries *rungs[MAX_RUNGS];
    BOOL              rungHasBreakPoint[MAX_RUNGS];
    BOOL              rungPowered[MAX_RUNGS];
    int               numRungs;
} PlcProgram;

//-----------------------------------------------
// For actually drawing the ladder logic on screen; constants that determine
// how the boxes are laid out in the window, need to know that lots of
// places for figuring out if a mouse click is in a box etc.

// dimensions, in characters, of the area reserved for 1 leaf element
#define POS_WIDTH   22
#define POS_HEIGHT  3

// offset from the top left of the window at which we start drawing, in pixels
#define X_PADDING    35
#define Y_PADDING    14

typedef struct PlcCursorTag {
    int left;
    int top;
    int width;
    int height;
} PlcCursor;

#define MAX_RECENT_ITEMS     10
#define MAX_RECENT_MENU_LEN 100

// Settings structure
typedef struct SettingsTag {
	// Simulation settings
	BOOL ShowSimulationWarnings;
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
// The syntax highlighting style colours; a structure for the palette.

typedef struct SyntaxHighlightingColoursTag {
    COLORREF    bg;             // background
    COLORREF    def;            // default foreground
    COLORREF    selected;       // selected element
    COLORREF    op;             // `op code' (like OSR, OSF, ADD, ...)
    COLORREF    punct;          // punctuation, like square or curly braces
    COLORREF    lit;            // a literal number
    COLORREF    name;           // the name of an item
    COLORREF    rungNum;        // rung numbers
    COLORREF    comment;        // user-written comment text

    COLORREF    bus;            // the `bus' at the right and left of screen

    COLORREF    simBg;          // background, simulation mode
    COLORREF    simRungNum;     // rung number, simulation mode
    COLORREF    simOff;         // de-energized element, simulation mode
    COLORREF    simOn;          // energzied element, simulation mode
    COLORREF    simBusLeft;     // the `bus,' can be different colours for
    COLORREF    simBusRight;    // right and left of the screen
} SyntaxHighlightingColours;
extern SyntaxHighlightingColours HighlightColours;

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


//-----------------------------------------------
// Function prototypes

// poptools.cpp
void ProcessMenu(int code);
void ProgramChanged(void);
void SetMenusEnabled(BOOL canNegate, BOOL canNormal, BOOL canResetOnly,
    BOOL canSetOnly, BOOL canDelete, BOOL canInsertEnd, BOOL canInsertOther,
    BOOL canPushRungDown, BOOL canPushRungUp, BOOL canInsertComment);
void SetUndoEnabled(BOOL undoEnabled, BOOL redoEnabled);
void RefreshScrollbars(void);
extern HINSTANCE Instance;
extern HWND MainWindow;
extern HDC Hdc;
extern PlcProgram Prog;
extern Settings POPSettings;
extern XMLWrapper XmlSettings;
extern char CurrentSaveFile[MAX_PATH];
extern char CurrentCompileFile[MAX_PATH];
extern McuIoInfo SupportedMcus[NUM_SUPPORTED_MCUS];
extern char *InternalFlags[]; // Internal flags available to the users.

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
HMENU MakeMainWindowMenus(void);
void VscrollProc(WPARAM wParam);
void HscrollProc(WPARAM wParam);
void GenerateIoListDontLoseSelection(void);
void RefreshControlsToSettings(void);
void MainWindowResized(void);
void ToggleSimulationMode(void);
void StopSimulation(void);
void PauseSimulation(void);
void StartSimulation(void);
void UpdateMainWindowTitleBar(void);
void StatusBarSetText(int bar, char * text);
void PopulateRecentListMenu(void);
void RefreshDrawWindow();
extern int ScrollWidth;
extern int ScrollHeight;
extern BOOL NeedHoriz;
extern HWND IoList;
extern HWND DrawWindow;
extern int IoListTop;
extern int IoListHeight;
extern HWND UartSimulationWindow;
extern HWND UartSimulationTextControl;
extern BOOL RealTimeSimulationRunning;

// draw.cpp
int ProgCountWidestRow(void);
int CountHeightOfElement(int which, void *elem);
BOOL DrawElement(int which, void *elem, int *cx, int *cy, BOOL poweredBefore);
void DrawEndRung(int cx, int cy);
extern int ColsAvailable;
extern BOOL SelectionActive;
extern BOOL ThisHighlighted;

// draw_outputdev.cpp
extern void (*DrawChars)(int, int, char *);
void CALLBACK BlinkCursor(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);
void PaintWindow(void);
void ExportDrawingAsText(char *file);
void InitForDrawing(void);
void SetUpScrollbars(BOOL *horizShown, SCROLLINFO *horiz, SCROLLINFO *vert);
int ScreenRowsAvailable(void);
int ScreenColsAvailable(void);
extern HFONT FixedWidthFont;
extern HFONT FixedWidthFontBold;
extern int SelectedGxAfterNextPaint;
extern int SelectedGyAfterNextPaint;
extern BOOL ScrollSelectedIntoViewAfterNextPaint;
extern int ScrollXOffset;
extern int ScrollYOffset;
extern int ScrollXOffsetMax;
extern int ScrollYOffsetMax;

// schematic.cpp
void SelectElement(int gx, int gy, int state);
void MoveCursorKeyboard(int keyCode, int shiftPressed);
void MoveCursorMouseClick(int x, int y);
BOOL MoveCursorTopLeft(void);
bool EditElementMouseDoubleclick(int x, int y);
bool EditSelectedElement(void);
bool MakeResetOnlySelected(void);
bool MakeSetOnlySelected(void);
bool MakeNormalSelected(void);
bool NegateSelected(void);
void ForgetFromGrid(void *p);
void ForgetEverything(void);
void WhatCanWeDoFromCursorAndTopology(void);
BOOL FindSelected(int *gx, int *gy);
void MoveCursorNear(int gx, int gy);
void ToggleBreakPoint(int y);

#define DISPLAY_MATRIX_X_SIZE 32
#define DISPLAY_MATRIX_Y_SIZE 4096
extern ElemLeaf *DisplayMatrix[DISPLAY_MATRIX_X_SIZE][DISPLAY_MATRIX_Y_SIZE];
extern int DisplayMatrixWhich[DISPLAY_MATRIX_X_SIZE][DISPLAY_MATRIX_Y_SIZE];
extern ElemLeaf DisplayMatrixFiller;
#define PADDING_IN_DISPLAY_MATRIX (&DisplayMatrixFiller)
#define VALID_LEAF(x) ((x) != NULL && (x) != PADDING_IN_DISPLAY_MATRIX)
extern ElemLeaf *Selected;
extern int SelectedWhich;

extern PlcCursor Cursor;
extern BOOL CanInsertEnd;
extern BOOL CanInsertOther;
extern BOOL CanInsertComment;

#define FAR_FIND_FIRST    0x01
#define FAR_FIND_NEXT     0x02
#define FAR_REPLACE_FIRST 0x11
#define FAR_REPLACE_NEXT  0x12
#define FAR_REPLACE_ALL   0x13

#define FAR_MODE_REPLACE(mode) (mode & 0x10)

unsigned int FindAndReplace(char *search_text, char *new_text, int mode);

// circuit.cpp
bool AddTimer(int which);
bool AddRTC(int which);
bool AddCoil(void);
bool AddContact(void);
bool AddEmpty(int which);
bool AddMove(void);
bool AddMath(int which);
bool AddSqrt(void);
bool AddCmp(int which);
bool AddReset(void);
bool AddCounter(int which);
bool AddReadAdc(void);
bool AddSetDA(void);
bool AddReadEnc(void);
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
bool AddComment(char *text);
bool AddSetBit(void);
bool AddCheckBit(void);
bool AddShiftRegister(void);
bool AddMasterRelay(void);
bool AddLookUpTable(void);
bool AddPiecewiseLinear(void);
bool AddFormattedString(void);
bool AddParallelStart(void);
int RemoveParallelStart(int which, void *any);
bool DeleteSelectedFromProgram(void);
bool DeleteSelectedRung(void);
bool InsertRung(BOOL afterCursor);
int RungContainingSelected(void);
BOOL ItemIsLastInCircuit(ElemLeaf *item);
BOOL UartFunctionUsed(void);
BOOL PwmFunctionUsed(void);
void CopyLeaf(ElemLeaf *leaf, int which);
bool PasteLeaf(void);
bool PushRung(bool up);
void NewProgram(void);
ElemLeaf *AllocLeaf(void);
ElemSubcktSeries *AllocSubcktSeries(void);
ElemSubcktParallel *AllocSubcktParallel(void);
void FreeCircuit(int which, void *any);
void FreeEntireProgram(void);
BOOL ContainsElem(int which, void *any, ElemLeaf *seek);

// undoredo.cpp
void UndoUndo(void);
void UndoRedo(void);
void UndoForget(void);
void UndoRemember(void);
void UndoFlush(void);
BOOL CanUndo(void);

// loadsave.cpp
BOOL LoadProjectFromFile(char *filename);
BOOL SaveProjectToFile(char *filename);
void SetAutoSaveInterval(int interval);
void CALLBACK AutoSaveNow(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);

// iolist.cpp
int GenerateIoList(int prevSel);
int GenerateIoMapList(int prevSel);
void SaveIoListToFile(FILE *f);
BOOL LoadIoListFromFile(FILE *f, int version);
void ShowIoDialog(int item);
void ShowIoMapDialog(int item);
void IoListProc(NMHDR *h);
void IoMapListProc(NMHDR *h);
void ShowAnalogSliderPopup(char *name);
void ShowEncoderSliderPopup(char *name);

// commentdialog.cpp
void ShowCommentDialog(char *comment);
// contactsdialog.cpp
void ShowContactsDialog(BOOL *negated, char *name, unsigned char * bit);
// coildialog.cpp
void ShowCoilDialog(BOOL *negated, BOOL *setOnly, BOOL *resetOnly, char *name, unsigned char *bit);
void ShowMultisetDADialog(ElemMultisetDA *l);
// simpledialog.cpp
void ShowTimerDialog(int which, int *delay, char *name);
void ShowRTCDialog(unsigned char * wday, unsigned char * mday, unsigned char * month, unsigned int * year, unsigned char * hour, unsigned char * minute, unsigned char * second);
void ShowCounterDialog(int which, int *count, char *name);
void ShowMoveDialog(char *dest, char *src);
void ShowReadAdcDialog(char *name);
void ShowSetBitDialog(char *name, int * set, int * bit);
void ShowCheckBitDialog(char *name, int * set, int * bit);
void ShowSetDADialog(char *name, int *mode);
void ShowReadEncDialog(char *name);
void ShowReadUSSDialog(char *name, int *id, int *parameter, int *parameter_set, int *index);
void ShowWriteUSSDialog(char *name, int *id, int *parameter, int *parameter_set, int *index);
void ShowModbusDialog(char *name, int *id, int *address, bool *set, bool *retransmitir);
void ShowReadModbusEthDialog(char *name, int *id, int *address);
void ShowWriteModbusEthDialog(char *name, int *id, int *address);
void ShowSetPwmDialog(char *name, int *targetFreq);
void ShowPersistDialog(char *var);
void ShowUartDialog(int which, char *name);
void ShowCmpDialog(int which, char *op1, char *op2);
void ShowMathDialog(int which, char *dest, char *op1, char *op2);
void ShowShiftRegisterDialog(char *name, int *stages);
void ShowFormattedStringDialog(char *var, char *string);
void ShowServoYaskawaDialog(char *id, char *var, char *string);
void ShowSimulationVarSetDialog(char *name, char *val);
void ShowLookUpTableDialog(ElemLeaf *l);
void ShowPiecewiseLinearDialog(ElemLeaf *l);
void ShowResetDialog(char *name);
void ShowSqrtDialog(char *dest, char *src);
// confdialog.cpp
extern struct strSerialConfig SerialConfig[];
extern char *SerialParityString[];
bool ShowConfDialog(bool NetworkSection);
// helpdialog.cpp
void ShowHelpDialog(BOOL about);
void OpenCHM(void);
void OpenCHM(unsigned int hID);
// FARdialog.cpp
bool ShowFARDialog();
// prefdialog.cpp
void ShowPrefDialog(void);

// iomap.cpp
void ExtractNamesFromCircuit(int which, void *any);
void UpdateTypeInCircuit(char *name, unsigned int type);
void UpdateTypesFromSeenPreviouslyList();
void UpdateTypeForInternalRelays();
BOOL ExistsCoilWithName(char *name);
int IoMap_GetIndex(PlcProgramSingleIo *io);
int IoMap_IsModBUS(PlcProgramSingleIo *io);

// miscutil.cpp
#define oops() { \
        dbp("bad at %d %s\n", __LINE__, __FILE__); \
        Error("Internal error at line %d file '%s'\n", __LINE__, __FILE__); \
        exit(1); \
    }
void dbp(char *str, ...);
void Error(char *str, ...);
void *CheckMalloc(size_t n);
void CheckFree(void *p);
extern HANDLE MainHeap;
void StartIhex(FILE *f);
void WriteIhex(FILE *f, BYTE b);
void FinishIhex(FILE *f);
char *IoTypeToString(int ioType);
void PinNumberForIo(char *dest, PlcProgramSingleIo *io);
HWND CreateWindowClient(DWORD exStyle, char *className, char *windowName,
    DWORD style, int x, int y, int width, int height, HWND parent,
    HMENU menu, HINSTANCE instance, void *param);
void MakeDialogBoxClass(void);
void NiceFont(HWND h);
void FixedFont(HWND h);
void CompileSuccessfulMessage(char *str);
void ProgramSuccessfulMessage(char *str);
bool IsNumber(char *str);
void LoadIOListToComboBox(HWND ComboBox, unsigned int mask);
int LoadCOMPorts(HWND ComboBox, unsigned int iDefaultPort, bool bHasAuto);
unsigned int GetTypeFromName(char *name);
bool IsInternalFlag(char *name);
bool IsValidNumber(char *number);
void ChangeFileExtension(char *name, char *ext);
char *GetPinADC(char *name);
unsigned short int CRC16(unsigned char *puchMsg, unsigned int usDataLen);
extern BOOL RunningInBatchMode;
extern HFONT MyNiceFont;
extern HFONT MyFixedFont;
extern HWND OkButton;
extern HWND CancelButton;
extern BOOL DialogDone;
extern BOOL DialogCancel;

// Regras de validação de variáveis
#define VALIDATE_IS_VAR				0x00000001
#define VALIDATE_IS_NUMBER			0x00000002
#define VALIDATE_IS_VAR_OR_NUMBER	0x00000004
#define VALIDATE_DONT_ASK           0x00000008
#define VALIDATE_TYPES_MUST_MATCH   0x00000010
#define VALIDATE_ACCEPT_IO_PENDING  0x00000020

bool IsValidNameAndType(char *old_name, char *name, unsigned int new_type);
bool IsValidNameAndType(char *old_name, char *name, char *FieldName, unsigned int Rules, unsigned int new_type, int MinVal, int MaxVal);

int iscontrol(WPARAM wParam);

// lang.cpp
char *_(char *in);

// Ribbon.cpp
extern unsigned int RibbonHeight;

extern HRESULT InitRibbon        (HWND hWindowFrame);
extern HRESULT RibbonSetCmdState (UINT uCmdID, BOOL bEnabled);
extern void    DestroyRibbon     (void);
extern void    SetApplicationMode(void);

// simulate.cpp
void SimulateOneCycle(BOOL forceRefresh);
void CALLBACK PlcCycleTimer(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);
void StartSimulationTimer(void);
void ClearSimulationData(void);
void DescribeForIoList(int val, int type, char *out);
void DescribeForIoList(char *name, int type, char *out);
void SimulationToggleContact(char *name);
void SetAdcShadow(char *name, SWORD val);
SWORD GetAdcShadow(char *name);
void SetEncShadow(char *name, SWORD val);
SWORD GetEncShadow(char *name);
void SetSimulationVariable(char *name, SWORD val);
SWORD GetSimulationVariable(char *name);
void DestroyUartSimulationWindow(void);
void ShowUartSimulationWindow(void);
extern BOOL InSimulationMode; 
extern BOOL SimulateRedrawAfterNextCycle;
#define MAX_SCROLLBACK 16384

// WatchPoint declarations
struct WatchPoint {
	char              *name;
	int                val;
	struct WatchPoint *next;
};

extern struct WatchPoint *listWP;

void  AddWP      (char *name, int val);
void  RemoveWP   (char *name);
int  *GetValWP   (char *name, int *val);
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
void AllocStart(void);
DWORD AllocOctetRam(void);
void AllocBitRam(DWORD *addr, int *bit);
void MemForVariable(char *name, DWORD *addrl, DWORD *addrh);
BYTE MuxForAdcVariable(char *name);
void MemForSingleBit(char *name, BOOL forRead, DWORD *addr, int *bit);
void MemCheckForErrorsPostCompile(void);
void BuildDirectionRegisters(BYTE *isInput, BYTE *isOutput);
void ComplainAboutBaudRateError(int divisor, double actual, double err);
void ComplainAboutBaudRateOverflow(void);
#define CompileError() longjmp(CompileErrorBuf, 1)
extern jmp_buf CompileErrorBuf;

// intcode.cpp
void IntDumpListing(char *outFile);
BOOL GenerateIntermediateCode(void);
// gcc.cpp
void GenerateDeclarations(FILE *f);
void CompileAnsiC(char *outFile);
DWORD CompileAnsiCToGCC(char *outFile);
DWORD GenerateCFile(char *filename);
// interpreted.c
void CompileInterpreted(char *outFile);

BOOL FlashProgram(char * hexFile, int ComPort, long BaudRate);

// debugdialog.cpp
void ShowDebugDialog(void);

// modbus_slave.cpp
void Init_MBDev_Slave        (void);
int  SimulationServer_Start  (void);
void SimulationServer_Stop   (void);
void SimulationServer_Message(WPARAM wParam, LPARAM lParam);

// mathdialog.cpp
void ShowMathDialog(int which, char *dest, char *op1, char *op2);

#endif
