#include "poptools.h"

//-----------------------------------------------------------------------------
// Routines to allocate memory for a new circuit element (contact, coil, etc.)
// and insert it into the current program with AddLeaf. Fill in some default
// parameters, name etc. when we create the leaf; user can change them later.
//-----------------------------------------------------------------------------
bool AddParallelStart(void)
{
	return ladder->AddParallelStart();
}

bool AddComment(char *str)
{
	ladder->CheckpointBegin(_("Adicionar Comentário"));
	bool ret = ladder->AddElement(new LadderElemComment(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddContact(void)
{
	ladder->CheckpointBegin(_("Adicionar Contato"));
	bool ret = ladder->AddElement(new LadderElemContact(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddCoil(void)
{
	ladder->CheckpointBegin(_("Adicionar Bobina"));
	bool ret = ladder->AddElement(new LadderElemCoil(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddTimer(int which)
{
	switch(which) {
	case ELEM_TON:
		ladder->CheckpointBegin(_("Adicionar Atraso para Ligar"));
		break;

	case ELEM_TOF:
		ladder->CheckpointBegin(_("Adicionar Atraso para Desligar"));
		break;

	case ELEM_RTO:
		ladder->CheckpointBegin(_("Adicionar Atraso Retentivo para Ligar"));
		break;

	default:
		ladder->CheckpointBegin(_("Adicionar Temporizador"));
		break;
	}

	bool ret = ladder->AddElement(new LadderElemTimer(ladder, which));

	ladder->CheckpointEnd();

	return ret;
}

bool AddRTC(int which)
{
	ladder->CheckpointBegin(_("Adicionar Agendador de Tarefas"));
	bool ret = ladder->AddElement(new LadderElemRTC(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddEmpty(int which)
{
	bool ret;

	switch(which) {
	case ELEM_OPEN:
		ladder->CheckpointBegin(_("Adicionar Circuito Aberto"));
		break;

	case ELEM_SHORT:
		ladder->CheckpointBegin(_("Adicionar Circuito Fechado"));
		break;

	case ELEM_ONE_SHOT_RISING:
		ladder->CheckpointBegin(_("Adicionar Borda de Subida"));
		break;

	case ELEM_ONE_SHOT_FALLING:
		ladder->CheckpointBegin(_("Adicionar Borda de Descida"));
		break;

	default:
		ladder->CheckpointBegin(_("Adicionar Elemento"));
		break;
	}

	switch(which) {
	case ELEM_OPEN:
	case ELEM_SHORT:
		ret = ladder->AddElement(new LadderElemOpenShort(ladder, which));

	case ELEM_ONE_SHOT_RISING:
	case ELEM_ONE_SHOT_FALLING:
		ret = ladder->AddElement(new LadderElemOneShot(ladder, which));
	}

	ladder->CheckpointEnd();

	return ret;
}

bool AddReset(void)
{
	ladder->CheckpointBegin(_("Adicionar Zerar Temporizador/Contador"));
	bool ret = ladder->AddElement(new LadderElemReset(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddMasterRelay(void)
{
	ladder->CheckpointBegin(_("Adicionado Relé Mestre"));
	bool ret = ladder->AddElement(new LadderElemMasterRelay(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddSetBit(void)
{
	ladder->CheckpointBegin(_("Adicionar Liga/Desliga Bit"));
	bool ret = ladder->AddElement(new LadderElemSetBit(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddCheckBit(void)
{
	ladder->CheckpointBegin(_("Adicionar Checar Bit"));
	bool ret = ladder->AddElement(new LadderElemCheckBit(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddShiftRegister(void)
{
	ladder->CheckpointBegin(_("Adicionar Fila de Variáveis"));
	bool ret = ladder->AddElement(new LadderElemShiftRegister(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddLookUpTable(void)
{
	ladder->CheckpointBegin(_("Adicionar Tabela de Busca"));
	bool ret = ladder->AddElement(new LadderElemLUT(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddPiecewiseLinear(void)
{
	ladder->CheckpointBegin(_("Adicionar Linearização por Segmentos"));
	bool ret = ladder->AddElement(new LadderElemPiecewise(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddMove(void)
{
	ladder->CheckpointBegin(_("Adicionar Mover Valor"));
	bool ret = ladder->AddElement(new LadderElemMove(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddSqrt(void)
{
	ladder->CheckpointBegin(_("Adicionar Raiz Quadrado"));
	bool ret = ladder->AddElement(new LadderElemSqrt(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddMath(int which)
{
	switch(which) {
	case ELEM_ADD:
		ladder->CheckpointBegin(_("Adicionar Operação Somar"));
		break;

	case ELEM_SUB:
		ladder->CheckpointBegin(_("Adicionar Operação Subtrair"));
		break;

	case ELEM_MUL:
		ladder->CheckpointBegin(_("Adicionar Operação Multiplicar"));
		break;

	case ELEM_DIV:
		ladder->CheckpointBegin(_("Adicionar Operação Dividir"));
		break;

	default:
		ladder->CheckpointBegin(_("Adicionar Operação Matemática"));
		break;
	}

	bool ret = ladder->AddElement(new LadderElemMath(ladder, which));

	ladder->CheckpointEnd();

	return ret;
}

bool AddRand(void)
{
	ladder->CheckpointBegin(_("Adicionar Número Randômico"));
	bool ret = ladder->AddElement(new LadderElemRand(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddAbs(void)
{
	ladder->CheckpointBegin(_("Adicionar Valor Absoluto"));
	bool ret = ladder->AddElement(new LadderElemAbs(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddCmp(int which)
{
	switch(which) {
	case ELEM_EQU:
		ladder->CheckpointBegin(_("Adicionar Condicional Igual a "));
		break;

	case ELEM_NEQ:
		ladder->CheckpointBegin(_("Adicionar Condicional Diferente de"));
		break;

	case ELEM_GRT:
		ladder->CheckpointBegin(_("Adicionar Condicional Maior que "));
		break;

	case ELEM_GEQ:
		ladder->CheckpointBegin(_("Adicionar Condicional Maior ou igual a"));
		break;

	case ELEM_LES:
		ladder->CheckpointBegin(_("Adicionar Condicional Menor que "));
		break;

	case ELEM_LEQ:
		ladder->CheckpointBegin(_("Adicionar Condicional Menor ou igual a"));
		break;

	default:
		ladder->CheckpointBegin(_("Adicionar Condicional"));
		break;
	}

	bool ret = ladder->AddElement(new LadderElemCmp(ladder, which));

	ladder->CheckpointEnd();

	return ret;
}

bool AddCounter(int which)
{
	switch(which) {
	case ELEM_CTU:
		ladder->CheckpointBegin(_("Adicionar Contador Incremental"));
		break;

	case ELEM_CTD:
		ladder->CheckpointBegin(_("Adicionar Contador Decremental"));
		break;

	case ELEM_CTC:
		ladder->CheckpointBegin(_("Adicionar Contador Circular"));
		break;

	default:
		ladder->CheckpointBegin(_("Adicionar Contador"));
		break;
	}

	bool ret = ladder->AddElement(new LadderElemCounter(ladder, which));

	ladder->CheckpointEnd();

	return ret;
}

bool AddSetDA(void)
{
	ladder->CheckpointBegin(_("Adicionar Escrita no D/A"));
	bool ret = ladder->AddElement(new LadderElemSetDa(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddReadAdc(void)
{
	ladder->CheckpointBegin(_("Adicionar Leitura do A/D"));
	bool ret = ladder->AddElement(new LadderElemReadAdc(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddReadEnc(void)
{
	ladder->CheckpointBegin(_("Adicionar Leitura do Encoder"));
	bool ret = ladder->AddElement(new LadderElemReadEnc(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddResetEnc(void)
{
	ladder->CheckpointBegin(_("Adicionar Escrita no Encoder"));
	bool ret = ladder->AddElement(new LadderElemResetEnc(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddMultisetDA(void)
{
	ladder->CheckpointBegin(_("Adicionar Rampa de Aceleração/Desaceleração"));
	bool ret = ladder->AddElement(new LadderElemMultisetDA(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddReadFormatString(void)
{
	ladder->CheckpointBegin(_("Adicionar Ler String Formatada"));
	bool ret = ladder->AddElement(new LadderElemFmtString(ladder, ELEM_READ_FORMATTED_STRING));
	ladder->CheckpointEnd();

	return ret;
}

bool AddWriteFormatString(void)
{
	ladder->CheckpointBegin(_("Adicionar Escrever String Formatada"));
	bool ret = ladder->AddElement(new LadderElemFmtString(ladder, ELEM_WRITE_FORMATTED_STRING));
	ladder->CheckpointEnd();

	return ret;
}

bool AddReadServoYaskawa(void)
{
	ladder->CheckpointBegin(_("Adicionar Leitura de NS-600 Yaskawa"));
	bool ret = ladder->AddElement(new LadderElemYaskawa(ladder, ELEM_READ_SERVO_YASKAWA));
	ladder->CheckpointEnd();

	return ret;
}

bool AddWriteServoYaskawa(void)
{
	ladder->CheckpointBegin(_("Adicionar Escrita de NS-600 Yaskawa"));
	bool ret = ladder->AddElement(new LadderElemYaskawa(ladder, ELEM_WRITE_SERVO_YASKAWA));
	ladder->CheckpointEnd();

	return ret;
}

bool AddReadUSS(void)
{
	ladder->CheckpointBegin(_("Adicionar Leitura USS"));
	bool ret = ladder->AddElement(new LadderElemUSS(ladder, ELEM_READ_USS));
	ladder->CheckpointEnd();

	return ret;
}

bool AddWriteUSS(void)
{
	ladder->CheckpointBegin(_("Adicionar Escrita USS"));
	bool ret = ladder->AddElement(new LadderElemUSS(ladder, ELEM_WRITE_USS));
	ladder->CheckpointEnd();

	return ret;
}

bool AddReadModbus(void)
{
	ladder->CheckpointBegin(_("Adicionar Ler Registrador ModBUS"));
	bool ret = ladder->AddElement(new LadderElemModBUS(ladder, ELEM_READ_MODBUS));
	ladder->CheckpointEnd();

	return ret;
}

bool AddWriteModbus(void)
{
	ladder->CheckpointBegin(_("Adicionar Escrever Registrador ModBUS"));
	bool ret = ladder->AddElement(new LadderElemModBUS(ladder, ELEM_WRITE_MODBUS));
	ladder->CheckpointEnd();

	return ret;
}

bool AddSetPwm(void)
{
	ladder->CheckpointBegin(_("Adicionar Saída PWM"));
	bool ret = ladder->AddElement(new LadderElemSetPWM(ladder));
	ladder->CheckpointEnd();

	return ret;
}

bool AddUart(int which)
{
	switch(which) {
	case ELEM_CTU:
		ladder->CheckpointBegin(_("Adicionar Leitura de Caracter da RS-485"));
		break;

	case ELEM_CTC:
		ladder->CheckpointBegin(_("Adicionar Escrita de Caracter na RS-485"));
		break;

	default:
		ladder->CheckpointBegin(_("Adicionar Elemento"));
		break;
	}

	bool ret = ladder->AddElement(new LadderElemUART(ladder, which));

	ladder->CheckpointEnd();

	return ret;
}

bool AddPersist(void)
{
	ladder->CheckpointBegin(_("Adicionar Persistente"));
	bool ret = ladder->AddElement(new LadderElemPersist(ladder));
	ladder->CheckpointEnd();

	return ret;
}

//-----------------------------------------------------------------------------
// Delete the selected item from the program. Just call
// DeleteSelectedFromSubckt on every rung till we find it.
//-----------------------------------------------------------------------------
bool DeleteSelectedFromProgram(void)
{
	return ladder->DelElement();
}

//-----------------------------------------------------------------------------
// Free the entire program.
//-----------------------------------------------------------------------------
void FreeEntireProgram(void)
{
    Prog.numRungs = 0;
    Prog.settings.cycleTime = 10000;
    Prog.settings.mcuClock = 100000000;
    Prog.settings.baudRate = 9600;
	Prog.settings.UART = 0;
	Prog.settings.ModBUSID = 0;
    Prog.io.count = 0;
    Prog.mcu = NULL;

	memset(&Prog.settings.ip, 0, sizeof(Prog.settings.ip));
	memset(&Prog.settings.mask, 0, sizeof(Prog.settings.mask));
	memset(&Prog.settings.gw, 0, sizeof(Prog.settings.gw));
	memset(&Prog.settings.dns, 0, sizeof(Prog.settings.dns));
	memset(&Prog.settings.sntp, 0, sizeof(Prog.settings.sntp));

	memset(&Prog.rungHasBreakPoint, 0, sizeof(Prog.rungHasBreakPoint));

	strcpy(Prog.settings.InfoName, "");
	strcpy(Prog.settings.InfoDeveloper, "");
	strcpy(Prog.settings.InfoDescription, "");
	strcpy(Prog.settings.InfoFWVersion, "1.5");
	Prog.settings.InfoBuildNumber = 0;
	Prog.settings.InfoCompileDate = 0;
	Prog.settings.InfoProgramDate = 0;

	strcpy(Prog.settings.sntp, "br.pool.ntp.org");

	Prog.settings.gmt = 9;
	Prog.settings.dailysave = 0;

	Prog.settings.ip[0] = 192;
	Prog.settings.ip[1] = 168;
	Prog.settings.ip[2] = 0;
	Prog.settings.ip[3] = 254;

	Prog.settings.mask[0] = 255;
	Prog.settings.mask[1] = 255;
	Prog.settings.mask[2] = 255;
	Prog.settings.mask[3] = 0;

	Prog.settings.gw[0] = 192;
	Prog.settings.gw[1] = 168;
	Prog.settings.gw[2] = 0;
	Prog.settings.gw[3] = 1;

	Prog.settings.dns[0] = 192;
	Prog.settings.dns[1] = 168;
	Prog.settings.dns[2] = 0;
	Prog.settings.dns[3] = 1;

	Prog.settings.enc_inc_conv_mode = ENCODER_MODE_RAW;
	Prog.settings.perimeter = 400;
	Prog.settings.pulses = 1024;
	Prog.settings.factor = 1;
	Prog.settings.x4 = 1;

	Prog.settings.canSave = TRUE;

	Prog.settings.ssi_mode = 0;
	Prog.settings.ssi_size = 24;
	Prog.settings.enc_ssi_conv_mode = ENCODER_MODE_RAW;
	Prog.settings.ssi_perimeter = 400;
	Prog.settings.ssi_factor = 1;
	Prog.settings.ssi_size_bpr = 12;

	Prog.settings.ramp_abort_mode = RAMP_ABORT_LEAVE;

	int i;
	for(i = 0; i < MAX_IO; i++)
	{
		memset(Prog.io.assignment[i].name, 0, sizeof(Prog.io.assignment[i].name));
		Prog.io.assignment[i].bit = 0;
		Prog.io.assignment[i].pin = 0;
		Prog.io.assignment[i].type = 0;
	}

	for(i = 0; i < NUM_SUPPORTED_MCUS; i++) 
	{
		if(_stricmp(SupportedMcus[i].mcuName, DEFAULT_CPU)==0) 
		{
			Prog.mcu = &SupportedMcus[i];
			break;
		}
	}

    strcpy(CurrentCompileFile, "");
}

//-----------------------------------------------------------------------------
// Delete the rung that contains the cursor.
//-----------------------------------------------------------------------------
bool DeleteSelectedRung(void)
{
	LadderContext context = ladder->getContext();
	if(!context.canDeleteRung) {
        Error(_("Cannot delete rung; program must have at least one rung."));
        return false;
    }

	ladder->DeleteRung(-1);

	return true;
}

//-----------------------------------------------------------------------------
// Insert a rung either before or after the rung that contains the cursor.
//-----------------------------------------------------------------------------
bool InsertRung(bool afterCursor)
{
	ladder->NewRung(afterCursor);

	return true; // Sempre insere uma nova linha!
}

//-----------------------------------------------------------------------------
// When up is false: Swap the row containing the selected element with the one
// under it, or do nothing if the rung is the last in the program.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// When up is true: Swap the row containing the selected element with the one
// above it, or do nothing if the rung is the first in the program.
//-----------------------------------------------------------------------------
bool PushRung(bool up)
{
	return ladder->PushRung(-1, up);
}

//-----------------------------------------------------------------------------
// Start a new project. Give them one rung, with a coil (that they can
// never delete) and nothing else.
//-----------------------------------------------------------------------------
void NewProgram(void)
{
	ladder->ClearDiagram();
}

//-----------------------------------------------------------------------------
// Are either of the UART functions (send or recv) used? Need to know this
// to know whether we must receive their pins.
//-----------------------------------------------------------------------------
BOOL UartFunctionUsed(void)
{
    return FALSE;
}

//-----------------------------------------------------------------------------
// Is the PWM function used? Need to know this to know whether we must reserve
// the pin.
//-----------------------------------------------------------------------------
BOOL PwmFunctionUsed(void)
{
    return FALSE;
}
