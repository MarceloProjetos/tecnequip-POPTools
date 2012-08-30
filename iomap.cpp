#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldmicro.h"

// I/O that we have seen recently, so that we don't forget pin assignments
// when we re-extract the list
#define MAX_IO_SEEN_PREVIOUSLY 1024
struct {
    char    name[MAX_NAME_LEN];
    int     type;
    int     pin;
	int		bit;
} IoSeenPreviously[MAX_IO_SEEN_PREVIOUSLY];
int IoSeenPreviouslyCount;

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

// stuff for the popup that lets you set the simulated value of an analog in
static HWND EncoderSliderMain;
static HWND EncoderSliderTrackbar;
static BOOL EncoderSliderDone;
static BOOL EncoderSliderCancel;

static int nSortColumn = 0;
static BOOL bSortAscending = TRUE;

// stuff for the popup that lets you set the simulated value of an encode in
/*static HWND EncoderSliderMain;
static HWND EncoderSliderTrackbar;
static BOOL EncoderSliderDone;
static BOOL EncoderSliderCancel;*/

const LPCTSTR ComboboxBitItens[] = { "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", "10",
									"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
									"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
									"31"/*, "32"*/};

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
    if(_stricmp(name, _("new"))==0) return;

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
void ExtractNamesFromCircuit(int which, void *any)
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
            switch(l->d.contacts.type) {
                case IO_TYPE_INTERNAL_RELAY:
                    AppendIo(l->d.contacts.name, IO_TYPE_INTERNAL_RELAY, l->d.contacts.bit);
                    break;

                case IO_TYPE_DIG_INPUT:
                    AppendIo(l->d.contacts.name, IO_TYPE_DIG_INPUT, l->d.contacts.bit);
                    break;

                case IO_TYPE_DIG_OUTPUT:
                    AppendIo(l->d.contacts.name, IO_TYPE_DIG_OUTPUT, l->d.contacts.bit);
                    break;

				case IO_TYPE_INTERNAL_FLAG:
                    AppendIo(l->d.contacts.name, IO_TYPE_INTERNAL_FLAG, l->d.contacts.bit);
                    break;

                default:
                    oops();
                    break;
            }
            break;

        case ELEM_COIL:
			if(l->d.coil.type == IO_TYPE_INTERNAL_RELAY || l->d.coil.type == IO_TYPE_DIG_OUTPUT) {
	            AppendIo(l->d.coil.name, l->d.coil.type, l->d.coil.bit);
			} else {
                oops();
			}
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
			if(!IsNumber(l->d.move.src))
				AppendIo(l->d.move.src, IO_TYPE_GENERAL, 0);
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
			if(!IsNumber(l->d.math.op1))
	            AppendIo(l->d.math.op1, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.math.op2))
	            AppendIo(l->d.math.op2, IO_TYPE_GENERAL, 0);
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
            if(strlen(l->d.servoYaskawa.var) > 0 && !IsNumber(l->d.servoYaskawa.var)) {
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
			if(!IsNumber(l->d.setDA.name))
	            AppendIo(l->d.setDA.name, IO_TYPE_SET_DA, 0);
            break;

        case ELEM_READ_ENC:
            AppendIo(l->d.readEnc.name, IO_TYPE_READ_ENC, 0);
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
            AppendIo(l->d.lookUpTable.dest , IO_TYPE_GENERAL, 0);
			AppendIo(l->d.lookUpTable.index, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_PIECEWISE_LINEAR:
            AppendIo(l->d.piecewiseLinear.dest , IO_TYPE_GENERAL, 0);
			AppendIo(l->d.piecewiseLinear.index, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_EQU:
        case ELEM_NEQ:
        case ELEM_GRT:
        case ELEM_GEQ:
        case ELEM_LES:
        case ELEM_LEQ:
			if(!IsNumber(l->d.cmp.op1))
	            AppendIo(l->d.cmp.op1, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.cmp.op2))
	            AppendIo(l->d.cmp.op2, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_PERSIST:
			AppendIo(l->d.persist.var, IO_TYPE_GENERAL, 0);

		case ELEM_RTC:
        case ELEM_RESET_ENC:
        case ELEM_PLACEHOLDER:
        case ELEM_COMMENT:
        case ELEM_SHORT:
        case ELEM_OPEN:
        case ELEM_MASTER_RELAY:
        case ELEM_ONE_SHOT_RISING:
        case ELEM_ONE_SHOT_FALLING:
        case ELEM_RES:
		case ELEM_MULTISET_DA:
            break;

        default:
            oops();
    }
}

void UpdateTypeInRung(char *name, unsigned int type, int which, void *any)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                UpdateTypeInRung(name, type, p->contents[i].which, p->contents[i].d.any);
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                UpdateTypeInRung(name, type, s->contents[i].which, s->contents[i].d.any);
            }
            break;
        }

		case ELEM_CONTACTS:
			if(!_stricmp(name, l->d.contacts.name))
				l->d.contacts.type = type;
            break;

        case ELEM_COIL:
			if(!_stricmp(name, l->d.coil.name))
				l->d.coil.type = type;
            break;
	}
}

//-----------------------------------------------------------------------------
// Walk a subcircuit, calling ourselves recursively and extracting all the
// I/O names out of it.
//-----------------------------------------------------------------------------
void UpdateTypeInCircuit(char *name, unsigned int type)
{
	int i;
    for(i = 0; i < Prog.numRungs; i++) {
		UpdateTypeInRung(name, type, ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    }
}

void UpdateTypeForInternalRelaysInRung(int which, void *any)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                UpdateTypeForInternalRelaysInRung(p->contents[i].which, p->contents[i].d.any);
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                UpdateTypeForInternalRelaysInRung(s->contents[i].which, s->contents[i].d.any);
            }
            break;
        }

		case ELEM_CONTACTS:
			if(l->d.contacts.type == IO_TYPE_PENDING)
				l->d.contacts.type = IO_TYPE_INTERNAL_RELAY;
            break;

        case ELEM_COIL:
			if(l->d.coil.type == IO_TYPE_PENDING)
				l->d.coil.type = IO_TYPE_INTERNAL_RELAY;
            break;
	}
}

//-----------------------------------------------------------------------------
// Walk a subcircuit, calling ourselves recursively and extracting all the
// I/O names out of it.
//-----------------------------------------------------------------------------
void UpdateTypeForInternalRelays()
{
	int i;
    for(i = 0; i < Prog.numRungs; i++) {
		UpdateTypeForInternalRelaysInRung(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    }
}

void UpdateTypesFromSeenPreviouslyList()
{
	int i;

	// Search for Coils and Contacts in the loaded I/O List to update their types in Program Rungs.
	for(i=0; i<IoSeenPreviouslyCount; i++) {
		if(IoSeenPreviously[i].type == IO_TYPE_INTERNAL_RELAY ||
		   IoSeenPreviously[i].type == IO_TYPE_DIG_INPUT ||
		   IoSeenPreviously[i].type == IO_TYPE_DIG_OUTPUT ||
		   IoSeenPreviously[i].type == IO_TYPE_INTERNAL_FLAG)
				UpdateTypeInCircuit(IoSeenPreviously[i].name, IoSeenPreviously[i].type);
	}
}

BOOL ExistsCoilWithNameInRung(char *name, int which, void *any)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                if(ExistsCoilWithNameInRung(name, p->contents[i].which, p->contents[i].d.any))
					return TRUE;
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                if(ExistsCoilWithNameInRung(name, s->contents[i].which, s->contents[i].d.any))
					return TRUE;
            }
            break;
        }

        case ELEM_COIL:
			if(!_stricmp(name, l->d.coil.name))
				return TRUE;
            break;
	}

	return FALSE;
}

BOOL ExistsCoilWithName(char *name)
{
	int i;
    for(i = 0; i < Prog.numRungs; i++) {
		if(ExistsCoilWithNameInRung(name, ELEM_SERIES_SUBCKT, Prog.rungs[i]))
			return TRUE;
    }
	return FALSE;
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

void SortList(int NewSortColumn)
{
	// get new sort parameters. If NewSortColumn < 0, use previous settings
	if (NewSortColumn == nSortColumn) {
		bSortAscending = !bSortAscending;
	} else if(NewSortColumn >= 0) {
		nSortColumn = NewSortColumn;
		bSortAscending = TRUE;
	}

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
           Prog.io.assignment[i].type == IO_TYPE_GENERAL ||
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

	SortList(-1);

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
// Load the I/O list from a file. Since we are just loading pin assignments,
// put it into IoSeenPreviously so that it will get used on the next
// extraction.
//-----------------------------------------------------------------------------
BOOL LoadIoListFromFile(FILE *f, int version)
{
    char line[80];
    char name[MAX_NAME_LEN];
    int pin, bit = 0;
	int i;
    unsigned int type;

	for (i = 0; i < MAX_IO_SEEN_PREVIOUSLY; i++)
	{
		memset(IoSeenPreviously[i].name, 0, sizeof(IoSeenPreviously[i].name));
		IoSeenPreviously[i].type = 0;
		IoSeenPreviously[i].pin = 0;
		IoSeenPreviously[i].bit = 0;
	}
	IoSeenPreviouslyCount = 0;

    while(fgets(line, sizeof(line), f)) {
        if(_stricmp(line, "END\n")==0) {
            return TRUE;
        }
        // Don't internationalize this! It's the file format, not UI.

		if (version < 2)
		{
			if (sscanf(line, "    %s at %d", name, &pin) != 2) 
			{
				return FALSE;
			}
		}
		else if (version < 3)
		{
			if(sscanf(line, "    %s at %d %d", name, &pin, &bit)!=3) 
			{
				return FALSE;
			}

			switch(name[0]) {
				case 'X': type = IO_TYPE_DIG_INPUT; break;
		        case 'Y': type = IO_TYPE_DIG_OUTPUT; break;
	            case 'A': type = IO_TYPE_READ_ADC; break;
				case 'E': type = IO_TYPE_READ_ENC; break;
			    case 'Z': type = IO_TYPE_RESET_ENC; break;
		        default: oops();
	        }
		}
		else
		{
			if(sscanf(line, "    %s at %d %d type %u", name, &pin, &bit, &type)!=4) 
			{
				return FALSE;
			}
		}

		if(type == IO_TYPE_DIG_OUTPUT && (pin == 17 || pin == 18)) { // Leds - change to internal relay type.
			pin  = 0;
			type = IO_TYPE_INTERNAL_RELAY;
		}

		AppendIoSeenPreviously(name, type, pin, bit);
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
// Write the I/O list to a file. Since everything except the pin assignment
// can be extracted from the schematic, just write the Xs and Ys.
//-----------------------------------------------------------------------------
void SaveIoListToFile(FILE *f)
{
    int i;
    for(i = 0; i < Prog.io.count; i++) {
        if(Prog.io.assignment[i].type == IO_TYPE_DIG_INPUT  ||
           Prog.io.assignment[i].type == IO_TYPE_DIG_OUTPUT ||
           Prog.io.assignment[i].type == IO_TYPE_GENERAL ||
		   Prog.io.assignment[i].type == IO_TYPE_INTERNAL_FLAG ||
           Prog.io.assignment[i].type == IO_TYPE_READ_ADC ||
           Prog.io.assignment[i].type == IO_TYPE_READ_ENC ||
           Prog.io.assignment[i].type == IO_TYPE_RESET_ENC)
        {
            // Don't internationalize this! It's the file format, not UI.
			if(Prog.io.assignment[i].type != IO_TYPE_GENERAL || Prog.io.assignment[i].pin)
	            fprintf(f, "    %s at %d %d type %u\n", Prog.io.assignment[i].name,
		            Prog.io.assignment[i].pin, Prog.io.assignment[i].bit, Prog.io.assignment[i].type);
        }
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

    if(Prog.io.assignment[item].type != IO_TYPE_DIG_INPUT && 
       Prog.io.assignment[item].type != IO_TYPE_DIG_OUTPUT &&
       Prog.io.assignment[item].type != IO_TYPE_GENERAL  &&
       Prog.io.assignment[item].type != IO_TYPE_READ_ADC &&
       Prog.io.assignment[item].type != IO_TYPE_READ_ENC &&
       Prog.io.assignment[item].type != IO_TYPE_RESET_ENC)
    {
        Error(_("Can only assign pin number to input/output pins (Xname or "
            "Yname or Aname)."));
        return;
    }

    if(Prog.io.assignment[item].type == IO_TYPE_READ_ADC && Prog.mcu->adcCount == 0) {
        Error(_("No ADC or ADC not supported for this micro."));
        return;
    }

	if(Prog.io.assignment[item].type == IO_TYPE_READ_ENC && Prog.mcu->encCount == 0) {
        Error(_("No Encoder or Encoder not supported for selected micro."));
        return;
    }

    if(_stricmp(Prog.io.assignment[item].name, _("new"))==0) {
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
				!((Prog.io.assignment[j].type == IO_TYPE_DIG_INPUT && i > 18) || (Prog.io.assignment[j].type == IO_TYPE_DIG_OUTPUT && i > 66)) )
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

        if(Prog.io.assignment[item].type == IO_TYPE_READ_ADC) {
            for(j = 0; j < Prog.mcu->adcCount; j++) 
			{
				if (j == 4) continue;
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

		if(Prog.io.assignment[item].type == IO_TYPE_READ_ENC || Prog.io.assignment[item].type == IO_TYPE_RESET_ENC) {
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

		if ((Prog.io.assignment[item].type == IO_TYPE_DIG_INPUT  && i < 51) ||
			(Prog.io.assignment[item].type == IO_TYPE_DIG_OUTPUT && i > 50) ||
			(Prog.io.assignment[item].type == IO_TYPE_GENERAL    && i > 66))
		{
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
	char val[20];
	int c = LVN_COLUMNCLICK;
    switch(h->code) {
		case LVN_COLUMNCLICK:
			{
				LPNMLISTVIEW pLVInfo = (LPNMLISTVIEW)h;
				SortList(pLVInfo->iSubItem);
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
                    if(type != IO_TYPE_DIG_INPUT && type != IO_TYPE_DIG_OUTPUT && type != IO_TYPE_GENERAL
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
							else if (Prog.io.assignment[item].type == IO_TYPE_GENERAL)
								sprintf(i->item.pszText, "M%d", Prog.mcu->pinInfo[j].bit);
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
                        DescribeForIoList(name, Prog.io.assignment[item].type, i->item.pszText);
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
                if(Prog.io.assignment[i->iItem].type == IO_TYPE_DIG_INPUT) {
                    SimulationToggleContact(name);
                } else if(Prog.io.assignment[i->iItem].type == IO_TYPE_READ_ADC) {
                    ShowAnalogSliderPopup(name);
                } else if(Prog.io.assignment[i->iItem].type == IO_TYPE_READ_ENC) {
                    ShowEncoderSliderPopup(name);
				} else if(Prog.io.assignment[i->iItem].type == IO_TYPE_COUNTER ||
						Prog.io.assignment[i->iItem].type == IO_TYPE_TOF ||
						Prog.io.assignment[i->iItem].type == IO_TYPE_TON ||
						Prog.io.assignment[i->iItem].type == IO_TYPE_GENERAL) {
					sprintf(val, "%d", GetSimulationVariable(name));
                    ShowSimulationVarSetDialog(name, val);
					SetSimulationVariable(name, atoi(val));
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
// A little toolbar-style window that pops up to allow the user to set the
// simulated value of an ADC pin.
//-----------------------------------------------------------------------------
void ShowAnalogSliderPopup(char *name)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)AnalogSliderDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "LDmicroAnalogSlider";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    POINT pt;
    GetCursorPos(&pt);

    SWORD currentVal = GetAdcShadow(name);

    SWORD maxVal;
    if(Prog.mcu) {
        maxVal = Prog.mcu->adcMax;
    } else {
        maxVal = 1023;
    }
    if(maxVal == 0) {
        Error(_("No ADC or ADC not supported for selected micro."));
        return;
    }

    int left = pt.x - 10;
    // try to put the slider directly under the cursor (though later we might
    // realize that that would put the popup off the screen)
    int top = pt.y - (15 + (73*currentVal)/maxVal);

    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    if(top + 110 >= r.bottom) {
        top = r.bottom - 110;
    }
    if(top < 0) top = 0;
    
    AnalogSliderMain = CreateWindowClient(0, "LDmicroAnalogSlider", _("I/O Pin"),
        WS_VISIBLE | WS_POPUP | WS_DLGFRAME,
        left, top, 30, 100, MainWindow, NULL, Instance, NULL);

    AnalogSliderTrackbar = CreateWindowEx(0, TRACKBAR_CLASS, "", WS_CHILD |
        TBS_AUTOTICKS | TBS_VERT | TBS_TOOLTIPS | WS_CLIPSIBLINGS | WS_VISIBLE, 
        0, 0, 30, 100, AnalogSliderMain, NULL, Instance, NULL);
    SendMessage(AnalogSliderTrackbar, TBM_SETRANGE, FALSE,
        MAKELONG(0, maxVal));
    SendMessage(AnalogSliderTrackbar, TBM_SETTICFREQ, (maxVal + 1)/8, 0);
    SendMessage(AnalogSliderTrackbar, TBM_SETPOS, TRUE, currentVal);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(AnalogSliderMain, TRUE);
    SetFocus(AnalogSliderTrackbar);

    DWORD ret;
    MSG msg;
    AnalogSliderDone = FALSE;
    AnalogSliderCancel = FALSE;

    SWORD orig = GetAdcShadow(name);

    while(!AnalogSliderDone && (ret = GetMessage(&msg, NULL, 0, 0))) {
        SWORD v = (SWORD)SendMessage(AnalogSliderTrackbar, TBM_GETPOS, 0, 0);

        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                AnalogSliderDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                AnalogSliderDone = TRUE;
                AnalogSliderCancel = TRUE;
                break;
            }
        } else if(msg.message == WM_LBUTTONUP) {
            if(v != orig) {
                AnalogSliderDone = TRUE;
            }
        }
        SetAdcShadow(name, v);

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!AnalogSliderCancel) {
        SWORD v = (SWORD)SendMessage(AnalogSliderTrackbar, TBM_GETPOS, 0, 0);
        SetAdcShadow(name, v);
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(AnalogSliderMain);
    ListView_RedrawItems(IoList, 0, Prog.io.count - 1);
}

//-----------------------------------------------------------------------------
// Dialog proc for the popup that lets you set the value of an analog input for
// simulation.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK EncoderSliderDialogProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            EncoderSliderDone = TRUE;
            EncoderSliderCancel = TRUE;
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

//-----------------------------------------------------------------------------
// A little toolbar-style window that pops up to allow the user to set the
// simulated value of an ADC pin.
//-----------------------------------------------------------------------------
void ShowEncoderSliderPopup(char *name)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)EncoderSliderDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "LDmicroEncoderSlider";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    POINT pt;
    GetCursorPos(&pt);

    SWORD currentVal = GetEncShadow(name);

    SWORD maxVal;
    if(Prog.mcu) {
        maxVal = Prog.mcu->encMax;
    } else {
        maxVal = 0x7FFFFFFF;
    }
    if(maxVal == 0) {
        Error(_("No Encoder or Encoder not supported for selected micro."));
        return;
    }

    int left = pt.x - 10;
    // try to put the slider directly under the cursor (though later we might
    // realize that that would put the popup off the screen)
    int top = pt.y - 55; //- (15 + (73*currentVal)/0x7fff);

    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    if(top + 110 >= r.bottom) {
        top = r.bottom - 110;
    }
    if(top < 0) top = 0;

    EncoderSliderMain = CreateWindowClient(0, "LDmicroEncoderSlider", _("I/O Pin"),
        WS_VISIBLE | WS_POPUP | WS_DLGFRAME,
        left, top, 30, 100, MainWindow, NULL, Instance, NULL);

    EncoderSliderTrackbar = CreateWindowEx(0, TRACKBAR_CLASS, "", WS_CHILD |
        TBS_AUTOTICKS | TBS_VERT | TBS_TOOLTIPS | WS_CLIPSIBLINGS | WS_VISIBLE, 
        0, 0, 30, 100, EncoderSliderMain, NULL, Instance, NULL);

    SendMessage(EncoderSliderTrackbar, TBM_SETRANGEMIN, FALSE,
        /*(maxVal + 1) * -1*/ 0);  // bug com valor negativo
    SendMessage(EncoderSliderTrackbar, TBM_SETRANGEMAX, FALSE,
        maxVal - 1);
    SendMessage(EncoderSliderTrackbar, TBM_SETTICFREQ, (maxVal)/8, 0);
    SendMessage(EncoderSliderTrackbar, TBM_SETPOS, TRUE, currentVal);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(EncoderSliderMain, TRUE);
    SetFocus(EncoderSliderTrackbar);

    DWORD ret;
    MSG msg;
    EncoderSliderDone = FALSE;
    EncoderSliderCancel = FALSE;

    SWORD orig = GetEncShadow(name);

    while(!EncoderSliderDone && (ret = GetMessage(&msg, NULL, 0, 0))) {
        SWORD v = (SWORD)SendMessage(EncoderSliderTrackbar, TBM_GETPOS, 0, 0);

        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                EncoderSliderDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                EncoderSliderDone = TRUE;
                EncoderSliderCancel = TRUE;
                break;
            }
        } else if(msg.message == WM_LBUTTONUP) {
            if(v != orig) {
                EncoderSliderDone = TRUE;
            }
        }
        SetEncShadow(name, v);

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!EncoderSliderCancel) {
        SWORD v = (SWORD)SendMessage(EncoderSliderTrackbar, TBM_GETPOS, 0, 0);
        SetEncShadow(name, v);
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(EncoderSliderMain);
    ListView_RedrawItems(IoList, 0, Prog.io.count - 1);
}
