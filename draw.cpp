#include "poptools.h"

// Number of drawing columns (leaf element units) available. We want to
// know this so that we can right-justify the coils.
int ColsAvailable;

// Set when we draw the selected element in the program. If there is no
// element selected then we ought to put the cursor at the top left of
// the screen.
BOOL SelectionActive;

// Is the element currently being drawn highlighted because it is selected?
// If so we must not do further syntax highlighting.
BOOL ThisHighlighted;

#define DM_BOUNDS(gx, gy) { \
        if((gx) >= DISPLAY_MATRIX_X_SIZE || (gx) < 0) oops(); \
        if((gy) >= DISPLAY_MATRIX_Y_SIZE || (gy) < 0) oops(); \
    }

//-----------------------------------------------------------------------------
// The display code is the only part of the program that knows how wide a
// rung will be when it's displayed; so this is the only convenient place to
// warn the user and undo their changes if they created something too wide.
// This is not very clean.
//-----------------------------------------------------------------------------
static BOOL CheckBoundsUndoIfFails(int gx, int gy)
{
    if(gx >= DISPLAY_MATRIX_X_SIZE || gx < 0 ||
       gy >= DISPLAY_MATRIX_Y_SIZE || gy < 0)
    {
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
// Determine the width, in leaf element units, of a particular subcircuit.
// The width of a leaf is 1, the width of a series circuit is the sum of
// of the widths of its members, and the width of a parallel circuit is
// the maximum of the widths of its members.
//-----------------------------------------------------------------------------
static int CountWidthOfElement(int which, void *elem, int soFar)
{
    switch(which) {
        case ELEM_PLACEHOLDER:
        case ELEM_OPEN:
        case ELEM_SHORT:
        case ELEM_CONTACTS:
        case ELEM_TON:
        case ELEM_TOF:
        case ELEM_RTO:
		case ELEM_RTC:
        case ELEM_CTU:
        case ELEM_CTD:
        case ELEM_ONE_SHOT_RISING:
        case ELEM_ONE_SHOT_FALLING:
        case ELEM_EQU:
        case ELEM_NEQ:
        case ELEM_GRT:
        case ELEM_GEQ:
        case ELEM_LES:
        case ELEM_LEQ:
        case ELEM_UART_RECV:
        case ELEM_UART_SEND:
        case ELEM_READ_USS:
        case ELEM_WRITE_USS:
		case ELEM_SET_BIT:
		case ELEM_CHECK_BIT:
        case ELEM_READ_MODBUS:
        case ELEM_WRITE_MODBUS:
            return 1;

		case ELEM_READ_FORMATTED_STRING:
		case ELEM_WRITE_FORMATTED_STRING:
		case ELEM_READ_SERVO_YASKAWA:
		case ELEM_WRITE_SERVO_YASKAWA:
        case ELEM_FORMATTED_STRING:
            return 2;

        case ELEM_COMMENT: {
            if(soFar != 0) oops();

            ElemLeaf *l = (ElemLeaf *)elem;
            char tbuf[MAX_COMMENT_LEN];

            strcpy(tbuf, l->d.comment.str);
            char *b = strchr(tbuf, '\n');

            int len;
            if(b) {
                *b = '\0';
                len = max(strlen(tbuf)-1, strlen(b+1));
            } else {
                len = strlen(tbuf);
            }
            // round up, and allow space for lead-in
            len = (len + 7 + (POS_WIDTH-1)) / POS_WIDTH;
            return max(ColsAvailable, len);
        }
        case ELEM_CTC:
        case ELEM_RES:
        case ELEM_COIL:
        case ELEM_MOVE:
		case ELEM_SET_DA:
        case ELEM_READ_ADC:
        case ELEM_READ_ENC:
        case ELEM_RESET_ENC:
		case ELEM_MULTISET_DA:
        case ELEM_SHIFT_REGISTER:
        case ELEM_LOOK_UP_TABLE:
        case ELEM_PIECEWISE_LINEAR:
        case ELEM_MASTER_RELAY:
        case ELEM_SET_PWM:
        case ELEM_SQRT:
        case ELEM_ABS:
        case ELEM_PERSIST:
            if(ColsAvailable - soFar > 1) {
                return ColsAvailable - soFar;
            } else {
                return 1;
            }

        case ELEM_RAND:
        case ELEM_ADD:
        case ELEM_SUB:
        case ELEM_MUL:
        case ELEM_DIV:
        case ELEM_MOD:
            if(ColsAvailable - soFar > 2) {
                return ColsAvailable - soFar;
            } else {
                return 2;
            }

        case ELEM_SERIES_SUBCKT: {
            // total of the width of the members
            int total = 0;
            int i;
            ElemSubcktSeries *s = (ElemSubcktSeries *)elem;
            for(i = 0; i < s->count; i++) {
                total += CountWidthOfElement(s->contents[i].which,
                    s->contents[i].d.any, total+soFar);
            }
            return total;
        }

        case ELEM_PARALLEL_SUBCKT: {
            // greatest of the width of the members
            int max = 0;
            int i;
            ElemSubcktParallel *p = (ElemSubcktParallel *)elem;
            for(i = 0; i < p->count; i++) {
                int w = CountWidthOfElement(p->contents[i].which,
                    p->contents[i].d.any, soFar);
                if(w > max) {
                    max = w;
                }
            }
            return max;
        }

        default:
            oops();
    }
}

//-----------------------------------------------------------------------------
// Determine the height, in leaf element units, of a particular subcircuit.
// The height of a leaf is 1, the height of a parallel circuit is the sum of
// of the heights of its members, and the height of a series circuit is the
// maximum of the heights of its members. (This is the dual of the width
// case.)
//-----------------------------------------------------------------------------
int CountHeightOfElement(int which, void *elem)
{
    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            // total of the height of the members
            int total = 0;
            int i;
            ElemSubcktParallel *s = (ElemSubcktParallel *)elem;
            for(i = 0; i < s->count; i++) {
                total += CountHeightOfElement(s->contents[i].which,
                    s->contents[i].d.any);
            }
            return total;
        }

        case ELEM_SERIES_SUBCKT: {
            // greatest of the height of the members
            int max = 0;
            int i;
            ElemSubcktSeries *s = (ElemSubcktSeries *)elem;
            for(i = 0; i < s->count; i++) {
                int w = CountHeightOfElement(s->contents[i].which,
                    s->contents[i].d.any);
                if(w > max) {
                    max = w;
                }
            }
            return max;
        }

        default:
            oops();
    }
}

//-----------------------------------------------------------------------------
// Determine the width, in leaf element units, of the widest row of the PLC
// program (i.e. loop over all the rungs and find the widest).
//-----------------------------------------------------------------------------
int ProgCountWidestRow(void)
{
    int i;
    int max = 0;
    int colsTemp = ColsAvailable;
    ColsAvailable = 0;
    for(i = 0; i < Prog.numRungs; i++) {
        int w = CountWidthOfElement(ELEM_SERIES_SUBCKT, Prog.rungs[i], 0);
        if(w > max) {
            max = w;
        }
    }
    ColsAvailable = colsTemp;
    return max;
}

//-----------------------------------------------------------------------------
// Draw a vertical wire one leaf element unit high up from (cx, cy), where cx
// and cy are in charcter units.
//-----------------------------------------------------------------------------
static void VerticalWire(int cx, int cy)
{
    int j;
    for(j = 1; j < POS_HEIGHT; j++) {
        DrawChars(cx, cy + (POS_HEIGHT/2 - j), "|");
    }
    DrawChars(cx, cy + (POS_HEIGHT/2), "+");
    DrawChars(cx, cy + (POS_HEIGHT/2 - POS_HEIGHT), "+");
}

//-----------------------------------------------------------------------------
// Count the length of a string, in characters. Nonstandard because the
// string may contain special characters to indicate formatting (syntax
// highlighting).
//-----------------------------------------------------------------------------
int FormattedStrlen(const char *str)
{
    int l = 0;
    while(*str) {
        if(*str > 10) {
            l++;
        }
        str++;
    }
    return l;
}

//-----------------------------------------------------------------------------
// Draw a string, centred in the space of a single position, with spaces on
// the left and right. Draws on the upper line of the position.
//-----------------------------------------------------------------------------
void CenterWithSpaces(int cx, int cy, char *str, BOOL powered,
    BOOL isName, int extra)
{
    if(extra < 0) {
		extra = POS_WIDTH - FormattedStrlen(str);
	}

    DrawChars(cx + (extra/2), cy + (POS_HEIGHT/2) - 1, str);
}

void CenterWithSpaces(int cx, int cy, char *str, BOOL powered,
    BOOL isName)
{
     CenterWithSpaces(cx, cy, str, powered, isName, -1);
}

//-----------------------------------------------------------------------------
// Like CenterWithWires, but for an arbitrary width position (e.g. for ADD
// and SUB, which are double-width).
//-----------------------------------------------------------------------------
void CenterWithWiresWidth(int cx, int cy, char *str, BOOL before,
    BOOL after, int totalWidth)
{
    int extra = totalWidth - FormattedStrlen(str);

    DrawChars(cx + (extra/2), cy + (POS_HEIGHT/2), str);

    int i;
    for(i = 0; i < (extra/2); i++) {
        DrawChars(cx + i, cy + (POS_HEIGHT/2), "-");
    }
    for(i = FormattedStrlen(str)+(extra/2); i < totalWidth; i++) {
        DrawChars(cx + i, cy + (POS_HEIGHT/2), "-");
    }
}

//-----------------------------------------------------------------------------
// Draw a string, centred in the space of a single position, with en dashes on
// the left and right coloured according to the powered state. Draws on the
// middle line.
//-----------------------------------------------------------------------------
void CenterWithWires(int cx, int cy, char *str, BOOL before, BOOL after)
{
    CenterWithWiresWidth(cx, cy, str, before, after, POS_WIDTH);
}

//-----------------------------------------------------------------------------
// Draw an end of line element (coil, RES, MOV, etc.). Special things about
// an end of line element: we must right-justify it.
//-----------------------------------------------------------------------------
static BOOL DrawEndOfLine(int which, ElemLeaf *leaf, int *cx, int *cy,
    BOOL poweredBefore)
{
    int cx0 = *cx, cy0 = *cy;

    BOOL poweredAfter = leaf->poweredAfter;

    int thisWidth;
    switch(which) {
        case ELEM_RAND:
        case ELEM_ADD:
        case ELEM_SUB:
        case ELEM_MUL:
        case ELEM_DIV:
        case ELEM_MOD:
            thisWidth = 2;
            break;

        default:
            thisWidth = 1;
            break;
    }

    while(*cx < (ColsAvailable-thisWidth)*POS_WIDTH) {
        int gx = *cx/POS_WIDTH;
        int gy = *cy/POS_HEIGHT;

        if(CheckBoundsUndoIfFails(gx, gy)) return FALSE;

        if(gx >= DISPLAY_MATRIX_X_SIZE) oops();
        DM_BOUNDS(gx, gy);
        DisplayMatrix[gx][gy] = PADDING_IN_DISPLAY_MATRIX;
        DisplayMatrixWhich[gx][gy] = ELEM_PADDING;

        int i;
        for(i = 0; i < POS_WIDTH; i++) {
            DrawChars(*cx + i, *cy + (POS_HEIGHT/2), "-");
        }
        *cx += POS_WIDTH;
        cx0 += POS_WIDTH;
    }

    if(leaf == Selected && !InSimulationMode) {
        ThisHighlighted = TRUE;
    } else {
        ThisHighlighted = FALSE;
    }

    switch(which) {
        case ELEM_CTC: {
            char buf[256];
            ElemCounter *c = &leaf->d.counter;
            sprintf(buf, _("{\x01""CTC\x02 0:%d}"), c->max);

            CenterWithSpaces(*cx, *cy, c->name, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);
            break;
        }
        case ELEM_RES: {
            ElemReset *r = &leaf->d.reset;
            CenterWithSpaces(*cx, *cy, r->name, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, _("{RES}"), poweredBefore, poweredAfter);
            break;
        }
        case ELEM_READ_ADC: {
			char txt[50];
            ElemReadAdc *r = &leaf->d.readAdc;
            CenterWithSpaces(*cx, *cy, r->name, poweredAfter, TRUE);
			sprintf(txt, _("{READ ADC %s }"), GetPinADC(r->name));
            CenterWithWires(*cx, *cy, txt, poweredBefore, poweredAfter);
            break;
        }
        case ELEM_SET_DA: {
			char name[50];
            ElemSetDA *r = &leaf->d.setDA;

			switch(r->mode) {
			case ELEM_SET_DA_MODE_RAW:
				strcpy(name, r->name);
				break;

			case ELEM_SET_DA_MODE_MV:
				sprintf(name, "%s mV", r->name);
				break;

			case ELEM_SET_DA_MODE_PCT:
				sprintf(name, "%s %%", r->name);
				break;
			}

			CenterWithSpaces(*cx, *cy, name, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, _("{SET DA}"), poweredBefore,
                poweredAfter);
            break;
        }
        case ELEM_READ_ENC: {
            ElemReadEnc *r = &leaf->d.readEnc;
            CenterWithSpaces(*cx, *cy, r->name, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, _("{READ ENC}"), poweredBefore,
                poweredAfter);
            break;
        }
		case ELEM_MULTISET_DA : {
			ElemMultisetDA *r = &leaf->d.multisetDA;
			//CenterWithSpaces(*cx, *cy, r->forward ? "AVANCAR" : "RECUAR", poweredAfter, TRUE);
			CenterWithSpaces(*cx, *cy, r->linear ? _("RAMPA LINEAR") : _("RAMPA CURVA"), poweredAfter, TRUE);
			CenterWithWires(*cx, *cy, r->StartFromCurrentValue ? "{COINCIDIR}": (r->speedup ? _("{ACELERACAO}") : _("{DESACELERACAO}")), poweredBefore,
                poweredAfter);
            break;
		}
        case ELEM_RESET_ENC: {
            ElemResetEnc *r = &leaf->d.resetEnc;
            CenterWithSpaces(*cx, *cy, r->name, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, _("{WRITE ENC}"), poweredBefore, poweredAfter);
            break;
        }
        case ELEM_SET_PWM: {
            ElemSetPwm *s = &leaf->d.setPwm;
			 
            CenterWithSpaces(*cx, *cy, s->name, poweredAfter, TRUE);
            char l[50];
            if(s->targetFreq >= 100000) {
                sprintf(l, _("{PWM %d kHz}"), (s->targetFreq+500)/1000);
            } else if(s->targetFreq >= 10000) {
                sprintf(l, _("{PWM %.1f kHz}"), s->targetFreq/1000.0);
            } else if(s->targetFreq >= 1000) {
                sprintf(l, _("{PWM %.2f kHz}"), s->targetFreq/1000.0);
            } else {
                sprintf(l, _("{PWM %d Hz}"), s->targetFreq);
            }
            CenterWithWires(*cx, *cy, l, poweredBefore,
                poweredAfter);
            break;
        }
        case ELEM_PERSIST:
            CenterWithSpaces(*cx, *cy, leaf->d.persist.var, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, _("{PERSIST}"), poweredBefore, poweredAfter);
            break;

        case ELEM_MOVE: 
		{
            char top[256];
            char bot[256];
            ElemMove *m = &leaf->d.move;

			sprintf(top, "{%s :=}", m->dest);
			sprintf(bot, _("{%s MOV}"), m->src);

            CenterWithSpaces(*cx, *cy, top, poweredAfter, FALSE);
            CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);
            break;
        }
        case ELEM_MASTER_RELAY:
            CenterWithWires(*cx, *cy, _("{MASTER RLY}"), poweredBefore, poweredAfter);
            break;

        /*case ELEM_SET_BIT: {
            ElemSetBit *r = &leaf->d.setBit;
            CenterWithSpaces(*cx, *cy, r->name, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, "{SET BIT}", poweredBefore, poweredAfter);
            break;
        }*/

		case ELEM_SHIFT_REGISTER: {
            char bot[MAX_NAME_LEN+20];
            memset(bot, ' ', sizeof(bot));
            bot[0] = '{';
            sprintf(bot+2, "%s:%d", leaf->d.shiftRegister.name, leaf->d.shiftRegister.stages-1);
            bot[strlen(bot)] = ' ';
            bot[19] = '}';
            bot[20] = '\0';
            CenterWithSpaces(*cx, *cy, _("{\x01SHIFT REG\x02   }"), poweredAfter, FALSE);
            CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);
            break;
        }
        case ELEM_PIECEWISE_LINEAR:
        case ELEM_LOOK_UP_TABLE: {
            char top[MAX_NAME_LEN+20], bot[MAX_NAME_LEN+20];
            char *dest, *index, *str;
            if(which == ELEM_PIECEWISE_LINEAR) {
                dest = leaf->d.piecewiseLinear.dest;
                index = leaf->d.piecewiseLinear.index;
                str = _("PWL");
            } else {
                dest = leaf->d.lookUpTable.dest;
                index = leaf->d.lookUpTable.index;
                str = _("LUT");
            }
            memset(top, ' ', sizeof(top));
            top[0] = '{';
            sprintf(top+2, "%s :=", dest);
            top[strlen(top)] = ' ';
            top[19] = '}';
            top[20] = '\0';
            CenterWithSpaces(*cx, *cy, top, poweredAfter, FALSE);
            memset(bot, ' ', sizeof(bot));
            bot[0] = '{';
            sprintf(bot+2, " %s[%s]", str, index);
            bot[strlen(bot)] = ' ';
            bot[19] = '}';
            bot[20] = '\0';
            CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);
            break;
        }
        case ELEM_COIL: {
            char buf[4], name_with_type[MAX_NAME_LEN+4];
            ElemCoil *c = &leaf->d.coil;

            buf[0] = '(';
            if(c->negated) {
                buf[1] = '/';
            } else if(c->setOnly) {
                buf[1] = 'S';
            } else if(c->resetOnly) {
                buf[1] = 'R';
            } else {
                buf[1] = ' ';
            }
            buf[2] = ')';
            buf[3] = '\0';

			// TODO: Internacionalizar
			sprintf(name_with_type, "%s (%c)", c->name, c->type == IO_TYPE_DIG_OUTPUT ? 'S' : 'R');

			CenterWithSpaces(*cx, *cy, name_with_type, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);
            break;
        }
        case ELEM_MOD:
        case ELEM_DIV:
        case ELEM_MUL:
        case ELEM_SUB:
        case ELEM_ADD: {
            char top[POS_WIDTH*2-3+2+10];
            char bot[POS_WIDTH*2-3+10];

            memset(top, ' ', sizeof(top)-1);
            top[0] = '{';
			top[1] = '\x01';

            memset(bot, ' ', sizeof(bot)-1);
            bot[0] = '{';

            int lt = 2;
            if(which == ELEM_ADD) {
                memcpy(top+lt, _("ADD\x02"), 4);
            } else if(which == ELEM_SUB) {
                memcpy(top+lt, _("SUB\x02"), 4);
            } else if(which == ELEM_MUL) {
                memcpy(top+lt, _("MUL\x02"), 4);
            } else if(which == ELEM_DIV) {
                memcpy(top+lt, _("DIV\x02"), 4);
            } else if(which == ELEM_MOD) {
                memcpy(top+lt, _("MOD\x02"), 4);
            } else oops();

            lt += 6;
            memcpy(top+lt, leaf->d.math.dest, strlen(leaf->d.math.dest));
            lt += strlen(leaf->d.math.dest) + 2;
            top[lt++] = ':';
            top[lt++] = '=';

            int lb = 2;
            memcpy(bot+lb, leaf->d.math.op1, strlen(leaf->d.math.op1));
            lb += strlen(leaf->d.math.op1) + 1;
            if(which == ELEM_ADD) {
                bot[lb++] = '+';
            } else if(which == ELEM_SUB) {
                bot[lb++] = '-';
            } else if(which == ELEM_MUL) {
                bot[lb++] = '*';
            } else if(which == ELEM_DIV) {
                bot[lb++] = '/';
            } else if(which == ELEM_MOD) {
                bot[lb++] = '%';
            } else oops();
            lb++;
            memcpy(bot+lb, leaf->d.math.op2, strlen(leaf->d.math.op2));
            lb += strlen(leaf->d.math.op2);

            int l = max(lb, lt - 2);
            top[l+2] = '}'; top[l+3] = '\0';
            bot[l] = '}'; bot[l+1] = '\0';

            int extra = 2*POS_WIDTH - FormattedStrlen(top);
            DrawChars(*cx + (extra/2), *cy + (POS_HEIGHT/2) - 1, top);
            CenterWithWiresWidth(*cx, *cy, bot, poweredBefore, poweredAfter,
                2*POS_WIDTH);

            *cx += POS_WIDTH;

            break;
        }
        case ELEM_SQRT: 
		{
            char top[256];
            char bot[256];
            ElemSqrt *s = &leaf->d.sqrt;

			sprintf(top, "{%s :=}", s->dest);
			sprintf(bot, _("{%s SQRT}"), s->src);

            CenterWithSpaces(*cx, *cy, top, poweredAfter, FALSE);
            CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);
            break;
        }
        case ELEM_RAND: 
		{
            char top[256];
            char bot[256];
            ElemRand *s = &leaf->d.rand;

			sprintf(top, "{ %s := }", s->var);
			sprintf(bot, "{%s <= ? <= %s}", s->min, s->max);

            int extra = 2*POS_WIDTH - FormattedStrlen(top);
            DrawChars(*cx + (extra/2), *cy + (POS_HEIGHT/2) - 1, top);
            CenterWithWiresWidth(*cx, *cy, bot, poweredBefore, poweredAfter, 2*POS_WIDTH);
            *cx += POS_WIDTH;
            break;
        }
        case ELEM_ABS: 
		{
            char top[256];
            char bot[256];
            ElemAbs *s = &leaf->d.abs;

			sprintf(top, "{ %s :=}", s->dest);
			sprintf(bot, "{ |%s| }", s->src);

            CenterWithSpaces(*cx, *cy, top, poweredAfter, FALSE);
            CenterWithWires(*cx, *cy, bot, poweredBefore, poweredAfter);
            break;
        }
        default:
            oops();
            break;
    }

    *cx += POS_WIDTH;

    return poweredAfter;
}

//-----------------------------------------------------------------------------
// Draw a leaf element. Special things about a leaf: no need to recurse
// further, and we must put it into the display matrix.
//-----------------------------------------------------------------------------
static BOOL DrawLeaf(int which, ElemLeaf *leaf, int *cx, int *cy,
    BOOL poweredBefore)
{return TRUE;/*
	char ch;
    int cx0 = *cx, cy0 = *cy;
    BOOL poweredAfter = leaf->poweredAfter;

    switch(which) {
        case ELEM_COMMENT: {
            char tbuf[MAX_COMMENT_LEN];
            char tlbuf[MAX_COMMENT_LEN+8];

            strcpy(tbuf, leaf->d.comment.str);
            char *b = strchr(tbuf, '\n');

            if(b) {
                if(b[-1] == '\r') b[-1] = '\0';
                *b = '\0';
                sprintf(tlbuf, "\x03 ; %s\x02", tbuf);
                DrawChars(*cx, *cy + (POS_HEIGHT/2) - 1, tlbuf);
                sprintf(tlbuf, "\x03 ; %s\x02", b+1);
                DrawChars(*cx, *cy + (POS_HEIGHT/2), tlbuf);
            } else {
                sprintf(tlbuf, "\x03 ; %s\x02", tbuf);
                DrawChars(*cx, *cy + (POS_HEIGHT/2) - 1, tlbuf);
            }

            *cx += ColsAvailable*POS_WIDTH;
            break;
        }
        case ELEM_PLACEHOLDER: {
            CenterWithWiresWidth(*cx, *cy, "--", FALSE, FALSE, 2);
            *cx += POS_WIDTH;
            break;
        }
        case ELEM_CONTACTS: {
            char buf[4], name_with_type[MAX_NAME_LEN+4];
            ElemContacts *c = &leaf->d.contacts;

            buf[0] = ']';
            buf[1] = c->negated ? '/' : ' ';
            buf[2] = '[';
            buf[3] = '\0';

			// TODO: Internacionalizar
			switch(c->type) {
			case IO_TYPE_DIG_INPUT:
				ch = 'E';
				break;

			case IO_TYPE_DIG_OUTPUT:
				ch = 'S';
				break;

			case IO_TYPE_INTERNAL_RELAY:
				ch = 'R';
				break;

			case IO_TYPE_INTERNAL_FLAG:
				ch = 'F';
				break;

			default:
				ch = '?';
				break;
			}
			sprintf(name_with_type, "%s (%c)", c->name, ch);

            CenterWithSpaces(*cx, *cy, name_with_type, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);

            *cx += POS_WIDTH;
            break;
        }
        {
            char *s;
            case ELEM_EQU:
                s = "=="; goto cmp;
            case ELEM_NEQ:
                s = "<>"; goto cmp;
            case ELEM_GRT:
                s = ">"; goto cmp;
            case ELEM_GEQ:
                s = ">="; goto cmp;
            case ELEM_LES:
                s = "<"; goto cmp;
            case ELEM_LEQ:
                s = "<="; goto cmp;
cmp:
                char s1[POS_WIDTH+10], s2[POS_WIDTH+10];
                int l1, l2, lmax;

                l1 = 2 + 1 + strlen(s) + strlen(leaf->d.cmp.op1);
                l2 = 2 + 1 + strlen(leaf->d.cmp.op2);
                lmax = max(l1, l2);

                memset(s1, ' ', sizeof(s1));
                s1[0] = '[';
                s1[lmax-1] = ']';
                s1[lmax] = '\0';
                strcpy(s2, s1);
                memcpy(s1+1, leaf->d.cmp.op1, strlen(leaf->d.cmp.op1));
                memcpy(s1+strlen(leaf->d.cmp.op1)+2, s, strlen(s));
                memcpy(s2+2, leaf->d.cmp.op2, strlen(leaf->d.cmp.op2));

                CenterWithSpaces(*cx, *cy, s1, poweredAfter, FALSE);
                CenterWithWires(*cx, *cy, s2, poweredBefore, poweredAfter);

                *cx += POS_WIDTH;
                break;
        }
        case ELEM_OPEN:
            CenterWithWires(*cx, *cy, "+      +", poweredBefore, poweredAfter);
            *cx += POS_WIDTH;
            break;

        case ELEM_SHORT:
            CenterWithWires(*cx, *cy, "+------+", poweredBefore, poweredAfter);
            *cx += POS_WIDTH;
            break;

        case ELEM_ONE_SHOT_RISING:
        case ELEM_ONE_SHOT_FALLING: {
            char *s1, *s2;
            if(which == ELEM_ONE_SHOT_RISING) {
                s1 = "      _ ";
                s2 = _("[\x01OSR\x02_/ ]");
            } else if(which == ELEM_ONE_SHOT_FALLING) {
                s1 = "    _   ";
                s2 = _("[\x01OSF\x02 \\_]");
            } else oops();

            CenterWithSpaces(*cx, *cy, s1, poweredAfter, FALSE);
            CenterWithWires(*cx, *cy, s2, poweredBefore, poweredAfter);

            *cx += POS_WIDTH;
            break;
        }
        case ELEM_CTU:
        case ELEM_CTD: {
            char *s, op;
            if(which == ELEM_CTU) {
                s = _("CTU");
				op = '>';
			} else if(which == ELEM_CTD) {
                s = _("CTD");
				op = '<';
			} else oops();

            char buf[256];
            ElemCounter *c = &leaf->d.counter;
            sprintf(buf, "[\x01%s\x02 %c=%d]", s, op, c->max);

            CenterWithSpaces(*cx, *cy, c->name, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);

            *cx += POS_WIDTH;
            break;
        }
        case ELEM_RTO:
        case ELEM_TON:
        case ELEM_TOF: {
            char *s;
            if(which == ELEM_TON)
                s = _("TON");
            else if(which == ELEM_TOF)
                s = _("TOF");
            else if(which == ELEM_RTO)
                s = _("RTO");
            else oops();

            char buf[256];
            ElemTimer *t = &leaf->d.timer;
            if(t->delay >= 1000*1000) {
                sprintf(buf, "[\x01%s\x02 %.3f s]", s, t->delay/1000000.0);
            } else if(t->delay >= 100*1000) {
                sprintf(buf, "[\x01%s\x02 %.1f ms]", s, t->delay/1000.0);
            } else {
                sprintf(buf, "[\x01%s\x02 %.2f ms]", s, t->delay/1000.0);
            }

            CenterWithSpaces(*cx, *cy, t->name, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, buf, poweredBefore, poweredAfter);

            *cx += POS_WIDTH;
            break;
        }
		case ELEM_RTC:
		{
			int linha;
            char bufs[256], bufe[256];

			char mday[10];
			char month[10];
			char year[10];

			memset(month, 0, sizeof(month));
			memset(year, 0, sizeof(year));

            ElemRTC *t = &leaf->d.rtc;

			for(linha=0; linha<2; linha++) {
				struct tm *ptm = linha ? &t->end : &t->start;

				if(t->wday & (1 << 7)) 
				{
					int i;

					sprintf(linha ? bufe : bufs, "[\x01%s %02d:%02d:%02d\x02]", _("SMTWTFS"),
						ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

					if(linha || t->mode == ELEM_RTC_MODE_FIXED) {
						for(i=0; i<7; i++) {
							if(!(t->wday & (1<<i))) {
								(linha ? bufe : bufs)[i+2] = ' ';
							} else if(linha) {
								bufe[i+2] = '*';
							}
						}
					}
				} else {
					if (ptm->tm_mday)
						sprintf(mday, "%02d", ptm->tm_mday);
					else
						sprintf(mday, " *");

					if (ptm->tm_mon)
						sprintf(month, "%02d", ptm->tm_mon);
					else
						sprintf(month, " *");

					if (ptm->tm_year)
						sprintf(year, "%02d", ptm->tm_year);			
					else
						sprintf(year, " *  ");

					sprintf(linha ? bufe : bufs, "[\x01%s/%s/%s %02d:%02d:%02d\x02]", mday, month, year, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				} 
			}

			CenterWithSpaces(*cx, *cy, t->mode == ELEM_RTC_MODE_FIXED ? _("RTC - Mode Fixed") : bufs, poweredAfter, TRUE);
            CenterWithWires(*cx, *cy, t->mode == ELEM_RTC_MODE_FIXED ? bufs : bufe, poweredBefore, poweredAfter);

            *cx += POS_WIDTH;
            break;
		}
		case ELEM_READ_FORMATTED_STRING:
		case ELEM_WRITE_FORMATTED_STRING:
        case ELEM_FORMATTED_STRING: {
            char bot[100];
            sprintf(bot, "%s: %s", which == ELEM_READ_FORMATTED_STRING ? _("READ") : _("WRITE"), 
										leaf->d.fmtdStr.var);

            int extra = 2*POS_WIDTH - strlen(bot);
            DrawChars(*cx + (extra/2), *cy + (POS_HEIGHT/2) - 1,
                bot);

			sprintf(bot, "{\"%s\"}", leaf->d.fmtdStr.string);

            CenterWithWiresWidth(*cx, *cy, bot, poweredBefore, poweredAfter,
                2*POS_WIDTH);
            *cx += 2*POS_WIDTH;
            break;
        }
		case ELEM_READ_SERVO_YASKAWA:
		case ELEM_WRITE_SERVO_YASKAWA:{
            char bot[100];
            sprintf(bot, "%s: %s", which == ELEM_READ_SERVO_YASKAWA ? _("RX NS600") : _("TX NS600"), 
										leaf->d.servoYaskawa.var);

            int extra = 2*POS_WIDTH - strlen(bot);
            DrawChars(*cx + (extra/2), *cy + (POS_HEIGHT/2) - 1,
                bot);

			sprintf(bot, "{\"%s\"}", leaf->d.servoYaskawa.string);

            CenterWithWiresWidth(*cx, *cy, bot, poweredBefore, poweredAfter,
                2*POS_WIDTH);
            *cx += 2*POS_WIDTH;
            break;
        }
        case ELEM_SET_BIT:
		{
			char str[100];
			sprintf(str, _("{%s BIT:%d}"), leaf->d.setBit.set ? _("SET") : _("RST"), leaf->d.setBit.bit);
            CenterWithWires(*cx, *cy, str, poweredBefore, poweredAfter);
            CenterWithSpaces(*cx, *cy, leaf->d.setBit.name, poweredAfter, TRUE);
            *cx += POS_WIDTH;
            break;
		}
        case ELEM_CHECK_BIT:
		{
			char str[100];
			sprintf(str, _("{CHECK %s:%d}"), leaf->d.checkBit.set ? _("ON") : _("OFF"), leaf->d.checkBit.bit);
            CenterWithWires(*cx, *cy, str, poweredBefore, poweredAfter);
            CenterWithSpaces(*cx, *cy, leaf->d.checkBit.name, poweredAfter, TRUE);
            *cx += POS_WIDTH;
            break;
		}
        case ELEM_READ_USS:
        case ELEM_WRITE_USS:
            CenterWithWires(*cx, *cy,
                (which == ELEM_READ_USS) ? _("{READ USS}") : _("{WRITE USS}"),
                poweredBefore, poweredAfter);
            CenterWithSpaces(*cx, *cy, (which == ELEM_READ_USS) ? leaf->d.readUSS.name : leaf->d.writeUSS.name, poweredAfter, TRUE);
            *cx += POS_WIDTH;
            break;

        case ELEM_READ_MODBUS:
        case ELEM_WRITE_MODBUS: {
			char buf[100];
			MbNodeList *l = MbNodeList_GetByNodeID(which == ELEM_READ_MODBUS ? leaf->d.readModbus.elem : leaf->d.writeModbus.elem);

			CenterWithWires(*cx, *cy,
				(l->node.iface == MB_IFACE_RS485 ? ((which == ELEM_READ_MODBUS) ? _("{READ MB 485}") : _("{WRITE MB 485}")) :
				((which == ELEM_READ_MODBUS) ? _("{READ MB ETH}") : _("{WRITE MB ETH}"))),
                poweredBefore, poweredAfter);

			sprintf(buf, "%s:%d", l->node.name, which == ELEM_READ_MODBUS ? leaf->d.readModbus.address : leaf->d.writeModbus.address);
			CenterWithSpaces(*cx, *cy, buf, poweredAfter, TRUE);

			*cx += POS_WIDTH;
            break;
		}

		case ELEM_UART_RECV:
        case ELEM_UART_SEND:
            CenterWithWires(*cx, *cy,
                (which == ELEM_UART_RECV) ? _("{UART RECV}") : _("{UART SEND}"),
                poweredBefore, poweredAfter);
            CenterWithSpaces(*cx, *cy, leaf->d.uart.name, poweredAfter, TRUE);
            *cx += POS_WIDTH;
            break;

        default:
            poweredAfter = DrawEndOfLine(which, leaf, cx, cy, poweredBefore);
            break;
    }

    // And now we can enter the element into the display matrix so that the
    // UI routines know what element is at position (gx, gy) when the user
    // clicks there, and so that we know where to put the cursor if this
    // element is selected.

    // Don't use original cx0, as an end of line element might be further
    // along than that.
    cx0 = *cx - POS_WIDTH;

    int gx = cx0/POS_WIDTH;
    int gy = cy0/POS_HEIGHT;
    if(CheckBoundsUndoIfFails(gx, gy)) 
		return FALSE;
    DM_BOUNDS(gx, gy);

    DisplayMatrix[gx][gy] = leaf;
    DisplayMatrixWhich[gx][gy] = which;

    int xadj = 0;
    switch(which) {
        case ELEM_RAND:
        case ELEM_ADD:
        case ELEM_SUB:
        case ELEM_MUL:
        case ELEM_DIV:
        case ELEM_MOD:
		case ELEM_READ_FORMATTED_STRING:
		case ELEM_WRITE_FORMATTED_STRING:
		case ELEM_READ_SERVO_YASKAWA:
		case ELEM_WRITE_SERVO_YASKAWA:
        case ELEM_FORMATTED_STRING:
            DM_BOUNDS(gx-1, gy);
            DisplayMatrix[gx-1][gy] = leaf;
            DisplayMatrixWhich[gx-1][gy] = which;
            xadj = POS_WIDTH*FONT_WIDTH;
            break;
    }

    if(which == ELEM_COMMENT) {
        int i;
        for(i = 0; i < ColsAvailable; i++) {
            DisplayMatrix[i][gy] = leaf;
            DisplayMatrixWhich[i][gy] = ELEM_COMMENT;
        }
        xadj = (ColsAvailable-1)*POS_WIDTH*FONT_WIDTH;
    }

    int x0 = X_PADDING + cx0*FONT_WIDTH;
    int y0 = Y_PADDING + cy0*FONT_HEIGHT;

    if(leaf->selectedState != SELECTED_NONE && leaf == Selected) {
        SelectionActive = TRUE;
    }
    switch(leaf->selectedState) {
        case SELECTED_LEFT:
            Cursor.left = x0 + FONT_WIDTH - 4 - xadj;
            Cursor.top = y0 - FONT_HEIGHT/2;
            Cursor.width = 2;
            Cursor.height = POS_HEIGHT*FONT_HEIGHT;
            break;

        case SELECTED_RIGHT:
            Cursor.left = x0 + (POS_WIDTH-1)*FONT_WIDTH - 5;
            Cursor.top = y0 - FONT_HEIGHT/2;
            Cursor.width = 2;
            Cursor.height = POS_HEIGHT*FONT_HEIGHT;
            break;

        case SELECTED_ABOVE:
            Cursor.left = x0 + FONT_WIDTH/2 - xadj;
            Cursor.top = y0 - 2;
            Cursor.width = (POS_WIDTH-2)*FONT_WIDTH + xadj;
            Cursor.height = 2;
            break;

        case SELECTED_BELOW:
            Cursor.left = x0 + FONT_WIDTH/2 - xadj;
            Cursor.top = y0 + (POS_HEIGHT-1)*FONT_HEIGHT +
                FONT_HEIGHT/2 - 2;
            Cursor.width = (POS_WIDTH-2)*(FONT_WIDTH) + xadj;
            Cursor.height = 2;
            break;

        default:
            break;
    }

    return poweredAfter;*/
}

//-----------------------------------------------------------------------------
// Draw a particular subcircuit with its top left corner at *cx and *cy (in
// characters). If it is a leaf element then just print it and return; else
// loop over the elements of the subcircuit and call ourselves recursively.
// At the end updates *cx and *cy.
//
// In simulation mode, returns TRUE the circuit is energized after the given
// element, else FALSE. This is needed to colour all the wires correctly,
// since the colouring indicates whether a wire is energized.
//-----------------------------------------------------------------------------
BOOL DrawElement(int which, void *elem, int *cx, int *cy, BOOL poweredBefore)
{
    BOOL poweredAfter;

    int cx0 = *cx, cy0 = *cy;
    ElemLeaf *leaf = (ElemLeaf *)elem;

    SetBkColor(Hdc, InSimulationMode ? HighlightColours.simBg :
        HighlightColours.bg);

    if(elem == Selected && !InSimulationMode) {
        ThisHighlighted = TRUE;
    } else {
        ThisHighlighted = FALSE;
    }

    switch(which) {
        case ELEM_SERIES_SUBCKT: {
            int i;
            ElemSubcktSeries *s = (ElemSubcktSeries *)elem;
            poweredAfter = poweredBefore;
            for(i = 0; i < s->count; i++) {
                poweredAfter = DrawElement(s->contents[i].which,
                    s->contents[i].d.any, cx, cy, poweredAfter);
            }
            break;
        }
        case ELEM_PARALLEL_SUBCKT: {
            int i;
            ElemSubcktParallel *p = (ElemSubcktParallel *)elem;
            int widthMax = CountWidthOfElement(which, elem, (*cx)/POS_WIDTH);
            int heightMax = CountHeightOfElement(which, elem);

            poweredAfter = FALSE;

            int lowestPowered = -1;
            int downBy = 0;
            for(i = 0; i < p->count; i++) {
                BOOL poweredThis;

                poweredThis = DrawElement(p->contents[i].which,
                    p->contents[i].d.any, cx, cy, poweredBefore);

                if(InSimulationMode) {
                    if(poweredThis) poweredAfter = TRUE;
                }

                while((*cx - cx0) < widthMax*POS_WIDTH) {
                    int gx = *cx/POS_WIDTH;
                    int gy = *cy/POS_HEIGHT;

                    if(CheckBoundsUndoIfFails(gx, gy)) return FALSE;

                    DM_BOUNDS(gx, gy);
					if(gx>0 && DisplayMatrixWhich[gx-1][gy] == ELEM_PLACEHOLDER)
						break;
                    DisplayMatrix[gx][gy] = PADDING_IN_DISPLAY_MATRIX;
                    DisplayMatrixWhich[gx][gy] = ELEM_PADDING;

                    char buf[256];
                    int j;
                    for(j = 0; j < POS_WIDTH; j++) {
                        buf[j] = '-';
                    }
                    buf[j] = '\0';
                    DrawChars(*cx, *cy + (POS_HEIGHT/2), buf);
                    *cx += POS_WIDTH;
                }

                *cx = cx0;
                int justDrewHeight = CountHeightOfElement(p->contents[i].which,
                    p->contents[i].d.any);
                *cy += POS_HEIGHT*justDrewHeight;

                downBy += justDrewHeight;
                if(poweredThis) {
                    lowestPowered = downBy - 1;
                }
            }
            *cx = cx0 + POS_WIDTH*widthMax;
            *cy = cy0;

            int j;
            BOOL needWire;

            if(*cx/POS_WIDTH != ColsAvailable) {
				int gx = *cx/POS_WIDTH;
				int gy = *cy/POS_HEIGHT;

                needWire = FALSE;
                for(j = heightMax - 1; j >= 1; j--) {
                    if(DisplayMatrix[gx - 1][gy + j] && DisplayMatrixWhich[gx - 1][gy + j] != ELEM_PLACEHOLDER) {
                        needWire = TRUE;
                    }
                    if(needWire) VerticalWire(*cx - 1, *cy + j*POS_HEIGHT);
                }
                // stupid special case
                if(lowestPowered == 0 && InSimulationMode) {
                    DrawChars(*cx - 1, *cy + (POS_HEIGHT/2), "+");
                }
            }

            needWire = FALSE;
            for(j = heightMax - 1; j >= 1; j--) {
                if(DisplayMatrix[cx0/POS_WIDTH][*cy/POS_HEIGHT + j]) {
                    needWire = TRUE;
                }
                if(needWire) VerticalWire(cx0 - 1, *cy + j*POS_HEIGHT);
            }

            break;
        }
        default:
            poweredAfter = DrawLeaf(which, leaf, cx, cy, poweredBefore);
            break;
    }


    return poweredAfter;
}

//-----------------------------------------------------------------------------
// Draw the rung that signals the end of the program. Kind of useless but
// do it anyways, for convention.
//-----------------------------------------------------------------------------
void DrawEndRung(int cx, int cy)
{
    int i;
    char *str = _("[END]");
    int lead = (ColsAvailable*POS_WIDTH - strlen(str))/2;
    ThisHighlighted = TRUE;
    for(i = 0; i < lead; i++) {
        DrawChars(cx + i, cy + (POS_HEIGHT/2), "-");
    }
    DrawChars(cx + i, cy + (POS_HEIGHT/2), str);
    i += strlen(str);
    for(; i < ColsAvailable*POS_WIDTH; i++) {
        DrawChars(cx + i, cy + (POS_HEIGHT/2), "-");
    }
}

// Dummy function
void DrawCharsToNothing(int x, int y, char *txt) {}

//-----------------------------------------------------------------------------
// Do a fake paint, just to update DisplayMatrix and DisplayMatrixWhich
//-----------------------------------------------------------------------------
void DummyPaint(void)
{
    int i;
    int thisHeight, cx, cy = 0;

	memset(DisplayMatrix, 0, sizeof(DisplayMatrix));
    memset(&Cursor, 0, sizeof(Cursor));

    DrawChars = DrawCharsToNothing;

    for(i = 0; i < Prog.numRungs; i++) {
		cx = 0;
		thisHeight = POS_HEIGHT*CountHeightOfElement(ELEM_SERIES_SUBCKT, Prog.rungs[i]);

		DrawElement(ELEM_SERIES_SUBCKT, Prog.rungs[i], &cx, &cy, 0);

        cy += thisHeight;
        cy += POS_HEIGHT;
    }
}
