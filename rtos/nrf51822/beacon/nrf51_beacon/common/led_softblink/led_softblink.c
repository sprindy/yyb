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
#include <string.h>
#include "led_softblink.h"
 
#include "nrf_gpio.h"
#include "app_timer.h"

/**@brief Macro for verifying NULL parameters are not passed to API.
 *
 * @param[in] PARAM Parameter checked for NULL.
 *
 * @retval (NRF_ERROR_NULL | DEVICE_MANAGER_ERR_BASE) when @ref PARAM is NULL.
 */
#define NULL_PARAM_CHECK(PARAM)                            \
    if ((PARAM) == NULL)                                   \
    {                                                      \
        return (NRF_ERROR_NULL); \
    }
    
/**@brief Macro for verifying module's initialization status.
 *
 * @retval (NRF_ERROR_INVALID_STATE | DEVICE_MANAGER_ERR_BASE) when module is not initialized.
 */
#define VERIFY_MODULE_INITIALIZED()                                     \
    do                                                                  \
    {                                                                   \
        if (!m_led_sb.initialized)                                      \
        {                                                               \
            return (NRF_ERROR_INVALID_STATE); \
        }                                                               \
    } while (0)
    
/**@brief Macro for verifying module's initialization status. Returns in case it is not initialized.
 */
#define VERIFY_MODULE_INITIALIZED_VOID() \
    do                                   \
    {                                    \
        if (!m_led_sb.initialized)       \
        {                                \
            return;                      \
        }                                \
    } while (0)    

#define APP_TIMER_PRESCALER         0   
#define LED_PWM_PERIOD_TICKS APP_TIMER_TICKS(20, APP_TIMER_PRESCALER)   /**< LED Softblink PWM period in app_timer ticks. */ 

typedef struct
{
    uint32_t             bit_mask;
    app_timer_id_t       timer_id;
    bool                 timer_running;
    bool                 leds_is_on;
    bool                 is_counting_up;
    int8_t               duty_cycle;
    bool                 initialized;
    led_sb_init_params_t params;
}led_sb_context_t;

static led_sb_context_t m_led_sb = {0};

static void led_softblink_on_timeout(void *p_context)
{
    uint32_t new_timeout_ticks;
    uint32_t err_code;
    
    VERIFY_MODULE_INITIALIZED_VOID();
    
    if (m_led_sb.leds_is_on)
    {   
        // Turn off LEDs
        if (m_led_sb.params.active_high)
        {
            NRF_GPIO->OUTCLR = m_led_sb.bit_mask;
        }
        else
        {
            NRF_GPIO->OUTSET = m_led_sb.bit_mask;
        }
        
        m_led_sb.leds_is_on = false;
        
        if (m_led_sb.is_counting_up)
        {
            m_led_sb.duty_cycle += m_led_sb.params.duty_cycle_step;
            if(m_led_sb.duty_cycle >= m_led_sb.params.duty_cycle_max)
            {
                // Max PWM duty cycle is reached, start decrementing.
                m_led_sb.is_counting_up = false;
            }
        }
        else
        {
            m_led_sb.duty_cycle -= m_led_sb.params.duty_cycle_step;
            if(m_led_sb.duty_cycle <= m_led_sb.params.duty_cycle_min)
            {
                // Min PWM duty cycle is reached, start incrementing.
                m_led_sb.is_counting_up = true;
            }
        }
        
        if (m_led_sb.duty_cycle <= m_led_sb.params.duty_cycle_min)
        {
            new_timeout_ticks = APP_TIMER_TICKS(m_led_sb.params.off_time_ms, APP_TIMER_PRESCALER);
        }
        else
        {
            new_timeout_ticks = (LED_PWM_PERIOD_TICKS / 100) * (100 - m_led_sb.duty_cycle) + 5;
        }
    }
    else
    {
        // Turn on LEDs
        if (m_led_sb.params.active_high)
        {
            NRF_GPIO->OUTSET = m_led_sb.bit_mask;;
        }
        else
        {
            NRF_GPIO->OUTCLR = m_led_sb.bit_mask;;
        }
        
        m_led_sb.leds_is_on = true;
        
        new_timeout_ticks = (LED_PWM_PERIOD_TICKS / 100) * m_led_sb.duty_cycle + 5;
    }
    
    err_code = app_timer_start(m_led_sb.timer_id, new_timeout_ticks, NULL);
    APP_ERROR_CHECK(err_code);
}

uint32_t led_softblink_start(uint32_t leds_pin_bit_mask)
{
    VERIFY_MODULE_INITIALIZED();
    
    m_led_sb.bit_mask |= leds_pin_bit_mask;
    
    if (!m_led_sb.timer_running)
    {
        uint32_t err_code;
        
        err_code = app_timer_start(m_led_sb.timer_id, LED_PWM_PERIOD_TICKS, NULL);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
        
        m_led_sb.timer_running = true;
    }
    
    return NRF_SUCCESS;
}

uint32_t led_softblink_stop(uint32_t leds_pin_bit_mask)
{
    VERIFY_MODULE_INITIALIZED();
    
    m_led_sb.bit_mask &= ~leds_pin_bit_mask;
    
    if (m_led_sb.timer_running && (m_led_sb.bit_mask == 0))
    {
        uint32_t err_code;
        
        err_code = app_timer_stop(m_led_sb.timer_id);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
        
        m_led_sb.timer_running = false;
    }
    
    return NRF_SUCCESS;
}

uint32_t led_softblink_off_time_set(uint32_t off_time_ms)
{
    VERIFY_MODULE_INITIALIZED();
    
    m_led_sb.params.off_time_ms = off_time_ms;
    
    return NRF_SUCCESS;
}

uint32_t led_softblink_init(led_sb_init_params_t *init_params)
{
    uint32_t bit_mask;
    uint32_t pin_number = 0;
    
    if (m_led_sb.initialized)
    {
        return NRF_ERROR_INVALID_STATE;
    }
    
    NULL_PARAM_CHECK(init_params);
    
    if ( (init_params->duty_cycle_max == 0)                           ||
         (init_params->duty_cycle_max <= init_params->duty_cycle_min) ||
         (init_params->duty_cycle_step == 0)                          ||
         (init_params->leds_pin_bm == 0))
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    memset(&m_led_sb, 0, sizeof(led_sb_context_t));
    memcpy(&m_led_sb.params, init_params, sizeof(led_sb_init_params_t));
    m_led_sb.is_counting_up = true;
    
    bit_mask = m_led_sb.params.leds_pin_bm;
    // Configure IO according to the leds_pin_bit_mask
    while (bit_mask)
    {
        if (bit_mask & 0x1UL)
        {
            nrf_gpio_cfg_output(pin_number);
            
            if (m_led_sb.params.active_high)
            {
                nrf_gpio_pin_clear(pin_number);
            }
            else
            {
                nrf_gpio_pin_set(pin_number);
            }
        }
        
        pin_number++;
        bit_mask >>= 1UL;
    }
    
    m_led_sb.initialized = true;
    
    return app_timer_create(&m_led_sb.timer_id, APP_TIMER_MODE_SINGLE_SHOT, led_softblink_on_timeout);
}
