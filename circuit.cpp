#include "poptools.h"

static ElemSubcktSeries *LoadSeriesFromFile(FILE *f);

//-----------------------------------------------------------------------------
// Convenience routines for allocating frequently-used data structures.
//-----------------------------------------------------------------------------
ElemLeaf *AllocLeaf(void)
{
    return (ElemLeaf *)CheckMalloc(sizeof(ElemLeaf));
}
ElemSubcktSeries *AllocSubcktSeries(void)
{
    return (ElemSubcktSeries *)CheckMalloc(sizeof(ElemSubcktSeries));
}
ElemSubcktParallel *AllocSubcktParallel(void)
{
    return (ElemSubcktParallel *)CheckMalloc(sizeof(ElemSubcktParallel));
}

static BOOL CollapseUnnecessarySubckts(int which, void *any);

//-----------------------------------------------------------------------------
// Returns TRUE if the subcircuit contains any of the given instruction
// types (ELEM_....), else FALSE.
//-----------------------------------------------------------------------------
static BOOL ContainsWhich(int which, void *any, int seek1, int seek2, int seek3)
{
    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                if(ContainsWhich(p->contents[i].which, p->contents[i].d.any,
                    seek1, seek2, seek3))
                {
                    return TRUE;
                }
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                if(ContainsWhich(s->contents[i].which, s->contents[i].d.any,
                    seek1, seek2, seek3))
                {
                    return TRUE;
                }
            }
            break;
        }
        default:
            if(which == seek1 || which == seek2 || which == seek3) {
                return TRUE;
            }
            break;
    }
    return FALSE;
}

// Saved Rung
ElemSubcktSeries *SavedRung;

static void *CopyRungWorker(int &dest_which, void *any, int which, bool UsePasteCheck)
{
	void *ret = NULL;

	switch(which) {
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *dest, *s = (ElemSubcktSeries *)any;
            int i;
			dest = AllocSubcktSeries();
			dest->count = s->count;
            for(i = 0; i < s->count; i++) {
				dest->contents[i].d.any = CopyRungWorker(dest->contents[i].which, s->contents[i].d.any, s->contents[i].which, UsePasteCheck);
            }

			dest_which = ELEM_SERIES_SUBCKT;
			ret = dest;

			break;
        }
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *dest, *p = (ElemSubcktParallel *)any;
            int i;
			dest = AllocSubcktParallel();
			dest->count = p->count;
            for(i = 0; i < p->count; i++) {
				dest->contents[i].d.any = CopyRungWorker(dest->contents[i].which, p->contents[i].d.any, p->contents[i].which, UsePasteCheck);
            }

			dest_which = ELEM_PARALLEL_SUBCKT;
			ret = dest;

			break;
        }
        CASE_LEAF {
			MbNodeList *l;
			ElemLeaf *leaf = AllocLeaf();
			memcpy(leaf, any, sizeof(ElemLeaf));

			if(UsePasteCheck) {
				if(which == ELEM_READ_MODBUS) {
					l = MbNodeList_GetByNodeID(leaf->d.readModbus.elem);
					if(l == NULL) {
						l = MbNodeList_GetByIndex(0);
						if(l == NULL) {
							l = &Prog.settings.mb_list[MbNodeList_Create("Default")];
						}
						leaf->d.readModbus.elem = l->NodeID;
					}

					MbNodeList_AddRef(leaf->d.readModbus.elem);
				} else if(which == ELEM_WRITE_MODBUS) {
					l = MbNodeList_GetByNodeID(leaf->d.writeModbus.elem);
					if(l == NULL) {
						l = MbNodeList_GetByIndex(0);
						if(l == NULL) {
							l = &Prog.settings.mb_list[MbNodeList_Create("Default")];
						}
						leaf->d.writeModbus.elem = l->NodeID;
					}

					MbNodeList_AddRef(leaf->d.writeModbus.elem);
				}
			}

			dest_which = which;
			ret = leaf;

			break;
		}

        default:
            oops();
            break;
    }

	return ret;
}

// Copy current rung to memory
void CopyRung(ElemSubcktSeries *rung)
{
	int dummy;

	if(rung == NULL) return;

	if(SavedRung != NULL && SavedRung != rung) { // If not NULL nor the same, free rung
		FreeCircuit(ELEM_SERIES_SUBCKT, SavedRung);
	}

	SavedRung = (ElemSubcktSeries *)CopyRungWorker(dummy, rung, ELEM_SERIES_SUBCKT, SavedRung == rung);
}

// Paste a previously saved rung, adding it to an empty rung
bool PasteRung(bool after)
{
	int i;

	InsertRung(after);
	i = ladder.RungContainingSelected();

	if(SavedRung != NULL && i >= 0 && Prog.rungs[i]->count == 1 && Prog.rungs[i]->contents[0].which == ELEM_PLACEHOLDER) {
		FreeCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i]);

		Prog.rungs[i] = SavedRung;
		CopyRung(SavedRung);

		return true;
	}

	return false;
}

// Saved leaf
int SavedWhich = 0;
ElemLeaf *SavedLeaf = NULL;

// Copy currently selected item to memory
void CopyLeaf(ElemLeaf *leaf, int which)
{
    if(!leaf || leaf->selectedState == SELECTED_NONE) return;

	if(which != ELEM_PLACEHOLDER) {
		SavedWhich = which;
		if(SavedLeaf == NULL || SavedLeaf == leaf) { // If NULL or the same, allocate a leaf for copy
			SavedLeaf = AllocLeaf();
		}
		memcpy(SavedLeaf, leaf, sizeof(ElemLeaf));
	}
}

// Paste a previously saved leaf, adding it to cursor's position
bool PasteLeaf(void)
{
	MbNodeList *l;
	ElemLeaf *PasteLeaf;

	if(SavedLeaf != NULL /*&& CanInsert(SavedWhich)*/) {
		if(SavedWhich == ELEM_READ_MODBUS) {
			l = MbNodeList_GetByNodeID(SavedLeaf->d.readModbus.elem);
			if(l == NULL) {
				l = MbNodeList_GetByIndex(0);
				if(l == NULL) {
					l = &Prog.settings.mb_list[MbNodeList_Create("Default")];
				}
				SavedLeaf->d.readModbus.elem = l->NodeID;
			}

			MbNodeList_AddRef(SavedLeaf->d.readModbus.elem);
		} else if(SavedWhich == ELEM_WRITE_MODBUS) {
			l = MbNodeList_GetByNodeID(SavedLeaf->d.writeModbus.elem);
			if(l == NULL) {
				l = MbNodeList_GetByIndex(0);
				if(l == NULL) {
					l = &Prog.settings.mb_list[MbNodeList_Create("Default")];
				}
				SavedLeaf->d.writeModbus.elem = l->NodeID;
			}

			MbNodeList_AddRef(SavedLeaf->d.writeModbus.elem);
		}

		PasteLeaf = SavedLeaf;
		CopyLeaf(SavedLeaf, SavedWhich);
//	    AddLeaf(SavedWhich, PasteLeaf);

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Routines to allocate memory for a new circuit element (contact, coil, etc.)
// and insert it into the current program with AddLeaf. Fill in some default
// parameters, name etc. when we create the leaf; user can change them later.
//-----------------------------------------------------------------------------
bool AddParallelStart(void)
{
	return ladder.AddParallelStart();
}
bool AddComment(char *str)
{
	return ladder.AddElement(new LadderElemComment(&ladder));
}
bool AddContact(void)
{
	return ladder.AddElement(new LadderElemContact(&ladder));
}
bool AddCoil(void)
{
	return ladder.AddElement(new LadderElemCoil(&ladder));
}
bool AddTimer(int which)
{
	return ladder.AddElement(new LadderElemTimer(&ladder, which));
}
bool AddRTC(int which)
{
	return ladder.AddElement(new LadderElemRTC(&ladder));
}
bool AddEmpty(int which)
{
	switch(which) {
	case ELEM_OPEN:
	case ELEM_SHORT:
		return ladder.AddElement(new LadderElemOpenShort(&ladder, which));

	case ELEM_ONE_SHOT_RISING:
	case ELEM_ONE_SHOT_FALLING:
		return ladder.AddElement(new LadderElemOneShot(&ladder, which));
	}

	return false;
}
bool AddReset(void)
{
	return ladder.AddElement(new LadderElemReset(&ladder));
}
bool AddMasterRelay(void)
{
	return ladder.AddElement(new LadderElemMasterRelay(&ladder));
}
bool AddSetBit(void)
{
	return ladder.AddElement(new LadderElemSetBit(&ladder));
}
bool AddCheckBit(void)
{
	return ladder.AddElement(new LadderElemCheckBit(&ladder));
}
bool AddShiftRegister(void)
{
	return ladder.AddElement(new LadderElemShiftRegister(&ladder));
}
bool AddLookUpTable(void)
{
	return ladder.AddElement(new LadderElemLUT(&ladder));
}
bool AddPiecewiseLinear(void)
{
	return ladder.AddElement(new LadderElemPiecewise(&ladder));
}
bool AddMove(void)
{
	return ladder.AddElement(new LadderElemMove(&ladder));
}
bool AddSqrt(void)
{
	return ladder.AddElement(new LadderElemSqrt(&ladder));
}
bool AddMath(int which)
{
	return ladder.AddElement(new LadderElemMath(&ladder, which));
}
bool AddRand(void)
{
	return ladder.AddElement(new LadderElemRand(&ladder));
}
bool AddAbs(void)
{
	return ladder.AddElement(new LadderElemAbs(&ladder));
}
bool AddCmp(int which)
{
	return ladder.AddElement(new LadderElemCmp(&ladder, which));
}
bool AddCounter(int which)
{
	return ladder.AddElement(new LadderElemCounter(&ladder, which));
}
bool AddSetDA(void)
{
	return ladder.AddElement(new LadderElemSetDa(&ladder));
}
bool AddReadAdc(void)
{
	return ladder.AddElement(new LadderElemReadAdc(&ladder));
}
bool AddReadEnc(void)
{
	return ladder.AddElement(new LadderElemReadEnc(&ladder));
}
bool AddResetEnc(void)
{
	return ladder.AddElement(new LadderElemResetEnc(&ladder));
}
bool AddMultisetDA(void)
{
	return ladder.AddElement(new LadderElemMultisetDA(&ladder));
}
bool AddReadFormatString(void)
{
	return ladder.AddElement(new LadderElemFmtString(&ladder, ELEM_READ_FORMATTED_STRING));
}
bool AddWriteFormatString(void)
{
	return ladder.AddElement(new LadderElemFmtString(&ladder, ELEM_WRITE_FORMATTED_STRING));
}
bool AddReadServoYaskawa(void)
{
	return ladder.AddElement(new LadderElemYaskawa(&ladder, ELEM_READ_SERVO_YASKAWA));
}
bool AddWriteServoYaskawa(void)
{
	return ladder.AddElement(new LadderElemYaskawa(&ladder, ELEM_WRITE_SERVO_YASKAWA));
}
bool AddReadUSS(void)
{
	return ladder.AddElement(new LadderElemUSS(&ladder, ELEM_READ_USS));
}
bool AddWriteUSS(void)
{
	return ladder.AddElement(new LadderElemUSS(&ladder, ELEM_WRITE_USS));
}
bool AddReadModbus(void)
{
	return ladder.AddElement(new LadderElemModBUS(&ladder, ELEM_READ_MODBUS));
}
bool AddWriteModbus(void)
{
	return ladder.AddElement(new LadderElemModBUS(&ladder, ELEM_WRITE_MODBUS));
}
bool AddSetPwm(void)
{
	return ladder.AddElement(new LadderElemSetPWM(&ladder));
}
bool AddUart(int which)
{
	return ladder.AddElement(new LadderElemUART(&ladder, which));
}
bool AddPersist(void)
{
	return ladder.AddElement(new LadderElemPersist(&ladder));
}

//-----------------------------------------------------------------------------
// Any subcircuit containing only one element should be collapsed into its
// parent. Call ourselves recursively to do this on every child of the passed
// subcircuit. The passed subcircuit will not be collapsed itself, so that
// the rung subcircuit may contain only one element as a special case. Returns
// TRUE if it or a child made any changes, and for completeness must be
// called iteratively on the root till it doesn't do anything.
//-----------------------------------------------------------------------------
static BOOL CollapseUnnecessarySubckts(int which, void *any)
{
    BOOL modified = FALSE;

    ok();
    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
			if(s == NULL)
				break;
            for(i = 0; i < s->count; i++) {
                if(s->contents[i].which == ELEM_PARALLEL_SUBCKT) {
                    ElemSubcktParallel *p = s->contents[i].d.parallel;
                    if(p->count == 1) {
                        if(p->contents[0].which == ELEM_SERIES_SUBCKT) {
                            // merge the two series subcircuits
                            ElemSubcktSeries *s2 = p->contents[0].d.series;
                            int makeSpaces = s2->count - 1;
                            memmove(&s->contents[i+makeSpaces+1],
                                &s->contents[i+1],
                                (s->count - i - 1)*sizeof(s->contents[0]));
                            memcpy(&s->contents[i], &s2->contents[0],
                                (s2->count)*sizeof(s->contents[0]));
                            s->count += makeSpaces;
                            CheckFree(s2);
                        } else {
                            s->contents[i].which = p->contents[0].which;
                            s->contents[i].d.any = p->contents[0].d.any;
                        }
                        CheckFree(p);
                        modified = TRUE;
                    } else {
                        if(CollapseUnnecessarySubckts(ELEM_PARALLEL_SUBCKT,
                            s->contents[i].d.parallel))
                        {
                            modified = TRUE;
                        }
                    }
                }
                // else a leaf, not a problem
            }
            break;
        }
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                if(p->contents[i].which == ELEM_SERIES_SUBCKT) {
                    ElemSubcktSeries *s = p->contents[i].d.series;
                    if(s->count == 1) {
                        if(s->contents[0].which == ELEM_PARALLEL_SUBCKT) {
                            // merge the two parallel subcircuits
                            ElemSubcktParallel *p2 = s->contents[0].d.parallel;
                            int makeSpaces = p2->count - 1;
                            memmove(&p->contents[i+makeSpaces+1],
                                &p->contents[i+1],
                                (p->count - i - 1)*sizeof(p->contents[0]));
                            memcpy(&p->contents[i], &p2->contents[0],
                                (p2->count)*sizeof(p->contents[0]));
                            p->count += makeSpaces;
                            CheckFree(p2);
                        } else {
                            p->contents[i].which = s->contents[0].which;
                            p->contents[i].d.any = s->contents[0].d.any;
                        }
                        CheckFree(s);
                        modified = TRUE;
                    } else {
                        if(CollapseUnnecessarySubckts(ELEM_SERIES_SUBCKT,
                            p->contents[i].d.series))
                        {
                            modified = TRUE;
                        }
                    }
                }
                // else a leaf, not a problem
            }
            break;
        }
        
        default:
            oops();
            break;
    }
    ok();
    return modified;
}

//-----------------------------------------------------------------------------
// Delete the selected leaf element from the circuit. Just pull it out of the
// subcircuit that it was in before, and don't worry about creating
// subcircuits with only one element; we will clean that up later in a second
// pass. Returns TRUE if it or a child (calls self recursively) found and
// removed the element.
//-----------------------------------------------------------------------------
static BOOL DeleteSelectedFromSubckt(int which, void *any)
{   
    ok();
    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                if(s->contents[i].d.any == Selected) {
					if(s->contents[i].which == ELEM_READ_MODBUS) {
						MbNodeList_DelRef(s->contents[i].d.leaf->d.readModbus.elem);
					} else if(s->contents[i].which == ELEM_WRITE_MODBUS) {
						MbNodeList_DelRef(s->contents[i].d.leaf->d.writeModbus.elem);
					}

					ForgetFromGrid(s->contents[i].d.any);
                    CheckFree(s->contents[i].d.any);
                    memmove(&s->contents[i], &s->contents[i+1],
                        (s->count - i - 1)*sizeof(s->contents[0]));
                    (s->count)--;
                    return TRUE;
                }
                if(s->contents[i].which == ELEM_PARALLEL_SUBCKT) {
                    if(DeleteSelectedFromSubckt(ELEM_PARALLEL_SUBCKT,
                        s->contents[i].d.any))
                    {
                        return TRUE;
                    }
                }
            }
            break;
        }
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                if(p->contents[i].d.any == Selected) {
					if(p->contents[i].which == ELEM_READ_MODBUS) {
						MbNodeList_DelRef(p->contents[i].d.leaf->d.readModbus.elem);
					} else if(p->contents[i].which == ELEM_WRITE_MODBUS) {
						MbNodeList_DelRef(p->contents[i].d.leaf->d.writeModbus.elem);
					}

					ForgetFromGrid(p->contents[i].d.any);
                    CheckFree(p->contents[i].d.any);
                    memmove(&p->contents[i], &p->contents[i+1],
                        (p->count - i - 1)*sizeof(p->contents[0]));
                    (p->count)--;
                    return TRUE;
                }
                if(p->contents[i].which == ELEM_SERIES_SUBCKT) {
                    if(DeleteSelectedFromSubckt(ELEM_SERIES_SUBCKT,
                        p->contents[i].d.any))
                    {
                        return TRUE;
                    }
                }
            }
            break;
        }
        default:
            oops();
            break;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
// Delete the selected item from the program. Just call
// DeleteSelectedFromSubckt on every rung till we find it.
//-----------------------------------------------------------------------------
bool DeleteSelectedFromProgram(void)
{
	return ladder.DelElement();
}

//-----------------------------------------------------------------------------
// Free a circuit and all of its subcircuits. Calls self recursively to do
// so.
//-----------------------------------------------------------------------------
void FreeCircuit(int which, void *any)
{
    ok();
    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                FreeCircuit(s->contents[i].which, s->contents[i].d.any);
            }
            CheckFree(s);
            break;
        }
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                FreeCircuit(p->contents[i].which, p->contents[i].d.any);
            }
            CheckFree(p);
            break;
        }
        CASE_LEAF
            ForgetFromGrid(any);
            CheckFree(any);
            break;

        default:
            oops();
            break;
    }
    ok();
}

//-----------------------------------------------------------------------------
// Free the entire program.
//-----------------------------------------------------------------------------
void FreeEntireProgram(void)
{
    ForgetEverything();

    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        FreeCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    }

    Prog.numRungs = 0;
    Prog.settings.cycleTime = 10000;
    Prog.settings.mcuClock = 100000000;
    Prog.settings.baudRate = 9600;
	Prog.settings.UART = 0;
	Prog.settings.ModBUSID = 0;
    Prog.io.count = 0;
    Prog.mcu = NULL;

	memset(&Prog.settings.ip, 0, sizeof(Prog.settings.ip));
	memset(&Prog.settings.mask, 0, sizeof(Prog.settings.mask));
	memset(&Prog.settings.gw, 0, sizeof(Prog.settings.gw));
	memset(&Prog.settings.dns, 0, sizeof(Prog.settings.dns));
	memset(&Prog.settings.sntp, 0, sizeof(Prog.settings.sntp));

	memset(&Prog.rungHasBreakPoint, 0, sizeof(Prog.rungHasBreakPoint));

	strcpy(Prog.settings.InfoName, "");
	strcpy(Prog.settings.InfoDeveloper, "");
	strcpy(Prog.settings.InfoDescription, "");
	strcpy(Prog.settings.InfoFWVersion, "1.5");
	Prog.settings.InfoBuildNumber = 0;
	Prog.settings.InfoCompileDate = 0;
	Prog.settings.InfoProgramDate = 0;

	strcpy(Prog.settings.sntp, "br.pool.ntp.org");

	Prog.settings.gmt = 9;
	Prog.settings.dailysave = 0;

	Prog.settings.ip[0] = 192;
	Prog.settings.ip[1] = 168;
	Prog.settings.ip[2] = 0;
	Prog.settings.ip[3] = 254;

	Prog.settings.mask[0] = 255;
	Prog.settings.mask[1] = 255;
	Prog.settings.mask[2] = 255;
	Prog.settings.mask[3] = 0;

	Prog.settings.gw[0] = 192;
	Prog.settings.gw[1] = 168;
	Prog.settings.gw[2] = 0;
	Prog.settings.gw[3] = 1;

	Prog.settings.dns[0] = 192;
	Prog.settings.dns[1] = 168;
	Prog.settings.dns[2] = 0;
	Prog.settings.dns[3] = 1;

	Prog.settings.enc_inc_conv_mode = ENCODER_MODE_RAW;
	Prog.settings.perimeter = 400;
	Prog.settings.pulses = 1024;
	Prog.settings.factor = 1;
	Prog.settings.x4 = 1;

	Prog.settings.canSave = TRUE;

	Prog.settings.ssi_mode = 0;
	Prog.settings.ssi_size = 24;
	Prog.settings.enc_ssi_conv_mode = ENCODER_MODE_RAW;
	Prog.settings.ssi_perimeter = 400;
	Prog.settings.ssi_factor = 1;
	Prog.settings.ssi_size_bpr = 12;

	Prog.settings.ramp_abort_mode = RAMP_ABORT_LEAVE;

	Prog.settings.mb_list_size = 0;

	for(i = 0; i < MAX_IO; i++)
	{
		memset(Prog.io.assignment[i].name, 0, sizeof(Prog.io.assignment[i].name));
		Prog.io.assignment[i].bit = 0;
		Prog.io.assignment[i].pin = 0;
		Prog.io.assignment[i].type = 0;
	}

	for(i = 0; i < NUM_SUPPORTED_MCUS; i++) 
	{
		if(_stricmp(SupportedMcus[i].mcuName, DEFAULT_CPU)==0) 
		{
			Prog.mcu = &SupportedMcus[i];
			break;
		}
	}

    strcpy(CurrentCompileFile, "");
}

//-----------------------------------------------------------------------------
// Returns true if the given subcircuit contains the given leaf.
//-----------------------------------------------------------------------------
BOOL ContainsElem(int which, void *any, ElemLeaf *seek)
{
    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                if(ContainsElem(s->contents[i].which, s->contents[i].d.any,
                    seek))
                {
                    return TRUE;
                }
            }
            break;
        }
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                if(ContainsElem(p->contents[i].which, p->contents[i].d.any, 
                    seek))    
                {
                    return TRUE;
                }
            }
            break;
        }
        CASE_LEAF
            if(any == seek)
                return TRUE;
            break;

        default:
            oops();
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
// Delete the rung that contains the cursor.
//-----------------------------------------------------------------------------
bool DeleteSelectedRung(void)
{
	LadderContext context = ladder.getContext();
	if(!context.canDeleteRung) {
        Error(_("Cannot delete rung; program must have at least one rung."));
        return false;
    }

	ladder.DeleteRung(-1);

	return true;
}

//-----------------------------------------------------------------------------
// Allocate a new `empty' rung, with only a single relay coil at the end. All
// the UI code assumes that rungs always have a coil in them, so it would
// add a lot of nasty special cases to create rungs totally empty.
//-----------------------------------------------------------------------------
static ElemSubcktSeries *AllocEmptyRung(void)
{
    ElemSubcktSeries *s = AllocSubcktSeries();
    s->count = 1;
    s->contents[0].which = ELEM_PLACEHOLDER;
    ElemLeaf *l = AllocLeaf();
    s->contents[0].d.leaf = l;

    return s;
}

//-----------------------------------------------------------------------------
// Insert a rung either before or after the rung that contains the cursor.
//-----------------------------------------------------------------------------
bool InsertRung(bool afterCursor)
{
	ladder.NewRung(afterCursor);

	return true; // Sempre insere uma nova linha!
}

//-----------------------------------------------------------------------------
// When up is false: Swap the row containing the selected element with the one
// under it, or do nothing if the rung is the last in the program.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// When up is true: Swap the row containing the selected element with the one
// above it, or do nothing if the rung is the first in the program.
//-----------------------------------------------------------------------------
bool PushRung(bool up)
{
	return ladder.PushRung(-1, up);
}

//-----------------------------------------------------------------------------
// Start a new project. Give them one rung, with a coil (that they can
// never delete) and nothing else.
//-----------------------------------------------------------------------------
void NewProgram(void)
{
    UndoFlush();
    FreeEntireProgram();

    Prog.numRungs = 1;
    Prog.rungs[0] = AllocEmptyRung();

	ladder.ClearDiagram();
}

//-----------------------------------------------------------------------------
// Worker for ItemIsLastInCircuit. Basically we look for seek in the given
// circuit, trying to see whether it occupies the last position in a series
// subcircuit (which may be in a parallel subcircuit that is in the last
// position of a series subcircuit that may be in a parallel subcircuit that
// etc.)
//-----------------------------------------------------------------------------
static void LastInCircuit(int which, void *any, ElemLeaf *seek,
    BOOL *found, BOOL *andItemAfter)
{
    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                LastInCircuit(p->contents[i].which, p->contents[i].d.any, seek,
                    found, andItemAfter);
                if(*found) return;
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            LastInCircuit(s->contents[s->count-1].which,
                s->contents[s->count-1].d.any, seek, found, andItemAfter);
            break;
        }
        default:
            if(*found) *andItemAfter = TRUE;
            if(any == seek) *found = TRUE;
            break;
    }
}

//-----------------------------------------------------------------------------
// Is an item the last one in the circuit (i.e. does one of its terminals go
// to the rightmost bus bar)? We need to know this because that is the only
// circumstance in which it is okay to insert a coil, RES, etc. after
// something
//-----------------------------------------------------------------------------
BOOL ItemIsLastInCircuit(ElemLeaf *item)
{
    int i = ladder.RungContainingSelected();
    if(i < 0) return FALSE;

    BOOL found = FALSE;
    BOOL andItemAfter = FALSE;

    LastInCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i], item, &found,
        &andItemAfter);

    if(found) return !andItemAfter;
    return FALSE;
}

//-----------------------------------------------------------------------------
// Are either of the UART functions (send or recv) used? Need to know this
// to know whether we must receive their pins.
//-----------------------------------------------------------------------------
BOOL UartFunctionUsed(void)
{
    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        if(ContainsWhich(ELEM_SERIES_SUBCKT, Prog.rungs[i],
            ELEM_UART_RECV, ELEM_UART_SEND, ELEM_FORMATTED_STRING))
        {
            return TRUE;
        }
        if(ContainsWhich(ELEM_SERIES_SUBCKT, Prog.rungs[i],
            ELEM_READ_FORMATTED_STRING, ELEM_WRITE_FORMATTED_STRING, ELEM_FORMATTED_STRING))
        {
            return TRUE;
        }
        if(ContainsWhich(ELEM_SERIES_SUBCKT, Prog.rungs[i],
            ELEM_READ_SERVO_YASKAWA, ELEM_WRITE_SERVO_YASKAWA, ELEM_FORMATTED_STRING))
        {
            return TRUE;
        }
        if(ContainsWhich(ELEM_SERIES_SUBCKT, Prog.rungs[i],
            ELEM_READ_USS, ELEM_WRITE_USS, ELEM_FORMATTED_STRING))
        {
            return TRUE;
        }
        if(ContainsWhich(ELEM_SERIES_SUBCKT, Prog.rungs[i],
            ELEM_READ_MODBUS, ELEM_WRITE_MODBUS, ELEM_FORMATTED_STRING))
        {
            return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
// Is the PWM function used? Need to know this to know whether we must reserve
// the pin.
//-----------------------------------------------------------------------------
BOOL PwmFunctionUsed(void)
{
    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        if(ContainsWhich(ELEM_SERIES_SUBCKT, Prog.rungs[i], ELEM_SET_PWM, 
            -1, -1))
        {
            return TRUE;
        }
    }
    return FALSE;
}
