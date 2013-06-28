#include "poptools.h"

// Funcao auxiliar que retorna um char* de uma string passada como parametro, alocando a memoria necessaria
char *AllocCharFromString(string s)
{
	char *buf = new char[s.size() + 1];
	strcpy(buf, s.c_str());
	return buf;
}

mapIO::mapIO(LadderDiagram *pDiagram)
{
	countIO    = 0;
	selectedIO = 0;

	diagram = pDiagram;

	// Adicionando itens que sao flags internas
	vectorInternalFlag.push_back("SerialReady"  );
	vectorInternalFlag.push_back("SerialTimeout");
	vectorInternalFlag.push_back("SerialAborted");
	vectorInternalFlag.push_back("RampActive"   );
	vectorInternalFlag.push_back("TcpReady"     );
	vectorInternalFlag.push_back("TcpTimeout"   );

	// Adicionando variaveis reservadas
	vectorReservedName.push_back(_("in"  ));
	vectorReservedName.push_back(_("out" ));
	vectorReservedName.push_back(_("new" ));
	vectorReservedName.push_back(_("var" ));
	vectorReservedName.push_back(_("char"));
}

void mapIO::Clear(void)
{
	IO.clear();
	countIO = 0;
	selectedIO = 0;
}

bool mapIO::IsReserved(string name)
{
	vector<string>::size_type i;
	for(i = 0; i < vectorReservedName.size(); i++) {
		if(name == vectorReservedName[i]) return true; // Encontrado!
	}

	// Caso especial para Shift Register:
	// Nome reservado se iniciar por reg e terminar em numero
	char cname[MAX_NAME_LEN];
	strcpy(cname, name.c_str());
	if(tolower(cname[0]) == 'r' && tolower(cname[1]) == 'e' && tolower(cname[2]) == 'g') {
		// inicia com reg, agora verifica se termina em numero
		int val = atoi(cname + 3);
		sprintf(cname + 3, "%d", val);
		if(name == cname) {
			return true;
		}
	}

	return false;
}

bool mapIO::IsInternalFlag(string name)
{
	vector<string>::size_type i;
	for(i = 0; i < vectorInternalFlag.size(); i++) {
		if(name == vectorInternalFlag[i]) return true; // Encontrado!
	}

	return false;
}

bool mapIO::Add(string name, eType type, bool isUndoRedo)
{
	bool ret = false;

	if(IO.count(name) == 0) {
		if(IsReserved(name)) {
			type = eType_Reserved;
		}

		IO[name].first       = 1 + countIO++; // ID a partir de 1. Zero indica id invalido
		IO[name].second.pin  = 0;
		IO[name].second.bit  = 0;
		IO[name].second.type = type;

		IO[name].second.countRequestBit = 0;
		IO[name].second.countRequestInt = 0;

		if(!isUndoRedo) {
			// Registro da acao para desfazer / refazer
			diagram->CheckpointBegin(_("Adicionar I/O"));

			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			data->Add.name = AllocCharFromString(name);
			data->Add.type = type;

			action.action        = eAdd;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = diagram->getContext();
			action.data          = data;
			action.io            = this;
			action.elem          = nullptr;
			action.subckt        = nullptr;

			diagram->RegisterAction(action);

			diagram->CheckpointEnd();
		}

		updateGUI();

		ret = true;
	}

	return true;
}

bool mapIO::Update(unsigned long id, string name, eType type, bool isUndoRedo)
{
	string old_name = getName(id);
	if(old_name.size() == 0) return false; // id invalido!

	if(IsReserved(name)) {
		type = eType_Reserved;
	}

	// Check for Internal Flags and Reserved Names restrictions
	if((type != eType_InternalFlag && IsInternalFlag(name)) ||
		(type == eType_InternalFlag && !IsInternalFlag(name)) ||
		(IsReserved(name) && name != old_name)) {
			return false;
	}

	if(!isUndoRedo) {
		// Registro da acao para desfazer / refazer
		diagram->CheckpointBegin(_("Atualizar I/O"));

		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->Update.id   = id;
		data->Update.name = AllocCharFromString(old_name);
		data->Update.type = IO[old_name].second.type;

		action.action        = eUpdate;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = diagram->getContext();
		action.data          = data;
		action.io            = this;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		diagram->RegisterAction(action);

		diagram->CheckpointEnd();
	}

	if(name == old_name) { // Nome inalterado, atualiza apenas o tipo!
		IO[name].second.type = type;
	} else {
		map<string, pair<unsigned long, mapDetails> >::iterator it = IO.find(old_name);

		// Atualizando o nome do I/O
		if(IO.count(name) > 0) {
			// Nesta condicao esta mesclando dois I/Os existentes.
			// Situacao possivel porem que nao ocorre atualmente devido a tratamento em LadderDiagram::getIO
			// Para garantir que nao vai mesmo ocorrer, vamos gerar erro aqui!
			// Se vier a ser alterado o tratamento em LadderDiagram::getIO, vai aparecer o erro e sera entao
			// possivel corrigi-lo ao inves de prejudicar toda a logica
			// Precisa de uma tarefa especifica de undo/redo para esta situacao e tambem mesclar as contagens
			// de requests int/bit (o que ja esta sendo feito abaixo porem precisa de verificacao)
			oops();

			IO[name].second.countRequestBit += it->second.second.countRequestBit;
			IO[name].second.countRequestInt += it->second.second.countRequestInt;
		} else {
			IO[name] = it->second;
		}

		// Atualizando o tipo do I/O
		IO[name].second.type = type;

		// Exclui o I/O antigo
		IO.erase(it);
	}

	updateGUI();

	return true;
}

unsigned long mapIO::Request(string name, bool isBit, eType type)
{
	bool result;
	unsigned long id = 0;

	// Registro da acao para desfazer / refazer
	diagram->CheckpointBegin(_("Solicitar I/O"));

	if(IO.count(name) > 0) {
		result = Update(getID(name), name, type);
	} else {
		result = Add(name, type);
	}

	if(result) {
		if(isBit) {
			IO[name].second.countRequestBit++;
		} else {
			IO[name].second.countRequestInt++;
		}

		id = getID(name);
	}

	UndoRedoAction action;
	UndoRedoData *data = new UndoRedoData;

	data->Request.id        = id;
	data->Request.isBit     = isBit;
	data->Request.isDiscard = false;

	action.action        = eRequest;
	action.contextAfter  = getEmptyContext();
	action.contextBefore = diagram->getContext();
	action.data          = data;
	action.io            = this;
	action.elem          = nullptr;
	action.subckt        = nullptr;

	diagram->RegisterAction(action);

	diagram->CheckpointEnd();

	return id;
}

void mapIO::Discard(unsigned long id, bool isBit)
{
	string name = getName(id);
	if(name.size() > 0) {
		if(isBit) {
			IO[name].second.countRequestBit--;
		} else {
			IO[name].second.countRequestInt--;
		}

		// Registro da acao para desfazer / refazer
		diagram->CheckpointBegin(_("Descartar I/O"));

		UndoRedoAction action;
		UndoRedoData *data = new UndoRedoData;

		data->Request.id        = id;
		data->Request.isBit     = isBit;
		data->Request.isDiscard = true;

		action.action        = eRequest;
		action.contextAfter  = getEmptyContext();
		action.contextBefore = diagram->getContext();
		action.data          = data;
		action.io            = this;
		action.elem          = nullptr;
		action.subckt        = nullptr;

		diagram->RegisterAction(action);

		// Se Variavel nao usada, remove do mapa
		if(IO[name].second.countRequestBit == 0 && IO[name].second.countRequestInt == 0) {
			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			data->Discard.id        = id;
			data->Discard.name      = AllocCharFromString(name);
			data->Discard.detailsIO = getDetails(id);

			action.action        = eDiscard;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = diagram->getContext();
			action.data          = data;
			action.io            = this;
			action.elem          = nullptr;
			action.subckt        = nullptr;

			diagram->RegisterAction(action);

			map<string, pair<unsigned long, mapDetails> >::iterator it;
			it = IO.find(name);
			IO.erase(it);

			updateGUI();
		}

		diagram->CheckpointEnd();
	}
}

unsigned long mapIO::getIndex(unsigned long id, bool isTotal)
{
	unsigned long idxBit = 1, idxInt = 1;
	map<string, pair<unsigned long, mapDetails> >::iterator it;

	for(it = IO.begin(); it != IO.end(); it++) {
		// Se encontrou, retorna o indice conforme modo do IO
		if(it->second.first == id) {
			if(isTotal) {
				return idxBit + idxInt - 1;
			} else if (it->second.second.countRequestInt == 0) {
				return idxBit;
			} else {
				return idxInt;
			}
		}

		// Incrementa o indice referente ao modo do IO atual
		if(it->second.second.countRequestInt == 0) {
			idxBit++;
		} else {
			idxInt++;
		}
	}

	return 0;
}

string mapIO::getName(unsigned long id)
{
	map<string, pair<unsigned long, mapDetails> >::iterator it;

	for(it = IO.begin(); it != IO.end(); it++) {
		if(it->second.first == id) {
			return it->first;
		}
	}

	return "";
}

mapDetails mapIO::getDetails(unsigned long id)
{
	string name = getName(id);

	if(name.size() > 0) {
		return IO[name].second;
	}

	mapDetails m = { 0, 0, eType_Pending, 0, 0 };
	return m;
}

unsigned long mapIO::getID(string name)
{
	// Retorna zero se o nome nao estiver no mapa
	if(IO.count(name) == 0) {
		return 0;
	}

	return IO[name].first;
}

unsigned long mapIO::getID(unsigned int index)
{
	// Retorna zero se o indice for maior que o numero de elementos do mapa
	if(index >= IO.size()) {
		return 0;
	}

	map<string, pair<unsigned long, mapDetails> >::iterator it = IO.begin();

	while(index--) {
		it++;
	}

	return it->second.first;
}

unsigned int mapIO::getCount(void)
{
	return IO.size();
}

void mapIO::Select(unsigned int index)
{
	selectedIO = getID(index);
}

bool mapIO::Assign(unsigned long id, unsigned int pin, unsigned int bit, bool isUndoRedo)
{
	string name = getName(id);

	if(name.size() > 0) {
		return Assign(name, pin, bit, isUndoRedo);
	}

	return false;
}

bool mapIO::Assign(string name, unsigned int pin, unsigned int bit, bool isUndoRedo)
{
	if(IO.count(name) > 0) {
		// Registro da acao para desfazer / refazer
		if(!isUndoRedo) {
			diagram->CheckpointBegin(_("Associar I/O"));

			UndoRedoAction action;
			UndoRedoData *data = new UndoRedoData;

			data->Assign.id  = IO[name].first;
			data->Assign.pin = IO[name].second.pin;
			data->Assign.bit = IO[name].second.bit;

			action.action        = eAssign;
			action.contextAfter  = getEmptyContext();
			action.contextBefore = diagram->getContext();
			action.data          = data;
			action.io            = this;
			action.elem          = nullptr;
			action.subckt        = nullptr;

			diagram->RegisterAction(action);

			diagram->CheckpointEnd();
		}

		// Associa o I/O
		IO[name].second.pin = pin;
		IO[name].second.bit = bit;

		return true;
	}

	return false;
}

string mapIO::getPortName(int index)
{
	McuIoInfo *mcu = diagram->getMCU();
	char buf[1024];
	mapDetails detailsIO = getDetails(getID(index));

	if((detailsIO.type != eType_DigInput && detailsIO.type != eType_DigOutput && detailsIO.type != eType_General
		&& detailsIO.type != eType_ReadADC && detailsIO.type != eType_ReadEnc && detailsIO.type != eType_ResetEnc
		&& detailsIO.type != eType_ReadUSS && detailsIO.type != eType_WriteUSS) || detailsIO.pin == 0 || !mcu)
	{
		return string("");
	}

	if(UartFunctionUsed() && mcu) {
		if((mcu->uartNeeds.rxPin == detailsIO.pin) ||
			(mcu->uartNeeds.txPin == detailsIO.pin))
		{
			return string(_("<UART needs!>"));
		}
	}

	if(PwmFunctionUsed() && mcu) {
		if(mcu->pwmNeedsPin == detailsIO.pin && detailsIO.type == eType_DigOutput) {
			return string(_("<PWM needs!>"));
		}
	}

	if((detailsIO.type == eType_DigOutput || detailsIO.type == eType_DigInput) && detailsIO.pin > 19)
		sprintf(buf, "M%d.%d", detailsIO.pin - 20, detailsIO.bit);
	else if (detailsIO.type == eType_DigOutput)
		sprintf(buf, "S%d", detailsIO.pin);
	else if (detailsIO.type == eType_General)
		sprintf(buf, "M%d", detailsIO.pin - 20);
	else if (detailsIO.type == eType_DigInput)
		sprintf(buf, "E%d", detailsIO.pin);
	else if (detailsIO.type == eType_ReadADC)
		if (detailsIO.pin == 6)
			strcpy(buf, "TEMP");
		else
			sprintf(buf, "AD%d", detailsIO.pin);
	else if (detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc)
		strcpy(buf, detailsIO.pin == 1 ? _("Enc. Inc.") : _("Enc. Abs."));
	else
		strcpy(buf, _("<not an I/O!>"));

	return string(buf);
}

//-----------------------------------------------------------------------------
// Map an I/O type to a string describing it. Used both in the on-screen
// list and when we write a text file to describe it.
//-----------------------------------------------------------------------------
char *mapIO::getTypeString(eType type)
{
    switch(type) {
        case eType_Reserved:			return _("reservado");
		case eType_DigInput:			return _("digital in");
        case eType_DigOutput:			return _("digital out");
		case eType_InternalRelay:		return _("int. relay");
        case eType_TxUART:				return _("UART tx");
		case eType_RxUART:				return _("UART rx");
        case eType_PWM:					return _("PWM out");
		case eType_TON:					return _("turn-on delay");
		case eType_TOF:					return _("turn-off delay");
        case eType_RTO:					return _("retentive timer");
        case eType_Counter:				return _("counter");
		case eType_General:				return _("general var");
        case eType_ReadADC:				return _("adc input");
        case eType_ReadEnc:				return _("entrada encoder");
        case eType_ResetEnc:			return _("write encoder");
        case eType_ReadUSS:				return _("read USS");
		case eType_WriteUSS:			return _("write USS");
		case eType_SetDAC:				return _("Set D/A");
		case eType_ReadModbus:			return _("leitura modbus");
		case eType_WriteModbus:			return _("escrita modbus");
		case eType_ReadYaskawa:			return _("leitura NS600");
		case eType_WriteYaskawa:		return _("escrita NS600");
		case eType_InternalFlag:		return _("Flag Interna");
		case eType_Pending:				return _("Sem tipo");
        default:                        return "";
    }
}

// Funcao que executa uma acao de desfazer / refazer
bool mapIO::DoUndoRedo(bool IsUndo, bool isDiscard, UndoRedoAction &action)
{
	UndoRedoData *data = (UndoRedoData *)action.data;
	switch(action.action) {
	case eAssign: {
		if(isDiscard) {
			// Nada a fazer...
		} else {
			mapDetails detailsIO = getDetails(data->Assign.id);
			Assign(data->Assign.id, data->Assign.pin, data->Assign.bit, true);

			// Atualiza a estrutura para se desfazer essa acao
			data->Assign.pin = detailsIO.pin;
			data->Assign.bit = detailsIO.bit;

			// Atualiza a tela para que se exiba corretamente a lista de I/Os
			updateGUI();
		}
		break;
	}

	case eRequest: {
		if(isDiscard) {
			// Nada a fazer...
		} else {
			int offset = data->Request.isDiscard ? -1 : +1;
			if(IsUndo) {
				offset *= -1;
			}

			if(data->Request.isBit) {
				IO[getName(data->Request.id)].second.countRequestBit += offset;
			} else {
				IO[getName(data->Request.id)].second.countRequestInt += offset;
			}
		}
		break;
	}

	case eAdd: {
		if(isDiscard) {
			delete [] data->Add.name; // Descarta o buffer com o nome do I/O
		} else if(IsUndo) {
			IO.erase(IO.find(data->Add.name));
			countIO--;
		} else {
			Add(data->Add.name, data->Add.type, true);
		}

		// Atualiza a tela para que se exiba corretamente a lista de I/Os
		updateGUI();

		break;
	}

	case eUpdate: {
		if(isDiscard) {
			delete [] data->Update.name; // Descarta o buffer com o nome do I/O
		} else {
			char       *name      = AllocCharFromString(getName(data->Update.id));
			mapDetails  detailsIO = getDetails(data->Update.id);

			Update(data->Update.id, data->Update.name, data->Update.type, true);

			if(name != nullptr) {
				delete [] data->Update.name; // Descarta o buffer com o nome (agora atual) do I/O
				data->Update.name    = name; // Salva o nome anterior para a operacao de Refazer
				data->Update.type    = detailsIO.type; // Atualiza o tipo para o anterior
			}
		}

		// Atualiza a tela para que se exiba corretamente a lista de I/Os
		updateGUI();

		break;
	}

	case eDiscard: {
		if(isDiscard) {
			delete [] data->Discard.name; // Descarta o buffer com o nome do I/O
		} else if(IsUndo) {
			IO[data->Discard.name].first  = data->Discard.id;
			IO[data->Discard.name].second = data->Discard.detailsIO;
		} else {
			map<string, pair<unsigned long, mapDetails> >::iterator it;
			it = IO.find(data->Discard.name);
			IO.erase(it);
		}

		// Atualiza a tela para que se exiba corretamente a lista de I/Os
		updateGUI();

		break;
	}

	default: return false;
	}

	// Se estamos descartando, desaloca a estrutura que representa a acao
	if(isDiscard) {
		delete data;
	}

	return true; // Nada mais a fazer
}

// I/O that we have seen recently, so that we don't forget pin assignments
// when we re-extract the list
#define MAX_IO_SEEN_PREVIOUSLY 1024
struct {
    char    name[MAX_NAME_LEN];
    int     type;
    int     pin;
	int		bit;
} IoSeenPreviously[MAX_IO_SEEN_PREVIOUSLY];
int IoSeenPreviouslyCount;

// stuff for the dialog box that lets you choose pin assignments
static BOOL DialogDone;
static BOOL DialogCancel;

static HWND IoDialog;

static HWND PinList;
static HWND lblBit;
static HWND OkButton;
static HWND CancelButton;
static HWND BitCombobox;
static HWND textLabelName;

// stuff for the popup that lets you set the simulated value of an analog in
static HWND AnalogSliderMain;
static HWND AnalogSliderTrackbar;
static BOOL AnalogSliderDone;
static BOOL AnalogSliderCancel;

// stuff for the popup that lets you set the simulated value of an analog in
static HWND EncoderSliderMain;
static HWND EncoderSliderTrackbar;
static BOOL EncoderSliderDone;
static BOOL EncoderSliderCancel;

static int nSortColumn = 0;
static BOOL bSortAscending = TRUE;

// stuff for the popup that lets you set the simulated value of an encode in
/*static HWND EncoderSliderMain;
static HWND EncoderSliderTrackbar;
static BOOL EncoderSliderDone;
static BOOL EncoderSliderCancel;*/

const LPCTSTR ComboboxBitItens[] = { "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9", "10",
									"11", "12", "13", "14", "15", "16", "17", "18", "19", "20", 
									"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", 
									"31"};

//-----------------------------------------------------------------------------
// Append an I/O to the I/O list if it is not in there already.
//-----------------------------------------------------------------------------
static void AppendIo(char *name, int type, unsigned char bit)
{
    int i;
	int idx = 0;
    for(i = 0; i < Prog.io.count; i++) {
        if(_stricmp(Prog.io.assignment[i].name, name)==0) {
            if(type != IO_TYPE_GENERAL && Prog.io.assignment[i].type ==
                IO_TYPE_GENERAL)
            {
                Prog.io.assignment[i].type = type;
            }
            // already in there
            return;
        }
    }

    if(i < MAX_IO) {
        Prog.io.assignment[i].type = type;
        Prog.io.assignment[i].pin = NO_PIN_ASSIGNED;
		Prog.io.assignment[i].bit = bit;
        strcpy(Prog.io.assignment[i].name, name);
        (Prog.io.count)++;
    }
}

//-----------------------------------------------------------------------------
// Move an I/O pin into the `seen previously' list. This means that if the
// user creates input Xasd, assigns it a pin, deletes, and then recreates it,
// then it will come back with the correct pin assigned.
//-----------------------------------------------------------------------------
static void AppendIoSeenPreviously(char *name, int type, int pin, int bit)
{
    if(_stricmp(name, _("new"))==0) return;

    int i;
    for(i = 0; i < IoSeenPreviouslyCount; i++) {
        if(_stricmp(name, IoSeenPreviously[i].name)==0 &&
            type == IoSeenPreviously[i].type)
        {
            if(pin != NO_PIN_ASSIGNED) {
                IoSeenPreviously[i].pin = pin;
				IoSeenPreviously[i].bit = bit;
            }
            return;
        }
    }
    if(IoSeenPreviouslyCount >= MAX_IO_SEEN_PREVIOUSLY) {
        // maybe improve later; just throw away all our old information, and
        // the user might have to reenter the pin if they delete and recreate
        // things
        IoSeenPreviouslyCount = 0;
    }

    i = IoSeenPreviouslyCount;
    IoSeenPreviously[i].type = type;
    IoSeenPreviously[i].pin = pin;
	IoSeenPreviously[i].bit = bit;
    strcpy(IoSeenPreviously[i].name, name);
    IoSeenPreviouslyCount++;
}

//-----------------------------------------------------------------------------
// Walk a subcircuit, calling ourselves recursively and extracting all the
// I/O names out of it.
//-----------------------------------------------------------------------------
void ExtractNamesFromCircuit(int which, void *any)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                ExtractNamesFromCircuit(p->contents[i].which,
                    p->contents[i].d.any);
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                ExtractNamesFromCircuit(s->contents[i].which,
                    s->contents[i].d.any);
            }
            break;
        }
        case ELEM_CONTACTS:
            switch(l->d.contacts.type) {
                case IO_TYPE_INTERNAL_RELAY:
                    AppendIo(l->d.contacts.name, IO_TYPE_INTERNAL_RELAY, l->d.contacts.bit);
                    break;

                case IO_TYPE_DIG_INPUT:
                    AppendIo(l->d.contacts.name, IO_TYPE_DIG_INPUT, l->d.contacts.bit);
                    break;

                case IO_TYPE_DIG_OUTPUT:
                    AppendIo(l->d.contacts.name, IO_TYPE_DIG_OUTPUT, l->d.contacts.bit);
                    break;

				case IO_TYPE_INTERNAL_FLAG:
                    AppendIo(l->d.contacts.name, IO_TYPE_INTERNAL_FLAG, l->d.contacts.bit);
                    break;

                default:
                    oops();
                    break;
            }
            break;

        case ELEM_COIL:
			if(l->d.coil.type == IO_TYPE_INTERNAL_RELAY || l->d.coil.type == IO_TYPE_DIG_OUTPUT) {
	            AppendIo(l->d.coil.name, l->d.coil.type, l->d.coil.bit);
			} else {
                oops();
			}
            break;

        case ELEM_TON:
        case ELEM_TOF:
            AppendIo(l->d.timer.name, which == ELEM_TON ?  IO_TYPE_TON :
                IO_TYPE_TOF, 0);
            break;

        case ELEM_RTO:
            AppendIo(l->d.timer.name, IO_TYPE_RTO, 0);
            break;

        case ELEM_MOVE:
            AppendIo(l->d.move.dest, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.move.src))
				AppendIo(l->d.move.src, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_SET_BIT:
			AppendIo(l->d.setBit.name, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_CHECK_BIT:
			AppendIo(l->d.checkBit.name, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_ADD:
        case ELEM_SUB:
        case ELEM_MUL:
        case ELEM_DIV:
        case ELEM_MOD:
            AppendIo(l->d.math.dest, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.math.op1))
	            AppendIo(l->d.math.op1, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.math.op2))
	            AppendIo(l->d.math.op2, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_SQRT:
            AppendIo(l->d.sqrt.dest, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.sqrt.src))
	            AppendIo(l->d.sqrt.src, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_RAND:
			AppendIo(l->d.rand.var, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.rand.min))
	            AppendIo(l->d.rand.min, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.rand.max))
	            AppendIo(l->d.rand.max, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_ABS:
			AppendIo(l->d.abs.dest, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.abs.src))
	            AppendIo(l->d.abs.src, IO_TYPE_GENERAL, 0);
            break;

		case ELEM_READ_FORMATTED_STRING:
		case ELEM_WRITE_FORMATTED_STRING:
        case ELEM_FORMATTED_STRING:
            if(strlen(l->d.fmtdStr.var) > 0) {
                AppendIo(l->d.fmtdStr.var, IO_TYPE_UART_TX, 0);
            }
            break;

		case ELEM_READ_SERVO_YASKAWA:
            if(strlen(l->d.servoYaskawa.var) > 0) {
                AppendIo(l->d.servoYaskawa.var, IO_TYPE_READ_YASKAWA, 0);
            }
            break;

		case ELEM_WRITE_SERVO_YASKAWA:
            if(strlen(l->d.servoYaskawa.var) > 0 && !IsNumber(l->d.servoYaskawa.var)) {
                AppendIo(l->d.servoYaskawa.var, IO_TYPE_WRITE_YASKAWA, 0);
            }
            break;

        case ELEM_UART_SEND:
            AppendIo(l->d.uart.name, IO_TYPE_UART_TX, 0);
            break;

        case ELEM_UART_RECV:
            AppendIo(l->d.uart.name, IO_TYPE_UART_RX, 0);
            break;

        case ELEM_SET_PWM:
            AppendIo(l->d.setPwm.name, IO_TYPE_PWM_OUTPUT, 0);
            break;

        case ELEM_CTU:
        case ELEM_CTD:
        case ELEM_CTC:
            AppendIo(l->d.counter.name, IO_TYPE_COUNTER, 0);
            break;

        case ELEM_READ_ADC:
            AppendIo(l->d.readAdc.name, IO_TYPE_READ_ADC, 0);
            break;

        case ELEM_SET_DA:
			if(!IsNumber(l->d.setDA.name))
	            AppendIo(l->d.setDA.name, IO_TYPE_SET_DA, 0);
            break;

        case ELEM_READ_ENC:
            AppendIo(l->d.readEnc.name, IO_TYPE_READ_ENC, 0);
//			switch(GetPinEnc(l->d.readEnc.name)) {
//			case 1:
				AppendIo(InternalVars[0][INTVAR_INC_PERIMRODA], IO_TYPE_GENERAL, 0);
				AppendIo(InternalVars[0][INTVAR_INC_PULSOS   ], IO_TYPE_GENERAL, 0);
				AppendIo(InternalVars[0][INTVAR_INC_FATOR    ], IO_TYPE_GENERAL, 0);
//				break;
//			case 2:
				AppendIo(InternalVars[0][INTVAR_ABS_PERIMRODA], IO_TYPE_GENERAL, 0);
				AppendIo(InternalVars[0][INTVAR_ABS_FATOR    ], IO_TYPE_GENERAL, 0);
//				break;
//			}
            break;

        case ELEM_RESET_ENC:
			AppendIo(l->d.resetEnc.name, IO_TYPE_RESET_ENC, 0);
			break;

		case ELEM_READ_USS:
            AppendIo(l->d.readUSS.name, IO_TYPE_READ_USS, 0);
            break;

        case ELEM_WRITE_USS:
            AppendIo(l->d.writeUSS.name, IO_TYPE_WRITE_USS, 0);
            break;

        case ELEM_READ_MODBUS:
            AppendIo(l->d.readModbus.name, IO_TYPE_READ_MODBUS, 0);
            break;

        case ELEM_WRITE_MODBUS:
            AppendIo(l->d.writeModbus.name, IO_TYPE_WRITE_MODBUS, 0);
            break;

        case ELEM_SHIFT_REGISTER: {
            int i;
            for(i = 0; i < l->d.shiftRegister.stages; i++) {
                char str[MAX_NAME_LEN+10];
                sprintf(str, "%s%d", l->d.shiftRegister.name, i);
                AppendIo(str, IO_TYPE_GENERAL, 0);
            }
            break;
        }

        case ELEM_LOOK_UP_TABLE:
            AppendIo(l->d.lookUpTable.dest , IO_TYPE_GENERAL, 0);
			AppendIo(l->d.lookUpTable.index, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_PIECEWISE_LINEAR:
            AppendIo(l->d.piecewiseLinear.dest , IO_TYPE_GENERAL, 0);
			AppendIo(l->d.piecewiseLinear.index, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_EQU:
        case ELEM_NEQ:
        case ELEM_GRT:
        case ELEM_GEQ:
        case ELEM_LES:
        case ELEM_LEQ:
			if(!IsNumber(l->d.cmp.op1))
	            AppendIo(l->d.cmp.op1, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.cmp.op2))
	            AppendIo(l->d.cmp.op2, IO_TYPE_GENERAL, 0);
            break;

        case ELEM_PERSIST:
			AppendIo(l->d.persist.var, IO_TYPE_GENERAL, 0);
			break;

		case ELEM_MULTISET_DA:
			if(!IsNumber(l->d.multisetDA.name))
				AppendIo(l->d.multisetDA.name, IO_TYPE_GENERAL, 0);
			if(!IsNumber(l->d.multisetDA.name1))
				AppendIo(l->d.multisetDA.name1, IO_TYPE_GENERAL, 0);
			break;

		case ELEM_RTC:
        case ELEM_PLACEHOLDER:
        case ELEM_COMMENT:
        case ELEM_SHORT:
        case ELEM_OPEN:
        case ELEM_MASTER_RELAY:
        case ELEM_ONE_SHOT_RISING:
        case ELEM_ONE_SHOT_FALLING:
        case ELEM_RES:
            break;

        default:
            oops();
    }
}

void UpdateTypeInRung(char *name, unsigned int type, int which, void *any)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                UpdateTypeInRung(name, type, p->contents[i].which, p->contents[i].d.any);
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                UpdateTypeInRung(name, type, s->contents[i].which, s->contents[i].d.any);
            }
            break;
        }

		case ELEM_CONTACTS:
			if(!_stricmp(name, l->d.contacts.name))
				l->d.contacts.type = type;
            break;

        case ELEM_COIL:
			if(!_stricmp(name, l->d.coil.name))
				l->d.coil.type = type;
            break;
	}
}

//-----------------------------------------------------------------------------
// Walk a subcircuit, calling ourselves recursively and extracting all the
// I/O names out of it.
//-----------------------------------------------------------------------------
void UpdateTypeInCircuit(char *name, unsigned int type)
{
	int i;
    for(i = 0; i < Prog.numRungs; i++) {
		UpdateTypeInRung(name, type, ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    }
}

void UpdateTypeForInternalRelaysInRung(int which, void *any)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                UpdateTypeForInternalRelaysInRung(p->contents[i].which, p->contents[i].d.any);
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                UpdateTypeForInternalRelaysInRung(s->contents[i].which, s->contents[i].d.any);
            }
            break;
        }

		case ELEM_CONTACTS:
			if(l->d.contacts.type == IO_TYPE_PENDING)
				l->d.contacts.type = IO_TYPE_INTERNAL_RELAY;
            break;

        case ELEM_COIL:
			if(l->d.coil.type == IO_TYPE_PENDING)
				l->d.coil.type = IO_TYPE_INTERNAL_RELAY;
            break;
	}
}

//-----------------------------------------------------------------------------
// Walk a subcircuit, calling ourselves recursively and extracting all the
// I/O names out of it.
//-----------------------------------------------------------------------------
void UpdateTypeForInternalRelays()
{
	int i;
    for(i = 0; i < Prog.numRungs; i++) {
		UpdateTypeForInternalRelaysInRung(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    }
}

void UpdateTypesFromSeenPreviouslyList()
{
	int i;

	// Search for Coils and Contacts in the loaded I/O List to update their types in Program Rungs.
	for(i=0; i<IoSeenPreviouslyCount; i++) {
		if(IoSeenPreviously[i].type == IO_TYPE_INTERNAL_RELAY ||
		   IoSeenPreviously[i].type == IO_TYPE_DIG_INPUT ||
		   IoSeenPreviously[i].type == IO_TYPE_DIG_OUTPUT ||
		   IoSeenPreviously[i].type == IO_TYPE_INTERNAL_FLAG)
				UpdateTypeInCircuit(IoSeenPreviously[i].name, IoSeenPreviously[i].type);
	}
}

BOOL ExistsCoilWithNameInRung(char *name, int which, void *any)
{
    ElemLeaf *l = (ElemLeaf *)any;

    switch(which) {
        case ELEM_PARALLEL_SUBCKT: {
            ElemSubcktParallel *p = (ElemSubcktParallel *)any;
            int i;
            for(i = 0; i < p->count; i++) {
                if(ExistsCoilWithNameInRung(name, p->contents[i].which, p->contents[i].d.any))
					return TRUE;
            }
            break;
        }
        case ELEM_SERIES_SUBCKT: {
            ElemSubcktSeries *s = (ElemSubcktSeries *)any;
            int i;
            for(i = 0; i < s->count; i++) {
                if(ExistsCoilWithNameInRung(name, s->contents[i].which, s->contents[i].d.any))
					return TRUE;
            }
            break;
        }

        case ELEM_COIL:
			if(!_stricmp(name, l->d.coil.name))
				return TRUE;
            break;
	}

	return FALSE;
}

BOOL ExistsCoilWithName(char *name)
{
	int i;
    for(i = 0; i < Prog.numRungs; i++) {
		if(ExistsCoilWithNameInRung(name, ELEM_SERIES_SUBCKT, Prog.rungs[i]))
			return TRUE;
    }
	return FALSE;
}

//-----------------------------------------------------------------------------
// Compare function to qsort() the I/O list. Group by type, then 
// alphabetically within each section.
//-----------------------------------------------------------------------------
static int CompareIoName(const void *av, const void *bv)
{
    PlcProgramSingleIo *a = (PlcProgramSingleIo *)av;
    PlcProgramSingleIo *b = (PlcProgramSingleIo *)bv;

    return bSortAscending ? _stricmp(a->name, b->name) : _stricmp(b->name, a->name);
}

static int CompareIoPin(const void *av, const void *bv)
{
    PlcProgramSingleIo *a = (PlcProgramSingleIo *)av;
    PlcProgramSingleIo *b = (PlcProgramSingleIo *)bv;

    /*if(a->type != b->type) {
        return a->type - b->type;
    }*/

    if(a->pin == NO_PIN_ASSIGNED && b->pin != NO_PIN_ASSIGNED) return  1;
    if(b->pin == NO_PIN_ASSIGNED && a->pin != NO_PIN_ASSIGNED) return -1;

    return bSortAscending ? a->pin - b->pin : b->pin - a->pin;
}

static int CompareIoPort(const void *av, const void *bv)
{
    PlcProgramSingleIo *a = (PlcProgramSingleIo *)av;
    PlcProgramSingleIo *b = (PlcProgramSingleIo *)bv;

	int i1 = 0;
	int i2 = 0;

    //if(a->type != b->type) {
    //    return a->type - b->type;
    //}

    if(a->pin == NO_PIN_ASSIGNED && b->pin != NO_PIN_ASSIGNED) return  1;
    if(b->pin == NO_PIN_ASSIGNED && a->pin != NO_PIN_ASSIGNED) return -1;

    if(Prog.mcu && Prog.mcu->whichIsa == ISA_ANSIC) {
        i1 = 0;
    }

    if(a->type != IO_TYPE_DIG_INPUT && a->type != IO_TYPE_DIG_OUTPUT
        && a->type != IO_TYPE_READ_ADC && a->type != IO_TYPE_READ_ENC && a->type != IO_TYPE_RESET_ENC
		&& a->type != IO_TYPE_READ_USS && a->type != IO_TYPE_WRITE_USS)
    {
        i1 = 0;;
    }

    int pin = a->pin;
    if(pin == NO_PIN_ASSIGNED || !Prog.mcu) {
        i1 = 0;
    }

    if(UartFunctionUsed() && Prog.mcu) {
        if((Prog.mcu->uartNeeds.rxPin == pin) ||
            (Prog.mcu->uartNeeds.txPin == pin))
        {
            i1 = 0; // "<UART needs!>"
        }
    }

    if(PwmFunctionUsed() && Prog.mcu) {
        if(Prog.mcu->pwmNeedsPin == pin) {
            i1 = 0; // "<PWM needs!>"
        }
    }

    int j;
    for(j = 0; j < Prog.mcu->pinCount; j++) 
	{
        if(Prog.mcu->pinInfo[j].pin == pin) 
		{
			if (a->type == IO_TYPE_DIG_OUTPUT && a->pin == 17)
				i1 = 90000;
			else if (a->type == IO_TYPE_DIG_OUTPUT && a->pin == 18)
				i1 = 90001;
			else if (a->type == IO_TYPE_GENERAL && a->pin > 19)
				i1 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + a->bit;
			else if (a->type == IO_TYPE_DIG_OUTPUT && a->pin > 19)
				i1 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + a->bit; // "M%d.%d"
			else if (a->type == IO_TYPE_DIG_OUTPUT)
				i1 = 40000 + Prog.mcu->pinInfo[j].bit;
			else if (a->type == IO_TYPE_DIG_INPUT && a->pin > 19)
				i1 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + a->bit;
			else if (a->type == IO_TYPE_DIG_INPUT)
				i1 = 30000 + Prog.mcu->pinInfo[j].bit;
			else if (a->type == IO_TYPE_READ_ADC)
				if (a->pin == 6)
					i1 = 90003;
				else
					i1 = 60000 + Prog.mcu->pinInfo[j].bit;
			else if (a->type == IO_TYPE_READ_ENC || a->type == IO_TYPE_RESET_ENC)
				i1 = 70000 + Prog.mcu->pinInfo[j].bit;
            break;
        }
    }
    if(j == Prog.mcu->pinCount) {
        i1 = 0; // "<not an I/O!>";
    }

	// 2o. parameter content compare 
    if(Prog.mcu && Prog.mcu->whichIsa == ISA_ANSIC) {
        i2 = 0;
    }

    if(b->type != IO_TYPE_DIG_INPUT && b->type != IO_TYPE_DIG_OUTPUT
        && b->type != IO_TYPE_READ_ADC && b->type != IO_TYPE_READ_ENC && b->type != IO_TYPE_RESET_ENC
		&& b->type != IO_TYPE_READ_USS && b->type != IO_TYPE_WRITE_USS)
    {
        i2 = 0;
    }

    pin = b->pin;
    if(pin == NO_PIN_ASSIGNED || !Prog.mcu) {
        i2 = 0;
    }

    if(UartFunctionUsed() && Prog.mcu) {
        if((Prog.mcu->uartNeeds.rxPin == pin) ||
            (Prog.mcu->uartNeeds.txPin == pin))
        {
            i2 = 0; // "<UART needs!>";
        }
    }

    if(PwmFunctionUsed() && Prog.mcu) {
        if(Prog.mcu->pwmNeedsPin == pin) {
            i2 = 0; // "<PWM needs!>";
        }
    }

    for(j = 0; j < Prog.mcu->pinCount; j++) 
	{
        if(Prog.mcu->pinInfo[j].pin == pin) 
		{
			if (b->type == IO_TYPE_DIG_OUTPUT && b->pin == 17)
				i2 = 90000;
			else if (b->type == IO_TYPE_DIG_OUTPUT && b->pin == 18)
				i2 = 90001;
			else if (b->type == IO_TYPE_GENERAL && b->pin > 19)
				i2 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + b->bit; // "M%d.%d"
			else if (b->type == IO_TYPE_DIG_OUTPUT && b->pin > 19)
				i2 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + b->bit; // "M%d.%d"
			else if (b->type == IO_TYPE_DIG_OUTPUT)
				i2 = 40000 + Prog.mcu->pinInfo[j].bit;
			else if (b->type == IO_TYPE_DIG_INPUT && b->pin > 19)
				i2 = 50000 + (Prog.mcu->pinInfo[j].bit * 100) + b->bit;
			else if (b->type == IO_TYPE_DIG_INPUT)
				i2 = 30000 + Prog.mcu->pinInfo[j].bit;
			else if (b->type == IO_TYPE_READ_ADC)
				if (b->pin == 6)
					i2 = 90003;
				else
					i2 = 60000 + Prog.mcu->pinInfo[j].bit;
			else if (b->type == IO_TYPE_READ_ENC || b->type == IO_TYPE_RESET_ENC)
				i2 = 70000 + Prog.mcu->pinInfo[j].bit;
            break;
        }
    }
    if(j == Prog.mcu->pinCount) {
        i2 = 0; // "<not an I/O!>";
    }

    return bSortAscending ? i1 - i2 : i2 - i1;
}

static int CompareIoType(const void *av, const void *bv)
{
    PlcProgramSingleIo *a = (PlcProgramSingleIo *)av;
    PlcProgramSingleIo *b = (PlcProgramSingleIo *)bv;

    //if(a->type != b->type) {
    //    return a->type - b->type;
    //}

    //if(a->pin == NO_PIN_ASSIGNED && b->pin != NO_PIN_ASSIGNED) return  1;
    //if(b->pin == NO_PIN_ASSIGNED && a->pin != NO_PIN_ASSIGNED) return -1;

	return bSortAscending ? a->type - b->type : b->type - a->type;
}

//-----------------------------------------------------------------------------
// Called in response to a notify for the listview. Handles click, text-edit
// operations etc., but also gets called to find out what text to display
// where (LPSTR_TEXTCALLBACK); that way we don't have two parallel copies of
// the I/O list to keep in sync.
//-----------------------------------------------------------------------------
int CALLBACK CompareListItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int nColumn = lParamSort;

	char lpszItem1[MAX_NAME_LEN];
	char lpszItem2[MAX_NAME_LEN];

	LVITEM itm;
	itm.mask = LVIF_TEXT;
	itm.iSubItem = nColumn;

	itm.iItem = lParam1;
	itm.pszText = lpszItem1;
	itm.cchTextMax = sizeof(lpszItem1);

	ListView_GetItem(IoList, &itm);

	itm.iItem = lParam2;
	itm.pszText = lpszItem2;
	itm.cchTextMax = sizeof(lpszItem2);

	ListView_GetItem(IoList, &itm);

	return bSortAscending ? _stricmp(lpszItem1, lpszItem2) : _stricmp(lpszItem2, lpszItem1);
}

void SortList(int NewSortColumn)
{
	// get new sort parameters. If NewSortColumn < 0, use previous settings
	if (NewSortColumn == nSortColumn) {
		bSortAscending = !bSortAscending;
	} else if(NewSortColumn >= 0) {
		nSortColumn = NewSortColumn;
		bSortAscending = TRUE;
	}

	// sort list
	ListView_SortItems(IoList, CompareListItems, nSortColumn);
/*	if (nSortColumn == 0)
		qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo), CompareIoName);
	else if (nSortColumn == 1)
		qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo), CompareIoType);
	else if (nSortColumn == 3)
		qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo), CompareIoPin);
	else if (nSortColumn == 4)
		qsort(Prog.io.assignment, Prog.io.count, sizeof(PlcProgramSingleIo), CompareIoPort);*/

	RefreshControlsToSettings();
}

//-----------------------------------------------------------------------------
// Wipe the I/O list and then re-extract it from the PLC program, taking
// care not to forget the pin assignments. Gets passed the selected item
// as an index into the list; modifies the list, so returns the new selected
// item as an index into the new list.
//-----------------------------------------------------------------------------
int GenerateIoMapList(int prevSel)
{
    int i, j;

    char selName[MAX_NAME_LEN];
    if(prevSel >= 0) {
        strcpy(selName, Prog.io.assignment[prevSel].name);
    }

    if(IoSeenPreviouslyCount > MAX_IO_SEEN_PREVIOUSLY/2) {
        // flush it so there's lots of room, and we don't run out and
        // forget important things
        IoSeenPreviouslyCount = 0;
    }
    
    // remember the pin assignments
    for(i = 0; i < Prog.io.count; i++) {
        AppendIoSeenPreviously(Prog.io.assignment[i].name,
            Prog.io.assignment[i].type, Prog.io.assignment[i].pin, Prog.io.assignment[i].bit);
    }
    // wipe the list
    Prog.io.count = 0;
    // extract the new list so that it must be up to date
    for(i = 0; i < Prog.numRungs; i++) {
        ExtractNamesFromCircuit(ELEM_SERIES_SUBCKT, Prog.rungs[i]);
    }

    for(i = 0; i < Prog.io.count; i++) {
        if(Prog.io.assignment[i].type == IO_TYPE_DIG_INPUT ||
           Prog.io.assignment[i].type == IO_TYPE_DIG_OUTPUT ||
           Prog.io.assignment[i].type == IO_TYPE_GENERAL ||
           Prog.io.assignment[i].type == IO_TYPE_READ_ADC ||
           Prog.io.assignment[i].type == IO_TYPE_READ_ENC ||
           Prog.io.assignment[i].type == IO_TYPE_RESET_ENC ||
		   Prog.io.assignment[i].type == IO_TYPE_READ_USS || 
		   Prog.io.assignment[i].type == IO_TYPE_WRITE_USS)
        {
            for(j = 0; j < IoSeenPreviouslyCount; j++) 
			{
                if(_stricmp(Prog.io.assignment[i].name, IoSeenPreviously[j].name) == 0 && Prog.io.assignment[i].type == IoSeenPreviously[j].type)
                {
                    Prog.io.assignment[i].pin = IoSeenPreviously[j].pin;
					Prog.io.assignment[i].bit = IoSeenPreviously[j].bit;
                    break;
                }
            }
        }
    }

	SortList(-1);

    if(prevSel >= 0) {
        for(i = 0; i < Prog.io.count; i++) {
            if(_stricmp(Prog.io.assignment[i].name, selName)==0)
                break;
        }
        if(i < Prog.io.count)
            return i;
    }
    // no previous, or selected was deleted
    return -1;
}

//-----------------------------------------------------------------------------
// Load the I/O list from a file. Since we are just loading pin assignments,
// put it into IoSeenPreviously so that it will get used on the next
// extraction.
//-----------------------------------------------------------------------------
BOOL LoadIoListFromFile(FILE *f, int version)
{
    char line[80];
    char name[MAX_NAME_LEN];
    int pin, bit = 0;
	int i;
    unsigned int type;

	for (i = 0; i < MAX_IO_SEEN_PREVIOUSLY; i++)
	{
		memset(IoSeenPreviously[i].name, 0, sizeof(IoSeenPreviously[i].name));
		IoSeenPreviously[i].type = 0;
		IoSeenPreviously[i].pin = 0;
		IoSeenPreviously[i].bit = 0;
	}
	IoSeenPreviouslyCount = 0;

    while(fgets(line, sizeof(line), f)) {
        if(_stricmp(line, "END\n")==0) {
            return TRUE;
        }
        // Don't internationalize this! It's the file format, not UI.

		if (version < 2)
		{
			if (sscanf(line, "    %s at %d", name, &pin) != 2) 
			{
				return FALSE;
			}
		}
		else if (version < 3)
		{
			if(sscanf(line, "    %s at %d %d", name, &pin, &bit)!=3) 
			{
				return FALSE;
			}

			switch(name[0]) {
				case 'X': type = IO_TYPE_DIG_INPUT; break;
		        case 'Y': type = IO_TYPE_DIG_OUTPUT; break;
	            case 'A': type = IO_TYPE_READ_ADC; break;
				case 'E': type = IO_TYPE_READ_ENC; break;
			    case 'Z': type = IO_TYPE_RESET_ENC; break;
		        default: oops();
	        }
		}
		else
		{
			if(sscanf(line, "    %s at %d %d type %u", name, &pin, &bit, &type)!=4) 
			{
				return FALSE;
			}
		}

		if(type == IO_TYPE_DIG_OUTPUT && (pin == 17 || pin == 18)) { // Leds - change to internal relay type.
			pin  = 0;
			type = IO_TYPE_INTERNAL_RELAY;
		}

		AppendIoSeenPreviously(name, type, pin, bit);
    }
    return FALSE;
}

//-----------------------------------------------------------------------------
// Write the I/O list to a file. Since everything except the pin assignment
// can be extracted from the schematic, just write the Xs and Ys.
//-----------------------------------------------------------------------------
void SaveIoListToFile(FILE *f)
{
    int i;
    for(i = 0; i < Prog.io.count; i++) {
        if(Prog.io.assignment[i].type == IO_TYPE_DIG_INPUT  ||
           Prog.io.assignment[i].type == IO_TYPE_DIG_OUTPUT ||
           Prog.io.assignment[i].type == IO_TYPE_GENERAL ||
		   Prog.io.assignment[i].type == IO_TYPE_INTERNAL_FLAG ||
           Prog.io.assignment[i].type == IO_TYPE_READ_ADC ||
           Prog.io.assignment[i].type == IO_TYPE_READ_ENC ||
           Prog.io.assignment[i].type == IO_TYPE_RESET_ENC)
        {
            // Don't internationalize this! It's the file format, not UI.
			if(Prog.io.assignment[i].type != IO_TYPE_GENERAL || Prog.io.assignment[i].pin)
	            fprintf(f, "    %s at %d %d type %u\n", Prog.io.assignment[i].name,
		            Prog.io.assignment[i].pin, Prog.io.assignment[i].bit, Prog.io.assignment[i].type);
        }
    }
}

//-----------------------------------------------------------------------------
// Window proc for the contacts dialog box
//-----------------------------------------------------------------------------
static LRESULT CALLBACK IoDialogProc(HWND hwnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg) {
        case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == OkButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
            } else if(h == CancelButton && wParam == BN_CLICKED) {
                DialogDone = TRUE;
                DialogCancel = TRUE;
            } else if(h == PinList && HIWORD(wParam) == LBN_DBLCLK) {
                DialogDone = TRUE;
			} else if (h == PinList && HIWORD(wParam) == LBN_SELCHANGE) {
				bool sem_bit;
				char pin[16], name[MAX_NAME_LEN];
				SendMessage(textLabelName, WM_GETTEXT, (WPARAM)MAX_NAME_LEN, (LPARAM)(name));

				SendMessage(PinList, LB_GETTEXT,
					(WPARAM)SendMessage(PinList, LB_GETCURSEL, 0, 0), (LPARAM)pin);

				sem_bit = atoi(pin)<20;
				if(sem_bit)
					SendMessage(BitCombobox, CB_SETCURSEL, 0, 0);
				EnableWindow(BitCombobox, sem_bit ? FALSE : TRUE);
			}
            break;
        }

        case WM_CLOSE:
        case WM_DESTROY:
            DialogDone = TRUE;
            DialogCancel = TRUE;
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}

//-----------------------------------------------------------------------------
// Create our window class; nothing exciting.
//-----------------------------------------------------------------------------
static BOOL MakeWindowClass()
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)IoDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "POPToolsIo";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 32, 32, 0);
    wc.hIconSm          = (HICON)LoadImage(Instance, MAKEINTRESOURCE(4000),
                            IMAGE_ICON, 16, 16, 0);

    return RegisterClassEx(&wc);
}

static void MakeControls(void)
{
    HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Assign:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        6, 1, 50, 17, IoDialog, NULL, Instance, NULL);
    NiceFont(textLabel);

    textLabelName = CreateWindowEx(0, WC_STATIC, "?",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        57, 1, 64, 17, IoDialog, NULL, Instance, NULL);
    NiceFont(textLabelName);

    PinList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTBOX, "",
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | WS_VSCROLL |
        LBS_NOTIFY, 6, 18, 115, 320, IoDialog, NULL, Instance, NULL);
    FixedFont(PinList);
	
    lblBit = CreateWindowEx(0, WC_STATIC, _("Bit:"),
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
        6, 327, 35, 21, IoDialog, NULL, Instance, NULL);
    NiceFont(lblBit);

	BitCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, WC_COMBOBOX, NULL,
        WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | WS_DISABLED,
        55, 325, 66, 140, IoDialog, NULL, Instance, NULL);
    NiceFont(BitCombobox);

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        10, 365, 107, 23, IoDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        10, 396, 107, 23, IoDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);
}

void mapIO::ShowIoMapDialog(int item)
{
	unsigned long id = getID(item);
	mapDetails detailsIO = getDetails(id);

	if(!Prog.mcu) {
        MessageBox(MainWindow,
            _("No microcontroller has been selected. You must select a "
            "microcontroller before you can assign I/O pins.\r\n\r\n"
            "Select a microcontroller under the Settings menu and try "
            "again."), _("I/O Pin Assignment"), MB_OK | MB_ICONWARNING);
        return;
    }

    if(Prog.mcu->whichIsa == ISA_ANSIC) {
        Error(_("Can't specify I/O assignment for ANSI C target; compile and "
            "see comments in generated source code."));
        return;
    }

    if(Prog.mcu->whichIsa == ISA_INTERPRETED) {
        Error(_("Can't specify I/O assignment for interpretable target; see "
            "comments in reference implementation of interpreter."));
        return;
    }

	if(IsReserved(getName(id))) {
        Error(_("Rename I/O from default name ('%s') before assigning "
			"MCU pin."), getName(id).c_str());
        return;
    }

	if(IsInternalFlag(getName(id))) {
        Error(_("Variáveis internas não podem ser atribuídas."));
        return;
    }

    if(detailsIO.type != eType_DigInput && 
       detailsIO.type != eType_DigOutput &&
       detailsIO.type != eType_General  &&
	   detailsIO.type != eType_ReadADC &&
       detailsIO.type != eType_ReadEnc &&
	   detailsIO.type != eType_ResetEnc)
    {
        Error(_("Can only assign pin number to input/output pins or general variable."));
        return;
    }

    if(detailsIO.type == eType_ReadADC && Prog.mcu->adcCount == 0) {
        Error(_("No ADC or ADC not supported for this micro."));
        return;
    }

	if((detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc) && Prog.mcu->encCount == 0) {
        Error(_("No Encoder or Encoder not supported for selected micro."));
        return;
    }

    MakeWindowClass();

    // We need the TOOLWINDOW style, or else the window will be forced to
    // a minimum width greater than our current width. And without the
    // APPWINDOW style, it becomes impossible to get the window back (by
    // Alt+Tab or taskbar).
    IoDialog = CreateWindowClient(WS_EX_TOOLWINDOW | WS_EX_APPWINDOW,
        "POPToolsIo", _("I/O Pin"),
        WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 127, 430, MainWindow, NULL, Instance, NULL);

    MakeControls();

    char buf[40];
	int i = 0;
	int PinListItemCount = 0;

	for (i = 0; i < sizeof(ComboboxBitItens) / sizeof(ComboboxBitItens[0]); i++)
		SendMessage(BitCombobox, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)ComboboxBitItens[i]));

	SendMessage(BitCombobox, CB_SETCURSEL, Prog.io.assignment[item].bit, 0);
	if(Prog.io.assignment[item].pin >= 20) {
		EnableWindow(BitCombobox, TRUE);
	}

	if(detailsIO.type != eType_DigInput && detailsIO.type != eType_DigOutput) {
		ShowWindow(BitCombobox, 0);
		ShowWindow(lblBit     , 0);

		RECT r, rOK;
		GetWindowRect(PinList , &r  );
		GetWindowRect(OkButton, &rOK);

		r.bottom = rOK.top - 6;

		SetWindowPos(PinList, NULL, 6, 18, r.right - r.left, r.bottom - r.top, SWP_NOZORDER);
	}

    SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)_("(no pin)"));
	PinListItemCount++;

	map<string, pair<unsigned long, mapDetails> >::iterator it;
	for(i = 0; i < Prog.mcu->pinCount; i++) {
        int j;
		for(it = IO.begin(); it != IO.end(); it++) 
		{
			if(it->second.first == id) continue;
			if(it->second.second.pin == Prog.mcu->pinInfo[i].pin && it->second.second.type == detailsIO.type && 
				!((it->second.second.type == eType_DigInput && i > 18) || (it->second.second.type == eType_DigOutput && i > 66)) )
			{
                goto cant_use_this_io;
            }
		}

		if(UartFunctionUsed() && Prog.mcu &&
                ((Prog.mcu->pinInfo[i].pin == Prog.mcu->uartNeeds.rxPin) ||
                 (Prog.mcu->pinInfo[i].pin == Prog.mcu->uartNeeds.txPin)))
        {
            goto cant_use_this_io;
        }

        if(PwmFunctionUsed() && 
            Prog.mcu->pinInfo[i].pin == Prog.mcu->pwmNeedsPin)
        {
            goto cant_use_this_io;
        }

		if(detailsIO.type == eType_ReadADC) {
            for(j = 0; j < Prog.mcu->adcCount; j++) 
			{
				if (j == 4) continue;
                //if(Prog.mcu->adcInfo[j].pin == Prog.mcu->pinInfo[i].pin) 
				//{
				if (j == Prog.mcu->adcCount - 1)
					sprintf(buf, "%3d TEMP", Prog.mcu->adcInfo[j].pin);
				else
					sprintf(buf, "%3d ADC%d", Prog.mcu->adcInfo[j].pin, Prog.mcu->adcInfo[j].muxRegValue);

				SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)buf);
				PinListItemCount++;
                //}
            }
            if(j == Prog.mcu->adcCount) {
				break;
                //goto cant_use_this_io;
            }
        }

		if(detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc) {
            for(j = 0; j < Prog.mcu->encCount; j++) 
			{
				char *strEnc[] = { _("Enc. Inc."), _("Enc. Abs.") };
                //if(Prog.mcu->adcInfo[j].pin == Prog.mcu->pinInfo[i].pin) 
				//{
					sprintf(buf, "%3d %s", Prog.mcu->encInfo[j].pin, strEnc[j]);
					SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)buf);
					PinListItemCount++;
                //}
            }
            if(j == Prog.mcu->encCount) {
				break;
                //goto cant_use_this_io;
            }
        }

		if ((detailsIO.type == eType_DigInput  && i < 51) ||
			(detailsIO.type == eType_DigOutput && i > 50) ||
			(detailsIO.type == eType_General   && i > 66))
		{
			sprintf(buf, "%3d %c%d", Prog.mcu->pinInfo[i].pin,
				Prog.mcu->pinInfo[i].port,
				Prog.mcu->pinInfo[i].bit);

			SendMessage(PinList, LB_ADDSTRING, 0, (LPARAM)buf);
			PinListItemCount++;
		}
cant_use_this_io:;
    }

	SendMessage(textLabelName, WM_SETTEXT, 0, (LPARAM)(getName(id).c_str()));

	EnableWindow(MainWindow, FALSE);
    ShowWindow(IoDialog, TRUE);
    SetFocus(PinList);

	//SendMessage(PinList, LB_SETCURSEL, item, 0);
	for (i = 0; i < PinListItemCount; i++)
	{
		int sel = SendMessage(PinList, LB_GETSEL, i, 0);
        char pin[16];
        SendMessage(PinList, LB_GETTEXT, (WPARAM)i, (LPARAM)pin);
		if (detailsIO.pin == atoi(pin))
			SendMessage(PinList, LB_SETCURSEL, i, 0);
	}

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

        if(IsDialogMessage(IoDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!DialogCancel) {
        int sel = SendMessage(PinList, LB_GETCURSEL, 0, 0);
        char pin[16];
		char buf[16];

		SendMessage(PinList    , LB_GETTEXT, (WPARAM)sel        , (LPARAM)pin);
		SendMessage(BitCombobox, WM_GETTEXT, (WPARAM)sizeof(buf), (LPARAM)(buf));

		Assign(id, atoi(pin), atoi(buf));
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(IoDialog);
    return;
}

// ShowWatchPoint Dialog functions
HWND CheckON;
HWND CheckOFF;
HWND CheckVal;
HWND EnabledWP;

static LONG_PTR PrevWPDialogProc;

void SetWPControlsEnabled()
{
	BOOL enabled = (Button_GetCheck(EnabledWP) == BST_CHECKED ? TRUE : FALSE);

	if(CheckON  != NULL)
		Button_Enable(CheckON , enabled);
	if(CheckOFF != NULL)
		Button_Enable(CheckOFF, enabled);
	if(CheckVal != NULL)
		Edit_Enable  (CheckVal, enabled);
}

static LRESULT CALLBACK WPDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_COMMAND: {
            HWND h = (HWND)lParam;
            if(h == EnabledWP && wParam == BN_CLICKED) {
				SetWPControlsEnabled();
            }
            break;
        }
    }

	return CallWindowProc((WNDPROC)PrevWPDialogProc, hwnd, msg, wParam, lParam);
}

int *ShowWatchPointDialog(char *name, int type, int *curval, int *newval)
{
	int *ret = NULL;
	char buf[20];

	// Create window
    MakeWindowClass();

	HWND WPDialog = CreateWindowClient(0, "POPToolsIo",
        _("Set WatchPoint"), WS_OVERLAPPED | WS_SYSMENU,
        100, 100, 230, 85, MainWindow, NULL, Instance, NULL);

	PrevWPDialogProc = SetWindowLongPtr(WPDialog, GWLP_WNDPROC, (LONG_PTR)WPDialogProc);

	// Create controls
	CheckON  = NULL;
	CheckOFF = NULL;
	CheckVal = NULL;

    EnabledWP = CreateWindowEx(0, WC_BUTTON, _("WatchPoint Enabled"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 10, 140, 21, WPDialog, NULL, Instance, NULL); 
    NiceFont(EnabledWP);

	if(type == IO_TYPE_DIG_INPUT || type == IO_TYPE_DIG_OUTPUT ||
			type == IO_TYPE_INTERNAL_RELAY || type == IO_TYPE_INTERNAL_FLAG) {
		CheckON = CreateWindowEx(0, WC_BUTTON, _("Ligado"),
			WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
			10, 35, 100, 20, WPDialog, NULL, Instance, NULL);
		NiceFont(CheckON);

		CheckOFF = CreateWindowEx(0, WC_BUTTON, _("Desligado"),
			WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE,
			10, 55, 100, 20, WPDialog, NULL, Instance, NULL);
		NiceFont(CheckOFF);

		Button_SetCheck(CheckON , BST_CHECKED  );
		Button_SetCheck(CheckOFF, BST_UNCHECKED);
	} else {
		HWND textLabel = CreateWindowEx(0, WC_STATIC, _("Name:"),
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SS_RIGHT,
			10, 45, 40, 21, WPDialog, NULL, Instance, NULL);
		NiceFont(textLabel);

		CheckVal = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, "",
			WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
			55, 45, 85, 21, WPDialog, NULL, Instance, NULL);
		FixedFont(CheckVal);
	}

    OkButton = CreateWindowEx(0, WC_BUTTON, _("OK"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
        150, 10, 70, 30, WPDialog, NULL, Instance, NULL); 
    NiceFont(OkButton);

    CancelButton = CreateWindowEx(0, WC_BUTTON, _("Cancel"),
        WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE,
        150, 45, 70, 30, WPDialog, NULL, Instance, NULL); 
    NiceFont(CancelButton);

	// Set initial values
	if(curval != NULL) {
		Button_SetCheck(EnabledWP , BM_SETCHECK);
		if(CheckVal == NULL) {
			if(*curval == 0) {
				Button_SetCheck(CheckON , BST_UNCHECKED);
				Button_SetCheck(CheckOFF, BST_CHECKED  );
			}
		} else {
			int val = *curval;
			if(type == IO_TYPE_RTO || type == IO_TYPE_TON || type == IO_TYPE_TOF)
				val = (val * Prog.settings.cycleTime) / 1000;

			sprintf(buf, "%d", val);
			Edit_SetText(CheckVal, buf);
		}
	}

	SetWPControlsEnabled();

	// Dialog Loop
    EnableWindow(MainWindow, FALSE);
    ShowWindow(WPDialog, TRUE);
    SetFocus(EnabledWP);

    MSG msg;
    DialogDone = FALSE;
    DialogCancel = FALSE;
    while(GetMessage(&msg, NULL, 0, 0) && !DialogDone) {
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

        if(IsDialogMessage(WPDialog, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	// Check return
    if(!DialogCancel) {
		if(Button_GetCheck(EnabledWP) == BST_CHECKED && newval != NULL) {
			if(CheckVal == NULL) {
				*newval = (Button_GetCheck(CheckON) == BST_CHECKED);
			} else {
				Edit_GetText(CheckVal, buf, sizeof(buf)-1);
				*newval = atoi(buf);
				if(type == IO_TYPE_RTO || type == IO_TYPE_TON || type == IO_TYPE_TOF)
					*newval = (*newval * 1000) / Prog.settings.cycleTime;
			}

			ret = newval;
		}
    } else {
		ret = curval;
	}

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(WPDialog);
	return ret;
}

void IoMapListProc(NMHDR *h)
{
	char val[20];
	int c = LVN_COLUMNCLICK;
	LPNMLISTVIEW pNM = (LPNMLISTVIEW)h;

	switch(h->code) {
		case LVN_ITEMCHANGED:
			if(pNM->uNewState & LVIS_SELECTED) {
				ladder->selectIO(pNM->iItem);
			} else if(pNM->uOldState & LVIS_SELECTED) {
				ladder->selectIO(0);
			}
			break;

		case LVN_COLUMNCLICK:
			{
				LPNMLISTVIEW pLVInfo = (LPNMLISTVIEW)h;
				SortList(pLVInfo->iSubItem);
			}
			break;
        case LVN_GETDISPINFO: {
            NMLVDISPINFO *i = (NMLVDISPINFO *)h;
            int item = i->item.iItem;
            switch(i->item.iSubItem) {
                case LV_IO_PIN:
					if(InSimulationMode) {
						int val;
						if(GetValWP(Prog.io.assignment[item].name, &val) != NULL) {
							DescribeForIoList(val, Prog.io.assignment[item].type, i->item.pszText);
						} else {
							strcpy(i->item.pszText, "");
						}
					} else {
						// Don't confuse people by displaying bogus pin assignments
						// for the C target.
						if(Prog.mcu && (Prog.mcu->whichIsa == ISA_ANSIC ||
										Prog.mcu->whichIsa == ISA_INTERPRETED) )
						{
							strcpy(i->item.pszText, "");
							break;
						}

						mapDetails detailsIO = ladder->getDetailsIO(ladder->getNameIObyIndex(item));
						if(detailsIO.pin != 0) {
							sprintf(i->item.pszText, "%d", detailsIO.pin);
						} else if(detailsIO.type == eType_DigInput || detailsIO.type == eType_DigOutput ||
							detailsIO.type == eType_ReadEnc || detailsIO.type == eType_ResetEnc ||
							detailsIO.type == eType_ReadADC) {
								strcpy(i->item.pszText, _("(not assigned)"));
						} else {
								strcpy(i->item.pszText, "");
						}
					}
                    break;

                case LV_IO_TYPE: {
                    strcpy(i->item.pszText, ladder->getStringTypeIO(item));
                    break;
                }
                case LV_IO_NAME: {
					string name = ladder->getNameIObyIndex(item);
					strcpy(i->item.pszText, name.c_str());
                    break;
				}

                case LV_IO_PORT: {
                    // Don't confuse people by displaying bogus pin assignments
                    // for the C target.
                    if(Prog.mcu && Prog.mcu->whichIsa == ISA_ANSIC) {
                        strcpy(i->item.pszText, "");
                        break;
                    }

					strcpy(i->item.pszText, ladder->getPortNameIO(item).c_str());

					break;
                }

                case LV_IO_STATE: {
                    if(InSimulationMode) {
                        char *name = Prog.io.assignment[item].name;
                        DescribeForIoList(name, Prog.io.assignment[item].type, i->item.pszText);
                    } else {
                        strcpy(i->item.pszText, "");
                    }
                    break;
                }

            }
            break;
        }
        case LVN_ITEMACTIVATE: {
			NMITEMACTIVATE *i = (NMITEMACTIVATE *)h;
            if(InSimulationMode) {
				char *name = Prog.io.assignment[i->iItem].name;
				if(i->iSubItem != LV_IO_PIN) {
					if(Prog.io.assignment[i->iItem].type == IO_TYPE_DIG_INPUT) {
						SimulationToggleContact(name);
					} else if(Prog.io.assignment[i->iItem].type == IO_TYPE_READ_ADC) {
						ShowAnalogSliderPopup(name);
					} else if(Prog.io.assignment[i->iItem].type == IO_TYPE_READ_ENC) {
						ShowEncoderSliderPopup(name);
					} else if(Prog.io.assignment[i->iItem].type == IO_TYPE_COUNTER ||
							Prog.io.assignment[i->iItem].type == IO_TYPE_TOF ||
							Prog.io.assignment[i->iItem].type == IO_TYPE_TON ||
							Prog.io.assignment[i->iItem].type == IO_TYPE_GENERAL) {
						sprintf(val, "%d", GetSimulationVariable(name));
						ShowSimulationVarSetDialog(name, val);
						SetSimulationVariable(name, atoi(val));
					}
				} else {
					int wpval;
					if(ShowWatchPointDialog(name, Prog.io.assignment[i->iItem].type, GetValWP(name, &wpval), &wpval) != NULL) {
						AddWP(name, wpval);
					} else {
						RemoveWP(name);
					}
					InvalidateRect(IoList, NULL, FALSE);
				}
            } else {
                ladder->ShowIoMapDialog(i->iItem);
                InvalidateRect(MainWindow, NULL, FALSE);
            }
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Dialog proc for the popup that lets you set the value of an analog input for
// simulation.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK AnalogSliderDialogProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            AnalogSliderDone = TRUE;
            AnalogSliderCancel = TRUE;
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

//-----------------------------------------------------------------------------
// A little toolbar-style window that pops up to allow the user to set the
// simulated value of an ADC pin.
//-----------------------------------------------------------------------------
void ShowAnalogSliderPopup(char *name)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)AnalogSliderDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "POPToolsAnalogSlider";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    POINT pt;
    GetCursorPos(&pt);

    SWORD currentVal = GetAdcShadow(name);

    SWORD maxVal;
    if(Prog.mcu) {
        maxVal = Prog.mcu->adcMax;
    } else {
        maxVal = 1023;
    }
    if(maxVal == 0) {
        Error(_("No ADC or ADC not supported for selected micro."));
        return;
    }

    int left = pt.x - 10;
    // try to put the slider directly under the cursor (though later we might
    // realize that that would put the popup off the screen)
    int top = pt.y - (15 + (73*currentVal)/maxVal);

    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    if(top + 110 >= r.bottom) {
        top = r.bottom - 110;
    }
    if(top < 0) top = 0;
    
    AnalogSliderMain = CreateWindowClient(0, "POPToolsAnalogSlider", _("I/O Pin"),
        WS_VISIBLE | WS_POPUP | WS_DLGFRAME,
        left, top, 30, 100, MainWindow, NULL, Instance, NULL);

    AnalogSliderTrackbar = CreateWindowEx(0, TRACKBAR_CLASS, "", WS_CHILD |
        TBS_AUTOTICKS | TBS_VERT | TBS_TOOLTIPS | WS_CLIPSIBLINGS | WS_VISIBLE, 
        0, 0, 30, 100, AnalogSliderMain, NULL, Instance, NULL);
    SendMessage(AnalogSliderTrackbar, TBM_SETRANGE, FALSE,
        MAKELONG(0, maxVal));
    SendMessage(AnalogSliderTrackbar, TBM_SETTICFREQ, (maxVal + 1)/8, 0);
    SendMessage(AnalogSliderTrackbar, TBM_SETPOS, TRUE, currentVal);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(AnalogSliderMain, TRUE);
    SetFocus(AnalogSliderTrackbar);

    DWORD ret;
    MSG msg;
    AnalogSliderDone = FALSE;
    AnalogSliderCancel = FALSE;

    SWORD orig = GetAdcShadow(name);

    while(!AnalogSliderDone && (ret = GetMessage(&msg, NULL, 0, 0))) {
        SWORD v = (SWORD)SendMessage(AnalogSliderTrackbar, TBM_GETPOS, 0, 0);

        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                AnalogSliderDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                AnalogSliderDone = TRUE;
                AnalogSliderCancel = TRUE;
                break;
            }
        } else if(msg.message == WM_LBUTTONUP) {
            if(v != orig) {
                AnalogSliderDone = TRUE;
            }
        }
        SetAdcShadow(name, v);

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!AnalogSliderCancel) {
        SWORD v = (SWORD)SendMessage(AnalogSliderTrackbar, TBM_GETPOS, 0, 0);
        SetAdcShadow(name, v);
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(AnalogSliderMain);
    ListView_RedrawItems(IoList, 0, Prog.io.count - 1);
}

//-----------------------------------------------------------------------------
// Dialog proc for the popup that lets you set the value of an analog input for
// simulation.
//-----------------------------------------------------------------------------
static LRESULT CALLBACK EncoderSliderDialogProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            EncoderSliderDone = TRUE;
            EncoderSliderCancel = TRUE;
            return 1;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

//-----------------------------------------------------------------------------
// A little toolbar-style window that pops up to allow the user to set the
// simulated value of an ADC pin.
//-----------------------------------------------------------------------------
void ShowEncoderSliderPopup(char *name)
{
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);

    wc.style            = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_OWNDC |
                            CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)EncoderSliderDialogProc;
    wc.hInstance        = Instance;
    wc.hbrBackground    = (HBRUSH)COLOR_BTNSHADOW;
    wc.lpszClassName    = "POPToolsEncoderSlider";
    wc.lpszMenuName     = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    POINT pt;
    GetCursorPos(&pt);

    SWORD currentVal = GetEncShadow(name);

    SWORD maxVal;
    if(Prog.mcu) {
        maxVal = Prog.mcu->encMax;
    } else {
        maxVal = 0x7FFFFFFF;
    }
    if(maxVal == 0) {
        Error(_("No Encoder or Encoder not supported for selected micro."));
        return;
    }

    int left = pt.x - 10;
    // try to put the slider directly under the cursor (though later we might
    // realize that that would put the popup off the screen)
    int top = pt.y - 55; //- (15 + (73*currentVal)/0x7fff);

    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    if(top + 110 >= r.bottom) {
        top = r.bottom - 110;
    }
    if(top < 0) top = 0;

    EncoderSliderMain = CreateWindowClient(0, "POPToolsEncoderSlider", _("I/O Pin"),
        WS_VISIBLE | WS_POPUP | WS_DLGFRAME,
        left, top, 30, 100, MainWindow, NULL, Instance, NULL);

    EncoderSliderTrackbar = CreateWindowEx(0, TRACKBAR_CLASS, "", WS_CHILD |
        TBS_AUTOTICKS | TBS_VERT | TBS_TOOLTIPS | WS_CLIPSIBLINGS | WS_VISIBLE, 
        0, 0, 30, 100, EncoderSliderMain, NULL, Instance, NULL);

    SendMessage(EncoderSliderTrackbar, TBM_SETRANGEMIN, FALSE,
        /*(maxVal + 1) * -1*/ 0);  // bug com valor negativo
    SendMessage(EncoderSliderTrackbar, TBM_SETRANGEMAX, FALSE,
        maxVal - 1);
    SendMessage(EncoderSliderTrackbar, TBM_SETTICFREQ, (maxVal)/8, 0);
    SendMessage(EncoderSliderTrackbar, TBM_SETPOS, TRUE, currentVal);

    EnableWindow(MainWindow, FALSE);
    ShowWindow(EncoderSliderMain, TRUE);
    SetFocus(EncoderSliderTrackbar);

    DWORD ret;
    MSG msg;
    EncoderSliderDone = FALSE;
    EncoderSliderCancel = FALSE;

    SWORD orig = GetEncShadow(name);

    while(!EncoderSliderDone && (ret = GetMessage(&msg, NULL, 0, 0))) {
        SWORD v = (SWORD)SendMessage(EncoderSliderTrackbar, TBM_GETPOS, 0, 0);

        if(msg.message == WM_KEYDOWN) {
            if(msg.wParam == VK_RETURN) {
                EncoderSliderDone = TRUE;
                break;
            } else if(msg.wParam == VK_ESCAPE) {
                EncoderSliderDone = TRUE;
                EncoderSliderCancel = TRUE;
                break;
            }
        } else if(msg.message == WM_LBUTTONUP) {
            if(v != orig) {
                EncoderSliderDone = TRUE;
            }
        }
        SetEncShadow(name, v);

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(!EncoderSliderCancel) {
        SWORD v = (SWORD)SendMessage(EncoderSliderTrackbar, TBM_GETPOS, 0, 0);
        SetEncShadow(name, v);
    }

    EnableWindow(MainWindow, TRUE);
    DestroyWindow(EncoderSliderMain);
    ListView_RedrawItems(IoList, 0, Prog.io.count - 1);
}

// Returns if io is associated to a ModBUS register or not.
int IoMap_IsModBUS(PlcProgramSingleIo *io)
{
	if(io == NULL || io->pin == NO_PIN_ASSIGNED) return 0;

	if(io->pin < 20)
		return 0;

	return 1;
}

// Returns index of register associated to pin. Examples:
// Pin associated to Digital Input 3: returns 2
// Pin associated to ModBUS register 5: returns 4
// Please note that there is no way to know if the index is for a Digital
// Input or ModBUS register. To know this, please call IoMap_IsModBUS()
int IoMap_GetIndex(PlcProgramSingleIo *io)
{
	int ret = 0;
	if(io == NULL || io->pin == NO_PIN_ASSIGNED) return 0;

	if(io->type == IO_TYPE_DIG_INPUT) {
		ret = IoMap_IsModBUS(io) ? io->pin - 20 : io->pin - 1;
	} else if(io->type == IO_TYPE_DIG_OUTPUT) {
		ret = IoMap_IsModBUS(io) ? io->pin - 20 : io->pin - 1;
	} else if(io->type == IO_TYPE_GENERAL) {
		ret = io->pin - 20;
	}

	return ret;
}
