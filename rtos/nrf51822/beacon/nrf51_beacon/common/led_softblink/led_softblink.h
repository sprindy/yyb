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
 
#ifndef __LED_SOFTBLINK_H__
#define __LED_SOFTBLINK_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t duty_cycle_max;
    uint8_t duty_cycle_min;
    uint8_t duty_cycle_step;
    uint32_t off_time_ms;
    uint32_t on_time_ms;
    uint32_t leds_pin_bm;
    bool     active_high;
}led_sb_init_params_t;

/**@brief Start blinking LED.
 *
 * @param[in] leds_pin_bit_mask Bit mask containing the pins for the leds to be started.
 * @return
 * @retval NRF_SUCCESS
 */
uint32_t led_softblink_start(uint32_t leds_pin_bit_mask);

/**@brief Stop blinking LED.
 *
 * @param[in] leds_pin_bit_mask Bit mask containing the pins for the leds to be stopped.
 * @return
 * @retval NRF_SUCCESS
 */
uint32_t led_softblink_stop(uint32_t leds_pin_bit_mask);

/**@brief Set the pause time between each blink.
 *
 * @param[in] off_time_ms Off time in ms.
 * @return
 * @retval NRF_SUCCESS
 */
uint32_t led_softblink_off_time_set(uint32_t off_time_ms);

/**@brief LED softblink initialization.
 *
 * @param[in] leds_pin_bit_mask Bit mask containing the pins for the leds.
 * @return
 * @retval NRF_SUCCESS
 */
uint32_t led_softblink_init(led_sb_init_params_t *init_params);

#endif
