#include <stdio.h>
#include <string.h>
#include "http.h"
#include "rtc.h"

#if LWIP_NETCONN

const static char http_not_found_hdr[] = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_index_html[] = "<html><head><title>Tecnequip Tecnologia em Equipamentos Ltda</title></head><body><h1>POP7</h1><p>Página de configuração do CLP POP7.</p>";
const static char http_bottom[] = "<p><a href=\"http://www.tecnequip.com.br\">http://www.tecnequip.com.br</a><p></body></html>";

OS_STK HTTPStack[HTTP_THREAD_STACKSIZE];

void HTTP_Request(struct netconn *conn) {
  struct netbuf *inbuf;
  char *buf;
  u16_t buflen;
  err_t err;

  //unsigned char Hour, Minute, Second;
  //unsigned short int Millisecond;

  //char http_data[80];
  
  //RTC_Time now = RTC_GetTime();

  /* Read the data from the port, blocking if nothing yet there. 
   We assume the request (the part we care about) is in one netbuf */
  err = netconn_recv(conn, &inbuf);
  
  if (err == ERR_OK) {
    netbuf_data(inbuf, (void **)&buf, &buflen);
    
    /* Is this an HTTP GET command? (only check the first 5 chars, since
    there are other formats for GET, and we're keeping it very simple )*/
    if (buflen>=5 &&
        buf[0]=='G' &&
        buf[1]=='E' &&
        buf[2]=='T' &&
        buf[3]==' ' &&
        buf[4]=='/' ) {

      /* Send the HTML header 
	   * subtract 1 from the size, since we dont send the \0 in the string
	   * NETCONN_NOCOPY: our data is const static, so no need to copy it
       */
      netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);

 	  /* Send our HTML page */
	  netconn_write(conn, http_index_html, sizeof(http_index_html)-1, NETCONN_NOCOPY);

      netconn_write(conn, http_bottom, sizeof(http_bottom)-1, NETCONN_NOCOPY);

      /*
      sprintf(http_data, "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n");
      netconn_write(conn, http_data, strlen(http_data), NETCONN_COPY);

      sprintf(http_data, "<html><head><title>Tecnequip Tecnologia em Equipamentos Ltda</title></head>");
      netconn_write(conn, http_data, strlen(http_data), NETCONN_COPY);

      sprintf(http_data, "<body><h1>POP7</h1><p>Página de configuração do CLP POP7.</p>");
      netconn_write(conn, http_data, strlen(http_data), NETCONN_COPY);

      sprintf(http_data, "<p><a href=\"http://www.tecnequip.com.br\">http://www.tecnequip.com.br</a><p></body></html>");
      netconn_write(conn, http_data, strlen(http_data), NETCONN_COPY);
      
      //sprintf(http_body, "<p>RTC: %02d/%02d/%02d %02d:%02d:%02d %d,%03d</p>", now.Mday, now.Mon, now.Year, now.Hour, now.Min, now.Sec, now.Wday, now.Yday);
      sprintf(http_data, "<p>RTC: %02d/%02d/%02d %02d:%02d:%02d</p>", now.Mday, now.Mon, now.Year, now.Hour, now.Min, now.Sec);
      netconn_write(conn, http_data, strlen(http_data), NETCONN_COPY);

      CoTickToTime (CoGetOSTime(), &Hour, &Minute, &Second, &Millisecond);
      sprintf(http_data, "<p>OSTime: %3d:%02d:%02d.%03d</p>", Hour, Minute, Second, Millisecond);
      netconn_write(conn, http_data, strlen(http_data), NETCONN_COPY);

      sprintf(http_data, "<p><a href=\"http://www.tecnequip.com.br\">http://www.tecnequip.com.br</a><p></body></html>");
      netconn_write(conn, http_data, strlen(http_data), NETCONN_COPY);
      */
    }
    else
    {
    	netconn_write(conn, http_not_found_hdr, sizeof(http_not_found_hdr)-1, NETCONN_NOCOPY);
    }
  }
  /* Close the connection (server closes in HTTP) */
  netconn_close(conn);
  
  /* Delete the buffer (netconn_recv gives us ownership,
   so we have to make sure to deallocate the buffer) */
  netbuf_delete(inbuf);
}

void HTTP_Connect(void *pdata) {
	err_t err;
  struct netconn *conn, *newconn;
  
  /* Create a new TCP connection handle */
  conn = netconn_new(NETCONN_TCP);
//  LWIP_ERROR("http_server: invalid conn", (conn != NULL), return -1;);
  
  /* Bind to port 80 (HTTP) with default IP address */
  netconn_bind(conn, NULL, 80);
  
  /* Put the connection into LISTEN state */
  netconn_listen(conn);
  
  while(1) {
    err = netconn_accept(conn, &newconn);

    if (err == ERR_OK)
    	HTTP_Request(newconn);

    netconn_delete(newconn);
  }
//  return 0;
}

unsigned char HTTP_Init(void)
{
  StatusType err;

  err = sys_thread_new("HTTP",
		  	  	  	   HTTP_Connect,
                       &HTTPStack[HTTP_THREAD_STACKSIZE - 1],
                       HTTP_THREAD_STACKSIZE,
                       HTTP_THREAD_PRIO);

  if(err == E_CREATE_FAIL) {
    return 1;
  }

  return 0;
}

#endif /* LWIP_NETCONN*/
