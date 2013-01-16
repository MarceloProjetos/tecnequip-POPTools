/* Includes ------------------------------------------------------------------- */
#include "encoder.h"

int INC_Offset, ABS_Offset;

int ENC_Read(unsigned int device)
{
	int val, offset;

	switch(device) {
		case ENC_DEVICE_INC:
			offset = INC_Offset;
			val = QEI_GetPosition(QEI) + INC_Offset;
			break;

		case ENC_DEVICE_ABS:
			val = (int)SSI_Read(); break;
	}

	return val;
}

void ENC_Reset(unsigned int device, int offset)
{
	switch(device) {
		case ENC_DEVICE_INC:
			INC_Offset = offset;
			QEI_Reset(QEI, QEI_CON_RESP | QEI_CON_RESV | QEI_CON_RESI | QEI_CON_RESV);
			break;

		case ENC_DEVICE_ABS:
			SSI_SetPosition(offset);
			break;
	}
}

/* --------------------------------- End Of File ------------------------------ */

