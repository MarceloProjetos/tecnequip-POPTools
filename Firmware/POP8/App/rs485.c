/******************************************************************************
* Comunicação RS485
******************************************************************************/
#include "chip.h"
#include "rs485.h"
#include "uss.h"
#include "modbus.h"

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
	// Configura os pinos RXD e TXD para a funcao da serial
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_HYS_EN); // TXD
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 1, IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_HYS_EN); // RXD

	// P0.20: Controle de direcao da RS-485
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 20, IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_HYS_EN);
	Chip_GPIO_SetDir(LPC_GPIO, 0, 20, 1); // Define pino como Saida
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 20, 0); // Configura nivel baixo para ativar recepcao

	// Pino que controla o RS485/CAN Select (RS485=0, CAN=1)
	Chip_GPIO_SetDir(LPC_GPIO, 1, 25, 1); // Define pino como Saida
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 25, 0); // Configura nivel baixo para selecionar RS485

	Chip_UART_Init(LPC_UART3);
	Chip_UART_SetBaud(LPC_UART3, 9600);
	Chip_UART_TXEnable(LPC_UART3);
}

unsigned int RS485_Write(unsigned char * buffer, unsigned int size)
{
  unsigned int i;

  Chip_GPIO_SetPinState(LPC_GPIO, 0, 20, 1); // Configura nivel alto para ativar transmissao
  i = Chip_UART_SendBlocking(LPC_UART3, buffer, size);

  // Espera enviar todos os bytes antes de desativar a transmissao
  while(!(Chip_UART_ReadLineStatus(LPC_UART3) & UART_LSR_TEMT))
	  ;

  Chip_GPIO_SetPinState(LPC_GPIO, 0, 20, 0); // Configura nivel baixo para ativar recepcao

  return i;
}

unsigned int RS485_Read(unsigned char * buffer, unsigned int size)
{
  unsigned int i = 0;

  if (!Chip_GPIO_GetPinState(LPC_GPIO, 0, 20)) {
	  i = Chip_UART_Read(LPC_UART3, buffer, size);
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
	uint32_t config = 0;

	switch(bits)
	{
	case 5:
		config |= UART_LCR_WLEN5;
		break;
	case 6:
		config |= UART_LCR_WLEN6;
		break;
	case 7:
		config |= UART_LCR_WLEN7;
		break;
	default:  // 8 bits
		config |= UART_LCR_WLEN8;
	}

	if (stopbit == 2)
		config |= UART_LCR_SBS_2BIT;

	switch(parity)
	{
	case 1: // odd
		config |= UART_LCR_PARITY_EN | UART_LCR_PARITY_ODD;
		break;
	case 2: // even
		config |= UART_LCR_PARITY_EN | UART_LCR_PARITY_EVEN;
		break;
	}

	// Desabilita transmissao
	Chip_UART_TXDisable(LPC_UART3);

	// Reseta FIFOs
	Chip_UART_SetupFIFOS(LPC_UART3, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS));

	// Configura serial
	Chip_UART_SetBaud(LPC_UART3, baudrate);
	Chip_UART_ConfigData(LPC_UART3, config);

	// Ativa transmissao
	Chip_UART_TXEnable(LPC_UART3);
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

