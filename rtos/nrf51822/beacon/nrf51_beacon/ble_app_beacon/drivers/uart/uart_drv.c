#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pca20006.h"
#include "app_uart.h"
#include "uart_drv.h"
#include "log.h"

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nordic_common.h"

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' i.e '\r\n' (hex 0x0D) or if the string has reached a length of
 *          @ref NUS_MAX_DATA_LENGTH.
 */
/**@snippet [Handling the data received over UART] */
static void uart_event_handler(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type) {
        case APP_UART_DATA_READY:
    		nrf_gpio_cfg_output(LED_RGB_RED);
			if(nrf_gpio_pin_read(LED_RGB_RED)) {
				nrf_gpio_pin_clear(LED_RGB_RED);
			}
			else {
				nrf_gpio_pin_set(LED_RGB_RED);
			}
#if 0  //test ok, but no need any more
			static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
			static uint8_t index = 0;
    		uint32_t err_code;
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN)))
            {
                err_code = ble_nus_string_send(&m_nus, data_array, index);
                if (err_code != NRF_ERROR_INVALID_STATE) {
                    APP_ERROR_CHECK(err_code);
                }

                index = 0;
            }
#endif
            break;
		case APP_UART_COMMUNICATION_ERROR:
	        APP_ERROR_HANDLER(p_event->data.error_communication);
	    break;
	    case APP_UART_FIFO_ERROR:
	    	APP_ERROR_HANDLER(p_event->data.error_code);
	    break;
        default: break;
    }
}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
uint32_t uart_init(void)
{
	uint32_t err_code;

#if ENABLE_UART_DEBUG
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
			uart_event_handler,
			APP_IRQ_PRIORITY_LOW,
			err_code);
	APP_ERROR_CHECK(err_code);
	if(err_code == NRF_SUCCESS) {
		log_d("[UART] %s success.\n", __func__);
	}

#endif
	return err_code;
}
/**@snippet [UART Initialization] */

