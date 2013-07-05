#include "poptools.h"

// Funcoes para ler e gravar dados de um arquivo
// Centralizando a leitura e gravacao de tipos padrao em funcoes especificas
// diminui a redundancia do codigo e torna o codigo mais portavel e simples

bool fwrite_uint(FILE *f, unsigned int var)
{
	return 1 == fwrite(&var, sizeof(var), 1, f);
}

bool fwrite_int(FILE *f, int var)
{
	return 1 == fwrite(&var, sizeof(var), 1, f);
}

bool fwrite_ulong(FILE *f, unsigned long var)
{
	return 1 == fwrite(&var, sizeof(var), 1, f);
}

bool fwrite_long(FILE *f, long var)
{
	return 1 == fwrite(&var, sizeof(var), 1, f);
}

bool fwrite_float(FILE *f, float var)
{
	return 1 == fwrite(&var, sizeof(var), 1, f);
}

bool fwrite_uchar(FILE *f, unsigned char var)
{
	return 1 == fwrite(&var, sizeof(var), 1, f);
}

bool fwrite_bool(FILE *f, bool var)
{
	return fwrite_uchar(f, var);
}

bool fwrite_string(FILE *f, string var)
{
	if(fwrite_uint(f, var.size())) {
		size_t size = sizeof(char) * var.size();
		return (size == 0) || (1 == fwrite(var.c_str(), size, 1, f));
	}

	return false;
}

bool fwrite_time_t(FILE *f, time_t var)
{
	return 1 == fwrite(&var, sizeof(var), 1, f);
}

bool fwrite_pointer(FILE *f, void *var, unsigned int size)
{
	return 1 == fwrite(var, size, 1, f);
}

bool fread_uint(FILE *f, unsigned int *var)
{
	return 1 == fread(var, sizeof(*var), 1, f);
}

bool fread_int(FILE *f, int *var)
{
	return 1 == fread(var, sizeof(*var), 1, f);
}

bool fread_ulong(FILE *f, unsigned long *var)
{
	return 1 == fread(var, sizeof(*var), 1, f);
}

bool fread_long(FILE *f, long *var)
{
	return 1 == fread(var, sizeof(*var), 1, f);
}

bool fread_float(FILE *f, float *var)
{
	return 1 == fread(var, sizeof(*var), 1, f);
}

bool fread_uchar(FILE *f, unsigned char *var)
{
	return 1 == fread(var, sizeof(*var), 1, f);
}

bool fread_bool(FILE *f, bool *var)
{
	unsigned char ch;
	if(fread_uchar(f, &ch)) {
		*var = ch != 0 ? true : false;
		return true;
	}

	return false;
}

bool fread_string(FILE *f, string *var)
{
	unsigned int n;
	if(fread_uint(f, &n)) {
		if(n > 0) {
			char *buffer = new char [n + 1];
			size_t size = sizeof(char) * n;
			if(1 == fread(buffer, n, 1, f)) {
				buffer[n] = 0;
				*var = buffer;

				delete [] buffer;

				return true;
			}
		} else {
			*var = "";
			return true;
		}
	}

	return false;
}

bool fread_time_t(FILE *f, time_t *var)
{
	return 1 == fread(var, sizeof(*var), 1, f);
}

bool fread_pointer(FILE *f, void *var, unsigned int size)
{
	return 1 == fread(var, size, 1, f);
}

//-----------------------------------------------------------------------------
// Load a project from a saved project description files. This describes the
// program, the target processor, plus certain configuration settings (cycle
// time, processor clock, etc.). Return TRUE for success, FALSE if anything
// went wrong.
//-----------------------------------------------------------------------------
BOOL LoadProjectFromFile(char *filename)
{
	char szAppPath[MAX_PATH]  = "";
	char szfilename[MAX_PATH] = "";

	if(filename[0] == '\\' || filename[1] == ':') { // Absolute path
		strcpy(szfilename, filename);
	} else { // Relative path
		::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
		// Extract directory
		strncpy(szfilename, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
		sprintf(szfilename, "%s\\%s", szfilename, filename);
	}

	return ladder->Load(szfilename);
}

//-----------------------------------------------------------------------------
// Save the program in memory to the given file. Returns TRUE for success,
// FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL SaveProjectToFile(char *filename, bool isBackup)
{
	char szfilename[MAX_PATH+1];

	GetFullPathName(filename, MAX_PATH, szfilename, NULL);

	return ladder->Save(szfilename, isBackup);
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
	string currentFilename = ladder->getCurrentFilename();
	LadderSettingsGeneral settings = ladder->getSettingsGeneral();
	if(currentFilename.size() > 0 && settings.canSave) {
		char AutoSaveName[MAX_PATH];
		strcpy(AutoSaveName, currentFilename.c_str());
		ChangeFileExtension(AutoSaveName, "bld");
		SaveProjectToFile(AutoSaveName, true);
	}
}
