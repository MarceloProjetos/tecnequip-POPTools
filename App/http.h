#ifndef __HTTP_H__
#define __HTTP_H__

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "coos.h"

unsigned char HTTP_Init(void);
void HTTP_Connect(void *pdata);
void HTTP_Request(struct netconn *conn);


#endif
