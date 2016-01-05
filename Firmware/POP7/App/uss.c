#include "serial.h"
#include "uss.h"

extern volatile unsigned char WAITING_FOR_USS;

extern volatile unsigned int I_SerialReady;

volatile int * USS_Value = NULL;

//---------------------------- Funções auxiliares -----------------------------
BCC USS_Calculate_BCC(unsigned char * ppo, unsigned int size)
{
	BCC bcc = 0;

	while(size--) {
		bcc ^= *(ppo++);
	}

	return bcc;
}

void USS_Set_BCC(unsigned char * ppo)
{
  BCC * bcc = ppo + *(ppo + 1) + sizeof(STX); // Endereco de BCC
  *bcc = USS_Calculate_BCC(ppo, bcc - ppo);
}

unsigned char USS_Check_CRC(unsigned char * ppo, unsigned int size)
{
  BCC * bcc = ppo + size - 1; // Endereco de BCC
  return *bcc == USS_Calculate_BCC(ppo, bcc - ppo);
}

void USS_Clean_PPO(unsigned char * ppo, unsigned int size)
{
  while(size--)
    *ppo++ = 0x00;
}
//------------------------- Fim de funções auxiliares -------------------------

void USS_PPO1(PPO1 *ppo, unsigned char adr, unsigned short int pke, unsigned short int ind, unsigned short int pweh, unsigned short int pwel, unsigned short int stw, unsigned short int sw1)
{
  USS_PPO_INIT     ((*ppo), PPO1_LGE);
  USS_PPO_SET_ADR  ((*ppo), 0, 0, adr);
  USS_PPO1_SET_PKE ((*ppo), pke);
  USS_PPO1_SET_IND ((*ppo), ind&0x3, ind>>2);
  USS_PPO1_SET_PWEH((*ppo), pweh);
  USS_PPO1_SET_PWEL((*ppo), pwel);
  USS_PPO1_SET_STW ((*ppo), stw);
  USS_PPO1_SET_SW1 ((*ppo), sw1);

  USS_Set_BCC(*ppo);
}

unsigned char USS_Get_Param(unsigned char addr, unsigned short int param, unsigned char param_set, unsigned char array, volatile int * val)
{
  PPO1 p;
  unsigned int sz = 0;

  USS_Value = val;

  if (array > 64)
    USS_PPO1(&p, addr, (0x60 << 8) | (param & 0x7FF), array & 0xFF, 0x0000, 0x0000, 0x0000, 0x0000);
  else
    USS_PPO1(&p, addr, ((array ? 0x60 : 0x10) << 8) | (param & 0x7FF), (((array ? array - 1 : 0) & 0x3F) << 2) | (param_set & 0x3), 0x0000, 0x0000, 0x0000, 0x0000);

  if ((sz = RS485_Write((unsigned char*)&p, sizeof(p))))
  {
    I_SerialReady = 0;
	WAITING_FOR_USS = 1; // uss
    return sz;
  }

  return 0;
}

unsigned char USS_Ready(unsigned char *r, unsigned int sz)
{
	if (!sz || !USS_Check_CRC(r, sz))
		return 0;

    if (*r == PPO_STX) // CRC ok e inicia com STX, pacote valido!
    {
      if (USS_Value)
        *USS_Value = (USS_PPO1_GET_PWEH(r)<<16) | USS_PPO1_GET_PWEL(r);
      I_SerialReady = 1;
      USS_Value = 0;
      return 1;
    }

  return 0;
}

unsigned char USS_Set_Param(unsigned char addr, unsigned short int param, unsigned char param_set, unsigned char array, volatile int * val)
{
  PPO1 p;
  unsigned int sz = 0;

  if (array > 64)
    USS_PPO1(&p, addr, (0x80 << 8) | (param & 0x7FF), array & 0xFF, *val >> 16, *val & 0xFFFF, 0x0000, 0x0000);
  else
    USS_PPO1(&p, addr, ((array ? 0x80 : 0x20) << 8) | (param & 0x7FF), (((array ? array - 1 : 0) & 0x3F) << 2) | (param_set & 0x3), *val >> 16, *val & 0xFFFF, 0x0000, 0x0000);

  if ((sz = RS485_Write((unsigned char*)&p, sizeof(p))))
  {
    I_SerialReady = 0;
	WAITING_FOR_USS = 1; // uss
    return sz;
  }

  return 0;
}
