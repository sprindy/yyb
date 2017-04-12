
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_DRV_H__
#define __FLASH_DRV_H__

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
/* #include "hlog_api.h" */
#include "stdint.h"
typedef enum
{
    FLASH_IRQ_NO = 0x0,
    FLASH_IRQ_NORMAL = 0x1,
} FLASH_IRQ_STATE;
typedef enum
{
    FLASH_WP_BL = 0x0,
    FLASH_WP_SYS = 0x6,
} FLASH_WP_AREA;

#define FLASH_HLOG_ENABLE 0

#if FLASH_HLOG_ENABLE
#define flash_emerg(fmt, ...) \
	hlog_printf(HLOG_LVL_EMERG, HLOG_TAG_FLASH, fmt, ##__VA_ARGS__)
#define flash_err(fmt, ...) \
	hlog_printf(HLOG_LVL_ERR, HLOG_TAG_FLASH, fmt, ##__VA_ARGS__)
#define flash_warning(fmt, ...) \
	hlog_printf(HLOG_LVL_WARNING, HLOG_TAG_FLASH, fmt, ##__VA_ARGS__)
#define flash_info(fmt, ...) \
	hlog_printf(HLOG_LVL_INFO, HLOG_TAG_FLASH, fmt, ##__VA_ARGS__)
#define flash_debug(fmt, ...) \
	hlog_printf(HLOG_LVL_DEBUG, HLOG_TAG_FLASH, fmt, ##__VA_ARGS__)
#else /* flash_HLOG_ENABLE */
#define flash_emerg(fmt, ...) \
	printf("[FLASH][EMR] :" fmt, ##__VA_ARGS__)
#define flash_err(fmt, ...) \
	printf("[FLASH][ERR] :" fmt, ##__VA_ARGS__)
#define flash_warning(fmt, ...) \
	printf("[FLASH][WARN]:" fmt, ##__VA_ARGS__)
#define flash_info(fmt, ...) \
	printf("[FLASH][INFO]:" fmt, ##__VA_ARGS__)
#define flash_debug(fmt, ...) \
	printf("[FLASH][DBG] :" fmt, ##__VA_ARGS__)
#endif /* flash_HLOG_ENABLE */

int flash_init(void);
uint32_t VR_flash_erase(uint32_t offset, uint32_t length);
uint32_t VR_flash_write(void * source, uint32_t offset, uint32_t length);
uint32_t VR_flash_read(uint32_t addr, void *buff, unsigned size);
uint32_t flash_ob_status(void);

uint32_t flash_ob_wp_disable(FLASH_WP_AREA wp_area);
uint32_t flash_ob_wp_enable(FLASH_WP_AREA wp_area);
uint32_t flash_ob_wp_is_BL_protected(void);
uint32_t flash_ob_rdp_level_set(uint32_t level);
uint32_t flash_ob_rdp_level_get(void);
 typedef enum {
	 PART_BL,		//32K		//0,1 sector
	 PART_MISC_DATA,//16K		//2 sector
	 PART_LOG,		//16K		//3 sector
	 PART_DISPLAY,	//64K		//4 sector
	 PART_FOTA,		//128K		//5 sector
	 PART_RTOS,		//128K		//6 sector
	 PART_MFG,		//128K		//7 sector
	 PART_ERROR,
 }FLASH_PART;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ifndef __FLASH_DRV_H__ */
