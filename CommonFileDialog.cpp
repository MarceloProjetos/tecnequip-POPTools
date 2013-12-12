//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//

#include "poptools.h"

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

const char *c_rgSaveTypes[][2][2] =
{
		{ { "Projetos POPTools (*.ld)", "*.ld" },
		  { "Todos os Arquivos (*.*)", "*.*"  } },

		{ { "Projetos POPTools (*.ld)", "*.ld" },
		  { "Todos os Arquivos (*.*)", "*.*"  } },

		{ { "Arquivos de Texto (*.txt)", "*.txt" },
		  { "Todos os Arquivos (*.*)", "*.*"  } },

		{ { "Arquivos de Linguagem C (*.c)", "*.c" },
		  { "Todos os Arquivos (*.*)", "*.*"  } },

		{ { "Arquivos CSV (*.csv)", "*.csv" },
		  { "Todos os Arquivos (*.*)", "*.*"  } },
};

/* File Dialog Event Handler *****************************************************************************************************/

class CDialogEventHandler : public IFileDialogEvents,
                            public IFileDialogControlEvents
{
public:
    // IUnknown methods
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        static const QITAB qit[] = {
            QITABENT(CDialogEventHandler, IFileDialogEvents),
            QITABENT(CDialogEventHandler, IFileDialogControlEvents),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&_cRef);
        if (!cRef)
            delete this;
        return cRef;
    }

    // IFileDialogEvents methods
    IFACEMETHODIMP OnFileOk(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnFolderChange(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *) { return S_OK; };
    IFACEMETHODIMP OnHelp(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnSelectionChange(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) { return S_OK; };
    IFACEMETHODIMP OnTypeChange(IFileDialog *pfd) { return S_OK; };
    IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) { return S_OK; };

    // IFileDialogControlEvents methods
    IFACEMETHODIMP OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem) { return S_OK; };
    IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize *, DWORD) { return S_OK; };
    IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize *, DWORD, BOOL) { return S_OK; };
    IFACEMETHODIMP OnControlActivating(IFileDialogCustomize *, DWORD) { return S_OK; };

    CDialogEventHandler() : _cRef(1) { };
private:
    ~CDialogEventHandler() { };
    long _cRef;
};

// Instance creation helper
HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
{
    *ppv = NULL;
    CDialogEventHandler *pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
    HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pDialogEventHandler->QueryInterface(riid, ppv);
        pDialogEventHandler->Release();
    }
    return hr;
}

/* Common File Dialog Snippets ***************************************************************************************************/

// This code snippet demonstrates how to work with the common file dialog interface
HRESULT FileDialogShow(enum FDS_Mode mode, char *DefExt, char *FileName)
{
    // CoCreate the File Open Dialog object.
	tConvString pStr;
    IFileDialog *pfd = NULL;

	char *PathName = nullptr, *pSlashPosition = strrchr(FileName, '\\');

	if(pSlashPosition != NULL) {
		int bufSize = strlen(FileName) + 1, pos = pSlashPosition - FileName;

		PathName = new char[bufSize];

		strncpy(PathName, FileName, pos);
		PathName[pos] = 0;

		strcpy (FileName, FileName + pos + 1);
	}

    HRESULT hr = CoCreateInstance(mode == LoadLadder ? CLSID_FileOpenDialog : CLSID_FileSaveDialog           , NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // Create an event handling object, and hook it up to the dialog.
        IFileDialogEvents *pfde = NULL;
        hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
        if (SUCCEEDED(hr))
        {
            // Hook up the event handler.
            DWORD dwCookie;
            hr = pfd->Advise(pfde, &dwCookie);
            if (SUCCEEDED(hr))
            {
                // Set the options on the dialog.
                DWORD dwFlags;

                // Before setting, always get the options first in order not to override existing options.
                hr = pfd->GetOptions(&dwFlags);
                if (SUCCEEDED(hr))
                {
					dwFlags &= ~(FOS_OVERWRITEPROMPT | FOS_FORCESHOWHIDDEN | FOS_PATHMUSTEXIST |
									FOS_FILEMUSTEXIST | FOS_ALLOWMULTISELECT);
					if(mode == LoadLadder) {
						dwFlags |= FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST;
					} else {
						dwFlags |= FOS_OVERWRITEPROMPT;

						pStr.pWideChar = ConvString_Convert(NULL, FileName);
						hr = pfd->SetFileName(pStr.pWideChar);
						delete pStr.pWideChar;
					}

                    if (SUCCEEDED(hr))
                    {
						// In this case, get shell items only for file system items.
						hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
						if (SUCCEEDED(hr))
						{
							int i, size = ARRAYSIZE(c_rgSaveTypes[mode]);
							COMDLG_FILTERSPEC *filter = new COMDLG_FILTERSPEC[size];
							for(i = 0; i < size; i++) {
								filter[i].pszName = ConvString_Convert(NULL, _(c_rgSaveTypes[mode][i][0]));
								filter[i].pszSpec = ConvString_Convert(NULL,   c_rgSaveTypes[mode][i][1] );
							}

							// Set the file types to display only. Notice that, this is a 1-based array.
							hr = pfd->SetFileTypes(size, filter);
							if (SUCCEEDED(hr))
							{
								pStr.pWideChar = ConvString_Convert(NULL, DefExt);
								hr = pfd->SetDefaultExtension(pStr.pWideChar);
								delete pStr.pWideChar;

								if (SUCCEEDED(hr) && PathName != nullptr) {
									IShellItem *psiFolder;
									pStr.pWideChar = ConvString_Convert(nullptr, PathName);
									hr = SHCreateItemFromParsingName ( pStr.pWideChar, NULL, IID_PPV_ARGS(&psiFolder) );
 									if ( SUCCEEDED(hr) ) {
										hr=pfd->SetFolder ( psiFolder );
									}
									delete pStr.pWideChar;
								}

								if (SUCCEEDED(hr))
								{
									// Show the dialog
									hr = pfd->Show(NULL);
									if (SUCCEEDED(hr))
									{
										// Obtain the result, once the user clicks the 'Open' button.
										// The result is an IShellItem object.
										IShellItem *psiResult;
										hr = pfd->GetResult(&psiResult);
										if (SUCCEEDED(hr))
										{
											// We are just going to print out the name of the file for sample sake.
											PWSTR pszFilePath = NULL;
											hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
											if (SUCCEEDED(hr))
											{
												pStr.pChar = ConvString_Convert(NULL, pszFilePath);
												strcpy(FileName, pStr.pChar);
												delete pStr.pChar;

												CoTaskMemFree(pszFilePath);
											}
											psiResult->Release();
										}
									}
								}
							}

							for(i = 0; i < size; i++) {
								delete filter[i].pszName;
								delete filter[i].pszSpec;
							}

							delete [] filter;
						}
					}
                }
                // Unhook the event handler.
                pfd->Unadvise(dwCookie);
            }
            pfde->Release();
        }
        pfd->Release();
    }

	if(!SUCCEEDED(hr)) *FileName = 0;

	if(PathName != nullptr) {
		delete [] PathName;
	}
	
	return hr;
}
