#include <string.h>
#include <stdio.h>
#include "rs485.h"
#include "modbus.h"
#include "net.h"

extern struct ip_addr IP_ADDRESS;
extern unsigned char  mac_addr[6];

extern volatile unsigned char WAITING_FOR_FMTSTR;
extern unsigned char rs485_rx_buffer[SERIAL_BUFFER_SIZE];
extern unsigned int rs485_rx_index;

char *Format_String_Generate(char *msg, char *format, volatile int * val)
{
	char *start, data[128], type = 'd';

	// Identifica o tipo de dado a ser enviado
	start = strchr(format, '%');
	if(start != NULL) {
		type = *(start + 1);

		switch(type) {
		case 0: return msg; // Depois de % nunca deve ser fim de string!
		case 'p':
			strcpy(data, ModBUS_GetAppName());
			break;
		case 'i':
			sprintf(data, "%d.%d.%d.%d", ip4_addr1(&IP_ADDRESS), ip4_addr2(&IP_ADDRESS), ip4_addr3(&IP_ADDRESS), ip4_addr4(&IP_ADDRESS));
			break;
		case 'm':
			sprintf(data, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[5], mac_addr[4], mac_addr[3], mac_addr[2], mac_addr[1], mac_addr[0]);
			break;
		default: {
				char new_format[3] = { '%', 0, 0 };
				new_format[1] = type;
				sprintf(data, new_format, *val);
				break;
			}
		}

		// Atualiza o tipo conforme a informacao que sera enviada
		strncpy(msg, format, start - format);
		strcpy (msg + (start - format), data);
		strcpy (msg + (start - format + strlen(data)), start + 2);
	} else {
		// Se nao encontrou %, nao precisa interpretar o formato...
		strcpy(msg, format);
	}

	return msg;
}

int Format_String_Write(char *format, volatile int * val)
{
	int sz = 0;
	char msg[128];

	Format_String_Generate(msg, format, val);

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
