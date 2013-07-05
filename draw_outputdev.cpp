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
	int i;
	int maxWidth    = ladder->getWidthTXT();
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

	McuIoInfo *mcu = ladder->getMCU();
	LadderSettingsGeneral settings = ladder->getSettingsGeneral();

	if(mcu != nullptr) {
        fprintf(f, _("Para '%s', cristal de %.6f MHz, tempo de ciclo de %.1f ms\n\n"),
            mcu->mcuName, settings.mcuClock/1e6, settings.cycleTime/1e3);
    } else {
        fprintf(f, _("no MCU assigned, %.6f MHz crystal, %.1f ms cycle time\n\n"),
            settings.mcuClock/1e6, settings.cycleTime/1e3);
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

	string name;
	mapDetails detailsIO;
	unsigned int index, count = ladder->getCountIO();
    for(index = 0; index < count; index++) {
        char b[1024];
        memset(b, '\0', sizeof(b));

		name      = ladder->getNameIObyIndex(index);
		detailsIO = ladder->getDetailsIO(name);

		const char *type = ladder->getStringTypeIO(index);
		const char *pin  = ladder->getPinNameIO(index).c_str();

        sprintf(b, "                             |                    | %s\n", pin);

		memcpy(b+2 , name.c_str(), name.size());
        memcpy(b+31, type        , strlen(type));
        fprintf(f, "%s", b);
    }

    fclose(f);

    // we may have trashed the grid tables a bit; a repaint will fix that
    InvalidateRect(MainWindow, NULL, FALSE);
}
