#include "lpc17xx.h"
#include "type.h"
#include "timer.h"
#include "modbus.h"
#include "ld.h"

/******************************************************************************
* Definições
******************************************************************************/
//SSP Status register
#define SSPSR_TNF     1 << 1
#define SSPSR_RNE     1 << 2
#define SSPSR_BSY     1 << 4

#define OUTPUT_CMD_CONTROL  (0 << 2)

#define LSR_RDR         0x01
#define LSR_OE          0x02
#define LSR_PE          0x04
#define LSR_FE          0x08
#define LSR_BI          0x10
#define LSR_THRE        0x20
#define LSR_TEMT        0x40
#define LSR_RXFE        0x80

#define RS485_ENABLE    (1 << 20)

#define HTONS(n) (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))

/******************************************************************************
* Prototipos de Funções
******************************************************************************/
unsigned int ModbusReadCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusTx(unsigned char *data, unsigned int size);

/******************************************************************************
* Variaveis Globais
******************************************************************************/
volatile unsigned int saidas = 0;
volatile unsigned int entradas = 0;

volatile unsigned int timer0_count = 0;

struct MB_Device modbus_device;

struct MB_Handler ModbusHandlers[] =
{
    { MB_FC_READ_COILS              , ModbusReadCoils                 },
    /*
    { MB_FC_READ_DISCRETE_INPUTS    , LPC_MB_Read_Discrete_Inputs     },
    { MB_FC_READ_HOLDING_REGISTERS  , LPC_MB_Read_Holding_Registers   },
    { MB_FC_READ_INPUT_REGISTERS    , LPC_MB_Read_Input_Registers     },
    { MB_FC_WRITE_SINGLE_COIL       , LPC_MB_Write_Single_Coil        },
    { MB_FC_WRITE_SINGLE_REGISTER   , LPC_MB_Write_Single_Register    },
    { MB_FC_WRITE_MULTIPLE_COILS    , LPC_MB_Write_Multiple_Coils     },
    { MB_FC_WRITE_MULTIPLE_REGISTERS, LPC_MB_Write_Multiple_Registers },
    { MB_FC_MASK_WRITE_REGISTER     , LPC_MB_Mask_Write_Register      },
    { MB_FC_RW_MULTIPLE_REGISTERS   , LPC_MB_RW_Multiple_Registers    },
    { MB_FC_READ_EXCEPTION_STATUS   , LPC_MB_Read_Exception_Status    },
    */
};

/******************************************************************************
* Temporizador
******************************************************************************/
void TIMER0_IRQHandler (void)
{
  TIM0->IR = 1;                       /* clear interrupt flag */

  timer0_count++;

  PlcCycle();

  GPIO0->FIOPIN = ~GPIO0->FIOPIN;
}

/******************************************************************************
* Comunicação Serial RS232
******************************************************************************/
unsigned int RS232Write(char * buffer, unsigned int size)
{
  unsigned int sz = 0;

  while (sz < size)
  {
    while (!(UART0->LSR & 0x20) );      /* THRE status, contain valid data */
    UART0->THR = *buffer;
    buffer++;
    sz++;
  }

  return 0;
}

unsigned int RS232Read(char * buffer, unsigned int size)
{
  return 0;
}

/******************************************************************************
* Comunicação RS485
******************************************************************************/
unsigned int RS485Write(unsigned char * buffer, unsigned int size)
{
  GPIO0->FIOSET = RS485_ENABLE;

  while (size)
  {
    while (!(UART3->LSR & 0x20) );      /* THRE status, contain valid data */
    UART3->THR = *buffer;
    buffer++;
    size--;
  }

  GPIO0->FIOCLR = RS485_ENABLE;

  return 0;
}

unsigned int RS485Read(unsigned char * buffer, unsigned int size)
{
  unsigned int i = 0;
  unsigned int c = 0;
  unsigned char dummy;

  while(i < size)
  {
    for(c = 0; c < 10000 && !(UART3->LSR & LSR_RDR); c++);

    if (UART3->LSR & (LSR_OE|LSR_PE|LSR_FE|LSR_BI|LSR_RXFE))
      dummy = UART3->RBR;
    else if ((UART3->LSR & LSR_RDR)) /** barramento tem dados ? */
    {
      *(buffer + i) = UART3->RBR;
      return 1;
    }
    break;
  }

  return i;
}

/******************************************************************************
* Modbus
******************************************************************************/
unsigned int ModbusRequest(unsigned char * rxb, unsigned int rxs, unsigned char * txb, unsigned int txs)
{
  if(rxs > 2)
  {
#ifdef MB_DEBUG
    unsigned int i;
    printf("Modbus: Recebidos %d bytes pela Ethernet:\n", rxs);
    for(i = 0; i < rxs; i++)
      printf("%02x ", rxb[i]);
    printf("\n");
#endif
    //reply_data.data = txb;
    //MB_Receive(&mbdev, MB_Validate(rxb, rxs));
  }

  /*if(reply_data.size)
    packet_out++;*/

  return 0; //reply_data.size; // Retorna o tamanho dos dados
}

unsigned int ModbusReadCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  //unsigned int i, out; // = output_read();
  //unsigned char *buf; // = reply->reply.read_coils.data; // Retorna no maximo 8 bytes ou 256 bits (saidas).

#ifdef MB_DEBUG
  printf("ModBus: recebida funcao READ_COILS. start=%d, quant=%d\n", data->read_coils.start, data->read_coils.quant);
#endif

  /*for(i=0; i<data->read_coils.quant; i++)
    buf[i/8] |= ((out>>(data->read_coils.start+i))&1) << (i%8);*/

  //reply->reply.read_coils.size = data->read_coils.quant/8 + (data->read_coils.quant%8 != 0);

  return MB_EXCEPTION_NONE;

}

unsigned int ModbusDiscreteInput(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return 0;
}

unsigned int ModbusTx(unsigned char *data, unsigned int size)
{
  unsigned int sz = 0;

#ifdef DEBUG
  unsigned int i;
#endif

  if ((sz = RS485Write((unsigned char*)data, size)))
  {
#ifdef DEBUG
    printf("%d bytes enviados para o soft-starter, size: %d\n", sz, size);

    for (i = 0; i < size; i++)
    {
      printf("0x%x ", data[i]);
    }
    printf("\n");
#endif

    timer0_count = 0;
    sz = 0;

    while (timer0_count < 100)
    {
      sz += RS485Read((unsigned char*)data + sz, MB_BUFFER_SIZE);
    }

#ifdef DEBUG
    if (sz)
    {
      printf("%d bytes recebidos do soft-starter em %d ms.\n", sz, tmr);

      for (i = 0; i < sz; i++)
      {
        printf("0x%x ", data[i]);
      }
      printf("\n");

    }
    else
    {
      printf("Timeout na resposta do soft-starter\n");
    }
#endif

  }
  else
  {
#ifdef DEBUG
    printf("Erro ao enviar pacote para soft-starter\n");
#endif
  }

  return sz;
}

/******************************************************************************
* Comunicação SSP
******************************************************************************/
unsigned char SSPWrite(unsigned char * buffer, unsigned int size)
{
  unsigned int i;

  for (i = 0; i < size; i++ )
  {
    while ( !(SSP0->SR & SSPSR_TNF) );
    SSP0->DR = *buffer;
    buffer++;
    while ( SSP0->SR & SSPSR_BSY );
  }
  return i;
}

unsigned char SSPRead(uint8_t * buffer, uint32_t size)
{
  unsigned int i;

  for (i = 0; i < size; i++ )
  {
    while ( !(SSP0->SR & SSPSR_RNE) );
    *buffer = SSP0->DR;
    buffer++;
  }
  return i;
}

/******************************************************************************
* Rotinas de atualização das Entradas/Saidas
******************************************************************************/
unsigned int AtualizaSaidas(void)
{
  unsigned int i = 0;
  unsigned int status = 0;
  unsigned char cmd = OUTPUT_CMD_CONTROL;

  i |= U_S1;
  i |= U_S2 << 1;
  i |= U_S3 << 2;
  i |= U_S4 << 3;
  i |= U_S5 << 4;
  i |= U_S6 << 5;
  i |= U_S7 << 6;
  i |= U_S8 << 7;
  i |= U_S9 << 8;
  i |= U_S10 << 9;
  i |= U_S11 << 10;
  i |= U_S12 << 11;
  i |= U_S13 << 12;
  i |= U_S14 << 13;
  i |= U_S15 << 14;
  i |= U_S16 << 15;

  i = HTONS(i);

  i &= 0xFFFF;

  GPIO0->FIOCLR = 0x10000;
  SSPWrite((unsigned char*)&cmd, 1);
  SSPWrite((unsigned char*)&i, 2);
  GPIO0->FIOSET = 0x10000;

  SSPRead((unsigned char*)&status, 3);

  return i;
}

unsigned int AtualizaEntradas(void)
{
  unsigned int i = 0;

  i |= 0x3FF & GPIO2->FIOPIN;            // P2.0 ~ P2.9    [00..10]
  i |= (0x3800 & GPIO2->FIOPIN) >> 1;    // P2.11 ~ P2.13  [11..13]
  i |= (0x30 & GPIO0->FIOPIN) << 9;      // P0.4 ~ P0.5    [14..15]
  i |= (0x80000 & GPIO0->FIOPIN) >> 4;   // P0.19 ~ P0.19  [16..16]
  i |= (0x600000 & GPIO0->FIOPIN) >> 5;  // P0.21 ~ P0.22  [17..18]

  i = ~i;

  I_E1 = i & 0x1;
  I_E2 = (i & (1 << 1)) >> 1;
  I_E3 = (i & (1 << 2)) >> 2;
  I_E4 = (i & (1 << 3)) >> 3;
  I_E5 = (i & (1 << 4)) >> 4;
  I_E6 = (i & (1 << 5)) >> 5;
  I_E7 = (i & (1 << 6)) >> 6;
  I_E8 = (i & (1 << 7)) >> 7;
  I_E9 = (i & (1 << 8)) >> 8;
  I_E10 = (i & (1 << 9)) >> 9;
  I_E11 = (i & (1 << 10)) >> 10;
  I_E12 = (i & (1 << 11)) >> 11;
  I_E13 = (i & (1 << 12)) >> 12;
  I_E14 = (i & (1 << 13)) >> 13;
  I_E15 = (i & (1 << 14)) >> 14;
  I_E16 = (i & (1 << 15)) >> 15;
  I_E17 = (i & (1 << 16)) >> 16;
  I_E18 = (i & (1 << 17)) >> 17;
  I_E19 = (i & (1 << 18)) >> 18;

  return i;
}

/******************************************************************************
* Inicializacao do Hardware
******************************************************************************/
void HardwareInit(void)
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

  // Inicializacao RS485
  SC->PCONP |= 1 << 25;
  PINCON->PINSEL0 &= ~0xF;
  PINCON->PINSEL0 |= 0xA;

  // WEG SoftStarter 9600 (8, N, 1)
  UART3->LCR = 0x83;          /* 8 bits, no Parity, 1 Stop bit */
  UART3->DLM = 0;
  UART3->DLL = 92;
  UART3->FDR = (13 << 4) | 10;
  UART3->LCR = 0x03;          /* DLAB = 0 */
  UART3->FCR = 0x07;          /* Enable and reset TX and RX FIFO. */

  // Pino de direcao da RS485
  PINCON->PINSEL1 &= ~(0x3 << 8);       // P0.20
  GPIO0->FIODIR |= RS485_ENABLE;        // P0.20 output

  // Entradas
  PINCON->PINSEL0 &= ~0xF00;
  PINCON->PINSEL1 &= ~0x3CC0;
  PINCON->PINSEL4 &= ~0xFCFFFFF;
  GPIO2->FIODIR &= ~0x3BFF;
  GPIO0->FIODIR &= ~0x680030;

  // Saidas (SSP)
  SC->PCONP |= 1 << 21;     // ssp on
  PINCON->PINSEL0 &= ~0xC0000000;
  PINCON->PINSEL0 |= 0x80000000;
  PINCON->PINSEL1 &= ~0x3F;
  PINCON->PINSEL1 |= 0x28; //0x2A; ChipSelect as GPIO
  GPIO0->FIODIR |= 0x10000; // P0.16 as output
  SSP0->CR1 = 0;        // disable, set to master
  SSP0->CR0 = 0x1F87;   // 8 bits, cpol=0, cpha=1, scr=32
  SSP0->CPSR = 6;       // prescaler=6
  SSP0->CR1 |= 0x2;     // enable

  unsigned char cmd = OUTPUT_CMD_CONTROL;
  SSPWrite((unsigned char*)&cmd, 1);

}

/***************************************************************************/
/*  main                                                                   */
/***************************************************************************/
int main (void)
{
  /**************************************************************************
   * Inicializa System
   *************************************************************************/
  SystemInit();
  HardwareInit();

  /**************************************************************************
   * Inicializa timer
   *************************************************************************/
  init_timer( 0, TIME_INTERVAL ); // 5ms
  enable_timer( 0 );

  /**************************************************************************
   * Inicialização do ModBus
   *************************************************************************/
  MB_Init(&modbus_device);

  modbus_device.identification.Id                 = 0x01;
  modbus_device.identification.VendorName         = "Tecnequip";
  modbus_device.identification.ProductCode        = "POP7";
  modbus_device.identification.MajorMinorRevision = "V1 Rev2";

  modbus_device.hl      = ModbusHandlers;
  modbus_device.hl_size = ARRAY_SIZE(ModbusHandlers);
  modbus_device.mode    = MB_MODE_SLAVE;
  modbus_device.TX      = ModbusTx;

  while(1)
  {
    AtualizaEntradas();
    AtualizaSaidas();
	U_M1 = 1;	// Automatico
  }

  return(0);
}

/*** EOF ***/
