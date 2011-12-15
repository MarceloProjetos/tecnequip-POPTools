#ifndef __MODBUS_TCP_H__
#define __MODBUS_TCP_H__

#include <stdio.h>

#include "lpc_types.h"

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
#include "lwip/tcp_impl.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/init.h"

#include "modbus.h"

unsigned int Modbus_TCP_Tx(unsigned char *data, unsigned int size);

void Modbus_TCP_Init(void);
void Modbus_TCP_Connect(void *pdata);
void Modbus_TCP_Request(struct netconn *conn);
void Modbus_TCP_Send(unsigned char id,
                  int fc,
                  unsigned short int address,
                  unsigned short int size,
                  volatile int * value);
err_t Modbus_TCP_Recv_Callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
err_t Modbus_TCP_Poll_Callback(void *arg, struct tcp_pcb *tpcb);
err_t Modbus_TCP_Send_Callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
err_t Modbus_TCP_Accept_Callback(void *arg, struct tcp_pcb *newpcb, err_t err);

#define MODBUS_TCP_STACK_SIZE  	200
#define MODBUS_TCP_TASK_PRIO  	50

#endif
