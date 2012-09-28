#include "poptools.h"
#include <modbus_rtu.h>

struct MODBUS_Device MBDev_TCP;
struct MODBUS_Device MBDev_Serial;

MODBUS_HANDLER_TX(Modbus_TCP_Tx)
{
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	int iResult;
	char strIP[20];

	sprintf(strIP, "%d.%d.%d.%d", Prog.settings.ip[0], Prog.settings.ip[1], Prog.settings.ip[2], Prog.settings.ip[3]);

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(strIP, "502", &hints, &result);
	if (iResult != 0) {
		Error("getaddrinfo failed: %d", iResult);
		return 0;
	}

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr=result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		Error("Error at socket(): %ld", WSAGetLastError());
		freeaddrinfo(result);
		return 0;
	}

	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		Error(_("Unable to connect to server!"));
		return 0;
	}

	// Send an initial buffer
	iResult = send(ConnectSocket, (const char *)data, size, 0);
	if (iResult == SOCKET_ERROR) {
		Error("send failed: %d", WSAGetLastError());
		closesocket(ConnectSocket);
		return 0;
	}

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		Error("shutdown failed: %d", WSAGetLastError());
		closesocket(ConnectSocket);
		return 0;
	}

	iResult = recv(ConnectSocket, (char *)data, MODBUS_BUFFER_SIZE, 0);
	if (iResult == 0)
		Error(_("Connection closed"));
	else if(iResult < 0) {
		iResult = 0;
		Error("recv failed: %d", WSAGetLastError());
	}

	// cleanup
	closesocket(ConnectSocket);

	return iResult;
}

HANDLE hCommPort = NULL;
	
bool OpenCOMPort(unsigned int iPort, DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits)
{
	TCHAR lpszCommPort[10];

	if(!iPort) {
		iPort = LoadCOMPorts(0, 0, 0);
	}

	StringCchPrintf(lpszCommPort, sizeof(lpszCommPort) / sizeof(TCHAR), "//./COM%d", iPort);

	hCommPort = ::CreateFile( lpszCommPort,
		   GENERIC_READ|GENERIC_WRITE,  // access ( read and write)
		   0,                           // (share) 0:cannot share the
										// COM port
		   0,                           // security  (None)
		   OPEN_EXISTING,               // creation : open_existing
		   NULL,        // we want overlapped operation
		   0                            // no templates file for
										// COM port...
		   );

	DCB dcb = {0};
	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(hCommPort,&dcb))
	{
		CloseCOMPort();
		return false;
	}

	dcb.BaudRate    = dwBaudRate;
	dcb.ByteSize    = bByteSize;
	dcb.Parity      = bParity;
	dcb.StopBits    = bStopBits;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = DTR_CONTROL_DISABLE;

	if (!SetCommState(hCommPort, &dcb))
	{
		CloseCOMPort();
		return false;
	}

	DWORD dwFunction;

	dwFunction = CLRRTS; // SETRTS;
	EscapeCommFunction(hCommPort, dwFunction);

	dwFunction = CLRDTR; // SETDTR;
	EscapeCommFunction(hCommPort, dwFunction);

	COMMTIMEOUTS comTimeOut;                   

	comTimeOut.ReadIntervalTimeout = 5;
	comTimeOut.ReadTotalTimeoutMultiplier = 3;
	comTimeOut.ReadTotalTimeoutConstant = 2;
	comTimeOut.WriteTotalTimeoutMultiplier = 3;
	comTimeOut.WriteTotalTimeoutConstant = 2;

	if (!SetCommTimeouts(hCommPort, &comTimeOut))
	{
		CloseCOMPort();
		return false;
	}

	return true;
}

void CloseCOMPort(void)
{
	if(hCommPort != NULL) {
		CloseHandle(hCommPort);
		hCommPort = NULL;
	}
}

MODBUS_HANDLER_TX(Modbus_Serial_Tx)
{
	DWORD writenBytes = 0, readedBytes = 0;

	PurgeComm(hCommPort, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);

	if (WriteFile(hCommPort, data, size, &writenBytes, NULL) == 0) {
		if (GetLastError() != ERROR_IO_PENDING) {
			return 0;
		}
	}

	if (writenBytes == size) {
		Sleep(100);
		ReadFile(hCommPort, data, MODBUS_BUFFER_SIZE, &readedBytes,	NULL);
	}

	return readedBytes;
}

void Init_MBDev(void)
{
	Modbus_RTU_Init(&MBDev_TCP);

	MBDev_TCP.identification.Id                 = 0x01;
	MBDev_TCP.identification.VendorName         = "Tecnequip";
	MBDev_TCP.identification.ProductCode        = "POPTools";
	MBDev_TCP.identification.MajorMinorRevision = "v1.4";

	MBDev_TCP.hl      = NULL;
	MBDev_TCP.hl_size = 0;
	MBDev_TCP.mode    = MODBUS_MODE_TCP_MASTER;
	MBDev_TCP.TX      = Modbus_TCP_Tx;

	Modbus_RTU_Init(&MBDev_Serial);

	MBDev_Serial.identification.Id                 = 0x01;
	MBDev_Serial.identification.VendorName         = "Tecnequip";
	MBDev_Serial.identification.ProductCode        = "POPTools";
	MBDev_Serial.identification.MajorMinorRevision = "v1.4";

	MBDev_Serial.hl      = NULL;
	MBDev_Serial.hl_size = 0;
	MBDev_Serial.mode    = MODBUS_MODE_MASTER;
	MBDev_Serial.TX      = Modbus_Serial_Tx;
}
