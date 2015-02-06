/* Includes ------------------------------------------------------------------- */
#include "qei.h"
#include "ssi.h"

#define ENC_DEVICE_INC 0
#define ENC_DEVICE_ABS 1

#define ENCODER_MODE_M   1
#define ENCODER_MODE_MM  2
#define ENCODER_MODE_TMM 3

int  ENC_Read  (unsigned int device);
void ENC_Reset (unsigned int device, int offset);
void ENC_Config(unsigned int device, int mode, int perimeter, float factor, int ppr);

/* --------------------------------- End Of File ------------------------------ */
