#include <string.h>
#include "i2c.h"

void         E2P_24AA02_Init (void);
int          E2P_24AA02_Busy (void);
unsigned int E2P_24AA02_Write(void *data, unsigned char address, int size);
unsigned int E2P_24AA02_Read (void *data, unsigned char address, int size);
