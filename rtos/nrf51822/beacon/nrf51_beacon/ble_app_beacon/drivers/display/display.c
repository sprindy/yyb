
#include <string.h>
#include "display.h"
#include "pca20006.h"
#include "nrf_gpio.h"
#include "app_timer.h"

typedef struct
{
    app_timer_id_t       timer_id;
    bool                 timer_running;
    bool                 initialized;
}display_context_t;

static display_context_t m_display_ct = {0};

static void display_led_on(uint8_t index, bool on)
{
	if( index > 31)
		return;
	switch(index){
		case 0:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
				nrf_gpio_pin_set(DISPLAY_GPIO_0);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			}
			break;
		case 1:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
				nrf_gpio_pin_set(DISPLAY_GPIO_4);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
			}
			break;
		case 2:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
				nrf_gpio_pin_set(DISPLAY_GPIO_0);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			}
			break;
		case 3:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
				nrf_gpio_pin_set(DISPLAY_GPIO_5);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
			}
			break;
		case 4:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
				nrf_gpio_pin_set(DISPLAY_GPIO_0);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			}
			break;
		case 5:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
				nrf_gpio_pin_set(DISPLAY_GPIO_6);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
			}
			break;
		case 6:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
				nrf_gpio_pin_set(DISPLAY_GPIO_0);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			}
			break;
		case 7:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
				nrf_gpio_pin_set(DISPLAY_GPIO_7);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
				nrf_gpio_pin_clear(DISPLAY_GPIO_0);
			}
			break;
		case 8:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
				nrf_gpio_pin_set(DISPLAY_GPIO_1);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			}
			break;
		case 9:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
				nrf_gpio_pin_set(DISPLAY_GPIO_4);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
			}
			break;
		case 10:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
				nrf_gpio_pin_set(DISPLAY_GPIO_1);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			}
			break;
		case 11:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
				nrf_gpio_pin_set(DISPLAY_GPIO_5);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
			}
			break;
		case 12:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
				nrf_gpio_pin_set(DISPLAY_GPIO_1);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			}
			break;
		case 13:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
				nrf_gpio_pin_set(DISPLAY_GPIO_6);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
			}
			break;
		case 14:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
				nrf_gpio_pin_set(DISPLAY_GPIO_1);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			}
			break;
		case 15:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
				nrf_gpio_pin_set(DISPLAY_GPIO_7);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
				nrf_gpio_pin_clear(DISPLAY_GPIO_1);
			}
			break;
		case 16:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
				nrf_gpio_pin_set(DISPLAY_GPIO_2);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			}
			break;
		case 17:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
				nrf_gpio_pin_set(DISPLAY_GPIO_4);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
			}
			break;
		case 18:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
				nrf_gpio_pin_set(DISPLAY_GPIO_2);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			}
			break;
		case 19:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
				nrf_gpio_pin_set(DISPLAY_GPIO_5);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
			}
			break;
		case 20:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
				nrf_gpio_pin_set(DISPLAY_GPIO_2);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			}
			break;
		case 21:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
				nrf_gpio_pin_set(DISPLAY_GPIO_6);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
			}
			break;
		case 22:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
				nrf_gpio_pin_set(DISPLAY_GPIO_2);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			}
			break;
		case 23:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
				nrf_gpio_pin_set(DISPLAY_GPIO_7);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
				nrf_gpio_pin_clear(DISPLAY_GPIO_2);
			}
			break;
		case 24:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
				nrf_gpio_pin_set(DISPLAY_GPIO_3);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			}
			break;
		case 25:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
				nrf_gpio_pin_set(DISPLAY_GPIO_4);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_4);
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
			}
			break;
		case 26:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
				nrf_gpio_pin_set(DISPLAY_GPIO_3);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			}
			break;
		case 27:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
				nrf_gpio_pin_set(DISPLAY_GPIO_5);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_5);
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
			}
			break;
		case 28:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
				nrf_gpio_pin_set(DISPLAY_GPIO_3);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			}
			break;
		case 29:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
				nrf_gpio_pin_set(DISPLAY_GPIO_6);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_6);
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
			}
			break;
		case 30:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
				nrf_gpio_pin_set(DISPLAY_GPIO_3);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			}
			break;
		case 31:
			if(on) {
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
				nrf_gpio_pin_set(DISPLAY_GPIO_7);
			}
			else {
				nrf_gpio_pin_clear(DISPLAY_GPIO_7);
				nrf_gpio_pin_clear(DISPLAY_GPIO_3);
			}
			break;
		default:break;

	}
}

uint32_t display_timer_start(void)
{
    if (!m_display_ct.timer_running)
    {
        uint32_t err_code;

        err_code = app_timer_start(m_display_ct.timer_id, 1000, NULL);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }

        m_display_ct.timer_running = true;
    }

    return NRF_SUCCESS;
}

static void display_work_timerout(void *p_context)
{
#define DISPLAY_LED_NUM		4
	uint32_t err_code;

	static uint8_t i = 0;

#if 1
	if(i>0)
		display_led_on(i-1, false);
	else
		display_led_on(DISPLAY_LED_NUM - 1, false);

	if(++i >= DISPLAY_LED_NUM)
		i = 0;
#endif
	err_code = app_timer_start(m_display_ct.timer_id, 1000, NULL);
	APP_ERROR_CHECK(err_code);
}

uint32_t display_work_init(void)
{
	uint32_t err_code;
	m_display_ct.timer_running = false;
	err_code = app_timer_create(&m_display_ct.timer_id, APP_TIMER_MODE_SINGLE_SHOT, display_work_timerout);
	if(err_code == NRF_SUCCESS)
		err_code = display_timer_start();

	return err_code;
}

uint32_t display_init(void)
{
	uint32_t err_code;

	nrf_gpio_cfg_output(DISPLAY_GPIO_0);
	nrf_gpio_cfg_output(DISPLAY_GPIO_1);
	nrf_gpio_cfg_output(DISPLAY_GPIO_2);
	nrf_gpio_cfg_output(DISPLAY_GPIO_3);
	nrf_gpio_cfg_output(DISPLAY_GPIO_4);
	nrf_gpio_cfg_output(DISPLAY_GPIO_5);
	nrf_gpio_cfg_output(DISPLAY_GPIO_6);
	nrf_gpio_cfg_output(DISPLAY_GPIO_7);

	nrf_gpio_pin_clear(DISPLAY_GPIO_0);
	nrf_gpio_pin_clear(DISPLAY_GPIO_1);
	nrf_gpio_pin_clear(DISPLAY_GPIO_2);
	nrf_gpio_pin_clear(DISPLAY_GPIO_3);
	nrf_gpio_pin_clear(DISPLAY_GPIO_4);
	nrf_gpio_pin_clear(DISPLAY_GPIO_5);
	nrf_gpio_pin_clear(DISPLAY_GPIO_6);
	nrf_gpio_pin_clear(DISPLAY_GPIO_7);

	err_code = display_work_init();

	return err_code;
}

