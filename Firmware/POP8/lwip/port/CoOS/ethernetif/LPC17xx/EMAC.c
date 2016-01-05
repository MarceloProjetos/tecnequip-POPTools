/******************************************************************
 *****                                                        *****
 *****  Name: cs8900.c                                        *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****  Func: ethernet packet-driver for use with LAN-        *****
 *****        controller CS8900 from Crystal/Cirrus Logic     *****
 *****                                                        *****
 *****  Keil: Module modified for use with Philips            *****
 *****        LPC2378 EMAC Ethernet controller                *****
 *****                                                        *****
 ******************************************************************/

#include <string.h>
#include "EMAC.h"
#include "chip.h"
#include "coos.h"
#include "ethernetif.h"

extern unsigned char mac_addr[6];

/* The Ethernet Block can only access Peripheral SRAM and External Memory. In this example,
   Peripheral SRAM is selected for storing descriptors, status arrays and send/receive buffers.*/
#define ENET_RX_DESC_BASE        (0x20000000UL)
#define ENET_RX_STAT_BASE        (ENET_RX_DESC_BASE + ENET_NUM_RX_DESC * sizeof(ENET_RXDESC_T))
#define ENET_TX_DESC_BASE        (ENET_RX_STAT_BASE + ENET_NUM_RX_DESC * sizeof(ENET_RXSTAT_T))
#define ENET_TX_STAT_BASE        (ENET_TX_DESC_BASE + ENET_NUM_TX_DESC * sizeof(ENET_TXDESC_T))
#define ENET_RX_BUF_BASE         (ENET_TX_STAT_BASE + ENET_NUM_TX_DESC * sizeof(ENET_TXSTAT_T))
#define ENET_TX_BUF_BASE         (ENET_RX_BUF_BASE  + ENET_NUM_RX_DESC * ENET_ETH_MAX_FLEN)
#define ENET_RX_BUF(i)           (ENET_RX_BUF_BASE + ENET_ETH_MAX_FLEN * i)
#define ENET_TX_BUF(i)           (ENET_TX_BUF_BASE + ENET_ETH_MAX_FLEN * i)

STATIC ENET_RXDESC_T *pRXDescs = (ENET_RXDESC_T *) ENET_RX_DESC_BASE;
STATIC ENET_RXSTAT_T *pRXStats = (ENET_RXSTAT_T *) ENET_RX_STAT_BASE;
STATIC ENET_TXDESC_T *pTXDescs = (ENET_TXDESC_T *) ENET_TX_DESC_BASE;
STATIC ENET_TXSTAT_T *pTXStats = (ENET_TXSTAT_T *) ENET_TX_STAT_BASE;

/* Transmit/receive buffers and indices */
STATIC int32_t rxConsumeIdx;
STATIC int32_t txProduceIdx;

static unsigned short *rptr;
static unsigned short *tptr;

extern struct netif *lpc17xx_netif;
extern OS_EventID semEthTx;
extern OS_EventID semEthRx;

unsigned int cnt = 0;

void ENET_IRQHandler (void)
{
  U32 status = Chip_ENET_GetIntStatus(LPC_ETHERNET);
  
  CoEnterISR();

  if(status & ENET_INT_RXDONE) {
    isr_PostSem(semEthRx);
    cnt ++;
  }
  
  if(status & ENET_INT_TXDONE) {
    isr_PostSem(semEthTx);
  }

  Chip_ENET_ClearIntStatus(LPC_ETHERNET, status);

  CoExitISR();
}

// Keil: function added to write PHY
void write_PHY (int PhyReg, int Value)
{
  unsigned int tout;

  Chip_ENET_StartMIIWrite(LPC_ETHERNET, PhyReg, Value);

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < ENET_MII_WR_TOUT; tout++) {
    if (Chip_ENET_IsMIIBusy(LPC_ETHERNET) == false) {
      break;
    }
  }
}


// Keil: function added to read PHY
unsigned short read_PHY (unsigned char PhyReg) 
{
  unsigned int tout;

  Chip_ENET_StartMIIRead(LPC_ETHERNET, PhyReg);

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < ENET_MII_RD_TOUT; tout++) {
    if (Chip_ENET_IsMIIBusy(LPC_ETHERNET) == false) {
      break;
    }
  }

  return Chip_ENET_ReadMIIData(LPC_ETHERNET);
}

/* Initialize MAC descriptors for simple packet receive/transmit */
STATIC void InitDescriptors(void)
{
	int i;

	/* Setup the descriptor list to a default state */
	memset(pTXDescs, 0, ENET_NUM_TX_DESC * sizeof(ENET_TXDESC_T));
	memset(pTXStats, 0, ENET_NUM_TX_DESC * sizeof(ENET_TXSTAT_T));
	memset(pRXDescs, 0, ENET_NUM_RX_DESC * sizeof(ENET_RXDESC_T));
	memset(pRXStats, 0, ENET_NUM_RX_DESC * sizeof(ENET_RXSTAT_T));

	rxConsumeIdx = 0;
	txProduceIdx = 0;

	/* Build linked list, CPU is owner of descriptors */
	for (i = 0; i < ENET_NUM_RX_DESC; i++) {
		pRXDescs[i].Packet = (uint32_t) ENET_RX_BUF(i);
		pRXDescs[i].Control = ENET_RCTRL_SIZE(ENET_ETH_MAX_FLEN) | ENET_RCTRL_INT;
		pRXStats[i].StatusInfo = 0;
		pRXStats[i].StatusHashCRC = 0;
	}
	for (i = 0; i < ENET_NUM_TX_DESC; i++) {
		pTXDescs[i].Packet = (uint32_t) ENET_TX_BUF(i);
		pTXDescs[i].Control = ENET_TCTRL_INT;
		pTXStats[i].StatusInfo = 0;
	}

	/* Setup list pointers in Ethernet controller */
	Chip_ENET_InitTxDescriptors(LPC_ETHERNET, pTXDescs, pTXStats, ENET_NUM_TX_DESC);
	Chip_ENET_InitRxDescriptors(LPC_ETHERNET, pRXDescs, pRXStats, ENET_NUM_RX_DESC);
}

// configure port-pins for use with LAN-controller,
// reset it and send the configuration-sequence

void Init_EMAC(void)
{
// Keil: function modified to access the EMAC
// Initializes the EMAC ethernet controller
  unsigned int regv,tout,id1,id2;

// Valor de configuracao dos GPIOs: funcao 0, com pull-up e histerese
  uint32_t gpio_mode = IOCON_MODE_INACT;

  /* Enable P1 Ethernet Pins. */
  Chip_IOCON_PinMux(LPC_IOCON, 1,  0, gpio_mode, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1,  1, gpio_mode, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1,  4, gpio_mode, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1,  8, gpio_mode, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1,  9, gpio_mode, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1, 10, gpio_mode, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1, 14, gpio_mode | IOCON_DIGMODE_EN, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1, 15, gpio_mode, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1, 16, gpio_mode | IOCON_DIGMODE_EN, IOCON_FUNC1);
  Chip_IOCON_PinMux(LPC_IOCON, 1, 17, gpio_mode | IOCON_DIGMODE_EN, IOCON_FUNC1);

  /* Initialize the EMAC controller. */
  Chip_ENET_Init(LPC_ETHERNET, true);

  /* Configure Reduced MII interface. */
  Chip_ENET_SetupMII(LPC_ETHERNET, Chip_ENET_FindMIIDiv(LPC_ETHERNET, 25000000), DP83848C_DEF_ADR);

  /* Put the DP83848C in reset mode */
  write_PHY (PHY_REG_BMCR, 0x8000);

  /* Wait for hardware reset to end. */
  for (tout = 0; tout < 0x100000; tout++) {
    regv = read_PHY (PHY_REG_BMCR);
    if (!(regv & 0x8000)) {
      /* Reset complete */
      break;
    }
  }

  /* Check if this is a DP83848C PHY. */
  id1 = read_PHY (PHY_REG_IDR1);
  id2 = read_PHY (PHY_REG_IDR2);
  if (((id1 << 16) | (id2 & 0xFFF0)) == DP83848C_ID) {
    /* Configure the PHY device */

    /* Use autonegotiation about the link speed. */
    write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
    /* Wait to complete Auto_Negotiation. */
    for (tout = 0; tout < 0x100000; tout++) {
      regv = read_PHY (PHY_REG_BMSR);
      if (regv & 0x0020) {
        /* Autonegotiation Complete. */
        break;
      }
    }
  }

  /* Check the link status. */
  for (tout = 0; tout < 0x10000; tout++) {
    regv = read_PHY (PHY_REG_STS);
    if (regv & 0x0001) {
      /* Link is on. */
      break;
    }
  }

  /* Configure Full/Half Duplex mode. */
  if (regv & 0x0004) {
    /* Full duplex is enabled. */
	Chip_ENET_SetFullDuplex(LPC_ETHERNET);
  }
  else {
    /* Half duplex mode. */
	Chip_ENET_SetHalfDuplex(LPC_ETHERNET);
  }

  /* Configure 100MBit/10MBit mode. */
  if (regv & 0x0002) {
    /* 10MBit mode. */
	Chip_ENET_Set10Mbps(LPC_ETHERNET);
  }
  else {
    /* 100MBit mode. */
	Chip_ENET_Set100Mbps(LPC_ETHERNET);
  }

  /* Set the Ethernet MAC Address registers */
  Chip_ENET_SetADDR(LPC_ETHERNET, mac_addr);

  /* Initialize Tx and Rx DMA Descriptors */
  InitDescriptors();

  /* Receive Broadcast and Perfect Match Packets */
  Chip_ENET_EnableRXFilter(LPC_ETHERNET, ENET_RXFILTERCTRL_ABE | ENET_RXFILTERCTRL_APE);

  /* Enable EMAC interrupts. */
  Chip_ENET_EnableInt(LPC_ETHERNET, ENET_INT_RXDONE | ENET_INT_TXDONE | ENET_INT_RXOVERRUN);

  /* Reset all interrupts */
  Chip_ENET_ClearIntStatus(LPC_ETHERNET, 0xFFFF);

  /* Enable receive and transmit mode of MAC Ethernet core */
  Chip_ENET_TXEnable(LPC_ETHERNET);
  Chip_ENET_RXEnable(LPC_ETHERNET);

  NVIC_EnableIRQ(ETHERNET_IRQn);
}


// reads a word in little-endian byte order from RX_BUFFER

unsigned short ReadFrame_EMAC(void)
{
  return (*rptr++);
}

// copies bytes from frame port to MCU-memory
// NOTES: * an odd number of byte may only be transfered
//          if the frame is read to the end!
//        * MCU-memory MUST start at word-boundary

void CopyFromFrame_EMAC(void *Dest, unsigned short Size)
{
  unsigned short * piDest;                       // Keil: Pointer added to correct expression

  piDest = Dest;                                 // Keil: Line added
  while (Size > 1) {
    *piDest++ = ReadFrame_EMAC();
    Size -= 2;
  }
  
  if (Size) {                                         // check for leftover byte...
    *(unsigned char *)piDest = (char)ReadFrame_EMAC();// the LAN-Controller will return 0
  }                                                   // for the highbyte
}

// does a dummy read on frame-I/O-port
// NOTE: only an even number of bytes is read!

void DummyReadFrame_EMAC(unsigned short Size)    // discards an EVEN number of bytes
{                                                // from RX-fifo
  while (Size > 1) {
    ReadFrame_EMAC();
    Size -= 2;
  }
}

// Reads the length of the received ethernet frame and checks if the 
// destination address is a broadcast message or not
// returns the frame length
unsigned short StartReadFrame(void) {
  unsigned short RxLen;
  unsigned int idx;

  idx = Chip_ENET_GetRXConsumeIndex(LPC_ETHERNET);
  RxLen = ENET_RINFO_SIZE(pRXStats[idx].StatusInfo) - 4;
  rptr = (unsigned short *)pRXDescs[idx].Packet;
  return(RxLen);
}

void EndReadFrame(void) {
  /* DMA free packet. */
	Chip_ENET_IncRXConsumeIndex(LPC_ETHERNET);
}

unsigned int CheckFrameReceived(void) {             // Packet received ?

	// more packets received ?
  if (Chip_ENET_GetRXProduceIndex(LPC_ETHERNET) != Chip_ENET_GetRXConsumeIndex(LPC_ETHERNET))
    return(1);
  else 
    return(0);
}

// requests space in EMAC memory for storing an outgoing frame

void RequestSend(unsigned short FrameSize)
{
  unsigned int idx;

  idx  = Chip_ENET_GetTXProduceIndex(LPC_ETHERNET);
  tptr = (unsigned short *)pTXDescs[idx].Packet;
  pTXDescs[idx].Control = FrameSize | ENET_TCTRL_LAST | ENET_TCTRL_INT;
}

// check if ethernet controller is ready to accept the
// frame we want to send

unsigned int Rdy4Tx(void)
{
  return (1);   // the ethernet controller transmits much faster
}               // than the CPU can load its buffers


// writes a word in little-endian byte order to TX_BUFFER
void WriteFrame_EMAC(unsigned short Data)
{
  *tptr++ = Data;
}

// copies bytes from MCU-memory to frame port
// NOTES: * an odd number of byte may only be transfered
//          if the frame is written to the end!
//        * MCU-memory MUST start at word-boundary

void CopyToFrame_EMAC_Start(void *Source, unsigned int Size)
{
  unsigned short * piSource;
  
  piSource = Source;
  Size = (Size + 1) & 0xFFFE;    // round Size up to next even number
  while (Size > 0) {
    WriteFrame_EMAC(*piSource++);
    Size -= 2;
  }
}

void CopyToFrame_EMAC_End(void)
{
  Chip_ENET_IncTXProduceIndex(LPC_ETHERNET);
}

void CopyToFrame_EMAC(void *Source, unsigned int Size)
{
	CopyToFrame_EMAC_Start(Source, Size);
	CopyToFrame_EMAC_End();
}

