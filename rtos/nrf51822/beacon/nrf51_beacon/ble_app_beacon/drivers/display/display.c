#include <stdio.h>
#include <string.h>
#include "display.h"
#include "pca20006.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "uart_drv.h"

typedef struct
{
    app_timer_id_t       timer_id;
    bool                 timer_running;
    bool                 initialized;
}display_context_t;

static display_context_t m_display_ct = {0};
char wData[128] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x08,
	0x02,0x00,0x00,0x08,0x02,0x00,0x00,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x03,0xFF,0xFF,0xF8,
	0x03,0xFF,0xFF,0xF8,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x80,0x08,0x06,0x00,0x80,0x08,0x0E,0x00,0x00,0x08,0x06,0x00,0x00,0x18,
	0x02,0x00,0x00,0x18,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
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
	nrf_gpio_cfg_output(DISPLAY_GPIO_0);
	nrf_gpio_cfg_output(DISPLAY_GPIO_1);
	nrf_gpio_cfg_output(DISPLAY_GPIO_2);
	nrf_gpio_cfg_output(DISPLAY_GPIO_3);
	nrf_gpio_cfg_output(DISPLAY_GPIO_4);
	nrf_gpio_cfg_output(DISPLAY_GPIO_5);
	nrf_gpio_cfg_output(DISPLAY_GPIO_6);
	nrf_gpio_cfg_output(DISPLAY_GPIO_7);

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

void display_one_line(uint32_t line_data)
{
	static uint32_t tmp = 0;
	static uint8_t i = DISPLAY_LED_NUM - 1;
	/* for(uint8_t i=0; i<DISPLAY_LED_NUM, i++) */
	{
		tmp = line_data & (1 << (i));
		display_turn_led_on(i, (tmp != 0));
	}
	if(i-- == 0)
		i = DISPLAY_LED_NUM - 1;
}

uint32_t display_timer_start(void)
{
#if 1
    if (!m_display_ct.timer_running) {
        uint32_t err_code;

        err_code = app_timer_start(m_display_ct.timer_id, 0x10, NULL);
        if (err_code != NRF_SUCCESS) {
			printf("display timer start fail\n");
            return err_code;
        }

        m_display_ct.timer_running = true;
    }

#endif
    return NRF_SUCCESS;
}

static void display_work_timerout(void *p_context)
{
#define DISPLAY_LINE_DELAY  20
	static uint8_t i = 0;
	static uint8_t j = 0;

	static uint32_t cnt_line = 1;
	static uint32_t tmp = 0x0;
	if (i <= DISPLAY_LED_NUM - 1) {
		display_one_line(tmp);
		/* display_turn_led_on(i++, true); */
	}

	if(cnt_line%(DISPLAY_LED_NUM * DISPLAY_LINE_DELAY) == 0) {
		printf("i:%d j:%d cnt_line:%d, tmp:0x%x\n", i, j, cnt_line, tmp);
		i = 0;
		cnt_line = 0;
		tmp = 0;
		tmp |= wData[j*4]<<24;
	    tmp |= wData[j*4+1]<<16;
	    tmp |= wData[j*4+2]<<8;
	    tmp |= wData[j*4+3];
		if(++j == sizeof(wData)/4) {
			j = 0;
		}
	}

	cnt_line++;
}

uint32_t display_work_init(void)
{
	uint32_t err_code;
#if 1
	m_display_ct.timer_running = false;
	err_code = app_timer_create(&m_display_ct.timer_id, APP_TIMER_MODE_REPEATED, display_work_timerout);
	if(err_code != NRF_SUCCESS) {
		printf("create display timer fail");
	}
#endif

#if 0
	if(err_code == NRF_SUCCESS) {
		err_code = display_timer_start();
			if(err_code == NRF_SUCCESS) {
				printf("display timer start sucess\n");
			}
	}
#endif
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

