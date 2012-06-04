#include "modbus_tcp.h"

extern struct MODBUS_Device modbus_rtu_master;
extern struct MODBUS_Device modbus_rtu_slave;
extern struct MODBUS_Device modbus_tcp_slave;

extern volatile int * MODBUS_RETURN_VAL;

struct netconn *current_conn;

OS_STK ModbusTCPStack[MODBUS_TCP_THREAD_STACKSIZE];

unsigned int Modbus_TCP_Tx(unsigned char *data, unsigned int size)
{
  if (netconn_write(current_conn, data, size, NETCONN_NOCOPY) != ERR_OK)
  {
//    tcp_close(tpcb_modbus_request);
    return 0;
  }

  return size;
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
