#include "poptools.h"

void OpenCHM(unsigned int hID)
{
	char szParam   [MAX_PATH] = "";
	char szAppPath [MAX_PATH] = "";
	char szHelpFile[MAX_PATH] = "";

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	strncpy(szHelpFile, szAppPath, strrchr(szAppPath, '\\') - szAppPath);
	strcat(szHelpFile, "\\Ajuda.chm");

	if(hID) {
		sprintf(szParam, "-mapid %d %s", hID, szHelpFile);
	} else {
		strcpy(szParam, szHelpFile);
	}

	ShellExecute(0,"open","HH",szParam,NULL,SW_SHOW);
}

void OpenCHM(void)
{
	OpenCHM(0);
}
