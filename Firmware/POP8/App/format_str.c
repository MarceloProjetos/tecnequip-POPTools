#include <string.h>
#include <stdio.h>
#include "rs485.h"
#include "modbus.h"
#include "net.h"
#include "rtc.h"

extern struct ip_addr IP_ADDRESS;
extern unsigned char  mac_addr[6];

extern volatile unsigned char WAITING_FOR_FMTSTR;
extern unsigned char rs485_rx_buffer[SERIAL_BUFFER_SIZE];
extern unsigned int rs485_rx_index;

char *Format_String_Generate(char *msg, unsigned int size, char *format, volatile int * val)
{
	RTC_Time Time;
	unsigned int offset = 0, first = 1, size_data;
	char *start_msg = msg, data[128], type = 'd';

	// Identifica o tipo de dado a ser enviado
	while(*format != 0 && offset < size) {
		if(*format == '%') {
			type = *(++format);

			switch(type) {
			case 0:
				*msg = 0;
				return start_msg; // Se terminou a string, apenas finalizamos a string atual e retornamos
			case '%':
				data[0] = '%';
				data[1] = 0;
				size_data = 1;
				break;
			case 'p':
				strcpy(data, ModBUS_GetAppName());
				size_data = strlen(data);
				break;
			case 'i':
				size_data = sprintf(data, "%d.%d.%d.%d", ip4_addr1(&IP_ADDRESS), ip4_addr2(&IP_ADDRESS), ip4_addr3(&IP_ADDRESS), ip4_addr4(&IP_ADDRESS));
				break;
			case 'm':
				size_data = sprintf(data, "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[5], mac_addr[4], mac_addr[3], mac_addr[2], mac_addr[1], mac_addr[0]);
				break;
			case 'h': {
					if(first) {
						first = 0;
						Time = RTC_GetTime();
					}
					switch(*(++format)) {
						case 'd': size_data = sprintf(data, "%02d", Time.Mday); break;
						case 'm': size_data = sprintf(data, "%02d", Time.Mon ); break;
						case 'y':
						case 'a': size_data = sprintf(data,   "%d", Time.Year); break;
						case 'h': size_data = sprintf(data, "%02d", Time.Hour); break;
						case 'M': size_data = sprintf(data, "%02d", Time.Min ); break;
						case 's': size_data = sprintf(data, "%02d", Time.Sec ); break;
						default : size_data = data[0] = 0;
					}
					break;
				}
			default: {
					char new_format[3] = { '%', 0, 0 };
					new_format[1] = type;
					size_data = sprintf(data, new_format, *val);
					break;
				}
			}

			// Nesse momento o formato foi tratado e o vetor data[] contem o texto formatado. Apenas copia este texto para o vetor msg[]
			if(size_data > (size - offset)) {
				size_data = (size - offset);
			} else if(size_data < 0) {
				size_data = 0;
			}
			strncpy(msg, data, size_data);
			offset += size_data;
			msg    += size_data;

			// Finalmente, avanca a posicao da string de formato
			format++;
		} else {
			// Sem formato. apenas copia o caracter atual e avancas as posicoes das strings
			*msg = *format;
			offset++;
			msg   ++;
			format++;
		}
	}

	*msg = 0;

	return start_msg;
}

int Format_String_Write(char *format, volatile int * val)
{
	int sz = 0;
	const int size = 128;
	char msg[size];

	Format_String_Generate(msg, size, format, val);

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
