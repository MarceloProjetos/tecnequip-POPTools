#include <stdio.h>
#include <string.h>
#include "rs485.h"

extern volatile unsigned char WAITING_FOR_YASKAWA;
extern volatile unsigned int I_SerialReady;

extern unsigned char rs485_rx_buffer[SERIAL_BUFFER_SIZE];

extern unsigned int rs485_rx_index;
extern unsigned int rs485_tx_index;
extern unsigned int rs485_tx_count;

char msg[128];

int Yaskawa_Write(char * id, char *format, volatile int * val)
{
	if (val == 0)
		return -4;

	if (id == 0 || format == 0)
	{
		*val = -4;  // formato invalido
		return 0;
	}

	strcpy(msg, id);
	sprintf((void*)(&msg[0] + strlen(id)), format, *val);
	strcat(msg, "\r\n");

	WAITING_FOR_YASKAWA = 1;

	rs485_tx_count = RS485_Write((unsigned char*)msg, strlen(msg));

	I_SerialReady = 1;

	return rs485_tx_count;
}

int Yaskawa_Read(char * id, char *format, volatile int *val)
{
	int sz       = rs485_rx_index;
	int tx_count = rs485_tx_count;
	char * pmsg  = (char*)rs485_rx_buffer;

	rs485_rx_index = 0;
	rs485_tx_count = 0;

	I_SerialReady = 1;
	WAITING_FOR_YASKAWA = 0;

	if (val == 0)
		return -4; // parametros invalidos

	if (id == 0 || format == 0)
	{
		*val = -4; // parametros invalidos
		return 0;
	}

	if (sz == 0)
	{
		*val = -2; // sem resposta
		return 0;
	}

	*val = 0;

	if (sz <= tx_count)
	{
		*val = -5; // resposta invalida
		return sz;
	}

	pmsg += tx_count;

	if (strncmp(pmsg, id, strlen(id)) != 0)
		*val = -3; // id diferente
	else
	{
		if (sscanf(pmsg, format, val) != 1)
			*val = 0;  // nenhum valor

		// compara se diferente do formato
		strcpy(msg, id);
		sprintf((void*)(&msg[0] + strlen(id)), format, *val);
		//strcat(cmp, "\r");

		if (strncmp(msg, pmsg, strlen(msg)) != 0)
			*val = -1;  // formato invalido
	}

	return sz;
}
