#include "chip.h"

/* Read/write buffer (32-bit aligned) */
uint32_t buffer[EEPROM_PAGE_SIZE / sizeof(uint32_t)];

void E2P_Init(void)
{
	/* Init EEPROM */
	Chip_EEPROM_Init(LPC_EEPROM);
}

int E2P_Busy(void)
{
	return 0;
}

void E2P_Read(void *data, unsigned char address, int size)
{
	/* Read all data from EEPROM page */
	Chip_EEPROM_Read(LPC_EEPROM, 0, address/EEPROM_PAGE_SIZE, buffer, EEPROM_RWSIZE_32BITS, EEPROM_PAGE_SIZE);

	memcpy(data, &buffer[address%EEPROM_PAGE_SIZE], size);
}

void E2P_Write(void *data, unsigned char address, int size)
{
	uint16_t page_addr = address/EEPROM_PAGE_SIZE;
	/* Read all data from EEPROM page */
	Chip_EEPROM_Read(LPC_EEPROM, 0, page_addr, buffer, EEPROM_RWSIZE_32BITS, EEPROM_PAGE_SIZE);

	/* Copy new data into buffer */
	memcpy(&buffer[address - page_addr], data, size);

	/* Erase page */
	Chip_EEPROM_Erase(LPC_EEPROM, page_addr);

	/* Write header + size + data to page */
	Chip_EEPROM_Write(LPC_EEPROM, 0, page_addr, buffer, EEPROM_RWSIZE_32BITS, EEPROM_PAGE_SIZE);
}
