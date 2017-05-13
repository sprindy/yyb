
#ifndef __LED_DISPLAY_H__
#define __LED_DISPLAY_H__

#include <stdbool.h>
#include <stdint.h>

uint32_t display_timer_start(void);

/**@brief display task initialization.
 *
 * @return
 * @retval NRF_SUCCESS
 */
uint32_t display_work_init(void);

uint32_t display_init(void);

#endif
