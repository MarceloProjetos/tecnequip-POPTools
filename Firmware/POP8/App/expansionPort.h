#include <stdlib.h>
#include <string.h>
#include "i2c.h"

void         XP_Init (void);
int          XP_Busy (void);
void         XP_SetAddress(unsigned int address);

unsigned int XP_Write     (unsigned char *data, unsigned int size);
unsigned int XP_Write_Byte(unsigned char data);
unsigned int XP_Read      (char *data, unsigned int size);
unsigned int XP_Read_Int  (void);
