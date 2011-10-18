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
// Constants, structures, declarations etc. for the PIC ladder logic compiler
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------

#ifndef __LDMICRO_H
#define __LDMICRO_H

#include <windows.h>
#include <tchar.h>
#include <wchar.h>
#include <math.h>

#include <setjmp.h>
typedef signed long SDWORD;
//typedef signed short SWORD;
typedef SDWORD SWORD;
//-----------------------------------------------
// `Configuration options.'

// The library that I use to do registry stuff.
#define FREEZE_SUBKEY "LDMicro"

#define DEFAULT_CPU "NXP LPC1768 LQFP100"

// Size of the font that we will use to draw the ladder diagrams, in pixels
#define FONT_WIDTH   7
#define FONT_HEIGHT 13

#include "resource.h"
#include "splash.h"

//-----------------------------------------------
// Constants for the GUI. We have drop-down menus, a listview for the I/Os,
// etc.

// Menu IDs

#define MNU_NEW                 0x01
#define MNU_OPEN                0x02
#define MNU_SAVE                0x03
#define MNU_SAVE_AS             0x04
#define MNU_EXPORT              0x05
#define MNU_EXIT                0x06

#define MNU_UNDO                0x10
#define MNU_REDO                0x11
#define MNU_PUSH_RUNG_UP        0x12
#define MNU_PUSH_RUNG_DOWN      0x13
#define MNU_INSERT_RUNG_BEFORE  0x14
#define MNU_INSERT_RUNG_AFTER   0x15
#define MNU_DELETE_ELEMENT      0x16
#define MNU_DELETE_RUNG         0x17

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
#define MNU_INSERT_FMTD_STR     0x40
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

#define MNU_MCU_SETTINGS        0x58
#define MNU_PROCESSOR_0         0xa0

#define MNU_SIMULATION_MODE     0x60
#define MNU_START_SIMULATION    0x61
#define MNU_STOP_SIMULATION     0x62
#define MNU_SINGLE_CYCLE        0x63

#define MNU_COMPILE             0x70
#define MNU_COMPILE_AS          0x71
#define MNU_PROGRAM				0x72
#define MNU_PROGRAM_AS			0x73

#define MNU_MANUAL              0x80
#define MNU_ABOUT               0x81

// Columns within the I/O etc. listview.
#define LV_IO_NAME              0x00
#define LV_IO_TYPE              0x01
#define LV_IO_STATE             0x02
#define LV_IO_PIN               0x03
#define LV_IO_PORT              0x04

// Timer IDs associated with the main window.
#define TIMER_BLINK_CURSOR      1
#define TIMER_SIMULATE          2

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
        case ELEM_MOVE: \
        case ELEM_SHORT: \
        case ELEM_OPEN: \
		case ELEM_SET_BIT: \
		case ELEM_CHECK_BIT: \
        case ELEM_READ_ADC: \
		case ELEM_SET_DA: \
        case ELEM_READ_ENC: \
        case ELEM_RESET_ENC: \
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
	unsigned char bit;
} ElemContacts;

typedef struct ElemCoilTag {
    char    name[MAX_NAME_LEN];
    BOOL    negated;
    BOOL    setOnly;
    BOOL    resetOnly;
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

typedef struct ElemSetDATag {
    char    name[MAX_NAME_LEN];
	int		value;
} ElemSetDA;

typedef struct ElemReadEncTag {
    char    name[MAX_NAME_LEN];
} ElemReadEnc;

typedef struct ElemResetEncTag {
    char    name[MAX_NAME_LEN];
} ElemResetEnc;

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
	int		value;
} ElemReadModbus;

typedef struct ElemWriteModbusTag {
    char    name[MAX_NAME_LEN];
	int		id;
	int		address;
	bool	int32;
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
        ElemCmp             cmp;
        ElemCounter         counter;
        ElemReadAdc         readAdc;
		ElemSetDA			setDA;
        ElemReadEnc         readEnc;
        ElemResetEnc        resetEnc;
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
#define IO_TYPE_PENDING         0

#define IO_TYPE_DIG_INPUT       1
#define IO_TYPE_DIG_OUTPUT      2
#define IO_TYPE_READ_ADC        3
#define IO_TYPE_UART_TX         4
#define IO_TYPE_UART_RX         5
#define IO_TYPE_PWM_OUTPUT      6
#define IO_TYPE_INTERNAL_RELAY  7
#define IO_TYPE_TON             8
#define IO_TYPE_TOF             9
#define IO_TYPE_RTO             10
#define IO_TYPE_COUNTER         11
#define IO_TYPE_GENERAL         12
#define IO_TYPE_READ_ENC        13
#define IO_TYPE_RESET_ENC       14
#define IO_TYPE_READ_USS        15
#define IO_TYPE_WRITE_USS       16
#define IO_TYPE_SET_DA			17
#define IO_TYPE_READ_MODBUS     18
#define IO_TYPE_WRITE_MODBUS    19
#define IO_TYPE_READ_MODBUS_ETH 20
#define IO_TYPE_WRITE_MODBUS_ETH 21
#define IO_TYPE_READ_YASKAWA	23
#define IO_TYPE_WRITE_YASKAWA	24

    int         type;
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
    int         cycleTime;
    int         mcuClock;
    int         baudRate;	// RS485 baudrate
	int			comPort;  // programming com port
	int			UART;
	unsigned char ip[4];
	unsigned char mask[4];
	unsigned char gw[4];

#define MAX_RUNGS 999
    ElemSubcktSeries *rungs[MAX_RUNGS];
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

// ldmicro.cpp
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
extern char CurrentSaveFile[MAX_PATH];
extern char CurrentCompileFile[MAX_PATH];
extern McuIoInfo SupportedMcus[NUM_SUPPORTED_MCUS];
// memory debugging, because I often get careless; ok() will check that the
// heap used for all the program storage is not yet corrupt, and oops() if
// it is
void CheckHeap(char *file, int line);
#define ok() CheckHeap(__FILE__, __LINE__)

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
void StartSimulation(void);
void UpdateMainWindowTitleBar(void);
void StatusBarSetText(int bar, char * text);
extern int ScrollWidth;
extern int ScrollHeight;
extern BOOL NeedHoriz;
extern HWND IoList;
extern int IoListTop;
extern int IoListHeight;
extern HWND UartSimulationWindow;
extern HWND UartSimulationTextControl;

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
void MoveCursorKeyboard(int keyCode);
void MoveCursorMouseClick(int x, int y);
BOOL MoveCursorTopLeft(void);
void EditElementMouseDoubleclick(int x, int y);
void EditSelectedElement(void);
void MakeResetOnlySelected(void);
void MakeSetOnlySelected(void);
void MakeNormalSelected(void);
void NegateSelected(void);
void ForgetFromGrid(void *p);
void ForgetEverything(void);
void WhatCanWeDoFromCursorAndTopology(void);
BOOL FindSelected(int *gx, int *gy);
void MoveCursorNear(int gx, int gy);

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

// circuit.cpp
void AddTimer(int which);
void AddRTC(int which);
void AddCoil(void);
void AddContact(void);
void AddEmpty(int which);
void AddMove(void);
void AddMath(int which);
void AddCmp(int which);
void AddReset(void);
void AddCounter(int which);
void AddReadAdc(void);
void AddSetDA(void);
void AddReadEnc(void);
void AddResetEnc(void);
void AddReadFormatString(void);
void AddWriteFormatString(void);
void AddReadServoYaskawa(void);
void AddWriteServoYaskawa(void);
void AddReadUSS(void);
void AddWriteUSS(void);
void AddReadModbus(void);
void AddWriteModbus(void);
void AddReadModbusEth(void);
void AddWriteModbusEth(void);
void AddSetPwm(void);
void AddUart(int which);
void AddPersist(void);
void AddComment(char *text);
void AddSetBit(void);
void AddCheckBit(void);
void AddShiftRegister(void);
void AddMasterRelay(void);
void AddLookUpTable(void);
void AddPiecewiseLinear(void);
void AddFormattedString(void);
void DeleteSelectedFromProgram(void);
void DeleteSelectedRung(void);
void InsertRung(BOOL afterCursor);
int RungContainingSelected(void);
BOOL ItemIsLastInCircuit(ElemLeaf *item);
BOOL UartFunctionUsed(void);
BOOL PwmFunctionUsed(void);
void PushRungUp(void);
void PushRungDown(void);
void NewProgram(void);
ElemLeaf *AllocLeaf(void);
ElemSubcktSeries *AllocSubcktSeries(void);
ElemSubcktParallel *AllocSubcktParallel(void);
void FreeCircuit(int which, void *any);
void FreeEntireProgram(void);
void UndoUndo(void);
void UndoRedo(void);
void UndoRemember(void);
void UndoFlush(void);
BOOL CanUndo(void);

// loadsave.cpp
BOOL LoadProjectFromFile(char *filename);
BOOL SaveProjectToFile(char *filename);

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
// simpledialog.cpp
void ShowTimerDialog(int which, int *delay, char *name);
void ShowRTCDialog(unsigned char * wday, unsigned char * mday, unsigned char * month, unsigned int * year, unsigned char * hour, unsigned char * minute, unsigned char * second);
void ShowCounterDialog(int which, int *count, char *name);
void ShowMoveDialog(char *dest, char *src);
void ShowReadAdcDialog(char *name);
void ShowSetBitDialog(char *name, int * set, int * bit);
void ShowCheckBitDialog(char *name, int * set, int * bit);
void ShowSetDADialog(char *name);
void ShowReadEncDialog(char *name);
void ShowResetEncDialog(char *name);
void ShowReadUSSDialog(char *name, int *id, int *parameter, int *parameter_set, int *index);
void ShowWriteUSSDialog(char *name, int *id, int *parameter, int *parameter_set, int *index);
void ShowReadModbusDialog(char *name, int *id, int *address);
void ShowWriteModbusDialog(char *name, int *id, int *address);
void ShowModbusDialog(char *name, int *id, int *address, bool *set);
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
void ShowLookUpTableDialog(ElemLeaf *l);
void ShowPiecewiseLinearDialog(ElemLeaf *l);
void ShowResetDialog(char *name);
// confdialog.cpp
void ShowConfDialog(void);
// helpdialog.cpp
void ShowHelpDialog(BOOL about);

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
extern BOOL RunningInBatchMode;
extern HFONT MyNiceFont;
extern HFONT MyFixedFont;
extern HWND OkButton;
extern HWND CancelButton;
extern BOOL DialogDone;
extern BOOL DialogCancel;

// lang.cpp
char *_(char *in);

// simulate.cpp
void SimulateOneCycle(BOOL forceRefresh);
void CALLBACK PlcCycleTimer(HWND hwnd, UINT msg, UINT_PTR id, DWORD time);
void StartSimulationTimer(void);
void ClearSimulationData(void);
void DescribeForIoList(char *name, char *out);
void SimulationToggleContact(char *name);
void SetAdcShadow(char *name, SWORD val);
SWORD GetAdcShadow(char *name);
void SetEncShadow(char *name, SWORD val);
SWORD GetEncShadow(char *name);
void DestroyUartSimulationWindow(void);
void ShowUartSimulationWindow(void);
extern BOOL InSimulationMode; 
extern BOOL SimulateRedrawAfterNextCycle;
#define MAX_SCROLLBACK 16384

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
// ansic.cpp
void GenerateDeclarations(FILE *f);
void GenerateAnsiC(FILE *f);
void CompileAnsiC(char *outFile);
DWORD CompileAnsiCToGCC(char *outFile);
// interpreted.c
void CompileInterpreted(char *outFile);

BOOL FlashProgram(char * hexFile, int ComPort, long BaudRate);

#endif
