#include "lpc17xx.h"
#include "type.h"
#include "timer.h"
#include "modbus.h"
#include "ld.h"

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

#define HTONS(n) (uint16_t)((((uint16_t) (n)) << 8) | (((uint16_t) (n)) >> 8))

typedef struct ad_type
{
  unsigned int m1;
  unsigned int m2;
  unsigned int m3;
  unsigned int m4;
  unsigned int m5;
} ads;

struct ad_type ad[5];

/******************************************************************************
* Variaveis Globais
******************************************************************************/
volatile unsigned int saidas = 0;
volatile unsigned int entradas = 0;

volatile unsigned int timer0_count = 0;

struct MB_Device modbus_serial;
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

unsigned char modbus_rx_buffer[MB_BUFFER_SIZE];
unsigned char modbus_tx_buffer[MB_BUFFER_SIZE];

/******************************************************************************
* Prototipos de Funções
******************************************************************************/
unsigned int ModbusReadCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply);
unsigned int ModbusTx(unsigned char *data, unsigned int size);

/******************************************************************************
* lwip TCP/IP
******************************************************************************/
#define TCP_RECONNECT_TIMEOUT 2000
#define TCP_MSG_TIMEOUT 30000

int Init_EMAC(void);
int Init_EMAC_phase2(void);

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
static struct netif *loop_netif;

static struct ip_addr gMyIpAddress;
static struct ip_addr gMyNetmask;
static struct ip_addr gMyGateway;
static struct ip_addr gRemoteIpAddress;

static struct tcp_pcb *gTcp = NULL;

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
                        tcp_close(tpcb);
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

                if ((bytestosend - len) >= 0)
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
    DEBUG("Not enough buffer left to send data. Discarding\n");
    return;
  }

  if (data)
    memcpy(&tcp_data_x[1], data, data_size);

  tcp_data_x[0] = message_type;

  err_t err = tcp_write(gTcp, tcp_data_x, len, 1);
  if (err != ERR_OK)
  {
    DEBUG("TCP: Error sending data... (%d)\n", err);
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

  if (err != ERR_OK)
    DEBUG("TCP: Error forcing data send... (%d)\n", err);
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
    DEBUG("TCP connection closed by remote host?\n");
    gTcpState = TCP_DISCONNECTED;
  }

  return ERR_OK;
}

err_t poll_callback(void *arg, struct tcp_pcb *tpcb)
{
  if (arg != NULL)
  {
    u32_t bytestosend = *((u32_t*) arg);
    DEBUG("poll_callback bytestosend=%u\n", bytestosend);

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
    DEBUG("TCP Connected to remote host\n");
    tcp_recv(newpcb, recv_callback);
    tcp_sent(newpcb, sent_callback);
    tcp_poll(newpcb, poll_callback, 1);
    gTcp = newpcb;
    DEBUG("Setting TCP state to TCP_JUST_CONNECTED\n");
    gTcpState = TCP_JUST_CONNECTED;
  }
  else
  {
    DEBUG("Error connecting to remote host: %d\n", err);
  }
  return ERR_OK;
}

void error(void *arg, err_t err)
{
  DEBUG("TCP Error: %d\n", err);
  if (gTcpState !=  TCP_DISCONNECTED)
  {
    DEBUG("Setting TCP state to TCP_DISCONNECTED\n");

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
    DEBUG("Failed to allocate tcp_client\n");
    return;
  }

  tcp_err(tcp_client, error);

  DEBUG("gRemotePort=%d\n", gRemotePort);
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
        DEBUG("Eth0 Link UP!\n");
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
    DEBUG("Local IP is %u.%u.%u.%u\n", netif_eth0->ip_addr.addr & 0xFF, netif_eth0->ip_addr.addr >> 8 & 0xFF, netif_eth0->ip_addr.addr >> 16 & 0xFF, netif_eth0->ip_addr.addr >> 24 & 0xFF);
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
    DEBUG("Setting TCP state to TCP_CONNECTED\n");
    gTcpState = TCP_CONNECTED;
    gTcpLastMessageTimer = 0;
    break;
  case TCP_CONNECTED:
    if (gTcpLastMessageTimer >= TCP_MSG_TIMEOUT)
    {
      DEBUG("TCP receive timeout.\n");
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
  TIM0->IR = 1;                       /* clear interrupt flag */

  timer0_count++;

  PlcCycle();
}

/******************************************************************************
* ADC Read
******************************************************************************/
unsigned int ADCRead(unsigned int a)
{
  unsigned int max, min;
  unsigned int i = 0;

  unsigned int soma = 0;

  max = 0;
  min = 0;

  if (a < 1 || a > 5) return 0;

  a -= 1;

  ad[a].m1 = ad[a].m2;
  ad[a].m2 = ad[a].m3;
  ad[a].m3 = ad[a].m4;
  ad[a].m4 = ad[a].m5;

  switch (a)
  {
  case 0:
    ad[a].m5 = 0xFFF & (ADC->ADDR5 >> 4);
    break;
  case 1:
    ad[a].m5 = 0xFFF & (ADC->ADDR2 >> 4);
    break;
  case 2:
    ad[a].m5 = 0xFFF & (ADC->ADDR1 >> 4);
    break;
  case 3:
    ad[a].m5 = 0xFFF & (ADC->ADDR0 >> 4);
    break;
  case 4:
    ad[a].m5 = 0xFFF & (ADC->ADDR3 >> 4);
    break;
  }

  if (max < ad[a].m1)
    max = ad[a].m1;
  if (max < ad[a].m2)
    max = ad[a].m2;
  if (max < ad[a].m3)
    max = ad[a].m3;
  if (max < ad[a].m4)
    max = ad[a].m4;
  if (max < ad[a].m5)
    max = ad[a].m5;

  min = max;

  if (min > ad[a].m1)
    min = ad[a].m1;
  if (min > ad[a].m2)
    min = ad[a].m2;
  if (min > ad[a].m3)
    min = ad[a].m3;
  if (min > ad[a].m4)
    min = ad[a].m4;
  if (min > ad[a].m5)
    min = ad[a].m5;

  if (ad[a].m1 > min && ad[a].m1 < max)
  {
    soma += ad[a].m1; i++;
  }

  if (ad[a].m2 > min && ad[a].m2 < max)
  {
    soma += ad[a].m2; i++;
  }

  if (ad[a].m3 > min && ad[a].m3 < max)
  {
    soma += ad[a].m3; i++;
  }

  if (ad[a].m4 > min && ad[a].m4 < max)
  {
    soma += ad[a].m4; i++;
  }

  if (ad[a].m5 > min && ad[a].m5 < max)
  {
    soma += ad[a].m5; i++;
  }

  return i > 0 ? (unsigned int)(soma / i) : 0;
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
unsigned int ModbusRequest(void)
{
  unsigned int sz = 0;

  memset(modbus_rx_buffer, 0, sizeof(modbus_rx_buffer));
  memset(modbus_tx_buffer, 0, sizeof(modbus_tx_buffer));

  if((sz = RS485Read(modbus_rx_buffer, sizeof(modbus_rx_buffer))) > 0)
  {
    reply_data.data = modbus_tx_buffer;
    if (sz > 2)
      MB_Receive(&modbus_serial, MB_Validate(modbus_rx_buffer, sz));
  }

  return reply_data.size; // Retorna o tamanho dos dados
}

unsigned int ModbusRS485Tx(unsigned char *data, unsigned int size)
{
  unsigned int sz = 0;

  sz = RS485Write(data, size);

  return sz;
}

unsigned int ModbusEthTx(unsigned char *data, unsigned int size)
{
  unsigned int sz = 0;

  u32_t  bytestosend;
  u32_t* newarg;

  bytestosend = size;

  newarg = (u32_t*) mem_malloc(sizeof(u32_t));
  *newarg = bytestosend;

  if (tcp_write(tpcb_modbus_request, data, size, 0) != ERR_OK)
  {
    mem_free(newarg);
    tcp_close(tpcb_modbus_request);
  }
  else
  {
    tcp_arg(tpcb_modbus_request, newarg);
    sz = bytestosend;
  }

  return sz;
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

#ifdef MB_DEBUG
  printf("ModBus: recebida funcao READ_COILS. start=%d, quant=%d\n", data->read_coils.start, data->read_coils.quant);
#endif

  return MB_EXCEPTION_NONE;

}

unsigned int ModbusReadDiscreteInputs(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  uint32_t i;
  uint8_t *buf = reply->reply.read_discrete_inputs.data; // Retorna no maximo 8 bytes ou 256 bits (entradas).

#ifdef MB_DEBUG
  printf("ModBus: recebida funcao READ_DISCRETE_INPUTS. start=%d, quant=%d\n",
  data->read_discrete_inputs.start, data->read_discrete_inputs.quant);
#endif

  for(i = 0; i < data->read_discrete_inputs.quant; i++)
    buf[i / 8] |= ((entradas >> (data->read_discrete_inputs.start + i)) & 1) << (i % 8);

  reply->reply.read_discrete_inputs.size = data->read_discrete_inputs.quant / 8 + (data->read_discrete_inputs.quant % 8 != 0);

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusReadHoldingRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  uint32_t sz = 0;

  uint32_t i = 0;
  uint8_t *buf = reply->reply.read_holding_registers.data;

  for(i = 0; i < data->read_holding_registers.quant; i++)
  {
    if (data->read_holding_registers.start + i < 32)
    {
      buf[(i + sz)] = (uint8_t)(U_M[data->read_holding_registers.start + i] >> 8);
      buf[(i + sz) + 1] = (uint8_t)(U_M[data->read_holding_registers.start + i]);
      sz += 2;
    }
    else
      return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  }

  reply->reply.read_holding_registers.size = sz;

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
    U_M[data->write_single_register.address] = data->write_single_register.val;
  else
    return MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.write_single_register = data->write_single_register;

  return MB_EXCEPTION_NONE;
}

unsigned int ModbusWriteMultipleCoils(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusWriteMultipleRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusMaskWriteRegister(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusRWMultipleRegisters(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
  return MB_EXCEPTION_NONE;
}

unsigned int ModbusReadExceptionStatus(struct MB_Device *dev, union MB_FCD_Data *data, struct MB_Reply *reply)
{
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
* Rotinas de atualização das Entradas/Saidas
******************************************************************************/
unsigned int AtualizaSaidas(void)
{
  unsigned int i = 0;
  unsigned int status = 0;
  unsigned char cmd = OUTPUT_CMD_CONTROL;

  i |= U_S1;
  i |= U_S2 << 1;
  i |= U_S3 << 2;
  i |= U_S4 << 3;
  i |= U_S5 << 4;
  i |= U_S6 << 5;
  i |= U_S7 << 6;
  i |= U_S8 << 7;
  i |= U_S9 << 8;
  i |= U_S10 << 9;
  i |= U_S11 << 10;
  i |= U_S12 << 11;
  i |= U_S13 << 12;
  i |= U_S14 << 13;
  i |= U_S15 << 14;
  i |= U_S16 << 15;

  i = HTONS(i);

  i &= 0xFFFF;

  GPIO0->FIOCLR = 0x10000;
  SSPWrite((unsigned char*)&cmd, 1);
  SSPWrite((unsigned char*)&i, 2);
  GPIO0->FIOSET = 0x10000;

  SSPRead((unsigned char*)&status, 3);

  return i;
}

unsigned int AtualizaEntradas(void)
{
  unsigned int i = 0;

  i |= 0x3FF & GPIO2->FIOPIN;            // P2.0 ~ P2.9    [00..10]
  i |= (0x3800 & GPIO2->FIOPIN) >> 1;    // P2.11 ~ P2.13  [11..13]
  i |= (0x30 & GPIO0->FIOPIN) << 9;      // P0.4 ~ P0.5    [14..15]
  i |= (0x80000 & GPIO0->FIOPIN) >> 4;   // P0.19 ~ P0.19  [16..16]
  i |= (0x600000 & GPIO0->FIOPIN) >> 5;  // P0.21 ~ P0.22  [17..18]

  i = ~i;

  I_E1 = i & 0x1;
  I_E2 = (i & (1 << 1)) >> 1;
  I_E3 = (i & (1 << 2)) >> 2;
  I_E4 = (i & (1 << 3)) >> 3;
  I_E5 = (i & (1 << 4)) >> 4;
  I_E6 = (i & (1 << 5)) >> 5;
  I_E7 = (i & (1 << 6)) >> 6;
  I_E8 = (i & (1 << 7)) >> 7;
  I_E9 = (i & (1 << 8)) >> 8;
  I_E10 = (i & (1 << 9)) >> 9;
  I_E11 = (i & (1 << 10)) >> 10;
  I_E12 = (i & (1 << 11)) >> 11;
  I_E13 = (i & (1 << 12)) >> 12;
  I_E14 = (i & (1 << 13)) >> 13;
  I_E15 = (i & (1 << 14)) >> 14;
  I_E16 = (i & (1 << 15)) >> 15;
  I_E17 = (i & (1 << 16)) >> 16;
  I_E18 = (i & (1 << 17)) >> 17;
  I_E19 = (i & (1 << 18)) >> 18;

  return i;
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
  UART3->LCR = 0x83;          /* 8 bits, no Parity, 1 Stop bit */
  UART3->DLM = 0;
  UART3->DLL = 92;
  UART3->FDR = (13 << 4) | 10;
  UART3->LCR = 0x03;          /* DLAB = 0 */
  UART3->FCR = 0x07;          /* Enable and reset TX and RX FIFO. */

  // Pino de direcao da RS485
  PINCON->PINSEL1 &= ~(0x3 << 8);       // P0.20
  GPIO0->FIODIR |= RS485_ENABLE;        // P0.20 output

  // Entradas
  PINCON->PINSEL0 &= ~0xF00;
  PINCON->PINSEL1 &= ~0x3CC0;
  PINCON->PINSEL4 &= ~0xFCFFFFF;
  GPIO2->FIODIR &= ~0x3BFF;
  GPIO0->FIODIR &= ~0x680030;

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

}

/***************************************************************************/
/*  main                                                                   */
/***************************************************************************/
int main (void)
{
  /**************************************************************************
   * Inicializa System
   *************************************************************************/
  SystemInit();
  HardwareInit();

  /**************************************************************************
   * Inicializa timer
   *************************************************************************/
  init_timer( 0, TIME_INTERVAL ); // PlcCycle
  enable_timer( 0 );

  /**************************************************************************
   * Inicialização do ModBus
   *************************************************************************/
  MB_Init(&modbus_serial);

  modbus_serial.identification.Id                 = 0x01;
  modbus_serial.identification.VendorName         = "Tecnequip";
  modbus_serial.identification.ProductCode        = "POP7";
  modbus_serial.identification.MajorMinorRevision = "V1 Rev2";

  modbus_serial.hl      = ModbusHandlers;
  modbus_serial.hl_size = ARRAY_SIZE(ModbusHandlers);
  modbus_serial.mode    = MB_MODE_SLAVE;
  modbus_serial.TX      = ModbusRS485Tx;

  MB_Init(&modbus_tcp);

  modbus_tcp.identification.Id                 = 0x01;
  modbus_tcp.identification.VendorName         = "Tecnequip";
  modbus_tcp.identification.ProductCode        = "POP7";
  modbus_tcp.identification.MajorMinorRevision = "V1 Rev2";

  modbus_tcp.hl      = ModbusHandlers;
  modbus_tcp.hl_size = ARRAY_SIZE(ModbusHandlers);
  modbus_tcp.mode    = MB_MODE_SLAVE;
  modbus_tcp.TX      = ModbusEthTx;

  memset((void*)U_M, 0, sizeof(U_M));

  gTcpState = TCP_WAIT_LINK;

  // Default config
  IP4_ADDR(&gMyIpAddress,          192, 168,   0, 237);
  IP4_ADDR(&gMyNetmask,            255, 255, 255,   0);
  IP4_ADDR(&gMyGateway,            192, 168,   0,  10);
  IP4_ADDR(&gRemoteIpAddress,      192, 168,   0,  10);
  gRemotePort = 5505;

  init_timer( 1, (25000000 / 1000) - 1 ); // 1seg
  init_network();

  enable_timer( 1 );

  while(1)
  {
    entradas = AtualizaEntradas();
    saidas = AtualizaSaidas();

    ModbusRequest();

    check_network();
    ethernetif_handlepackets(netif_eth0);
  }
  return(0);
}

/*** EOF ***/
