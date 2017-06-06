#ifndef __LOG_H__
#define __LOG_H__


#include <stdio.h>
#include "pca20006.h"

extern void ble_printf(char *fmt, ...);

enum {
	LOG_LVL_EMERG,
	LOG_LVL_ERR,
	LOG_LVL_WARNING,
	LOG_LVL_INFO,
	LOG_LVL_DEBUG,
	LOG_LVL_MAX
};

#define TAG_ACC  [ACC]
#define TAG_BTN  [BTN]
#define TAG_LED  [LED]
#define TAG_BLE  [BLE]
#define TAG_DISP [DISP]

#ifdef ENABLE_UART_DEBUG
#define log_d printf
#define acc_log_i(args...) printf(TAG_ACC args...)
#define acc_log_e(args...) printf(TAG_ACC args...)
#else
#define log_d(...)
#endif

#ifdef ENABLE_BLE_NUS_DEBUG
/* #define acc_log_d ble_printf */
/* #define acc_log_i(args...) ble_printf(TAG_ACC args...) */
/* #define acc_log_e(args...) ble_printf(TAG_ACC args...) */
#define ble_log_d ble_printf
#define ble_log_i(args...) ble_printf(TAG_BLE args...)
#define ble_log_e(args...) ble_printf(TAG_BLE args...)
#else
#define ble_log_d(...)
#endif


#endif /* __LOG_H__ */
