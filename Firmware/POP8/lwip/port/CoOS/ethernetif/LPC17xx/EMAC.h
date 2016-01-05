/*----------------------------------------------------------------------------
 *      LPC2378 Ethernet Definitions
 *----------------------------------------------------------------------------
 *      Name:    EMAC.H
 *      Purpose: Philips LPC2378 EMAC hardware definitions
 *----------------------------------------------------------------------------
 *      Copyright (c) 2006 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#ifndef __EMAC_H
#define __EMAC_H

/* EMAC Memory Buffer configuration for 16K Ethernet RAM. */
#define ENET_NUM_RX_DESC         4           /* Num.of RX Fragments 4*1536= 6.0kB */
#define ENET_NUM_TX_DESC         2           /* Num.of TX Fragments 3*1536= 4.6kB */

/* DP83848C PHY Registers */
#define PHY_REG_BMCR        0x00        /* Basic Mode Control Register       */
#define PHY_REG_BMSR        0x01        /* Basic Mode Status Register        */
#define PHY_REG_IDR1        0x02        /* PHY Identifier 1                  */
#define PHY_REG_IDR2        0x03        /* PHY Identifier 2                  */
#define PHY_REG_ANAR        0x04        /* Auto-Negotiation Advertisement    */
#define PHY_REG_ANLPAR      0x05        /* Auto-Neg. Link Partner Abitily    */
#define PHY_REG_ANER        0x06        /* Auto-Neg. Expansion Register      */
#define PHY_REG_ANNPTR      0x07        /* Auto-Neg. Next Page TX            */

/* PHY Extended Registers */
#define PHY_REG_STS         0x10        /* Status Register                   */
#define PHY_REG_MICR        0x11        /* MII Interrupt Control Register    */
#define PHY_REG_MISR        0x12        /* MII Interrupt Status Register     */
#define PHY_REG_FCSCR       0x14        /* False Carrier Sense Counter       */
#define PHY_REG_RECR        0x15        /* Receive Error Counter             */
#define PHY_REG_PCSR        0x16        /* PCS Sublayer Config. and Status   */
#define PHY_REG_RBR         0x17        /* RMII and Bypass Register          */
#define PHY_REG_LEDCR       0x18        /* LED Direct Control Register       */
#define PHY_REG_PHYCR       0x19        /* PHY Control Register              */
#define PHY_REG_10BTSCR     0x1A        /* 10Base-T Status/Control Register  */
#define PHY_REG_CDCTRL1     0x1B        /* CD Test Control and BIST Extens.  */
#define PHY_REG_EDCR        0x1D        /* Energy Detect Control Register    */

#define PHY_FULLD_100M      0x2100      /* Full Duplex 100Mbit               */
#define PHY_HALFD_100M      0x2000      /* Half Duplex 100Mbit               */
#define PHY_FULLD_10M       0x0100      /* Full Duplex 10Mbit                */
#define PHY_HALFD_10M       0x0000      /* Half Duplex 10MBit                */
#define PHY_AUTO_NEG        0x3000      /* Select Auto Negotiation           */

#define DP83848C_DEF_ADR    ((uint8_t)0x0001)      /* Default PHY device address        */
#define DP83848C_ID         0x20005C90  /* PHY Identifier                    */

// prototypes
void           Init_EMAC(void);
unsigned short ReadFrameBE_EMAC(void);
void           CopyToFrame_EMAC(void *Source, unsigned int Size);
void           CopyFromFrame_EMAC(void *Dest, unsigned short Size);
void           DummyReadFrame_EMAC(unsigned short Size);
unsigned short StartReadFrame(void);
void           EndReadFrame(void);
unsigned int   CheckFrameReceived(void);
void           RequestSend(unsigned short FrameSize);
unsigned int   Rdy4Tx(void);
void CopyToFrame_EMAC_Start(void *Source, unsigned int Size);
void CopyToFrame_EMAC_End(void);

#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

