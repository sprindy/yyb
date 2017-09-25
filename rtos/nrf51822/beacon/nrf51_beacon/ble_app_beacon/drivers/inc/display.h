
#ifndef __LED_DISPLAY_H__
#define __LED_DISPLAY_H__

#include <stdbool.h>
#include <stdint.h>
#include "log.h"
#include "beacon.h"

void display_update_data(uint8_t *p_buf, uint8_t length);

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

uint32_t display_init(beacon_flash_db_t *pdata);


#endif
