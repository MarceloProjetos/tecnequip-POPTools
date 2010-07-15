/*
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIP_LWIOPTS_H__
#define __LWIP_LWIOPTS_H__

// Application specific lwIP Options.
#define IPv6                            0
#define NO_SYS                          1
#define LWIP_ARP                        1
#define LWIP_RAW                        0
#define LWIP_UDP                        1
#define LWIP_TCP                        1
#define LWIP_DNS                        0
#define LWIP_DHCP                       0
#define LWIP_IGMP                       0
#define LWIP_SNMP                       0
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0
#define LWIP_AUTOIP                     0
#define LWIP_CALLBACK_API               1

#define LWIP_NETIF_HOSTNAME             1
#define HOSTNAME "POP7"

/* TCP sender buffer space (bytes) */
#define TCP_SND_BUF 1024

/* Align memory on 4 byte boundery (32-bit) */
#define MEM_ALIGNMENT                   4

/* Time in milliseconds to perform ARP processing */
#define ETHARP_TMR_INTERVAL             5000

#endif /* __LWIP_LWIOPTS_H__ */
