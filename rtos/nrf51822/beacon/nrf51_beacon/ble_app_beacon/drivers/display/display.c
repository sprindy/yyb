#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"
#include "pca20006.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "sdk_config.h"
#include "nrf_drv_timer.h"
#include "uart_drv.h"
#include "log.h"

#define DISPLAY_WORDS_CNT      4
#define DISPLAY_ONE_WORD_BYTES (DISPLAY_LED_NUM * 4)
#define DISPLAY_DATA_BYTE_LEN  (DISPLAY_WORDS_CNT * DISPLAY_ONE_WORD_BYTES)

const nrf_drv_timer_t TIMER_LED = NRF_DRV_TIMER_INSTANCE(1);

typedef struct
{
    app_timer_id_t       timer_id;
    bool                 timer_running;
    bool                 initialized;
}display_context_t;

static display_context_t m_display_ct = {0};
static bool display_dir = false;
static uint8_t display_cur_word = 0;
static uint8_t display_repeat_cnt = 0;
static uint8_t display_rcv_word_cnt = 0;
static uint32_t display_cur_line = 0;
static uint32_t display_timer_period = 0;
static uint32_t display_hw_timer_period = 0;
static uint8_t display_words_num = 0;
static uint8_t display_enable_hw_timer = 0;

char wData[DISPLAY_DATA_BYTE_LEN] = {
#if 0 //wang
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x08,
	0x02,0x00,0x00,0x08,0x02,0x00,0x00,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x03,0xFF,0xFF,0xF8,
	0x03,0xFF,0xFF,0xF8,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x80,0x08,0x06,0x00,0x80,0x08,0x0E,0x00,0x00,0x08,0x06,0x00,0x00,0x18,
	0x02,0x00,0x00,0x18,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
#else //sheng ri kuai le
#if 1
	0x00,0x00,0x00,0x00,0x00,0x80,0x03,0x10,0x00,0x70,0x00,0x10,0x80,0x0F,0x00,0x10,0xE0,0x04,0x08,0x10,0x00,0x04,0x08,0x10,0x00,0x04,0x08,0x10,0x04,0x04,0x08,0x10,
	0xF8,0xFF,0xFF,0x1F,0x00,0x04,0x08,0x10,0x00,0x04,0x08,0x10,0x00,0x04,0x04,0x10,0x00,0x06,0x06,0x10,0x00,0x06,0x00,0x18,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,//"生",0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0xFF,0xFF,0x3F,0x10,0x40,0x00,0x04,0x10,0x40,0x00,0x04,0x10,0x40,0x00,0x04,0x10,0x40,0x00,0x04,
	0x10,0x40,0x00,0x04,0x10,0x40,0x00,0x04,0x10,0x40,0x00,0x04,0x10,0x40,0x00,0x04,0xF0,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//"日",1
	0x00,0x00,0x00,0x00,0x00,0xF8,0x01,0x00,0x00,0x00,0x00,0x00,0xFC,0xFF,0xFF,0x7F,0x00,0x06,0x00,0x40,0x00,0x3C,0x02,0x30,0x00,0x01,0x02,0x0C,0x00,0x01,0x82,0x03,
	0x00,0x81,0x7F,0x00,0xFC,0x7F,0x0E,0x00,0x00,0x01,0xF2,0x00,0x00,0x01,0x02,0x03,0x00,0xFF,0x03,0x1C,0x00,0x01,0x02,0x38,0x00,0x00,0x03,0x30,0x00,0x00,0x00,0x00,//"快",2
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x80,0x00,0x04,0xE0,0xFF,0x01,0x03,0x40,0x80,0xE0,0x00,0x40,0x80,0x3C,0x08,0x20,0x80,0x10,0x10,0x20,0x80,0x00,0x30,
	0x20,0xFF,0xFF,0x3F,0x30,0x80,0x00,0x00,0x10,0x80,0x08,0x00,0x18,0x80,0x30,0x00,0x10,0x80,0xC0,0x01,0x00,0xC0,0x80,0x0F,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00 //"乐",3
#endif
#endif
    };

/* turn on selected led on, and the others will off */
static void display_turn_led_on(uint8_t index, bool on)
{
	if( index >= DISPLAY_LED_NUM)
		return;

	if(on == false) {
		nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(DISPLAY_GPIO_4, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(DISPLAY_GPIO_5, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(DISPLAY_GPIO_6, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(DISPLAY_GPIO_7, NRF_GPIO_PIN_NOPULL);

		return;
	}

#if 1
	/* reinit all led gpio as output */
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_0);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_1);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_2);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_3);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_4);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_5);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_6);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_7);
#else
	nrf_gpio_cfg_output(DISPLAY_GPIO_0);
	nrf_gpio_cfg_output(DISPLAY_GPIO_1);
	nrf_gpio_cfg_output(DISPLAY_GPIO_2);
	nrf_gpio_cfg_output(DISPLAY_GPIO_3);
	nrf_gpio_cfg_output(DISPLAY_GPIO_4);
	nrf_gpio_cfg_output(DISPLAY_GPIO_5);
	nrf_gpio_cfg_output(DISPLAY_GPIO_6);
	nrf_gpio_cfg_output(DISPLAY_GPIO_7);
#endif

	switch(index){
		case 0:
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_0);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 1:
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_0);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			break;
		case 2:
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_0);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 3:
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_0);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			break;
		case 4:
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_0);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 5:
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_0);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			break;
		case 6:
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_set(DISPLAY_GPIO_0);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			break;
		case 7:
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_0);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 8:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_1);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 9:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_1);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			break;
		case 10:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_1);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 11:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_1);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			break;
		case 12:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_1);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 13:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_1);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			break;
		case 14:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_set(DISPLAY_GPIO_1);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			break;
		case 15:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_1);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 16:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_2);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 17:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_2);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			break;
		case 18:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_2);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 19:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_2);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			break;
		case 20:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_2);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 21:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_2);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			break;
		case 22:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_set(DISPLAY_GPIO_2);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			break;
		case 23:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_3, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_2);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 24:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_3);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 25:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_3);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			break;
		case 26:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_3);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 27:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_3);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			break;
		case 28:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_set(DISPLAY_GPIO_3);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		case 29:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_3);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			break;
		case 30:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_7);
			nrf_gpio_pin_set(DISPLAY_GPIO_3);
			nrf_gpio_pin_set(DISPLAY_GPIO_4);
			nrf_gpio_pin_set(DISPLAY_GPIO_5);
			nrf_gpio_pin_set(DISPLAY_GPIO_6);
			break;
		case 31:
			nrf_gpio_cfg_input(DISPLAY_GPIO_0, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(DISPLAY_GPIO_2, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_pin_clear(DISPLAY_GPIO_5);
			nrf_gpio_pin_clear(DISPLAY_GPIO_6);
			nrf_gpio_pin_clear(DISPLAY_GPIO_4);
			nrf_gpio_pin_clear(DISPLAY_GPIO_3);
			nrf_gpio_pin_set(DISPLAY_GPIO_7);
			break;
		default:break;

	}
}

/* length is 20, the last 16 are for display */
void display_update_data(uint8_t *p_buf, uint8_t length)
{
	if(p_buf == NULL) {
		log_d("[DISP] no data!\n");
		return;
	}

	static uint8_t package_num = 0;
	uint8_t index = p_buf[1];
	uint16_t len = length - 4;
	uint16_t len_add = display_rcv_word_cnt * 4 * DISPLAY_LED_NUM;
	/* log_d("[DISP] %s index=%d, len=%d. cnt=%d, len_add=%d\n", __func__, */
			/* index, len, display_rcv_word_cnt, len_add); */
	for(int i=0; i<len; i++ ) {
		wData[index * len + len_add + i] = p_buf[4+i];
	}
	if(++package_num >= 4*DISPLAY_WORDS_CNT) {
		package_num = 0;
		if( ++display_rcv_word_cnt >= DISPLAY_WORDS_CNT) {
			display_rcv_word_cnt = 0;
		}
	}
}

void display_one_line(uint32_t line_data)
{
	static uint32_t tmp = 0;
	/* static uint8_t i = DISPLAY_LED_NUM - 1; */
	static uint8_t i = 0;
	/* for(uint8_t i=0; i<DISPLAY_LED_NUM, i++) */
	{
		tmp = line_data & (1 << (DISPLAY_LED_NUM - 1 - i));
		display_turn_led_on(i, (tmp != 0));
	}
	if(++i >= DISPLAY_LED_NUM)
		i = 0;
}

uint32_t display_timer_start(void)
{
	uint32_t err_code = NRF_SUCCESS;
	if(display_enable_hw_timer) {
		nrf_drv_timer_resume(&TIMER_LED);
	}

/* #if ENABLE_DISPLAY_TIMER */
	else if(DISPLAY_WORDS_ZERO_FOR_TEST == display_words_num) {
		/* ToDoList: this will be set by nus command later. */
		/* display_timer_period = 0x2000; */
		if (!m_display_ct.timer_running) {
			err_code = app_timer_start(m_display_ct.timer_id, display_timer_period, NULL);
			APP_ERROR_CHECK(err_code);
			if (err_code != NRF_SUCCESS) {
				log_d("[DISP] display timer start fail\n");
				return err_code;
			}
				log_d("[DISP] display timer start sucess\n");

			m_display_ct.timer_running = true;
		}
	}
/* #endif */

    return err_code;
}

uint32_t display_timer_stop(void)
{
	uint32_t err_code = NRF_SUCCESS;
	if(display_enable_hw_timer) {
		nrf_drv_timer_pause(&TIMER_LED);
	}

/* #if ENABLE_DISPLAY_TIMER */
	else if(DISPLAY_WORDS_ZERO_FOR_TEST == display_words_num) {
		if (m_display_ct.timer_running) {
			err_code = app_timer_stop(m_display_ct.timer_id);
			if (err_code != NRF_SUCCESS) {
				log_d("[DISP] display timer stop fail\n");
				return err_code;
			}
				log_d("[DISP] display timer stoped \n");

			m_display_ct.timer_running = false;
		}
	}
/* #endif */

    return err_code;
}
uint32_t display_change_direction(bool dir)
{
	if(dir) {
		display_dir = true;
		if(DISPLAY_WORDS_ONE == display_words_num) {
/* #if DISPLAY_ONE_WORD */
			display_cur_line = display_cur_word * DISPLAY_LED_NUM;

			if(++display_cur_word >= sizeof(wData)/4/DISPLAY_LED_NUM) {
				display_cur_word = 0;

				display_repeat_cnt = 0;
				/* display_timer_start(); */
			}
		}
/* #else */
		else {
			display_cur_line = 0;
		}
/* #endif */
	}
	else {
		display_dir = false;
		if(DISPLAY_WORDS_ONE == display_words_num) {
/* #if DISPLAY_ONE_WORD */
			display_cur_line = (display_cur_word+1) * DISPLAY_LED_NUM - 1;
		}
/* #else */
		else {
			display_cur_line = sizeof(wData)/4 - 1;
		}
/* #endif */
	}

	return 0;
}

uint32_t display_get_repeat_cnt(void)
{
	return display_repeat_cnt;
}

static void display_work()
{
	static uint32_t cnt_line = 0;
	static uint32_t tmp = 0x0;
	/* if (i++ <= DISPLAY_LED_NUM - 1) { */
	{
		display_one_line(tmp);
	}

	if(cnt_line%(DISPLAY_LED_NUM * DISPLAY_LINE_DELAY) == 0) {
		/* if(cnt_line%(DISPLAY_LED_NUM << 4) == 0) { */
			/* log_d("[DISP] l:%3d cnt:%8d, t:0x%x\n", display_cur_line, cnt_line, tmp); */
			/* ble_printf(&wData[display_cur_line], 4); */
			/* if(display_repeat_cnt++ > 80) { */
				/* display_timer_stop(); */
			/* } */
		/* } */

		tmp = 0;
		if(display_cur_line >= sizeof(wData)/4) {
			display_cur_line = 0;
			return;
		}
		if(display_cur_line < 0) {
			return;
		}
		if(display_dir == false) {
			tmp |= wData[display_cur_line*4]<<24;
			tmp |= wData[display_cur_line*4+1]<<16;
			tmp |= wData[display_cur_line*4+2]<<8;
			tmp |= wData[display_cur_line*4+3];
			if(DISPLAY_WORDS_ONE == display_words_num) {
/* #if DISPLAY_ONE_WORD */
				if(++display_cur_line >= (display_cur_word+1) * DISPLAY_LED_NUM) {
					display_cur_line = display_cur_word * DISPLAY_LED_NUM;
				}
			}
/* #else */
			else {
				if(++display_cur_line >= sizeof(wData)/4) {
					display_cur_line = 0;
				}
			}
/* #endif */
		}
		else {
			tmp |= wData[display_cur_line*4]<<24;
			tmp |= wData[display_cur_line*4+1]<<16;
			tmp |= wData[display_cur_line*4+2]<<8;
			tmp |= wData[display_cur_line*4+3];
			if(DISPLAY_WORDS_ONE == display_words_num) {
/* #if DISPLAY_ONE_WORD */
				if(--display_cur_line < display_cur_word * DISPLAY_LED_NUM) {
					display_cur_line = (display_cur_word+1) * DISPLAY_LED_NUM - 1;
				}
			}
/* #else */
			else {
				if(display_cur_line-- == 0) {
					display_cur_line = sizeof(wData)/4 - 1;
				}
			}
/* #endif */
		}
	}

	cnt_line++;

}

static void display_work_timerout(void *p_context)
{
	/* 0 for leds test */
	if(DISPLAY_WORDS_ZERO_FOR_TEST == display_words_num) {
/* #if DISPLAY_LED_TEST */
		/* only for led test */
		static uint8_t i = 0;
		if (i++ <= DISPLAY_LED_NUM - 1) {
			display_turn_led_on(i, true);
			/* uint8_t buf=nrf_gpio_word_byte_read(&NRF_GPIO->PIN_CNF[4], 2); */
			/* log_d("[DISP] %s %d\n",__func__, i); */
			/* log_d("[DISP] %s %d 0x%2x\n",__func__, i, buf); */
			/* ble_log_d("[DISP] %s %d 0x%2x\n",__func__, i, buf); */
			/* i++; */
		}
		else
			i = 0;
/* #else */
	} else {
		display_work();
	}
/* #endif */
}

static uint32_t display_timer_init(void)
{
	uint32_t err_code;

/* #if ENABLE_DISPLAY_TIMER */
	m_display_ct.timer_running = false;
	err_code = app_timer_create(&m_display_ct.timer_id, APP_TIMER_MODE_REPEATED, display_work_timerout);
	APP_ERROR_CHECK(err_code);
	if(err_code != NRF_SUCCESS) {
		log_d("[DISP] create display timer fail\n");
	}
	else {
		log_d("[DISP] create display timer sucess\n");
	}
/* #endif */

	return err_code;
}

/**
 * @brief Handler for timer events.
 */
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE1:
#if 0
			static uint32_t cnt=0;
			if(cnt++ == 100) {
				cnt = 0;
				nrf_gpio_cfg_output(LED_RGB_RED);
				if(nrf_gpio_pin_read(LED_RGB_RED))
					nrf_gpio_pin_clear(LED_RGB_RED);
				else
					nrf_gpio_pin_set(LED_RGB_RED);
			}
#else
			display_work();
#endif
            break;
        default: break;
    }
}

static uint32_t display_drv_timer_init(void)
{
    uint32_t time_us = display_hw_timer_period; //Time between consecutive compare events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIMER_LED, &timer_cfg, timer_led_event_handler);
	APP_ERROR_CHECK(err_code);
	if(err_code == NRF_SUCCESS) {
		log_d("[DISP] %s success.\n", __func__);
	}

    time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_LED, time_us);

    nrf_drv_timer_extended_compare(
         &TIMER_LED, NRF_TIMER_CC_CHANNEL1, time_ticks, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, true);

    nrf_drv_timer_enable(&TIMER_LED);

	return err_code;
}

uint32_t display_init(beacon_flash_db_t *pdata)
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

	/* set default value */
	pdata->yyb_data.display_timer_period = 0x2000;
	display_timer_period = pdata->yyb_data.display_timer_period;
	if((0xffffffff == display_timer_period) || (0 == display_timer_period)) {
		display_timer_period = 0x2000;
	}
	display_hw_timer_period = pdata->yyb_data.display_hw_timer_period;
	if((0xffffffff == display_hw_timer_period) || (0 == display_hw_timer_period)) {
		display_hw_timer_period = 40;
	}
	display_words_num = pdata->yyb_data.display_words_num;
	log_d("[DISP] %s display words num:%d\n", __func__, display_words_num);
	display_enable_hw_timer = pdata->yyb_data.display_enable_hw_timer;
	log_d("[DISP] %s display_enable_hw_timer:0x%x, address:0x%x\n", __func__,
			display_enable_hw_timer, &pdata->yyb_data.display_enable_hw_timer);
	if(display_enable_hw_timer) {
		err_code = display_drv_timer_init();
	}
/* #if ENABLE_DISPLAY_TIMER */
	else if(DISPLAY_WORDS_ZERO_FOR_TEST == display_words_num) {
		err_code = display_timer_init();
		if(err_code == NRF_SUCCESS) {
			err_code = display_timer_start();
		}
/* #endif */
	}
	return err_code;
}
