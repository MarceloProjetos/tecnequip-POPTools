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
	eMoveCursor_DiagramHome = 0, ///< Movimenta o cursor para o inicio do diagrama, ou seja, o primeiro elemento da primeira linha
	eMoveCursor_RungHome,        ///< Movimenta o cursor para o inicio da linha atual
	eMoveCursor_DiagramEnd,      ///< Movimenta o cursor para o final do diagrama, ou seja, o ultimo elemento da ultima linha
	eMoveCursor_RungEnd,         ///< Movimenta o cursor para o final da linha atual
	eMoveCursor_Up,              ///< Movimenta o cursor para cima. Altera a selecao do objeto atual para a parte superior e, caso ja selecionado, busca o objeto mais proximo.
	eMoveCursor_Down,            ///< Movimenta o cursor para baixo. Altera a selecao do objeto atual para a parte inferior e, caso ja selecionado, busca o objeto mais proximo.
	eMoveCursor_Left,            ///< Movimenta o cursor para a esquerda. Altera a selecao do objeto atual para a lateral esquerda e, caso ja selecionado, busca o objeto mais proximo.
	eMoveCursor_Right,           ///< Movimenta o cursor para a direita. Altera a selecao do objeto atual para a lateral direita e, caso ja selecionado, busca o objeto mais proximo.
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
	bool canDeleteRung;             ///< Indica se a linha atual pode ser excluida
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

/*** Estruturas auxiliares relacionada a lista de nos do ModBUS ***/

/// Enumeracao que indica o tipo de no do ModBUS
enum eMbTypeNode {
	eMbTypeNode_RS485 = 0, ///< Este no comunica utilizando a interface RS-485
	eMbTypeNode_Ethernet   ///< Este no comunica utilizando a interface Ethernet
};

/// Estrutura que representa um no do ModBUS
typedef struct {
	string        name;  ///< Nome do no do ModBUS
	int           id;    ///< ID do no, utilizado quando comunicando por RS-485
	unsigned long ip;    ///< IP do no, utilizado quando comunicando por Ethernet
	eMbTypeNode   iface; ///< Interface utilizada pelo no
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

	/** Funcao puramente virtual executada ao atualizar as propriedades de um elemento
	 *  @param[in] data       Ponteiro para as propriedades, conforme o tipo do elemento
	 *  @param[in] isUndoRedo Flag que indica se esta funcao foi chamada durante uma operacao de desfazer/refazer
	 *  @return               Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalSetProperties(void *data, bool isUndoRedo) = 0;

	/** Funcao puramente virtual que gera o codigo intermediario, implementada pelo elemento para gerar seu codigo especifico
	 *  @param[in,out] ic Referencia ao vetor contendo o codigo intermediario, onde as novas instrucoes serao agregadas
	 *  @return           Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalGenerateIntCode(IntCode &ic) = 0;

	/** Funcao puramente virtual que salva os dados do elemento em disco
	 *  @param[in] f Ponteiro para o handler do arquivo sendo salvo
	 *  @return      Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalSave(FILE *f) = 0;
	/** Funcao puramente virtual que carrega os dados do elemento do disco
	 *  @param[in] f       Ponteiro para o handler do arquivo sendo carregado
	 *  @param[in] version Variavel indicando a versao do arquivo sendo carregado, possibilitando conversoes de formato
	 *  @return            Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalLoad(FILE *f, unsigned int version) = 0;

	/** Funcao que inicializa as variaveis privadas
	 */
	void Init(void);

	/** Funcao puramente virtual que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	 *  @param[in] index Indice para indicar qual o I/O que deve ser atualizado
	 *  @param[in] name  Nome para o qual o I/O deve ser atualizado
	 *  @param[in] type  Tipo para o qual o I/O deve ser atualizado
	 *  @return          Indica se a operacao foi realizada com sucesso (true) ou se falhou (false)
	 */
	virtual bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) = 0;

	/** Funcao puramente virtual que executa uma acao de desfazer / refazer nas classes derivadas
	 *  @param[in]     IsUndo    Indice se a acao se refere a operacao de Desfazer (true) ou Refazer (false)
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

	/** Funcao puramente virtual que gera o texto utilizado para impressao e exportacao em modo texto
	 *  @return Par de strings (Linha superior e linha inferior) que representam o elemento em modo texto
	 */
	virtual pair<string, string> DrawTXT(void) = 0;
	/** Funcao puramente virtual que desenha o elemento na tela
	 *  @param[in] poweredBefore Indica se a entrada do elemento esta energizada em uma simulacao
	 *  @param[in] data          Variavel que contem informacoes para desenhar o objeto na tela, especifica da interface
	 *  @return                  Estado da saida do elemento na simulacao: energizada (true) ou nao (false)
	 */
	virtual bool DrawGUI(bool poweredBefore, void *data) = 0;

	/** Funcao puramente virtual que exibe a janela de dialogo para configuracao do elemento
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
	/** Funcao puramente virtual que retorna as propriedades do elemento
	 *  @return          Ponteiro para as propriedades especificas deste elemento
	 */
	virtual void *getProperties(void) = 0;

	/** Funcao puramente virtual que cria uma copia do elemento
	 *  @param[in] diagram Ponteiro para o diagrama ao qual o novo elemento deve ser associado
	 *  @return            Ponteiro para o elemento criado
	 */
	virtual LadderElem *Clone(LadderDiagram *diagram) = 0;

	/*** Funcoes relacionadas com I/O ***/

	/** Funcao puramente virtual que indica se o I/O pode ser alterado para o tipo solicitado
	 *  @param[in] id   ID do I/O sendo alterado
	 *  @param[in] type Tipo para o qual o I/O sera alterado
	 *  @return         Indica se a mudanca de tipo do I/O eh permitida (true) ou nao (false)
	 */
	virtual bool  acceptIO        (unsigned long id, eType type) = 0;
	/** Funcao puramente virtual que informa o elemento que ele deve atualizar seus I/Os
	 *  @param[in] owner Ponteiro para o diagrama ao qual o I/O pertencia originalmente
	 *  @param[in] isDiscard Informa se nesta operacao os I/Os devem ser descartados (true) ou atualizados (false)
	 */
	virtual void  updateIO        (LadderDiagram *owner, bool isDiscard) = 0;
	/** Funcao puramente virtual que retorna o tipo permitido para este I/O pelo elemento atual
	 *  @param[in] id ID do I/O sendo solicitado
	 *  @return       Tipo de I/O permitido pelo elemento atual
	 */
	virtual eType getAllowedTypeIO(unsigned long id) = 0;
	/** Funcao puramente virtual que realiza uma busca / substituicao de nome de I/O entre os seus I/Os
	 *  @param[in] idSearch  ID do I/O sendo buscado / substituido
	 *  @param[in] sNewText  string contendo o novo nome para o I/O
	 *  @param[in] isReplace Flag que indica se esta operacao eh de substituicao (true) ou busca (false)
	 *  @return              Numero de correspondencias encontradas no elemento atual
	 */
	virtual int   SearchAndReplace(unsigned long idSearch, string sNewText, bool isReplace) = 0;

	/** Funcao puramente virtual que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
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

// Classe do elemento PlaceHolder
class LadderElemPlaceHolder : public LadderElem {
	// Sem propriedades privadas...
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Comment
struct LadderElemCommentProp {
	string str;
};

class LadderElemComment : public LadderElem {
private:
	LadderElemCommentProp prop;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Contato
struct LadderElemContactProp {
    bool                     negated;
	pair<unsigned long, int> idName;
};

class LadderElemContact : public LadderElem {
private:
	LadderElemContactProp prop;
	tRequestIO            infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Bobina
struct LadderElemCoilProp {
    bool                     negated;
    bool                     setOnly;
    bool                     resetOnly;
	pair<unsigned long, int> idName;
};

class LadderElemCoil : public LadderElem {
private:
	LadderElemCoilProp prop;
	tRequestIO         infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Timer
struct LadderElemTimerProp {
	pair<unsigned long, int> idName;
    int                      delay;
};

class LadderElemTimer : public LadderElem {
private:
	LadderElemTimerProp prop;
	tRequestIO          infoIO_Name;

	int TimerPeriod(void);

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento RTC
struct LadderElemRTCProp {
	int           mode;  // Continuous or Intermittent (when using date/hour range) or only a fixed date/hour.
    unsigned char wday;  // [0:0] Sum, [0:1] Mon, [0:2] Tue, [0:3] Wed, [0:4] Thu, [0:5] Fri, [0:6] Sat, [0:7] WDay 1=YES, 0=No
	struct tm     start; // Start date/time of the range or the fixed one
	struct tm     end;   // End date/time when using range.
};

class LadderElemRTC : public LadderElem {
private:
	LadderElemRTCProp prop;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Counter
struct LadderElemCounterProp {
	pair<unsigned long, int> idName;
    int                      max;
};

class LadderElemCounter : public LadderElem {
private:
	LadderElemCounterProp prop;
	tRequestIO            infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Reset Timer / Counter
struct LadderElemResetProp {
	pair<unsigned long, int> idName;
};

class LadderElemReset : public LadderElem {
private:
	LadderElemResetProp prop;
	tRequestIO          infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento One Shot
class LadderElemOneShot : public LadderElem {
	// Sem propriedades privadas...
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Cmp
struct LadderElemCmpProp {
	pair<unsigned long, int> idOp1;
	pair<unsigned long, int> idOp2;
};

class LadderElemCmp : public LadderElem {
private:
	LadderElemCmpProp prop;
	tRequestIO        infoIO_Op1;
	tRequestIO        infoIO_Op2;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Math
struct LadderElemMathProp {
	pair<unsigned long, int> idOp1;
	pair<unsigned long, int> idOp2;
	pair<unsigned long, int> idDest;
};

class LadderElemMath : public LadderElem {
private:
	LadderElemMathProp prop;
	tRequestIO         infoIO_Op1;
	tRequestIO         infoIO_Op2;
	tRequestIO         infoIO_Dest;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Sqrt
struct LadderElemSqrtProp {
	pair<unsigned long, int> idDest;
	pair<unsigned long, int> idSrc;
};

class LadderElemSqrt : public LadderElem {
private:
	LadderElemSqrtProp prop;
	tRequestIO         infoIO_Dest;
	tRequestIO         infoIO_Src;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Rand
struct LadderElemRandProp {
	pair<unsigned long, int> idVar;
	pair<unsigned long, int> idMin;
	pair<unsigned long, int> idMax;
};

class LadderElemRand : public LadderElem {
private:
	LadderElemRandProp prop;
	tRequestIO         infoIO_Var;
	tRequestIO         infoIO_Min;
	tRequestIO         infoIO_Max;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Abs
struct LadderElemAbsProp {
	pair<unsigned long, int> idDest;
	pair<unsigned long, int> idSrc;
};

class LadderElemAbs : public LadderElem {
private:
	LadderElemAbsProp prop;
	tRequestIO        infoIO_Dest;
	tRequestIO        infoIO_Src;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Move
struct LadderElemMoveProp {
	pair<unsigned long, int> idDest;
	pair<unsigned long, int> idSrc;
};

class LadderElemMove : public LadderElem {
private:
	LadderElemMoveProp prop;
	tRequestIO         infoIO_Dest;
	tRequestIO         infoIO_Src;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Open & Short
class LadderElemOpenShort : public LadderElem {
	// Sem propriedades privadas...
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Set Bit
struct LadderElemSetBitProp {
	pair<unsigned long, int> idName;
	int    bit;
	bool   set;
};

class LadderElemSetBit : public LadderElem {
private:
	LadderElemSetBitProp prop;
	tRequestIO           infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Check Bit
struct LadderElemCheckBitProp {
	pair<unsigned long, int> idName;
	int    bit;
	bool   set;
};

class LadderElemCheckBit : public LadderElem {
private:
	LadderElemCheckBitProp prop;
	tRequestIO             infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Read A/D
struct LadderElemReadAdcProp {
	pair<unsigned long, int> idName;
};

class LadderElemReadAdc : public LadderElem {
private:
	LadderElemReadAdcProp prop;
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

// Classe do elemento Set D/A
struct LadderElemSetDaProp {
	pair<unsigned long, int> idName;
	int    mode;
};

class LadderElemSetDa : public LadderElem {
private:
	LadderElemSetDaProp prop;
	tRequestIO          infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
	LadderElemSetDa(LadderDiagram *diagram);

	pair<string, string> DrawTXT(void);
	bool DrawGUI(bool poweredBefore, void *data);

	bool ShowDialog(LadderContext context);

	bool internalGenerateIntCode(IntCode &ic);

	void *getProperties(void);

	bool CanInsert(LadderContext context);

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

// Classe do elemento Read Encoder
struct LadderElemReadEncProp {
	pair<unsigned long, int> idName;
};

class LadderElemReadEnc : public LadderElem {
private:
	LadderElemReadEncProp prop;
	tRequestIO            infoIO_Name;

	// Estruturas de request das variaveis internas
	tRequestIO             infoIO_IncPerimRoda;
	tRequestIO             infoIO_IncPulsosVolta;
	tRequestIO             infoIO_IncFatorCorr;
	tRequestIO             infoIO_AbsPerimRoda;
	tRequestIO             infoIO_AbsFatorCorr;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Reset Encoder
struct LadderElemResetEncProp {
	pair<unsigned long, int> idName;
};

class LadderElemResetEnc : public LadderElem {
private:
	LadderElemResetEncProp prop;
	tRequestIO             infoIO_Name;

	// Estruturas de request das variaveis internas
	tRequestIO             infoIO_IncPerimRoda;
	tRequestIO             infoIO_IncPulsosVolta;
	tRequestIO             infoIO_IncFatorCorr;
	tRequestIO             infoIO_AbsPerimRoda;
	tRequestIO             infoIO_AbsFatorCorr;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Multiset D/A
struct LadderElemMultisetDAProp {
	pair<unsigned long, int>	idTime;					// Tempo
	pair<unsigned long, int>	idDesl;					// Deslocamento
	bool						linear;					// true = Linear, false = Curva Ascendente/Descendente
	bool						forward;				// true = Avança, false = Recua
	bool						speedup;				// true = Aceleração, false = Desaceleração
	int							initval;				// valor inicial do DA na rampa
	int							type;					// 0 = Valor saida do DA (2048 ~ -2048), 1 = milivolt (mV) (10V ~-10V), 2 = percentual (%)
	int							gaint;					// tempo da curva de ganho em %
	int							gainr;					// resolução da curva de ganho em %
	bool						StartFromCurrentValue;	// false = Iniciar ou ir para zero, conforme speedup. true = partir do valor atual até o valor configurado
	int							ramp_abort_mode;
};

class LadderElemMultisetDA : public LadderElem {
private:
	LadderElemMultisetDAProp prop;
	tRequestIO               infoIO_Time;
	tRequestIO               infoIO_Desl;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Read / Write USS
struct LadderElemUSSProp {
	pair<unsigned long, int> idName;
	int		id;
	int		parameter;
	int		parameter_set;
	int		index;
};

class LadderElemUSS : public LadderElem {
private:
	LadderElemUSSProp prop;
	tRequestIO        infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento ModBUS
struct LadderElemModBUSProp {
	pair<unsigned long, int> idName;
	int		elem;
	int		address;
	bool	int32;
	bool	retransmitir;
};

class LadderElemModBUS : public LadderElem {
private:
	LadderElemModBUSProp prop;
	tRequestIO           infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Set PWM
struct LadderElemSetPWMProp {
	pair<unsigned long, int> idName;
    int     targetFreq;
};

class LadderElemSetPWM : public LadderElem {
private:
	LadderElemSetPWMProp prop;
	tRequestIO           infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Send / Receive UART
struct LadderElemUARTProp {
	pair<unsigned long, int> idName;
};

class LadderElemUART : public LadderElem {
private:
	LadderElemUARTProp prop;
	tRequestIO         infoIO_Name;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Master Relay
class LadderElemMasterRelay : public LadderElem {
	// Flag que indica se a chave esta ligada ou nao
	bool isMasterRelayActive;

	// Sem propriedades privadas...
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Shift Register
struct LadderElemShiftRegisterProp {
	vector< pair<unsigned long, int> > vectorIdRegs;
	string nameReg;
    int    stages;
};

class LadderElemShiftRegister : public LadderElem {
private:
	LadderElemShiftRegisterProp prop;
	tRequestIO                  InfoIO_Regs;

	bool internalSetProperties(void *data, bool isUndoRedo, bool isCloning);
	bool internalSetProperties(void *data, bool isUndoRedo = false) { return internalSetProperties(data, isUndoRedo, false); }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	void createRegs(void);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

// Classe do elemento Look Up Table
struct LadderElemLUTProp {
	pair<unsigned long, int>           idDest;
	pair<unsigned long, int>           idIndex;
    int                                count;
    bool                               editAsString;
    array<long, MAX_LOOK_UP_TABLE_LEN> vals;
};

class LadderElemLUT : public LadderElem {
private:
	LadderElemLUTProp prop;
	tRequestIO        infoIO_Dest;
	tRequestIO        infoIO_Index;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Piecewise Linear
struct LadderElemPiecewiseProp {
	pair<unsigned long, int>           idDest;
	pair<unsigned long, int>           idIndex;
    int                                count;
    array<long, MAX_LOOK_UP_TABLE_LEN> vals;
};

class LadderElemPiecewise : public LadderElem {
private:
	LadderElemPiecewiseProp prop;
	tRequestIO              infoIO_Dest;
	tRequestIO              infoIO_Index;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Formatted String
struct LadderElemFmtStringProp {
	pair<unsigned long, int> idVar;
    string txt;
};

class LadderElemFmtString : public LadderElem {
private:
	LadderElemFmtStringProp prop;
	tRequestIO              infoIO_Var;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Read / Write Yaskawa
struct LadderElemYaskawaProp {
	pair<unsigned long, int> idVar;
    int id;
    string txt;
};

class LadderElemYaskawa : public LadderElem {
private:
	LadderElemYaskawaProp prop;
	tRequestIO            infoIO_Var;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento Persist
struct LadderElemPersistProp {
	pair<unsigned long, int> idVar;
};

class LadderElemPersist : public LadderElem {
private:
	LadderElemPersistProp prop;
	tRequestIO            infoIO_Var;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento PID
struct LadderElemPIDProp {
	pair<unsigned long, int> idSetpoint;
	pair<unsigned long, int> idProcessValue;
	pair<unsigned long, int> idOutput;
	pair<unsigned long, int> idGainP;
	pair<unsigned long, int> idGainI;
	pair<unsigned long, int> idGainD;
};

class LadderElemPID : public LadderElem {
private:
	LadderElemPIDProp prop;
	tRequestIO        infoIO_Setpoint;
	tRequestIO        infoIO_ProcessValue;
	tRequestIO        infoIO_Output;
	tRequestIO        infoIO_GainP;
	tRequestIO        infoIO_GainI;
	tRequestIO        infoIO_GainD;

	bool internalSetProperties(void *data, bool isUndoRedo = false);

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f);
	bool internalLoad(FILE *f, unsigned int version);

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type);

public:
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

// Classe do elemento X
struct LadderElemXProp {
};

class LadderElemX : public LadderElem {
private:
	// Sem propriedades privadas...
	LadderElemXProp prop;
	tRequestIO      infoIO_;

	bool internalSetProperties(void *data, bool isUndoRedo = false) { return true; }

	// Funcoes para ler / gravar dados especificos do elemento no disco
	bool internalSave(FILE *f) { return true; }
	bool internalLoad(FILE *f, unsigned int version) { return true; }

	// Funcao que atualiza o I/O indicado por index para o novo nome/tipo (se possivel)
	bool internalUpdateNameTypeIO(unsigned int index, string name, eType type) { return false; }

public:
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

/*** Classe representando os circuitos (Serie / Paralelo) ***/

class LadderCircuit {
private:
	bool isSeries;

	vector<Subckt> vectorSubckt;

	bool DrawElementTXT(vector< vector<int> > &DisplayMatrix, LadderElem *elem, int *cx, int *cy, bool poweredBefore);
	void VerticalWireTXT(int cx, int cy);

	// Estrutura de dados para acoes de Desfazer / Refazer
	enum UndoRedoActionsEnum { eCheckpoint = 0, eInsertSubckt, eDeleteSubckt, eMoveSubckt };
	union UndoRedoData {
		struct {
			unsigned int pos;
			Subckt       subckt;
		} InsertSubckt;
		struct {
			unsigned int pos;
			Subckt       subckt;
		} DeleteSubckt;
		struct {
			unsigned int   pos;
			unsigned int   fromPos;
			LadderCircuit *circuit;
		} MoveSubckt;
	};

protected:
	bool InsertSubckt(LadderContext context, unsigned int pos, Subckt s, bool isMove = false,
		bool isUndoRedo = false);
	bool DeleteSubckt(LadderContext context, unsigned int pos, bool isMove = false,	bool isUndoRedo = false);
	bool MoveSubckt  (LadderContext context, unsigned int pos, LadderCircuit *fromCircuit,
		unsigned int fromPos, bool isUndoRedo = false);

public:
	LadderCircuit(void);
	LadderCircuit(bool newSeries);

	~LadderCircuit(void);

	bool DrawTXT(vector< vector<int> > &DisplayMatrix, int *cx, int *cy, bool poweredBefore, int ColsAvailable);
	bool DrawGUI(bool poweredBefore, void *data);

	bool IsComment(void);
	bool IsEmpty(void);
	bool IsLast(LadderElem *elem);
	bool IsSeries(void) { return isSeries; }
	bool HasEOL(void);
	bool UartFunctionUsed(void);

	bool GenerateIntCode(IntCode &ic);

	unsigned int   getSize            (void) { return vectorSubckt.size(); }
	Subckt         getNext            (Subckt         next);
	LadderCircuit *getSubcktForElement(LadderElem    *elem);
	LadderCircuit *getParentSubckt    (LadderCircuit *subckt);
	Subckt         getSubckt          (unsigned int   pos);

	// Funcao que retorna o primeiro elemento (nao subcircuito) do circuito atual
	// Se o primeiro elemento for um subcircuito, busca o primeiro elemento desde subcircuito
	LadderElem *getFirstElement(void);

	// Funcao que retorna o ultimo elemento (nao subcircuito) do circuito atual
	// Se o ultimo elemento for um subcircuito, busca o ultimo elemento desde subcircuito
	LadderElem *getLastElement(void);

	int            getWidthTXT (int ColsAvailable);
	int            getHeightTXT(void);

	void           setDiagram  (LadderDiagram *newDiagram);

	void AddPlaceHolderIfNoEOL(LadderContext context);
	void DeleteEndPlaceHolder (LadderContext context);
	bool AddElement(LadderElem *elem, LadderContext &context);
	bool InsertParallel(LadderElem *elem, unsigned int start, unsigned int end, LadderContext &context);
	bool DelElement(LadderElem *elem, LadderContext &context);

	void RemoveUnnecessarySubckts(LadderContext context);

	int SearchMatch(LadderCircuit *series, int direction);
	int ElemInSubcktSeries(LadderContext &context, InsertionPoint *point);

	LadderCircuit *Clone(LadderDiagram *diagram);

	void doPostInsert(void);
	void doPostRemove(void);

	// Funcoes para Salvar / Carregar um circuito do disco
	bool Save(LadderDiagram *diagram, FILE *f);
	bool Load(LadderDiagram *diagram, FILE *f, unsigned int version);

	// Funcoes relacionadas com I/O
	bool acceptIO       (unsigned long id, eType type);
	void updateIO       (LadderDiagram *owner, bool isDiscard);
	void getAllowedTypes(unsigned long id, vector<eType> *types);

	int SearchAndReplace(LadderElem **refElem, unsigned long idSearch, string sNewText, eSearchAndReplaceMode mode);

	// Funcao que executa uma acao de desfazer / refazer
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

/*** Classe representando o Diagrama Ladder ***/
class LadderDiagram {
private:
	// Variaveis e funcoes relacionados a acoes de Desfazer / Refazer
	unsigned int CheckPointLevels;
	unsigned int CheckPointLevelsMax;
	unsigned int CheckpointBeginCount;

	bool CheckpointDoRollback;
	// Indica se existem acoes registradas ao fechar um checkpoint 
	bool isCheckpointEmpty;

	// list of rungs
	vector<LadderRung *> rungs;

	// Undo / Redo Action Lists
	deque<UndoRedoAction> UndoList;
	deque<UndoRedoAction> RedoList;

	void DiscardCheckpoint(bool isUndo = true);
	bool ExecuteAction(bool isUndo, bool isDiscard, UndoRedoAction Action);

	// Configuracoes do Diagrama Ladder
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

	tLadderSettings LadderSettings;

	// Funcao que Registra no Undo/Redo a configuracao atual para que possa ser desfeito
	void RegisterSettingsChanged(void);

	// Lista de nos do ModBUS
	typedef struct {
		int          NodeID;
		unsigned int NodeCount;
		LadderMbNode node;
	} LadderMbNodeList;

	vector<LadderMbNodeList *> vectorMbNodeList;

	// Estrutura de dados para acoes de Desfazer / Refazer
	enum UndoRedoActionsEnum { eCheckpoint = 0, ePushRung, eNewRung, eDelRung, eSettingsChanged,
		eMbNodeCreate, eMbNodeUpdate, eMbNodeDelete, eMbNodeRef };
	union UndoRedoData {
		struct {
			int  pos;
			bool isUp;
		} PushRung;
		struct {
			int  pos;
			LadderRung *rung;
		} NewRung;
		struct {
			int  pos;
			LadderRung *rung;
		} DelRung;
		struct {
			tLadderSettings *settings;
		} SettingsChanged;
		struct {
			LadderMbNodeList *nl;
		} MbNodeCreate;
		struct {
			unsigned int      index;
			LadderMbNode     *node;
		} MbNodeUpdate;
		struct {
			unsigned int      index;
			LadderMbNodeList *nl;
		} MbNodeDelete;
		struct {
			unsigned int index;
			bool         isAddRef;
		} MbNodeRef;
	};

	// Context properties
	LadderContext context;
	bool          isLocked;

	// Objeto que contem todos os I/Os
	mapIO *IO;

	// Estrutura que representa a CPU que estamos trabalhando
	// Heranca do antigo codigo, mantido para nao desenvolver tudo de novo
	McuIoInfo  *mcu;

	bool NeedScrollSelectedIntoView;

	IntCode ic;

	string currentFilename;

	void Init(void);

	LadderCircuit *getSubcktForElement(LadderElem    *elem);
	LadderCircuit *getParentSubckt    (LadderCircuit *subckt);

	void updateUndoContextAfter(bool forceNotNull = false);

	bool InsertParallel(LadderElem *elem);

public:
	LadderDiagram(void);
	~LadderDiagram(void);

	vector<IntOp> getVectorIntCode(void);
	bool GenerateIntCode(void);

	int getWidthTXT (void);
	int getHeightTXT(void);

	void          updateContext(void);
	LadderContext getContext   (void) { return context; }
	void SetLock               (bool state);
	bool IsLocked              (void) { return isLocked; }

	void FreeDiagram (void);
	void ClearDiagram(void);
	void NewDiagram  (void);
	void Activate    (bool isActive);

	void SelectElement(LadderElem *elem, int state);

	bool AddElement         (LadderElem *elem);
	bool DelElement         (LadderElem *elem = nullptr);
	bool CopyElement        (LadderClipboard *clipboard, LadderElem *elem = nullptr);
	bool PasteElement       (LadderClipboard  clipboard);
	bool EditSelectedElement(void);

	int  RungContainingElement (LadderElem *elem);
	int  RungContainingSelected(void);

	void NewRung    (bool isAfter);
	bool PushRung   (int rung, bool up);
	bool DeleteRung (int rung);
	bool IsRungEmpty(unsigned int n);
	bool CopyRung   (LadderClipboard *clipboard, LadderElem *elem = nullptr);
	bool PasteRung  (LadderClipboard  clipboard, bool isAfter);

	void DrawTXT(int OffsetX);
	void DrawGUI(void);
	void NeedRedraw(bool isFullRedraw);

	bool IsElementVisible (LadderElem *elem, bool isFullyVisible = true);
	bool IsSelectedVisible(bool isFullyVisible = true) { return IsElementVisible(context.SelectedElem, isFullyVisible); }

	void        MouseMove    (int x, int y);
	void        MouseClick   (int x, int y, bool isDown, bool isDouble);
	void        MoveCursor   (eMoveCursor moveTo);
	LadderElem *SearchElement(eMoveCursor moveTo);

	bool AddParallelStart(void);

	// Funcoes para Salvar / Carregar um Diagrama Ladder do disco
	bool   Save              (string filename, bool dontSaveFilename = false);
	bool   Load              (string filename);
	string getCurrentFilename(void);

	// Funcao que indica que houve uma mudanca no diagrama ladder.
	// Dessa forma sabemos que o arquivo precisa ser salvo e podemos alertar o usuario
	void ProgramChanged(void);

	// Funcao que retorna a estrutura que representa a CPU
	McuIoInfo *getMCU(void) { return mcu; }

	// Funcao para inverter o estado do breakpoint (ativo / inativo) em uma determinada linha
	void ToggleBreakPoint(unsigned int rung);

	// Funcoes para ler / gravar a configuracao do ladder
	LadderSettingsGeneral            getSettingsGeneral           (void) { return LadderSettings.General; }
	LadderSettingsUART               getSettingsUART              (void) { return LadderSettings.Uart   ; }
	LadderSettingsNetwork            getSettingsNetwork           (void) { return LadderSettings.Network; }
	LadderSettingsSNTP               getSettingsSNTP              (void) { return LadderSettings.Sntp   ; }
	LadderSettingsEncoderIncremental getSettingsEncoderIncremental(void) { return LadderSettings.EncInc ; }
	LadderSettingsEncoderSSI         getSettingsEncoderSSI        (void) { return LadderSettings.EncSSI ; }
	LadderSettingsADC                getSettingsADC               (void) { return LadderSettings.Adc    ; }
	LadderSettingsDAC                getSettingsDAC               (void) { return LadderSettings.Dac    ; }
	LadderSettingsModbusSlave        getSettingsModbusSlave       (void) { return LadderSettings.MbSlave; }
	LadderSettingsInformation        getSettingsInformation       (void) { return LadderSettings.Info   ; }

	void                             setSettingsGeneral           (LadderSettingsGeneral            setGeneral);
	void                             setSettingsUART              (LadderSettingsUART               setUart   );
	void                             setSettingsNetwork           (LadderSettingsNetwork            setNetwork);
	void                             setSettingsSNTP              (LadderSettingsSNTP               setSntp   );
	void                             setSettingsEncoderIncremental(LadderSettingsEncoderIncremental setEncInc );
	void                             setSettingsEncoderSSI        (LadderSettingsEncoderSSI         setEncSSI );
	void                             setSettingsADC               (LadderSettingsADC                setAdc    );
	void                             setSettingsDAC               (LadderSettingsDAC                setDac    );
	void                             setSettingsModbusSlave       (LadderSettingsModbusSlave        setMbSlave);
	void                             setSettingsInformation       (LadderSettingsInformation        setInfo   );

	// Funcao para configurar o modo de simulacao
	void setSimulationState(bool state);

	// Funcoes para manipular/acessar lista de nos do ModBUS
	void         mbClearNode       (LadderMbNode *node);
	int          mbCreateNode      (string NodeName);
	int          mbCreateNode      (LadderMbNode node);
	int          mbUpdateNode      (int NodeID, LadderMbNode node);
	void         mbDeleteNode      (int NodeID );
	LadderMbNode mbGetNodeByIndex  (int elem   );
	LadderMbNode mbGetNodeByNodeID (int NodeID );
	int          mbGetNodeIDByName (string name);
	int          mbGetNodeIDByIndex(int elem   );
	int          mbGetIndexByNodeID(int NodeID );
	unsigned int mbGetNodeCount    (int NodeID );
	void         mbAddRef          (int NodeID );
	void         mbDelRef          (int NodeID );

	// Funcoes relacionadas com I/O
	bool            acceptIO                (unsigned long  id, eType type);
	void            updateIO                (LadderDiagram *owner, tRequestIO &infoIO, bool isDiscard);
	bool            getIO                   (tRequestIO &infoIO);
	bool            getIO                   (pair<unsigned long, int> &pin, string name, eType type, tRequestIO infoIO);
	bool            getIO                   (vector<tRequestIO> &vectorGetIO);
	unsigned long   getIdIO                 (string name);
	eType           getTypeIO               (string previousName, string newName, eType default_type = eType_Pending, bool isGeneric = false);
	string          getNameIO               (unsigned long id);
	string          getNameIO               (pair<unsigned long, int> pin);
	string          getNameIObyIndex        (unsigned int index);
	mapDetails      getDetailsIO            (unsigned long  id);
	mapDetails      getDetailsIO            (string name);
	char           *getStringTypeIO         (unsigned int   index);
	unsigned int    getCountIO              (void);
	void            selectIO                (unsigned int index);
	bool            IsInternalVarIO         (string name);
	bool            IsGenericTypeIO         (eType type);
	string          getPortNameIO           (int index);
	string          getPinNameIO            (int index);
	string          getInternalVarNameIO    (string name);
	void            ShowIoMapDialog         (int item);
	vector<string>  getVectorInternalFlagsIO(void);
	vector<eType>   getGeneralTypes         (void);
	vector<string>  getListIO               (void);
	string          getReservedNameIO       (eType type);

	// Funcao para ordenar a lista de I/O conforme o campo especificado
	void            sortIO                  (eSortBy sortby);

	// Funcoes que indicam se existe algum I/O que use a UART e o PWM, respectivamente
	bool UartFunctionUsed(void);
	bool PwmFunctionUsed (void);

	// Funcao que busca / renomeia I/O
	int SearchAndReplace(string sSearchText, string sNewText, eSearchAndReplaceMode mode);

	// Funcoes relacionadas aos comandos de Desfazer / Refazer
	void RegisterAction    (UndoRedoAction Action, bool isUndo = true);
	void CheckpointBegin   (string description);
	void CheckpointRollback(void) { CheckpointDoRollback = true; }
	void CheckpointEnd     (void);
	void UndoRedo          (bool isUndo);
	void Undo              (void) { UndoRedo( true); }
	void Redo              (void) { UndoRedo(false); }

	bool equalsNameIO             (string name1, string name2);

	bool IsValidNumber     (string varnumber);
	bool IsValidVarName    (string varname);
	bool IsValidNameAndType(unsigned long id, string name, eType type, const char *FieldName, unsigned int Rules, int MinVal, int MaxVal, eReply *reply = nullptr);
	bool IsValidNameAndType(unsigned long id, string name, eType type, eReply *reply = nullptr);

	eValidateResult Validate(void);

	// Funcao que exibe uma janela de dialogo para o usuario. Dependente de implementacao da interface
	eReply ShowDialog(eDialogType type, bool hasCancel, char *title, char *message, ...);

	// Funcao que executa uma acao de desfazer / refazer
	bool DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action);
};

#endif
