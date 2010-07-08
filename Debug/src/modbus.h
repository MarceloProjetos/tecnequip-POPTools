#ifndef MODBUS_H
#define MODBUS_H

#include <string.h> // Para memset

//#define MB_DEBUG

// Codigos de excecoes
#define MB_EXCEPTION_NONE                 0x00
#define MB_EXCEPTION_ILLEGAL_FUNCTION     0x01
#define MB_EXCEPTION_ILLEGAL_DATA_ADDRESS 0x02
#define MB_EXCEPTION_ILLEGAL_DATA_VALUE   0x03
#define MB_EXCEPTION_SLAVE_DEVICE_FAILURE 0x04

// Codigos de funcoes
#define MB_FC_READ_COILS                 0x0001
#define MB_FC_READ_DISCRETE_INPUTS       0x0002
#define MB_FC_READ_HOLDING_REGISTERS     0x0003
#define MB_FC_READ_INPUT_REGISTERS       0x0004
#define MB_FC_WRITE_SINGLE_COIL          0x0005
#define MB_FC_WRITE_SINGLE_REGISTER      0x0006
#define MB_FC_READ_EXCEPTION_STATUS      0x0007
#define MB_FC_WRITE_MULTIPLE_COILS       0x000F
#define MB_FC_WRITE_MULTIPLE_REGISTERS   0x0010
#define MB_FC_MASK_WRITE_REGISTER        0x0016
#define MB_FC_RW_MULTIPLE_REGISTERS      0x0017
#define MB_FC_READ_DEVICE_IDENTIFICATION 0x2B0E

// Modos de operacao do ModBus
#define MB_MODE_MASTER 0
#define MB_MODE_SLAVE  1

#define MB_BUFFER_SIZE 260

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

struct MB_PDU {
  unsigned char   Id;
  unsigned short int  FunctionCode;
  unsigned char  *Data;
  unsigned int  ds;
};

// Estruturas com os dados formatados para cada um dos Function Codes existentes
union MB_FCD_Data {
  struct {
    unsigned short int start;
    unsigned short int quant;
  } read_coils;

  struct {
    unsigned short int start;
    unsigned short int quant;
  } read_discrete_inputs;

  struct {
    unsigned short int start;
    unsigned short int quant;
  } read_input_registers;

  struct {
    unsigned short int start;
    unsigned short int quant;
  } read_holding_registers;

  struct MB_FCD_write_single_coil {
    unsigned short int output;
    unsigned char  val;
  } write_single_coil;

  struct MB_FCD_write_single_register {
    unsigned short int address;
    unsigned short int val;
  } write_single_register;

  struct MB_FCD_write_multiple_coils {
    unsigned short int start;
    unsigned short int quant;
    unsigned char  size;
    unsigned char *val;
  } write_multiple_coils;

  struct MB_FCD_write_multiple_registers {
    unsigned short int start;
    unsigned short int quant;
    unsigned char  size;
    unsigned char *val;
  } write_multiple_registers;

  struct MB_FCD_mask_write_register {
    unsigned short int address;
    unsigned short int and;
    unsigned short int or;
  } mask_write_register;

  struct {
    unsigned short int start_read;
    unsigned short int quant_read;
    unsigned short int start_write;
    unsigned short int quant_write;
    unsigned char  size;
    unsigned char *val;
  } rw_multiple_registers;

  struct {
    unsigned char id_code;
    unsigned char object_id;
  } read_device_identification;
};

struct MB_Reply {
  unsigned char  Id;
  unsigned short int FunctionCode; // Para retorno do status
  unsigned char  ExceptionCode; // Para retornar o erro
  union {
    struct {
      unsigned char size;
      unsigned char data[];
    } read_coils;

    struct {
      unsigned char size;
      unsigned char data[];
    } read_discrete_inputs;

    struct {
      unsigned char size;
      unsigned char data[];
    } read_holding_registers;

    struct {
      unsigned char size;
      unsigned char data[];
    } read_input_registers;

    struct MB_FCD_write_single_coil        write_single_coil;

    struct MB_FCD_write_single_register    write_single_register;

    struct MB_FCD_write_multiple_coils     write_multiple_coils;

    struct MB_FCD_write_multiple_registers write_multiple_registers;

    struct MB_FCD_mask_write_register      mask_write_register;

    struct {
      unsigned char size;
      unsigned char data[];
    } rw_multiple_registers;

    struct {
      unsigned char status;
    } read_exception_status;

    struct {
      unsigned char id_code;
      unsigned char object_id;
      char  data[];
    } read_device_identification;

    unsigned char reply_buffer[MB_BUFFER_SIZE];
  } reply;
};

struct MB_Device;

// Handler para funcoes que interpretam os Function Codes
#define MB_HANDLER_FC_PTR(fnc) unsigned int (*fnc)(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
#define MB_HANDLER_FC(fnc)     unsigned int   fnc (struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)

#define MB_HANDLER_TX_PTR(fnc) unsigned int (*fnc)(unsigned char *data, unsigned int size)
#define MB_HANDLER_TX(fnc)     unsigned int   fnc (unsigned char *data, unsigned int size)

struct MB_Handler {
  unsigned short int FunctionCode;
  MB_HANDLER_FC_PTR(FunctionHandler);
};

// Estrutura que representa o ModBus
struct MB_Device {
  struct {
    unsigned char  Id;
    char  *VendorName;
    char  *ProductCode;
    char  *MajorMinorRevision;
    char  *VendorURL;
    char  *ProductName;
    char  *ModelName;
    char  *UserApplicationName;
  } identification;
  struct MB_Handler *hl;
  unsigned int hl_size, mode;
  MB_HANDLER_TX_PTR(TX); // Ponteiro para funcao que faz a transmissao do pacote
};

void            MB_Init     (struct MB_Device *dev);
struct MB_PDU   MB_Validate (unsigned char *buf, unsigned int size);
struct MB_Reply MB_Send     (struct MB_Device *dev, unsigned short int FunctionCode, union MB_FCD_Data *data);
unsigned int    MB_SendReply(struct MB_Device *dev, struct MB_Reply *msg);
unsigned int    MB_Receive  (struct MB_Device *dev, struct MB_PDU    msg);

unsigned short int CRC16(unsigned char *puchMsg, unsigned short int usDataLen);

#endif
