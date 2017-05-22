#include <stdint.h>
#include <stdlib.h>
#include "pca20006.h"
#include "app_uart.h"
#include "uart_drv.h"
#include "ble_nus.h"

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nordic_common.h"

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

static ble_nus_t m_nus;                              /**< Structure to identify the Nordic UART Service. */

/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        while (app_uart_put(p_data[i]) != NRF_SUCCESS);
    }
    while (app_uart_put('\r') != NRF_SUCCESS);
    while (app_uart_put('\n') != NRF_SUCCESS);
}
/**@snippet [Handling the data received over BLE] */


/**@brief Function for initializing ble nus.
 */
void ble_nus_uart_init(void)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;

    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}



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
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;

    switch (p_event->evt_type) {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN)))
            {
                err_code = ble_nus_string_send(&m_nus, data_array, index);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }

                index = 0;
            }
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

uint32_t ble_printf(void)
{
    static uint8_t data_array[2];
    static uint8_t index = 0;
	uint32_t err_code;

	data_array[0]='9';
	/* data_array[1]='\n'; */
	err_code = ble_nus_string_send(&m_nus, data_array, index);
}

/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
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
			uart_event_handler,
			APP_IRQ_PRIORITY_LOW,
			err_code);
	APP_ERROR_CHECK(err_code);

	/* nus_services_init(); */

	return err_code;
}
/**@snippet [UART Initialization] */
