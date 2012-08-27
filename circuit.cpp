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
// Routines for modifying the circuit: add a particular element at a
// particular point, delete the selected element, etc. 
// Jonathan Westhues, Oct 2004
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ldmicro.h"

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

// The following element holds the start point where a parallel subcircuit should be inserted.
ElemLeaf *ParallelStart;

#define SUBCKT_STATUS_NOTFOUND 0
#define SUBCKT_STATUS_FIRST    1
#define SUBCKT_STATUS_INSIDE   2
#define SUBCKT_STATUS_LAST     3

struct InsertionPoint {
	int                 point;
	void               *elem;
	ElemSubcktSeries   *series;
	ElemSubcktParallel *parallel;
};

int ElemInSubcktSeries(int which, void *any, InsertionPoint *point)
{
	int i, status = SUBCKT_STATUS_NOTFOUND;

    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            for(i = 0; i < s->count; i++) {
                if(s->contents[i].d.any == point->elem) {
					point->point  = i;
					point->series = s;
                    break;
                }
                if(s->contents[i].which == ELEM_PARALLEL_SUBCKT) {
                    status = ElemInSubcktSeries(ELEM_PARALLEL_SUBCKT, s->contents[i].d.any, point);
					if(status != SUBCKT_STATUS_NOTFOUND) {
						if(point->series == NULL) {
							point->point  = i;
							point->series = s;
						}
						break;
					}
                }
            }
			if(point->series != NULL && status != SUBCKT_STATUS_INSIDE) {
				if(!i && (status != SUBCKT_STATUS_LAST) &&
					(point->elem != Selected ? 1 : Selected->selectedState == SELECTED_LEFT)) {
					status = SUBCKT_STATUS_FIRST;
				} else if(i == s->count - 1 && (status != SUBCKT_STATUS_FIRST) &&
					(point->elem != Selected ? 1 : Selected->selectedState == SELECTED_RIGHT)) {
					status = SUBCKT_STATUS_LAST;
				} else {
					status = SUBCKT_STATUS_INSIDE;
				}
			}
            break;
		}

		case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
			for(i=0; i < p->count; i++) {
				if(p->contents[i].d.any == point->elem) {
					status = SUBCKT_STATUS_FIRST;
                    break;
				}
				if(p->contents[i].which == ELEM_SERIES_SUBCKT) {
                    status = ElemInSubcktSeries(ELEM_SERIES_SUBCKT, p->contents[i].d.any, point);
					if(point->series != NULL) {
	                    break;
					}
				}
			}
			if(i == p->count) break;
			if(status != SUBCKT_STATUS_INSIDE) {
				point->series   = NULL;
				point->parallel = p;
			} else if(point->parallel == NULL) {
				point->parallel = p;
			}
			break;
		}
	}

	return status;
}

int SearchMatch(ElemSubcktSeries *series, int which, void *any, int direction)
{
	if(which != ELEM_PARALLEL_SUBCKT) return SUBCKT_STATUS_NOTFOUND;

	int i, status = SUBCKT_STATUS_NOTFOUND;
	ElemSubcktParallel *p = (ElemSubcktParallel *)any;

	for(i=0; i < p->count; i++) {
		if(p->contents[i].which == ELEM_SERIES_SUBCKT) {
			int index = (direction == SUBCKT_STATUS_FIRST ? 0 : p->contents[i].d.series->count-1);
			int newWhich = p->contents[i].d.series->contents[index].which;
			void *newAny = p->contents[i].d.series->contents[index].d.any;

			if(series == p->contents[i].d.series) {
				series = p->contents[i].d.series;
				status = SUBCKT_STATUS_INSIDE;
			} else if(newWhich == ELEM_PARALLEL_SUBCKT) {
				status = SearchMatch(series, newWhich, newAny, direction);
			}

			if(status == SUBCKT_STATUS_INSIDE) break;
		}
	}

	return status;
}

int RemoveParallelStart(int which, void *any)
{
	int i, ret = SUBCKT_STATUS_NOTFOUND;
	if(ParallelStart != NULL) {
		if(any == NULL) {
			InsertionPoint Point = { 0, ParallelStart, NULL, NULL };
			for(i=0; i < Prog.numRungs; i++) {
				if(ElemInSubcktSeries(ELEM_SERIES_SUBCKT, Prog.rungs[i], &Point) != SUBCKT_STATUS_NOTFOUND) {
					any = Prog.rungs[i];
					which = ELEM_SERIES_SUBCKT;
					break;
				}
			}
		}
		if(any == NULL) {
			CheckFree(ParallelStart);
			ParallelStart = NULL;
		} else {
			switch(which) {
				case ELEM_SERIES_SUBCKT: {
					ElemSubcktSeries *s = (ElemSubcktSeries *)any;
					for(i = 0; i < s->count; i++) {
						// ParallelStart will be always an element inside a Parallel subcircuit.
						if(s->contents[i].which == ELEM_PARALLEL_SUBCKT) {
							ret = RemoveParallelStart(ELEM_PARALLEL_SUBCKT, s->contents[i].d.any);
							if(ret != SUBCKT_STATUS_NOTFOUND)
								break;
						}
					}
					break;
				}
				case ELEM_PARALLEL_SUBCKT: {
					ElemSubcktParallel *p = (ElemSubcktParallel *)any;
					for(i = 0; i < p->count; i++) {
						if(p->contents[i].d.any == ParallelStart) {
							CheckFree(ParallelStart);
							memmove(&p->contents[i], &p->contents[i+1],
								(p->count - i - 1)*sizeof(p->contents[0]));
							(p->count)--;
							ParallelStart = NULL;
							ret = SUBCKT_STATUS_INSIDE;
							break;
						} else if(p->contents[i].which == ELEM_SERIES_SUBCKT) {
							ret = RemoveParallelStart(ELEM_SERIES_SUBCKT, p->contents[i].d.any);
							if(ret != SUBCKT_STATUS_NOTFOUND)
								break;
						}
					}
					break;
				}
			}
		}
	}
	return ret;
}

// Tries to insert a parallel subcircuit between ParallelStart and currently selected object
void InsertParallel(int newWhich, ElemLeaf *newElem)
{
	int i, CurrentRung, UseCurrentParallel = 1;
	InsertionPoint StartPoint = { 0, ParallelStart, NULL, NULL}, EndPoint = { 0, Selected, NULL, NULL};

	// First we should to remove last saved state because UndoRemember was called before to add this new elemment.
	// We don't want user going back to this state (with parallel start)!
	UndoForget();

	// Phase 1: check if ParallelStart and currently selected object are in the same subcircuit.
	for(i=0; i < Prog.numRungs; i++) {
		ElemInSubcktSeries(ELEM_SERIES_SUBCKT, Prog.rungs[i], &StartPoint);
		if(StartPoint.series != NULL) {
			// Start found, now we will search for End in the same subcircuit.
			ElemInSubcktSeries(ELEM_SERIES_SUBCKT, StartPoint.series, &EndPoint);
			if(EndPoint.series == NULL) { // End not found. Maybe start and end are swapped?
				ElemInSubcktSeries(ELEM_SERIES_SUBCKT, Prog.rungs[i], &EndPoint);
				if(EndPoint.series) { // End found in the same rung. Now we search for start in the same subcircuit
					StartPoint.point    = 0;
					StartPoint.series   = NULL;
					StartPoint.parallel = NULL;
					ElemInSubcktSeries(ELEM_SERIES_SUBCKT, EndPoint.series, &StartPoint);
				}
			}
			break;
		}
	}

	CurrentRung = i;

	// Phase 2: check if we can make a parallel between two selected points.
	if(StartPoint.series && EndPoint.series && StartPoint.series != EndPoint.series) {
		UseCurrentParallel = 0;
		if(StartPoint.parallel == EndPoint.parallel) {
			StartPoint.point  = 0;
			StartPoint.series = EndPoint.series;
		} else if(EndPoint.parallel && EndPoint.parallel->count == 2 &&
			EndPoint.parallel->contents[1].which == ELEM_PLACEHOLDER) { // Special condition
			StartPoint.point  = 0;
			StartPoint.series = EndPoint.series;
		} else {
			int PreviousWhich, NextWhich;
			void *Previous = NULL, *Next = NULL;

			if(Selected->selectedState == SELECTED_RIGHT) {
				// Next
				if(EndPoint.series->contents[EndPoint.point].d.any == EndPoint.elem && EndPoint.point < (EndPoint.series->count-1)) {
					Next      = EndPoint.series->contents[EndPoint.point+1].d.any;
					NextWhich = EndPoint.series->contents[EndPoint.point+1].which;
				} else if(EndPoint.series->contents[EndPoint.point].which == ELEM_PARALLEL_SUBCKT) {
					while(EndPoint.series->contents[EndPoint.series->count-1].d.any == EndPoint.parallel) {
						EndPoint.elem     = EndPoint.parallel;
						EndPoint.series   = NULL;
						EndPoint.parallel = NULL;
						ElemInSubcktSeries(ELEM_SERIES_SUBCKT, Prog.rungs[CurrentRung], &EndPoint);
					}
					Previous      = EndPoint.series->contents[EndPoint.point].d.any;
					PreviousWhich = ELEM_PARALLEL_SUBCKT;
				}
			} else if(Selected->selectedState == SELECTED_LEFT) {
				// Next
				if(EndPoint.series->contents[EndPoint.point].which == ELEM_PARALLEL_SUBCKT) {
					Next      = EndPoint.series->contents[EndPoint.point].d.any;
					NextWhich = EndPoint.series->contents[EndPoint.point].which;
				}
				// Previous
				if(EndPoint.point) {
					Previous      = EndPoint.series->contents[EndPoint.point-1].d.any;
					PreviousWhich = EndPoint.series->contents[EndPoint.point-1].which;
				}
			}

			if(Next && SearchMatch(StartPoint.series, NextWhich, Next, SUBCKT_STATUS_FIRST)) {
				if(Selected->selectedState == SELECTED_RIGHT && StartPoint.point) {
					StartPoint.point--;
				}
				EndPoint.point  = 0;
				EndPoint.series = StartPoint.series;
			} else if(Previous && SearchMatch(StartPoint.series, PreviousWhich, Previous, SUBCKT_STATUS_LAST)) {
				if(Selected->selectedState == SELECTED_RIGHT) {
					EndPoint.point    = StartPoint.series->count-1;
				} else {
					EndPoint.point  = StartPoint.series->count;
				}
				EndPoint.series = StartPoint.series;
			}
		}
	}

	// Phase 3: creates a series subcircuit containing all elements between start and end points.
	if(StartPoint.series && StartPoint.series == EndPoint.series) {
		if(StartPoint.point > EndPoint.point) {
			i = StartPoint.point;
			StartPoint.point = EndPoint.point;
			EndPoint.point = i;
		}
		if(!UseCurrentParallel || StartPoint.point != EndPoint.point) {
			if(Selected->selectedState == SELECTED_LEFT && EndPoint.point)
				EndPoint.point--;
			ElemSubcktSeries *s = AllocSubcktSeries();
			s->count = EndPoint.point - StartPoint.point + 1;
			memmove(s->contents, &StartPoint.series->contents[StartPoint.point], (s->count)*sizeof(s->contents[0]));
			memmove(&StartPoint.series->contents[StartPoint.point+1], &StartPoint.series->contents[StartPoint.point+s->count],
				(StartPoint.series->count - StartPoint.point - s->count)*sizeof(s->contents[0]));
			StartPoint.series->count -= s->count-1;

			// Phase 4: creates a new parallel subcircuit which will hold the previously created series
			//          subcircuit and the new element.
			ElemSubcktParallel *p = AllocSubcktParallel();
			p->count = 2;

			p->contents[0].which = ELEM_SERIES_SUBCKT;
			p->contents[0].d.series = s;
			p->contents[1].which = newWhich;
			p->contents[1].d.leaf = newElem;

			StartPoint.series->contents[StartPoint.point].which = ELEM_PARALLEL_SUBCKT;
			StartPoint.series->contents[StartPoint.point].d.parallel = p;
		} else {
            if(StartPoint.parallel->count >= (MAX_ELEMENTS_IN_SUBCKT-1)) {
                Error(_("Too many elements in subcircuit!"));
				CheckFree(newElem);
            } else {
				StartPoint.parallel->contents[StartPoint.parallel->count].which = newWhich;
				StartPoint.parallel->contents[StartPoint.parallel->count].d.leaf = newElem;
				StartPoint.parallel->count++;
			}
		}
	} else {
		Error(_("Impossível criar paralelo entre os pontos selecionados!"));
		CheckFree(newElem);
	}

	// Phase 5: free ParallelStart and collapse.
	RemoveParallelStart(ELEM_SERIES_SUBCKT, Prog.rungs[CurrentRung]);
	while(CollapseUnnecessarySubckts(ELEM_SERIES_SUBCKT, Prog.rungs[CurrentRung]));
}

//-----------------------------------------------------------------------------
// Routine that does the actual work of adding a leaf element to the left/
// right of or above/below the selected element. If we are adding left/right
// in a series circuit then it's easy; just increase length of that
// subcircuit and stick it in. Same goes for above/below in a parallel
// subcircuit. If we are adding above/below in a series circuit or left/right
// in a parallel circuit then we must create a new parallel (for series) or
// series (for parallel) subcircuit with 2 elements, one for the previously
// selected element and one for the new element. Calls itself recursively on
// all subcircuits. Returns TRUE if it or a child made the addition.
//-----------------------------------------------------------------------------
static BOOL AddLeafWorker(int which, void *any, int newWhich, ElemLeaf *newElem)
{
    int i;

    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            for(i = 0; i < s->count; i++) {
                if(s->contents[i].d.any == Selected) {
                    break;
                }
                if(s->contents[i].which == ELEM_PARALLEL_SUBCKT) {
                    if(AddLeafWorker(ELEM_PARALLEL_SUBCKT, s->contents[i].d.any,
                        newWhich, newElem))
                    {
                        return TRUE;
                    }
                }
            }
            if(i == s->count) break;
			if(ParallelStart != NULL) {
				InsertParallel(newWhich, newElem);
				return TRUE;
			}
            if(s->contents[i].which == ELEM_PLACEHOLDER) {
                // Special case--placeholders are replaced. They only appear
                // in the empty series subcircuit that I generate for them,
                // so there is no need to consider them anywhere but here.
                // If we copy instead of replacing then the DisplayMatrix
                // tables don't get all messed up.
                memcpy(s->contents[i].d.leaf, newElem, sizeof(ElemLeaf));
                s->contents[i].d.leaf->selectedState = SELECTED_LEFT;
                CheckFree(newElem);
                s->contents[i].which = newWhich;
                SelectedWhich = newWhich;
                return TRUE;
            }
            if(s->count >= (MAX_ELEMENTS_IN_SUBCKT-1)) {
                Error(_("Too many elements in subcircuit!"));
                return TRUE;
            }
            switch(Selected->selectedState) {
                case SELECTED_LEFT:
                    memmove(&s->contents[i+1], &s->contents[i],
                        (s->count - i)*sizeof(s->contents[0]));
                    s->contents[i].d.leaf = newElem;
                    s->contents[i].which = newWhich;
                    (s->count)++;
                    break;

                case SELECTED_RIGHT:
                    memmove(&s->contents[i+2], &s->contents[i+1],
                        (s->count - i - 1)*sizeof(s->contents[0]));
                    s->contents[i+1].d.leaf = newElem;
                    s->contents[i+1].which = newWhich;
                    (s->count)++;
                    break;

                case SELECTED_BELOW:
                case SELECTED_ABOVE: {
                    ElemSubcktParallel *p = AllocSubcktParallel();
                    p->count = 2;
    
                    int t;
                    t = (Selected->selectedState == SELECTED_ABOVE) ? 0 : 1;
                    p->contents[t].which = newWhich;
                    p->contents[t].d.leaf = newElem;
                    t = (Selected->selectedState == SELECTED_ABOVE) ? 1 : 0;
                    p->contents[t].which = s->contents[i].which;
                    p->contents[t].d.any = s->contents[i].d.any;
                    
                    s->contents[i].which = ELEM_PARALLEL_SUBCKT;
                    s->contents[i].d.parallel = p;
                    break;
                }
                default:
                    oops();
                    break;
            }
            return TRUE;
            break;
        }
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            for(i = 0; i < p->count; i++) {
                if(p->contents[i].d.any == Selected) {
                    break;
                }
                if(p->contents[i].which == ELEM_SERIES_SUBCKT) {
                    if(AddLeafWorker(ELEM_SERIES_SUBCKT, p->contents[i].d.any,
                        newWhich, newElem))
                    {
                        return TRUE;
                    }
                }
            }
            if(i == p->count) break;
			if(ParallelStart != NULL) {
				InsertParallel(newWhich, newElem);
				return TRUE;
			}
            if(p->count >= (MAX_ELEMENTS_IN_SUBCKT-1)) {
                Error(_("Too many elements in subcircuit!"));
                return TRUE;
            }
            switch(Selected->selectedState) {
                case SELECTED_ABOVE:
                    memmove(&p->contents[i+1], &p->contents[i],
                        (p->count - i)*sizeof(p->contents[0]));
                    p->contents[i].d.leaf = newElem;
                    p->contents[i].which = newWhich;
                    (p->count)++;
                    break;

                case SELECTED_BELOW:
                    memmove(&p->contents[i+2], &p->contents[i+1],
                        (p->count - i - 1)*sizeof(p->contents[0]));
                    p->contents[i+1].d.leaf = newElem;
                    p->contents[i+1].which = newWhich;
                    (p->count)++;
                    break;

                case SELECTED_LEFT:
                case SELECTED_RIGHT: {
                    ElemSubcktSeries *s = AllocSubcktSeries();
                    s->count = 2;
    
                    int t;
                    t = (Selected->selectedState == SELECTED_LEFT) ? 0 : 1;
                    s->contents[t].which = newWhich;
                    s->contents[t].d.leaf = newElem;
                    t = (Selected->selectedState == SELECTED_LEFT) ? 1 : 0;
                    s->contents[t].which = p->contents[i].which;
                    s->contents[t].d.any = p->contents[i].d.any;
                    
                    p->contents[i].which = ELEM_SERIES_SUBCKT;
                    p->contents[i].d.series = s;
                    break;
                }
                default:
                    oops();
                    break;
            }
            return TRUE;
            break;
        }
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// Add the specified leaf node in the position indicated by the cursor. We
// will search through the entire program using AddLeafWorker to find the
// insertion point, and AddLeafWorker will stick it in at the requested
// location and return TRUE. We return TRUE if it worked, else FALSE.
//-----------------------------------------------------------------------------
static BOOL AddLeaf(int newWhich, ElemLeaf *newElem)
{
    if(!Selected || Selected->selectedState == SELECTED_NONE) return FALSE;

    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        if(AddLeafWorker(ELEM_SERIES_SUBCKT, Prog.rungs[i], newWhich, newElem))
        {
            WhatCanWeDoFromCursorAndTopology();
            return TRUE;
        }
    }
    return FALSE;
}

bool CanInsert(int which)
{
    if(!Selected || Selected->selectedState == SELECTED_NONE) return false;

	switch(which) {
	case ELEM_COMMENT:
	    if(CanInsertComment) return true;
		break;

	case ELEM_PLACEHOLDER:
		if(ParallelStart != NULL || SelectedWhich == ELEM_PLACEHOLDER) {
			break;
		}
		if(Selected->selectedState != SELECTED_BELOW) {
			Selected->selectedState = SELECTED_BELOW;
		}
	case ELEM_CONTACTS:
	case ELEM_EQU:
	case ELEM_NEQ:
	case ELEM_GRT:
	case ELEM_GEQ:
	case ELEM_LES:
	case ELEM_LEQ:
	case ELEM_OPEN:
	case ELEM_SHORT:
	case ELEM_ONE_SHOT_RISING:
	case ELEM_ONE_SHOT_FALLING:
	case ELEM_CTU:
	case ELEM_CTD:
	case ELEM_RTO:
	case ELEM_TON:
	case ELEM_TOF:
	case ELEM_RTC:
	case ELEM_READ_FORMATTED_STRING:
	case ELEM_WRITE_FORMATTED_STRING:
	case ELEM_FORMATTED_STRING:
	case ELEM_READ_SERVO_YASKAWA:
	case ELEM_WRITE_SERVO_YASKAWA:
	case ELEM_SET_BIT:
	case ELEM_CHECK_BIT:
	case ELEM_READ_USS:
	case ELEM_WRITE_USS:
	case ELEM_READ_MODBUS:
	case ELEM_WRITE_MODBUS:
	case ELEM_READ_MODBUS_ETH:
	case ELEM_WRITE_MODBUS_ETH:
	case ELEM_UART_RECV:
	case ELEM_UART_SEND:
	    if(CanInsertOther) return true;
		break;

	case ELEM_CTC:
	case ELEM_RES:
	case ELEM_READ_ADC:
	case ELEM_SET_DA:
	case ELEM_READ_ENC:
	case ELEM_MULTISET_DA :
	case ELEM_RESET_ENC:
	case ELEM_SET_PWM:
	case ELEM_PERSIST:
	case ELEM_MOVE: 
	case ELEM_MASTER_RELAY:
	case ELEM_SHIFT_REGISTER:
	case ELEM_PIECEWISE_LINEAR:
	case ELEM_LOOK_UP_TABLE:
	case ELEM_COIL:
	case ELEM_DIV:
	case ELEM_MUL:
	case ELEM_SUB:
	case ELEM_ADD:
	    if(CanInsertEnd) return true;
		break;

	default:
		oops();
		break;
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
		SavedLeaf = AllocLeaf();
		memcpy(SavedLeaf, leaf, sizeof(ElemLeaf));
	}
}

// Paste a previously saved leaf, adding it to cursor's position
bool PasteLeaf(void)
{
	ElemLeaf *PasteLeaf;

	if(SavedLeaf != NULL && CanInsert(SavedWhich)) {
		PasteLeaf = SavedLeaf;
		CopyLeaf(SavedLeaf, SavedWhich);
	    AddLeaf(SavedWhich, PasteLeaf);
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
	ElemLeaf *elem;

	if(!CanInsert(ELEM_PLACEHOLDER)) return false;

	elem = AllocLeaf();
	AddLeaf(ELEM_PLACEHOLDER, elem);
	ParallelStart = elem;

	return true;
}
bool AddComment(char *str)
{
    if(!CanInsert(ELEM_COMMENT)) return false;

    ElemLeaf *c = AllocLeaf();
    strcpy(c->d.comment.str, str);

    AddLeaf(ELEM_COMMENT, c);

	return true;
}
bool AddContact(void)
{
    if(!CanInsert(ELEM_CONTACTS)) return false;

    ElemLeaf *c = AllocLeaf();
    strcpy(c->d.contacts.name, _("in"));
    c->d.contacts.negated = FALSE;
	c->d.contacts.type = IO_TYPE_DIG_INPUT;

    AddLeaf(ELEM_CONTACTS, c);

	return true;
}
bool AddCoil(void)
{
    if(!CanInsert(ELEM_COIL)) return false;

    ElemLeaf *c = AllocLeaf();
    strcpy(c->d.coil.name, _("out"));
    c->d.coil.negated = FALSE;
    c->d.coil.setOnly = FALSE;
    c->d.coil.resetOnly = FALSE;
	c->d.coil.bit = 0;
	c->d.coil.type = IO_TYPE_DIG_OUTPUT;

    AddLeaf(ELEM_COIL, c);

	return true;
}
bool AddTimer(int which)
{
    if(!CanInsert(which)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.timer.name, _("new"));
    t->d.timer.delay = 100000;

    AddLeaf(which, t);

	return true;
}
bool AddRTC(int which)
{
    if(!CanInsert(which)) return false;

	time_t rawtime;
	struct tm * t;

	time ( &rawtime );
	t = localtime ( &rawtime );

	t->tm_year += 1900;
	t->tm_mon++;
	t->tm_sec = t->tm_sec > 59 ? 59 : t->tm_sec;

    ElemLeaf *l = AllocLeaf();
	l->d.rtc.wday = 255;
	l->d.rtc.mday = 0;
	l->d.rtc.month = 0;
	l->d.rtc.year = 0;
	l->d.rtc.hour = t->tm_hour;
	l->d.rtc.minute = t->tm_min;
	l->d.rtc.second = t->tm_sec;

    AddLeaf(which, l);

	return true;
}
bool AddEmpty(int which)
{
    if(!CanInsert(which)) return false;

    ElemLeaf *t = AllocLeaf();
    AddLeaf(which, t);

	return true;
}
bool AddReset(void)
{
    if(!CanInsert(ELEM_RES)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.reset.name, _("new"));
    AddLeaf(ELEM_RES, t);

	return true;
}
bool AddMasterRelay(void)
{
    if(!CanInsert(ELEM_MASTER_RELAY)) return false;

    ElemLeaf *t = AllocLeaf();
    AddLeaf(ELEM_MASTER_RELAY, t);

	return true;
}
bool AddSetBit(void)
{
    if(!CanInsert(ELEM_SET_BIT)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.setBit.name, _("new"));
    AddLeaf(ELEM_SET_BIT, t);

	return true;
}
bool AddCheckBit(void)
{
    if(!CanInsert(ELEM_CHECK_BIT)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.checkBit.name, _("new"));
    AddLeaf(ELEM_CHECK_BIT, t);

	return true;
}
bool AddShiftRegister(void)
{
    if(!CanInsert(ELEM_SHIFT_REGISTER)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.shiftRegister.name, _("reg"));
    t->d.shiftRegister.stages = 7;
    AddLeaf(ELEM_SHIFT_REGISTER, t);

	return true;
}
bool AddLookUpTable(void)
{
    if(!CanInsert(ELEM_LOOK_UP_TABLE)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.lookUpTable.dest, _("dest"));
    strcpy(t->d.lookUpTable.index, _("index"));
    t->d.lookUpTable.count = 0;
    t->d.lookUpTable.editAsString = 0;
    AddLeaf(ELEM_LOOK_UP_TABLE, t);

	return true;
}
bool AddPiecewiseLinear(void)
{
    if(!CanInsert(ELEM_PIECEWISE_LINEAR)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.piecewiseLinear.dest, _("yvar"));
    strcpy(t->d.piecewiseLinear.index, _("xvar"));
    t->d.piecewiseLinear.count = 0;
    AddLeaf(ELEM_PIECEWISE_LINEAR, t);

	return true;
}
bool AddMove(void)
{
    if(!CanInsert(ELEM_MOVE)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.move.dest, _("dest"));
    strcpy(t->d.move.src, _("src"));
    AddLeaf(ELEM_MOVE, t);

	return true;
}
bool AddMath(int which)
{
    if(!CanInsert(which)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.math.dest, _("dest"));
    strcpy(t->d.math.op1, _("src"));
    strcpy(t->d.math.op2, "1");
    AddLeaf(which, t);

	return true;
}
bool AddCmp(int which)
{
    if(!CanInsert(which)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.cmp.op1, _("var"));
    strcpy(t->d.cmp.op2, "1");
    AddLeaf(which, t);

	return true;
}
bool AddCounter(int which)
{
    if(!CanInsert(which)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.counter.name, _("new"));
    t->d.counter.max = 0;
    AddLeaf(which, t);

	return true;
}
bool AddSetDA(void)
{
    if(!CanInsert(ELEM_SET_DA)) return false;

    ElemLeaf *t = AllocLeaf();

	strcpy(t->d.setDA.name, _("new"));
	t->d.setDA.mode = ELEM_SET_DA_MODE_RAW;

	AddLeaf(ELEM_SET_DA, t);

	return true;
}
bool AddReadAdc(void)
{
    if(!CanInsert(ELEM_READ_ADC)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.readAdc.name, _("new"));
    AddLeaf(ELEM_READ_ADC, t);

	return true;
}
bool AddReadEnc(void)
{
    if(!CanInsertEnd) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.readEnc.name, _("new"));
    AddLeaf(ELEM_READ_ENC, t);

	return true;
}
bool AddMultisetDA(void)
{
    if(!CanInsert(ELEM_MULTISET_DA)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.multisetDA.name, "600");
	strcpy(t->d.multisetDA.name1, "2047");
	t->d.multisetDA.time = 600;
	t->d.multisetDA.gaint = 10;
	t->d.multisetDA.gainr = 5;
	t->d.multisetDA.initval = DA_RESOLUTION - 1;
	t->d.multisetDA.linear = 1; // 1 linear, 0 curva
	t->d.multisetDA.forward = 1; // 1 avanço, 0 recuo
	t->d.multisetDA.speedup = 0; // ' aceleração, 0 desaceleração
	
    AddLeaf(ELEM_MULTISET_DA, t);

	return true;
}
bool AddReadFormatString(void)
{
    if(!CanInsert(ELEM_READ_FORMATTED_STRING)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.fmtdStr.var, _("var"));
    strcpy(t->d.fmtdStr.string, _("value: %d\\r\\n"));
    AddLeaf(ELEM_READ_FORMATTED_STRING, t);

	return true;
}
bool AddWriteFormatString(void)
{
    if(!CanInsert(ELEM_WRITE_FORMATTED_STRING)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.fmtdStr.var, _("var"));
    strcpy(t->d.fmtdStr.string, _("value: %d\\r\\n"));
    AddLeaf(ELEM_WRITE_FORMATTED_STRING, t);

	return true;
}
bool AddReadServoYaskawa(void)
{
    if(!CanInsert(ELEM_READ_SERVO_YASKAWA)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.servoYaskawa.id, "0");
    strcpy(t->d.servoYaskawa.var, _("var"));
    strcpy(t->d.servoYaskawa.string, _("0ZSET%d"));
    AddLeaf(ELEM_READ_SERVO_YASKAWA, t);

	return true;
}
bool AddWriteServoYaskawa(void)
{
    if(!CanInsert(ELEM_WRITE_SERVO_YASKAWA)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.servoYaskawa.id, "0");
    strcpy(t->d.servoYaskawa.var, _("var"));
    strcpy(t->d.servoYaskawa.string, _("value: %d"));
    AddLeaf(ELEM_WRITE_SERVO_YASKAWA, t);

	return true;
}
bool AddReadUSS(void)
{
    if(!CanInsert(ELEM_READ_USS)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.readUSS.name, _("new"));
    AddLeaf(ELEM_READ_USS, t);

	return true;
}
bool AddWriteUSS(void)
{
    if(!CanInsert(ELEM_WRITE_USS)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.writeUSS.name, _("new"));
    AddLeaf(ELEM_WRITE_USS, t);

	return true;
}
bool AddReadModbus(void)
{
    if(!CanInsert(ELEM_READ_MODBUS)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.readModbus.name, _("new"));
	t->d.readModbus.retransmitir = TRUE;
    AddLeaf(ELEM_READ_MODBUS, t);

	return true;
}
bool AddWriteModbus(void)
{
    if(!CanInsert(ELEM_WRITE_MODBUS)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.writeModbus.name, _("new"));
	t->d.writeModbus.retransmitir = TRUE;
    AddLeaf(ELEM_WRITE_MODBUS, t);

	return true;
}
bool AddReadModbusEth(void)
{
    if(!CanInsert(ELEM_READ_MODBUS_ETH)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.readModbusEth.name, _("new"));
    AddLeaf(ELEM_READ_MODBUS_ETH, t);

	return true;
}
bool AddWriteModbusEth(void)
{
    if(!CanInsert(ELEM_WRITE_MODBUS_ETH)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.writeModbusEth.name, _("new"));
    AddLeaf(ELEM_WRITE_MODBUS_ETH, t);

	return true;
}
bool AddSetPwm(void)
{
    if(!CanInsert(ELEM_SET_PWM)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.setPwm.name, _("duty_cycle"));
    t->d.setPwm.targetFreq = 1000;
    AddLeaf(ELEM_SET_PWM, t);

	return true;
}
bool AddUart(int which)
{
    if(!CanInsert(which)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.uart.name, _("char"));
    AddLeaf(which, t);

	return true;
}
bool AddPersist(void)
{
    if(!CanInsert(ELEM_PERSIST)) return false;

    ElemLeaf *t = AllocLeaf();
    strcpy(t->d.persist.var, _("saved"));
    AddLeaf(ELEM_PERSIST, t);

	return true;
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
    if(!Selected || Selected->selectedState == SELECTED_NONE) return false;
    int i = RungContainingSelected();
    if(i < 0) return false;

    if(Prog.rungs[i]->count == 1 && 
        Prog.rungs[i]->contents[0].which != ELEM_PARALLEL_SUBCKT)
    {
        Prog.rungs[i]->contents[0].which = ELEM_PLACEHOLDER;
        SelectedWhich = ELEM_PLACEHOLDER;
        Selected->selectedState = SELECTED_LEFT;
        WhatCanWeDoFromCursorAndTopology();
        return true;
    }

    int gx, gy;
    if(!FindSelected(&gx, &gy)) {
        gx = 0;
        gy = 0;
    }

    if(DeleteSelectedFromSubckt(ELEM_SERIES_SUBCKT, Prog.rungs[i])) {
        while(CollapseUnnecessarySubckts(ELEM_SERIES_SUBCKT, Prog.rungs[i]))
            ;
        WhatCanWeDoFromCursorAndTopology();
        MoveCursorNear(gx, gy);
        return true;
    }

	return false;
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
    Prog.cycleTime = 10000;
    Prog.mcuClock = 100000000;
    Prog.baudRate = 9600;
	Prog.UART = 0;
	Prog.ModBUSID = 0;
    Prog.io.count = 0;
    Prog.mcu = NULL;

	memset(&Prog.ip, 0, sizeof(Prog.ip));
	memset(&Prog.mask, 0, sizeof(Prog.mask));
	memset(&Prog.gw, 0, sizeof(Prog.gw));
	memset(&Prog.dns, 0, sizeof(Prog.dns));
	memset(&Prog.sntp, 0, sizeof(Prog.sntp));

	strncpy(Prog.sntp, "br.pool.ntp.org", sizeof(Prog.sntp));

	Prog.gmt = 9;
	Prog.dailysave = 0;

	Prog.ip[0] = 192;
	Prog.ip[1] = 168;
	Prog.ip[2] = 0;
	Prog.ip[3] = 254;

	Prog.mask[0] = 255;
	Prog.mask[1] = 255;
	Prog.mask[2] = 255;
	Prog.mask[3] = 0;

	Prog.gw[0] = 192;
	Prog.gw[1] = 168;
	Prog.gw[2] = 0;
	Prog.gw[3] = 1;

	Prog.dns[0] = 192;
	Prog.dns[1] = 168;
	Prog.dns[2] = 0;
	Prog.dns[3] = 1;

	Prog.diameter = 0;
	Prog.pulses = 0;
	Prog.factor = 0;
	Prog.x4 = 1;

	Prog.canSave = TRUE;

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
}

//-----------------------------------------------------------------------------
// Returns true if the given subcircuit contains the given leaf.
//-----------------------------------------------------------------------------
static BOOL ContainsElem(int which, void *any, ElemLeaf *seek)
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
// Use ContainsElem to find the rung containing the cursor.
//-----------------------------------------------------------------------------
int RungContainingSelected(void)
{
    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        if(ContainsElem(ELEM_SERIES_SUBCKT, Prog.rungs[i], Selected)) {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Delete the rung that contains the cursor.
//-----------------------------------------------------------------------------
bool DeleteSelectedRung(void)
{
    if(Prog.numRungs == 1) {
        Error(_("Cannot delete rung; program must have at least one rung."));
        return false;
    }

    int gx, gy;
    BOOL foundCursor;
    foundCursor = FindSelected(&gx, &gy);

    int i = RungContainingSelected();
    if(i < 0) return false;

    FreeCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    Prog.numRungs--;
    memmove(&Prog.rungs[i], &Prog.rungs[i+1], 
        (Prog.numRungs - i)*sizeof(Prog.rungs[0]));

    if(foundCursor) MoveCursorNear(gx, gy);

    WhatCanWeDoFromCursorAndTopology();

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
bool InsertRung(BOOL afterCursor)
{
    if(Prog.numRungs >= (MAX_RUNGS - 1)) {
        Error(_("Too many rungs!"));
        return false;
    }
    
    int i = RungContainingSelected();
    if(i < 0) return false;

    if(afterCursor) i++;
    memmove(&Prog.rungs[i+1], &Prog.rungs[i],
        (Prog.numRungs - i)*sizeof(Prog.rungs[0]));
    Prog.rungs[i] = AllocEmptyRung();

	Selected->selectedState = SELECTED_NONE;
	SelectedWhich = Prog.rungs[i]->contents->which;
	Selected = Prog.rungs[i]->contents->d.leaf;
	Selected->selectedState = SELECTED_LEFT;

	(Prog.numRungs)++;

    WhatCanWeDoFromCursorAndTopology();

	return true;
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
    int offset, i = RungContainingSelected();
    if(up ? i==0 : i == (Prog.numRungs-1)) return false;

	if(up) {
		offset = -1;
	} else {
		offset = +1;
	}

    ElemSubcktSeries *temp = Prog.rungs[i];
    Prog.rungs[i] = Prog.rungs[i+offset];
    Prog.rungs[i+offset] = temp;

    WhatCanWeDoFromCursorAndTopology();
    ScrollSelectedIntoViewAfterNextPaint = TRUE;

	return true;
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
    int i = RungContainingSelected();
    if(i < 0) return FALSE;

    BOOL found = FALSE;
    BOOL andItemAfter = FALSE;

    LastInCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i], item, &found,
        &andItemAfter);

    if(found) return !andItemAfter;
    return FALSE;
}

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
        if(ContainsWhich(ELEM_SERIES_SUBCKT, Prog.rungs[i],
            ELEM_READ_MODBUS_ETH, ELEM_WRITE_MODBUS_ETH, ELEM_FORMATTED_STRING))
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
