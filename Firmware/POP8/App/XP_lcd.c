#include "XP_lcd.h"
#include "format_str.h"

volatile unsigned int I_LCDReady     = 0;
static   unsigned int posCursor      = 0;
static   unsigned int charsToDiscard = 0;

static unsigned char maskBL      = XP_LCD_CTRL_BLIGHT;
static unsigned int  lcd_model   = XP_LCD_TYPE_GENERIC;
static unsigned int  lcd_address = 0;

static struct {
	unsigned int initCount;
	unsigned int delayScreenClear;
	unsigned int delayCursorReturn;
	unsigned int delayWrite;
} XP_lcd_config[] = {
		{ 3,  11500,   5000,     0 }, // Generico
		{ 2, 111500, 115000, 11500 }, // Sunlike SC2004A
};

unsigned int XP_lcd_Init(unsigned int address, unsigned int model)
{
	unsigned int ret, i;

	lcd_address = address; // Configura o endereco do LCD
	lcd_model = model;     // Primeiro configura o modelo assim as funcoes utilizam a configuracao correta

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

	I_LCDReady = 1;

	return ret;
}

unsigned int XP_lcd_Clear(void)
{
	unsigned int ret;
	volatile unsigned int i;

	I_LCDReady = 0;

	ret = XP_lcd_WriteInstr(XP_LCD_INSTR_SCREEN_CLEAR);
	for(i = 0; i < XP_lcd_config[lcd_model].delayScreenClear; i++);

	I_LCDReady = 1;

	return ret;
}

unsigned int XP_lcd_MoveCursor(int lin, int col)
{
	volatile unsigned int count;
	unsigned int i, ret = 1;

	I_LCDReady = 0;

	// Limpa o numero de caracteres a descartar ja que estamos reposicionando o cursor
	charsToDiscard = 0;

	// Coordenadas podem ser negativas quando o usuario deseja exibir um texto a partir de determinado trecho, como no caso de deslocamento de texto na tela
	// Porem, no calculo de posicao do cursor, devemos desconsiderar a parte negativa, calculando a partir de zero
	if(lin < 0) {
		lin = 0;
	}

	if(col < 0) {
		charsToDiscard = (unsigned int)(-col); // Indica o numero de caracteres a descartar ao exibir o proximo texto
		col = 0;
	}

	// Calcula o deslocamento necessario para chegar na coordenada desejada
	if(lin == 1) {
		lin = 2;
	} else if(lin == 2) {
		lin = 1;
	}

	i = (lin * 20) + col;

	if(i < posCursor) {
		ret = XP_lcd_WriteInstr(XP_LCD_INSTR_CURSOR_RETURN);
		for(count = 0; count < XP_lcd_config[lcd_model].delayCursorReturn; count++);
		posCursor = 0;
	}

	i -= posCursor;
	while(ret && i--) {
		ret = XP_lcd_WriteInstr(XP_LCD_INSTR_CURSOR_SHIFT);
		posCursor++;
	}

	I_LCDReady = 1;

	return ret;
}

void XP_lcd_setBL(unsigned int enable)
{
	if(enable) {
		maskBL = XP_LCD_CTRL_BLIGHT;
	} else {
		maskBL = 0;
	}

	I_LCDReady = 0;
	XP_lcd_Write(0, 0);
	I_LCDReady = 1;
}

unsigned int XP_lcd_Write(unsigned char cmd, unsigned char data)
{
	unsigned int ret;
	volatile unsigned int i;
	unsigned char val = cmd | ((data&0xF) << 4) | maskBL;

	// Configura o endereco do LCD
	XP_SetAddress(lcd_address);

	// Configura pinos
	for(i = 0; i < XP_lcd_config[lcd_model].delayWrite; i++);
	ret = XP_Write_Byte(val);

	// Habilita
	for(i = 0; i < XP_lcd_config[lcd_model].delayWrite; i++);
	if(ret) ret = XP_Write_Byte(val | XP_LCD_CTRL_ENABLE);

	// Desabilita
	for(i = 0; i < XP_lcd_config[lcd_model].delayWrite; i++);
	if(ret) ret = XP_Write_Byte(val);

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

unsigned int XP_lcd_WriteText(char *format, volatile int *val)
{
	const int size = 128;
	char msg[size];
	unsigned int i = 0, ret;

	Format_String_Generate(msg, size, format, val);

	I_LCDReady = 0;

	// Descarta os caracteres solicitados
	while(charsToDiscard > 0 && msg[i]) {
		i++;
		charsToDiscard--;
	}

	// Se restaram itens, exibe o texto
	if(msg[i]) {
		do {
			ret = XP_lcd_WriteData((unsigned char)msg[i]);
			posCursor++;
		} while(ret && msg[++i] && (posCursor % 20)); // Se o cursor for multiplo de 20, estourou a linha
	}

	I_LCDReady = 1;

	return ret;
}
