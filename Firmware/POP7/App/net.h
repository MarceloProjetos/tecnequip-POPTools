#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "lpc17xx.h"
#include "system_lpc17xx.h"
#include "coos.h"

#include <stdio.h>

#include "ethernetif.h"
#include "lwip/debug.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/api.h"
#include "lwip/stats.h"
#include "lwip/dns.h"
#include "netif/etharp.h"

#include "sntp.h"
#include "modbus.h"

void LWIPDebug(const char *pcString, ...);
void Net_Init(void);

#endif
