#include <string.h>
#include <stdio.h>
#include "rs485.h"

int Format_String_Write(char *format, volatile int * val)
{
	int sz = 0;
	char msg[128];

	sprintf((void*)msg, format, *val);

	//I_SerialReady = 1;

	sz = RS485_Write((unsigned char*)msg, strlen(msg));

	return sz;
}

int Format_String_Read(char *format, volatile int *val)
{
	int sz = 0;
	char msg[128];
	char cmp[128];

	*val = 0;
	memset(msg, 0, sizeof(msg));

	sz = RS485_Read((unsigned char*)msg, sizeof(msg));

	if (sz == 0)
		*val = -2;
	else
	{
		memset(cmp, 0, sizeof(msg));

		//sprintf((void*)msg, format, val);
		if (sscanf(msg, format, val) != 1)
			*val = -1;
		else
		{
			sprintf(cmp, format, val);

			if (strncmp(cmp, msg, sz) != 0)
				*val = -1;
		}
	}

	//I_SerialReady = 1;

	return sz;
}
