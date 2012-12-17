#include "poptools.h"

//PURPOSE: Implement the properties that describe a Recent Item to the Windows Ribbon
class CRecentFileProperties
    : public IUISimplePropertySet
{
public:

    // Static method to create an instance of the object.
    __checkReturn static HRESULT CreateInstance(__in PWSTR wszFullPath, __deref_out_opt CRecentFileProperties **ppProperties)
    {
        if (!wszFullPath || !ppProperties)
        {
            return E_POINTER;
        }

        *ppProperties = NULL;

        HRESULT hr;

        CRecentFileProperties* pProperties = new CRecentFileProperties();

        if (pProperties != NULL)
        {
            hr = ::StringCchCopyW(pProperties->m_wszFullPath, MAX_PATH, wszFullPath);
            SHFILEINFOW sfi;

            DWORD_PTR dwPtr = NULL;
            if (SUCCEEDED(hr))
            {
                dwPtr = ::SHGetFileInfoW(wszFullPath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_USEFILEATTRIBUTES);
            }

            if (dwPtr != NULL)
            {
                hr = ::StringCchCopyW(pProperties->m_wszDisplayName, MAX_PATH, sfi.szDisplayName);
            }
            else // Provide a reasonable fallback.
            {
                hr = ::StringCchCopyW(pProperties->m_wszDisplayName, MAX_PATH, pProperties->m_wszFullPath);
            }
        }
        else
        {
            hr = E_OUTOFMEMORY;
        }

        if (SUCCEEDED(hr))
        {
            *ppProperties = pProperties;
            (*ppProperties)->AddRef();
        }
        
        if (pProperties)
        {
            pProperties->Release();
        }

        return hr;
    }

    // IUnknown methods.
    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release()
    {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
        {
            delete this;
        }

        return cRef;
    }

    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
    {
        if (!ppv)
        {
            return E_POINTER;
        }

        if (iid == __uuidof(IUnknown))
        {
            *ppv = static_cast<IUnknown*>(this);
        }
        else if (iid == __uuidof(IUISimplePropertySet))
        {
            *ppv = static_cast<IUISimplePropertySet*>(this);
        }
        else 
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    // IUISimplePropertySet methods.
    STDMETHODIMP GetValue(__in REFPROPERTYKEY key, __out PROPVARIANT *value)
    {
        HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

        if (key == UI_PKEY_Label)
        {
            hr = UIInitPropertyFromString(UI_PKEY_Label, m_wszDisplayName, value);
        }
        else if (key == UI_PKEY_LabelDescription)
        {
            hr = UIInitPropertyFromString(UI_PKEY_LabelDescription, m_wszFullPath, value);
        }

        return hr;
    }

private:
    CRecentFileProperties()
        : m_cRef(1)
    {
        m_wszFullPath[0] = L'\0';
        m_wszDisplayName[0] = L'\0';
    }

    LONG m_cRef;                        // Reference count.
    WCHAR m_wszDisplayName[MAX_PATH];
    WCHAR m_wszFullPath[MAX_PATH];
};

HRESULT PopulateRibbonRecentItems(__deref_out PROPVARIANT* pvarValue)
{
	int iFileCount;
    LONG iCurrentFile = 0;
    HRESULT hr = E_FAIL;

	for(iFileCount=0; iFileCount<MAX_RECENT_ITEMS && POPSettings.recent_list[iFileCount][0]; iFileCount++)
		;

	SAFEARRAY* psa = SafeArrayCreateVector(VT_UNKNOWN, 0, iFileCount);

    if (psa != NULL)
    {
        while (iCurrentFile < iFileCount)
        {
            WCHAR wszCurrentFile[MAX_PATH] = {0};
			if (0 != ::MultiByteToWideChar(CP_ACP, NULL, POPSettings.recent_list[iCurrentFile], -1, wszCurrentFile, MAX_PATH))
            {
                CRecentFileProperties* pPropertiesObj;
                hr = CRecentFileProperties::CreateInstance(wszCurrentFile, &pPropertiesObj);

                if (SUCCEEDED(hr))
                {
                    IUnknown* pUnk = NULL;

#pragma warning( disable : 6011)    // pPropertiesObj cannot be NULL.
                    hr = pPropertiesObj->QueryInterface(__uuidof(IUnknown), reinterpret_cast<void**>(&pUnk));
#pragma warning( default : 6011)    //

                    if (SUCCEEDED(hr))
                    {
                        hr = SafeArrayPutElement(psa, &iCurrentFile, static_cast<void*>(pUnk));
                        pUnk->Release();
                    }
                }

                if (pPropertiesObj)
                {
                    pPropertiesObj->Release();
                }

                if (FAILED(hr))
                {
                    break;
                }

                iCurrentFile ++;
            }
            else
            {
                break;
            }
        }

        // We will only populate items up to before the first failed item, and discard the rest.
        SAFEARRAYBOUND sab = {iCurrentFile,0};
        SafeArrayRedim(psa, &sab);
        hr = UIInitPropertyFromIUnknownArray(UI_PKEY_RecentItems, psa, pvarValue);

        SafeArrayDestroy(psa);
    }

    // Note that this result could be S_OK even when we encounter errors when
    // populating the list of items.
    return hr;
}

/****************************************
****                                 ****
***   End of MRU Classes Definition   ***
****                                 ****
****************************************/

#define MAX_ITEMS 100

typedef struct {
	PCWSTR CatName;
	int    ListID[MAX_ITEMS];
} GalleryItems;

IUIFramework* g_pFramework = NULL;

// The code below implements a list where the current state of ribbon' comands are saved. We have to do this because
// we get an error when trying to set the current state for a command that is being created or was not created yet.
// This occurs for modal tabs, for example.

struct strCmdStateList {
	UINT uCmdID;
	BOOL state;
	struct strCmdStateList *next;
} CmdStateList;

BOOL RibbonGetCmdState(UINT uCmdID)
{
	struct strCmdStateList *sCurrCmd = &CmdStateList;

	while(sCurrCmd) {
		if(sCurrCmd->uCmdID == uCmdID)
			return sCurrCmd->state; // Yes!

		sCurrCmd = sCurrCmd->next;
	}

	return TRUE;
}

HRESULT RibbonSetCmdState(UINT uCmdID, BOOL bEnabled)
{
	HRESULT hr;
    PROPVARIANT varNew;
	struct strCmdStateList *sCurrCmd = &CmdStateList, *sPrevCmd = NULL;

	// Is it already in the list?
	while(sCurrCmd) {
		if(sCurrCmd->uCmdID == uCmdID)
			break; // Yes!

		sPrevCmd = sCurrCmd;
		sCurrCmd = sCurrCmd->next;
	}

	if(sCurrCmd == NULL) { // uCmdID is not in the list, lets add a new item.
		// Empty list?
		if(!CmdStateList.uCmdID) { // Yes, lets use the first item.
			sCurrCmd = &CmdStateList;
		} else { // No, we have to alocate memory for a new item.
			sCurrCmd = new strCmdStateList;
			sPrevCmd->next = sCurrCmd;
			sCurrCmd->next = NULL;
		}

		sCurrCmd->uCmdID = uCmdID;
	}

	sCurrCmd->state = bEnabled;

	// Now state is saved to the list, lets try to update the property...
	hr = UIInitPropertyFromBoolean(UI_PKEY_Enabled, bEnabled, &varNew);

	if (SUCCEEDED(hr)) {
	    hr = g_pFramework->SetUICommandProperty(uCmdID, UI_PKEY_Enabled, varNew);
    }

	PropVariantClear(&varNew);

	return hr;
}

// Height of Ribbon in Main Window, used to know where to start to draw ladder logic
unsigned int RibbonHeight = 0;

class CApplication
    : public CComObjectRootEx<CComMultiThreadModel>
    , public IUIApplication
    , public IUICommandHandler
{
public:
    BEGIN_COM_MAP(CApplication)
        COM_INTERFACE_ENTRY(IUIApplication)
        COM_INTERFACE_ENTRY(IUICommandHandler)
    END_COM_MAP()

	CApplication(void)
	{
		m_pifbFactory = NULL;
	}

    STDMETHOD(OnViewChanged)(UINT32 nViewID, __in UI_VIEWTYPE typeID, __in IUnknown* pView, UI_VIEWVERB verb, INT32 uReasonCode)  
    { 
        HRESULT hr;

        // The Ribbon size has changed.
		if (verb == UI_VIEWVERB_SIZE)
        {
            CComQIPtr<IUIRibbon> pRibbon = pView;
            if (!pRibbon)
                return E_FAIL;

            // Get the Ribbon height.
            hr = pRibbon->GetHeight(&RibbonHeight);
            if (FAILED(hr))
                return hr;

            // Update the Ribbon bar so that the main frame can recalculate the child layout.
		    RefreshScrollbars();
			RefreshDrawWindow();
	        InvalidateRect(MainWindow, NULL, FALSE);
        }

        return S_OK;
    }

    STDMETHOD(OnCreateUICommand)(UINT32 nCmdID, __in UI_COMMANDTYPE typeID, __deref_out IUICommandHandler** ppCommandHandler) 
    { 
		return QueryInterface(IID_PPV_ARGS(ppCommandHandler));
    }

    STDMETHOD(OnDestroyUICommand)(UINT32 commandId, __in UI_COMMANDTYPE typeID, __in_opt IUICommandHandler* pCommandHandler) 
    { 
        return E_NOTIMPL; 
    }

    STDMETHODIMP Execute(UINT nCmdID,
        UI_EXECUTIONVERB verb, 
        __in_opt const PROPERTYKEY* key,
        __in_opt const PROPVARIANT* ppropvarValue,
        __in_opt IUISimplePropertySet* pCommandExecutionProperties)
    {
		int opcode = 0, OpenHelp = 0;
        HRESULT hr = S_OK;

        if(verb == UI_EXECUTIONVERB_EXECUTE) {
			// If the user is pressing CONTROL key, we should to
			// open UserManual page related to selected command.
			if(GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				OpenHelp = 1;
			}

			switch(nCmdID) {
			case cmdFindText:
				opcode=0;
				break;

			case cmdRecentList:
				int i;
				char *szFile;
				PROPVARIANT vFile;

				pCommandExecutionProperties->GetValue(UI_PKEY_LabelDescription, &vFile);
				for(i=0; i<MAX_RECENT_ITEMS; i++) {
					szFile = _com_util::ConvertBSTRToString(vFile.bstrVal);
					if(!strcmp(szFile, POPSettings.recent_list[i])) {
						delete [] szFile;
						break;
					}

					delete [] szFile;
				}

				if(i<MAX_RECENT_ITEMS) {
					opcode = MNU_RECENT_START+i;
				}

				OpenHelp = 0;

				break;

#define EXEC_OPCODE(cmd,op) case cmd: opcode = op; break;

			EXEC_OPCODE(cmdFileNew                , MNU_NEW                    );
			EXEC_OPCODE(cmdFileOpen               , MNU_OPEN                   );
			EXEC_OPCODE(cmdFileSave               , MNU_SAVE                   );
			EXEC_OPCODE(cmdFileSaveAsLadder       , MNU_SAVE_AS                );
			EXEC_OPCODE(cmdFileSaveAsC            , MNU_SAVE_AS_C              );
			EXEC_OPCODE(cmdFileSaveAsText         , MNU_EXPORT                 );
			EXEC_OPCODE(cmdAbout                  , MNU_ABOUT                  );
			EXEC_OPCODE(cmdExit                   , MNU_EXIT                   );

			EXEC_OPCODE(cmdUndo                   , MNU_UNDO                   );
			EXEC_OPCODE(cmdRedo                   , MNU_REDO                   );
			EXEC_OPCODE(cmdMoveLineUp             , MNU_PUSH_RUNG_UP           );
			EXEC_OPCODE(cmdMoveLineDown           , MNU_PUSH_RUNG_DOWN         );
			EXEC_OPCODE(cmdInsertLineAfter        , MNU_INSERT_RUNG_AFTER      );
			EXEC_OPCODE(cmdInsertLineBefore       , MNU_INSERT_RUNG_BEFORE     );
			EXEC_OPCODE(cmdGoHome                 , MNU_GO_HOME                );
			EXEC_OPCODE(cmdGoEnd                  , MNU_GO_END                 );
			EXEC_OPCODE(cmdInstrParallel          , MNU_INSERT_PARALLEL        );
			EXEC_OPCODE(cmdCopyInstr              , MNU_COPY_ELEMENT           );
			EXEC_OPCODE(cmdPasteInstr             , MNU_PASTE_ELEMENT          );
			EXEC_OPCODE(cmdDeleteInstr            , MNU_DELETE_ELEMENT         );
			EXEC_OPCODE(cmdDeleteLine             , MNU_DELETE_RUNG            );
			EXEC_OPCODE(cmdFindDialog             , MNU_FIND_AND_REPLACE       );
			EXEC_OPCODE(cmdFind                   , MNU_FIND_AND_REPLACE_NEXT  );

			EXEC_OPCODE(cmdInstrComment           , MNU_INSERT_COMMENT         );

			EXEC_OPCODE(cmdInstrContact           , MNU_INSERT_CONTACTS        );
			EXEC_OPCODE(cmdInstrCoil              , MNU_INSERT_COIL            );
			EXEC_OPCODE(cmdInstrOneShotFalling    , MNU_INSERT_OSF             );
			EXEC_OPCODE(cmdInstrOneShotRising     , MNU_INSERT_OSR             );
			EXEC_OPCODE(cmdInstrCircuitOpen       , MNU_INSERT_OPEN            );
			EXEC_OPCODE(cmdInstrCircuitClosed     , MNU_INSERT_SHORT           );
			EXEC_OPCODE(cmdInstrMasterRelayControl, MNU_INSERT_MASTER_RLY      );

			EXEC_OPCODE(cmdInstrTimerON           , MNU_INSERT_TON             );
			EXEC_OPCODE(cmdInstrTimerOFF          , MNU_INSERT_TOF             );
			EXEC_OPCODE(cmdInstrTimerONRet        , MNU_INSERT_RTO             );
			EXEC_OPCODE(cmdInstrSchedule          , MNU_INSERT_RTC             );
			EXEC_OPCODE(cmdInstrTimerReset        , MNU_INSERT_RES             );

			EXEC_OPCODE(cmdInstrCounterInc        , MNU_INSERT_CTU             );
			EXEC_OPCODE(cmdInstrCounterDec        , MNU_INSERT_CTD             );
			EXEC_OPCODE(cmdInstrCounterCirc       , MNU_INSERT_CTC             );
			EXEC_OPCODE(cmdInstrCounterReset      , MNU_INSERT_RES             );

			EXEC_OPCODE(cmdInstrMathAdd           , MNU_INSERT_ADD             );
			EXEC_OPCODE(cmdInstrMathSub           , MNU_INSERT_SUB             );
			EXEC_OPCODE(cmdInstrMathMult          , MNU_INSERT_MUL             );
			EXEC_OPCODE(cmdInstrMathDivide        , MNU_INSERT_DIV             );
			EXEC_OPCODE(cmdInstrMathSqrt          , MNU_INSERT_SQRT            );
			EXEC_OPCODE(cmdInstrShiftRegister     , MNU_INSERT_SHIFT_REG       );
			EXEC_OPCODE(cmdInstrLookUpTable       , MNU_INSERT_LUT             );
			EXEC_OPCODE(cmdInstrPieceWiseLinear   , MNU_INSERT_PWL             );
			EXEC_OPCODE(cmdInstrRand              , MNU_INSERT_RAND            );
			EXEC_OPCODE(cmdInstrAbs               , MNU_INSERT_ABS             );

			EXEC_OPCODE(cmdInstrMov               , MNU_INSERT_MOV             );
			EXEC_OPCODE(cmdInstrSetBit            , MNU_INSERT_SET_BIT         );
			EXEC_OPCODE(cmdInstrCheckBit          , MNU_INSERT_CHECK_BIT       );
			EXEC_OPCODE(cmdInstrPersist           , MNU_INSERT_PERSIST         );

			EXEC_OPCODE(cmdInstrCondEqual         , MNU_INSERT_EQU             );
			EXEC_OPCODE(cmdInstrCondNotEqual      , MNU_INSERT_NEQ             );
			EXEC_OPCODE(cmdInstrCondGreater       , MNU_INSERT_GRT             );
			EXEC_OPCODE(cmdInstrCondGreaterEqual  , MNU_INSERT_GEQ             );
			EXEC_OPCODE(cmdInstrCondLesser        , MNU_INSERT_LES             );
			EXEC_OPCODE(cmdInstrCondLesserEqual   , MNU_INSERT_LEQ             );

			EXEC_OPCODE(cmdInstrReadAD            , MNU_INSERT_READ_ADC        );
			EXEC_OPCODE(cmdInstrWriteDA           , MNU_INSERT_SET_DA          );

			EXEC_OPCODE(cmdInstrPWM               , MNU_INSERT_SET_PWM         );
			EXEC_OPCODE(cmdInstrRampDA            , MNU_INSERT_MULTISET_DA     );
			EXEC_OPCODE(cmdInstrReadEnc           , MNU_INSERT_READ_ENC        );
			EXEC_OPCODE(cmdInstrResetEnc          , MNU_INSERT_RESET_ENC       );

			EXEC_OPCODE(cmdInstrWriteChar         , MNU_INSERT_UART_SEND       );
			EXEC_OPCODE(cmdInstrReadChar          , MNU_INSERT_UART_RECV       );
			EXEC_OPCODE(cmdInstrReadFormatString  , MNU_READ_FMTD_STR          );
			EXEC_OPCODE(cmdInstrWriteFormatString , MNU_WRITE_FMTD_STR         );

			EXEC_OPCODE(cmdInstrReadUSS           , MNU_INSERT_READ_USS        );
			EXEC_OPCODE(cmdInstrWriteUSS          , MNU_INSERT_WRITE_USS       );
			EXEC_OPCODE(cmdInstrReadNS600         , MNU_READ_SERVO_YASKAWA     );
			EXEC_OPCODE(cmdInstrWriteNS600        , MNU_WRITE_SERVO_YASKAWA    );

			EXEC_OPCODE(cmdInstrReadModBUS485     , MNU_INSERT_READ_MODBUS     );
			EXEC_OPCODE(cmdInstrWriteModBUS485    , MNU_INSERT_WRITE_MODBUS    );
			EXEC_OPCODE(cmdInstrReadModBUSEth     , MNU_INSERT_READ_MODBUS_ETH );
			EXEC_OPCODE(cmdInstrWriteModBUSEth    , MNU_INSERT_WRITE_MODBUS_ETH);

			EXEC_OPCODE(cmdConfig                 , MNU_MCU_SETTINGS           );
			EXEC_OPCODE(cmdSettings               , MNU_MCU_PREFERENCES        );

			EXEC_OPCODE(cmdSimulation             , MNU_SIMULATION_MODE        );
			EXEC_OPCODE(cmdSimulationExit         , MNU_SIMULATION_MODE        );
			EXEC_OPCODE(cmdSimulationStart        , MNU_START_SIMULATION       );
			EXEC_OPCODE(cmdSimulationPause        , MNU_PAUSE_SIMULATION       );
			EXEC_OPCODE(cmdSimulationStop         , MNU_STOP_SIMULATION        );
			EXEC_OPCODE(cmdSimulationSingleCycle  , MNU_SINGLE_CYCLE           );

			EXEC_OPCODE(cmdCompile                , MNU_COMPILE                );
			EXEC_OPCODE(cmdCompileAndProgram      , MNU_PROGRAM                );

			EXEC_OPCODE(cmdDebug                  , MNU_DEBUG                  );

			EXEC_OPCODE(cmdUserManual             , MNU_MANUAL                 );
			EXEC_OPCODE(cmdKeyboardManual         , MNU_KEYBOARD_MANUAL        );

			// Example comands list
			EXEC_OPCODE(cmdExampleComment           , MNU_EXAMPLE_COMMENT         );

			EXEC_OPCODE(cmdExampleContact           , MNU_EXAMPLE_CONTACTS        );
			EXEC_OPCODE(cmdExampleCoil              , MNU_EXAMPLE_COIL            );
			EXEC_OPCODE(cmdExampleOneShotFalling    , MNU_EXAMPLE_OSF             );
			EXEC_OPCODE(cmdExampleOneShotRising     , MNU_EXAMPLE_OSR             );
			EXEC_OPCODE(cmdExampleCircuitOpen       , MNU_EXAMPLE_OPEN            );
			EXEC_OPCODE(cmdExampleCircuitClosed     , MNU_EXAMPLE_SHORT           );
			EXEC_OPCODE(cmdExampleMasterRelayControl, MNU_EXAMPLE_MASTER_RLY      );

			EXEC_OPCODE(cmdExampleTimerON           , MNU_EXAMPLE_TON             );
			EXEC_OPCODE(cmdExampleTimerOFF          , MNU_EXAMPLE_TOF             );
			EXEC_OPCODE(cmdExampleTimerONRet        , MNU_EXAMPLE_RTO             );
			EXEC_OPCODE(cmdExampleSchedule          , MNU_EXAMPLE_RTC             );
			EXEC_OPCODE(cmdExampleTimerReset        , MNU_EXAMPLE_RES             );

			EXEC_OPCODE(cmdExampleCounterInc        , MNU_EXAMPLE_CTU             );
			EXEC_OPCODE(cmdExampleCounterDec        , MNU_EXAMPLE_CTD             );
			EXEC_OPCODE(cmdExampleCounterCirc       , MNU_EXAMPLE_CTC             );
			EXEC_OPCODE(cmdExampleCounterReset      , MNU_EXAMPLE_RES             );

			EXEC_OPCODE(cmdExampleMathAdd           , MNU_EXAMPLE_ADD             );
			EXEC_OPCODE(cmdExampleMathSub           , MNU_EXAMPLE_SUB             );
			EXEC_OPCODE(cmdExampleMathMult          , MNU_EXAMPLE_MUL             );
			EXEC_OPCODE(cmdExampleMathDivide        , MNU_EXAMPLE_DIV             );
			EXEC_OPCODE(cmdExampleMathSqrt          , MNU_EXAMPLE_SQRT            );
			EXEC_OPCODE(cmdExampleAbs               , MNU_EXAMPLE_ABS             );
			EXEC_OPCODE(cmdExampleShiftRegister     , MNU_EXAMPLE_SHIFT_REG       );
			EXEC_OPCODE(cmdExampleLookUpTable       , MNU_EXAMPLE_LUT             );
			EXEC_OPCODE(cmdExamplePieceWiseLinear   , MNU_EXAMPLE_PWL             );
			EXEC_OPCODE(cmdExampleRand              , MNU_EXAMPLE_RAND            );

			EXEC_OPCODE(cmdExampleMov               , MNU_EXAMPLE_MOV             );
			EXEC_OPCODE(cmdExampleSetBit            , MNU_EXAMPLE_SET_BIT         );
			EXEC_OPCODE(cmdExampleCheckBit          , MNU_EXAMPLE_CHECK_BIT       );
			EXEC_OPCODE(cmdExamplePersist           , MNU_EXAMPLE_PERSIST         );

			EXEC_OPCODE(cmdExampleCondEqual         , MNU_EXAMPLE_EQU             );
			EXEC_OPCODE(cmdExampleCondNotEqual      , MNU_EXAMPLE_NEQ             );
			EXEC_OPCODE(cmdExampleCondGreater       , MNU_EXAMPLE_GRT             );
			EXEC_OPCODE(cmdExampleCondGreaterEqual  , MNU_EXAMPLE_GEQ             );
			EXEC_OPCODE(cmdExampleCondLesser        , MNU_EXAMPLE_LES             );
			EXEC_OPCODE(cmdExampleCondLesserEqual   , MNU_EXAMPLE_LEQ             );

			EXEC_OPCODE(cmdExampleReadAD            , MNU_EXAMPLE_READ_ADC        );
			EXEC_OPCODE(cmdExampleWriteDA           , MNU_EXAMPLE_SET_DA          );

			EXEC_OPCODE(cmdExamplePWM               , MNU_EXAMPLE_SET_PWM         );
			EXEC_OPCODE(cmdExampleRampDA            , MNU_EXAMPLE_MULTISET_DA     );
			EXEC_OPCODE(cmdExampleReadEnc           , MNU_EXAMPLE_READ_ENC        );
			EXEC_OPCODE(cmdExampleResetEnc          , MNU_EXAMPLE_RESET_ENC       );

			EXEC_OPCODE(cmdExampleWriteChar         , MNU_EXAMPLE_UART_SEND       );
			EXEC_OPCODE(cmdExampleReadChar          , MNU_EXAMPLE_UART_RECV       );
			EXEC_OPCODE(cmdExampleReadFormatString  , MNU_EXAMPLE_READ_FMTD_STR   );
			EXEC_OPCODE(cmdExampleWriteFormatString , MNU_EXAMPLE_WRITE_FMTD_STR  );

			EXEC_OPCODE(cmdExampleReadUSS           , MNU_EXAMPLE_READ_USS        );
			EXEC_OPCODE(cmdExampleWriteUSS          , MNU_EXAMPLE_WRITE_USS       );
			EXEC_OPCODE(cmdExampleReadNS600         , MNU_EXAMPLE_READ_SERVO_YASK );
			EXEC_OPCODE(cmdExampleWriteNS600        , MNU_EXAMPLE_WRITE_SERVO_YASK);

			EXEC_OPCODE(cmdExampleReadModBUS485     , MNU_EXAMPLE_READ_MODBUS     );
			EXEC_OPCODE(cmdExampleWriteModBUS485    , MNU_EXAMPLE_WRITE_MODBUS    );
			EXEC_OPCODE(cmdExampleReadModBUSEth     , MNU_EXAMPLE_READ_MODBUS_ETH );
			EXEC_OPCODE(cmdExampleWriteModBUSEth    , MNU_EXAMPLE_WRITE_MODBUS_ETH);

			EXEC_OPCODE(cmdExampleAdcLed            , MNU_EXAMPLE_ADC_LED         );
			EXEC_OPCODE(cmdExampleSemaphore         , MNU_EXAMPLE_SEMAPHORE       );
			EXEC_OPCODE(cmdExampleTester            , MNU_EXAMPLE_TESTER          );

/*
			case cmdMyChoice:
                PROPVARIANT var, varNew;

                hr = g_pFramework->GetUICommandProperty(cmdMyChoice, UI_PKEY_BooleanValue, &var);
                if (SUCCEEDED(hr)) {
	                hr = PropVariantToBoolean(var, &_fEnabled);
                }
                if (SUCCEEDED(hr)) {
					_fEnabled = !_fEnabled;
					hr = UIInitPropertyFromBoolean(UI_PKEY_Enabled, _fEnabled, &varNew);
                }
                if (SUCCEEDED(hr)) {
	                hr = g_pFramework->SetUICommandProperty(cmdMyButton, UI_PKEY_Enabled, varNew);
                }
                if (SUCCEEDED(hr)) {
	                hr = g_pFramework->InvalidateUICommand(cmdMyChoice, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Label);
				}*/
            }
        }	    

		if (SUCCEEDED(hr) && opcode) {
			if(OpenHelp) {
				OpenCHM(nCmdID);
			} else {
				ProcessMenu(opcode);
		        InvalidateRect(MainWindow, NULL, FALSE);
			}
		}

		return hr;
    }

    STDMETHODIMP UpdateProperty(UINT nCmdID,
        __in REFPROPERTYKEY key,
        __in_opt const PROPVARIANT* ppropvarCurrentValue,
        __out PROPVARIANT* ppropvarNewValue)
    {
        UNREFERENCED_PARAMETER(ppropvarCurrentValue);

        HRESULT hr = E_FAIL;

        if (key == UI_PKEY_Label)
        {
/*            // Update the Label of ToggleButton control
            if (nCmdID == cmdMyChoice)
            {
                if (_fEnabled)
                {
                    hr = UIInitPropertyFromString(UI_PKEY_Label, 
                        L"Disable Button", ppropvarNewValue);
                }
                else
                {
                    hr = UIInitPropertyFromString(UI_PKEY_Label, 
                        L"Enable Button", ppropvarNewValue);
                }
            }*/
		} else if (key == UI_PKEY_RecentItems) {
			if (!ppropvarNewValue)
				return E_INVALIDARG;

			hr = PopulateRibbonRecentItems(ppropvarNewValue);
		} else if (key == UI_PKEY_Enabled) {
			hr = UIInitPropertyFromBoolean(UI_PKEY_Enabled, RibbonGetCmdState(nCmdID), ppropvarNewValue);
		} else if (nCmdID == cmdExamples && (key == UI_PKEY_Categories || key == UI_PKEY_ItemsSource)) {
			GalleryItems gi[] = {
				// TODO: Internacionalizar
				{ L"I/O"            , { cmdExampleContact, cmdExampleCoil, cmdExampleOneShotRising, cmdExampleOneShotFalling, cmdExampleMasterRelayControl, cmdExampleCircuitOpen, cmdExampleCircuitClosed, 0 } },
				{ L"Temporizadores" , { cmdExampleTimerON, cmdExampleTimerOFF, cmdExampleTimerONRet, cmdExampleSchedule, cmdExampleTimerReset, 0 } },
				{ L"Contadores"     , { cmdExampleCounterInc, cmdExampleCounterDec, cmdExampleCounterCirc, cmdExampleCounterReset, 0 } },
				{ L"Variáveis"      , { cmdExampleMov, cmdExampleSetBit, cmdExampleCheckBit, cmdExamplePersist, 0 } },
				{ L"Condicionais"   , { cmdExampleCondEqual, cmdExampleCondGreater, cmdExampleCondLesser, cmdExampleCondNotEqual, cmdExampleCondGreaterEqual, cmdExampleCondLesserEqual, 0 } },
				{ L"Matemática"     , { cmdExampleMathAdd, cmdExampleMathSub, cmdExampleMathMult, cmdExampleMathDivide, cmdExampleAbs, cmdExampleMathSqrt, cmdExampleShiftRegister, cmdExampleLookUpTable, cmdExamplePieceWiseLinear, cmdExampleRand, 0 } },
				{ L"Analógicos"     , { cmdExampleReadAD, cmdExampleWriteDA, 0 } },
				{ L"Motores"        , { cmdExamplePWM, cmdExampleRampDA, cmdExampleReadEnc, cmdExampleResetEnc, 0 } },
				{ L"ModBUS"         , { cmdExampleReadModBUS485, cmdExampleWriteModBUS485, cmdExampleReadModBUSEth, cmdExampleWriteModBUSEth, 0 } },
				{ L"RS-485 - Texto" , { cmdExampleReadFormatString, cmdExampleWriteFormatString, cmdExampleReadChar, cmdExampleWriteChar, 0 } },
				{ L"RS-485 - Outros", { cmdExampleReadNS600, cmdExampleWriteNS600, cmdExampleReadUSS, cmdExampleWriteUSS, 0 } },
				{ L"Aplicações"     , { cmdExampleAdcLed, cmdExampleSemaphore, cmdExampleTester, 0 } },
				{ NULL, NULL }
			};
			hr = PopulateGallery(ppropvarCurrentValue, key, gi);
		}

        return hr;
	}

private:
    BOOL _fEnabled;
    IUIImageFromBitmap* m_pifbFactory;

	// Factory method to create IUIImages from resource identifiers.
	HRESULT CreateUIImageFromBitmapResource(LPCTSTR pszResource, __out IUIImage **ppimg)
	{
		HRESULT hr = E_FAIL;

		*ppimg = NULL;

		if (NULL == m_pifbFactory)
		{
			hr = CoCreateInstance(CLSID_UIRibbonImageFromBitmapFactory, NULL, CLSCTX_ALL, IID_PPV_ARGS(&m_pifbFactory));
			if (FAILED(hr))
			{
				return hr;
			}
		}

		// Load the bitmap from the resource file.
		HBITMAP hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), pszResource, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		if (hbm)
		{
			// Use the factory implemented by the framework to produce an IUIImage.
			hr = m_pifbFactory->CreateImage(hbm, UI_OWNERSHIP_TRANSFER, ppimg);
			if (FAILED(hr))
			{
				DeleteObject(hbm);
			}
		}
		return hr;
	}

	// Factory method to create IUIImages from resource identifiers.
	HRESULT PopulateGallery(const PROPVARIANT* ppropvarCurrentValue, REFPROPERTYKEY key, GalleryItems *gi)
	{
		HRESULT hr = S_OK;
		CPropertySet *pProp = NULL;
		IUICollection* pCollection = NULL;

		if(key == UI_PKEY_Categories) {
			hr = ppropvarCurrentValue->punkVal->QueryInterface(IID_PPV_ARGS(&pCollection));
			if(FAILED(hr)) goto exit;

			for(int i=0; gi[i].CatName != NULL; i++) {
				hr = CPropertySet::CreateInstance(&pProp);
				if(FAILED(hr)) goto exit;

				// Initialize the property set
				pProp->InitializeCategoryProperties(gi[i].CatName, i);

				// Add the newly-created property set to the collection supplied 
				// by the framework.
				hr = pCollection->Add(pProp);
				if(FAILED(hr)) goto exit;

				SAFE_RELEASE(pProp);
			}
		} else if (key == UI_PKEY_ItemsSource) {
			hr = ppropvarCurrentValue->punkVal->QueryInterface(IID_PPV_ARGS(&pCollection));
			if(FAILED(hr)) goto exit;

			for(int i=0; gi[i].CatName != NULL; i++) {
				for(int j=0; gi[i].ListID[j]; j++) {
					hr = CPropertySet::CreateInstance(&pProp);
					if(FAILED(hr)) goto exit;

					// Initialize the property set with the appropriate command id and
					// category id and type Action.
					pProp->InitializeCommandProperties(i, gi[i].ListID[j], UI_COMMANDTYPE_ACTION);

					// Add the newly-created property set to the collection supplied
					// by the framework.
					hr = pCollection->Add(pProp);
					if(FAILED(hr)) goto exit;

					SAFE_RELEASE(pProp);
				}
			}
		}

exit:
		SAFE_RELEASE(pProp);
		SAFE_RELEASE(pCollection);
		return hr;
	}
};

HRESULT InitRibbon(HWND hWindowFrame)
{
	HRESULT hr = ::CoCreateInstance(CLSID_UIRibbonFramework, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pFramework));
    if(FAILED(hr))
    {
        return hr;
    }

    CComObject<CApplication> *pApplication = NULL;
    hr = CComObject<CApplication>::CreateInstance(&pApplication);
    if(FAILED(hr))
    {
        return hr;
    }

    hr = g_pFramework->Initialize(hWindowFrame, pApplication);
    if(FAILED(hr))
    {
        return hr;
    }

    hr = g_pFramework->LoadUI(GetModuleHandle(NULL), L"APPLICATION_RIBBON");
    if(FAILED(hr))
    {
        return hr;
    }

	memset(&CmdStateList, 0, sizeof(CmdStateList));

	return S_OK;
}

HRESULT UpdateRibbonHeight(void)
{
	static bool first = true;
    IUIRibbon* pRibbon;
    HRESULT hr = E_FAIL;

	RibbonHeight = 0;
    hr = g_pFramework->GetView(0, IID_PPV_ARGS(&pRibbon));
    if (SUCCEEDED(hr))
    {
		if(first) {
			first = false;
/*			IPropertyStore *pPropertyStore = NULL;
			hr = pRibbon->QueryInterface(__uuidof(IPropertyStore), (void**)&pPropertyStore);
			if (SUCCEEDED(hr))
			{
				// Set the ribbon display state based on the toggle state.
				PROPVARIANT propvar;
				PropVariantInit(&propvar);
				UIInitPropertyFromBoolean(UI_PKEY_Minimized, TRUE, &propvar);
				hr = pPropertyStore->SetValue(UI_PKEY_Minimized, propvar);
				pPropertyStore->Commit();
				pPropertyStore->Release();
			}*/
		}

      hr = pRibbon->GetHeight(&RibbonHeight);
      pRibbon->Release();
    }

	return hr;
}

void DestroyRibbon()
{
    if (g_pFramework)
    {
        g_pFramework->Destroy();
        g_pFramework->Release();
        g_pFramework = NULL;
    }
}

#define APP_NORMAL_MODE     0
#define APP_SIMULATION_MODE 1

void SetApplicationMode(void)
{
	INT32 mode = InSimulationMode ? UI_MAKEAPPMODE(APP_SIMULATION_MODE) : UI_MAKEAPPMODE(APP_NORMAL_MODE);
	HRESULT hr = g_pFramework->SetModes(mode);
}
