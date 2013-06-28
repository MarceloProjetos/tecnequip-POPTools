#include "poptools.h"

#define USE_BINFMT 1

// Magic number to help us to identify if the file is valid.
// Format: xxxxyyzz
// Where:
// xxxx = always 0f5a
// yy   = flags. reserved for future use.
// zz   = format version
const unsigned long int  BINFMT_MAGIC      = 0x0f5a0007;
const unsigned long int  BINFMT_MAGIC_MASK = 0xffff0000;
#define BINFMT_GET_VERSION(m) (m & 0xff)

//-----------------------------------------------------------------------------
// Load a project from a saved project description files. This describes the
// program, the target processor, plus certain configuration settings (cycle
// time, processor clock, etc.). Return TRUE for success, FALSE if anything
// went wrong.
//-----------------------------------------------------------------------------
BOOL LoadProjectFromFile(char *filename)
{
	unsigned long int magic;

    FreeEntireProgram();

	char szAppPath[MAX_PATH]  = "";
	char szfilename[MAX_PATH] = "";
    FILE *f;

	if(filename[0] == '\\' || filename[1] == ':') { // Absolute path
		strcpy(szfilename, filename);
	} else { // Relative path
		::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
		// Extract directory
		strncpy(szfilename, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
		sprintf(szfilename, "%s\\%s", szfilename, filename);
	}

	f = fopen(szfilename, "rb");
    if(!f) return FALSE;

	// Start with the magic number
	fread(&magic, sizeof(magic), 1, f);
	if((magic ^ BINFMT_MAGIC) & BINFMT_MAGIC_MASK) { // Bad magic number. Maybe it is old format?
	} else {
	}

	fclose(f);
    return TRUE;
}

//-----------------------------------------------------------------------------
// Save the program in memory to the given file. Returns TRUE for success,
// FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL SaveProjectToFile(char *filename)
{
	char szfilename[MAX_PATH+1];
    FILE *f;

#ifndef _DEBUG
	Prog.settings.canSave = TRUE;
#endif

	// When the line bellow is active, saved files cannot be overwrited.
//	Prog.settings.canSave=FALSE;

	GetFullPathName(filename, MAX_PATH, szfilename, NULL);
	f = fopen(szfilename, USE_BINFMT ? "wb+" : "w");
    if(!f) return FALSE;

	fclose(f);
    return TRUE;
}

void SetAutoSaveInterval(int interval)
{
	KillTimer(MainWindow, TIMER_AUTOSAVE);
	if(interval) {
		SetTimer(MainWindow, TIMER_AUTOSAVE, interval*60000, AutoSaveNow);
	}
}

void CALLBACK AutoSaveNow(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
	if(strlen(CurrentSaveFile) && Prog.ParallelStart == NULL && Prog.settings.canSave) {
		char AutoSaveName[MAX_PATH];
		strcpy(AutoSaveName, CurrentSaveFile);
		ChangeFileExtension(AutoSaveName, "bld");
		SaveProjectToFile(AutoSaveName);
	}
}
