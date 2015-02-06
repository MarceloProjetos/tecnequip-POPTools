#ifndef __SSI_H__
#define __SSI_H__

/******************************************************************************
* Encoder SSI
******************************************************************************/
#define SSI_MODE_GRAY 0
#define SSI_MODE_BIN  1

void         SSI_Init(unsigned int size, unsigned int mode);
unsigned int SSI_Read(void);
void         SSI_SetPosition(int offset);

#endif
