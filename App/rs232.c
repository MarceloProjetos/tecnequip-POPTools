/******************************************************************************
* Comunicação Serial RS232
******************************************************************************/
#ifndef __RS232_H__
#define __RS232_H__

#include <string.h>
#include "lpc17xx.h"
#include "lpc17xx_uart.h"
#include "serial.h"
#include "rs232.h"

unsigned char rs232_rx_buffer[SERIAL_BUFFER_SIZE];
unsigned char rs232_tx_buffer[SERIAL_BUFFER_SIZE];

unsigned int rs232_rx_index = 0;
unsigned int rs232_tx_index = 0;

void RS232_Init()
{
	memset(rs232_rx_buffer, 0, sizeof(rs232_rx_buffer));
	memset(rs232_tx_buffer, 0, sizeof(rs232_tx_buffer));
	rs232_rx_index = 0;
	rs232_tx_index = 0;

	// Serial Console RS232
	PINCON->PINSEL0 &= ~(0xF << 4);
	PINCON->PINSEL0 |= 0x5 << 4;

	// Serial Console 115200 (8, N, 1)
	UART0->LCR = 0x83;          /* 8 bits, no Parity, 1 Stop bit */
	UART0->DLM = 0;
	UART0->DLL = 10;
	UART0->FDR = (14 << 4) | 5;
	UART0->LCR = 0x03;          /* DLAB = 0 */
	UART0->FCR = 0x07;          /* Enable and reset TX and RX FIFO. */

}

unsigned int RS232_Write(unsigned char * buffer, unsigned int size)
{
	unsigned int i = 0;

	while (!(UART0->LSR & UART_LSR_TEMT) );      /* THRE status, contain valid data */

	while (i < size)
	{
		UART0->THR = *(buffer + i++);
		while(!(UART0->LSR & UART_LSR_TEMT));      /* THRE status, contain valid data */
	}

	return i;
}

unsigned int RS232_Read(unsigned char * buffer, unsigned int size)
{
	unsigned int i = 0;
	unsigned int c = 0;
	unsigned char dummy;

	while(i < size)
	{
		for(c = 0; c < 10000 && !(UART0->LSR & UART_LSR_RDR); c++);

		if (UART0->LSR & (UART_LSR_OE|UART_LSR_PE|UART_LSR_FE|UART_LSR_BI|UART_LSR_RXFE))
			dummy = UART0->RBR;
		else if ((UART0->LSR & UART_LSR_RDR)) /** barramento tem dados ? */
			*(buffer + i++) = UART0->RBR;
		else
			break;
	}

	return i;
}

#endif
