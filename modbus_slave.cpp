#include "poptools.h"
#include <modbus_rtu.h>

struct MODBUS_Device MBDev_Slave;
SOCKET SimSocket    = INVALID_SOCKET;
SOCKET AcceptSocket = INVALID_SOCKET;

// ModBUS Handlers

MODBUS_HANDLER_FC(MBSlave_ReadCoils)
{
	unsigned int i, temp = ((hwreg.DigitalOutput >> 8) & 0xFF) | ((hwreg.DigitalOutput << 8) & 0xFF00);
  unsigned char *buf = reply->reply.read_coils.data; // Retorna no maximo 8 bytes ou 256 bits (saidas).

  reply->reply.read_coils.size = data->read_coils.quant / 8 + (data->read_coils.quant % 8 != 0);

  memset(buf, 0, reply->reply.read_coils.size);

  for(i = 0; i < data->read_coils.quant; i++)
    buf[i / 8] |= ((temp >> (data->read_coils.start + i)) & 1) << (i % 8);

  return MODBUS_EXCEPTION_NONE;

}

MODBUS_HANDLER_FC(MBSlave_ReadDiscreteInputs)
{
  unsigned int i;
  unsigned char *buf = reply->reply.read_discrete_inputs.data; // Retorna no maximo 8 bytes ou 256 bits (entradas).

  for(i = 0; i < data->read_discrete_inputs.quant; i++) {
	if(!(i % 8)) { // First bit of byte, set it to zero.
		buf[i / 8] = 0;
	}
    buf[i / 8] |= ((hwreg.DigitalInput >> (data->read_discrete_inputs.start + i)) & 1) << (i % 8);
  }

  reply->reply.read_discrete_inputs.size = data->read_discrete_inputs.quant / 8 + (data->read_discrete_inputs.quant % 8 != 0);

  return MODBUS_EXCEPTION_NONE;
}

MODBUS_HANDLER_FC(MBSlave_ReadHoldingRegisters)
{
  int i, x;

  if (data->read_holding_registers.start + data->read_holding_registers.quant - 1 >= HWR_MODBUS_SIZE)
	  return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  else
  {
	  x = data->read_holding_registers.start;
	  for(i = data->read_holding_registers.start;
		  i <= data->read_holding_registers.start + (data->read_holding_registers.quant * 2); i += 2) {
			  reply->reply.read_holding_registers.data[i - data->read_holding_registers.start] = hwreg.ModBUS[x];
		reply->reply.read_holding_registers.data[i - data->read_holding_registers.start + 1] = hwreg.ModBUS[x] >> 8;
		x++;
	  }
  }

  reply->reply.read_holding_registers.size = data->read_holding_registers.quant * 2;

  return MODBUS_EXCEPTION_NONE;
}

MODBUS_HANDLER_FC(MBSlave_WriteSingleRegister)
{
  if (data->write_single_register.address < HWR_MODBUS_SIZE) {
    hwreg.ModBUS[data->write_single_register.address] = data->write_single_register.val;
    HardwareRegisters_Sync(&hwreg);
  } else
    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.write_single_register = data->write_single_register;

  return MODBUS_EXCEPTION_NONE;
}

MODBUS_HANDLER_FC(MBSlave_WriteMultipleRegisters)
{
	int i, pos;

  if (data->write_multiple_registers.start + data->write_multiple_registers.quant < HWR_MODBUS_SIZE)
  {
    for (i = 0; i < data->write_multiple_registers.size; i += 2)
    {
      pos = data->write_multiple_registers.start + (i / 2);
      hwreg.ModBUS[pos] = *(unsigned short *)(&data->write_multiple_registers.val[i]);
    }
    HardwareRegisters_Sync(&hwreg);
  } else
    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;

  reply->reply.write_multiple_registers = data->write_multiple_registers;

  return MODBUS_EXCEPTION_NONE;
}

struct MODBUS_Handler MBSlave_Handlers[] =
{
    { MODBUS_FC_READ_COILS              , MBSlave_ReadCoils              },
    { MODBUS_FC_READ_DISCRETE_INPUTS    , MBSlave_ReadDiscreteInputs     },
    { MODBUS_FC_READ_HOLDING_REGISTERS  , MBSlave_ReadHoldingRegisters   },
    { MODBUS_FC_WRITE_SINGLE_REGISTER   , MBSlave_WriteSingleRegister    },
    { MODBUS_FC_WRITE_MULTIPLE_REGISTERS, MBSlave_WriteMultipleRegisters },
};

// We are a slave modbus. TX will be used ot send a reply so we are already connected!
// Just send it.
MODBUS_HANDLER_TX(Modbus_Slave_Tx)
{
	// Send an initial buffer
	if(send(AcceptSocket, (const char *)data, size, 0) == SOCKET_ERROR) {
		Error("send failed: %d", WSAGetLastError());
		size = 0; // no data sent
		closesocket(AcceptSocket);
		AcceptSocket = INVALID_SOCKET;
	}

	return size;
}

void Init_MBDev_Slave(void)
{
	Modbus_RTU_Init(&MBDev_Slave);

	MBDev_Slave.identification.Id                 = Prog.settings.ModBUSID;
	MBDev_Slave.identification.VendorName         = "Tecnequip";
	MBDev_Slave.identification.ProductCode        = "POPTools";
	MBDev_Slave.identification.MajorMinorRevision = "v1.4";

	MBDev_Slave.hl      = MBSlave_Handlers;
	MBDev_Slave.hl_size = sizeof(MBSlave_Handlers)/sizeof(MBSlave_Handlers[0]);
	MBDev_Slave.mode    = MODBUS_MODE_TCP_SLAVE;
	MBDev_Slave.TX      = Modbus_Slave_Tx;
}

void SimulationServer_Message(WPARAM wParam, LPARAM lParam)
{
			switch(WSAGETSELECTEVENT(lParam)){
				case FD_READ: {
					unsigned char szIncoming[1024];
					ZeroMemory(szIncoming,sizeof(szIncoming));

					int inDataLength=recv(AcceptSocket,
						(char*)szIncoming,
						sizeof(szIncoming)/sizeof(szIncoming[0]),
						0);

					if(inDataLength!=-1) {
						Modbus_RTU_Receive(&MBDev_Slave, Modbus_RTU_Validate(szIncoming, inDataLength, TRUE));
					}
				}
				break;

				case FD_ACCEPT: {
					if(AcceptSocket == INVALID_SOCKET) {
						int size=sizeof(sockaddr);
						AcceptSocket=accept(wParam, NULL,&size);
						if (AcceptSocket==INVALID_SOCKET) {
							int nret = WSAGetLastError();
							Error(_("Erro aceitando conexão.\nCódigo do erro: %d"), nret);
						}
					}
    			}
				break;

				case FD_CLOSE: {
					if(AcceptSocket != INVALID_SOCKET) {
						closesocket(AcceptSocket);
						AcceptSocket = INVALID_SOCKET;
					}
    			}
			}
}

void SimulationServer_Stop(void)
{
	if(SimSocket != INVALID_SOCKET) {
		shutdown(SimSocket, SD_BOTH);
		closesocket(SimSocket);
		SimSocket = INVALID_SOCKET;
	}

	if(AcceptSocket != INVALID_SOCKET) {
		shutdown(AcceptSocket, SD_BOTH);
		closesocket(AcceptSocket);
		AcceptSocket = INVALID_SOCKET;
	}
}

int SimulationServer_Start(void)
{
	char *error_msg = _("Erro ao criar servidor ModBUS. Código do Erro: %d\nNão será possível receber conexões externas nessa sessão!");

	SimSocket = socket(AF_UNSPEC,SOCK_STREAM,IPPROTO_TCP);
	if(SimSocket == INVALID_SOCKET) {
		Error(error_msg, WSAGetLastError());
		return 0;
	}

	SOCKADDR_IN SockAddr;
	SockAddr.sin_port=htons(502);
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(SimSocket,(LPSOCKADDR)&SockAddr,sizeof(SockAddr))==SOCKET_ERROR) {
		Error(error_msg, WSAGetLastError());
		return 0;
	}

	if(WSAAsyncSelect(SimSocket, MainWindow, WM_SOCKET, (FD_CLOSE|FD_ACCEPT|FD_READ))) {
		Error(error_msg, WSAGetLastError());
		return 0;
	}

	if(listen(SimSocket,SOMAXCONN)==SOCKET_ERROR) {
		Error(error_msg, WSAGetLastError());
		return 0;
	}

	return 1;
}
