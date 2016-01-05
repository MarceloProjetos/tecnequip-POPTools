#include <string.h>
#include <stdio.h>
#include "rs485.h"

extern volatile unsigned char WAITING_FOR_FMTSTR;
extern unsigned char rs485_rx_buffer[SERIAL_BUFFER_SIZE];
extern unsigned int rs485_rx_index;

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
	int tmp;
	WAITING_FOR_FMTSTR = 1;

	if (rs485_rx_index >= strlen(format) - 1) {
		// Se o formato for "abc%d", strlen retorna 5. Porem o tamanho dos dados no
		// buffer devem ser, pelo menos, o tamanho do texto fixo + 1 caracter para %d
		// Assim para um formato de 5 precisamos de 4 caracteres ou strlen - 1
		if (sscanf((char *)rs485_rx_buffer, format, &tmp) == 1) {
			char buf[128];
			sprintf(buf, format, tmp);
			if(!strncmp(buf, (char *)rs485_rx_buffer, strlen(buf))) {
				*val = tmp;
				WAITING_FOR_FMTSTR = 0;
				rs485_rx_index = 0;
				return 1;
			}
		}
	}

	return 0;
}
