// definitions common to all driver DLLs
#ifndef _FMCOMMONH_
#define _FMCOMMONH_

#define FM_PROGOPT_NONE     0
#define FM_PROGOPT_RESERVED 1

#include <windows.h>

#ifdef __cplusplus
  #define CSRC "C"
#else
  #define CSRC
#endif

#ifdef _BUILD_DLL_
  #define DLL_FUNC extern CSRC __declspec(dllexport)
#else
  #define DLL_FUNC extern CSRC __declspec(dllimport)
#endif

#define FM_OK          1
#define FM_OK_BLANK    2
#define FM_OK_NOTBLANK 3

#define FM_ERROR                    -1
#define FM_ERROR_PORT               -2
#define FM_ERROR_BAUDRATE           -3
#define FM_ERROR_AUTOBAUD           -4
#define FM_ERROR_INVALID_PARAMS     -5
#define FM_ERROR_UNKNOWN_DEVICE     -6
#define FM_ERROR_CONNECT            -7
#define FM_ERROR_READ               -8
#define FM_ERROR_FILE               -9
#define FM_ERROR_PROGRAM            -10
#define FM_ERROR_HEX_CHECKSUM       -11
#define FM_ERROR_CHECKSUMS          -12
#define FM_ERROR_GENERATE_CHECKSUMS -13
#define FM_ERROR_FILL               -14
#define FM_ERROR_OPEN               -15
#define FM_ERROR_UNSUPPORTED        -16
#define FM_ERROR_VERIFY             -17
#define FM_ERROR_CMD                -18
#define FM_ERROR_ALLOCATION         -19
#define FM_ERROR_CANCELLED          -20
#define FM_ERROR_SECURITY           -21
#define FM_ERROR_NACK               -22
#define FM_ERROR_JIT                -23
#define FM_ERROR_PASSWORD           -24
#define FM_ERROR_ALREADYAUTOBAUDED  -25

#define FM_BLOCKS -1
#define FM_NEEDED -2
#define FM_DEVICE -3

// values passed to callback func
#define FM_STATUS_ERASING_BLOCK  1
#define FM_STATUS_ERASING_DEVICE 2
#define FM_STATUS_PROGRAMMING    3
#define FM_STATUS_CHECKSUMS      4
#define FM_STATUS_FILLING_MEMORY 5
#define FM_STATUS_VERIFYING      6
#define FM_STATUS_READING        7
#define FM_STATUS_ERASING_PAGE   8
#define FM_STATUS_JIT_COMPLETE   9

// debug mode selections
#define FM_DEBUG_MODE_OFF 0
#define FM_DEBUG_MODE_ON  1

// hardware configuration selections
#define FM_HWNONE        0
#define FM_HWBOOTEXEC    1
#define FM_HWBOOTEXECRTS 2
#define FM_HWASSERT      3
#define FM_HWKEILMCB900  4

// programming finishing options
#define FM_NORMAL     0
#define FM_RETAINBVSB 1

// reset types
#define FM_RESET   0
#define FM_EXECUTE 1
// reset options
#define FM_ARM   0
#define FM_THUMB 1

// raw connect hardware configuration selections
#define FM_HWDEASSERTDTRDEASSERTRTS 1
#define FM_HWDEASSERTDTRASSERTRTS   2
#define FM_HWASSERTDTRDEASSERTRTS   3
#define FM_HWASSERTDTRASSERTRTS     4

// icp interfaces supported
#define FM_INTERFACETYPE_NONE              0     // none
#define FM_INTERFACETYPE_NXPBRIDGEICP      1     // NXP ISP to ICP bridge
#define FM_INTERFACETYPE_FDIUSBICPLPC9XX   2     // FDI USB-ICP-LPC9XX
#define FM_INTERFACETYPE_FDIUSBDONGLE      3     // FDI USB-DONGLE
#define FM_INTERFACETYPE_FDIUSBICP80C51ISP 4     // FDI USB-ICP-80C51ISP
#define FM_INTERFACETYPE_FDIUSBICPLPC2K    5     // FDI USB-ICP-LPC2K
#define FM_INTERFACETYPE_NXPBRIDGEPP       6     // NXP ISP to PP bridge

// maximum results data sizes
#define FM_MAXDATALEN    60
#define FM_MAXDETAILSLEN 400

#define FM_MAX_BLOCKS              32      // max number of flash blocks
#define FM_MAX_PAGES               1024    // max number of flash pages
#define FM_MAX_SECURITYBITS        96      // max security bits for any device
#define FM_MAX_EEPROM_PAGES        128     // max number of eeprom pages
#define FM_MAX_EEPROM_SECURITYBITS 96      // max eeprom security bits for any device

// program options
#define FM_PROGOPT_NONE     0
#define FM_PROGOPT_RESERVED 1

// CAN interface types
#define FM_CAN_PEAKUSB      2
#define FM_CAN_PEAKPCI      3
#define FM_CAN_PEAKPCCARD   4

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  int result;
  unsigned char data[FM_MAXDATALEN];
  char details[FM_MAXDETAILSLEN];
} fm_results;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  double osc;                   // oscillator frequency
  int port;                     // com port
  long baudrate;                // baudrate to connect at
  int selecteddevice;           // device connecting to
  int highspeed;                // 1 = use high speed comms, 0 = don't use
  int clocks;                   // 6 or 12 (or 0 if not required)
  int halfduplex;               // 1 = use half duplex, 0 = don't use
  int hwconfig;                 // specifies DTR and RTS configuration
  int hwt1;                     // DTR and RTS timing parameter 1
  int hwt2;                     // DTR and RTS timing parameter 2
  unsigned char i2caddr;        // I2C address for devices on I2C bus (depreciated)
  long maxbaudrate;             // maximum baudrate to use for high speed communications
  int usinginterface;           // set to 1 if using an interface
  int interfacetype;            // interface type being used (FM_INTERFACETYPE_xxx)
} fm_connectoptions_com;

typedef struct
{
  int adapterindex;             // ethernet adapter index as returned from GetAdaptersInfo
  int selecteddevice;           // device connecting to
  unsigned long ipaddr;         // ip address to use for bootloader
  unsigned char macaddr[6];     // MAC address of bootloader
} fm_connectoptions_ethernet;

typedef struct
{
  double osc;                   // oscillator frequency
  int selecteddevice;           // device connecting to
  int highspeed;                // 1 = use high speed comms, 0 = don't use
  int interfacetype;            // can interface type (FM_CAN_xxx)
  long baudrate;                // baudrate in kbps
  unsigned long hwconfig[2];    // can hardware configuration (depends on interface type)
  unsigned char nodeid;         // id of node to connect to
  long sdotimeout;              // sdo response timeout in milliseconds
} fm_connectoptions_can;
#pragma pack(pop)

typedef int (_cdecl *progressfunc)(int status, unsigned long value, unsigned long value2, void *callbackparam);

// #define kludge. If we are being compiled with MSVC, then just tack on a
// leading underscore because Borland C++ will export Foo and Bar as _Foo
// and _Bar respectively
#ifdef _MSC_VER
#define fm_version                        _fm_version
#define fm_longversion                    _fm_longversion
#define fm_connect                        _fm_connect
#define fm_disconnect                     _fm_disconnect
#define fm_select_device                  _fm_select_device
#define fm_read_signature                 _fm_read_signature
#define fm_erase                          _fm_erase
#define fm_program                        _fm_program
#define fm_verify                         _fm_verify
#define fm_read_security_bits             _fm_read_security_bits
#define fm_program_security_bits          _fm_program_security_bits
#define fm_blank_check                    _fm_blank_check
#define fm_read_data                      _fm_read_data
#define fm_read_boot_vector_status_byte   _fm_read_boot_vector_status_byte
#define fm_erase_boot_vector_status_byte  _fm_erase_boot_vector_status_byte
#define fm_program_boot_vector            _fm_program_boot_vector
#define fm_program_status_byte            _fm_program_status_byte
#define fm_read_data_to_buffer            _fm_read_data_to_buffer
#define fm_reset                          _fm_reset
#define fm_select_debug_mode              _fm_select_debug_mode
#define fm_start_bootrom                  _fm_start_bootrom
#define fm_start_bootloader               _fm_start_bootloader
#define fm_read_clocks_bit                _fm_read_clocks_bit
#define fm_program_clocks_bit             _fm_program_clocks_bit
#define fm_program_i2c_address            _fm_program_i2c_address
#define fm_read_configuration             _fm_read_configuration
#define fm_program_configuration          _fm_program_configuration
#define fm_clear_configuration_protection _fm_clear_configuration_protection
#define fm_erase_pages                    _fm_erase_pages
#define fm_read_crc                       _fm_read_crc
#define fm_read_misr                      _fm_read_misr
#define fm_cancelautobaud                 _fm_cancelautobaud
#define fm_set_password                   _fm_set_password
#define fm_reset_password                 _fm_reset_password
#define fm_verify_password                _fm_verify_password
#define fm_set_timeouts                   _fm_set_timeouts
#define fm_set_default_timeouts           _fm_set_default_timeouts
#define fm_enable_softice                 _fm_enable_softice
#define fm_program_addl_security_bits     _fm_program_addl_security_bits
#define fm_erase_addl_security_bits       _fm_erase_addl_security_bits
#define fm_read_addl_security_bits        _fm_read_addl_security_bits
#define fm_update_bootloader              _fm_update_bootloader
#define fm_raw_connect                    _fm_raw_connect
#define fm_raw_disconnect                 _fm_raw_disconnect
#define fm_raw_transmit                   _fm_raw_transmit
#define fm_raw_receive                    _fm_raw_receive
#define fm_raw_cancelreceive              _fm_raw_cancelreceive
#define fm_bootloader_version             _fm_bootloader_version
#define fm_serial_number                  _fm_serial_number
#define fm_read_eeprom_security_bits      _fm_read_eeprom_security_bits
#define fm_program_eeprom_security_bits   _fm_program_eeprom_security_bits
#define fm_erase_eeprom_pages             _fm_erase_eeprom_pages
#define fm_read_eeprom_crc                _fm_read_eeprom_crc
#define fm_program_eeprom                 _fm_program_eeprom
#endif

DLL_FUNC fm_results * _cdecl fm_version(void);
DLL_FUNC fm_results * _cdecl fm_longversion(void);
DLL_FUNC fm_results * _cdecl fm_connect(void *options, int optionssize);
DLL_FUNC fm_results * _cdecl fm_disconnect(void);
DLL_FUNC fm_results * _cdecl fm_select_device(int new_device);
DLL_FUNC fm_results * _cdecl fm_read_signature(void);
DLL_FUNC fm_results * _cdecl fm_erase(int mode, unsigned long blocks, int protectisp, progressfunc progress, void *callbackparam, char *path);
DLL_FUNC fm_results * _cdecl fm_program(char *path, int checksums, int fill, unsigned char fillbyte, int protectisp, char *jitcmd, char *jitoptions, int jittimeout, int finishopt, unsigned long options, progressfunc progress, void *callbackparam);
DLL_FUNC fm_results * _cdecl fm_verify(char *path, int ignorechecksumlocs, progressfunc progress, void *callbackparam);
DLL_FUNC fm_results * _cdecl fm_read_security_bits(void);
DLL_FUNC fm_results * _cdecl fm_program_security_bits(int bits[]);
DLL_FUNC fm_results * _cdecl fm_blank_check(unsigned long start, unsigned long end);
DLL_FUNC fm_results * _cdecl fm_read_data(unsigned long start, unsigned long end, char *path, progressfunc progress, void *callbackparam);
DLL_FUNC fm_results * _cdecl fm_read_boot_vector_status_byte(void);
DLL_FUNC fm_results * _cdecl fm_erase_boot_vector_status_byte(void);
DLL_FUNC fm_results * _cdecl fm_program_boot_vector(unsigned int value);
DLL_FUNC fm_results * _cdecl fm_program_status_byte(unsigned char value);
DLL_FUNC fm_results * _cdecl fm_read_data_to_buffer(unsigned long start, unsigned long end, unsigned char *buffer, progressfunc progress, void *callbackparam);
DLL_FUNC fm_results * _cdecl fm_reset(int type, unsigned long address, unsigned long options);
DLL_FUNC fm_results * _cdecl fm_select_debug_mode(int mode, char *path);
DLL_FUNC fm_results * _cdecl fm_start_bootrom(int port, long baudrate, char *command, int halfduplex, int brk);
DLL_FUNC fm_results * _cdecl fm_start_bootloader(int port, long baudrate, char *command, int halfduplex, int brk);
DLL_FUNC fm_results * _cdecl fm_read_clocks_bit(void);
DLL_FUNC fm_results * _cdecl fm_program_clocks_bit(void);
DLL_FUNC fm_results * _cdecl fm_program_i2c_address(unsigned char value);
DLL_FUNC fm_results * _cdecl fm_read_configuration(void);
DLL_FUNC fm_results * _cdecl fm_program_configuration(unsigned long value);
DLL_FUNC fm_results * _cdecl fm_clear_configuration_protection(void);
DLL_FUNC fm_results * _cdecl fm_erase_pages(int pages[], int protectisp, progressfunc progress, void *callbackparam);
DLL_FUNC fm_results * _cdecl fm_read_crc(unsigned long block);
DLL_FUNC fm_results * _cdecl fm_read_misr(unsigned long block);
DLL_FUNC fm_results * _cdecl fm_cancelautobaud(void);
DLL_FUNC fm_results * _cdecl fm_set_password(unsigned char *password, int passwordlength);
DLL_FUNC fm_results * _cdecl fm_reset_password(void);
DLL_FUNC fm_results * _cdecl fm_verify_password(unsigned char *password, int passwordlength);
DLL_FUNC void _cdecl         fm_set_timeouts(unsigned long shorttimeout, unsigned long longtimeout);
DLL_FUNC void _cdecl         fm_set_default_timeouts(void);
DLL_FUNC fm_results * _cdecl fm_enable_softice(void);
DLL_FUNC fm_results * _cdecl fm_program_addl_security_bits(int bits[]);
DLL_FUNC fm_results * _cdecl fm_erase_addl_security_bits(void);
DLL_FUNC fm_results * _cdecl fm_read_addl_security_bits(void);
DLL_FUNC fm_results * _cdecl fm_update_bootloader(char *path, progressfunc progress, void *callbackparam);
DLL_FUNC fm_results * _cdecl fm_raw_connect(int port, long baudrate, int hwconfig);
DLL_FUNC fm_results * _cdecl fm_raw_disconnect(void);
DLL_FUNC fm_results * _cdecl fm_raw_transmit(unsigned char *buffer, unsigned int bytestowrite);
DLL_FUNC fm_results * _cdecl fm_raw_receive(unsigned char *buffer, unsigned int bytestoread, unsigned int *bytesread);
DLL_FUNC fm_results * _cdecl fm_raw_cancelreceive(void);
DLL_FUNC fm_results * _cdecl fm_bootloader_version(void);
DLL_FUNC fm_results * _cdecl fm_serial_number(void);
DLL_FUNC fm_results * _cdecl fm_read_eeprom_security_bits(void);
DLL_FUNC fm_results * _cdecl fm_program_eeprom_security_bits(int bits[]);
DLL_FUNC fm_results * _cdecl fm_erase_eeprom_pages(int pages[], progressfunc progress, void *callbackparam);
DLL_FUNC fm_results * _cdecl fm_read_eeprom_crc(unsigned long page);
DLL_FUNC fm_results * _cdecl fm_program_eeprom(char *path, int checksums, int fill, unsigned char fillbyte, char *jitcmd, char *jitoptions, int jittimeout, progressfunc progress, void *callbackparam);

#undef DLL_FUNC
#undef CSRC

#endif // _FMCOMMONH_
