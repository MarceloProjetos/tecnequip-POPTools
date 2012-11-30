#include "poptools.h"
#include "KeyboardMap.h"

void KeyboardMap::Init(void)
{
	pMapList    = NULL;
	pCurrentMap = NULL;
}

void KeyboardMap::ClearMaps(void)
{
	sKeyBindings *pCurrentKeys, *pTmpKeys;
	sKeyMap      *pCurrentMapList;

	while(pMapList != NULL) {
		pCurrentKeys = pMapList->pKeyList;
		while(pCurrentKeys != NULL) {
			pTmpKeys = pCurrentKeys;
			pCurrentKeys = pCurrentKeys->pNext;
			delete pTmpKeys;
		}

		pCurrentMapList = pMapList;
		pMapList = pMapList->pNext;
		delete pCurrentMapList;
	}
}

void KeyboardMap::CreateMap(int iMapID)
{
	struct sKeyMap *pMap, *pNew, *pLast = NULL;

	pMap = pMapList;
	while(pMap != NULL) {
		if(pMap->iMapID == iMapID) return; // Map already created!

		pLast = pMap;
		pMap  = pMap->pNext;
	}

	pNew = new sKeyMap;

	pNew->iMapID   = iMapID;
	pNew->pKeyList = NULL;
	pNew->pNext    = NULL;

	if(pLast == NULL) {
		pMapList = pNew;
	} else {
		pLast->pNext = pNew;
	}
}

void KeyboardMap::AddKey(int iKey, int iFlags, int (*pFnc)(void *pUserData), void *pUserData)
{
	struct sKeyBindings *pLast, *pNew;

	if(pFnc == NULL || pCurrentMap == NULL) return;

	pNew = new sKeyBindings;

	pNew->iKey      = iKey;
	pNew->iFlags    = iFlags;
	pNew->pFnc      = pFnc;
	pNew->pUserData = pUserData;
	pNew->pNext     = NULL;
	
	if(pCurrentMap->pKeyList == NULL) {
		pCurrentMap->pKeyList = pNew;
	} else {
		pLast = pCurrentMap->pKeyList;
		while(pLast->pNext != NULL) // Search for the last item
			pLast = pLast->pNext;
		pLast->pNext = pNew;
	}
}

int KeyboardMap::Execute(int iKey)
{
	int iFlags = 0, iRet = 0;
	sKeyBindings *pCurrent;

	if(pCurrentMap != NULL) {
		pCurrent = pCurrentMap->pKeyList;

		if(GetAsyncKeyState(VK_SHIFT  ) & 0x8000) iFlags |= KBM_KEYFLAG_SHIFT;
		if(GetAsyncKeyState(VK_CONTROL) & 0x8000) iFlags |= KBM_KEYFLAG_CONTROL;

		while(pCurrent != NULL) {
			if(pCurrent->iKey == iKey && (pCurrent->iFlags == iFlags || pCurrent->iFlags == KBM_KEYFLAG_ANY)) {
				iRet = (pCurrent->pFnc)(pCurrent->pUserData);
				if(iRet) break;
			}

			pCurrent = pCurrent->pNext;
		}
	}

	return iRet;
}

void KeyboardMap::SetMap(int iMapID)
{
	struct sKeyMap *pMap;

	pMap = pMapList;
	while(pMap != NULL) {
		if(pMap->iMapID == iMapID) {
			pCurrentMap = pMap;
			return; // Map found!
		}

		pMap  = pMap->pNext;
	}

	pCurrentMap = NULL;
}
