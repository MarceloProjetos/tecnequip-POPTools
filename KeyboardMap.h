#ifndef KEYBOARD_MAP_H
#define KEYBOARD_MAP_H

#define KBM_KEYFLAG_NONE    0x00
#define KBM_KEYFLAG_CONTROL 0x01
#define KBM_KEYFLAG_SHIFT   0x02
#define KBM_KEYFLAG_ALT     0x04
#define KBM_KEYFLAG_ANY     0xFF

class KeyboardMap {
private:
	// Private Members
	struct sKeyBindings {
		int                   iKey;
		int                   iFlags;
		int                 (*pFnc)(void *pUserData);
		void                 *pUserData;
		struct sKeyBindings  *pNext;
	};

	struct sKeyMap {
		int           iMapID;
		sKeyBindings *pKeyList; // List of keys
		sKeyMap      *pNext;
	} *pMapList;

	struct sKeyMap *pCurrentMap;

	// Private Methods
	void Init(void);

public:
	// Constructors
	KeyboardMap(void) { Init(); }

	// Destructor
	~KeyboardMap(void) { ClearMaps(); }

	// Public Methods
	void ClearMaps(void);
	void AddKey   (int iKey, int iFlags, int (*pFnc)(void *pUserData), void *pUserData);
	void CreateMap(int iMapID);
	int  Execute  (int iKey);
	void SetMap   (int iMapID);
};

#endif