#include "eeprom_24AA02.h"

void         E2P_Init (void);
int          E2P_Busy (void);
unsigned int E2P_Read (void *data, unsigned char address, int size);
unsigned int E2P_Write(void *data, unsigned char address, int size);
