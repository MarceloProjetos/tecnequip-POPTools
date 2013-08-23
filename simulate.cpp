#include "poptools.h"

#include <vector>
#include <map>

static vector<IntOp>::size_type IntCodeLen;
static vector<IntOp> vectorIntCode;

map<string, bool> SingleBitItems;
map<string, int > Variables;
map<string, int > AdcShadows;

SWORD DAShadow;

map<string, int > EncShadows;
map<string, int > ResetEncShadows;
map<string, int > readUSSShadows;
map<string, int > writeUSSShadows;
map<string, int > readModbusShadows;
map<string, int > writeModbusShadows;
map<string, int > readModbusEthShadows;
map<string, int > writeModbusEthShadows;

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
static vector<IntOp>::size_type IntPc;

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

int * GetValWP(const char *name, int *val)
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

void CheckWP(const char *name, int val)
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
		DescribeForIoList(wpval, ladder->getDetailsIO(name).type, desc);
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

void HardwareRegisters_Update(HardwareRegisters *hwr, const char *name, int val)
{
	if(name == NULL) return;

	mapDetails detailsIO = ladder->getDetailsIO(name);

	if(detailsIO.pin > 0) { // name has assignment
		unsigned int index = IoMap_GetIndex(detailsIO);
		bool         ismb  = IoMap_IsModBUS(detailsIO);

		// Now we have to update register associated to name
		switch(detailsIO.type) {
		case eType_DigInput:
			HardwareRegisters_SetBit(hwr, ismb ? HWR_REG_MODBUS : HWR_REG_INPUT,
				index, ismb ? detailsIO.bit : index, val);
			break;

		case eType_DigOutput:
			HardwareRegisters_SetBit(hwr, ismb ? HWR_REG_MODBUS : HWR_REG_OUTPUT,
				index, ismb ? detailsIO.bit : index, val);
			break;

		case eType_General:
			HardwareRegisters_SetValue(hwr, index, val);
			break;
		}
	}
}

static void SetSingleBit(const char *name, bool state);

void HardwareRegisters_Sync(HardwareRegisters *hwr)
{
	bool         ismb;
	string       name;
	mapDetails   detailsIO;
	unsigned int i, index, mask, count = ladder->getCountIO();
	if(hwr == NULL) return;

	hwr->Syncing = true;

	for(i = 0; i < count; i++) {
		name = ladder->getNameIObyIndex(i);
		detailsIO = ladder->getDetailsIO(name);
		if(detailsIO.pin > 0) {
			index = IoMap_GetIndex(detailsIO);
			ismb  = IoMap_IsModBUS(detailsIO);
			mask  = 1UL << detailsIO.bit;

			// Now we have to update register associated to name
			switch(detailsIO.type) {
			case eType_DigInput:
			case eType_DigOutput:
				if(ismb)
					SetSingleBit(name.c_str(), (hwreg.ModBUS[index] & mask) ? true : false);
				break;

			case eType_General:
				SetSimulationVariable(name.c_str(), hwreg.ModBUS[index]);
				break;
			}
		}
	}

	hwr->Syncing    = false;
	hwr->NeedUpdate = true;
}

// Update Hardware Registers, check WatchPoints, etc.
void UpdateSimulation(const char *name, int val)
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
static bool SingleBitOn(char *name)
{
	if(SingleBitItems.count(name) > 0) {
		return SingleBitItems[name];
	}

	return false;
}

//-----------------------------------------------------------------------------
// Set the state of a single-bit item. Adds it to the list if it is not there
// already.
//-----------------------------------------------------------------------------
static void SetSingleBit(const char *name, bool state)
{
	SingleBitItems[name] = state;
	UpdateSimulation(name, state);
}

//-----------------------------------------------------------------------------
// Count a timer up (i.e. increment its associated count by 1). Must already
// exist in the table.
//-----------------------------------------------------------------------------
static void IncrementVariable(char *name)
{
	if(Variables.count(name) == 0) {
		Variables[name] = 0;
	}

	Variables[name]++;
	UpdateSimulation(name, Variables[name]);
}

static void SetSimulationBit(char *name, int bit)
{
	if(Variables.count(name) == 0) {
		Variables[name] = 0;
	}

	Variables[name] |= 1 << bit;
	UpdateSimulation(name, Variables[name]);
	NeedRedraw = TRUE;
}

static void ClearSimulationBit(char *name, int bit)
{
	if(Variables.count(name) == 0) {
		Variables[name] = 0;
	}

	Variables[name] &= ~(1 << bit);
	UpdateSimulation(name, Variables[name]);
	NeedRedraw = TRUE;
}

static bool CheckSimulationBit(char *name, int bit)
{
	if(Variables.count(name) > 0) {
		return ((Variables[name] & (1 << bit)) > 0);
	}

	return false;
}

// Funcoes para ler e escrever em uma lista de variaveis
int getIntFromMap(map <string, int> *m, const char *name)
{
	if(m->count(name) > 0) {
		return (*m)[name];
	}

	return 0;
}

void setIntInMap(map <string, int> *m, const char *name, int val)
{
	(*m)[name] = val;
	UpdateSimulation(name, val);
	NeedRedraw = TRUE;
}

//-----------------------------------------------------------------------------
// Set a variable to a value.
//-----------------------------------------------------------------------------
void SetSimulationVariable(const char *name, SWORD val)
{
	setIntInMap(&Variables, name, val);
}

//-----------------------------------------------------------------------------
// Read a variable's value.
//-----------------------------------------------------------------------------
SWORD GetSimulationVariable(const char *name)
{
	return getIntFromMap(&Variables, name);
}

//-----------------------------------------------------------------------------
// Set the shadow copy of a variable associated with a READ ADC operation. This
// will get committed to the real copy when the rung-in condition to the
// READ ADC is true.
//-----------------------------------------------------------------------------
void SetAdcShadow(char *name, SWORD val)
{
	setIntInMap(&AdcShadows, name, val);
}

//-----------------------------------------------------------------------------
// Return the shadow value of a variable associated with a READ ADC. This is
// what gets copied into the real variable when an ADC read is simulated.
//-----------------------------------------------------------------------------
SWORD GetAdcShadow(char *name)
{
	return getIntFromMap(&AdcShadows, name);
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
	setIntInMap(&EncShadows, name, val);
}

//-----------------------------------------------------------------------------
// Return the shadow value of a variable associated with a READ ADC. This is
// what gets copied into the real variable when an ADC read is simulated.
//-----------------------------------------------------------------------------
SWORD GetEncShadow(char *name)
{
	return getIntFromMap(&EncShadows, name);
}

//-----------------------------------------------------------------------------
// Set the shadow copy of a variable associated with a READ ADC operation. This
// will get committed to the real copy when the rung-in condition to the
// READ ADC is true.
//-----------------------------------------------------------------------------
void SetResetEncShadow(char *name, SWORD val)
{
	setIntInMap(&ResetEncShadows, name, val);
}

//-----------------------------------------------------------------------------
// Return the shadow value of a variable associated with a READ ADC. This is
// what gets copied into the real variable when an ADC read is simulated.
//-----------------------------------------------------------------------------
SWORD GetResetEncShadow(char *name)
{
	return getIntFromMap(&ResetEncShadows, name);
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
    if(INT_ELSE_GROUP(vectorIntCode[IntPc].op)) {
        int nesting = 1;
        for(; ; IntPc++) {
            if(IntPc >= IntCodeLen) oops();

            if(vectorIntCode[IntPc].op == INT_END_IF) {
                nesting--;
            } else if(INT_IF_GROUP(vectorIntCode[IntPc].op) && vectorIntCode[IntPc-1].op != INT_ELSE_IF) {
                nesting++;
            }
            if(nesting == 0) break;
        }
    } else if(vectorIntCode[IntPc].op == INT_END_IF) {
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
	bool is_elseif = vectorIntCode[IntPc-1].op == INT_ELSE_IF;

	IntPc++;

	for(; ; IntPc++) {
        if(IntPc >= IntCodeLen) oops();

        if(vectorIntCode[IntPc].op == INT_END_IF) {
            nesting--;
        } else if(INT_IF_GROUP(vectorIntCode[IntPc].op)) {
			if(vectorIntCode[IntPc-1].op != INT_ELSE_IF) // when else if, no nesting!
				nesting++;
        } else if(INT_ELSE_GROUP(vectorIntCode[IntPc].op) && nesting == 1) {
            break;
        }
		if(nesting == 0) break;
    }

    // now PC is on the ELSE, ELSE IF or the END IF
    if(INT_ELSE_GROUP(vectorIntCode[IntPc].op)) {
        IntPc++;
        SimulateIntCode();
    } else if(vectorIntCode[IntPc].op == INT_END_IF) {
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
        IntOp *a = &vectorIntCode[IntPc];
        switch(a->op) {
            case INT_SIMULATE_NODE_STATE:
                if(*(a->poweredAfter) != SingleBitOn(a->name1)) {
                    NeedRedraw = TRUE;
	                *(a->poweredAfter) = SingleBitOn(a->name1);
				}
                break;

            case INT_SET_BIT:
                SetSingleBit(a->name1, true);
                break;

            case INT_CLEAR_BIT:
                SetSingleBit(a->name1, false);
                break;

            case INT_SET_SINGLE_BIT:
				SetSimulationBit(a->name1, a->bit);
                break;

            case INT_CLEAR_SINGLE_BIT:
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
                case INT_SET_VARIABLE_MODULO:
                    if(GetSimulationVariable(a->name3) != 0) {
                        v = GetSimulationVariable(a->name2) %
                            GetSimulationVariable(a->name3);
                    } else {
                        v = 0;
                        Error(_("Divisão por zero na variavel: '%s', a simulação será interrompida."), a->name3);
                        StopSimulation();
                    }
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
                SetSingleBit(a->name1, true);
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
					SetSingleBit("$RampActive", true);
				} else if(SingleBitOn("$RampActive")) {
					switch(a->literal) {
					case RAMP_ABORT_LEAVE:
						SimulateMultisetDA.Count = 0;
						SetSingleBit("$RampActive", false);
						break;
					case RAMP_ABORT_STOP:
						SimulateMultisetDA.Count = 1;  // Desacelera em  10 ms =  1 ciclo
						SimulateMultisetDA.FinalValue = DA_RESOLUTION;
						SetSingleBit("$RampActive", true);
						break;
					case RAMP_ABORT_DESACEL:
						SimulateMultisetDA.Count = 10; // Desacelera em 100 ms = 10 ciclos
						SimulateMultisetDA.FinalValue = DA_RESOLUTION;
						SetSingleBit("$RampActive", true);
						break;
					}
				}

				SimulateMultisetDA.StartCount = SimulateMultisetDA.Count;

				break;

			case INT_READ_FORMATTED_STRING:
				SetSingleBit("$SerialReady", false);
				SimulateUSSTxCountdown = 0;
				AppendToFormattedStringSimulationTextControl("RS485 Read: %s\r\n", a->name2, GetSimulationVariable(a->name1));
                break;
			case INT_WRITE_FORMATTED_STRING:
				SetSingleBit("$SerialReady", false);
				SimulateUSSTxCountdown = 0;
				AppendToFormattedStringSimulationTextControl("RS485 Write: %s\r\n", a->name2, GetSimulationVariable(a->name1));
                break;
			case INT_READ_SERVO_YASKAWA:
				SetSingleBit("$SerialReady", false);
				SimulateUSSTxCountdown = 0;
				AppendToFormattedStringSimulationTextControl("Servo Read: %s\r\n", a->name2, GetSimulationVariable(a->name1));
                break;
			case INT_WRITE_SERVO_YASKAWA:
				SetSingleBit("$SerialReady", false);
				SimulateUSSTxCountdown = 0;
				AppendToFormattedStringSimulationTextControl("Servo Write: %s\r\n", a->name2, GetSimulationVariable(a->name1));
                break;
            case INT_READ_USS:
				SetSingleBit("$USSReady", false);
				SimulateUSSTxCountdown = 0;
				AppendToUSSSimulationTextControl(atoi(a->name2), atoi(a->name3), atoi(a->name4), a->name1, GetSimulationVariable(a->name1));
                break;

            case INT_WRITE_USS:
				SetSingleBit("$USSReady", false);
				SimulateUSSTxCountdown = 0;
				AppendToUSSSimulationTextControl(atoi(a->name2), atoi(a->name3), atoi(a->name4), a->name1, GetSimulationVariable(a->name1));
                break;

            case INT_READ_MODBUS:
            case INT_WRITE_MODBUS:
				if(a->literal == 0) { // RS-485
					SetSingleBit("$SerialReady", false);
					SimulateModbus485Countdown = 2;
				} else { // Ethernet
					SetSingleBit("$TcpReady", false);
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
                    SetSingleBit(a->name2, false);
                } else {
                    SetSingleBit(a->name2, true);
                }
                break;

            case INT_UART_RECV:
                if(QueuedUartCharacter >= 0) {
                    SetSingleBit(a->name2, true);
                    SetSimulationVariable(a->name1, (SWORD)QueuedUartCharacter);
                    QueuedUartCharacter = -1;
                } else {
                    SetSingleBit(a->name2, false);
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

						SetSingleBit(a->name1, RTC_OutputState(RTC_StartTM, RTC_EndTM, t, RTC_Mode, a->literal & 0x7F) ? true : false);
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
		SetSingleBit("$SerialReady", true);
	}

	if (SimulateModbus485Countdown > 0) {
		SimulateModbus485Countdown--;
		if (!SimulateModbus485Countdown) {
			SetSingleBit("$SerialReady", true);
		}
	}

	if (SimulateModbusEthCountdown > 0) {
		SimulateModbusEthCountdown--;
		if (!SimulateModbusEthCountdown) {
			SetSingleBit("$TcpReady", true);
		}
	}

	if (SimulateMultisetDA.Count > 0)	{
		float offset;

		SimulateMultisetDA.Count--;
		if(!SimulateMultisetDA.Count) {
			SetSingleBit("$RampActive", false);
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
		ListView_RedrawItems(IoList, 0, ladder->getCountIO() - 1);
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
	LadderSettingsGeneral settings = ladder->getSettingsGeneral();
    int p = settings.cycleTime/1000;
    if(p < 5) {
        SetTimer(MainWindow, TIMER_SIMULATE, 10, PlcCycleTimer);
        CyclesPerTimerTick = 10000 / settings.cycleTime;
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
	Variables      .clear();
	AdcShadows     .clear();
	EncShadows     .clear();
	SingleBitItems .clear();
	ResetEncShadows.clear();

	QueuedUartCharacter = -1;
    QueuedUSSCharacter = -1;

	SimulateUartTxCountdown    = 0;
    SimulateUSSTxCountdown     = -1;
    SimulateModbus485Countdown = 0;
    SimulateModbusEthCountdown = 0;

	SetSingleBit("$USSReady", true);
	SetSingleBit("$SerialReady", true);
	SetSingleBit("$TcpReady", true);

// A checagem deve ser realizada na classe de mapa de I/O
// Deve ser criado codigo que permita reservar o I/O conforme descrito a seguir:
// Flags independentes que marquem o I/O para apenas uso exclusivo (1 único lugar)
// de leitura e escrita e variaveis para controle de numero de referencias para cada operacao
// Variaveis TON e TOF so podem ser usadas em seu proprio elemento
// Variaveis RTO e Contadores so podem ser lidas em seu elemento mas escritas quantas vezes quiser
//	CheckVariableNames();

    SimulateRedrawAfterNextCycle = TRUE;

    if(!GenerateIntermediateCode()) {
        ToggleSimulationMode();
        return;
    }

	vectorIntCode = ladder->getVectorIntCode();
	IntCodeLen = vectorIntCode.size();

	if(ladder->Validate() == eValidateResult_Error) {
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
void DescribeForIoList(int val, eType type, char *out)
{
    switch(type) {
        case eType_InternalFlag:
        case eType_InternalRelay:
        case eType_DigOutput:
        case eType_DigInput:
            sprintf(out, "%d", val);
            break;

		case eType_SetDAC: {
            sprintf(out, "%i (0x%08x)", val, val);
            break;
		}

        case eType_TON:
        case eType_TOF:
        case eType_RTO: {
			LadderSettingsGeneral settings = ladder->getSettingsGeneral();
            double dtms = val * (settings.cycleTime / 1000.0);
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

void DescribeForIoList(const char *name, eType type, char *out)
{
	int val;
	char varname[MAX_NAME_LEN];

	strcpy(varname, name);

	switch(type) {
		case eType_InternalFlag:
			sprintf(varname, "$%s", name);
		case eType_InternalRelay:
        case eType_DigOutput:
        case eType_DigInput:
            val = SingleBitOn(varname);
            break;

		case eType_SetDAC:
            val = GetDAShadow();
            break;

		case eType_TOF:
        case eType_TON:
        case eType_RTO:
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
    ListView_RedrawItems(IoList, 0, ladder->getCountIO() - 1);
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
