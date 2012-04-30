/******************************************************************************
* Comunicação Serial RS232
******************************************************************************/
#ifndef __RS232_H__
#define __RS232_H__

#include <string.h>
#include <stdio.h>
#include "lpc17xx.h"
#include "lpc17xx_uart.h"
#include "serial.h"
#include "rs232.h"
#include "rtc.h"

char rs232_rx_buffer[SERIAL_BUFFER_SIZE];
char rs232_tx_buffer[SERIAL_BUFFER_SIZE];

unsigned int rs232_rx_index = 0;
unsigned int rs232_tx_index = 0;

void RS232_Init()
{
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

	rs232_rx_index = 0;
	rs232_tx_index = 0;

	memset(rs232_rx_buffer, 0, sizeof(rs232_rx_buffer));
	memset(rs232_tx_buffer, 0, sizeof(rs232_tx_buffer));
}

unsigned int RS232_Write(char * buffer, unsigned int size)
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

unsigned int RS232_Read(char * buffer, unsigned int size)
{
	unsigned int i = 0;
	unsigned char dummy;

	while(i < size)
	{
		if (UART0->LSR & (UART_LSR_OE|UART_LSR_PE|UART_LSR_FE|UART_LSR_BI|UART_LSR_RXFE))
			dummy = UART0->RBR;
		else if ((UART0->LSR & UART_LSR_RDR)) /** barramento tem dados ? */
			*(buffer + i++) = UART0->RBR;
		else
			break;
	}

	return i;
}

#ifdef RS232_CONSOLE
unsigned char RS232_CRC(char * buffer, unsigned int size)
{
	uint8_t crc = 0;
	unsigned int i = 0;

	while (i < size)
	{
		crc ^= *(buffer + i++);
	}

	return crc;
}

void RS232_Console(void)
{
	unsigned int sz = 0;

	sz = RS232_Read(rs232_rx_buffer + rs232_rx_index, sizeof(rs232_rx_buffer) - rs232_rx_index);
	rs232_rx_index += sz;

	RTC_Time Time;

	if (sz > 0)
	{
		if (rs232_rx_index == 11)
		{
			memset(&Time, 0, sizeof(Time));

			Time.Mday = *(rs232_rx_buffer);
			Time.Mon = *(rs232_rx_buffer + 1);
			Time.Year = *((uint16_t*)(rs232_rx_buffer + 2));
			Time.Hour = *(rs232_rx_buffer + 4);
			Time.Min = *(rs232_rx_buffer + 5);
			Time.Sec = *(rs232_rx_buffer + 6);
			Time.Wday = *(rs232_rx_buffer + 7);
			Time.Yday = *((uint16_t*)(rs232_rx_buffer + 8));

			memset(rs232_tx_buffer, 0, sizeof(rs232_tx_buffer));

			if (RS232_CRC(rs232_rx_buffer, 10) != rs232_rx_buffer[10])
			{
				memset(rs232_tx_buffer, 0xFF, 10);
				rs232_tx_buffer[10] = RS232_CRC(rs232_tx_buffer, 10);
				RS232_Write(rs232_tx_buffer, 11);
			}
			else
			{
				RTC_SetTime(Time);

				memset(&Time, 0, sizeof(Time));

				Time = RTC_GetTime();

				rs232_tx_buffer[0] = (uint8_t)Time.Mday;
				rs232_tx_buffer[1] = (uint8_t)Time.Mon;
				rs232_tx_buffer[2] = (uint8_t)Time.Year;
				rs232_tx_buffer[3] = (uint8_t)(Time.Year >> 8);
				rs232_tx_buffer[4] = (uint8_t)Time.Hour;
				rs232_tx_buffer[5] = (uint8_t)Time.Min;
				rs232_tx_buffer[6] = (uint8_t)Time.Sec;
				rs232_tx_buffer[7] = (uint8_t)Time.Wday;
				rs232_tx_buffer[8] = (uint8_t)Time.Yday;
				rs232_tx_buffer[9] = (uint8_t)(Time.Yday >> 8);
				rs232_tx_buffer[10] = RS232_CRC(rs232_tx_buffer, 10);

				RS232_Write(rs232_tx_buffer, 11);
			}

			rs232_rx_index = 0;
			memset(rs232_rx_buffer, 0, sizeof(rs232_rx_buffer));
		}
		else if (strncmp(rs232_rx_buffer, "?", 1) == 0)
		{
			memset(rs232_tx_buffer, 0, sizeof(rs232_tx_buffer));

			Time = RTC_GetTime();

			sprintf(rs232_tx_buffer, "%02d/%02d/%04d %02d:%02d:%02d WEEKDAY:%01d YEARDAY:%03d\n",
				Time.Mday, Time.Mon, Time.Year, Time.Hour, Time.Min, Time.Sec, Time.Wday, Time.Yday);

			RS232_Write(rs232_tx_buffer, strlen(rs232_tx_buffer));

			rs232_rx_index = 0;
			memset(rs232_rx_buffer, 0, sizeof(rs232_rx_buffer));
		}

	}
}
#else
#include "modbus.h"

extern struct MODBUS_Device modbus_rs232;

void RS232_Console(void)
{
	unsigned int sz;
	static unsigned int rs232_timeout = 0;

	sz = RS232_Read(rs232_rx_buffer + rs232_rx_index, sizeof(rs232_rx_buffer) - rs232_rx_index);
	rs232_rx_index += sz;

  if(rs232_rx_index) {
    if(sz) {
      rs232_timeout  = 0;
    } else if(++rs232_timeout > 3) {
      Modbus_RTU_Receive(&modbus_rs232, Modbus_RTU_Validate((unsigned char *)rs232_rx_buffer, rs232_rx_index, 0));

      rs232_timeout  = 0;
      rs232_rx_index = 0;
    }
  }
}
#endif // RS232_CONSOLE

#endif
