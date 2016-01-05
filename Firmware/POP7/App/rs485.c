/******************************************************************************
* Comunicação RS485
******************************************************************************/
#include "lpc17xx.h"
#include "lpc17xx_uart.h"
#include "serial.h"
#include "rs485.h"
#include "uss.h"
#include "modbus.h"

#define RS485_ENABLE    (1 << 20)

#define MAX_RETRIES 1

volatile unsigned int I_SerialReady = 1;
volatile unsigned int I_SerialTimeout = 0;
volatile unsigned int I_SerialAborted = 0;
volatile unsigned int I_USSReady;
volatile unsigned int I_ModbusReady;

volatile unsigned char WAITING_FOR_USS = 0;	//
volatile unsigned char WAITING_FOR_YASKAWA = 0;	//
volatile unsigned char WAITING_FOR_FMTSTR = 0;	//

volatile unsigned int rs485_timeout = 0;
volatile unsigned int rs485_reset_timeout = 0;

unsigned char rs485_rx_buffer[SERIAL_BUFFER_SIZE];

unsigned int rs485_rx_index = 0;
unsigned int rs485_tx_index = 0;
unsigned int rs485_tx_count = 0;

extern struct MODBUS_Device modbus_rs485;

void RS485_Init()
{
	// Inicializacao RS485
	SC->PCONP |= 1 << 25;
	PINCON->PINSEL0 &= ~0xF;
	PINCON->PINSEL0 |= 0xA;

	// WEG SoftStarter 9600 (8, N, 1)
	#ifndef NORD_USS_ENABLE
	UART3->LCR = 0x83;          /* 8 bits, no parity, 1 stop bit */
	#else
	UART3->LCR = 0x9B;          /* 8 bits, even-parity, 1 stop bit */
	#endif

	// 9600
	UART3->DLM = 0;
	UART3->DLL = 92;
	UART3->FDR = (13 << 4) | 10;

	// 9091
	//UART3->DLM = 0;
	//UART3->DLL = 125;
	//UART3->FDR = (8 << 4) | 3;

	// 9259
	//UART3->DLM = 0;
	//UART3->DLL = 135;
	//UART3->FDR = (4 << 4) | 1;

	#ifndef NORD_USS_ENABLE
	UART3->LCR = 0x03;          /* DLAB = 0, 8 bits, no parity, 1 stop bit */
	#else
	UART3->LCR = 0x1B;          /* DLAB = 0, 8 bits, even-parity, 1 stop bit */
	#endif
	UART3->FCR = 0x07;          /* Enable and reset TX and RX FIFO. */

	//rs485_open(3, 19200, 8, 1, 1);

	// Pino de direcao da RS485
	PINCON->PINSEL1 &= ~(0x3 << 8);       // P0.20
	GPIO0->FIODIR |= RS485_ENABLE;        // P0.20 output
}
unsigned int RS485_Write(unsigned char * buffer, unsigned int size)
{
  unsigned int i = 0;

  //UART3->FCR = 0x02;                      /* Reset RX */
  //UART3->FCR = 0x04;                      /* Reset TX */

  GPIO0->FIOSET = RS485_ENABLE;

  while (!(UART3->LSR & UART_LSR_TEMT) );      /* THRE status, contain valid data */

  while (i < size)
  {
    UART3->THR = *(buffer + i++);
    while(!(UART3->LSR & UART_LSR_TEMT));      /* THRE status, contain valid data */
  }

  GPIO0->FIOCLR = RS485_ENABLE;

  return i;
}

unsigned int RS485_Read(unsigned char * buffer, unsigned int size)
{
  unsigned int i = 0;
  unsigned char dummy;

  if (!(GPIO0->FIOPIN & RS485_ENABLE))
  {
	  while(i < size)
	  {
		if (UART3->LSR & (UART_LSR_OE|UART_LSR_PE|UART_LSR_FE|UART_LSR_BI|UART_LSR_RXFE))
		  dummy = UART3->RBR;
		else if ((UART3->LSR & UART_LSR_RDR)) /** barramento tem dados ? */
		  *(buffer + i++) = UART3->RBR;
		else
		  break;
	  }
  }

  return i;
}

unsigned int RS485_ReadChar(unsigned char * buffer)
{
	if(rs485_rx_index > 0) {
		rs485_rx_index--;
		*buffer = rs485_rx_buffer[0];
		memcpy(rs485_rx_buffer, rs485_rx_buffer + 1, rs485_rx_index);
		return 1;
	}

	return 0;
}

void RS485_Config(int baudrate, int bits, int parity, int stopbit)
{
	UART3->FCR = 0x7; // FIFO TX/RX Enable/Reset
	UART3->LCR = 1 << 7; // Enable DLAB

	switch(bits)
	{
	case 5:
		break;
	case 6:
		UART3->LCR |= 0x1;
		break;
	case 7:
		UART3->LCR |= 0x2;
		break;
	default:  // 8 bits
		UART3->LCR |= 0x3;
	}

	if (stopbit == 2)
		UART3->LCR |= 0x4;

	switch(parity)
	{
	case 1: // odd
		UART3->LCR |= 0x8;
		break;
	case 2: // even
		UART3->LCR |= 0x18;
		break;
	}

	if (baudrate == 19200)
	{
		UART3->DLM = 0;
		UART3->DLL = 46;
		UART3->FDR = (13 << 4) | 10;
	}
	else if (baudrate == 14400)
	{
		UART3->DLM = 0;
		UART3->DLL = 93;
		UART3->FDR = (6 << 4) | 1;
	}
	else  // 9600
	{
		UART3->DLM = 0;
		UART3->DLL = 92;
		UART3->FDR = (13 << 4) | 10;
	}
	UART3->LCR &= ~(1 << 7); // Disable DLAB
}

void RS485_Handler (unsigned int cycle)
{
	unsigned int sz;
	static unsigned int retries = 0;

	sz = RS485_Read(rs485_rx_buffer + rs485_rx_index, sizeof(rs485_rx_buffer) - rs485_rx_index);
	rs485_rx_index += sz;

	if(rs485_rx_index) {
		if(sz) {
			rs485_timeout  = cycle + 3;
		} else if(cycle >= rs485_timeout) {
			// Se aguardando por uma string formatada, nenhum protocolo deve interpretar strings
			// pois podem ser recebidos comandos invalidos que gerem erros ou mesmo o buffer ser descartado
			// antes de o objeto de string formatada conseguir interpretar os dados.
			if (WAITING_FOR_FMTSTR == 0) { // formatted string
				if (WAITING_FOR_USS == 1){ // uss
					USS_Ready(rs485_rx_buffer, rs485_rx_index);
					rs485_rx_index = 0;
					WAITING_FOR_USS = 0;
				} else if (WAITING_FOR_YASKAWA == 0) { // modbus
					Modbus_Request(&modbus_rs485, rs485_rx_buffer, rs485_rx_index);
					rs485_rx_index = 0;
				}
			}
		}
	}

  if(I_SerialReady) {
    retries = 0;
    I_SerialTimeout = 0;
    I_SerialAborted = 0;
    rs485_reset_timeout = cycle + 50;
  } else {
    if(cycle >= rs485_reset_timeout && !I_SerialTimeout && retries < MAX_RETRIES) {
      retries++;
      I_SerialTimeout = 1;
	  rs485_reset_timeout = cycle + 50;
    } else if (cycle >= rs485_reset_timeout && retries >= MAX_RETRIES && !I_SerialAborted) {
    	I_SerialAborted = 1;
    } else if (cycle >= (rs485_reset_timeout + 250)) {
      retries = 0;
      I_SerialReady = 1;
      I_SerialTimeout = 0;
      I_SerialAborted = 0;
	  WAITING_FOR_USS = 0;
	  //WAITING_FOR_YASKAWA = 0;
    }
  }

}

