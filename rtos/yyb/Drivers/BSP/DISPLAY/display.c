#include "stm32f4xx_hal.h"
#include "display.h"

/* 0: led on */
/* 1: led off */
void display_one_line(uint32_t line)
{
	uint32_t i = 0;
	uint32_t t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D00, GPIO_PIN_LED_D00, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D01, GPIO_PIN_LED_D01, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D02, GPIO_PIN_LED_D02, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D03, GPIO_PIN_LED_D03, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D04, GPIO_PIN_LED_D04, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D05, GPIO_PIN_LED_D05, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D06, GPIO_PIN_LED_D06, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D07, GPIO_PIN_LED_D07, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D08, GPIO_PIN_LED_D08, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D09, GPIO_PIN_LED_D09, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D10, GPIO_PIN_LED_D10, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D11, GPIO_PIN_LED_D11, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D12, GPIO_PIN_LED_D12, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D13, GPIO_PIN_LED_D13, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D14, GPIO_PIN_LED_D14, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D15, GPIO_PIN_LED_D15, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D16, GPIO_PIN_LED_D16, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D17, GPIO_PIN_LED_D17, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D18, GPIO_PIN_LED_D18, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D19, GPIO_PIN_LED_D19, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D20, GPIO_PIN_LED_D20, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D21, GPIO_PIN_LED_D21, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D22, GPIO_PIN_LED_D22, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D23, GPIO_PIN_LED_D23, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D24, GPIO_PIN_LED_D24, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D25, GPIO_PIN_LED_D25, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D26, GPIO_PIN_LED_D26, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D27, GPIO_PIN_LED_D27, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D28, GPIO_PIN_LED_D28, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D29, GPIO_PIN_LED_D29, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D30, GPIO_PIN_LED_D30, (GPIO_PinState)(GPIO_PIN_SET && t));
	t = line & (1 << (i++));
	HAL_GPIO_WritePin(GPIO_PORT_LED_D31, GPIO_PIN_LED_D31, (GPIO_PinState)(GPIO_PIN_SET && t));

}
