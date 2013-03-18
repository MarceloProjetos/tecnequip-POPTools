#include "poptools.h"
#include "intreg.h"

struct MODBUS_Reply USB_Send(unsigned short fc, MODBUS_FCD_Data *mbdata)
{
	struct MODBUS_Reply reply;

	memset(&reply, 0, sizeof(reply));
	reply.ExceptionCode = MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE;

	if(OpenCOMPort(POPSettings.COMPortFlash, 115200, 8, NOPARITY, ONESTOPBIT)) {
		reply = Modbus_RTU_Send(&MBDev_Serial, 0, fc, mbdata);
		CloseCOMPort();
	}

	return reply;
}

bool USB_SetDateTime(struct tm *t)
{
	time_t rawtime;
	bool ret = false;
	struct MODBUS_Reply reply;
	BYTE cWriteBuffer[16];
	MODBUS_FCD_Data mbdata1;

	if(t == NULL) {
		time ( &rawtime );
		t = localtime ( &rawtime );
	}

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

	mbdata1.write_multiple_registers.quant = INTREG_DATETIME_SIZE;
	mbdata1.write_multiple_registers.size  = INTREG_DATETIME_SIZE*2;
	mbdata1.write_multiple_registers.start = INTREG_DATETIME_START;
	mbdata1.write_multiple_registers.val   = cWriteBuffer;

	reply = USB_Send(MODBUS_FC_WRITE_MULTIPLE_REGISTERS, &mbdata1);
	if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
		ret = true;
	}

	return ret;
}

bool USB_GetDateTime(struct tm *t)
{
	struct tm dt;
	bool ret = false;
	struct MODBUS_Reply reply;

	if(t != NULL) {
		MODBUS_FCD_Data mbdata1;

		mbdata1.read_holding_registers.quant = INTREG_DATETIME_SIZE;
		mbdata1.read_holding_registers.start = INTREG_DATETIME_START;

		reply = USB_Send(MODBUS_FC_READ_HOLDING_REGISTERS, &mbdata1);
		if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
			dt.tm_mday = *(reply.reply.read_holding_registers.data + 0);
			dt.tm_mon  = *(reply.reply.read_holding_registers.data + 1);
			dt.tm_year = (int)(*(reply.reply.read_holding_registers.data + 2)) | ((int)(*(reply.reply.read_holding_registers.data + 3)) << 8);
			dt.tm_hour = *(reply.reply.read_holding_registers.data + 4);
			dt.tm_min  = *(reply.reply.read_holding_registers.data + 5);
			dt.tm_sec  = *(reply.reply.read_holding_registers.data + 6);
			dt.tm_wday = *(reply.reply.read_holding_registers.data + 7);
			dt.tm_yday = (int)(*(reply.reply.read_holding_registers.data + 8)) | ((int)(*(reply.reply.read_holding_registers.data + 3)) << 8);

			dt.tm_year -= 1900;
			dt.tm_mon--;
			dt.tm_sec = dt.tm_sec > 59 ? 59 : dt.tm_sec;

			*t = dt;

			ret = true;
		}
	}

	return ret;
}

unsigned long USB_GetInput(void)
{
	unsigned long ret = 0;
	struct MODBUS_Reply reply;
	MODBUS_FCD_Data mbdata1;

	mbdata1.read_discrete_inputs.start = 0;
	mbdata1.read_discrete_inputs.quant = 19;

	reply = USB_Send(MODBUS_FC_READ_DISCRETE_INPUTS, &mbdata1);
	if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
		unsigned char *d = reply.reply.read_discrete_inputs.data;
		ret = (unsigned long)(*d) | ((unsigned long)(*(d+1)) << 8) | (((unsigned long)(*(d+2)) & 7) << 16);
	}

	return ret;
}

unsigned int USB_GetOutput(void)
{
	unsigned int ret = 0;
	struct MODBUS_Reply reply;
	MODBUS_FCD_Data mbdata1;

	mbdata1.read_coils.start = 0;
	mbdata1.read_coils.quant = 16;

	reply = USB_Send(MODBUS_FC_READ_COILS, &mbdata1);
	if(reply.ExceptionCode == MODBUS_EXCEPTION_NONE) {
		unsigned char *d = reply.reply.read_discrete_inputs.data;
		ret = (unsigned int)(*d) | ((unsigned int)(*(d+1)) << 8);
	}

	return ret;
}
