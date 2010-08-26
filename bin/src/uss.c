#include "uss.h"

extern unsigned int uss_timeout;
extern unsigned int serial_timeout;
extern volatile unsigned int I_USSReady;
extern volatile unsigned int I_SerialReady;
volatile unsigned int * USSValue = NULL;

//PPO0 uss_PPO0(ADR adr, PKE pke, IND ind, PWEH pwe, STW stw, SW1 sw1)
PPO0 uss_PPO0(unsigned char adr, unsigned short int pke, unsigned short int ind, unsigned short int pwe, unsigned short int stw, unsigned short int sw1)
{
  volatile PPO0 ppo;

  uss_clean_ppo((unsigned char*)&ppo, sizeof(PPO0));

  ppo.stx = PPO_STX;
  ppo.lge = PPO0_LGE;
  ppo.adr.byte = adr;
  ppo.pke.word = (pke >> 8) | (pke << 8);
  ppo.ind.word = (ind >> 8) | (ind << 8);
  ppo.pwe = (pwe >> 8) | (pwe << 8);
  ppo.stw.word = (stw >> 8) | (stw << 8);
  ppo.sw1.word = (sw1 >> 8) | (sw1 << 8);

  uss_set_BCC((unsigned char*)&ppo);

  return ppo;
}

//PPO1 uss_PPO1(ADR adr, PKE pke, IND ind, PWEH pweh, PWEL pwel, STW stw, SW1 sw1)
PPO1 uss_PPO1(unsigned char adr, unsigned short int pke, unsigned short int ind, unsigned short int pweh, unsigned short int pwel, unsigned short int stw, unsigned short int sw1)
{
  volatile PPO1 ppo;

  uss_clean_ppo((unsigned char*)&ppo, sizeof(PPO1));

  ppo.stx = PPO_STX;
  ppo.lge = PPO1_LGE;
  ppo.adr.byte = adr;
  ppo.pke.word = HTONS(pke); // (pke >> 8) | (pke << 8);
  ppo.ind.word = HTONS(ind); // (ind >> 8) | (ind << 8);
  ppo.pweh = HTONS(pweh); // (pweh >> 8) | (pweh << 8);
  ppo.pwel = HTONS(pwel); // (pwel >> 8) | (pwel << 8);
  ppo.stw.word = HTONS(stw); // (stw >> 8) | (stw << 8);
  ppo.sw1.word = HTONS(sw1); // (sw1 >> 8) | (sw1 << 8);

  uss_set_BCC((unsigned char*)&ppo);

  return ppo;
}

//PPO2 uss_PPO2(ADR adr, PKE pke, IND ind, PWEH pweh, PWEL pwel, STW stw, SW1 sw1, SW3 sw3, SW2 sw2)
PPO2 uss_PPO2(unsigned char adr, unsigned short int pke, unsigned short int ind, unsigned short int pweh, unsigned short int pwel, unsigned short int stw, unsigned short int sw1, unsigned short int sw3, unsigned short int sw2)
{
  volatile PPO2 ppo;

  uss_clean_ppo((unsigned char*)&ppo, sizeof(PPO2));

  ppo.stx = PPO_STX;
  ppo.lge = PPO2_LGE;
  ppo.adr.byte = adr;
  ppo.pke.word = (pke >> 8) | (pke << 8);
  ppo.ind.word = (ind >> 8) | (ind << 8);
  ppo.pweh = (pweh >> 8) | (pweh << 8);
  ppo.pwel = (pwel >> 8) | (pwel << 8);
  ppo.stw.word = (stw >> 8) | (stw << 8);
  ppo.sw1.word = (sw1 >> 8) | (sw1 << 8);
  ppo.sw3.word = (sw3 >> 8) | (sw3 << 8);
  ppo.sw2.word = (sw2 >> 8) | (sw2 << 8);

  uss_set_BCC((unsigned char*)&ppo);

  return ppo;
}

//PPO3 uss_PPO3(ADR adr, STW stw, SW1 sw1)
PPO3 uss_PPO3(unsigned char adr, unsigned short int stw, unsigned short int sw1)
{
  volatile PPO3 ppo;

  uss_clean_ppo((unsigned char*)&ppo, sizeof(PPO3));

  ppo.stx = PPO_STX;
  ppo.lge = PPO3_LGE;
  ppo.adr.byte = adr;
  ppo.stw.word = (stw >> 8) | (stw << 8);
  ppo.sw1.word = (sw1 >> 8) | (sw1 << 8);

  uss_set_BCC((unsigned char*)&ppo);

  return ppo;
}

//PPO4 uss_PPO4(ADR adr, STW stw, SW1 sw1, SW3 sw3, SW2 sw2)
PPO4 uss_PPO4(unsigned char adr, unsigned short int stw, unsigned short int sw1, unsigned short int sw3, unsigned short int sw2)
{
  volatile PPO4 ppo;

  uss_clean_ppo((unsigned char*)&ppo, sizeof(PPO4));

  ppo.stx = PPO_STX;
  ppo.lge = PPO4_LGE;
  ppo.adr.byte = adr;
  ppo.stw.word = (stw >> 8) | (stw << 8);
  ppo.sw1.word = (sw1 >> 8) | (sw1 << 8);
  ppo.sw3.word = (sw3 >> 8) | (sw3 << 8);
  ppo.sw2.word = (sw2 >> 8) | (sw2 << 8);

  uss_set_BCC((unsigned char*)&ppo);

  return ppo;
}

//------------------- Funções de nível mais alto de abstração -----------------
unsigned char uss_set_control_bits(ADR adr, unsigned int control_bits)
{
  //volatile STW stw;
  /*volatile PPO4 ppo = uss_PPO4(adr,
       stw,
       (SW1)((control_bits >> 24) & 0xFF),
       (SW3)((control_bits >> 16) & 0xFF),
       (SW2)((control_bits >> 8) & 0xFF));*/

  return 0;
}

unsigned short int uss_get_status_bits(void)
{
  return 0;
}

unsigned char uss_get_param(unsigned char addr, unsigned short int param, unsigned char param_set, unsigned char array, volatile unsigned int * val)
{
  volatile PPO1 p;
  //unsigned char * x;
  unsigned int sz = 0;

  USSValue = val;

  // limpa residuo transmissão/recepcao anterior
  //rs485_rx_fifo_reset(3);
  //rs485_tx_fifo_reset(3);

  if (array > 64)
    p = uss_PPO1(addr, (0x60 << 8) | (param & 0x7FF), array & 0xFF, 0x0000, 0x0000, 0x0000, 0x0000);
  else
    p = uss_PPO1(addr, ((array ? 0x60 : 0x10) << 8) | (param & 0x7FF), (((array ? array - 1 : 0) & 0x3F) << 2) | (param_set & 0x3), 0x0000, 0x0000, 0x0000, 0x0000);

  uss_timeout = 0;
  serial_timeout = 0;

  //while(uss_timeout < 5);      // delay 1ms

  if ((sz = RS485Write((unsigned char*)&p, sizeof(p))))
  {
    I_SerialReady = 0;
    //uss_timeout = 0;
    //while(uss_timeout < 50); // delay
    return sz;
  }

  return 0;
}

unsigned char uss_ready(PPO1 * r, unsigned int sz)
{
  //volatile PPO1 r;
  //volatile PPO1 p;
  //unsigned int rval = 0;
  //unsigned char * x;
  //unsigned int sz = 0;

  //memset((unsigned char*)&r, 0, sizeof(r));

  //uss_timeout = 0;

  /*if ((sz = RS485Read((unsigned char*)&r, sizeof(r))))
  {*/
    //if (sz != sizeof(r))
    //  return 0;

    //if (!uss_check_crc((unsigned char*)&r))
    //  return 0;

    //x = (unsigned char*)&r;
    //r.pke.word = HTONS(r.pke.word);
    //r.ind.word = HTONS(r.ind.word);
    //r.pweh = HTONS(r.pweh);
    //r.pwel = HTONS(r.pwel);
    //if (r.lge == 14 && r.adr.ADDRESS == p.adr.ADDRESS /*&& r.pke.PNU == (param & 0x7FF)*/)
    if (sz >= 1 && *((unsigned char*)r) == 0x02)
    {
      if (USSValue)
        *USSValue = (r->pweh << 16) | r->pwel;
      I_SerialReady = 1;
      return 1;
    }
  //}

  return 0;
}

unsigned char uss_set_param(unsigned char addr, unsigned short int param, unsigned char param_set, unsigned char array, volatile unsigned int * val)
{
  volatile PPO1 p;
  //unsigned int rval = 0;
  unsigned int sz = 0;
  //unsigned char * x;

  // limpa residuo transmissão/recepcao anterior
  //rs485_rx_fifo_reset(3);
  //rs485_tx_fifo_reset(3);

  if (array > 64)
    p = uss_PPO1(addr, (0x80 << 8) | (param & 0x7FF), array & 0xFF, *val >> 16, *val & 0xFFFF, 0x0000, 0x0000);
  else
    p = uss_PPO1(addr, ((array ? 0x80 : 0x20) << 8) | (param & 0x7FF), (((array ? array - 1 : 0) & 0x3F) << 2) | (param_set & 0x3), *val >> 16, *val & 0xFFFF, 0x0000, 0x0000);

  uss_timeout = 0;
  serial_timeout = 0;
  //while(uss_timeout < 5);      // delay 1ms

  if ((sz = RS485Write((unsigned char*)&p, sizeof(p))))
  {
    I_SerialReady = 0;
    //uss_timeout = 0;
    //while(uss_timeout < 50); // delay
    return sz;
  }

  return 0;
}

  /*
  memset((unsigned char*)&r, 0, sizeof(r));

  if ((sz = RS485Read((unsigned char*)&r, sizeof(r))))
  {
    if (sz != sizeof(r))
      return 0;

    if (!uss_check_crc((unsigned char*)&r))
      return 0;

    x = (unsigned char*)&r;
    r.pke.word = HTONS(r.pke.word);
    r.ind.word = HTONS(r.ind.word);
    r.pweh = HTONS(r.pweh);
    r.pwel = HTONS(r.pwel);
    if (r.pke.word >> 12 == 7) // job cannot be executed (retrieve error number in PWE2)
      return 0;

    if (r.lge == 14 && r.adr.ADDRESS == p.adr.ADDRESS && r.pke.PNU == (num & 0x7FF))
      return 1;
    else
      if (uss_get_param(param, idx, &rval))
        if (rval == val)
          return 1;
  }

  return 0;
}*/

//---------------------------- Funções auxiliares -----------------------------
void uss_clean_ppo(unsigned char * ppo, unsigned int size)
{
  while(size--)
    *ppo++ = 0x00;
}

void uss_set_BCC(unsigned char * ppo)
{
  volatile BCC * bcc = ppo + *(ppo + 1) + sizeof(STX); // Endereco de BCC

  *bcc = 0x00; // garante que o byte estara limpo para o primeiro xor

  for(; ppo < bcc;)
  {
     *bcc ^= *(ppo++);
  }

}

unsigned char uss_check_crc(unsigned char * ppo)
{
  unsigned char crc;

  volatile BCC * bcc = ppo + *(ppo + 1) + sizeof(STX); // Endereco de BCC

  crc = *bcc;
  *bcc = 0x00; // garante que o byte estara limpo para o primeiro xor

  for(; ppo < bcc;)
  {
     *bcc ^= *(ppo++);
  }

  return crc == *bcc;
}

