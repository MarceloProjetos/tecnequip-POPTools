/******************************************************************************
* Comunicação Serial RS232
******************************************************************************/
#ifndef __RS232_H__
#define __RS232_H__

#include <string.h>
#include <stdio.h>
#include "chip.h"
#include "rs232.h"
#include "rtc.h"

#define SERIAL_BUFFER_SIZE 64

char rs232_rx_buffer[SERIAL_BUFFER_SIZE];
char rs232_tx_buffer[SERIAL_BUFFER_SIZE];

unsigned int rs232_rx_index = 0;
unsigned int rs232_tx_index = 0;

void RS232_Init()
{
	// Configura os pinos RXD e TXD para a funcao da serial
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 2, IOCON_FUNC1 | IOCON_MODE_PULLUP | IOCON_HYS_EN); // TXD
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 3, IOCON_FUNC1 | IOCON_MODE_PULLUP | IOCON_HYS_EN); // RXD

	// Serial Console RS232
	Chip_UART_Init(LPC_UART0);
	Chip_UART_SetBaud(LPC_UART0, 115200);
	Chip_UART_TXEnable(LPC_UART0);

	rs232_rx_index = 0;
	rs232_tx_index = 0;

	memset(rs232_rx_buffer, 0, sizeof(rs232_rx_buffer));
	memset(rs232_tx_buffer, 0, sizeof(rs232_tx_buffer));
}

unsigned int RS232_Write(char * buffer, unsigned int size)
{
	return Chip_UART_SendBlocking(LPC_UART0, buffer, size);
}

unsigned int RS232_Read(char * buffer, unsigned int size)
{
	return Chip_UART_Read(LPC_UART0, buffer, size);
}

#include "modbus.h"

extern struct MODBUS_Device modbus_rs232;

void RS232_Console(unsigned int cycle)
{
	unsigned int sz;
	static unsigned int rs232_timeout = 0;

	sz = RS232_Read(rs232_rx_buffer + rs232_rx_index, sizeof(rs232_rx_buffer) - rs232_rx_index);
	rs232_rx_index += sz;

  if(rs232_rx_index) {
    if(sz) {
      rs232_timeout  = cycle + 3;
    } else if(cycle >= rs232_timeout) {
      Modbus_RTU_Receive(&modbus_rs232, Modbus_RTU_Validate((unsigned char *)rs232_rx_buffer, rs232_rx_index, 0));

      rs232_rx_index = 0;
    }
  }
}

#endif
