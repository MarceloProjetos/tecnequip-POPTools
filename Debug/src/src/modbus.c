#include "modbus.h"

/* Table of CRC values for high-order byte */
static unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

/* Table of CRC values for low-order byte */
static unsigned char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

struct MB_Device mbdev;

void MB_Init(struct MB_Device *dev)
{
  memset(dev, 0, sizeof(struct MB_Device));

  // Inicializa todas as strings para vazio.
  dev->identification.VendorName          = \
  dev->identification.ProductCode         = \
  dev->identification.MajorMinorRevision  = \
  dev->identification.VendorURL           = \
  dev->identification.ProductName         = \
  dev->identification.ModelName           = \
  dev->identification.UserApplicationName = "";
}

struct MB_PDU MB_Validate(unsigned char *buf, unsigned int size)
{
  unsigned int offset = 0;
  unsigned short int crc16_calc = CRC16(buf, size-2), crc16_real;
  struct MB_PDU msg;

  crc16_real = (unsigned short int)(buf[size-2]) | (((unsigned short int)(buf[size-1]))<<8);

  if(crc16_calc == crc16_real) {
    msg.Id           = buf[offset++];
    msg.FunctionCode = buf[offset++];
    if(msg.FunctionCode == 0x08 || msg.FunctionCode == 0x2B)
      msg.FunctionCode = (msg.FunctionCode<<8) | buf[offset++];
    msg.Data         = buf  + offset;
    msg.ds           = size - offset - 2; // total - tamanho utilizado - CRC16
  } else {
#ifdef MB_DEBUG
    printf("MODBUS: Erro de CRC. Recebido 0x%x, Calculado: 0x%x\n", crc16_real, crc16_calc);
#endif
    memset(&msg, 0, sizeof(msg));
  }

  return msg;
}

struct MB_Reply MB_Send(struct MB_Device *dev, unsigned short int FunctionCode, union MB_FCD_Data *data)
{
  unsigned short int crc16;
  unsigned int size=0, i;
  struct MB_Reply r;
  struct MB_PDU msg;
  unsigned char buf[MB_BUFFER_SIZE];

  memset(r.reply.reply_buffer, 0, ARRAY_SIZE(r.reply.reply_buffer));
  r.ExceptionCode = MB_EXCEPTION_ILLEGAL_FUNCTION; // Invalid Function Code

  buf[size++] = dev->identification.Id; // Id
  if(FunctionCode & 0xFF00) // Possui SubFunctionCode
    buf[size++] = (FunctionCode>>8) & 0xFF;
  buf[size++] = FunctionCode & 0xFF;

  switch(FunctionCode) {
    case MB_FC_READ_COILS:
      buf[size++] = (data->read_coils.start>>8)&0xFF;
      buf[size++] = (data->read_coils.start   )&0xFF;
      buf[size++] = (data->read_coils.quant>>8)&0xFF;
      buf[size++] = (data->read_coils.quant   )&0xFF;
      break;

    case MB_FC_READ_DISCRETE_INPUTS:
      buf[size++] = (data->read_discrete_inputs.start>>8)&0xFF;
      buf[size++] = (data->read_discrete_inputs.start   )&0xFF;
      buf[size++] = (data->read_discrete_inputs.quant>>8)&0xFF;
      buf[size++] = (data->read_discrete_inputs.quant   )&0xFF;
      break;

    case MB_FC_READ_HOLDING_REGISTERS:
      buf[size++] = (data->read_holding_registers.start>>8)&0xFF;
      buf[size++] = (data->read_holding_registers.start   )&0xFF;
      buf[size++] = (data->read_holding_registers.quant>>8)&0xFF;
      buf[size++] = (data->read_holding_registers.quant   )&0xFF;
      break;

    case MB_FC_READ_INPUT_REGISTERS:
      buf[size++] = (data->read_input_registers.start>>8)&0xFF;
      buf[size++] = (data->read_input_registers.start   )&0xFF;
      buf[size++] = (data->read_input_registers.quant>>8)&0xFF;
      buf[size++] = (data->read_input_registers.quant   )&0xFF;
      break;

    case MB_FC_WRITE_SINGLE_COIL:
      buf[size++] = (data->write_single_coil.output>>8)&0xFF;
      buf[size++] = (data->write_single_coil.output   )&0xFF;
      buf[size++] =  data->write_single_coil.val ? 0xFF : 0x00;
      buf[size++] = 0x00;
      break;

    case MB_FC_WRITE_SINGLE_REGISTER:
      buf[size++] = (data->write_single_register.address >> 8) & 0xFF;
      buf[size++] = (data->write_single_register.address     ) & 0xFF;
      buf[size++] = (data->write_single_register.val     >> 8) & 0xFF;
      buf[size++] = (data->write_single_register.val         ) & 0xFF;
      break;

    case MB_FC_WRITE_MULTIPLE_COILS:
      buf[size++]  = (data->write_multiple_coils.start>>8)&0xFF;
      buf[size++]  = (data->write_multiple_coils.start   )&0xFF;
      buf[size++]  = (data->write_multiple_coils.quant>>8)&0xFF;
      buf[size++]  = (data->write_multiple_coils.quant   )&0xFF;
      buf[size++]  =  data->write_multiple_coils.size;
      for(i=0; i<data->write_multiple_coils.size; i++)
        buf[size++] = data->write_multiple_coils.val[i];
      break;

    case MB_FC_WRITE_MULTIPLE_REGISTERS:
      buf[size++]  = (data->write_multiple_registers.start>>8)&0xFF;
      buf[size++]  = (data->write_multiple_registers.start   )&0xFF;
      buf[size++]  = (data->write_multiple_registers.quant>>8)&0xFF;
      buf[size++]  = (data->write_multiple_registers.quant   )&0xFF;
      buf[size++]  =  data->write_multiple_registers.size;
      for(i=0; i<data->write_multiple_registers.size; i++)
        buf[size++] = data->write_multiple_registers.val[i];
      break;

    case MB_FC_MASK_WRITE_REGISTER:
      buf[size++] = (data->mask_write_register.address >> 8) & 0xFF;
      buf[size++] = (data->mask_write_register.address     ) & 0xFF;
      buf[size++] = (data->mask_write_register.and     >> 8) & 0xFF;
      buf[size++] = (data->mask_write_register.and         ) & 0xFF;
      buf[size++] = (data->mask_write_register.or      >> 8) & 0xFF;
      buf[size++] = (data->mask_write_register.or          ) & 0xFF;
      break;

    case MB_FC_RW_MULTIPLE_REGISTERS:
      buf[size++]  = (data->rw_multiple_registers.start_read  >> 8) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.start_read      ) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.quant_read  >> 8) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.quant_read      ) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.start_write >> 8) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.start_write     ) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.quant_write >> 8) & 0xFF;
      buf[size++]  = (data->rw_multiple_registers.quant_write     ) & 0xFF;
      buf[size++]  =  data->rw_multiple_registers.size;
      for(i=0; i<data->rw_multiple_registers.size; i++)
        buf[size++] = data->rw_multiple_registers.val[i];
      break;

    case MB_FC_READ_EXCEPTION_STATUS: // Nao existem dados para este Function Code
      break;

    case MB_FC_READ_DEVICE_IDENTIFICATION:
      buf[size++] = data->read_device_identification.id_code;
      buf[size++] = data->read_device_identification.object_id;
      break;

    default:
      r.FunctionCode = FunctionCode; // Invalid Function Code
      return r;
    }

  crc16 = CRC16(buf, size);

  buf[size++] = (crc16   ) & 0xFF;
  buf[size++] = (crc16>>8) & 0xFF;

  size = (*dev->TX)(buf, size);

  if(size) {
    msg = MB_Validate(buf, size);
    r.FunctionCode = msg.FunctionCode & 0xFF ? msg.FunctionCode & 0x7FFF : msg.FunctionCode & 0x7F;
    r.ExceptionCode = MB_EXCEPTION_NONE;
    switch(msg.FunctionCode) {
    case MB_FC_READ_COILS:
      r.reply.read_coils.size =  msg.Data[0];
      memcpy(r.reply.read_coils.data, &msg.Data[1], msg.Data[0]);
      break;

    case MB_FC_READ_DISCRETE_INPUTS:
      r.reply.read_discrete_inputs.size =  msg.Data[0];
      memcpy(r.reply.read_discrete_inputs.data, &msg.Data[1], msg.Data[0]);
      break;

    case MB_FC_READ_HOLDING_REGISTERS:
      r.reply.read_holding_registers.size =  msg.Data[0];
      memcpy(r.reply.read_holding_registers.data, &msg.Data[1], msg.Data[0]);
      break;

    case MB_FC_READ_INPUT_REGISTERS:
      r.reply.read_input_registers.size =  msg.Data[0];
      memcpy(r.reply.read_input_registers.data, &msg.Data[1], msg.Data[0]);
      break;

    case MB_FC_WRITE_SINGLE_COIL:
      r.reply.write_single_coil = data->write_single_coil;
      break;

    case MB_FC_WRITE_SINGLE_REGISTER:
      r.reply.write_single_register = data->write_single_register;
      break;

    case MB_FC_WRITE_MULTIPLE_COILS:
      r.reply.write_multiple_coils = data->write_multiple_coils;
      break;

    case MB_FC_WRITE_MULTIPLE_REGISTERS:
      r.reply.write_multiple_registers = data->write_multiple_registers;
      break;

    case MB_FC_MASK_WRITE_REGISTER:
      r.reply.mask_write_register = data->mask_write_register;
      break;

    case MB_FC_RW_MULTIPLE_REGISTERS:
      r.reply.rw_multiple_registers.size =  msg.Data[0];
      memcpy(r.reply.rw_multiple_registers.data, &msg.Data[1], msg.Data[0]);
      break;

    case MB_FC_READ_EXCEPTION_STATUS:
      r.reply.read_exception_status.status = msg.Data[0];
      break;

    case MB_FC_READ_DEVICE_IDENTIFICATION:
      //r.reply.read_device_identification.id_code   = msg.Data[0];
      //r.reply.read_device_identification.object_id = msg.Data[1];
      //strncpy(r.reply.read_device_identification.data, (int8_t *)&msg.Data[7], msg.Data[6]);
      //r.reply.read_device_identification.data[msg.Data[6]] = 0;
      memcpy(r.reply.reply_buffer, &msg.Data[4], msg.ds - 4);
      break;

    default:
      if(!msg.Data) // Erro de CRC
        r.ExceptionCode = MB_EXCEPTION_ILLEGAL_DATA_VALUE;
      else
        r.ExceptionCode = msg.Data[0];
    }
  } else {
    r.FunctionCode  = FunctionCode;
    r.ExceptionCode = MB_EXCEPTION_SLAVE_DEVICE_FAILURE;
  }

  return r;
}

unsigned int MB_SendReply(struct MB_Device *dev, struct MB_Reply *msg)
{
  unsigned short int crc16;
  unsigned int i, size = 0;
  unsigned char buf[MB_BUFFER_SIZE];

  buf[size++] = msg->Id;

  if(msg->ExceptionCode == MB_EXCEPTION_NONE) {
    if(msg->FunctionCode & 0xFF00) // Possui SubFunctionCode
      buf[size++] = (msg->FunctionCode>>8) & 0xFF;
    buf[size++] = msg->FunctionCode & 0xFF;

    switch(msg->FunctionCode) {
    case MB_FC_READ_COILS:
      buf[size++] = msg->reply.read_coils.size;
      for(i=0; i<msg->reply.read_coils.size; i++)
        buf[size++] = msg->reply.read_coils.data[i];
      break;

    case MB_FC_READ_DISCRETE_INPUTS:
      buf[size++] = msg->reply.read_discrete_inputs.size;
      for(i=0; i<msg->reply.read_discrete_inputs.size; i++)
        buf[size++] = msg->reply.read_discrete_inputs.data[i];
      break;

    case MB_FC_READ_HOLDING_REGISTERS:
      buf[size++] = msg->reply.read_holding_registers.size;
      for(i=0; i<msg->reply.read_holding_registers.size; i++) {
        buf[size++] = msg->reply.read_holding_registers.data[i];
      }
      break;

    case MB_FC_READ_INPUT_REGISTERS:
      buf[size++] = msg->reply.read_input_registers.size;
      for(i=0; i<msg->reply.read_input_registers.size; i++) {
        buf[size++] = msg->reply.read_input_registers.data[i];
      }
      break;

    case MB_FC_WRITE_SINGLE_COIL:
      buf[size++] = (msg->reply.write_single_coil.output>>8) & 0xFF;
      buf[size++] = (msg->reply.write_single_coil.output   ) & 0xFF;
      buf[size++] = msg->reply.write_single_coil.val ? 0xFF : 0x00;
      buf[size++] = 0x00;
      break;

    case MB_FC_WRITE_SINGLE_REGISTER:
      buf[size++] = (msg->reply.write_single_register.address >> 8) & 0xFF;
      buf[size++] = (msg->reply.write_single_register.address     ) & 0xFF;
      buf[size++] = (msg->reply.write_single_register.val     >> 8) & 0xFF;
      buf[size++] = (msg->reply.write_single_register.val         ) & 0xFF;
      break;

    case MB_FC_WRITE_MULTIPLE_COILS:
      buf[size++] = (msg->reply.write_multiple_coils.start>>8) & 0xFF;
      buf[size++] = (msg->reply.write_multiple_coils.start   ) & 0xFF;
      buf[size++] = (msg->reply.write_multiple_coils.quant>>8) & 0xFF;
      buf[size++] = (msg->reply.write_multiple_coils.quant   ) & 0xFF;
      buf[size++] =  msg->reply.write_multiple_coils.size;
      for(i=0; i<msg->reply.write_multiple_coils.size; i++)
        buf[size++] = msg->reply.write_multiple_coils.val[i];
      break;

    case MB_FC_WRITE_MULTIPLE_REGISTERS:
      buf[size++]  = (msg->reply.write_multiple_registers.start>>8)&0xFF;
      buf[size++]  = (msg->reply.write_multiple_registers.start   )&0xFF;
      buf[size++]  = (msg->reply.write_multiple_registers.quant>>8)&0xFF;
      buf[size++]  = (msg->reply.write_multiple_registers.quant   )&0xFF;
      buf[size++]  =  msg->reply.write_multiple_registers.size;
      for(i=0; i<msg->reply.write_multiple_registers.size; i++)
        buf[size++] =   msg->reply.write_multiple_registers.val[i];
      break;

    case MB_FC_MASK_WRITE_REGISTER:
      buf[size++] = (msg->reply.mask_write_register.address >> 8) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.address     ) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.and     >> 8) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.and         ) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.or      >> 8) & 0xFF;
      buf[size++] = (msg->reply.mask_write_register.or          ) & 0xFF;
      break;

    case MB_FC_RW_MULTIPLE_REGISTERS:
      buf[size++] = msg->reply.rw_multiple_registers.size;
      for(i=0; i<msg->reply.rw_multiple_registers.size; i++) {
        buf[size++] = msg->reply.rw_multiple_registers.data[i];
      }
      break;

    case MB_FC_READ_EXCEPTION_STATUS:
      buf[size++] = msg->reply.read_exception_status.status;
      break;

    case MB_FC_READ_DEVICE_IDENTIFICATION:
      buf[size++] = msg->reply.read_device_identification.id_code;
      buf[size++] = 0x82; // ??? - Passando como stream e individual a nivel regular
      buf[size++] = 0x00;
      buf[size++] = 0x00;
      buf[size++] = 1;
      buf[size++] = msg->reply.read_device_identification.object_id;
      buf[size++] = strlen(msg->reply.read_device_identification.data);
      strcpy((char *)(buf+size), (char *)msg->reply.read_device_identification.data);
      size += strlen(msg->reply.read_device_identification.data);
      break;
    }
  } else {
    buf[size++] = ((msg->FunctionCode & 0xFF00) ? msg->FunctionCode>>8 : msg->FunctionCode) | 0x80;
    buf[size++] = msg->ExceptionCode;
  }

  crc16 = CRC16(buf, size);

  buf[size++] = (crc16   ) & 0xFF;
  buf[size++] = (crc16>>8) & 0xFF;

  (*dev->TX)(buf, size);

  return 0;
}

unsigned int MB_Receive(struct MB_Device *dev, struct MB_PDU msg)
{
  unsigned int i;
  struct MB_Reply reply;
  union MB_FCD_Data data;

  // Erro de CRC. Nao envia resposta pois pode ter sido, inclusive, ruido na linha.
  if(msg.Data == NULL)
    return MB_EXCEPTION_ILLEGAL_DATA_VALUE;

  reply.Id            = msg.Id;
  reply.FunctionCode  = msg.FunctionCode;
  reply.ExceptionCode = MB_EXCEPTION_ILLEGAL_FUNCTION;

  for(i=0; i<dev->hl_size; i++) {
    if(msg.FunctionCode == dev->hl[i].FunctionCode) {
      reply.ExceptionCode = MB_EXCEPTION_NONE;
      switch(msg.FunctionCode) {
      case MB_FC_READ_COILS:
        data.read_coils.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.read_coils.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        if(!data.read_discrete_inputs.quant)
          reply.ExceptionCode = MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        break;

      case MB_FC_READ_DISCRETE_INPUTS:
        data.read_discrete_inputs.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.read_discrete_inputs.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        if(!data.read_discrete_inputs.quant)
          reply.ExceptionCode = MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        break;

      case MB_FC_READ_HOLDING_REGISTERS:
        data.read_holding_registers.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.read_holding_registers.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        if(!data.read_holding_registers.quant || data.read_holding_registers.quant > 125)
          reply.ExceptionCode = MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        break;

      case MB_FC_READ_INPUT_REGISTERS:
        data.read_input_registers.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.read_input_registers.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        if(!data.read_input_registers.quant || data.read_input_registers.quant > 125)
          reply.ExceptionCode = MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        break;

      case MB_FC_WRITE_SINGLE_COIL:
        data.write_single_coil.output = (msg.Data[0]<<8) | (msg.Data[1]);
        data.write_single_coil.val    = msg.Data[2] ? 1 : 0;
        if((msg.Data[2] != 0xFF && msg.Data[2] != 0x00) || msg.Data[3] != 0x00)
          reply.ExceptionCode = MB_EXCEPTION_ILLEGAL_DATA_VALUE;
        break;

      case MB_FC_WRITE_SINGLE_REGISTER:
        data.write_single_register.address = (msg.Data[0]<<8) | (msg.Data[1]);
        data.write_single_register.val     = (msg.Data[2]<<8) | (msg.Data[3]);
        break;

      case MB_FC_WRITE_MULTIPLE_COILS:
        data.write_multiple_coils.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.write_multiple_coils.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        data.write_multiple_coils.size  =  msg.Data[4];
        data.write_multiple_coils.val   =  msg.Data+5;
        break;

      case MB_FC_WRITE_MULTIPLE_REGISTERS:
        data.write_multiple_registers.start = (msg.Data[0]<<8) | (msg.Data[1]);
        data.write_multiple_registers.quant = (msg.Data[2]<<8) | (msg.Data[3]);
        data.write_multiple_registers.size  =  msg.Data[4];
        data.write_multiple_registers.val   =  msg.Data+5;
        break;

      case MB_FC_MASK_WRITE_REGISTER:
        data.mask_write_register.address = (msg.Data[0]<<8) | (msg.Data[1]);
        data.mask_write_register.and     = (msg.Data[2]<<8) | (msg.Data[3]);
        data.mask_write_register.or      = (msg.Data[4]<<8) | (msg.Data[5]);
        data.mask_write_register.or &= ~data.mask_write_register.and;
        break;

      case MB_FC_RW_MULTIPLE_REGISTERS:
        data.rw_multiple_registers.start_read  = (msg.Data[0]<<8) | (msg.Data[1]);
        data.rw_multiple_registers.quant_read  = (msg.Data[2]<<8) | (msg.Data[3]);
        data.rw_multiple_registers.start_write = (msg.Data[4]<<8) | (msg.Data[5]);
        data.rw_multiple_registers.quant_write = (msg.Data[6]<<8) | (msg.Data[7]);
        data.rw_multiple_registers.size        =  msg.Data[8];
        data.rw_multiple_registers.val         =  msg.Data+9;
        break;

      case MB_FC_READ_EXCEPTION_STATUS: // Nao existem dados para este Function Code
        break;

      default:
        reply.ExceptionCode = MB_EXCEPTION_ILLEGAL_FUNCTION;
      }

      if(reply.ExceptionCode == MB_EXCEPTION_NONE) {
        reply.ExceptionCode = (*dev->hl[i].FunctionHandler)(dev, &data, &reply);
        reply.FunctionCode = msg.FunctionCode; // Atualiza Function Code com codigo enviado.
        reply.Id = msg.Id; // Atualiza Id com o valor recebido do handler.
      }

      break; // Encontrou handler, para a busca.
    }
  }

  // Funcao nao suportada pelos handlers, verifica se manipulamos diretamente.
  if(reply.ExceptionCode == MB_EXCEPTION_ILLEGAL_FUNCTION) {
    switch(msg.FunctionCode) {
    case MB_FC_READ_DEVICE_IDENTIFICATION:
      reply.ExceptionCode = MB_EXCEPTION_NONE;
      reply.reply.read_device_identification.id_code   = msg.Data[0];
      reply.reply.read_device_identification.object_id = msg.Data[1];

      switch(reply.reply.read_device_identification.object_id) {
      case 0x00:
        strcpy(reply.reply.read_device_identification.data, dev->identification.VendorName);
        break;

      case 0x01:
        strcpy(reply.reply.read_device_identification.data, dev->identification.ProductCode);
        break;

      case 0x02:
        strcpy(reply.reply.read_device_identification.data, dev->identification.MajorMinorRevision);
        break;

      case 0x03:
        strcpy(reply.reply.read_device_identification.data, dev->identification.VendorURL);
        break;

      case 0x04:
        strcpy(reply.reply.read_device_identification.data, dev->identification.ProductName);
        break;

      case 0x05:
        strcpy(reply.reply.read_device_identification.data, dev->identification.ModelName);
        break;

      case 0x06:
        strcpy(reply.reply.read_device_identification.data, dev->identification.UserApplicationName);
        break;
      }
      break;
    }
  }

  MB_SendReply(dev, &reply);

  return reply.ExceptionCode; // Invalid Function Code
}

unsigned short int CRC16(unsigned char *puchMsg, unsigned short int usDataLen)
{
  unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
  unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
  unsigned int uIndex ; /* will index into CRC lookup table */

  while (usDataLen--) /* pass through message buffer */
    {
    uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
    uchCRCLo = auchCRCLo[uIndex] ;
    }

  //return (uchCRCHi << 8 | uchCRCLo) ;
  return (uchCRCLo << 8 | uchCRCHi) ;
}
