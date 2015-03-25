#include "XP_lcd.h"

extern volatile unsigned int I2CCount;
extern volatile unsigned char I2CMasterBuffer[BUFSIZE];
extern volatile unsigned int I2CCmd, I2CMasterState;
extern volatile unsigned int I2CReadLength, I2CWriteLength;

static unsigned char maskBL    = XP_LCD_CTRL_BLIGHT;
static unsigned int  lcd_model = XP_LCD_TYPE_GENERIC;

static struct {
	unsigned int initCount;
	unsigned int delayScreenClear;
	unsigned int delayCursorReturn;
	unsigned int delayWrite;
} XP_lcd_config[] = {
		{ 3,  11500, 5000,    0 }, // Generico
		{ 2, 111500, 115000, 11500 }, // Sunlike SC2004A
};

unsigned int XP_lcd_Init(unsigned int model)
{
	unsigned int ret, i;

	lcd_model = model; // Primeiro configura o modelo assim as funcoes utilizam a configuracao correta

	XP_lcd_setBL(1);

	// Envia sequencia de inicializacao
	// Devem ser enviadas 3 mensagens com os pinos D4 e D5 ligados, D6 e D7 desligados
	for(i = 0; i < XP_lcd_config[lcd_model].initCount; i++) {
	        ret = XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, 0x03);
	        if(!ret) break; // Interrompe em caso de erro
	}

	// Desloca os bits pois nesse momento o LCD ainda trabalha em modo 8 bits
	if(ret) ret = XP_lcd_Write(XP_LCD_CTRL_INSTR | XP_LCD_CTRL_WRITE, (XP_LCD_INSTR_4BITS & 0xF0) >> 4);

	// Configura novamente o modo de 4 bits, agora incluindo a parte baixa
	if(ret) ret = XP_lcd_WriteInstr(XP_LCD_INSTR_4BITS);

	// Configura cursor piscante
	if(ret) ret = XP_lcd_WriteInstr(XP_LCD_INSTR_CURSOR_HIDE);

//	if(ret) ret = XP_lcd_WriteInstr(0x04);

	// Limpa o display
	XP_lcd_Clear();

	// Envia Texto
	XP_lcd_MoveCursor(1, 5);
	XP_lcd_WriteText("Teste");
	XP_lcd_MoveCursor(2, 10);
	XP_lcd_WriteText("Outro...");

	return ret;
}

unsigned int XP_lcd_Clear(void)
{
	unsigned int ret = XP_lcd_WriteInstr(XP_LCD_INSTR_SCREEN_CLEAR);
	volatile unsigned int i;

	for(i = 0; i < XP_lcd_config[lcd_model].delayScreenClear; i++);

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

	for(count = 0; count < XP_lcd_config[lcd_model].delayCursorReturn; count++);

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
	unsigned int ret;
	volatile unsigned int i;
	unsigned char val = cmd | ((data&0xF) << 4) | maskBL;

	// Configura o endereco do LCD
	XP_SetAddress(0x27);

	// Configura pinos
	for(i = 0; i < XP_lcd_config[lcd_model].delayWrite; i++);
	ret = XP_Write(val);

	// Habilita
	for(i = 0; i < XP_lcd_config[lcd_model].delayWrite; i++);
	if(ret) ret = XP_Write(val | XP_LCD_CTRL_ENABLE);

	// Desabilita
	for(i = 0; i < XP_lcd_config[lcd_model].delayWrite; i++);
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
