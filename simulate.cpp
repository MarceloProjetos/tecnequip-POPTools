#include "poptools.h"

static struct {
    char name[MAX_NAME_LEN];
    BOOL powered;
} SingleBitItems[MAX_IO];
static int SingleBitItemsCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
    DWORD   usedFlags;
} Variables[MAX_IO];
static int VariablesCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} AdcShadows[MAX_IO];
static int AdcShadowsCount;

SWORD DAShadow;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} EncShadows[MAX_IO];
static int EncShadowsCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} ResetEncShadows[MAX_IO];
static int ResetEncShadowsCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} readUSSShadows[MAX_IO];
static int readUSSShadowsCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} writeUSSShadows[MAX_IO];
static int writeUSSShadowsCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} readModbusShadows[MAX_IO];
static int readModbusShadowsCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} writeModbusShadows[MAX_IO];
static int writeModbusShadowsCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} readModbusEthShadows[MAX_IO];
static int readModbusEthShadowsCount;

static struct {
    char    name[MAX_NAME_LEN];
    SWORD   val;
} writeModbusEthShadows[MAX_IO];
static int writeModbusEthShadowsCount;

#define VAR_FLAG_TON  0x00000001
#define VAR_FLAG_TOF  0x00000002
#define VAR_FLAG_RTO  0x00000004
#define VAR_FLAG_CTU  0x00000008
#define VAR_FLAG_CTD  0x00000010
#define VAR_FLAG_CTC  0x00000020
#define VAR_FLAG_RES  0x00000040
#define VAR_FLAG_ANY  0x00000080

#define VAR_FLAG_OTHERWISE_FORGOTTEN  0x80000000


// Schematic-drawing code needs to know whether we're in simulation mode or
// note, as that changes how everything is drawn; also UI code, to disable
// editing during simulation.
BOOL InSimulationMode;

// Don't want to redraw the screen unless necessary; track whether a coil
// changed state or a timer output switched to see if anything could have
// changed (not just coil, as we show the intermediate steps too).
static BOOL NeedRedraw;
// Have to let the effects of a coil change in cycle k appear in cycle k+1,
// or set by the UI code to indicate that user manually changed an Xfoo
// input.
BOOL SimulateRedrawAfterNextCycle;

// Don't want to set a timer every 100 us to simulate a 100 us cycle
// time...but we can cycle multiple times per timer interrupt and it will
// be almost as good, as long as everything runs fast.
static int CyclesPerTimerTick;

// Program counter as we evaluate the intermediate code.
static int IntPc;

// A window to allow simulation with the UART stuff (insert keystrokes into
// the program, view the output, like a terminal window).
HWND UartSimulationWindow = 0;
HWND UartSimulationTextControl = 0;

static LONG_PTR PrevTextProc;

static int QueuedUartCharacter = -1;
static int QueuedUSSCharacter = -1;

static int SimulateUartTxCountdown = 0;

static int SimulateUSSTxCountdown = 0;
static int SimulateUSSRxCountdown = 0;
static int SimulateModbus485Countdown = 0;
static int SimulateModbusEthCountdown = 0;

unsigned int Simulate_DaConv(int val, unsigned int mode)
{
	int ret;
	switch(mode) {
	case 0:
		ret = val + DA_RESOLUTION;
		break;

	case 1:
		ret = ((val * DA_RESOLUTION) / 10000) + DA_RESOLUTION;
		break;

	case 2:
		ret = ((val * DA_RESOLUTION) / 100) + DA_RESOLUTION;
		break;

	default:
		ret = DA_RESOLUTION;
	}

	if(ret < 0)
		ret = 0;
	else if (ret >= 2*DA_RESOLUTION)
		ret = 2*DA_RESOLUTION - 1;

	return (unsigned int)ret;
}

static struct {
	int Count;
	int StartCount;
	int StartValue;
	int FinalValue;
} SimulateMultisetDA = { 0, DA_RESOLUTION, DA_RESOLUTION };

static void AppendToUartSimulationTextControl(BYTE b);
static void AppendToFormattedStringSimulationTextControl(char * text, char *string, SWORD val);
static void AppendToUSSSimulationTextControl(unsigned char id, unsigned int param, unsigned int index, char *name, unsigned int val);
static void AppendToModbusSimulationTextControl(unsigned char id, unsigned int address, char *name);
static void AppendToModbusEthSimulationTextControl(unsigned char id, unsigned int address, char *name);

static void SimulateIntCode(void);
static char *MarkUsedVariable(char *name, DWORD flag);

struct WatchPoint *listWP = NULL;

void AddWP(char *name, int val)
{
	struct WatchPoint *previous = NULL, *current = listWP, *newWP;

	while(current != NULL) {
		if(!_stricmp(name, current->name))
			break;

		previous = current;
		current = current->next;
	}

	if(current != NULL) { // name already in the list, change it!
		current->val = val;
	} else {
		newWP = (struct WatchPoint *)CheckMalloc(sizeof(struct WatchPoint));
		newWP->name = name;
		newWP->val = val;
		if(previous != NULL) {
			previous->next = newWP;
		} else { // list empty!
			listWP = newWP;
		}
	}
}

void RemoveWP(char *name)
{
	struct WatchPoint *previous = NULL, *current = listWP;

	while(current != NULL) {
		if(!_stricmp(name, current->name)) {
			if(previous != NULL) {
				previous->next = current->next;
			} else {
				listWP = current->next;
			}
			CheckFree(current);
			break;
		}

		previous = current;
		current = current->next;
	}
}

int * GetValWP(char *name, int *val)
{
	struct WatchPoint *current = listWP;

	while(current != NULL && val != NULL) {
		if(!_stricmp(name, current->name)) {
			*val = current->val;
			return val;
		}

		current = current->next;
	}

	return NULL;
}

void ClearListWP(void)
{
	while(listWP != NULL) {
		RemoveWP(listWP->name);
	}
}

void CheckWP(char *name, int val)
{
	int wpval;
	char desc[100];
	bool NeedStop = false;

	if(name == NULL) {
		struct WatchPoint *current = listWP;
		while(current != NULL) {
			name = current->name;
			val  = GetSimulationVariable(name);
			if(GetValWP(name, &wpval) != NULL && wpval == val) {
				NeedStop = true;
				break;
			}
			current = current->next;
		}
	} else if(GetValWP(name, &wpval) != NULL && wpval == val) {
		NeedStop = true;
	}

	if(NeedStop) {
		PauseSimulation();
		DescribeForIoList(wpval, GetTypeFromName(name), desc);
		Error(_("Simulação interrompida!\n\n%s = %s"), name, desc);
	}
}

/*** Hardware Registers Simulation ***/

//-----------------------------------------------------------------------------
// The following code will simulate the registers of POP-7 so we will be able
// to emulate a POP-7 hardware in POPTools. When simulation is running, a
// socket is opened waiting for ModBUS messages.
//-----------------------------------------------------------------------------

HardwareRegisters hwreg;

void HardwareRegisters_Init(HardwareRegisters *hwr)
{
	if(hwr != NULL) {
		memset(hwr, 0, sizeof(HardwareRegisters));
	}
}

void HardwareRegisters_SetBit(HardwareRegisters *hwr, int reg, int index, int bit, int val)
{
	unsigned int mask;

	if(hwr == NULL || bit<0 || bit>31 || index<0) return;

	mask = 1UL << bit;

	switch(reg) {
	case HWR_REG_INPUT:
		if(!val) {
			hwr->DigitalInput &= ~mask;
		} else {
			hwr->DigitalInput |=  mask;
		}
		break;

	case HWR_REG_OUTPUT:
		if(!val) {
			hwr->DigitalOutput &= ~mask;
		} else {
			hwr->DigitalOutput |=  mask;
		}
		break;

	case HWR_REG_MODBUS:
		if(index < HWR_MODBUS_SIZE) {
			if(!val) {
				hwr->ModBUS[index] &= ~mask;
			} else {
				hwr->ModBUS[index] |=  mask;
			}
		}
	}
}

void HardwareRegisters_SetValue(HardwareRegisters *hwr, int index, int val)
{
	if(hwr != NULL && index >= 0 && index < HWR_MODBUS_SIZE) {
		hwr->ModBUS[index] = val;
	}
}

void HardwareRegisters_Update(HardwareRegisters *hwr, char *name, int val)
{
	int i, index, ismb;
	if(name == NULL) return;

	for(i=0; i<Prog.io.count; i++) {
		if(Prog.io.assignment[i].pin != NO_PIN_ASSIGNED &&
				!strcmp(name, Prog.io.assignment[i].name)) { // name has assignment
			index = IoMap_GetIndex(&Prog.io.assignment[i]);
			ismb  = IoMap_IsModBUS(&Prog.io.assignment[i]);

			// Now we have to update register associated to name
			switch(Prog.io.assignment[i].type) {
			case IO_TYPE_DIG_INPUT:
				HardwareRegisters_SetBit(hwr, ismb ? HWR_REG_MODBUS : HWR_REG_INPUT,
					index, ismb ? Prog.io.assignment[i].bit : index, val);
				break;

			case IO_TYPE_DIG_OUTPUT:
				HardwareRegisters_SetBit(hwr, ismb ? HWR_REG_MODBUS : HWR_REG_OUTPUT,
					index, ismb ? Prog.io.assignment[i].bit : index, val);
				break;

			case IO_TYPE_GENERAL:
				HardwareRegisters_SetValue(hwr, index, val);
				break;
			}
			// Register updated, exit search loop.
			break;
		}
	}
}

static void SetSingleBit(char *name, BOOL state);

void HardwareRegisters_Sync(HardwareRegisters *hwr)
{
	int i, index, ismb;
	unsigned int mask;
	if(hwr == NULL) return;

	hwr->Syncing = true;

	for(i=0; i<Prog.io.count; i++) {
		if(Prog.io.assignment[i].pin != NO_PIN_ASSIGNED) {
			index = IoMap_GetIndex(&Prog.io.assignment[i]);
			ismb  = IoMap_IsModBUS(&Prog.io.assignment[i]);
				mask = 1UL << Prog.io.assignment[i].bit;

			// Now we have to update register associated to name
			switch(Prog.io.assignment[i].type) {
			case IO_TYPE_DIG_INPUT:
//				HardwareRegisters_SetBit(hwr, ismb ? HWR_REG_MODBUS : HWR_REG_INPUT,
//					index, ismb ? Prog.io.assignment[i].bit : index, val);
				break;

			case IO_TYPE_DIG_OUTPUT:
				if(ismb)
					SetSingleBit(Prog.io.assignment[i].name, (hwreg.ModBUS[index] & mask) ? TRUE : FALSE);
				break;

			case IO_TYPE_GENERAL:
				SetSimulationVariable(Prog.io.assignment[i].name, hwreg.ModBUS[index]);
				break;
			}
		}
	}

	hwr->Syncing    = false;
	hwr->NeedUpdate = true;
}

// Update Hardware Registers, check WatchPoints, etc.
void UpdateSimulation(char *name, int val)
{
	if(!hwreg.Syncing) {
		HardwareRegisters_Update(&hwreg, name, val);
		CheckWP(name, val);
	}
}

/*** Functions related to RTC Simulation ***/

#define RTC_MODE_DATE_CONTINUOUS      0
#define RTC_MODE_DATE_INTERMITTENT    1
#define RTC_MODE_WEEKDAY_CONTINUOUS   2
#define RTC_MODE_WEEKDAY_INTERMITTENT 3

#define RTC_GETDATE_MODE_START 0
#define RTC_GETDATE_MODE_END   1

struct tm RTC_StartTM, RTC_EndTM;

// Adjust TM struct
struct tm * AdjustDate(struct tm timeinfo, int mode)
{
	int last_day;
	time_t rawtime = 0;
	struct tm tm_tmp, *ptr;

	// recebe referencia a estrutura tm do sistema
	ptr = localtime(&rawtime);

	// Ajuste de mes e ano para o caso de ter sido escolhido "Todos". No caso, digitado zero.
	if(!timeinfo.tm_mon ) timeinfo.tm_mon  = (mode == RTC_GETDATE_MODE_START ?    1 :   12);
	if(!timeinfo.tm_year) timeinfo.tm_year = (mode == RTC_GETDATE_MODE_START ? 1970 : 2037);

	// Atualizacao da estrutura tm com os valores esperados
	timeinfo.tm_year -= 1900;
	timeinfo.tm_mon  -= 1;

	// Ajusta de dia para o caso de ter sido escolhido "Todos". No caso, digitado zero.
	if(!timeinfo.tm_mday) {
		if(timeinfo.tm_mon == 11) { // Dezembro
			last_day = 31;
		} else {
			tm_tmp = timeinfo;
			tm_tmp.tm_mon++;
			tm_tmp.tm_mday = 1;
			tm_tmp.tm_hour = 0;
			rawtime = mktime(&tm_tmp) - 3600; // Hora definida - 1 hora, volta para o mes anterior
			tm_tmp = *localtime(&rawtime);
			last_day = tm_tmp.tm_mday;
		}

		timeinfo.tm_mday = (mode == RTC_GETDATE_MODE_START ? 1 : last_day);
	}

	// Retorna estrutura com a data
	*ptr = timeinfo;
	return ptr;
}

// Tempo em segundos no dia
static time_t GetHourOffset(struct tm timeinfo)
{
	time_t now = mktime(&timeinfo);

	timeinfo.tm_hour = timeinfo.tm_min = timeinfo.tm_sec = 0;

	return now - mktime(&timeinfo);
}

// Calculate Output State according date/time and mode.
int RTC_OutputState(struct tm start, struct tm end, struct tm now, int mode, int mask_wday)
{
	int previous_wday_on;
	time_t tstart, tend, tnow, offset_start, offset_end, offset_now;
	int ret=0, seg = -1, seg_previous = -1, i, first_wday[4], last_wday[4], segs_wday = 0, first_found;

	offset_start = GetHourOffset(start);
	offset_end   = GetHourOffset(end  );
	offset_now   = GetHourOffset(now  );

	tstart = mktime(&start);
	tend   = mktime(&end);
	tnow   = mktime(&now);

	if(mode == RTC_MODE_DATE_CONTINUOUS) {
		if(tnow >= tstart && tnow < tend) {
			ret = 1;
		}
	} else if(mode == RTC_MODE_DATE_INTERMITTENT) {
		if(tnow >= tstart && tnow <= tend &&
			(offset_start > offset_end ? (offset_now >= offset_start || offset_now < offset_end) :
			(offset_now >= offset_start && offset_now < offset_end))) {
			ret = 1;
		}
	} else {
		first_found = 0;
		for(i=0; i<7; i++) { // Loop entre todos os dias da semana
			if(!first_found && ((1 << i) & mask_wday)) {
				first_found = 1;
				first_wday[segs_wday] = i;
			}
			if(first_found && !((1 << (i+1)) & mask_wday)) {
				first_found = 0;
				last_wday[segs_wday++] = i;
			}
		}

		// Se o ultimo segmento termina no sabado e o primeiro comeca no domingo, eles fazem parte do mesmo segmento
		// Isso apenas se houver mais de 1 segmento senao indica que a semana inteira foi selecionada
		if(segs_wday > 1 && last_wday[segs_wday-1] == 6 && first_wday[0] == 0) {
			segs_wday--;
			first_wday[0] = first_wday[segs_wday];
		}

		for(i=0; i < segs_wday; i++) {
			if(first_wday[i] > last_wday[i]) {
				if(now.tm_wday >= first_wday[i] || now.tm_wday <= last_wday[i]) {
					seg = i;
					break;
				}
			} else  if(now.tm_wday >= first_wday[i] && now.tm_wday <= last_wday[i]) {
					seg = i;
					break;
			}
		}

		if(mode == RTC_MODE_WEEKDAY_CONTINUOUS) {
			if(seg >= 0 && (offset_now >= offset_start || now.tm_wday != first_wday[seg]) &&
				(offset_now <= offset_end || now.tm_wday != last_wday[seg])) {
				ret = 1;
			}
		} else if(mode == RTC_MODE_WEEKDAY_INTERMITTENT) {
			previous_wday_on = now.tm_wday - 1;
			if(previous_wday_on < 0) previous_wday_on = 6;
			if(seg < 0 && offset_start > offset_end) {
				for(i=0; i < segs_wday; i++) {
					if(first_wday[i] > last_wday[i]) {
						if(previous_wday_on >= first_wday[i] || previous_wday_on <= last_wday[i]) {
							seg_previous = i;
							break;
						}
					} else  if(previous_wday_on >= first_wday[i] && previous_wday_on <= last_wday[i]) {
							seg_previous = i;
							break;
					}
				}
			}
			previous_wday_on = (1 << previous_wday_on) & mask_wday;

			if((seg >= 0 || seg_previous >= 0) && (offset_start > offset_end ? ((offset_now >= offset_start && seg>=0) || (offset_now < offset_end && previous_wday_on)) :
				(offset_now >= offset_start && offset_now < offset_end))) {
				ret = 1;
			}
		}
	}

	return ret;
}

/*** End of functions related to RTC Simulation ***/

//-----------------------------------------------------------------------------
// Query the state of a single-bit element (relay, digital in, digital out).
// Looks in the SingleBitItems list; if an item is not present then it is
// FALSE by default.
//-----------------------------------------------------------------------------
static BOOL SingleBitOn(char *name)
{
    int i;
    for(i = 0; i < SingleBitItemsCount; i++) {
        if(_stricmp(SingleBitItems[i].name, name)==0) {
            return SingleBitItems[i].powered;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
// Set the state of a single-bit item. Adds it to the list if it is not there
// already.
//-----------------------------------------------------------------------------
static void SetSingleBit(char *name, BOOL state)
{
    int i;
    for(i = 0; i < SingleBitItemsCount; i++) {
        if(_stricmp(SingleBitItems[i].name, name)==0) {
            SingleBitItems[i].powered = state;
			UpdateSimulation(name, state);
            return;
        }
    }
    if(i < MAX_IO) {
        strcpy(SingleBitItems[i].name, name);
        SingleBitItems[i].powered = state;
		UpdateSimulation(name, state);
        SingleBitItemsCount++;
    }
}

//-----------------------------------------------------------------------------
// Count a timer up (i.e. increment its associated count by 1). Must already
// exist in the table.
//-----------------------------------------------------------------------------
static void IncrementVariable(char *name)
{
    int i;
    for(i = 0; i < VariablesCount; i++) {
        if(_stricmp(Variables[i].name, name)==0) {
            (Variables[i].val)++;
			UpdateSimulation(name, Variables[i].val);
            return;
        }
    }
    oops();
}

static void SetSimulationBit(char *name, int bit)
{
    int i;
    for(i = 0; i < VariablesCount; i++) {
        if(_stricmp(Variables[i].name, name)==0) {
            (Variables[i].val) |= 1 << bit;
			UpdateSimulation(name, Variables[i].val);
			NeedRedraw = TRUE;
            return;
        }
    }
	MarkUsedVariable(name, VAR_FLAG_OTHERWISE_FORGOTTEN);
    SetSimulationBit(name, bit);
} 
static void ClearSimulationBit(char *name, int bit)
{
    int i;
    for(i = 0; i < VariablesCount; i++) {
        if(_stricmp(Variables[i].name, name)==0) {
            (Variables[i].val) &= ~(1 << bit);
			UpdateSimulation(name, Variables[i].val);
			NeedRedraw = TRUE;
            return;
        }
    }
	MarkUsedVariable(name, VAR_FLAG_OTHERWISE_FORGOTTEN);
    SetSimulationBit(name, bit);
}

static bool CheckSimulationBit(char *name, int bit)
{
    int i;
    for(i = 0; i < VariablesCount; i++) {
        if(_stricmp(Variables[i].name, name)==0) {
            return (((Variables[i].val) & (1 << bit)) > 0);
        }
    }
	MarkUsedVariable(name, VAR_FLAG_OTHERWISE_FORGOTTEN);
    return CheckSimulationBit(name, bit);
}


//-----------------------------------------------------------------------------
// Set a variable to a value.
//-----------------------------------------------------------------------------
void SetSimulationVariable(char *name, SWORD val)
{
    int i;
    for(i = 0; i < VariablesCount; i++) {
        if(_stricmp(Variables[i].name, name)==0) {
            Variables[i].val = val;
			UpdateSimulation(name, val);
			NeedRedraw = TRUE;
            return;
        }
    }
    MarkUsedVariable(name, VAR_FLAG_OTHERWISE_FORGOTTEN);
    SetSimulationVariable(name, val);
}

//-----------------------------------------------------------------------------
// Read a variable's value.
//-----------------------------------------------------------------------------
SWORD GetSimulationVariable(char *name)
{
    int i;
    for(i = 0; i < VariablesCount; i++) {
        if(_stricmp(Variables[i].name, name)==0) {
            return Variables[i].val;
        }
    }
    MarkUsedVariable(name, VAR_FLAG_OTHERWISE_FORGOTTEN);
    return GetSimulationVariable(name);
}

//-----------------------------------------------------------------------------
// Set the shadow copy of a variable associated with a READ ADC operation. This
// will get committed to the real copy when the rung-in condition to the
// READ ADC is true.
//-----------------------------------------------------------------------------
void SetAdcShadow(char *name, SWORD val)
{
    int i;
    for(i = 0; i < AdcShadowsCount; i++) {
        if(_stricmp(AdcShadows[i].name, name)==0) {
            AdcShadows[i].val = val;
			UpdateSimulation(name, val);
            return;
        }
    }
    strcpy(AdcShadows[i].name, name);
    AdcShadows[i].val = val;
	UpdateSimulation(name, val);
    AdcShadowsCount++;
}

//-----------------------------------------------------------------------------
// Return the shadow value of a variable associated with a READ ADC. This is
// what gets copied into the real variable when an ADC read is simulated.
//-----------------------------------------------------------------------------
SWORD GetAdcShadow(char *name)
{
    int i;
    for(i = 0; i < AdcShadowsCount; i++) {
        if(_stricmp(AdcShadows[i].name, name)==0) {
            return AdcShadows[i].val;
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Set the shadow copy of a variable associated with a READ ADC operation. This
// will get committed to the real copy when the rung-in condition to the
// READ ADC is true.
//-----------------------------------------------------------------------------
void SetDAShadow(SWORD val)
{
    DAShadow = val;
	UpdateSimulation("$DA", val);
}

//-----------------------------------------------------------------------------
// Return the shadow value of a variable associated with a DAC. This is
// what gets copied into the real variable when an DAC write is simulated.
//-----------------------------------------------------------------------------
SWORD GetDAShadow(void)
{
    return DAShadow;
}

//-----------------------------------------------------------------------------
// Set the shadow copy of a variable associated with a READ ADC operation. This
// will get committed to the real copy when the rung-in condition to the
// READ ADC is true.
//-----------------------------------------------------------------------------
void SetEncShadow(char *name, SWORD val)
{
    int i;
    for(i = 0; i < EncShadowsCount; i++) {
        if(_stricmp(EncShadows[i].name, name)==0) {
            EncShadows[i].val = val;
			UpdateSimulation(name, val);
            return;
        }
    }
    strcpy(EncShadows[i].name, name);
    EncShadows[i].val = val;
	UpdateSimulation(name, val);
    EncShadowsCount++;
}

//-----------------------------------------------------------------------------
// Return the shadow value of a variable associated with a READ ADC. This is
// what gets copied into the real variable when an ADC read is simulated.
//-----------------------------------------------------------------------------
SWORD GetEncShadow(char *name)
{
    int i;
    for(i = 0; i < EncShadowsCount; i++) {
        if(_stricmp(EncShadows[i].name, name)==0) {
            return EncShadows[i].val;
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Set the shadow copy of a variable associated with a READ ADC operation. This
// will get committed to the real copy when the rung-in condition to the
// READ ADC is true.
//-----------------------------------------------------------------------------
void SetResetEncShadow(char *name, SWORD val)
{
    int i;
    for(i = 0; i < ResetEncShadowsCount; i++) {
        if(_stricmp(ResetEncShadows[i].name, name)==0) {
            ResetEncShadows[i].val = val;
			UpdateSimulation(name, val);
            return;
        }
    }
    strcpy(ResetEncShadows[i].name, name);
    ResetEncShadows[i].val = val;
	UpdateSimulation(name, val);
    ResetEncShadowsCount++;
}

//-----------------------------------------------------------------------------
// Return the shadow value of a variable associated with a READ ADC. This is
// what gets copied into the real variable when an ADC read is simulated.
//-----------------------------------------------------------------------------
SWORD GetResetEncShadow(char *name)
{
    int i;
    for(i = 0; i < ResetEncShadowsCount; i++) {
        if(_stricmp(ResetEncShadows[i].name, name)==0) {
            return ResetEncShadows[i].val;
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------
// Mark how a variable is used; a series of flags that we can OR together,
// then we can check to make sure that only valid combinations have been used
// (e.g. just a TON, an RTO with its reset, etc.). Returns NULL for success,
// else an error string.
//-----------------------------------------------------------------------------
static char *MarkUsedVariable(char *name, DWORD flag)
{
    int i;
    for(i = 0; i < VariablesCount; i++) {
        if(_stricmp(Variables[i].name, name)==0) {
            break;
        }
    }
    if(i >= MAX_IO) return "";

    if(i == VariablesCount) {
        strcpy(Variables[i].name, name);
        Variables[i].usedFlags = 0;
        Variables[i].val = 0;
        VariablesCount++;
    }

    switch(flag) {
        case VAR_FLAG_TOF:
            if(Variables[i].usedFlags != 0) 
                return _("TOF: variable cannot be used elsewhere");
            break;

        case VAR_FLAG_TON:
            if(Variables[i].usedFlags != 0)
                return _("TON: variable cannot be used elsewhere");
            break;
        
        case VAR_FLAG_RTO:
            if(Variables[i].usedFlags & ~VAR_FLAG_RES)
                return _("RTO: variable can only be used for RES elsewhere");
            break;

        case VAR_FLAG_CTU:
        case VAR_FLAG_CTD:
        case VAR_FLAG_CTC:
        case VAR_FLAG_RES:
        case VAR_FLAG_ANY:
            break;

        case VAR_FLAG_OTHERWISE_FORGOTTEN:
            if(name[0] != '$') 
			{
                /*Error(_("Variable '%s' not assigned to, e.g. with a "
                    "MOV statement, an ADD statement, etc.\r\n\r\n"
                    "This is probably a programming error; now it "
                    "will always be zero."), name);*/
            }
            break;

        default:
            oops();
    }

    Variables[i].usedFlags |= flag;
    return NULL;
}

//-----------------------------------------------------------------------------
// Check for duplicate uses of a single variable. For example, there should
// not be two TONs with the same name. On the other hand, it would be okay
// to have an RTO with the same name as its reset; in fact, verify that
// there must be a reset for each RTO.
//-----------------------------------------------------------------------------
static void MarkWithCheck(char *name, int flag)
{
    char *s = MarkUsedVariable(name, flag);
    if(s) {
        Error(_("Variable for '%s' incorrectly assigned: %s."), name, s);
    }
}
static void CheckVariableNamesCircuit(int which, void *elem)
{
    ElemLeaf *l = (ElemLeaf *)elem;
    char *name = NULL;
    DWORD flag;

    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            int i;
            ElemSubcktSeries *s = (ElemSubcktSeries *)elem;
            for(i = 0; i < s->count; i++) {
                CheckVariableNamesCircuit(s->contents[i].which,
                    s->contents[i].d.any);
            }
            break;
        }

        case ELEM_PARALLEL_SUBCKT: {
            int i;
            ElemSubcktParallel *p = (ElemSubcktParallel *)elem;
            for(i = 0; i < p->count; i++) {
                CheckVariableNamesCircuit(p->contents[i].which,
                    p->contents[i].d.any);
            }
            break;
        }
        
        case ELEM_RTO:
        case ELEM_TOF:
        case ELEM_TON:
            if(which == ELEM_RTO)
                flag = VAR_FLAG_RTO;
            else if(which == ELEM_TOF)
                flag = VAR_FLAG_TOF;
            else if(which == ELEM_TON)
                flag = VAR_FLAG_TON;
            else oops();

            MarkWithCheck(l->d.timer.name, flag);

            break;

        case ELEM_CTU:
        case ELEM_CTD:
        case ELEM_CTC:
            if(which == ELEM_CTU)
                flag = VAR_FLAG_CTU;
            else if(which == ELEM_CTD)
                flag = VAR_FLAG_CTD;
            else if(which == ELEM_CTC)
                flag = VAR_FLAG_CTC;
            else oops();

            MarkWithCheck(l->d.counter.name, flag);

            break;

        case ELEM_RES:
            MarkWithCheck(l->d.reset.name, VAR_FLAG_RES);
            break;

        case ELEM_MOVE:
            MarkWithCheck(l->d.move.dest, VAR_FLAG_ANY);
            break;

        case ELEM_SQRT:
            MarkWithCheck(l->d.sqrt.dest, VAR_FLAG_ANY);
            break;

        case ELEM_RAND:
			MarkWithCheck(l->d.rand.var, VAR_FLAG_ANY);
            break;

        case ELEM_ABS:
            MarkWithCheck(l->d.abs.dest, VAR_FLAG_ANY);
            break;

        case ELEM_LOOK_UP_TABLE:
            MarkWithCheck(l->d.lookUpTable.dest, VAR_FLAG_ANY);
            break;

        case ELEM_PIECEWISE_LINEAR:
            MarkWithCheck(l->d.piecewiseLinear.dest, VAR_FLAG_ANY);
            break;

        case ELEM_READ_ADC:
            MarkWithCheck(l->d.readAdc.name, VAR_FLAG_ANY);
            break;

        case ELEM_SET_DA:
            MarkWithCheck(l->d.setDA.name, VAR_FLAG_ANY);
            break;

        case ELEM_READ_ENC:
            MarkWithCheck(l->d.readEnc.name, VAR_FLAG_ANY);
            break;

        case ELEM_READ_USS:
            MarkWithCheck(l->d.readUSS.name, VAR_FLAG_ANY);
            break;

        case ELEM_WRITE_USS:
            MarkWithCheck(l->d.writeUSS.name, VAR_FLAG_ANY);
			MarkWithCheck("$USSReady", VAR_FLAG_ANY);
            break;

        case ELEM_READ_MODBUS:
            MarkWithCheck(l->d.readModbus.name, VAR_FLAG_ANY);
            break;

        case ELEM_WRITE_MODBUS:
            MarkWithCheck(l->d.writeModbus.name, VAR_FLAG_ANY);
			MarkWithCheck("$ModbusReady", VAR_FLAG_ANY);
            break;

        case ELEM_ADD:
        case ELEM_SUB:
        case ELEM_MUL:
        case ELEM_DIV:
            MarkWithCheck(l->d.math.dest, VAR_FLAG_ANY);
            break;

        case ELEM_UART_RECV:
            MarkWithCheck(l->d.uart.name, VAR_FLAG_ANY);
            break;

        case ELEM_SHIFT_REGISTER: {
            int i;
            for(i = 1; i < l->d.shiftRegister.stages; i++) {
                char str[MAX_NAME_LEN+10];
                sprintf(str, "%s%d", l->d.shiftRegister.name, i);
                MarkWithCheck(str, VAR_FLAG_ANY);
            }
            break;
        }

        case ELEM_PERSIST:
        case ELEM_RESET_ENC:
		case ELEM_READ_FORMATTED_STRING:
		case ELEM_WRITE_FORMATTED_STRING:
		case ELEM_READ_SERVO_YASKAWA:
		case ELEM_WRITE_SERVO_YASKAWA:
        case ELEM_FORMATTED_STRING:
        case ELEM_SET_PWM:
        case ELEM_MASTER_RELAY:
        case ELEM_UART_SEND:
        case ELEM_PLACEHOLDER:
        case ELEM_COMMENT:
        case ELEM_OPEN:
        case ELEM_SHORT:
        case ELEM_COIL:
        case ELEM_CONTACTS:
		case ELEM_SET_BIT:
		case ELEM_CHECK_BIT:
        case ELEM_ONE_SHOT_RISING:
        case ELEM_ONE_SHOT_FALLING:
        case ELEM_EQU:
        case ELEM_NEQ:
        case ELEM_GRT:
        case ELEM_GEQ:
        case ELEM_LES:
        case ELEM_LEQ:
		case ELEM_RTC:
		case ELEM_MULTISET_DA:
            break;

        default:
            oops();
    }
}
static void CheckVariableNames(void)
{
    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        CheckVariableNamesCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    }
}

//-----------------------------------------------------------------------------
// The IF condition is true. Execute the body, up until the ELSE or the
// END IF, and then skip the ELSE if it is present. Called with PC on the
// IF, returns with PC on the END IF.
//-----------------------------------------------------------------------------
static void IfConditionTrue(void)
{
    IntPc++;
    // now PC is on the first statement of the IF body
    SimulateIntCode();
    // now PC is on the ELSE or the END IF
    if(INT_ELSE_GROUP(IntCode[IntPc].op)) {
        int nesting = 1;
        for(; ; IntPc++) {
            if(IntPc >= IntCodeLen) oops();

            if(IntCode[IntPc].op == INT_END_IF) {
                nesting--;
            } else if(INT_IF_GROUP(IntCode[IntPc].op) && IntCode[IntPc-1].op != INT_ELSE_IF) {
                nesting++;
            }
            if(nesting == 0) break;
        }
    } else if(IntCode[IntPc].op == INT_END_IF) {
        return;
    } else {
        oops();
    }
}

//-----------------------------------------------------------------------------
// The IF condition is false. Skip the body, up until the ELSE or the END
// IF, and then execute the ELSE if it is present. Called with PC on the IF,
// returns with PC on the END IF.
//-----------------------------------------------------------------------------
static void IfConditionFalse(void)
{
    int nesting = 1;
	bool is_elseif = IntCode[IntPc-1].op == INT_ELSE_IF;

	IntPc++;

	for(; ; IntPc++) {
        if(IntPc >= IntCodeLen) oops();

        if(IntCode[IntPc].op == INT_END_IF) {
            nesting--;
        } else if(INT_IF_GROUP(IntCode[IntPc].op)) {
			if(IntCode[IntPc-1].op != INT_ELSE_IF) // when else if, no nesting!
				nesting++;
        } else if(INT_ELSE_GROUP(IntCode[IntPc].op) && nesting == 1) {
            break;
        }
		if(nesting == 0) break;
    }

    // now PC is on the ELSE, ELSE IF or the END IF
    if(INT_ELSE_GROUP(IntCode[IntPc].op)) {
        IntPc++;
        SimulateIntCode();
    } else if(IntCode[IntPc].op == INT_END_IF) {
		if(is_elseif) IntPc--; // I am elseif, this endif is for upper level
        return;
    } else {
        oops();
    }

	if(is_elseif) IntPc--; // I am elseif, this endif is for upper level
}

//-----------------------------------------------------------------------------
// Evaluate a circuit, calling ourselves recursively to evaluate if/else
// constructs. Updates the on/off state of all the leaf elements in our
// internal tables. Returns when it reaches an end if or an else construct,
// or at the end of the program.
//-----------------------------------------------------------------------------
static void SimulateIntCode(void)
{
    for(; IntPc < IntCodeLen; IntPc++) {
        IntOp *a = &IntCode[IntPc];
        switch(a->op) {
            case INT_SIMULATE_NODE_STATE:
                if(*(a->poweredAfter) != SingleBitOn(a->name1))
                    NeedRedraw = TRUE;
                *(a->poweredAfter) = SingleBitOn(a->name1);
                break;

            case INT_SET_BIT:
                SetSingleBit(a->name1, TRUE);
                break;

            case INT_CLEAR_BIT:
                SetSingleBit(a->name1, FALSE);
                break;

            case INT_SET_SINGLE_BIT:
                //SetSingleBit(a->name1, TRUE);
				SetSimulationBit(a->name1, a->bit);
                break;

            case INT_CLEAR_SINGLE_BIT:
                //SetSingleBit(a->name1, FALSE);
				ClearSimulationBit(a->name1, a->bit);
                break;

            case INT_COPY_BIT_TO_BIT:
                SetSingleBit(a->name1, SingleBitOn(a->name2));
                break;

            case INT_SET_VARIABLE_TO_LITERAL:
                if(GetSimulationVariable(a->name1) !=
                    a->literal && a->name1[0] != '$')
                {
                    NeedRedraw = TRUE;
                }
                SetSimulationVariable(a->name1, a->literal);
                break;

            case INT_SET_VARIABLE_TO_VARIABLE:
                if(GetSimulationVariable(a->name1) != 
                    GetSimulationVariable(a->name2))
                {
                    NeedRedraw = TRUE;
                }
                SetSimulationVariable(a->name1,
                    GetSimulationVariable(a->name2));
                break;

            case INT_INCREMENT_VARIABLE:
                IncrementVariable(a->name1);
				NeedRedraw = TRUE;
                break;

            {
                SWORD v;
                case INT_SET_VARIABLE_ADD:
                    v = GetSimulationVariable(a->name2) +
                        GetSimulationVariable(a->name3);
                    goto math;
                case INT_SET_VARIABLE_SUBTRACT:
                    v = GetSimulationVariable(a->name2) -
                        GetSimulationVariable(a->name3);
                    goto math;
                case INT_SET_VARIABLE_MULTIPLY:
                    v = GetSimulationVariable(a->name2) *
                        GetSimulationVariable(a->name3);
                    goto math;
                case INT_SET_VARIABLE_DIVIDE:
                    if(GetSimulationVariable(a->name3) != 0) {
                        v = GetSimulationVariable(a->name2) /
                            GetSimulationVariable(a->name3);
                    } else {
                        v = 0;
                        Error(_("Divisão por zero na variavel: '%s', a simulação será interrompida."), a->name3);
                        StopSimulation();
                    }
                    goto math;
math:
                    if(GetSimulationVariable(a->name1) != v) {
                        NeedRedraw = TRUE;
                        SetSimulationVariable(a->name1, v);
                    }
                    break;
            }

#define IF_BODY \
    { \
        IfConditionTrue(); \
    } else { \
        IfConditionFalse(); \
    }
            case INT_IF_BIT_SET:
                if(SingleBitOn(a->name1))
                    IF_BODY
                break;

            case INT_IF_BIT_CLEAR:
                if(!SingleBitOn(a->name1))
                    IF_BODY
                break;

			case INT_IF_BIT_CHECK_SET:
                if(CheckSimulationBit(a->name1, a->bit))
                    IF_BODY
                break;

            case INT_IF_BIT_CHECK_CLEAR:
                if(!CheckSimulationBit(a->name1, a->bit))
                    IF_BODY
                break;

            case INT_IF_VARIABLE_LES_LITERAL:
                if(GetSimulationVariable(a->name1) < a->literal)
                    IF_BODY
                break;

            case INT_IF_VARIABLE_EQUALS_VARIABLE:
                if(GetSimulationVariable(a->name1) ==
                    GetSimulationVariable(a->name2))
                    IF_BODY
                break;

            case INT_IF_VARIABLE_GRT_VARIABLE:
                if(GetSimulationVariable(a->name1) >
                    GetSimulationVariable(a->name2))
                    IF_BODY
                break;

            case INT_SET_PWM:
                // Dummy call will cause a warning if no one ever assigned
                // to that variable.
                (void)GetSimulationVariable(a->name1);
                break;

            // Don't try to simulate the EEPROM stuff: just hold the EEPROM
            // busy all the time, so that the program never does anything
            // with it.
            case INT_EEPROM_BUSY_CHECK:
                SetSingleBit(a->name1, TRUE);
                break;

            case INT_EEPROM_READ:
            case INT_EEPROM_WRITE:
                oops();
                break;

            case INT_READ_ADC:
                // Keep the shadow copies of the ADC variables because in
                // the real device they will not be updated until an actual
                // read is performed, which occurs only for a true rung-in
                // condition there.
                SetSimulationVariable(a->name1, GetAdcShadow(a->name1));
                break;

            case INT_SET_DA:
                SetDAShadow(GetSimulationVariable(a->name1));
                break;

            case INT_READ_ENC:
                SetSimulationVariable(a->name1, GetEncShadow(a->name1));
                break;

            case INT_RESET_ENC:
                SetSimulationVariable(a->name1, GetResetEncShadow(a->name1));
                break;

            case INT_SQRT:
                SetSimulationVariable(a->name1, (int)sqrt((float)GetSimulationVariable(a->name1)));
                break;

            case INT_RAND: {
				int min, max;
				min = IsNumber(a->name2) ? atoi(a->name2) : (int)GetSimulationVariable(a->name2);
				max = IsNumber(a->name3) ? atoi(a->name3) : (int)GetSimulationVariable(a->name3);

				SetSimulationVariable(a->name1, min + (rand() % (max - min + 1)));
				}
                break;

			case INT_MULTISET_DA:
				SimulateMultisetDA.StartValue = GetDAShadow();

				if(strlen(a->name1)) {
					int time = IsNumber(a->name1) ? atoi(a->name1) : GetSimulationVariable(a->name1);
					SimulateMultisetDA.Count = time/PLC_CYCLE;
					SimulateMultisetDA.FinalValue = Simulate_DaConv(IsNumber(a->name2) ? atoi(a->name2) : GetSimulationVariable(a->name2), atoi(a->name5));
					SetSingleBit("$RampActive", TRUE);
				} else if(SingleBitOn("$RampActive")) {
					switch(a->literal) {
					case RAMP_ABORT_LEAVE:
						SimulateMultisetDA.Count = 0;
						SetSingleBit("$RampActive", FALSE);
						break;
					case RAMP_ABORT_STOP:
						SimulateMultisetDA.Count = 1;  // Desacelera em  10 ms =  1 ciclo
						SimulateMultisetDA.FinalValue = DA_RESOLUTION;
						SetSingleBit("$RampActive", TRUE);
						break;
					case RAMP_ABORT_DESACEL:
						SimulateMultisetDA.Count = 10; // Desacelera em 100 ms = 10 ciclos
						SimulateMultisetDA.FinalValue = DA_RESOLUTION;
						SetSingleBit("$RampActive", TRUE);
						break;
					}
				}

				SimulateMultisetDA.StartCount = SimulateMultisetDA.Count;

				break;

			case INT_READ_FORMATTED_STRING:
				SetSingleBit("$SerialReady", FALSE);
				SimulateUSSTxCountdown = 0;
				AppendToFormattedStringSimulationTextControl("RS485 Read: %s\r\n", a->name2, GetSimulationVariable(a->name1));
                break;
			case INT_WRITE_FORMATTED_STRING:
				SetSingleBit("$SerialReady", FALSE);
				SimulateUSSTxCountdown = 0;
				AppendToFormattedStringSimulationTextControl("RS485 Write: %s\r\n", a->name2, GetSimulationVariable(a->name1));
                break;
			case INT_READ_SERVO_YASKAWA:
				SetSingleBit("$SerialReady", FALSE);
				SimulateUSSTxCountdown = 0;
				AppendToFormattedStringSimulationTextControl("Servo Read: %s\r\n", a->name2, GetSimulationVariable(a->name1));
                break;
			case INT_WRITE_SERVO_YASKAWA:
				SetSingleBit("$SerialReady", FALSE);
				SimulateUSSTxCountdown = 0;
				AppendToFormattedStringSimulationTextControl("Servo Write: %s\r\n", a->name2, GetSimulationVariable(a->name1));
                break;
            case INT_READ_USS:
				SetSingleBit("$USSReady", FALSE);
				SimulateUSSTxCountdown = 0;
				AppendToUSSSimulationTextControl(atoi(a->name2), atoi(a->name3), atoi(a->name4), a->name1, GetSimulationVariable(a->name1));
                break;

            case INT_WRITE_USS:
				SetSingleBit("$USSReady", FALSE);
				SimulateUSSTxCountdown = 0;
				AppendToUSSSimulationTextControl(atoi(a->name2), atoi(a->name3), atoi(a->name4), a->name1, GetSimulationVariable(a->name1));
                break;

            case INT_READ_MODBUS:
            case INT_WRITE_MODBUS:
				if(a->literal == 0) { // RS-485
					SetSingleBit("$SerialReady", FALSE);
					SimulateModbus485Countdown = 2;
				} else { // Ethernet
					SetSingleBit("$TcpReady", FALSE);
					SimulateModbusEthCountdown = 2;
				}
				AppendToModbusSimulationTextControl(atoi(a->name2), atoi(a->name3), a->name1);
                break;

            case INT_UART_SEND:
                if(SingleBitOn(a->name2) && (SimulateUartTxCountdown == 0)) 
				{
                    SimulateUartTxCountdown = 2;
                    AppendToUartSimulationTextControl((BYTE)GetSimulationVariable(a->name1));
                }
                if(SimulateUartTxCountdown == 0) {
                    SetSingleBit(a->name2, FALSE);
                } else {
                    SetSingleBit(a->name2, TRUE);
                }
                break;

            case INT_UART_RECV:
                if(QueuedUartCharacter >= 0) {
                    SetSingleBit(a->name2, TRUE);
                    SetSimulationVariable(a->name1, (SWORD)QueuedUartCharacter);
                    QueuedUartCharacter = -1;
                } else {
                    SetSingleBit(a->name2, FALSE);
                }
                break;

			case INT_SET_RTC:
				{
					struct tm * t = a->bit ? &RTC_EndTM : &RTC_StartTM;

					t->tm_mday  = atoi(a->name1);
					t->tm_mon   = atoi(a->name2);
					t->tm_year  = atoi(a->name3);
					t->tm_hour  = atoi(a->name4);
					t->tm_min   = atoi(a->name5);
					t->tm_sec   = atoi(a->name6);
				}
				break;

			case INT_CHECK_RTC:
				{
					int RTC_Mode, mode = atoi(a->name2), UseWeekDays = (a->literal>>7)&1;
					time_t rawtime;
					struct tm t;

					time ( &rawtime );
					t = *localtime ( &rawtime );

					if(mode == ELEM_RTC_MODE_FIXED) {
						int DateOK;
						if (UseWeekDays) {
							DateOK = a->literal & (1 << t.tm_wday);
						} else {
							DateOK = (!RTC_StartTM.tm_mday || t.tm_mday == RTC_StartTM.tm_mday       ) &&
								     (!RTC_StartTM.tm_mon  || t.tm_mon  == RTC_StartTM.tm_mon  -    1) &&
									 (!RTC_StartTM.tm_year || t.tm_year == RTC_StartTM.tm_year - 1900);
						}

						SetSingleBit(a->name1, DateOK &&
						                      (t.tm_hour == RTC_StartTM.tm_hour) && 
						                      (t.tm_min  == RTC_StartTM.tm_min ) && 
						                      (t.tm_sec  == RTC_StartTM.tm_sec ));
					} else {
						RTC_StartTM = *AdjustDate(RTC_StartTM, RTC_GETDATE_MODE_START);
						RTC_EndTM   = *AdjustDate(RTC_EndTM  , RTC_GETDATE_MODE_END  );

						if(mode == ELEM_RTC_MODE_CONTINUOUS) {
							RTC_Mode = UseWeekDays ? RTC_MODE_WEEKDAY_CONTINUOUS   : RTC_MODE_DATE_CONTINUOUS;
						} else {
							RTC_Mode = UseWeekDays ? RTC_MODE_WEEKDAY_INTERMITTENT : RTC_MODE_DATE_INTERMITTENT;
						}

						SetSingleBit(a->name1, RTC_OutputState(RTC_StartTM, RTC_EndTM, t, RTC_Mode, a->literal & 0x7F));
					}
				}
				break;

			case INT_END_IF:
            case INT_ELSE_IF:
            case INT_ELSE:
                return;

            case INT_COMMENT:
                break;
            
            default:
                oops();
                break;
        }
    }
}

//-----------------------------------------------------------------------------
// Called by the Windows timer that triggers cycles when we are running
// in real time.
//-----------------------------------------------------------------------------
void CALLBACK PlcCycleTimer(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
    int i;
    for(i = 0; i < CyclesPerTimerTick; i++) {
        SimulateOneCycle(FALSE);
    }
}

//-----------------------------------------------------------------------------
// Simulate one cycle of the PLC. Update everything, and keep track of whether
// any outputs have changed. If so, force a screen refresh. If requested do
// a screen refresh regardless.
//-----------------------------------------------------------------------------
void SimulateOneCycle(BOOL forceRefresh)
{
    // When there is an error message up, the modal dialog makes its own
    // event loop, and there is risk that we would go recursive. So let
    // us fix that. (Note that there are no concurrency issues; we really
    // would get called recursively, not just reentrantly.)
    static BOOL Simulating = FALSE;

    if(Simulating) return;
    Simulating = TRUE;

    if(SimulateUartTxCountdown > 0) {
        SimulateUartTxCountdown--;
    } else {
        SimulateUartTxCountdown = 0;
    }

	if (SimulateUSSTxCountdown >= 0 && !SingleBitOn("$SerialReady"))
		SimulateUSSTxCountdown++;

	if (SimulateUSSTxCountdown > 2)
	{
		SetSingleBit("$SerialReady", TRUE);
	}

	if (SimulateModbus485Countdown > 0) {
		SimulateModbus485Countdown--;
		if (!SimulateModbus485Countdown) {
			SetSingleBit("$SerialReady", TRUE);
		}
	}

	if (SimulateModbusEthCountdown > 0) {
		SimulateModbusEthCountdown--;
		if (!SimulateModbusEthCountdown) {
			SetSingleBit("$TcpReady", TRUE);
		}
	}

	if (SimulateMultisetDA.Count > 0)	{
		float offset;

		SimulateMultisetDA.Count--;
		if(!SimulateMultisetDA.Count) {
			SetSingleBit("$RampActive", FALSE);
			SetDAShadow(SimulateMultisetDA.FinalValue);
		} else {
			offset = (float)(SimulateMultisetDA.StartValue - SimulateMultisetDA.FinalValue) / SimulateMultisetDA.StartCount;
			SetDAShadow(SimulateMultisetDA.FinalValue + (int)(offset*SimulateMultisetDA.Count));
		}
	}

    IntPc = 0;
    SimulateIntCode();

    if(NeedRedraw || SimulateRedrawAfterNextCycle || forceRefresh) {
		NeedRedraw = FALSE;
        InvalidateRect(MainWindow, NULL, FALSE);
        ListView_RedrawItems(IoList, 0, Prog.io.count - 1);
    }

    SimulateRedrawAfterNextCycle = FALSE;
    if(NeedRedraw) SimulateRedrawAfterNextCycle = TRUE;

	if(hwreg.NeedUpdate && hwreg.Syncing == false) {
		UpdateSimulation(NULL, 0);
		hwreg.NeedUpdate = false;
	}

    Simulating = FALSE;
}

//-----------------------------------------------------------------------------
// Start the timer that we use to trigger PLC cycles in approximately real
// time. Independently of the given cycle time, just go at 40 Hz, since that
// is about as fast as anyone could follow by eye. Faster timers will just
// go instantly.
//-----------------------------------------------------------------------------
void StartSimulationTimer(void)
{
    int p = Prog.settings.cycleTime/1000;
    if(p < 5) {
        SetTimer(MainWindow, TIMER_SIMULATE, 10, PlcCycleTimer);
        CyclesPerTimerTick = 10000 / Prog.settings.cycleTime;
    } else {
        SetTimer(MainWindow, TIMER_SIMULATE, p, PlcCycleTimer);
        CyclesPerTimerTick = 1;
    }
}

//-----------------------------------------------------------------------------
// Clear out all the parameters relating to the previous simulation.
//-----------------------------------------------------------------------------
void ClearSimulationData(void)
{
    VariablesCount = 0;
    SingleBitItemsCount = 0;
    AdcShadowsCount = 0;
	readUSSShadowsCount = 0;
	writeUSSShadowsCount = 0;
    QueuedUartCharacter = -1;
    QueuedUSSCharacter = -1;
    SimulateUartTxCountdown = 0;
    SimulateUSSTxCountdown = -1;
    SimulateModbus485Countdown = 0;
    SimulateModbusEthCountdown = 0;

	SetSingleBit("$USSReady", TRUE);
	SetSingleBit("$SerialReady", TRUE);
	SetSingleBit("$TcpReady", TRUE);

	CheckVariableNames();

    SimulateRedrawAfterNextCycle = TRUE;

    if(!GenerateIntermediateCode()) {
        ToggleSimulationMode();
        return;
    }

	HardwareRegisters_Init(&hwreg);

	SimulateOneCycle(TRUE);
}

//-----------------------------------------------------------------------------
// Provide a description for an item (Xcontacts, Ycoil, Rrelay, Ttimer,
// or other) in the I/O list.
//-----------------------------------------------------------------------------
void DescribeForIoList(int val, int type, char *out)
{
    switch(type) {
        case IO_TYPE_INTERNAL_FLAG:
        case IO_TYPE_INTERNAL_RELAY:
        case IO_TYPE_DIG_OUTPUT:
        case IO_TYPE_DIG_INPUT:
            sprintf(out, "%d", val);
            break;

		case IO_TYPE_SET_DA: {
            sprintf(out, "%i (0x%08x)", val, val);
            break;
		}

        case IO_TYPE_TON:
        case IO_TYPE_TOF:
        case IO_TYPE_RTO: {
            double dtms = val * (Prog.settings.cycleTime / 1000.0);
            if(dtms < 1000) {
                sprintf(out, "%.2f ms", dtms);
            } else {
                sprintf(out, "%.3f s", dtms / 1000);
            }
            break;
			}
        default: {
            sprintf(out, "%i (0x%08x)", val, val);
            break;
        }
    }
}

void DescribeForIoList(char *name, int type, char *out)
{
	int val;
	char varname[MAX_NAME_LEN];

	strcpy(varname, name);

	switch(type) {
        case IO_TYPE_INTERNAL_FLAG:
			sprintf(varname, "$%s", name);
        case IO_TYPE_INTERNAL_RELAY:
        case IO_TYPE_DIG_OUTPUT:
        case IO_TYPE_DIG_INPUT:
            val = SingleBitOn(varname);
            break;

		case IO_TYPE_SET_DA:
            val = GetDAShadow();
            break;

        case IO_TYPE_TON:
        case IO_TYPE_TOF:
        case IO_TYPE_RTO:
            val = GetSimulationVariable(varname);
			break;

        default:
            val = GetSimulationVariable(varname);
    }

	DescribeForIoList(val, type, out);
}

//-----------------------------------------------------------------------------
// Toggle the state of a contact input; for simulation purposes, so that we
// can set the input state of the program.
//-----------------------------------------------------------------------------
void SimulationToggleContact(char *name)
{
    SetSingleBit(name, !SingleBitOn(name));
    ListView_RedrawItems(IoList, 0, Prog.io.count - 1);
}

//-----------------------------------------------------------------------------
// Dialog proc for the popup that lets you interact with the UART stuff.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK UartSimulationProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_DESTROY:
            DestroyUartSimulationWindow();
            break;

        case WM_CLOSE:
            break;

        case WM_SIZE:
            MoveWindow(UartSimulationTextControl, 0, 0, LOWORD(lParam),
                HIWORD(lParam), TRUE);
            break;

        case WM_ACTIVATE:
            if(wParam != WA_INACTIVE) {
                SetFocus(UartSimulationTextControl);
            }
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 1;
}

//-----------------------------------------------------------------------------
// Intercept WM_CHAR messages that to the terminal simulation window so that
// we can redirect them to the PLC program.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK UartSimulationTextProc(HWND hwnd, UINT msg, 
    WPARAM wParam, LPARAM lParam)
{
    if(msg == WM_CHAR) {
        QueuedUartCharacter = (BYTE)wParam;
        return 0;
    }

    return CallWindowProc((WNDPROC)PrevTextProc, hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Pop up the UART simulation window; like a terminal window where the
// characters that you type go into UART RECV instruction and whatever
// the program puts into UART SEND shows up as text.
//-----------------------------------------------------------------------------
void ShowUartSimulationWindow(void)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)UartSimulationProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "POPToolsUartSimulationWindow";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    DWORD TerminalX = 200, TerminalY = 200, TerminalW = 300, TerminalH = 150;

    ThawDWORD(TerminalX);
    ThawDWORD(TerminalY);
    ThawDWORD(TerminalW);
    ThawDWORD(TerminalH);

    if(TerminalW > 800) TerminalW = 100;
    if(TerminalH > 800) TerminalH = 100;

    //RECT r;

    //GetClientRect(GetDesktopWindow(), &r);
    //if(TerminalX >= (DWORD)(r.right - 10)) TerminalX = 100;
    //if(TerminalY >= (DWORD)(r.bottom - 10)) TerminalY = 100;

    //UartSimulationWindow = CreateWindowClient(WS_EX_TOOLWINDOW |
    //    WS_EX_APPWINDOW, "POPToolsUartSimulationWindow",
    //    "UART Simulation (Terminal)", WS_VISIBLE | WS_SIZEBOX,
    //    TerminalX, TerminalY, TerminalW, TerminalH,
    //    MainWindow, NULL, Instance, NULL);

    UartSimulationWindow = CreateWindowClient(/*WS_EX_TOOLWINDOW |
        WS_EX_APPWINDOW, "POPToolsUartSimulationWindow",
        "UART Simulation (Terminal)", WS_VISIBLE | WS_SIZEBOX*/
		WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_CHILD |
        LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS | WS_TABSTOP |
        LVS_SINGLESEL | WS_CLIPSIBLINGS,
        TerminalX, TerminalY, TerminalW, TerminalH,
        MainWindow, NULL, Instance, NULL);

	ListView_SetExtendedListViewStyle(UartSimulationWindow, LVS_EX_DOUBLEBUFFER);

    UartSimulationTextControl = CreateWindowEx(0, WC_EDIT, "", WS_CHILD |
        WS_CLIPSIBLINGS | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE |
        WS_VSCROLL, 0, 0, TerminalW, TerminalH, UartSimulationWindow, NULL,
        Instance, NULL);

    HFONT fixedFont = CreateFont(14, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        FF_DONTCARE, "Lucida Console");
    if(!fixedFont)
        fixedFont = (HFONT)GetStockObject(SYSTEM_FONT);

    SendMessage((HWND)UartSimulationTextControl, WM_SETFONT, (WPARAM)fixedFont,
        TRUE);

    PrevTextProc = SetWindowLongPtr(UartSimulationTextControl,
        GWLP_WNDPROC, (LONG_PTR)UartSimulationTextProc);

	MainWindowResized();

    ShowWindow(UartSimulationWindow, TRUE);
    SetFocus(MainWindow);
}

//-----------------------------------------------------------------------------
// Get rid of the UART simulation terminal-type window.
//-----------------------------------------------------------------------------
void DestroyUartSimulationWindow(void)
{
    // Try not to destroy the window if it is already destroyed; that is
    // not for the sake of the window, but so that we don't trash the
    // stored position.
    if(UartSimulationWindow == NULL) return;

    DWORD TerminalX, TerminalY, TerminalW, TerminalH;
    RECT r;

    GetClientRect(UartSimulationWindow, &r);
    TerminalW = r.right - r.left;
    TerminalH = r.bottom - r.top;

    GetWindowRect(UartSimulationWindow, &r);
    TerminalX = r.left;
    TerminalY = r.top;

    FreezeDWORD(TerminalX);
    FreezeDWORD(TerminalY);
    FreezeDWORD(TerminalW);
    FreezeDWORD(TerminalH);

    DestroyWindow(UartSimulationWindow);
    UartSimulationWindow = NULL;
	MainWindowResized();
}

//-----------------------------------------------------------------------------
// Append a received character to the terminal buffer.
//-----------------------------------------------------------------------------
static void AppendToUartSimulationTextControl(BYTE b)
{
    char append[5];

    if((isalnum(b) || strchr("[]{};':\",.<>/?`~ !@#$%^&*()-=_+|", b) ||
        b == '\r' || b == '\n') && b != '\0')
    {
        append[0] = b;
        append[1] = '\0';
    } else {
        sprintf(append, "\\x%02x", b);
    }

    char buf[MAX_SCROLLBACK];

    SendMessage(UartSimulationTextControl, WM_GETTEXT, (WPARAM)sizeof(buf),
        (LPARAM)buf);

    int overBy = (strlen(buf) + strlen(append) + 1) - sizeof(buf);
    if(overBy > 0) {
        memmove(buf, buf + overBy, strlen(buf));
    }
    strcat(buf, append);

    SendMessage(UartSimulationTextControl, WM_SETTEXT, 0, (LPARAM)buf);
    SendMessage(UartSimulationTextControl, EM_LINESCROLL, 0, (LPARAM)INT_MAX);
}

static void AppendToFormattedStringSimulationTextControl(char * text, char *string, SWORD val)
{
	char formatstring[125];
    char append[125];

 	sprintf(formatstring, string, val);
    sprintf(append, text, formatstring);


    char buf[MAX_SCROLLBACK];

    SendMessage(UartSimulationTextControl, WM_GETTEXT, (WPARAM)sizeof(buf),
        (LPARAM)buf);

    int overBy = (strlen(buf) + strlen(append) + 1) - sizeof(buf);
    if(overBy > 0) {
        memmove(buf, buf + overBy, strlen(buf));
    }
    strcat(buf, append);

    SendMessage(UartSimulationTextControl, WM_SETTEXT, 0, (LPARAM)buf);
    SendMessage(UartSimulationTextControl, EM_LINESCROLL, 0, (LPARAM)INT_MAX);
}

static void AppendToUSSSimulationTextControl(unsigned char id, unsigned int param, unsigned int index, char *name, unsigned int val)
{
    char append[125];

    sprintf(append, _("USS: id=%d, param=%d, index=%d, name=%s, value=%d\r\n"), id, param, index, name, val);

    char buf[MAX_SCROLLBACK];

    SendMessage(UartSimulationTextControl, WM_GETTEXT, (WPARAM)sizeof(buf),
        (LPARAM)buf);

    int overBy = (strlen(buf) + strlen(append) + 1) - sizeof(buf);
    if(overBy > 0) {
        memmove(buf, buf + overBy, strlen(buf));
    }
    strcat(buf, append);

    SendMessage(UartSimulationTextControl, WM_SETTEXT, 0, (LPARAM)buf);
    SendMessage(UartSimulationTextControl, EM_LINESCROLL, 0, (LPARAM)INT_MAX);
}

static void AppendToModbusSimulationTextControl(unsigned char id, unsigned int address, char *name)
{
    char append[125];

    sprintf(append, _("MODBUS: id=%d, address=%d, name=%s\r\n"), id, address, name);

    char buf[MAX_SCROLLBACK];

    SendMessage(UartSimulationTextControl, WM_GETTEXT, (WPARAM)sizeof(buf),
        (LPARAM)buf);

    int overBy = (strlen(buf) + strlen(append) + 1) - sizeof(buf);
    if(overBy > 0) {
        memmove(buf, buf + overBy, strlen(buf));
    }
    strcat(buf, append);

    SendMessage(UartSimulationTextControl, WM_SETTEXT, 0, (LPARAM)buf);
    SendMessage(UartSimulationTextControl, EM_LINESCROLL, 0, (LPARAM)INT_MAX);
}

static void AppendToModbusEthSimulationTextControl(unsigned char id, unsigned int address, char *name)
{
    char append[125];

    sprintf(append, _("MODBUS_ETH: id=%d, address=%d, name=%s\r\n"), id, address, name);

    char buf[MAX_SCROLLBACK];

    SendMessage(UartSimulationTextControl, WM_GETTEXT, (WPARAM)sizeof(buf),
        (LPARAM)buf);

    int overBy = (strlen(buf) + strlen(append) + 1) - sizeof(buf);
    if(overBy > 0) {
        memmove(buf, buf + overBy, strlen(buf));
    }
    strcat(buf, append);

    SendMessage(UartSimulationTextControl, WM_SETTEXT, 0, (LPARAM)buf);
    SendMessage(UartSimulationTextControl, EM_LINESCROLL, 0, (LPARAM)INT_MAX);
}
