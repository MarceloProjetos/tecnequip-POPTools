

/*
* file:
*       uart.c
* description:
*       Application for STM32 uart driver.
*/

#include <LPC17xx.H>                              /* LPC17xx definitions    */
#include "Serial.h"
#include <stdio.h>

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void serial_init (int uart) {
UART_TypeDef *pUart;
   
if (uart == 0) {                               /* UART0 */
    PINCON->PINSEL0 |= (1 << 4);                 /* Pin P0.2 used as TXD0 (Com0) */
    PINCON->PINSEL0 |= (1 << 6);                 /* Pin P0.3 used as RXD0 (Com0) */

    pUart = (UART_TypeDef *)UART0;
  } else {                                       /* UART1 */
    PINCON->PINSEL4 |= (2 << 0);                 /* Pin P2.0 used as TXD1 (Com1) */
    PINCON->PINSEL4 |= (2 << 2);                 /* Pin P2.1 used as RXD1 (Com1) */

    pUart = (UART_TypeDef *)UART1;
  }

  pUart->LCR    = 0x83;                          /* 8 bits, no Parity, 1 Stop bit  */
  pUart->DLL    = 6;                             /* 115200 Baud Rate @ 18.0 MHZ PCLK */
  pUart->FDR    = 0x85;                          /* FR = 1,627 DIVADDVAL = 5, MULVAL = 8 */
  pUart->DLM    = 0;                             /* High divisor latch = 0         */
  pUart->LCR    = 0x03;                          /* DLAB = 0                       */
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int serial_putChar (int uart, int c) {
  UART_TypeDef *pUart;

  pUart = (uart == 0) ? UART0 : (UART_TypeDef *)UART1;
  while (!(pUart->LSR & 0x20));
  return (pUart->THR = c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int serial_getChar (int uart) {
  UART_TypeDef *pUart;

  pUart = (uart == 0) ? UART0 : (UART_TypeDef *)UART1;
  while (!(pUart->LSR & 0x01));
  return (pUart->RBR);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (non blocking read)
 *----------------------------------------------------------------------------*/
int serial_getChar_nb (int uart) {
  UART_TypeDef *pUart;

  pUart = (uart == 0) ? UART0 : (UART_TypeDef *)UART1;
  if (pUart->LSR & 0x01)
    return (pUart->RBR);
  else
    return 0;
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
void serial_putString (int uart, unsigned char *s) {

  while (*s != 0) {
   serial_putChar(uart, *s++);
  }
}


//------------------------------------------------------------------------------
/// \exclude
/// Implementation of fputc using the DBGU as the standard output. Required
/// for printf().
/// \param c  Character to write.
/// \param pStream  Output stream.
/// \param The character written if successful, or -1 if the output stream is
/// not stdout or stderr.
//------------------------------------------------------------------------------
signed int fputc(signed int c, FILE *pStream)
{
    if ((pStream == stdout) || (pStream == stderr)) {
    
        serial_putChar(1,c);
        return c;
    }
    else {

        return EOF;
    }
}

//------------------------------------------------------------------------------
/// \exclude
/// Implementation of fputs using the DBGU as the standard output. Required
/// for printf(). Does NOT currently use the PDC.
/// \param pStr  String to write.
/// \param pStream  Output stream.
/// \return Number of characters written if successful, or -1 if the output
/// stream is not stdout or stderr.
//------------------------------------------------------------------------------
signed int fputs(const char *pStr, FILE *pStream)
{
    signed int num = 0;

    while (*pStr != 0) {

        if (fputc(*pStr, pStream) == -1) {

            return -1;
        }
        num++;
        pStr++;
    }

    return num;
}

#undef putchar

//------------------------------------------------------------------------------
/// \exclude
/// Outputs a character on the DBGU.
/// \param c  Character to output.
/// \return The character that was output.
//------------------------------------------------------------------------------
signed int putchar(signed int c)
{
    return fputc(c, stdout);
}

unsigned char CRC(unsigned char * buffer, unsigned int size)
{
	unsigned char crc = 0;
	unsigned int i = 0;

	while (i < size)
	{
		crc ^= *(buffer + i++);
	}

	return crc;
}

