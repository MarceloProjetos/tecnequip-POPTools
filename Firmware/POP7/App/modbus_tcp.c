#include "modbus_tcp.h"

extern struct MODBUS_Device modbus_tcp;

struct strMBTCP_Tansfer MBTCP_Transfer;

extern volatile int * MODBUS_RETURN_VAL;
extern volatile unsigned char MODBUS_TCP_MASTER;  // 0 = Slave, 1 = Master

struct netconn *current_conn;

OS_STK ModbusTCPStack[MODBUS_TCP_THREAD_STACKSIZE];

volatile unsigned int I_TcpReady   = 1;
volatile unsigned int I_TcpTimeout = 0;

MODBUS_HANDLER_TX(Modbus_TCP_Slave_Tx)
{
  if (netconn_write(current_conn, data, size, NETCONN_NOCOPY) != ERR_OK)
  {
//    tcp_close(tpcb_modbus_request);
    return 0;
  }

  return size;
}

MODBUS_HANDLER_TX(Modbus_TCP_Master_Tx)
{
	if(MBTCP_Transfer.status == MBTCP_STATUS_READY) {
		I_TcpReady = 0;
		memcpy(MBTCP_Transfer.buf, data, size);

		IP4_ADDR(&MBTCP_Transfer.RemoteIP,
				(ip >> 24) & 0xff,
				(ip >> 16) & 0xff,
				(ip >>  8) & 0xff,
				(ip >>  0) & 0xff);

		MBTCP_Transfer.bufsize = size;
		MBTCP_Transfer.status = MBTCP_STATUS_BUSY;
	}

	return 0; // Nao podemos comunicar agora ou o ladder sera bloqueado
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
		if(MODBUS_TCP_MASTER) {
			memcpy(MBTCP_Transfer.buf, buf, buflen);
			MBTCP_Transfer.bufsize = buflen;
		} else {
			Modbus_RTU_Receive(&modbus_tcp, Modbus_RTU_Validate((unsigned char*)buf, buflen, TRUE));
		}

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

static struct netconn *global_conn;
void Modbus_TCP_Connect(void *pdata)
{
//	Aparentemente seqno em globalconn->pcb->tcp->unacked->tcphdr está sendo corrompido em algum
// lugar. O valor desta variável é armazenada invertida! (Byte alto no lugar do byte baixo)


	err_t err;
	struct netconn *conn, *newconn;

	if(MODBUS_TCP_MASTER) {
		while(1) {
			if(MBTCP_Transfer.status == MBTCP_STATUS_BUSY) {
				/* Create a new TCP connection handle */
				conn = netconn_new(NETCONN_TCP);
				//  LWIP_ERROR("http_server: invalid conn", (conn != NULL), return -1;);

				/* Bind to port 80 (HTTP) with default IP address */
				netconn_bind(conn, IP_ADDR_ANY, 0);

				// Criar conexao
				global_conn = conn;
				if(netconn_connect(conn, &MBTCP_Transfer.RemoteIP, 502) == ERR_OK) {
					// Envia pacote
					netconn_write(conn, MBTCP_Transfer.buf, MBTCP_Transfer.bufsize, NETCONN_NOCOPY);

					// Repassa conexao para Modbus_TCP_Request que vai receber e tratar a resposta
					Modbus_TCP_Request(conn);
				} else {
					I_TcpTimeout = 1;
				}

				// Finaliza indicando que terminou a comunicacao
				netconn_delete(conn);
				MBTCP_Transfer.status = MBTCP_STATUS_DONE;
			}
		}
	} else {
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
	}
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
