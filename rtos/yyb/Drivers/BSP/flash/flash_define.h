/*
 * HTC Corporation Proprietary Rights Acknowledgment
 *
 * Copyright (C) 2013 HTC Corporation
 *
 * All Rights Reserved.
 *
 * The information contained in this work is the exclusive property of HTC Corporation
 * ("HTC").  Only the user who is legally authorized by HTC ("Authorized User") has
 * right to employ this work within the scope of this statement.  Nevertheless, the
 * Authorized User shall not use this work for any purpose other than the purpose
 * agreed by HTC.  Any and all addition or modification to this work shall be
 * unconditionally granted back to HTC and such addition or modification shall be
 * solely owned by HTC.  No right is granted under this statement, including but not
 * limited to, distribution, reproduction, and transmission, except as otherwise
 * provided in this statement.  Any other usage of this work shall be subject to the
 * further written consent of HTC.
 */

#ifndef __HTC_MEMORY_DEFINE_H__
#define __HTC_MEMORY_DEFINE_H__
#define USE_512KB_MCU
#ifdef USE_512KB_MCU
/* Internal Flash */
#define REGION_FLASH_START	0x08000000
#define REGION_FLASH_SECTOR0 0x08000000	//16K
#define REGION_FLASH_SECTOR1 0x08004000	//16K
#define REGION_FLASH_SECTOR2 0x08008000	//16K
#define REGION_FLASH_SECTOR3 0x0800C000	//16K
#define REGION_FLASH_SECTOR4 0x08010000	//64K
#define REGION_FLASH_SECTOR5 0x08020000	//128K
#define REGION_FLASH_SECTOR6 0x08040000	//128K
#define REGION_FLASH_SECTOR7 0x08060000	//128K
#define REGION_FLASH_END	 0x0807FFFF	//512K
#define SYSTEM_MEMORY_START	0x1FFF0000	//30K
#define SYSTEM_MEMORY_END	0x1FFF77FF
#define MAIN_MEMORY_START	0x20000000	//128K
#define MAIN_MEMORY_END		0x20020000

#define BL_FOTA			"fota"
#define BL_HEADER		"bootload"
#define SYS_HEADER		"systrtos"
#define DIS_HEADER		"dis_leds"
#define BLE_HEADER		"blutooth"
#define LOG_PART_START_ADDRESS REGION_FLASH_SECTOR3

#define IS_FLASH_PROGRAM_ADDRESS(ADDRESS)   (((ADDRESS) >=REGION_FLASH_SECTOR0 ) && ((ADDRESS) <= REGION_FLASH_END))
#define HTC_SYS_VERSION_LOCATION	(REGION_FLASH_SECTOR6+4096-64)
#define HTC_BOOT_VERSION_LOCATION  (REGION_FLASH_SECTOR0+4096-64)
#define HTC_BOOT_FOTA_VERSION_LOCATION	(HTC_BOOT_VERSION_LOCATION-REGION_FLASH_SECTOR0+REGION_FLASH_SECTOR4)
#else /* USE_1MB_MCU */



#endif /* __HTC_MEMORY_DEFINE_H__ */
#endif
