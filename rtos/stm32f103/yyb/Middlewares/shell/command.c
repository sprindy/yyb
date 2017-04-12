#include "stm32f1xx_hal.h"
#include "freeRTOS.h"
#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "gpio.h"
#include "BMI160_ACC_GYRO_Driver_HL.h"


static osThreadId commandTaskHandle;
static osMessageQId CommdMsgQueueHandle;
/* static osSemaphoreId cmd_semaphore; */

extern SPI_HandleTypeDef hspi1;

#define CMD_BUF_LEN 130
#define PARAMETER_SIZE 100
#define MAX_ARGS      88
typedef struct
{
    char cmd_buf[CMD_BUF_LEN];
    char cmd_length;
} cmd_struct;


const unsigned char whiteSpace[] = {' ', '\t', '=', '\r', '\n'};
int cmd_help(int argc, char * argv[], _command_source source);

__weak int get_task_state(int argc, char *argv[], _command_source source)
{
    int i;

    for (i = 0; i < argc; i++)
    {
        shell_debug("argv[%d]:%s\r\n",i,(char const*)argv[i]);
    }

    return 0;
}
__weak int usb_cdc_printf(const char *fmt, ...)
{
    return 0;
}

unsigned long my_strtoul (char *data)
{
	int base = 10;
	if (data[0] == '0' && (data[1] == 'x' || data[1] == 'X'))
		base = 16;

	return strtoul(data, NULL, base);
}

#if SENSOR_LATENCY_TEST
extern uint32_t latency_data[SENSOR_LATENCY_TEST_COUNT][SENSOR_LATENCY_TEST_POINT];
extern volatile uint32_t latency_index;
extern uint32_t latency_lock;
extern void watchdog_refresh();

#endif
static int performance_test(int argc, char * argv[], _command_source source)
{
#if SENSOR_LATENCY_TEST
    uint32_t i = 0;
    uint32_t index;
    uint32_t sum[3] = {0};
    uint32_t sys_clk;
    sys_clk = HAL_RCC_GetHCLKFreq()/1000000;
    latency_lock = 1;
    osDelay(10); // wait for lock effect

    index= latency_index;
    shell_info("latency_index = %d\r\n", index);
    for(i=0; i<SENSOR_LATENCY_TEST_COUNT; i++){
        watchdog_refresh();
        shell_info("<%2d> Notify:%d \t read data:%d \t IMU:%d \r\n",i,
            (latency_data[i][1]-latency_data[i][0]),
            (latency_data[i][2]-latency_data[i][1]),
            (latency_data[i][3]-latency_data[i][2]));
        if(i == index) continue;
        sum[0] += (latency_data[i][1]-latency_data[i][0]);
        sum[1] += (latency_data[i][2]-latency_data[i][1]);
        sum[2] += (latency_data[i][3]-latency_data[i][2]);
    }
    latency_lock = 0;
    shell_info("<everage> Notify:%.2fus \t read data:%.2fus \tIMU:%.2fus\r\n\n",
        (float)(sum[0]/(float)((SENSOR_LATENCY_TEST_COUNT-1)*sys_clk)),
        (float)(sum[1]/(float)((SENSOR_LATENCY_TEST_COUNT-1)*sys_clk)),
        (float)(sum[2]/(float)((SENSOR_LATENCY_TEST_COUNT-1)*sys_clk)));


#endif
    return 0;
}

int gpio_test_cmd(int argc, char * argv[], _command_source source)
{
    int ret = 0;
    if((argc < 2) ||(!strcmp(argv[1], "?"))||(!strcmp(argv[1], "help"))){
        shell_info("Useage:\n\tgpio dump --> dump all of gpio status\n");
        shell_info("\n\tgpio set PXn=x\n\teg:\n\tgpio set PA1=1\n");
        return -1;
    }
    if(!strcmp(argv[1], "dump")){
        gpio_dump_status();
    }
    else if(!strcmp(argv[1], "set")){
        uint8_t PinState = 0;
        uint8_t gpio_index = 0;
        uint16_t GPIO_PIN_x = 0;
        GPIO_TypeDef *GPIOx;
        if(argc < 4){
            shell_err("Useage:\n\tgpio set PXn=x\neg\t:\n\tgpio set PA1=1\n");
            return -2;
        }
        PinState = atoi(argv[3]);
        gpio_index = atoi(argv[2]+2);
        GPIO_PIN_x = (1 << gpio_index);
        if(!strncasecmp("PA",argv[2],2)){
            GPIOx = GPIOA;
        }
        else if(!strncasecmp("PB",argv[2],2)){
            GPIOx = GPIOB;
        }
        else if(!strncasecmp("PC",argv[2],2)){
            GPIOx = GPIOC;
        }
        else if(!strncasecmp("PD",argv[2],2)){
            GPIOx = GPIOD;
        }
        HAL_GPIO_WritePin(GPIOx, GPIO_PIN_x, (GPIO_PinState)PinState);
    }
    return ret;
}

int spi_test_cmd(int argc, char * argv[], _command_source source)
{
    int ret = 0;
    if((argc < 2) ||(!strcmp(argv[1], "?"))||(!strcmp(argv[1], "help"))){
        shell_info("Useage:\n\t");
        shell_info("\n\tspi read [addr][reg]\n\teg:\n\tspi read 0x68 0x00\n");
        shell_info("\n\tspi write [addr][reg][value]\n\teg:\n\tspi write 0x68 0x02 0x00\n");
        return -1;
    }
    if(!strcmp(argv[1], "read")){
        if(argc < 4){
        	shell_err("\n\tspi read [addr][reg]\n\teg:\n\tspi read 0x68 0x00\n");
            return -2;
        }
		else{
#define MASK_DATA1	0xFF
#define MASK_DATA2	0x80
#define MASK_DATA3	0x7F
			uint8_t array[8] = {MASK_DATA1};
			uint8_t array_pTx[8] = {MASK_DATA1};
			/* uint16_t dev_addr = (uint16_t)my_strtoul(argv[2]); */
			uint16_t reg_addr = (uint16_t)my_strtoul(argv[3]);

			HAL_StatusTypeDef status = HAL_OK;

			array_pTx[0] = reg_addr|MASK_DATA2;

			BMI160_CS_LOW();
			status = HAL_SPI_TransmitReceive(&hspi1, array_pTx,array, 1, 0x100);
			BMI160_CS_HIGH();
			/* Check the communication status */
			if(status != HAL_OK) {
			/* Execute user timeout callback */
			/* SPI1_Error(); */
        		shell_err("\n\tspi read fail\n");
				return -2;
			}
			else
        		shell_info("\n\tspi read reg:0x%0x, value:0x%0x\n", reg_addr, array[0]);

		}
    }
    return ret;
}

const MONITOR_COMMAND commandTable[] =
{
    {"task",    get_task_state},
    /* {"pm",      PowerDoCommand}, */
    {"sensor",  performance_test},
    {"gpio",    gpio_test_cmd},
    {"spi",     spi_test_cmd},
    {"?",       cmd_help}, //This must be the last command
};
const unsigned long ulNumberOfCommands = (sizeof(commandTable) / sizeof(commandTable[0]));
int cmd_help(int argc, char * argv[], _command_source source)
{
    uint8_t i;

    shell_info("\tAll Command list\r\n");

    for (i=0; i<(ulNumberOfCommands-1); i++){
        shell_info("\t%s\r\n", commandTable[i].command);
    }
    shell_info("________ ^_^ ________\r\n");
    return 0;
}

int isaspace(unsigned char c)
{
    int     i;

    for (i = 0; i < sizeof(whiteSpace); i++)
    {
        if (c == whiteSpace[i])
        {
            return 1;
        }
    }
    return 0;
}

int ParseCommandAndData(unsigned char *pLineBuf, int *argn, unsigned char *argv[], int MaxArgs)
{
    int             n;
    int             i;
    unsigned char   quoteChar;

    n = 0;
    while (n < MaxArgs)
    {
        while (isaspace(*pLineBuf))
        {
            pLineBuf++;
        }

        if (*pLineBuf == '"' || *pLineBuf == '\'')
        {
            quoteChar = *pLineBuf;
            *pLineBuf = (unsigned char)1;
            argv[n++] = pLineBuf++;
            while (*pLineBuf && (*pLineBuf != quoteChar))
            {
                pLineBuf++;
            }
            if (*pLineBuf)
            {
                *pLineBuf = 0;
                pLineBuf++;
            }
            else
            {
                n = 0;                     // Error, no matching quote char
                break;
            }
        }
        else if (*pLineBuf)
        {
            argv[n++] = pLineBuf;
            //
            // Go to the next whiteSpace
            //
            while (*pLineBuf && !isaspace(*pLineBuf))
            {
                pLineBuf++;
            }
            if (*pLineBuf)
            {
                *pLineBuf = 0;
                pLineBuf++;
            }
            else break;
        }
        else break;
    }

    if ((n >= 1) && *argv[0] == '?' && *(argv[0] + 1))
    {
        n++;
        if (n <= MaxArgs)
        {
            for (i = 1; i < n; i++)
            {
                argv[i] = argv[i - 1];
            }
            (argv[1])++;
            argv[0] = (unsigned char*)"?";
        }
    }
    if (n > MaxArgs)
    {
        shell_err("Too many arguments\n");
        n = 0;
    }
    *argn = n;
    return n;
}


int DoCommand(int argn,unsigned char *argv[], _command_source source)
{
    unsigned int uiCount;

    //
    // The first argument should be the command
    //
    for (uiCount = 0; uiCount < ulNumberOfCommands; uiCount++)
    {
        char resut_cmp = (strcmp((char const*)argv[0], (char const*)commandTable[uiCount].command)== 0);
        if ( resut_cmp )
        {
            return(*(commandTable[uiCount].pFunc))(argn, (char **)argv, source);

        }
    }

     shell_err("Command error !!!\n");
#if 0
	 if (source == CMD_SOURCE_USBCDC) {
	 	usb_app_transmit_data("Command error !!!\r\n",strlen("Command error !!!\r\n"));
	 } else if (source == CMD_SOURCE_BLE) {
		 ble_printf("Command error !!!\r\n");
	 }
#endif

    return -1;
}

void StartCommandTask(void const * argument)
{
    for(;;)
    {
        int argn = 0;
        unsigned char *argv[MAX_ARGS] = {0};
        cmd_struct commdBuf;
        if(!CommdMsgQueueHandle) continue;
        xQueueReceive( CommdMsgQueueHandle, &commdBuf,  portMAX_DELAY );
        if (commdBuf.cmd_length > CMD_BUF_LEN) { // in IAR is unsigned char
            shell_err("[Command too long]\n");
            continue;
        }

        memset(commdBuf.cmd_buf+commdBuf.cmd_length-1,'\0',CMD_BUF_LEN-commdBuf.cmd_length);
        /* if(!cmd_semaphore) continue; */
        /* osSemaphoreWait(cmd_semaphore, osWaitForever); */

        if(commdBuf.cmd_length!= 0)
        {
            if(0 != ParseCommandAndData((unsigned char *)commdBuf.cmd_buf, &argn, argv, (sizeof(argv) / sizeof(argv[0]))))
            {
				shell_err("[Unkown source]\n");
#if 0
				switch (commdBuf.cmd_source) {
					case CMD_SOURCE_UART:
					case CMD_SOURCE_USBCDC:
					case CMD_SOURCE_BLE:
						DoCommand(argn, argv, commdBuf.cmd_source);
						break;
					default:
						shell_err("[Unkown source]\n");
						break;
				}
#endif
            }
            else
            {
                shell_err("CommandManger_Thread %s \r\n", "[No Processor for Command]");
            }
        }
        else
        {
            shell_err("[No Command]\n");
        }
        /* osSemaphoreRelease(cmd_semaphore); */

    }

}

osMessageQId get_queue_commd(void)
{
    return CommdMsgQueueHandle;
}

osThreadId get_commd_task_handle(void)
{
    return commandTaskHandle;
}


void cmd_init(void)
{
    osThreadDef(commandTask, StartCommandTask, osPriorityAboveNormal, 0,
                configMINIMAL_STACK_SIZE*4);
    commandTaskHandle = osThreadCreate(osThread(commandTask), NULL);

    osMessageQDef(CommdMsgQueue, 1, cmd_struct);
    CommdMsgQueueHandle = osMessageCreate(osMessageQ(CommdMsgQueue), NULL);

    /* osSemaphoreDef(cmd_semaphore); */
    /* cmd_semaphore = osSemaphoreCreate(osSemaphore(cmd_semaphore),1); */

    shell_info("cmd_init success!\n");

}

