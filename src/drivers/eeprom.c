/**
 * @file eeprom.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2023-10-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "eeprom.h"
#include "cyhal_hw_types.h"
#include <sys/types.h>


/** Determine if the EEPROM is busy writing the last
 *  transaction to non-volatile storage
 *
 * @param
 *
 */
void eeprom_wait_for_write(cyhal_spi_t *spi_obj, cyhal_gpio_t cs_pin)
{
	uint8_t tx_buffer[2];
	uint8_t rx_buffer[2];

	tx_buffer[0] = EEPROM_CMD_RDSR;
	tx_buffer[1] = 0xFF;

	// Poll status register until the WIP bit (bit 0) clears.
	do
	{
		cyhal_gpio_write(cs_pin, 0);
		cyhal_spi_transfer(spi_obj, tx_buffer, 2, rx_buffer, 2, 0xFF);
		cyhal_gpio_write(cs_pin, 1);
	} while ((rx_buffer[1] & 0x01) != 0);
}

/** Enables Writes to the EEPROM
 *
 * @param
 *
 */
void eeprom_write_enable(cyhal_spi_t *spi_obj, cyhal_gpio_t cs_pin)
{
	uint8_t tx_buffer[1];
	uint8_t rx_buffer[1];

	tx_buffer[0] = EEPROM_CMD_WREN;

	cyhal_gpio_write(cs_pin, 0);
	cyhal_spi_transfer(spi_obj, tx_buffer, 1, rx_buffer, 1, 0xFF);
	cyhal_gpio_write(cs_pin, 1);
}

/** Disable Writes to the EEPROM
 *
 * @param
 *
 */
void eeprom_write_disable(cyhal_spi_t *spi_obj, cyhal_gpio_t cs_pin)
{
	uint8_t tx_buffer[1];
	uint8_t rx_buffer[1];

	tx_buffer[0] = EEPROM_CMD_WRDI;

	cyhal_gpio_write(cs_pin, 0);
	cyhal_spi_transfer(spi_obj, tx_buffer, 1, rx_buffer, 1, 0xFF);
	cyhal_gpio_write(cs_pin, 1);
}

/** Writes a single byte to the specified address
 *
 * @param address -- 16 bit address in the EEPROM
 * @param data    -- value to write into memory
 *
 */
void eeprom_write_byte(cyhal_spi_t *spi_obj, cyhal_gpio_t cs_pin, uint16_t address, uint8_t data)
{
	uint8_t tx_buffer[4];
	uint8_t rx_buffer[4];

	// Ensure any previous write cycle has completed.
	eeprom_wait_for_write(spi_obj, cs_pin);

	// Latch write-enable prior to issuing the WRITE command.
	eeprom_write_enable(spi_obj, cs_pin);

	tx_buffer[0] = EEPROM_CMD_WRITE;
	tx_buffer[1] = (address >> 8) & 0xFF;
	tx_buffer[2] = address & 0xFF;
	tx_buffer[3] = data;

	cyhal_gpio_write(cs_pin, 0);
	cyhal_spi_transfer(spi_obj, tx_buffer, 4, rx_buffer, 4, 0xFF);
	cyhal_gpio_write(cs_pin, 1);

	// Block until the internal write cycle completes.
	eeprom_wait_for_write(spi_obj, cs_pin);
}

/** Reads a single byte to the specified address
 *
 * @param address -- 16 bit address in the EEPROM
 *
 */
uint8_t eeprom_read_byte(cyhal_spi_t *spi_obj, cyhal_gpio_t cs_pin, uint16_t address)
{
	uint8_t tx_buffer[4];
	uint8_t rx_buffer[4];

	// Wait if a prior write cycle is still active.
	eeprom_wait_for_write(spi_obj, cs_pin);

	tx_buffer[0] = EEPROM_CMD_READ;
	tx_buffer[1] = (address >> 8) & 0xFF;
	tx_buffer[2] = address & 0xFF;
	tx_buffer[3] = 0xFF;

	cyhal_gpio_write(cs_pin, 0);
	cyhal_spi_transfer(spi_obj, tx_buffer, 4, rx_buffer, 4, 0xFF);
	cyhal_gpio_write(cs_pin, 1);

	return rx_buffer[3];
}