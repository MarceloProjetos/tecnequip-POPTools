#include "net.h"

struct netif *lpc17xx_netif;

//char SNTP_SERVER_ADDRESS[] = "servidor.local";
//char SNTP_SERVER_ADDRESS[] = "pool.ntp.org";

struct ip_addr 			IP_ADDRESS;
struct ip_addr 			IP_NETMASK;
struct ip_addr 			IP_GATEWAY;
struct ip_addr			IP_DNS;

void LWIPDebug(const char *pcString, ...)
{
	printf(pcString);
}

void Net_Init(void)
{
	//struct ip_addr ipaddr, netmask, gw, dns1, dns2;

	tcpip_init(NULL, NULL);

	lpc17xx_netif = mem_malloc(sizeof(struct netif));

	//IP4_ADDR(&IP_ADDRESS, 10,0,0,5);
	//IP4_ADDR(&IP_NETMASK, 255,255,255,0);
	//IP4_ADDR(&IP_GATEWAY, 10,0,0,1);
	//IP4_ADDR(&IP_DNS, 10,0,0,2);

	netif_set_default(netif_add(lpc17xx_netif, &IP_ADDRESS, &IP_NETMASK, &IP_GATEWAY, NULL, ethernetif_init, tcpip_input));

	dns_setserver(0, &IP_DNS);
	dns_init();

	//HTTP_Init();
	//SNTP_Init();
	Modbus_Init();
//	Modbus_TCP_Init();

	netif_set_up(lpc17xx_netif);
}
