#ifndef LADDEROBJECTS_H
#define LADDEROBJECTS_H

#include <deque>
#include <array>
#include <vector>
#include <string>

#include "intcode.h"
 
using namespace std;

/*** Estruturas auxiliares ***/
class mapIO;
class LadderElem;
class LadderCircuit;
class LadderDiagram;

/*** Estruturas para gravar / ler as configuracoes do ladder ***/

/// Configuracoes gerais do diagrama
typedef struct {
	bool			canSave;   ///< Flag que indica se o diagrama atual pode ser salvo
	int				cycleTime; ///< Tempo de ciclo do CLP em milissegundos
	int				mcuClock;  ///< Frequencia do processador do CLP
} LadderSettingsGeneral;

/// Configuracoes da Interface Serial do CLP
typedef struct {
	int				baudRate; ///< Velocidade em bps da interface RS-485
	int				UART;     ///< Variável contendo configuracao da RS-485: paridade, stop bits, etc.
} LadderSettingsUART;

/// Configuracoes de rede do CLP
typedef struct {
	unsigned long	ip;   ///< IP do CLP
	unsigned long	mask; ///< Mascara de rede
	unsigned long	gw;   ///< Gateway padrao
	unsigned long	dns;  ///< Servidor DNS
} LadderSettingsNetwork;

/// Configuracoes de Data/Hora do CLP
typedef struct {
	bool			sntp_enable; ///< Flag que indica se o sincronismo por SNTP esta ativado
	string			sntp_server; ///< Indica o servidor SNTP de onde devemos receber Data/Hora
	int				gmt;         ///< Fuso horario em que o CLP esta trabalhando
	bool			dailysave;   ///< Indica se o horari ode verao esta ativado
} LadderSettingsSNTP;

/// Configuracoes de Encoder Incremental
typedef struct {
	int				perimeter; ///< Perimetro da roda on o encoder esta fixado
	int				pulses;    ///< Numero de pulsos por volta do encoder (PPR)
	float			factor;    ///< Fator de correcao do encoder
	int				conv_mode; ///< Modo de conversao: leitura direta, mm, decimos de mm, etc.
	bool			x4;        ///< Flag indicando se o encoder dexe usar multiplicador 4 ou 2
} LadderSettingsEncoderIncremental;

/// Configuracoes de Encoder Absoluto (SSI)
typedef struct {
	int		size;      ///< Numero de bits total de resolucao do encoder
	int		mode;      ///< Modo de leitura: Gray ou Binario

	int		conv_mode; ///< Modo de conversao: leitura direta, mm, decimos de mm, etc.
	int		perimeter; ///< Perimetro da roda on o encoder esta fixado
	float	factor;    ///< Fator de correcao do encoder
	int		size_bpr;  ///< Numero de bits que representa uma volta do encoder
} LadderSettingsEncoderSSI;

/// Configuracoes da interface Analogica para Digital (A/D)
typedef struct {
	bool isCelsius; ///< Flag indicando que a leitura do canal de temperatura retorna Celsius ou Fahrenheit
} LadderSettingsADC;

/// Configuracoes da interface Digital para Analogica (D/A)
typedef struct {
	int ramp_abort_mode; ///< Modo de abandono da rampa: Manter o valor atual, retornar a zero, etc.
} LadderSettingsDAC;

/// Configuracoes do protocolo ModBUS
typedef struct {
	int			ModBUSID; ///< ID que o CLP ira responder quando atuando em modo Escravo
} LadderSettingsModbusSlave;

/// Informacoes gerais do diagrama
typedef struct {
	// Project Information
	string Name       ; ///< Nome do projeto
	string Developer  ; ///< Nome do desenvolvedor
	string Description; ///< Descricao do projeto
	string FWVersion  ; ///< Versao atual do firmware no POPTools
	long   BuildNumber; ///< Numero da compilacao do projeto
	time_t CompileDate; ///< Data/Hora da ultima compilacao
	time_t ProgramDate; ///< Dat/Hora da ultima gravacao do diagrama em um CLP
} LadderSettingsInformation;

typedef void (*tFncDrawChars)(int, int, const char *);

/*** Estruturas auxiliares relacionadas com o I/O ***/

/// Tipo do I/O
enum eType  {
	eType_Pending = 0,   ///< Pendente. Indica que nao tem tipo ou o I/O eh um numero, nao variavel.
	eType_Reserved,      ///< Reservado. Indica que o nome atual eh reservado para uso do sistema e deve ser substituido
	eType_General,       ///< Geral. Tipo generico, utilizado por I/Os nao associados a nenhuma logica especifica
	eType_DigInput,      ///< Entrada. Tipo de I/Os que sao associados a uma entrada digital do CLP
	eType_InternalRelay, ///< Rele interno. Utilizado para logicas internas de contatos e bobinas
	eType_DigOutput,     ///< Saida. Tipo de I/Os que sao associados a uma saida digital do CLP
	eType_InternalFlag,  ///< Flag interna. Sao I/Os associados a flags internas do firmware do CLP
	eType_Counter,       ///< Contador. Sao I/Os utilizados em contadores (Incrementais, Decrementais, Retentivos)
	eType_TOF,           ///< Atraso para Desligar. I/O utilizado em atraso para desligar, nao pode ser reutilizado
	eType_TON,           ///< Atraso para Ligar. I/O utilizado em atraso para ligar, nao pode ser reutilizado
	eType_RTO,           ///< Atraso para Ligar com memoria. I/O utilizado em atraso para ligar com memoria, nao pode ser reutilizado
	eType_ReadADC,       ///< Leitura de A/D. Tipo de I/Os que sao associados a um canal A/D do CLP
	eType_SetDAC,        ///< Escrita no D/A. Tipo de I/Os que sao associados ao canal D/A do CLP
	eType_ReadEnc,       ///< Leitura de Encoder. I/O associado a um canal de encoder do CLP, utilizado em operacoes de leitura
	eType_ResetEnc,      ///< Escrita no Encoder. I/O associado a um canal de encoder do CLP, utilizado em operacoes de escrita
	eType_ReadUSS,       ///< Leitura USS. I/O associado a um elemento de leitura atraves de protocolo USS (Siemens)
	eType_WriteUSS,      ///< Escrita USS. I/O associado a um elemento de escrita atraves de protocolo USS (Siemens)
	eType_ReadModbus,    ///< Leitura ModBUS. I/O associado a um elemento de leitura atraves de protocolo ModBUS
	eType_WriteModbus,   ///< Escrita ModBUS. I/O associado a um elemento de escrita atraves de protocolo ModBUS
	eType_PWM,           ///< PWM. Tipo de I/Os que sao associados a uma saida digital do CLP e cuja saida opera como PWM
	eType_RxUART,        ///< Leitura UART. I/O associado a um elemento de leitura de string da interface serial (RS-485)
	eType_TxUART,        ///< Escrita UART. I/O associado a um elemento de escrita de string da interface serial (RS-485)
	eType_ReadYaskawa,   ///< Leitura NS-600. I/O associado a um elemento de leitura de equipamento NS-600 (Yaskawa)
	eType_WriteYaskawa,  ///< Escrita NS-600. I/O associado a um elemento de escrita de equipamento NS-600 (Yaskawa)
	eType_Last           ///< Ultimo elemento. Utilizado como um marcador para elementos que definem novos I/Os, nunca associado a um I/O
};

/// Estrutura que define as caracteristias e dados de um I/O
typedef struct {
	// Numero de solicitacoes de I/O para cada tipo de dado
	unsigned int countRequestBit; ///< Numero de elementos que requisitaram acesso de tipo bit a este I/O
	unsigned int countRequestInt; ///< Numero de elementos que requisitaram acesso de tipo int a este I/O

	// Numero de solicitacoes de I/O para cada tipo de acesso
	unsigned int countRequestRead;  ///< Numero de elementos que requisitaram acesso de tipo leitura a este I/O
	unsigned int countRequestWrite; ///< Numero de elementos que requisitaram acesso de tipo escrita a este I/O

	// Flags indicando restricao de uso do I/O
	bool         isUniqueRead;  ///< Flag indicando que somente permite o uso para leitura por 1 I/O
	bool         isUniqueWrite; ///< Flag indicando que somente permite o uso para escrita por 1 I/O

	eType        type; ///< Tipo do I/O
	unsigned int pin;  ///< Pino do CLP ao qual este I/O esta associado
	unsigned int bit;  ///< Bit do CLP ao qual este I/O esta associado, como o bit do registrador ModBUS
} mapDetails;

/// Enumeracao com o tipo de solicitacao do I/O: Leitura, Escrita ou Ambos
enum eRequestAccessType {
	eRequestAccessType_Read = 0, ///< Acesso somente leitura
	eRequestAccessType_Write,    ///< Acesso somente escrita
	eRequestAccessType_ReadWrite ///< Acesso leitura e escrita
};

/// Enumeracao para informar o tipo de ordenacao que deve ser realizada na lista de I/O
enum eSortBy {
	eSortBy_Nothing = 0, ///< Nenhum. Sem uma ordenacao especifica
	eSortBy_Name,        ///< Nome. Ordena a lista de I/Os por nome do I/O
	eSortBy_Type,        ///< Tipo. Ordena a lista de I/Os pelo tipo do I/O, em ordem alfabetica conforme a descricao do tipo
	eSortBy_Pin,         ///< Pino. Ordena a lista de I/Os pelo numero do pino do I/O
	eSortBy_Port         ///< Nome. Ordena a lista de I/Os pela porta associada ao I/O, que seria a descricao do pino do I/O
};

/// Enumeracao que indica o resultado da validacao do diagrama
enum eValidateResult {
	eValidateResult_OK = 0,  ///< OK. Indica que nao ocorreram erros durante a validacao do diagrama
	eValidateResult_Warning, ///< Aviso. Indica que ocorreram apenas avisos (variavel nao utilizada, por exemplo)
	eValidateResult_Error    ///< Erro. Indica que houve erro durante a validacao. Ou seja: o diagrama nao pode ser compilado ou simulado
};

/// Enumeracao que indica o modo de validacao do I/O.
/// Exemplo: Para a simulacao, nao precisa checar por associacoes
enum eValidateIO {
	eValidateIO_OnlyNames, ///< Somente valida os nomes dos I/Os, nao sua associacao
	eValidateIO_Full       ///< Validacao completa, checando inclusive se um I/O nao foi associado a um pino do CLP
};

/// Enumeracao com os modos de Search & Replace
enum eSearchAndReplaceMode {
	eSearchAndReplaceMode_FindFirst,    ///< Inicia a busca do inicio do diagrama
	eSearchAndReplaceMode_FindNext,     ///< Inicia a busca a partir do objeto atualmente selecionado
	eSearchAndReplaceMode_ReplaceFirst, ///< Inicia a substituicao do inicio do diagrama
	eSearchAndReplaceMode_ReplaceNext,  ///< Inicia a substituicao a partir do objeto atualmente selecionado
	eSearchAndReplaceMode_ReplaceAll    ///< substitui de uma so vez todos os elementos encontrados ao inves de apenas o primeiro
};

/// Enumeracao que define os modos de movimentacao do cursor
enum eMoveCursor  {
	eMoveCursor_Any = 0,     ///< Movimenta o cursor em qualquer direcao, indicando que devemos localizar o elemento mais proximo
	eMoveCursor_DiagramHome, ///< Movimenta o cursor para o inicio do diagrama, ou seja, o primeiro elemento da primeira linha
	eMoveCursor_RungHome,    ///< Movimenta o cursor para o inicio da linha atual
	eMoveCursor_DiagramEnd,  ///< Movimenta o cursor para o final do diagrama, ou seja, o ultimo elemento da ultima linha
	eMoveCursor_RungEnd,     ///< Movimenta o cursor para o final da linha atual
	eMoveCursor_Up,          ///< Movimenta o cursor para cima. Altera a selecao do objeto atual para a parte superior e, caso ja selecionado, busca o objeto mais proximo.
	eMoveCursor_Down,        ///< Movimenta o cursor para baixo. Altera a selecao do objeto atual para a parte inferior e, caso ja selecionado, busca o objeto mais proximo.
	eMoveCursor_Left,        ///< Movimenta o cursor para a esquerda. Altera a selecao do objeto atual para a lateral esquerda e, caso ja selecionado, busca o objeto mais proximo.
	eMoveCursor_Right,       ///< Movimenta o cursor para a direita. Altera a selecao do objeto atual para a lateral direita e, caso ja selecionado, busca o objeto mais proximo.
};

/// Enumeracao que define o retorno das caixas de dialogo
enum eReply {
	eReply_Pending = 0, ///< Este estado indica que a caixa de dialogo ainda esta aguardando uma resposta do usuario
	eReply_No,          ///< O usuario respondeu Nao
	eReply_Yes,         ///< O usuario respondeu Sim
	eReply_Ok,          ///< O usuario respondeu Ok
	eReply_Cancel,      ///< O usuario respondeu Cancelar
};

/// Enumeracao com os tipos de caixa de dialogo existentes
enum eDialogType {
	eDialogType_Message = 0, ///< Caixa de mensagem. Exibe o botao Ok e a imagem de informacao
	eDialogType_Error,       ///< Caixa de Erro. Exibe o botao Ok e a imagem de erro
	eDialogType_Question     ///< Caixa de Pergunta. Existe os botoes Sim e Nao, alem da imagem de interrogacao
};

/// Estrutura auxiliar que representa um subcircuito, ou seja, um elemento do circuito atual
typedef struct {
	LadderElem    *elem;   ///< Ponteiro para o elemento. Se nulo, indica que aqui temos um subcircuito (Serie ou Paralelo)
	LadderCircuit *subckt; ///< Ponteiro para o subcircuito (Serie ou Paralelo) ou, se elemento for valido, o circuito atual
} Subckt;

/// Definicao indicando que o elemento procurado ainda nao foi encontrado
#define SUBCKT_STATUS_NOTFOUND 0
/// Definicao indicando que o elemento procurado esta no primeiro elemento do circuito atual
#define SUBCKT_STATUS_FIRST    1
/// Definicao indicando que o elemento procurado esta dentro do circuito atual, nao em suas extremidades
#define SUBCKT_STATUS_INSIDE   2
/// Definicao indicando que o elemento procurado esta no ultimo elemento do circuito atual
#define SUBCKT_STATUS_LAST     3

/// Estrutura que representa um ponto de insercao para o paralelo.
typedef struct {
	unsigned int   point;    ///< Indice para o ponto de insercao dentro do circuito atual
	Subckt         subckt;   ///< Subcircuito que representa o elemento de referencia
	LadderCircuit *series;   ///< O circuito serie a que pertence o elemento de referencia
	LadderCircuit *parallel; ///< O circuito paralelo a que pertence o elemento de referencia
} InsertionPoint;

/// Estrutura que representa o contexto do diagrama, ou seja, seu estado atual
typedef struct {
	LadderElem    *SelectedElem;    ///< Ponteiro para o elemento atualmente selecionado
	LadderElem    *ParallelStart;   ///< Ponteiro que, se nao nulo, indica o ponto inicial ao adicionar um elemento em paralelo com o circuito atual
	LadderCircuit *SelectedCircuit; ///< Indica o circuito atualmente selecionado. Representa o circuito a que pertence o elemento selecionado
	LadderDiagram *Diagram;         ///< Ponteiro para o diagrama a que pertence o contexto

	int SelectedState;              ///< Variavel que armazena o estado de selecao atual: cima, baixo, esquerda, direita...

	bool inSimulationMode;          ///< Flag que indica se estamos no modo de simulacao
	bool isLoadingFile;             ///< Flag que indica se um arquivo esta sendo carregado

	bool   programChangedNotSaved;  ///< Flag que indica se houve alteracao no diagrama desde o ultimo salvamento

	bool canNegate;                 ///< Indica se o elemento atual pode ser alterado para o modo negado
	bool canNormal;                 ///< Indica se o elemento atual pode ser alterado para o modo normal
	bool canSetOnly;                ///< Indica se o elemento atual pode ser alterado para o modo "Apenas Ligar"
	bool canResetOnly;              ///< Indica se o elemento atual pode ser alterado para o modo "Apenas Desligar"
	bool canPushUp;                 ///< Indica se a linha atual pode ser movida para cima
	bool canPushDown;               ///< Indica se a linha atual pode ser movida para baixo
	bool canDelete;                 ///< Indica se o elemento atual pode ser excluido
	bool canInsertEnd;              ///< Indica se pode ser adicionado um elemento de final de linha
	bool canInsertOther;            ///< Indica se pode ser adicionado um elemento de meio de linha
	bool canInsertComment;          ///< Indica se um comentario pode ser adicionado
} LadderContext;

/// Estrutura auxiliar para solicitar um I/O.
typedef struct {
	pair<unsigned long, int> pin;           ///< Par que representa um pino: ID ou, se ID for zero, seu valor numerico
	string                   name;          ///< Nome do I/O
	bool                     isBit;         ///< Flag indicando se o modo de acesso ao I/O sera por bit ou int
	eType                    type;          ///< Tipo do I/O
	eRequestAccessType       access;        ///< Tipo de acesso como leitura, escrita, etc.
	bool                     isUniqueRead;  ///< Indica se deve existir apenas 1 acesso para leitura deste I/O
	bool                     isUniqueWrite; ///< Indica se deve existir apenas 1 acesso para escrita deste I/O
} tRequestIO;

/// Estrutura auxiliar que representa uma acao de desfazer / refazer
typedef struct {
	/// Texto que descreve a acao de forma amigavel para o usuario
	string         Description;

	// Objeto responsavel por executar a acao. Se todos os ponteiros forem nulos, indica acao de desfazer do diagrama
	mapIO         *io;     ///< Quando nao nulo, indica acao de desfazer do mapa de I/O
	LadderElem    *elem;   ///< Quando nao nulo, indica acao de desfazer do elemento
	LadderCircuit *subckt; ///< Quando nao nulo, indica acao de desfazer do circuito

	// Contexto Antes e Depois de executar a acao
	LadderContext  contextAfter;  ///< Contexto do diagrama depois de executar esta acao
	LadderContext  contextBefore; ///< Contexto do diagrama antes de executar esta acao

	// Codigo da acao e ponteiro para dados especificos da acao
	unsigned int   action; ///< Valor que representa o tipo de acao cadastrada
	void          *data;   ///< Ponteiro para o dado especifico da acao, conforme fornecido por quem a registrou
} UndoRedoAction;

/** Funcao auxiliar que retorna o tipo de I/O conforme o tipo do timer indicado
 *  @param[in] which Codigo que indica o tipo de elemento (TON, OFF, RTO)
 *  @return    Tipo de I/O para o timer indicado
 */
eType getTimerTypeIO(int which);

/** Funcao auxiliar que entrega um contexto "vazio"
	@return Contexto inicializado
*/
LadderContext getEmptyContext(void);

/*** Estruturas auxiliares relacionada a lista de nodes do ModBUS ***/

/// Enumeracao que indica o tipo de no do ModBUS
enum eMbTypeNode {
	eMbTypeNode_RS485 = 0, ///< Este node comunica utilizando a interface RS-485
	eMbTypeNode_Ethernet   ///< Este node comunica utilizando a interface Ethernet
};

/// Estrutura que representa um no do ModBUS
typedef struct {
	string        name;  ///< Nome do node do ModBUS
	int           id;    ///< ID do node, utilizado quando comunicando por RS-485
	unsigned long ip;    ///< IP do node, utilizado quando comunicando por Ethernet
	eMbTypeNode   iface; ///< Interface utilizada pelo node
} LadderMbNode;

/// Estrutura que define uma linha do ladder.
/// Basicamente um LadderCircuit com algumas informacoes adicionais
struct LadderRung {
	LadderCircuit *rung;          ///< Ponteiro para o circuito que representa a linha
	bool           hasBreakpoint; ///< Flag que indica se existe um breakpoint para esta linha
	bool           isPowered;     ///< Flag que indica se a linha esta energizada
};

/// Estrutura que armazena informacoes sobre objetos copiados (Linhas / Elementos)
typedef struct {
	// Informacoes de elemento copiado
	LadderElem    *elemCopy;  ///< Ponteiro para o elemento copiado
	LadderDiagram *elemOwner; ///< Ponteiro para o diagrama de origem do elemento copiado

	// Informacoes de linha copiada
	LadderRung    *rungCopy;  ///< Ponteiro para a linha copiada
	LadderDiagram *rungOwner; ///< Ponteiro para o diagrama de origem da linha copiada
} LadderClipboard;

/** @brief Funcao que desaloca um elemento da memoria
 *
 * Esta funcao auxiliar converte um ponteiro da classe base (LadderElem) para uma classe
 *  derivada (LadderElemCoil, LadderElemContact, etc) e entao o desaloca da memoria
 *  @param[in] elem Ponteiro para o elemento que deve ser desalocado
 */
void UnallocElem(LadderElem *elem);

/*** Classes representando os elementos do Ladder ***/

/// Classe base de elementos - Abstrata, todos os elementos derivam dessa classe base
class LadderElem {
private:
	bool isEndOfLine; ///< Indica se este eh um elemento de final de linha
	bool isComment;   ///< Indica se este elemento eh do tipo comentario
	bool isFormatted; ///< Indica se a saida da funcao DrawTXT deste elemento eh um texto formatado
	bool isUART;      ///< Indica que este elemento usa a UART, devendo ser exibido o console serial na simulacao
	int  which;       ///< Codigo que identifica o tipo de elemento atual

	/** Funcao executada ao atualizar as propriedades de um elemento
	 *  @param[in] data       Ponteiro para as propriedades, conforme o tipo do elemento
	 *  @param[in] isUndoRedo Flag que indica se esta funcao foi chamada durante uma operacao de desfazer/refazer
	 *  @return               Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalSetProperties(void *data, bool isUndoRedo) = 0;

	/** Funcao que gera o codigo intermediario, implementada pelo elemento para gerar seu codigo especifico
	 *  @param[in,out] ic Referencia ao vetor contendo o codigo intermediario, onde as novas instrucoes serao agregadas
	 *  @return           Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalGenerateIntCode(IntCode &ic) = 0;

	/** Funcao que salva os dados do elemento em disco
	 *  @param[in] f Ponteiro para o handler do arquivo sendo salvo
	 *  @return      Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalSave(FILE *f) = 0;
	/** Funcao que carrega os dados do elemento do disco
	 *  @param[in] f       Ponteiro para o handler do arquivo sendo carregado
	 *  @param[in] version Variavel indicando a versao do arquivo sendo carregado, possibilitando conversoes de formato
	 *  @return            Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalLoad(FILE *f, unsigned int version) = 0;

	/** Funcao que inicializa as variaveis privadas
	 */
	void Init(void);

	/** Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	 *  @param[in] index Indice para indicar qual o I/O que deve ser atualizado
	 *  @param[in] name  Nome para o qual o I/O deve ser atualizado
	 *  @param[in] type  Tipo para o qual o I/O deve ser atualizado
	 *  @return          Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) = 0;

	/** Funcao que executa uma acao de desfazer / refazer nas classes derivadas
	 *  @param[in]     IsUndo    Indica se a acao se refere a operacao de Desfazer (true) ou Refazer (false)
	 *  @param[in]     isDiscard Indica se devemos descartar a acao (true) ou executa-la (false)
	 *  @param[in,out] action    Variavel contendo os dados para Desfazer/Refazer a operacao
	 *  @return                  Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action) = 0;

protected:
	/// Flag que indica se a saida do elemento esta energizada na simulacao
	bool poweredAfter;

	/// Ponteiro para o diagrama ao qual este elemento esta associado
	LadderDiagram *Diagram;

	/// Estrutura de dados para acoes de Desfazer / Refazer
	union UndoRedoData {
		/// Estrutura com dados para registro de acao desfazer/refazer para alteracao de propriedades do elemento
		struct {
			void *data; ///< Ponteiro para as propriedades originais
		} SetProp;
	};

	/// Enumeracao com os tipos de eventos de Undo/Redo suportados pela classe base (LadderElem)
	enum UndoRedoActionsEnum {
		eCheckpoint = 0, ///< Marcador de Checkpoint
		eSetProp,        ///< Acao de alteracao de propriedades do elemento
		eActionsEnd      ///< Marcador de fim das acoes da classe base, permitindo que suas derivadas criem suas proprias acoes
	};

public:
	/*** Construtores ***/

	/** Construtor geral, apenas inicializa com valores padrao
	 */
	LadderElem(void);
	/** Construtor que recebe parametros para inicializacao das variaveis conforme o elemento sendo criado
	 *  @param[in] EOL       Utilizado para inicializar a flag que indica que este elemento eh de final de linha
	 *  @param[in] Comment   Utilizado para inicializar a flag que indica que este elemento eh um comentario
	 *  @param[in] Formatted Utilizado para inicializar a flag que indica que este elemento gera texto formatado
	 *  @param[in] UART      Utilizado para inicializar a flag que indica que este elemento eh usa a interface serial
	 *  @param[in] elemWhich Utilizado para inicializar a variavel com o tipo do elemento. Nao pode ser alterado posteriormente
	 */
	LadderElem(bool EOL, bool Comment, bool Formatted, bool UART, int elemWhich);

	/// Destrutor padrao, nada a fazer...
	~LadderElem(void) { }

	/** Funcao que gera o texto utilizado para impressao e exportacao em modo texto
	 *  @return Par de strings (Linha superior e linha inferior) que representam o elemento em modo texto
	 */
	virtual pair<string, string> DrawTXT(void) = 0;
	/** Funcao que desenha o elemento na tela
	 *  @param[in] poweredBefore Indica se a entrada do elemento esta energizada em uma simulacao
	 *  @param[in] data          Variavel que contem informacoes para desenhar o objeto na tela, especifica da interface
	 *  @return                  Estado da saida do elemento na simulacao: energizada (true) ou nao (false)
	 */
	virtual bool DrawGUI(bool poweredBefore, void *data) = 0;

	/** Funcao que exibe a janela de dialogo para configuracao do elemento
	 *  @param[in] context Contexto do Diagrama com informacoes sobre o que pode ser feito, objeto selecionado, etc.
	 *  @return            Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool ShowDialog(LadderContext context) = 0;

	/** Funcao que gera o codigo intermediario do elemento, realizando as acoes gerais para todos os elementos
	 *  @param[in,out] ic Referencia ao vetor contendo o codigo intermediario, onde as novas instrucoes serao agregadas
	 *  @return           Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool GenerateIntCode(IntCode &ic);

	/** Funcao que informa se este elemento eh um comentario
	 *  @return Booleano indicando se este elemento eh um comentario (true) ou nao (false)
	 */
	inline bool           IsComment     (void)                      { return isComment;    }
	/** Funcao que informa se este elemento eh do tipo final de linha
	 *  @return Booleano indicando se este elemento eh do tipo final de linha (true) ou nao (false)
	 */
	inline bool           IsEOL         (void)                      { return isEndOfLine;  }
	/** Funcao que informa o estado da saida deste elemento
	 *  @return Booleano indicando se a saida deste elemento esta energizada (true) ou nao (false)
	 */
	inline bool           IsPoweredAfter(void)                      { return poweredAfter; }
	/** Funcao que informa se este elemento gera texto formatado
	 *  @return Booleano indicando se este elemento gera texto formatado (true) ou nao (false)
	 */
	inline bool           IsFormatted   (void)                      { return isFormatted;  }
	/** Funcao que informa se este elemento utiliza a interface serial
	 *  @return Booleano indicando se este elemento utiliza a interface serial (true) ou nao (false)
	 */
	inline bool           IsUART        (void)                      { return isUART;       }

	/** Funcao que informa se este elemento eh de fim de linha e esta ativado na simulacao
	 *  @return Booleano indicando se este elemento eh de fim de linha e esta ativado na simulacao (true) ou nao (false)
	 */
	inline bool           IsActiveEOL   (void)                      { return isEndOfLine && poweredAfter; }

	/** Funcao que retorna o ID do elemento
	 *  @return Codigo que indica o tipo do elemento (LadderElemContact, LadderElemCoil, etc)
	 */
	inline int            getWhich      (void)                      { return which;        }
	/** Funcao que retorna um ponteiro para o diagrama
	 *  @return Ponteiro para o diagrama ao qual este elemento pertence
	 */
	inline LadderDiagram *getDiagram    (void)                      { return Diagram;      }
	/** Funcao que associa um elemento a um diagrama
	 *  @param[in] newDiagram Ponteiro para o diagrama para o qual este elemento deve ser associado
	 */
	inline void           setDiagram    (LadderDiagram *newDiagram) { Diagram = newDiagram; }

	/** Funcao que gera o codigo intermediario do elemento, realizando as acoes gerais para todos os elementos
	 *  @param[in] context Contexto com informacoes do estado atual do diagrama
	 *  @return            Indica se a insercao deste elemento pode ser realizada para este contexto (true) ou nao (false)
	 */
	virtual bool CanInsert(LadderContext context) = 0;

	/** Funcao executada apos o elemento ser inserido no diagrama
	 */
	virtual void doPostInsert(void) = 0;
	/** Funcao executada apos o elemento ser removido do diagrama
	 */
	virtual void doPostRemove(void) = 0;

	/** Funcao que indica o espaco em texto ocupado pelo elemento
	 *  @return Espaco ocupado por este elemento na saida em modo texto
	 */
	virtual inline int getWidthTXT(void) = 0;

	/** Funcao executada ao atualizar as propriedades de um elemento, executando as acoes gerais de todos os elementos
	 *  @param[in] context  Contexto com informacoes do estado atual do diagrama
	 *  @param[in] propData Ponteiro para as propriedades, conforme o tipo do elemento
	 */
	void          setProperties(LadderContext context, void *propData);
	/** Funcao que retorna as propriedades do elemento
	 *  @return          Ponteiro para as propriedades especificas deste elemento
	 */
	virtual void *getProperties(void) = 0;

	/** Funcao que cria uma copia do elemento
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @return            Ponteiro para o elemento criado
	 */
	virtual LadderElem *Clone(LadderDiagram *diagram) = 0;

	/*** Funcoes relacionadas com I/O ***/

	/** Funcao que indica se o I/O pode ser alterado para o tipo solicitado
	 *  @param[in] id   ID do I/O sendo alterado
	 *  @param[in] type Tipo para o qual o I/O sera alterado
	 *  @return         Indica se a mudanca de tipo do I/O eh permitida (true) ou nao (false)
	 */
	virtual bool  acceptIO        (unsigned long id, eType type) = 0;
	/** Funcao que informa o elemento que ele deve atualizar seus I/Os
	 *  @param[in] owner Ponteiro para o diagrama ao qual o I/O pertencia originalmente
	 *  @param[in] isDiscard Informa se nesta operacao os I/Os devem ser descartados (true) ou atualizados (false)
	 */
	virtual void  updateIO        (LadderDiagram *owner, bool isDiscard) = 0;
	/** Funcao que retorna o tipo permitido para este I/O pelo elemento atual
	 *  @param[in] id ID do I/O sendo solicitado
	 *  @return       Tipo de I/O permitido pelo elemento atual
	 */
	virtual eType getAllowedTypeIO(unsigned long id) = 0;
	/** Funcao que realiza uma busca / substituicao de nome de I/O entre os seus I/Os
	 *  @param[in] idSearch  ID do I/O sendo buscado / substituido
	 *  @param[in] sNewText  string contendo o novo nome para o I/O
	 *  @param[in] isReplace Flag que indica se esta operacao eh de substituicao (true) ou busca (false)
	 *  @return              Numero de correspondencias encontradas no elemento atual
	 */
	virtual int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) = 0;

	/** Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	 *  @param[in] index Indice para indicar qual o I/O que deve ser atualizado
	 *  @param[in] name  Nome para o qual o I/O deve ser atualizado
	 *  @param[in] type  Tipo para o qual o I/O deve ser atualizado
	 *  @return          Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool updateNameTypeIO(unsigned int index, string name, eType type);

	/*** Funcoes para ler / gravar elementos para o disco ***/
	/** Funcao que salva os dados do elemento em disco, executando as acoes gerais de todos os elementos
	 *  @param[in] f Ponteiro para o handler do arquivo sendo salvo
	 *  @return      Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool Save(FILE *f);
	/** Funcao que carrega os dados do elemento do disco, executando as acoes gerais de todos os elementos
	 *  @param[in] f       Ponteiro para o handler do arquivo sendo carregado
	 *  @param[in] version Variavel indicando a versao do arquivo sendo carregado, possibilitando conversoes de formato
	 *  @return            Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool Load(FILE *f, unsigned int version);

	/** Funcao que executa uma acao de desfazer / refazer, executando as acoes gerais de todos os elementos
	 *  @param[in]     IsUndo    Indice se a acao se refere a operacao de Desfazer (true) ou Refazer (false)
	 *  @param[in]     isDiscard Indica se devemos descartar a acao (true) ou executa-la (false)
	 *  @param[in,out] action    Variavel contendo os dados para Desfazer/Refazer a operacao
	 *  @return                  Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Classe derivada de LadderElem que representa o elemento PlaceHolder
class LadderElemPlaceHolder : public LadderElem {
	// Sem propriedades privadas...
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemPlaceHolder(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	void *getProperties(void) { return nullptr; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type) { return true; }
	void  updateIO        (LadderDiagram *owner, bool isDiscard) { }
	eType getAllowedTypeIO(unsigned long id) { return eType_Pending; }
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) { return 0; }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemComment
struct LadderElemCommentProp {
	string str; ///< String que armazena o texto do comentario
};

/// Classe derivada de LadderElem que representa o elemento Comment
class LadderElemComment : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemCommentProp prop;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemComment(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void);

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type) { return true; }
	void  updateIO        (LadderDiagram *owner, bool isDiscard) { }
	eType getAllowedTypeIO(unsigned long id) { return eType_Pending; }
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) { return 0; }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemContact
struct LadderElemContactProp {
    bool                     negated; ///< Flag que indica que o contato esta negado, ou seja, NF
	pair<unsigned long, int> idName;  ///< Referencia do I/O associado ao contato
};

/// Classe derivada de LadderElem que representa o elemento Contato
class LadderElemContact : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemContactProp prop;
	/// Variavel com os dados para fazer Request do I/O do contato
	tRequestIO            infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemContact(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemCoil
struct LadderElemCoilProp {
    bool                     negated;   ///< Flag que indica se a bobina esta no modo negado (/)
    bool                     setOnly;   ///< Flag que indica se a bobina esta no modo Somente Ligar (S)
    bool                     resetOnly; ///< Flag que indica se a bobina esta no modo Somente Desligar (R)
	pair<unsigned long, int> idName;    ///< Referencia do I/O associado a bobina
};

/// Classe derivada de LadderElem que representa o elemento Bobina
class LadderElemCoil : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemCoilProp prop;
	/// Variavel com os dados para fazer Request do I/O da bobina
	tRequestIO         infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemCoil(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemTimer
struct LadderElemTimerProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao timer
    int                      delay;  ///< Tempo de espera para ativacao do timer
};

/// Classe derivada de LadderElem que representa o elemento Timer
class LadderElemTimer : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemTimerProp prop;
	/// Variavel com os dados para fazer Request do I/O do Timer
	tRequestIO          infoIO_Name;

	int TimerPeriod(void);

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: TON, TOF, RTO
	 */
	LadderElemTimer(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemRTC
struct LadderElemRTCProp {
	int           mode;  ///< Modo do RTC. Continuo ou Intermitente (quando usando faixa de data/hora) ou data/hora fixa
    unsigned char wday;  ///< Mascara com dias da semana. De bit 0 (Domingo) ate 6 (Sabado). Bit 7 indica se esta usando dias da semana (ligado) ou Data/Hora (desligado)
	struct tm     start; ///< Data/Hora inicial quando usando faixa ou Data/Hora Fixa.
	struct tm     end;   ///< Data/Hora final quando usando faixa de Data/Hora
};

/// Classe derivada de LadderElem que representa o elemento RTC
class LadderElemRTC : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemRTCProp prop;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemRTC(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type) { return true; }
	void  updateIO        (LadderDiagram *owner, bool isDiscard) { }
	eType getAllowedTypeIO(unsigned long id) { return eType_Pending; }
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) { return 0; }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemCounter
struct LadderElemCounterProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao contador
	pair<unsigned long, int> idMax;  ///< Referencia do I/O associado ao limite do contador
};

/// Classe derivada de LadderElem que representa o elemento Counter
class LadderElemCounter : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemCounterProp prop;
	/// Variavel com os dados para fazer Request do I/O do Counter
	tRequestIO            infoIO_Name;
	/// Variavel com os dados para fazer Request do I/O do Limite do Counter
	tRequestIO            infoIO_Max;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: CTC, CTD, CTU
	 */
	LadderElemCounter(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemReset
struct LadderElemResetProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao elemento
};

/// Classe derivada de LadderElem que representa o elemento Reset Timer / Counter
class LadderElemReset : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemResetProp prop;
	/// Variavel com os dados para fazer Request do I/O do Reset
	tRequestIO          infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemReset(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Classe derivada de LadderElem que representa o elemento One Shot
class LadderElemOneShot : public LadderElem {
	// Sem propriedades privadas...
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: OSR, OSF
	 */
	LadderElemOneShot(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void) { return nullptr; }

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type) { return true; }
	void  updateIO        (LadderDiagram *owner, bool isDiscard) { }
	eType getAllowedTypeIO(unsigned long id) { return eType_Pending; }
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) { return 0; }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemCmp
struct LadderElemCmpProp {
	pair<unsigned long, int> idOp1; ///< Referencia do I/O associado ao primeiro operador
	pair<unsigned long, int> idOp2; ///< Referencia do I/O associado ao segundo operador
};

/// Classe derivada de LadderElem que representa o elemento Cmp
class LadderElemCmp : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemCmpProp prop;
	/// Variavel com os dados para fazer Request do I/O do Reset (primeiro operador)
	tRequestIO        infoIO_Op1;
	/// Variavel com os dados para fazer Request do I/O do Reset (segundo operador)
	tRequestIO        infoIO_Op2;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: EQU, GRT, GEQ, etc.
	 */
	LadderElemCmp(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemMath
struct LadderElemMathProp {
	pair<unsigned long, int> idOp1;  ///< Referencia do I/O associado ao primeiro operador
	pair<unsigned long, int> idOp2;  ///< Referencia do I/O associado ao segundo operador
	pair<unsigned long, int> idDest; ///< Referencia do I/O associado ao destino
};

/// Classe derivada de LadderElem que representa o elemento Math
class LadderElemMath : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemMathProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento (primeiro operador)
	tRequestIO         infoIO_Op1;
	/// Variavel com os dados para fazer Request do I/O do elemento (segundo operador)
	tRequestIO         infoIO_Op2;
	/// Variavel com os dados para fazer Request do I/O do elemento (destino)
	tRequestIO         infoIO_Dest;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: ADD, MUL, DIV, etc.
	 */
	LadderElemMath(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 2; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemSqrt
struct LadderElemSqrtProp {
	pair<unsigned long, int> idDest; ///< Referencia do I/O associado ao destino
	pair<unsigned long, int> idSrc;  ///< Referencia do I/O associado a origem
};

/// Classe derivada de LadderElem que representa o elemento Sqrt
class LadderElemSqrt : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemSqrtProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento (destino)
	tRequestIO         infoIO_Dest;
	/// Variavel com os dados para fazer Request do I/O do elemento (origem)
	tRequestIO         infoIO_Src;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemSqrt(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemRand
struct LadderElemRandProp {
	pair<unsigned long, int> idVar; ///< Referencia do I/O associado ao destino
	pair<unsigned long, int> idMin; ///< Referencia do I/O associado ao valor minimo
	pair<unsigned long, int> idMax; ///< Referencia do I/O associado ao valor maximo
};

/// Classe derivada de LadderElem que representa o elemento Rand
class LadderElemRand : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemRandProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento (destino)
	tRequestIO         infoIO_Var;
	/// Variavel com os dados para fazer Request do I/O do elemento (valor minimo)
	tRequestIO         infoIO_Min;
	/// Variavel com os dados para fazer Request do I/O do elemento (valor maximo)
	tRequestIO         infoIO_Max;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemRand(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 2; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemAbs
struct LadderElemAbsProp {
	pair<unsigned long, int> idDest; ///< Referencia do I/O associado ao destino
	pair<unsigned long, int> idSrc;  ///< Referencia do I/O associado a origem
};

/// Classe derivada de LadderElem que representa o elemento Abs
class LadderElemAbs : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemAbsProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento (destino)
	tRequestIO        infoIO_Dest;
	/// Variavel com os dados para fazer Request do I/O do elemento (origem)
	tRequestIO        infoIO_Src;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemAbs(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemMove
struct LadderElemMoveProp {
	pair<unsigned long, int> idDest; ///< Referencia do I/O associado ao destino
	pair<unsigned long, int> idSrc;  ///< Referencia do I/O associado a origem
};

/// Classe derivada de LadderElem que representa o elemento Move
class LadderElemMove : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemMoveProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento (destino)
	tRequestIO         infoIO_Dest;
	/// Variavel com os dados para fazer Request do I/O do elemento (origem)
	tRequestIO         infoIO_Src;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemMove(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Classe derivada de LadderElem que representa o elemento Open & Short
class LadderElemOpenShort : public LadderElem {
	// Sem propriedades privadas...
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: OPEN, SHORT
	 */
	LadderElemOpenShort(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void) { return nullptr; }

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type) { return true; }
	void  updateIO        (LadderDiagram *owner, bool isDiscard) { }
	eType getAllowedTypeIO(unsigned long id) { return eType_Pending; }
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) { return 0; }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemSetBit
struct LadderElemSetBitProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao elemento
	int    bit;                      ///< Numero do bit a ser manipulado
	bool   set;                      ///< Indica se o bit deve ser ligado (true) ou desligado (false)
};

/// Classe derivada de LadderElem que representa o elemento Set Bit
class LadderElemSetBit : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemSetBitProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO           infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemSetBit(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemCheckBit
struct LadderElemCheckBitProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao elemento
	int    bit;                      ///< Numero do bit a ser verificado
	bool   set;                      ///< Indica se devemos verificar se o bit esta ligado (true) ou desligado (false)
};

/// Classe derivada de LadderElem que representa o elemento Check Bit
class LadderElemCheckBit : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemCheckBitProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO             infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemCheckBit(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemReadAdc
struct LadderElemReadAdcProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao elemento
};

/// Classe derivada de LadderElem que representa o elemento Read A/D
class LadderElemReadAdc : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemReadAdcProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO            infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que retorna o nome do A/D conforme atribuicao do pino
	string GetNameADC(void);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemReadAdc(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemSetDa
struct LadderElemSetDaProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao elemento
	int                      mode;   ///< Modo de conversao do valor escrito: Sem conversao, em mV ou %
};

/// Classe derivada de LadderElem que representa o elemento Set D/A
class LadderElemSetDa : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemSetDaProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO          infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemSetDa(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	/** Verifica se o valor passado eh valido para o tipo de conversao indicado
	 *  @param[in] new_name Novo nome ou o valor numerico a validar
	 *  @param[in] new_mode Novo modo de conversao que sera utilizado para a validacao
	 *  @return             Indica se o valor eh valido (true) ou invalido (false)
	 */
	bool isValidDaValue(string new_name, int new_mode);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemReadEnc
struct LadderElemReadEncProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao elemento
};

/// Classe derivada de LadderElem que representa o elemento Read Encoder
class LadderElemReadEnc : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemReadEncProp prop;
	tRequestIO            infoIO_Name;

	// Estruturas de request das variaveis internas

	/// Variavel com os dados para fazer Request da variavel interna que armazena o perimetro da roda do encoder incremental
	tRequestIO             infoIO_IncPerimRoda;

	/// Variavel com os dados para fazer Request da variavel interna que armazena o numero de pulsos por volta (PPR) do encoder incremental
	tRequestIO             infoIO_IncPulsosVolta;

	/// Variavel com os dados para fazer Request da variavel interna que armazena o fator de correcao do encoder incremental
	tRequestIO             infoIO_IncFatorCorr;

	/// Variavel com os dados para fazer Request da variavel interna que armazena o perimetro da roda do encoder absoluto
	tRequestIO             infoIO_AbsPerimRoda;

	/// Variavel com os dados para fazer Request da variavel interna que armazena o fator de correcao do encoder absoluto
	tRequestIO             infoIO_AbsFatorCorr;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemReadEnc(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemResetEnc
struct LadderElemResetEncProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao elemento
};

/// Classe derivada de LadderElem que representa o elemento Reset Encoder
class LadderElemResetEnc : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemResetEncProp prop;
	tRequestIO             infoIO_Name;

	// Estruturas de request das variaveis internas

	/// Variavel com os dados para fazer Request da variavel interna que armazena o perimetro da roda do encoder incremental
	tRequestIO             infoIO_IncPerimRoda;

	/// Variavel com os dados para fazer Request da variavel interna que armazena o numero de pulsos por volta (PPR) do encoder incremental
	tRequestIO             infoIO_IncPulsosVolta;

	/// Variavel com os dados para fazer Request da variavel interna que armazena o fator de correcao do encoder incremental
	tRequestIO             infoIO_IncFatorCorr;

	/// Variavel com os dados para fazer Request da variavel interna que armazena o perimetro da roda do encoder absoluto
	tRequestIO             infoIO_AbsPerimRoda;

	/// Variavel com os dados para fazer Request da variavel interna que armazena o fator de correcao do encoder absoluto
	tRequestIO             infoIO_AbsFatorCorr;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemResetEnc(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemMultisetDA
struct LadderElemMultisetDAProp {
	pair<unsigned long, int>	idTime;					///< Referencia do I/O associado ao elemento: Tempo
	pair<unsigned long, int>	idDesl;					///< Referencia do I/O associado ao elemento: Deslocamento
	bool						linear;					///< Indica se esta eh uma rampa linear (true) ou Curva Ascendente/Descendente (false).
	bool						forward;				///< Indica se a rampa Avanca (true) ou Recua (false).
	bool						speedup;				///< Indica se a rampa eh Aceleracao (true) ou Desaceleracao (false).
	int							initval;				///< Valor inicial do DA na rampa
	int							type;					///< Modo de conversao. 0 = Valor saida do DA (2048 ~ -2048), 1 = milivolt (mV) (10V ~-10V), 2 = percentual (%)
	int							gaint;					///< Tempo da curva de ganho em %
	int							gainr;					///< Resolução da curva de ganho em %
	bool						StartFromCurrentValue;	///< Indica se deve iniciar ou ir para zero conforme speedup (false) ou partir do valor atual até o valor configurado (true).
	int							ramp_abort_mode;        ///< Modo de abandono da rampa ao desativar o elemento durante a execucao
};

/// Classe derivada de LadderElem que representa o elemento Multiset D/A
class LadderElemMultisetDA : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemMultisetDAProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento: Tempo
	tRequestIO               infoIO_Time;
	/// Variavel com os dados para fazer Request do I/O do elemento: Deslocamento
	tRequestIO               infoIO_Desl;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemMultisetDA(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemUSS
struct LadderElemUSSProp {
	pair<unsigned long, int> idName;        ///< Referencia do I/O associado ao elemento
	int		                 id;            ///< ID do equipamento para o qual esta mensagem se destina
	int                      parameter;     ///< Numero do parametro que iremos trabalhar
	int                      parameter_set; ///< Jogo de parametros a usar
	int                      index;         ///< Indice do array com a posicao do elemento para parametros em array
};

/// Classe derivada de LadderElem que representa o elemento Read / Write USS
class LadderElemUSS : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemUSSProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO        infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: READ_USS, WRITE_USS
	 */
	LadderElemUSS(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemModBUS
struct LadderElemModBUSProp {
	pair<unsigned long, int> idName;       ///< Referencia do I/O associado ao elemento
	int                      elem;         ///< ID de referencia ao no do ModBUS associado a este elemento
	int                      address;      ///< Numero do registrador a ser acessado
	bool                     int32;        ///< Indica se esta operacao trabalhara com um registrador de 32 bits (true) ou 16 bits (false).
	bool                     retransmitir; ///< Se true, o pacote sera reenviado em caso de falha na transmissao
};

/// Classe derivada de LadderElem que representa o elemento ModBUS
class LadderElemModBUS : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemModBUSProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO           infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: READ_MODBUS, WRITE_MODBUS
	 */
	LadderElemModBUS(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void);
	void doPostRemove(void);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemSetPWM
struct LadderElemSetPWMProp {
	pair<unsigned long, int> idName;     ///< Referencia do I/O associado ao elemento
    int                      targetFreq; ///< Frequencia de trabalho do PWM
};

/// Classe derivada de LadderElem que representa o elemento Set PWM
class LadderElemSetPWM : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemSetPWMProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO           infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemSetPWM(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	void *getProperties(void);

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemUART
struct LadderElemUARTProp {
	pair<unsigned long, int> idName; ///< Referencia do I/O associado ao elemento
};

/// Classe derivada de LadderElem que representa o elemento Send / Receive UART
class LadderElemUART : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemUARTProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO         infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: UART_RECV, UART_SEND
	 */
	LadderElemUART(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Classe derivada de LadderElem que representa o elemento Master Relay
class LadderElemMasterRelay : public LadderElem {
	/// Flag que indica se a chave esta ligada (true) ou nao (false)
	bool isMasterRelayActive;

	// Sem propriedades privadas...
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemMasterRelay(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void) { return nullptr; }

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type) { return true; }
	void  updateIO        (LadderDiagram *owner, bool isDiscard) { }
	eType getAllowedTypeIO(unsigned long id) { return eType_Pending; }
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) { return 0; }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemShiftRegister
struct LadderElemShiftRegisterProp {
	vector< pair<unsigned long, int> > vectorIdRegs; ///< Vetor que armazena as referencias aos I/Os que representam cada posicao da fila de registradores
	string                             nameReg;      ///< Nome base do registrador da fila de variaveis. Ex.: Um registrador de nome base Teste ira gerar os registradores Teste0, Teste1, etc...
    int                                stages;       ///< Numero de registradores que compoem a fila de variaveis
};

/// Classe derivada de LadderElem que representa o elemento Shift Register
class LadderElemShiftRegister : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemShiftRegisterProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO                  InfoIO_Regs;

	bool internalSetProperties(void *data, bool isUndoRedo, bool isCloning);
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return internalSetProperties(data, isUndoRedo, false); }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	/** Funcao que cria os registradores necessarios utilizando o nome base conforme o numero de elementos da fila de variaveis
	 */
	void createRegs(void);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemShiftRegister(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemLUT
struct LadderElemLUTProp {
	pair<unsigned long, int>           idDest;       ///< Referencia do I/O associado ao elemento: Destino
	pair<unsigned long, int>           idIndex;      ///< Referencia do I/O associado ao elemento: Indice
    int                                count;        ///< Numero de itens na tabela
    bool                               editAsString; ///< Se true, a janela de configuracao exibira os valores como string ao inves de campos numericos independentes
    array<long, MAX_LOOK_UP_TABLE_LEN> vals;
};

/// Classe derivada de LadderElem que representa o elemento Look Up Table
class LadderElemLUT : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemLUTProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento: Destino
	tRequestIO        infoIO_Dest;
	/// Variavel com os dados para fazer Request do I/O do elemento: Indice
	tRequestIO        infoIO_Index;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemLUT(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemPiecewise
struct LadderElemPiecewiseProp {
	pair<unsigned long, int>           idDest;  ///< Referencia do I/O associado ao elemento: Destino
	pair<unsigned long, int>           idIndex; ///< Referencia do I/O associado ao elemento: Indice
    int                                count;   ///< Numero de itens na tabela
    array<long, MAX_LOOK_UP_TABLE_LEN> vals;
};

/// Classe derivada de LadderElem que representa o elemento Piecewise Linear
class LadderElemPiecewise : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemPiecewiseProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento: Destino
	tRequestIO              infoIO_Dest;
	/// Variavel com os dados para fazer Request do I/O do elemento: Indice
	tRequestIO              infoIO_Index;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemPiecewise(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemFmtString
struct LadderElemFmtStringProp {
	pair<unsigned long, int> idVar; ///< Referencia do I/O associado ao elemento: Variavel
    string                   txt;   ///< Texto contendo o formato que sera utilizado para criar o texto a ser enviado
};

/// Classe derivada de LadderElem que representa o elemento Formatted String
class LadderElemFmtString : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemFmtStringProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO              infoIO_Var;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: READ_FORMATTED_STRING, WRITE_FORMATTED_STRING
	 */
	LadderElemFmtString(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 2; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemYaskawa
struct LadderElemYaskawaProp {
	pair<unsigned long, int> idVar; ///< Referencia do I/O associado ao elemento: Variavel
    int                      id;    ///< ID do equipamento para o qual esta mensagem se destina
    string                   txt;   ///< Texto contendo o formato que sera utilizado para criar o texto a ser enviado
};

/// Classe derivada de LadderElem que representa o elemento Read / Write Yaskawa
class LadderElemYaskawa : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemYaskawaProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO            infoIO_Var;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @param[in] which   Codigo do elemento sendo criado: READ_SERVO_YASKAWA, WRITE_SERVO_YASKAWA
	 */
	LadderElemYaskawa(LadderDiagram *diagram, int which);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 2; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemPersist
struct LadderElemPersistProp {
	pair<unsigned long, int> idVar; ///< Referencia do I/O associado ao elemento
};

/// Classe derivada de LadderElem que representa o elemento Persist
class LadderElemPersist : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemPersistProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO            infoIO_Var;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemPersist(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemPID
struct LadderElemPIDProp {
	pair<unsigned long, int> idSetpoint;     ///< Referencia do I/O associado ao elemento: SetPoint
	pair<unsigned long, int> idProcessValue; ///< Referencia do I/O associado ao elemento: Process Value
	pair<unsigned long, int> idOutput;       ///< Referencia do I/O associado ao elemento: Output
	pair<unsigned long, int> idGainP;        ///< Referencia do I/O associado ao elemento: Ganho P
	pair<unsigned long, int> idGainI;        ///< Referencia do I/O associado ao elemento: Ganho I
	pair<unsigned long, int> idGainD;        ///< Referencia do I/O associado ao elemento: Ganho D
};

/// Classe derivada de LadderElem que representa o elemento PID
class LadderElemPID : public LadderElem {
private:
	/// Propriedades do elemento
	LadderElemPIDProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento: SetPoint
	tRequestIO        infoIO_Setpoint;
	/// Variavel com os dados para fazer Request do I/O do elemento: Process Value
	tRequestIO        infoIO_ProcessValue;
	/// Variavel com os dados para fazer Request do I/O do elemento: Output
	tRequestIO        infoIO_Output;
	/// Variavel com os dados para fazer Request do I/O do elemento: Ganho P
	tRequestIO        infoIO_GainP;
	/// Variavel com os dados para fazer Request do I/O do elemento: Ganho I
	tRequestIO        infoIO_GainI;
	/// Variavel com os dados para fazer Request do I/O do elemento: Ganho D
	tRequestIO        infoIO_GainD;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemPID(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type);
	void  updateIO        (LadderDiagram *owner, bool isDiscard);
	eType getAllowedTypeIO(unsigned long id);
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace);

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/// Estrutura que define as propriedades de um elemento LadderElemX
struct LadderElemXProp {
};

/// Classe derivada de LadderElem que representa o elemento X.
/// Esta classe serve apenas como referencia para criar novos objetos, nao sendo um elemento utilizavel
class LadderElemX : public LadderElem {
private:
	// Sem propriedades privadas...
	/// Propriedades do elemento
	LadderElemXProp prop;
	/// Variavel com os dados para fazer Request do I/O do elemento
	tRequestIO      infoIO_;

	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
	/** Construtor
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 */
	LadderElemX(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void) { return nullptr; }

	bool CanInsert(LadderContext context);

	void doPostInsert(void) { }
	void doPostRemove(void) { }

	inline int getWidthTXT(void) { return 1; }

	LadderElem *Clone(LadderDiagram *diagram);

	// Funcao que indica se pode alterar o I/O para o tipo especificado
	bool  acceptIO        (unsigned long id, eType type) { return true; }
	void  updateIO        (LadderDiagram *owner, bool isDiscard) { }
	eType getAllowedTypeIO(unsigned long id) { return eType_Pending; }
	int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) { return 0; }

	// Funcao que executa uma acao de desfazer / refazer
	bool internalDoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/** Classe que representa os circuitos (Serie / Paralelo) que compoem o Diagrama Ladder
 */
class LadderCircuit {
private:
	/// Indica se este circuito eh do tipo serie (true) ou paralelo (false)
	bool isSeries;

	/// Vetor que contem os elementos que compoem este circuito, podendo ser um elemento diretamente ou outro subcircuito
	vector<Subckt> vectorSubckt;

	/** Funcao que desenha um elemento em modo texto, usado para imprimir ou exportar em .txt
	 *  @param[in]    DrawChars     Ponteiro para a funcao que desenha os caracteres no buffer, fornecida pelo usuario
	 *  @param[inout] DisplayMatrix Matriz que representa o diagrama, indicando se naquela coordenada [x,y] existe algum elemento, apenas uma linha ou se eh um espaco vazio
	 *  @param[in]    elem          Ponteiro para o elemento que sera desenhado
	 *  @param[inout] cx            Ponteiro para variavel contendo a coordenada atual no eixo X, atualiza para refletir o deslocamento ocorrido ao desenhar o elemento atual
	 *  @param[inout] cy            Ponteiro para variavel contendo a coordenada atual no eixo Y, atualiza para refletir o deslocamento ocorrido ao desenhar o elemento atual
	 *  @param[in]    poweredBefore Indica se a entrada do elemento esta energizada (true) ou nao (false).
	 *  @return                     Indica se a saida do elemento esta energizada (true) ou nao (false).
	 */
	bool DrawElementTXT(tFncDrawChars DrawChars, vector< vector<int> > &DisplayMatrix, LadderElem *elem, int *cx, int *cy, bool poweredBefore);
	/** Funcao que desenha uma linha vertical em modo texto
	 *  @param[in] DrawChars Ponteiro para a funcao que desenha os caracteres no buffer, fornecida pelo usuario
	 *  @param[in] cx        Ponteiro para variavel contendo a coordenada atual no eixo X, atualiza para refletir o deslocamento ocorrido ao desenhar o elemento atual
	 *  @param[in] cy        Ponteiro para variavel contendo a coordenada atual no eixo Y, atualiza para refletir o deslocamento ocorrido ao desenhar o elemento atual
	 */
	void VerticalWireTXT(tFncDrawChars DrawChars, int cx, int cy);

	/// Enumeracao com os tipos de eventos de Undo/Redo suportados por esta classe
	enum UndoRedoActionsEnum {
		eCheckpoint = 0, ///< Marcador de Checkpoint
		eInsertSubckt,   ///< Acao de insercao de subcircuito no circuito atual
		eDeleteSubckt,   ///< Acao de remocao de subcircuito no circuito atual 
		eMoveSubckt      ///< Acao de mover subcircuito do / para o circuito atual
	};

	/// Estrutura de dados para acoes de Desfazer / Refazer
	union UndoRedoData {
		/// Estrutura com dados para registro de acao desfazer/refazer para inclusao de subcircuito
		struct {
			unsigned int pos;    ///< Indice da posicao onde o subcircuito foi inserido
			Subckt       subckt; ///< Subcircuito que foi adicionado ao circuito
		} InsertSubckt;
		/// Estrutura com dados para registro de acao desfazer/refazer para remocao de subcircuito
		struct {
			unsigned int pos;    ///< Indice da posicao de onde o subcircuito foi removido
			Subckt       subckt; ///< Subcircuito que foi removido do circuito
		} DeleteSubckt;
		/// Estrutura com dados para registro de acao desfazer/refazer para mover subcircuito
		struct {
			unsigned int   pos;     ///< Indice da posicao onde o subcircuito foi inserido
			unsigned int   fromPos; ///< Indice da posicao do subcircuito no circuito de origem
			LadderCircuit *circuit; ///< Ponteiro para o circuito de origem do subcircuito movido
		} MoveSubckt;
	};

protected:
	/** Funcao que insere um subcircuito em uma determinada posicao do circuito atual
	 *  @param[in] context    Contexto indicando a situacao atual do diagrama ladder
	 *  @param[in] pos        Indice da posicao onde o subcircuito deve ser inserido
	 *  @param[in] s          Subcircuito que deve ser inserido
	 *  @param[in] isMove     Flag que indica se a operacao atual eh uma inclusao normal (false) ou se faz parte de um comando Move (true)
	 *  @param[in] isUndoRedo Flag que informa se a operacao atual esta sendo executada normalmente (false) ou se trata-se de uma operacao de desfazer / refazer (true)
	 *  @return               Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool InsertSubckt(LadderContext context, unsigned int pos, Subckt s, bool isMove = false, bool isUndoRedo = false);
	/** Funcao que remove um subcircuito de uma determinada posicao do circuito atual
	 *  @param[in] context    Contexto indicando a situacao atual do diagrama ladder
	 *  @param[in] pos        Indice da posicao de onde o subcircuito deve ser removido
	 *  @param[in] isMove     Flag que indica se a operacao atual eh uma remocao normal (false) ou se faz parte de um comando Move (true)
	 *  @param[in] isUndoRedo Flag que informa se a operacao atual esta sendo executada normalmente (false) ou se trata-se de uma operacao de desfazer / refazer (true)
	 *  @return               Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool DeleteSubckt(LadderContext context, unsigned int pos, bool isMove = false,	bool isUndoRedo = false);
	/** Funcao que insere um subcircuito em uma determinada posicao do circuito atual
	 *  @param[in] context     Contexto indicando a situacao atual do diagrama ladder
	 *  @param[in] pos         Indice da posicao no circuito atual para onde o subcircuito deve ser movido
	 *  @param[in] fromCircuit Circuito de origem do subcircuito que deve ser movido
	 *  @param[in] fromPos     Indice da posicao do subcircuito no circuito de origem
	 *  @param[in] isUndoRedo Flag que informa se a operacao atual esta sendo executada normalmente (false) ou se trata-se de uma operacao de desfazer / refazer (true)
	 *  @return               Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool MoveSubckt  (LadderContext context, unsigned int pos, LadderCircuit *fromCircuit,
		unsigned int fromPos, bool isUndoRedo = false);

public:
	/** Construtor geral
	 */
	LadderCircuit(void);
	/** Construtor
	 *  @param[in] newSeries Flag que indica se o circuito sendo criado eh do tipo serie (true) ou paralelo (false).
	 */
	LadderCircuit(bool newSeries);

	/** Destrutor
	 */
	~LadderCircuit(void);

	/** Funcao que desenha o circuito em modo texto, usado para imprimir ou exportar em .txt
	 *  @param[inout] DrawChars     Ponteiro para funcao que desenha os caracteres, dependente de implementacao do usuario conforme sua necessidade
	 *  @param[inout] DisplayMatrix Matriz que representa o diagrama, indicando se naquela coordenada [x,y] existe algum elemento, apenas uma linha ou se eh um espaco vazio
	 *  @param[inout] cx            Ponteiro para variavel contendo a coordenada atual no eixo X, atualiza para refletir o deslocamento ocorrido ao desenhar o elemento atual
	 *  @param[inout] cy            Ponteiro para variavel contendo a coordenada atual no eixo Y, atualiza para refletir o deslocamento ocorrido ao desenhar o elemento atual
	 *  @param[in]    poweredBefore Indica se a entrada do elemento esta energizada (true) ou nao (false).
	 *  @param[in]    ColsAvailable Tamanho de colunas disponiveis ate o fim do diagrama, permitindo com que seja completado o desenho do circuito atual ate o final da linha
	 *                              ao inves de deixar os elementos sem ligacao na extremidade direita
	 *  @return                     Indica se a saida do circuito esta energizada (true) ou nao (false).
	 */
	bool DrawTXT(tFncDrawChars DrawChars, vector< vector<int> > &DisplayMatrix, int *cx, int *cy, bool poweredBefore, int ColsAvailable);
	/** Funcao que desenha o circuito em modo grafico
	 *  @param[in] poweredBefore Indica se a entrada do elemento esta energizada (true) ou nao (false).
	 *  @param[in] data          Ponteiro para os dados relevantes para desenhar graficamente o circuito, dependente da implementacao da interface grafica
	 *  @return                  Indica se a saida do circuito esta energizada (true) ou nao (false).
	 */
	bool DrawGUI(bool poweredBefore, void *data);

	/** Funcao que indica se o circuito representa um comentario
	 *  @return Indica se o circuito representa um comentario (true) ou um circuito normal (false).
	 */
	bool IsComment(void);
	/** Funcao que indica se o circuito esta vazio, ou seja, sem elementos ou apenas o placeholder inicial
	 *  @return Indica se o circuito esta vazio (true) ou nao (false).
	 */
	bool IsEmpty(void);
	/** Funcao que busca pelo elemento passado como parametro, indicando se ele eh o ultimo elemento do circuito
	 *  @param[in] elem Ponteiro para o elemento sendo analisado
	 *  @return         Indica se o elemento eh o ultimo do circuito (true) ou nao (false).
	 */
	bool IsLast(LadderElem *elem);
	/** Funcao que indica o tipo do circuito (serie ou paralelo)
	 *  @return Indica se o circuito eh serie (true) ou paralelo (false).
	 */
	bool IsSeries(void) { return isSeries; }
	/** Funcao que indica se o circuito possui um elemento de final de linha e, opcionalmente, se esta com a saida ativa na simulacao
	 *  @param[in] checkActive Indica se devemos verificar se o elemento esta com a saida ativa na simulacao (true) ou nao (false)
	 *  @return Indica se o circuito possui um elemento de final de linha (true) ou nao (false).
	 */
	bool HasEOL(bool checkActive = false);
	/** Funcao que indica se o circuito possui um elemento de final de linha com a saida ativa na simulacao
	 *  @return Indica se o o circuito possui um elemento de final de linha com a saida ativa na simulacao (true) ou nao (false).
	 */
	bool HasActiveEOL(void) { return HasEOL(true); }
	/** Funcao que indica se o circuito possui um elemento que utilize a interface serial
	 *  @return Indica se algum elemento do circuito utiliza a interface serial (true) ou nao (false).
	 */
	bool UartFunctionUsed(void);

	/** Funcao que gera o codigo intermediario de todo o circuito
	 *  @param[in,out] ic Referencia ao vetor contendo o codigo intermediario, onde as novas instrucoes serao agregadas
	 *  @return           Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool GenerateIntCode(IntCode &ic);

	/** Funcao que retorna o numero de elementos do circuito
	 *  @return Numero de elementos existentes no circuito
	 */
	unsigned int   getSize            (void) { return vectorSubckt.size(); }

	/** Funcao que retorna a posicao do subcircuito passado como parametro no circuito atual
	 *  @return Indice do subcircuito ou o tamanho do vetor em caso de nao ser encontrado (mesmo que getSize())
	 */
	unsigned int   getSubcktIndex     (Subckt s);

	/** Funcao que retorna o proximo elemento do circuito (ou algum de seus subcircuitos) tendo como referencia um subcircuito.
	 *  @param[in] next Subcircuito de referencia para a busca do subcircuito seguinte
	 *  @return         Proximo subcircuito de acordo com a referencia passada como parametro. Se ambos elemento e circuito do subcircuito
	 *                  retornado forem nulos, indica que o subcircuito de referencia nao foi encontrado ou nao existia um proximo elemento
	 *                  para a referencia fornecida.
	 */
	Subckt         getNext            (Subckt         next);
	/** Funcao que retorna o elemento anterior do circuito (ou algum de seus subcircuitos) tendo como referencia um subcircuito.
	 *  @param[in] previous Subcircuito de referencia para a busca do subcircuito anterior
	 *  @return             Subcircuito anterior de acordo com a referencia passada como parametro. Se ambos elemento e circuito do subcircuito
	 *                      retornado forem nulos, indica que o subcircuito de referencia nao foi encontrado ou nao existia um elemento anterior
	 *                      para a referencia fornecida.
	 */
	Subckt         getPrevious        (Subckt         previous);
	/** Funcao que retorna o ponteiro para o circuito ao qual o elemento pertence
	 *  @param[in] elem Elemento sendo buscado
	 *  @return         Ponteiro para o circuito onde se encontra o elemento passado como parametro ou nulo se nao for encontrado
	 */
	LadderCircuit *getSubcktForElement(LadderElem    *elem);
	/** Funcao que retorna o ponteiro para o circuito pai do circuito passado como referencia
	 *  @param[in] subckt Subcircuito sendo buscado
	 *  @return           Ponteiro para o circuito pai do circuito passado como parametro ou nulo se nao for encontrado
	 */
	LadderCircuit *getParentSubckt    (LadderCircuit *subckt);
	/** Funcao que retorna o subcircuito na posicao indicada
	 *  @param[in] pos Indice da posicao do subcircuito sendo solicitado
	 *  @return        Subcircuito da posicao solicitada. Se nao encontrado, tanto circuito como elemento do subcircuito retornado serao nulos
	 */
	Subckt         getSubckt          (unsigned int   pos);

	/** Funcao que retorna o primeiro elemento (nao subcircuito) do circuito atual.
	 *  Se o primeiro elemento for um subcircuito, busca o primeiro elemento desse subcircuito
	 *  @return Ponteiro para o primeiro elemento do circuito ou nulo se nao existente
	 */
	LadderElem *getFirstElement(void);

	/** Funcao que retorna o ultimo elemento (nao subcircuito) do circuito atual.
	 *  Se o ultimo elemento for um subcircuito, busca o ultimo elemento desse subcircuito
	 *  @return Ponteiro para o ultimo elemento do circuito ou nulo se nao existente
	 */
	LadderElem *getLastElement(void);

	/** Funcao que retorna a largura do circuito em colunas para o modo texto.
	 *  @param[in] ColsAvailable Colunas disponiveis ate o final do diagrama
	 *  @return                  Numero de colunas ocupadas pelo circuito
	 */
	int            getWidthTXT (int ColsAvailable);
	/** Funcao que retorna a altura do circuito em numero de elementos para o modo texto.
	 *  @return Altura do circuito em numero de elementos. Por exemplo: um circuito paralelo com 3 elementos retornara 3. Um circuito serie com 3 elementos retornara 1.
	 */
	int            getHeightTXT(void);

	/** Funcao que associa os elementos e subcircuitos deste circuito para um novo diagrama
	 *  @param[in] newDiagram Ponteiro para o novo diagrama ao qual os elementos deste circuito devem ser associados.
	 */
	void           setDiagram  (LadderDiagram *newDiagram);

	/** Funcao que adiciona um elemento de placeholder ao final do circuito se nao houver elemento de final de linha e o ultimo elemento for um subcircuito do tipo paralelo.
	 *  @param[in] context contexto contendo a situacao atual do diagrama
	 *  @param[in] elem    ponteiro para o elemento sendo adicionado no caso de estarmos adicionando o placeholder de inicio de paralelo
	 */
	void AddPlaceHolderIfNoEOL(LadderContext context, LadderElem *elem);
	/** Funcao que remove um placeholder de final de linha se ele nao for mais necessario (como quando estamos adicionando um novo placeholder ao final do circuito apos alguma modificacao
	 *  @param[in] context Contexto indicando a situacao atual do diagrama ladder
	 *  @param[in] elem    Ponteiro para o elemento sendo adicionado no caso de estarmos adicionando o placeholder de inicio de paralelo, para que ele nao seja removido indevidamente
	 */
	void DeleteEndPlaceHolder (LadderContext context, LadderElem *elem);
	/** Funcao que adiciona um elemento ao circuito
	 *  @param[in]    elem    Ponteiro para o elemento a ser adicionado ao circuito
	 *  @param[inout] context Contexto indicando a situacao atual do diagrama ladder, sendo alterado para refletir as alteracoes realizadas
	 *  @return               Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool AddElement(LadderElem *elem, LadderContext &context);
	/** Funcao que adiciona um elemento em paralelo a elementos/subcircuitos deste circuito
	 *  @param[in]    elem    Ponteiro para o elemento a ser adicionado ao circuito
	 *  @param[in]    start   Indice da posicao inicial da insercao
	 *  @param[in]    end     Indice da posicao final da insercao
	 *  @param[inout] context Contexto indicando a situacao atual do diagrama ladder, sendo alterado para refletir as alteracoes realizadas
	 *  @return               Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool InsertParallel(LadderElem *elem, unsigned int start, unsigned int end, LadderContext &context);
	/** Funcao que remove um elemento do circuito
	 *  @param[in]    elem    Ponteiro para o elemento a ser removido do circuito
	 *  @param[inout] context Contexto indicando a situacao atual do diagrama ladder, sendo alterado para refletir as alteracoes realizadas
	 *  @return               Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool DelElement(LadderElem *elem, LadderContext &context);

	/** Funcao que verifica o circuito, removendo subcircuitos desnecessarios como, por exemplo, um subcircuito contendo apenas 1 elemento.
	 *  @param[inout] context Contexto indicando a situacao atual do diagrama ladder, sendo alterado para refletir as alteracoes realizadas
	 */
	void RemoveUnnecessarySubckts(LadderContext &context);

	/** Funcao que busca um subcircuito serie no circuito conforme a direcao solicitada
	 *  @param[in]    series    Ponteiro para o subcircuito a ser buscado
	 *  @param[inout] direction Direcao em que a busca deve ser realizada: SUBCKT_STATUS_FIRST (primeiro elemento) ou SUBCKT_STATUS_LAST (ultimo elemento)
	 *  @return                 Indica se o subcircuito foi encontrado (SUBCKT_STATUS_INSIDE) ou nao (SUBCKT_STATUS_NOTFOUND)
	 */
	int SearchMatch(LadderCircuit *series, int direction);
	/** Funcao que busca do elemento referenciado na estrutura InsertionPoint, preenchendo os campos que indicam o subcircuito serie e paralelo relacionados ao elemento (quando possivel)
	 *  @param[in]    context Contexto indicando a situacao atual do diagrama ladder, sendo alterado para refletir as alteracoes realizadas
	 *  @param[inout] point   Ponteiro para o ponto de insercao con a referencia ao elemento e os subcircuitos relacionados
	 *  @return               Resultado da operacao conforme os status a seguir:
	 *                        * SUBCKT_STATUS_NOTFOUND: Elemento nao encontrado.
	 *                        * SUBCKT_STATUS_FIRST   : Elemento encontrado e eh o primeiro elemento do circuito.
	 *                        * SUBCKT_STATUS_INSIDE  : Elemento encontrado e esta interno ao circuito, ou seja, nao se encontra em uma extremidade.
	 *                        * SUBCKT_STATUS_LAST    : Elemento encontrado e eh o ultimo elemento do circuito.
	 */
	int ElemInSubcktSeries(LadderContext context, InsertionPoint *point);

	/** Funcao que retorna uma copia do circuito com todos os elementos/subcircuitos
	 *  @param[in] diagram Ponteiro para o diagrama ao qual os elementos/subcircuitos devem ser associados
	 *  @return            Ponteiro para o circuito criado
	 */
	LadderCircuit *Clone(LadderDiagram *diagram);

	/** Funcao chamada apos a inclusao de um elemento no circuito para que etapas adicionais de atualizacao possam ser realizadas, como a atualizacao de referencia aos nodes do ModBUS
	 */
	void doPostInsert(void);
	/** Funcao chamada apos a remocao de um elemento do circuito para que etapas adicionais de atualizacao possam ser realizadas, como a atualizacao de referencia aos nodes do ModBUS
	 */
	void doPostRemove(void);

	/*** Funcoes para Salvar / Carregar um circuito do disco ***/

	/** Funcao que salva o circuito em disco
	 *  @param[in] f Handler do arquivo onde o circuito deve ser salvo
	 *  @return      Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool Save(FILE *f);
	/** Funcao que carrega o circuito do disco
	 *  @param[in] diagram Ponteiro para o diagrama ao qual os elementos/subcircuitos sendo carregados devem ser associados
	 *  @param[in] f       Handler do arquivo de onde o circuito deve ser carregado
	 *  @param[in] version Numero da versao do arquivo que esta sendo carregado
	 *  @return            Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool Load(LadderDiagram *diagram, FILE *f, unsigned int version);

	/*** Funcoes relacionadas com I/O ***/

	/** Funcao que indica se o tipo fornecido eh aceito pelos elementos/subcircuitos que o utilizam
	 *  @param[in] elem Ponteiro para o elemento a que pertence o I/O. Se nao for nulo, indica que este elemento esta descartando este I/O e portanto nao deve ser verificado se ele aceita o novo tipo
	 *  @param[in] id   ID que identifica o I/O que estamos consultando
	 *  @param[in] type tipo de I/O a ser checado
	 *  @return         Indica se o tipo eh aceito (true) ou nao (false)
	 */
	bool acceptIO       (LadderElem *elem, unsigned long id, eType type);
	/** Funcao que atualiza o I/O dos elementos do circuito, descartando ou fazendo request conforme a necessidade.
	 *  @param[in] owner     Ponteiro para o diagrama ao qual pertencia este circuito.
	 *  @param[in] isDiscard Indica se a operacao eh de descarte (true) ou nao (false).
	 */
	void updateIO       (LadderDiagram *owner, bool isDiscard);
	/** Funcao que carrega no vetor passado como parametro todos os tipos aceitos pelos elementos/subcircuitos para o I/O passado como parametro
	 *  @param[in]  elem  Ponteiro para o elemento a que pertence o I/O. Se nao for nulo, indica que os tipos permitidos por este elemento devem ser ignorados
	 *  @param[in]  id    ID que identifica o I/O que estamos consultando
	 *  @param[out] types vetor onde serao carregados os tipos de I/O aceitos pelos elementos/subcircuitos
	 */
	void getAllowedTypes(LadderElem *elem, unsigned long id, vector<eType> *types);

	/** Funcao que executa uma busca ou substituicao nos I/Os dos elementos/subcircuitos
	 *  @param[inout] refElem  Ponteiro que contem um ponteiro para o elemento de referencia para a busca. Ponteiro aponta para nulo se nao houver uma referencia (como no caso de buscar a partir do inicio, por exemplo)
	 *  @param[in]    idSearch ID que identifica o I/O que estamos trabalhando
	 *  @param[in]    sNewText Novo texto para o I/O sendo trabalhado
	 *  @param[in]    mode     Operacao de busca / substituicao a ser executada
	 *  @return                Numero de correspondencias encontradas. Se ocorrer um erro, retorna -1.
	 */
	int SearchAndReplace(LadderElem **refElem, unsigned long idSearch, string sNewText, eSearchAndReplaceMode mode);

	/** Funcao que executa uma acao de desfazer / refazer no circuito
	 *  @param[in]     IsUndo    Indica se a acao se refere a operacao de Desfazer (true) ou Refazer (false)
	 *  @param[in]     isDiscard Indica se devemos descartar a acao (true) ou executa-la (false)
	 *  @param[in,out] action    Variavel contendo os dados para Desfazer/Refazer a operacao
	 *  @return                  Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/** Classe representando o Diagrama Ladder, agregando tudo que representa um diagrama: circuitos, I/Os, codigo gerado, etc.
 */
class LadderDiagram {
private:
	/*** Variaveis e funcoes relacionados a acoes de Desfazer / Refazer ***/

	/// Numero de niveis de checkpoint, ou seja, numero de alteracoes que podem ser desfeitas
	unsigned int CheckPointLevels;
	/// Limite no numero de niveis de checkpoint. Ao alcancar esta marca, os checkpoints antigos serao removidos, impossibilitando que se desfaca as alteracoes descartadas
	unsigned int CheckPointLevelsMax;
	/// Quantidade de vezes que a funcao CheckpointBegin foi chamada em sequencia devido ao cascateamento de comandos
	unsigned int CheckpointBeginCount;

	/// Flag que indica se o checkpoint sendo registrado atualmente deve ser descartado no final. Isso permite que, no caso de ocorrer algum erro durante uma operacao,
	/// as alteracoes realizadas desde o ultimo checkpoint possam ser desfeitas, garantindo que o diagrama nao fique em um estado invalido
	bool CheckpointDoRollback;
	/// Indica se existem acoes registradas ao fechar um checkpoint 
	bool isCheckpointEmpty;

	/// Vetor que contem as linhas (circuitos) existentes no diagrama
	vector<LadderRung *> rungs;

	/// Ponteiro para funcao externa que desenha caracteres no buffer de texto, fornecida pelo usuario ao solicitar que o diagrama seja desenhado em modo texto
	tFncDrawChars fncDrawChars;

	/// Lista que contem os comandos registrados para se desfazer operacoes efetivadas
	deque<UndoRedoAction> UndoList;
	/// Lista que contem os comandos desfeitos, podendo ser utilizados para se refazer os comandos
	deque<UndoRedoAction> RedoList;

	/** Funcao que descarta um checkpoint registrado
	 *  @param[in] isUndo Indica se o checkpoint que deve ser descartado eh da lista de desfazer (true) ou refazer (false)
	 */
	void DiscardCheckpoint(bool isUndo = true);
	/** @brief Funcao que executa uma acao registrada
	 *  @param[in]     isUndo    Indica se a acao se refere a operacao de Desfazer (true) ou Refazer (false)
	 *  @param[in]     isDiscard Indica se devemos descartar a acao (true) ou executa-la (false)
	 *  @param[in,out] Action    Variavel contendo os dados da acao
	 *  @return                  Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool ExecuteAction(bool isUndo, bool isDiscard, UndoRedoAction Action);

	/// Estrutura que agraga todas as configuracoes do Diagrama Ladder
	typedef struct {
		LadderSettingsGeneral            General;
		LadderSettingsUART               Uart;
		LadderSettingsNetwork            Network;
		LadderSettingsSNTP               Sntp;
		LadderSettingsEncoderIncremental EncInc;
		LadderSettingsEncoderSSI         EncSSI;
		LadderSettingsADC                Adc;
		LadderSettingsDAC                Dac;
		LadderSettingsModbusSlave        MbSlave;
		LadderSettingsInformation        Info;
	} tLadderSettings;

	/// Variavel que representa as configuracoes do diagrama
	tLadderSettings LadderSettings;

	/// Funcao que registra uma acao no Undo/Redo com a configuracao atual, permitindo que uma alteracao possa ser desfeita
	void RegisterSettingsChanged(void);

	/// Estrutura que armazena dados de um item da fila de nodes do ModBUS
	typedef struct {
		int          NodeID;    ///< ID do node
		unsigned int NodeCount; ///< Contagem do node, ou seja, numero de locais em que ele eh utilizado
		LadderMbNode node;      ///< Dados do node em si
	} LadderMbNodeList;

	/// Vetor que armazena a lista de nodes cadastrados do ModBUS
	vector<LadderMbNodeList *> vectorMbNodeList;

	/// Enumeracao com os tipos de eventos de Undo/Redo suportados pelo diagrama
	enum UndoRedoActionsEnum {
		eCheckpoint = 0,  ///< Marcador de Checkpoint
		ePushRung,        ///< Acao de movimentacao de linha acima / abaixo
		eNewRung,         ///< Acao de insercao de nova linha no diagrama
		eDelRung,         ///< Acao de remocao de linha do diagrama
		eSettingsChanged, ///< Acao de alteracao de configuracao do diagrama
		eMbNodeCreate,    ///< Acao de criar node do ModBUS
		eMbNodeUpdate,    ///< Acao de atualizar node do ModBUS
		eMbNodeDelete,    ///< Acao de remocao de node do ModBUS
		eMbNodeRef        ///< Acao de referenciar node do ModBUS
	};

	/// Estrutura de dados para acoes de Desfazer / Refazer
	union UndoRedoData {
		/// Estrutura com dados para registro de acao desfazer/refazer movimentacao de linha do diagrama
		struct {
			int  pos;  ///< Posicao da linha a ser movimentada
			bool isUp; ///< Flag indicando se a linha deve ser movimentada para cima (true) ou para baixo (false)
		} PushRung;
		/// Estrutura com dados para registro de acao desfazer/refazer insercao de linha do diagrama
		struct {
			int  pos;         ///< Posicao onde a linha foi inserida
			LadderRung *rung; ///< Ponteiro para a linha inserida
		} NewRung;
		/// Estrutura com dados para registro de acao desfazer/refazer remocao de linha do diagrama
		struct {
			int  pos;         ///< Posicao de onde a linha foi removida
			LadderRung *rung; ///< Ponteiro para a linha removida
		} DelRung;
		/// Estrutura com dados para registro de acao desfazer/refazer alteracao de configuracao do diagrama
		struct {
			tLadderSettings *settings; ///< Ponteiro para estrutura que contem as configuracoes salvas para que possam ser restauradas
		} SettingsChanged;
		/// Estrutura com dados para registro de acao desfazer/refazer criacao de node do modBUS
		struct {
			LadderMbNodeList *nl; ///< Ponteiro para o item da lista de nodes que foi adicionado
		} MbNodeCreate;
		/// Estrutura com dados para registro de acao desfazer/refazer atualizacao de node do ModBUS
		struct {
			unsigned int      index; ///< ID do node que foi alterado
			LadderMbNode     *node;  ///< Ponteiro para a estrutura que contem informacoes do node alterado
		} MbNodeUpdate;
		/// Estrutura com dados para registro de acao desfazer/refazer remocao de node do ModBUS
		struct {
			unsigned int      index; ///< ID do node que foi removido
			LadderMbNodeList *nl;    ///< Ponteiro para o item da lista de nodes que foi removido
		} MbNodeDelete;
		/// Estrutura com dados para registro de acao desfazer/refazer referencia a node do ModBUS
		struct {
			unsigned int index;    ///< ID do node que foi referenciado
			bool         isAddRef; ///< Flag que indica se a operacao foi de adicionar (true) ou remover (false) referencia a um node do ModBUS
		} MbNodeRef;
	};

	// Variavel que contem o contexto do diagrama, ou seja, sua situacao atual como o que eh permitido fazer, elemento atualmente selecionado, etc
	LadderContext context;
	/// Flag que informa se o diagrama esta travado (true) ou nao (false), ou seja, se nao aceita alteracoes ou se aceita, respectivamente.
	bool          isLocked;

	/// Ponteiro para o mapa de I/Os, objeto que representa todos os I/Os utilizados no diagrama
	mapIO *IO;

	/// Estrutura que representa a CPU que estamos trabalhando.
	/// Heranca do antigo codigo, mantido para nao desenvolver tudo de novo
	McuIoInfo  *mcu;

	/// Flag que indica se a tela deve ser deslocada para exibir o item selecionado 
	bool NeedScrollSelectedIntoView;

	/// Objeto que armazena o codigo gerado pelo diagrama
	IntCode ic;

	/// Nome do arquivo atualmente carregado, permitindo o salvamento sem perguntar o nome ao usuario
	string currentFilename;

	/// Funcao que inicializa todas as variaveis do diagrama, garantindo que esteja com valores validos e possa ser utilizado
	void Init(void);

	/** Funcao que retorna o circuito ao qual o elemento pertence
	 *  @param[in] elem Ponteiro para o elemento sendo buscado
	 *  @return         Ponteiro para o circuito a que pertence o elemento ou nulo se nao for encontrado
	 */
	LadderCircuit *getSubcktForElement(LadderElem    *elem);
	/** Funcao que retorna o circuito ao qual pertence o circuito passado como parametro
	 *  @param[in] subckt Ponteiro para o circuito sendo buscado
	 *  @return           Ponteiro para o circuito a que pertence o circuito passado como parametro ou nulo se nao for encontrado
	 *                    ou nao tiver um pai, como no caso de ser passado como parametro uma linha do diagrama.
	 */
	LadderCircuit *getParentSubckt    (LadderCircuit *subckt);

	/** Funcao que percorre a lista de desfazer, atualizando a propriedade contextAfter das ultimas acoes cadastradas (especificamente as em que o
	 *  ponteiro para Diagram estiver nulo) para refletir o contexto do final da operacao.
	 *  @param[in] forceNotNull Flag para forcar a atualizacao de ContextAfter mesmo quando Diagram nao for nulo, atualizando o checkpoint mais recente
	 */
	void updateUndoContextAfter(bool forceNotNull = false);

	/** Funcao que adiciona um elemento em paralelo ao circuito, tendo como ponto inicial o elemento ParallelStart registrado na variavel de contexto
	 *  @param[in] elem Ponteiro para o elemento a ser inserido
	 *  @return         Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool InsertParallel(LadderElem *elem);

public:
	/// Construtor
	LadderDiagram(void);
	/// Destrutor
	~LadderDiagram(void);

	/** Funcao que retorna o vetor com o codigo intermediario do diagrama, previamente gerado pela funcao LadderDiagram::GenerateIntCode
	 *  @return Vetor com o codigo intermediario do diagrama
	 */
	vector<IntOp> getVectorIntCode(void);
	/** Funcao que gera o codigo intermediario para o diagrama
	 *  @return Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool GenerateIntCode(void);

	/** Funcao que calcula a largura do diagrama ao gerar saida em modo texto
	 *  @return Largura em colunas ocupada pelo diagrama em modo texto
	 */
	int getWidthTXT (void);
	/** Funcao que calcula a altura do diagrama ao gerar saida em modo texto
	 *  @return Altura em linhas ocupada pelo diagrama em modo texto
	 */
	int getHeightTXT(void);

	/// Funcao que verifica o diagrama, atualizando o contexto para refletir essa situacao
	void          updateContext(void);
	/** Funcao que retorna o contexto atual do diagrama
	 *  @return Variavel contendo o contexto atual
	 */
	LadderContext getContext   (void) { return context; }
	/** Funcao que configura a trava do diagrama, indicando se podem ser realizadas alteracoes ou nao.
	 *  @param[in] state Flag que configura o estado da trava do diagrama para travado (true) ou destravado (false).
	 */
	void SetLock               (bool state);
	/** Funcao que retorna se o diagrama esta travado, ou seja, se alteracoes estao bloqueadas
	 *  @return Flag indicando se o diagrama esta travado (true) ou nao (false).
	 */
	bool IsLocked              (void) { return isLocked; }

	/// Funcao que libera todas as variaveis do diagrama, desalocando toda a memoria previamente alocada
	void FreeDiagram (void);
	/// Funcao que limpa o diagrama, descarregando o diagrama atual e configurando as variaveis para o estado inicial
	void ClearDiagram(void);
	/// Funcao que cria um novo diagrama, limpando-o e adicionando a primeira linha e seu respectivo placeholder
	void NewDiagram  (void);
	/** Funcao que ativa ou desativa o diagrama para que nao gere eventos de atualizacao quando nao estiver em uso
	 *  @param[in] isActive Flag que configura o diagrama como ativado (true) ou desativado (false).
	 */
	void Activate    (bool isActive);

	/** Funcao que seleciona um elemento do diagrama, que eh o elemento que recebera os comandos
	 *  @param[in] elem  Ponteiro para o elemento que deve ser selecionado
	 *  @param[in] state Estado da selecao para o elemento, podendo ser:
	 *                   * SELECTED_ABOVE: Indica que a face superior do elemento esta selecionado
	 *                   * SELECTED_BELOW: Indica que a face inferior do elemento esta selecionado
	 *                   * SELECTED_RIGHT: Indica que a face direita do elemento esta selecionado
	 *                   * SELECTED_LEFT : Indica que a face esquerda do elemento esta selecionado
	 */
	void SelectElement(LadderElem *elem, int state);

	/** Funcao que adiciona um elemento ao circuito no ponto indicado pelo objeto selecionado. Assim, se a selecao estiver a direita do elemento
	 *  atualmente selecionado, o novo elemento sera adicionado a sua direita.
	 *  @param[in] elem Ponteiro para o elemento a ser inserido
	 *  @return         Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool AddElement         (LadderElem *elem);
	/** Funcao que remove um elemento do circuito
	 *  @param[in] elem Ponteiro para o elemento a ser inserido. Se nulo, sera removido o elemento atualmente selecionado
	 *  @return         Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool DelElement         (LadderElem *elem = nullptr);
	/** Funcao que copia um elemento para a area de transferencia passada como parametro
	 *  @param[in] clipboard Ponteiro para a area de transferencia onde a copia do elemento sera armazenada
	 *  @param[in] elem      Ponteiro para o elemento a ser copiado. Se nulo, sera copiado o elemento atualmente selecionado
	 *  @return              Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool CopyElement        (LadderClipboard *clipboard, LadderElem *elem = nullptr);
	/** Funcao que cola no diagrama o elemento da area de transferencia passada como parametro
	 *  @param[in] clipboard Ponteiro para a area de transferencia onde a copia do elemento esta armazenada
	 *  @return              Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool PasteElement       (LadderClipboard  clipboard);
	/** Funcao que cola no diagrama o elemento da area de transferencia passada como parametro
	 *  @return Indica se o elemento foi alterado (true) ou nao (false)
	 */
	bool EditSelectedElement(void);

	/** Funcao que retorna o indice da linha do diagrama que contem o elemento passado como parametro
	 *  @param[in] elem Ponteiro para o elemento sendo buscado
	 *  @return         Numero da linha do diagrama que contem o elemento ou -1 se nao foi encontrado
	 */
	int  RungContainingElement (LadderElem *elem);
	/** Funcao que retorna o indice da linha do diagrama que contem o elemento atualmente selecionado
	 *  @return Numero da linha do diagrama que contem o elemento atualmente selecionado ou -1 se nao foi encontrado (Ex.: nenhum elemento selecionado)
	 */
	int  RungContainingSelected(void);

	/** Funcao que adiciona uma nova linha ao diagrama
	 *  @param[in] isAfter Flag indicando se a linha deve ser adicionada antes (false) ou depois (true) da linha com o elemento atualmente selecionado
	 *  @return            Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool NewRung    (bool isAfter);
	/** Funcao que move no diagrama a linha da posicao indicada
	 *  @param[in] rung Indice da linha que deve ser movida
	 *  @param[in] up   Flag indicando se a linha deve ser movida para cima (true) ou para baixo (false)
	 *  @return         Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool PushRung   (int rung, bool up);
	/** Funcao que move no diagrama a linha da posicao indicada
	 *  @param[in] rung          Indice da linha que deve ser removida
	 *  @param[in] isFreeDiagram Flag que indica se esta operacao esta sendo realizada devido a descarte do diagrama (true) ou nao (false)
	 *  @return                  Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool DeleteRung (int rung, bool isFreeDiagram = false);
	/** Funcao que indica se a linha esta vazia
	 *  @param[in] rung Indice da linha que deve ser verificada
	 *  @return         Indica se a linha esta vazia (true) ou nao (false)
	 */
	bool IsRungEmpty(unsigned int rung);
	/** Funcao que copia uma linha para a area de transferencia passada como parametro
	 *  @param[in] clipboard Ponteiro para a area de transferencia onde a copia da linha sera armazenada
	 *  @param[in] elem      Ponteiro para um elemento pertencente a linha a ser copiada. Se nulo, sera copiada a linha contendo o elemento atualmente selecionado
	 *  @return              Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool CopyRung   (LadderClipboard *clipboard, LadderElem *elem = nullptr);
	/** Funcao que cola no diagrama a linha da area de transferencia passada como parametro
	 *  @param[in] clipboard Ponteiro para a area de transferencia onde a copia da linha esta armazenada
	 *  @param[in] isAfter   Flag indicando se a linha deve ser adicionada antes (false) ou depois (true) da linha com o elemento atualmente selecionado
	 *  @return              Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool PasteRung  (LadderClipboard  clipboard, bool isAfter);

	/** Funcao que desenha o diagrama em modo texto
	 *  @param[in] fnc Ponteiro para funcao que desenha os caracteres enviados, implementada pelo usuario conforme a necessidade
	 */
	void DrawTXT(tFncDrawChars fnc);
	/** Funcao que retorna o ponteiro para a funcao de desenho de cacteres em modo texto
	 *  @return Ponteiro para a funcao de desenho de caracteres atualmente em uso ou nulo se nao existir
	 */
	tFncDrawChars getFncDrawChars(void) { return fncDrawChars; }

	/// Funcao que desenha o diagrama em modo grafico
	void DrawGUI(void);
	/** Funcao que informa ao diagrama que o diagrama precisa ser redesenhado
	 *  @param[in] isFullRedraw Flag que indica se precisa ser totalmente redesenhado (true) ou nao (false)
	 */
	void NeedRedraw(bool isFullRedraw);

	/** Funcao que indica se um elemento esta visivel na tela ou nao
	 *  @param[in] elem           Ponteiro para um elemento sendo verificado
	 *  @param[in] isFullyVisible Flag que indica se deve ser informado se o elemento esta totalmente visivel (true) ou apenas parcialmente (false)
	 *  @return                   Indica se o elemento esta visivel (true) ou nao (false). Se elemento nao for encontrado, retorna false.
	 */
	bool IsElementVisible (LadderElem *elem, bool isFullyVisible = true);
	/** Funcao que indica se o elemento atualmente selecionado esta visivel na tela ou nao
	 *  @param[in] isFullyVisible Flag que indica se deve ser informado se o elemento esta totalmente visivel (true) ou apenas parcialmente (false)
	 *  @return                   Indica se o elemento esta visivel (true) ou nao (false). Se elemento nao for encontrado, retorna false.
	 */
	bool IsSelectedVisible(bool isFullyVisible = true) { return IsElementVisible(context.SelectedElem, isFullyVisible); }

	/** Funcao que informa ao diagrama que houve movimento do mouse
	 *  @param[in] x Nova posicao do mouse no eixo X
	 *  @param[in] y Nova posicao do mouse no eixo Y
	 */
	void        MouseMove    (int x, int y);
	/** Funcao que informa ao diagrama que houve clique do mouse
	 *  @param[in] x        Posicao onde ocorreu o clique do mouse no eixo X
	 *  @param[in] y        Posicao onde ocorreu o clique do mouse no eixo Y
	 *  @param[in] isDown   Flag que indica se o botao do mouse foi pressionado (true) ou solto (false)
	 *  @param[in] isDouble Flag que indica se ocorreu um duplo-clique (true) ou clique simples (false)
	 */
	void        MouseClick   (int x, int y, bool isDown, bool isDouble);
	/** Funcao que move o cursor de selecao conforme o comando passado como parametro
	 *  @param[in] moveTo Indica o tipo de movimento do cursor como: cima, baixo, inicio da linha, etc.
	 */
	void        MoveCursor   (eMoveCursor moveTo);
	/** Funcao que busca por um elemento a partir do elemento selecionado, conforme o tipo de movimento solicitado
	 *  @param[in] moveTo Indica o tipo de movimento a executar, como: cima, baixo, inicio da linha, etc.
	 *  @return           Ponteiro para o elemento encontrado ou nulo se nao foi encontrado
	 */
	LadderElem *SearchElement(eMoveCursor moveTo);

	/** Funcao que adiciona o ponto de inicio do paralelo na posicao atual
	 *  @return Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool AddParallelStart(void);

	/*** Funcoes para Salvar / Carregar um Diagrama Ladder do disco ***/

	/** Funcao que salva o diagrama em disco
	 *  @param[in] filename String com o nome do arquivo em que o diagrama deve ser salvo. Se vazia, sera utilizado o nome atual
	 *  @param[in] isBackup Flag que indica se estamos salvando uma copia de backup (true) ou nao (false)
	 *  @return             Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool   Save              (string filename, bool isBackup = false);
	/** Funcao que carrega o diagrama do disco
	 *  @param[in] filename String com o nome do arquivo de onde o diagrama deve ser carregado. Se vazia, sera utilizado o nome atual
	 *  @return             Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool   Load              (string filename);
	/** Funcao que retorna o nome atual do arquivo (nome utilizado ao carregar ou salvar o arquivo anteriormente)
	 *  @return String com o nome do arquivo ou vazio se for um novo diagrama ainda nao salvo
	 */
	string getCurrentFilename(void);

	/// Funcao que indica que houve uma mudanca no diagrama ladder.
	/// Dessa forma sabemos que o arquivo precisa ser salvo e podemos alertar o usuario
	void ProgramChanged(void);

	/** Funcao que retorna a estrutura que representa a CPU
	 *  @return Ponteiro para a estrutura com a configuracao da CPU em uso
	 */
	McuIoInfo *getMCU(void) { return mcu; }

	/*** Funcoes relacionadas ao Breakpoint ***/

	/** Funcao que inverte o estado do breakpoint (ativo / inativo) em uma determinada linha
	 *  @param[in] rung Indice da linha que deve ser utilizada
	 */
	void ToggleBreakPoint(unsigned int rung);

	/** Funcao que retorna o numero da linha (a partir de 1) que contem um breakpoint ativo ou zero se nao houver
	 *  @return Indice a partir de 1 da linha que possui breakpoint ativo. 0 = Nenhuma linha com breakpoint ativo
	 */
	unsigned int getBreakPointActiveAtRung(void);

	/*** Funcoes para ler / gravar a configuracao do ladder ***/

	/** Funcao que retorna as configuracoes gerais do diagrama
	 *  @return Estrutura com as configuracoes gerais do diagrama
	 */
	LadderSettingsGeneral            getSettingsGeneral           (void) { return LadderSettings.General; }
	/** Funcao que retorna as configuracoes de serial do diagrama
	 *  @return Estrutura com as configuracoes de serial do diagrama
	 */
	LadderSettingsUART               getSettingsUART              (void) { return LadderSettings.Uart   ; }
	/** Funcao que retorna as configuracoes de rede do diagrama
	 *  @return Estrutura com as configuracoes de rede do diagrama
	 */
	LadderSettingsNetwork            getSettingsNetwork           (void) { return LadderSettings.Network; }
	/** Funcao que retorna as configuracoes de SNTP do diagrama
	 *  @return Estrutura com as configuracoes de SNTP do diagrama
	 */
	LadderSettingsSNTP               getSettingsSNTP              (void) { return LadderSettings.Sntp   ; }
	/** Funcao que retorna as configuracoes de encoder incremental do diagrama
	 *  @return Estrutura com as configuracoes de encoder incremental do diagrama
	 */
	LadderSettingsEncoderIncremental getSettingsEncoderIncremental(void) { return LadderSettings.EncInc ; }
	/** Funcao que retorna as configuracoes de encoder SSI do diagrama
	 *  @return Estrutura com as configuracoes de encoder SSI do diagrama
	 */
	LadderSettingsEncoderSSI         getSettingsEncoderSSI        (void) { return LadderSettings.EncSSI ; }
	/** Funcao que retorna as configuracoes de A/D do diagrama
	 *  @return Estrutura com as configuracoes A/D do diagrama
	 */
	LadderSettingsADC                getSettingsADC               (void) { return LadderSettings.Adc    ; }
	/** Funcao que retorna as configuracoes de D/A do diagrama
	 *  @return Estrutura com as configuracoes de D/A do diagrama
	 */
	LadderSettingsDAC                getSettingsDAC               (void) { return LadderSettings.Dac    ; }
	/** Funcao que retorna as configuracoes de ModBUS escravo do diagrama
	 *  @return Estrutura com as configuracoes de ModBUS escravo do diagrama
	 */
	LadderSettingsModbusSlave        getSettingsModbusSlave       (void) { return LadderSettings.MbSlave; }
	/** Funcao que retorna as informacoes do diagrama (nome do diagrama e autor, ultima compilacao, gravacao, etc.
	 *  @return Estrutura com as informacoes do diagrama
	 */
	LadderSettingsInformation        getSettingsInformation       (void) { return LadderSettings.Info   ; }

	/** Funcao que atualiza as configuracoes gerais do diagrama
	 *  @param[in] setGeneral Estrutura com as novas configuracoes gerais do diagrama
	 */
	void                             setSettingsGeneral           (LadderSettingsGeneral            setGeneral);
	/** Funcao que atualiza as configuracoes de serial do diagrama
	 *  @param[in] setUart Estrutura com as novas configuracoes de serial do diagrama
	 */
	void                             setSettingsUART              (LadderSettingsUART               setUart   );
	/** Funcao que atualiza as configuracoes de rede do diagrama
	 *  @param[in] setNetwork Estrutura com as novas configuracoes de rede do diagrama
	 */
	void                             setSettingsNetwork           (LadderSettingsNetwork            setNetwork);
	/** Funcao que atualiza as configuracoes de SNTP do diagrama
	 *  @param[in] setSntp Estrutura com as novas configuracoes de SNTP do diagrama
	 */
	void                             setSettingsSNTP              (LadderSettingsSNTP               setSntp   );
	/** Funcao que atualiza as configuracoes de encpoder incremental do diagrama
	 *  @param[in] setEncInc Estrutura com as novas configuracoes de encoder incremental do diagrama
	 */
	void                             setSettingsEncoderIncremental(LadderSettingsEncoderIncremental setEncInc );
	/** Funcao que atualiza as configuracoes de encoder SSI do diagrama
	 *  @param[in] setEncSSI Estrutura com as novas configuracoes de encoder SSI do diagrama
	 */
	void                             setSettingsEncoderSSI        (LadderSettingsEncoderSSI         setEncSSI );
	/** Funcao que atualiza as configuracoes de A/D do diagrama
	 *  @param[in] setAdc Estrutura com as novas configuracoes de A/D do diagrama
	 */
	void                             setSettingsADC               (LadderSettingsADC                setAdc    );
	/** Funcao que atualiza as configuracoes de D/A do diagrama
	 *  @param[in] setDac Estrutura com as novas configuracoes de D/A do diagrama
	 */
	void                             setSettingsDAC               (LadderSettingsDAC                setDac    );
	/** Funcao que atualiza as configuracoes de ModBUS escravo do diagrama
	 *  @param[in] setMbSlave Estrutura com as novas configuracoes de ModBUS escravo do diagrama
	 */
	void                             setSettingsModbusSlave       (LadderSettingsModbusSlave        setMbSlave);
	/** Funcao que atualiza as informacoes do diagrama (nome do diagrama e autor, ultima compilacao, gravacao, etc.
	 *  @param[in] setInfo Estrutura com as novas informacoes do diagrama
	 */
	void                             setSettingsInformation       (LadderSettingsInformation        setInfo   );

	/** Funcao que configura o modo de simulacao
	 *  @param[in] state Flag indicando se o modo de simulacao esta ativado (true) ou desativado (false)
	 */
	void setSimulationState(bool state);

	/*** Funcoes para manipular/acessar lista de nodes do ModBUS ***/

	/** Funcao que carrega os valores padrao a um node do ModBUS
	 *  @param[in] node Ponteiro para o node do ModBUS
	 */
	void         mbClearNode       (LadderMbNode *node);
	/** Funcao que cria um node do ModBUS com os valores padrao a partir do nome do node. Se ja existir um node
	 *  com o nome solicitado, este node ja existente sera atualizado.
	 *  @param[in] NodeName Nome do node do ModBUS a ser criado
	 *  @return             ID do node criado/atualizado ou -1 em caso de erro
	 */
	int          mbCreateNode      (string NodeName);
	/** Funcao que cria um node do ModBUS a partir da estrutura node. Se ja existir um node com o nome solicitado,
	 *  este node ja existente sera atualizado.
	 *  @param[in] node Estrutura com as informacoes do node do ModBUS a ser criado
	 *  @return         ID do node criado/atualizado ou -1 em caso de erro
	 */
	int          mbCreateNode      (LadderMbNode node);
	/** Funcao que atualiza um node do ModBUS com os dados da estrutura de node passada como parametro. Se nao existir
	 *  um node com o ID passado como parametro, ele sera criado
	 *  @param[in] NodeID ID do node do ModBUS a ser atualizado
	 *  @param[in] node   Estrutura com as informacoes do node do ModBUS a ser atualizado
	 *  @return           ID do node atualizado/criado ou -1 em caso de erro
	 */
	int          mbUpdateNode      (int NodeID, LadderMbNode node);
	/** Funcao que exclui um node do ModBUS a partir de seu ID
	 *  @param[in] NodeID Nome do node do ModBUS a ser criado
	 */
	void         mbDeleteNode      (int NodeID );
	/** Funcao que retorna a estrutura de dados de um node do ModBUS a partir de seu indice na lista
	 *  @param[in] elem Indice do node na lista de nodes do ModBUS
	 *  @return         Estrutura com as informacoes do node do ModBUS. Se node inexistente, retorna um node com os dados padrao
	 */
	LadderMbNode mbGetNodeByIndex  (int elem   );
	/** Funcao que retorna a estrutura de dados de um node do ModBUS a partir de seu ID
	 *  @param[in] NodeID ID do node do ModBUS
	 *  @return           Estrutura com as informacoes do node do ModBUS. Se node inexistente, retorna um node com os dados padrao
	 */
	LadderMbNode mbGetNodeByNodeID (int NodeID );
	/** Funcao que retorna o ID de um node do ModBUS a partir de seu nome
	 *  @param[in] name Nome do node do ModBUS
	 *  @return         ID do node do ModBUS ou -1 se node inexistente
	 */
	int          mbGetNodeIDByName (string name);
	/** Funcao que retorna o ID de um node do ModBUS a partir de seu indice na lista
	 *  @param[in] elem Indice do node na lista de nodes do ModBUS
	 *  @return         ID do node do ModBUS ou -1 se node inexistente
	 */
	int          mbGetNodeIDByIndex(int elem   );
	/** Funcao que retorna o indice de um node na lista de nodes do ModBUS a partir de seu ID
	 *  @param[in] NodeID ID do node do ModBUS
	 *  @return           Indice do node na lista de nodes do ModBUS ou -1 se node inexistente
	 */
	int          mbGetIndexByNodeID(int NodeID );
	/** Funcao que retorna a contagem (numero de referencias) de um node do ModBUS
	 *  @param[in] NodeID ID do node do ModBUS
	 *  @return           Contagem do node do ModBUS
	 */
	unsigned int mbGetNodeCount    (int NodeID );
	/** Funcao que adiciona referencia a um node do ModBUS
	 *  @param[in] NodeID ID do node do ModBUS
	 */
	void         mbAddRef          (int NodeID );
	/** Funcao que remove referencia de um node do ModBUS
	 *  @param[in] NodeID ID do node do ModBUS
	 */
	void         mbDelRef          (int NodeID );

	/*** Funcoes relacionadas com I/O ***/

	/** Funcao que checa se um tipo para um determinado I/O eh aceito pelo diagrama
	 *  @param[in] id ID para o I/O sendo verificado
	 *  @param[in] type  Tipo sendo verificado
	 *  @return          Indica se o tipo eh aceito pelo diagrama (true) ou nao (false)
	 */
	bool            acceptIO                (unsigned long  id, eType type);
	/** Funcao que atualiza um I/O
	 *  @param[in]    owner     Ponteiro para o diagrama a que pertence o I/O, utilizado no caso de estarmos copiando o I/O de outro diagrama
	 *  @param[in]    elem      Ponteiro para o elemento a que pertence o I/O, utilizado no caso de estarmos copiando o I/O de outro diagrama ou para operacoes de descarte
	 *  @param[inout] infoIO    Dados do I/O sendo requisitado / descartado
	 *  @param[in]    isDiscard Flag indicando se o I/O esta sendo requisitado (false) ou descartado (true)
	 */
	void            updateIO                (LadderDiagram *owner, LadderElem *elem, tRequestIO &infoIO, bool isDiscard);
	/** Funcao que atualiza o tipo de um I/O, corrigindo falhas como uma variavel reservada mas marcada com algum tipo especifico
	 *  @param[in] id ID para o I/O sendo atualizado
	 */
	void            updateTypeIO            (unsigned long id);
	/** Funcao que requisita um I/O
	 *  @param[in]    elem                Ponteiro para o elemento a que pertence o I/O, utilizado em operacoes de descarte para ignorar os tipos aceitos por este elemento
	 *  @param[inout] infoIO              Dados do I/O sendo requisitado. O pino contido nesta estrutura sera atualizado com o ID do I/O
	 *  @param[in]    tryGeneralTypeFirst Indica que devemos tentar usar o tipo geral e, se falhar, usar o tipo passado em infoIO
	 *  @return                           Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool            getIO                   (LadderElem *elem, tRequestIO &infoIO, bool tryGeneralTypeFirst = false);
	/** Funcao que requisita um I/O conforme dados passados por parametro. Se nao for possivel, tenta utilizar os dados da estrutura infoIO
	 *  @param[in]  elem                Ponteiro para o elemento a que pertence o I/O, utilizado em operacoes de descarte para ignorar os tipos aceitos por este elemento
	 *  @param[out] pin                 Referencia ao pino do I/O, sera carregada com o ID do I/O solicitado
	 *  @param[in]  name                Nome do I/O sendo requisitado
	 *  @param[in]  type                Tipo para o I/O sendo requisitado
	 *  @param[in]  infoIO              Dados padrao do I/O sendo requisitado para que , caso falhar com os dados passados, tente utilizar os contidos nesta estrutura
	 *  @param[in]  tryGeneralTypeFirst Indica que devemos tentar usar o tipo geral e, se falhar, usar o tipo em type
	 *  @return                         Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool            getIO                   (LadderElem *elem, pair<unsigned long, int> &pin, string name, eType type, tRequestIO infoIO, bool tryGeneralTypeFirst = false);
	/** Funcao que requisita varios I/Os simultaneamente conforme vetor de dados de I/O passado por parametro.
	 *  @param[in]    elem        Ponteiro para o elemento a que pertence o I/O, utilizado em operacoes de descarte para ignorar os tipos aceitos por este elemento
	 *  @param[inout] vectorGetIO Vetor de dados dos I/Os sendo requisitados. Os pinos de cada item do vetor serao atualizados para o ID do I/O solicitado
	 *  @return                   Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool            getIO                   (LadderElem *elem, vector<tRequestIO> &vectorGetIO);
	/** Funcao que retorna o ID de um I/O a partir de seu nome
	 *  @param[in] name Nome do I/O sendo pesquisado
	 *  @return         ID do I/O ou 0 se inexistente
	 */
	unsigned long   getIdIO                 (string name);
	/** Funcao que retorna o tipo correto para um I/O. Tente utilizar o tipo do novo nome, o nome anterior e, por ultimo, o tipo default. Verifica tambem
	 *  se o I/O eh do tipo generico se assim for solicitado
	 *  @param[in] previousName Nome anterior do I/O
	 *  @param[in] newName      Novo nome do I/O
	 *  @param[in] default_type Tipo default a ser utilizado se nao conseguir recuperar os tipos dos I/Os atual e anterior
	 *  @param[in] isGeneric    Flag que indica se o tipo deve ser do tipo generico (true) ou se pode ser qualquer tipo (false)
	 *  @return                 Tipo do I/O
	 */
	eType           getTypeIO               (string previousName, string newName, eType default_type = eType_Pending, bool isGeneric = false);
	/** Funcao que retorna o nome de um I/O a partir de seu ID
	 *  @param[in] id ID para o I/O sendo pesquisado
	 *  @return       Nome do I/O ou string vazia se inexistente
	 */
	string          getNameIO               (unsigned long id);
	/** Funcao que retorna o nome de um I/O a partir de seu pino
	 *  @param[in] pin Pino que representa o I/O
	 *  @return        Nome (ou valor, quando id do pino for zero) do I/O ou string vazia se inexistente
	 */
	string          getNameIO               (pair<unsigned long, int> pin);
	/** Funcao que retorna o nome de um I/O a partir de seu indice no vetor de I/O
	 *  @param[in] index Indice com a posicao no vetor de I/Os
	 *  @return          Nome do I/O ou string vazia se inexistente
	 */
	string          getNameIObyIndex        (unsigned int index);
	/** Funcao que retorna a estrutura com detalhes de um I/O a partir de seu ID
	 *  @param[in] id ID para o I/O sendo pesquisado
	 *  @return       Estrutura com os detalhes do I/O. Se nao encontrado, retorna uma estrutura com os dados padrao e tipo Pendente
	 */
	mapDetails      getDetailsIO            (unsigned long  id);
	/** Funcao que retorna a estrutura com detalhes de um I/O a partir de seu nome
	 *  @param[in] name Nome do I/O sendo pesquisado
	 *  @return         Estrutura com os detalhes do I/O. Se nao encontrado, retorna uma estrutura com os dados padrao e tipo Pendente
	 */
	mapDetails      getDetailsIO            (string name);
	/** Funcao que retorna o nome do tipo do I/O na posicao do vetor passada como parametro
	 *  @param[in] index Indice com a posicao no vetor de I/Os
	 *  @return          Nome do tipo do I/O solicitado ou string vazia se inexistente
	 */
	const char     *getStringTypeIO         (unsigned int   index);
	/** Funcao que retorna o numero de I/Os registrados
	 *  @return Numero de I/Os registrados
	 */
	unsigned int    getCountIO              (void);
	/** Funcao que marca um I/O como selecionado para que seja exibido na lista de I/Os na tela
	 *  @param[in] index Indice do vetor de I/Os com a posicao do I/O que deve ser selecionado
	 */
	void            selectIO                (unsigned int index);
	/** Funcao que indica se um I/O eh uma variavel interna a partir de seu nome
	 *  @param[in] name Nome do I/O sendo verificado
	 *  @return         Indica se o I/O eh uma variavel interna (true) ou nao (false)
	 */
	bool            IsInternalVarIO         (string name);
	/** Funcao que indica se um I/O eh uma flag interna a partir de seu nome
	 *  @param[in] name Nome do I/O sendo verificado
	 *  @return         Indica se o I/O eh uma flag interna (true) ou nao (false)
	 */
	bool            IsInternalFlagIO        (string name);
	/** Funcao que indica se um tipo de I/O eh do tipo generico
	 *  @param[in] type Tipo sendo verificado
	 *  @return         Indica se o tipo eh generico (true) ou nao (false)
	 */
	bool            IsGenericTypeIO         (eType type);
	/** Funcao que retorna o nome da porta do I/O na posicao do vetor passada como parametro
	 *  @param[in] index Indice com a posicao no vetor de I/Os
	 *  @return          Nome da porta do I/O solicitado ou string vazia se inexistente
	 */
	string          getPortNameIO           (int index);
	/** Funcao que retorna o nome do pino do I/O na posicao do vetor passada como parametro
	 *  @param[in] index Indice com a posicao no vetor de I/Os
	 *  @return          Nome do pino do I/O solicitado ou string vazia se inexistente
	 */
	string          getPinNameIO            (int index);
	/** Funcao que retorna o numero do pino associado ao I/O com a descricao passada como parametro
	 *  @param[in] name Nome do pino associado ao I/O conforme exibido na tela de atribuicao de I/O
	 *  @return         Numero do I/O referente a descricao passada como parametro
	 */
	int             getPinFromDialogListIO  (string name);
	/** Funcao que retorna o nome no firmware da variavel interna passada como parametro
	 *  @param[in] name Nome de usuario da variavel interna
	 *  @return         Nome da variavel interna no firmware
	 */
	string          getInternalVarNameIO    (string name);
	/** Funcao que exibe a janela de dialogo para o I/O na posicao do vetor de I/Os indicada pelo parametro
	 *  @param[in] item Indice com a posicao no vetor de I/Os
	 */
	void            ShowIoMapDialog         (int item);
	/** Funcao que retorna o vetor com os nomes de todas as flags internas existentes
	 *  @return Vetor com os nomes de todas as flags internas existentes
	 */
	vector<string>  getVectorInternalFlagsIO(void);
	/** Funcao que retorna vetor com todos os tipos de uso geral
	 *  @return Vetor com todos os tipos de uso geral
	 */
	vector<eType>   getGeneralTypes         (void);
	/** Funcao que retorna vetor com os nomes de todos os I/Os registrados
	 *  @return Vetor com os nomes de todos os I/Os registrados
	 */
	vector<string>  getListIO               (void);
	/** Funcao que retorna o nome reservado padrao associado ao tipo solicitado
	 *  @param[in] type Tipo do I/O
	 *  @return         Nome da variavel reservada para o tipo solicitado
	 */
	string          getReservedNameIO       (eType type);

	/** Funcao que retorna um I/O do cache
	 *  @param[in] id ID do I/O sendo requisitado
	 *  @return       Nome e tipo do I/O. Se inexistente, retorna nome vazio e tipo pendente
	 */
	pair<string, eType> getCachedIO(unsigned long id);

	/** Funcao que retorna o tipo de I/O permitido para o ID fornecido como parametro
	 *  @param[in]  elem  Ponteiro para o elemento a que pertence o I/O. Se nao for nulo, indica que os tipos permitidos por este elemento devem ser ignorados
	 *  @param[in] id           ID do I/O sendo verificado
	 *  @param[in] default_type Tipo padrao a ser retornado caso nao se encontre um tipo permitido
	 *  @return                 Tipo de I/O permitido para o ID fornecido como parametro
	 */
	eType getAllowedType(LadderElem *elem, unsigned long id, eType default_type);

	/** Funcao para ordenar a lista de I/Os conforme o campo especificado
	 *  @param[in] sortby Campo que deve ser usado para ordenar a lista de I/Os
	 */
	void            sortIO                  (eSortBy sortby);

	/*** Funcoes que indicam se existe algum I/O que use a UART e o PWM, respectivamente ***/

	/** Funcao que indica se o diagrama possui um elemento que utilize a interface serial
	 *  @return Indica se algum elemento do diagrama utiliza a interface serial (true) ou nao (false).
	 */
	bool UartFunctionUsed(void);
	/** Funcao que indica se o diagrama possui um elemento de PWM
	 *  @return Indica se o diagrama possui um elemento de PWM (true) ou nao (false).
	 */
	bool PwmFunctionUsed (void);

	/** Funcao que realiza uma busca / substituicao de nome de I/O entre os I/Os do diagrama
	 *  @param[in] sSearchText Nome atual do I/O
	 *  @param[in] sNewText    Novo nome para o I/O
	 *  @param[in] mode        Operacao de busca / substituicao a ser executada
	 *  @return                Numero de correspondencias encontradas no diagrama
	 */
	int SearchAndReplace(string sSearchText, string sNewText, eSearchAndReplaceMode mode);

	/*** Funcoes relacionadas aos comandos de Desfazer / Refazer ***/

	/** Funcao que registra uma acao de desfazer / refazer
	 *  @param[in] Action Variavel contendo os dados para Desfazer/Refazer a operacao
	 *  @param[in] isUndo Indica se a acao se refere a operacao de Desfazer (true) ou Refazer (false)
	 */
	void RegisterAction    (UndoRedoAction Action, bool isUndo = true);
	/** Funcao que cria um checkpoint com o nome da tarefa relacionada
	 *  @param[in] description Nome da operacao sendo registrada
	 */
	void CheckpointBegin   (string description);
	/// Funcao que informa que ocorreu algum erro na operacao atual e o checkpoint sendo registrado deve ser descartado
	void CheckpointRollback(void) { CheckpointDoRollback = true; }
	/// Funcao que finaliza o checkpoint iniciado por LadderDiagram::CheckpointBegin
	void CheckpointEnd     (void);
	/** Funcao que executa uma operacao de Desfazer / Refazer
	 *  @param[in] isUndo Flag indicando se a operacao a ser executada eh de Desfazer (true) ou Refazer (false)
	 */
	void UndoRedo          (bool isUndo);
	/// Funcao que executa uma operacao de Desfazer
	void Undo              (void) { UndoRedo( true); }
	/// Funcao que executa uma operacao de Refazer
	void Redo              (void) { UndoRedo(false); }

	/** Funcao que verifica se os dois nomes de I/O sao equivalentes, como no caso de nomes truncados ou com diferencas de Maiuscula / Minuscula
	 *  @param[in] name1 Primeiro nome a ser verificado
	 *  @param[in] name2 Segundo nome a ser verificado
	 *  @return          Indica se os nomes sao equivalentes (true) ou nao (false)
	 */
	bool equalsNameIO             (string name1, string name2);

	/** Funcao que verifica se o texto passado como parametro representa um numero
	 *  @param[in] varnumber Texto a ser verificado
	 *  @return              Indica se o texto fornecido corresponde a um numero (true) ou nao (false)
	 */
	bool IsValidNumber     (string varnumber);
	/** Funcao que verifica se o texto passado como parametro representa um nome valido para um I/O
	 *  @param[in] varname Texto a ser verificado
	 *  @return            Indica se o texto fornecido corresponde a um nome valido (true) ou nao (false)
	 */
	bool IsValidVarName    (string varname);
	/** Funcao que verifica se o I/O passado como parametro eh valido
	 *  @param[in] id        ID atual do I/O (quando existir)
	 *  @param[in] name      Nome solicitado para o I/O
	 *  @param[in] type      Tipo solicitado para o I/O
	 *  @param[in] FieldName Nome do campo sendo atualizado, sera utilizado para exibir uma mensagem amigavel ao usuario no caso de valor invalido
	 *  @param[in] Rules     Regras a serem usadas para a validacao, podendo ser agregadas com o comando OU binario ( | ). Os valores aceitos sao:
	 *                       * VALIDATE_IS_VAR           : O nome do I/O deve obrigatoriamente ser uma variavel. Incompativel com VALIDATE_IS_NUMBER e VALIDATE_IS_VAR_OR_NUMBER
	 *                       * VALIDATE_IS_NUMBER        : O nome do I/O deve obrigatoriamente ser um numero. Incompativel com VALIDATE_IS_VAR e VALIDATE_IS_VAR_OR_NUMBER
	 *                       * VALIDATE_IS_VAR_OR_NUMBER : O nome do I/O pode ser uma variavel ou numero. Incompativel com VALIDATE_IS_VAR e VALIDATE_IS_NUMBER
	 *                       * VALIDATE_DONT_ASK         : Nao perguntar ao usuario uma solucao no caso de conflitos, retornando erro na validacao
	 *                       * VALIDATE_TYPES_MUST_MATCH : Os tipos de I/Os devem ser iguais, nao apenas compativeis
	 *                       * VALIDATE_ACCEPT_IO_PENDING: Tipo de I/O Pendente eh aceito na validacao
	 *  @param[in] MinVal    Valor minimo aceito para valores numericos. Se for zero e MaxVal tambem for zero, desabilita a checagem por limites
	 *  @param[in] MaxVal    Valor maximo aceito para valores numericos. Se for zero e MinVal tambem for zero, desabilita a checagem por limites
	 *  @param[in] reply     Ponteiro para variavel indicando o tipo de resposta dada pelo usuario no caso de haver sido exibida alguma caixa de dialogo
	 *  @return              Indica se os dados fornecidos sao validos (true) ou nao (false)
	 */
	bool IsValidNameAndType(unsigned long id, string name, eType type, const char *FieldName, unsigned int Rules, int MinVal, int MaxVal, eReply *reply = nullptr);
	/** Funcao que verifica se o I/O passado como parametro eh valido
	 *  @param[in] id        ID atual do I/O (quando existir)
	 *  @param[in] name      Nome solicitado para o I/O
	 *  @param[in] type      Tipo solicitado para o I/O
	 *  @param[in] reply     Ponteiro para variavel indicando o tipo de resposta dada pelo usuario no caso de haver sido exibida alguma caixa de dialogo
	 *  @return              Indica se os dados fornecidos sao validos (true) ou nao (false)
	 */
	bool IsValidNameAndType(unsigned long id, string name, eType type, eReply *reply = nullptr);

	/** Funcao que verifica o diagrama, buscando por erros como pinos sem atribuicao, valores incompativeis, variaveis nao usadas, etc.
	 *  @return Indica se o diagrama esta OK (eValidateResult_OK), se houve algum aviso (eValidateResult_Warning) ou se existe um erro (eValidateResult_Error)
	 */
	eValidateResult Validate(void);

	eReply ShowValidateDialog(bool isError, const char *msg);

	/** Funcao que exibe uma janela de dialogo para o usuario. Dependente de implementacao da interface
	 *  @param[in] type      Tipo de janela de dialogo a ser exibida
	 *  @param[in] hasCancel Flag indicando se deve ser exibida a opcao para cancelar (true) ou nao (false)
	 *  @param[in] title     Titulo da janela de dialogo
	 *  @param[in] message   Mensagem da janela de dialogo. Formato de printf, podendo passar argumentos adicionais para montar o texto conforme o formato
	 *  @return              Resposta do usuario para a janela de dialogo
	 */
	eReply ShowDialog(eDialogType type, bool hasCancel, const char *title, const char *message, ...);

	/** Funcao que executa uma acao de desfazer / refazer registrada para o diagrama
	 *  @param[in]     IsUndo    Indice se a acao se refere a operacao de Desfazer (true) ou Refazer (false)
	 *  @param[in]     isDiscard Indica se devemos descartar a acao (true) ou executa-la (false)
	 *  @param[in,out] action    Variavel contendo os dados para Desfazer/Refazer a operacao
	 *  @return                  Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

#endif
