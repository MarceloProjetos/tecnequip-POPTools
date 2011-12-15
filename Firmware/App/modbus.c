#include "modbus.h"

struct MODBUS_Device mbdev;

extern volatile unsigned int GPIO_OUTPUT;
extern volatile unsigned int GPIO_INPUT;

extern volatile unsigned int I_SerialReady;
extern volatile unsigned int rs485_timeout;
extern volatile unsigned int rs485_reset_timeout;

volatile unsigned char MODBUS_MASTER;  // 0 = Slave, 1 = Master

volatile int * MODBUS_RETURN_VAL = NULL;

struct MODBUS_Device modbus_rtu_master;
struct MODBUS_Device modbus_rtu_slave;
struct MODBUS_Device modbus_tcp_slave;

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
	Modbus_RTU_Init(&modbus_rtu_master);

	modbus_rtu_master.identification.Id                 = 0x01;
	modbus_rtu_master.identification.VendorName         = "Tecnequip";
	modbus_rtu_master.identification.ProductCode        = "POP7";
	modbus_rtu_master.identification.MajorMinorRevision = "V1 Rev2";

	modbus_rtu_master.hl      = ModbusHandlers;
	modbus_rtu_master.hl_size = ARRAY_SIZE(ModbusHandlers);
	modbus_rtu_master.mode    = MODBUS_MODE_MASTER;
	modbus_rtu_master.TX      = Modbus_RTU_Tx;

	Modbus_RTU_Init(&modbus_rtu_slave);

	modbus_rtu_slave.identification.Id                 = 0x01;
	modbus_rtu_slave.identification.VendorName         = "Tecnequip";
	modbus_rtu_slave.identification.ProductCode        = "POP7";
	modbus_rtu_slave.identification.MajorMinorRevision = "V1 Rev2";

	modbus_rtu_slave.hl      = ModbusHandlers;
	modbus_rtu_slave.hl_size = ARRAY_SIZE(ModbusHandlers);
	modbus_rtu_slave.mode    = MODBUS_MODE_SLAVE;
	modbus_rtu_slave.TX      = Modbus_RTU_Tx;

	Modbus_RTU_Init(&modbus_tcp_slave);

	modbus_tcp_slave.identification.Id                 = 0x01;
	modbus_tcp_slave.identification.VendorName         = "Tecnequip";
	modbus_tcp_slave.identification.ProductCode        = "POP7";
	modbus_tcp_slave.identification.MajorMinorRevision = "V1 Rev2";

	modbus_tcp_slave.hl      = ModbusHandlers;
	modbus_tcp_slave.hl_size = ARRAY_SIZE(ModbusHandlers);
	modbus_tcp_slave.mode    = MODBUS_MODE_SLAVE;
	modbus_tcp_slave.TX      = Modbus_TCP_Tx;

	Modbus_TCP_Init();

	memset((void*)MODBUS_REGISTER, 0, sizeof(MODBUS_REGISTER));

}

void Modbus_Send(unsigned char id,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value)
{
  uint8_t out[] = { 0, 0 };

  struct MODBUS_Device * mbdev;

  if (MODBUS_MASTER)
    mbdev = &modbus_rtu_master;
  else
    mbdev = &modbus_rtu_slave;

  MODBUS_RETURN_VAL = value;

  union MODBUS_FCD_Data data;
  struct MODBUS_Reply rp;

  mbdev->identification.Id = id;

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
    rp.FunctionCode = fc;
    rp.ExceptionCode = MODBUS_EXCEPTION_ILLEGAL_FUNCTION;
	return;
  }

  rp = Modbus_RTU_Send(mbdev, fc, &data);

}


/******************************************************************************
* Modbus
******************************************************************************/
unsigned int Modbus_Request(unsigned char * buffer, unsigned int sz)
{
  struct MODBUS_Reply r;

  //memset(serial_rx_buffer, 0, sizeof(serial_rx_buffer));
  //memset(serial_tx_buffer, 0, sizeof(serial_tx_buffer));

  if (MODBUS_MASTER)
  {
    r = Modbus_RTU_ReceiveReply(&modbus_rtu_master, Modbus_RTU_Validate(buffer, sz));
    if (r.ExceptionCode == MODBUS_EXCEPTION_NONE)
    {
      I_SerialReady = 1;
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
    if (Modbus_RTU_Receive(&modbus_rtu_slave, Modbus_RTU_Validate(buffer, sz)) == MODBUS_EXCEPTION_NONE)
      I_SerialReady = 1;
  }

  return 0; // Retorna o tamanho dos dados
}

unsigned int Modbus_ReadCoils(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  uint32_t i;
  uint32_t temp = ((GPIO_OUTPUT >> 8) & 0xFF) | ((GPIO_OUTPUT << 8) & 0xFF00);
  uint8_t *buf = reply->reply.read_coils.data; // Retorna no maximo 8 bytes ou 256 bits (saidas).

  reply->reply.read_coils.size = data->read_coils.quant / 8 + (data->read_coils.quant % 8 != 0);

  memset(buf, 0, reply->reply.read_coils.size);

  for(i = 0; i < data->read_coils.quant; i++)
    buf[i / 8] |= ((temp >> (data->read_coils.start + i)) & 1) << (i % 8);

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
  /*uint32_t sz = 0;

  uint32_t i = 0;
  uint8_t *buf = reply->reply.read_holding_registers.data;

  for(i = 0; i < data->read_holding_registers.quant; i++)
  {
    if (data->read_holding_registers.start + i < 32)
    {
      buf[(2 * i)] = (uint8_t)(M[data->read_holding_registers.start + i] >> 8);
      buf[(2 * i) + 1] = (uint8_t)(M[data->read_holding_registers.start + i]);
      sz += 2;
    }
    else
      return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  }*/

  int i, x;

  if (data->read_holding_registers.start + data->read_holding_registers.quant >= 32)
	  return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  else
  {
	  //memcpy(reply->reply.read_holding_registers.data, (void *)&M[data->read_holding_registers.start], data->read_holding_registers.quant * 2);
	  x = data->read_holding_registers.start;
	  for(i = data->read_holding_registers.start;
		  i <= data->read_holding_registers.start + (data->read_holding_registers.quant * 2); i += 2) {
		reply->reply.read_holding_registers.data[i - data->read_holding_registers.start] = MODBUS_REGISTER[x];
		reply->reply.read_holding_registers.data[i - data->read_holding_registers.start + 1] = MODBUS_REGISTER[x] >> 8;
		x++;
	  }
  }

  reply->reply.read_holding_registers.size = data->read_holding_registers.quant * 2;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_ReadInputRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_WriteSingleCoil(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  if(data->write_single_coil.val)
    GPIO_OUTPUT |= 1 << data->write_single_coil.output;
  else
	  GPIO_OUTPUT &= ~(1 << data->write_single_coil.output);

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
    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  }*/

  reply->reply.write_single_coil = data->write_single_coil;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_WriteSingleRegister(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  if (data->write_single_register.address < 32)
    MODBUS_REGISTER[data->write_single_register.address] = data->write_single_register.val;
  else
    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.write_single_register = data->write_single_register;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_WriteMultipleCoils(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
  reply->reply.write_multiple_coils  = data->write_multiple_coils;

  return MODBUS_EXCEPTION_NONE;
}

unsigned int Modbus_WriteMultipleRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply)
{
	int i, pos;

  /*if (data->write_multiple_registers.start + data->write_multiple_registers.quant >= 32)
	  return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  else
	  memcpy((void*)&M[data->write_multiple_registers.start], data->write_multiple_registers.val, data->write_multiple_registers.size);*/

  if (data->write_multiple_registers.start + data->write_multiple_registers.quant < 32)
  {
    for (i = 0; i < data->write_multiple_registers.size; i += 2)
    {
      pos = data->write_multiple_registers.start + (i / 2);
      MODBUS_REGISTER[pos] = *(unsigned short *)(&data->write_multiple_registers.val[i]);
    }
//      data->write_single_register.val[i] = M[data->write_multiple_registers.address + (i] ;
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


