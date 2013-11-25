#include "poptools.h"

// Number of drawing columns (leaf element units) available. We want to
// know this so that we can right-justify the coils.
int ColsAvailable;

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

    (ladder->getFncDrawChars())(cx + (extra/2), cy + (POS_HEIGHT/2) - 1, str);
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
	tFncDrawChars DrawChars = ladder->getFncDrawChars();
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
