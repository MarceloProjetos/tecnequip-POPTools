#include "modbus_tcp.h"

extern struct MODBUS_Device modbus_rtu_master;
extern struct MODBUS_Device modbus_rtu_slave;
extern struct MODBUS_Device modbus_tcp_slave;

extern volatile int * MODBUS_RETURN_VAL;

struct netconn *current_conn;

OS_STK ModbusTCPStack[MODBUS_TCP_STACK_SIZE];

unsigned int Modbus_TCP_Tx(unsigned char *data, unsigned int size)
{
  if (netconn_write(current_conn, data, size, NETCONN_NOCOPY) != ERR_OK)
  {
//    tcp_close(tpcb_modbus_request);
    return 0;
  }

  return size;
}

void Modbus_TCP_Send(unsigned char id,
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

  rp = Modbus_RTU_Send(mbdev, 0, fc, &data);

}

err_t Modbus_TCP_Recv_Callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  if (p != NULL)
  {
    if (p->len > 2)
      Modbus_RTU_Receive(&modbus_tcp_slave, Modbus_RTU_Validate(p->payload, p->len, TRUE));

    tcp_recved(tpcb, p->len);
    pbuf_free(p);
  }

  return ERR_OK;
}

err_t Modbus_TCP_Poll_Callback(void *arg, struct tcp_pcb *tpcb)
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

err_t Modbus_TCP_Send_Callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
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

err_t Modbus_TCP_Accept_Callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  tcp_arg(newpcb, NULL);
  tcp_recv(newpcb, Modbus_TCP_Recv_Callback);
  tcp_sent(newpcb, Modbus_TCP_Send_Callback);
  tcp_poll(newpcb, Modbus_TCP_Poll_Callback, 1);

  return ERR_OK;
}

void Modbus_TCP_Request(struct netconn *conn) {
  struct netbuf *inbuf;
  char *buf;
  u16_t buflen;
  err_t err;

  /* Read the data from the port, blocking if nothing yet there.
   We assume the request (the part we care about) is in one netbuf */
  err = netconn_recv(conn, &inbuf);

  if (err == ERR_OK) {
    netbuf_data(inbuf, (void **)&buf, &buflen);

    /* Is this an HTTP GET command? (only check the first 5 chars, since
    there are other formats for GET, and we're keeping it very simple )*/
    if ( buflen >= 2 )
    {
    	current_conn = conn;
    	Modbus_RTU_Receive(&modbus_tcp_slave, Modbus_RTU_Validate((unsigned char*)buf, buflen, TRUE));

    	//netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);

      /* Send our HTML page */
      //netconn_write(conn, http_index_html, sizeof(http_index_html)-1, NETCONN_NOCOPY);
    }
  }
  /* Close the connection (server closes in HTTP) */
  netconn_close(conn);

  /* Delete the buffer (netconn_recv gives us ownership,
   so we have to make sure to deallocate the buffer) */
  netbuf_delete(inbuf);
}

void Modbus_TCP_Connect(void *pdata)
{

	err_t err;
	struct netconn *conn, *newconn;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	//  LWIP_ERROR("http_server: invalid conn", (conn != NULL), return -1;);

	/* Bind to port 80 (HTTP) with default IP address */
	netconn_bind(conn, IP_ADDR_ANY, 502);

	/* Put the connection into LISTEN state */
	netconn_listen(conn);

	while(1)
	{
		err = netconn_accept(conn, &newconn);

		if (err == ERR_OK)
			Modbus_TCP_Request(newconn);

		netconn_delete(newconn);
	}

  /*struct tcp_pcb* tcpweb;
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

  tcp_accept(tcpweb, Modbus_TCP_Accept_Callback);*/
}

void Modbus_TCP_Init(void)
{
	StatusType err;

	err = sys_thread_new(MODBUS_TCP_THREAD_NAME,
					   Modbus_TCP_Connect,
					   &ModbusTCPStack[MODBUS_TCP_THREAD_STACKSIZE - 1],
					   MODBUS_TCP_THREAD_STACKSIZE,
					   MODBUS_TCP_THREAD_PRIO);

	if(err == E_CREATE_FAIL) {
		printf("Modbus_TCP: Create task failed !");
	}
}
