#include <stdio.h>
#include <string.h>
#include "display.h"
#include "pca20006.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "sdk_config.h"
#include "nrf_drv_timer.h"
#include "uart_drv.h"
#include "log.h"

#define DISPLAY_WORDS_CNT      8
#define DISPLAY_ONE_WORD_BYTES (DISPLAY_LED_NUM * 4)
#define DISPLAY_DATA_BYTE_LEN  (DISPLAY_WORDS_CNT * DISPLAY_ONE_WORD_BYTES)

typedef struct
{
    app_timer_id_t       timer_id;
    bool                 timer_running;
    bool                 initialized;
}display_context_t;

static display_context_t m_display_ct = {0};
static bool display_dir = false;
#if DISPLAY_ONE_WORD
static uint8_t display_cur_word = 0;
static uint8_t display_repeat_cnt = 0;
static uint8_t display_rcv_word_cnt = 0;
#endif
static uint32_t display_cur_line = 0;

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
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x04,0x00,0x04,0x10,0x08,0x00,0x04,0x10,0x10,0x02,0x04,0x10,0x10,
	0x02,0x04,0x10,0x20,0x02,0x04,0x10,0xC0,0x02,0x04,0x11,0xC0,0x02,0x84,0x13,0x80,0x02,0xC4,0x17,0x00,0x02,0x64,0x12,0x08,
	0x02,0x3C,0x10,0x08,0x02,0x14,0x10,0x04,0x22,0x04,0x10,0x0E,0x1A,0x07,0xFF,0xFE,0x1E,0x07,0xFF,0xFC,0x02,0x04,0x10,0x00,
	0x02,0x0C,0x10,0x00,0x02,0x34,0x10,0x00,0x03,0xE4,0x12,0x00,0x02,0xC4,0x11,0x00,0x02,0x84,0x11,0x80,0x02,0x04,0x10,0xC0,
	0x02,0x04,0x10,0xE0,0x06,0x04,0x30,0x70,0x06,0x04,0x70,0x38,0x02,0x0C,0x70,0x3C,0x00,0x0C,0x30,0x00,0x00,0x04,0x10,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*"亲",0*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x04,0x00,0x0E,0x00,0x04,0x04,0x30,0x40,0x08,
	0x04,0x10,0x40,0x12,0x04,0x10,0x40,0x22,0x06,0x10,0x40,0x62,0x05,0x90,0x40,0xC2,0x05,0xF0,0x43,0x84,0x04,0xD0,0x4E,0x04,
	0x08,0x10,0xFC,0x04,0x08,0x17,0xEC,0x08,0x0C,0x12,0x4A,0x08,0x0A,0x10,0x49,0x98,0x09,0xD0,0x48,0xD0,0x09,0xD0,0x48,0x70,
	0x08,0x10,0x48,0x60,0x08,0x10,0x48,0xF0,0x18,0x10,0x49,0x90,0x10,0x70,0x4B,0x18,0x10,0xD0,0x4E,0x18,0x1F,0x90,0x5C,0x08,
	0x36,0x11,0x88,0x0C,0x30,0x12,0x80,0x0C,0x30,0x1C,0x00,0x0C,0x10,0x38,0x00,0x0C,0x00,0x18,0x00,0x0C,0x00,0x00,0x00,0x08,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*"爱",1*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFC,0x00,0xFF,0xFF,0xFC,
	0x00,0x80,0x40,0x10,0x01,0x80,0x40,0x10,0x3E,0x80,0x40,0x10,0x38,0x80,0x40,0x10,0x10,0x80,0x40,0x10,0x00,0x80,0x40,0x10,
	0x00,0xFF,0xFF,0xFC,0x01,0xFF,0xFF,0xF8,0x00,0x81,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x72,0x00,0x00,
	0x01,0xE1,0x00,0x00,0x0F,0xC0,0xE0,0x00,0x3E,0x40,0x78,0x10,0x18,0x40,0x10,0x10,0x00,0x40,0x00,0x08,0x00,0x40,0x00,0x0C,
	0x00,0x40,0x00,0x0E,0x00,0x40,0x00,0x1C,0x00,0xFF,0xFF,0xF8,0x01,0xFF,0xFF,0xE0,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*"的",2*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x02,0x02,0x00,0x60,0x02,0x02,0x00,0x60,0x02,0x02,0x00,0x40,0x02,0x02,0x00,0xC0,
	0x03,0xFF,0xFF,0x80,0x03,0xFF,0xFF,0x80,0x02,0x02,0x01,0x00,0x04,0x06,0x01,0x02,0x04,0x02,0x11,0x04,0x00,0x80,0x1E,0x0C,
	0x00,0x40,0x18,0x18,0x00,0x78,0x30,0x30,0x00,0x3C,0x20,0x60,0x00,0x00,0x41,0xC0,0x00,0x00,0x5F,0x80,0x1F,0xFF,0xFE,0x00,
	0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xF8,0x10,0x04,0x80,0x18,
	0x10,0x08,0x40,0x08,0x00,0x30,0x60,0x08,0x00,0x60,0x30,0x08,0x01,0xC0,0x38,0x08,0x00,0xC0,0x1D,0xF8,0x00,0x40,0x00,0x18,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*"珧",3*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x08,0x00,0x00,0x80,0x08,
	0x00,0x03,0x00,0x08,0x00,0x1C,0x00,0x08,0x00,0x78,0x00,0x08,0x07,0xE0,0x10,0x08,
	0x0F,0xA0,0x10,0x08,0x06,0x20,0x10,0x08,0x04,0x20,0x10,0x08,0x00,0x20,0x10,0x08,
	0x00,0x20,0x10,0x08,0x00,0x20,0x10,0x08,0x00,0x20,0x10,0x08,0x3F,0xFF,0xFF,0xF8,
	0x3F,0xFF,0xFF,0xF8,0x00,0x20,0x10,0x08,0x00,0x20,0x10,0x08,0x00,0x20,0x10,0x08,
	0x00,0x20,0x10,0x08,0x00,0x20,0x10,0x08,0x00,0x20,0x10,0x08,0x00,0x20,0x20,0x08,
	0x00,0x20,0x60,0x08,0x00,0x60,0x20,0x08,0x00,0x60,0x00,0x08,0x00,0x20,0x00,0x18,
	0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*"生",0*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFC,
	0x0F,0xFF,0xFF,0xF8,0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,
	0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,
	0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,
	0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,0x08,0x02,0x00,0x20,0x0F,0xFF,0xFF,0xFC,
	0x1F,0xFF,0xFF,0xFC,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*"日",1*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0x00,0x00,0x1F,0x00,0x00,
	0x00,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0xFE,0x3F,0xFF,0xFF,0xFC,
	0x00,0x40,0x00,0x00,0x00,0x20,0x00,0x02,0x00,0x38,0x40,0x04,0x00,0x1C,0x40,0x0C,
	0x00,0x80,0x40,0x18,0x00,0x80,0x40,0x30,0x00,0x80,0x40,0x60,0x00,0x80,0x41,0xC0,
	0x00,0x80,0x4F,0x00,0x3F,0xFF,0xFC,0x00,0x3F,0xFF,0xE0,0x00,0x10,0x80,0x60,0x00,
	0x00,0x80,0x58,0x00,0x00,0x80,0x47,0x00,0x00,0x80,0x41,0xC0,0x00,0x80,0x40,0xE0,
	0x00,0x80,0x40,0x70,0x01,0xFF,0xC0,0x38,0x01,0xFF,0xC0,0x1C,0x00,0x80,0x40,0x0E,
	0x00,0x00,0xC0,0x0C,0x00,0x00,0x40,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*"快",2*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x10,
	0x00,0x00,0x00,0x20,0x00,0x01,0x00,0x20,0x00,0x3F,0x80,0xC0,0x07,0xFF,0x01,0x80,
	0x07,0x81,0x03,0x00,0x02,0x01,0x06,0x00,0x02,0x01,0x1C,0x00,0x02,0x01,0x18,0x08,
	0x04,0x01,0x08,0x08,0x04,0x01,0x00,0x08,0x04,0x01,0x00,0x0C,0x04,0x01,0x00,0x0E,
	0x04,0xFF,0xFF,0xFE,0x04,0xFF,0xFF,0xFC,0x04,0x41,0x00,0x00,0x0C,0x01,0x00,0x00,
	0x08,0x01,0x00,0x00,0x08,0x01,0x10,0x00,0x18,0x01,0x08,0x00,0x18,0x01,0x04,0x00,
	0x18,0x01,0x07,0x00,0x08,0x01,0x03,0x80,0x00,0x03,0x01,0xF0,0x00,0x03,0x00,0xF0,
	0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 /*"乐",3*/
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

	/* reinit all led gpio as output */
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_0);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_1);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_2);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_3);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_4);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_5);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_6);
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_7);

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
void display_receive_data(uint8_t *p_buf, uint8_t length)
{
	if(p_buf == NULL) {
		printf("no data!\n");
		return;
	}

	static uint8_t package_num = 0;
	uint8_t index = p_buf[1];
	uint16_t len = length - 4;
	uint16_t len_add = display_rcv_word_cnt * 4 * DISPLAY_LED_NUM;
	/* printf("%s index=%d, len=%d. cnt=%d, len_add=%d\n", __func__, */
			/* index, len, display_rcv_word_cnt, len_add); */
	for(int i=0; i<len; i++ ) {
		wData[index * len + len_add + i] = p_buf[4+i];
	}
	if(++package_num >= 8) {
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
#if ENABLE_DISPLAY_TIMER
    if (!m_display_ct.timer_running) {
        err_code = app_timer_start(m_display_ct.timer_id, DISPLAY_TIMER_PERIOD, NULL);
        if (err_code != NRF_SUCCESS) {
			printf("display timer start fail\n");
            return err_code;
        }
			printf("display timer start sucess\n");

        m_display_ct.timer_running = true;
    }

#endif
    return err_code;
}

uint32_t display_timer_stop(void)
{
	uint32_t err_code = NRF_SUCCESS;
#if ENABLE_DISPLAY_TIMER
    if (m_display_ct.timer_running) {
        err_code = app_timer_stop(m_display_ct.timer_id);
        if (err_code != NRF_SUCCESS) {
			printf("display timer stop fail\n");
            return err_code;
        }
			printf("display timer stoped \n");

        m_display_ct.timer_running = false;
    }

#endif
    return err_code;
}
uint32_t display_change_direction(bool dir)
{
	if(dir) {
		display_dir = true;
#if DISPLAY_ONE_WORD
		display_cur_line = display_cur_word * DISPLAY_LED_NUM;

		if(++display_cur_word >= sizeof(wData)/4/DISPLAY_LED_NUM) {
			display_cur_word = 0;

			display_repeat_cnt = 0;
			/* display_timer_start(); */
		}
#else
		display_cur_line = 0;
#endif
	}
	else {
		display_dir = false;
#if DISPLAY_ONE_WORD
		display_cur_line = (display_cur_word+1) * DISPLAY_LED_NUM - 1;
#else
		display_cur_line = sizeof(wData)/4 - 1;
#endif
	}

	return 0;
}

uint32_t display_get_repeat_cnt(void)
{
	return display_repeat_cnt;
}

static void display_work_timerout(void *p_context)
{
#if DISPLAY_LED_TEST
	/* only for led test */
	static uint8_t i = 0;
	/* if (i++ <= DISPLAY_LED_NUM - 1) { */
	/* if (i++ <= 3) { */
	if(1)
	{
		display_turn_led_on(4, true);
		uint8_t buf=nrf_gpio_word_byte_read(&NRF_GPIO->PIN_CNF[4], 2);
		/* printf("%s %d\n",__func__, i); */
		printf("%s %d 0x%2x\n",__func__, i, buf);
		ble_printf("%s %d 0x%2x\n",__func__, i, buf);
	}
	else
		i = 0;
#else
	static uint32_t cnt_line = 0;
	static uint32_t tmp = 0x0;
	/* if (i++ <= DISPLAY_LED_NUM - 1) { */
	{
		display_one_line(tmp);
	}

	if(cnt_line%(DISPLAY_LED_NUM * DISPLAY_LINE_DELAY) == 0) {
		if(cnt_line%(DISPLAY_LED_NUM << 4) == 0) {
			/* printf("l:%3d cnt:%8d, t:0x%x\n", display_cur_line, cnt_line, tmp); */
			ble_printf(&wData[display_cur_line], 4);
			if(display_repeat_cnt++ > 80) {
				/* display_timer_stop(); */
			}
		}

		tmp = 0;
		if(display_cur_line >= sizeof(wData)/4) {
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
#if DISPLAY_ONE_WORD
			if(++display_cur_line >= (display_cur_word+1) * DISPLAY_LED_NUM) {
				display_cur_line = display_cur_word * DISPLAY_LED_NUM;
			}
#else
			if(++display_cur_line >= sizeof(wData)/4) {
				display_cur_line = 0;
			}
#endif
		}
		else {
			tmp |= wData[display_cur_line*4]<<24;
			tmp |= wData[display_cur_line*4+1]<<16;
			tmp |= wData[display_cur_line*4+2]<<8;
			tmp |= wData[display_cur_line*4+3];
#if DISPLAY_ONE_WORD
			if(--display_cur_line < display_cur_word * DISPLAY_LED_NUM) {
				display_cur_line = (display_cur_word+1) * DISPLAY_LED_NUM - 1;
			}
#else
			if(display_cur_line-- == 0) {
				display_cur_line = sizeof(wData)/4 - 1;
			}
#endif
		}
	}

	cnt_line++;
#endif
}

static uint32_t display_timer_init(void)
{
	uint32_t err_code;

#if ENABLE_DISPLAY_TIMER
	m_display_ct.timer_running = false;
	err_code = app_timer_create(&m_display_ct.timer_id, APP_TIMER_MODE_REPEATED, display_work_timerout);
	if(err_code != NRF_SUCCESS) {
		printf("create display timer fail\n");
	}
	else {
		printf("create display timer sucess\n");
	}
#endif

	return err_code;
}

const nrf_drv_timer_t TIMER_LED = NRF_DRV_TIMER_INSTANCE(0);
/**
 * @brief Handler for timer events.
 */
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    static uint32_t i;
    /* uint32_t led_to_invert = ((i++) % LEDS_NUMBER); */

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
			nrf_gpio_cfg_output(LED_RGB_RED);
			if(nrf_gpio_pin_read(LED_RGB_RED))
				nrf_gpio_pin_clear(LED_RGB_RED);
			else
				nrf_gpio_pin_set(LED_RGB_RED);

            /* bsp_board_led_invert(led_to_invert); */
            break;

        default:
            //Do nothing.
            break;
    }
}

static uint32_t display_drv_timer_init(void)
{
    uint32_t time_ms = 500; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;

    //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIMER_LED, &timer_cfg, timer_led_event_handler);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_LED, time_ms);

    nrf_drv_timer_extended_compare(
         &TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrf_drv_timer_enable(&TIMER_LED);

	return err_code;
}

uint32_t display_init(void)
{
	uint32_t err_code;

	/* nrf_gpio_cfg_output(DISPLAY_GPIO_0); */
	nrf_gpio_cfg_output_high_drive(DISPLAY_GPIO_0);
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

	/* err_code = display_drv_timer_init(); */
	err_code = display_timer_init();
	if(err_code == NRF_SUCCESS) {
		err_code = display_timer_start();
	}

	return err_code;
}

