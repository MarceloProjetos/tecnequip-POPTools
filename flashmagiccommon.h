// definitions common to all driver DLLs
#ifndef _FMCOMMONH_
#define _FMCOMMONH_

#define FM_PROGOPT_NONE     0
#define FM_PROGOPT_RESERVED 1

#ifdef __cplusplus
  #define CSRC "C"
#else
  #define CSRC
#endif

#ifdef __GNUC__
  #define DLL_FUNC
#else
  #ifdef _BUILD_DLL_
    #define DLL_FUNC extern CSRC __declspec(dllexport)
  #else
    #define DLL_FUNC extern CSRC __declspec(dllimport)
  #endif
#endif

#ifdef __GNUC__
  #define CDECL __attribute__((cdecl))
#else
  #undef CDECL
  #define CDECL _cdecl
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
#define FM_HWDISABLE     5

// programming finishing options
#define FM_NORMAL     0
#define FM_RETAINBVSB 1

// reset types
#define FM_RESET   0
#define FM_EXECUTE 1
// reset options
// bit 0
#define FM_ARM     0x00000000
#define FM_THUMB   0x00000001
// bit 1
#define FM_VECTORS 0x00000002

// raw connect hardware configuration selections
#define FM_HWDEASSERTDTRDEASSERTRTS 1
#define FM_HWDEASSERTDTRASSERTRTS   2
#define FM_HWASSERTDTRDEASSERTRTS   3
#define FM_HWASSERTDTRASSERTRTS     4

// icp/swd interfaces supported
#define FM_INTERFACETYPE_NONE              0     // none
#define FM_INTERFACETYPE_NXPBRIDGEICP      1     // NXP ISP to ICP bridge
#define FM_INTERFACETYPE_FDIUSBICPLPC9XX   2     // FDI USB-ICP-LPC9XX
#define FM_INTERFACETYPE_FDIUSBDONGLE      3     // FDI USB-DONGLE
#define FM_INTERFACETYPE_FDIUSBICP80C51ISP 4     // FDI USB-ICP-80C51ISP
#define FM_INTERFACETYPE_FDIUSBICPLPC2K    5     // FDI USB-ICP-LPC2K
#define FM_INTERFACETYPE_NXPBRIDGEPP       6     // NXP ISP to PP bridge
#define FM_INTERFACETYPE_SWDLPC18004300DFU 7     // USB to SWD LPC1800/4300 DFU
#define FM_INTERFACETYPE_SWDLPC11U30       8     // USB to SWD LPC11U30

// maximum length of a com port name
#define FM_MAXCOMPORTNAMELEN 50

// maximum results data sizes
#define FM_MAXDATALEN    128
#define FM_MAXDETAILSLEN 400

// maximum length of direct entry production task data
#define FM_MAXPTDATALEN 128

#define FM_MAX_BLOCKS              128     // max number of flash blocks
#define FM_MAX_PAGES               2048    // max number of flash pages
#define FM_MAX_SECURITYBITS        96      // max security bits for any device
#define FM_MAX_EEPROM_PAGES        128     // max number of eeprom pages
#define FM_MAX_EEPROM_SECURITYBITS 96      // max eeprom security bits for any device

// maximum length of a file path and name
#define FM_MAXPATH 1024

// program options
#define FM_PROGOPT_NONE     0
#define FM_PROGOPT_RESERVED 1

// CAN interface types
#define FM_CAN_PEAKUSB      2
#define FM_CAN_PEAKPCI      3
#define FM_CAN_PEAKPCCARD   4

// production task types
#define FM_PRODUCTIONTASK_DISABLED     0
#define FM_PRODUCTIONTASK_DIRECTENTRY  1
#define FM_PRODUCTIONTASK_HEXFILE      2
#define FM_PRODUCTIONTASK_BINARYFILE   3
#define FM_PRODUCTIONTASK_HEXGENERATOR 4

// COM protocol options
#define FM_PROTOCOLCOM_NONE     0x00000000
#define FM_PROTOCOLCOM_DONTTXLF 0x00000001
#define FM_PROTOCOLCOM_DONTRXLF 0x00000002
#define FM_PROTOCOLCOM_AUTOLF   0x00000004

#define FM_INVALID_HANDLE_VALUE -1

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
  unsigned long startaddr;
  unsigned long type;
  unsigned long datalen;
  unsigned char data[FM_MAXPTDATALEN];
  char path[FM_MAXPATH];
} fm_productiontask;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  double osc;                                        // oscillator frequency
  char comportname[FM_MAXCOMPORTNAMELEN];            // com port name
  long baudrate;                                     // baudrate to connect at
  int selecteddevice;                                // device connecting to
  int highspeed;                                     // 1 = use high speed comms, 0 = don't use
  int clocks;                                        // 6 or 12 (or 0 if not required)
  int halfduplex;                                    // 1 = use half duplex, 0 = don't use
  int hwconfig;                                      // specifies DTR and RTS configuration
  int hwt1;                                          // DTR and RTS timing parameter 1
  int hwt2;                                          // DTR and RTS timing parameter 2
  unsigned char i2caddr;                             // I2C address for devices on I2C bus (depreciated)
  long maxbaudrate;                                  // maximum baudrate to use for high speed communications
  int usinginterface;                                // set to 1 if using an interface
  int interfacetype;                                 // interface type being used (FM_INTERFACETYPE_xxx)
  char bootloaderpath[FM_MAXPATH];                   // path and name of intermediate bootloader file (e.g. external flash bootloader)
  unsigned long flashbank;                           // flash bank to use (0-indexed)
  unsigned long protocoloptions;                     // protocol options (FM_PROTOCOLCOM_xxx)
} fm_connectoptions_com;

typedef struct
{
  int adapterindex;                                  // ethernet adapter index as returned from GetAdaptersInfo
  int selecteddevice;                                // device connecting to
  unsigned long ipaddr;                              // ip address to use for bootloader
  unsigned char macaddr[6];                          // MAC address of bootloader
  unsigned long flashbank;                           // flash bank to use (0-indexed)
  int udptxdelay;                                    // delay after UDP transmit in ms
} fm_connectoptions_ethernet;

typedef struct
{
  double osc;                                        // oscillator frequency
  int selecteddevice;                                // device connecting to
  int highspeed;                                     // 1 = use high speed comms, 0 = don't use
  int interfacetype;                                 // can interface type (FM_CAN_xxx)
  long baudrate;                                     // baudrate in kbps
  unsigned long hwconfig[2];                         // can hardware configuration (depends on interface type)
  unsigned char nodeid;                              // id of node to connect to
  long sdotimeout;                                   // sdo response timeout in milliseconds
  unsigned long flashbank;                           // flash bank to use (0-indexed)
} fm_connectoptions_can;
#pragma pack(pop)

typedef int (CDECL *progressfunc)(int status, unsigned long value, unsigned long value2, void *callbackparam);

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
#define fm_program2                       _fm_program2
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
#define fm_activate_flash_bank            _fm_activate_flash_bank
#define fm_program_eeprom_from_buffer     _fm_program_eeprom_from_buffer
#define fm_read_eeprom_to_buffer          _fm_read_eeprom_to_buffer
#endif

#define DEFINE_FUNC0(retval, name) \
  DLL_FUNC retval CDECL name(void); \
  typedef retval (CDECL * TYPE_##name)(void);

#define DEFINE_FUNC1(retval, name, param1) \
  DLL_FUNC retval CDECL name(param1); \
  typedef retval (CDECL * TYPE_##name)(param1);

#define DEFINE_FUNC2(retval, name, param1, param2) \
  DLL_FUNC retval CDECL name(param1, param2); \
  typedef retval (CDECL * TYPE_##name)(param1, param2);

#define DEFINE_FUNC3(retval, name, param1, param2, param3) \
  DLL_FUNC retval CDECL name(param1, param2, param3); \
  typedef retval (CDECL * TYPE_##name)(param1, param2, param3);

#define DEFINE_FUNC4(retval, name, param1, param2, param3, param4) \
  DLL_FUNC retval CDECL name(param1, param2, param3, param4); \
  typedef retval (CDECL * TYPE_##name)(param1, param2, param3, param4);

#define DEFINE_FUNC5(retval, name, param1, param2, param3, param4, param5) \
  DLL_FUNC retval CDECL name(param1, param2, param3, param4, param5); \
  typedef retval (CDECL * TYPE_##name)(param1, param2, param3, param4, param5);

#define DEFINE_FUNC6(retval, name, param1, param2, param3, param4, param5, param6) \
  DLL_FUNC retval CDECL name(param1, param2, param3, param4, param5, param6); \
  typedef retval (CDECL * TYPE_##name)(param1, param2, param3, param4, param5, param6);

#define DEFINE_FUNC9(retval, name, param1, param2, param3, param4, param5, param6, param7, param8, param9) \
  DLL_FUNC retval CDECL name(param1, param2, param3, param4, param5, param6, param7, param8, param9); \
  typedef retval (CDECL * TYPE_##name)(param1, param2, param3, param4, param5, param6, param7, param8, param9);

#define DEFINE_FUNC11(retval, name, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11) \
  DLL_FUNC retval CDECL name(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11); \
  typedef retval (CDECL * TYPE_##name)(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);

#define DEFINE_FUNC12(retval, name, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12) \
  DLL_FUNC retval CDECL name(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12); \
  typedef retval (CDECL * TYPE_##name)(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12);

DEFINE_FUNC0 (fm_results *, fm_version);
DEFINE_FUNC0 (fm_results *, fm_longversion);
DEFINE_FUNC2 (fm_results *, fm_connect,                      void *options, int optionssize);
DEFINE_FUNC0 (fm_results *, fm_disconnect);
DEFINE_FUNC2 (fm_results *, fm_select_device,                int new_device, unsigned long new_flashbank);
DEFINE_FUNC0 (fm_results *, fm_read_signature);
DEFINE_FUNC6 (fm_results *, fm_erase,                        int mode, int blocks[], int protectisp, progressfunc progress, void *callbackparam, char *path);
DEFINE_FUNC12(fm_results *, fm_program,                      char *path, int checksums, int fill, unsigned char fillbyte, int protectisp, char *jitcmd, char *jitoptions, int jittimeout, int finishopt, unsigned long options, progressfunc progress, void *callbackparam);
DEFINE_FUNC11(fm_results *, fm_program2,                     char *path, int checksums, int fill, unsigned char fillbyte, int protectisp, int productiontasknum, fm_productiontask *tasks, int finishopt, unsigned long options, progressfunc progress, void *callbackparam);
DEFINE_FUNC4 (fm_results *, fm_verify,                       char *path, int ignorechecksumlocs, progressfunc progress, void *callbackparam);
DEFINE_FUNC0 (fm_results *, fm_read_security_bits);
DEFINE_FUNC1 (fm_results *, fm_program_security_bits,        int bits[]);
DEFINE_FUNC2 (fm_results *, fm_blank_check,                  unsigned long start, unsigned long end);
DEFINE_FUNC5 (fm_results *, fm_read_data,                    unsigned long start, unsigned long end, char *path, progressfunc progress, void *callbackparam);
DEFINE_FUNC0 (fm_results *, fm_read_boot_vector_status_byte);
DEFINE_FUNC0 (fm_results *, fm_erase_boot_vector_status_byte);
DEFINE_FUNC1 (fm_results *, fm_program_boot_vector,          unsigned int value);
DEFINE_FUNC1 (fm_results *, fm_program_status_byte,          unsigned char value);
DEFINE_FUNC5 (fm_results *, fm_read_data_to_buffer,          unsigned long start, unsigned long end, unsigned char *buffer, progressfunc progress, void *callbackparam);
DEFINE_FUNC3 (fm_results *, fm_reset,                        int type, unsigned long address, unsigned long options);
DEFINE_FUNC2 (fm_results *, fm_select_debug_mode,            int mode, char *path);
DEFINE_FUNC6 (fm_results *, fm_start_bootloader,             char *comportname, long baudrate, char *command, int halfduplex, int brk, int interfacetype);
DEFINE_FUNC0 (fm_results *, fm_read_clocks_bit);
DEFINE_FUNC0 (fm_results *, fm_program_clocks_bit);
DEFINE_FUNC1 (fm_results *, fm_program_i2c_address,          unsigned char value);
DEFINE_FUNC0 (fm_results *, fm_read_configuration);
DEFINE_FUNC1 (fm_results *, fm_program_configuration,        unsigned long value);
DEFINE_FUNC0 (fm_results *, fm_clear_configuration_protection);
DEFINE_FUNC4 (fm_results *, fm_erase_pages,                  int pages[], int protectisp, progressfunc progress, void *callbackparam);
DEFINE_FUNC1 (fm_results *, fm_read_crc,                     unsigned long block);
DEFINE_FUNC1 (fm_results *, fm_read_misr,                    unsigned long block);
DEFINE_FUNC0 (fm_results *, fm_cancelautobaud);
DEFINE_FUNC2 (fm_results *, fm_set_password,                 unsigned char *password, int passwordlength);
DEFINE_FUNC0 (fm_results *, fm_reset_password);
DEFINE_FUNC2 (fm_results *, fm_verify_password,              unsigned char *password, int passwordlength);
DEFINE_FUNC2 (void,         fm_set_timeouts,                 unsigned long shorttimeout, unsigned long longtimeout);
DEFINE_FUNC0 (void,         fm_set_default_timeouts);
DEFINE_FUNC0 (fm_results *, fm_enable_softice);
DEFINE_FUNC1 (fm_results *, fm_program_addl_security_bits,   int bits[]);
DEFINE_FUNC0 (fm_results *, fm_erase_addl_security_bits);
DEFINE_FUNC0 (fm_results *, fm_read_addl_security_bits);
DEFINE_FUNC3 (fm_results *, fm_update_bootloader,            char *path, progressfunc progress, void *callbackparam);
DEFINE_FUNC4 (fm_results *, fm_raw_connect,                  char *comportname, long baudrate, int hwconfig, int interfacetype);
DEFINE_FUNC0 (fm_results *, fm_raw_disconnect);
DEFINE_FUNC2 (fm_results *, fm_raw_transmit,                 unsigned char *buffer, unsigned int bytestowrite);
DEFINE_FUNC3 (fm_results *, fm_raw_receive,                  unsigned char *buffer, unsigned int bytestoread, unsigned int *bytesread);
DEFINE_FUNC0 (fm_results *, fm_raw_cancelreceive);
DEFINE_FUNC0 (fm_results *, fm_bootloader_version);
DEFINE_FUNC0 (fm_results *, fm_serial_number);
DEFINE_FUNC0 (fm_results *, fm_read_eeprom_security_bits);
DEFINE_FUNC1 (fm_results *, fm_program_eeprom_security_bits, int bits[]);
DEFINE_FUNC3 (fm_results *, fm_erase_eeprom_pages,           int pages[], progressfunc progress, void *callbackparam);
DEFINE_FUNC1 (fm_results *, fm_read_eeprom_crc,              unsigned long page);
DEFINE_FUNC9 (fm_results *, fm_program_eeprom,               char *path, int checksums, int fill, unsigned char fillbyte, char *jitcmd, char *jitoptions, int jittimeout, progressfunc progress, void *callbackparam);
DEFINE_FUNC3 (void,         fm_set_autobaud_configuration,   unsigned long readtimeout, unsigned long writetimeout, int retries);
DEFINE_FUNC1 (fm_results *, fm_activate_flash_bank,          int flashbank);
DEFINE_FUNC3 (fm_results *, fm_program_eeprom_from_buffer,   unsigned char *buffer, unsigned long eepromaddress, unsigned long length);
DEFINE_FUNC3 (fm_results *, fm_read_eeprom_to_buffer,        unsigned char *buffer, unsigned long eepromaddress, unsigned long length);

#undef DLL_FUNC
#undef CSRC

#endif // _FMCOMMONH_

