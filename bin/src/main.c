#include <stdio.h>

#include "lpc17xx.h"
#include "type.h"
#include "timer.h"
#include "modbus.h"
#include "ld.h"
#include "uss.h"
#include "i2c.h"
#include "dac.h"

#define NORD_USS_ENABLE

#define QEI_CHECK
#ifdef QEI_CHECK
#include "qei.h"
#endif

#include "lwip/debug.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"

#include "netif/emac.h"
#include "netif/etharp.h"
#include "netif/loopif.h"
#include "netif/ethernetif.h"

#include "timer.h"

extern volatile unsigned int CYCLE_TIME;

extern volatile unsigned char MODBUS_MASTER;  // 0 = Slave, 1 = Master
volatile unsigned char WAITING_FOR_USS = 0;	// 
volatile unsigned char WAITING_FOR_YASKAWA = 0;	// 
volatile int * MODBUS_RETURN_VAL = NULL;

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

typedef struct ad_type
{
  unsigned int m[50];
} ads;

struct ad_type ad[5];

typedef struct ads_fator
{
  unsigned int tensao;
  unsigned int fator;
} ad_fator;

//-------------------------------   0V  --- 250mV ---  500mV --- 750mV ------- 1V ------ 1,25V ------ 1,5V ----- 2V ------- 3V ------- 4V -------- 5V ------ 6V ------- 7V ------ 8V ------- 9V -------- 10v ---
struct ads_fator ad0fator[16] = { {7, 0}, {37, 68}, {59, 85}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108} };
struct ads_fator ad1fator[16] = { {7, 0}, {37, 68}, {59, 88}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108} };
struct ads_fator ad2fator[16] = { {7, 0}, {37, 68}, {59, 85}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108} };
struct ads_fator ad3fator[16] = { {7, 0}, {37, 68}, {59, 83}, {81, 93}, {104, 96}, {126, 99}, {149, 101}, {196, 102}, {293, 102}, {383, 104}, {477, 105}, {571, 105}, {658, 106}, {758, 106}, {849, 106}, {926, 108} };

extern volatile unsigned int I2CCount;
extern volatile unsigned char I2CMasterBuffer[BUFSIZE];
extern volatile unsigned int I2CCmd, I2CMasterState;
extern volatile unsigned int I2CReadLength, I2CWriteLength;

extern volatile unsigned char IP_ADDR[4];
extern volatile unsigned char IP_MASK[4];
extern volatile unsigned char IP_GW[4];

#ifdef QEI_CHECK
/************************** PRIVATE MACROS *************************/
/** Signal Mode setting:
 * - When = 0, PhA and PhB function as quadrature encoder inputs.
 * - When = 1, PhA functions as the direction signal and PhB functions
 * as the clock signal
 */
#define SIGNAL_MODE             0

/** Capture Mode setting:
 * - When = 0, only PhA edges are counted (2X).
 * - When = 1, BOTH PhA and PhB edges are counted (4X), increasing
 * resolution but decreasing range
 */
#define CAP_MODE                        1

/** Velocity capture period definition (in microsecond) */
#define CAP_PERIOD                      250000UL

/** Delay time to Read Velocity Accumulator and display (in microsecond)*/
#define DISP_TIME                       3000000UL
/** Max velocity capture times calculated */
#define MAX_CAP_TIMES           (DISP_TIME/CAP_PERIOD)

#define ENC_RES                         2048UL  /**< Encoder resolution (PPR) */

volatile int ENC_VAL = 0;

#endif

/******************************************************************************
* Variaveis Globais
******************************************************************************/
unsigned char MAC_ADDRESS[6];

volatile unsigned int saidas = 0;
volatile unsigned int entradas = 0;

volatile unsigned int plccycle_timer = 0;
volatile unsigned int serial_timeout = 0;
volatile unsigned int uss_timeout = 0;
volatile unsigned int modbus_timeout = 0;

struct MB_Device modbus_master;
struct MB_Device modbus_slave;
struct MB_Device modbus_tcp;

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

unsigned char serial_rx_buffer[MB_BUFFER_SIZE];
unsigned char serial_tx_buffer[MB_BUFFER_SIZE];

unsigned int serial_rx_index = 0;
unsigned int serial_tx_index = 0;
unsigned int serial_tx_count = 0;

#ifdef QEI_CHECK
/************************** ENCODER *************************/
/** Position Counter */
__IO uint32_t PosCnt;
/** Velocity Accumulator */
__IO uint32_t VeloAcc;
/** Times of Velocity capture */
__IO uint32_t VeloCapCnt;
/** Flag indicates Times of Velocity capture is enough to read out */
__IO FlagStatus VeloAccFlag;
#endif

/******************************************************************************
* Prototipos de Funções
******************************************************************************/
unsigned int ModbusReadCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusTx(unsigned char *data, unsigned int size);

unsigned int ModbusRequest(unsigned char * buffer, unsigned int sz);

/******************************************************************************
* lwip TCP/IP
******************************************************************************/
#define TCP_RECONNECT_TIMEOUT 2000
#define TCP_MSG_TIMEOUT 30000

int Init_EMAC(void);
int Init_EMAC_phase2(void);

volatile unsigned int I_SerialReady;
volatile unsigned int I_USSReady;
volatile unsigned int I_ModbusReady;

volatile uint16_t gArpTimer = 0;
volatile uint16_t gTcpTimer = 0;
volatile uint16_t gTcpLastMessageTimer = 0;
volatile uint16_t gTcpReconnectTimer = 0;
volatile uint16_t gDhcpCoarseTimer = 0;
volatile uint16_t gDhcpFineTimer = 0;

int gUsingDhcp = 0;
int gReadersInitialized = 0;
int gMessagesInitialized = 0;
int gShowingDefaultScreen = 0;
int gRemotePort;

extern struct tcp_pcb * tcp_bound_pcbs;
struct tcp_pcb *tpcb_modbus_request;

char tcp_data_x[20];
static volatile enum { TCP_UNCONFIG=0, TCP_WAIT_LINK, TCP_WAIT_IP, TCP_DISCONNECTED, TCP_CONNECTING, TCP_JUST_CONNECTED, TCP_CONNECTED } gTcpState;

static struct netif *netif_eth0;

static struct ip_addr gMyIpAddress;
static struct ip_addr gMyNetmask;
static struct ip_addr gMyGateway;
static struct ip_addr gRemoteIpAddress;

static struct tcp_pcb *gTcp = NULL;

#ifdef QEI_CHECK
/**
 * @brief QEI interrupt handler. This sub-routine will update current
 *                      value of captured velocity in to velocity accumulate.
 * @param[in]   None
 * @return[in]  None
 */
void QEI_IRQHandler(void)
{
  //uint32_t tmp;

  // Check whether if velocity timer overflow
  if (QEI_GetIntStatus(QEI, QEI_INTFLAG_TIM_Int) == SET)
  {
    if (VeloAccFlag == RESET) {
      // Get current velocity captured and update to accumulate
      //VeloAcc += QEI_GetVelocityCap(QEI);
      //tmp = QEI_GetVelocityCap(QEI);
      //VeloAcc += tmp;
      //_DBH32(tmp); _DBG_("");

      // Update Velocity capture times
      VeloAccFlag = ((VeloCapCnt++) >= MAX_CAP_TIMES) ? SET : RESET;
    }
    // Reset Interrupt flag pending
    QEI_IntClear(QEI, QEI_INTFLAG_TIM_Int);
  }

  // Check whether if direction change occurred
  if (QEI_GetIntStatus(QEI, QEI_INTFLAG_DIR_Int) == SET) {
    // Reset Interrupt flag pending
    QEI_IntClear(QEI, QEI_INTFLAG_DIR_Int);
  }
}

int ENCRead(void)
{
  return ENC_VAL;
}

void ENCReset(void)
{
  VeloAccFlag = RESET;

  QEI->QEICON = 0xF;

  ENC_VAL = 0;

  PosCnt = 0;
  // Reset value of Acc and Acc count to default
  VeloAcc = 0;
  VeloCapCnt = 0;
}
#endif

void TIMER1_IRQHandler (void)
{
  TIM1 -> IR = 1;                       /* clear interrupt flag */

  gTcpTimer++;
  gTcpLastMessageTimer++;
  gDhcpCoarseTimer++;
  gDhcpFineTimer++;
  gArpTimer++;

  if (gTcpState == TCP_DISCONNECTED)
    gTcpReconnectTimer++;

}

void modbus_send(unsigned char id,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value)
{
  uint8_t out[] = { 0, 0 };

  struct MB_Device * mbdev;

  if (MODBUS_MASTER)
    mbdev = &modbus_master;
  else
    mbdev = &modbus_slave;

  MODBUS_RETURN_VAL = value;

  union MB_FCD_Data data;
  struct MB_Reply rp;

  mbdev->identification.Id = id;

  /* Enviando mensagem pelo Modbus */
  switch(fc)
  {
  case MB_FC_READ_COILS:
    data.read_coils.start = address;
    data.read_coils.quant = size;
    break;
  case MB_FC_READ_DISCRETE_INPUTS:
    data.read_discrete_inputs.start = address;
    data.read_discrete_inputs.quant = size;
    break;
  case MB_FC_READ_HOLDING_REGISTERS:
    data.read_holding_registers.start = address;
    data.read_holding_registers.quant = size;
    break;
  case MB_FC_READ_INPUT_REGISTERS:
    data.read_input_registers.start = address;
    data.read_input_registers.quant = size;
    break;
  case MB_FC_WRITE_SINGLE_COIL:
    data.write_single_coil.output = address;
    data.write_single_coil.val    = *value;
    break;
  case MB_FC_WRITE_SINGLE_REGISTER:
    data.write_single_register.address = address;
    data.write_single_register.val     = *value;
    break;
  case MB_FC_WRITE_MULTIPLE_COILS:
    data.write_multiple_coils.start = address;
    data.write_multiple_coils.quant = size;
    data.write_multiple_coils.size  = 2 * size;
    data.write_multiple_coils.val   = (unsigned char *)value;
    break;
  case MB_FC_WRITE_MULTIPLE_REGISTERS:
    data.write_multiple_registers.start = address;
    data.write_multiple_registers.quant = size;
    data.write_multiple_registers.size  = 2 * size;
    data.write_multiple_registers.val   = (unsigned char *)value;
    break;
  case MB_FC_MASK_WRITE_REGISTER:
    data.mask_write_register.address = address;
    data.mask_write_register.and     = 0; //and;
    data.mask_write_register.or      = 0; //or;
    break;
  case MB_FC_RW_MULTIPLE_REGISTERS:
    data.rw_multiple_registers.start_read  = address;
    data.rw_multiple_registers.quant_read  = size;
    data.rw_multiple_registers.start_write = data.rw_multiple_registers.start_read;
    data.rw_multiple_registers.quant_write = data.rw_multiple_registers.quant_read;
    data.rw_multiple_registers.size  = size;
    data.rw_multiple_registers.val   = out;
    out[0] = 0xaa;
    out[1] = 0x55;
    break;
  case MB_FC_READ_EXCEPTION_STATUS:
    break;
  case MB_FC_READ_DEVICE_IDENTIFICATION:
    data.read_device_identification.id_code   = 0x01;
    data.read_device_identification.object_id = 0x00;
    break;
  default:
    rp.FunctionCode = fc;
    rp.ExceptionCode = MB_EXCEPTION_ILLEGAL_FUNCTION;
	return;
  }

  rp = MB_Send(mbdev, fc, &data);

}

void modbus_tcp_send(unsigned char id,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value)
{
  uint8_t out[] = { 0, 0 };

  struct MB_Device * mbdev;

  if (MODBUS_MASTER)
    mbdev = &modbus_master;
  else
    mbdev = &modbus_slave;

  MODBUS_RETURN_VAL = value;

  union MB_FCD_Data data;
  struct MB_Reply rp;

  mbdev->identification.Id = id;

  /* Enviando mensagem pelo Modbus */
  switch(fc)
  {
  case MB_FC_READ_COILS:
    data.read_coils.start = address;
    data.read_coils.quant = size;
    break;
  case MB_FC_READ_DISCRETE_INPUTS:
    data.read_discrete_inputs.start = address;
    data.read_discrete_inputs.quant = size;
    break;
  case MB_FC_READ_HOLDING_REGISTERS:
    data.read_holding_registers.start = address;
    data.read_holding_registers.quant = size;
    break;
  case MB_FC_READ_INPUT_REGISTERS:
    data.read_input_registers.start = address;
    data.read_input_registers.quant = size;
    break;
  case MB_FC_WRITE_SINGLE_COIL:
    data.write_single_coil.output = address;
    data.write_single_coil.val    = *value;
    break;
  case MB_FC_WRITE_SINGLE_REGISTER:
    data.write_single_register.address = address;
    data.write_single_register.val     = *value;
    break;
  case MB_FC_WRITE_MULTIPLE_COILS:
    data.write_multiple_coils.start = address;
    data.write_multiple_coils.quant = size;
    data.write_multiple_coils.size  = 2 * size;
    data.write_multiple_coils.val   = (unsigned char *)value;
    break;
  case MB_FC_WRITE_MULTIPLE_REGISTERS:
    data.write_multiple_registers.start = address;
    data.write_multiple_registers.quant = size;
    data.write_multiple_registers.size  = 2 * size;
    data.write_multiple_registers.val   = (unsigned char *)value;
    break;
  case MB_FC_MASK_WRITE_REGISTER:
    data.mask_write_register.address = address;
    data.mask_write_register.and     = 0; //and;
    data.mask_write_register.or      = 0; //or;
    break;
  case MB_FC_RW_MULTIPLE_REGISTERS:
    data.rw_multiple_registers.start_read  = address;
    data.rw_multiple_registers.quant_read  = size;
    data.rw_multiple_registers.start_write = data.rw_multiple_registers.start_read;
    data.rw_multiple_registers.quant_write = data.rw_multiple_registers.quant_read;
    data.rw_multiple_registers.size  = size;
    data.rw_multiple_registers.val   = out;
    out[0] = 0xaa;
    out[1] = 0x55;
    break;
  case MB_FC_READ_EXCEPTION_STATUS:
    break;
  case MB_FC_READ_DEVICE_IDENTIFICATION:
    data.read_device_identification.id_code   = 0x01;
    data.read_device_identification.object_id = 0x00;
    break;
  default:
    rp.FunctionCode = fc;
    rp.ExceptionCode = MB_EXCEPTION_ILLEGAL_FUNCTION;
	return;
  }

  //rp = MB_Send(mbdev, fc, &data);

}

err_t modbus_tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  if (p != NULL)
  {
    tpcb_modbus_request = tpcb;

    if (p->len > 2)
      MB_Receive(&modbus_tcp, MB_Validate(p->payload, p->len));

    tcp_recved(tpcb, p->len);
    pbuf_free(p);
  }

  return ERR_OK;
}

err_t modbus_tcp_poll_callback(void *arg, struct tcp_pcb *tpcb)
{
        if (arg != NULL)
        {
                u32_t bytestosend = *((u32_t*) arg);

                if (bytestosend == 0)
                {
                        mem_free(arg);
                        tcp_arg(tpcb, NULL);
                        //tcp_close(tpcb);
                }
        }

        return ERR_OK;
}

err_t modbus_tcp_send_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
        u32_t bytestosend;

        if (arg != NULL)
        {
                bytestosend = *((u32_t*) arg);

                if (bytestosend > len)
                        bytestosend -= len;
                else
                        bytestosend = 0;

                *((u32_t*) arg) = bytestosend;
        }

        return ERR_OK;
}

err_t modbus_tcp_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  tcp_arg(newpcb, NULL);
  tcp_recv(newpcb, modbus_tcp_recv_callback);
  tcp_sent(newpcb, modbus_tcp_send_callback);
  tcp_poll(newpcb, modbus_tcp_poll_callback, 1);

  return ERR_OK;
}

static void modbus_tcp_init(void)
{
  struct tcp_pcb* tcpweb;
  struct tcp_pcb* tcpweb_listen;

  tcpweb = tcp_new();
  if (tcpweb == NULL)
    return;

  // Bind to port 502 modbus default
  if (tcp_bind(tcpweb, IP_ADDR_ANY, 502) != ERR_OK)
    return;

  tcpweb_listen = tcp_listen(tcpweb);
  if (tcpweb_listen == NULL)
  {
    tcp_abort(tcpweb);
    tcpweb = NULL;
    return;
  }

  tcpweb = tcpweb_listen;

  tcp_accept(tcpweb, modbus_tcp_accept_callback);
}

void init_network()
{
  /* Clear globals. */
  tcp_listen_pcbs.listen_pcbs = NULL;
  tcp_active_pcbs = NULL;
  tcp_bound_pcbs = NULL;
  tcp_tw_pcbs = NULL;
  tcp_tmp_pcb = NULL;

  Init_EMAC();

  netif_list = NULL;

  lwip_init();

  netif_eth0 = mem_malloc(sizeof(struct netif));

  modbus_tcp_init();

  gTcpState = TCP_UNCONFIG;

}

void send(unsigned char message_type, unsigned char data[], int data_size)
{
  if (!(gTcpState == TCP_CONNECTED || (gTcpState == TCP_JUST_CONNECTED))) {
  return;
  }

  u16_t len = 0;// = data_size + 1;

  if (len > tcp_sndbuf(gTcp))
  {
    //DEBUG("Not enough buffer left to send data. Discarding\n");
    return;
  }

  if (data)
    memcpy(&tcp_data_x[1], data, data_size);

  tcp_data_x[0] = message_type;

  err_t err = tcp_write(gTcp, tcp_data_x, len, 1);
  if (err != ERR_OK)
  {
    //DEBUG("TCP: Error sending data... (%d)\n", err);
    return;
  }
  else
  {
    u32_t* newarg;
    newarg = (u32_t*) mem_malloc(sizeof(u32_t));
    *newarg = len;
    tcp_arg(gTcp, newarg);
  }
  err = tcp_output(gTcp);

  //if (err != ERR_OK)
    //DEBUG("TCP: Error forcing data send... (%d)\n", err);
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  unsigned char* rq;
  int consumed, expected_size;

  gTcpLastMessageTimer = 0;

  if (p != NULL)
  {
    rq = p->payload;
    consumed = 0;
    while (consumed < p->len)
    {
      //unsigned char msg_type = rq[consumed];
      /*if (msg_type == MSG_SHOW_TEXT || msg_type == MSG_CONFIG_TXT_SCREEN)
        expected_size = 42;
      else
        expected_size = 9;*/

      if (expected_size > (p->len - consumed))
        expected_size = p->len - consumed;

      //process_received_message(msg_type, rq+1+consumed, expected_size-1, 1);
      consumed += expected_size;
    }
    tcp_recved(tpcb, p->len);
    pbuf_free(p);
  }
  else
  {
    //DEBUG("TCP connection closed by remote host?\n");
    gTcpState = TCP_DISCONNECTED;
  }

  return ERR_OK;
}

err_t poll_callback(void *arg, struct tcp_pcb *tpcb)
{
  if (arg != NULL)
  {
    u32_t bytestosend = *((u32_t*) arg);
    //DEBUG("poll_callback bytestosend=%u\n", bytestosend);

    if (bytestosend == 0)
    {
      mem_free(arg);
      tcp_arg(tpcb, NULL);
    }
  }

  return ERR_OK;
}

err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  u32_t bytestosend;

  if (arg != NULL)
  {
    bytestosend = *((u32_t*) arg);
    //DEBUG("sent_callback bytestosend=%u len=%d\n", bytestosend, len);

    if (bytestosend <= len)
    {
      bytestosend = 0;
    }
    else
    {
      bytestosend -= len;
    }

    *((u32_t*) arg) = bytestosend;
  }

  return ERR_OK;
}

err_t connect_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  tcp_arg(newpcb, NULL);

  if (err == ERR_OK)
  {
    //DEBUG("TCP Connected to remote host\n");
    tcp_recv(newpcb, recv_callback);
    tcp_sent(newpcb, sent_callback);
    tcp_poll(newpcb, poll_callback, 1);
    gTcp = newpcb;
    //DEBUG("Setting TCP state to TCP_JUST_CONNECTED\n");
    gTcpState = TCP_JUST_CONNECTED;
  }
  else
  {
    //DEBUG("Error connecting to remote host: %d\n", err);
  }
  return ERR_OK;
}

void error(void *arg, err_t err)
{
  //DEBUG("TCP Error: %d\n", err);
  if (gTcpState !=  TCP_DISCONNECTED)
  {
    //DEBUG("Setting TCP state to TCP_DISCONNECTED\n");

    if (gTcpState != TCP_CONNECTING)
      gTcpReconnectTimer = TCP_RECONNECT_TIMEOUT;

    gTcpState = TCP_DISCONNECTED;
  }
}

void connect()
{
  struct tcp_pcb* tcp_client;
  err_t err;
  tcp_client = tcp_new();

  if (tcp_client == NULL)
  {
    //DEBUG("Failed to allocate tcp_client\n");
    return;
  }

  tcp_err(tcp_client, error);

  //DEBUG("gRemotePort=%d\n", gRemotePort);
  err = tcp_connect(tcp_client, &gRemoteIpAddress, gRemotePort, connect_callback);
  gTcpState = TCP_CONNECTING;
}

void check_network(void)
{
  if (gTcpState == TCP_UNCONFIG)
  {
    if (gTcpTimer > 200)
    {
      if (Init_EMAC_phase2())
      {
        gTcpState = TCP_WAIT_IP;
        netif_add(netif_eth0, &gMyIpAddress, &gMyNetmask, &gMyGateway, NULL, ethernetif_init, ethernet_input);
        netif_set_default(netif_eth0);
        netif_set_up(netif_eth0);
        //DEBUG("Eth0 Link UP!\n");
      }
    gTcpTimer = 0;
    }
  }

  if (gTcpState == TCP_WAIT_IP && gUsingDhcp && netif_eth0->ip_addr.addr == 0)
  {
    /*if (gDhcpFineTimer >= DHCP_FINE_TIMER_MSECS)
    {
      dhcp_fine_tmr();
      gDhcpFineTimer = 0;
      if (gDhcpCoarseTimer >= DHCP_COARSE_TIMER_SECS*1000)
      {
        dhcp_coarse_tmr();
        gDhcpCoarseTimer = 0;
      }
    }*/
  }

  if (gTcpState == TCP_WAIT_IP && netif_eth0->ip_addr.addr)
  {
    //DEBUG("Local IP is %u.%u.%u.%u\n", netif_eth0->ip_addr.addr & 0xFF, netif_eth0->ip_addr.addr >> 8 & 0xFF, netif_eth0->ip_addr.addr >> 16 & 0xFF, netif_eth0->ip_addr.addr >> 24 & 0xFF);
    //              DEBUG("netmask was set to 0x%x\n", netif_eth0.netmask.addr);
    //              DEBUG("gw was set to 0x%x\n", netif_eth0.gw.addr);
    //              DEBUG("Setting TCP state to TCP_DISCONNECTED\n");
    gTcpState = TCP_DISCONNECTED;
    gTcpReconnectTimer = TCP_RECONNECT_TIMEOUT;
    gTcpTimer = 0;
  }

  if (gArpTimer >= ETHARP_TMR_INTERVAL)
  {
    etharp_tmr();
    gArpTimer = 0;
  }

  if (netif_eth0->ip_addr.addr)
  {
    if (gTcpTimer >= TCP_TMR_INTERVAL)
    {
    tcp_tmr();
    gTcpTimer = 0;
    }
  }

  switch (gTcpState)
  {
  case TCP_DISCONNECTED:
    if (gTcpReconnectTimer >= TCP_RECONNECT_TIMEOUT)
    {
      //DEBUG("TCP state is TCP_DISCONNECTED. Trying to connect\n");
      //connect();
      gTcpReconnectTimer = 0;
    }
    #ifdef ENABLE_TCP_FILES
    if (!once)
    {
      tcp_db_create_and_listen();
      tcp_config_create_and_listen();
      once = 1;
    }
    #endif
    break;
  case TCP_JUST_CONNECTED:
    gTcpState = TCP_CONNECTED;
    gTcpLastMessageTimer = 0;
    break;
  case TCP_CONNECTED:
    if (gTcpLastMessageTimer >= TCP_MSG_TIMEOUT)
    {
      tcp_err(gTcp, NULL);
      tcp_recv(gTcp, NULL);
      tcp_abort(gTcp);
      gTcp = NULL;
      gTcpState = TCP_DISCONNECTED;
      gTcpLastMessageTimer = 0;
    }
    break;
  default:
    break;
  }
}

/******************************************************************************
* Temporizador
******************************************************************************/
void TIMER0_IRQHandler (void)
{
	unsigned int sz;

	TIM0->IR = 1;                       /* clear interrupt flag */

	serial_timeout++;
	//uss_timeout++;
	modbus_timeout++;

	plccycle_timer++;

	if (WAITING_FOR_YASKAWA)
	{
		if (serial_tx_index < serial_tx_count)
		{
			RS485Write(serial_tx_buffer + serial_tx_index++, 1);
		}
	}

	if (serial_timeout > 10)
	{
		sz = RS485Read(serial_rx_buffer + serial_rx_index, sizeof(serial_rx_buffer) - serial_rx_index);
		serial_rx_index += sz;
		serial_timeout = 0;

		if (serial_rx_index && !sz)
		{
			modbus_timeout = 0;

			if (WAITING_FOR_USS == 1) // uss
			{
				uss_ready((PPO1*)serial_rx_buffer, serial_rx_index);
				serial_rx_index = 0;
				WAITING_FOR_USS = 0;
			} 
			else if (WAITING_FOR_YASKAWA == 0) // modbus
			{
				ModbusRequest(serial_rx_buffer, serial_rx_index);
				serial_rx_index = 0;
			}
		}
	}
  
 // if (!I_SerialReady && serial_rx_index)
 // {
 //   uss_ready((PPO1*)serial_rx_buffer, serial_rx_index);
 //   serial_rx_index = 0;
 // }

  if (modbus_timeout > 1000)
  {
    I_SerialReady = 1;
	WAITING_FOR_USS = 0;
	WAITING_FOR_YASKAWA = 0;
	modbus_timeout = 0;
  } 

}

/******************************************************************************
* ADC Read
******************************************************************************/
#define ARRAY_SIZEOF(a) (sizeof(a)/sizeof(a[0]))

unsigned int ADCRead(unsigned int a)
{
  unsigned int max = 0;
  unsigned int min = 0;
  unsigned int soma = 0;
  unsigned int i = 0;
  unsigned int z = 0;

  unsigned int val = 0;
  unsigned int tensao = 0;
  unsigned char set = 0;

  if (a < 1 || a > 6) return 0;

  a -= 1;

  switch (a)
  {
  case 0:
    if (ADC->ADDR5 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR5 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

	  for (i = 1; i < ARRAY_SIZEOF(ad1fator); i++)
	  {
		  if (tensao < ad0fator[i].tensao)
		  {
			  tensao *= ad0fator[i - 1].fator;
			  tensao /= 100;
			  break;
		  }
	  }

	  val = tensao;
    }
    break;
  case 1:
    if (ADC->ADDR2 & 0x7FFFFFFF)
    {
      val = 0xFFF & (ADC->ADDR2 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

	  for (i = 1; i < ARRAY_SIZEOF(ad1fator); i++)
	  {
		  if (tensao < ad1fator[i].tensao)
		  {
			  tensao *= ad1fator[i - 1].fator;
			  tensao /= 100;
			  break;
		  }
	  }

	  val = tensao;
    }
    break;
  case 2:
    if (ADC->ADDR1 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR1 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

	  for (i = 1; i < ARRAY_SIZEOF(ad2fator); i++)
	  {
		  if (tensao < ad2fator[i].tensao)
		  {
			  tensao *= ad2fator[i - 1].fator;
			  tensao /= 100;
			  break;
		  }
	  }

	  val = tensao;
	}
    break;
  case 3:
    if (ADC->ADDR0 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR0 >> 4);
      set = 1;

	  tensao = (val * 1000) / 4096;	// 2 casas decimais, ex: 1,04V

	  for (i = 1; i < ARRAY_SIZEOF(ad3fator); i++)
	  {
		  if (tensao < ad3fator[i].tensao)
		  {
			  tensao *= ad3fator[i - 1].fator;
			  tensao /= 100;
			  break;
		  }
	  }

	  val = tensao;
	}
    break;
  case 4:  // AD CPU
    if (ADC->ADDR4 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR4 >> 4);
      set = 1;
	  
	  tensao = (val * 1000) / 4096; // 2 casas decimais, ex: 2,05V

	  tensao *= 48;	// fator de correção
  	  tensao /= 100;

	  val = tensao;
    }
    break;
  case 5:  // SENSOR DE TEMPERATURA
    if (ADC->ADDR5 & 0x7FFFFFFF)
    {
      val = 0x7FFF & (ADC->ADDR5 >> 4);
      set = 1;
	  
	  tensao = (val * 1000) / 4096; // 2 casas decimais, ex: 2,05V

	  tensao *= 48;	// fator de correção
  	  tensao /= 100;

	  val = tensao;
    }
    break;
  }

  soma = 0;
  z = 0;

  if (set == 1)
  {
    for (i = 0; i < ARRAY_SIZEOF(ad[a].m) - 1; i++)
      ad[a].m[i] = ad[a].m[i + 1];

    ad[a].m[ARRAY_SIZEOF(ad[0].m) - 1] = val;

    for (i = 0; i < ARRAY_SIZEOF(ad[a].m); i++)
      if (ad[a].m[max] < ad[a].m[i])
        max = i;

    for (i = 0; i < ARRAY_SIZEOF(ad[a].m); i++)
      if (ad[a].m[min] > ad[a].m[i])
        min = i;

	if (min == max)
	{
		soma = min;
		z = 1;
	}
	else
	{
		for (i = 0; i < ARRAY_SIZEOF(ad[a].m); i++)
		{
		  if (i != min && i != max)
		  {
			  if (ad[a].m[i])
			  {
				soma += ad[a].m[i];
				z++;
			  }
		  }
		}
	}
  }

  return z > 0 ? (unsigned int)(soma / z) : 0;
}

int write_formatted_string(char *format, int val)
{
	int sz = 0;
	char msg[128];

	sprintf((void*)msg, format, val);

	I_SerialReady = 1;

	sz = RS485Write((unsigned char*)msg, strlen(msg));

	return sz;
}

int read_formatted_string(char *format, int *val)
{
	int sz = 0;
	char msg[128];
	char cmp[128];

	*val = 0;
	memset(msg, 0, sizeof(msg));

	sz = RS485Read((unsigned char*)msg, sizeof(msg));

	if (sz == 0)
		*val = -2;
	else
	{
		memset(cmp, 0, sizeof(msg));

		//sprintf((void*)msg, format, val);
		if (sscanf(msg, format, val) != 1)
			*val = -1;
		else
		{
			sprintf(cmp, format, val);

			if (strncmp(cmp, msg, sz) != 0)
				*val = -1;		
		}
	}

	I_SerialReady = 1;

	return sz;
}
int write_servo_yaskawa(char * id, char *format, int * val)
{
	int sz = 0;
	char msg[128];

	if (WAITING_FOR_YASKAWA == 0)
	{
		if (val == 0)
			return -4;

		if (id == 0 || format == 0)
		{
			*val = -4;  // formato invalido
			return 0;
		}

		memset(msg, 0, sizeof(msg));

		strcpy(msg, id);
		sprintf((void*)(&msg[0] + strlen(id)), format, val);
		strcat(msg, "\r\n");

		WAITING_FOR_YASKAWA = 1;

		serial_tx_count = 0;
		serial_tx_index = 0;

		//sz = RS485Write((unsigned char*)msg, strlen(msg));
		memset(serial_rx_buffer, 0, sizeof(serial_rx_buffer));
		memcpy(serial_tx_buffer, msg, strlen(msg));

		serial_rx_index = 0;
		serial_tx_count = strlen(msg);

		I_SerialReady = 1;

	}

	return sz;
}

int read_servo_yaskawa(char * id, char *format, int *val)
{
	int sz = 0;
	char msg[128];
	char cmp[128];
	char * pmsg = &msg[0];

	if (val == 0)
		return -4; // parametros invalidos

	if (id == 0 || format == 0)
	{
		*val = -4; // parametros invalidos
		return 0;
	}

	if (serial_rx_index == 0)
	{
		*val = -2; // sem resposta
		return 0;
	}

	*val = 0;
	memset(msg, 0, sizeof(msg));

	sz = serial_rx_index;
	memcpy(msg, serial_rx_buffer, sz);

	if (sz <= serial_tx_count)
	{
		*val = -5; // resposta invalida
		return sz;
	}

	pmsg += serial_tx_count;

	if (strncmp(pmsg, id, strlen(id)) != 0)
		*val = -3; // id diferente
	else
	{
		memset(cmp, 0, sizeof(cmp));

		if (sscanf(pmsg, format, val) != 1)
			*val = 0;  // nenhum valor

		// compara se diferente do formato
		strcpy(cmp, id);
		sprintf((void*)(&cmp[0] + strlen(id)), format, val);
		//strcat(cmp, "\r\n");

		if (strncmp(cmp, pmsg, strlen(cmp)) != 0)
			*val = -1;  // formato invalido
	}

	I_SerialReady = 1;
	WAITING_FOR_YASKAWA = 0;
	serial_rx_index = 0;
	memset(serial_rx_buffer, 0, sizeof(serial_rx_buffer));

	return sz;
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

  //UART3->FCR = 0x02;                      /* Reset RX */
  //UART3->FCR = 0x04;                      /* Reset TX */

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
unsigned int ModbusRequest(unsigned char * buffer, unsigned int sz)
{
  struct MB_Reply r;

  //memset(serial_rx_buffer, 0, sizeof(serial_rx_buffer));
  //memset(serial_tx_buffer, 0, sizeof(serial_tx_buffer));

  if (MODBUS_MASTER)
  {
    r = MB_ReceiveReply(&modbus_master, MB_Validate(buffer, sz));
    if (r.ExceptionCode == MB_EXCEPTION_NONE)
    {
      I_SerialReady = 1;
      if (MODBUS_RETURN_VAL == NULL)
        return 0;

      switch (r.FunctionCode)
      {
      case MB_FC_READ_COILS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.read_coils.data, r.reply.read_coils.size);
        break;

      case MB_FC_READ_DISCRETE_INPUTS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.read_discrete_inputs.data, r.reply.read_discrete_inputs.size);
        break;

      case MB_FC_READ_HOLDING_REGISTERS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.read_holding_registers.data, r.reply.read_holding_registers.size);
        break;

      case MB_FC_READ_INPUT_REGISTERS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.read_input_registers.data, r.reply.read_input_registers.size);
        break;

      case MB_FC_RW_MULTIPLE_REGISTERS:
        memcpy((void*)MODBUS_RETURN_VAL, r.reply.rw_multiple_registers.data, r.reply.rw_multiple_registers.size);
        break;
      };
    }
  }
  else
  {
    if (MB_Receive(&modbus_slave, MB_Validate(buffer, sz)) == MB_EXCEPTION_NONE)
      I_SerialReady = 1;
  }

  return 0; // Retorna o tamanho dos dados
}

unsigned int ModbusRS485Tx(unsigned char *data, unsigned int size)
{
  unsigned int sz = 0;

  sz = RS485Write(data, size);

  modbus_timeout = 0;

  I_SerialReady = 0;

  //return sz;
  return 0;
}

unsigned int ModbusEthTx(unsigned char *data, unsigned int size)
{
  if (tcp_write(tpcb_modbus_request, data, size, 1) != ERR_OK)
  {
//    tcp_close(tpcb_modbus_request);
    return 0;
  }

  return size;
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

  return MB_EXCEPTION_NONE;

}

unsigned int ModbusReadDiscreteInputs(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  uint32_t i;
  uint8_t *buf = reply->reply.read_discrete_inputs.data; // Retorna no maximo 8 bytes ou 256 bits (entradas).

  for(i = 0; i < data->read_discrete_inputs.quant; i++)
    buf[i / 8] |= ((entradas >> (data->read_discrete_inputs.start + i)) & 1) << (i % 8);

  reply->reply.read_discrete_inputs.size = data->read_discrete_inputs.quant / 8 + (data->read_discrete_inputs.quant % 8 != 0);

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusReadHoldingRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
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
      return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  }*/

  if (data->read_holding_registers.start + data->read_holding_registers.quant >= 32)
	  return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  else
	  memcpy(reply->reply.read_holding_registers.data, (void *)&M[data->read_holding_registers.start], data->read_holding_registers.quant * 2);

  reply->reply.read_holding_registers.size = data->read_holding_registers.quant * 2;

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
  if (data->write_single_register.address < 32)
    M[data->write_single_register.address] = data->write_single_register.val;
  else
    return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.write_single_register = data->write_single_register;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusWriteMultipleCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  reply->reply.write_multiple_coils  = data->write_multiple_coils;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusWriteMultipleRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
	int i, pos;

  /*if (data->write_multiple_registers.start + data->write_multiple_registers.quant >= 32)
	  return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  else
	  memcpy((void*)&M[data->write_multiple_registers.start], data->write_multiple_registers.val, data->write_multiple_registers.size);*/

  if (data->write_multiple_registers.start + data->write_multiple_registers.quant < 32)
  {
    for (i = 0; i < data->write_multiple_registers.size; i += 2)
    {
      pos = data->write_multiple_registers.start + (i / 2);
      M[pos] = *(unsigned short *)(&data->write_multiple_registers.val[i]);
    }
//      data->write_single_register.val[i] = M[data->write_multiple_registers.address + (i] ;
  } else
    return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.write_multiple_registers = data->write_multiple_registers;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusMaskWriteRegister(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  reply->reply.mask_write_register = data->mask_write_register;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusRWMultipleRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  reply->reply.read_holding_registers.size = 0;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusReadExceptionStatus(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  reply->reply.read_exception_status.status = 0x00;

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
* Comunicação Protocolo USS (Inversores Nord)
******************************************************************************/
void cmd_uss_send(char * args[], void * vars, unsigned int args_size)
{
  unsigned short int parametro = 0;
  unsigned char indice = 0;
  unsigned short int valor = 0;

  if (args_size < 3)
    return;

  parametro = atoi(args[0]); //console_decode_num(in, &console_offset, size);
  indice = atoi(args[1]); //console_decode_num(in, &console_offset, size);
  valor = atoi(args[2]); //console_decode_num(in, &console_offset, size);
}

void cmd_uss_recv(char * args[], void * vars, unsigned int args_size)
{
  unsigned short int parametro = 0;
  unsigned char indice = 0;
  //unsigned short int valor = 0;

  if (args_size < 2)
    return;

  parametro = atoi(args[0]); //console_decode_num(in, &console_offset, size);
  indice = atoi(args[1]); //console_decode_num(in, &console_offset, size);

}

/******************************************************************************
* Rotinas de atualização das Entradas/Saidas
******************************************************************************/
unsigned int AtualizaSaidas(void)
{
  unsigned int i = 0;
  unsigned int status = 0;
  unsigned char cmd = OUTPUT_CMD_CONTROL;

  i |= U1;
  i |= U2 << 1;
  i |= U3 << 2;
  i |= U4 << 3;
  i |= U5 << 4;
  i |= U6 << 5;
  i |= U7 << 6;
  i |= U8 << 7;
  i |= U9 << 8;
  i |= U10 << 9;
  i |= U11 << 10;
  i |= U12 << 11;
  i |= U13 << 12;
  i |= U14 << 13;
  i |= U15 << 14;
  i |= U16 << 15;

  i = HTONS(i);

  i &= 0xFFFF;

  GPIO0->FIOCLR = 0x10000;
  SSPWrite((unsigned char*)&cmd, 1);
  SSPWrite((unsigned char*)&i, 2);
  GPIO0->FIOSET = 0x10000;

  SSPRead((unsigned char*)&status, 3);

  // U17 - ERRO
  // U18 - CPU
  GPIO1->FIOPIN |= 0x3 << 21;
  GPIO1->FIOPIN &= ~(((U17 & 0x1) << 21) | ((U18 & 0x1) << 22));

  return i;
}

unsigned int AtualizaEntradas(void)
{
  unsigned int i = 0;

  i |= 0x3FF & GPIO2->FIOPIN;              // P2.0 ~ P2.9    [00..10]
  i |= (0x3800 & GPIO2->FIOPIN) >> 1;      // P2.11 ~ P2.13  [11..13]
  i |= (0x30 & GPIO0->FIOPIN) << 9;        // P0.4 ~ P0.5    [14..15]
  i |= (0x80000 & GPIO0->FIOPIN) >> 4;     // P0.19 ~ P0.19  [16..16]
  i |= (0x600000 & GPIO0->FIOPIN) >> 5;    // P0.21 ~ P0.22  [17..18]
  i |= (0x20000000 & GPIO0->FIOPIN) >> 11; // P0.29          [19]

  i = ~i;

  I1 = i & 0x1;
  I2 = (i & (1 << 1)) >> 1;
  I3 = (i & (1 << 2)) >> 2;
  I4 = (i & (1 << 3)) >> 3;
  I5 = (i & (1 << 4)) >> 4;
  I6 = (i & (1 << 5)) >> 5;
  I7 = (i & (1 << 6)) >> 6;
  I8 = (i & (1 << 7)) >> 7;
  I9 = (i & (1 << 8)) >> 8;
  I10 = (i & (1 << 9)) >> 9;
  I11 = (i & (1 << 10)) >> 10;
  I12 = (i & (1 << 11)) >> 11;
  I13 = (i & (1 << 12)) >> 12;
  I14 = (i & (1 << 13)) >> 13;
  I15 = (i & (1 << 14)) >> 14;
  I16 = (i & (1 << 15)) >> 15;
  I17 = (i & (1 << 16)) >> 16;
  I18 = (i & (1 << 17)) >> 17;
  I19 = (i & (1 << 18)) >> 18;

  return i;
}

void RS485Config(int baudrate, int bits, int parity, int stopbit)
{
	UART3->LCR = 0;

	switch(bits)
	{
	case 5:
		break;
	case 6:
		UART3->LCR |= 0x1;
		break;
	case 7:
		UART3->LCR |= 0x2;
		break;
	default:  // 8 bits
		UART3->LCR |= 0x3;
	}

	if (stopbit == 2)
		UART3->LCR |= 0x4;

	switch(parity)
	{
	case 1: // odd
		UART3->LCR |= 0x8;
		break;
	case 2: // even
		UART3->LCR |= 0x18;
		break;
	}

	if (baudrate == 19200)
	{
		UART3->DLM = 0;
		UART3->DLL = 46;
		UART3->FDR = (13 << 4) | 10;
	}
	else if (baudrate == 14400)
	{
		UART3->DLM = 0;
		UART3->DLL = 92;
		UART3->FDR = (6 << 4) | 1;
	}
	else  // 9600
	{
		UART3->DLM = 0;
		UART3->DLL = 92;
		UART3->FDR = (13 << 4) | 10;
	}

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
#ifndef NORD_USS_ENABLE
  UART3->LCR = 0x83;          /* 8 bits, no parity, 1 stop bit */
#else
  UART3->LCR = 0x9B;          /* 8 bits, even-parity, 1 stop bit */
#endif

  // 9600
  UART3->DLM = 0;
  UART3->DLL = 92;
  UART3->FDR = (13 << 4) | 10;

  // 9091
  //UART3->DLM = 0;
  //UART3->DLL = 125;
  //UART3->FDR = (8 << 4) | 3;

  // 9259
  //UART3->DLM = 0;
  //UART3->DLL = 135;
  //UART3->FDR = (4 << 4) | 1;

#ifndef NORD_USS_ENABLE
  UART3->LCR = 0x03;          /* DLAB = 0, 8 bits, no parity, 1 stop bit */
#else
  UART3->LCR = 0x1B;          /* DLAB = 0, 8 bits, even-parity, 1 stop bit */
#endif
  UART3->FCR = 0x07;          /* Enable and reset TX and RX FIFO. */

  //rs485_open(3, 19200, 8, 1, 1);

  // Pino de direcao da RS485
  PINCON->PINSEL1 &= ~(0x3 << 8);       // P0.20
  GPIO0->FIODIR |= RS485_ENABLE;        // P0.20 output

  // Entradas
  PINCON->PINSEL0 &= ~0xF00;
  PINCON->PINSEL1 &= ~0x3CC0;
  PINCON->PINSEL4 &= ~0xFCFFFFF;
  GPIO2->FIODIR &= ~0x3BFF;
  GPIO0->FIODIR &= ~0x680030;

  // Saidas Led Erro/CPU
  PINCON->PINSEL3 &= ~0x3c00;
  GPIO1->FIODIR |= 0x3 << 21;

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

  // Reset AD´s
  memset(ad, 0, sizeof(ad));

  I2CInit( (unsigned int)I2CMASTER );

  ld_Init();
}

void GetMACAddress(unsigned char * addr)
{
  memset((void *)I2CMasterBuffer, 0, BUFSIZE);

  I2CWriteLength = 2;
  I2CReadLength = 0;
  I2CMasterBuffer[0] = E2PROM_ADDR | E2PROM_CMD_WRITE;
  I2CMasterBuffer[1] = 0xFA;
  I2CEngine();

  I2CWriteLength = 1;
  I2CReadLength = 6;
  I2CMasterBuffer[0] = E2PROM_ADDR | E2PROM_CMD_READ;
  I2CEngine();

  memcpy(addr, (void*)&I2CMasterBuffer[3], 6);
}

/***************************************************************************/
/*  main                                                                   */
/***************************************************************************/
int main (void)
{
    //unsigned int sz = 0;

#ifdef QEI_CHECK
  QEI_CFG_Type QEIConfig;
  //QEI_RELOADCFG_Type ReloadConfig;
  //uint32_t rpm, averageVelo;
#endif

  /**************************************************************************
   * Inicializa System
   *************************************************************************/
  SystemInit();
  HardwareInit();

   /**************************************************************************
   * Inicializa Encoder
   *************************************************************************/
#ifdef QEI_CHECK
  /* Configure the NVIC Preemption Priority Bits:
   * two (2) bits of preemption priority, six (6) bits of sub-priority.
   * Since the Number of Bits used for Priority Levels is five (5), so the
   * actual bit number of sub-priority is three (3)
   */
  //NVIC_SetPriorityGrouping(0x05);

  /* Initialize QEI configuration structure to default value */
  #if CAP_MODE
    QEIConfig.CaptureMode = QEI_CAPMODE_4X;
  #else
    QEIConfig.CaptureMode = QEI_CAPMODE_2X;
  #endif
  QEIConfig.DirectionInvert = QEI_DIRINV_NONE;
  QEIConfig.InvertIndex = QEI_INVINX_NONE;
  #if SIGNAL_MODE
    QEIConfig.SignalMode = QEI_SIGNALMODE_CLKDIR;
  #else
    QEIConfig.SignalMode = QEI_SIGNALMODE_QUAD;
  #endif

  /* Initialize QEI peripheral with given configuration structure */
  QEI_Init(QEI, &QEIConfig);

  // Set timer reload value for  QEI that used to set velocity capture period
  //ReloadConfig.ReloadOption = QEI_TIMERRELOAD_USVAL;
  //ReloadConfig.ReloadValue = CAP_PERIOD;
  //QEI_SetTimerReload(QEI, &ReloadConfig);

  // Set Max Position
  QEI_SetMaxPosition(QEI, 0xffffffff);

  /* preemption = 1, sub-priority = 1 */
  //NVIC_SetPriority(QEI_IRQn, ((0x01<<3)|0x01));

  // Reset VeloAccFlag
  //VeloAccFlag = RESET;
  // Reset value of Acc and Acc count to default
  //VeloAcc = 0;
  //VeloCapCnt = 0;

  // Enable interrupt for velocity Timer overflow for capture velocity into Acc */
  //QEI_IntCmd(QEI, QEI_INTFLAG_TIM_Int, ENABLE);
  // Enable interrupt for direction change */
  //QEI_IntCmd(QEI, QEI_INTFLAG_DIR_Int, ENABLE);

  /* Enable interrupt for QEI  */
  //NVIC_EnableIRQ(QEI_IRQn);
#endif

  /**************************************************************************
   * Inicializa timer
   *************************************************************************/
  init_timer( 0, TIME_INTERVAL ); // PlcCycle

  I_SerialReady = 1;

  enable_timer( 0 );

  /**************************************************************************
   * Inicialização do ModBus
   *************************************************************************/
  MB_Init(&modbus_master);

  modbus_master.identification.Id                 = 0x01;
  modbus_master.identification.VendorName         = "Tecnequip";
  modbus_master.identification.ProductCode        = "POP7";
  modbus_master.identification.MajorMinorRevision = "V1 Rev2";

  modbus_master.hl      = ModbusHandlers;
  modbus_master.hl_size = ARRAY_SIZE(ModbusHandlers);
  modbus_master.mode    = MB_MODE_MASTER;
  modbus_master.TX      = ModbusRS485Tx;

  MB_Init(&modbus_slave);

  modbus_slave.identification.Id                 = 0x01;
  modbus_slave.identification.VendorName         = "Tecnequip";
  modbus_slave.identification.ProductCode        = "POP7";
  modbus_slave.identification.MajorMinorRevision = "V1 Rev2";

  modbus_slave.hl      = ModbusHandlers;
  modbus_slave.hl_size = ARRAY_SIZE(ModbusHandlers);
  modbus_slave.mode    = MB_MODE_SLAVE;
  modbus_slave.TX      = ModbusRS485Tx;

  MB_Init(&modbus_tcp);

  modbus_tcp.identification.Id                 = 0x01;
  modbus_tcp.identification.VendorName         = "Tecnequip";
  modbus_tcp.identification.ProductCode        = "POP7";
  modbus_tcp.identification.MajorMinorRevision = "V1 Rev2";

  modbus_tcp.hl      = ModbusHandlers;
  modbus_tcp.hl_size = ARRAY_SIZE(ModbusHandlers);
  modbus_tcp.mode    = MB_MODE_SLAVE;
  modbus_tcp.TX      = ModbusEthTx;

  memset((void*)M, 0, sizeof(M));

  gTcpState = TCP_WAIT_LINK;

  // Microchip 0x00:0x04:0xa3:0x??:0x??:0x?? (ipv4)
  MAC_ADDRESS[0] = 0x00;
  MAC_ADDRESS[1] = 0x04;
  MAC_ADDRESS[2] = 0xa3;
  MAC_ADDRESS[3] = 0x00;
  MAC_ADDRESS[4] = 0x00;
  MAC_ADDRESS[5] = 0x00;

  GetMACAddress(MAC_ADDRESS);

  // Default config
  IP4_ADDR(&gMyIpAddress,          IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3]);
  IP4_ADDR(&gMyNetmask,            IP_MASK[0], IP_MASK[1], IP_MASK[2], IP_MASK[3]);
  IP4_ADDR(&gMyGateway,            IP_GW[0], IP_GW[1], IP_GW[2], IP_GW[3]);
  IP4_ADDR(&gRemoteIpAddress,      192, 168, 0, 10);
  gRemotePort = 5505;

  DAC_Init();

  init_timer( 1, (25000000 / 1000) - 1 ); // 1seg
  init_network();

  enable_timer( 1 );

  while(1)
  {

#ifdef NORD_USS_ENABLE
    /*if (!I_USSReady && uss_timeout > 50)
      uss_ready();
    if (uss_timeout > 2000)
      I_USSReady = 1;*/
#else
      //ModbusRequest();
#endif

    check_network();
    ethernetif_handlepackets(netif_eth0);

#ifdef QEI_CHECK
    // Check VeloAccFlag continuously
	ENC_VAL = QEI_GetPosition(QEI);

    if (VeloAccFlag == SET)
	{
      //Get Position
      //PosCnt = QEI_GetPosition(QEI);
      //ENC_VAL = QEI_GetPosition(QEI);
      // Get Acc
      //averageVelo = VeloAcc / VeloCapCnt;
      //rpm = QEI_CalculateRPM(QEI, averageVelo, ENC_RES);
      // Reset VeloAccFlag
      //VeloAccFlag = RESET;
      // Reset value of Acc and Acc count to default
      //VeloAcc = 0;
      //VeloCapCnt = 0;
    }
#endif

	if (plccycle_timer >= 1)
	{
	  plccycle_timer = 0;
      entradas = AtualizaEntradas();
      saidas = AtualizaSaidas();
	  PlcCycle();
    }
  }

  return(0);
}

/*** EOF ***/
