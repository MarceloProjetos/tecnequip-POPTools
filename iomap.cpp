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
// Routines to maintain the processor I/O list. Whenever the user changes the
// name of an element, rebuild the I/O list from the PLC program, so that new
// assigned names are automatically reflected in the I/O list. Also keep a 
// list of old I/Os that have been deleted, so that if the user deletes a
// a name and then recreates it the associated settings (e.g. pin number)
// will not be forgotten. Also the dialog box for assigning I/O pins.
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldmicro.h"

// I/O that we have seen recently, so that we don't forget pin assignments
// when we re-extract the list
#define MAX_IO_SEEN_PREVIOUSLY 512
extern struct {
    char    name[MAX_NAME_LEN];
    int     type;
    int     pin;
	int		bit;
} IoSeenPreviously[MAX_IO_SEEN_PREVIOUSLY];
extern int IoSeenPreviouslyCount;

// stuff for the dialog box that lets you choose pin assignments
static BOOL DialogDone;
static BOOL DialogCancel;

static HWND IoDialog;

static HWND PinList;
static HWND OkButton;
static HWND CancelButton;
static HWND BitCombobox;

// stuff for the popup that lets you set the simulated value of an analog in
static HWND AnalogSliderMain;
static HWND AnalogSliderTrackbar;
static BOOL AnalogSliderDone;
static BOOL AnalogSliderCancel;

static int nSortColumn = 0;
static BOOL bSortAscending = TRUE;

// stuff for the popup that lets you set the simulated value of an encode in
/*static HWND EncoderSliderMain;
static HWND EncoderSliderTrackbar;
static BOOL EncoderSliderDone;
static BOOL EncoderSliderCancel;*/

const LPCTSTR ComboboxBitItens[] = { _("0"), _("1"), _("2"), _("3"), _("4"), _("5"), _("6"), _("7"), _("8"), _("9"), _("10"), 
									_("11"), _("12"), _("13"), _("14"), _("15"), _("16"), _("17"), _("18"), _("19"), _("20"), 
									_("21"), _("22"), _("23"), _("24"), _("25"), _("26"), _("27"), _("28"), _("29"), _("30"), 
									_("31")/*, _("32")*/};

//-----------------------------------------------------------------------------
// Append an I/O to the I/O list if it is not in there already.
//-----------------------------------------------------------------------------
static void AppendIo(char *name, int type, unsigned char bit)
{
    int i;
	int idx = 0;
    for(i = 0; i < Prog.io.count; i++) {
        if(_stricmp(Prog.io.assignment[i].name, name)==0) {
            if(type != IO_TYPE_GENERAL && Prog.io.assignment[i].type ==
                IO_TYPE_GENERAL)
            {
                Prog.io.assignment[i].type = type;
            }
            // already in there
            return;
        }
    }

    if(i < MAX_IO) {
        Prog.io.assignment[i].type = type;
        Prog.io.assignment[i].pin = NO_PIN_ASSIGNED;
		Prog.io.assignment[i].bit = bit;
        strcpy(Prog.io.assignment[i].name, name);
        (Prog.io.count)++;
    }
}

//-----------------------------------------------------------------------------
// Move an I/O pin into the `seen previously' list. This means that if the
// user creates input Xasd, assigns it a pin, deletes, and then recreates it,
// then it will come back with the correct pin assigned.
//-----------------------------------------------------------------------------
static void AppendIoSeenPreviously(char *name, int type, int pin, int bit)
{
    if(_stricmp(name+1, "new")==0) return;

    int i;
    for(i = 0; i < IoSeenPreviouslyCount; i++) {
        if(_stricmp(name, IoSeenPreviously[i].name)==0 &&
            type == IoSeenPreviously[i].type)
        {
            if(pin != NO_PIN_ASSIGNED) {
                IoSeenPreviously[i].pin = pin;
				IoSeenPreviously[i].bit = bit;
            }
            return;
        }
    }
    if(IoSeenPreviouslyCount >= MAX_IO_SEEN_PREVIOUSLY) {
        // maybe improve later; just throw away all our old information, and
        // the user might have to reenter the pin if they delete and recreate
        // things
        IoSeenPreviouslyCount = 0;
    }

    i = IoSeenPreviouslyCount;
    IoSeenPreviously[i].type = type;
    IoSeenPreviously[i].pin = pin;
	IoSeenPreviously[i].bit = bit;
    strcpy(IoSeenPreviously[i].name, name);
    IoSeenPreviouslyCount++;
}

//-----------------------------------------------------------------------------
// Walk a subcircuit, calling ourselves recursively and extracting all the
// I/O names out of it.
//-----------------------------------------------------------------------------
static void ExtractNamesFromCircuit(int which, void *any)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                ExtractNamesFromCircuit(p->contents[i].which,
                    p->contents[i].d.any);
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                ExtractNamesFromCircuit(s->contents[i].which,
                    s->contents[i].d.any);
            }
            break;
        }
        case ELEM_CONTACTS:
            switch(l->d.contacts.name[0]) {
                case 'R':
                    AppendIo(l->d.contacts.name, IO_TYPE_INTERNAL_RELAY, l->d.contacts.bit);
                    break;

                case 'Y':
                    AppendIo(l->d.contacts.name, IO_TYPE_DIG_OUTPUT, l->d.contacts.bit);
                    break;

                case 'X':
                    AppendIo(l->d.contacts.name, IO_TYPE_DIG_INPUT, l->d.contacts.bit);
                    break;

                default:
                    oops();
                    break;
            }
            break;

        case ELEM_COIL:
            AppendIo(l->d.coil.name, l->d.coil.name[0] == 'R' ? IO_TYPE_INTERNAL_RELAY : IO_TYPE_DIG_OUTPUT, l->d.coil.bit);
            break;

        case ELEM_TON:
        case ELEM_TOF:
            AppendIo(l->d.timer.name, which == ELEM_TON ?  IO_TYPE_TON :
                IO_TYPE_TOF, 0);
            break;

        case ELEM_RTO:
            AppendIo(l->d.timer.name, IO_TYPE_RTO, 0);
            break;

        case ELEM_MOVE:
            AppendIo(l->d.move.dest, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_SET_BIT:
			AppendIo(l->d.setBit.name, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_CHECK_BIT:
			AppendIo(l->d.checkBit.name, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_ADD:
        case ELEM_SUB:
        case ELEM_MUL:
        case ELEM_DIV:
            AppendIo(l->d.math.dest, IO_TYPE_GENERAL, 0);
            break;

		case ELEM_READ_FORMATTED_STRING:
		case ELEM_WRITE_FORMATTED_STRING:
        case ELEM_FORMATTED_STRING:
            if(strlen(l->d.fmtdStr.var) > 0) {
                AppendIo(l->d.fmtdStr.var, IO_TYPE_UART_TX, 0);
            }
            break;

		case ELEM_READ_SERVO_YASKAWA:
            if(strlen(l->d.servoYaskawa.var) > 0) {
                AppendIo(l->d.servoYaskawa.var, IO_TYPE_READ_YASKAWA, 0);
            }
            break;

		case ELEM_WRITE_SERVO_YASKAWA:
            if(strlen(l->d.servoYaskawa.var) > 0) {
                AppendIo(l->d.servoYaskawa.var, IO_TYPE_WRITE_YASKAWA, 0);
            }
            break;

        case ELEM_UART_SEND:
            AppendIo(l->d.uart.name, IO_TYPE_UART_TX, 0);
            break;

        case ELEM_UART_RECV:
            AppendIo(l->d.uart.name, IO_TYPE_UART_RX, 0);
            break;

        case ELEM_SET_PWM:
            AppendIo(l->d.setPwm.name, IO_TYPE_PWM_OUTPUT, 0);
            break;

        case ELEM_CTU:
        case ELEM_CTD:
        case ELEM_CTC:
            AppendIo(l->d.counter.name, IO_TYPE_COUNTER, 0);
            break;

        case ELEM_READ_ADC:
            AppendIo(l->d.readAdc.name, IO_TYPE_READ_ADC, 0);
            break;

        case ELEM_SET_DA:
            AppendIo(l->d.setDA.name, IO_TYPE_SET_DA, 0);
            break;

        case ELEM_READ_ENC:
            AppendIo(l->d.readEnc.name, IO_TYPE_READ_ENC, 0);
            break;

        case ELEM_RESET_ENC:
            AppendIo(l->d.resetEnc.name, IO_TYPE_RESET_ENC, 0);
            break;

        case ELEM_READ_USS:
            AppendIo(l->d.readUSS.name, IO_TYPE_READ_USS, 0);
            break;

        case ELEM_WRITE_USS:
            AppendIo(l->d.writeUSS.name, IO_TYPE_WRITE_USS, 0);
            break;

        case ELEM_READ_MODBUS:
            AppendIo(l->d.readModbus.name, IO_TYPE_READ_MODBUS, 0);
            break;

        case ELEM_WRITE_MODBUS:
            AppendIo(l->d.writeModbus.name, IO_TYPE_WRITE_MODBUS, 0);
            break;

        case ELEM_READ_MODBUS_ETH:
            AppendIo(l->d.readModbusEth.name, IO_TYPE_READ_MODBUS_ETH, 0);
            break;

        case ELEM_WRITE_MODBUS_ETH:
            AppendIo(l->d.writeModbusEth.name, IO_TYPE_WRITE_MODBUS_ETH, 0);
            break;

        case ELEM_SHIFT_REGISTER: {
            int i;
            for(i = 0; i < l->d.shiftRegister.stages; i++) {
                char str[MAX_NAME_LEN+10];
                sprintf(str, "%s%d", l->d.shiftRegister.name, i);
                AppendIo(str, IO_TYPE_GENERAL, 0);
            }
            break;
        }

        case ELEM_LOOK_UP_TABLE:
            AppendIo(l->d.lookUpTable.dest, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_PIECEWISE_LINEAR:
            AppendIo(l->d.piecewiseLinear.dest, IO_TYPE_GENERAL, 0);
            break;

		case ELEM_RTC:
        case ELEM_PLACEHOLDER:
        case ELEM_COMMENT:
        case ELEM_SHORT:
        case ELEM_OPEN:
        case ELEM_MASTER_RELAY:
        case ELEM_ONE_SHOT_RISING:
        case ELEM_ONE_SHOT_FALLING:
        case ELEM_EQU:
        case ELEM_NEQ:
        case ELEM_GRT:
        case ELEM_GEQ:
        case ELEM_LES:
        case ELEM_LEQ:
        case ELEM_RES:
        case ELEM_PERSIST:
            break;

        default:
            oops();
    }
}

//-----------------------------------------------------------------------------
// Compare function to qsort() the I/O list. Group by type, then 
// alphabetically within each section.
//-----------------------------------------------------------------------------
static int CompareIoName(const void *av, const void *bv)
{
    PlcProgramSingleIo *a = (PlcProgramSingleIo *)av;
    PlcProgramSingleIo *b = (PlcProgramSingleIo *)bv;

    return bSortAscending ? _stricmp(a->name, b->name) : _stricmp(b->name, a->name);
}

static int CompareIoPin(const void *av, const void *bv)
{
    PlcProgramSingleIo *a = (PlcProgramSingleIo *)av;
    PlcProgramSingleIo *b = (PlcProgramSingleIo *)bv;

    /*if(a->type != b->type) {
        return a->type - b->type;
    }*/

    if(a->pin == NO_PIN_ASSIGNED && b->pin != NO_PIN_ASSIGNED) return  1;
    if(b->pin == NO_PIN_ASSIGNED && a->pin != NO_PIN_ASSIGNED) return -1;

    return bSortAscending ? a->pin - b->pin : b->pin - a->pin;
}

static int CompareIoPort(const void *av, const void *bv)
{
    PlcProgramSingleIo *a = (PlcProgramSingleIo *)av;
    PlcProgramSingleIo *b = (PlcProgramSingleIo *)bv;

	int i1 = 0;
	int i2 = 0;

    //if(a->type != b->type) {
    //    return a->type - b->type;
    //}

    if(a->pin == NO_PIN_ASSIGNED && b->pin != NO_PIN_ASSIGNED) return  1;
    if(b->pin == NO_PIN_ASSIGNED && a->pin != NO_PIN_ASSIGNED) return -1;

    if(Prog.mcu && Prog.mcu->whichIsa == ISA_ANSIC) {
        i1 = 0;
    }

    if(a->type != IO_TYPE_DIG_INPUT && a->type != IO_TYPE_DIG_OUTPUT
        && a->type != IO_TYPE_READ_ADC && a->type != IO_TYPE_READ_ENC && a->type != IO_TYPE_RESET_ENC
		&& a->type != IO_TYPE_READ_USS && a->type != IO_TYPE_WRITE_USS)
    {
        i1 = 0;;
    }

    int pin = a->pin;
    if(pin == NO_PIN_ASSIGNED || !Prog.mcu) {
        i1 = 0;
    }

    if(UartFunctionUsed() && Prog.mcu) {
        if((Prog.mcu->uartNeeds.rxPin == pin) ||
            (Prog.mcu->uartNeeds.txPin == pin))
        {
            i1 = 0; // "<UART needs!>"
        }
    }

    if(PwmFunctionUsed() && Prog.mcu) {
        if(Prog.mcu->pwmNeedsPin == pin) {
            i1 = 0; // "<PWM needs!>"
        }
    }

    int j;
    for(j = 0; j < Prog.mcu->pinCount; j++) 
	{
        if(Prog.mcu->pinInfo[j].pin == pin) 
		{
			if (a->type == IO_TYPE_DIG_OUTPUT && a->pin == 17)
				i1 = 90000;
			else if (a->type == IO_TYPE_DIG_OUTPUT && a->pin == 18)
				i1 = 90001;
			else if (a->type == IO_TYPE_DIG_OUTPUT && a->pin > 19)
				i1 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + a->bit; // "M%d.%d"
			else if (a->type == IO_TYPE_DIG_OUTPUT)
				i1 = 40000 + Prog.mcu->pinInfo[j].bit;
			else if (a->type == IO_TYPE_DIG_INPUT && a->pin > 19)
				i1 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + a->bit;
			else if (a->type == IO_TYPE_DIG_INPUT)
				i1 = 30000 + Prog.mcu->pinInfo[j].bit;
			else if (a->type == IO_TYPE_READ_ADC)
				if (a->pin == 6)
					i1 = 90003;
				else
					i1 = 60000 + Prog.mcu->pinInfo[j].bit;
			else if (a->type == IO_TYPE_READ_ENC || a->type == IO_TYPE_RESET_ENC)
				i1 = 70000 + Prog.mcu->pinInfo[j].bit;
            break;
        }
    }
    if(j == Prog.mcu->pinCount) {
        i1 = 0; // "<not an I/O!>";
    }

	// 2o. parameter content compare 
    if(Prog.mcu && Prog.mcu->whichIsa == ISA_ANSIC) {
        i2 = 0;
    }

    if(b->type != IO_TYPE_DIG_INPUT && b->type != IO_TYPE_DIG_OUTPUT
        && b->type != IO_TYPE_READ_ADC && b->type != IO_TYPE_READ_ENC && b->type != IO_TYPE_RESET_ENC
		&& b->type != IO_TYPE_READ_USS && b->type != IO_TYPE_WRITE_USS)
    {
        i2 = 0;
    }

    pin = b->pin;
    if(pin == NO_PIN_ASSIGNED || !Prog.mcu) {
        i2 = 0;
    }

    if(UartFunctionUsed() && Prog.mcu) {
        if((Prog.mcu->uartNeeds.rxPin == pin) ||
            (Prog.mcu->uartNeeds.txPin == pin))
        {
            i2 = 0; // "<UART needs!>";
        }
    }

    if(PwmFunctionUsed() && Prog.mcu) {
        if(Prog.mcu->pwmNeedsPin == pin) {
            i2 = 0; // "<PWM needs!>";
        }
    }

    for(j = 0; j < Prog.mcu->pinCount; j++) 
	{
        if(Prog.mcu->pinInfo[j].pin == pin) 
		{
			if (b->type == IO_TYPE_DIG_OUTPUT && b->pin == 17)
				i2 = 90000;
			else if (b->type == IO_TYPE_DIG_OUTPUT && b->pin == 18)
				i2 = 90001;
			else if (b->type == IO_TYPE_DIG_OUTPUT && b->pin > 19)
				i2 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + b->bit; // "M%d.%d"
			else if (b->type == IO_TYPE_DIG_OUTPUT)
				i2 = 40000 + Prog.mcu->pinInfo[j].bit;
			else if (b->type == IO_TYPE_DIG_INPUT && b->pin > 19)
				i2 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + b->bit;
			else if (b->type == IO_TYPE_DIG_INPUT)
				i2 = 30000 + Prog.mcu->pinInfo[j].bit;
			else if (b->type == IO_TYPE_READ_ADC)
				if (b->pin == 6)
					i2 = 90003;
				else
					i2 = 60000 + Prog.mcu->pinInfo[j].bit;
			else if (b->type == IO_TYPE_READ_ENC || b->type == IO_TYPE_RESET_ENC)
				i2 = 70000 + Prog.mcu->pinInfo[j].bit;
            break;
        }
    }
    if(j == Prog.mcu->pinCount) {
        i2 = 0; // "<not an I/O!>";
    }

    return bSortAscending ? i1 - i2 : i2 - i1;
}

static int CompareIoType(const void *av, const void *bv)
{
    PlcProgramSingleIo *a = (PlcProgramSingleIo *)av;
    PlcProgramSingleIo *b = (PlcProgramSingleIo *)bv;

    //if(a->type != b->type) {
    //    return a->type - b->type;
    //}

    //if(a->pin == NO_PIN_ASSIGNED && b->pin != NO_PIN_ASSIGNED) return  1;
    //if(b->pin == NO_PIN_ASSIGNED && a->pin != NO_PIN_ASSIGNED) return -1;

	return bSortAscending ? a->type - b->type : b->type - a->type;
}
//-----------------------------------------------------------------------------
// Wipe the I/O list and then re-extract it from the PLC program, taking
// care not to forget the pin assignments. Gets passed the selected item
// as an index into the list; modifies the list, so returns the new selected
// item as an index into the new list.
//-----------------------------------------------------------------------------
int GenerateIoMapList(int prevSel)
{
    int i, j;

    char selName[MAX_NAME_LEN];
    if(prevSel >= 0) {
        strcpy(selName, Prog.io.assignment[prevSel].name);
    }

    if(IoSeenPreviouslyCount > MAX_IO_SEEN_PREVIOUSLY/2) {
        // flush it so there's lots of room, and we don't run out and
        // forget important things
        IoSeenPreviouslyCount = 0;
    }
    
    // remember the pin assignments
    for(i = 0; i < Prog.io.count; i++) {
        AppendIoSeenPreviously(Prog.io.assignment[i].name,
            Prog.io.assignment[i].type, Prog.io.assignment[i].pin, Prog.io.assignment[i].bit);
    }
    // wipe the list
    Prog.io.count = 0;
    // extract the new list so that it must be up to date
    for(i = 0; i < Prog.numRungs; i++) {
        ExtractNamesFromCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    }

    for(i = 0; i < Prog.io.count; i++) {
        if(Prog.io.assignment[i].type == IO_TYPE_DIG_INPUT ||
           Prog.io.assignment[i].type == IO_TYPE_DIG_OUTPUT ||
           Prog.io.assignment[i].type == IO_TYPE_READ_ADC ||
           Prog.io.assignment[i].type == IO_TYPE_READ_ENC ||
           Prog.io.assignment[i].type == IO_TYPE_RESET_ENC ||
		   Prog.io.assignment[i].type == IO_TYPE_READ_USS || 
		   Prog.io.assignment[i].type == IO_TYPE_WRITE_USS)
        {
            for(j = 0; j < IoSeenPreviouslyCount; j++) 
			{
                if(_stricmp(Prog.io.assignment[i].name, IoSeenPreviously[j].name) == 0 && Prog.io.assignment[i].type == IoSeenPreviously[j].type)
                {
                    Prog.io.assignment[i].pin = IoSeenPreviously[j].pin;
					Prog.io.assignment[i].bit = IoSeenPreviously[j].bit;
                    break;
                }
            }
        }
    }

	qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo),
        CompareIoName);

    if(prevSel >= 0) {
        for(i = 0; i < Prog.io.count; i++) {
            if(_stricmp(Prog.io.assignment[i].name, selName)==0)
                break;
        }
        if(i < Prog.io.count)
            return i;
    }
    // no previous, or selected was deleted
    return -1;
}

//-----------------------------------------------------------------------------
// Dialog proc for the popup that lets you set the value of an analog input for
// simulation.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK AnalogSliderDialogProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            AnalogSliderDone = TRUE;
            AnalogSliderCancel = TRUE;
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

//-----------------------------------------------------------------------------
// Window proc for the contacts dialog box
//-----------------------------------------------------------------------------
static LRESULT CALLBACK IoDialogProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
        case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == OkButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
            } else if(h == CancelButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
            } else if(h == PinList && HIWORD(wParam) == LBN_DBLCLK) {
                DialogDone = TRUE;
            }
            break;
        }

        case WM_CLOSE:
        case WM_DESTROY:
            DialogDone = TRUE;
            DialogCancel = TRUE;
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

//-----------------------------------------------------------------------------
// Create our window class; nothing exciting.
//-----------------------------------------------------------------------------
static BOOL MakeWindowClass()
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)IoDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "LDmicroIo";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 32, 32, 0);
    wc.hIconSm          = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 16, 16, 0);

    return RegisterClassEx(&wc);
}

static void MakeControls(void)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Assign:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        6, 1, 80, 17, IoDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    PinList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTBOX, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | WS_VSCROLL |
        LBS_NOTIFY, 6, 18, 115, 320, IoDialog, NULL, Instance, NULL);
    FixedFont(PinList);
	
    HWND textLabel2 = CreateWindowEx(0, WC_STATIC, _("Bit:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        6, 325, 35, 21, IoDialog, NULL, Instance, NULL);
    NiceFont(textLabel2);

	BitCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        55, 325, 66, 140, IoDialog, NULL, Instance, NULL);
    NiceFont(BitCombobox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        10, 365, 105, 23, IoDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        10, 396, 105, 23, IoDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

void ShowIoMapDialog(int item)
{
    if(!Prog.mcu) {
        MessageBox(MainWindow,
            _("No microcontroller has been selected. You must select a "
            "microcontroller before you can assign I/O pins.\r\n\r\n"
            "Select a microcontroller under the Settings menu and try "
            "again."), _("I/O Pin Assignment"), MB_OK | MB_ICONWARNING);
        return;
    }

    if(Prog.mcu->whichIsa == ISA_ANSIC) {
        Error(_("Can't specify I/O assignment for ANSI C target; compile and "
            "see comments in generated source code."));
        return;
    }

    if(Prog.mcu->whichIsa == ISA_INTERPRETED) {
        Error(_("Can't specify I/O assignment for interpretable target; see "
            "comments in reference implementation of interpreter."));
        return;
    }

    if(Prog.io.assignment[item].name[0] != 'X' && 
       Prog.io.assignment[item].name[0] != 'Y' &&
       Prog.io.assignment[item].name[0] != 'A' &&
       Prog.io.assignment[item].name[0] != 'E' &&
       Prog.io.assignment[item].name[0] != 'Z')
    {
        Error(_("Can only assign pin number to input/output pins (Xname or "
            "Yname or Aname)."));
        return;
    }

    if(Prog.io.assignment[item].name[0] == 'A' && Prog.mcu->adcCount == 0) {
        Error(_("No ADC or ADC not supported for this micro."));
        return;
    }

    if(Prog.io.assignment[item].name[0] == 'E' && Prog.mcu->encCount == 0) {
        Error(_("ENCODER not supported for this micro."));
        return;
    }

    if(_stricmp(Prog.io.assignment[item].name+1, "new")==0) {
        Error(_("Rename I/O from default name ('%s') before assigning "
            "MCU pin."), Prog.io.assignment[item].name);
        return;
    }

    MakeWindowClass();

    // We need the TOOLWINDOW style, or else the window will be forced to
    // a minimum width greater than our current width. And without the
    // APPWINDOW style, it becomes impossible to get the window back (by
    // Alt+Tab or taskbar).
    IoDialog = CreateWindowClient(WS_EX_TOOLWINDOW | WS_EX_APPWINDOW,
        "LDmicroIo", _("I/O Pin"),
        WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 127, 430, MainWindow, NULL, Instance, NULL);

    MakeControls();

    char buf[40];
	int i = 0;
	int PinListItemCount = 0;

	for (i = 0; i < sizeof(ComboboxBitItens) / sizeof(ComboboxBitItens[0]); i++)
		SendMessage(BitCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBitItens[i]));

	SendMessage(BitCombobox, CB_SETCURSEL, Prog.io.assignment[item].bit, 0);

    SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)_("(no pin)"));
	PinListItemCount++;

	for(i = 0; i < Prog.mcu->pinCount; i++) {
        int j;
        for(j = 0; j < Prog.io.count; j++) 
		{
            if(j == item) continue;
            if(Prog.io.assignment[j].pin == Prog.mcu->pinInfo[i].pin && Prog.io.assignment[j].type == Prog.io.assignment[item].type && 
				!((Prog.io.assignment[j].name[0] == 'X' && i > 18) || (Prog.io.assignment[j].name[0] == 'Y' && i > 68)) )
			{
                goto cant_use_this_io;
            }
		}

		if(UartFunctionUsed() && Prog.mcu &&
                ((Prog.mcu->pinInfo[i].pin == Prog.mcu->uartNeeds.rxPin) ||
                 (Prog.mcu->pinInfo[i].pin == Prog.mcu->uartNeeds.txPin)))
        {
            goto cant_use_this_io;
        }

        if(PwmFunctionUsed() && 
            Prog.mcu->pinInfo[i].pin == Prog.mcu->pwmNeedsPin)
        {
            goto cant_use_this_io;
        }

        if(Prog.io.assignment[item].name[0] == 'A') {
            for(j = 0; j < Prog.mcu->adcCount; j++) 
			{
                //if(Prog.mcu->adcInfo[j].pin == Prog.mcu->pinInfo[i].pin) 
				//{
				if (j == Prog.mcu->adcCount - 1)
					sprintf(buf, "%3d TEMP", Prog.mcu->adcInfo[j].pin);
				else
					sprintf(buf, "%3d ADC%d", Prog.mcu->adcInfo[j].pin, Prog.mcu->adcInfo[j].muxRegValue);

				SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)buf);
				PinListItemCount++;
                //}
            }
            if(j == Prog.mcu->adcCount) {
				break;
                //goto cant_use_this_io;
            }
        }

        if(Prog.io.assignment[item].name[0] == 'E' || Prog.io.assignment[item].name[0] == 'Z') {
            for(j = 0; j < Prog.mcu->encCount; j++) 
			{
                //if(Prog.mcu->adcInfo[j].pin == Prog.mcu->pinInfo[i].pin) 
				//{
					sprintf(buf, "%3d ENC%d", Prog.mcu->encInfo[j].pin, Prog.mcu->encInfo[j].muxRegValue);
					SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)buf);
					PinListItemCount++;
                //}
            }
            if(j == Prog.mcu->encCount) {
				break;
                //goto cant_use_this_io;
            }
        }

		if ((Prog.io.assignment[item].name[0] == 'X' && i < 51) ||
			(Prog.io.assignment[item].name[0] == 'Y' && i > 50))
		{
			if (i == 67)
				sprintf(buf, "%3d LED ERRO", Prog.mcu->pinInfo[i].bit,
					Prog.mcu->pinInfo[i].port,
					Prog.mcu->pinInfo[i].pin);
			else if (i == 68)
				sprintf(buf, "%3d LED CPU", Prog.mcu->pinInfo[i].bit,
					Prog.mcu->pinInfo[i].port,
					Prog.mcu->pinInfo[i].pin);
			else
				sprintf(buf, "%3d %c%d", Prog.mcu->pinInfo[i].pin,
					Prog.mcu->pinInfo[i].port,
					Prog.mcu->pinInfo[i].bit);

			SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)buf);
			PinListItemCount++;
		}
cant_use_this_io:;
    }

    EnableWindow(MainWindow, FALSE);
    ShowWindow(IoDialog, TRUE);
    SetFocus(PinList);

	//SendMessage(PinList, LB_SETCURSEL, item, 0);
	for (i = 0; i < PinListItemCount; i++)
	{
		int sel = SendMessage(PinList, LB_GETSEL, i, 0);
        char pin[16];
        SendMessage(PinList, LB_GETTEXT, (WPARAM)i, (LPARAM)pin);
		if (Prog.io.assignment[item].pin == atoi(pin))
			SendMessage(PinList, LB_SETCURSEL, i, 0);
	}

    MSG msg;
    DWORD ret;
    DialogDone = FALSE;
    DialogCancel = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                DialogDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
                break;
            }
        }

        if(IsDialogMessage(IoDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
        int sel = SendMessage(PinList, LB_GETCURSEL, 0, 0);
        char pin[16];
        SendMessage(PinList, LB_GETTEXT, (WPARAM)sel, (LPARAM)pin);
        if(_stricmp(pin, _("(no pin)"))==0) {
            int i;
            for(i = 0; i < IoSeenPreviouslyCount; i++) {
                if(_stricmp(IoSeenPreviously[i].name,
					Prog.io.assignment[item].name)==0 && IoSeenPreviously[i].type == Prog.io.assignment[item].type)
                {
                    IoSeenPreviously[i].pin = NO_PIN_ASSIGNED;
					IoSeenPreviously[i].bit = NO_PIN_ASSIGNED;
                }
            }
            Prog.io.assignment[item].pin = NO_PIN_ASSIGNED;
			Prog.io.assignment[item].bit = NO_PIN_ASSIGNED;
        } else {
            Prog.io.assignment[item].pin = atoi(pin);

			char buf[16];
			SendMessage(BitCombobox, WM_GETTEXT, (WPARAM)sizeof(buf),
				(LPARAM)(buf));

			Prog.io.assignment[item].bit = atoi(buf);

			for(int i = 0; i < DISPLAY_MATRIX_X_SIZE; i++) 
			{
				for(int j = 0; j < DISPLAY_MATRIX_Y_SIZE; j++) 
				{
					ElemLeaf *l = DisplayMatrix[i][j];
					if (l && DisplayMatrixWhich[i][j] == ELEM_COIL) 
						if (_stricmp(Prog.io.assignment[item].name, l->d.coil.name) == 0)
							l->d.coil.bit = Prog.io.assignment[item].bit;
					if (l && DisplayMatrixWhich[i][j] == ELEM_CONTACTS) 
						if (_stricmp(Prog.io.assignment[item].name, l->d.contacts.name) == 0)
							l->d.contacts.bit = Prog.io.assignment[item].bit;
				}
			}
			// Only one name can be bound to each pin; make sure that there's
            // not another entry for this pin in the IoSeenPreviously list,
            // that might get used if the user creates a new pin with that
            // name.
            int i;
            for(i = 0; i < IoSeenPreviouslyCount; i++) {
				if(IoSeenPreviously[i].pin == atoi(pin) && IoSeenPreviously[i].type == Prog.io.assignment[item].type) {
                    IoSeenPreviously[i].pin = NO_PIN_ASSIGNED;
					IoSeenPreviously[i].bit = NO_PIN_ASSIGNED;
                }
            }

		}
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(IoDialog);
    return;
}

//-----------------------------------------------------------------------------
// Called in response to a notify for the listview. Handles click, text-edit
// operations etc., but also gets called to find out what text to display
// where (LPSTR_TEXTCALLBACK); that way we don't have two parallel copies of
// the I/O list to keep in sync.
//-----------------------------------------------------------------------------
int CALLBACK CompareListItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	BOOL bSortAscending = (lParamSort > 0);
	int nColumn = abs(lParamSort) - 1;

	char lpszItem1[MAX_NAME_LEN];
	char lpszItem2[MAX_NAME_LEN];

	LVITEM itm;
	itm.mask = LVIF_TEXT;
	itm.iSubItem = nColumn;

	itm.iItem = lParam1;
	itm.pszText = lpszItem1;
	itm.cchTextMax = sizeof(lpszItem1);

	ListView_GetItem(IoList, &itm);

	itm.iItem = lParam2;
	itm.pszText = lpszItem2;
	itm.cchTextMax = sizeof(lpszItem2);

	ListView_GetItem(IoList, &itm);

	return bSortAscending ? _stricmp(lpszItem1, lpszItem2) : _stricmp(lpszItem2, lpszItem1);
}

void IoMapListProc(NMHDR *h)
{
	int c = LVN_COLUMNCLICK;
    switch(h->code) {
		case LVN_COLUMNCLICK:
			{
				LPARAM lParamSort;

				LPNMLISTVIEW pLVInfo = (LPNMLISTVIEW)h;

				// get new sort parameters
				if (pLVInfo->iSubItem == nSortColumn)
					bSortAscending = !bSortAscending;
				else
				{
					nSortColumn = pLVInfo->iSubItem;
					bSortAscending = TRUE;
				}

				// combine sort info into a single value we can send to our sort function
				lParamSort = 1 + nSortColumn;
				if (!bSortAscending)
					lParamSort = -lParamSort;

				// sort list
				//ListView_SortItems(pLVInfo->hdr.hwndFrom, CompareListItems, lParamSort);
				if (nSortColumn == 0)
					qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo), CompareIoName);
				else if (nSortColumn == 1)
					qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo), CompareIoType);
				else if (nSortColumn == 3)
					qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo), CompareIoPin);
				else if (nSortColumn == 4)
					qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo), CompareIoPort);

				RefreshControlsToSettings();

			}
			break;
        case LVN_GETDISPINFO: {
            NMLVDISPINFO *i = (NMLVDISPINFO *)h;
            int item = i->item.iItem;
            switch(i->item.iSubItem) {
                case LV_IO_PIN:
                    // Don't confuse people by displaying bogus pin assignments
                    // for the C target.
                    if(Prog.mcu && (Prog.mcu->whichIsa == ISA_ANSIC ||
                                    Prog.mcu->whichIsa == ISA_INTERPRETED) )
                    {
                        strcpy(i->item.pszText, "");
                        break;
                    }

                    PinNumberForIo(i->item.pszText,
                        &(Prog.io.assignment[item]));
                    break;

                case LV_IO_TYPE: {
                    char *s = IoTypeToString(Prog.io.assignment[item].type);
                    strcpy(i->item.pszText, s);
                    break;
                }
                case LV_IO_NAME:
                    strcpy(i->item.pszText, Prog.io.assignment[item].name);
                    break;

                case LV_IO_PORT: {
                    // Don't confuse people by displaying bogus pin assignments
                    // for the C target.
                    if(Prog.mcu && Prog.mcu->whichIsa == ISA_ANSIC) {
                        strcpy(i->item.pszText, "");
                        break;
                    }

                    int type = Prog.io.assignment[item].type;
                    if(type != IO_TYPE_DIG_INPUT && type != IO_TYPE_DIG_OUTPUT
                        && type != IO_TYPE_READ_ADC && type != IO_TYPE_READ_ENC && type != IO_TYPE_RESET_ENC
						&& type != IO_TYPE_READ_USS && type != IO_TYPE_WRITE_USS)
                    {
                        strcpy(i->item.pszText, "");
                        break;
                    }

                    int pin = Prog.io.assignment[item].pin;
                    if(pin == NO_PIN_ASSIGNED || !Prog.mcu) {
                        strcpy(i->item.pszText, "");
                        break;
                    }

                    if(UartFunctionUsed() && Prog.mcu) {
                        if((Prog.mcu->uartNeeds.rxPin == pin) ||
                           (Prog.mcu->uartNeeds.txPin == pin))
                        {
                            strcpy(i->item.pszText, _("<UART needs!>"));
                            break;
                        }
                    }

                    if(PwmFunctionUsed() && Prog.mcu) {
                        if(Prog.mcu->pwmNeedsPin == pin) {
                            strcpy(i->item.pszText, _("<PWM needs!>"));
                            break;
                        }
                    }

                    int j;
                    for(j = 0; j < Prog.mcu->pinCount; j++) 
					{
                        if(Prog.mcu->pinInfo[j].pin == pin) 
						{
							if (Prog.io.assignment[item].type == IO_TYPE_DIG_OUTPUT && pin == 17)
								sprintf(i->item.pszText, "LED ERRO");
							else if (Prog.io.assignment[item].type == IO_TYPE_DIG_OUTPUT && pin == 18)
								sprintf(i->item.pszText, "LED CPU");
							else if (Prog.io.assignment[item].type == IO_TYPE_DIG_OUTPUT && pin > 19)
								sprintf(i->item.pszText, "M%d.%d", Prog.mcu->pinInfo[j].bit, Prog.io.assignment[item].bit);
							else if (Prog.io.assignment[item].type == IO_TYPE_DIG_OUTPUT)
								sprintf(i->item.pszText, "S%d", Prog.mcu->pinInfo[j].bit);
							else if (Prog.io.assignment[item].type == IO_TYPE_DIG_INPUT && pin > 19)
								sprintf(i->item.pszText, "M%d.%d", Prog.mcu->pinInfo[j].bit, Prog.io.assignment[item].bit);
							else if (Prog.io.assignment[item].type == IO_TYPE_DIG_INPUT)
								sprintf(i->item.pszText, "E%d", Prog.mcu->pinInfo[j].bit);
							else if (Prog.io.assignment[item].type == IO_TYPE_READ_ADC)
								if (pin == 6)
									sprintf(i->item.pszText, "TEMP");
								else
									sprintf(i->item.pszText, "AD%d", Prog.mcu->pinInfo[j].bit);
							else if (Prog.io.assignment[item].type == IO_TYPE_READ_ENC || Prog.io.assignment[item].type == IO_TYPE_RESET_ENC)
								sprintf(i->item.pszText, "ENC%d", Prog.mcu->pinInfo[j].bit);
                            break;
                        }
                    }
                    if(j == Prog.mcu->pinCount) {
                        sprintf(i->item.pszText, _("<not an I/O!>"));
                    }
                    break;
                }

                case LV_IO_STATE: {
                    if(InSimulationMode) {
                        char *name = Prog.io.assignment[item].name;
                        DescribeForIoList(name, i->item.pszText);
                    } else {
                        strcpy(i->item.pszText, "");
                    }
                    break;
                }

            }
            break;
        }
        case LVN_ITEMACTIVATE: {
			NMITEMACTIVATE *i = (NMITEMACTIVATE *)h;
            if(InSimulationMode) {
                char *name = Prog.io.assignment[i->iItem].name;
                if(name[0] == 'X') {
                    SimulationToggleContact(name);
                } else if(name[0] == 'A') {
                    ShowAnalogSliderPopup(name);
                } else if(name[0] == 'E') {
                    ShowEncoderSliderPopup(name);
                }
            } else {
                UndoRemember();
                ShowIoMapDialog(i->iItem);
                ProgramChanged();
                InvalidateRect(MainWindow, NULL, FALSE);
            }
            break;
        }
    }
}
