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

#include "netif/emac.h"
#include "arch/lpc17xx.h"

static volatile unsigned short *rptr;
static volatile unsigned short *tptr;

// easyWEB internal function
// help function to swap the byte order of a WORD

static unsigned short SwapBytes(unsigned short Data)
{
  return (Data >> 8) | (Data << 8);
}

// Keil: function added to write PHY
void write_PHY (int PhyReg, int Value)
{
  unsigned int tout;

  EMAC->MADR = DP83848C_DEF_ADR | PhyReg;
  EMAC->MWTD = Value;

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < MII_WR_TOUT; tout++) {
    if ((EMAC->MIND & MIND_BUSY) == 0) {
      break;
    }
  }
}


// Keil: function added to read PHY
unsigned short read_PHY (unsigned char PhyReg)
{
  unsigned int tout;

  EMAC->MADR = DP83848C_DEF_ADR | PhyReg;
  EMAC->MCMD = MCMD_READ;

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < MII_RD_TOUT; tout++) {
    if ((EMAC->MIND & MIND_BUSY) == 0) {
      break;
    }
  }
  EMAC->MCMD = 0;
  return (EMAC->MRDD);
}


// Keil: function added to initialize Rx Descriptors
void rx_descr_init (void)
{
  unsigned int i;

  for (i = 0; i < NUM_RX_FRAG; i++) {
    RX_DESC_PACKET(i)  = RX_BUF(i);
    RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
    RX_STAT_INFO(i)    = 0;
    RX_STAT_HASHCRC(i) = 0;
  }

  /* Set EMAC Receive Descriptor Registers. */
  EMAC->RxDescriptor    = RX_DESC_BASE;
  EMAC->RxStatus        = RX_STAT_BASE;
  EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

  /* Rx Descriptors Point to 0 */
  EMAC->RxConsumeIndex  = 0;
}


// Keil: function added to initialize Tx Descriptors
void tx_descr_init (void) {
  unsigned int i;

  for (i = 0; i < NUM_TX_FRAG; i++) {
    TX_DESC_PACKET(i) = TX_BUF(i);
    TX_DESC_CTRL(i)   = 0;
    TX_STAT_INFO(i)   = 0;
  }

  /* Set EMAC Transmit Descriptor Registers. */
  EMAC->TxDescriptor    = TX_DESC_BASE;
  EMAC->TxStatus        = TX_STAT_BASE;
  EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

  /* Tx Descriptors Point to 0 */
  EMAC->TxProduceIndex  = 0;
}


// configure port-pins for use with LAN-controller,
// reset it and send the configuration-sequence
#if 1
#define NATIONAL_PHY
#define uint32_t unsigned int
#define uint8_t unsigned char
#define uint16_t unsigned short

#define MAC_FRAG_SIZE       1536        /* Packet Fragment size 1536 Bytes   */

#define OLD_MAC_MODULE_ID  0x39022000  /* Rev. ID for first rev '-'         */
#define MCFG_CLK_DIV40     0x00000024  /* MDC = hclk / 20 */
#define MAC_MAX_FLEN        1536        /* Max. Ethernet Frame Size          */
#define FALSE 0
#define CORE_DEBUG printf


uint32_t mac_get_buffer(uint8_t * buffer)
{
  if(EMAC->TxProduceIndex + 1 == EMAC->TxConsumeIndex)
    return 0;

  *buffer = TX_BUF(EMAC->TxProduceIndex);

  return MAC_FRAG_SIZE;
}

void phy_write(uint32_t reg, uint32_t val)
{
  uint32_t tout;

  EMAC->MADR = DP83848C_DEF_ADR | reg;
  EMAC->MWTD = val;

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < MII_WR_TOUT; tout++)
    if ((EMAC->MIND & MIND_BUSY) == 0)
      break;
}

uint16_t phy_read(uint8_t reg)
{
  uint32_t tout;

  EMAC->MADR = DP83848C_DEF_ADR | reg;
  EMAC->MCMD = MCMD_READ;

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < MII_RD_TOUT; tout++)
    if ((EMAC->MIND & MIND_BUSY) == 0)
      break;

  EMAC->MCMD = 0;
  return (EMAC->MRDD);
}

void mac_rx_descr_init(void)
{
  uint32_t i;

  for (i = 0; i < NUM_RX_FRAG; i++)
  {
    RX_DESC_PACKET(i)  = RX_BUF(i);
    RX_DESC_CTRL(i)    = RCTRL_INT | (MAC_FRAG_SIZE-1);
    RX_STAT_INFO(i)    = 0;
    RX_STAT_HASHCRC(i) = 0;
  }

  /* Set EMAC Receive Descriptor Registers. */
  EMAC->RxDescriptor    = RX_DESC_BASE;
  EMAC->RxStatus       = RX_STAT_BASE;
  EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

  /* Rx Descriptors Point to 0 */
  EMAC->RxConsumeIndex  = 0;
}

void mac_tx_descr_init(void)
{
  uint32_t i;

  for (i = 0; i < NUM_TX_FRAG; i++)
  {
    TX_DESC_PACKET(i) = TX_BUF(i);
    TX_DESC_CTRL(i)   = (1<<31) | (1<<30) | (1<<29) | (1<<28) | (1<<26) | (MAC_FRAG_SIZE-1);
    TX_STAT_INFO(i)   = 0;
  }

  /* Set EMAC Transmit Descriptor Registers. */
  EMAC->TxDescriptor    = TX_DESC_BASE;
  EMAC->TxStatus       = TX_STAT_BASE;
  EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

  /* Tx Descriptors Point to 0 */
  EMAC->TxProduceIndex  = 0;
}




int Init_EMAC(void)
{
  /* Initialize the EMAC ethernet controller. */
  uint32_t regv, tout, id1, id2;

  /* Power Up the EMAC controller. */
  SC->PCONP |= 0x40000000;

  /* Enable P1 Ethernet Pins. */
  PINCON->PINSEL2 &= ~0xF03F030F;
  PINCON->PINSEL2 |=  0x50150105;

  PINCON->PINSEL3 = (PINCON->PINSEL3 & ~0x0000000F) | 0x00000005;

  /* Reset all EMAC internal modules. */
  EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX |
             MAC1_SIM_RES | MAC1_SOFT_RES;
  EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;

  /* A short delay after reset. */
  for (tout = 100; tout; tout--);

  /* Initialize MAC control registers. */
  EMAC->MAC1 = MAC1_PASS_ALL;
  EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
  EMAC->MAXF = MAC_MAX_FLEN;
  EMAC->CLRT = CLRT_DEF;
  EMAC->IPGR = IPGR_DEF;

/*PCLK=25MHz, clock select=10, MDC=25/10=2,5MHz */
  /* Enable Reduced MII interface. */
  EMAC->MCFG = MCFG_CLK_DIV40 | MCFG_RES_MII;
  for (tout = 100; tout; tout--);
  EMAC->MCFG = MCFG_CLK_DIV40;

  /* Enable Reduced MII interface. */
  EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM /*| CR_PASS_RX_FILT*/;

  /* Reset Reduced MII Logic. */
  EMAC->SUPP = SUPP_RES_RMII /*| SUPP_SPEED*/;
  for (tout = 100; tout; tout--);
  EMAC->SUPP = 0 /*SUPP_SPEED*/;

  /* Put the PHY in reset mode */
  phy_write(PHY_REG_BMCR, 0x8000);
  //for (tout = 1000; tout; tout--);

  /* Wait for hardware reset to end. */
  for (tout = 0; tout < 0x100000; tout++) {
    regv = phy_read(PHY_REG_BMCR);
#if defined (NATIONAL_PHY)
    if (!(regv & 0x8000))
#elif defined (MICREL_PHY)
  if (!(regv & 0x8800))
#else
  #error "No board!"
#endif
  {
      /* Reset complete */
      break;
    }
  }

  if (tout >= 0x100000)
  {
    DEBUG("Eth: Timeout on hardware reset !\n");
    return FALSE; /* reset failed */
  }

  /* Check if this is a DP83848C PHY. */
  id1 = phy_read(PHY_REG_IDR1);
  id2 = phy_read(PHY_REG_IDR2);

#if defined (NATIONAL_PHY)
    if (((id1 << 16) | (id2 & 0xFFF0)) != DP83848C_ID)
#elif defined (MICREL_PHY)
    if (((id1 << 16) | (id2 & 0xFFF0)) != KSZ8001_ID)
#else
  #error "No board"
#endif
  /* Configure the PHY device */
#if defined (_10MBIT_)
  /* Connect at 10MBit */
  phy_write(PHY_REG_BMCR, PHY_FULLD_10M);
#elif defined (_100MBIT_)
  /* Connect at 100MBit */
  phy_write(PHY_REG_BMCR, PHY_FULLD_100M);
#else
  /* Use autonegotiation about the link speed. */
  phy_write(PHY_REG_BMCR, PHY_AUTO_NEG);
#endif

  return 1;
}

int Init_EMAC_phase2(void)
{
  /* Initialize the EMAC ethernet controller. */
  uint32_t regv, tout/*, id1, id2*/;

  if (!(phy_read(PHY_REG_BMSR) & 0x0020))
    return 0;

  /* Check the link status. */
  for (tout = 0; tout < 0x10000; tout++)
  {
    /* Link is on. */

#if defined (NATIONAL_PHY)
    regv = phy_read(PHY_REG_STS);
    if (regv & 0x0001)
#elif defined (MICREL_PHY)
    regv = phy_read(PHY_REG_100TPCR);
    if (regv & 0x001C)
#else
  #error "No board"
#endif
      break;
  }

  if (tout >= 0x10000)
  {
    DEBUG("Eth: Timeout on check link status !\n");
    return FALSE; // auto_neg failed
  }

  /* Configure Full/Half Duplex mode. */
#if defined (NATIONAL_PHY)
  if (regv & 0x0004)
#elif defined (MICREL_PHY)
  if (regv & 0x0010)
#else
  #error "No board"
#endif
  {
    /* Full duplex is enabled. */
    EMAC->MAC2    |= MAC2_FULL_DUP;
    EMAC->Command |= CR_FULL_DUP;
    EMAC->IPGT     = IPGT_FULL_DUP;
  }
  else
    /* Half duplex mode. */
    EMAC->IPGT = IPGT_HALF_DUP;

  /* Configure 100MBit/10MBit mode. */
#if defined (NATIONAL_PHY)
  if (regv & 0x0002)
#elif defined (MICREL_PHY)
   if (regv & 0x0004)
#else
  #error "No board"
#endif
    /* 10MBit mode. */
    EMAC->SUPP = 0;
  else
    /* 100MBit mode. */
    EMAC->SUPP = SUPP_SPEED;

  /* Set the Ethernet MAC Address registers */
  EMAC -> SA0 = (MYMAC_6 << 8) | MYMAC_5;
  EMAC -> SA1 = (MYMAC_4 << 8) | MYMAC_3;
  EMAC -> SA2 = (MYMAC_2 << 8) | MYMAC_1;

  /* Initialize Tx and Rx DMA Descriptors */
  mac_rx_descr_init ();
  mac_tx_descr_init ();

  /* Receive Broadcast and Perfect Match Packets */
  EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

  /* Enable EMAC interrupts. */
  //EMAC->INTENABLE = INT_RX_DONE | INT_TX_DONE;
  //EMAC->INTENABLE = INT_RX_DONE | INT_RX_OVERRUN | INT_RX_ERR | INT_TX_ERR;
  EMAC->IntEnable = INT_RX_DONE;

  /* Reset all interrupts */
  EMAC->IntClear  = 0xFFFF;

  /* Enable receive and transmit mode of MAC Ethernet core */
  EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
  EMAC->MAC1     |= MAC1_REC_EN;

  /* Configure VIC for EMAC interrupt. */
  //VICVectAddrxx = (UNS_32)xx;
//   vic_install_irq(EEMAC->INT, (void *)mac_int, LOWEST_PRIORITY);
  return 1;
}
#else
void Init_EMAC(void)
{
// Keil: function modified to access the EMAC
// Initializes the EMAC ethernet controller
  unsigned int regv,tout,id1,id2;
  printf("%s:%d\n", __FUNCTION__, __LINE__);
#if 0
  /* Enable P1 Ethernet Pins. */
  PINSEL2 = 0x55555555;
  PINSEL3 = (PINSEL3 & ~0x0000000F) | 0x00000005;

  /* Power Up the EMAC controller. */
  PCONP |= 0x40000000;

#else
#define OLD_MAC_MODULE_ID  0x39022000
  /* Power Up the EMAC controller. */
  PCONP |= 0x40000000;

  /* Enable P1 Ethernet Pins. */
  PINSEL2 &= ~0xF03F330F;

  if (EMAC->MODULEID == OLD_EEMAC->MODULE_ID)  /* For the first silicon rev.'-' ID P1.6 should be set. */
    PINSEL2 |= 0x50151105;
  else /* on rev. 'A' and later, P1.6 should NOT be set. */
    PINSEL2 |= 0x50150105;

  PINSEL3 = (PINSEL3 & ~0x0000000F) | 0x00000005;
#endif

  /* Reset all EMAC internal modules. */
  EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX |
             MAC1_SIM_RES | MAC1_SOFT_RES;
  EMAC -> Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;

  printf("%s:%d\n", __FUNCTION__, __LINE__);
  /* A short delay after reset. */
  for (tout = 100; tout; tout--);
  printf("%s:%d\n", __FUNCTION__, __LINE__);

  /* Initialize MAC control registers. */
  EMAC -> MAC1 = MAC1_PASS_ALL;
  EMAC -> MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
  EMAC -> MAXF = ETH_MAX_FLEN;
  EMAC -> CLRT = CLRT_DEF;
  EMAC -> IPGR = IPGR_DEF;

  printf("%s:%d\n", __FUNCTION__, __LINE__);
  /* Enable Reduced MII interface. */
  EMAC -> Command = CR_RMII | CR_PASS_RUNT_FRM;

  /* Reset Reduced MII Logic. */
  EMAC->SUPP = SUPP_RES_RMII;
  printf("%s:%d\n", __FUNCTION__, __LINE__);
  for (tout = 100; tout; tout--);
  EMAC->SUPP = 0;

  /* Put the DP83848C in reset mode */
  write_PHY (PHY_REG_BMCR, 0x8000);

  printf("%s:%d\n", __FUNCTION__, __LINE__);
  /* Wait for hardware reset to end. */
  for (tout = 0; tout < 0x100000; tout++) {
    regv = read_PHY (PHY_REG_BMCR);
    if (!(regv & 0x8000)) {
      /* Reset complete */
      break;
    }
  }
  printf("%s:%d\n", __FUNCTION__, __LINE__);

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

  printf("%s:%d\n", __FUNCTION__, __LINE__);
  /* Check the link status. */
  for (tout = 0; tout < 0x10000; tout++) {
    regv = read_PHY (PHY_REG_STS);
    if (regv & 0x0001) {
      /* Link is on. */
      break;
    }
  }

  printf("%s:%d\n", __FUNCTION__, __LINE__);
  /* Configure Full/Half Duplex mode. */
  if (regv & 0x0004) {
    /* Full duplex is enabled. */
    EMAC->MAC2    |= MAC2_FULL_DUP;
    EMAC->COMMAND |= CR_FULL_DUP;
    EMAC->IPGT     = IPGT_FULL_DUP;
  }
  else {
    /* Half duplex mode. */
    EMAC->IPGT = IPGT_HALF_DUP;
  }

  printf("%s:%d\n", __FUNCTION__, __LINE__);
  /* Configure 100MBit/10MBit mode. */
  if (regv & 0x0002) {
    /* 10MBit mode. */
    EMAC->SUPP = 0;
  }
  else {
    /* 100MBit mode. */
    EMAC->SUPP = SUPP_SPEED;
  }

  printf("%s:%d\n", __FUNCTION__, __LINE__);
  /* Set the Ethernet MAC Address registers */
  EMAC -> SA0 = (MYMAC_6 << 8) | MYMAC_5;
  EMAC -> SA1 = (MYMAC_4 << 8) | MYMAC_3;
  EMAC -> SA2 = (MYMAC_2 << 8) | MYMAC_1;

  /* Initialize Tx and Rx DMA Descriptors */
  printf("%s:%d\n", __FUNCTION__, __LINE__);
  rx_descr_init ();
  printf("%s:%d\n", __FUNCTION__, __LINE__);
  tx_descr_init ();
  printf("%s:%d\n", __FUNCTION__, __LINE__);

  /* Receive Broadcast and Perfect Match Packets */
  EMAC -> RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

  /* Enable EMAC interrupts. */
  EMAC -> IntEnable = INT_RX_DONE | INT_TX_DONE;

  /* Reset all interrupts */
  EMAC -> IntClear  = 0xFFFF;

  /* Enable receive and transmit mode of MAC Ethernet core */
  EMAC -> Command  |= (CR_RX_EN | CR_TX_EN);
  EMAC -> MAC1     |= MAC1_REC_EN;
}
#endif

// reads a word in little-endian byte order from RX_BUFFER

unsigned short ReadFrame_EMAC(void)
{
  return (*rptr++);
}

// reads a word in big-endian byte order from RX_FRAME_PORT
// (useful to avoid permanent byte-swapping while reading
// TCP/IP-data)

unsigned short ReadFrameBE_EMAC(void)
{
  unsigned short ReturnValue;

  ReturnValue = SwapBytes (*rptr++);
  return (ReturnValue);
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
  volatile unsigned short RxLen;
  volatile unsigned int idx;

  idx = EMAC->RxConsumeIndex;
  RxLen = (RX_STAT_INFO(idx) & RINFO_SIZE) - 3;
  rptr = (unsigned short *)RX_DESC_PACKET(idx);
  return(RxLen);
}

void EndReadFrame(void) {
  unsigned int idx;

  /* DMA free packet. */
  idx = EMAC->RxConsumeIndex;

  if (++idx == NUM_RX_FRAG)
    idx = 0;

  EMAC->RxConsumeIndex = idx;
}

unsigned int CheckFrameReceived(void) {             // Packet received ?

  if (EMAC->RxProduceIndex != EMAC->RxConsumeIndex)     // more packets received ?
    return(1);
  else
    return(0);
}

// requests space in EMAC memory for storing an outgoing frame

void RequestSend(void)
{
  unsigned int idx;

  idx  = EMAC->TxProduceIndex;
  tptr = (unsigned short *)TX_DESC_PACKET(idx);
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

void CopyToFrame_EMAC(void *Source, unsigned int Size)
{
  unsigned short * piSource;

  piSource = Source;
  Size = (Size + 1) & 0xFFFE;    // round Size up to next even number
  while (Size > 0) {
    WriteFrame_EMAC(*piSource++);
    Size -= 2;
  }
}

void DoSend_EMAC(unsigned short FrameSize)
{
  unsigned int idx;

  idx = EMAC->TxProduceIndex;
  TX_DESC_CTRL(idx) = FrameSize | TCTRL_LAST;
  if (++idx == NUM_TX_FRAG) idx = 0;
  EMAC->TxProduceIndex = idx;
}

