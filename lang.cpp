#include "poptools.h"

#define SET_LANGUAGE_STRINGS(cmd,ptbr,en,es) case cmd: pPTBR = ptbr; pEN = en; pES = es; break;

vector<string> vectorNoMatchFound;

/*** Inicializacao das tabelas com string de todas as linguas suportadas ***/

typedef struct strLangTableChar
{
	strLangTableChar *mapChar[255];
	const char       *out;
} LangTableChar;

LangTableChar *newLangTableChar(void)
{
	LangTableChar *l = new LangTableChar;
	memset(l->mapChar, 0, sizeof(l->mapChar));

	l->out = nullptr;

	return l;
}

void addTextToLangTable(LangTableChar *table, const char *in, const char *out)
{
	if(table == nullptr || in == nullptr || out == nullptr) return;

	unsigned char index = (unsigned char)*in;

	if(index--) { // Decrementa indice pois o caracter zero retorna out...
		if(table->mapChar[index] == nullptr) {
			table->mapChar[index] = newLangTableChar();
		}

		addTextToLangTable(table->mapChar[index], in + 1, out);
	} else {
		if(table->out == nullptr) {
			table->out = out;
		} else {
			Error("Entrada ja existente!!! out atual = '%s', novo out = '%s'", table->out, out);
		}
	}
}

void freeLangTable(LangTableChar *table)
{
	int i;
	for(i = 0; i < 255; i++) {
		if(table->mapChar[i] != nullptr) {
			freeLangTable(table->mapChar[i]);
			delete table->mapChar[i];
		}
	}
}

static LangTableChar *l = nullptr;

void freeLangTables(void)
{
	if(l != nullptr) {
		freeLangTable(l);
		delete l;
	}
}

void setLanguage(unsigned int id)
{
	freeLangTables();

	if(id == 1) { // Idioma Inglês
		l = newLangTableChar();

		addTextToLangTable(l, "( ) Normal", "( ) Normal");
		addTextToLangTable(l, "(/) Negado", "(/) Negated");
		addTextToLangTable(l, "(S) Ativar", "(S) Set-Only");
		addTextToLangTable(l, "(R) Desativar", "(R) Reset-Only");
		addTextToLangTable(l, "Saída Digital", "Digital Output");
		addTextToLangTable(l, "Bobina", "Coil");
		addTextToLangTable(l, "Comentário", "Comment");
		addTextToLangTable(l, "Tempo de Ciclo (ms):", "Cycle Time (ms):");
		addTextToLangTable(l, "Configuração PLC", "PLC Configuration");
		addTextToLangTable(l, "Fonte", "Source");
		addTextToLangTable(l, "Relé Interno", "Internal Relay");
		addTextToLangTable(l, "Entrada Digital", "Input pin");
		addTextToLangTable(l, "Contatos", "Contacts");
		addTextToLangTable(l, "O dispositivo selecionado não possui ADC ou não suporta.", "No ADC or ADC not supported for selected micro.");
		addTextToLangTable(l, "Atribua:", "Assign:");
		addTextToLangTable(l, "Atribuição de Pinos de E/S", "I/O Pin Assignment");
		addTextToLangTable(l, "Somente pode atribuir números dos pinos aos pinos de Entrada/Saída ou Variável de Uso Geral.", "Can only assign pin number to input/output pins or general variable.");
		addTextToLangTable(l, "Renomear as E/S para um nome diferente do padrão ('%s') antes de atribuir um pino.", "Rename I/O from default name ('%s') before assigning MCU pin.");
		addTextToLangTable(l, "Pino E/S", "I/O Pin");
		addTextToLangTable(l, "(sem pino)", "(no pin)");
		addTextToLangTable(l, "<UART necessária!>", "<UART needs!>");
		addTextToLangTable(l, "<PWM necessário!>", "<PWM needs!>");
		addTextToLangTable(l, "<Não é uma E/S!>", "<not an I/O!>");
		addTextToLangTable(l, "Não pode gravar para '%s'.", "Couldn't write to '%s'.");
		addTextToLangTable(l, "Deve selecionar um dispositivo antes de compilar.", "Must choose a target microcontroller before compiling.");
		addTextToLangTable(l, "Função UART é usada porem não suportada por este dispositivo.", "UART function used but not supported for this micro.");
		addTextToLangTable(l, "Função PWM é usada porem não suportada por este dispositivo.", "PWM function used but not supported for this micro.");
		addTextToLangTable(l, "Este programa contém mudanças desde a última vez salva.\r\n\r\n Você quer salvar as mudanças?", "The program has changed since it was last saved.\r\n\r\nDo you want to save the changes?");
		addTextToLangTable(l, "--Adicione Comentários Aqui--", "--add comment here--");
		addTextToLangTable(l, "Não pode abrir '%s'.", "Couldn't open '%s'.");
		addTextToLangTable(l, "Nome", "Name");
		addTextToLangTable(l, "Estado", "State");
		addTextToLangTable(l, "Nro Porta E/S", "Pin on Processor");
		addTextToLangTable(l, "Porta do PLC", "MCU Port");
		addTextToLangTable(l, "POPTools - Simulação (Executando)", "POPTools - Simulation (Running)");
		addTextToLangTable(l, "POPTools - Simulação (Parado)", "POPTools - Simulation (Stopped)");
		addTextToLangTable(l, "POPTools", "POPTools - Program Editor");
		addTextToLangTable(l, " - (ainda não salvo)", " - (not yet saved)");
		addTextToLangTable(l, "Escrita de String Formatada na Serial RS485", "Send Formatted String Over UART");
		addTextToLangTable(l, "Leitura de String Formatada na Serial RS485", "Receive Formatted String Over UART");
		addTextToLangTable(l, "Tipo", "Type");
		addTextToLangTable(l, "Contador", "Counter");
		addTextToLangTable(l, "Reiniciar", "Reset");
		addTextToLangTable(l, "OK", "OK");
		addTextToLangTable(l, "Ok", "Ok");
		addTextToLangTable(l, "Cancelar", "Cancel");
		addTextToLangTable(l, "Texto vazio; não é permitido", "Empty textbox; not permitted.");
		addTextToLangTable(l, "Mau uso das aspas: <%s>", "Bad use of quotes: <%s>");
		addTextToLangTable(l, "Atraso Ligar", "Turn-On Delay");
		addTextToLangTable(l, "Atraso Desligar", "Turn-Off Delay");
		addTextToLangTable(l, "Atraso com memória", "Retentive Turn-On Delay");
		addTextToLangTable(l, "Tempo (ms):", "Delay (ms):");
		addTextToLangTable(l, "Contador Crescente", "Count Up");
		addTextToLangTable(l, "Contador Decrescente", "Count Down");
		addTextToLangTable(l, "Contador Circular", "Circular Counter");
		addTextToLangTable(l, "Valor Max:", "Max value:");
		addTextToLangTable(l, "Verdadeiro se >= :", "True if >= :");
		addTextToLangTable(l, "Verdadeiro se <= :", "True if <= :");
		addTextToLangTable(l, "Se Igual", "If Equals");
		addTextToLangTable(l, "Se Diferente", "If Not Equals");
		addTextToLangTable(l, "Se Maior Que", "If Greater Than");
		addTextToLangTable(l, "Se Maior ou Igual Que", "If Greater Than or Equal To");
		addTextToLangTable(l, "Se Menor Que", "If Less Than");
		addTextToLangTable(l, "Se Menor ou Igual Que", "If Less Than or Equal To");
		addTextToLangTable(l, "Mover Valor", "Move");
		addTextToLangTable(l, "Ler Conversor A/D", "Read A/D Converter");
		addTextToLangTable(l, "% Ciclo Trabalho:", "Duty cycle var:");
		addTextToLangTable(l, "Frequencia (Hz):", "Frequency (Hz):");
		addTextToLangTable(l, "Acionar PWM Duty Cycle", "Set PWM Duty Cycle");
		addTextToLangTable(l, "Origem:", "Source:");
		addTextToLangTable(l, "Recebe da UART", "Receive from UART");
		addTextToLangTable(l, "Envia para UART", "Send to UART");
		addTextToLangTable(l, "Somar", "Add");
		addTextToLangTable(l, "Subtrair", "Subtract");
		addTextToLangTable(l, "Multiplicar", "Multiply");
		addTextToLangTable(l, "Dividir", "Divide");
		addTextToLangTable(l, "Destino:", "Destination:");
		addTextToLangTable(l, "Nome:", "Name:");
		addTextToLangTable(l, "Fases:", "Stages:");
		addTextToLangTable(l, "Fila de Variáveis", "Shift Register");
		addTextToLangTable(l, "String:", "String:");
		addTextToLangTable(l, "String", "String");
		addTextToLangTable(l, "Variavel:", "Variable:");
		addTextToLangTable(l, "Fazer Permanente", "Make Persistent");
		addTextToLangTable(l, "Erro", "Error");
		addTextToLangTable(l, "Deve associar pinos a todas E/S.\r\n\r\n'%s' não está associado.", "Must assign pins for all I/O.\r\n\r\n'%s' is not assigned.");
		addTextToLangTable(l, "Não pode abrir o arquivo '%s'\n", "Couldn't open file '%s'\n");
		addTextToLangTable(l, "Período de Tempo muito curto (necessita de um tempo de ciclo menor).", "Timer period too short (needs faster cycle time).");
		addTextToLangTable(l, "Tempo do Temporizador muito grande(max. 2147483647 tempo de ciclo); use um tempo de ciclo maior.", "Timer period too long (max 2147483647 times cycle time); use a slower cycle time.");
		addTextToLangTable(l, "Constante %d fora do range: -2147483648 a 2147483647 inclusive.", "Constant %d out of range: -2147483648 to 2147483647 inclusive.");
		addTextToLangTable(l, "Instrução Mover: o destino '%s' não é válido.", "Move instruction: '%s' not a valid destination.");
		addTextToLangTable(l, "Instruções Math: o destino '%s' não é válido.", "Math instruction: '%s' not a valid destination.");
		addTextToLangTable(l, "Consulta da Tabela de Linearização por Segmentos sem elementos!", "Piecewise linear lookup table with zero elements!");
		addTextToLangTable(l, "Os valores X na Tabela de Linearização por Segmentos deve ser estritamente incrementais.", "x values in piecewise linear table must be strictly increasing.");
		addTextToLangTable(l, "Linha Vazia; apague ou adicione instruções antes de simular/compilar.", "Empty row; delete it or add instructions before compiling.");
		addTextToLangTable(l, "Índice:", "Index:");
		addTextToLangTable(l, "Pontos:", "Points:");
		addTextToLangTable(l, "Contador:", "Count:");
		addTextToLangTable(l, "Editar tabela do ASCII, valores como uma string", "Edit table of ASCII values like a string");
		addTextToLangTable(l, "Tabela de Busca", "Look-Up Table");
		addTextToLangTable(l, "Tabela de Linearização por Segmentos", "Piecewise Linear Table");
		addTextToLangTable(l, "POPTools Erro", "POPTools Error");
		addTextToLangTable(l, "entrada digital", "digital in");
		addTextToLangTable(l, "saída digital", "digital out");
		addTextToLangTable(l, "rele interno", "int. relay");
		addTextToLangTable(l, "UART tx", "UART tx");
		addTextToLangTable(l, "UART rx", "UART rx");
		addTextToLangTable(l, "saída PWM", "PWM out");
		addTextToLangTable(l, "atraso p/ ativar", "turn-on delay");
		addTextToLangTable(l, "atraso p/ desativar", "turn-off delay");
		addTextToLangTable(l, "atraso com memória", "retentive timer");
		addTextToLangTable(l, "contador", "counter");
		addTextToLangTable(l, "var geral", "general var");
		addTextToLangTable(l, "entrada adc", "adc input");
		addTextToLangTable(l, "(sem atribuição)", "(not assigned)");
		addTextToLangTable(l, "\n\nE/S ATRIBUIDA:\n\n", "\n\nI/O ASSIGNMENT:\n\n");
		addTextToLangTable(l, "  Nome                       | Tipo               | Pino\n", "  Name                       | Type               | Pin\n");
		addTextToLangTable(l, "'%s' deve ser número!", "'%s' must be number!");
		addTextToLangTable(l, "'%s' fora dos limites! Deve estar entre %d e %d.", "'%s' out of range! Must be between %d and %d");
		addTextToLangTable(l, "'%s' não pode ser número!", "'%s' cannot be a number!");
		addTextToLangTable(l, "'%s' deve ser um contador ou timer!", "'%s' must be a counter or a timer!");
		addTextToLangTable(l, "%s '%s' inválido!\n\nVariável: Apenas letras (A a Z), números ou _ (underline) e não inicar com número\nNúmero: Apenas números, podendo iniciar por - (menos)", "%s '%s' invalid!\n\nVariable: only letter (A to Z), numbers or _ (underline) and do not start with numbers\nNumbers: only numbers, can start with - (minus)");
		addTextToLangTable(l, "(GMT-01) Açores, Cabo Verde", "(GMT-01) Açores, Cabo Verde");
		addTextToLangTable(l, "(GMT-02) Atlântico Central", "(GMT-02) Atlântico Central");
		addTextToLangTable(l, "(GMT-03) Brasilia, Buenos Aires", "(GMT-03) Brasilia, Buenos Aires");
		addTextToLangTable(l, "(GMT-04) Rio Branco, Manaus, Caracas, La Paz", "(GMT-04) Rio Branco, Manaus, Caracas, La Paz");
		addTextToLangTable(l, "(GMT-05) Lima, Bogotá", "(GMT-05) Lima, Bogotá");
		addTextToLangTable(l, "(GMT-06) América Central, Hora Central EUA/Canadá", "(GMT-06) América Central, Hora Central EUA/Canadá");
		addTextToLangTable(l, "(GMT-07) Hora das Montanhas (EUA e Canadá)", "(GMT-07) Hora das Montanhas (EUA e Canadá)");
		addTextToLangTable(l, "(GMT-08) Hora do Pacífico", "(GMT-08) Hora do Pacífico");
		addTextToLangTable(l, "(GMT-09) Alasca", "(GMT-09) Alasca");
		addTextToLangTable(l, "(GMT-10) Hawaí", "(GMT-10) Hawaí");
		addTextToLangTable(l, "(GMT-11) Ilhas Midway,Samoa", "(GMT-11) Ilhas Midway,Samoa");
		addTextToLangTable(l, "(GMT-12) Linha de Data Internacional Oeste", "(GMT-12) Linha de Data Internacional Oeste");
		addTextToLangTable(l, "(GMT 00) Hora de Greenwich: Londres, Dublin, Lisboa", "(GMT 00) Hora de Greenwich: Londres, Dublin, Lisboa");
		addTextToLangTable(l, "(GMT+01) Berlim, Estocolmo, Roma, Bruxelas", "(GMT+01) Berlim, Estocolmo, Roma, Bruxelas");
		addTextToLangTable(l, "(GMT+02) Atenas, Helsinque, Leste Europeu, Jerusalém", "(GMT+02) Atenas, Helsinque, Leste Europeu, Jerusalém");
		addTextToLangTable(l, "(GMT+03) Bagdá, Kuwait, Nairóbi, Moscou,Riad", "(GMT+03) Bagdá, Kuwait, Nairóbi, Moscou,Riad");
		addTextToLangTable(l, "(GMT+04) Abu Dhabi, Mascate, Hora Padrão do Cáucaso", "(GMT+04) Abu Dhabi, Mascate, Hora Padrão do Cáucaso");
		addTextToLangTable(l, "(GMT+05) Islamabad, Karachi, Ekaterinburgo", "(GMT+05) Islamabad, Karachi, Ekaterinburgo");
		addTextToLangTable(l, "(GMT+06) Almaty, Dacca", "(GMT+06) Almaty, Dacca");
		addTextToLangTable(l, "(GMT+07) Bangcoc, Jacarta, Hanói", "(GMT+07) Bangcoc, Jacarta, Hanói");
		addTextToLangTable(l, "(GMT+08) Pequim, Hong Kong, Taiwan, Cingapura", "(GMT+08) Pequim, Hong Kong, Taiwan, Cingapura");
		addTextToLangTable(l, "(GMT+09) Tóquio, Osaka, Sapporo, Seul, Yakutsk", "(GMT+09) Tóquio, Osaka, Sapporo, Seul, Yakutsk");
		addTextToLangTable(l, "(GMT+10) Brisbane, Camberra, Melbourne, Sydney", "(GMT+10) Brisbane, Camberra, Melbourne, Sydney");
		addTextToLangTable(l, "(GMT+11) Magadã, Ilhas Salomão, Nova Caledônia", "(GMT+11) Magadã, Ilhas Salomão, Nova Caledônia");
		addTextToLangTable(l, "(GMT+12) Fiji, Kamchatka, Auckland", "(GMT+12) Fiji, Kamchatka, Auckland");
		addTextToLangTable(l, "(GMT+13) Nuku'alofa", "(GMT+13) Nuku'alofa");
		addTextToLangTable(l, "[\x01""BRD\x02 \\_]", "[\x01OSF\x02 \\_]");
		addTextToLangTable(l, "[\x01""BRD\x02_/ ]", "[\x01OSR\x02_/ ]");
		addTextToLangTable(l, "[FIM]", "[END]");
		addTextToLangTable(l, "\nDiagrama Ladder:\n\n", "\nLadderDiagram:\n\n");
		addTextToLangTable(l, "{%s BIT:%d}", "{%s BIT:%d}");
		addTextToLangTable(l, "{%s MOV}", "{%s MOV}");
		addTextToLangTable(l, "{\x01""CTC\x02 0:%d}", "{\x01""CTC\x02 0:%d}");
		addTextToLangTable(l, "{\x01SHIFT REG\x02   }", "{\x01SHIFT REG\x02   }");
		addTextToLangTable(l, "{ACELERACAO}", "{ACCEL}");
		addTextToLangTable(l, "{CHECAR %s:%d}", "{CHECK %s:%d}");
		addTextToLangTable(l, "{DESACELERACAO}", "{DECEL}");
		addTextToLangTable(l, "{CHAVE GERAL}", "{MASTER RLY}");
		addTextToLangTable(l, "{PERSIST}", "{PERSIST}");
		addTextToLangTable(l, "{PWM %.1f kHz}", "{PWM %.1f kHz}");
		addTextToLangTable(l, "{PWM %.2f kHz}", "{PWM %.2f kHz}");
		addTextToLangTable(l, "{PWM %d Hz}", "{PWM %d Hz}");
		addTextToLangTable(l, "{PWM %d kHz}", "{PWM %d kHz}");
		addTextToLangTable(l, "{LER A/D %s }", "{READ ADC %s }");
		addTextToLangTable(l, "{LER ENC}", "{READ ENC}");
		addTextToLangTable(l, "{LER MB 485}", "{READ MB 485}");
		addTextToLangTable(l, "{LER MB ETH}", "{READ MB ETH}");
		addTextToLangTable(l, "{LER USS}", "{READ USS}");
		addTextToLangTable(l, "{RES}", "{RES}");
		addTextToLangTable(l, "{ZERAR ENC}", "{RESET ENC}");
		addTextToLangTable(l, "{SET DA}", "{SET DA}");
		addTextToLangTable(l, "{UART RECEBE}", "{UART RECV}");
		addTextToLangTable(l, "{UART ENVIA}", "{UART SEND}");
		addTextToLangTable(l, "{ESCR MB 485}", "{WRITE MB 485}");
		addTextToLangTable(l, "{ESCR MB ETH}", "{WRITE MB ETH}");
		addTextToLangTable(l, "{ESCR USS}", "{WRITE USS}");
		addTextToLangTable(l, "Índice", "Index");
		addTextToLangTable(l, "0ZSET%d", "0ZSET%d");
		addTextToLangTable(l, "16 bits", "16 bits");
		addTextToLangTable(l, "32 bits", "32 bits");
		addTextToLangTable(l, "7-Par-1", "7-Even-1");
		addTextToLangTable(l, "7-Nenhum-1", "7-None-1");
		addTextToLangTable(l, "7-Ímpar-1", "7-Odd-1");
		addTextToLangTable(l, "8-Par-1", "8-Even-1");
		addTextToLangTable(l, "8-Nenhum-1", "8-None-1");
		addTextToLangTable(l, "8-Ímpar-1", "8-Odd-1");
		addTextToLangTable(l, "A Data/Hora do relógio da POP não foi atualizada.\n\nAplicações que dependem do relogio RTC da placa talvez não funcionem corretamente.", "Date/Time of POP's RTC was not updated.\n\nApplications depending on RTC may not operate correctly.");
		addTextToLangTable(l, "Abril", "April");
		addTextToLangTable(l, "Aceleração", "Acceleration");
		addTextToLangTable(l, "ADD\x02", "ADD\x02");
		addTextToLangTable(l, "Agosto", "August");
		addTextToLangTable(l, "Ao cancelar:", "When cancelling:");
		addTextToLangTable(l, "Arquivo gerado com sucesso!", "File successfully created!");
		addTextToLangTable(l, "Atualizando o relógio RTC da POP...", "Updating POP's RTC clock...");
		addTextToLangTable(l, "Avançar", "Forward");
		addTextToLangTable(l, "Warning", "Aviso");
		addTextToLangTable(l, "Aviso: Este projeto não pode ser aberto porque foi gravado com uma versão mais nova do programa POPTools !", "Warning: This project cannot be opened because was saved using a newer version of POPTools software!");
		addTextToLangTable(l, "Aviso: O arquivo deste projeto é de uma versão anterior e será atualizado para a versão atual quando for gravado !", "Warning: This project was saved using an older version of POPTools software and will be updated for current version when saved");
		addTextToLangTable(l, "Avisos durante a simulação", "Warnings during simulation");
		addTextToLangTable(l, "Bit:", "Bit:");
		addTextToLangTable(l, "char", "char");
		addTextToLangTable(l, "Comando inválido durante a gravação do programa !", "Invalid command while writing program !");
		addTextToLangTable(l, "Comando inválido enviado ao apagar a memória flash !", "Invalid command sent while trying to erase flash memory !");
		addTextToLangTable(l, "Compilando... aguarde !", "Compiling... please wait !");
		addTextToLangTable(l, "Conexão cancelada !", "Connection canceled");
		addTextToLangTable(l, "Configurações", "Configuration");
		addTextToLangTable(l, "Confirmar alteração de Entrada para Relé Interno", "Confirm change from Input to Internal Relay");
		addTextToLangTable(l, "Confirmar alteração de Entrada para Saída", "Confirm change from Input to Output");
		addTextToLangTable(l, "Confirmar alteração de Rele Interno para Saída", "Confirm change from Internal Relay to Output");
		addTextToLangTable(l, "Confirmar alteração de Saída para Rele Interno", "Confirm change from Output to Internal Relay");
		addTextToLangTable(l, "Conflito entre tipos! Operação não permitida.", "Type conflict! Operation not allowed.");
		addTextToLangTable(l, "Conexão encerrada", "Connection closed");
		addTextToLangTable(l, "Não foi possível carregar imagem de abertura!", "Could not load splash image!");
		addTextToLangTable(l, "CTD", "CTD");
		addTextToLangTable(l, "CTU", "CTU");
		addTextToLangTable(l, "Curva", "Curve");
		addTextToLangTable(l, "Curva de Ganho", "Gain Curve");
		addTextToLangTable(l, "Data inválida.", "Invalid date");
		addTextToLangTable(l, "Data/Paridade/Stop:", "Data/Parity/Stop:");
		addTextToLangTable(l, "Desaceleração", "Deceleration");
		addTextToLangTable(l, "Desligado", "Turned off");
		addTextToLangTable(l, "Desligar", "Turn off");
		addTextToLangTable(l, "dest", "dest");
		addTextToLangTable(l, "Destino", "Destination");
		addTextToLangTable(l, "Dezembro", "December");
		addTextToLangTable(l, "Dia da Semana", "Day of Week");
		addTextToLangTable(l, "Dia do Mês", "Day Month");
		addTextToLangTable(l, "Perímetro Roda:", "Wheel Perimeter:");
		addTextToLangTable(l, "Dias", "Days");
		addTextToLangTable(l, "Direção do Movimento", "Moviment Direction");
		addTextToLangTable(l, "DIV\x02", "DIV\x02");
		addTextToLangTable(l, "Divisão por zero na variavel: '%s', a simulação será interrompida.", "Divided by zero on variable: '%s', simulation will be interrupted.");
		addTextToLangTable(l, "DNS", "DNS");
		addTextToLangTable(l, "DNS:", "DNS:");
		addTextToLangTable(l, "DSTQQSS", "SMTWTFS");
		addTextToLangTable(l, "duty_cycle", "duty_cycle");
		addTextToLangTable(l, "Endereço IP:", "IP Address:");
		addTextToLangTable(l, "Endereço:", "Address");
		addTextToLangTable(l, "entrada encoder", "encoder input");
		addTextToLangTable(l, "Enviado comando inválido (conectando) !", "Invalid command sent (connecting) !");
		addTextToLangTable(l, "Enviado comando inválido (verificando) !", "Invalid command sent (verifying) !");
		addTextToLangTable(l, "ERRO", "ERROR");
		addTextToLangTable(l, "Erro abrindo porta serial!", "Error opening serial port!");
		addTextToLangTable(l, "Erro ao abrir o arquivo de programa (.hex) !", "Error opening program file (.hex) !");
		addTextToLangTable(l, "Erro ao conectar na porta COM%d !", "Connection error on port COM%d !");
		addTextToLangTable(l, "Erro ao gerar checksum durante a gravação do programa !", "Error creating checksum while writing program !");
		addTextToLangTable(l, "Erro ao preencher a memória flash !", "Error filling flash memory !");
		addTextToLangTable(l, "Erro de checksum ao gravar o programa !", "Checksum error while writing program !");
		addTextToLangTable(l, "Erro de checksum no arquivo de programa (.hex) !", "Checksum error in program file (.hex) !");
		addTextToLangTable(l, "Erro durante a gravação do programa !", "Error while writing program !");
		addTextToLangTable(l, "Erro na compilacao !!!", "Compile error !!!");
		addTextToLangTable(l, "Erro na execução do módulo JIT !", "Error while running JIT Module");
		addTextToLangTable(l, "Erro na verificação !", "Verify error !");
		addTextToLangTable(l, "Escrever", "Write");
		addTextToLangTable(l, "escrita modbus", "modbus write");
		addTextToLangTable(l, "escrita modbus eth", "eth modbus write");
		addTextToLangTable(l, "escrita NS600", "NS600 write");
		addTextToLangTable(l, "Estágios", "Stages");
		addTextToLangTable(l, "Executar", "Execute");
		addTextToLangTable(l, "Fator Correção:", "Correction factor:");
		addTextToLangTable(l, "Fator Multiplicação:", "Multiplication Factor:");
		addTextToLangTable(l, "Fevereiro", "February");
		addTextToLangTable(l, "Flag Interna", "Internal Flag");
		addTextToLangTable(l, "flag interna", "internal flag");
		addTextToLangTable(l, "Frequency", "Frequência");
		addTextToLangTable(l, "Função", "function");
		addTextToLangTable(l, "Fuso Horário:", "Timezone");
		addTextToLangTable(l, "Gateway:", "Gateway:");
		addTextToLangTable(l, "Gravando... %d%% concluído !", "Writing... %d%% done !");
		addTextToLangTable(l, "Gravação concluída com sucesso", "Write successfully done");
		addTextToLangTable(l, "Gravação concluída com sucesso !", "Write successfully done !");
		addTextToLangTable(l, "Gravação do programa cancelada !", "Program write canceled !");
		addTextToLangTable(l, "GW", "GW");
		addTextToLangTable(l, "Hora invalida, deve estar entre 0 e 23.", "Invalid hour, should be between 0 and 23");
		addTextToLangTable(l, "Horário de verão", "Daylight saving");
		addTextToLangTable(l, "ID", "ID");
		addTextToLangTable(l, "ID deve ser maior ou igual a zero!", "ID should be equal or bigger than zero");
		addTextToLangTable(l, "ID:", "ID:");
		addTextToLangTable(l, "Impossível criar paralelo entre os pontos selecionados!", "Impossible to create a parallel between selected points");
		addTextToLangTable(l, "entrada", "in");
		addTextToLangTable(l, "indice", "index");
		addTextToLangTable(l, "Interface", "Interface");
		addTextToLangTable(l, "IP", "IP");
		addTextToLangTable(l, "IP:", "IP:");
		addTextToLangTable(l, "Janeiro", "January");
		addTextToLangTable(l, "Já existe um Rele Interno com este nome. Alterar para Saída?", "There is an Internal Relay with the same name. Change for Output?");
		addTextToLangTable(l, "Já existe uma Entrada com este nome. Alterar para Relé Interno?", "There is an Input with the same name. Change for Internal Relay?");
		addTextToLangTable(l, "Já existe uma Entrada com este nome. Alterar para Saída?", "There is an Input with the same name. Change for Output?");
		addTextToLangTable(l, "Já existe uma Saída com este nome. Alterar para Rele Interno?", "There is an Output with the same name. Change for Internal Relay?");
		addTextToLangTable(l, "Julho", "July");
		addTextToLangTable(l, "Junho", "June");
		addTextToLangTable(l, "leitura modbus", "modbus read");
		addTextToLangTable(l, "leitura modbus eth", "modbus eth read");
		addTextToLangTable(l, "leitura NS600", "NS600 read");
		addTextToLangTable(l, "Ler", "Read");
		addTextToLangTable(l, "Ler Parâmetro por USS", "Read Parameter by USS");
		addTextToLangTable(l, "Escrever Parâmetro por USS", "Write Parameter by USS");
		addTextToLangTable(l, "Ligado", "Turned On");
		addTextToLangTable(l, "Ligar", "Turn On");
		addTextToLangTable(l, "Limpar Registro", "Clear Data");
		addTextToLangTable(l, "Limpar!", "Clear!");
		addTextToLangTable(l, "Limpeza da memória flash cancelada !", "Flash memory blanking canceled !");
		addTextToLangTable(l, "Limpeza da memória flash não suportado !", "Flash memory blanking not supported !");
		addTextToLangTable(l, "Linear", "Linear");
		addTextToLangTable(l, "Localizar", "Find");
		addTextToLangTable(l, "Localizar e Substituir", "Find and Replace");
		addTextToLangTable(l, "Localizar:", "Find:");
		addTextToLangTable(l, "LUT", "LUT");
		addTextToLangTable(l, "Maio", "May");
		addTextToLangTable(l, "Março", "March");
		addTextToLangTable(l, "Máscara:", "Mask:");
		addTextToLangTable(l, "MASK", "MASK");
		addTextToLangTable(l, "Minuto inválido, deve estar entre 0 e 59.", "Minute invalid, should be between 0 and 59");
		addTextToLangTable(l, "Modbus", "Modbus");
		addTextToLangTable(l, "ModBUS ID:", "ModBUS ID:");
		addTextToLangTable(l, "MODBUS: id=%d, address=%d, name=%s\r\n", "MODBUS: id=%d, address=%d, name=%s\r\n");
		addTextToLangTable(l, "MODBUS_ETH: id=%d, address=%d, name=%s\r\n", "MODBUS_ETH: id=%d, address=%d, name=%s\r\n");
		addTextToLangTable(l, "Modelo do CLP:", "CLP Model:");
		addTextToLangTable(l, "Write Mode", "Modo de Escrita");
		addTextToLangTable(l, "MUL\x02", "MUL\x02");
		addTextToLangTable(l, "Número '%s' inválido!", "Number '%s' invalid!");
		addTextToLangTable(l, "Número Pulsos:", "Pulse Count:");
		addTextToLangTable(l, "Nao encontrado!", "Not Found!");
		addTextToLangTable(l, "novo", "new");
		addTextToLangTable(l, "Sem encoder ou Encoder não suportado para o dispositivo selecionado.", "No Encoder or Encoder not supported for selected micro.");
		addTextToLangTable(l, "Nome '%s' reservado para uso interno, favor escolher outro nome.", "Name '%s' reserved for internal use, please choose another name.");
		addTextToLangTable(l, "Nome Inválido! Para o tipo 'Flag Interna' é obrigatório selecionar um item da lista.", "Name invalid! For 'Internal Flag' type is mandatory to select an item from list");
		addTextToLangTable(l, "Não foi possível abrir o arquivo .hex ao apagar a memória flash !", "Couldn't to open .hex file while erasing flash memory");
		addTextToLangTable(l, "Não foi possível alocar memória durante a gravação do programa !", "Couldn't to allocate memory while writing program");
		addTextToLangTable(l, "Não foi possível conectar com baudrate %i !", "Couldn't to connect with baudrate %i !");
		addTextToLangTable(l, "Não foi possível conectar para apagar a memória flash !", "Couldn't to connect for erase flash memory !");
		addTextToLangTable(l, "Couldn't to connect for write flash memory", "Não foi possível conectar para gravar o programa !");
		addTextToLangTable(l, "Não suporta gravação do programa !", "Program write not supported !");
		addTextToLangTable(l, "Não suporta verificação !", "Verification not supported !");
		addTextToLangTable(l, "Novembro", "November");
		addTextToLangTable(l, "Novo Valor", "New value");
		addTextToLangTable(l, "O valor máximo permitido para o campo Resolução (%%) da Curva de Ganho é %s.", "Maximum allowed value for field Resolution (%%) of Gain Curve is %s.");
		addTextToLangTable(l, "O valor máximo permitido para o campo Tempo (%%) da Curva de Ganho é %d.", "Maximum allowed value for field Time (%%) of Gain Curve is %s.");
		addTextToLangTable(l, "O valor permitido para o campo Resolução DA no Tamanho da Rampa está entre %d e %d.", "Allowed value for field DA Resolution of Ramp Size is between %d and %d.");
		addTextToLangTable(l, "O valor permitido para o campo Tempo (ms) no Tamanho da Rampa está entre %d e %d.", "Allowed value for field Time (ms) of Ramp Size is between %d and %d.");
		addTextToLangTable(l, "Ocorreu um erro ao desenhar o gráfico do D/A.", "An unknown error ocurred when drawing graphic D/A.");
		addTextToLangTable(l, "Ocorreu um erro desconhecido ao apagar a memória flash !", "An unknown error ocurred when erasing flash memory !");
		addTextToLangTable(l, "Ocorreu um erro desconhecido ao conectar !", "An unknown error ocurred when connecting !");
		addTextToLangTable(l, "Ocorreu um erro desconhecido ao desconectar !", "An unknown error ocurred when disconnecting !");
		addTextToLangTable(l, "Ocorreu um erro desconhecido ao gravar o programa !", "An unknown error ocurred when writing program !");
		addTextToLangTable(l, "Ocorreu um erro desconhecido ao verificar a gravação do programa !", "An unknown error ocurred when verifying program !");
		addTextToLangTable(l, "OFF", "OFF");
		addTextToLangTable(l, "ON", "ON");
		addTextToLangTable(l, "Operação", "Operation");
		addTextToLangTable(l, "Operação bit", "Bit Operation");
		addTextToLangTable(l, "Origem", "Source");
		addTextToLangTable(l, "saida", "out");
		addTextToLangTable(l, "Outubro", "October");
		addTextToLangTable(l, "Parâmetro:", "Parameter:");
		addTextToLangTable(l, "Set de Parâmetro:", "Parameter Set:");
		addTextToLangTable(l, "Parâmetros de conexão inválidos !", "Invalid connection parameters !");
		addTextToLangTable(l, "Parâmetros inválidos ao apagar a memória flash !", "Invalid parameters while erasing flash memory !");
		addTextToLangTable(l, "Parâmetro", "Parameter");
		addTextToLangTable(l, "POP-7 não encontrada!", "POP-7 not found!");
		addTextToLangTable(l, "POPTools iniciado", "POPTools started");
		addTextToLangTable(l, "Porta de Depuração:", "Debug Port:");
		addTextToLangTable(l, "Porta de Gravação:", "Write Port:");
		addTextToLangTable(l, "Posicionamento", "Positioning");
		addTextToLangTable(l, "Preferências", "Preferences");
		addTextToLangTable(l, "Preparando para gravar... %d%% concluído !", "Preparing for write... %d%% done !");
		addTextToLangTable(l, "Preparação concluída com sucesso !", "Preparing done succesfully !");
		addTextToLangTable(l, "Projetos Recentes:", "Recent Projects:");
		addTextToLangTable(l, "PWL", "PWL");
		addTextToLangTable(l, "RAMPA", "RAMP");
		addTextToLangTable(l, "Rampa de Aceleração/Desaceleração", "Acceleration/Deceleration Ramp");
		addTextToLangTable(l, "LER", "READ");
		addTextToLangTable(l, "Ler do Encoder", "Read Encoder");
		addTextToLangTable(l, "Escrever no Encoder", "Write Encoder");
		addTextToLangTable(l, "ler USS", "read USS");
		addTextToLangTable(l, "Recuar", "Backward");
		addTextToLangTable(l, "Rede", "Network");
		addTextToLangTable(l, "reg", "reg");
		addTextToLangTable(l, "Registrador", "Register");
		addTextToLangTable(l, "Registrador:", "Register:");
		addTextToLangTable(l, "Registro", "Data");
		addTextToLangTable(l, "Relógio da POP", "POP Clock");
		addTextToLangTable(l, "Resolução DA (%):", "DA Resolution (%):");
		addTextToLangTable(l, "Resolução DA (%)[-100 ~ 100]", "D/A Resolution (%)[-100 ~ 100]");
		addTextToLangTable(l, "Resolução DA (12 bits)[-2048 ~ 2047]", "D/A Resolution (12 bits)[-2048 ~ 2047]");
		addTextToLangTable(l, "Resolução DA (mV)[-10000 ~ 10000]", "D/A Resolution (mV)[-10000 ~ 10000]");
		addTextToLangTable(l, "Resposta", "Reply");
		addTextToLangTable(l, "Retransmitir", "Retransmit");
		addTextToLangTable(l, "Retransmitir Pacote", "Retransmit Packet");
		addTextToLangTable(l, "RS485 Baud Rate (bps):", "RS485 Baud Rate (bps):");
		addTextToLangTable(l, "RST", "RST");
		addTextToLangTable(l, "RTC", "RTC");
		addTextToLangTable(l, "RTO", "RTO");
		addTextToLangTable(l, "RX NS600", "RX NS600");
		addTextToLangTable(l, "Sair", "Exit");
		addTextToLangTable(l, "Salvar como linguagem C", "Save as C Language");
		addTextToLangTable(l, "Saída em uso! Não é possível alterar para Entrada.", "Output in Use! Cannot change to Input");
		addTextToLangTable(l, "Segundo inválido, deve estar entre 0 e 59.", "Second invalid, should be between 0 and 59.");
		addTextToLangTable(l, "Serial", "Serial");
		addTextToLangTable(l, "Servidor de Tempo:", "Time server:");
		addTextToLangTable(l, "Servo Yaskawa", "Servo Yaskawa");
		addTextToLangTable(l, "SET", "SET");
		addTextToLangTable(l, "Set Bit:", "Set Bit");
		addTextToLangTable(l, "Set D/A", "Set D/A");
		addTextToLangTable(l, "Set de Parâmetro", "Parameter Set:");
		addTextToLangTable(l, "Setembro", "September");
		addTextToLangTable(l, "Sobre...", "About...");
		addTextToLangTable(l, "src", "src");
		addTextToLangTable(l, "SUB\x02", "SUB\x02");
		addTextToLangTable(l, "Replace", "Substituir");
		addTextToLangTable(l, "Substituir por", "Replace by");
		addTextToLangTable(l, "Substituir por:", "Replace by:");
		addTextToLangTable(l, "Substituir Todos", "Reaplce All");
		addTextToLangTable(l, "Sucesso", "Success");
		addTextToLangTable(l, "Tamanho da Rampa", "Ramp Size");
		addTextToLangTable(l, "Tempo", "Time");
		addTextToLangTable(l, "Tempo (%):", "Time (%):");
		addTextToLangTable(l, "Tempo:", "Time:");
		addTextToLangTable(l, "Tipo de arquivo deve ser .hex\nA extensão será alterada automaticamente.", "File type must be .hex\nThe extension will be changed automatically.");
		addTextToLangTable(l, "Tipo de Rampa", "Ramp Type");
		addTextToLangTable(l, "Todos", "All");
		addTextToLangTable(l, "TOF", "TOF");
		addTextToLangTable(l, "TON", "TON");
		addTextToLangTable(l, "TX NS600", "TX NS600");
		addTextToLangTable(l, "Incapaz de conectar ao servidor!", "Unable to connect to server!");
		addTextToLangTable(l, "USS: id=%d, param=%d, index=%d, name=%s, value=%d\r\n", "USS: id=%d, param=%d, index=%d, name=%s, value=%d\r\n");
		addTextToLangTable(l, "Valor", "Value");
		addTextToLangTable(l, "Valor inválido no campo Resolução (%) !", "Invalid value in field Resolution (%) !");
		addTextToLangTable(l, "Valor inválido no campo Resolução DA !", "Invalid value in field D/A Resolution !");
		addTextToLangTable(l, "Valor inválido no campo Tempo (%) !", "Invalid value in field Time (%) !");
		addTextToLangTable(l, "Valor inválido no campo Tempo (ms) !", "Invalid value in field Time (ms) !");
		addTextToLangTable(l, "Conflito de tipos: Entrada <-> Saída", "Invalid value! Conflict between types: Input <-> Output");
		addTextToLangTable(l, "Conflito de tipos: Rele Interno <-> Entrada", "Invalid value! Conflict between types: Internal Relay <-> Input");
		addTextToLangTable(l, "Conflito de tipos: Rele Interno <-> Saída", "Invalid value! Conflict between types: Internal Relay <-> Output");
		addTextToLangTable(l, "Valor:", "Value:");
		addTextToLangTable(l, "valor: %d\\r\\n", "value: %d\\r\\n");
		addTextToLangTable(l, "var", "var");
		addTextToLangTable(l, "Variação Velocidade", "Speed Variation");
		addTextToLangTable(l, "Variável:", "Variable:");
		addTextToLangTable(l, "Variável", "Variable");
		addTextToLangTable(l, "Verificando... %d%% concluído !", "Verifying... %d%% done !");
		addTextToLangTable(l, "Verifying done successfully !", "Verificação concluída com sucesso !");
		addTextToLangTable(l, "ESCREVER", "WRITE");
		addTextToLangTable(l, "escrever USS", "write USS");
		addTextToLangTable(l, "x 2", "x 2");
		addTextToLangTable(l, "x 4", "x 4");
		addTextToLangTable(l, "xvar", "xvar");
		addTextToLangTable(l, "yvar", "yvar");
		addTextToLangTable(l, "Operador 1:", "Operator 1:");
		addTextToLangTable(l, "Operador 2:", "Operator 2:");
		addTextToLangTable(l, "Raiz Quadrada", "Square Root");
		addTextToLangTable(l, "Depurar", "Debug");
		addTextToLangTable(l, "Padrão", "Default");
		addTextToLangTable(l, "Manter", "Leave");
		addTextToLangTable(l, "Parar", "Stop");
		addTextToLangTable(l, "Desacelerar", "Decelerate");
		addTextToLangTable(l, "Data Específica", "Mode Fixed");
		addTextToLangTable(l, "{\x01 Fila de Variáveis\x02}", "{\x01SHIFT REG\x02   }");
		addTextToLangTable(l, "Escrever Registrador do ModBUS", "Write ModBUS");
		addTextToLangTable(l, "Ler Registrador do ModBUS", "Read ModBUS");
		addTextToLangTable(l, "Escrever no Servo Yaskawa", "Write Servo Yaskawa");
		addTextToLangTable(l, "Ler do Servo Yaskawa", "Read Servo Yaskawa");
		addTextToLangTable(l, "Progresso", "Progress");
		addTextToLangTable(l, "Agendador de Tarefa", "Scheduler");
		addTextToLangTable(l, "Sim", "Yes");
		addTextToLangTable(l, "Não", "No");
		addTextToLangTable(l, "IP: %d.%d.%d.%d", "IP: %d.%d.%d.%d");
		addTextToLangTable(l, "ModBUS ID: %d", "ModBUS ID: %d");
		addTextToLangTable(l, "RS-485: %d bps, %d bits de dados, %s, Bits de Parada: %d", "RS-485: %d bps, %d data bits, %s, Stop Bits: %d");
		addTextToLangTable(l, "Ir Para:", "Go To:");
		addTextToLangTable(l, "Preencha abaixo os dados referentes ao projeto para sua referência.", "Enter below the data related to the project for your reference.");
		addTextToLangTable(l, "Nome do Projeto:", "Project Name:");
		addTextToLangTable(l, "Autor do Projeto:", "Project Author:");
		addTextToLangTable(l, "Observações:", "Remarks:");
		addTextToLangTable(l, "Abaixo são exibidas informações sobre o projeto.", "Information about the project are shown below.");
		addTextToLangTable(l, "Versão do Firmware:", "Firmware Version:");
		addTextToLangTable(l, "Número da Compilação:", "Build Number:");
		addTextToLangTable(l, "Data de Compilação:", "Build date:");
		addTextToLangTable(l, "Data de Gravação:", "Flash date:");
		addTextToLangTable(l, "Ajuste a configuração da rede conforme a sua necessidade", "Set the network configuration according to your needs");
		addTextToLangTable(l, "Ajuste o fuso horário e indique se o horário de verão está ativado", "Set the time zone and indicate whether daylight saving is enabled");
		addTextToLangTable(l, "Ativar sincronização", "Enable syncing");
		addTextToLangTable(l, "Ajuste aqui a configuração da interface RS-485 da POP-7", "Set here the configuration of the RS-485 interface of POP-7");
		addTextToLangTable(l, "Sem Paridade", "No Parity");
		addTextToLangTable(l, "Paridade Ímpar", "Odd Parity");
		addTextToLangTable(l, "Paridade Par", "Even Parity");
		addTextToLangTable(l, "Inserir Linha", "Insert Rung");
		addTextToLangTable(l, "Idioma atual:", "Current Language:");
		addTextToLangTable(l, "Idioma da Interface", "Interface Language");
		addTextToLangTable(l, "Intervalo:", "Interval:");
		addTextToLangTable(l, "Cópia de Segurança", "Backup copy");
		addTextToLangTable(l, "Portas Seriais", "Serial Ports");
		addTextToLangTable(l, "RS485 Baud Rate(bps):", "RS485 Baud Rate(bps):");
		addTextToLangTable(l, "Aqui indicamos o tipo de escala utilizada ao ler o canal de A/D do sensor de temperatura.", "Here we set the type of scale used to read A/D channel: temperature sensor.");
		addTextToLangTable(l, "Escala:", "Scale:");
		addTextToLangTable(l, "Celsius", "Celsius");
		addTextToLangTable(l, "Fahrenheit", "Fahrenheit");
		addTextToLangTable(l, "Este Modo de Abandono será utilizado em todas as rampas de D/A selecionadas para o modo Padrão.", "This Abandon mode will be used on all D/A ramps selected for the Default mode.");
		addTextToLangTable(l, "Modo de Abandono:", "Abandon Mode:");
		addTextToLangTable(l, "Modo de Conversão:", "Conversion Mode:");
		addTextToLangTable(l, "Modo de Leitura:", "Read Mode:");
		addTextToLangTable(l, "Resolução total em bits:", "Total resolution in bits:");
		addTextToLangTable(l, "Sendo bits por volta:", "Bits per revolution:");
		addTextToLangTable(l, "Elemento:", "Element:");
		addTextToLangTable(l, "Interface:", "Interface:");
		addTextToLangTable(l, "ID do ModBUS:", "ModBUS ID:");
		addTextToLangTable(l, "Adicionar", "Add");
		addTextToLangTable(l, "Excluir", "Delete");
		addTextToLangTable(l, "Dados do Elemento Selecionado", "Selected Element Data");
		addTextToLangTable(l, "Preencha aqui o ID utilizado pela interface ModBUS quando atuando em modo escravo.\r\nPreencha com valor entre 0 e 127", "Fill in the ID when operating ModBUS interface as Slave.\r\nFill in with values between 0 and 127");
		addTextToLangTable(l, "Indefinido", "Undefined");
		addTextToLangTable(l, "Alterar Configurações", "Change Configuration");
		addTextToLangTable(l, "Remover Linha", "Delete Rung");
		addTextToLangTable(l, "Modo", "Mode");
		addTextToLangTable(l, "Período Contínuo", "Continuous Period");
		addTextToLangTable(l, "Período Intermitente", "Intermittent Period");
		addTextToLangTable(l, "De", "Fro");
		addTextToLangTable(l, "Até", "To");
		addTextToLangTable(l, "Horário Inicial:", "Initial Time:");
		addTextToLangTable(l, "Horário Final:", "Final Time:");
		addTextToLangTable(l, "Alterado Elemento", "Element Changed");
		addTextToLangTable(l, "RS-485", "RS-485");
		addTextToLangTable(l, "Ethernet", "Ethernet");
		addTextToLangTable(l, "Sem conversão", "No conversion");
		addTextToLangTable(l, "Metros", "Meters");
		addTextToLangTable(l, "Milímetros", "Milimeters");
		addTextToLangTable(l, "Décimos de milímetro", "Tenths of a millimeter");
		addTextToLangTable(l, "Leitura Gray", "Gray Code");
		addTextToLangTable(l, "Leitura Binário", "Binary Code");
		addTextToLangTable(l, "Idioma alterado", "Language changed");
		addTextToLangTable(l, "O idioma da interface foi alterado.\nFavor reiniciar o programa para que as mudanças tenham efeito", "Interface language has been changed.\nChanges will take effect the next time you execute POPTools.");
		addTextToLangTable(l, "Desativado", "Disabled");
		addTextToLangTable(l, "1 Minuto", "1 Minute");
		addTextToLangTable(l, "5 Minutos", "5 Minutes");
		addTextToLangTable(l, "10 Minutos", "10 Minutes");
		addTextToLangTable(l, "15 Minutos", "15 Minutes");
		addTextToLangTable(l, "30 Minutos", "30 Minutes");
		addTextToLangTable(l, "1 Hora", "1 Hour");
		addTextToLangTable(l, "Editar Elemento", "Edit element");
		addTextToLangTable(l, "Duplo clique para editar:", "Double-click to edit:");
		addTextToLangTable(l, "Inverter Contato", "Reverse Contact");
		addTextToLangTable(l, "CONTATO", "CONTACT");
		addTextToLangTable(l, "Negado", "Negated");
		addTextToLangTable(l, "Solicitar I/O", "Request I/O");
		addTextToLangTable(l, "Adicionar I/O", "Add I/O");
		addTextToLangTable(l, "reservado", "reserved");
		addTextToLangTable(l, "Descartar I/O", "Discard I/O");
		addTextToLangTable(l, "Atualizar I/O", "Update I/O");
		addTextToLangTable(l, "SOMAR", "ADD");
		addTextToLangTable(l, "SUBTRAIR", "SUBTRACT");
		addTextToLangTable(l, "MULTIPLICAR", "MULTIPLY");
		addTextToLangTable(l, "DIVIDIR", "DIVIDE");
		addTextToLangTable(l, "RESTO", "REMAINDER");
		addTextToLangTable(l, "BRD", "BRD");
		addTextToLangTable(l, "ABERTO", "OPEN");
		addTextToLangTable(l, "FECHADO", "CLOSED");
		addTextToLangTable(l, "BOBINA", "COIL");
		addTextToLangTable(l, "CHAVE MESTRE", "MASTER RELAY");
		addTextToLangTable(l, "ATRASO", "DELAY");
		addTextToLangTable(l, "CONTADOR", "COUNTER");
		addTextToLangTable(l, "ESCREVER BIT", "WRITE BIT");
		addTextToLangTable(l, "%s bit %d", "%s bit %d");
		addTextToLangTable(l, "CHECAR BIT", "CHECK BIT");
		addTextToLangTable(l, "bit %d %s?", "bit %d %s?");
		addTextToLangTable(l, "Max:", "Max:");
		addTextToLangTable(l, "COMPARAR", "COMPARE");
		addTextToLangTable(l, "LISTA", "LIST");
		addTextToLangTable(l, "TABELA DE BUSCA", "LOOKUP TABLE");
		addTextToLangTable(l, "LINEARIZACAO", "PWL");
		addTextToLangTable(l, "ALEATORIO", "RANDOM");
		addTextToLangTable(l, "LER A/D", "A/D READ");
		addTextToLangTable(l, "ESCREVER D/A", "D/A WRITE");
		addTextToLangTable(l, "RAMPA D/A", "D/A RAMP");
		addTextToLangTable(l, "PWM", "PWM");
		addTextToLangTable(l, "LER ENCODER", "ENCODER READ");
		addTextToLangTable(l, "ESCR. ENCODER", "ENCODER WRITE");
		addTextToLangTable(l, "LER MB", "MB READ");
		addTextToLangTable(l, "ESCREVER MB", "MB WRITE");
		addTextToLangTable(l, "LER STRING", "STRING READ");
		addTextToLangTable(l, "ESCR. STRING", "STRING WRITE");
		addTextToLangTable(l, "LER SERIAL", "UART READ");
		addTextToLangTable(l, "ESCR. SERIAL", "UART WRITE");
		addTextToLangTable(l, "LER USS", "USS READ");
		addTextToLangTable(l, "ESCREVER USS", "USS WRITE");
		addTextToLangTable(l, "LER NS-600", "NS-600 READ");
		addTextToLangTable(l, "ESCREVER NS-600", "NS-600 WRITE");
		addTextToLangTable(l, "Enc. Inc.", "Inc. Enc.");
		addTextToLangTable(l, "Enc. Abs.", "Abs. Enc.");
		addTextToLangTable(l, "Associar I/O", "Assign I/O");
		addTextToLangTable(l, "Checar Bit", "Check Bit");
		addTextToLangTable(l, "Ligar/Desligar Bit", "Set Bit");
		addTextToLangTable(l, "Resto da Divisão", "Modulo");
		addTextToLangTable(l, "Absoluto", "Absolute");
		addTextToLangTable(l, "ABSOLUTO", "ABSOLUTE");
		addTextToLangTable(l, "Mínimo:", "Minimum:");
		addTextToLangTable(l, "Mínimo", "Minimum");
		addTextToLangTable(l, "Máximo", "Maximum");
		addTextToLangTable(l, "Máximo:", "Maximum:");
		addTextToLangTable(l, "Randômico", "Random");
		addTextToLangTable(l, "Coincidir", "Coincide");
		addTextToLangTable(l, "Resolução DA:", "D/A Resolution:");
		addTextToLangTable(l, "(12 bits)[2047 ~ -2048]", "(12 bits)[2047 ~ -2048]");
		addTextToLangTable(l, "(mV)[10000 ~ -10000]", "(mV)[10000 ~ -10000]");
		addTextToLangTable(l, "(%)[0 ~ 100]", "(%)[0 ~ 100]");
		addTextToLangTable(l, "Configuração PWM", "PWM Configuration");
		addTextToLangTable(l, "Interromper se =", "Break if =");
		addTextToLangTable(l, "Variável '%s' não é lida, apenas escrita!", "Variable '%s' is not read, just write!");
		addTextToLangTable(l, "Variável '%s' não é escrita, apenas lida!", "Variable '%s' is not write, just read!");
		addTextToLangTable(l, "Erro ao criar servidor ModBUS. Código do Erro: %d\nNão será possível receber conexões externas nessa sessão!", "Error while creating ModBUS server. Error code: %d\nWill not be possible to receive external connections on this session!");
		addTextToLangTable(l, "Texto: %s", "Text: %s");
		addTextToLangTable(l, "Início: %s", "Start: %s");
		addTextToLangTable(l, "Fim: %s", "End: %s");
		addTextToLangTable(l, "Fim linha", "End Line");
		addTextToLangTable(l, "CR", "CR");
		addTextToLangTable(l, "LF", "LF");
		addTextToLangTable(l, "CR+LF", "CR+LF");
		addTextToLangTable(l, "Reenviar", "Resend");
		addTextToLangTable(l, "Nome '%s' utilizado também em outros elementos.\nAlterar o nome em todos os elementos que o utilizam?", "Name '%s' also used in other elements.\nChange the name of all elements that use it?");
		addTextToLangTable(l, "Nome Alterado", "Name Changed");
		addTextToLangTable(l, "escrever encoder", "write encoder");
		addTextToLangTable(l, "Entrada", "Input");
		addTextToLangTable(l, "Saída", "Output");
		addTextToLangTable(l, "Rele Interno", "Internal Relay");
		addTextToLangTable(l, "Mult.", "Mult.");
		addTextToLangTable(l, "x2", "x2");
		addTextToLangTable(l, "x4", "x4");
		addTextToLangTable(l, "Eixo X: %s", "X Axis: %s");
		addTextToLangTable(l, "Eixo Y: %s", "Y Axis: %s");
		addTextToLangTable(l, "Registradores:", "Registers:");
		addTextToLangTable(l, "BORDA DE SUBIDA", "ONE-SHOT RISING");
		addTextToLangTable(l, "BORDA DE DESCIDA", "ONE-SHOT FALLING");
		addTextToLangTable(l, "PID", "PID");
		addTextToLangTable(l, "Setpoint (SP):", "Setpoint (SP):");
		addTextToLangTable(l, "Ganho P:", "Gain P:");
		addTextToLangTable(l, "Ganho I:", "Gain I:");
		addTextToLangTable(l, "Ganho D:", "Gain D:");
		addTextToLangTable(l, "Output (OP):", "Output (OP):");
		addTextToLangTable(l, "Process Value(PV):", "Process Value(PV):");
		addTextToLangTable(l, "SP", "SP");
		addTextToLangTable(l, "P", "P");
		addTextToLangTable(l, "I", "I");
		addTextToLangTable(l, "D", "D");
		addTextToLangTable(l, "OP", "OP");
		addTextToLangTable(l, "PV", "PV");
		addTextToLangTable(l, "Nome Inválido", "Invalid Name");
		addTextToLangTable(l, "Atenção", "Warning");
		addTextToLangTable(l, "Variáveis persistentes devem ser usadas cautelosamente. Excesso no uso pode interferir no desempenho da execução do diagrama ladder e reduzir a vida útil do CLP.\nA memória interna possui um limite no número de gravações.", "Persistent variables should be used carefully. Excess use can interfere in the ladder logic performance and reduce the life of the PLC.\nThe internal memory has a limit of writes before to be damaged");
		addTextToLangTable(l, "Atualizar Data/Hora", "Update Date/Time");
		addTextToLangTable(l, "Informações da POP-7 (Lido a partir da interface USB)", "POP-7 Information (Read from USB Interface");
		addTextToLangTable(l, "Data:", "Date:");
		addTextToLangTable(l, "--/--/----", "--/--/----");
		addTextToLangTable(l, "Hora:", "Time:");
		addTextToLangTable(l, "--:--:--", "--:--:--");
		addTextToLangTable(l, "Entradas:", "Inputs:");
		addTextToLangTable(l, "Saídas:", "Outputs:");
		addTextToLangTable(l, "Fabricante:", "Manufacturer:");
		addTextToLangTable(l, "Produto:", "Product:");
		addTextToLangTable(l, "Versão:", "Version:");
		addTextToLangTable(l, "Projeto:", "Project:");
		addTextToLangTable(l, "Compilando :", "Compiling :");
		addTextToLangTable(l, "Compilando...", "Compiling...");
		addTextToLangTable(l, "Variável '%s' reservada! Favor alterar para um nome válido", "Variable '%s' reserved! Please change to a valid name");
		addTextToLangTable(l, "Cuidado ao utilizar mais de uma instrução PWM em sua lógica.\nSe duas instruções PWM forem ativadas ao mesmo tempo, o PWM não funcionará corretamente.", "Be careful when using more than one PWM instruction in ladder logic.\nIf two or more of them are activated at the same time, PWM will not work properly.");
		addTextToLangTable(l, "Erro durante Compilação! Código de erro: %d", "Error while compiling! Error code: %d");
		addTextToLangTable(l, "Erro ao validar diagrama!", "Error when validating diagram!");
		addTextToLangTable(l, "Log indisponível", "Log unavailable");
		addTextToLangTable(l, "Adicionar Condicional Igual a", "Add Conditional Equal To");
		addTextToLangTable(l, "Adicionar Condicional Diferente de", "Add Conditional Different of");
		addTextToLangTable(l, "Adicionar Condicional Maior que", "Add Conditional Greater than");
		addTextToLangTable(l, "Adicionar Condicional Maior ou igual a", "Add Conditional Greater or Equal To");
		addTextToLangTable(l, "Adicionar Condicional Menor que", "Add Conditional Lesser than");
		addTextToLangTable(l, "Adicionar Condicional Menor ou igual a", "Add Conditional Lesser or Equal To");
		addTextToLangTable(l, "Adicionar Condicional", "Add Conditional");
		addTextToLangTable(l, "Adicionar Comentário", "Add Comment");
		addTextToLangTable(l, "Adicionar Contato", "Add Contact");
		addTextToLangTable(l, "Adicionar Bobina", "Add Coil");
		addTextToLangTable(l, "Adicionar Atraso para Ligar", "Add Turn-On Delay");
		addTextToLangTable(l, "Adicionar Atraso para Desligar", "Add Turn-Off Delay");
		addTextToLangTable(l, "Adicionar Atraso Retentivo para Ligar", "Add Retentive Turn-On Delay");
		addTextToLangTable(l, "Adicionar Temporizador", "Add Timer");
		addTextToLangTable(l, "Adicionar Agendador de Tarefas", "Add Scheduler");
		addTextToLangTable(l, "Adicionar Circuito Aberto", "Add Open Circuit");
		addTextToLangTable(l, "Adicionar Circuito Fechado", "Add Closed Circuit");
		addTextToLangTable(l, "Adicionar Borda de Subida", "Add One-Shot Rising");
		addTextToLangTable(l, "Adicionar Borda de Descida", "Add One-Shot Falling");
		addTextToLangTable(l, "Adicionar Zerar Temporizador/Contador", "Add Reset Timer/Counter");
		addTextToLangTable(l, "Adicionado Relé Mestre", "Add Master Relay");
		addTextToLangTable(l, "Adicionar Liga/Desliga Bit", "Add Turn On/Turn Off Bit");
		addTextToLangTable(l, "Adicionar Checar Bit", "Add Check Bit");
		addTextToLangTable(l, "Adicionar Fila de Variáveis", "Add Shift Register");
		addTextToLangTable(l, "Adicionar Tabela de Busca", "Add LookUp Table");
		addTextToLangTable(l, "Adicionar Linearização por Segmentos", "Add Piecewise Linear");
		addTextToLangTable(l, "Adicionar Mover Valor", "Add Move Value");
		addTextToLangTable(l, "Adicionar Raiz Quadrado", "Add Square Root");
		addTextToLangTable(l, "Adicionar Operação Somar", "Add Math Add Operation");
		addTextToLangTable(l, "Adicionar Operação Subtrair", "Add Math Subtract Operation");
		addTextToLangTable(l, "Adicionar Operação Multiplicar", "Add Math Multiply Operation");
		addTextToLangTable(l, "Adicionar Operação Dividir", "Add Math Divide Operation");
		addTextToLangTable(l, "Adicionar Operação Matemática", "Add Math Operation");
		addTextToLangTable(l, "Adicionar Número Randômico", "Add Random Number");
		addTextToLangTable(l, "Adicionar Valor Absoluto", "Add Absolut Number");
		addTextToLangTable(l, "Adicionar Contador Incremental", "Add Incremental Counter");
		addTextToLangTable(l, "Adicionar Contador Decremental", "Add Decremental Counter");
		addTextToLangTable(l, "Adicionar Contador Circular", "Add Circular Counter");
		addTextToLangTable(l, "Adicionar Contador", "Add Counter");
		addTextToLangTable(l, "Adicionar Escrita no D/A", "Add D/A Write");
		addTextToLangTable(l, "Adicionar Leitura do A/D", "Add A/D Read");
		addTextToLangTable(l, "Adicionar Leitura do Encoder", "Add Encoder Read");
		addTextToLangTable(l, "Adicionar Escrita no Encoder", "Add Encoder Write");
		addTextToLangTable(l, "Adicionar Rampa de Aceleração/Desaceleração", "Add Acceleration/Deceleration Ramp");
		addTextToLangTable(l, "Adicionar Ler String Formatada", "Add Formatted String Read");
		addTextToLangTable(l, "Adicionar Escrever String Formatada", "Add Formatted String Write");
		addTextToLangTable(l, "Adicionar Leitura de NS-600 Yaskawa", "Add NS-600 Read");
		addTextToLangTable(l, "Adicionar Escrita de NS-600 Yaskawa", "Add NS-600 Write");
		addTextToLangTable(l, "Adicionar Leitura USS", "Add USS Read");
		addTextToLangTable(l, "Adicionar Escrita USS", "Add USS Write");
		addTextToLangTable(l, "Adicionar Ler Registrador ModBUS", "Add ModBUS Register Read");
		addTextToLangTable(l, "Adicionar Escrever Registrador ModBUS", "Add ModBUS Register Write");
		addTextToLangTable(l, "Adicionar Saída PWM", "Add Set PWM");
		addTextToLangTable(l, "Adicionar Leitura de Caracter da RS-485", "Add UART Read");
		addTextToLangTable(l, "Adicionar Escrita de Caracter na RS-485", "Add UART Write");
		addTextToLangTable(l, "Adicionar Elemento", "Add Element");
		addTextToLangTable(l, "Adicionar Persistente", "Add Persistent");
		addTextToLangTable(l, "Adicionar PID", "Add PID");
		addTextToLangTable(l, "iniciando linha %d", "start rung %d");
		addTextToLangTable(l, "iniciando serie [", "start series [");
		addTextToLangTable(l, "iniciando paralelo [", "start parallel [");
		addTextToLangTable(l, "terminando serie [", "finish series [");
		addTextToLangTable(l, "terminando paralelo [", "finish parallel [");
		addTextToLangTable(l, "Tem certeza que deseja sair da simulação?", "Are you sure you want to end the simulation?");
		addTextToLangTable(l, "O processo será interrompido", "The process will be interrupted");
		addTextToLangTable(l, "Sempre mostrar avisos da simulação", "Always show simulation warnings");
		addTextToLangTable(l, "Encontrada(s) %d ocorrência(s)", "Found %d matches");
		addTextToLangTable(l, "Já existe um elemento com esse nome!", "There is already an element with this name!");
		addTextToLangTable(l, "Este elemento está em uso!", "Element in use!");
		addTextToLangTable(l, "Primeiro remova sua referência de todas as instruções que o utilizam.", "First remove references from all instructions that use it");
		addTextToLangTable(l, "Tem certeza que deseja excluir o item selecionado?", "Are you sure you want to delete the selected item?");
		addTextToLangTable(l, "Erro ao carregar as preferências", "Error while loading saved preferences");
		addTextToLangTable(l, "Será utilizada a configuração padrão", "The default preferences will be used instead");
		addTextToLangTable(l, "Informações", "Information");
		addTextToLangTable(l, "Projeto", "Project");
		addTextToLangTable(l, "Detalhes", "Details");
		addTextToLangTable(l, "Comunicação", "Communication");
		addTextToLangTable(l, "Fuso Horário", "Timezone");
		addTextToLangTable(l, "Interfaces", "Interfaces");
		addTextToLangTable(l, "Entrada Analógica", "A/D Input");
		addTextToLangTable(l, "Saída Analógica", "D/A Output");
		addTextToLangTable(l, "Encoder Incremental", "Incremental Encoder");
		addTextToLangTable(l, "Encoder Absoluto", "Absolute Encoder");
		addTextToLangTable(l, "ModBUS", "ModBUS");
		addTextToLangTable(l, "Mestre", "Master");
		addTextToLangTable(l, "Escravo", "Slave");
		addTextToLangTable(l, "Projetos POPTools (*.ld)", "POPTools Projects (*.ld)");
		addTextToLangTable(l, "Arquivos de Texto (*.txt)", "Text Files (*.txt)");
		addTextToLangTable(l, "Arquivos de Linguagem C (*.c)", "C Language Files (*.c)");
		addTextToLangTable(l, "Arquivos CSV (*.csv)", "CSV Files (*.csv)");
		addTextToLangTable(l, "Todos os Arquivos (*.*)", "All Files (*.*)");
		addTextToLangTable(l, "Mais informações", "More Information");
		addTextToLangTable(l, "Menos informações", "Less Information");
		addTextToLangTable(l, "ESRF", "IORF");
	} else if(id == 2) { // Idioma Espanhol
		l = newLangTableChar();

		addTextToLangTable(l, "Target frequency %d Hz, closest achievable is %d Hz (warning, >5%% error).", "Frecuencia Micro %d Hz, la mejor aproximación es %d Hz (aviso, >5%% error).");
		addTextToLangTable(l, "Compile successful; wrote IHEX for AVR to '%s'.\r\n\r\nRemember to set the processor configuration (fuses) correctly. This does not happen automatically.", "Compilación correcta; se escribió IHEX para AVR en '%s'.\r\n\r\nRecuerde marcar la configuración (fuses) del micro correctamente. Esto NO se hace automaticamente.");
		addTextToLangTable(l, "( ) Normal", "( ) Normal");
		addTextToLangTable(l, "(/) Negated", "(/) Negado");
		addTextToLangTable(l, "(S) Set-Only", "(S) Activar");
		addTextToLangTable(l, "(R) Reset-Only", "(R) Desactivar");
		addTextToLangTable(l, "Pin on MCU", "Pata del Micro");
		addTextToLangTable(l, "Coil", "Bobina");
		addTextToLangTable(l, "Comment", "Comentario");
		addTextToLangTable(l, "Cycle Time (ms):", "Tiempo Ciclo (ms):");
		addTextToLangTable(l, "Crystal Frequency (MHz):", "Frecuencia Cristal (MHz):");
		addTextToLangTable(l, "UART Baud Rate (bps):", "Baudios UART (bps):");
		addTextToLangTable(l, "Serie (UART) will use pins %d and %d.\r\n\r\n", "Puerto Serie (UART) usará las patas %d y %d.\r\n\r\n");
		addTextToLangTable(l, "Please select a micro with a UART.\r\n\r\n", "Por favor. Seleccione un micro con UART.\r\n\r\n");
		addTextToLangTable(l, "No serial instructions (UART Send/UART Receive) are in use; add one to program before setting baud rate.\r\n\r\n", "No se han usado instrucciones (UART Enviar/UART Recibir) para el puerto serie aun; Añada una al programa antes de configurar los baudios.\r\n\r\n");
		addTextToLangTable(l, "The cycle time for the 'PLC' runtime generated by LDmicro is user-configurable. Very short cycle times may not be achievable due to processor speed constraints, and very long cycle times may not be achievable due to hardware overflows. Cycle times between 10 ms and 100 ms will usually be practical.\r\n\r\nThe compiler must know what speed crystal you are using with the micro to convert between timing in clock cycles and timing in seconds. A 4 MHz to 20 MHz crystal is typical; check the speed grade of the part you are using to determine the maximum allowable clock speed before choosing a crystal.", "El tiempo de ciclo de ejecución para el 'PLC' es configurable. Un tiempo de ciclo muy corto puede no funcionar debido a la baja velocidad del micro, y un tiempo de ciclo muy largo puede no funcionar por limitaciones del temporizador del micro. Ciclos de tiempo entre 10 y 100 ms suele ser lo normal.\r\n\r\nEl compilador debe conocer la velocidad del cristal que estas usando para poder convertir entre tiempo en ciclos de reloj y tiempo en segundos. Un cristal entre 4 Mhz y 20 Mhz es lo típico; Comprueba la velocidad a la que puede funcionar tu micro y calcula la velocidad máxima del reloj antes de elegir el cristal.");
		addTextToLangTable(l, "PLC Configuration", "Configuración PLC");
		addTextToLangTable(l, "Zero cycle time not valid; resetting to 10 ms.", "No es valido un tiempo de ciclo 0; forzado a 10 ms.");
		addTextToLangTable(l, "Source", "Fuente");
		addTextToLangTable(l, "Internal Relay", "Rele Interno");
		addTextToLangTable(l, "Input pin", "Pata Entrada");
		addTextToLangTable(l, "Output pin", "Pata Salida");
		addTextToLangTable(l, "|/| Negated", "|/| Negado");
		addTextToLangTable(l, "Contacts", "Contacto");
		addTextToLangTable(l, "No ADC or ADC not supported for selected micro.", "El micro seleccionado no tiene ADC o no esta soportado.");
		addTextToLangTable(l, "Assign:", "Asignar:");
		addTextToLangTable(l, "No microcontroller has been selected. You must select a microcontroller before you can assign I/O pins.\r\n\r\nSelect a microcontroller under the Settings menu and try again.", "No se ha seleccionado micro. Debes seleccionar un micro antes de asignar patas E/S.\r\n\r\nElije un micro en el menu de configuración y prueba otra vez.");
		addTextToLangTable(l, "I/O Pin Assignment", "Asignación de pata E/S");
		addTextToLangTable(l, "Can't specify I/O assignment for ANSI C target; compile and see comments in generated source code.", "No se puede asignar la E/S especificadas para el ANSI C generado; compile y vea los comentarios generados en el código fuente.");
		addTextToLangTable(l, "Can't specify I/O assignment for interpretable target; see comments in reference implementation of interpreter.", "No se puede asignar la E/S especificadas para el código generado para el interprete; vea los comentarios en la implementación del interprete.");
		addTextToLangTable(l, "Can only assign pin number to input/output pins or general variable.", "Solo puede asignar numero de pata a las patas de Entrada/Salida.");
		addTextToLangTable(l, "No ADC or ADC not supported for this micro.", "Este micro no tiene ADC o no esta soportado.");
		addTextToLangTable(l, "Renomear as E/S para um nome diferente do padrão ('%s') antes de atribuir um pino.", "Cambie el nombre por defecto ('%s') antes de asignarle una pata del micro.");
		addTextToLangTable(l, "Pino E/S", "E/S Pata");
		addTextToLangTable(l, "(sem pino)", "(falta pata)");
		addTextToLangTable(l, "<UART necessária!>", "<Se necesita UART!>");
		addTextToLangTable(l, "<PWM necessário!>", "<Se necesita PWM!>");
		addTextToLangTable(l, "<Não é uma E/S!>", "<No es una E/S!>");
		addTextToLangTable(l, "Export As Text", "Exportar como Texto");
		addTextToLangTable(l, "Não pode gravar para '%s'.", "No puedo escribir en '%s'.");
		addTextToLangTable(l, "Compile To", "Compilar");
		addTextToLangTable(l, "Deve selecionar um dispositivo antes de compilar.", "Debe elegir un micro antes de compilar.");
		addTextToLangTable(l, "UART function used but not supported for this micro.", "Usadas Funciones para UART. Este micro no las soporta.");
		addTextToLangTable(l, "PWM function used but not supported for this micro.", "Usadas Funciones para PWM. Este micro no las soporta.");
		addTextToLangTable(l, "The program has changed since it was last saved.\r\n\r\nDo you want to save the changes?", "El programa ha cambiado desde la última vez que los guardo.\r\n\r\n¿Quieres guardar los cambios?");
		addTextToLangTable(l, "--add comment here--", "--añade el comentario aquí--");
		addTextToLangTable(l, "Start new program?", "¿Empezar un nuevo programa?");
		addTextToLangTable(l, "Couldn't open '%s'.", "No puedo abrir '%s'.");
		addTextToLangTable(l, "Name", "Nombre");
		addTextToLangTable(l, "State", "Estado");
		addTextToLangTable(l, "Pin on Processor", "Pata del Micro");
		addTextToLangTable(l, "MCU Port", "Puerto del Micro");
		addTextToLangTable(l, "POPTools - Simulation (Running)", "POPTools - Simulación (Ejecutando)");
		addTextToLangTable(l, "POPTools - Simulation (Stopped)", "POPTools - Simulación (Parada)");
		addTextToLangTable(l, "POPTools - Program Editor", "POPTools – Editor de Programa");
		addTextToLangTable(l, " - (not yet saved)", " - (no guardado aún)");
		addTextToLangTable(l, "&New\tCtrl+N", "&Nuevo\tCtrl+N");
		addTextToLangTable(l, "&Open...\tCtrl+O", "&Abrir...\tCtrl+O");
		addTextToLangTable(l, "&Save\tCtrl+S", "&Guardar\tCtrl+S");
		addTextToLangTable(l, "Save &As...", "Guardar &Como...");
		addTextToLangTable(l, "&Export As Text...\tCtrl+E", "&Exportar a Texto...\tCtrl+E");
		addTextToLangTable(l, "E&xit", "&Salir");
		addTextToLangTable(l, "&Undo\tCtrl+Z", "&Deshacer\tCtrl+Z");
		addTextToLangTable(l, "&Redo\tCtrl+Y", "&Rehacer\tCtrl+Y");
		addTextToLangTable(l, "Insert Rung &Before\tShift+INS", "Insertar Línea (Rung) &Antes\tShift+INS");
		addTextToLangTable(l, "Insert Rung &After\tINS", "Insertar Línea (Rung) &Despues\tINS");
		addTextToLangTable(l, "Move Selected Rung &Up\tShift+Up", "Subir Línea (Rung) Seleccionada\tShift+Up");
		addTextToLangTable(l, "Move Selected Rung &Down\tShift+Down", "Bajar Línea (Rung) Seleccionada\tShift+Down");
		addTextToLangTable(l, "&Delete Selected Element\tDel", "&Borrar Elemento Seleccionado\tSupr");
		addTextToLangTable(l, "D&elete Rung\tShift+Del", "B&orrar Línea (Rung) Seleccionada\tShift+Supr");
		addTextToLangTable(l, "Insert Co&mment\tF2", "Insertar Co&mentario\tF2");
		addTextToLangTable(l, "Insert &Contacts\tC", "Insertar &Contacto\tC");
		addTextToLangTable(l, "Insert OSR (One Shot Rising)\t/", "Insertar OSR (Flanco de Subida)\t/");
		addTextToLangTable(l, "Insert OSF (One Shot Falling)\t\\", "Insertar OSF (Flanco de Bajada)\t\\");
		addTextToLangTable(l, "Insert T&ON (Delayed Turn On)\tO", "Insertar T&ON (Encendido Retardado)\tO");
		addTextToLangTable(l, "Insert TO&F (Delayed Turn Off)\tF", "Insertar TO&F (Apagado Retardado)\tF");
		addTextToLangTable(l, "Insert R&TO (Retentive Delayed Turn On)\tT", "Insertar R&TO (Encendido Retardado con Memoria)\tT");
		addTextToLangTable(l, "Insert CT&U (Count Up)\tI", "Insertar CT&U (Contador Incremental)\tU");
		addTextToLangTable(l, "Insert CT&D (Count Down)\tU", "Insertar CT&D (Contador Decremental)\tI");
		addTextToLangTable(l, "Insert CT&C (Count Circular)\tJ", "Insertar CT&C (Contador Circular)\tJ");
		addTextToLangTable(l, "Insert EQU (Compare for Equals)\t=", "Insertar EQU (Comparador si Igual)\t=");
		addTextToLangTable(l, "Insert NEQ (Compare for Not Equals)\t!", "Insertar NEQ (Comparador si NO Igual)\t!");
		addTextToLangTable(l, "Insert GRT (Compare for Greater Than)\t>", "Insertar GRT (Comparador si Mayor que)\t>");
		addTextToLangTable(l, "Insert GEQ (Compare for Greater Than or Equal)\t.", "Insertar GEQ (Comparador si Mayor o Igual que)\t.");
		addTextToLangTable(l, "Insert LES (Compare for Less Than)\t<", "Insertar LES (Comparador si Menor que)\t<");
		addTextToLangTable(l, "Insert LEQ (Compare for Less Than or Equal)\t,", "Insertar LEQ (Comparador si Menor o Igual que)\t,");
		addTextToLangTable(l, "Insert Open-Circuit", "Insertar Circuito-Abierto");
		addTextToLangTable(l, "Insert Short-Circuit", "Insertar Circuito-Cerrado");
		addTextToLangTable(l, "Insert Master Control Relay", "Insertar Rele de Control Maestro");
		addTextToLangTable(l, "Insert Coi&l\tL", "Insertar &Bobina\tL");
		addTextToLangTable(l, "Insert R&ES (Counter/RTO Reset)\tE", "Insertar R&ES (Contador/RTO Reinicio)\tE");
		addTextToLangTable(l, "Insert MOV (Move)\tM", "Insertar MOV (Mover)\tM");
		addTextToLangTable(l, "Insert ADD (16-bit Integer Add)\t+", "Insertar ADD (Suma Entero 16-bit)\t+");
		addTextToLangTable(l, "Insert SUB (16-bit Integer Subtract)\t-", "Insertar SUB (Resta Entero 16-bit)\t-");
		addTextToLangTable(l, "Insert MUL (16-bit Integer Multiply)\t*", "Insertar MUL (Multiplica Entero 16-bit)\t*");
		addTextToLangTable(l, "Insert DIV (16-bit Integer Divide)\tD", "Insertar DIV (Divide Entero 16-bit)\tD");
		addTextToLangTable(l, "Insert Shift Register", "Insertar Registro de Desplazamiento");
		addTextToLangTable(l, "Insert Look-Up Table", "Insertar Tabla de Busqueda");
		addTextToLangTable(l, "Insert Piecewise Linear", "Insertar Linealización por Segmentos");
		addTextToLangTable(l, "Insert Formatted String Over UART", "Insertar Cadena Formateada en la UART");
		addTextToLangTable(l, "Insert &UART Send", "Insertar &UART Enviar");
		addTextToLangTable(l, "Insert &UART Receive", "Insertar &UART Recibir");
		addTextToLangTable(l, "Insert Set PWM Output", "Insertar Valor Salida PWM");
		addTextToLangTable(l, "Insert A/D Converter Read\tP", "Insertar Lectura Conversor A/D\tP");
		addTextToLangTable(l, "Insert Make Persistent", "Insertar Hacer Permanente");
		addTextToLangTable(l, "Make Norm&al\tA", "Hacer Norm&al\tA");
		addTextToLangTable(l, "Make &Negated\tN", "Hacer &Negado\tN");
		addTextToLangTable(l, "Make &Set-Only\tS", "Hacer &Solo-Activar\tS");
		addTextToLangTable(l, "Make &Reset-Only\tR", "Hace&r Solo-Desactivar\tR");
		addTextToLangTable(l, "&MCU Parameters...", "&Parametros del Micro...");
		addTextToLangTable(l, "(no microcontroller)", "(no microcontrolador)");
		addTextToLangTable(l, "&Microcontroller", "&Microcontrolador");
		addTextToLangTable(l, "Si&mulation Mode\tCtrl+M", "Modo Si&mulación \tCtrl+M");
		addTextToLangTable(l, "Start &Real-Time Simulation\tCtrl+R", "Empezar Simulación en Tiempo &Real\tCtrl+R");
		addTextToLangTable(l, "&Halt Simulation\tCtrl+H", "Parar Simulación\tCtrl+H");
		addTextToLangTable(l, "Single &Cycle\tSpace", "Solo un &Ciclo\tSpace");
		addTextToLangTable(l, "&Compile\tF5", "&Compilar\tF5");
		addTextToLangTable(l, "Compile &As...", "Compilar &Como...");
		addTextToLangTable(l, "&Manual...\tF1", "&Manual...\tF1");
		addTextToLangTable(l, "&About...", "&Acerca de...");
		addTextToLangTable(l, "&File", "&Archivo");
		addTextToLangTable(l, "&Edit", "&Editar");
		addTextToLangTable(l, "&Settings", "&Configuraciones");
		addTextToLangTable(l, "&Instruction", "&Instrucción");
		addTextToLangTable(l, "Si&mulate", "Si&mular");
		addTextToLangTable(l, "&Compile", "&Compilar");
		addTextToLangTable(l, "&Help", "&Ayuda");
		addTextToLangTable(l, "no MCU selected", "micro no seleccionado");
		addTextToLangTable(l, "cycle time %.2f ms", "tiempo ciclo %.2f ms");
		addTextToLangTable(l, "processor clock %.4f MHz", "reloj procesador %.4f MHz");
		addTextToLangTable(l, "Internal error relating to PIC paging; make program smaller or reshuffle it.", "Error interno relativo a la paginación del PIC; Haz el programa mas pequeño o reorganizalo");
		addTextToLangTable(l, "PWM frequency too fast.", "Frecuencia del PWM demasiado alta.");
		addTextToLangTable(l, "PWM frequency too slow.", "Frecuencia del PWM demasiado baja.");
		addTextToLangTable(l, "Cycle time too fast; increase cycle time, or use faster crystal.", "Tiempo del Ciclo demasiado rapido; aumenta el tiempo de ciclo, o usa un cristal de mas Mhz.");
		addTextToLangTable(l, "Cycle time too slow; decrease cycle time, or use slower crystal.", "Tiempo del Ciclo demasiado lento; incrementa el tiempo de ciclo, o usa un cristal de menos Mhz.");
		addTextToLangTable(l, "Couldn't open file '%s'", "No puedo abrir el archivo '%s'");
		addTextToLangTable(l, "Zero baud rate not possible.", "Cero baudios no es posible.");
		addTextToLangTable(l, "Compile successful; wrote IHEX for PIC16 to '%s'.\r\n\r\nConfiguration word (fuses) has been set for crystal oscillator, BOD enabled, LVP disabled, PWRT enabled, all code protection off.\r\n\r\nUsed %d/%d words of program flash (chip %d%% full).", "Compilación correcta; escrito IHEX para PIC16 en '%s'.\r\n\r\nBits de Configurarión (fuses) han sido establecidos para oscilador a cristal, BOD activado, LVP desactivado, PWRT activado, Todos los bits de protección desactivados.\r\n\r\nUsadas %d/%d palabras de programa en flash (Chip %d%% lleno).");
		addTextToLangTable(l, "Type", "Tipo");
		addTextToLangTable(l, "Timer", "Temporizador");
		addTextToLangTable(l, "Counter", "Contador");
		addTextToLangTable(l, "Reset", "Reiniciar");
		addTextToLangTable(l, "OK", "OK");
		addTextToLangTable(l, "Cancel", "Cancelar");
		addTextToLangTable(l, "Empty textbox; not permitted.", "Texto vacio; no permitido");
		addTextToLangTable(l, "Bad use of quotes: <%s>", "Mal uso de las comillas: <%s>");
		addTextToLangTable(l, "Turn-On Delay", "Activar Retardado");
		addTextToLangTable(l, "Turn-Off Delay", "Desactivar Retardado");
		addTextToLangTable(l, "Retentive Turn-On Delay", "Activar Retardado con Memoria");
		addTextToLangTable(l, "Delay (ms):", "Retardo (ms):");
		addTextToLangTable(l, "Delay too long; maximum is 2**31 us.", "Retardo demasiado largo; maximo 2**31 us.");
		addTextToLangTable(l, "Delay cannot be zero or negative.", "El retardo no puede ser cero o negativo.");
		addTextToLangTable(l, "Count Up", "Contador Creciente");
		addTextToLangTable(l, "Count Down", "Contador Decreciente");
		addTextToLangTable(l, "Circular Counter", "Contador Circular");
		addTextToLangTable(l, "Max value:", "Valor Max:");
		addTextToLangTable(l, "True if >= :", "Verdad si >= :");
		addTextToLangTable(l, "True if <= :", "Verdad si <= :");
		addTextToLangTable(l, "If Equals", "Si igual");
		addTextToLangTable(l, "If Not Equals", "Si NO igual");
		addTextToLangTable(l, "If Greater Than", "Si mayor que");
		addTextToLangTable(l, "If Greater Than or Equal To", "Si mayor o igual que");
		addTextToLangTable(l, "If Less Than", "Si menor que");
		addTextToLangTable(l, "If Less Than or Equal To", "Si menor o igual que");
		addTextToLangTable(l, "'Closed' if:", "'Cerrado' si:");
		addTextToLangTable(l, "Move", "Mover");
		addTextToLangTable(l, "Read A/D Converter", "Ler Conversor A/D");
		addTextToLangTable(l, "Duty cycle var:", "Var Ancho Ciclo:");
		addTextToLangTable(l, "Frequency (Hz):", "Frecuencia (Hz):");
		addTextToLangTable(l, "Set PWM Duty Cycle", "Poner Ancho de Pulso PWM");
		addTextToLangTable(l, "Source:", "Fuente:");
		addTextToLangTable(l, "Receive from UART", "Recibido en la UART");
		addTextToLangTable(l, "Send to UART", "Enviado a la UART");
		addTextToLangTable(l, "Add", "Sumar");
		addTextToLangTable(l, "Subtract", "Restar");
		addTextToLangTable(l, "Multiply", "Multiplicar");
		addTextToLangTable(l, "Divide", "Dividir");
		addTextToLangTable(l, "Destination:", "Destino:");
		addTextToLangTable(l, "is set := :", "esta puesto := :");
		addTextToLangTable(l, "Name:", "Nombre:");
		addTextToLangTable(l, "Stages:", "Fases:");
		addTextToLangTable(l, "Shift Register", "Registro Desplazamiento");
		addTextToLangTable(l, "Not a reasonable size for a shift register.", "No es un tamaño razonable para el Registro de Desplazamiento.");
		addTextToLangTable(l, "String:", "Cadena:");
		addTextToLangTable(l, "Formatted String Over UART", "Cadena Formateada para UART");
		addTextToLangTable(l, "Variable:", "Variable:");
		addTextToLangTable(l, "Make Persistent", "Hacer permanente");
		addTextToLangTable(l, "Too many elements in subcircuit!", "Demasiados elementos en un SubCircuito!");
		addTextToLangTable(l, "Too many rungs!", "Demasiadas Lineas (rungs)!");
		addTextToLangTable(l, "Error", "Error");
		addTextToLangTable(l, "ANSI C target does not support peripherals (UART, PWM, ADC, EEPROM). Skipping that instruction.", "ANSI C de destino no soporta perifericos (UART, PWM, ADC, EEPROM). Evite esa instrucción.");
		addTextToLangTable(l, "Compile successful; wrote C source code to '%s'.\r\n\r\nThis is not a complete C program. You have to provide the runtime and all the I/O routines. See the comments in the source code for information about how to do this.", "Compilación correcta: Escrito Código Fuente en C en '%s'.\r\n\r\nNo es un programa completo en C. Tiene que añadirle el procedimiento principal y todas las rutinas de E/S. Vea los comentarios en el código fuente para mas información sobre como hacer esto");
		addTextToLangTable(l, "Cannot delete rung; program must have at least one rung.", "No puedo borrar la Linea (rung); el programa debe tener al menos una Linea (rung).");
		addTextToLangTable(l, "Out of memory; simplify program or choose microcontroller with more memory.", "Fuera de Memoria; Simplifique el programa o elija un micro con mas memoria..");
		addTextToLangTable(l, "Must assign pins for all ADC inputs (name '%s').", "Debe asignar patas para todas las entradas del ADC (nombre '%s').");
		addTextToLangTable(l, "Internal limit exceeded (number of vars)", "Limite interno superado (numero de variables)");
		addTextToLangTable(l, "Internal relay '%s' never assigned; add its coil somewhere.", "No ha asignado el rele interno '%s'; añada la bobina en cualquier parte del programa.");
		addTextToLangTable(l, "Must assign pins for all I/O.\r\n\r\n'%s' is not assigned.", "Debe asignar patas a todas las E/S.\r\n\r\n'%s' no esta asignada.");
		addTextToLangTable(l, "UART in use; pins %d and %d reserved for that.", "UART en uso; patas %d y %d reservadas para eso.");
		addTextToLangTable(l, "PWM in use; pin %d reserved for that.", "PWM en uso; pata %d reservada para eso.");
		addTextToLangTable(l, "UART baud rate generator: divisor=%d actual=%.4f for %.2f%% error.\r\n\r\nThis is too large; try a different baud rate (slower probably), or a crystal frequency chosen to be divisible by many common baud rates (e.g. 3.6864 MHz, 14.7456 MHz).\r\n\r\nCode will be generated anyways but serial may be unreliable or completely broken.", "UART generador de baudios: divisor=%d actual=%.4f para %.2f%% error.\r\n\r\nEs demasiado grande; Prueba con otro valor de baudios (probablemente menor), o un cristal cuya frecuencia sea divible por los baudios mas comunes (p.e. 3.6864MHz, 14.7456MHz).\r\n\r\nEl código se genera de todas formas pero las tramas serie sean inestable o no entendible.");
		addTextToLangTable(l, "UART baud rate generator: too slow, divisor overflows. Use a slower crystal or a faster baud rate.\r\n\r\nCode will be generated anyways but serial will likely be completely broken.", "UART generador de baudios: demasiado lento, divisor demasiado grande. Use un cristal mas lento o mayor baudios.\r\n\r\nEl código se genera de todas formas pero las tramas serie serán no entendible..");
		addTextToLangTable(l, "Couldn't open '%s'\n", "No puedo abrir '%s'\n");
		addTextToLangTable(l, "Timer period too short (needs faster cycle time).", "Periodo de Tiempo demasiado corto (se necesita un tiempo de ciclo menor).");
		addTextToLangTable(l, "Timer period too long (max 32767 times cycle time); use a slower cycle time.", "Periodo del temporizador demasiado largo (max. 32767 veces el tiempo de ciclo); use un tiempo de ciclo mayor.");
		addTextToLangTable(l, "Constant %d out of range: -32768 to 32767 inclusive.", "Constante %d fuera de rango: -32768 a 32767 inclusive.");
		addTextToLangTable(l, "Move instruction: '%s' not a valid destination.", "Instrucción Move: '%s' no es valido el destino.");
		addTextToLangTable(l, "Math instruction: '%s' not a valid destination.", "Instrucción Math: '%s' no es valido el destino.");
		addTextToLangTable(l, "Piecewise linear lookup table with zero elements!", "tabla de linealizacion por segmentos con cero elementos!");
		addTextToLangTable(l, "x values in piecewise linear table must be strictly increasing.", "Los valores X en la tabla de linealización por segmentos deben ser estrictamente incrementales.");
		addTextToLangTable(l, "Numerical problem with piecewise linear lookup table. Either make the table entries smaller, or space the points together more closely.\r\n\r\nSee the help file for details.", "Problema numérico con la tabla de linealización por segmentos. Haz la tabla de entradas mas pequeña, o aleja mas los puntos juntos.\r\n\r\nMira la ayuda para mas detalles.");
		addTextToLangTable(l, "Multiple escapes (\\0-9) present in format string, not allowed.", "No esta permitido mas de un caracter especial (\\0-9) dentro de la cadena de caractares.");
		addTextToLangTable(l, "Bad escape: correct form is \\xAB.", "Caracter Especial Erroneo: la forma correcta es = \\xAB.");
		addTextToLangTable(l, "Bad escape '\\%c'", "Caracter Especial Erroneo '\\%c'");
		addTextToLangTable(l, "Variable is interpolated into formatted string, but none is specified.", "Se ha declarado un parametro dentro la cadena de caracteres, pero falta especificar la variable.");
		addTextToLangTable(l, "No variable is interpolated into formatted string, but a variable name is specified. Include a string like '\\-3', or leave variable name blank.", "No se ha declarado un parametro dentro de la cadena de caractares pero sin embargo se ha especificado una variable. Añada un cadena como '\\-3', o quite el nombre de la variable.");
		addTextToLangTable(l, "Empty row; delete it or add instructions before compiling.", "Fila vacia; borrela o añada instrucciones antes de compilar.");
		addTextToLangTable(l, "Couldn't write to '%s'", "No puedo escribir en  '%s'.");
		addTextToLangTable(l, "Unsupported op (anything ADC, PWM, UART, EEPROM) for interpretable target.", "Op no soportada en el interprete (algun ADC, PWM, UART, EEPROM).");
		addTextToLangTable(l, "Compile successful; wrote interpretable code to '%s'.\r\n\r\nYou probably have to adapt the interpreter to your application. See the documentation.", "Compilación correcta: Código para interprete escrito en '%s'.\r\n\r\nProblablemente tengas que adaptar el interprete a tu aplicación. Mira la documentación.");
		addTextToLangTable(l, "Microcontroller '%s' not supported.\r\n\r\nDefaulting to no selected MCU.", "Microcontrolador '%s' no sorportado.\r\n\r\nForzando ninguna CPU.");
		addTextToLangTable(l, "File format error; perhaps this program is for a newer version of LDmicro?", "Error en el formato de archivo; quizas este programa es una version mas moderna de LDmicro?.");
		addTextToLangTable(l, "Index:", "Indice:");
		addTextToLangTable(l, "Points:", "Puntos:");
		addTextToLangTable(l, "Count:", "Cantidad:");
		addTextToLangTable(l, "Edit table of ASCII values like a string", "Editar tabla de valores ascii como una cadena");
		addTextToLangTable(l, "Look-Up Table", "Buscar en Tabla");
		addTextToLangTable(l, "Piecewise Linear Table", "Tabla de linealización por segmentos");
		addTextToLangTable(l, "POPTools Error", "POPTools Error");
		addTextToLangTable(l, "Compile Successful", "Compilación Correcta");
		addTextToLangTable(l, "digital in", "entrada digital");
		addTextToLangTable(l, "digital out", "salida digital");
		addTextToLangTable(l, "int. relay", "rele interno");
		addTextToLangTable(l, "UART tx", "UART tx");
		addTextToLangTable(l, "UART rx", "UART rx");
		addTextToLangTable(l, "PWM out", "salida PWM");
		addTextToLangTable(l, "turn-on delay", "activar retardo");
		addTextToLangTable(l, "turn-off delay", "desactivar retardo");
		addTextToLangTable(l, "retentive timer", "temporizador con memoria");
		addTextToLangTable(l, "counter", "contador");
		addTextToLangTable(l, "general var", "var general");
		addTextToLangTable(l, "adc input", "entrada adc");
		addTextToLangTable(l, "<corrupt!>", "<estropeado!>");
		addTextToLangTable(l, "(not assigned)", "(no asignado)");
		addTextToLangTable(l, "<no UART!>", "<no UART!>");
		addTextToLangTable(l, "<no PWM!>", "<no PWM!>");
		addTextToLangTable(l, "TOF: variable cannot be used elsewhere", "TOF: la variable no puede ser usada en otra parte");
		addTextToLangTable(l, "TON: variable cannot be used elsewhere", "TON: la variable no puede ser usada en otra parte");
		addTextToLangTable(l, "RTO: variable can only be used for RES elsewhere", "RTO: la variable solo puede ser usada como RES en otra parte");
		addTextToLangTable(l, "Variable '%s' not assigned to, e.g. with a MOV statement, an ADD statement, etc.\r\n\r\nThis is probably a programming error; now it will always be zero.", "Variable '%s' no asignada, p.e. con el comando MOV, una instrucción ADD, etc.\r\n\r\nEsto es probablemente un error de programación; valdrá cero.");
		addTextToLangTable(l, "Variable for '%s' incorrectly assigned: %s.", "Variable para '%s' incorrectamente asignada: %s.");
		addTextToLangTable(l, "Division by zero; halting simulation", "División por cero; Parando simulación");
		addTextToLangTable(l, "!!!too long!!!", "!!Muy grande!!");
		addTextToLangTable(l, "\n\nI/O ASSIGNMENT:\n\n", "\n\nE/S ASIGNACIÓN:\n\n");
		addTextToLangTable(l, "  Name                       | Type               | Pin\n", "  Nombre                     | Tipo               | Pata\n");
		addTextToLangTable(l, "Serial (UART) will use pins %d and %d.\r\n\r\n", "El Puerto Serie (UART) usará los pines %d y %d.\r\n\r\n");
	} else {
		l = nullptr;
	}
}

PCWSTR getRibbonLocalizedExampleGalleryHeader(eExampleGalleryHeader header)
{
	PCWSTR pPTBR = L"", pEN = L"", pES = L"";

	switch(header) {
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_IO          , L"I/O"            , L"I/O"            , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Timers      , L"Temporizadores" , L"Timers"         , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Counters    , L"Contadores"     , L"Counters"       , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Variables   , L"Variáveis"      , L"Variables"      , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Conditionals, L"Condicionais"   , L"Conditionals"   , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Mathematics , L"Matemática"     , L"Mathematics"    , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Analogs     , L"Analógicos"     , L"Analogs"        , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Motors      , L"Motores"        , L"Motors"         , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_ModBUS      , L"ModBUS"         , L"ModBUS"         , L"ModBUS");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_RS485_Text  , L"RS-485 - Texto" , L"RS-485 - Text"  , L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_RS485_Others, L"RS-485 - Outros", L"RS-485 - Others", L"I/O");
			SET_LANGUAGE_STRINGS(eExampleGalleryHeader_Applications, L"Aplicações"     , L"Applications"   , L"I/O");
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
		SET_LANGUAGE_STRINGS(cmdTabHome                  , L"Início"                           , L"Home"                          , L"Inicio"                             );
		SET_LANGUAGE_STRINGS(cmdTabInstrLogic            , L"Controles Lógicos"                , L"Logic Controls"                , L"Controles logicos"                  );
		SET_LANGUAGE_STRINGS(cmdTabInstrAutom            , L"Controles Automação"              , L"Automation Controls"           , L"Controles de Automacion"            );
		SET_LANGUAGE_STRINGS(cmdTabSimulation            , L"Simulação"                        , L"Simulation"                    , L"Simulacion"                         );
		SET_LANGUAGE_STRINGS(cmdGroupInstr               , L"Instrução"                        , L"Instruction"                   , L"Instruccion"                        );
		SET_LANGUAGE_STRINGS(cmdGroupLine                , L"Linhas"                           , L"Lines"                         , L"Lineas"                             );
		SET_LANGUAGE_STRINGS(cmdGroupFind                , L"Busca"                            , L"Find"                          , L"Busqueda"                           );
		SET_LANGUAGE_STRINGS(cmdGroupConfig              , L"Configurações"                    , L"Configuration"                 , L"Configuraciones"                    );
		SET_LANGUAGE_STRINGS(cmdGroupHelp                , L"Ajuda"                            , L"Help"                          , L"Ayuda"                              );
		SET_LANGUAGE_STRINGS(cmdGroupIO                  , L"I/O"                              , L"I/O"                           , L"I/O"                                );
		SET_LANGUAGE_STRINGS(cmdGroupTimer               , L"Temporizadores"                   , L"Timers"                        , L"Temporizadores"                     );
		SET_LANGUAGE_STRINGS(cmdGroupCounter             , L"Contadores"                       , L"Counters"                      , L"Contador"                           );
		SET_LANGUAGE_STRINGS(cmdGroupVar                 , L"Variáveis"                        , L"Variables"                     , L"Variable"                           );
		SET_LANGUAGE_STRINGS(cmdGroupCond                , L"Condicionais"                     , L"Conditionals"                  , L"Condicionales"                      );
		SET_LANGUAGE_STRINGS(cmdGroupMath                , L"Matemática"                       , L"Mathematics"                   , L"Matematica"                         );
		SET_LANGUAGE_STRINGS(cmdGroupAnalog              , L"Analógicos"                       , L"Analogs"                       , L"Analogico"                          );
		SET_LANGUAGE_STRINGS(cmdGroupMotors              , L"Motores"                          , L"Motors"                        , L"Motores"                            );
		SET_LANGUAGE_STRINGS(cmdGroupEnc                 , L"Encoder Incremental"              , L"Incremental encoder"           , L"Encoder Incremental"                );
		SET_LANGUAGE_STRINGS(cmdGroupSSIEnc              , L"Encoder SSI"                      , L"SSI Encoder"                   , L"Encoder SSI"                        );
		SET_LANGUAGE_STRINGS(cmdGroupCommText            , L"RS-485 - Texto"                   , L"RS-485 - Text"                 , L"RS-485 - Texto"                     );
		SET_LANGUAGE_STRINGS(cmdGroupCommModBUS          , L"ModBUS"                           , L"ModBUS"                        , L"ModBUS"                             );
		SET_LANGUAGE_STRINGS(cmdGroupCommOthers          , L"RS-485 - Outros"                  , L"RS-485 - Others"               , L"RS-485 - Otros"                     );
		SET_LANGUAGE_STRINGS(cmdDropDownIO               , L"Mais I/O"                         , L"More I/O"                      , L"Mas I/O"                            );
		SET_LANGUAGE_STRINGS(cmdDropDownMath             , L"Mais"                             , L"More"                          , L"Mas"                                );
		SET_LANGUAGE_STRINGS(cmdGroupSimulation          , L"Simulação"                        , L"Simulation"                    , L"Simulacion"                         );
		SET_LANGUAGE_STRINGS(cmdGroupSimulLog            , L"Registro de Dados"                , L"Data log"                      , L"Registro de Datos"                  );
		SET_LANGUAGE_STRINGS(cmdGroupSimulExit           , L"Sair"                             , L"Exit"                          , L"Salir"                              );
		SET_LANGUAGE_STRINGS(cmdGroupActions             , L"Ações"                            , L"Actions"                       , L"Acciones"                           );
		SET_LANGUAGE_STRINGS(cmdGroupComment             , L"Documentação"                     , L"Documentation"                 , L"Documentos"                         );
		SET_LANGUAGE_STRINGS(cmdGroupNavigate            , L"Navegação"                        , L"Navigation"                    , L"Navegacion"                         );

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
		SET_LANGUAGE_STRINGS(cmdInsertLineAfter          , L"Inserir Após"                     , L"Insert After"                  , L"Inserir despues"                    );
		SET_LANGUAGE_STRINGS(cmdInsertLineBefore         , L"Inserir Antes"                    , L"Insert Before"                 , L"Inserir antes"                      );
		SET_LANGUAGE_STRINGS(cmdGoHome                   , L"Início"                           , L"Home"                          , L"Inicio"                             );
		SET_LANGUAGE_STRINGS(cmdGoEnd                    , L"Final"                            , L"End"                           , L"Final"                              );
		SET_LANGUAGE_STRINGS(cmdInstrParallel            , L"Paralelo"                         , L"Parallel"                      , L"Paralelo"                           );
		SET_LANGUAGE_STRINGS(cmdCopyInstr                , L"Copiar"                           , L"Copy"                          , L"Copiar"                             );
		SET_LANGUAGE_STRINGS(cmdPasteInstr               , L"Colar"                            , L"Paste"                         , L"Pegar"                              );
		SET_LANGUAGE_STRINGS(cmdDeleteInstr              , L"Excluir"                          , L"Delete"                        , L"Excluir"                            );
		SET_LANGUAGE_STRINGS(cmdDeleteLine               , L"Excluir"                          , L"Delete"                        , L"Nuevo");
		SET_LANGUAGE_STRINGS(cmdFindDialog               , L"Localizar e Substituir"           , L"Find and Replace"              , L"Localizar y Substituir"             );
		SET_LANGUAGE_STRINGS(cmdFind                     , L"Localizar Próxima"                , L"Find Next"                     , L"Localizar Proxima"                  );

		SET_LANGUAGE_STRINGS(cmdInstrComment             , L"Comentário"                       , L"Comment"                       , L"Comentario"                         );

		SET_LANGUAGE_STRINGS(cmdInstrContact             , L"Contato"                          , L"Contact"                       , L"Contacto"                           );
		SET_LANGUAGE_STRINGS(cmdInstrCoil                , L"Bobina"                           , L"Coil"                          , L"Bobina"                             );
		SET_LANGUAGE_STRINGS(cmdInstrOneShotFalling      , L"Borda de Descida"                 , L"One Shot Falling"              , L"Borde de bajada"                    );
		SET_LANGUAGE_STRINGS(cmdInstrOneShotRising       , L"Borda de Subida"                  , L"One Shot Rising"               , L"Borde de subida"                    );
		SET_LANGUAGE_STRINGS(cmdInstrCircuitOpen         , L"Circuito Aberto"                  , L"Open Circuit"                  , L"Circuito abierto"                   );
		SET_LANGUAGE_STRINGS(cmdInstrCircuitClosed       , L"Circuito Fechado"                 , L"Closed Circuit"                , L"Circuito abierto"                   );
		SET_LANGUAGE_STRINGS(cmdInstrMasterRelayControl  , L"Chave Geral"                      , L"Master Relay Control"          , L"Llave general"                      );

		SET_LANGUAGE_STRINGS(cmdInstrTimerON             , L"Atraso para Ligar"                , L"Timer ON"                      , L"Retrazo para prender"               );
		SET_LANGUAGE_STRINGS(cmdInstrTimerOFF            , L"Atraso para Desligar"             , L"Timer OFF"                     , L"Retrazo para apagar"                );
		SET_LANGUAGE_STRINGS(cmdInstrTimerONRet          , L"Atraso com Memória"               , L"Timer ON Retentive"            , L"Retrazo con memoria"                );
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
		SET_LANGUAGE_STRINGS(cmdInstrMathModulo          , L"Resto da Divisão"                 , L"Modulo"                        , L"Modulo"                             );
		SET_LANGUAGE_STRINGS(cmdInstrMathSqrt            , L"Raiz Quadrada"                    , L"Square Root"                   , L"Raiz Cuadrada"                      );
		SET_LANGUAGE_STRINGS(cmdInstrShiftRegister       , L"Fila de Variáveis"                , L"Shift Register"                , L"Fila de Variables"                  );
		SET_LANGUAGE_STRINGS(cmdInstrLookUpTable         , L"Tabela de Busca"                  , L"Look-Up Table"                 , L"Tabla de Busca"                     );
		SET_LANGUAGE_STRINGS(cmdInstrPieceWiseLinear     , L"Linearização por Segmentos"       , L"Piecewise Linear"              , L"Linear por seguimientos"            );
		SET_LANGUAGE_STRINGS(cmdInstrRand                , L"Número Randômico"                 , L"Random Number"                 , L"Numero Randonico"                   );
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

		SET_LANGUAGE_STRINGS(cmdInstrPWM                 , L"Saída PWM"                        , L"PWM Output"                    , L"Salida PWM"                         );
		SET_LANGUAGE_STRINGS(cmdInstrRampDA              , L"Rampa de Aceleração/Desaceleração", L"Acceleration/Deceleration Ramp", L"Rampa de Aceleracion/Desaceleracion");
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
		SET_LANGUAGE_STRINGS(cmdSettings                 , L"Preferências"                     , L"Settings"                      , L"Preferencias"                       );

		SET_LANGUAGE_STRINGS(cmdSimulation               , L"Modo de Simulação"                , L"Simulation mode"               , L"Modo de Simulacion"                 );
		SET_LANGUAGE_STRINGS(cmdSimulationExit           , L"Sair da Simulação"                , L"Exit Simulation"               , L"Salir de Simulacion"                );
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

		SET_LANGUAGE_STRINGS(cmdExampleComment           , L"Comentário"                       , L"Comment"                       , L"Comentario"                         );

		SET_LANGUAGE_STRINGS(cmdExampleContact           , L"Contato"                          , L"Contact"                       , L"Contacto"                           );
		SET_LANGUAGE_STRINGS(cmdExampleCoil              , L"Bobina"                           , L"Coil"                          , L"Bobina"                             );
		SET_LANGUAGE_STRINGS(cmdExampleOneShotFalling    , L"Borda de Descida"                 , L"One Shot Falling"              , L"Borde de bajada"                    );
		SET_LANGUAGE_STRINGS(cmdExampleOneShotRising     , L"Borda de Subida"                  , L"One Shot Rising"               , L"Borde de subida"                    );
		SET_LANGUAGE_STRINGS(cmdExampleCircuitOpen       , L"Circuito Aberto"                  , L"Open Circuit"                  , L"Circuito abierto"                   );
		SET_LANGUAGE_STRINGS(cmdExampleCircuitClosed     , L"Circuito Fechado"                 , L"Closed Circuit"                , L"Circuito abierto"                   );
		SET_LANGUAGE_STRINGS(cmdExampleMasterRelayControl, L"Chave Geral"                      , L"Master Relay Control"          , L"Llave general"                      );

		SET_LANGUAGE_STRINGS(cmdExampleTimerON           , L"Atraso para Ligar"                , L"Timer ON"                      , L"Retrazo para prender"               );
		SET_LANGUAGE_STRINGS(cmdExampleTimerOFF          , L"Atraso para Desligar"             , L"Timer OFF"                     , L"Retrazo para apagar"                );
		SET_LANGUAGE_STRINGS(cmdExampleTimerONRet        , L"Atraso com Memória"               , L"Timer ON Retentive"            , L"Retrazo con memoria"                );
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
		SET_LANGUAGE_STRINGS(cmdExampleMathModulo        , L"Resto da Divisão"                 , L"Modulo"                        , L"Modulo"                             );
		SET_LANGUAGE_STRINGS(cmdExampleMathSqrt          , L"Raiz Quadrada"                    , L"Square Root"                   , L"Raiz Cuadrada"                      );
		SET_LANGUAGE_STRINGS(cmdExampleShiftRegister     , L"Fila de Variáveis"                , L"Shift Register"                , L"Fila de Variables"                  );
		SET_LANGUAGE_STRINGS(cmdExampleLookUpTable       , L"Tabela de Busca"                  , L"Look-Up Table"                 , L"Tabla de Busca"                     );
		SET_LANGUAGE_STRINGS(cmdExamplePieceWiseLinear   , L"Linearização por Segmentos"       , L"Piecewise Linear"              , L"Linear por seguimientos"            );
		SET_LANGUAGE_STRINGS(cmdExampleRand              , L"Número Randômico"                 , L"Random Number"                 , L"Numero Randonico"                   );
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

		SET_LANGUAGE_STRINGS(cmdExamplePWM               , L"Saída PWM"                        , L"PWM Output"                    , L"Salida PWM"                         );
		SET_LANGUAGE_STRINGS(cmdExampleRampDA            , L"Rampa de Aceleração/Desaceleração", L"Acceleration/Deceleration Ramp", L"Rampa de Aceleracion/Desaceleracion");
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
		SET_LANGUAGE_STRINGS(cmdExampleSemaphore         , L"Semáforo"                         , L"Semaphore"                     , L"Semaforo"                           );
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
		SET_LANGUAGE_STRINGS(cmdInsertLineAfter          , L"Inserir Após (Insert)"                        , L"Insert After (Insert)"                     , L"Inserir despues (Insert)"                       );
		SET_LANGUAGE_STRINGS(cmdInsertLineBefore         , L"Inserir Antes (Shift + Insert)"               , L"Insert Before (Shift + Insert)"            , L"Inserir antes (Shift + Insert)"                 );
		SET_LANGUAGE_STRINGS(cmdGoHome                   , L"Início (Shift + Home)"                        , L"Home (Shift + Home)"                       , L"Inicio (Shift + Home)"                          );
		SET_LANGUAGE_STRINGS(cmdGoEnd                    , L"Final (Shift + End)"                          , L"End (Shift + End)"                         , L"Final (Shift + End)"                            );
		SET_LANGUAGE_STRINGS(cmdInstrParallel            , L"Paralelo (P)"                                 , L"Parallel (P)"                              , L"Paralelo (P)"                                   );
		SET_LANGUAGE_STRINGS(cmdCopyInstr                , L"Copiar (Control + C)"                         , L"Copy (Control + C)"                        , L"Copiar (Control + C)"                           );
		SET_LANGUAGE_STRINGS(cmdPasteInstr               , L"Colar (Control + V)"                            , L"Paste (Control + V)"                     , L"Pegar (Control + V)"                            );
		SET_LANGUAGE_STRINGS(cmdDeleteInstr              , L"Excluir (Delete)"                             , L"Delete (Delete)"                           , L"Excluir (Delete)"                               );
		SET_LANGUAGE_STRINGS(cmdDeleteLine               , L"Excluir (Shift + Delete)"                     , L"Delete (Shift + Delete)"                   , L"Excluir (Shift + Delete)"                       );
		SET_LANGUAGE_STRINGS(cmdFindDialog               , L"Localizar e Substituir (F3)"                  , L"Find and Replace (F3)"                     , L"Localizar y Substituir (F3)"                    );
		SET_LANGUAGE_STRINGS(cmdFind                     , L"Localizar Próxima (Shift+F3)"                 , L"Find Next (Shift+F3)"                      , L"Localizar Proxima (Shift+F3)"                   );

		SET_LANGUAGE_STRINGS(cmdInstrComment             , L"Comentário (;)"                               , L"Comment (;)"                               , L"Comentario (;)"                                 );

		SET_LANGUAGE_STRINGS(cmdInstrContact             , L"Contato (C)"                                  , L"Contact (C)"                               , L"Contacto (C)"                                   );
		SET_LANGUAGE_STRINGS(cmdInstrCoil                , L"Bobina (B)"                                   , L"Coil (B)"                                  , L"Bobina (B)"                                     );
		SET_LANGUAGE_STRINGS(cmdInstrOneShotFalling      , L"Borda de Descida (\\)"                        , L"One Shot Falling (\\)"                     , L"Borde de bajada (\\)"                           );
		SET_LANGUAGE_STRINGS(cmdInstrOneShotRising       , L"Borda de Subida (/)"                          , L"One Shot Rising (/)"                       , L"Borde de subida (/)"                            );

		SET_LANGUAGE_STRINGS(cmdInstrTimerON             , L"Atraso para Ligar (A)"                        , L"Timer ON (A)"                              , L"Retrazo para prender (A)"                       );
		SET_LANGUAGE_STRINGS(cmdInstrTimerOFF            , L"Atraso para Desligar (Shift + A)"             , L"Timer OFF (Shift + A)"                     , L"Retrazo para apagar (Shift + A)"                );
		SET_LANGUAGE_STRINGS(cmdInstrTimerONRet          , L"Atraso com Memória (T)"                       , L"Timer ON Retentive (T)"                    , L"Retrazo con memoria (T)"                        );
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

		SET_LANGUAGE_STRINGS(cmdInstrPWM                 , L"Saída PWM (Shift + P)"                        , L"PWM Output (Shift + P)"                    , L"Salida PWM (Shift + P)"                         );
		SET_LANGUAGE_STRINGS(cmdInstrRampDA              , L"Rampa de Aceleração/Desaceleração (Shift + R)", L"Acceleration/Deceleration Ramp (Shift + R)", L"Rampa de Aceleracion/Desaceleracion (Shift + R)");
		SET_LANGUAGE_STRINGS(cmdInstrReadEnc             , L"Leitura de Encoder (E)"                       , L"Read Encoder (E)"                          , L"Lectura de Encoder (E)"                         );
		SET_LANGUAGE_STRINGS(cmdInstrResetEnc            , L"Escrita no Encoder (Shift + E)"               , L"Write Encoder (Shift + E)"                 , L"Escrita de Encoder (Shift + E)"                 );

		SET_LANGUAGE_STRINGS(cmdInstrReadUSS             , L"Leitura USS (Shift + U)"                      , L"Read USS (Shift + U)"                      , L"Lectura USS (Shift + U)"                        );
		SET_LANGUAGE_STRINGS(cmdInstrWriteUSS            , L"Escrita USS (U)"                              , L"Write USS (U)"                             , L"Escrita USS (U)"                                );
		SET_LANGUAGE_STRINGS(cmdInstrReadNS600           , L"Leitura de NS-600 Yaskawa (Shift + Y)"        , L"Read NS-600 Yaskawa (Shift + Y)"           , L"Lectura de NS-600 Yaskawa (Shift + Y)"          );
		SET_LANGUAGE_STRINGS(cmdInstrWriteNS600          , L"Escrita de NS-600 Yaskawa (Y)"                , L"Write NS-600 Yaskawa (Y)"                  , L"Escrita de NS-600 Yaskawa (Y)"                  );

		SET_LANGUAGE_STRINGS(cmdInstrReadModBUS          , L"Ler Registrador ModBUS (Shift + X)"           , L"Read ModBUS Register (Shift + X)"          , L"Leer Registrador ModBUS (Shift + X)"            );
		SET_LANGUAGE_STRINGS(cmdInstrWriteModBUS         , L"Escrever Registrador ModBUS (X)"              , L"Write ModBUS Register (X)"                 , L"Escribir Registrador ModBUS (X)"                );

		SET_LANGUAGE_STRINGS(cmdConfig                   , L"Projeto (F2)"                                 , L"Project (F2)"                              , L"Proyecto (F2)"                                  );

		SET_LANGUAGE_STRINGS(cmdSimulation               , L"Modo de Simulação (F7)"                       , L"Simulation mode (F7)"                      , L"Modo de Simulacion (F7)"                        );
		SET_LANGUAGE_STRINGS(cmdSimulationExit           , L"Sair da Simulação (ESC)"                      , L"Exit Simulation (ESC)"                     , L"Salir de Simulacion (ESC)"                      );
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
				L"Insere uma linha vazia após a linha atualmente selecionada.",
				L"Insert After",
				L"Inserir despues");
			SET_LANGUAGE_STRINGS(cmdCopyLine,
				L"Copia a linha atualmente selecionada para a memória.",
				L"Copy Line",
				L"Copiar Linea");
			SET_LANGUAGE_STRINGS(cmdPasteLineBefore,
				L"Colar a linha previamente copiada antes da linha atualmente selecionada.",
				L"Paste Before",
				L"Pegar antes");
			SET_LANGUAGE_STRINGS(cmdPasteLineAfter,
				L"Colar a linha previamente copiada após a linha atualmente selecionada.",
				L"Paste After",
				L"Pegar despues");
			SET_LANGUAGE_STRINGS(cmdInstrCircuitOpen,
				L"Inserir circuito aberto na posição do cursor.",
				L"Open Circuit",
				L"Circuito abierto");
			SET_LANGUAGE_STRINGS(cmdInstrCircuitClosed,
				L"Inserir circuito fechado na posição do cursor.",
				L"Closed Circuit",
				L"Circuito abierto");
			SET_LANGUAGE_STRINGS(cmdInstrMasterRelayControl,
				L"Inserir chave geral na posição do cursor. Utilizado para desativar o barramento abaixo desta instrução.",
				L"Master Relay Control",
				L"Llave general");
			SET_LANGUAGE_STRINGS(cmdInstrShiftRegister,
				L"Inserir fila de variáveis na posição do cursor.",
				L"Shift Register",
				L"Fila de Variables");
			SET_LANGUAGE_STRINGS(cmdInstrLookUpTable,
				L"Inserir tabela de busca na posição do cursor.",
				L"Look-Up Table",
				L"Tabla de Busca");
			SET_LANGUAGE_STRINGS(cmdInstrPieceWiseLinear,
				L"Inserir linearização por segmentos na posição do cursor.",
				L"Piecewise Linear",
				L"Linear por seguimientos");
			SET_LANGUAGE_STRINGS(cmdInstrRand,
				L"Inserir Número Randômico na posição do cursor.",
				L"Random Number",
				L"Numero Randonico");
			SET_LANGUAGE_STRINGS(cmdCompile,
				L"Gerar arquivo .hex para gravação na POP-7 a patir do Diagrama Ladder.",
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
				L"Desfazer a última ação.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdRedo                     ,
				L"Refazer a última ação desfeita.",
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
				L"Copia a linha atualmente selecionada para a memória.",
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
				L"Colar a linha previamente copiada após a linha atualmente selecionada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInsertLineAfter          ,
				L"Insere uma linha vazia após a linha atualmente selecionada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInsertLineBefore         ,
				L"Insere uma linha vazia antes da linha atualmente selecionada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdGoHome                   ,
				L"Move o cursor para o início do diagrama.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdGoEnd                    ,
				L"Move o cursor para o final do diagrama.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrParallel            ,
				L"Inserir início de paralelo na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdCopyInstr                ,
				L"Copia o objeto selecionado para a memória.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdPasteInstr               ,
				L"Insere na posição atual o objeto previamente copiado para a memória.",
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
				L"Inserir comentário na linha atual. Se a linha não estiver vazia, uma nova será criada.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrContact             ,
				L"Inserir um contato na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCoil                ,
				L"Inserir bobina na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrOneShotFalling      ,
				L"Inserir detecção de borda de descida na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrOneShotRising       ,
				L"Inserir detecção de borda de subida na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCircuitOpen         ,
				L"Inserir circuito aberto na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCircuitClosed       ,
				L"Inserir circuito fechado na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMasterRelayControl  ,
				L"Inserir chave geral na posição do cursor. Utilizado para desativar o barramento abaixo desta instrução.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrTimerON             ,
				L"Inserir temporizador com atraso para ligar na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrTimerOFF            ,
				L"Inserir temporizador com atraso para desligar na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrTimerONRet          ,
				L"Abrir exemplo de instrução atraso com memória.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrSchedule            ,
				L"Inserir tarefa agendada na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrTimerReset          ,
				L"Inserir zerar atraso na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrCounterInc          ,
				L"Inserir contador incremental na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCounterDec          ,
				L"Inserir contador decremental na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCounterCirc         ,
				L"Inserir contador circular na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCounterReset        ,
				L"Inserir zerar contador na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrMathAdd             ,
				L"Inserir instrução matemática somar na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathSub             ,
				L"Inserir instrução matemática subtrair na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathMult            ,
				L"Inserir instrução matemática multiplicar na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathDivide          ,
				L"Inserir instrução matemática dividir na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathModulo          ,
				L"Inserir instrução matemática resto de divisão na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrMathSqrt            ,
				L"Inserir instrução matemática raiz quadrada na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrShiftRegister       ,
				L"Inserir fila de variáveis na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrLookUpTable         ,
				L"Inserir tabela de busca na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrPieceWiseLinear     ,
				L"Inserir linearização por segmentos na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrRand                ,
				L"Inserir Número Randômico na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrAbs                 ,
				L"Inserir a instrução de Valor Absoluto na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrMov                 ,
				L"Inserir mover valor na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrSetBit              ,
				L"Inserir liga/desliga bit na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCheckBit            ,
				L"Inserir checar bit na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrPersist             ,
				L"Inserir persistente na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrCondEqual           ,
				L"Inserir condicional igual na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondNotEqual        ,
				L"Inserir condicional diferente na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondGreater         ,
				L"Inserir condicional maior na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondGreaterEqual    ,
				L"Inserir condicional maior ou igual na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondLesser          ,
				L"Inserir condicional menor na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrCondLesserEqual     ,
				L"Inserir condicional menor ou igual na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrReadAD              ,
				L"Inserir leitura de A/D na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteDA             ,
				L"Inserir escrita no D/A na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrPWM                 ,
				L"Inserir saída PWM na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrRampDA              ,
				L"Inserir rampa de aceleração/desaceleração na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrReadEnc             ,
				L"Inserir leitura de encoder na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrResetEnc            ,
				L"Inserir escrita no encoder na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrPID                 ,
				L"Inserir Controle PID na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrWriteChar           ,
				L"Inserir escrita de caracter na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrReadChar            ,
				L"Inserir leitura de caracter na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrReadFormatString    ,
				L"Inserir ler string formatada na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteFormatString   ,
				L"Inserir escrever string formatada na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrReadUSS             ,
				L"Inserir leitura uss na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteUSS            ,
				L"Inserir escrita uss na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrReadNS600           ,
				L"Inserir leitura de NS-600 Yaskawa na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteNS600          ,
				L"Inserir escrita no NS-600 Yaskawa na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdInstrReadModBUS          ,
				L"Inserir ler registrador modbus na posição do cursor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdInstrWriteModBUS         ,
				L"Inserir escrever registrador modbus na posição do cursor.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdConfig                   ,
				L"Abrir a janela de configurações do projeto atual.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSettings                 ,
				L"Abrir a janela para configuração do programa POPTools como porta serial para gravação, desativar avisos, etc.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdSimulation               ,
				L"Entrar no modo de simulação do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationExit           ,
				L"Sair do modo de simulação do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationStart          ,
				L"Iniciar a simulação em tempo real do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationPause          ,
				L"Pausar a simulação em tempo real do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationStop           ,
				L"Interromper a simulação do Diagrama Ladder, retornando toda a lógica ao estado inicial.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationSingleCycle    ,
				L"Executar um único ciclo.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationLogStart       ,
				L"Iniciar o registro de dados da Simulação.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdSimulationLogStop        ,
				L"Parar o registro de dados da Simulação.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdCompile                  ,
				L"Gerar arquivo .hex para gravação na POP-7 a patir do Diagrama Ladder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdCompileAndProgram        ,
				L"Gerar arquivo .hex e gravá-lo na POP-7 a partir do Diagrama Ladder.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdDebug                    ,
				L"Abrir janela de depuração da POP-7, onde é possível ler e alterar seus registradores.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdSplitUserManual          ,
				L"Abrir o Manual do Usuário, contendo a ajuda para todo o programa.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdUserManual          ,
				L"Abrir o Manual do Usuário, contendo a ajuda para todo o programa.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdKeyboardManual           ,
				L"Abrir o Manual do Teclado, contendo todos os atalhos para os comandos da interface.",
				L"",
				L"");

			// Example comands list
			SET_LANGUAGE_STRINGS(cmdExamples                 ,
				L"Abrir a lista de lógicas de exemplo.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleComment           ,
				L"Abrir exemplo de comentário.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleContact           ,
				L"Abrir exemplo de instrução contato.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCoil              ,
				L"Abrir exemplo de instrução bobina.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleOneShotFalling    ,
				L"Abrir exemplo de instrução detecção de borda de descida.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleOneShotRising     ,
				L"Abrir exemplo de instrução detecção de borda de subida.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCircuitOpen       ,
				L"Abrir exemplo de instrução circuito aberto.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCircuitClosed     ,
				L"Abrir exemplo de instrução circuito fechado.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMasterRelayControl,
				L"Abrir exemplo de instrução chave geral.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleTimerON           ,
				L"Abrir exemplo de instrução temporizador com atraso para ligar.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleTimerOFF          ,
				L"Abrir exemplo de instrução temporizador com atraso para desligar.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleTimerONRet        ,
				L"Abrir exemplo de instrução atraso com memória.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleSchedule          ,
				L"Abrir exemplo de instrução tarefa agendada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleTimerReset        ,
				L"Abrir exemplo de instrução zerar atraso.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleCounterInc        ,
				L"Abrir exemplo de instrução contador incremental.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCounterDec        ,
				L"Abrir exemplo de instrução contador decremental.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCounterCirc       ,
				L"Abrir exemplo de instrução contador circular.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCounterReset      ,
				L"Abrir exemplo de instrução zerar contador.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleMathAdd           ,
				L"Abrir exemplo de instrução matemática somar.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathSub           ,
				L"Abrir exemplo de instrução matemática subtrair.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathMult          ,
				L"Abrir exemplo de instrução matemática multiplicar.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathDivide        ,
				L"Abrir exemplo de instrução matemática dividir.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathModulo        ,
				L"Abrir exemplo de instrução matemática resto de divisão.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleMathSqrt          ,
				L"Abrir exemplo de instrução matemática raiz quadrada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleShiftRegister     ,
				L"Abrir exemplo de instrução fila de variáveis.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleLookUpTable       ,
				L"Abrir exemplo de instrução tabela de busca.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExamplePieceWiseLinear   ,
				L"Abrir exemplo de instrução linearização por segmentos.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleRand              ,
				L"Abrir exemplo de Número Randômico.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleAbs               ,
				L"Abrir exemplo de instrução que calcula Valor Absoluto.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleMov               ,
				L"Abrir exemplo de instrução mover valor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleSetBit            ,
				L"Abrir exemplo de instrução liga/desliga bit.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCheckBit          ,
				L"Abrir exemplo de instrução checar bit.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExamplePersist           ,
				L"Abrir exemplo de instrução persistente.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleCondEqual         ,
				L"Abrir exemplo de instrução condicional igual.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondNotEqual      ,
				L"Abrir exemplo de instrução condicional diferente.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondGreater       ,
				L"Abrir exemplo de instrução condicional maior.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondGreaterEqual  ,
				L"Abrir exemplo de instrução condicional maior ou igual.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondLesser        ,
				L"Abrir exemplo de instrução condicional menor.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleCondLesserEqual   ,
				L"Abrir exemplo de instrução condicional menor ou igual.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleReadAD            ,
				L"Abrir exemplo de instrução leitura de A/D.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteDA           ,
				L"Abrir exemplo de instrução escrita no D/A.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExamplePWM               ,
				L"Abrir exemplo de instrução saída PWM.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleRampDA            ,
				L"Abrir exemplo de instrução rampa de aceleração/desaceleração.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleReadEnc           ,
				L"Abrir exemplo de instrução leitura de encoder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleResetEnc          ,
				L"Abrir exemplo de instrução escrita no encoder.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExamplePID               ,
				L"Abrir exemplo de instrução Controle de PID.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleWriteChar         ,
				L"Abrir exemplo de instrução escrita de caracter.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleReadChar          ,
				L"Abrir exemplo de instrução leitura de caracter.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleReadFormatString  ,
				L"Abrir exemplo de instrução ler string formatada.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteFormatString ,
				L"Abrir exemplo de instrução escrever string formatada.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleReadUSS           ,
				L"Abrir exemplo de instrução leitura uss.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteUSS          ,
				L"Abrir exemplo de instrução escrita uss.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleReadNS600         ,
				L"Abrir exemplo de instrução leitura de NS-600 Yaskawa.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteNS600        ,
				L"Abrir exemplo de instrução escrita no NS-600 Yaskawa.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleReadModBUS        ,
				L"Abrir exemplo de instrução ler registrador modbus.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleWriteModBUS       ,
				L"Abrir exemplo de instrução escrever registrador modbus.",
				L"",
				L"");

			SET_LANGUAGE_STRINGS(cmdExampleAdcLed            ,
				L"Abrir exemplo de aplicação - Barra de Leds.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleSemaphore         ,
				L"Abrir exemplo de aplicação - Semáforo.",
				L"",
				L"");
			SET_LANGUAGE_STRINGS(cmdExampleTester            ,
				L"Abrir exemplo de aplicação - Testador.",
				L"",
				L"");
	}

	switch(POPSettings.idLanguage) {
		case  1: return pEN;
		case  2: return pES;
		default: return pPTBR;
	}
}

const char *_(const char *in)
{
	int i;
	LangTableChar *table = l;

	if(table == nullptr) {
		return in;
	}

	for(i = 0; table != nullptr; i++) {
		if(in[i] == 0) {
			if(table->out != nullptr) {
				return table->out;
			} else {
				break;
			}
		}

		table = table->mapChar[(unsigned char)in[i] - 1];
	}

#ifdef _DEBUG
	if(l != nullptr) {
		vectorNoMatchFound.push_back(in);
		addTextToLangTable(l, in, in);
	}
#endif

	return in;
}
