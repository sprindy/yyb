#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "display.h"
#include "gpio.h"

osThreadId displayTaskHandle;

static char data[] ={
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x08,
	0x02,0x00,0x00,0x08,0x02,0x00,0x00,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x03,0xFF,0xFF,0xF8,
	0x03,0xFF,0xFF,0xF8,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x40,0x08,0x02,0x00,0x40,0x08,
	0x02,0x00,0x80,0x08,0x06,0x00,0x80,0x08,
	0x0E,0x00,0x00,0x08,0x06,0x00,0x00,0x18,
	0x02,0x00,0x00,0x18,0x00,0x00,0x00,0x08,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


int i = 0;
void display_thread_work(void const * argument)
{
	while(1)
	{
		int data_line = data[i*4]<<24 | data[i*4+1]<<16 | data[i*4+2]<<8 | data[i*4+3];
		/* printf("turn all leds on: index(%d), 0x%x\r\n", i, data_line); */
		display_one_line(data_line);
		osDelay(10);
		if(++i == 32) {
			i = 0;
			osDelay(1000);
		}
		/* HAL_GPIO_WritePin(GPIO_PORT_POWER_LANCH, GPIO_PIN_POWER_LANCH, GPIO_PIN_SET); */
	}
}

osStatus display_init(void)
{
	osStatus ret = osOK;

	osThreadDef(displayTask, display_thread_work, osPriorityNormal, 0, 128);
	displayTaskHandle = osThreadCreate(osThread(displayTask), NULL);

	if(displayTaskHandle == NULL)
		ret = osErrorResource;

	return ret;
}

/* 1: led on */
/* 0: led off */
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
