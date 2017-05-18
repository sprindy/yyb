/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef PCA20006_H
#define PCA20006_H

// LEDs and buttons definition for PCA20006 board (beacon) 
#define DISPLAY_LED_NUM 32
#define DISPLAY_GPIO_0	 0
#define DISPLAY_GPIO_1	 1
#define DISPLAY_GPIO_2	 2
#define DISPLAY_GPIO_3	 3
#define DISPLAY_GPIO_4	 4
#define DISPLAY_GPIO_5	 5
#define DISPLAY_GPIO_6	 6
#define DISPLAY_GPIO_7	15

#define LEDS_NUMBER    3

#define LED_RGB_RED    7
#define LED_RGB_GREEN  24
#define LED_RGB_BLUE   25

#define BSP_LED_0 LED_RGB_RED
#define BSP_LED_1 LED_RGB_GREEN
#define BSP_LED_2 LED_RGB_BLUE

#define BSP_LED_0_MASK    (1<<BSP_LED_0)
#define BSP_LED_1_MASK    (1<<BSP_LED_1)
#define BSP_LED_2_MASK    (1<<BSP_LED_2)
#define LEDS_MASK      (BSP_LED_0_MASK | BSP_LED_1_MASK | BSP_LED_2_MASK)
#define LEDS_INV_MASK  0x00000000

#define BUTTON_0       16
#define BUTTON_1       17
/* #define BUTTON_1       15 */
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BSP_BUTTON_0   BUTTON_0
#define BSP_BUTTON_1   BUTTON_1

#define BUTTONS_NUMBER 2

#define BSP_BUTTON_0_MASK (1<<BUTTON_0)
#define BSP_BUTTON_1_MASK (1<<BUTTON_1)
#define BUTTONS_MASK   (BSP_BUTTON_0_MASK | BSP_BUTTON_1_MASK)

#define BUTTONS_LIST { BUTTON_0, BUTTON_1 }

#define RX_PIN_NUMBER  21
#define TX_PIN_NUMBER  30
#define CTS_PIN_NUMBER 20
#define RTS_PIN_NUMBER 22
#define HWFC           false
#define UART_BAUDRATE  38400

/* config acc LIS2DH */
#define ACC_TIMER_PERIOD 0x800
#define ACC_PIN_MISO     10
#define ACC_PIN_MOSI      9
#define ACC_PIN_CSN      11
#define ACC_PIN_SCK      12
#define ACC_PIN_INT1     13
#define ACC_PIN_INT2     14
/* #define ACC_FREQUENCY  LIS2DH12_FREQUENCY */
#endif