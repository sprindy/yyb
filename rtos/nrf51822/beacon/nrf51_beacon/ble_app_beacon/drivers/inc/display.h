
#ifndef __LED_DISPLAY_H__
#define __LED_DISPLAY_H__

#include <stdbool.h>
#include <stdint.h>
#include "log.h"

void display_receive_data(uint8_t *p_buf, uint8_t length);

uint32_t display_change_direction(bool dir);
uint32_t display_get_repeat_cnt(void);

uint32_t display_timer_start(void);
uint32_t display_timer_stop(void);

/**@brief display task initialization.
 *
 * @return
 * @retval NRF_SUCCESS
 */
uint32_t display_work_init(void);

uint32_t display_init(void);


#endif
