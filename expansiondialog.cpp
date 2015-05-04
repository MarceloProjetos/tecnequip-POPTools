#include "poptools.h"

static HWND ExpansionDialog;

static HWND AddedBoardsListView;
static HWND BoardModelsListView;
static HWND AddBoardButton;
static HWND DelBoardButton;
static HWND BoardUseInterruptCheckbox;
static HWND BoardUpdateButton;
static HWND BoardCancelButton;
static HWND BoardNameTextBox;
static HWND BoardAddressTextBox;
static HWND BoardImageLabel;
static HWND BoardInfoLabel;

static HWND BoardLists[2] = { NULL, NULL };

static int currentItem = -1;

static LONG_PTR PrevExpansionDialogProc;

#define LV_BOARD_NAME 0x00
#define LV_BOARD_TYPE 0x01
#define LV_BOARD_ADDR 0x02

// Flag indicando se a lista de placas preenchidas ja foi carregada
bool isBoardModelsListViewReady;

static void MakeControls(void)
{
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
#define LV_ADD_COLUMN(hWnd, i, w, s) do { \
        lvc.iSubItem = i; \
        lvc.pszText = (char *)s; \
        lvc.iOrder = 0; \
        lvc.cx = w; \
        ListView_InsertColumn(hWnd, i, &lvc); \
    } while(0)

	AddedBoardsListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | WS_HSCROLL |
        LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
		15, 47, 310, 215, ExpansionDialog, NULL, Instance, NULL);
    ListView_SetExtendedListViewStyle(AddedBoardsListView, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES);
    NiceFont(AddedBoardsListView);

    LV_ADD_COLUMN(AddedBoardsListView, LV_BOARD_NAME, 120, _("Nome"));
    LV_ADD_COLUMN(AddedBoardsListView, LV_BOARD_TYPE,  80, _("Modelo"));
    LV_ADD_COLUMN(AddedBoardsListView, LV_BOARD_ADDR,  80, _("Endereço"));

	BoardModelsListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | WS_VSCROLL |
		LVS_ICON | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
		365, 47, 200, 215, ExpansionDialog, NULL, Instance, NULL);
    ListView_SetExtendedListViewStyle(BoardModelsListView, LVS_EX_DOUBLEBUFFER);
    NiceFont(BoardModelsListView);

	HICON hiconItem;     // Icon for list-view items.
    HIMAGELIST hLarge;   // Image list for icon view.
    HIMAGELIST hSmall;   // Image list for other views.

    // Create the full-sized icon image lists. 
    hLarge = ImageList_Create(GetSystemMetrics(SM_CXICON), 
                              GetSystemMetrics(SM_CYICON), 
                              ILC_COLOR32, 1, 1); 

    hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), 
                              GetSystemMetrics(SM_CYSMICON), 
                              ILC_COLOR32, 1, 1); 
    
    // Add an icon to each image list.  
	unsigned int i;
	for(i = eExpansionBoard_DigitalInput; i != eExpansionBoard_End; i++) {
		unsigned int res;
		switch(i) {
			default:
			case eExpansionBoard_DigitalInput : res = IDI_BOARD_DI ; break;
			case eExpansionBoard_DigitalOutput: res = IDI_BOARD_DO ; break;
			case eExpansionBoard_AnalogInput  : res = IDI_BOARD_AI ; break;
			case eExpansionBoard_LCD          : res = IDI_BOARD_LCD; break;
		}

		hiconItem = LoadIcon(Instance, MAKEINTRESOURCE(res));

		ImageList_AddIcon(hLarge, hiconItem);
		ImageList_AddIcon(hSmall, hiconItem);

		DestroyIcon(hiconItem);
	}
    
    // Assign the image lists to the list-view control. 
    ListView_SetImageList(BoardModelsListView, hLarge, LVSIL_NORMAL); 
    ListView_SetImageList(BoardModelsListView, hSmall, LVSIL_SMALL); 

	AddBoardButton = CreateWindowEx(0, WC_BUTTON, "<<",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        330, 117, 30, 23, ExpansionDialog, NULL, Instance, NULL); 
    NiceFont(AddBoardButton);

	DelBoardButton = CreateWindowEx(0, WC_BUTTON, ">>",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        330, 169, 30, 23, ExpansionDialog, NULL, Instance, NULL); 
    NiceFont(DelBoardButton);

	HWND grouper = CreateWindowEx(0, WC_BUTTON, _("Lista de Placas"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 7, 568, 262, ExpansionDialog, NULL, Instance, NULL);
    NiceFont(grouper);

	HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Placas Adicionadas:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        10, 17, 140, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Modelos Compatíveis:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        360, 17, 140, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

    BoardNameTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,   
        70, 290, 175, 21, ExpansionDialog, NULL, Instance, NULL);
    NiceFont(BoardNameTextBox);

    BoardAddressTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
        WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,   
        135, 317, 110, 21, ExpansionDialog, NULL, Instance, NULL);
    NiceFont(BoardAddressTextBox);

	BoardUpdateButton = CreateWindowEx(0, WC_BUTTON, _("Confirmar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        250, 290, 75, 23, ExpansionDialog, NULL, Instance, NULL); 
    NiceFont(BoardUpdateButton);

    BoardCancelButton = CreateWindowEx(0, WC_BUTTON, _("Desfazer"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        250, 315, 75, 23, ExpansionDialog, NULL, Instance, NULL); 
    NiceFont(BoardCancelButton);

	BoardUseInterruptCheckbox = CreateWindowEx(0, WC_BUTTON, "Usar Interrupção?",
        WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE,
        375, 290, 150, 21, ExpansionDialog, NULL, Instance, NULL);
    NiceFont(BoardUseInterruptCheckbox);

	grouper = CreateWindowEx(0, WC_BUTTON, _("Detalhes"),
        WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
        7, 272, 568, 168, ExpansionDialog, NULL, Instance, NULL);
    NiceFont(grouper);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Nome:"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        8, 20, 50, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("Endereço (hex):"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        8, 47, 100, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabel = CreateWindowEx(0, WC_STATIC, _("0x"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        110, 47, 15, 21, grouper, NULL, Instance, NULL);
    NiceFont(textLabel);

	// Imagem da placa de expansao
	BoardImageLabel = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR, WC_STATIC, "",
        SS_BITMAP | SS_BLACKRECT | SS_GRAYFRAME | SS_LEFT | SS_LEFTNOWORDWRAP | SS_RIGHT | SS_WHITERECT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        8, 85, 64, 64, grouper, NULL, Instance, NULL);

    BoardInfoLabel = CreateWindowEx(0, WC_STATIC, _("Endereço (hex):  0xEndereço (hex):  0xEndereço (hex):  0xEndereço (hex):  0xEndereço (hex):  0xEndereço (hex):  0xEndereço (hex):  0xEndereço (hex):  0xEndereço (hex):  0xEndereço (hex):  0x\nEndereço (hex):  0x\nEndereço (hex):  0x\nEndereço (hex):  0x\nEndereço (hex):  0x\nEndereço (hex):  0x\nEndereço (hex):  0x"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        80, 70, 483, 93, grouper, NULL, Instance, NULL);
    NiceFont(BoardInfoLabel);

	OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        430, 445, 70, 23, ExpansionDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancelar"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        505, 445, 70, 23, ExpansionDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

void UpdateControls(HWND lv, unsigned int id)
{
	char buf[1024];
	eExpansionBoard type = eExpansionBoard_End;

	// Se nao houve selecao, utilizamos a selecao anterior
	if(lv == NULL) {
		if(currentItem < 0) {
			id = 0;
			lv = BoardModelsListView;
		} else {
			id = currentItem;
			lv = AddedBoardsListView;
		}
	}

	if(lv == AddedBoardsListView) {
		tExpansionBoardItem item = ladder->getBoardById(id);

		currentItem = item.id;
		type        = item.type;

		sprintf(buf, "%x", item.address);

		Edit_SetText(BoardNameTextBox   , item.name.c_str());
		Edit_SetText(BoardAddressTextBox, buf);
		Static_SetText(BoardInfoLabel, ladder->getBoardDescription(item.type).c_str());
		Button_SetCheck(BoardUseInterruptCheckbox, item.useIRQ ? TRUE : FALSE);
	} else {
		currentItem = -1; // Nenhum item atual
		Edit_SetText(BoardNameTextBox   , "");
		Edit_SetText(BoardAddressTextBox, "");
		Button_SetCheck(BoardUseInterruptCheckbox, FALSE);

		// Carrega descricao da placa selecionada (se houver)
		int index = ListView_GetNextItem(lv, -1, LVNI_SELECTED);
		if(index >= 0) {
			type = static_cast<eExpansionBoard>(index);
			strcpy(buf, ladder->getBoardDescription(type).c_str());
			Static_SetText(BoardInfoLabel, buf);
		}
	}

	unsigned int res = 0;
	switch(type) {
		default:
		case eExpansionBoard_DigitalInput : res = IDB_BOARD_DI ; break;
		case eExpansionBoard_DigitalOutput: res = IDB_BOARD_DO ; break;
		case eExpansionBoard_AnalogInput  : res = IDB_BOARD_AI ; break;
		case eExpansionBoard_LCD          : res = IDB_BOARD_LCD; break;
	}

	HBITMAP hBmp = (HBITMAP) LoadImage(Instance,MAKEINTRESOURCE(res),IMAGE_BITMAP,0,0, LR_DEFAULTSIZE);
	SendMessage(BoardImageLabel,STM_SETIMAGE,(WPARAM) IMAGE_BITMAP,(LPARAM) hBmp);
}

static LRESULT CALLBACK ExpansionDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    char buf[1024];
	bool success = false;

	switch (msg) {
        case WM_NOTIFY: {
			LPNMHDR nm = (LPNMHDR)lParam;

			if(nm->hwndFrom == BoardModelsListView || nm->hwndFrom == AddedBoardsListView) {
				switch(nm->code) {
				// Essa notificacao informa que a selecao foi alterada (item clicado)
				// Eu preciso retornar falso (zero) para permitir a alteracao de item
				// Se retornar verdadeiro (como quando nao trato a mensagem) a selecao nao ocorre
				case LVN_ITEMCHANGING:
					return 0;

				case LVN_ITEMCHANGED: {
					LPNMLISTVIEW pNM = (LPNMLISTVIEW)nm;
					if(pNM->uNewState & LVIS_SELECTED) {
						UpdateControls(nm->hwndFrom, pNM->lParam);
					}

					return 0;
				}

				case NM_CLICK:
					if((nm->hwndFrom == AddedBoardsListView && currentItem < 0) ||
						(nm->hwndFrom == BoardModelsListView && currentItem >= 0)) {
						int index = ListView_GetNextItem(nm->hwndFrom, -1, LVNI_SELECTED);
						if(index >= 0) {
							LVITEM item;
							item.iItem    = index;
							item.iSubItem = 0;

							ListView_GetItem(nm->hwndFrom, &item);
							UpdateControls(nm->hwndFrom, item.lParam);
						}
					}
					break;

				default:
					return 0;
				}
	        }
            break;
		}

		case WM_COMMAND: {
            HWND h = (HWND)lParam;
			if(wParam == BN_CLICKED) {
				// Aqui verificamos o botao que foi clicado. Nao utilizamos else pois assim podemos
				// alterar o valor de h, direcionando o clique de um botao para outro!
				// Exemplo: clicar no botao de atualizar placa direciona para inserir quando nao
				// estiver selecionada uma placa existente
				if(h == DelBoardButton) {
					if(currentItem >= 0) { // Existe um item selecionado, remove-o!
						tExpansionBoardItem item = ladder->getBoardById(currentItem);

						// Antes de remover, pede confirmacao
						if(ladder->ShowDialog(eDialogType_Question, false, _("Remover Placa de Expansão"),
								_("Tem certeza que deseja remover a placa '%s' ?"), item.name.c_str()) == eReply_Yes) {
							success = ladder->boardDel(currentItem);
							if(success) {
								ladder->boardPopulateListGUI((void*)BoardLists);

								// Item atual foi removido, desmarca e atualiza controles!
								currentItem = -1;
								UpdateControls(NULL, 0);
							} else {
								ladder->ShowDialog(eDialogType_Message, false, _("Remover Placa de Expansão"), _("Ocorreu um erro durante a operação. Verifique os dados e tente novamente!"));
							}
						}
					}
				}
				if(h == BoardUpdateButton) {
					if(currentItem >= 0) { // Existe um item selecionado, atualiza-o!
						// Primeiro recuperamos o item atual
						tExpansionBoardItem item = ladder->getBoardById(currentItem);

						// Carregamos o novo nome
						Edit_GetText(BoardNameTextBox, buf, MAX_NAME_LEN);
						item.name = buf;

						// Carregamos o novo endereco
						Edit_GetText(BoardAddressTextBox, buf, MAX_NAME_LEN);
						item.address = strtol(buf, NULL, 16);

						// Carregamos a flag de interrupcao
						item.useIRQ = Button_GetCheck(BoardUseInterruptCheckbox) ? true : false;

						// Atualiza a placa
						success = ladder->boardUpdate(currentItem, item);
						if(success) {
							// Atualiza a tela
							ladder->boardPopulateListGUI((void*)BoardLists);
							UpdateControls(NULL, 0);
						} else {
							ladder->ShowDialog(eDialogType_Message, false, _("Atualizar Placa de Expansão"), _("Ocorreu um erro durante a operação. Verifique os dados e tente novamente!"));
						}
					} else {
						h = AddBoardButton;
					}
				}
				if(h == AddBoardButton) {
					if(currentItem < 0) { // Existe um item selecionado, remove-o!
						tExpansionBoardItem item;
						int index = ListView_GetNextItem(BoardModelsListView, -1, LVNI_SELECTED);

						// Carregamos o novo nome
						Edit_GetText(BoardNameTextBox, buf, MAX_NAME_LEN);
						item.name = buf;

						// Se houver item selecionado e nome nao estiver em branco, adiciona placa
						if(index >= 0 && item.name.size() > 0) {
							// Identificamos o tipo de placa
							item.type = static_cast<eExpansionBoard>(index);

							// Carregamos o novo endereco
							Edit_GetText(BoardAddressTextBox, buf, MAX_NAME_LEN);
							item.address = strtol(buf, NULL, 16);

							// Carregamos a flag de interrupcao
							item.useIRQ = Button_GetCheck(BoardUseInterruptCheckbox) ? true : false;

							success = ladder->boardAdd(item);
							if(success) {
								// Atualiza a tela
								ladder->boardPopulateListGUI((void*)BoardLists);
								UpdateControls(NULL, 0);
							} else {
								ladder->ShowDialog(eDialogType_Message, false, _("Adicionar Placa de Expansão"), _("Ocorreu um erro durante a operação. Verifique os dados e tente novamente!"));
							}
						} else {
							ladder->ShowDialog(eDialogType_Message, false, _("Adicionar Placa de Expansão"), _("Selecione o tipo de placa e preencha seu nome antes de adicionar!"));
						}
					}
				}
				if(h == BoardCancelButton) {
					UpdateControls(NULL, 0);
				}
				if(h ==       OkButton) {
	                DialogDone = TRUE;
				}
				if(h ==   CancelButton) {
					DialogDone   = TRUE;
					DialogCancel = TRUE;
				}
			}
            break;
			}

        case WM_CLOSE:
        case WM_DESTROY:
            DialogDone = TRUE;
            DialogCancel = TRUE;
            break;
	}

	return CallWindowProc((WNDPROC)PrevExpansionDialogProc, hwnd, msg, wParam, lParam);
}

bool ShowExpansionDialog(void)
{
	bool changed = false;

	POINT size = { 582, 475 };
	POINT startPoint = getWindowStart(size);

    ExpansionDialog = CreateWindowClient(0, "POPToolsDialog",
        _("Placas de Expansão"), WS_OVERLAPPED | WS_SYSMENU,
        startPoint.x, startPoint.y, size.x, size.y, MainWindow, NULL, Instance, NULL);

    MakeControls();

	isBoardModelsListViewReady = false;
	BoardLists[0] = BoardModelsListView;
	BoardLists[1] = AddedBoardsListView;
	ladder->boardPopulateListGUI((void*)BoardLists);

	PrevExpansionDialogProc = SetWindowLongPtr(ExpansionDialog, GWLP_WNDPROC, (LONG_PTR)ExpansionDialogProc);

    EnableWindow(MainWindow, FALSE);
	SetFocus(BoardModelsListView);
    ShowWindow(ExpansionDialog, TRUE);

	// Seleciona o primeiro item das placas disponiveis
	ListView_SetItemState(BoardModelsListView, 0, LVIS_SELECTED, LVIS_SELECTED);

	ladder->CheckpointBegin(_("Alterar Placas de Expansão"));

	MSG msg;
    DWORD ret;
    DialogDone = FALSE;
    DialogCancel = FALSE;
    while((ret = GetMessage(&msg, NULL, 0, 0)) && !DialogDone) {
        if(msg.message == WM_KEYDOWN) {
			if(msg.wParam == VK_RETURN) {
				DialogDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
                break;
            }
        }

        if(IsDialogMessage(ExpansionDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	if(!DialogCancel) {
		ladder->updateGuiIO();
	} else {
		ladder->CheckpointRollback();
	}

	ladder->CheckpointEnd();

	EnableWindow(MainWindow, TRUE);
	DestroyWindow(ExpansionDialog);
	SetFocus(MainWindow);

	ladder->updateContext();

	return changed;
}
