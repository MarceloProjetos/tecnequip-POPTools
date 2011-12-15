#include <stdio.h>
#include <stdlib.h>

/******************************************************************************
* Comunicação Protocolo USS (Inversores Nord)
******************************************************************************/
void cmd_uss_send(char * args[], void * vars, unsigned int args_size)
{
  unsigned short int parametro = 0;
  unsigned char indice = 0;
  unsigned short int valor = 0;

  if (args_size < 3)
    return;

  parametro = atoi(args[0]); //console_decode_num(in, &console_offset, size);
  indice = atoi(args[1]); //console_decode_num(in, &console_offset, size);
  valor = atoi(args[2]); //console_decode_num(in, &console_offset, size);
}

void cmd_uss_recv(char * args[], void * vars, unsigned int args_size)
{
  unsigned short int parametro = 0;
  unsigned char indice = 0;
  //unsigned short int valor = 0;

  if (args_size < 2)
    return;

  parametro = atoi(args[0]); //console_decode_num(in, &console_offset, size);
  indice = atoi(args[1]); //console_decode_num(in, &console_offset, size);

}
