#include "poptools.h"

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

// Not all options all available e.g. can't delete the only relay coil in
// a rung, can't insert two coils in series, etc. Keep track of what is
// allowed so we don't corrupt our program.
BOOL CanInsertEnd;
BOOL CanInsertOther;
BOOL CanInsertComment;

// Ladder logic program is laid out on a grid program; this matrix tells
// us which leaf element is in which box on the grid, which allows us
// to determine what element has just been selected when the user clicks
// on something, for example.
ElemLeaf *DisplayMatrix[DISPLAY_MATRIX_X_SIZE][DISPLAY_MATRIX_Y_SIZE];
int DisplayMatrixWhich[DISPLAY_MATRIX_X_SIZE][DISPLAY_MATRIX_Y_SIZE];
ElemLeaf *Selected;
int SelectedWhich;

ElemLeaf DisplayMatrixFiller;

// Cursor within the ladder logic program. The paint routines figure out
// where the cursor should go and calculate the coordinates (in pixels)
// of the rectangle that describes it; then BlinkCursor just has to XOR
// the requested rectangle at periodic intervals.
PlcCursor Cursor;

//-----------------------------------------------------------------------------
// Find the address in the DisplayMatrix of the selected leaf element. Set
// *gx and *gy if we succeed and return TRUE, else return FALSE.
//-----------------------------------------------------------------------------
BOOL FindSelected(int *gx, int *gy)
{
    return FALSE;
}

#define FAR_EXEC_ACTION(name) do \
{ \
	if(!_stricmp(name, search_text)) { \
		if(FAR_MODE_REPLACE(mode)) { \
			strcpy(name, new_text); \
		} \
		matches++; \
		SelectElement(x, y, SELECTED_LEFT); \
	} \
} while(0)

unsigned int FindAndReplace(char *search_text, char *new_text, int mode)
{
	int x, y, i;
	unsigned int matches=0;

	if(search_text == NULL || !strlen(search_text)) {
		if(!strlen(POPSettings.last_search_text)) {
			return 0;
		} else {
			search_text = POPSettings.last_search_text;
		}
	} else {
		strcpy(POPSettings.last_search_text, search_text);
	}

	if(new_text == NULL) {
		new_text = POPSettings.last_new_text;
	} else {
		strcpy(POPSettings.last_new_text, new_text);
	}

	if((mode == FAR_FIND_NEXT || mode == FAR_REPLACE_NEXT) && FindSelected(&x, &y)) {
		x++;
	} else {
		x = 0;
		y = 0;
	}

	for(; y<DISPLAY_MATRIX_Y_SIZE && (!matches || mode==FAR_REPLACE_ALL); y++, x=0) {
		for(; x<DISPLAY_MATRIX_X_SIZE && (!matches || mode==FAR_REPLACE_ALL); x++) {
			if(VALID_LEAF(DisplayMatrix[x][y])) {
				switch(DisplayMatrixWhich[x][y]) {
				case ELEM_CONTACTS:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.contacts.name);
					break;

				case ELEM_COIL:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.coil.name);
					break;

				case ELEM_TON:
				case ELEM_TOF:
				case ELEM_RTO:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.timer.name);
					break;

				case ELEM_CTD:
				case ELEM_CTU:
				case ELEM_CTC:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.counter.name);
					break;

				case ELEM_RES:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.reset.name);
					break;

				case ELEM_EQU:
				case ELEM_NEQ:
				case ELEM_GRT:
				case ELEM_GEQ:
				case ELEM_LES:
				case ELEM_LEQ:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.cmp.op1);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.cmp.op2);
					break;

				case ELEM_ADD:
				case ELEM_SUB:
				case ELEM_MUL:
				case ELEM_DIV:
				case ELEM_MOD:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.math.dest);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.math.op1);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.math.op2);
					break;

				case ELEM_SQRT:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.sqrt.src);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.sqrt.dest);
					break;

				case ELEM_RAND:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.rand.var);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.rand.min);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.rand.max);
					break;

				case ELEM_ABS:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.abs.src);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.abs.dest);
					break;

				case ELEM_MOVE:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.move.dest);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.move.src);
					break;

				case ELEM_SET_BIT:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.setBit.name);
					break;

				case ELEM_CHECK_BIT:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.checkBit.name);
					break;

				case ELEM_READ_ADC:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.readAdc.name);
					break;

				case ELEM_SET_DA:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.setDA.name);
					break;

				case ELEM_MULTISET_DA:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.multisetDA.name);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.multisetDA.name1);
					break;

				case ELEM_READ_ENC:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.readEnc.name);
					break;

				case ELEM_RESET_ENC:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.resetEnc.name);
					break;

				case ELEM_READ_USS:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.readUSS.name);
					break;

				case ELEM_WRITE_USS:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.writeUSS.name);
					break;

				case ELEM_READ_MODBUS:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.readModbus.name);
					break;

				case ELEM_WRITE_MODBUS:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.writeModbus.name);
					break;

				case ELEM_SET_PWM:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.setPwm.name);
					break;

				case ELEM_UART_SEND:
				case ELEM_UART_RECV:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.uart.name);
					break;

				case ELEM_SHIFT_REGISTER:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.shiftRegister.name);
					break;

				case ELEM_LOOK_UP_TABLE:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.lookUpTable.dest);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.lookUpTable.index);
					break;

				case ELEM_PIECEWISE_LINEAR:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.piecewiseLinear.dest);
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.piecewiseLinear.index);
					break;

				case ELEM_READ_FORMATTED_STRING:
				case ELEM_WRITE_FORMATTED_STRING:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.fmtdStr.var);
					break;

				case ELEM_READ_SERVO_YASKAWA:
				case ELEM_WRITE_SERVO_YASKAWA:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.servoYaskawa.var);
					break;

				case ELEM_FORMATTED_STRING:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.fmtdStr.var);
					break;

				case ELEM_PERSIST:
					FAR_EXEC_ACTION(DisplayMatrix[x][y]->d.persist.var);
					break;

				case ELEM_RTC:
				case ELEM_SHORT:
				case ELEM_OPEN:
				case ELEM_MASTER_RELAY:
				case ELEM_ONE_SHOT_RISING:
				case ELEM_ONE_SHOT_FALLING:
					break;
				}
			}
		}
	}

	// Program changed! Update internal references
	if(matches && FAR_MODE_REPLACE(mode)) {
		// There were matches when replacing all occurrences and the new name has IO_TYPE as pending...
		// Probably it is an inexistent variable so we can securely use the previous assignment for this new one.
		if(mode==FAR_REPLACE_ALL && GetTypeFromName(new_text)==IO_TYPE_PENDING) {
			for(i=0; i<IoSeenPreviouslyCount; i++) {
				if(!_stricmp(search_text, IoSeenPreviously[i].name)) {
					strcpy(IoSeenPreviously[i].name, new_text);
					break;
				}
			}
		}
		ProgramChanged();
	}

	return matches;
}

//-----------------------------------------------------------------------------
// Select the item in DisplayMatrix at (gx, gy), and then update everything
// to reflect that. In particular change the enabled state of the menus so
// that those items that do not apply are grayed, and scroll the element in
// to view.
//-----------------------------------------------------------------------------
void SelectElement(int gx, int gy, int state)
{
}

//-----------------------------------------------------------------------------
// Select the top left element of the program. Returns TRUE if it was able
// to do so, FALSE if not. The latter occurs given a completely empty
// program.
//-----------------------------------------------------------------------------
BOOL MoveCursorTopLeft(void)
{
    int i, j;
    // Let us first try to place it somewhere on-screen, so start at the
    // vertical scroll offset, not the very top (to avoid placing the
    // cursor in a position that would force us to scroll to put it in to
    // view.)
    for(i = 0; i < DISPLAY_MATRIX_X_SIZE; i++) {
        for(j = ScrollYOffset; 
            j < DISPLAY_MATRIX_Y_SIZE && j < (ScrollYOffset+16); j++)
        {
            if(VALID_LEAF(DisplayMatrix[i][j])) {
                SelectElement(i, j, SELECTED_LEFT);
                return TRUE;
            }
        }
    }

    // If that didn't work, then try anywhere on the diagram before giving
    // up entirely.
    for(i = 0; i < DISPLAY_MATRIX_X_SIZE; i++) {
        for(j = 0; j < 16; j++) {
            if(VALID_LEAF(DisplayMatrix[i][j])) {
                SelectElement(i, j, SELECTED_LEFT);
                return TRUE;
            }
        }
    }
    return FALSE;
}

#define DM_INSIDE_BOUNDS(x,y) ((x >= 0 && x < DISPLAY_MATRIX_X_SIZE) && (y >= 0 && y < DISPLAY_MATRIX_Y_SIZE))

ElemLeaf * FindSelectable(int *x, int *y, int Direction, ElemLeaf *Discard)
{
	int OffsetX = 0, OffsetY = 0;
	switch(Direction) {
	case SELECTED_LEFT:
		OffsetX = -1;
		OffsetY =  0;
		break;

	case SELECTED_RIGHT:
		OffsetX = +1;
		OffsetY =  0;
		break;

	case SELECTED_ABOVE:
		OffsetX =  0;
		OffsetY = -1;
		break;

	case SELECTED_BELOW:
		OffsetX =  0;
		OffsetY = +1;
		break;
	}

	while(DM_INSIDE_BOUNDS(*x, *y) && (!VALID_LEAF(DisplayMatrix[*x][*y]) || DisplayMatrix[*x][*y] == Discard)) {
		*x += OffsetX;
		*y += OffsetY;
	}

	if(DM_INSIDE_BOUNDS(*x, *y)) {
		return DisplayMatrix[*x][*y];
	} else {
		return NULL;
	}
}

#define SEL_DIR_HOR(SelectState) (SelectState == SELECTED_LEFT  || SelectState == SELECTED_RIGHT)
#define SEL_DIR_VER(SelectState) (SelectState == SELECTED_ABOVE || SelectState == SELECTED_BELOW)

//-----------------------------------------------------------------------------
// Move the cursor in response to a keyboard command (arrow keys). Basically
// we move the cursor within the currently selected element until we hit
// the edge (e.g. left edge for VK_LEFT), and then we see if we can select
// a new item that would let us keep going.
//-----------------------------------------------------------------------------
void MoveCursorKeyboard(int keyCode, BOOL shiftPressed)
{
	int i, j, *ij, SelectNewState = SELECTED_NONE, SelectState = SELECTED_NONE, SelectOffset = 0;
	int GoToTop = 0, GoToEdge = 0, SelectX = -1, SelectY = -1;

	if(!Selected || Selected->selectedState == SELECTED_NONE) {
        MoveCursorTopLeft();
        return;
    }

    switch(keyCode) {
		case VK_HOME:
			if(shiftPressed) {
				i = 0;
				j = 0;
				FindSelectable(&i, &j, SELECTED_RIGHT, NULL);
				SelectElement(i, j, SELECTED_LEFT);
				return;
			}

			GoToEdge = 1;
        case VK_LEFT:
			ij = &i;
			SelectState    = SELECTED_LEFT;
			SelectNewState = SELECTED_RIGHT;
			SelectOffset = -1;
            break;

		case VK_END:
			if(shiftPressed) {
				j = DISPLAY_MATRIX_Y_SIZE - 1;
				while(DisplayMatrix[0][j] == NULL)
					j--;

				i = DISPLAY_MATRIX_X_SIZE - 1;
				FindSelectable(&i, &j, SELECTED_LEFT , NULL);

				SelectElement(i, j, SELECTED_LEFT);
				return;
			}

			GoToEdge = 1;
		case VK_RIGHT:
			ij = &i;
			SelectState    = SELECTED_RIGHT;
			SelectNewState = SELECTED_LEFT;
			SelectOffset = +1;
            break;

        case VK_UP:
			ij = &j;
			SelectState    = SELECTED_ABOVE;
			SelectNewState = SELECTED_BELOW;
			SelectOffset = -1;
            break;

        case VK_DOWN:
			ij = &j;
			SelectState    = SELECTED_BELOW;
			SelectNewState = SELECTED_ABOVE;
			SelectOffset = +1;
            break;

		default:
			return;
	}

	if(Selected->selectedState != SelectState && !GoToEdge &&
		SelectedWhich != ELEM_PLACEHOLDER) {
		SelectElement(-1, -1, SelectState);
		return;
	}

	if(FindSelected(&i, &j)) {
		*ij += SelectOffset;

		while(DM_INSIDE_BOUNDS(i, j)) {
			// First situation: we are at a null position when searching on X axis. This means we have reached the end of a parallel circuit.
			// We should to go up and continue trying the normal sequence.
			if(DisplayMatrix[i][j] == NULL && SEL_DIR_HOR(SelectState)) {
				while(DM_INSIDE_BOUNDS(i, j) && DisplayMatrix[i][j] == NULL)
					j--;
				*ij -= SelectOffset;
			// Second situation: we are at a comment element and searching on Y axis.
			// We should to search for any element on lines below or above instead to look just the last one.
			} else if(SelectedWhich == ELEM_COMMENT && SEL_DIR_VER(SelectState)) {
				int x, y;

				x = DISPLAY_MATRIX_X_SIZE - 1;
				y = j;

				if(FindSelectable(&x, &y, SELECTED_LEFT , NULL) != NULL) {
					SelectX = x;
					SelectY = y;
					break;
				}
			// Third situation: we have found a new element. We will use it.
			} else if(VALID_LEAF(DisplayMatrix[i][j]) && DisplayMatrix[i][j] != Selected) {
				SelectX = i;
				SelectY = j;
				if(!GoToEdge)
					break;
			// Fourth situation: we was not able to find a new element and we are searching on Y axis.
			// We should to search for elements on X axis too!
			} else if(!VALID_LEAF(DisplayMatrix[i][j]) && SEL_DIR_VER(SelectState)) {
				int x, y;
				// We will search for elements on X axis. The found element should to be up to 5 positions to left or right
				// First we search to LEFT
				x = i;
				y = j;
				if(FindSelectable(&x, &y, SELECTED_LEFT , NULL) != NULL && abs(x-i)<5) {
					SelectX = x;
					SelectY = y;
					break;
				} else {
					// Then we search to RIGHT
					x = i;
					y = j;
					if(FindSelectable(&x, &y, SELECTED_RIGHT, NULL) != NULL && abs(x-i)<5) {
						SelectX = x;
						SelectY = y;
						break;
					}
				}
			}

			*ij += SelectOffset;
		}

		if(SelectX >= 0 && SelectY >= 0) {
			SelectElement(SelectX, SelectY, GoToEdge ? SelectState : SelectNewState);
		} else if(SelectState == SELECTED_BELOW && (ScrollYOffsetMax - ScrollYOffset < 3)) {
			// special case: scroll the end marker into view
			ScrollYOffset = ScrollYOffsetMax;
			RefreshScrollbars();
		}
	}
}

int FindRung(int gx, int gy)
{
	while(DM_INSIDE_BOUNDS(gx, gy) && DisplayMatrix[gx][gy] == NULL)
		gy--;

	if(DM_INSIDE_BOUNDS(gx, gy) && DisplayMatrix[gx][gy] == PADDING_IN_DISPLAY_MATRIX) {
		while(DM_INSIDE_BOUNDS(gx, gy) && DisplayMatrix[gx][gy] == PADDING_IN_DISPLAY_MATRIX)
			gx++;
		if(DisplayMatrix[gx][gy] == NULL) {
			gx--;
			while(DM_INSIDE_BOUNDS(gx, gy) && DisplayMatrix[gx][gy] == PADDING_IN_DISPLAY_MATRIX)
				gx--;
		}
	}

	return -1;
}

void ToggleBreakPoint(int y)
{
	int rung;

    y += FONT_HEIGHT/2;

    int gy = (y - Y_PADDING)/(POS_HEIGHT*FONT_HEIGHT);

    gy += ScrollYOffset;

	rung = FindRung(0, gy);
	if(rung >= 0 && (Prog.rungs[rung]->contents[0].which != ELEM_COMMENT || Prog.rungHasBreakPoint[rung]))
		Prog.rungHasBreakPoint[rung] = !Prog.rungHasBreakPoint[rung];
}

//-----------------------------------------------------------------------------
// Edit the element under the mouse cursor. This will typically be the
// selected element, since the first click would have selected it. If there
// is no element under the mouser cursor then do nothing; do not edit the
// selected element (which is elsewhere on screen), as that would be
// confusing.
//-----------------------------------------------------------------------------
bool EditElementMouseDoubleclick(int x, int y)
{
	bool changed = false;
    x += ScrollXOffset;

    y += FONT_HEIGHT/2;

    int gx = (x - X_PADDING)/(POS_WIDTH*FONT_WIDTH);
    int gy = (y - Y_PADDING)/(POS_HEIGHT*FONT_HEIGHT);

    gy += ScrollYOffset;

    if(InSimulationMode) {
        ElemLeaf *l = DisplayMatrix[gx][gy];
        if(l && DisplayMatrixWhich[gx][gy] == ELEM_CONTACTS) {
            char *name = l->d.contacts.name;
			if(GetTypeFromName(name) == IO_TYPE_DIG_INPUT || GetTypeFromName(name) == IO_TYPE_INTERNAL_FLAG) {
                SimulationToggleContact(name);
            } 
        } else if(l && DisplayMatrixWhich[gx][gy] == ELEM_READ_ADC) {
            ShowAnalogSliderPopup(l->d.readAdc.name);
        }
    }

	return changed;
}

//-----------------------------------------------------------------------------
// Move the cursor in response to a mouse click. If they clicked in a leaf
// element box then figure out which edge they're closest too (with appropriate
// fudge factors to make all edges equally easy to click on) and put the
// cursor there.
//-----------------------------------------------------------------------------
void MoveCursorMouseClick(int x, int y)
{
    x += ScrollXOffset;

    y += FONT_HEIGHT/2;

	y -= RibbonHeight;

    int gx0 = (x - X_PADDING)/(POS_WIDTH*FONT_WIDTH);
    int gy0 = (y - Y_PADDING)/(POS_HEIGHT*FONT_HEIGHT);

    int gx = gx0;
    int gy = gy0 + ScrollYOffset;

    if(VALID_LEAF(DisplayMatrix[gx][gy])) {
        int i, j;
        for(i = 0; i < DISPLAY_MATRIX_X_SIZE; i++) {
            for(j = 0; j < DISPLAY_MATRIX_Y_SIZE; j++) {
                if(DisplayMatrix[i][j])
                    DisplayMatrix[i][j]->selectedState = SELECTED_NONE;
            }
        }
        int dx = x - (gx0*POS_WIDTH*FONT_WIDTH + X_PADDING);
        int dy = y - (gy0*POS_HEIGHT*FONT_HEIGHT + Y_PADDING);

        int dtop = dy;
        int dbottom = POS_HEIGHT*FONT_HEIGHT - dy;
        int dleft = dx;
        int dright = POS_WIDTH*FONT_WIDTH - dx;

        int extra = 1;
        if(DisplayMatrixWhich[gx][gy] == ELEM_COMMENT) {
            dleft += gx*POS_WIDTH*FONT_WIDTH;
            dright += (ColsAvailable - gx - 1)*POS_WIDTH*FONT_WIDTH;
            extra = ColsAvailable;
        } else {
            if((gx > 0) && (DisplayMatrix[gx-1][gy] == DisplayMatrix[gx][gy])) {
                dleft += POS_WIDTH*FONT_WIDTH;
                extra = 2;
            }
            if((gx < (DISPLAY_MATRIX_X_SIZE-1)) && 
                (DisplayMatrix[gx+1][gy] == DisplayMatrix[gx][gy]))
            {
                dright += POS_WIDTH*FONT_WIDTH;
                extra = 2;
            }
        }

        int decideX = (dright - dleft);
        int decideY = (dtop - dbottom);

        decideY = decideY*3*extra;

        int state;
        if(abs(decideY) > abs(decideX)) {
            if(decideY > 0) {
                state = SELECTED_BELOW;
            } else {
                state = SELECTED_ABOVE;
            }
        } else {
            if(decideX > 0) {
                state = SELECTED_LEFT;
            } else {
                state = SELECTED_RIGHT;
            }
        }
        SelectElement(gx, gy, state);
	} else if (DisplayMatrix[gx][gy] && DisplayMatrix[gx][gy] == PADDING_IN_DISPLAY_MATRIX) {
		int offset = 1;
		while(gx+offset<DISPLAY_MATRIX_X_SIZE && DisplayMatrix[gx+offset][gy] &&
				DisplayMatrix[gx+offset][gy] == PADDING_IN_DISPLAY_MATRIX) {
			offset++;
		}
		if(VALID_LEAF(DisplayMatrix[gx+offset][gy])) {
			SelectElement(gx+offset, gy, SELECTED_LEFT);
		} else {
			offset = -1;
			while(gx+offset>0 && DisplayMatrix[gx+offset][gy] && DisplayMatrix[gx+offset][gy] == PADDING_IN_DISPLAY_MATRIX) {
				offset--;
			}
			if(VALID_LEAF(DisplayMatrix[gx+offset][gy])) {
				SelectElement(gx+offset, gy, SELECTED_RIGHT);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Place the cursor as near to the given point on the grid as possible. Used
// after deleting an element, for example.
//-----------------------------------------------------------------------------
void MoveCursorNear(int gx, int gy)
{
    int out = 0;

    for(out = 0; out < 8; out++) {
        if(gx - out >= 0) {
            if(VALID_LEAF(DisplayMatrix[gx-out][gy])) {
                SelectElement(gx-out, gy, SELECTED_RIGHT);
                return;
            }
        }
        if(gx + out < DISPLAY_MATRIX_X_SIZE) {
            if(VALID_LEAF(DisplayMatrix[gx+out][gy])) {
                SelectElement(gx+out, gy, SELECTED_LEFT);
                return;
            }
        }
        if(gy - out >= 0) {
            if(VALID_LEAF(DisplayMatrix[gx][gy-out])) {
                SelectElement(gx, gy-out, SELECTED_BELOW);
                return;
            }
        }
        if(gy + out < DISPLAY_MATRIX_Y_SIZE) {
            if(VALID_LEAF(DisplayMatrix[gx][gy+out])) {
                SelectElement(gx, gy+out, SELECTED_ABOVE);
                return;
            }
        }

        if(out == 1) {
            // Now see if we have a straight shot to the right; might be far
            // if we have to go up to a coil or other end of line element.
            int across;
            for(across = 1; gx+across < DISPLAY_MATRIX_X_SIZE; across++) {
                if(VALID_LEAF(DisplayMatrix[gx+across][gy])) {
                    SelectElement(gx+across, gy, SELECTED_LEFT);
                    return;
                }
                if(!DisplayMatrix[gx+across][gy]) break;
            }
        }
    }

    MoveCursorTopLeft();
}

//-----------------------------------------------------------------------------
// Negate the selected item, if this is meaningful.
//-----------------------------------------------------------------------------
bool NegateSelected(void)
{
    switch(SelectedWhich) {
        case ELEM_CONTACTS:
            Selected->d.contacts.negated = TRUE;
            break;

        case ELEM_COIL: {
            ElemCoil *c = &Selected->d.coil;
            c->negated = TRUE;
            c->resetOnly = FALSE;
            c->setOnly = FALSE;
            break;
        }
        default:
			return false;
            break;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Make the item selected normal: not negated, not set/reset only.
//-----------------------------------------------------------------------------
bool MakeNormalSelected(void)
{
    switch(SelectedWhich) {
        case ELEM_CONTACTS:
            Selected->d.contacts.negated = FALSE;
            break;

        case ELEM_COIL: {
            ElemCoil *c = &Selected->d.coil;
            c->negated = FALSE;
            c->setOnly = FALSE;
            c->resetOnly = FALSE;
            break;
        }
        default:
			return false;
            break;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Make the selected item set-only, if it is a coil.
//-----------------------------------------------------------------------------
bool MakeSetOnlySelected(void)
{
    if(SelectedWhich != ELEM_COIL) return false;

    ElemCoil *c = &Selected->d.coil;
    c->setOnly = TRUE;
    c->resetOnly = FALSE;
    c->negated = FALSE;

	return true;
}

//-----------------------------------------------------------------------------
// Make the selected item reset-only, if it is a coil.
//-----------------------------------------------------------------------------
bool MakeResetOnlySelected(void)
{
    if(SelectedWhich != ELEM_COIL) return false;

    ElemCoil *c = &Selected->d.coil;
    c->resetOnly = TRUE;
    c->setOnly = FALSE;
    c->negated = FALSE;

	return true;
}
