#include "sntp.h"
#include "time.h"
#include "rtc.h"

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

extern char 			SNTP_SERVER_ADDRESS[];
extern int 				SNTP_GMT;
extern int 				SNTP_DAILY_SAVE;

static int
SNTP_LeapYear(int year)
{
	return (!(year & 3) && ((year % 100) || !(year % 400)));
}

static int
SNTP_LastDayOfMonth(int month, int year)
{
	return (month == 2 ? 28 + SNTP_LeapYear(year) : 30 + ((month & 1) ^ (month > 7)));
}

/**
 * SNTP processing
 */
static void
SNTP_Process( time_t t)
{
	RTC_Time rtc;
	int h = 0;

	/* change system time and/or the update the RTC clock */
	SNTP_SYSTEM_TIME(t);

	struct tm * local_time;

	local_time = localtime ( &t );

	local_time->tm_year += 1900;
	local_time->tm_mon++;
	local_time->tm_sec = local_time->tm_sec > 59 ? 59 : local_time->tm_sec;

	h = local_time->tm_hour + SNTP_GMT + SNTP_DAILY_SAVE;

	if (h > 24)
	{
		h -= 24;
		if (local_time->tm_mday == SNTP_LastDayOfMonth(local_time->tm_mon, local_time->tm_year) )
		{
			if (local_time->tm_mon == 12)
			{
				local_time->tm_mon = 1;
				local_time->tm_year++;
			}
			else
				local_time->tm_mon++;

			local_time->tm_mday = 1;
		}
		local_time->tm_mday++;
	}
	else if (h < 0)
	{
		h += 24;
		if (local_time->tm_mday == 1)
		{
			if (local_time->tm_mon == 1)
			{
				local_time->tm_mon = 12;
				local_time->tm_year--;
			}
			else
				local_time->tm_mon--;

			local_time->tm_mday = SNTP_LastDayOfMonth(local_time->tm_mon, local_time->tm_year);
		}
		local_time->tm_mday--;
	}

	rtc.Mday = local_time->tm_mday;
	rtc.Mon = local_time->tm_mon;
	rtc.Year = local_time->tm_year;
	rtc.Hour = (unsigned int)h;
	rtc.Min = local_time->tm_min;
	rtc.Sec = local_time->tm_sec;
	rtc.Wday = local_time->tm_wday;
	rtc.Yday = local_time->tm_yday;

	RTC_SetTime(rtc);

	/* display local time from GMT time */
	LWIP_DEBUGF( SNTP_DEBUG, ("sntp_process: %s", ctime(&t)));
}

/**
 * SNTP request
 */
static void
SNTP_Request()
{
  int                sock;
  struct sockaddr_in local;
  struct sockaddr_in to;
  int                tolen;
  int                size;
  int                timeout;
  u8_t               sntp_request [SNTP_MAX_DATA_LEN];
  u8_t               sntp_response[SNTP_MAX_DATA_LEN];
  ip_addr_t          sntp_server_address;
  u32_t              timestamp;
  time_t             t;
  err_t				 err;
  
  /* initialize SNTP server address */
  err = netconn_gethostbyname(SNTP_SERVER_ADDRESS, &sntp_server_address);

  /* if we got a valid SNTP server address... */
  if (/*sntp_server_address!=0 &&*/ err == ERR_OK) {
    /* create new socket */
    sock = socket( AF_INET, SOCK_DGRAM, 0);
    if (sock>=0) {
      /* prepare local address */
      memset(&local, 0, sizeof(local));
      local.sin_family      = AF_INET;
      local.sin_port        = htons(INADDR_ANY);
      local.sin_addr.s_addr = htonl(INADDR_ANY);

      /* bind to local address */
      if (bind( sock, (struct sockaddr *)&local, sizeof(local))==0) {
        /* set recv timeout */
        timeout = SNTP_RECV_TIMEOUT;
        setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

        /* prepare SNTP request */
        memset( sntp_request, 0, sizeof(sntp_request));
        sntp_request[0] = SNTP_LI_NO_WARNING | SNTP_VERSION | SNTP_MODE_CLIENT;

        /* prepare SNTP server address */
        memset(&to, 0, sizeof(to));
        to.sin_family      = AF_INET;
        to.sin_port        = htons(SNTP_PORT);
        to.sin_addr.s_addr = sntp_server_address.addr;
    
        /* send SNTP request to server */
        if (sendto( sock, sntp_request, sizeof(sntp_request), 0, (struct sockaddr *)&to, sizeof(to))>=0) {
          /* receive SNTP server response */
          tolen = sizeof(to);
          size  = recvfrom( sock, sntp_response, sizeof(sntp_response), 0, (struct sockaddr *)&to, (socklen_t *)&tolen);

          /* if the response size is good */
          if (size == SNTP_MAX_DATA_LEN) {
            /* if this is a SNTP response... */
            if (((sntp_response[0] & SNTP_MODE_MASK) == SNTP_MODE_SERVER) || ((sntp_response[0] & SNTP_MODE_MASK) == SNTP_MODE_BROADCAST)) {
              /* extract GMT time from response */
              SMEMCPY( &timestamp, (sntp_response+SNTP_RCV_TIME_OFS), sizeof(timestamp));
              t = (ntohl(timestamp) - (unsigned int)DIFF_SEC_1900_1970);

              /* do time processing */
              SNTP_Process(t);

            } else {
              LWIP_DEBUGF( SNTP_DEBUG, ("sntp_request: not response frame code\n"));
            }
          } else {
            LWIP_DEBUGF( SNTP_DEBUG, ("sntp_request: not recvfrom==%i\n", errno));
          }
        } else {
          LWIP_DEBUGF( SNTP_DEBUG, ("sntp_request: not sendto==%i\n", errno));
        }
      }
      /* close the socket */
      closesocket(sock);
    }
  }
}

/**
 * SNTP thread
 */
static void
SNTP_Thread(void *arg)
{
  LWIP_UNUSED_ARG(arg);

  // Pausa de 3 segundos para aguardar término da inicialização da interface de rede
  sys_msleep(3000);

  while(1)
  {
    SNTP_Request();
    sys_msleep(SNTP_UPDATE_DELAY);
  }
}
// Stack for tcpip_thread
OS_STK stkSNTPThread[SNTP_THREAD_STACKSIZE];
void
SNTP_Init(void)
{
	StatusType err;

	err = sys_thread_new(SNTP_THREAD_NAME,
					SNTP_Thread,
					&stkSNTPThread[SNTP_THREAD_STACKSIZE - 1],
					SNTP_THREAD_STACKSIZE,
					SNTP_THREAD_PRIO);

	if(err == E_CREATE_FAIL)
		return;

}

#endif /* LWIP_SOCKET */
