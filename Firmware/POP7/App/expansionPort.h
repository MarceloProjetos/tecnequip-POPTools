#include <string.h>
#include "i2c.h"

void         XP_Init (void);
int          XP_Busy (void);
void         XP_SetAddress(unsigned int address);
unsigned int XP_Write(unsigned int data);
unsigned int XP_Read (void);
