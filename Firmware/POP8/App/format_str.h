#ifndef __FORMAT_STRING_H__
#define __FORMAT_STRING_H__

char *Format_String_Generate(char *msg, char *format, volatile int * val);
int   Format_String_Write(char *format, volatile int * val);
int   Format_String_Read(char *format, volatile int *val);

#endif
