#include "poptools.h"
#include "KeyboardMap.h"

// Keyboard Handlers

// Default for Portuguese: F7
int KBH_Simulation_Toggle(void *user_data)
{
	ProcessMenu(MNU_SIMULATION_MODE);

	return 1;
}

// Default for Portuguese: TAB
int KBH_SetFocus_IoList(void *user_data)
{
	SetFocus(IoList);
	BlinkCursor(0, 0, 0, 0);

	return 1;
}

// Default for Portuguese: F1
int KBH_Manual_Open(void *user_data)
{
	ProcessMenu(MNU_MANUAL);

	return 1;
}

// Default for Portuguese: F2
int KBH_Config(void *user_data)
{
	ProcessMenu(MNU_MCU_SETTINGS);

	return 1;
}

// Default for Portuguese: ENTER
int KBH_Simulation_SingleCycle(void *user_data)
{
	if(!InSimulationMode) return 0;

	ProcessMenu(MNU_SINGLE_CYCLE);

	return 1;
}

// Default for Portuguese: I
int KBH_Simulation_Start(void *user_data)
{
	if(!InSimulationMode) return 0;

	ProcessMenu(MNU_START_SIMULATION);

	return 1;
}

// Default for Portuguese: P
int KBH_Simulation_Pause(void *user_data)
{
	if(!InSimulationMode) return 0;

	ProcessMenu(MNU_PAUSE_SIMULATION);

	return 1;
}

// Default for Portuguese: DOWN
int KBH_Simulation_Down(void *user_data)
{
	if(!InSimulationMode) return 0;

	if(ScrollYOffset < ScrollYOffsetMax)
		ScrollYOffset++;
	RefreshScrollbars();
	InvalidateRect(MainWindow, NULL, FALSE);

	return 1;
}

// Default for Portuguese: UP
int KBH_Simulation_Up(void *user_data)
{
	if(!InSimulationMode) return 0;

	if(ScrollYOffset > 0)
		ScrollYOffset--;
	RefreshScrollbars();
	InvalidateRect(MainWindow, NULL, FALSE);

	return 1;
}

// Default for Portuguese: LEFT
int KBH_Simulation_Left(void *user_data)
{
	if(!InSimulationMode) return 0;

	ScrollXOffset -= FONT_WIDTH;
	if(ScrollXOffset < 0) ScrollXOffset = 0;
	RefreshScrollbars();
	InvalidateRect(MainWindow, NULL, FALSE);

	return 1;
}

// Default for Portuguese: RIGHT
int KBH_Simulation_Right(void *user_data)
{
	if(!InSimulationMode) return 0;

	ScrollXOffset += FONT_WIDTH;
	if(ScrollXOffset >= ScrollXOffsetMax)
		ScrollXOffset = ScrollXOffsetMax;
	RefreshScrollbars();
	InvalidateRect(MainWindow, NULL, FALSE);

	return 1;
}

// Default for Portuguese: ESC
int KBH_Simulation_Exit(void *user_data)
{
	if(!InSimulationMode) return 0;

	if(!POPSettings.ShowSimulationWarnings || ShowTaskDialog(L"Tem certeza que deseja sair da simulação?", L"O processo será interrompido", TD_WARNING_ICON, TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, L"Sempre mostrar avisos da simulação", &POPSettings.ShowSimulationWarnings) == IDYES) {
		ProcessMenu(MNU_SIMULATION_MODE);
	}

	return 1;
}

// Default for Portuguese: F3
int KBH_FAR_Open(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_FIND_AND_REPLACE);

	return 1;
}

// Default for Portuguese: SHIFT + F3
int KBH_FAR_Next(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_FIND_AND_REPLACE_NEXT);

	return 1;
}

// Default for Portuguese: F4
int KBH_Debug(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_DEBUG);

	return 1;
}

// Default for Portuguese: F5
int KBH_Compile(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_COMPILE);

	return 1;
}

// Default for Portuguese: F6
int KBH_Program(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_PROGRAM);

	return 1;
}

// Default for Portuguese: SHIFT + UP
int KBH_Rung_MoveUP(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_PUSH_RUNG_UP);

	return 1;
}

// Default for Portuguese: UP
int KBH_Cursor_MoveUp(void *user_data)
{
	if(InSimulationMode) return 0;

	MoveCursorKeyboard(VK_UP, FALSE);

	return 1;
}

// Default for Portuguese: SHIFT + DOWN
int KBH_Rung_MoveDown(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_PUSH_RUNG_DOWN);

	return 1;
}

// Default for Portuguese: DOWN
int KBH_Cursor_MoveDown(void *user_data)
{
	if(InSimulationMode) return 0;

	MoveCursorKeyboard(VK_DOWN, FALSE);

	return 1;
}

// Default for Portuguese: HOME
int KBH_Cursor_Home(void *user_data)
{
	if(InSimulationMode) return 0;

	MoveCursorKeyboard(VK_HOME, FALSE);

	return 1;
}

// Default for Portuguese: SHIFT + HOME
int KBH_Ladder_Home(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_GO_HOME);

	return 1;
}

// Default for Portuguese: END
int KBH_Cursor_End(void *user_data)
{
	if(InSimulationMode) return 0;

	MoveCursorKeyboard(VK_END, FALSE);

	return 1;
}

// Default for Portuguese: SHIFT + END
int KBH_Ladder_End(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_GO_END);

	return 1;
}

// Default for Portuguese: LEFT
int KBH_Cursor_Left(void *user_data)
{
	if(InSimulationMode) return 0;

	MoveCursorKeyboard(VK_LEFT, FALSE);

	return 1;
}

// Default for Portuguese: RIGHT
int KBH_Cursor_Right(void *user_data)
{
	if(InSimulationMode) return 0;

	MoveCursorKeyboard(VK_RIGHT, FALSE);

	return 1;
}

// Default for Portuguese: ENTER
int KBH_Element_Edit(void *user_data)
{
	if(InSimulationMode) return 0;

	CHANGING_PROGRAM(EditSelectedElement());

	return 1;
}

// Default for Portuguese: DEL or BACKSPACE
int KBH_Element_Delete(void *user_data)
{
	int i, emptyRung;

	if(InSimulationMode) return 0;

	i = RungContainingSelected();
	emptyRung = i<0 ? 0 : Prog.rungs[i]->count == 1 && Prog.rungs[i]->contents[0].which == ELEM_PLACEHOLDER;

	if(emptyRung) {
		ProcessMenu(MNU_DELETE_RUNG);
	} else {
		ProcessMenu(MNU_DELETE_ELEMENT);
	}

	return 1;
}

// Default for Portuguese: SHIFT + DEL
int KBH_Rung_Delete(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_DELETE_RUNG);

	return 1;
}

// Default for Portuguese: ;
int KBH_Element_Add_Comment(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_COMMENT);

	return 1;
}

// Default for Portuguese: SHIFT + C
int KBH_Element_Add_Counter_Circular(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_CTC);

	return 1;
}

// Default for Portuguese: CTRL + C
int KBH_Element_Copy(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_COPY_ELEMENT);

	return 1;
}

// Default for Portuguese: C
int KBH_Element_Add_Contact(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_CONTACTS);

	return 1;
}

// Default for Portuguese: SHIFT + B
int KBH_Element_Add_Bit_Check(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_CHECK_BIT);

	return 1;
}

// Default for Portuguese: CTRL + B
int KBH_Element_Add_Bit_Set(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_SET_BIT);

	return 1;
}

// Default for Portuguese: SHIFT + Y
int KBH_Element_Add_Yaskawa_Read(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_READ_SERVO_YASKAWA);

	return 1;
}

// Default for Portuguese: Y
int KBH_Element_Add_Yaskawa_Write(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_WRITE_SERVO_YASKAWA);

	return 1;
}

// Default for Portuguese: B
int KBH_Element_Add_Coil(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_COIL);

	return 1;
}

// Default for Portuguese: SHIFT + R
int KBH_Element_Add_DA_Ramp(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_MULTISET_DA);

	return 1;
}

// Default for Portuguese: R
int KBH_Element_Make_Reset(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_MAKE_RESET_ONLY);

	return 1;
}

// Default for Portuguese: SPACE
int KBH_Element_Make_Normal(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_MAKE_NORMAL);

	return 1;
}

// Default for Portuguese: CTRL + E
int KBH_File_Export(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_EXPORT);

	return 1;
}

// Default for Portuguese: Z
int KBH_Element_Add_Reset(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_RES);

	return 1;
}

// Default for Portuguese: CTRL + S
int KBH_File_Save(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_SAVE);

	return 1;
}

// Default for Portuguese: S
int KBH_Element_Make_Set(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_MAKE_SET_ONLY);

	return 1;
}

// Default for Portuguese: CTRL + N
int KBH_File_New(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_NEW);

	return 1;
}

// Default for Portuguese: N
int KBH_Element_Make_Negate(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_NEGATE);

	return 1;
}

// Default for Portuguese: CTRL + A
int KBH_Element_Add_AD(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_READ_ADC);

	return 1;
}

// Default for Portuguese: T
int KBH_Element_Timer_Retentive(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_RTO);

	return 1;
}

// Default for Portuguese: CTRL + O
int KBH_File_Open(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_OPEN);

	return 1;
}

// Default for Portuguese: A
int KBH_Element_Add_Timer_On(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_TON);

	return 1;
}

// Default for Portuguese: SHIFT + A
int KBH_Element_Add_Timer_Off(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_TOF);

	return 1;
}

// Default for Portuguese: I
int KBH_Element_Add_Counter_Incremental(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_CTU);

	return 1;
}

// Default for Portuguese: SHIFT + U
int KBH_Element_Add_USS_Read(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_READ_USS);

	return 1;
}

// Default for Portuguese: U
int KBH_Element_Add_USS_Write(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_WRITE_USS);

	return 1;
}

// Default for Portuguese: SHIFT + X
int KBH_Element_Add_ModBUS_Read(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_READ_MODBUS);

	return 1;
}

// Default for Portuguese: X
int KBH_Element_Add_ModBUS_Write(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_WRITE_MODBUS);

	return 1;
}

// Default for Portuguese: M
int KBH_Element_Add_Move(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_MOV);

	return 1;
}

// Default for Portuguese: P
int KBH_Element_Add_Parallel(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_PARALLEL);

	return 1;
}

// Default for Portuguese: SHIFT + E
int KBH_Element_Add_Encoder_Reset(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_RESET_ENC);

	return 1;
}

// Default for Portuguese: E
int KBH_Element_Add_Encoder_Read(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_READ_ENC);

	return 1;
}

// Default for Portuguese: SHIFT + P
int KBH_Element_Add_PWM(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_SET_PWM);

	return 1;
}

// Default for Portuguese: +
int KBH_Element_Add_Math_Add(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_ADD);

	return 1;
}

// Default for Portuguese: =
int KBH_Element_Add_Comparison_Equal(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_EQU);

	return 1;
}

// Default for Portuguese: -
int KBH_Element_Add_Math_Subtract(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_SUB);

	return 1;
}

// Default for Portuguese: SHIFT + /
int KBH_Element_Add_Math_Divide(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_DIV);

	return 1;
}

// Default for Portuguese: /
int KBH_Element_Add_OneShot_Rising(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_OSR);

	return 1;
}

// Default for Portuguese: \ 
int KBH_Element_Add_OneShot_Falling(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_OSF);

	return 1;
}

// Default for Portuguese: *
int KBH_Element_Add_Math_Multiply(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_MUL);

	return 1;
}

// Default for Portuguese: CTRL + D
int KBH_Element_Add_DA_Set(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_SET_DA);

	return 1;
}

// Default for Portuguese: D
int KBH_Element_Add_Counter_Decremental(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_CTD);

	return 1;
}

// Default for Portuguese: !
int KBH_Element_Add_Comparison_NotEqual(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_NEQ);

	return 1;
}

// Default for Portuguese: >
int KBH_Element_Add_Comparison_Greater(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_GRT);

	return 1;
}

// Default for Portuguese: SHIFT + >
int KBH_Element_Add_Comparison_GreaterEqual(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_GEQ);

	return 1;
}

// Default for Portuguese: <
int KBH_Element_Add_Comparison_Less(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_LES);

	return 1;
}

// Default for Portuguese: SHIFT + <
int KBH_Element_Add_Comparison_LessEqual(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_LEQ);

	return 1;
}

// Default for Portuguese: CTRL + V
int KBH_Element_Paste(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_PASTE_ELEMENT);

	return 1;
}

// Default for Portuguese: CTRL + X
int KBH_Element_Cut(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_CUT_ELEMENT);

	return 1;
}

// Default for Portuguese: SHIFT + INSERT
int KBH_Rung_Insert_Before(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_RUNG_BEFORE);

	return 1;
}

// Default for Portuguese: INSERT
int KBH_Rung_Insert_After(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_INSERT_RUNG_AFTER);

	return 1;
}

// Default for Portuguese: CTRL + Z
int KBH_Undo(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_UNDO);

	return 1;
}

// Default for Portuguese: CTRL + Y
int KBH_Redo(void *user_data)
{
	if(InSimulationMode) return 0;

	ProcessMenu(MNU_REDO);

	return 1;
}

// Keyboard Maps
KeyboardMap ShortcutMaps;

void KeyboardHandlers_LangChanged(void)
{
	WORD wLangID = PRIMARYLANGID(LOWORD(GetKeyboardLayout(0)));

	switch(wLangID) {
	case LANG_ENGLISH:
		break;

	default: // For default, use Portuguese keyboard map
		wLangID = LANG_PORTUGUESE;
	case LANG_PORTUGUESE:
		break;
	}

	ShortcutMaps.SetMap(wLangID);
}

void KeyboardHandlers_Init(void)
{
	ShortcutMaps.CreateMap(LANG_PORTUGUESE);
	ShortcutMaps.SetMap   (LANG_PORTUGUESE);

	ShortcutMaps.AddKey(VK_F1        , KBM_KEYFLAG_NONE   , KBH_Manual_Open                        , NULL);
	ShortcutMaps.AddKey(VK_F2        , KBM_KEYFLAG_NONE   , KBH_Config                             , NULL);
	ShortcutMaps.AddKey(VK_F3        , KBM_KEYFLAG_NONE   , KBH_FAR_Open                           , NULL);
	ShortcutMaps.AddKey(VK_F3        , KBM_KEYFLAG_SHIFT  , KBH_FAR_Next                           , NULL);
	ShortcutMaps.AddKey(VK_F4        , KBM_KEYFLAG_NONE   , KBH_Debug                              , NULL);
	ShortcutMaps.AddKey(VK_F5        , KBM_KEYFLAG_NONE   , KBH_Compile                            , NULL);
	ShortcutMaps.AddKey(VK_F6        , KBM_KEYFLAG_NONE   , KBH_Program                            , NULL);
	ShortcutMaps.AddKey(VK_F7        , KBM_KEYFLAG_NONE   , KBH_Simulation_Toggle                  , NULL);
	ShortcutMaps.AddKey(VK_TAB       , KBM_KEYFLAG_NONE   , KBH_SetFocus_IoList                    , NULL);
	ShortcutMaps.AddKey(VK_RETURN    , KBM_KEYFLAG_NONE   , KBH_Simulation_SingleCycle             , NULL);
	ShortcutMaps.AddKey(VK_UP        , KBM_KEYFLAG_SHIFT  , KBH_Rung_MoveUP                        , NULL);
	ShortcutMaps.AddKey(VK_UP        , KBM_KEYFLAG_ANY    , KBH_Cursor_MoveUp                      , NULL);
	ShortcutMaps.AddKey(VK_UP        , KBM_KEYFLAG_NONE   , KBH_Simulation_Up                      , NULL);
	ShortcutMaps.AddKey(VK_DOWN      , KBM_KEYFLAG_NONE   , KBH_Simulation_Down                    , NULL);
	ShortcutMaps.AddKey(VK_DOWN      , KBM_KEYFLAG_SHIFT  , KBH_Rung_MoveDown                      , NULL);
	ShortcutMaps.AddKey(VK_DOWN      , KBM_KEYFLAG_ANY    , KBH_Cursor_MoveDown                    , NULL);
	ShortcutMaps.AddKey(VK_LEFT      , KBM_KEYFLAG_NONE   , KBH_Cursor_Left                        , NULL);
	ShortcutMaps.AddKey(VK_LEFT      , KBM_KEYFLAG_NONE   , KBH_Simulation_Left                    , NULL);
	ShortcutMaps.AddKey(VK_RIGHT     , KBM_KEYFLAG_NONE   , KBH_Simulation_Right                   , NULL);
	ShortcutMaps.AddKey(VK_RIGHT     , KBM_KEYFLAG_NONE   , KBH_Cursor_Right                       , NULL);
	ShortcutMaps.AddKey(VK_HOME      , KBM_KEYFLAG_NONE   , KBH_Cursor_Home                        , NULL);
	ShortcutMaps.AddKey(VK_HOME      , KBM_KEYFLAG_SHIFT  , KBH_Ladder_Home                        , NULL);
	ShortcutMaps.AddKey(VK_END       , KBM_KEYFLAG_NONE   , KBH_Cursor_End                         , NULL);
	ShortcutMaps.AddKey(VK_END       , KBM_KEYFLAG_SHIFT  , KBH_Ladder_End                         , NULL);
	ShortcutMaps.AddKey(VK_RETURN    , KBM_KEYFLAG_NONE   , KBH_Element_Edit                       , NULL);
	ShortcutMaps.AddKey(VK_BACK      , KBM_KEYFLAG_NONE   , KBH_Element_Delete                     , NULL);
	ShortcutMaps.AddKey(VK_DELETE    , KBM_KEYFLAG_NONE   , KBH_Element_Delete                     , NULL);
	ShortcutMaps.AddKey(VK_DELETE    , KBM_KEYFLAG_SHIFT  , KBH_Rung_Delete                        , NULL);
	ShortcutMaps.AddKey(VK_ESCAPE    , KBM_KEYFLAG_NONE   , KBH_Simulation_Exit                    , NULL);
	ShortcutMaps.AddKey('A'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Timer_On               , NULL);
	ShortcutMaps.AddKey('A'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Timer_Off              , NULL);
	ShortcutMaps.AddKey('A'          , KBM_KEYFLAG_CONTROL, KBH_Element_Add_AD                     , NULL);
	ShortcutMaps.AddKey('B'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Coil                   , NULL);
	ShortcutMaps.AddKey('B'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Bit_Check              , NULL);
	ShortcutMaps.AddKey('B'          , KBM_KEYFLAG_CONTROL, KBH_Element_Add_Bit_Set                , NULL);
	ShortcutMaps.AddKey('C'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Contact                , NULL);
	ShortcutMaps.AddKey('C'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Counter_Circular       , NULL);
	ShortcutMaps.AddKey('C'          , KBM_KEYFLAG_CONTROL, KBH_Element_Copy                       , NULL);
	ShortcutMaps.AddKey('D'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Counter_Decremental    , NULL);
	ShortcutMaps.AddKey('D'          , KBM_KEYFLAG_CONTROL, KBH_Element_Add_DA_Set                 , NULL);
	ShortcutMaps.AddKey('E'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Encoder_Read           , NULL);
	ShortcutMaps.AddKey('E'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Encoder_Reset          , NULL);
	ShortcutMaps.AddKey('E'          , KBM_KEYFLAG_CONTROL, KBH_File_Export                        , NULL);
	ShortcutMaps.AddKey('I'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Counter_Incremental    , NULL);
	ShortcutMaps.AddKey('I'          , KBM_KEYFLAG_NONE   , KBH_Simulation_Start                   , NULL);
	ShortcutMaps.AddKey('M'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Move                   , NULL);
	ShortcutMaps.AddKey('N'          , KBM_KEYFLAG_NONE   , KBH_Element_Make_Negate                , NULL);
	ShortcutMaps.AddKey('N'          , KBM_KEYFLAG_CONTROL, KBH_File_New                           , NULL);
	ShortcutMaps.AddKey('O'          , KBM_KEYFLAG_CONTROL, KBH_File_Open                          , NULL);
	ShortcutMaps.AddKey('P'          , KBM_KEYFLAG_NONE   , KBH_Simulation_Pause                   , NULL);
	ShortcutMaps.AddKey('P'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Parallel               , NULL);
	ShortcutMaps.AddKey('P'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_PWM                    , NULL);
	ShortcutMaps.AddKey('R'          , KBM_KEYFLAG_NONE   , KBH_Element_Make_Reset                 , NULL);
	ShortcutMaps.AddKey('R'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_DA_Ramp                , NULL);
	ShortcutMaps.AddKey('S'          , KBM_KEYFLAG_NONE   , KBH_Element_Make_Set                   , NULL);
	ShortcutMaps.AddKey('S'          , KBM_KEYFLAG_CONTROL, KBH_File_Save                          , NULL);
	ShortcutMaps.AddKey('T'          , KBM_KEYFLAG_NONE   , KBH_Element_Timer_Retentive            , NULL);
	ShortcutMaps.AddKey('U'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_USS_Write              , NULL);
	ShortcutMaps.AddKey('U'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_USS_Read               , NULL);
	ShortcutMaps.AddKey('V'          , KBM_KEYFLAG_CONTROL, KBH_Element_Paste                      , NULL);
	ShortcutMaps.AddKey('X'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_ModBUS_Write           , NULL);
	ShortcutMaps.AddKey('X'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_ModBUS_Read            , NULL);
	ShortcutMaps.AddKey('X'          , KBM_KEYFLAG_CONTROL, KBH_Element_Cut                        , NULL);
	ShortcutMaps.AddKey('Y'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Yaskawa_Write          , NULL);
	ShortcutMaps.AddKey('Y'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Yaskawa_Read           , NULL);
	ShortcutMaps.AddKey('Y'          , KBM_KEYFLAG_CONTROL, KBH_Redo                               , NULL);
	ShortcutMaps.AddKey('Z'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Reset                  , NULL);
	ShortcutMaps.AddKey('Z'          , KBM_KEYFLAG_CONTROL, KBH_Undo                               , NULL);
	ShortcutMaps.AddKey('1'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Comparison_NotEqual    , NULL);
	ShortcutMaps.AddKey('8'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Math_Multiply          , NULL);
	ShortcutMaps.AddKey(VK_OEM_PLUS  , KBM_KEYFLAG_NONE   , KBH_Element_Add_Comparison_Equal       , NULL);
	ShortcutMaps.AddKey(VK_OEM_PLUS  , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Math_Add               , NULL);
	ShortcutMaps.AddKey(VK_OEM_MINUS , KBM_KEYFLAG_NONE   , KBH_Element_Add_Math_Subtract          , NULL);
	ShortcutMaps.AddKey(0xC1         , KBM_KEYFLAG_NONE   , KBH_Element_Add_OneShot_Rising         , NULL);
	ShortcutMaps.AddKey(0xC1         , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Math_Divide            , NULL);
	ShortcutMaps.AddKey(VK_OEM_102   , KBM_KEYFLAG_NONE   , KBH_Element_Add_OneShot_Falling        , NULL);
	ShortcutMaps.AddKey(VK_INSERT    , KBM_KEYFLAG_NONE   , KBH_Rung_Insert_After                  , NULL);
	ShortcutMaps.AddKey(VK_INSERT    , KBM_KEYFLAG_SHIFT  , KBH_Rung_Insert_Before                 , NULL);
	ShortcutMaps.AddKey(VK_OEM_2     , KBM_KEYFLAG_NONE   , KBH_Element_Add_Comment                , NULL);
	ShortcutMaps.AddKey(VK_OEM_PERIOD, KBM_KEYFLAG_NONE   , KBH_Element_Add_Comparison_Greater     , NULL);
	ShortcutMaps.AddKey(VK_OEM_PERIOD, KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Comparison_GreaterEqual, NULL);
	ShortcutMaps.AddKey(VK_OEM_COMMA , KBM_KEYFLAG_NONE   , KBH_Element_Add_Comparison_Less        , NULL);
	ShortcutMaps.AddKey(VK_OEM_COMMA , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Comparison_LessEqual   , NULL);
	ShortcutMaps.AddKey(' '          , KBM_KEYFLAG_NONE   , KBH_Element_Make_Normal                , NULL);

	ShortcutMaps.CreateMap(LANG_ENGLISH);
	ShortcutMaps.SetMap   (LANG_ENGLISH);

	ShortcutMaps.AddKey(VK_F1        , KBM_KEYFLAG_NONE   , KBH_Manual_Open                        , NULL);
	ShortcutMaps.AddKey(VK_F2        , KBM_KEYFLAG_NONE   , KBH_Config                             , NULL);
	ShortcutMaps.AddKey(VK_F3        , KBM_KEYFLAG_NONE   , KBH_FAR_Open                           , NULL);
	ShortcutMaps.AddKey(VK_F3        , KBM_KEYFLAG_SHIFT  , KBH_FAR_Next                           , NULL);
	ShortcutMaps.AddKey(VK_F4        , KBM_KEYFLAG_NONE   , KBH_Debug                              , NULL);
	ShortcutMaps.AddKey(VK_F5        , KBM_KEYFLAG_NONE   , KBH_Compile                            , NULL);
	ShortcutMaps.AddKey(VK_F6        , KBM_KEYFLAG_NONE   , KBH_Program                            , NULL);
	ShortcutMaps.AddKey(VK_F7        , KBM_KEYFLAG_NONE   , KBH_Simulation_Toggle                  , NULL);
	ShortcutMaps.AddKey(VK_TAB       , KBM_KEYFLAG_NONE   , KBH_SetFocus_IoList                    , NULL);
	ShortcutMaps.AddKey(VK_RETURN    , KBM_KEYFLAG_NONE   , KBH_Simulation_SingleCycle             , NULL);
	ShortcutMaps.AddKey(VK_UP        , KBM_KEYFLAG_NONE   , KBH_Simulation_Up                      , NULL);
	ShortcutMaps.AddKey(VK_UP        , KBM_KEYFLAG_SHIFT  , KBH_Rung_MoveUP                        , NULL);
	ShortcutMaps.AddKey(VK_UP        , KBM_KEYFLAG_ANY    , KBH_Cursor_MoveUp                      , NULL);
	ShortcutMaps.AddKey(VK_DOWN      , KBM_KEYFLAG_NONE   , KBH_Simulation_Down                    , NULL);
	ShortcutMaps.AddKey(VK_DOWN      , KBM_KEYFLAG_SHIFT  , KBH_Rung_MoveDown                      , NULL);
	ShortcutMaps.AddKey(VK_DOWN      , KBM_KEYFLAG_ANY    , KBH_Cursor_MoveDown                    , NULL);
	ShortcutMaps.AddKey(VK_LEFT      , KBM_KEYFLAG_NONE   , KBH_Cursor_Left                        , NULL);
	ShortcutMaps.AddKey(VK_LEFT      , KBM_KEYFLAG_NONE   , KBH_Simulation_Left                    , NULL);
	ShortcutMaps.AddKey(VK_RIGHT     , KBM_KEYFLAG_NONE   , KBH_Simulation_Right                   , NULL);
	ShortcutMaps.AddKey(VK_RIGHT     , KBM_KEYFLAG_NONE   , KBH_Cursor_Right                       , NULL);
	ShortcutMaps.AddKey(VK_HOME      , KBM_KEYFLAG_NONE   , KBH_Cursor_Home                        , NULL);
	ShortcutMaps.AddKey(VK_HOME      , KBM_KEYFLAG_SHIFT  , KBH_Ladder_Home                        , NULL);
	ShortcutMaps.AddKey(VK_END       , KBM_KEYFLAG_NONE   , KBH_Cursor_End                         , NULL);
	ShortcutMaps.AddKey(VK_END       , KBM_KEYFLAG_SHIFT  , KBH_Ladder_End                         , NULL);
	ShortcutMaps.AddKey(VK_RETURN    , KBM_KEYFLAG_NONE   , KBH_Element_Edit                       , NULL);
	ShortcutMaps.AddKey(VK_BACK      , KBM_KEYFLAG_NONE   , KBH_Element_Delete                     , NULL);
	ShortcutMaps.AddKey(VK_DELETE    , KBM_KEYFLAG_NONE   , KBH_Element_Delete                     , NULL);
	ShortcutMaps.AddKey(VK_DELETE    , KBM_KEYFLAG_SHIFT  , KBH_Rung_Delete                        , NULL);
	ShortcutMaps.AddKey(VK_ESCAPE    , KBM_KEYFLAG_NONE   , KBH_Simulation_Exit                    , NULL);
	ShortcutMaps.AddKey('A'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Timer_On               , NULL);
	ShortcutMaps.AddKey('A'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Timer_Off              , NULL);
	ShortcutMaps.AddKey('A'          , KBM_KEYFLAG_CONTROL, KBH_Element_Add_AD                     , NULL);
	ShortcutMaps.AddKey('B'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Coil                   , NULL);
	ShortcutMaps.AddKey('B'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Bit_Check              , NULL);
	ShortcutMaps.AddKey('B'          , KBM_KEYFLAG_CONTROL, KBH_Element_Add_Bit_Set                , NULL);
	ShortcutMaps.AddKey('C'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Contact                , NULL);
	ShortcutMaps.AddKey('C'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Counter_Circular       , NULL);
	ShortcutMaps.AddKey('C'          , KBM_KEYFLAG_CONTROL, KBH_Element_Copy                       , NULL);
	ShortcutMaps.AddKey('D'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Counter_Decremental    , NULL);
	ShortcutMaps.AddKey('D'          , KBM_KEYFLAG_CONTROL, KBH_Element_Add_DA_Set                 , NULL);
	ShortcutMaps.AddKey('E'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Encoder_Read           , NULL);
	ShortcutMaps.AddKey('E'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Encoder_Reset          , NULL);
	ShortcutMaps.AddKey('E'          , KBM_KEYFLAG_CONTROL, KBH_File_Export                        , NULL);
	ShortcutMaps.AddKey('I'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Counter_Incremental    , NULL);
	ShortcutMaps.AddKey('I'          , KBM_KEYFLAG_NONE   , KBH_Simulation_Start                   , NULL);
	ShortcutMaps.AddKey('M'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Move                   , NULL);
	ShortcutMaps.AddKey('N'          , KBM_KEYFLAG_NONE   , KBH_Element_Make_Negate                , NULL);
	ShortcutMaps.AddKey('N'          , KBM_KEYFLAG_CONTROL, KBH_File_New                           , NULL);
	ShortcutMaps.AddKey('O'          , KBM_KEYFLAG_CONTROL, KBH_File_Open                          , NULL);
	ShortcutMaps.AddKey('P'          , KBM_KEYFLAG_NONE   , KBH_Simulation_Pause                   , NULL);
	ShortcutMaps.AddKey('P'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Parallel               , NULL);
	ShortcutMaps.AddKey('P'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_PWM                    , NULL);
	ShortcutMaps.AddKey('R'          , KBM_KEYFLAG_NONE   , KBH_Element_Make_Reset                 , NULL);
	ShortcutMaps.AddKey('R'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_DA_Ramp                , NULL);
	ShortcutMaps.AddKey('S'          , KBM_KEYFLAG_NONE   , KBH_Element_Make_Set                   , NULL);
	ShortcutMaps.AddKey('S'          , KBM_KEYFLAG_CONTROL, KBH_File_Save                          , NULL);
	ShortcutMaps.AddKey('T'          , KBM_KEYFLAG_NONE   , KBH_Element_Timer_Retentive            , NULL);
	ShortcutMaps.AddKey('U'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_USS_Write              , NULL);
	ShortcutMaps.AddKey('U'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_USS_Read               , NULL);
	ShortcutMaps.AddKey('V'          , KBM_KEYFLAG_CONTROL, KBH_Element_Paste                      , NULL);
	ShortcutMaps.AddKey('X'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_ModBUS_Write           , NULL);
	ShortcutMaps.AddKey('X'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_ModBUS_Read            , NULL);
	ShortcutMaps.AddKey('X'          , KBM_KEYFLAG_CONTROL, KBH_Element_Cut                        , NULL);
	ShortcutMaps.AddKey('Y'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Yaskawa_Write          , NULL);
	ShortcutMaps.AddKey('Y'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Yaskawa_Read           , NULL);
	ShortcutMaps.AddKey('Y'          , KBM_KEYFLAG_CONTROL, KBH_Redo                               , NULL);
	ShortcutMaps.AddKey('Z'          , KBM_KEYFLAG_NONE   , KBH_Element_Add_Reset                  , NULL);
	ShortcutMaps.AddKey('Z'          , KBM_KEYFLAG_CONTROL, KBH_Undo                               , NULL);
	ShortcutMaps.AddKey('1'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Comparison_NotEqual    , NULL);
	ShortcutMaps.AddKey('8'          , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Math_Multiply          , NULL);
	ShortcutMaps.AddKey(VK_OEM_PLUS  , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Math_Add               , NULL);
	ShortcutMaps.AddKey(VK_OEM_PLUS  , KBM_KEYFLAG_NONE   , KBH_Element_Add_Comparison_Equal       , NULL);
	ShortcutMaps.AddKey(VK_OEM_MINUS , KBM_KEYFLAG_NONE   , KBH_Element_Add_Math_Subtract          , NULL);
	ShortcutMaps.AddKey(VK_OEM_2     , KBM_KEYFLAG_NONE   , KBH_Element_Add_OneShot_Rising         , NULL);
	ShortcutMaps.AddKey(VK_DIVIDE    , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Math_Divide            , NULL);
	ShortcutMaps.AddKey(VK_OEM_5     , KBM_KEYFLAG_NONE   , KBH_Element_Add_OneShot_Falling        , NULL);
	ShortcutMaps.AddKey(VK_INSERT    , KBM_KEYFLAG_NONE   , KBH_Rung_Insert_After                  , NULL);
	ShortcutMaps.AddKey(VK_INSERT    , KBM_KEYFLAG_SHIFT  , KBH_Rung_Insert_Before                 , NULL);
	ShortcutMaps.AddKey(VK_OEM_1     , KBM_KEYFLAG_NONE   , KBH_Element_Add_Comment                , NULL);
	ShortcutMaps.AddKey(VK_OEM_PERIOD, KBM_KEYFLAG_NONE   , KBH_Element_Add_Comparison_Greater     , NULL);
	ShortcutMaps.AddKey(VK_OEM_PERIOD, KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Comparison_GreaterEqual, NULL);
	ShortcutMaps.AddKey(VK_OEM_COMMA , KBM_KEYFLAG_NONE   , KBH_Element_Add_Comparison_Less        , NULL);
	ShortcutMaps.AddKey(VK_OEM_COMMA , KBM_KEYFLAG_SHIFT  , KBH_Element_Add_Comparison_LessEqual   , NULL);
	ShortcutMaps.AddKey(' '          , KBM_KEYFLAG_NONE   , KBH_Element_Make_Normal                , NULL);

	KeyboardHandlers_LangChanged();
}

int KeyboardHandlers_Execute(WPARAM wParam)
{
//	Error("Tecla: %x", wParam);
	return ShortcutMaps.Execute(wParam);
}
