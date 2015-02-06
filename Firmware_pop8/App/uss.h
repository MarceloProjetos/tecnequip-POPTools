///////////////////////////////////////////////////////////////////////////////
/// \defgroup USS Universal Serial Interface Protocol (USS)
/// \author Alessandro Holanda
/// \version 1.0
/// \date 26.02.2009
/// \brief
/// Implementação do protocolo UniverSal Serial Interface (USS)
/// para a comunicação com os inversores de frequência da Nord:\n\n
/// NORDAC 700E
/// \note
/// O texto em inglês foi retirado do "Nord USS Protocol Manual For
/// NORDAC SK 700E / trio SK 300E / vector mc Frequency Inverters"(BU 0050 GB)
///////////////////////////////////////////////////////////////////////////////

#ifndef USS_H
#define USS_H

#include <string.h>
#include <stdlib.h>

#define HTONS(n) (unsigned short int)((((unsigned short int) (n)) << 8) | (((unsigned short int) (n)) >> 8))

extern unsigned int RS485_Read(unsigned char * buffer, unsigned int size);
extern unsigned int RS485_Write(unsigned char * buffer, unsigned int size);

//#define USS_DEBUG

/* @{ */
// ---------------------------------------------------------------------------
// Inicio das estruturas de dados. Ajusta o alinhamento para 1 byte
// Nao é suportado na plataforma ARM, utilizar -fpack-struct=1 nos param gcc
//----------------------------------------------------------------------------
//#pragma pack(push, 1)

///////////////////////////////////////////////////////////////////////////////
/// \b Abreviations
///
/// PPO Parameter Process Data Object
/// PKW Parameter Label Value
/// PZD Process Data
/// PKE Parameter Label
/// IND Index
/// PWE Parameter Value
/// STW Control word
/// ZSW Status word
/// SW1..3 Setpoints 1-3
/// IW1..3 Actual values 1-3
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// \section prot Protocol specification
///
/// \brief
/// The USS protocol provides a method of access to and communication over a serial bus system based on the
/// master-slave principle. The USS protocol also includes point-to-point communication as a subset. It allows for
/// connecting one master and a maximum number of 31 slaves to the bus. The master will select any individual slave
/// via an address character in the telegram. The protocol does not provide for a direct exchange of messages
/// between one slave and another. The bus system will be operated using a two-way alternate type of
/// communication. The master status, once assigned, cannot be transferred to any other unit (single-master system).
/// Data transfer along the two-wire bus is carried out character by character in the following format:
/// 1 start bit, 8 data bits, 1 even-parity bit and 1 stop bit (8E1) – the character frame amounting to 11 bits in all.
/// The direction of data transmission over the bus (send or receive) is controlled by the master.
///////////////////////////////////////////////////////////////////////////////

/**----------------------------------------------------------------------------
/// \subpage chframe Character frame
//-----------------------------------------------------------------------------
/// \brief
/// At the beginning of every transmitted character there is a start bit (logical zero), and a stop bit (logical one) is at its
/// end. A number of 8 bits (1 byte) is transmitted in between. A parity bit (even parity) will follow the data byte to
//exclude transmission errors. Thus 11 bits will be transmitted per character in all.
//---------------------------------------------------------------------------*/

typedef unsigned char STX;    /** (Start of text): ASCII character : 02 hex */
typedef unsigned char LGE;    /** (Length of telegram): contains the telegram length. */

///////////////////////////////////////////////////////////////////////////////
/// \union ADR
/// \brief
/// Each telegram begins with the STX start character (= 02 hex), which is followed by the length of telegram
/// information (LGE) and the address byte (ADR). The useful characters (the actual message) are transmitted next.
/// The telegram is terminated by the BCC (Block Check Character) which is used to determine whether the amount of
/// data transmitted is correct.
///////////////////////////////////////////////////////////////////////////////
typedef union {
  unsigned char byte;
  struct {                /** (Address byte): contains the slave address and other information.*/
    unsigned ADDRESS:5;   /** [0-4] The USS address */
    unsigned BC:1;        /** [5] Broadcasting */
    unsigned M:1;         /** [6] Mirror */
    unsigned reserved:1;  /** [7] reserved */
  };
} ADR;

/// ATENÇÃO: Alguns exemplos do datasheet contem erros de calculo do checksum
typedef unsigned char BCC;  /** (Block Check Character) for verification of data amount */

/**----------------------------------------------------------------------------
/// \section struct Structure of useful data characters
//-----------------------------------------------------------------------------
/// \brief
/// The useful data are divided into two categories:
/// - PZD section (Prozessdaten = Process Data)
/// - PKW section (and containing parameter information; Parameter-Kennung-Wert = Parameter-Label-Value)
//
/// An access operation always consists of a job telegram and a reply telegram. In a job telegram useful data are
/// transmitted from the master to the slave. In the reply telegram useful data are transmitted from the slave to the
/// master. The format of the two telegram types is the same.
//---------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
/// \sub-page pzd PZD - Process Data Section
/// \brief
/// The PZD (process data) section is used for frequency inverter control. Transmission of process data means
/// transmission of control words and setpoint values in one direction and transmission of status words and actual
/// values in the other.
/// The process data (PZD) are processed in the frequency inverter immediately (high priority) to ensure that the
/// system will respond to control commands instantly or, the other way round, to enable status changes to be
/// signalled to the master without delay.
//---------------------------------------------------------------------------*/

//--------------------------- Control word (STW) ------------------------------
typedef union {
  unsigned short int word;
  struct {
    unsigned char byteL;
    unsigned char byteH;
  };
  struct {
    unsigned bit0:1; /** Read for operation */
    unsigned bit1:1; /** OFF 2 is cancelled */
    unsigned bit2:1; /** OFF 3 is cancelled */
    unsigned bit3:1; /** Enable operation: The output voltage is enable, ramping to the setpoint applied */
    unsigned bit4:1; /** Ramp generation is enable */
    unsigned bit5:1; /** Setpoint is enable on the ramp generator */
    unsigned bit6:1; /** Selected setpoint is activated on the ramp gererator */
    unsigned bit7:1; /** Acknowledge */
    unsigned bit8:1; /** Pre-assigned */
    unsigned bit9:1; /** Pre-assigned */
    unsigned bit10:1; /** PZD valid */
    unsigned bit11:1; /** Positive phase sequence ON */
    unsigned bit12:1; /** Negative phase sequence OFF */
    unsigned bit13:1; /** Pre-assigned */
    unsigned bit14:1; /** Switch parameter set [00] - Parameter Set 1 ~ [11] Parameter Set 4 */
    unsigned bit15:1; /** Parameter Set */
  };
} STW;

//--------------------------- The status word (ZSW) ---------------------------
typedef union {
  unsigned short int word;
  struct {
    unsigned char byteL;
    unsigned char byteH;
  };
  struct {
    unsigned bit0:1; /** Read to start/Not ready to be started */
    unsigned bit1:1; /** Read for operation/Not read for operation */
    unsigned bit2:1; /** Operation enable/Operation disabled */
    unsigned bit3:1; /** Trouble/No trouble */
    unsigned bit4:1; /** Voltage disabled */
    unsigned bit5:1; /** No quick stopping */
    unsigned bit6:1; /** Starting disable */
    unsigned bit7:1; /** Warning/No warning */
    unsigned bit8:1; /** Setpoint=actual value */
    unsigned bit9:1; /** Bus control */
    unsigned bit10:1; /** MFR 1 reference value has been equalled/Failure to reach reference value */
    unsigned bit11:1; /** Positive phase sequence ON */
    unsigned bit12:1; /** Negative phase sequence ON/Negative phase sequence OFF */
    unsigned bit13:1; /** MFR 4 reference value has been equalled/Pre-assigned */
    unsigned bit14:1; /** Parameter set bit 0 ON/Parameter set bit 0 OFF */
    unsigned bit15:1; /** Parameter set bit 1 ON/Parameter set bit 1 OFF */
  };
} ZSW;

//--------------------------- Setpoint no. 1 (SW1) ---------------------------
//The function to be associated with the 1st setpoint is selected in parameter P546.
/// The following options are available:
/// - Setpoint frequency (16 bits)
/// - Setpoint position (16 or 32 bits)
//----------------------------------------------------------------------------
typedef union {
  unsigned short int word;
  struct {
    unsigned char byteL;
    unsigned char byteH;
  };
} SW1;

//-------------------- Second / third setpoint (SW 2/3) -----------------------
/// The second and the third setpoint are always 16 bits wide. The function of the second and third setpoint can be
/// selected in the inverter in the parameter P547 'Function setpoint 2' or in P548 'Function setpoint 3'.
/// Both setpoints are transmitted as integral numbers in the range (-32768 to 32767). The value 16384 (4000 HEX) is
/// equivalent to 100%. The value C000 HEX is equivalent to –100%. A 100% setpoint will be equivalent to the
/// respective nominal value:
/// Setting                                        100% correspond to the
/// Off
/// Setpoint frequency, PID actual frequency, PID  Maximum frequency
/// actual frequency limited, PID actual frequency
/// monitored,    frequency   addition,  frequency
/// subtraction, maximum frequency
/// Torque current limit                           Torque current limit (P112)
/// Current limit                                  Nominal inverter current
/// Torque servo mode                              Nominal torque
/// Torque derivative action                       Torque derivative action (P214)
//-----------------------------------------------------------------------------
typedef union {
  unsigned short int word;
  struct {
    unsigned char byteL;
    unsigned char byteH;
  };
} SW3;

typedef union {
  unsigned short int word;
  struct {
    unsigned char byteL;
    unsigned char byteH;
  };
} SW2;

//--------------------------- Actual value no. 1 (IW1) ------------------------
typedef short int IW1;
typedef short int IW3;
typedef short int IW2;

/**----------------------------------------------------------------------------
/// \section pkw PKW - Parameter Label Value
//-----------------------------------------------------------------------------
/// \brief
/// The PKW section of useful data allows for parameter values to be read and written. Those jobs which are
/// transmitted via the PKW interface mainly affect configuration, monitoring, and diagnosis.
/// A lower priority is assigned to the speed with which PKW data are processed so that processing
/// these may take noticeably longer.
//---------------------------------------------------------------------------*/

//---------------------------Parameter label PKE ------------------------------
typedef union {
  unsigned short int word;
  struct {
    unsigned char byteL;
    unsigned char byteH;
  };
  struct {
    unsigned PNU:11; /** [0-10] contain the number of the parameter about to be addressed */
    unsigned SPM:1; /** [11] toggle bit for spontaneous messages */
    unsigned AK:4; /** [12-15] contain the job or the reply code respectively */
  };
} PKE;

/// The table below lists all of the jobs which can be transmitted from the master to the inverter. The right column
/// contains the reply which will be sent if everything is o.k. (positive acknowledgement). Each job code is associated
/// with one particular reply code excluding any others. If an error has occurred (negative acknowledgement) the
/// inverter will always submit the value 7 to the master in the reply code (AK).

#define PKE_AK_NO_JOB_TRANSMITTED (0 << 12)
#define PKE_AK_REQUEST_PARAM_VALUE (1 << 12)
#define PKE_AK_CHANGE_PARAM_VALUE_W (2 << 12)
#define PKE_AK_CHANGE_PARAM_VALUE_DW (3 << 12)
#define PKE_AK_REQUEST_PARAM_VALUE_ARRAY (6 << 12)
#define PKE_AK_REQUEST_PARAM_VALUE_ARRAY_W (7 << 12)
#define PKE_AK_REQUEST_PARAM_VALUE_ARRAY_DW (8 << 12)
#define PKE_AK_REQUEST_NUM_ARRAY_ELEMENTS (9 << 12)
#define PKE_AK_CHANGE_PARAM_VALUE_ARRAY_DW_NO_EEPROM (11 << 12)
#define PKE_AK_CHANGE_PARAM_VALUE_ARRAY_W_NO_EEPROM (12 << 12)
#define PKE_AK_CHANGE_PARAM_VALUE_DW_NO_EEPROM (13 << 12)
#define PKE_AK_CHANGE_PARAM_VALUE_W_NO_EEPROM (14 << 12)

/// Meaning of the values transmitted with the reply code
#define PKE_AK_NO_REPLY 0
#define PKE_AK_PARAM_VALUE_TRANSMITTED_W 1
#define PKE_AK_PARAM_VALUE_TRANSMITTED_DW 2
#define PKE_AK_PARAM_VALUE_TRANSMITTED_ARRAY_W 4
#define PKE_AK_PARAM_VALUE_TRANSMITTED_ARRAY_DW 5
#define PKE_AK_JOB_CANNOT_EXECUTED 7

/// If the reply code says "job cannot be executed" (AK = 7), an error code is appended to the parameter value
/// (PWE2) the inverter sends as a reply. The table below shows the meaning of the transmitted values.

#define PWE2_ILLEGAL_PARAM_NUM 0
#define PWE2_PARAM_VALUE_CANNOT_CHANGE 1
#define PWE2_VALUE_LIMITS_EXCEEDED 2
#define PWE2_SUBINDEX_WRONG 3
#define PWE2_IS_NOT_ARRAY 4
#define PWE2_DATA_TYPE_ILLEGAL 5
#define PWE2_ONLY_FOR_RESET 6
#define PWE2_ELEMENT_CANNOT_CHANGED 7
#define PWE2_DATA_REQUEST_NOT_AVAILABLE 9
#define PWE2_JOB_CONTAINS_INVALID_ELEMENT 201
#define PWE2_INTERNAL_REPLY_CODE_CANNOT_MAPPED 202

//----------------------------- Subindex (IND) --------------------------------
//Structure and function of the parameter index (IND) vary with the type of the parameter to be transmitted.
//Wherever a value will be effective in a single parameter set only, bits 0 and 1 of the index (IND) may be used to
//select the parameter set (0 = parameter set 1, 1 = parameter set 2,...).
//If the parameter about to be edited is also an array parameter, (e.g. a position array type when the PosiCon option
//is involved), the subindex of the desired parameter can be addressed using bit 2 to bit 7 (0 = array element 1, 1 =
//array element 2, ...):
//-----------------------------------------------------------------------------
typedef union {
  unsigned short int word;
  struct {
    unsigned char byteL;
    unsigned char byteH;
  };
  struct {
    unsigned parameter1:2;
    unsigned nothing:14;
  };
  struct {
    unsigned parameter2:2;
    unsigned array64:6;
    unsigned nothing1:8;
  };
  struct {
    unsigned subindex:8;
    unsigned nothing2:8;
  };
  struct {
    unsigned array256:8;
    unsigned nothing3:8;
  };
} IND;

//----------------------------- Parameter value (PWE) -------------------------
/// Parameter values (PWE, ParameterWert) are always transmitted as single words (16 bits) or as double words (32
/// bits) depending on the PPO type and the parameter type. Only one parameter value can be transmitted in a
/// telegram at a time.
/// A 32-bit parameter value is composed of a PWEH (most significant word) and a PWEL (least significant word, 4th
/// word) element.
/// With PPO1 and PPO2 a 16-bit parameter value is transmitted in PWEL. If the values are negative, the more
/// significant word must be set to FFFF hex.
/// The parameter value is transmitted as an integral number. Where parameters are based on resolutions of 0.1 or
/// 0.01 respectively the parameter value must be multiplied with the reciprocal of the amount of resolution.
/// Example: The intention is to set a ramp-up period of 99.99 seconds:
/// 99.99s → 99.99 * 1/0.01 = 99.99 * 100 = 9999. Thus the value to be transmitted will have to be 9999dez = 270Fhex.
//-----------------------------------------------------------------------------
typedef short int PWEH;
typedef short int PWEL;

/**----------------------------------------------------------------------------
/// \section ppo PPO Types
//-----------------------------------------------------------------------------
/// \brief
/// The telegrams used for cyclic data traffic are referred to and defined as Parameter Process data Objects (PPO)
/// and will transmit both process data (PZD) and parameter information (PKW) from the master to the inverter. The
/// inverter is capable of processing any of the PPO types 0, 1, 2, 3 or 4.
/// Type  Length (LGE) Job
/// PPO 0      12      Standard telegram containing process data and a 16-bit parameter value
/// PPO 1      14      Enlarged parameter telegram with a 32-bit parameter value and process data
/// PPO 2      18      Telegram with more process data (principal plus two secondary setpoints)
///                    and a 32-bit parameter value
/// PPO 3       6      Process data telegram with principal setpoint, without parameter data
/// PPO 4      10      Enlarged process data telegram with principal and secondary setpoints,
///                    without parameter data
//---------------------------------------------------------------------------*/

//------------------------- PPO Message Type ----------------------------------
#define PPO1_TYPE 1  /** PPO1 message type */

//------------------------- STX Start Character -------------------------------
#define PPO_STX 0x02

//------------------------- PPO LGE Message Size ------------------------------
#define PPO1_LGE 14
#define PPO1_SIZE PPO1_LGE

// Devido a problemas de alinhamento, refazendo estrutura utilizando um buffer char simples
// e com funções de macro para carregar os valores na posição correta do buffer
typedef unsigned char PPO1[PPO1_SIZE+2]; // Comprimento da mensagem + STX + LGE

#define USS_PPO_INIT(bf,lge)                 do { memset((void *)bf, 0, sizeof(bf)); bf[0] = PPO_STX; bf[1] = lge; } while(0)
#define USS_PPO_SET_ADR(bf,multi,bc,address) do { bf[ 2] = (multi<<6) | (bc<<5) | address; } while(0)

#define USS_PPO1_SET_PKE(bf,pke)             do { bf[ 3] = pke>>8; bf[4] = pke&0xff; } while(0)
#define USS_PPO1_SET_IND(bf,par,idx)         do { bf[ 6] = ((idx)<<2) | (par); } while(0)
#define USS_PPO1_SET_PWEH(bf,pewh)           do { bf[ 7] = pewh>>8; bf[ 8] = pewh&0xff; } while(0)
#define USS_PPO1_SET_PWEL(bf,pewl)           do { bf[ 9] = pewl>>8; bf[10] = pewl&0xff; } while(0)
#define USS_PPO1_SET_STW(bf,pewl)            do { bf[11] = pewl>>8; bf[12] = pewl&0xff; } while(0)
#define USS_PPO1_SET_SW1(bf,pewl)            do { bf[13] = pewl>>8; bf[14] = pewl&0xff; } while(0)

#define USS_PPO1_GET_PKE(bf)     (((unsigned int)(bf[ 3])<<8) | bf[ 4])
#define USS_PPO1_GET_IND_PAR(bf) (bf[ 6] & 0x03)
#define USS_PPO1_GET_IND_IDX(bf) (bf[ 6] >> 2)
#define USS_PPO1_GET_PWEH(bf)    (((unsigned int)(bf[ 7])<<8) | bf[ 8])
#define USS_PPO1_GET_PWEL(bf)    (((unsigned int)(bf[ 9])<<8) | bf[10])
#define USS_PPO1_GET_STW(bf)     (((unsigned int)(bf[11])<<8) | bf[12])
#define USS_PPO1_GET_SW1(bf)     (((unsigned int)(bf[13])<<8) | bf[14])

/** ---------------------------------------------------------------------------
/// Fim das estruturas de dados. Retorna o alinhamento original
//---------------------------------------------------------------------------*/
//#pragma pack(pop)

/**----------------------------------------------------------------------------
/// Função: USS_PPO1
/// \param PKE
/// \param IND
/// \param PWEH
/// \param PWEL
/// \param STW
/// \param SW1
/// \return * PPO1 - Ponteiro para PPO1
/// , 0 (NULL) em caso de erro
/// \brief
/// Esta função retorna um telegrama do tipo PPO1 pronto para ser enviado
/// pelo meio fisico (UART, CAN, etc)
//---------------------------------------------------------------------------*/
//PPO1 USS_PPO1(ADR adr, PKE pke, IND ind, PWEH pweh, PWEL pwel, STW stw, SW1 sw1);
void USS_PPO1(PPO1 *ppo, unsigned char adr, unsigned short int pke, unsigned short int ind, unsigned short int pweh, unsigned short int pwel, unsigned short int stw, unsigned short int sw1);

//---------------- Funções com nivel de abstração mais alto -------------------

/**----------------------------------------------------------------------------
/// Função: USS_get_param
/// \param param_num
/// \return param value
/// \brief
/// Esta função retorna o valor de um parametro do inversor
//---------------------------------------------------------------------------*/
unsigned char USS_Get_Param(unsigned char addr, unsigned short int param, unsigned char param_set, unsigned char array, volatile int * val);

/**----------------------------------------------------------------------------
/// Função: USS_get_param
/// \param param_num
/// \param param_value
/// \return ok
/// \brief
/// Esta função altera o valor de um parametro no inversor
//---------------------------------------------------------------------------*/
unsigned char USS_Set_Param(unsigned char addr, unsigned short int param, unsigned char param_set, unsigned char array, volatile int * val);

/**----------------------------------------------------------------------------
/// Função: USS_set_BCC
/// \param unsigned char * ppo - Ponteiro para telegrama USS
/// \brief
/// O campo LGE com tipo da mensagem precisa estar preenchido
/// Esta função calcula o checksum do telegrama
/// O retorno dessa função pode ser usado para indicar para a rotina de envio
/// do meio fisico (UART, CAN, etc) o tamanho total em bytes a ser transmitido
//---------------------------------------------------------------------------*/
void USS_Set_BCC(unsigned char * ppo);
unsigned char USS_Check_CRC(unsigned char * ppo, unsigned int size);
unsigned char USS_Ready(unsigned char * r, unsigned int sz);

/**----------------------------------------------------------------------------
/// Função: USS_clean_ppo
/// \param unsigned char * ppo - Ponteiro para telegrama USS
/// \param unsigned int tamanho
/// \brief
/// Faz a limpeza da memoria antes de calcular o BCC, pois caso haja algum
/// lixo o calculo do checksum (BCC) vai dar errado
//---------------------------------------------------------------------------*/
void USS_Clean_PPO(unsigned char * ppo, unsigned int size);

#endif
