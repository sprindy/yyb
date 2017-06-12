/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**@file
 *
 * @defgroup ble_app_bootloader_main main.c
 * @{
 * @ingroup dfu_bootloader_api
 * @brief Bootloader project main file.
 *
 * -# Receive start data packet. 
 * -# Based on start packet, prepare NVM area to store received data. 
 * -# Receive data packet. 
 * -# Validate data packet.
 * -# Write Data packet to NVM.
 * -# If not finished - Wait for next packet.
 * -# Receive stop data packet.
 * -# Activate Image, boot application.
 *
 */
#include "dfu_transport.h"
#include "bootloader.h"
#include "bootloader_util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "nrf51.h"
#include "ble_hci.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "nrf_error.h"
#include "bsp.h"
#include "softdevice_handler.h"
#include "pstorage_platform.h"
#include "nrf_mbr.h"
#include "led_softblink.h"
#include "uart_drv.h"

#define LED_R_MSK                             (1UL << LED_RGB_RED)                              /**< Red LED bitmask */
#define LED_G_MSK                             (1UL << LED_RGB_GREEN)                            /**< Green LED bitmask */
#define LED_B_MSK                             (1UL << LED_RGB_BLUE)                             /**< Blue LED bitmask */

#define IS_SRVC_CHANGED_CHARACT_PRESENT 1                                                       /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define BOOTLOADER_BUTTON               BUTTON_0                                                /**< Button used to enter SW update mode. */
#define UPDATE_IN_PROGRESS_LED_MSK      (LED_R_MSK)                                             /**< Led used to indicate that DFU is active. */

#define APP_TIMER_PRESCALER             0                                                       /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            4                                                       /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         5                                                       /**< Size of timer operation queues. */

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)                /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, 0)                        /**< Maximum size of scheduler events. */

#define SCHED_QUEUE_SIZE                20                                                      /**< Maximum number of events in the scheduler queue. */


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] file_name   File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for initialization of LEDs.
 */
static void leds_init(void)
{   
    uint32_t             err_code;
    led_sb_init_params_t led_sb_init_params;
    
    nrf_gpio_cfg_output(LED_RGB_RED);
    nrf_gpio_cfg_output(LED_RGB_GREEN);
    nrf_gpio_cfg_output(LED_RGB_BLUE);
    
    nrf_gpio_pin_set(LED_RGB_RED);
    nrf_gpio_pin_set(LED_RGB_GREEN);
    nrf_gpio_pin_set(LED_RGB_BLUE);    
    
    led_sb_init_params.active_high     = false;
    led_sb_init_params.duty_cycle_max  = 20;
    led_sb_init_params.duty_cycle_min  = 0;
    led_sb_init_params.duty_cycle_step = 1;
    led_sb_init_params.leds_pin_bm     = (LED_R_MSK | LED_G_MSK | LED_B_MSK);
    led_sb_init_params.off_time_ms     = 2000;
    led_sb_init_params.on_time_ms      = 0;
    
    err_code = led_softblink_init(&led_sb_init_params);
    APP_ERROR_CHECK(err_code);        
}


/**@brief Function for initializing the timer handler module (app_timer).
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);
}


/**@brief Function for initializing the button module.
 */
static void buttons_init(void)
{   
    nrf_gpio_cfg_sense_input(BOOTLOADER_BUTTON,
                             BUTTON_PULL, 
                             NRF_GPIO_PIN_SENSE_LOW);

}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void sys_evt_dispatch(uint32_t event)
{
    pstorage_sys_event_handler(event);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 *
 * @param[in] init_softdevice  true if SoftDevice should be initialized. The SoftDevice must only 
 *                             be initialized if a chip reset has occured. Soft reset from 
 *                             application must not reinitialize the SoftDevice.
 */
static void ble_stack_init(bool init_softdevice)
{
    uint32_t         err_code;
    sd_mbr_command_t com = {SD_MBR_COMMAND_INIT_SD, };

    if (init_softdevice)
    {
        err_code = sd_mbr_command(&com);
        APP_ERROR_CHECK(err_code);
    }
    
    err_code = sd_softdevice_vector_table_base_set(BOOTLOADER_REGION_START);
    APP_ERROR_CHECK(err_code);
   
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, true);

    // Enable BLE stack 
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for event scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Function for bootloader main entry.
 */
int main(void)
{
    uint32_t err_code;
    bool     dfu_start = false;
    bool     app_reset = (NRF_POWER->GPREGRET == BOOTLOADER_DFU_START);

    if (app_reset)
    {
        NRF_POWER->GPREGRET = 0;
    }

    // This check ensures that the defined fields in the bootloader corresponds with actual
    // setting in the nRF51 chip.
    APP_ERROR_CHECK_BOOL(*((uint32_t *)NRF_UICR_BOOT_START_ADDRESS) == BOOTLOADER_REGION_START);
    APP_ERROR_CHECK_BOOL(NRF_FICR->CODEPAGESIZE == CODE_PAGE_SIZE);

    // Initialize.
	/* no enough rom */
	/* uart_init(); */
    timers_init();
    buttons_init();
    leds_init();

    (void)bootloader_init();

    if (bootloader_dfu_sd_in_progress())
    {
        led_softblink_start(UPDATE_IN_PROGRESS_LED_MSK);

        err_code = bootloader_dfu_sd_update_continue();
        APP_ERROR_CHECK(err_code);

        ble_stack_init(!app_reset);
        scheduler_init();

        err_code = bootloader_dfu_sd_update_finalize();
        APP_ERROR_CHECK(err_code);

        led_softblink_stop(UPDATE_IN_PROGRESS_LED_MSK);
    }
    else
    {
        // If stack is present then continue initialization of bootloader.
        ble_stack_init(!app_reset);
        scheduler_init();
    }

    dfu_start  = app_reset;
    dfu_start |= ((nrf_gpio_pin_read(BOOTLOADER_BUTTON) == 0) ? true: false);
    
    if (dfu_start || (!bootloader_app_is_valid(DFU_BANK_0_REGION_START)))
    {
        led_softblink_start(UPDATE_IN_PROGRESS_LED_MSK);
        // Initiate an update of the firmware.
        err_code = bootloader_dfu_start();
        APP_ERROR_CHECK(err_code);
        
        led_softblink_stop(UPDATE_IN_PROGRESS_LED_MSK);
    }

    if (bootloader_app_is_valid(DFU_BANK_0_REGION_START))
    {

        // Select a bank region to use as application region.
        // @note: Only applications running from DFU_BANK_0_REGION_START is supported.
        bootloader_app_start(DFU_BANK_0_REGION_START);
    }

	nrf_gpio_cfg_output(DISPLAY_GPIO_0);
	nrf_gpio_cfg_output(DISPLAY_GPIO_4);
	nrf_gpio_pin_clear(DISPLAY_GPIO_4);
	nrf_gpio_pin_set(DISPLAY_GPIO_0);

    NVIC_SystemReset();
}
