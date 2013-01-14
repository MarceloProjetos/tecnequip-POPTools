/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/

#ifndef _LD_H_
#define _LD_H_

#include <string.h>

/******************************************************************************
* Encoder SSI
******************************************************************************/
#ifndef __SSI_H__
#define SSI_MODE_GRAY 0
#define SSI_MODE_BIN  1

void         SSI_Init       ();
unsigned int SSI_Read       (unsigned int size, unsigned int mode);
void         SSI_SetPosition(int offset);

#endif

/******************************************************************************
* Encoder Wrapper
******************************************************************************/
#ifndef __ENCODER_H__
int  ENC_Read (unsigned int device);
void ENC_Reset(unsigned int device, int offset);
#endif

/***************************************************************************/
/* EEPROM                                                                  */
/***************************************************************************/
#ifndef __EEPROM_H__
void         E2P_Init (void);
int          E2P_Busy (void);
unsigned int E2P_Read (void *data, unsigned char address, int size);
unsigned int E2P_Write(void *data, unsigned char address, int size);
#endif

/***************************************************************************/
/* RTC                                                                 */
/***************************************************************************/
#ifndef __RTC_H__

#include <time.h>

typedef struct {
    unsigned int Sec;     /* Second value - [0,59] */
    unsigned int Min;     /* Minute value - [0,59] */
    unsigned int Hour;    /* Hour value - [0,23] */
    unsigned int Mday;    /* Day of the month value - [1,31] */
    unsigned int Mon;     /* Month value - [1,12] */
    unsigned int Year;    /* Year value - [0,4095] */
    unsigned int Wday;    /* Day of week value - [0,6] */
    unsigned int Yday;    /* Day of year value - [1,365] */
} RTC_Time;
RTC_Time RTC_GetTime( void );

#define RTC_MODE_DATE_CONTINUOUS      0
#define RTC_MODE_DATE_INTERMITTENT    1
#define RTC_MODE_WEEKDAY_CONTINUOUS   2
#define RTC_MODE_WEEKDAY_INTERMITTENT 3

#define RTC_GETDATE_MODE_START 0
#define RTC_GETDATE_MODE_END   1

struct tm RTC_GetTM( void );
struct tm * AdjustDate(struct tm timeinfo, int mode);
int RTC_OutputState(struct tm start, struct tm end, struct tm now, int mode, int mask_wday);
#endif

/***************************************************************************/
/* LWIP 1.4.0                                                              */
/***************************************************************************/
#ifndef __LWIP_IP_ADDR_H__
struct ip_addr {
  unsigned int addr;
};

#define IP4_ADDR(ipaddr, a,b,c,d) \
        (ipaddr)->addr = ((unsigned int)((d) & 0xff) << 24) | \
                         ((unsigned int)((c) & 0xff) << 16) | \
                         ((unsigned int)((b) & 0xff) << 8)  | \
                          (unsigned int)((a) & 0xff)
#endif

/***************************************************************************/
/* GPIO                                                                    */
/***************************************************************************/
extern unsigned int GPIO_Output(void);
extern unsigned int GPIO_Input(void);

/***************************************************************************/
/* RS485                                                                   */
/***************************************************************************/
#ifndef __RS485_H__
extern void RS485_Config(int baudrate, int bits, int parity, int stopbit);
extern void RS485_Handler (void);
extern unsigned int RS485_Write(unsigned char * buffer, unsigned int size);
extern unsigned int RS485_Read(unsigned char * buffer, unsigned int size);
#endif

/***************************************************************************/
/* RS232                                                                   */
/***************************************************************************/
#ifndef __RS232_H__
extern unsigned int RS232_Write(unsigned char * buffer, unsigned int size);
extern unsigned int RS232_Read(unsigned char * buffer, unsigned int size);
extern void RS232_Console(void);
#endif

/***************************************************************************/
/* Modbus  RTU                                                             */
/***************************************************************************/
#ifndef __MODBUS_TCP_H__
void ModBUS_SetID(unsigned int id);

void Modbus_TCP_Send(unsigned char id,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value);
#endif

/***************************************************************************/
/* Modbus  RTU                                                             */
/***************************************************************************/
#ifndef __MODBUS_H__
// Codigos de funcoes
#define MODBUS_FC_READ_COILS                 0x0001
#define MODBUS_FC_READ_DISCRETE_INPUTS       0x0002
#define MODBUS_FC_READ_HOLDING_REGISTERS     0x0003
#define MODBUS_FC_READ_INPUT_REGISTERS       0x0004
#define MODBUS_FC_WRITE_SINGLE_COIL          0x0005
#define MODBUS_FC_WRITE_SINGLE_REGISTER      0x0006
#define MODBUS_FC_READ_EXCEPTION_STATUS      0x0007
#define MODBUS_FC_WRITE_MULTIPLE_COILS       0x000F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS   0x0010
#define MODBUS_FC_MASK_WRITE_REGISTER        0x0016
#define MODBUS_FC_RW_MULTIPLE_REGISTERS      0x0017
#define MODBUS_FC_READ_DEVICE_IDENTIFICATION 0x2B0E

#define MODBUS_BUFFER_SIZE 260

// Estruturas com os dados formatados para cada um dos Function Codes existentes
union MODBUS_FCD_Data {
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

  struct MODBUS_FCD_write_single_coil {
    unsigned short int output;
    unsigned char  val;
  } write_single_coil;

  struct MODBUS_FCD_write_single_register {
    unsigned short int address;
    unsigned short int val;
  } write_single_register;

  struct MODBUS_FCD_write_multiple_coils {
    unsigned short int start;
    unsigned short int quant;
    unsigned char  size;
    unsigned char *val;
  } write_multiple_coils;

  struct MODBUS_FCD_write_multiple_registers {
    unsigned short int start;
    unsigned short int quant;
    unsigned char  size;
    unsigned char *val;
  } write_multiple_registers;

  struct MODBUS_FCD_mask_write_register {
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

struct MODBUS_Reply {
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

    struct MODBUS_FCD_write_single_coil        write_single_coil;

    struct MODBUS_FCD_write_single_register    write_single_register;

    struct MODBUS_FCD_write_multiple_coils     write_multiple_coils;

    struct MODBUS_FCD_write_multiple_registers write_multiple_registers;

    struct MODBUS_FCD_mask_write_register      mask_write_register;

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

    unsigned char reply_buffer[MODBUS_BUFFER_SIZE];
  } reply;
};
#define MODBUS_HANDLER_TX_PTR(fnc) unsigned int (*fnc)(unsigned char *data, unsigned int size)
#define MODBUS_HANDLER_TX(fnc)     unsigned int   fnc (unsigned char *data, unsigned int size)
// Estrutura que representa o ModBus
struct MODBUS_Device {
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
  struct MODBUS_Handler *hl;
  unsigned int hl_size, mode;
  MODBUS_HANDLER_TX_PTR(TX); // Ponteiro para funcao que faz a transmissao do pacote
};

extern unsigned int Modbus_ReadCoils(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
extern unsigned int Modbus_ReadDiscreteInputs(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
extern unsigned int Modbus_ReadHoldingRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
extern unsigned int Modbus_ReadInputRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
extern unsigned int Modbus_WriteSingleCoil(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
extern unsigned int Modbus_WriteSingleRegister(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
extern unsigned int Modbus_WriteMultipleCoils(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
extern unsigned int Modbus_WriteMultipleRegisters(struct MODBUS_Device *dev, union MODBUS_FCD_Data *data, struct MODBUS_Reply *reply);
extern unsigned int Modbus_Tx(unsigned char *data, unsigned int size);
extern unsigned int Modbus_Request(unsigned char * buffer, unsigned int sz);
extern void Modbus_Send(unsigned char id,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value);
#endif

/***************************************************************************/
/* USS                                                                     */
/***************************************************************************/
#ifndef USS_H
unsigned char USS_Set_Param(unsigned char addr, unsigned short int param, unsigned char param_set, unsigned char array, volatile int * val);
unsigned char USS_Get_Param(unsigned char addr, unsigned short int param, unsigned char param_set, unsigned char array, volatile int * val);
#endif

/***************************************************************************/
/* YASKAWA                                                                 */
/***************************************************************************/
#ifndef __YASKAWA_H__
extern int Yaskawa_Read(char * id, char *format, volatile int *val);
extern int Yaskawa_Write(char * id, char *format, volatile int * val);
#endif

/***************************************************************************/
/* FORMAT STRING                                                           */
/***************************************************************************/
extern int Format_String_Write(char *format, volatile int * val);
extern int Format_String_Read(char *format, volatile int *val);

/***************************************************************************/
/* CoOS                                                                    */
/***************************************************************************/
#ifndef _COOCOX_H
extern unsigned char CoTickDelay(unsigned int ticks);
unsigned char  CoTimeDelay(unsigned char hour,unsigned char minute,unsigned char sec,unsigned short int millsec);
#define CoCreateTask(task,argv,prio,stk,stkSz)              \
            CreateTask(task,argv,(prio)|(((stkSz)<<8) &0x000FFF00 ),stk)
#define CoCreateTaskEx(task,argv,prio,stk,stkSz,timeSlice,isWaitting)  \
           CreateTask(task,argv,(prio)|(((stkSz)<<8)&0x000FFF00)|(((timeSlice)<<20)&0x7FF00000)|((isWaitting<<31)&0x80000000),stk)
#endif

/***************************************************************************/
/* DAC                                                                     */
/***************************************************************************/
#ifndef __DAC_H
unsigned int	DAC_Read (void);
void			DAC_Abort(int mode);
void			DAC_Write(unsigned int val);
unsigned int    DAC_Conv(int val, unsigned int mode);
void            DAC_Start(unsigned char up, unsigned char linear, unsigned char gaint, unsigned char gainr, int time, int value);
#endif

/***************************************************************************/
/* ADC                                                                     */
/***************************************************************************/
#ifndef __ADC_H__
extern unsigned int ADC_Read   (unsigned int a);
extern void         ADC_Update (void);
extern void         ADC_SetMask(unsigned int ad_mask);
#endif

/***************************************************************************/
/* ENC                                                                     */
/***************************************************************************/
#ifndef __QEI_H
/* QEI Capture Mode Option */
#define QEI_CAPMODE_2X			((unsigned int)(0))		/**< Capture mode: Only Phase-A edges are counted (2X) */
#define QEI_CAPMODE_4X			((unsigned int)(1))		/**< Capture mode: BOTH PhA and PhB edges are counted (4X)*/

typedef struct {
	unsigned int DirectionInvert	:1; 	/**< Direction invert option:
										- QEI_DIRINV_NONE: QEI Direction is normal
										- QEI_DIRINV_CMPL: QEI Direction is complemented
										*/
	unsigned int SignalMode			:1; 	/**< Signal mode Option:
										- QEI_SIGNALMODE_QUAD: Signal is in Quadrature phase mode
										- QEI_SIGNALMODE_CLKDIR: Signal is in Clock/Direction mode
										*/
	unsigned int CaptureMode		:1;		/**< Capture Mode Option:
										- QEI_CAPMODE_2X: Only Phase-A edges are counted (2X)
										- QEI_CAPMODE_4X: BOTH Phase-A and Phase-B edges are counted (4X)
										*/
	unsigned int InvertIndex		:1; 	/**< Invert Index Option:
										- QEI_INVINX_NONE: the sense of the index input is normal
										- QEI_INVINX_EN: inverts the sense of the index input
										*/
} QEI_CFG_Type;
extern QEI_CFG_Type QEIConfig;
#endif

/***************************************************************************/
/* MATH                                                                    */
/***************************************************************************/
int srint(int *sr);

/***************************************************************************/
/* PLC                                                                     */
/***************************************************************************/
void PLC_Run(void);
void PLC_Cycle(void *pdata);
void PLC_Init(void);

#endif
