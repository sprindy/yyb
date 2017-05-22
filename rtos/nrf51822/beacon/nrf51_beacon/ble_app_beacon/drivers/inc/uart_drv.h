#ifndef UART_DRV_H
#define UART_DRV_H

#include <stdint.h>

uint32_t uart_init(void);

void ble_nus_uart_init(void);
uint32_t ble_printf(void);

#endif
