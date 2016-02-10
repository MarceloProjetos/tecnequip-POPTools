#include "XP_io.h"

void XP_input_Update (struct strExpansionBoard *board)
{
	XP_SetAddress(board->address);
	board->value.port = XP_Read_Int();
}

void XP_output_Update(struct strExpansionBoard *board)
{
	XP_SetAddress(board->address);
	XP_Write_Byte(board->value.port);
}

void XP_ad_Update    (struct strExpansionBoard *board)
{

}

void XP_da_Update    (struct strExpansionBoard *board)
{

}
