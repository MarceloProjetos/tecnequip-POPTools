#include "expansionPort.h"

unsigned int XP_lcd_Clear     (void);
unsigned int XP_lcd_MoveCursor(unsigned int lin, unsigned int col);
void         XP_lcd_setBL     (unsigned int enable);
unsigned int XP_lcd_Init      (unsigned int model);
unsigned int XP_lcd_Write     (unsigned char cmd, unsigned char data);
unsigned int XP_lcd_WriteData (unsigned char data);
unsigned int XP_lcd_WriteInstr(unsigned char instr);
unsigned int XP_lcd_WriteText (char *format, volatile int *val);

#define XP_LCD_TYPE_GENERIC 0
#define XP_LCD_TYPE_SC2004A 1

// Pinos do LCD (Bit no I2C):
//  1 (-): GND - Alimentacao, Referencia
//  2 (-): VCC - Alimentacao, 5 Volts
//  3 (-): VEE - Brilho
//  4 (0): RS  - Baixo: Comando      / Alto: Dados
//  5 (1): RW  - Baixo: Escrita      / Alto: Leitura
//  6 (2): E   - Baixo: Desabilitado / Alto: Habilitado
//  7 (-): D0  - Dado, bit 0 do LCD
//  8 (-): D1  - Dado, bit 1 do LCD
//  9 (-): D2  - Dado, bit 2 do LCD
// 10 (-): D3  - Dado, bit 3 do LCD
// 11 (4): D4  - Dado, bit 4 do LCD
// 12 (5): D5  - Dado, bit 5 do LCD
// 13 (6): D6  - Dado, bit 6 do LCD
// 14 (7): D7  - Dado, bit 7 do LCD
// 15 (-): BLA - BackLight, Anodo
// 16 (-): BLK - BackLight, Catodo

// Flags para pino RS
#define XP_LCD_CTRL_DATA   (0x01)
#define XP_LCD_CTRL_INSTR  (0x00)

// Flags para pino RW
#define XP_LCD_CTRL_READ   (0x02)
#define XP_LCD_CTRL_WRITE  (0x00)

// Flag para pino E (Enable)
#define XP_LCD_CTRL_ENABLE (0x04)

// Flag para pino E (Enable)
#define XP_LCD_CTRL_BLIGHT (0x08)

/*** Instrucoes suportadas pelo LCD ***/

#define XP_LCD_INSTR_4BITS         (0x28)
#define XP_LCD_INSTR_CURSOR_BLINK  (0x0F)
#define XP_LCD_INSTR_CURSOR_HIDE   (0x0C)
#define XP_LCD_INSTR_CURSOR_RETURN (0x02)
#define XP_LCD_INSTR_CURSOR_SHIFT  (0x14)
#define XP_LCD_INSTR_SCREEN_CLEAR  (0x01)
