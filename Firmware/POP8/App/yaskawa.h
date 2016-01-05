#ifndef __YASKAWA_H__
#define __YASKAWA_H__

int Yaskawa_Read(char * id, char *format, volatile int *val);
int Yaskawa_Write(char * id, char *format, volatile int * val);

#endif
