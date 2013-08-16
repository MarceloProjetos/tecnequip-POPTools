#include "poptools.h"

void (*DrawChars)(int, int, char *);

// Buffer that we write to when exporting (drawing) diagram to a text file. 
// Dynamically allocated so that we're at least slightly efficient.
static char **ExportBuffer;

// Parameters that determine our offset if we are scrolled
int ScrollXOffset;
int ScrollYOffset;
int ScrollXOffsetMax;
int ScrollYOffsetMax;

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
