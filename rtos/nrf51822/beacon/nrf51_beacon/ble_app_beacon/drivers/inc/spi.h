#ifndef __SPI_DRV_H__
#define __SPI_DRV_H__

#include "stdio.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"
#include "nrf_gpio.h"
#include "pca20006.h"
#include "spi.h"

#define ACC_SPI_INTERFACE NRF_SPI1
#define INVAILVALUE 0x55

static __INLINE void spi_cs_enable(void)
{
	nrf_gpio_pin_clear(ACC_PIN_CSN);
}

static __INLINE void spi_cs_disable(void)
{
	nrf_gpio_pin_set(ACC_PIN_CSN);
}

static __INLINE void spi_clear_flog(void)
{
	ACC_SPI_INTERFACE->EVENTS_READY = 0;
}

static __INLINE void spi_write(uint8_t data)
{
	ACC_SPI_INTERFACE->TXD = data;
}

static __INLINE uint8_t spi_read(void)
{
	uint8_t RX;

	while(ACC_SPI_INTERFACE->EVENTS_READY == 0)
		;
	ACC_SPI_INTERFACE->EVENTS_READY = 0;
	RX = ACC_SPI_INTERFACE->RXD;

	return RX;
}

static __INLINE uint8_t spi_rw(uint8_t data)
{
	spi_write(data);
	return spi_read();
}

void SPI_Mems_Init(void)
{
	spi_cs_disable();

	ACC_SPI_INTERFACE->PSELMISO = ACC_PIN_MISO;       //MISO
	ACC_SPI_INTERFACE->PSELMOSI = ACC_PIN_MOSI;       //MOSI
	ACC_SPI_INTERFACE->PSELSCK  = ACC_PIN_SCK;        //SCK
	//config frequency
	ACC_SPI_INTERFACE->FREQUENCY = SPI_FREQUENCY_FREQUENCY_M4;

	//config config
	ACC_SPI_INTERFACE->CONFIG = (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos)|
		(SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos)|
		(SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos);

	//enable nrf51822 spi interface
	ACC_SPI_INTERFACE->ENABLE = SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos;
}

uint8_t SPI_Mems_Read_Reg(uint8_t reg)
{
	uint8_t RX;
	uint8_t cmd;

	spi_clear_flog();
	spi_cs_enable();

	cmd = 0x80 | reg;
	cmd &= ~0x40;

	spi_write(cmd);
	spi_rw(INVAILVALUE);
	RX = spi_read();

	spi_cs_disable();

	return RX;
}

uint32_t SPI_Mems_Write_Reg(uint8_t reg, uint8_t value)
{
	spi_clear_flog();
	spi_cs_enable();

	spi_write(reg & (~0x80));
	spi_rw(value);
	spi_read();

	spi_cs_disable();

	return 0;
}

#endif /*__SPI_DRV_H__*/

