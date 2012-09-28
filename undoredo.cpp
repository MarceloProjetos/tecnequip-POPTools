#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "poptools.h"

// Store a `deep copy' of the entire program before every change, in a
// circular buffer so that the first one scrolls out as soon as the buffer
// is full and we try to push another one.
#define MAX_LEVELS_UNDO 32
typedef struct ProgramStackTag {
    PlcProgram prog[MAX_LEVELS_UNDO];
    struct {
        int gx;
        int gy;
    } cursor[MAX_LEVELS_UNDO];
    int write;
    int count;
} ProgramStack;

static struct {
    ProgramStack undo;
    ProgramStack redo;
} Undo;

//-----------------------------------------------------------------------------
// Make a `deep copy' of a circuit. Used for making a copy of the program
// whenever we change it, for undo purposes. Fast enough that we shouldn't
// need to be smart.
//-----------------------------------------------------------------------------
static void *DeepCopy(int which, void *any)
{
    switch(which) {
        CASE_LEAF {
            ElemLeaf *l = AllocLeaf();
            memcpy(l, any, sizeof(*l));
            l->selectedState = SELECTED_NONE;
            return l;
        }
        case ELEM_SERIES_SUBCKT: {
            int i;
            ElemSubcktSeries *n = AllocSubcktSeries();
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            n->count = s->count;
            for(i = 0; i < s->count; i++) {
                n->contents[i].which = s->contents[i].which;
                n->contents[i].d.any = DeepCopy(s->contents[i].which, 
                    s->contents[i].d.any);
            }
            return n;
        }
        case ELEM_PARALLEL_SUBCKT: {
            int i;
            ElemSubcktParallel *n = AllocSubcktParallel();
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            n->count = p->count;
            for(i = 0; i < p->count; i++) {
                n->contents[i].which = p->contents[i].which;
                n->contents[i].d.any = DeepCopy(p->contents[i].which,
                    p->contents[i].d.any);
            }
            return n;
        }
        default:
            oops();
            break;
    }
}

//-----------------------------------------------------------------------------
// Empty out a ProgramStack data structure, either .undo or .redo: set the
// count to zero and free all the program copies in it.
//-----------------------------------------------------------------------------
static void EmptyProgramStack(ProgramStack *ps)
{
    while(ps->count > 0) {
        int a = (ps->write - 1);
        if(a < 0) a += MAX_LEVELS_UNDO;
        ps->write = a;
        (ps->count)--;

        int i;
        for(i = 0; i < ps->prog[ps->write].numRungs; i++) {
            FreeCircuit(ELEM_SERIES_SUBCKT, ps->prog[ps->write].rungs[i]);
        }
    }
}

//-----------------------------------------------------------------------------
// Push the current program onto a program stack. Can either make a deep or
// a shallow copy of the linked data structures.
//-----------------------------------------------------------------------------
static void PushProgramStack(ProgramStack *ps, BOOL deepCopy)
{
    if(ps->count == MAX_LEVELS_UNDO) {
        int i;
        for(i = 0; i < ps->prog[ps->write].numRungs; i++) {
            FreeCircuit(ELEM_SERIES_SUBCKT,
                ps->prog[ps->write].rungs[i]);
        }
    } else {
        (ps->count)++;
    }

    memcpy(&(ps->prog[ps->write]), &Prog, sizeof(Prog));
    if(deepCopy) {
        int i;
        for(i = 0; i < Prog.numRungs; i++) {
            ps->prog[ps->write].rungs[i] = (ElemSubcktSeries *)DeepCopy(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
        }
    }

    int gx, gy;
    if(FindSelected(&gx, &gy)) {
        ps->cursor[ps->write].gx = gx;
        ps->cursor[ps->write].gy = gy;
    } else {
        ps->cursor[ps->write].gx = -1;
        ps->cursor[ps->write].gy = -1;
    }

    int a = (ps->write + 1);
    if(a >= MAX_LEVELS_UNDO) a -= MAX_LEVELS_UNDO;
    ps->write = a;
}

//-----------------------------------------------------------------------------
// Pop a program stack onto the current program. Always does a shallow copy.
// Internal error if the stack was empty.
//-----------------------------------------------------------------------------
static void PopProgramStack(ProgramStack *ps)
{
    int a = (ps->write - 1);
    if(a < 0) a += MAX_LEVELS_UNDO;
    ps->write = a;
    (ps->count)--;

    memcpy(&Prog, &ps->prog[ps->write], sizeof(Prog));

    SelectedGxAfterNextPaint = ps->cursor[ps->write].gx;
    SelectedGyAfterNextPaint = ps->cursor[ps->write].gy;
}

//-----------------------------------------------------------------------------
// Push a copy of the PLC program onto the undo history, replacing (and
// freeing) the oldest one if necessary.
//-----------------------------------------------------------------------------
void UndoRemember(void)
{
    // can't redo after modifying the program
    EmptyProgramStack(&(Undo.redo));
    PushProgramStack(&(Undo.undo), TRUE);

    SetUndoEnabled(TRUE, FALSE);
}

//-----------------------------------------------------------------------------
// Discard previous state. It is useful when we had saved previous state but
//  no changes was made in Prog.settings.
//-----------------------------------------------------------------------------
void UndoForget(void)
{
	ProgramStack *ps = &(Undo.undo);
    int a = (ps->write - 1);
    if(a < 0) a += MAX_LEVELS_UNDO;
    ps->write = a;
    (ps->count)--;

    int i;
    for(i = 0; i < ps->prog[ps->write].numRungs; i++) {
        FreeCircuit(ELEM_SERIES_SUBCKT, ps->prog[ps->write].rungs[i]);
    }
}

//-----------------------------------------------------------------------------
// Pop the undo history one level, or do nothing if we're out of levels of
// undo. This means that we push the current program on the redo stack, and
// pop the undo stack onto the current program.
//-----------------------------------------------------------------------------
void UndoUndo(void)
{
    if(Undo.undo.count <= 0) return;

    PushProgramStack(&(Undo.redo), FALSE);

	ForgetEverything();

    PopProgramStack(&(Undo.undo));

	RemoveParallelStart(0, NULL);

    if(Undo.undo.count > 0) {
        SetUndoEnabled(TRUE, TRUE);
    } else {
        SetUndoEnabled(FALSE, TRUE);
    }
    RefreshControlsToSettings(); 
    RefreshScrollbars();
    InvalidateRect(MainWindow, NULL, FALSE);
}

//-----------------------------------------------------------------------------
// Redo an undone operation. Push the current program onto the undo stack,
// and pop the redo stack into the current program.
//-----------------------------------------------------------------------------
void UndoRedo(void)
{
    if(Undo.redo.count <= 0) return;

    PushProgramStack(&(Undo.undo), FALSE);

	ForgetEverything();

    PopProgramStack(&(Undo.redo));

	RemoveParallelStart(0, NULL);

    if(Undo.redo.count > 0) {
        SetUndoEnabled(TRUE, TRUE);
    } else {
        SetUndoEnabled(TRUE, FALSE);
    }
    RefreshControlsToSettings(); 
    RefreshScrollbars();
    InvalidateRect(MainWindow, NULL, FALSE);
}

//-----------------------------------------------------------------------------
// Empty out our undo history entirely, as when loading a new file.
//-----------------------------------------------------------------------------
void UndoFlush(void)
{
    EmptyProgramStack(&(Undo.undo));
    EmptyProgramStack(&(Undo.redo));
    SetUndoEnabled(FALSE, FALSE);
}

//-----------------------------------------------------------------------------
// Is it possible to undo some operation? The display code needs to do that,
// due to an ugly hack for handling too-long lines; the only thing that
// notices that easily is the display code, which will respond by undoing
// the last operation, presumably the one that added the long line.
//-----------------------------------------------------------------------------
BOOL CanUndo(void)
{
    return (Undo.undo.count > 0);
}

