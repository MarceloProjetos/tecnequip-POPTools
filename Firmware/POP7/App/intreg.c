#include "modbus.h"
#include "rtc.h"
#include "intreg.h"

RTC_Time Time;

extern volatile int ArrayBitUser_Count;
extern volatile int ArrayBitUser[];
extern volatile int ArrayIntUser_Count;
extern volatile int ArrayIntUser[];

extern unsigned char mac_addr[6];

// Checa se a faixa de registradores eh valida
int IntReg_CheckBounds(int start, int quant)
{
	return (quant > 0) && (((start >= INTREG_START) && (start + quant - 1 < INTREG_RESERVED_START)) ||
			((start >= INTREG_USERBIT_START) && (start - INTREG_USERBIT_START + quant - 1 < ArrayBitUser_Count)) ||
			((start >= INTREG_USERINT_START) && (start - INTREG_USERINT_START + quant - 1 < ArrayIntUser_Count)));
}

// Le o valor do registrador
int IntReg_Read(int reg)
{
	if(reg < (INTREG_MODBUS_START + INTREG_MODBUS_SIZE)) {
		return MODBUS_REGISTER[reg];
	} else if(reg < (INTREG_DATETIME_START + INTREG_DATETIME_SIZE)) {
		switch(reg - INTREG_DATETIME_START) {
		case 0:
			Time = RTC_GetTime();
			return (int)(Time.Mday) | ((int)(Time.Mon) << 8);

		case 1:
			return Time.Year;

		case 2:
			return (int)(Time.Hour) | ((int)(Time.Min) << 8);

		case 3:
			return (int)(Time.Sec) | ((int)(Time.Wday) << 8);

		case 4:
			return Time.Yday;
		}
	} else if(reg < (INTREG_VARCOUNT_START + INTREG_VARCOUNT_SIZE)) {
		if(reg == INTREG_VARCOUNT_START) {
			return ArrayBitUser_Count;
		} else {
			return ArrayIntUser_Count;
		}
	} else if(reg < (INTREG_MACADDRESS_START + INTREG_MACADDRESS_SIZE)) {
		reg -= INTREG_MACADDRESS_START;
		reg *= 2;
		return (int)(((unsigned int)(mac_addr[reg + 1]) << 8) | ((unsigned int)mac_addr[reg]));
	} else if(reg < (INTREG_USERBIT_START + INTREG_USERBIT_SIZE)) {
		reg -= INTREG_USERBIT_START;
		if(reg < ArrayBitUser_Count) return ArrayBitUser[reg];
	} else if(reg < (INTREG_USERINT_START + INTREG_USERINT_SIZE)) {
		reg -= INTREG_USERINT_START;
		if(reg < ArrayIntUser_Count) return ArrayIntUser[reg];
	}

	return 0;
}

// Escreve o valor no registrador
void IntReg_Write(int reg, int val)
{
	if(reg < (INTREG_MODBUS_START + INTREG_MODBUS_SIZE)) {
		MODBUS_REGISTER[reg] = val;
	} else if(reg < (INTREG_DATETIME_START + INTREG_DATETIME_SIZE)) {
		switch(reg - INTREG_DATETIME_START) {
		case 0:
			memset(&Time, 0, sizeof(Time));

			Time.Mday = (val     ) & 0xFF;
			Time.Mon  = (val >> 8) & 0xFF;
			break;

		case 1:
			Time.Year = val;
			break;

		case 2:
			Time.Hour = (val     ) & 0xFF;
			Time.Min  = (val >> 8) & 0xFF;
			break;

		case 3:
			Time.Sec  = (val     ) & 0xFF;
			Time.Wday = (val >> 8) & 0xFF;
			break;

		case 4:
			Time.Yday = val;

			RTC_SetTime(Time);
			break;
		}
	}
}
