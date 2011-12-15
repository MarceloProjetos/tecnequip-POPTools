#include <stdio.h>
#include <string.h>
#include "rs485.h"

extern volatile unsigned char WAITING_FOR_YASKAWA;
extern volatile unsigned int I_SerialReady;

extern unsigned char rs485_rx_buffer[SERIAL_BUFFER_SIZE];
extern unsigned char rs485_tx_buffer[SERIAL_BUFFER_SIZE];

extern unsigned int rs485_rx_index;
extern unsigned int rs485_tx_index;
extern unsigned int rs485_tx_count;

int Yaskawa_Write(char * id, char *format, volatile int * val)
{
	int sz = 0;
	char msg[128];

	if (val == 0)
		return -4;

	if (id == 0 || format == 0)
	{
		*val = -4;  // formato invalido
		return 0;
	}

	memset(msg, 0, sizeof(msg));

	strcpy(msg, id);
	sprintf((void*)(&msg[0] + strlen(id)), format, *val);
	strcat(msg, "\r\n");

	WAITING_FOR_YASKAWA = 1;

	rs485_rx_index = 0;
	memset(rs485_rx_buffer, 0, sizeof(rs485_rx_buffer));

	sz = RS485_Write((unsigned char*)msg, strlen(msg));
	rs485_tx_count = sz;

	I_SerialReady = 1;

	return sz;
}

int Yaskawa_Read(char * id, char *format, volatile int *val)
{
	int sz = 0;
	int tx_count = 0;
	char msg[128];
	char cmp[128];
	char * pmsg = &msg[0];

	memset(msg, 0, sizeof(msg));

	sz = rs485_rx_index;
	tx_count = rs485_tx_count;
	memcpy(msg, rs485_rx_buffer, sz);

	rs485_rx_index = 0;
	rs485_tx_count = 0;

	memset(rs485_rx_buffer, 0, sizeof(rs485_rx_buffer));
	memset(rs485_tx_buffer, 0, sizeof(rs485_tx_buffer));

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
		memset(cmp, 0, sizeof(cmp));

		if (sscanf(pmsg, format, val) != 1)
			*val = 0;  // nenhum valor

		// compara se diferente do formato
		strcpy(cmp, id);
		sprintf((void*)(&cmp[0] + strlen(id)), format, *val);
		//strcat(cmp, "\r");

		if (strncmp(cmp, pmsg, strlen(cmp)) != 0)
			*val = -1;  // formato invalido
	}

	return sz;
}
