#include <stdint.h>
#include <stdlib.h>
#include "pca20006.h"
/* #include "app_uart.h" */
#include "uart_drv.h"

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

uint8_t uart_get(void)
{
    while (NRF_UART0->EVENTS_RXDRDY != 1)
    {
        // Wait for RXD data to be received
    }

    NRF_UART0->EVENTS_RXDRDY = 0;
    return (uint8_t)NRF_UART0->RXD;
}


bool uart_get_with_timeout(int32_t timeout_ms, uint8_t * rx_data)
{
    bool ret = true;

    while (NRF_UART0->EVENTS_RXDRDY != 1)
    {
        if (timeout_ms-- >= 0)
        {
            // wait in 1ms chunk before checking for status.
            nrf_delay_us(1000);
        }
        else
        {
            ret = false;
            break;
        }
    } // Wait for RXD data to be received.

    if (timeout_ms >= 0)
    {
        // clear the event and set rx_data with received byte.
        NRF_UART0->EVENTS_RXDRDY = 0;
        *rx_data                 = (uint8_t)NRF_UART0->RXD;
    }

    return ret;
}

void uart_put(uint8_t cr)
{
    NRF_UART0->TXD = (uint8_t)cr;

    while (NRF_UART0->EVENTS_TXDRDY != 1)
    {
        // Wait for TXD data to be sent.
    }

    NRF_UART0->EVENTS_TXDRDY = 0;
}


void uart_putstring(const uint8_t * str)
{
    uint_fast8_t i  = 0;
    uint8_t      ch = str[i++];

    while (ch != '\0')
    {
        uart_put(ch);
        ch = str[i++];
    }
}

uint32_t uart_init(void)
{
	uint32_t err_code;
#if 0
	app_uart_comm_params_t common_params = {
		RX_PIN_NUMBER,
		TX_PIN_NUMBER,
		RTS_PIN_NUMBER,
		CTS_PIN_NUMBER,
		(app_uart_flow_control_t) APP_UART_FLOW_CONTROL_DISABLED,
		HWFC,
		UART_BAUDRATE
	};

	err_code = app_uart_init(&common_params,
			NULL,
			NULL,
			APP_IRQ_PRIORITY_LOW,
			0);
#endif

/** @snippet [Configure UART RX and TX pin] */
    nrf_gpio_cfg_output(TX_PIN_NUMBER);
    nrf_gpio_cfg_input(RX_PIN_NUMBER, NRF_GPIO_PIN_NOPULL);

    NRF_UART0->PSELTXD = TX_PIN_NUMBER;
    NRF_UART0->PSELRXD = RX_PIN_NUMBER;
/** @snippet [Configure UART RX and TX pin] */
    if (HWFC) {
        nrf_gpio_cfg_output(RTS_PIN_NUMBER);
        nrf_gpio_cfg_input(CTS_PIN_NUMBER, NRF_GPIO_PIN_NOPULL);
        NRF_UART0->PSELCTS = CTS_PIN_NUMBER;
        NRF_UART0->PSELRTS = RTS_PIN_NUMBER;
        NRF_UART0->CONFIG  = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);
    }

    NRF_UART0->BAUDRATE      = (UART_BAUDRATE_BAUDRATE_Baud38400 << UART_BAUDRATE_BAUDRATE_Pos);
    NRF_UART0->ENABLE        = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 1;
    NRF_UART0->EVENTS_RXDRDY = 0;

	return err_code;
}
