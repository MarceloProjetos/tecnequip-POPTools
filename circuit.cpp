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
	ladder->CheckpointBegin(_("Adicionar Coment�rio"));
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
		break;

	case ELEM_ONE_SHOT_RISING:
	case ELEM_ONE_SHOT_FALLING:
		ret = ladder->AddElement(new LadderElemOneShot(ladder, which));
		break;
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
	ladder->CheckpointBegin(_("Adicionado Rel� Mestre"));
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
	ladder->CheckpointBegin(_("Adicionar Fila de Vari�veis"));
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
	ladder->CheckpointBegin(_("Adicionar Lineariza��o por Segmentos"));
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
		ladder->CheckpointBegin(_("Adicionar Opera��o Somar"));
		break;

	case ELEM_SUB:
		ladder->CheckpointBegin(_("Adicionar Opera��o Subtrair"));
		break;

	case ELEM_MUL:
		ladder->CheckpointBegin(_("Adicionar Opera��o Multiplicar"));
		break;

	case ELEM_DIV:
		ladder->CheckpointBegin(_("Adicionar Opera��o Dividir"));
		break;

	default:
		ladder->CheckpointBegin(_("Adicionar Opera��o Matem�tica"));
		break;
	}

	bool ret = ladder->AddElement(new LadderElemMath(ladder, which));

	ladder->CheckpointEnd();

	return ret;
}

bool AddRand(void)
{
	ladder->CheckpointBegin(_("Adicionar N�mero Rand�mico"));
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
	ladder->CheckpointBegin(_("Adicionar Rampa de Acelera��o/Desacelera��o"));
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
	ladder->CheckpointBegin(_("Adicionar Sa�da PWM"));
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
	ladder->NewDiagram();
}
