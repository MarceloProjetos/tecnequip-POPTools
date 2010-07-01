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

typedef struct ad_type
{
  unsigned int m1;
  unsigned int m2;
  unsigned int m3;
  unsigned int m4;
  unsigned int m5;
};

struct ad_type ad[5];

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

unsigned int ModbusReadCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusReadDiscreteInputs(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusReadHoldingRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusReadInputRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusWriteSingleCoil(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusWriteSingleRegister(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusWriteMultipleCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusWriteMultipleRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusMaskWriteRegister(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusRWMultipleRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusReadExceptionStatus(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);

struct MB_Handler ModbusHandlers[] =
{
    { MB_FC_READ_COILS              , ModbusReadCoils              },
    { MB_FC_READ_DISCRETE_INPUTS    , ModbusReadDiscreteInputs     },
    { MB_FC_READ_HOLDING_REGISTERS  , ModbusReadHoldingRegisters   },
    { MB_FC_READ_INPUT_REGISTERS    , ModbusReadInputRegisters     },
    { MB_FC_WRITE_SINGLE_COIL       , ModbusWriteSingleCoil        },
    { MB_FC_WRITE_SINGLE_REGISTER   , ModbusWriteSingleRegister    },
    { MB_FC_WRITE_MULTIPLE_COILS    , ModbusWriteMultipleCoils     },
    { MB_FC_WRITE_MULTIPLE_REGISTERS, ModbusWriteMultipleRegisters },
    { MB_FC_MASK_WRITE_REGISTER     , ModbusMaskWriteRegister      },
    { MB_FC_RW_MULTIPLE_REGISTERS   , ModbusRWMultipleRegisters    },
    { MB_FC_READ_EXCEPTION_STATUS   , ModbusReadExceptionStatus    },
};

struct
{
  uint8_t * data;
  uint32_t  size;
} reply_data;

unsigned char modbus_rx_buffer[MB_BUFFER_SIZE];
unsigned char modbus_tx_buffer[MB_BUFFER_SIZE];

/******************************************************************************
* Temporizador
******************************************************************************/
void TIMER0_IRQHandler (void)
{
  TIM0->IR = 1;                       /* clear interrupt flag */

  timer0_count++;

  PlcCycle();
}

/******************************************************************************
* ADC Read
******************************************************************************/
unsigned int ADCRead(unsigned int a)
{
  unsigned int max, min;
  unsigned int i = 0;

  unsigned int soma = 0;

  max = 0;
  min = 0;

  if (a < 1 || a > 5) return 0;

  a -= 1;

  ad[a].m1 = ad[a].m2;
  ad[a].m2 = ad[a].m3;
  ad[a].m3 = ad[a].m4;
  ad[a].m4 = ad[a].m5;

  switch (a)
  {
  case 0:
    ad[a].m5 = 0xFFF & (ADC->ADDR5 >> 4);
    break;
  case 1:
    ad[a].m5 = 0xFFF & (ADC->ADDR2 >> 4);
    break;
  case 2:
    ad[a].m5 = 0xFFF & (ADC->ADDR1 >> 4);
    break;
  case 3:
    ad[a].m5 = 0xFFF & (ADC->ADDR0 >> 4);
    break;
  case 4:
    ad[a].m5 = 0xFFF & (ADC->ADDR3 >> 4);
    break;
  }

  if (max < ad[a].m1)
    max = ad[a].m1;
  if (max < ad[a].m2)
    max = ad[a].m2;
  if (max < ad[a].m3)
    max = ad[a].m3;
  if (max < ad[a].m4)
    max = ad[a].m4;
  if (max < ad[a].m5)
    max = ad[a].m5;

  min = max;

  if (min > ad[a].m1)
    min = ad[a].m1;
  if (min > ad[a].m2)
    min = ad[a].m2;
  if (min > ad[a].m3)
    min = ad[a].m3;
  if (min > ad[a].m4)
    min = ad[a].m4;
  if (min > ad[a].m5)
    min = ad[a].m5;

  if (ad[a].m1 > min && ad[a].m1 < max)
  {
    soma += ad[a].m1; i++;
  }

  if (ad[a].m2 > min && ad[a].m2 < max)
  {
    soma += ad[a].m2; i++;
  }

  if (ad[a].m3 > min && ad[a].m3 < max)
  {
    soma += ad[a].m3; i++;
  }

  if (ad[a].m4 > min && ad[a].m4 < max)
  {
    soma += ad[a].m4; i++;
  }

  if (ad[a].m5 > min && ad[a].m5 < max)
  {
    soma += ad[a].m5; i++;
  }

  return i > 0 ? (unsigned int)(soma / i) : 0;
}

/******************************************************************************
* Comunicação Serial RS232
******************************************************************************/
unsigned int RS232Write(unsigned int c)
{
  while (!(UART0->LSR & LSR_TEMT) );      /* THRE status, contain valid data */
  UART0->THR = (char)c;
  while (!(UART0->LSR & LSR_TEMT) );

  return 0;  // not busy
}

unsigned int RS232Read(void)
{
  return 0;
}

/******************************************************************************
* Comunicação RS485
******************************************************************************/
unsigned int RS485Write(unsigned char * buffer, unsigned int size)
{
  unsigned int i = 0;

  GPIO0->FIOSET = RS485_ENABLE;

  while (!(UART3->LSR & LSR_TEMT) );      /* THRE status, contain valid data */

  while (i < size)
  {
    UART3->THR = *(buffer + i++);
    while(!(UART3->LSR & LSR_TEMT));      /* THRE status, contain valid data */
  }

  GPIO0->FIOCLR = RS485_ENABLE;

  return i;
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
      *(buffer + i++) = UART3->RBR;
    else
      break;
  }

  return i;
}

/******************************************************************************
* Modbus
******************************************************************************/
unsigned int ModbusTx(unsigned char *data, unsigned int size)
{
  unsigned int sz = 0;

#ifdef DEBUG
  unsigned int i;
#endif

  if ((sz = RS485Write(data, size)))
  {
#ifdef DEBUG
    printf("%d bytes enviados para o soft-starter, size: %d\n", sz, size);

    for (i = 0; i < size; i++)
    {
      printf("0x%x ", data[i]);
    }
    printf("\n");
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

unsigned int ModbusRequest(void)
{
  unsigned int sz = 0;

  memset(modbus_rx_buffer, 0, sizeof(modbus_rx_buffer));
  memset(modbus_tx_buffer, 0, sizeof(modbus_tx_buffer));

  if((sz = RS485Read(modbus_rx_buffer, sizeof(modbus_rx_buffer))) > 0)
  {
    reply_data.data = modbus_tx_buffer;
    if (sz > 2)
      MB_Receive(&modbus_device, MB_Validate(modbus_rx_buffer, sz));
  }

  return reply_data.size; // Retorna o tamanho dos dados
}

unsigned int ModbusReadCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  uint32_t i;
  uint32_t temp = ((saidas >> 8) & 0xFF) | ((saidas << 8) & 0xFF00);
  uint8_t *buf = reply->reply.read_coils.data; // Retorna no maximo 8 bytes ou 256 bits (saidas).

  reply->reply.read_coils.size = data->read_coils.quant / 8 + (data->read_coils.quant % 8 != 0);

  memset(buf, 0, reply->reply.read_coils.size);

  for(i = 0; i < data->read_coils.quant; i++)
    buf[i / 8] |= ((temp >> (data->read_coils.start + i)) & 1) << (i % 8);

#ifdef MB_DEBUG
  printf("ModBus: recebida funcao READ_COILS. start=%d, quant=%d\n", data->read_coils.start, data->read_coils.quant);
#endif

  return MB_EXCEPTION_NONE;

}

unsigned int ModbusReadDiscreteInputs(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  uint32_t i;
  uint8_t *buf = reply->reply.read_discrete_inputs.data; // Retorna no maximo 8 bytes ou 256 bits (entradas).

#ifdef MB_DEBUG
  printf("ModBus: recebida funcao READ_DISCRETE_INPUTS. start=%d, quant=%d\n",
  data->read_discrete_inputs.start, data->read_discrete_inputs.quant);
#endif

  for(i = 0; i < data->read_discrete_inputs.quant; i++)
    buf[i / 8] |= ((entradas >> (data->read_discrete_inputs.start + i)) & 1) << (i % 8);

  reply->reply.read_discrete_inputs.size = data->read_discrete_inputs.quant / 8 + (data->read_discrete_inputs.quant % 8 != 0);

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusReadHoldingRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  uint32_t sz = 0;

  uint32_t i = 0;
  uint8_t *buf = reply->reply.read_holding_registers.data;

  for(i = 0; i < data->read_holding_registers.quant; i++)
  {
    switch(data->read_holding_registers.start + i)
    {
    case 1:
      buf[(i + sz)] = (uint8_t)(U_M1 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M1);
      sz += 2;
      break;
    case 2:
      buf[(i + sz)] = (uint8_t)(U_M2 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M2);
      sz += 2;
      break;
    case 3:
      buf[(i + sz)] = (uint8_t)(U_M3 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M3);
      sz += 2;
      break;
    case 4:
      buf[(i + sz)] = (uint8_t)(U_M4 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M4);
      sz += 2;
      break;
    case 5:
      buf[(i + sz)] = (uint8_t)(U_M5 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M5);
      sz += 2;
      break;
    case 6:
      buf[(i + sz)] = (uint8_t)(U_M6 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M6);
      sz += 2;
      break;
    case 7:
      buf[(i + sz)] = (uint8_t)(U_M7 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M7);
      sz += 2;
      break;
    case 8:
      buf[(i + sz)] = (uint8_t)(U_M8 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M8);
      sz += 2;
      break;
    case 9:
      buf[(i + sz)] = (uint8_t)(U_M9 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M9);
      sz += 2;
      break;
    case 10:
      buf[(i + sz)] = (uint8_t)(U_M10 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M10);
      sz += 2;
      break;
    case 11:
      buf[(i + sz)] = (uint8_t)(U_M11 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M11);
      sz += 2;
      break;
    case 12:
      buf[(i + sz)] = (uint8_t)(U_M12 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M12);
      sz += 2;
      break;
    case 13:
      buf[(i + sz)] = (uint8_t)(U_M13 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M13);
      sz += 2;
      break;
    case 14:
      buf[(i + sz)] = (uint8_t)(U_M14 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M14);
      sz += 2;
      break;
    case 15:
      buf[(i + sz)] = (uint8_t)(U_M15 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M15);
      sz += 2;
      break;
    case 16:
      buf[(i + sz)] = (uint8_t)(U_M16 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M16);
      sz += 2;
      break;
    case 17:
      buf[(i + sz)] = (uint8_t)(U_M17 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M17);
      sz += 2;
      break;
    case 18:
      buf[(i + sz)] = (uint8_t)(U_M18 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M18);
      sz += 2;
      break;
    case 19:
      buf[(i + sz)] = (uint8_t)(U_M19 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M19);
      sz += 2;
      break;
    case 20:
      buf[(i + sz)] = (uint8_t)(U_M20 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M20);
      sz += 2;
      break;
    case 21:
      buf[(i + sz)] = (uint8_t)(U_M21 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M21);
      sz = 2;
      break;
    case 22:
      buf[(i + sz)] = (uint8_t)(U_M22 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M22);
      sz += 2;
      break;
    case 23:
      buf[(i + sz)] = (uint8_t)(U_M23 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M23);
      sz += 2;
      break;
    case 24:
      buf[(i + sz)] = (uint8_t)(U_M24 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M24);
      sz += 2;
      break;
    case 25:
      buf[(i + sz)] = (uint8_t)(U_M25 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M25);
      sz += 2;
      break;
    case 26:
      buf[(i + sz)] = (uint8_t)(U_M26 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M26);
      sz += 2;
      break;
    case 27:
      buf[(i + sz)] = (uint8_t)(U_M27 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M27);
      sz += 2;
      break;
    case 28:
      buf[(i + sz)] = (uint8_t)(U_M28 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M28);
      sz += 2;
      break;
    case 29:
      buf[(i + sz)] = (uint8_t)(U_M29 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M29);
      sz += 2;
      break;
    case 30:
      buf[(i + sz)] = (uint8_t)(U_M30 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M30);
      sz += 2;
      break;
    case 31:
      buf[(i + sz)] = (uint8_t)(U_M31 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M31);
      sz += 2;
      break;
    case 32:
      buf[(i + sz)] = (uint8_t)(U_M32 >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M32);
      sz += 2;
      break;
    default:
      return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
    }
  }

  reply->reply.read_holding_registers.size = sz;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusReadInputRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusWriteSingleCoil(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  if(data->write_single_coil.val)
    saidas |= 1 << data->write_single_coil.output;
  else
    saidas &= ~(1 << data->write_single_coil.output);

  /*switch(data->write_single_coil.output)
  {
  case 1:
    if(data->write_single_coil.val)
      I_C1 |= 1 << data->write_single_coil.output;
    else
      I_C1 &= ~(1 << data->write_single_coil.output);
  case 2:
    if(data->write_single_coil.val)
      I_C1 |= 1 << data->write_single_coil.output;
    else
      I_C1 &= ~(1 << data->write_single_coil.output);
  default:
    return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  }*/

  reply->reply.write_single_coil = data->write_single_coil;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusWriteSingleRegister(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  switch(data->write_single_register.address)
  {
  case 1:
    I_M1 = data->write_single_register.val;
    break;
  case 2:
    I_M2 = data->write_single_register.val;
    break;
  case 3:
    I_M3 = data->write_single_register.val;
    break;
  case 4:
    I_M4 = data->write_single_register.val;
    break;
  case 5:
    I_M5 = data->write_single_register.val;
    break;
  case 6:
    I_M6 = data->write_single_register.val;
    break;
  case 7:
    I_M7 = data->write_single_register.val;
    break;
  case 8:
    I_M8 = data->write_single_register.val;
    break;
  case 9:
    I_M9 = data->write_single_register.val;
    break;
  case 10:
    I_M10 = data->write_single_register.val;
    break;
  case 11:
    I_M11 = data->write_single_register.val;
    break;
  case 12:
    I_M12 = data->write_single_register.val;
    break;
  default:
    return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  }

  reply->reply.write_single_register = data->write_single_register;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusWriteMultipleCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusWriteMultipleRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusMaskWriteRegister(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusRWMultipleRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusReadExceptionStatus(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
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

  // AD's
  SC->PCONP |= 1 << 12;         // ADC
  PINCON->PINSEL1 &= ~((0x3 << 14) | (0x3 << 16) | (0x3 << 18) | (0x3 << 20)); // AD0.0, AD0.1, AD0.2, AD0.3(REF)
  PINCON->PINSEL1 |= (0x1 << 14) | (0x1 << 16) | (0x1 << 18) | (0x1 << 20);
  PINCON->PINSEL3 |= 0x3 << 30; // AD0.5
  ADC->ADCR = 0x2F | (0x18 << 8) | (1 << 16) | (1 << 21); // Enable AD0:1:2:3:5, 1Mhz, BURST ON, PDN Enable

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
    entradas = AtualizaEntradas();
    saidas = AtualizaSaidas();
    ModbusRequest();
  }

  return(0);
}

/*** EOF ***/
