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
	if(Prog.ParallelStart != NULL) {
		if(any == NULL) {
			InsertionPoint Point = { 0, Prog.ParallelStart, NULL, NULL };
			for(i=0; i < Prog.numRungs; i++) {
				if(ElemInSubcktSeries(ELEM_SERIES_SUBCKT, Prog.rungs[i], &Point) != SUBCKT_STATUS_NOTFOUND) {
					any = Prog.rungs[i];
					which = ELEM_SERIES_SUBCKT;
					break;
				}
			}
		}
		if(any == NULL) {
			// Prog.ParallelStart was not found. We should to look for ELEM_PLACEHOLDER instead.
			for(i=0; i < Prog.numRungs; i++) {
				if(RemoveParallelStart(ELEM_SERIES_SUBCKT, Prog.rungs[i]) != SUBCKT_STATUS_NOTFOUND)
					break;
			}
		} else {
			switch(which) {
				case ELEM_SERIES_SUBCKT: {
					ElemSubcktSeries *s = (ElemSubcktSeries *)any;
					for(i = 0; i < s->count; i++) {
						// Prog.ParallelStart will be always an element inside a Parallel subcircuit.
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
						if(p->contents[i].which == ELEM_PLACEHOLDER) {
							CheckFree(p->contents[i].d.leaf);

							memmove(&p->contents[i], &p->contents[i+1],
								(p->count - i - 1)*sizeof(p->contents[0]));
							(p->count)--;

							if(Selected == Prog.ParallelStart) {
								Selected = p->contents[0].d.leaf;
								SelectedWhich = p->contents[0].which;
								Selected->selectedState = SELECTED_LEFT;
								WhatCanWeDoFromCursorAndTopology();
							}

							ret = SUBCKT_STATUS_INSIDE;
							Prog.ParallelStart = NULL;

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

// Tries to insert a parallel subcircuit between Prog.ParallelStart and currently selected object
void InsertParallel(int newWhich, ElemLeaf *newElem)
{
	int i, CurrentRung, UseCurrentParallel = 1;
	InsertionPoint StartPoint = { 0, Prog.ParallelStart, NULL, NULL}, EndPoint = { 0, Selected, NULL, NULL};

	// First we should to remove last saved state because UndoRemember was called before to add this new elemment.
	// We don't want user going back to this state (with parallel start)!
	UndoForget();

	// Phase 1: check if Prog.ParallelStart and currently selected object are in the same subcircuit.
	for(i=0; i < Prog.numRungs; i++) {
		ElemInSubcktSeries(ELEM_SERIES_SUBCKT, Prog.rungs[i], &StartPoint);
		if(StartPoint.series != NULL) {
			// Start found, now we will search for End in the same rung and stop the search.
			ElemInSubcktSeries(ELEM_SERIES_SUBCKT, Prog.rungs[i], &EndPoint);
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
	if(StartPoint.series && StartPoint.series == EndPoint.series && SelectedWhich!= ELEM_PLACEHOLDER) {
		if(StartPoint.point > EndPoint.point) {
			// Special condition: selectedstate is SELECTED_RIGHT
			if(Selected->selectedState == SELECTED_RIGHT) {
				EndPoint.point++;
				StartPoint.point--;
				UseCurrentParallel = 0;
			}

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

			int pos_series = 0;
			if(Selected->selectedState == SELECTED_ABOVE) {
				pos_series = 1;
			}

			p->contents[ pos_series].which = ELEM_SERIES_SUBCKT;
			p->contents[ pos_series].d.series = s;
			p->contents[!pos_series].which = newWhich;
			p->contents[!pos_series].d.leaf = newElem;

			StartPoint.series->contents[StartPoint.point].which = ELEM_PARALLEL_SUBCKT;
			StartPoint.series->contents[StartPoint.point].d.parallel = p;
		} else {
            if(StartPoint.parallel->count >= (MAX_ELEMENTS_IN_SUBCKT-1)) {
                Error(_("Too many elements in subcircuit!"));
				CheckFree(newElem);
            } else {
				int pos_series = StartPoint.parallel->count;
				if(Selected->selectedState == SELECTED_ABOVE) {
					pos_series = 0;
                    memmove(&StartPoint.parallel->contents[1], &StartPoint.parallel->contents[0],
						(StartPoint.parallel->count)*sizeof(StartPoint.parallel->contents[0]));
				}

				StartPoint.parallel->contents[pos_series].which = newWhich;
				StartPoint.parallel->contents[pos_series].d.leaf = newElem;
				StartPoint.parallel->count++;
			}
		}
	} else {
		Error(_("Impossível criar paralelo entre os pontos selecionados!"));
		CheckFree(newElem);
	}

	// Phase 5: free Prog.ParallelStart and collapse.
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
			if(Prog.ParallelStart != NULL) {
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
			if(Prog.ParallelStart != NULL) {
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

bool MoveCoilsOutsideWorker(ElemSubcktSeries *rung, ElemSubcktParallel *p, ElemSubcktSeries *CoilSeries, void **any)
{
	int i;
	bool NeedMove = false;

	for(i = 0; i < p->count; i++) {
		if(p->contents[i].which == ELEM_SERIES_SUBCKT) {
			ElemSubcktSeries *s = p->contents[i].d.series;
			switch(s->contents[s->count-1].which) {
			case ELEM_COIL:
				CoilSeries = s;
				if(*any != NULL) {
					NeedMove = true;
				}
				break;
			case ELEM_PARALLEL_SUBCKT:
				if(MoveCoilsOutsideWorker(rung, s->contents[s->count-1].d.parallel, CoilSeries, any)) {
					return true; // Coil moved, exiting...
				}
				break;
			default:
				if(CoilSeries != NULL) {
					NeedMove = true;
				} else {
					*any = s->contents[s->count-1].d.any;
				}
			}
		} else if(p->contents[i].which == ELEM_COIL) {
			; // do nothing for now...
		} else if(CoilSeries != NULL) {
			NeedMove = true;
		} else {
			*any = p->contents[i].d.any;
		}

		if(NeedMove) {
			// Found a coil inside parallel! Lets move it outside.
			if(rung->count >= (MAX_ELEMENTS_IN_SUBCKT-1)) {
				// What should we do now? I can't move coil outside!
				// For now, just keep it as is...
				Error(_("Too many elements in subcircuit!"));
			} else {
				rung->contents[rung->count].d.any = CoilSeries->contents[CoilSeries->count-1].d.any;
				rung->contents[rung->count].which = CoilSeries->contents[CoilSeries->count-1].which;
				rung->count++;
				CoilSeries->count--;
			}

			// Coil moved, end loop.
			break;
		}
	}

	return NeedMove;
}

// A coil was added. We should check if there is different elements ending in other subckts.
void MoveCoilsOutside(ElemSubcktSeries *s)
{
	if(s == NULL) s = Prog.rungs[0];

	if(s->contents[s->count-1].which == ELEM_PARALLEL_SUBCKT) {
		ElemSubcktParallel *p = s->contents[s->count-1].d.parallel;
		if(s->count == 1 && p->count == 2 &&
			p->contents[0].which == ELEM_COIL && p->contents[0].which == ELEM_COIL) {
				Error("Duas bobinas!");
		} else {
			void *any = NULL;
			MoveCoilsOutsideWorker(s, p, NULL, &any);
		}
	}
}

void AddPlaceHolderIfNoEOL(ElemSubcktSeries *s)
{
	if(s->count && s->contents[s->count-1].which == ELEM_PARALLEL_SUBCKT) {
		// The line ends in a parallel. Now we have to check if there is any EOL element inside it.
		BOOL HasEOL = ContainsWhich(ELEM_SERIES_SUBCKT, s, ELEM_CTC, ELEM_RES, ELEM_READ_ADC) ||
						ContainsWhich(ELEM_SERIES_SUBCKT, s, ELEM_SET_DA, ELEM_READ_ENC, ELEM_MULTISET_DA) ||
						ContainsWhich(ELEM_SERIES_SUBCKT, s, ELEM_RESET_ENC, ELEM_SET_PWM, ELEM_PERSIST) ||
						ContainsWhich(ELEM_SERIES_SUBCKT, s, ELEM_MOVE, ELEM_MASTER_RELAY, ELEM_SHIFT_REGISTER) ||
						ContainsWhich(ELEM_SERIES_SUBCKT, s, ELEM_PIECEWISE_LINEAR, ELEM_LOOK_UP_TABLE, ELEM_COIL) ||
						ContainsWhich(ELEM_SERIES_SUBCKT, s, ELEM_DIV, ELEM_MUL, ELEM_SUB) ||
						ContainsWhich(ELEM_SERIES_SUBCKT, s, ELEM_RAND, ELEM_MOD, 0) ||
						ContainsWhich(ELEM_SERIES_SUBCKT, s, ELEM_ADD, ELEM_SQRT, ELEM_ABS);

		if(!HasEOL) {
			s->contents[s->count].d.leaf = AllocLeaf();
			s->contents[s->count].which  = ELEM_PLACEHOLDER;
			s->count++;
		}
	}
}

void RemovePlaceHolderIfNoParallel(ElemSubcktSeries *s)
{
	if(s->count > 1 && s->contents[s->count-1].which == ELEM_PLACEHOLDER && s->contents[s->count-1].which != ELEM_PARALLEL_SUBCKT) {
		ForgetFromGrid(s->contents[s->count-1].d.any);
		CheckFree(s->contents[s->count-1].d.any);
		s->count--;
	}
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
			// If the last item is a parallel and there is no EOL element,
			// we should to add a placeholder as the last item so the user can
			// insert an EOL element for the whole circuit.
			AddPlaceHolderIfNoEOL(Prog.rungs[i]);

			// Now we need to check if there is a coil inside a parallel.
			// If yes, it should to be moved outside!
			if(newWhich == ELEM_COIL) {
//				MoveCoilsOutside(Prog.rungs[i]);
			}
			DummyPaint();
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
		if(Prog.ParallelStart != NULL || SelectedWhich == ELEM_PLACEHOLDER) {
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
	case ELEM_UART_RECV:
	case ELEM_UART_SEND:
	    if(CanInsertOther) return true;
		break;

	case ELEM_READ_MODBUS:
	case ELEM_WRITE_MODBUS:
		if(CanInsertOther && Prog.settings.mb_list_size) return true;
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
	case ELEM_MOD:
	case ELEM_MUL:
	case ELEM_SUB:
	case ELEM_ADD:
	case ELEM_SQRT:
	case ELEM_RAND:
	case ELEM_ABS:
	    if(CanInsertEnd) return true;
		break;

	default:
		oops();
		break;
	}

	return false;
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
	i = RungContainingSelected();

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

	if(SavedLeaf != NULL && CanInsert(SavedWhich)) {
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
	Prog.ParallelStart = elem;

	return true;
}
bool AddComment(char *str)
{
	return ladder.AddElement(new LadderElemComment);
}
bool AddContact(void)
{
	return ladder.AddElement(new LadderElemContact);
}
bool AddCoil(void)
{
	return ladder.AddElement(new LadderElemCoil);
}
bool AddTimer(int which)
{
	return ladder.AddElement(new LadderElemTimer(which));
}
bool AddRTC(int which)
{
	return ladder.AddElement(new LadderElemRTC);
}
bool AddEmpty(int which)
{
	switch(which) {
	case ELEM_OPEN:
	case ELEM_SHORT:
		return ladder.AddElement(new LadderElemOpenShort(which));

	case ELEM_ONE_SHOT_RISING:
	case ELEM_ONE_SHOT_FALLING:
		return ladder.AddElement(new LadderElemOneShot(which));
	}

	return false;
}
bool AddReset(void)
{
	return ladder.AddElement(new LadderElemReset);
}
bool AddMasterRelay(void)
{
	return ladder.AddElement(new LadderElemMasterRelay);
}
bool AddSetBit(void)
{
	return ladder.AddElement(new LadderElemSetBit);
}
bool AddCheckBit(void)
{
	return ladder.AddElement(new LadderElemCheckBit);
}
bool AddShiftRegister(void)
{
	return ladder.AddElement(new LadderElemShiftRegister);
}
bool AddLookUpTable(void)
{
	return ladder.AddElement(new LadderElemLUT);
}
bool AddPiecewiseLinear(void)
{
	return ladder.AddElement(new LadderElemPiecewise);
}
bool AddMove(void)
{
	return ladder.AddElement(new LadderElemMove);
}
bool AddSqrt(void)
{
	return ladder.AddElement(new LadderElemSqrt);
}
bool AddMath(int which)
{
	return ladder.AddElement(new LadderElemMath(which));
}
bool AddRand(void)
{
	return ladder.AddElement(new LadderElemRand);
}
bool AddAbs(void)
{
	return ladder.AddElement(new LadderElemAbs);
}
bool AddCmp(int which)
{
	return ladder.AddElement(new LadderElemCmp(which));
}
bool AddCounter(int which)
{
	return ladder.AddElement(new LadderElemCounter(which));
}
bool AddSetDA(void)
{
	return ladder.AddElement(new LadderElemSetDa);
}
bool AddReadAdc(void)
{
	return ladder.AddElement(new LadderElemReadAdc);
}
bool AddReadEnc(void)
{
	return ladder.AddElement(new LadderElemReadEnc);
}
bool AddResetEnc(void)
{
	return ladder.AddElement(new LadderElemResetEnc);
}
bool AddMultisetDA(void)
{
	return ladder.AddElement(new LadderElemMultisetDA);
}
bool AddReadFormatString(void)
{
	return ladder.AddElement(new LadderElemFmtString(ELEM_READ_FORMATTED_STRING));
}
bool AddWriteFormatString(void)
{
	return ladder.AddElement(new LadderElemFmtString(ELEM_WRITE_FORMATTED_STRING));
}
bool AddReadServoYaskawa(void)
{
	return ladder.AddElement(new LadderElemYaskawa(ELEM_READ_SERVO_YASKAWA));
}
bool AddWriteServoYaskawa(void)
{
	return ladder.AddElement(new LadderElemYaskawa(ELEM_WRITE_SERVO_YASKAWA));
}
bool AddReadUSS(void)
{
	return ladder.AddElement(new LadderElemUSS(ELEM_READ_USS));
}
bool AddWriteUSS(void)
{
	return ladder.AddElement(new LadderElemUSS(ELEM_WRITE_USS));
}
bool AddReadModbus(void)
{
	return ladder.AddElement(new LadderElemModBUS(ELEM_READ_MODBUS));
}
bool AddWriteModbus(void)
{
	return ladder.AddElement(new LadderElemModBUS(ELEM_WRITE_MODBUS));
}
bool AddSetPwm(void)
{
	return ladder.AddElement(new LadderElemSetPWM);
}
bool AddUart(int which)
{
	return ladder.AddElement(new LadderElemUART(which));
}
bool AddPersist(void)
{
	return ladder.AddElement(new LadderElemPersist);
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
	bool SelectedIsParallelStart = Prog.ParallelStart == Selected;
	if(!Selected || Selected->selectedState == SELECTED_NONE) return false;
    int i = RungContainingSelected();
    if(i < 0) return false;

	RemoveParallelStart(0, NULL);
	if(SelectedIsParallelStart) {
		return true;
	}

    if(Prog.rungs[i]->count == 1 && 
        Prog.rungs[i]->contents[0].which != ELEM_PARALLEL_SUBCKT)
    {
		if(Prog.rungs[i]->contents[0].which == ELEM_READ_MODBUS) {
			MbNodeList_DelRef(Prog.rungs[i]->contents[0].d.leaf->d.readModbus.elem);
		} else if(Prog.rungs[i]->contents[0].which == ELEM_WRITE_MODBUS) {
			MbNodeList_DelRef(Prog.rungs[i]->contents[0].d.leaf->d.writeModbus.elem);
		}

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

    if(SelectedWhich != ELEM_PLACEHOLDER && DeleteSelectedFromSubckt(ELEM_SERIES_SUBCKT, Prog.rungs[i])) {
		AddPlaceHolderIfNoEOL(Prog.rungs[i]);
		RemovePlaceHolderIfNoParallel(Prog.rungs[i]);
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

	RemoveParallelStart(0, NULL);

    int gx, gy;
    BOOL foundCursor;
    foundCursor = FindSelected(&gx, &gy);

    int i = RungContainingSelected();
    if(i < 0) return false;

    FreeCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    Prog.numRungs--;
    memmove(&Prog.rungs[i], &Prog.rungs[i+1], 
        (Prog.numRungs - i)*sizeof(Prog.rungs[0]));
	memmove(&Prog.rungHasBreakPoint[i], &Prog.rungHasBreakPoint[i+1], 
        (Prog.numRungs - i)*sizeof(Prog.rungHasBreakPoint[0]));

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
