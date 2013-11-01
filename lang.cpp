#include "poptools.h"

#define SET_LANGUAGE_STRINGS(cmd,ptbr,en,es) case cmd: pPTBR = ptbr; pEN = en; pES = es; break;

typedef struct LangTableTag {
    char *from;
    char *to;
} LangTable;

typedef struct LangTag {
    LangTable   *tab;
    int          n;
} Lang;

// These are the actual translation tables, so should be included in just
// one place.
#include "lang-tables.h"

static Lang *l;

void setLanguage(unsigned int id)
{
	switch(id) {
	case 1: // Ingles
		l = nullptr;
		break;
	case 2: // Espanhol
		l = &LangEs;
		break;
	default: // Qualquer outro id: Portugues!
		l = &LangPt;
	}
}

PCWSTR getRibbonLocalizedExampleGalleryHeader(eExampleGalleryHeader header)
{
	PCWSTR pPTBR = L"", pEN = L"", pES = L"";

	switch(header) {
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_IO          , L"I/O"            , L"I/O"            , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Timers      , L"Temporizadores" , L"Timers"         , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Counters    , L"Contadores"     , L"Counters"       , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Variables   , L"Vari�veis"      , L"Variables"      , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Conditionals, L"Condicionais"   , L"Conditionals"   , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Mathematics , L"Matem�tica"     , L"Mathematics"    , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Analogs     , L"Anal�gicos"     , L"Analogs"        , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Motors      , L"Motores"        , L"Motors"         , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_ModBUS      , L"ModBUS"         , L"ModBUS"         , L"ModBUS");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_RS485_Text  , L"RS-485 - Texto" , L"RS-485 - Text"  , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_RS485_Others, L"RS-485 - Outros", L"RS-485 - Others", L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Applications, L"Aplica��es"     , L"Applications"   , L"I/O");
	}

	switch(POPSettings.idLanguage) {
		case  1: return pEN;
		case  2: return pES;
		default: return pPTBR;
	}
}

PCWSTR getRibbonLocalizedLabel(UINT nCmdID)
{
	PCWSTR pPTBR = L"", pEN = L"", pES = L"";

	switch(nCmdID) {
		SET_LANGUAGE_STRINGS(cmdTabHome                  , L"In�cio"                           , L"Home"                          , L"Inicio"                             );
		SET_LANGUAGE_STRINGS(cmdTabInstrLogic            , L"Controles L�gicos"                , L"Logic Controls"                , L"Controles logicos"                  );
		SET_LANGUAGE_STRINGS(cmdTabInstrAutom            , L"Controles Automa��o"              , L"Automation Controls"           , L"Controles de Automacion"            );
		SET_LANGUAGE_STRINGS(cmdTabSimulation            , L"Simula��o"                        , L"Simulation"                    , L"Simulacion"                         );
		SET_LANGUAGE_STRINGS(cmdGroupInstr               , L"Instru��o"                        , L"Instruction"                   , L"Instruccion"                        );
		SET_LANGUAGE_STRINGS(cmdGroupLine                , L"Linhas"                           , L"Lines"                         , L"Lineas"                             );
		SET_LANGUAGE_STRINGS(cmdGroupFind                , L"Busca"                            , L"Find"                          , L"Busqueda"                           );
		SET_LANGUAGE_STRINGS(cmdGroupConfig              , L"Configura��es"                    , L"Configuration"                 , L"Configuraciones"                    );
		SET_LANGUAGE_STRINGS(cmdGroupHelp                , L"Ajuda"                            , L"Help"                          , L"Ayuda"                              );
		SET_LANGUAGE_STRINGS(cmdGroupIO                  , L"I/O"                              , L"I/O"                           , L"I/O"                                );
		SET_LANGUAGE_STRINGS(cmdGroupTimer               , L"Temporizadores"                   , L"Timers"                        , L"Temporizadores"                     );
		SET_LANGUAGE_STRINGS(cmdGroupCounter             , L"Contadores"                       , L"Counters"                      , L"Contador"                           );
		SET_LANGUAGE_STRINGS(cmdGroupVar                 , L"Vari�veis"                        , L"Variables"                     , L"Variable"                           );
		SET_LANGUAGE_STRINGS(cmdGroupCond                , L"Condicionais"                     , L"Conditionals"                  , L"Condicionales"                      );
		SET_LANGUAGE_STRINGS(cmdGroupMath                , L"Matem�tica"                       , L"Mathematics"                   , L"Matematica"                         );
		SET_LANGUAGE_STRINGS(cmdGroupAnalog              , L"Anal�gicos"                       , L"Analogs"                       , L"Analogico"                          );
		SET_LANGUAGE_STRINGS(cmdGroupMotors              , L"Motores"                          , L"Motors"                        , L"Motores"                            );
		SET_LANGUAGE_STRINGS(cmdGroupEnc                 , L"Encoder Incremental"              , L"Incremental encoder"           , L"Encoder Incremental"                );
		SET_LANGUAGE_STRINGS(cmdGroupSSIEnc              , L"Encoder SSI"                      , L"SSI Encoder"                   , L"Encoder SSI"                        );
		SET_LANGUAGE_STRINGS(cmdGroupCommText            , L"RS-485 - Texto"                   , L"RS-485 - Text"                 , L"RS-485 - Texto"                     );
		SET_LANGUAGE_STRINGS(cmdGroupCommModBUS          , L"ModBUS"                           , L"ModBUS"                        , L"ModBUS"                             );
		SET_LANGUAGE_STRINGS(cmdGroupCommOthers          , L"RS-485 - Outros"                  , L"RS-485 - Others"               , L"RS-485 - Otros"                     );
		SET_LANGUAGE_STRINGS(cmdDropDownIO               , L"Mais I/O"                         , L"More I/O"                      , L"Mas I/O"                            );
		SET_LANGUAGE_STRINGS(cmdDropDownMath             , L"Mais"                             , L"More"                          , L"Mas"                                );
		SET_LANGUAGE_STRINGS(cmdGroupSimulation          , L"Simula��o"                        , L"Simulation"                    , L"Simulacion"                         );
		SET_LANGUAGE_STRINGS(cmdGroupSimulLog            , L"Registro de Dados"                , L"Data log"                      , L"Registro de Datos"                  );
		SET_LANGUAGE_STRINGS(cmdGroupSimulExit           , L"Sair"                             , L"Exit"                          , L"Salir"                              );
		SET_LANGUAGE_STRINGS(cmdGroupActions             , L"A��es"                            , L"Actions"                       , L"Acciones"                           );
		SET_LANGUAGE_STRINGS(cmdGroupComment             , L"Documenta��o"                     , L"Documentation"                 , L"Documentos"                         );
		SET_LANGUAGE_STRINGS(cmdGroupNavigate            , L"Navega��o"                        , L"Navigation"                    , L"Navegacion"                         );

		SET_LANGUAGE_STRINGS(cmdRecentList               , L"Projetos Recentes"                , L"Recent Projects"               , L"Proyectos Recientes"                );
		SET_LANGUAGE_STRINGS(cmdFileNew                  , L"Novo"                             , L"New"                           , L"Nuevo"                              );
		SET_LANGUAGE_STRINGS(cmdFileOpen                 , L"Abrir"                            , L"Open"                          , L"Abrir"                              );
		SET_LANGUAGE_STRINGS(cmdFileClose                , L"Fechar"                           , L"Close"                         , L"Cerrar"                             );
		SET_LANGUAGE_STRINGS(cmdFileSave                 , L"Salvar"                           , L"Save"                          , L"Salvar"                             );
		SET_LANGUAGE_STRINGS(cmdFileSaveAsLadder         , L"Salvar como Diagrama Ladder"      , L"Save as Ladder Diagram"        , L"Salvar como Diagrama Ladder"        );
		SET_LANGUAGE_STRINGS(cmdFileSaveAsC              , L"Exportar como arquivo C"          , L"Export as C File"              , L"Exportar como archivo C"            );
		SET_LANGUAGE_STRINGS(cmdFileSaveAsText           , L"Exportar como arquivo texto"      , L"Export as Text File"           , L"Exportar como archivo texto"        );
		SET_LANGUAGE_STRINGS(cmdPrint                    , L"Imprimir..."                      , L"Print..."                      , L"Imprimir"                           );
		SET_LANGUAGE_STRINGS(cmdAbout                    , L"Sobre..."                         , L"About..."                      , L"Sobre..."                           );
		SET_LANGUAGE_STRINGS(cmdExit                     , L"Sair"                             , L"Exit"                          , L"Salir"                              );

		SET_LANGUAGE_STRINGS(cmdUndo                     , L"Desfazer"                         , L"Undo"                          , L"Deshacer"                           );
		SET_LANGUAGE_STRINGS(cmdRedo                     , L"Refazer"                          , L"Redo"                          , L"Rehacer"                            );
		SET_LANGUAGE_STRINGS(cmdMoveLineUp               , L"Mover Acima"                      , L"Move Up"                       , L"Mover hacia arriba"                 );
		SET_LANGUAGE_STRINGS(cmdMoveLineDown             , L"Mover Abaixo"                     , L"Move Down"                     , L"Mover hacia abajo"                  );
		SET_LANGUAGE_STRINGS(cmdCopyLine                 , L"Copiar Linha"                     , L"Copy Line"                     , L"Copiar Linea"                       );
		SET_LANGUAGE_STRINGS(cmdPasteLineBefore          , L"Colar Antes"                      , L"Paste Before"                  , L"Pegar antes"                        );
		SET_LANGUAGE_STRINGS(cmdPasteLineAfter           , L"Colar Depois"                     , L"Paste After"                   , L"Pegar despues"                      );
		SET_LANGUAGE_STRINGS(cmdInsertLineAfter          , L"Inserir Ap�s"                     , L"Insert After"                  , L"Inserir despues"                    );
		SET_LANGUAGE_STRINGS(cmdInsertLineBefore         , L"Inserir Antes"                    , L"Insert Before"                 , L"Inserir antes"                      );
		SET_LANGUAGE_STRINGS(cmdGoHome                   , L"In�cio"                           , L"Home"                          , L"Inicio"                             );
		SET_LANGUAGE_STRINGS(cmdGoEnd                    , L"Final"                            , L"End"                           , L"Final"                              );
		SET_LANGUAGE_STRINGS(cmdInstrParallel            , L"Paralelo"                         , L"Parallel"                      , L"Paralelo"                           );
		SET_LANGUAGE_STRINGS(cmdCopyInstr                , L"Copiar"                           , L"Copy"                          , L"Copiar"                             );
		SET_LANGUAGE_STRINGS(cmdPasteInstr               , L"Colar"                            , L"Paste"                         , L"Pegar"                              );
		SET_LANGUAGE_STRINGS(cmdDeleteInstr              , L"Excluir"                          , L"Delete"                        , L"Excluir"                            );
		SET_LANGUAGE_STRINGS(cmdDeleteLine               , L"Excluir"                          , L"Delete"                        , L"Nuevo");
		SET_LANGUAGE_STRINGS(cmdFindDialog               , L"Localizar e Substituir"           , L"Find and Replace"              , L"Localizar y Substituir"             );
		SET_LANGUAGE_STRINGS(cmdFind                     , L"Localizar Pr�xima"                , L"Find Next"                     , L"Localizar Proxima"                  );

		SET_LANGUAGE_STRINGS(cmdInstrComment             , L"Coment�rio"                       , L"Comment"                       , L"Comentario"                         );

		SET_LANGUAGE_STRINGS(cmdInstrContact             , L"Contato"                          , L"Contact"                       , L"Contacto"                           );
		SET_LANGUAGE_STRINGS(cmdInstrCoil                , L"Bobina"                           , L"Coil"                          , L"Bobina"                             );
		SET_LANGUAGE_STRINGS(cmdInstrOneShotFalling      , L"Borda de Descida"                 , L"One Shot Falling"              , L"Borde de bajada"                    );
		SET_LANGUAGE_STRINGS(cmdInstrOneShotRising       , L"Borda de Subida"                  , L"One Shot Rising"               , L"Borde de subida"                    );
		SET_LANGUAGE_STRINGS(cmdInstrCircuitOpen         , L"Circuito Aberto"                  , L"Open Circuit"                  , L"Circuito abierto"                   );
		SET_LANGUAGE_STRINGS(cmdInstrCircuitClosed       , L"Circuito Fechado"                 , L"Closed Circuit"                , L"Circuito abierto"                   );
		SET_LANGUAGE_STRINGS(cmdInstrMasterRelayControl  , L"Chave Geral"                      , L"Master Relay Control"          , L"Llave general"                      );

		SET_LANGUAGE_STRINGS(cmdInstrTimerON             , L"Atraso para Ligar"                , L"Timer ON"                      , L"Retrazo para prender"               );
		SET_LANGUAGE_STRINGS(cmdInstrTimerOFF            , L"Atraso para Desligar"             , L"Timer OFF"                     , L"Retrazo para apagar"                );
		SET_LANGUAGE_STRINGS(cmdInstrTimerONRet          , L"Atraso com Mem�ria"               , L"Timer ON Retentive"            , L"Retrazo con memoria"                );
		SET_LANGUAGE_STRINGS(cmdInstrSchedule            , L"Agendador de Tarefa"              , L"Scheduler"                     , L"Agenda de tareas"                   );
		SET_LANGUAGE_STRINGS(cmdInstrTimerReset          , L"Zerar"                            , L"Reset"                         , L"______"                             );

		SET_LANGUAGE_STRINGS(cmdInstrCounterInc          , L"Incremental"                      , L"Incremental"                   , L"Incremental"                        );
		SET_LANGUAGE_STRINGS(cmdInstrCounterDec          , L"Decremental"                      , L"Decremental"                   , L"Decrementalo"                       );
		SET_LANGUAGE_STRINGS(cmdInstrCounterCirc         , L"Circular"                         , L"Circular"                      , L"Circular"                           );
		SET_LANGUAGE_STRINGS(cmdInstrCounterReset        , L"Zerar"                            , L"Reset"                         , L"______"                             );

		SET_LANGUAGE_STRINGS(cmdInstrMathAdd             , L"Somar"                            , L"Add"                           , L"Sumar"                              );
		SET_LANGUAGE_STRINGS(cmdInstrMathSub             , L"Subtrair"                         , L"Subtract"                      , L"Subtrair"                           );
		SET_LANGUAGE_STRINGS(cmdInstrMathMult            , L"Multiplicar"                      , L"Multiply"                      , L"Multiplicar"                        );
		SET_LANGUAGE_STRINGS(cmdInstrMathDivide          , L"Dividir"                          , L"Divide"                        , L"Dividir"                            );
		SET_LANGUAGE_STRINGS(cmdInstrMathModulo          , L"Resto da Divis�o"                 , L"Modulo"                        , L"Modulo"                             );
		SET_LANGUAGE_STRINGS(cmdInstrMathSqrt            , L"Raiz Quadrada"                    , L"Square Root"                   , L"Raiz Cuadrada"                      );
		SET_LANGUAGE_STRINGS(cmdInstrShiftRegister       , L"Fila de Vari�veis"                , L"Shift Register"                , L"Fila de Variables"                  );
		SET_LANGUAGE_STRINGS(cmdInstrLookUpTable         , L"Tabela de Busca"                  , L"Look-Up Table"                 , L"Tabla de Busca"                     );
		SET_LANGUAGE_STRINGS(cmdInstrPieceWiseLinear     , L"Lineariza��o por Segmentos"       , L"Piecewise Linear"              , L"Linear por seguimientos"            );
		SET_LANGUAGE_STRINGS(cmdInstrRand                , L"N�mero Rand�mico"                 , L"Random Number"                 , L"Numero Randonico"                   );
		SET_LANGUAGE_STRINGS(cmdInstrAbs                 , L"Valor Absoluto"                   , L"Absolute Value"                , L"Valor Absoluto"                     );

		SET_LANGUAGE_STRINGS(cmdInstrMov                 , L"Mover Valor"                      , L"Move Value"                    , L"Mover valor"                        );
		SET_LANGUAGE_STRINGS(cmdInstrSetBit              , L"Liga / Desliga Bit"               , L"Set / Reset Bit"               , L"Prende / Apaga Bit"                 );
		SET_LANGUAGE_STRINGS(cmdInstrCheckBit            , L"Checar Bit"                       , L"Check Bit"                     , L"Chequear Bit"                       );
		SET_LANGUAGE_STRINGS(cmdInstrPersist             , L"Persistente"                      , L"Persist"                       , L"Persistente"                        );

		SET_LANGUAGE_STRINGS(cmdInstrCondEqual           , L"Igual"                            , L"Equal"                         , L"Igual"                              );
		SET_LANGUAGE_STRINGS(cmdInstrCondNotEqual        , L"Diferente"                        , L"Not Equal"                     , L"Diferente"                          );
		SET_LANGUAGE_STRINGS(cmdInstrCondGreater         , L"Maior"                            , L"Greater"                       , L"Mayor"                              );
		SET_LANGUAGE_STRINGS(cmdInstrCondGreaterEqual    , L"Maior ou Igual"                   , L"Greater or Equal"              , L"Mayor o Igual"                      );
		SET_LANGUAGE_STRINGS(cmdInstrCondLesser          , L"Menor"                            , L"Lesser"                        , L"Menor"                              );
		SET_LANGUAGE_STRINGS(cmdInstrCondLesserEqual     , L"Menor ou Igual"                   , L"Lesser or Equal"               , L"Menor o Igual"                      );

		SET_LANGUAGE_STRINGS(cmdInstrReadAD              , L"Leitura de A/D"                   , L"Read A/D"                      , L"Lectura de A/D"                     );
		SET_LANGUAGE_STRINGS(cmdInstrWriteDA             , L"Escrita no D/A"                   , L"Write D/A"                     , L"Escrita no D/A"                     );

		SET_LANGUAGE_STRINGS(cmdInstrPWM                 , L"Sa�da PWM"                        , L"PWM Output"                    , L"Salida PWM"                         );
		SET_LANGUAGE_STRINGS(cmdInstrRampDA              , L"Rampa de Acelera��o/Desacelera��o", L"Acceleration/Deceleration Ramp", L"Rampa de Aceleracion/Desaceleracion");
		SET_LANGUAGE_STRINGS(cmdInstrReadEnc             , L"Leitura de Encoder"               , L"Read Encoder"                  , L"Lectura de Encoder"                 );
		SET_LANGUAGE_STRINGS(cmdInstrResetEnc            , L"Escrita no Encoder"               , L"Write Encoder"                 , L"Escrita de Encoder"                 );
		SET_LANGUAGE_STRINGS(cmdInstrPID                 , L"PID"                              , L"PID"                           , L"PID"                                );

		SET_LANGUAGE_STRINGS(cmdInstrWriteChar           , L"Escrita de Caracter"              , L"Write Char"                    , L"Escrita de Caracteres"              );
		SET_LANGUAGE_STRINGS(cmdInstrReadChar            , L"Leitura de Caracter"              , L"Read Char"                     , L"Lectura de Caracteres"              );
		SET_LANGUAGE_STRINGS(cmdInstrReadFormatString    , L"Ler String Formatada"             , L"Read Formatted String"         , L"Leer String Formatada"              );
		SET_LANGUAGE_STRINGS(cmdInstrWriteFormatString   , L"Escrever String Formatada"        , L"Write Formatted String"        , L"Escribir String Formatada"          );

		SET_LANGUAGE_STRINGS(cmdInstrReadUSS             , L"Leitura USS"                      , L"Read USS"                      , L"Lectura USS"                        );
		SET_LANGUAGE_STRINGS(cmdInstrWriteUSS            , L"Escrita USS"                      , L"Write USS"                     , L"Escrita USS"                        );
		SET_LANGUAGE_STRINGS(cmdInstrReadNS600           , L"Leitura de NS-600 Yaskawa"        , L"Read NS-600 Yaskawa"           , L"Lectura de NS-600 Yaskawa"          );
		SET_LANGUAGE_STRINGS(cmdInstrWriteNS600          , L"Escrita de NS-600 Yaskawa"        , L"Write NS-600 Yaskawa"          , L"Escrita de NS-600 Yaskawa"          );

		SET_LANGUAGE_STRINGS(cmdInstrReadModBUS          , L"Ler Registrador ModBUS"           , L"Read ModBUS Register"          , L"Leer Registrador ModBUS"            );
		SET_LANGUAGE_STRINGS(cmdInstrWriteModBUS         , L"Escrever Registrador ModBUS"      , L"Write ModBUS Register"         , L"Escribir Registrador ModBUS"        );

		SET_LANGUAGE_STRINGS(cmdConfig                   , L"Projeto"                          , L"Project"                       , L"Proyecto"                           );
		SET_LANGUAGE_STRINGS(cmdSettings                 , L"Prefer�ncias"                     , L"Settings"                      , L"Preferencias"                       );

		SET_LANGUAGE_STRINGS(cmdSimulation               , L"Modo de Simula��o"                , L"Simulation mode"               , L"Modo de Simulacion"                 );
		SET_LANGUAGE_STRINGS(cmdSimulationExit           , L"Sair da Simula��o"                , L"Exit Simulation"               , L"Salir de Simulacion"                );
		SET_LANGUAGE_STRINGS(cmdSimulationStart          , L"Iniciar"                          , L"Start"                         , L"Iniciar"                            );
		SET_LANGUAGE_STRINGS(cmdSimulationPause          , L"Pausar"                           , L"Pause"                         , L"Pausar"                             );
		SET_LANGUAGE_STRINGS(cmdSimulationStop           , L"Interromper"                      , L"Stop"                          , L"Interrumpir"                        );
		SET_LANGUAGE_STRINGS(cmdSimulationSingleCycle    , L"Simples Ciclo"                    , L"Single Cycle"                  , L"Simples ciclo"                      );
		SET_LANGUAGE_STRINGS(cmdSimulationLogStart       , L"Iniciar"                          , L"Start"                         , L"Iniciar"                            );
		SET_LANGUAGE_STRINGS(cmdSimulationLogStop        , L"Parar"                            , L"Stop"                          , L"Parar"                              );

		SET_LANGUAGE_STRINGS(cmdCompile                  , L"Compilar"                         , L"Compile"                       , L"Compilar"                           );
		SET_LANGUAGE_STRINGS(cmdCompileAndProgram        , L"Compilar e Gravar"                , L"Compile and Program"           , L"Compilar y Grabar"                  );

		SET_LANGUAGE_STRINGS(cmdDebug                    , L"Depurar"                          , L"Debug"                         , L"Depurar"                            );

		SET_LANGUAGE_STRINGS(cmdUserManual               , L"Manual"                           , L"Manual"                        , L"Manual"                             );
		SET_LANGUAGE_STRINGS(cmdKeyboardManual           , L"Teclado"                          , L"Keyboard"                      , L"Teclado"                            );

		// Example comands list
		SET_LANGUAGE_STRINGS(cmdExamples                 , L"Exemplos"                         , L"Examples"                       , L"Nuevo"                             );

		SET_LANGUAGE_STRINGS(cmdExampleComment           , L"Coment�rio"                       , L"Comment"                       , L"Comentario"                         );

		SET_LANGUAGE_STRINGS(cmdExampleContact           , L"Contato"                          , L"Contact"                       , L"Contacto"                           );
		SET_LANGUAGE_STRINGS(cmdExampleCoil              , L"Bobina"                           , L"Coil"                          , L"Bobina"                             );
		SET_LANGUAGE_STRINGS(cmdExampleOneShotFalling    , L"Borda de Descida"                 , L"One Shot Falling"              , L"Borde de bajada"                    );
		SET_LANGUAGE_STRINGS(cmdExampleOneShotRising     , L"Borda de Subida"                  , L"One Shot Rising"               , L"Borde de subida"                    );
		SET_LANGUAGE_STRINGS(cmdExampleCircuitOpen       , L"Circuito Aberto"                  , L"Open Circuit"                  , L"Circuito abierto"                   );
		SET_LANGUAGE_STRINGS(cmdExampleCircuitClosed     , L"Circuito Fechado"                 , L"Closed Circuit"                , L"Circuito abierto"                   );
		SET_LANGUAGE_STRINGS(cmdExampleMasterRelayControl, L"Chave Geral"                      , L"Master Relay Control"          , L"Llave general"                      );

		SET_LANGUAGE_STRINGS(cmdExampleTimerON           , L"Atraso para Ligar"                , L"Timer ON"                      , L"Retrazo para prender"               );
		SET_LANGUAGE_STRINGS(cmdExampleTimerOFF          , L"Atraso para Desligar"             , L"Timer OFF"                     , L"Retrazo para apagar"                );
		SET_LANGUAGE_STRINGS(cmdExampleTimerONRet        , L"Atraso com Mem�ria"               , L"Timer ON Retentive"            , L"Retrazo con memoria"                );
		SET_LANGUAGE_STRINGS(cmdExampleSchedule          , L"Agendador de Tarefa"              , L"Scheduler"                     , L"Agenda de tareas"                   );
		SET_LANGUAGE_STRINGS(cmdExampleTimerReset        , L"Zerar"                            , L"Reset"                         , L"______"                             );

		SET_LANGUAGE_STRINGS(cmdExampleCounterInc        , L"Incremental"                      , L"Incremental"                   , L"Incremental"                        );
		SET_LANGUAGE_STRINGS(cmdExampleCounterDec        , L"Decremental"                      , L"Decremental"                   , L"Decrementalo"                       );
		SET_LANGUAGE_STRINGS(cmdExampleCounterCirc       , L"Circular"                         , L"Circular"                      , L"Circular"                           );
		SET_LANGUAGE_STRINGS(cmdExampleCounterReset      , L"Zerar"                            , L"Reset"                         , L"______"                             );

		SET_LANGUAGE_STRINGS(cmdExampleMathAdd           , L"Somar"                            , L"Add"                           , L"Sumar"                              );
		SET_LANGUAGE_STRINGS(cmdExampleMathSub           , L"Subtrair"                         , L"Subtract"                      , L"Subtrair"                           );
		SET_LANGUAGE_STRINGS(cmdExampleMathMult          , L"Multiplicar"                      , L"Multiply"                      , L"Multiplicar"                        );
		SET_LANGUAGE_STRINGS(cmdExampleMathDivide        , L"Dividir"                          , L"Divide"                        , L"Dividir"                            );
		SET_LANGUAGE_STRINGS(cmdExampleMathModulo        , L"Resto da Divis�o"                 , L"Modulo"                        , L"Modulo"                             );
		SET_LANGUAGE_STRINGS(cmdExampleMathSqrt          , L"Raiz Quadrada"                    , L"Square Root"                   , L"Raiz Cuadrada"                      );
		SET_LANGUAGE_STRINGS(cmdExampleShiftRegister     , L"Fila de Vari�veis"                , L"Shift Register"                , L"Fila de Variables"                  );
		SET_LANGUAGE_STRINGS(cmdExampleLookUpTable       , L"Tabela de Busca"                  , L"Look-Up Table"                 , L"Tabla de Busca"                     );
		SET_LANGUAGE_STRINGS(cmdExamplePieceWiseLinear   , L"Lineariza��o por Segmentos"       , L"Piecewise Linear"              , L"Linear por seguimientos"            );
		SET_LANGUAGE_STRINGS(cmdExampleRand              , L"N�mero Rand�mico"                 , L"Random Number"                 , L"Numero Randonico"                   );
		SET_LANGUAGE_STRINGS(cmdExampleAbs               , L"Valor Absoluto"                   , L"Absolute Value"                , L"Valor Absoluto"                     );

		SET_LANGUAGE_STRINGS(cmdExampleMov               , L"Mover Valor"                      , L"Move Value"                    , L"Mover valor"                        );
		SET_LANGUAGE_STRINGS(cmdExampleSetBit            , L"Liga / Desliga Bit"               , L"Set / Reset Bit"               , L"Prende / Apaga Bit"                 );
		SET_LANGUAGE_STRINGS(cmdExampleCheckBit          , L"Checar Bit"                       , L"Check Bit"                     , L"Chequear Bit"                       );
		SET_LANGUAGE_STRINGS(cmdExamplePersist           , L"Persistente"                      , L"Persist"                       , L"Persistente"                        );

		SET_LANGUAGE_STRINGS(cmdExampleCondEqual         , L"Igual"                            , L"Equal"                         , L"Igual"                              );
		SET_LANGUAGE_STRINGS(cmdExampleCondNotEqual      , L"Diferente"                        , L"Not Equal"                     , L"Diferente"                          );
		SET_LANGUAGE_STRINGS(cmdExampleCondGreater       , L"Maior"                            , L"Greater"                       , L"Mayor"                              );
		SET_LANGUAGE_STRINGS(cmdExampleCondGreaterEqual  , L"Maior ou Igual"                   , L"Greater or Equal"              , L"Mayor o Igual"                      );
		SET_LANGUAGE_STRINGS(cmdExampleCondLesser        , L"Menor"                            , L"Lesser"                        , L"Menor"                              );
		SET_LANGUAGE_STRINGS(cmdExampleCondLesserEqual   , L"Menor ou Igual"                   , L"Lesser or Equal"               , L"Menor o Igual"                      );

		SET_LANGUAGE_STRINGS(cmdExampleReadAD            , L"Leitura de A/D"                   , L"Read A/D"                      , L"Lectura de A/D"                     );
		SET_LANGUAGE_STRINGS(cmdExampleWriteDA           , L"Escrita no D/A"                   , L"Write D/A"                     , L"Escrita no D/A"                     );

		SET_LANGUAGE_STRINGS(cmdExamplePWM               , L"Sa�da PWM"                        , L"PWM Output"                    , L"Salida PWM"                         );
		SET_LANGUAGE_STRINGS(cmdExampleRampDA            , L"Rampa de Acelera��o/Desacelera��o", L"Acceleration/Deceleration Ramp", L"Rampa de Aceleracion/Desaceleracion");
		SET_LANGUAGE_STRINGS(cmdExampleReadEnc           , L"Leitura de Encoder"               , L"Read Encoder"                  , L"Lectura de Encoder"                 );
		SET_LANGUAGE_STRINGS(cmdExampleResetEnc          , L"Escrita no Encoder"               , L"Write Encoder"                 , L"Escrita de Encoder"                 );
		SET_LANGUAGE_STRINGS(cmdExamplePID               , L"PID"                              , L"PID"                           , L"PID"                                );

		SET_LANGUAGE_STRINGS(cmdExampleWriteChar         , L"Escrita de Caracter"              , L"Write Char"                    , L"Escrita de Caracteres"              );
		SET_LANGUAGE_STRINGS(cmdExampleReadChar          , L"Leitura de Caracter"              , L"Read Char"                     , L"Lectura de Caracteres"              );
		SET_LANGUAGE_STRINGS(cmdExampleReadFormatString  , L"Ler String Formatada"             , L"Read Formatted String"         , L"Leer String Formatada"              );
		SET_LANGUAGE_STRINGS(cmdExampleWriteFormatString , L"Escrever String Formatada"        , L"Write Formatted String"        , L"Escribir String Formatada"          );

		SET_LANGUAGE_STRINGS(cmdExampleReadUSS           , L"Leitura USS"                      , L"Read USS"                      , L"Lectura USS"                        );
		SET_LANGUAGE_STRINGS(cmdExampleWriteUSS          , L"Escrita USS"                      , L"Write USS"                     , L"Escrita USS"                        );
		SET_LANGUAGE_STRINGS(cmdExampleReadNS600         , L"Leitura de NS-600 Yaskawa"        , L"Read NS-600 Yaskawa"           , L"Lectura de NS-600 Yaskawa"          );
		SET_LANGUAGE_STRINGS(cmdExampleWriteNS600        , L"Escrita de NS-600 Yaskawa"        , L"Write NS-600 Yaskawa"          , L"Escrita de NS-600 Yaskawa"          );

		SET_LANGUAGE_STRINGS(cmdExampleReadModBUS        , L"Ler Registrador ModBUS"           , L"Read ModBUS Register"          , L"Leer Registrador ModBUS"            );
		SET_LANGUAGE_STRINGS(cmdExampleWriteModBUS       , L"Escrever Registrador ModBUS"      , L"Write ModBUS Register"         , L"Escribir Registrador ModBUS"        );

		SET_LANGUAGE_STRINGS(cmdExampleAdcLed            , L"Barra de Leds - A/D"              , L"Leds Bar - A/D"                , L"Barra de Leds - A/D"                );
		SET_LANGUAGE_STRINGS(cmdExampleSemaphore         , L"Sem�foro"                         , L"Semaphore"                     , L"Semaforo"                           );
		SET_LANGUAGE_STRINGS(cmdExampleTester            , L"Testador"                         , L"Tester"                        , L"Testeador"                          );
	}

	switch(POPSettings.idLanguage) {
		case  1: return pEN;
		case  2: return pES;
		default: return pPTBR;
	}
}

PCWSTR getRibbonLocalizedTooltipTitle(UINT nCmdID)
{
	PCWSTR pPTBR = L"", pEN = L"", pES = L"";

	switch(nCmdID) {
		SET_LANGUAGE_STRINGS(cmdFileNew                  , L"Novo (Control + N)"                           , L"New (Control + N)"                         , L"Nuevo (Control + N)"                            );
		SET_LANGUAGE_STRINGS(cmdFileOpen                 , L"Abrir (Control + O)"                          , L"Open (Control + O)"                        , L"Abrir (Control + O)"                            );
		SET_LANGUAGE_STRINGS(cmdFileSave                 , L"Salvar (Control + S)"                         , L"Save (Control + S)"                        , L"Salvar (Control + S)"                           );
		SET_LANGUAGE_STRINGS(cmdFileSaveAsLadder         , L"Salvar como Diagrama Ladder"                         , L"Save (Control + S)"                        , L"Salvar (Control + S)"                           );
		SET_LANGUAGE_STRINGS(cmdFileSaveAsText           , L"Exportar como arquivo texto (Control + E)"    , L"Export as Text File (Control + E)"         , L"Exportar como archivo texto (Control + E)"      );

		SET_LANGUAGE_STRINGS(cmdUndo                     , L"Desfazer (Control + Z)"                       , L"Undo (Control + Z)"                        , L"Deshacer (Control + Z)"                         );
		SET_LANGUAGE_STRINGS(cmdRedo                     , L"Refazer (Control + Y)"                        , L"Redo (Control + Y)"                        , L"Rehacer (Control + Y)"                          );
		SET_LANGUAGE_STRINGS(cmdMoveLineUp               , L"Mover Acima (Shift + Cima)"                   , L"Move Up (Shift + Up)"                      , L"Mover hacia arriba (Shift + Arriba)"            );
		SET_LANGUAGE_STRINGS(cmdMoveLineDown             , L"Mover Abaixo (Shift + Baixo)"                 , L"Move Down (Shift + Down)"                  , L"Mover hacia abajo (Shift + Abajo)"              );
		SET_LANGUAGE_STRINGS(cmdInsertLineAfter          , L"Inserir Ap�s (Insert)"                        , L"Insert After (Insert)"                     , L"Inserir despues (Insert)"                       );
		SET_LANGUAGE_STRINGS(cmdInsertLineBefore         , L"Inserir Antes (Shift + Insert)"               , L"Insert Before (Shift + Insert)"            , L"Inserir antes (Shift + Insert)"                 );
		SET_LANGUAGE_STRINGS(cmdGoHome                   , L"In�cio (Shift + Home)"                        , L"Home (Shift + Home)"                       , L"Inicio (Shift + Home)"                          );
		SET_LANGUAGE_STRINGS(cmdGoEnd                    , L"Final (Shift + End)"                          , L"End (Shift + End)"                         , L"Final (Shift + End)"                            );
		SET_LANGUAGE_STRINGS(cmdInstrParallel            , L"Paralelo (P)"                                 , L"Parallel (P)"                              , L"Paralelo (P)"                                   );
		SET_LANGUAGE_STRINGS(cmdCopyInstr                , L"Copiar (Control + C)"                         , L"Copy (Control + C)"                        , L"Copiar (Control + C)"                           );
		SET_LANGUAGE_STRINGS(cmdPasteInstr               , L"Colar (Control + V)"                            , L"Paste (Control + V)"                     , L"Pegar (Control + V)"                            );
		SET_LANGUAGE_STRINGS(cmdDeleteInstr              , L"Excluir (Delete)"                             , L"Delete (Delete)"                           , L"Excluir (Delete)"                               );
		SET_LANGUAGE_STRINGS(cmdDeleteLine               , L"Excluir (Shift + Delete)"                     , L"Delete (Shift + Delete)"                   , L"Excluir (Shift + Delete)"                       );
		SET_LANGUAGE_STRINGS(cmdFindDialog               , L"Localizar e Substituir (F3)"                  , L"Find and Replace (F3)"                     , L"Localizar y Substituir (F3)"                    );
		SET_LANGUAGE_STRINGS(cmdFind                     , L"Localizar Pr�xima (Shift+F3)"                 , L"Find Next (Shift+F3)"                      , L"Localizar Proxima (Shift+F3)"                   );

		SET_LANGUAGE_STRINGS(cmdInstrComment             , L"Coment�rio (;)"                               , L"Comment (;)"                               , L"Comentario (;)"                                 );

		SET_LANGUAGE_STRINGS(cmdInstrContact             , L"Contato (C)"                                  , L"Contact (C)"                               , L"Contacto (C)"                                   );
		SET_LANGUAGE_STRINGS(cmdInstrCoil                , L"Bobina (B)"                                   , L"Coil (B)"                                  , L"Bobina (B)"                                     );
		SET_LANGUAGE_STRINGS(cmdInstrOneShotFalling      , L"Borda de Descida (\\)"                        , L"One Shot Falling (\\)"                     , L"Borde de bajada (\\)"                           );
		SET_LANGUAGE_STRINGS(cmdInstrOneShotRising       , L"Borda de Subida (/)"                          , L"One Shot Rising (/)"                       , L"Borde de subida (/)"                            );

		SET_LANGUAGE_STRINGS(cmdInstrTimerON             , L"Atraso para Ligar (A)"                        , L"Timer ON (A)"                              , L"Retrazo para prender (A)"                       );
		SET_LANGUAGE_STRINGS(cmdInstrTimerOFF            , L"Atraso para Desligar (Shift + A)"             , L"Timer OFF (Shift + A)"                     , L"Retrazo para apagar (Shift + A)"                );
		SET_LANGUAGE_STRINGS(cmdInstrTimerONRet          , L"Atraso com Mem�ria (T)"                       , L"Timer ON Retentive (T)"                    , L"Retrazo con memoria (T)"                        );
		SET_LANGUAGE_STRINGS(cmdInstrTimerReset          , L"Zerar (Z)"                                    , L"Reset (Z)"                                 , L"______ (Z)"                                     );

		SET_LANGUAGE_STRINGS(cmdInstrCounterInc          , L"Incremental (I)"                              , L"Incremental (I)"                           , L"Incremental (I)"                                );
		SET_LANGUAGE_STRINGS(cmdInstrCounterDec          , L"Decremental (D)"                              , L"Decremental (D)"                           , L"Decrementalo (D)"                               );
		SET_LANGUAGE_STRINGS(cmdInstrCounterCirc         , L"Circular (Shift + C)"                         , L"Circular (Shift + C)"                      , L"Circular (Shift + C)"                           );
		SET_LANGUAGE_STRINGS(cmdInstrCounterReset        , L"Zerar (Z)"                                    , L"Reset (Z)"                                 , L"______ (Z)"                                     );

		SET_LANGUAGE_STRINGS(cmdInstrMathAdd             , L"Somar (+)"                                    , L"Add (+)"                                   , L"Sumar (+)"                                      );
		SET_LANGUAGE_STRINGS(cmdInstrMathSub             , L"Subtrair (-)"                                 , L"Subtract (-)"                              , L"Subtrair (-)"                                   );
		SET_LANGUAGE_STRINGS(cmdInstrMathMult            , L"Multiplicar (*)"                              , L"Multiply (*)"                              , L"Multiplicar (*)"                                );
		SET_LANGUAGE_STRINGS(cmdInstrMathDivide          , L"Dividir (Shift + /)"                          , L"Divide (Shift + /)"                        , L"Dividir (Shift + /)"                            );

		SET_LANGUAGE_STRINGS(cmdInstrMov                 , L"Mover Valor (M)"                              , L"Move Value (M)"                            , L"Mover valor (M)"                                );
		SET_LANGUAGE_STRINGS(cmdInstrSetBit              , L"Liga / Desliga Bit (Control + B)"             , L"Set / Reset Bit (Control + B)"             , L"Prende / Apaga Bit (Control + B)"               );
		SET_LANGUAGE_STRINGS(cmdInstrCheckBit            , L"Checar Bit (Shift + B)"                       , L"Check Bit (Shift + B)"                     , L"Chequear Bit (Shift + B)"                       );

		SET_LANGUAGE_STRINGS(cmdInstrCondEqual           , L"Igual (=)"                                    , L"Equal (=)"                                 , L"Igual (=)"                                      );
		SET_LANGUAGE_STRINGS(cmdInstrCondNotEqual        , L"Diferente (!)"                                , L"Not Equal (!)"                             , L"Diferente (!)"                                  );
		SET_LANGUAGE_STRINGS(cmdInstrCondGreater         , L"Maior (>)"                                    , L"Greater (>)"                               , L"Mayor (>)"                                      );
		SET_LANGUAGE_STRINGS(cmdInstrCondGreaterEqual    , L"Maior ou Igual (.)"                           , L"Greater or Equal (.)"                      , L"Mayor o Igual (.)"                              );
		SET_LANGUAGE_STRINGS(cmdInstrCondLesser          , L"Menor (<)"                                    , L"Lesser (<)"                                , L"Menor (<)"                                      );
		SET_LANGUAGE_STRINGS(cmdInstrCondLesserEqual     , L"Menor ou Igual (,)"                           , L"Lesser or Equal (,)"                       , L"Menor o Igual (,)"                              );

		SET_LANGUAGE_STRINGS(cmdInstrReadAD              , L"Leitura de A/D (Control + A)"                 , L"Read A/D (Control + A)"                    , L"Lectura de A/D (Control + A)"                   );
		SET_LANGUAGE_STRINGS(cmdInstrWriteDA             , L"Escrita no D/A (Control + D)"                 , L"Write D/A (Control + D)"                   , L"Escrita no D/A (Control + D)"                   );

		SET_LANGUAGE_STRINGS(cmdInstrPWM                 , L"Sa�da PWM (Shift + P)"                        , L"PWM Output (Shift + P)"                    , L"Salida PWM (Shift + P)"                         );
		SET_LANGUAGE_STRINGS(cmdInstrRampDA              , L"Rampa de Acelera��o/Desacelera��o (Shift + R)", L"Acceleration/Deceleration Ramp (Shift + R)", L"Rampa de Aceleracion/Desaceleracion (Shift + R)");
		SET_LANGUAGE_STRINGS(cmdInstrReadEnc             , L"Leitura de Encoder (E)"                       , L"Read Encoder (E)"                          , L"Lectura de Encoder (E)"                         );
		SET_LANGUAGE_STRINGS(cmdInstrResetEnc            , L"Escrita no Encoder (Shift + E)"               , L"Write Encoder (Shift + E)"                 , L"Escrita de Encoder (Shift + E)"                 );

		SET_LANGUAGE_STRINGS(cmdInstrReadUSS             , L"Leitura USS (Shift + U)"                      , L"Read USS (Shift + U)"                      , L"Lectura USS (Shift + U)"                        );
		SET_LANGUAGE_STRINGS(cmdInstrWriteUSS            , L"Escrita USS (U)"                              , L"Write USS (U)"                             , L"Escrita USS (U)"                                );
		SET_LANGUAGE_STRINGS(cmdInstrReadNS600           , L"Leitura de NS-600 Yaskawa (Shift + Y)"        , L"Read NS-600 Yaskawa (Shift + Y)"           , L"Lectura de NS-600 Yaskawa (Shift + Y)"          );
		SET_LANGUAGE_STRINGS(cmdInstrWriteNS600          , L"Escrita de NS-600 Yaskawa (Y)"                , L"Write NS-600 Yaskawa (Y)"                  , L"Escrita de NS-600 Yaskawa (Y)"                  );

		SET_LANGUAGE_STRINGS(cmdInstrReadModBUS          , L"Ler Registrador ModBUS (Shift + X)"           , L"Read ModBUS Register (Shift + X)"          , L"Leer Registrador ModBUS (Shift + X)"            );
		SET_LANGUAGE_STRINGS(cmdInstrWriteModBUS         , L"Escrever Registrador ModBUS (X)"              , L"Write ModBUS Register (X)"                 , L"Escribir Registrador ModBUS (X)"                );

		SET_LANGUAGE_STRINGS(cmdConfig                   , L"Projeto (F2)"                                 , L"Project (F2)"                              , L"Proyecto (F2)"                                  );

		SET_LANGUAGE_STRINGS(cmdSimulation               , L"Modo de Simula��o (F7)"                       , L"Simulation mode (F7)"                      , L"Modo de Simulacion (F7)"                        );
		SET_LANGUAGE_STRINGS(cmdSimulationExit           , L"Sair da Simula��o (ESC)"                      , L"Exit Simulation (ESC)"                     , L"Salir de Simulacion (ESC)"                      );
		SET_LANGUAGE_STRINGS(cmdSimulationStart          , L"Iniciar (I)"                                  , L"Start (I)"                                 , L"Iniciar (I)"                                    );
		SET_LANGUAGE_STRINGS(cmdSimulationPause          , L"Pausar (P)"                                   , L"Pause (P)"                                 , L"Pausar (P)"                                     );
		SET_LANGUAGE_STRINGS(cmdSimulationSingleCycle    , L"Simples Ciclo (Enter)"                        , L"Single Cycle (Return)"                     , L"Simples ciclo (Enter)"                          );

		SET_LANGUAGE_STRINGS(cmdCompile                  , L"Compilar (F5)"                                , L"Compile (F5)"                              , L"Compilar (F5)"                                  );
		SET_LANGUAGE_STRINGS(cmdCompileAndProgram        , L"Compilar e Gravar (F6)"                       , L"Compile and Program (F6)"                  , L"Compilar y Grabar (F6)"                         );

		SET_LANGUAGE_STRINGS(cmdDebug                    , L"Depurar (F4)"                                 , L"Debug (F4)"                                , L"Depurar (F4)"                                   );

		SET_LANGUAGE_STRINGS(cmdUserManual               , L"Manual (F1)"                                  , L"Manual (F1)"                               , L"Manual (F1)"                                    );

		// Comandos sem um atalho podem usar o label normal
		default: return getRibbonLocalizedLabel(nCmdID);
	}

	switch(POPSettings.idLanguage) {
		case  1: return pEN;
		case  2: return pES;
		default: return pPTBR;
	}
}

PCWSTR getRibbonLocalizedLabelDescription(UINT nCmdID)
{
	PCWSTR pPTBR = L"", pEN = L"", pES = L"";

	switch(nCmdID) {
			SET_LANGUAGE_STRINGS(cmdFileSaveAsLadder,
				L"Salvar o Diagrama Ladder atual como um novo arquivo.",
				L"Save as Ladder Diagram",
				L"Salvar como Diagrama Ladder");
			SET_LANGUAGE_STRINGS(cmdFileSaveAsC,
				L"Exportar o Diagrama Ladder atual como um novo arquivo em Linguagem C.",
				L"Export as C File",
				L"Exportar como archivo C");
			SET_LANGUAGE_STRINGS(cmdFileSaveAsText,
				L"Exportar o Diagrama Ladder atual como um novo arquivo de texto.",
				L"Export as Text File",
				L"Exportar como arquivo texto");
			SET_LANGUAGE_STRINGS(cmdInsertLineBefore,
				L"Insere uma linha vazia antes da linha atualmente selecionada.",
				L"Insert Before",
				L"Inserir antes");
			SET_LANGUAGE_STRINGS(cmdInsertLineAfter,
				L"Insere uma linha vazia ap�s a linha atualmente selecionada.",
				L"Insert After",
				L"Inserir despues");
			SET_LANGUAGE_STRINGS(cmdCopyLine,
				L"Copia a linha atualmente selecionada para a mem�ria.",
				L"Copy Line",
				L"Copiar Linea");
			SET_LANGUAGE_STRINGS(cmdPasteLineBefore,
				L"Colar a linha previamente copiada antes da linha atualmente selecionada.",
				L"Paste Before",
				L"Pegar antes");
			SET_LANGUAGE_STRINGS(cmdPasteLineAfter,
				L"Colar a linha previamente copiada ap�s a linha atualmente selecionada.",
				L"Paste After",
				L"Pegar despues");
			SET_LANGUAGE_STRINGS(cmdInstrCircuitOpen,
				L"Inserir circuito aberto na posi��o do cursor.",
				L"Open Circuit",
				L"Circuito abierto");
			SET_LANGUAGE_STRINGS(cmdInstrCircuitClosed,
				L"Inserir circuito fechado na posi��o do cursor.",
				L"Closed Circuit",
				L"Circuito abierto");
			SET_LANGUAGE_STRINGS(cmdInstrMasterRelayControl,
				L"Inserir chave geral na posi��o do cursor. Utilizado para desativar o barramento abaixo desta instru��o.",
				L"Master Relay Control",
				L"Llave general");
			SET_LANGUAGE_STRINGS(cmdInstrShiftRegister,
				L"Inserir fila de vari�veis na posi��o do cursor.",
				L"Shift Register",
				L"Fila de Variables");
			SET_LANGUAGE_STRINGS(cmdInstrLookUpTable,
				L"Inserir tabela de busca na posi��o do cursor.",
				L"Look-Up Table",
				L"Tabla de Busca");
			SET_LANGUAGE_STRINGS(cmdInstrPieceWiseLinear,
				L"Inserir lineariza��o por segmentos na posi��o do cursor.",
				L"Piecewise Linear",
				L"Linear por seguimientos");
			SET_LANGUAGE_STRINGS(cmdInstrRand,
				L"Inserir N�mero Rand�mico na posi��o do cursor.",
				L"Random Number",
				L"Numero Randonico");
			SET_LANGUAGE_STRINGS(cmdCompile,
				L"Gerar arquivo .hex para grava��o na POP-7 a patir do Diagrama Ladder.",
				L"Compile",
				L"Nuevo");
	}

	switch(POPSettings.idLanguage) {
		case  1: return pEN;
		case  2: return pES;
		default: return pPTBR;
	}
}

PCWSTR getRibbonLocalizedTooltipDescription(UINT nCmdID)
{
	PCWSTR pPTBR = L"", pEN = L"", pES = L"";

	switch(nCmdID) {
			SET_LANGUAGE_STRINGS(cmdFileNew                  ,
				L"Criar um novo Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdFileOpen                 ,
				L"Abrir um Diagrama Ladder existente.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdFileClose                ,
				L"Fechar o diagrama atual.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdFileSave                 ,
				L"Salvar o Diagrama Ladder atual.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdFileSaveAsText           ,
				L"Exportar Diagrama Ladder atual como um novo arquivo de texto.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdUndo                     ,
				L"Desfazer a �ltima a��o.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdRedo                     ,
				L"Refazer a �ltima a��o desfeita.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInsertLine               ,
				L"Obter outras formas para inserir uma linha",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdMoveLineUp               ,
				L"Mover a linha atualmente selecionada para cima",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdMoveLineDown             ,
				L"Mover a linha atualmente selecionada para baixo",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdCopyLine                 ,
				L"Copia a linha atualmente selecionada para a mem�ria.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdPasteLine                ,
				L"Obter outras formas para colar uma linha",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdPasteLineBefore          ,
				L"Colar a linha previamente copiada antes da linha atualmente selecionada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdPasteLineAfter           ,
				L"Colar a linha previamente copiada ap�s a linha atualmente selecionada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInsertLineAfter          ,
				L"Insere uma linha vazia ap�s a linha atualmente selecionada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInsertLineBefore         ,
				L"Insere uma linha vazia antes da linha atualmente selecionada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdGoHome                   ,
				L"Move o cursor para o in�cio do diagrama.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdGoEnd                    ,
				L"Move o cursor para o final do diagrama.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrParallel            ,
				L"Inserir in�cio de paralelo na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdCopyInstr                ,
				L"Copia o objeto selecionado para a mem�ria.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdPasteInstr               ,
				L"Insere na posi��o atual o objeto previamente copiado para a mem�ria.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdDeleteInstr              ,
				L"Exclui o comando atualmente selecionado",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdDeleteLine               ,
				L"Exclui a linha atualmente selecionada",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdFindDialog               ,
				L"Abrir a janela de busca.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdFind                     ,
				L"Procura pelo texto informado na caixa de busca a partir do objeto atualmente selecionado.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrComment             ,
				L"Inserir coment�rio na linha atual. Se a linha n�o estiver vazia, uma nova ser� criada.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrContact             ,
				L"Inserir um contato na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCoil                ,
				L"Inserir bobina na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrOneShotFalling      ,
				L"Inserir detec��o de borda de descida na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrOneShotRising       ,
				L"Inserir detec��o de borda de subida na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCircuitOpen         ,
				L"Inserir circuito aberto na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCircuitClosed       ,
				L"Inserir circuito fechado na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMasterRelayControl  ,
				L"Inserir chave geral na posi��o do cursor. Utilizado para desativar o barramento abaixo desta instru��o.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrTimerON             ,
				L"Inserir temporizador com atraso para ligar na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrTimerOFF            ,
				L"Inserir temporizador com atraso para desligar na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrTimerONRet          ,
				L"Abrir exemplo de instru��o atraso com mem�ria.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrSchedule            ,
				L"Inserir tarefa agendada na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrTimerReset          ,
				L"Inserir zerar atraso na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrCounterInc          ,
				L"Inserir contador incremental na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCounterDec          ,
				L"Inserir contador decremental na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCounterCirc         ,
				L"Inserir contador circular na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCounterReset        ,
				L"Inserir zerar contador na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrMathAdd             ,
				L"Inserir instru��o matem�tica somar na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathSub             ,
				L"Inserir instru��o matem�tica subtrair na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathMult            ,
				L"Inserir instru��o matem�tica multiplicar na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathDivide          ,
				L"Inserir instru��o matem�tica dividir na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathModulo          ,
				L"Inserir instru��o matem�tica resto de divis�o na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathSqrt            ,
				L"Inserir instru��o matem�tica raiz quadrada na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrShiftRegister       ,
				L"Inserir fila de vari�veis na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrLookUpTable         ,
				L"Inserir tabela de busca na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrPieceWiseLinear     ,
				L"Inserir lineariza��o por segmentos na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrRand                ,
				L"Inserir N�mero Rand�mico na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrAbs                 ,
				L"Inserir a instru��o de Valor Absoluto na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrMov                 ,
				L"Inserir mover valor na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrSetBit              ,
				L"Inserir liga/desliga bit na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCheckBit            ,
				L"Inserir checar bit na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrPersist             ,
				L"Inserir persistente na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrCondEqual           ,
				L"Inserir condicional igual na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondNotEqual        ,
				L"Inserir condicional diferente na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondGreater         ,
				L"Inserir condicional maior na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondGreaterEqual    ,
				L"Inserir condicional maior ou igual na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondLesser          ,
				L"Inserir condicional menor na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondLesserEqual     ,
				L"Inserir condicional menor ou igual na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrReadAD              ,
				L"Inserir leitura de A/D na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteDA             ,
				L"Inserir escrita no D/A na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrPWM                 ,
				L"Inserir sa�da PWM na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrRampDA              ,
				L"Inserir rampa de acelera��o/desacelera��o na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrReadEnc             ,
				L"Inserir leitura de encoder na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrResetEnc            ,
				L"Inserir escrita no encoder na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrPID                 ,
				L"Inserir Controle PID na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrWriteChar           ,
				L"Inserir escrita de caracter na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrReadChar            ,
				L"Inserir leitura de caracter na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrReadFormatString    ,
				L"Inserir ler string formatada na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteFormatString   ,
				L"Inserir escrever string formatada na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrReadUSS             ,
				L"Inserir leitura uss na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteUSS            ,
				L"Inserir escrita uss na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrReadNS600           ,
				L"Inserir leitura de NS-600 Yaskawa na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteNS600          ,
				L"Inserir escrita no NS-600 Yaskawa na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrReadModBUS          ,
				L"Inserir ler registrador modbus na posi��o do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteModBUS         ,
				L"Inserir escrever registrador modbus na posi��o do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdConfig                   ,
				L"Abrir a janela de configura��es do projeto atual.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSettings                 ,
				L"Abrir a janela para configura��o do programa POPTools como porta serial para grava��o, desativar avisos, etc.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdSimulation               ,
				L"Entrar no modo de simula��o do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationExit           ,
				L"Sair do modo de simula��o do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationStart          ,
				L"Iniciar a simula��o em tempo real do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationPause          ,
				L"Pausar a simula��o em tempo real do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationStop           ,
				L"Interromper a simula��o do Diagrama Ladder, retornando toda a l�gica ao estado inicial.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationSingleCycle    ,
				L"Executar um �nico ciclo.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationLogStart       ,
				L"Iniciar o registro de dados da Simula��o.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationLogStop        ,
				L"Parar o registro de dados da Simula��o.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdCompile                  ,
				L"Gerar arquivo .hex para grava��o na POP-7 a patir do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdCompileAndProgram        ,
				L"Gerar arquivo .hex e grav�-lo na POP-7 a partir do Diagrama Ladder.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdDebug                    ,
				L"Abrir janela de depura��o da POP-7, onde � poss�vel ler e alterar seus registradores.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdSplitUserManual          ,
				L"Abrir o Manual do Usu�rio, contendo a ajuda para todo o programa.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdUserManual          ,
				L"Abrir o Manual do Usu�rio, contendo a ajuda para todo o programa.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdKeyboardManual           ,
				L"Abrir o Manual do Teclado, contendo todos os atalhos para os comandos da interface.",
				L"",
				L"");

			// Example comands list
			SET_LANGUAGE_STRINGS(cmdExamples                 ,
				L"Abrir a lista de l�gicas de exemplo.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleComment           ,
				L"Abrir exemplo de coment�rio.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleContact           ,
				L"Abrir exemplo de instru��o contato.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCoil              ,
				L"Abrir exemplo de instru��o bobina.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleOneShotFalling    ,
				L"Abrir exemplo de instru��o detec��o de borda de descida.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleOneShotRising     ,
				L"Abrir exemplo de instru��o detec��o de borda de subida.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCircuitOpen       ,
				L"Abrir exemplo de instru��o circuito aberto.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCircuitClosed     ,
				L"Abrir exemplo de instru��o circuito fechado.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMasterRelayControl,
				L"Abrir exemplo de instru��o chave geral.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleTimerON           ,
				L"Abrir exemplo de instru��o temporizador com atraso para ligar.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleTimerOFF          ,
				L"Abrir exemplo de instru��o temporizador com atraso para desligar.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleTimerONRet        ,
				L"Abrir exemplo de instru��o atraso com mem�ria.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleSchedule          ,
				L"Abrir exemplo de instru��o tarefa agendada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleTimerReset        ,
				L"Abrir exemplo de instru��o zerar atraso.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleCounterInc        ,
				L"Abrir exemplo de instru��o contador incremental.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCounterDec        ,
				L"Abrir exemplo de instru��o contador decremental.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCounterCirc       ,
				L"Abrir exemplo de instru��o contador circular.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCounterReset      ,
				L"Abrir exemplo de instru��o zerar contador.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleMathAdd           ,
				L"Abrir exemplo de instru��o matem�tica somar.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathSub           ,
				L"Abrir exemplo de instru��o matem�tica subtrair.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathMult          ,
				L"Abrir exemplo de instru��o matem�tica multiplicar.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathDivide        ,
				L"Abrir exemplo de instru��o matem�tica dividir.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathModulo        ,
				L"Abrir exemplo de instru��o matem�tica resto de divis�o.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathSqrt          ,
				L"Abrir exemplo de instru��o matem�tica raiz quadrada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleShiftRegister     ,
				L"Abrir exemplo de instru��o fila de vari�veis.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleLookUpTable       ,
				L"Abrir exemplo de instru��o tabela de busca.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExamplePieceWiseLinear   ,
				L"Abrir exemplo de instru��o lineariza��o por segmentos.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleRand              ,
				L"Abrir exemplo de N�mero Rand�mico.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleAbs               ,
				L"Abrir exemplo de instru��o que calcula Valor Absoluto.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleMov               ,
				L"Abrir exemplo de instru��o mover valor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleSetBit            ,
				L"Abrir exemplo de instru��o liga/desliga bit.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCheckBit          ,
				L"Abrir exemplo de instru��o checar bit.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExamplePersist           ,
				L"Abrir exemplo de instru��o persistente.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleCondEqual         ,
				L"Abrir exemplo de instru��o condicional igual.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondNotEqual      ,
				L"Abrir exemplo de instru��o condicional diferente.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondGreater       ,
				L"Abrir exemplo de instru��o condicional maior.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondGreaterEqual  ,
				L"Abrir exemplo de instru��o condicional maior ou igual.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondLesser        ,
				L"Abrir exemplo de instru��o condicional menor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondLesserEqual   ,
				L"Abrir exemplo de instru��o condicional menor ou igual.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleReadAD            ,
				L"Abrir exemplo de instru��o leitura de A/D.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteDA           ,
				L"Abrir exemplo de instru��o escrita no D/A.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExamplePWM               ,
				L"Abrir exemplo de instru��o sa�da PWM.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleRampDA            ,
				L"Abrir exemplo de instru��o rampa de acelera��o/desacelera��o.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleReadEnc           ,
				L"Abrir exemplo de instru��o leitura de encoder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleResetEnc          ,
				L"Abrir exemplo de instru��o escrita no encoder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExamplePID               ,
				L"Abrir exemplo de instru��o Controle de PID.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleWriteChar         ,
				L"Abrir exemplo de instru��o escrita de caracter.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleReadChar          ,
				L"Abrir exemplo de instru��o leitura de caracter.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleReadFormatString  ,
				L"Abrir exemplo de instru��o ler string formatada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteFormatString ,
				L"Abrir exemplo de instru��o escrever string formatada.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleReadUSS           ,
				L"Abrir exemplo de instru��o leitura uss.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteUSS          ,
				L"Abrir exemplo de instru��o escrita uss.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleReadNS600         ,
				L"Abrir exemplo de instru��o leitura de NS-600 Yaskawa.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteNS600        ,
				L"Abrir exemplo de instru��o escrita no NS-600 Yaskawa.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleReadModBUS        ,
				L"Abrir exemplo de instru��o ler registrador modbus.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteModBUS       ,
				L"Abrir exemplo de instru��o escrever registrador modbus.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleAdcLed            ,
				L"Abrir exemplo de aplica��o - Barra de Leds.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleSemaphore         ,
				L"Abrir exemplo de aplica��o - Sem�foro.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleTester            ,
				L"Abrir exemplo de aplica��o - Testador.",
				L"",
				L"");
	}

	switch(POPSettings.idLanguage) {
		case  1: return pEN;
		case  2: return pES;
		default: return pPTBR;
	}
}

char *_(char *in)
{
    int i;

	if(l == nullptr) return in;

    for(i = 0; i < l->n; i++) {
        if(_stricmp(in, l->tab[i].from)==0) {
            return l->tab[i].to;
        }
    }

#ifdef _DEBUG
//	Error("No match found: '%s'", in);
#endif

	return in;
}
