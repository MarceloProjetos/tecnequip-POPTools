#include "intreg.h"
#include "modbus.h"
#include "rtc.h"

extern volatile unsigned int GPIO_OUTPUT;
extern volatile unsigned int GPIO_INPUT;
extern unsigned int OUTPUT_FORCED_MASK;
extern unsigned int OUTPUT_FORCED_STATE;

extern volatile unsigned int I_TcpReady;
extern volatile unsigned int I_SerialReady;

static char *AppName;

// Declaracao do ID do ModBUS e funcao para configura-lo
static unsigned int ModBUSID = 0;

void ModBUS_SetID(unsigned int id)
{
	ModBUSID = id;
}

MODBUS_HANDLER_TX(Modbus_RS485_Tx)
{
  RS485_Write(data, size);

  I_SerialReady = 0;

  return 0; // Retorna zero pois, se aguardarmos pela resposta, o ladder ficara bloqueado!
}

MODBUS_HANDLER_TX(Modbus_RS232_Tx)
{
  RS232_Write((char *)data, size);

  return 0; // Retorna zero pois, se aguardarmos pela resposta, o ladder ficara bloqueado!
}

volatile unsigned char MODBUS_RS485_MASTER = 0;  // 0 = Slave, 1 = Master
volatile unsigned char MODBUS_TCP_MASTER   = 0;  // 0 = Slave, 1 = Master

volatile int * MODBUS_RETURN_VAL = NULL;

struct MODBUS_Device modbus_rs485;
struct MODBUS_Device modbus_rs232;
struct MODBUS_Device modbus_tcp;

void ModBUS_SetAppName(char *appname)
{
	AppName = appname;
}

/*** ModBUS Handlers ***/

unsigned int Modbus_ReadCoils(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  uint32_t i;
  uint8_t *buf = reply->reply.read_coils.data; // Retorna no maximo 8 bytes ou 256 bits (saidas).

  reply->reply.read_coils.size = data->read_coils.quant / 8 + (data->read_coils.quant % 8 != 0);

  memset(buf, 0, reply->reply.read_coils.size);

  for(i = 0; i < data->read_coils.quant; i++)
    buf[i / 8] |= ((GPIO_OUTPUT >> (data->read_coils.start + i)) & 1) << (i % 8);

  return MODBUS_EXCEPTION_NONE;

}

unsigned int Modbus_ReadDiscreteInputs(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  uint32_t i;
  uint8_t *buf = reply->reply.read_discrete_inputs.data; // Retorna no maximo 8 bytes ou 256 bits (entradas).

  for(i = 0; i < data->read_discrete_inputs.quant; i++)
    buf[i / 8] |= ((GPIO_INPUT >> (data->read_discrete_inputs.start + i)) & 1) << (i % 8);

  reply->reply.read_discrete_inputs.size = data->read_discrete_inputs.quant / 8 + (data->read_discrete_inputs.quant % 8 != 0);

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_ReadHoldingRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  int i, val;

  if (IntReg_CheckBounds(data->read_holding_registers.start, data->read_holding_registers.quant))
  {
	  //memcpy(reply->reply.read_holding_registers.data, (void *)&M[data->read_holding_registers.start], data->read_holding_registers.quant * 2);
	  for(i = 0; i <= data->read_holding_registers.quant; i++) {
		val = IntReg_Read(i + data->read_holding_registers.start);
		reply->reply.read_holding_registers.data[i*2    ] = val;
		reply->reply.read_holding_registers.data[i*2 + 1] = val >> 8;
	  }
  }
  else
	  return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.read_holding_registers.size = data->read_holding_registers.quant * 2;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_ReadInputRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_WriteSingleCoil(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
	unsigned int mask = (1UL << data->write_single_coil.output);
	OUTPUT_FORCED_MASK &= ~mask;
	if(data->write_single_coil.val) {
		OUTPUT_FORCED_STATE |= mask;
	} else {
		OUTPUT_FORCED_STATE &= ~mask;
	}

  reply->reply.write_single_coil = data->write_single_coil;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_WriteSingleRegister(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  if (IntReg_CheckBounds(data->write_single_register.address, 1))
    IntReg_Write(data->write_single_register.address, (short int)data->write_single_register.val);
  else
    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.write_single_register = data->write_single_register;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_WriteMultipleCoils(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
	unsigned int i, mask = 0;

	if(data->write_multiple_coils.start + data->write_multiple_coils.quant > 16) {
		return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
	}

	for(i = 0; i < data->write_multiple_coils.quant; i++) {
		mask <<= 1;
		mask  |= 1;

		if(data->write_multiple_coils.val[i/8] & (1UL << (i % 8))) {
			OUTPUT_FORCED_STATE |=   1UL << (i + data->write_multiple_coils.start);
		} else {
			OUTPUT_FORCED_STATE &= ~(1UL << (i + data->write_multiple_coils.start));
		}
	}

	OUTPUT_FORCED_MASK &= ~(mask << data->write_multiple_coils.start);

	reply->reply.write_multiple_coils  = data->write_multiple_coils;

	return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_WriteMultipleRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
	int i, pos;

  if (IntReg_CheckBounds(data->write_multiple_registers.start, data->write_multiple_registers.quant))
  {
    for (i = 0; i < data->write_multiple_registers.size; i += 2)
    {
      pos = data->write_multiple_registers.start + (i / 2);
      IntReg_Write(pos, *(short int *)(&data->write_multiple_registers.val[i]));
    }
  } else
    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.write_multiple_registers = data->write_multiple_registers;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_MaskWriteRegister(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  reply->reply.mask_write_register = data->mask_write_register;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_RWMultipleRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  reply->reply.read_holding_registers.size = 0;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_ReadExceptionStatus(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  reply->reply.read_exception_status.status = 0x00;

  return MODBUS_EXCEPTION_NONE;
}

struct MODBUS_Handler ModbusHandlers[] =
{
    { MODBUS_FC_READ_COILS              , Modbus_ReadCoils              },
    { MODBUS_FC_READ_DISCRETE_INPUTS    , Modbus_ReadDiscreteInputs     },
    { MODBUS_FC_READ_HOLDING_REGISTERS  , Modbus_ReadHoldingRegisters   },
    { MODBUS_FC_READ_INPUT_REGISTERS    , Modbus_ReadInputRegisters     },
    { MODBUS_FC_WRITE_SINGLE_COIL       , Modbus_WriteSingleCoil        },
    { MODBUS_FC_WRITE_SINGLE_REGISTER   , Modbus_WriteSingleRegister    },
    { MODBUS_FC_WRITE_MULTIPLE_COILS    , Modbus_WriteMultipleCoils     },
    { MODBUS_FC_WRITE_MULTIPLE_REGISTERS, Modbus_WriteMultipleRegisters },
    { MODBUS_FC_MASK_WRITE_REGISTER     , Modbus_MaskWriteRegister      },
    { MODBUS_FC_RW_MULTIPLE_REGISTERS   , Modbus_RWMultipleRegisters    },
    { MODBUS_FC_READ_EXCEPTION_STATUS   , Modbus_ReadExceptionStatus    },
};

void Modbus_Init(void)
{
	Modbus_RTU_Init(&modbus_rs485);

	modbus_rs485.identification.Id                  = ModBUSID;
	modbus_rs485.identification.VendorName          = "Tecnequip";
	modbus_rs485.identification.ProductCode         = "POP7";
	modbus_rs485.identification.MajorMinorRevision  = "V1 Rev1";
	modbus_rs485.identification.UserApplicationName = AppName;

	modbus_rs485.hl      = ModbusHandlers;
	modbus_rs485.hl_size = ARRAY_SIZE(ModbusHandlers);
	modbus_rs485.mode    = MODBUS_RS485_MASTER ? MODBUS_MODE_MASTER : MODBUS_MODE_SLAVE;
	modbus_rs485.TX      = Modbus_RS485_Tx;

	Modbus_RTU_Init(&modbus_rs232);

	modbus_rs232.identification.Id                  = 0x01;
	modbus_rs232.identification.VendorName          = "Tecnequip";
	modbus_rs232.identification.ProductCode         = "POP7";
	modbus_rs232.identification.MajorMinorRevision  = "V1 Rev1";
	modbus_rs232.identification.UserApplicationName = AppName;

	modbus_rs232.hl      = ModbusHandlers;
	modbus_rs232.hl_size = ARRAY_SIZE(ModbusHandlers);
	modbus_rs232.mode    = MODBUS_MODE_SLAVE;
	modbus_rs232.TX      = Modbus_RS232_Tx;

	Modbus_RTU_Init(&modbus_tcp);

	modbus_tcp.identification.Id                  = ModBUSID;
	modbus_tcp.identification.VendorName          = "Tecnequip";
	modbus_tcp.identification.ProductCode         = "POP7";
	modbus_tcp.identification.MajorMinorRevision  = "V1 Rev1";
	modbus_tcp.identification.UserApplicationName = AppName;

	modbus_tcp.hl      = ModbusHandlers;
	modbus_tcp.hl_size = ARRAY_SIZE(ModbusHandlers);
	modbus_tcp.mode    = MODBUS_TCP_MASTER ? MODBUS_MODE_TCP_MASTER : MODBUS_MODE_TCP_SLAVE;
	modbus_tcp.TX      = MODBUS_TCP_MASTER ? Modbus_TCP_Master_Tx   : Modbus_TCP_Slave_Tx;

	memset((void*)MODBUS_REGISTER, 0, sizeof(MODBUS_REGISTER));

}

void Modbus_Send(unsigned char id,
                  unsigned long ip,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value)
{
  uint8_t out[] = { 0, 0 };

  MODBUS_RETURN_VAL = value;

  union MODBUS_FCD_Data data;
  struct MODBUS_Device *dev;

  if(ip) {
	  dev = &modbus_tcp;
  } else {
	  dev = &modbus_rs485;
  }

  dev->identification.Id = id;
  dev->identification.Ip = ip;

  /* Enviando mensagem pelo Modbus */
  switch(fc)
  {
  case MODBUS_FC_READ_COILS:
    data.read_coils.start = address;
    data.read_coils.quant = size;
    break;
  case MODBUS_FC_READ_DISCRETE_INPUTS:
    data.read_discrete_inputs.start = address;
    data.read_discrete_inputs.quant = size;
    break;
  case MODBUS_FC_READ_HOLDING_REGISTERS:
    data.read_holding_registers.start = address;
    data.read_holding_registers.quant = size;
    break;
  case MODBUS_FC_READ_INPUT_REGISTERS:
    data.read_input_registers.start = address;
    data.read_input_registers.quant = size;
    break;
  case MODBUS_FC_WRITE_SINGLE_COIL:
    data.write_single_coil.output = address;
    data.write_single_coil.val    = *value;
    break;
  case MODBUS_FC_WRITE_SINGLE_REGISTER:
    data.write_single_register.address = address;
    data.write_single_register.val     = *value;
    break;
  case MODBUS_FC_WRITE_MULTIPLE_COILS:
    data.write_multiple_coils.start = address;
    data.write_multiple_coils.quant = size;
    data.write_multiple_coils.size  = 2 * size;
    data.write_multiple_coils.val   = (unsigned char *)value;
    break;
  case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
    data.write_multiple_registers.start = address;
    data.write_multiple_registers.quant = size;
    data.write_multiple_registers.size  = 2 * size;
    data.write_multiple_registers.val   = (unsigned char *)value;
    break;
  case MODBUS_FC_MASK_WRITE_REGISTER:
    data.mask_write_register.address = address;
    data.mask_write_register.and     = 0; //and;
    data.mask_write_register.or      = 0; //or;
    break;
  case MODBUS_FC_RW_MULTIPLE_REGISTERS:
    data.rw_multiple_registers.start_read  = address;
    data.rw_multiple_registers.quant_read  = size;
    data.rw_multiple_registers.start_write = data.rw_multiple_registers.start_read;
    data.rw_multiple_registers.quant_write = data.rw_multiple_registers.quant_read;
    data.rw_multiple_registers.size  = size;
    data.rw_multiple_registers.val   = out;
    out[0] = 0xaa;
    out[1] = 0x55;
    break;
  case MODBUS_FC_READ_EXCEPTION_STATUS:
    break;
  case MODBUS_FC_READ_DEVICE_IDENTIFICATION:
    data.read_device_identification.id_code   = 0x01;
    data.read_device_identification.object_id = 0x00;
    break;
  default:
	return;
  }

  Modbus_RTU_Send(dev, 0, fc, &data);

}


/******************************************************************************
* Modbus
******************************************************************************/
unsigned int Modbus_Request(struct MODBUS_Device *dev, unsigned char * buffer, unsigned int sz)
{
  struct MODBUS_Reply r;

  //memset(serial_rx_buffer, 0, sizeof(serial_rx_buffer));
  //memset(serial_tx_buffer, 0, sizeof(serial_tx_buffer));

  if (MODBUS_MODE_IS_MASTER(dev->mode))
  {
    r = Modbus_RTU_ReceiveReply(dev, Modbus_RTU_Validate(buffer, sz, MODBUS_MODE_IS_TCP(dev->mode)));
    if (r.ExceptionCode == MODBUS_EXCEPTION_NONE)
    {
      if(MODBUS_MODE_IS_TCP(dev->mode)) {
    	  I_TcpReady = 1;
      } else {
    	  I_SerialReady = 1;
      }

      if (MODBUS_RETURN_VAL == NULL)
        return 0;

      switch (r.FunctionCode)
      {
      case MODBUS_FC_READ_COILS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.read_coils.data, r.reply.read_coils.size);
        break;

      case MODBUS_FC_READ_DISCRETE_INPUTS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.read_discrete_inputs.data, r.reply.read_discrete_inputs.size);
        break;

      case MODBUS_FC_READ_HOLDING_REGISTERS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.read_holding_registers.data, r.reply.read_holding_registers.size);
        break;

      case MODBUS_FC_READ_INPUT_REGISTERS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.read_input_registers.data, r.reply.read_input_registers.size);
        break;

      case MODBUS_FC_RW_MULTIPLE_REGISTERS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.rw_multiple_registers.data, r.reply.rw_multiple_registers.size);
        break;
      };
    }
  }
  else
  {
    if (Modbus_RTU_Receive(dev, Modbus_RTU_Validate(buffer, sz, MODBUS_MODE_IS_TCP(dev->mode))) == MODBUS_EXCEPTION_NONE) {
		if(MODBUS_MODE_IS_TCP(dev->mode)) {
		  I_TcpReady = 1;
		} else {
		  I_SerialReady = 1;
		}
    }
  }

  return 0; // Retorna o tamanho dos dados
}
