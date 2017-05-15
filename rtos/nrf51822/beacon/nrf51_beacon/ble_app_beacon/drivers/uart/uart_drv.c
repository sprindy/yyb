#include <stdint.h>
#include <stdlib.h>
#include "pca20006.h"
#include "app_uart.h"
#include "uart_drv.h"

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

static void uart_error_handler(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type) {
		case APP_UART_COMMUNICATION_ERROR:
	        APP_ERROR_HANDLER(p_event->data.error_communication);
	    break;
	    case APP_UART_FIFO_ERROR:
	    	APP_ERROR_HANDLER(p_event->data.error_code);
	    break;
        default: break;
    }
}

uint32_t uart_init(void)
{
	uint32_t err_code;

	app_uart_comm_params_t common_params = {
		RX_PIN_NUMBER,
		TX_PIN_NUMBER,
		RTS_PIN_NUMBER,
		CTS_PIN_NUMBER,
		APP_UART_FLOW_CONTROL_DISABLED,
		HWFC,
		UART_BAUDRATE_BAUDRATE_Baud38400
	};

	/* err_code = app_uart_init(&common_params, */
	APP_UART_FIFO_INIT(&common_params,
			UART_RX_BUF_SIZE,
			UART_TX_BUF_SIZE,
			uart_error_handler,
			APP_IRQ_PRIORITY_LOW,
			err_code);
	APP_ERROR_CHECK(err_code);

	return err_code;
}
