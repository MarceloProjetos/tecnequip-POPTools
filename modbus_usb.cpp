#include "poptools.h"
#include "intreg.h"

bool USB_SetDateTime(struct tm *t)
{
	time_t rawtime;
	bool ret = false;
	struct MODBUS_Reply reply;

	if(t == NULL) {
		time ( &rawtime );
		t = localtime ( &rawtime );
	}

	memset(&reply, 0, sizeof(reply));
	reply.ExceptionCode = MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE;

	if(OpenCOMPort(POPSettings.COMPortFlash, 115200, 8, NOPARITY, ONESTOPBIT)) {
		BYTE cWriteBuffer[16];
		MODBUS_FCD_Data mbdata1, mbdata2;

		t->tm_year += 1900;
		t->tm_mon++;
		t->tm_sec = t->tm_sec > 59 ? 59 : t->tm_sec;

		memset(cWriteBuffer, 0, sizeof(cWriteBuffer));

		memcpy(&cWriteBuffer[0], &t->tm_mday, 1);
		memcpy(&cWriteBuffer[1], &t->tm_mon , 1);
		memcpy(&cWriteBuffer[2], &t->tm_year, 2);
		memcpy(&cWriteBuffer[4], &t->tm_hour, 1);
		memcpy(&cWriteBuffer[5], &t->tm_min , 1);
		memcpy(&cWriteBuffer[6], &t->tm_sec , 1);
		memcpy(&cWriteBuffer[7], &t->tm_wday, 1);
		memcpy(&cWriteBuffer[8], &t->tm_yday, 2);

		mbdata1.write_multiple_registers.quant = 3;
		mbdata1.write_multiple_registers.size  = mbdata1.write_multiple_registers.quant*2;
		mbdata1.write_multiple_registers.start = INTREG_DATETIME_START;
		mbdata1.write_multiple_registers.val   = cWriteBuffer;

		mbdata2.write_multiple_registers.quant = 2;
		mbdata2.write_multiple_registers.size  = mbdata2.write_multiple_registers.quant*2;
		mbdata2.write_multiple_registers.start = INTREG_DATETIME_START + mbdata1.write_multiple_registers.quant;
		mbdata2.write_multiple_registers.val   = cWriteBuffer+(mbdata1.write_multiple_registers.quant*2);

		reply = Modbus_RTU_Send(&MBDev_Serial, 0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, &mbdata1);
		if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
			reply = Modbus_RTU_Send(&MBDev_Serial, 0, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, &mbdata2);
			if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
				ret = true;
			}
		}

		CloseCOMPort();
	}

	return ret;
}

bool USB_GetDateTime(struct tm *t)
{
	struct tm dt;
	bool ret = false;
	struct MODBUS_Reply reply;

	if(t != NULL) {
		memset(&reply, 0, sizeof(reply));
		reply.ExceptionCode = MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE;

		if(OpenCOMPort(POPSettings.COMPortFlash, 115200, 8, NOPARITY, ONESTOPBIT)) {
			MODBUS_FCD_Data mbdata1, mbdata2;

			mbdata1.read_holding_registers.quant = 3;
			mbdata1.read_holding_registers.start = INTREG_DATETIME_START;

			mbdata2.read_holding_registers.quant = 2;
			mbdata2.read_holding_registers.start = INTREG_DATETIME_START + mbdata1.write_multiple_registers.quant;

			reply = Modbus_RTU_Send(&MBDev_Serial, 0, MODBUS_FC_READ_HOLDING_REGISTERS, &mbdata1);
			if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
				dt.tm_mday = *(reply.reply.read_holding_registers.data + 0);
				dt.tm_mon  = *(reply.reply.read_holding_registers.data + 1);
				dt.tm_year = (int)(*(reply.reply.read_holding_registers.data + 2)) | ((int)(*(reply.reply.read_holding_registers.data + 3)) << 8);
				dt.tm_hour = *(reply.reply.read_holding_registers.data + 4);
				dt.tm_min  = *(reply.reply.read_holding_registers.data + 5);

				reply = Modbus_RTU_Send(&MBDev_Serial, 0, MODBUS_FC_READ_HOLDING_REGISTERS, &mbdata2);
				if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
					dt.tm_sec  = *(reply.reply.read_holding_registers.data + 0);
					dt.tm_wday = *(reply.reply.read_holding_registers.data + 1);
					dt.tm_yday = (int)(*(reply.reply.read_holding_registers.data + 2)) | ((int)(*(reply.reply.read_holding_registers.data + 3)) << 8);

					dt.tm_year -= 1900;
					dt.tm_mon--;
					dt.tm_sec = dt.tm_sec > 59 ? 59 : dt.tm_sec;

					*t = dt;

					ret = true;
				}
			}

			CloseCOMPort();
		}
	}

	return ret;
}
