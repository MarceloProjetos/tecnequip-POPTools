/******************************************************************************
* Comunicação CAN
******************************************************************************/
#include "chip.h"
#include "can.h"

#define MAX_RETRIES 1

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#if defined(CHIP_LPC175X_6X)
#define CAN_CTRL_NO         1
#else
#define CAN_CTRL_NO         0
#endif
#if (CAN_CTRL_NO == 0)
#define LPC_CAN             (LPC_CAN1)
#else
#define LPC_CAN             (LPC_CAN2)
#endif
#define AF_LUT_USED         0
#if AF_LUT_USED
#define FULL_CAN_AF_USED    1
#endif
#define CAN_TX_MSG_STD_ID (0x200)
#define CAN_TX_MSG_REMOTE_STD_ID (0x300)
#define CAN_TX_MSG_EXT_ID (0x10000200)
#define CAN_RX_MSG_ID (0x100)

#if AF_LUT_USED
#if FULL_CAN_AF_USED
CAN_STD_ID_ENTRY_T FullCANSection[] = {
	{CAN_CTRL_NO, 0, 0x03},
	{CAN_CTRL_NO, 0, 0x05},
	{CAN_CTRL_NO, 0, 0x07},
	{CAN_CTRL_NO, 0, 0x09},
};
#endif
CAN_STD_ID_ENTRY_T SFFSection[] = {
	{CAN_CTRL_NO, 0, 0x30},
	{CAN_CTRL_NO, 0, 0x50},
	{CAN_CTRL_NO, 0, 0x70},
	{CAN_CTRL_NO, 0, 0x90},
	{CAN_CTRL_NO, 0, 0xB0},
};
CAN_STD_ID_RANGE_ENTRY_T SffGrpSection[] = {
	{{CAN_CTRL_NO, 0, 0x300}, {CAN_CTRL_NO, 0, 0x400}},
	{{CAN_CTRL_NO, 0, 0x500}, {CAN_CTRL_NO, 0, 0x600}},
	{{CAN_CTRL_NO, 0, 0x700}, {CAN_CTRL_NO, 0, 0x780}},
};
CAN_EXT_ID_ENTRY_T EFFSection[] = {
	{CAN_CTRL_NO, ((1 << 11) | 0x03)},
	{CAN_CTRL_NO, ((1 << 11) | 0x05)},
	{CAN_CTRL_NO, ((1 << 11) | 0x07)},
	{CAN_CTRL_NO, ((1 << 11) | 0x09)},
};
CAN_EXT_ID_RANGE_ENTRY_T EffGrpSection[] = {
	{{CAN_CTRL_NO, ((1 << 11) | 0x300)}, {CAN_CTRL_NO, ((1 << 11) | 0x400)}},
	{{CAN_CTRL_NO, ((1 << 11) | 0x500)}, {CAN_CTRL_NO, ((1 << 11) | 0x600)}},
	{{CAN_CTRL_NO, ((1 << 11) | 0x700)}, {CAN_CTRL_NO, ((1 << 11) | 0x780)}},
};
CANAF_LUT_T AFSections = {
#if FULL_CAN_AF_USED
	FullCANSection, sizeof(FullCANSection) / sizeof(CAN_STD_ID_ENTRY_T),
#else
	NULL, 0,
#endif
	SFFSection, sizeof(SFFSection) / sizeof(CAN_STD_ID_ENTRY_T),
	SffGrpSection, sizeof(SffGrpSection) / sizeof(CAN_STD_ID_RANGE_ENTRY_T),
	EFFSection, sizeof(EFFSection) / sizeof(CAN_EXT_ID_ENTRY_T),
	EffGrpSection, sizeof(EffGrpSection) / sizeof(CAN_EXT_ID_RANGE_ENTRY_T),
};
#endif /*AF_LUT_USED*/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
static char WelcomeMenu[] = "\n\rHello NXP Semiconductors \r\n"
							"CAN DEMO : Use CAN to transmit and receive Message from CAN Analyzer\r\n"
							"CAN bit rate : 500kBit/s\r\n";

volatile unsigned int can_timeout = 0;
volatile unsigned int can_reset_timeout = 0;

unsigned int can_rx_index = 0;
unsigned int can_tx_index = 0;
unsigned int can_tx_count = 0;

CAN_MSG_T SendMsgBuf;
CAN_MSG_T RcvMsgBuf;
/*****************************************************************************
 * Private functions
 ****************************************************************************/
/* Print error */
static void PrintCANErrorInfo(uint32_t Status)
{
	if (Status & CAN_ICR_EI) {
		DEBUGOUT("Error Warning!\r\n");
	}
	if (Status & CAN_ICR_DOI) {
		DEBUGOUT("Data Overrun!\r\n");
	}
	if (Status & CAN_ICR_EPI) {
		DEBUGOUT("Error Passive!\r\n");
	}
	if (Status & CAN_ICR_ALI) {
		DEBUGOUT("Arbitration lost in the bit: %d(th)\r\n", CAN_ICR_ALCBIT_VAL(Status));
	}
	if (Status & CAN_ICR_BEI) {

		DEBUGOUT("Bus error !!!\r\n");

		if (Status & CAN_ICR_ERRDIR_RECEIVE) {
			DEBUGOUT("\t Error Direction: Transmiting\r\n");
		}
		else {
			DEBUGOUT("\t Error Direction: Receiving\r\n");
		}

		DEBUGOUT("\t Error Location: 0x%2x\r\n", CAN_ICR_ERRBIT_VAL(Status));
		DEBUGOUT("\t Error Type: 0x%1x\r\n", CAN_ICR_ERRC_VAL(Status));
	}
}

/* Print CAN Message */
static void PrintCANMsg(CAN_MSG_T *pMsg)
{
	uint8_t i;
	DEBUGOUT("\t**************************\r\n");
	DEBUGOUT("\tMessage Information: \r\n");
	DEBUGOUT("\tMessage Type: ");
	if (pMsg->ID & CAN_EXTEND_ID_USAGE) {
		DEBUGOUT(" Extend ID Message");
	}
	else {
		DEBUGOUT(" Standard ID Message");
	}
	if (pMsg->Type & CAN_REMOTE_MSG) {
		DEBUGOUT(", Remote Message");
	}
	DEBUGOUT("\r\n");
	DEBUGOUT("\tMessage ID :0x%x\r\n", (pMsg->ID & (~CAN_EXTEND_ID_USAGE)));
	DEBUGOUT("\tMessage Data :");
	for (i = 0; i < pMsg->DLC; i++)
		DEBUGOUT("%x ", pMsg->Data[i]);
	DEBUGOUT("\r\n\t**************************\r\n");
}

/* Reply remote message received */
static void ReplyRemoteMessage(CAN_MSG_T *pRcvMsg)
{
	CAN_MSG_T SendMsgBuf;
	CAN_BUFFER_ID_T   TxBuf;
	uint8_t i;
	SendMsgBuf.ID  = pRcvMsg->ID;
	SendMsgBuf.DLC = pRcvMsg->DLC;
	SendMsgBuf.Type = 0;
	for (i = 0; i < pRcvMsg->DLC; i++)
		SendMsgBuf.Data[i] = '0' + i;
	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);
	DEBUGOUT("Message Replied!!!\r\n");
	PrintCANMsg(&SendMsgBuf);
}

/* Reply message received */
static void ReplyNormalMessage(CAN_MSG_T *pRcvMsg)
{
	CAN_MSG_T SendMsgBuf = *pRcvMsg;
	CAN_BUFFER_ID_T   TxBuf;
	SendMsgBuf.ID = CAN_TX_MSG_STD_ID;
	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);
	DEBUGOUT("Message Replied!!!\r\n");
	PrintCANMsg(&SendMsgBuf);
}

#if AF_LUT_USED
/* Print entries in AF LUT */
static void PrintAFLUT(void)
{
	uint16_t i, num;
	CAN_STD_ID_ENTRY_T StdEntry;
	CAN_EXT_ID_ENTRY_T ExtEntry;
	CAN_STD_ID_RANGE_ENTRY_T StdGrpEntry;
	CAN_EXT_ID_RANGE_ENTRY_T ExtGrpEntry;
#if FULL_CAN_AF_USED
	/* Full CAN Table */
	DEBUGOUT("\tFULL CAN Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, i, &StdEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x, Dis: %1d\r\n",
				 i, StdEntry.CtrlNo, StdEntry.ID_11, StdEntry.Disable);
	}
#endif
	/* Standard ID Table */
	DEBUGOUT("\tIndividual Standard ID Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadSTDEntry(LPC_CANAF, LPC_CANAF_RAM, i, &StdEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x, Dis: %1d\r\n",
				 i, StdEntry.CtrlNo, StdEntry.ID_11, StdEntry.Disable);
	}
	/* Group Standard ID Table */
	DEBUGOUT("\tGroup Standard ID Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_GRP_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, i, &StdGrpEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x-0x%x, Dis: %1d\r\n",
				 i, StdGrpEntry.LowerID.CtrlNo, StdGrpEntry.LowerID.ID_11,
				 StdGrpEntry.UpperID.ID_11, StdGrpEntry.LowerID.Disable);
	}
	/* Extended ID Table */
	DEBUGOUT("\tExtended ID Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadEXTEntry(LPC_CANAF, LPC_CANAF_RAM, i, &ExtEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x,\r\n",
				 i, ExtEntry.CtrlNo, ExtEntry.ID_29);
	}
	/* Group Extended ID Table */
	DEBUGOUT("\tGroup Extended ID Table: \r\n");
	num = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_GRP_SEC);
	for (i = 0; i < num; i++) {
		Chip_CAN_ReadGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, i, &ExtGrpEntry);
		DEBUGOUT("\t\t%d: Controller ID: %d, ID: 0x%x-0x%x\r\n",
				 i, ExtGrpEntry.LowerID.CtrlNo, ExtGrpEntry.LowerID.ID_29,
				 ExtGrpEntry.UpperID.ID_29);
	}

}

/* Setup AF LUT */
static void SetupAFLUT(void)
{
	DEBUGOUT("Setup AF LUT... \r\n");
	Chip_CAN_SetAFLUT(LPC_CANAF, LPC_CANAF_RAM, &AFSections);
	//PrintAFLUT();
}

/* Insert/Remove entries to/from AF LUT */
static void ChangeAFLUT(void)
{
#if FULL_CAN_AF_USED
	CAN_STD_ID_ENTRY_T FullEntry = {CAN_CTRL_NO, 0, 0x0A};
#endif
	CAN_STD_ID_ENTRY_T StdEntry = {CAN_CTRL_NO, 0, 0xC0};
	CAN_EXT_ID_ENTRY_T ExtEntry = {CAN_CTRL_NO, ((1 << 11) | 0x0A)};
	CAN_STD_ID_RANGE_ENTRY_T StdGrpEntry = {{CAN_CTRL_NO, 0, 0x7A0}, {CAN_CTRL_NO, 0, 0x7B0}};
	CAN_EXT_ID_RANGE_ENTRY_T ExtGrpEntry = {{CAN_CTRL_NO, ((1 << 11) | 0x7A0)}, {CAN_CTRL_NO, ((1 << 11) | 0x7B0)}};

	//DEBUGOUT("Insert new entries into the current LUT... \r\n");
#if FULL_CAN_AF_USED
	/* Edit Full CAN Table */
	Chip_CAN_InsertFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, &FullEntry);
	FullEntry.ID_11 = 2;
	Chip_CAN_InsertFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, &FullEntry);
	FullEntry.ID_11 = 4;
	Chip_CAN_InsertFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, &FullEntry);
#endif /*FULL_CAN_AF_USED*/

	/* Edit Individual STD ID Table */
	Chip_CAN_InsertSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdEntry);
	StdEntry.ID_11 = 0x20;
	Chip_CAN_InsertSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdEntry);
	StdEntry.ID_11 = 0x40;
	Chip_CAN_InsertSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdEntry);

	/* Edit Individual EXT ID Table */
	Chip_CAN_InsertEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtEntry);
	ExtEntry.ID_29 = (1 << 11) | 0x02;
	Chip_CAN_InsertEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtEntry);
	ExtEntry.ID_29 = (1 << 11) | 0x04;
	Chip_CAN_InsertEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtEntry);

	/* Edit STD ID Group Table */
	Chip_CAN_InsertGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdGrpEntry);
	StdGrpEntry.LowerID.ID_11 = 0x200;
	StdGrpEntry.UpperID.ID_11 = 0x300;
	Chip_CAN_InsertGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdGrpEntry);
	StdGrpEntry.LowerID.ID_11 = 0x400;
	StdGrpEntry.UpperID.ID_11 = 0x500;
	Chip_CAN_InsertGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, &StdGrpEntry);

	/* Edit EXT ID Group Table */
	Chip_CAN_InsertGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtGrpEntry);
	ExtGrpEntry.LowerID.ID_29 = (1 << 11) | 0x200;
	ExtGrpEntry.UpperID.ID_29 = (1 << 11) | 0x300;
	Chip_CAN_InsertGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtGrpEntry);
	ExtGrpEntry.LowerID.ID_29 = (1 << 11) | 0x400;
	ExtGrpEntry.UpperID.ID_29 = (1 << 11) | 0x500;
	Chip_CAN_InsertGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, &ExtGrpEntry);

	//PrintAFLUT();

	//DEBUGOUT("Remove entries into the current LUT... \r\n");
	/* Remove entries from the current LUT */
#if FULL_CAN_AF_USED
	Chip_CAN_RemoveFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC) - 1);
	Chip_CAN_RemoveFullCANEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC) / 2);
#endif
	Chip_CAN_RemoveSTDEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveSTDEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_SEC) - 1);
	Chip_CAN_RemoveSTDEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_SEC) / 2);
	Chip_CAN_RemoveGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_GRP_SEC) - 1);
	Chip_CAN_RemoveGroupSTDEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_SFF_GRP_SEC) / 2);
	Chip_CAN_RemoveEXTEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveEXTEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_SEC) - 1);
	Chip_CAN_RemoveEXTEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_SEC) / 2);
	Chip_CAN_RemoveGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, 0);
	Chip_CAN_RemoveGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_GRP_SEC) - 1);
	Chip_CAN_RemoveGroupEXTEntry(LPC_CANAF, LPC_CANAF_RAM, Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_EFF_GRP_SEC) / 2);
	//PrintAFLUT();
}

#endif

void CAN_Init()
{
	// Pino que controla o CAN/CAN Select (CAN=0, CAN=1)
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 25, IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_HYS_EN);
	Chip_GPIO_SetDir(LPC_GPIO, 1, 25, 1); // Define pino como Saida
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 25, 1); // Configura nivel alto para selecionar CAN

	if (pCAN == LPC_CAN1) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_FUNC1 | IOCON_MODE_INACT /*| IOCON_MODE_PULLUP | IOCON_HYS_EN*/));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 1, (IOCON_FUNC1 | IOCON_MODE_INACT /*| IOCON_MODE_PULLUP | IOCON_HYS_EN*/));
	}
	else {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 4, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_HYS_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 5, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_HYS_EN));
	}

	DEBUGOUT(WelcomeMenu);
	Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);
	Chip_CAN_SetBitRate(LPC_CAN, 125000);
	//Chip_CAN_EnableInt(LPC_CAN, CAN_IER_BITMASK);

#if AF_LUT_USED
	SetupAFLUT();
	ChangeAFLUT();
	PrintAFLUT();
#if FULL_CAN_AF_USED
	Chip_CAN_ConfigFullCANInt(LPC_CANAF, ENABLE);
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_FULL_MODE);
#else
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_NORMAL_MODE);
#endif /*FULL_CAN_AF_USED*/
#else
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_BYBASS_MODE);
#endif /*AF_LUT_USED*/
	//NVIC_EnableIRQ(CAN_IRQn);

}


/* Returns clock for the peripheral block */
STATIC CHIP_SYSCTL_CLOCK_T Chip_CAN_GetClockIndex(LPC_CAN_T *pCAN)
{
	CHIP_SYSCTL_CLOCK_T clkCAN;

	if (pCAN == LPC_CAN1) {
		clkCAN = SYSCTL_CLOCK_CAN1;
	}
	else {
		clkCAN = SYSCTL_CLOCK_CAN2;
	}

	return clkCAN;
}

unsigned int CAN_Write(CAN_MSG_T SendMsgBuf)
{
	CAN_BUFFER_ID_T TxBuf;

	int CANEnabled = Chip_Clock_IsPeripheralClockEnabled(Chip_CAN_GetClockIndex(LPC_CAN));

	int timeout = 0;

	SendMsgBuf.ID = CAN_TX_MSG_STD_ID;
	SendMsgBuf.DLC = 4;
	SendMsgBuf.Type = 0;
	SendMsgBuf.Data[0] = 'A';
	SendMsgBuf.Data[1] = 'B';
	SendMsgBuf.Data[2] = 'C';
	SendMsgBuf.Data[3] = 'D';

	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);

	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);

	timeout = 1000;
	while ((Chip_CAN_GetStatus(LPC_CAN) & CAN_SR_TCS(TxBuf)) == 0 && --timeout) {}

	DEBUGOUT("Message Sent!!!\r\n");
	PrintCANMsg(&SendMsgBuf);

	SendMsgBuf.ID = CAN_TX_MSG_REMOTE_STD_ID;
	SendMsgBuf.Type = CAN_REMOTE_MSG;
	SendMsgBuf.DLC = 8;
	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);

	timeout = 1000;
	while ((Chip_CAN_GetStatus(LPC_CAN) & CAN_SR_TCS(TxBuf)) == 0 && --timeout) {}

	DEBUGOUT("Message Sent!!!\r\n");
	PrintCANMsg(&SendMsgBuf);

	SendMsgBuf.ID = CAN_EXTEND_ID_USAGE | CAN_TX_MSG_EXT_ID;
	SendMsgBuf.Type = 0;
	SendMsgBuf.Data[0] = 'E';
	SendMsgBuf.Data[1] = 'F';
	SendMsgBuf.Data[2] = 'G';
	SendMsgBuf.Data[3] = 'H';
	SendMsgBuf.Data[4] = 'I';
	SendMsgBuf.Data[5] = 'J';
	SendMsgBuf.Data[6] = 'K';
	SendMsgBuf.Data[7] = 'L';
	TxBuf = Chip_CAN_GetFreeTxBuf(LPC_CAN);
	Chip_CAN_Send(LPC_CAN, TxBuf, &SendMsgBuf);

	timeout = 1000;
	while ((Chip_CAN_GetStatus(LPC_CAN) & CAN_SR_TCS(TxBuf)) == 0 && --timeout) {}

	DEBUGOUT("Message Sent!!!\r\n");
	PrintCANMsg(&SendMsgBuf);

	return 1;

}

unsigned int CAN_Read(CAN_MSG_T RcvMsgBuf)
{
#if FULL_CAN_AF_USED
	uint16_t i = 0, FullCANEntryNum = 0;
#endif
	uint32_t IntStatus;

	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN);

	PrintCANErrorInfo(IntStatus);

	/* New Message came */
	if (IntStatus & CAN_ICR_RI) {
		Chip_CAN_Receive(LPC_CAN, &RcvMsgBuf);
		DEBUGOUT("Message Received!!!\r\n");
		PrintCANMsg(&RcvMsgBuf);

		if (RcvMsgBuf.Type & CAN_REMOTE_MSG) {
			ReplyRemoteMessage(&RcvMsgBuf);
		}
		else {
			ReplyNormalMessage(&RcvMsgBuf);
		}

	}
#if FULL_CAN_AF_USED
	FullCANEntryNum = Chip_CAN_GetEntriesNum(LPC_CANAF, LPC_CANAF_RAM, CANAF_RAM_FULLCAN_SEC);
	if (FullCANEntryNum > 64) {
		FullCANEntryNum = 64;
	}
	for (i = 0; i < FullCANEntryNum; i++)
		if (Chip_CAN_GetFullCANIntStatus(LPC_CANAF, i)) {
			uint8_t SCC;
			Chip_CAN_FullCANReceive(LPC_CANAF, LPC_CANAF_RAM, i, &RcvMsgBuf, &SCC);
			if (SCC == CAN_CTRL_NO) {
				DEBUGOUT("FullCAN Message Received!!!\r\n");
				PrintCANMsg(&RcvMsgBuf);
				if (RcvMsgBuf.Type & CAN_REMOTE_MSG) {
					ReplyRemoteMessage(&RcvMsgBuf);
				}
				else {
					ReplyNormalMessage(&RcvMsgBuf);
				}
			}
		}

#endif /*FULL_CAN_AF_USED*/
	return 0;

}

unsigned int CAN_WriteChar(unsigned char e)
{
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 0, e); // Configura nivel baixo
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 1, e); // Configura nivel baixo

	return 0;
}

unsigned int CAN_ReadChar(unsigned char * buffer)
{
	/*if(can_rx_index > 0) {
		can_rx_index--;
		*buffer = can_rx_buffer[0];
		memcpy(can_rx_buffer, can_rx_buffer + 1, can_rx_index);
		return 1;
	}*/

	return 0;
}

void CAN_Config(int CANbitRate)
{
	// Pino que controla o CAN/CAN Select (CAN=0, CAN=1)
	Chip_GPIO_SetDir(LPC_GPIO, 1, 25, 1); // Define pino como Saida
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 25, 1); // Configura nivel alto para selecionar CAN

	//Chip_CAN_IntEnable(LPC_CAN, CAN_IER_BITMASK);
	/* Configure CAN module registers */
	Chip_CAN_Init(LPC_CAN, LPC_CANAF, LPC_CANAF_RAM);

	/*CANbitRate Valid values are (in kbps): 10, 20, 50, 125, 250, 500, 800, 1000.*/
	Chip_CAN_SetBitRate(LPC_CAN, (CANbitRate * 1000));

	/* CAN module filters are not used, all messages with standard 11-bit */
	/* identifier will be received */
	/* Configure mask 0 so, that all messages with standard identifier are accepted */
	Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_BYBASS_MODE);

	#if AF_LUT_USED
		SetupAFLUT();
		ChangeAFLUT();
		PrintAFLUT();
		#if FULL_CAN_AF_USED
			Chip_CAN_FullCANIntConfig(LPC_CANAF, ENABLE);
			Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_FULL_MODE);
		#else
			Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_NORMAL_MODE);
		#endif /*FULL_CAN_AF_USED*/
	#else
		Chip_CAN_SetAFMode(LPC_CANAF, CAN_AF_BYBASS_MODE);
	#endif /*AF_LUT_USED*/

	//NVIC_EnableIRQ(CAN_IRQn);

}

void CAN_Handler(unsigned int cycle)
{
	unsigned int sz;
	//static unsigned int retries = 0;

	sz = CAN_Read(RcvMsgBuf);
	can_rx_index += sz;

	if(can_rx_index) {
		if(sz) {
			can_timeout  = cycle + 3;
		} else if(cycle >= can_timeout) {
			// Se aguardando por uma string formatada, nenhum protocolo deve interpretar strings
			// pois podem ser recebidos comandos invalidos que gerem erros ou mesmo o buffer ser descartado
			// antes de o objeto de string formatada conseguir interpretar os dados.
			/*if (WAITING_FOR_FMTSTR == 0) { // formatted string
				if (WAITING_FOR_USS == 1){ // uss
					USS_Ready(can_rx_buffer, can_rx_index);
					can_rx_index = 0;
					WAITING_FOR_USS = 0;
				} else if (WAITING_FOR_YASKAWA == 0) { // modbus
					Modbus_Request(&modbus_can, can_rx_buffer, can_rx_index);
					can_rx_index = 0;
				}
			}*/
		}
	}

  /*if(I_SerialReady) {
    retries = 0;
    I_SerialTimeout = 0;
    I_SerialAborted = 0;
    can_reset_timeout = cycle + 50;
  } else {
    if(cycle >= can_reset_timeout && !I_SerialTimeout && retries < MAX_RETRIES) {
      retries++;
      I_SerialTimeout = 1;
	  can_reset_timeout = cycle + 50;
    } else if (cycle >= can_reset_timeout && retries >= MAX_RETRIES && !I_SerialAborted) {
    	I_SerialAborted = 1;
    } else if (cycle >= (can_reset_timeout + 250)) {
      retries = 0;
      I_SerialReady = 1;
      I_SerialTimeout = 0;
      I_SerialAborted = 0;
	  WAITING_FOR_USS = 0;
	  WAITING_FOR_YASKAWA = 0;
    }
  }*/

}

