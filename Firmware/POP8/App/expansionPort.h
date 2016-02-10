#ifndef __EXPANSION_PORT_H__
#define __EXPANSION_PORT_H__

#include <stdlib.h>
#include <string.h>
#include "i2c.h"

enum eBoardType {
	eBoardType_None,
	eBoardType_Input,
	eBoardType_Output,
	eBoardType_AD,
	eBoardType_DA
};

struct strExpansionBoard {
	enum eBoardType type;
	unsigned char   address;
	union {
		struct {
			unsigned int bit0  : 1;
			unsigned int bit1  : 1;
			unsigned int bit2  : 1;
			unsigned int bit3  : 1;
			unsigned int bit4  : 1;
			unsigned int bit5  : 1;
			unsigned int bit6  : 1;
			unsigned int bit7  : 1;
			unsigned int bit8  : 1;
			unsigned int bit9  : 1;
			unsigned int bit10 : 1;
			unsigned int bit11 : 1;
			unsigned int bit12 : 1;
			unsigned int bit13 : 1;
			unsigned int bit14 : 1;
			unsigned int bit15 : 1;
			unsigned int bit16 : 1;
			unsigned int bit17 : 1;
			unsigned int bit18 : 1;
			unsigned int bit19 : 1;
			unsigned int bit20 : 1;
			unsigned int bit21 : 1;
			unsigned int bit22 : 1;
			unsigned int bit23 : 1;
			unsigned int bit24 : 1;
			unsigned int bit25 : 1;
			unsigned int bit26 : 1;
			unsigned int bit27 : 1;
			unsigned int bit28 : 1;
			unsigned int bit29 : 1;
			unsigned int bit30 : 1;
			unsigned int bit31 : 1;
		} bits;
		unsigned int port;
	} value;
	unsigned int    channel;
	unsigned char   useInterrupt;
};

extern struct strExpansionBoard expansionBoards[];

void         XP_Init (void);
int          XP_Busy (void);
void         XP_SetAddress(unsigned int address);

unsigned int XP_Write     (unsigned char *data, unsigned int size);
unsigned int XP_Write_Byte(unsigned char data);
unsigned int XP_Read      (char *data, unsigned int size);
unsigned int XP_Read_Int  (void);

void XP_InputUpdate ();
void XP_OutputUpdate();

#endif // __EXPANSION_PORT_H__
