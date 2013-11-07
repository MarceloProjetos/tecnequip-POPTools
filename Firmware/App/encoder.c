/* Includes ------------------------------------------------------------------- */
#include "encoder.h"

int   INC_Mode     , ABS_Mode;
int   INC_Offset   ;
int   INC_Perimeter, ABS_Perimeter;
int   INC_PPR      , ABS_PPR;
int   INC_Factor10k, ABS_Factor10k;

int ENC_Read(unsigned int device)
{
	int mult = 1, div = 1;
	int mode, perimeter, ppr;
	float factor;
	int val, offset;

	switch(device) {
		default:
		case ENC_DEVICE_INC:
			mode      = INC_Mode;
			perimeter = INC_Perimeter;
			factor    = (float)(INC_Factor10k)/10000;
			ppr       = INC_PPR;
			offset    = INC_Offset;
			val = QEI_GetPosition(QEI) + INC_Offset;
			break;

		case ENC_DEVICE_ABS:
			mode      = ABS_Mode;
			perimeter = ABS_Perimeter;
			factor    = (float)(ABS_Factor10k)/10000;
			ppr       = ABS_PPR;
			val = (int)SSI_Read(); break;
	}

	switch(mode) {
		case ENCODER_MODE_M:
			div = 1000;
			break;

		case ENCODER_MODE_MM:
			// Nada a fazer...
			break;

		case ENCODER_MODE_TMM:
			mult = 10;
			break;

		default:
			return val;
	}

	return (val * perimeter * factor * mult) / (ppr * div);
}

void ENC_Reset(unsigned int device, int offset)
{
	int mult = 1, div = 1;
	int mode, perimeter, ppr;
	float factor;

	switch(device) {
		default:
		case ENC_DEVICE_INC:
			mode      = INC_Mode;
			perimeter = INC_Perimeter;
			factor    = (float)(INC_Factor10k)/10000;
			ppr       = INC_PPR;
			break;

		case ENC_DEVICE_ABS:
			mode      = ABS_Mode;
			perimeter = ABS_Perimeter;
			factor    = (float)(ABS_Factor10k)/10000;
			ppr       = ABS_PPR;
	}

	if(mode) {
		switch(mode) {
			case ENCODER_MODE_M:
				div = 1000;
				break;

			case ENCODER_MODE_MM:
				// Nada a fazer...
				break;

			case ENCODER_MODE_TMM:
				mult = 10;
				break;
		}

		offset = (offset * ppr * div) / (perimeter * mult);
	}

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

void ENC_Config(unsigned int device, int mode, int perimeter, float factor, int ppr)
{
	switch(device) {
		case ENC_DEVICE_INC:
			INC_Mode      = mode;
			INC_Perimeter = perimeter;
			INC_Factor10k = (int)(factor*10000);
			INC_PPR       = ppr;
			break;

		case ENC_DEVICE_ABS:
			ABS_Mode      = mode;
			ABS_Perimeter = perimeter;
			ABS_Factor10k = (int)(factor*10000);
			ABS_PPR       = 1UL << ppr;
			break;
	}
}

/* --------------------------------- End Of File ------------------------------ */

