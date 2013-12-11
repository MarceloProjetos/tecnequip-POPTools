#include "poptools.h"

// Buffer that we write to when exporting (drawing) diagram to a text file. 
// Dynamically allocated so that we're at least slightly efficient.
static char **ExportBuffer;

//-----------------------------------------------------------------------------
// DrawChars function, for drawing to the export buffer instead of to the
// screen.
//-----------------------------------------------------------------------------
static void DrawCharsToExportBuffer(int cx, int cy, const char *str)
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

	ladder->DrawTXT(DrawCharsToExportBuffer);

    FILE *f = fopen(file, "w");
    if(!f) {
        Error(_("Não pode abrir o arquivo '%s'\n"), f);
        return;
    }

    fprintf(f, _("POPTools - Diagrama Ladder exportado\n\n"));

	McuIoInfo *mcu = ladder->getMCU();
	LadderSettingsGeneral settings = ladder->getSettingsGeneral();

	LadderSettingsInformation info = ladder->getSettingsInformation();
	fprintf(f, _("Name........: %s\n"), info.Name.c_str());
	fprintf(f, _("Description.: %s\n"), info.Description.c_str());
	fprintf(f, _("Developer...: %s\n"), info.Developer.c_str());
	fprintf(f, _("Build Number: %ld\n\n"), info.BuildNumber);

    fprintf(f, _("\nDiagrama Ladder:\n\n"));

    for(i = 0; i < totalHeight; i++) {
        ExportBuffer[i][l-3] = '|';
        fprintf(f, "%s\n", ExportBuffer[i]);
        CheckFree(ExportBuffer[i]);
    }
    CheckFree(ExportBuffer);
    ExportBuffer = NULL;

    fprintf(f, _("\n\nE/S ATRIBUIDA:\n\n"));
    
    fprintf(f, _("  Nome                       | Tipo               | Pino\n"));
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

	ladder->ShowDialog(eDialogType_Message, false, _("Sucesso"), _("Arquivo gerado com sucesso!"));
}
