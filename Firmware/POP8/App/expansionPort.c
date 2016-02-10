#include "expansionPort.h"

#define XP_CMD_WRITE 0X00UL
#define XP_CMD_READ  0x80UL

extern volatile unsigned int I2CCount;
extern volatile unsigned char I2CMasterBuffer[BUFSIZE];
extern volatile unsigned int I2CCmd, I2CMasterState;
extern volatile unsigned int I2CReadLength, I2CWriteLength;

extern void XP_input_Update (struct strExpansionBoard *board);
extern void XP_output_Update(struct strExpansionBoard *board);
extern void XP_ad_Update    (struct strExpansionBoard *board);
extern void XP_da_Update    (struct strExpansionBoard *board);

static unsigned int xpAddress;

void XP_Init(void)
{
	XP_SetAddress(0);
}

static int timeout = 0;

int XP_Busy(void)
{
	return timeout++ < 100;
}

void XP_SetAddress(unsigned int address)
{
	xpAddress = address << 1UL;
}

/*** Funcoes de Escrita no barramento de expansao ***/

unsigned int XP_Write(unsigned char *data, unsigned int size)
{
	unsigned int i;
	timeout = 0;
	memset((void *)I2CMasterBuffer, 0, BUFSIZE);

	I2CWriteLength = size + 1;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = xpAddress | XP_CMD_WRITE;

	for(i = 1; size > 0; i++, size--) {
		I2CMasterBuffer[i] = data[i - 1];
	}

	return I2C_Engine(2);
}

unsigned int XP_Write_Byte(unsigned char data)
{
	return XP_Write(&data, 1);
}

/*** Funcoes de Leitura no barramento de expansao ***/

unsigned int XP_Read(char *data, unsigned int size)
{
	unsigned int ret;
	timeout = 0;
	memset((void *)I2CMasterBuffer, 0, BUFSIZE);

	I2CWriteLength = 1;
	I2CReadLength  = size;
	I2CMasterBuffer[0] = xpAddress | XP_CMD_READ;
	ret = I2C_Engine(2);

	if(ret) {
		memcpy(data, (char *)I2CMasterBuffer, size);
	}

	return ret;
}

unsigned int XP_Read_Int(void)
{
	char data[3];

	if(!XP_Read(data, 2)) {
		return 0;
	}

	data[2] = 0; // Finaliza como string para usar a funcao atoi
	return atoi(data);
}

/*** Fim das funcoes ***/

void XP_InputUpdate()
{
	struct strExpansionBoard *board = expansionBoards;

	while(board->type != eBoardType_None) {
		switch(board->type) {
		case eBoardType_Input:
			XP_input_Update(board);
			break;

		case eBoardType_AD:
			XP_ad_Update(board);
			break;

		default: break;
		}

		board++;
	}
}

void XP_OutputUpdate()
{
	struct strExpansionBoard *board = expansionBoards;

	while(board->type != eBoardType_None) {
		switch(board->type) {
		case eBoardType_Output:
			XP_output_Update(board);
			break;

		case eBoardType_DA:
			XP_da_Update(board);
			break;

		default: break;
		}

		board++;
	}
}
