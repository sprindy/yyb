/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
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
 
#ifndef __BEACON_H__
#define __BEACON_H__

#include <stdint.h>
#include <stdbool.h>
#include "app_util.h"
/* #include "ble_bcs.h" */

#define APP_BEACON_ADV_INTERVAL_MIN_MS        100   
#define APP_BEACON_ADV_INTERVAL_MAX_MS      10240   
#define APP_BEACON_DEFAULT_ADV_INTERVAL_MS 760                            /**< Default advertising interval is 760ms*/
#define APP_BEACON_ADV_TIMEOUT               0                            /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */

#define APP_BEACON_MANUF_DATA_LEN       0x17                              /**< Total length of information advertised by the beacon. */
#define APP_ADV_DATA_LENGTH             0x15                              /**< Length of manufacturer specific data in the advertisement. */
#define APP_DEVICE_TYPE                 0x02                              /**< 0x02 refers to beacon. */
#define APP_DEFAULT_MEASURED_RSSI       0xBB                              /**< The beacon's measured RSSI at 1 meter distance in dBm. */
#define APP_DEFAULT_COMPANY_IDENTIFIER  0x0059                            /**< Company identifier for Nordic Semiconductor. as per www.bluetooth.org. */
#define APP_DEFAULT_MAJOR_VALUE         0x00, 0x01                        /**< Major value used to identify beacons. */ 
#define APP_DEFAULT_MINOR_VALUE         0x00, 0x00                        /**< Minor value used to identify beacons. */ 
#define APP_DEFAULT_BEACON_UUID         0x01, 0x12, 0x23, 0x34, \
                                        0x45, 0x56, 0x67, 0x78, \
                                        0x89, 0x9a, 0xab, 0xbc, \
                                        0xcd, 0xde, 0xef, 0xf0            /**< Proprietary UUID for beacon. */

#define BEACON_MANUF_DAT_UUID_IDX     2
#define BEACON_MANUF_DAT_MAJOR_H_IDX 18
#define BEACON_MANUF_DAT_MAJOR_L_IDX 19
#define BEACON_MANUF_DAT_MINOR_H_IDX 20
#define BEACON_MANUF_DAT_MINOR_L_IDX 21
#define BEACON_MANUF_DAT_RSSI_IDX    22

#define MAGIC_FLASH_BYTE 0x42                                             /**< Magic byte used to recognise that flash has been written */

typedef enum
{
    beacon_mode_config,  /**< Beacon configuration mode. */
    beacon_mode_normal   /**< Normal beacon operation mode. */
}beacon_mode_t;

typedef enum
{
	DISPLAY_WORDS_ZERO_FOR_TEST,
	DISPLAY_WORDS_ONE,
	DISPLAY_WORDS_TWO,
	DISPLAY_WORDS_FOUR = 4
}display_words_num_t;

typedef struct
{
	uint8_t total_pkg;
	uint8_t index_pkg;
	uint8_t led_state;
	uint8_t reserved;
	uint8_t disp_data[16];
}display_data_t;

typedef struct
{
    uint8_t  magic_byte;                             /**< Magic byte in flash to detect if data is valid or not. */
    uint8_t  beacon_data[APP_BEACON_MANUF_DATA_LEN]; /**< Beacon manufacturer specific data*/
    uint16_t company_id;                             /**< Advertised beacon company idetifier. */
    uint16_t adv_interval;                           /**< Advertising interval in ms */
    uint8_t  led_state;  		          			 /**< Softblinking LEDs state */
}beacon_data_t;

typedef struct
{
	beacon_mode_t boot_mode;               // = "config"
	uint8_t       pcb_id[8];               // = "20170208";
	uint8_t       display_led_test;        // = 0
	uint8_t       display_words_num;       // = 4
	uint8_t       display_enable_hw_timer;
	uint32_t      display_hw_timer_period; // = 40
	uint32_t      display_timer_period;    // = 0x2000
	uint8_t       display_line_delay;      // = 1
	/* row delay control by display_hw_timer_period */
	/* uint8_t       display_row_delay;       // = 1 */
	uint32_t      acc_timer_period;        // = 0x200
	uint8_t       acc_enable_timer;
	uint8_t       acc_int_use_x;
	uint8_t       acc_int_use_y;
	uint8_t       acc_int_use_z;
	uint32_t      acc_int_ths;
	uint8_t       acc_int_dur;
	uint8_t       enable_nus_debug;
	uint8_t       enable_uart_debug;
	uint8_t       enable_uart_hwfc;        // = 0
	uint8_t       uart_baudrate[6];        // = 38400
}yyb_data_t;

typedef struct
{
    beacon_data_t data;
	yyb_data_t    yyb_data;
    /* uint8_t      padding[CEIL_DIV(sizeof(beacon_data_t), 4)]; */
} __attribute__((aligned(4))) beacon_flash_db_t;

#if YYB_NEW_PARAMS_FLASH
typedef struct
{
	uint16_t data_array[10];
}yyb_dis_data_t;

typedef union
{
	yyb_dis_data_t data;
}yyb_flash_db_t;
#endif /* YYB_NEW_PARAMS_FLASH */

#endif
