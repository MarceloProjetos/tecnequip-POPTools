/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/

#ifndef _LD_H_
#define _LD_H_

#include <string.h>
#include <stdlib.h>

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
int  ENC_Read  (unsigned int device);
void ENC_Reset (unsigned int device, int offset);
void ENC_Config(unsigned int device, int mode, int perimeter, float factor, int ppr);
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
extern unsigned int GPIO_Output(unsigned int OUTPUT);
extern unsigned int GPIO_Input(void);

/***************************************************************************/
/* RS485                                                                   */
/***************************************************************************/
#ifndef __RS485_H__
extern void RS485_Init();
extern void RS485_Config(int baudrate, int bits, int parity, int stopbit);
extern void RS485_Handler (void);
extern unsigned int RS485_Write(unsigned char * buffer, unsigned int size);
extern unsigned int RS485_Read(unsigned char * buffer, unsigned int size);
extern unsigned int RS485_ReadChar(unsigned char * buffer);
#endif

/***************************************************************************/
/* CAN                                                                     */
/***************************************************************************/
#ifndef __CAN_H__
#define     __I     volatile
#define     __O     volatile
#define     __IO    volatile
#define CAN_MSG_MAX_DATA_LEN       (8)

typedef struct							/*!< CAN Transmit structure                  */
{
	__IO uint32_t TFI;					/*!< CAN Transmit Frame Information register*/
	__IO uint32_t TID;					/*!< CAN Transfer Identifier register*/
	__IO uint32_t TD[2];				/*!<CAN Transmit Data register*/
} LPC_CAN_TX_T;

typedef struct				/*!< CAN Receive Frame structure                  */
{
	__IO uint32_t RFS;		/*!< Characteristic of the received frame. It includes the following characteristics:
							   CAN_RFS_BP: indicate that the current message is received in Bypass mode.
							 *							CAN_RFS_RTR: indicate the value of Remote Transmission Request bit in the current message.
							 *							CAN_RFS_FF: indicate that the identifier in the current message is 11-bit or 29-bit identifier.
							   Use CAN_RFS_ID_INDEX(RFS value) to get the ID Index of the matched entry in the Lookup Table RAM.
							   Use CAN_RFS_DLC(RFS value) to get the Data Length Code field of the current received message.
							 */
	__IO uint32_t RID;		/*!<Identifier in the received message. Use RFS field to determine if it is 11-bit or 29-bit identifier.*/
	__IO uint32_t RD[2];	/*!< Data bytes of the received message. Use DLC value in RFS fied to determine the number of data bytes.*/
} IP_CAN_001_RX_T;

typedef struct							/*!< CANn structure               */
{
	__IO uint32_t MOD;					/*!< CAN Mode Register */
	__O  uint32_t CMR;					/*!< CAN Command Register */
	__IO uint32_t GSR;					/*!< CAN Global Status Register */
	__I  uint32_t ICR;					/*!< CAN Interrupt and Capture Register */
	__IO uint32_t IER;					/*!< CAN Interrupt Enable Register*/
	__IO uint32_t BTR;					/*!< CAN Bus Timing Register*/
	__IO uint32_t EWL;					/*!< CAN Error Warning Limit Register*/
	__I  uint32_t SR;					/*!< CAN Status Register*/
	__IO IP_CAN_001_RX_T RX;			/*!< CAN Receive Registers*/
	__IO LPC_CAN_TX_T TX[3];		/*!< CAN Transmit Registers*/
} LPC_CAN_T;


typedef struct						/*!< Message structure */
{
	uint32_t ID;					/*!< Message Identifier. If 30th-bit is set, this is 29-bit ID, othewise 11-bit ID */
	uint32_t Type;					/*!< Message Type. which can include: - CAN_REMOTE_MSG type*/
	uint32_t DLC;					/*!< Message Data Length: 0~8 */
	uint8_t  Data[CAN_MSG_MAX_DATA_LEN];/*!< Message Data */
} CAN_MSG_T;

extern LPC_CAN_T LPC_CAN;
extern CAN_MSG_T SendMsgBuf;
extern CAN_MSG_T RcvMsgBuf;

extern void CAN_Init();
extern void CAN_Config(int baudrate);
extern void CAN_Handler (void);
extern unsigned int CAN_Write(unsigned char id, unsigned short int length, volatile int * value);
extern unsigned int CAN_Read(unsigned char id, unsigned short int length, volatile int * value);
extern unsigned int CAN_ReadChar(unsigned char * buffer);
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
void         ModBUS_SetID(unsigned int id);
void         ModBUS_SetAppName(char *appname);

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
                  unsigned long ip,
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
/* SNTP                                                                    */
/***************************************************************************/
#ifndef __SNTP_H__
extern void SNTP_Init(void);
#endif

/***************************************************************************/
/* QEI                                                                     */
/***************************************************************************/
#ifndef __QEI_H__
extern void QEI_Init(void);
#endif

/***************************************************************************/
/* ADC                                                                     */
/***************************************************************************/
#ifndef __ADC_H__
extern void ADC_Init(void);
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
void			DAC_Init (void);
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
/* PWM                                                                     */
/***************************************************************************/
#ifndef __PWM_H__

void PWM_Set(int duty_cycle, int frequency);
void PWM_Init(void);

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

/***************************************************************************/
/* Expansion Port                                                          */
/***************************************************************************/
#ifndef _XP_H__

enum eBoardType {
	eBoardType_None,
	eBoardType_Input,
	eBoardType_Output,
	eBoardType_AD,
	eBoardType_DA
};

struct strExpansionBoard {
	enum eBoardType type;
	unsigned char   address;
	union {
		struct {
			unsigned int bit0  : 1;
			unsigned int bit1  : 1;
			unsigned int bit2  : 1;
			unsigned int bit3  : 1;
			unsigned int bit4  : 1;
			unsigned int bit5  : 1;
			unsigned int bit6  : 1;
			unsigned int bit7  : 1;
			unsigned int bit8  : 1;
			unsigned int bit9  : 1;
			unsigned int bit10 : 1;
			unsigned int bit11 : 1;
			unsigned int bit12 : 1;
			unsigned int bit13 : 1;
			unsigned int bit14 : 1;
			unsigned int bit15 : 1;
			unsigned int bit16 : 1;
			unsigned int bit17 : 1;
			unsigned int bit18 : 1;
			unsigned int bit19 : 1;
			unsigned int bit20 : 1;
			unsigned int bit21 : 1;
			unsigned int bit22 : 1;
			unsigned int bit23 : 1;
			unsigned int bit24 : 1;
			unsigned int bit25 : 1;
			unsigned int bit26 : 1;
			unsigned int bit27 : 1;
			unsigned int bit28 : 1;
			unsigned int bit29 : 1;
			unsigned int bit30 : 1;
			unsigned int bit31 : 1;
		} bits;
		unsigned int port;
	} value;
	unsigned int    channel;
	unsigned char   useInterrupt;
};

void         XP_Init (void);
int          XP_Busy (void);
void         XP_SetAddress(unsigned int address);
unsigned int XP_Write(unsigned int data);
unsigned int XP_Read (void);

#endif

/***************************************************************************/
/* LCD                                                                     */
/***************************************************************************/
#ifndef _XP_LCD_H__

unsigned int XP_lcd_Clear     (void);
unsigned int XP_lcd_MoveCursor(         int lin, int col);
void         XP_lcd_setBL     (unsigned int enable);
unsigned int XP_lcd_Init      (unsigned int address, unsigned int model);
unsigned int XP_lcd_Write     (unsigned char cmd, unsigned char data);
unsigned int XP_lcd_WriteData (unsigned char data);
unsigned int XP_lcd_WriteInstr(unsigned char instr);
unsigned int XP_lcd_WriteText (char *format, volatile int *val);

#define XP_LCD_TYPE_GENERIC 0
#define XP_LCD_TYPE_SC2004A 1

#endif
