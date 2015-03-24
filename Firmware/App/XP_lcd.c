#include "XP_lcd.h"

extern volatile unsigned int I2CCount;
extern volatile unsigned char I2CMasterBuffer[BUFSIZE];
extern volatile unsigned int I2CCmd, I2CMasterState;
extern volatile unsigned int I2CReadLength, I2CWriteLength;

static unsigned char maskBL = XP_LCD_CTRL_BLIGHT;

unsigned int XP_lcd_Init(void)
{
	unsigned int ret;

	XP_lcd_setBL(1);

	// Envia sequencia de inicializacao
	// Devem ser enviadas 3 mensagens com os pinos D4 e D5 ligados, D6 e D7 desligados
	        ret = XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x03);
	if(ret) ret = XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x03);
	if(ret) ret = XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x03);

	// Desloca os bits pois nesse momento o LCD ainda trabalha em modo 8 bits
	if(ret) ret = XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, (XP_LCD_INSTR_4BITS & 0xF0) >> 4);

	// Configura novamente o modo de 4 bits, agora incluindo a parte baixa
	if(ret) ret = XP_lcd_WriteInstr(XP_LCD_INSTR_4BITS);

	// Configura cursor piscante
	if(ret) ret = XP_lcd_WriteInstr(XP_LCD_INSTR_CURSOR_HIDE);

	// Limpa o display
	XP_lcd_Clear();

	// Envia Texto
//	if(ret) ret = XP_lcd_WriteText("Linha 1... abcdefghi");
//	if(ret) ret = XP_lcd_WriteText("Linha 3!!! :-) X.X |");
//	if(ret) ret = XP_lcd_WriteText("Linha 2??? %+-/=@#$&");
//	if(ret) ret = XP_lcd_WriteText("Linha 4. ç,.<>;:{}][");
	XP_lcd_MoveCursor(1, 5);
	XP_lcd_WriteText("Teste");
	XP_lcd_MoveCursor(2, 10);
	XP_lcd_WriteText("Outro...");
/*
	XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x03);
	XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x03);
	XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x03);
	XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x02);
	XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x02);
//	XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x00);
//	XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x0F);
	XP_lcd_WriteInstr(XP_LCD_INSTR_CURSOR_BLINK);
	XP_lcd_WriteInstr(XP_LCD_INSTR_CURSOR_BLINK);
	XP_lcd_WriteData('A');
	XP_lcd_WriteData('B');
	XP_lcd_WriteData('C');
*/
	return ret;
}

unsigned int XP_lcd_Clear(void)
{
	unsigned int ret = XP_lcd_WriteInstr(XP_LCD_INSTR_SCREEN_CLEAR);
	volatile unsigned int i;

	for(i=0; i < 11500; i++);

	return ret;
}

unsigned int XP_lcd_MoveCursor(unsigned int lin, unsigned int col)
{
	volatile unsigned int count;
	unsigned int i, ret = XP_lcd_WriteInstr(XP_LCD_INSTR_CURSOR_RETURN);

	// Calcula o deslocamento necessario para chegar na coordenada desejada
	if(lin == 1) {
		lin = 2;
	} else if(lin == 2) {
		lin = 1;
	}

	i = (lin * 20) + col;

	for(count = 0; count < 5000; count++);

	while(ret && i--) {
		ret = XP_lcd_WriteInstr(XP_LCD_INSTR_CURSOR_SHIFT);
	}

	return ret;
}

void XP_lcd_setBL(unsigned int enable)
{
	if(enable) {
		maskBL = XP_LCD_CTRL_BLIGHT;
	} else {
		maskBL = 0;
	}
}

unsigned int XP_lcd_Write(unsigned char cmd, unsigned char data)
{
	volatile unsigned int ret;
	unsigned char val = cmd | ((data&0xF) << 4) | maskBL;

	// Configura o endereco do LCD
	XP_SetAddress(0x27);

	// Configura pinos
	ret = XP_Write(val);

	// Habilita
	if(ret) ret = XP_Write(val | XP_LCD_CTRL_ENABLE);

	// Desabilita
	if(ret) ret = XP_Write(val);

	return ret;
}

unsigned int XP_lcd_WriteInstr(unsigned char instr)
{
	unsigned int  ret;

	    ret = XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, (instr & 0xF0) >> 4);
	if(ret) {
		ret = XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, instr);
	}

	return ret;
}

unsigned int XP_lcd_WriteData(unsigned char data)
{
	unsigned int  ret;

	    ret = XP_lcd_Write(XP_LCD_CTRL_DATA | XP_LCD_CTRL_WRITE, (data & 0xF0) >> 4);
	if(ret) {
		ret = XP_lcd_Write(XP_LCD_CTRL_DATA | XP_LCD_CTRL_WRITE, data);
	}

	return ret;
}

unsigned int XP_lcd_WriteText(char *data)
{
	unsigned int i = 0, ret;

	do {
		ret = XP_lcd_WriteData((unsigned char)data[i]);
	} while(ret && data[++i]);

	return ret;
}
