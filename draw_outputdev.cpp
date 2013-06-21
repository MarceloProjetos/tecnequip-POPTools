#include "poptools.h"

void (*DrawChars)(int, int, char *);
void DrawCharsToEngineGUI(int cx, int cy, char *str);

// After an undo all the memory addresses change but make an effort to put
// the cursor roughly where it should be.
int SelectedGxAfterNextPaint = -1;
int SelectedGyAfterNextPaint = -1;

// After pushing a rung up or down the position of that rung in the table
// changes, but the cursor should stay where it was.
BOOL ScrollSelectedIntoViewAfterNextPaint;

// Buffer that we write to when exporting (drawing) diagram to a text file. 
// Dynamically allocated so that we're at least slightly efficient.
static char **ExportBuffer;

// The fonts that we will use to draw the ladder diagram: fixed width, one
// normal-weight, one bold.
HFONT       FixedWidthFont;
HFONT       FixedWidthFontBold;

// Different colour brushes for right and left buses in simulation, but same
// colour for both in edit mode; also for the backgrounds in simulation and
// edit modes.
static HBRUSH   BusRightBus;
static HBRUSH   BusLeftBrush;
static HBRUSH   BusBrush;
static HBRUSH   BgBrush;
static HBRUSH   SimBgBrush;

// Parameters that determine our offset if we are scrolled
int ScrollXOffset;
int ScrollYOffset;
int ScrollXOffsetMax;
int ScrollYOffsetMax;

// Is the cursor currently drawn? We XOR it so this gets toggled.
BOOL CursorDrawn;

// Colours with which to do syntax highlighting, configurable
SyntaxHighlightingColours HighlightColours;

#define X_RIGHT_PADDING 30

//-----------------------------------------------------------------------------
// Blink the cursor on the schematic; called by a Windows timer. We XOR
// draw it so just draw the same rectangle every time to show/erase the
// cursor. Cursor may be in one of four places in the selected leaf (top,
// bottom, left, right) but we don't care; just go from the coordinates
// computed when we drew the schematic in the paint procedure.
//-----------------------------------------------------------------------------
void CALLBACK BlinkCursor2(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
    if(GetFocus() != MainWindow && !CursorDrawn) return;
    if(Cursor.left == 0) return;

    PlcCursor c;
    memcpy(&c, &Cursor, sizeof(c));

    c.top -= ScrollYOffset*POS_HEIGHT*FONT_HEIGHT;
    c.left -= ScrollXOffset;

    if(c.top >= IoListTop) return;

    if(c.top + c.height >= IoListTop) {
        c.height = IoListTop - c.top - 3;
    }

    Hdc = GetDC(DrawWindow);
    SelectObject(Hdc, GetStockObject(WHITE_BRUSH));
    PatBlt(Hdc, c.left, c.top, c.width, c.height, PATINVERT);
    CursorDrawn = !CursorDrawn;
    ReleaseDC(DrawWindow, Hdc);
}

//-----------------------------------------------------------------------------
// Output a string to the screen at a particular location, in character-
// sized units.
//-----------------------------------------------------------------------------
static void DrawCharsToScreen(int cx, int cy, char *str)
{
    cy -= ScrollYOffset*POS_HEIGHT;
    if(cy < -2) return;
    if(cy*FONT_HEIGHT + Y_PADDING + (int)RibbonHeight > IoListTop) return;

    COLORREF prev;
    BOOL firstTime = TRUE;
    BOOL inNumber = FALSE;
    BOOL inComment = FALSE;
    int inBrace = 0;
    for(; *str; str++, cx++) {
        int x = cx*FONT_WIDTH + X_PADDING;
        int y = cy*FONT_HEIGHT + Y_PADDING;

        BOOL hiOk = !(InSimulationMode || ThisHighlighted);

        if(strchr("{}[]", *str) && hiOk && !inComment)  {
            if(*str == '{' || *str == '[') inBrace++;
            if(inBrace == 1) {
                prev = GetTextColor(Hdc);
                SetTextColor(Hdc, HighlightColours.punct);
                TextOut(Hdc, x, y, str, 1);
                SetTextColor(Hdc, prev);
            } else {
                TextOut(Hdc, x, y, str, 1);
            }
            if(*str == ']' || *str == '}') inBrace--;
        } else if((
            ((*str > 0 ? isdigit(*str) : false) && (firstTime || isspace(str[-1]) 
                || str[-1] == ':' || str[-1] == '[')) ||
            (*str == '-' && (*str > 0 ? isdigit(str[1]) : false))) && hiOk && !inComment)
        {
            prev = GetTextColor(Hdc);
            SetTextColor(Hdc, HighlightColours.lit);
            TextOut(Hdc, x, y, str, 1);
            SetTextColor(Hdc, prev);
            inNumber = TRUE;
        } else if(*str == '\x01') {
            cx--;
            if(hiOk) {
                prev = GetTextColor(Hdc);
                SetTextColor(Hdc, HighlightColours.op);
            }
        } else if(*str == '\x02') {
            cx--;
            if(hiOk) {
                SetTextColor(Hdc, prev);
                inComment = FALSE;
            }
        } else if(*str == '\x03') {
            cx--;
            if(hiOk) {
                prev = GetTextColor(Hdc);
                SetTextColor(Hdc, HighlightColours.comment);
                inComment = TRUE;
            }
        } else if(inNumber) {
            if((*str > 0 ? isdigit(*str) : false) || *str == '.') {
                prev = GetTextColor(Hdc);
                SetTextColor(Hdc, HighlightColours.lit);
                TextOut(Hdc, x, y, str, 1);
                SetTextColor(Hdc, prev);
            } else {
                TextOut(Hdc, x, y, str, 1);
                inNumber = FALSE;
            }
		} else if(*str == '-') {
			HPEN hOldPen = (HPEN)SelectObject(Hdc, CreatePen(PS_SOLID, 1, GetTextColor(Hdc)));
			MoveToEx(Hdc, x, y + FONT_HEIGHT / 2, NULL);
			LineTo(Hdc, x + FONT_WIDTH, y + FONT_HEIGHT / 2);
			DeleteObject(SelectObject(Hdc, hOldPen));
		} else if(*str == '+') {
			if (x > X_PADDING - FONT_WIDTH + 3)
			{
				HPEN hOldPen = (HPEN)SelectObject(Hdc, CreatePen(PS_SOLID, 1, GetTextColor(Hdc)));
				MoveToEx(Hdc, x, y + FONT_HEIGHT / 2, NULL);
				LineTo(Hdc, x + FONT_WIDTH, y + FONT_HEIGHT / 2);
				MoveToEx(Hdc, x + FONT_WIDTH / 2, y + FONT_HEIGHT / 4, NULL);
				LineTo(Hdc, x + FONT_WIDTH / 2, y + ((FONT_HEIGHT / 4) * 3) + 1);
				DeleteObject(SelectObject(Hdc, hOldPen));
			}
		} else if(*str == '|') {
			if (x > X_PADDING - FONT_WIDTH + 3)
			{
				HPEN hOldPen = (HPEN)SelectObject(Hdc, CreatePen(PS_SOLID, 1, GetTextColor(Hdc)));
				MoveToEx(Hdc, x + FONT_WIDTH / 2, y, NULL);
				LineTo(Hdc, x + FONT_WIDTH / 2, y + FONT_HEIGHT);
				DeleteObject(SelectObject(Hdc, hOldPen));
			}
        } else {
            TextOut(Hdc, x, y, str, 1);
        }

        firstTime = FALSE;
    }
}

//-----------------------------------------------------------------------------
// Total number of columns that we can display in the given amount of 
// window area. Need to leave some slop on the right for the scrollbar, of
// course.
//-----------------------------------------------------------------------------
int ScreenColsAvailable(void)
{
    RECT r;
    GetClientRect(MainWindow, &r);

    return (r.right - (X_PADDING + X_RIGHT_PADDING)) / (POS_WIDTH*FONT_WIDTH);
}

//-----------------------------------------------------------------------------
// Total number of columns that we can display in the given amount of 
// window area. Need to leave some slop on the right for the scrollbar, of
// course, and extra slop at the bottom for the horiz scrollbar if it is
// shown.
//-----------------------------------------------------------------------------
int ScreenRowsAvailable(void)
{
    int adj;
    if(ScrollXOffsetMax == 0) {
        adj = 0;
    } else {
        adj = 18;
    }
    return (IoListTop - Y_PADDING - adj - RibbonHeight) / (POS_HEIGHT*FONT_HEIGHT);
}

HRESULT InitRibbon(HWND hWindowFrame);
HRESULT UpdateRibbonHeight(void);

//-----------------------------------------------------------------------------
// Set up the syntax highlighting colours, according to the currently selected
// scheme.
//-----------------------------------------------------------------------------
static void SetSyntaxHighlightingColours(void)
{
     static const SyntaxHighlightingColours Schemes[] = {
        {
			RGB(0, 0, 0),           // bg
            RGB(255, 255, 225),     // def
            RGB(255, 90, 90),       // selected
            RGB(255, 150, 90),      // op
            RGB(255, 255, 100),     // punct
            RGB(255, 160, 160),     // lit
            RGB(120, 255, 130),     // name
            RGB(130, 130, 255),     // rungNum
            RGB(130, 130, 255),     // comment

            RGB(255, 255, 255),     // bus

            RGB(255, 0, 0),         // breakpoint

            RGB(0, 0, 0),           // simBg
            RGB(130, 130, 255),     // simRungNum
            RGB(100, 130, 130),     // simOff
            RGB(255, 150, 150),     // simOn

            RGB(255, 150, 150),     // simBusLeft
            RGB(150, 150, 255),     // simBusRight
        },
        {
			RGB(250, 250, 255),     // bg
            RGB(0, 0, 255),         // def
            RGB(255, 0, 0),         // selected
            RGB(255, 0, 0),         // op
            RGB(250, 100, 0),       // punct
            RGB(190, 0, 250),       // lit
            RGB(30, 130, 10),       // name
            RGB(20, 20, 255),       // rungNum
            RGB(40, 200, 0),        // comment

            RGB(0, 0, 255),         // bus

            RGB(255, 0, 0),         // breakpoint

            RGB(10, 10, 38),        // simBg
            RGB(44, 231, 148),      // simRungNum
            RGB(100, 130, 130),     // simOff
            RGB(255, 150, 150),     // simOn

            RGB(255, 150, 150),     // simBusLeft
            RGB(150, 150, 255),     // simBusRight            
        },
    };

#if CORES_CONSOLE
    memcpy(&HighlightColours, &Schemes[0], sizeof(Schemes[0]));
#else
    memcpy(&HighlightColours, &Schemes[1], sizeof(Schemes[1]));
#endif
}

//-----------------------------------------------------------------------------
// Set up the stuff we'll need to draw our schematic diagram. Fonts, brushes,
// pens, etc.
//-----------------------------------------------------------------------------
void InitForDrawing(void)
{
    SetSyntaxHighlightingColours();

    FixedWidthFont = CreateFont(
        FONT_HEIGHT, FONT_WIDTH,
        0, 0,
#if CORES_CONSOLE
        FW_REGULAR,
#else
		FW_MEDIUM,
#endif
        FALSE, // Italic 
        FALSE, // Underline
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FF_DONTCARE,
        "Lucida Console");

    FixedWidthFontBold = CreateFont(
        FONT_HEIGHT, FONT_WIDTH,
        0, 0,
#if CORES_CONSOLE
        FW_REGULAR,
#else
		FW_MEDIUM,
#endif
        FALSE, // Italic 
        FALSE, // Underline
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FF_DONTCARE,
        "Lucida Console");

    LOGBRUSH lb;
    lb.lbStyle = BS_SOLID;
    lb.lbColor = HighlightColours.simBusRight;
    BusRightBus = CreateBrushIndirect(&lb);

    lb.lbColor = HighlightColours.simBusLeft;
    BusLeftBrush = CreateBrushIndirect(&lb);

    lb.lbColor = HighlightColours.bus;
    BusBrush = CreateBrushIndirect(&lb);

    lb.lbColor = HighlightColours.bg;
    BgBrush = CreateBrushIndirect(&lb);

    lb.lbColor = HighlightColours.simBg;
    SimBgBrush = CreateBrushIndirect(&lb);
}

//-----------------------------------------------------------------------------
// DrawChars function, for drawing to the export buffer instead of to the
// screen.
//-----------------------------------------------------------------------------
static void DrawCharsToExportBuffer(int cx, int cy, char *str)
{
    while(*str) {
        if(*str >= 10) {
            ExportBuffer[cy][cx] = *str;
            cx++;
        }
        str++;
    }
}

//-----------------------------------------------------------------------------
// Export a text drawing of the ladder logic program to a file.
//-----------------------------------------------------------------------------
void ExportDrawingAsText(char *file)
{
	int i, maxWidth = ladder->getWidthTXT();
	int totalHeight = ladder->getHeightTXT()*POS_HEIGHT + 3;

	ColsAvailable = maxWidth;

    ExportBuffer = (char **)CheckMalloc(totalHeight * sizeof(char *));
   
    int l = maxWidth*POS_WIDTH + 8;
    for(i = 0; i < totalHeight; i++) {
        ExportBuffer[i] = (char *)CheckMalloc(l);
        memset(ExportBuffer[i], ' ', l-1);
        ExportBuffer[i][5] = '|';
        ExportBuffer[i][4] = '|';
        ExportBuffer[i][l-3] = '|';
        ExportBuffer[i][l-2] = '|';
        ExportBuffer[i][l-1] = '\0';
    }

    DrawChars = DrawCharsToExportBuffer;
	ladder->DrawTXT(6);

    FILE *f = fopen(file, "w");
    if(!f) {
        Error(_("Couldn't open '%s'\n"), f);
        return;
    }

    fprintf(f, _("POPTools - Diagrama Ladder exportado\n"));

    if(Prog.mcu) {
        fprintf(f, _("Para '%s', cristal de %.6f MHz, tempo de ciclo de %.1f ms\n\n"),
            Prog.mcu->mcuName, Prog.settings.mcuClock/1e6, Prog.settings.cycleTime/1e3);
    } else {
        fprintf(f, _("no MCU assigned, %.6f MHz crystal, %.1f ms cycle time\n\n"),
            Prog.settings.mcuClock/1e6, Prog.settings.cycleTime/1e3);
    }

    fprintf(f, _("\nDiagrama Ladder:\n\n"));

    for(i = 0; i < totalHeight; i++) {
        ExportBuffer[i][l-3] = '|';
        fprintf(f, "%s\n", ExportBuffer[i]);
        CheckFree(ExportBuffer[i]);
    }
    CheckFree(ExportBuffer);
    ExportBuffer = NULL;

    fprintf(f, _("\n\nI/O ASSIGNMENT:\n\n"));
    
    fprintf(f, _("  Name                       | Type               | Pin\n"));
    fprintf(f,   " ----------------------------+--------------------+------\n");
    for(i = 0; i < Prog.io.count; i++) {
        char b[1024];
        memset(b, '\0', sizeof(b));

        PlcProgramSingleIo *io = &Prog.io.assignment[i];
        char *type = IoTypeToString(io->type);
        char pin[MAX_NAME_LEN];

        PinNumberForIo(pin, io);

        sprintf(b, "                             |                    | %s\n",
            pin);

        memcpy(b+2, io->name, strlen(io->name));
        memcpy(b+31, type, strlen(type));
        fprintf(f, "%s", b);
    }

    fclose(f);

    // we may have trashed the grid tables a bit; a repaint will fix that
    InvalidateRect(MainWindow, NULL, FALSE);
}

//-----------------------------------------------------------------------------
// Determine the settings of the vertical and (if needed) horizontal
// scrollbars used to scroll our view of the program.
//-----------------------------------------------------------------------------
void SetUpScrollbars2(BOOL *horizShown, SCROLLINFO *horiz, SCROLLINFO *vert)
{
    int totalHeight = 0;
    int i;
    for(i = 0; i < Prog.numRungs; i++) {
        totalHeight += CountHeightOfElement(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
        totalHeight++;
    }
    totalHeight += 1; // for the end rung

    int totalWidth = ProgCountWidestRow();

    if(totalWidth <= ScreenColsAvailable()) {
        *horizShown = FALSE;
        ScrollXOffset = 0;
        ScrollXOffsetMax = 0;
    } else {
        *horizShown = TRUE;
        memset(horiz, 0, sizeof(*horiz));
        horiz->cbSize = sizeof(*horiz);
        horiz->fMask = SIF_DISABLENOSCROLL | SIF_ALL;
        horiz->nMin = 0;
        horiz->nMax = X_PADDING + totalWidth*POS_WIDTH*FONT_WIDTH;
        RECT r;
        GetClientRect(MainWindow, &r);
        horiz->nPage = r.right - X_PADDING;
        horiz->nPos = ScrollXOffset;

        ScrollXOffsetMax = horiz->nMax - horiz->nPage + 1;
        if(ScrollXOffset > ScrollXOffsetMax) ScrollXOffset = ScrollXOffsetMax;
        if(ScrollXOffset < 0) ScrollXOffset = 0;
    }

    vert->cbSize = sizeof(*vert);
    vert->fMask = SIF_DISABLENOSCROLL | SIF_ALL;
    vert->nMin = 0;
    vert->nMax = totalHeight - 1;
    vert->nPos = ScrollYOffset;
    vert->nPage = ScreenRowsAvailable();

    ScrollYOffsetMax = vert->nMax - vert->nPage + 1;

    if(ScrollYOffset > ScrollYOffsetMax) ScrollYOffset = ScrollYOffsetMax;
    if(ScrollYOffset < 0) ScrollYOffset = 0;
}
