#include "stdio.h"
#include "pca20006.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "lis2dh12.h"

typedef struct
{
    app_timer_id_t       timer_id;
    bool                 timer_running;
    bool                 initialized;
	bool				 acc_en;
}acc_context_t;

static acc_context_t m_acc_ct = {0};

uint32_t acc_timer_start(void)
{
    if (!m_acc_ct.timer_running) {
        uint32_t err_code;

        err_code = app_timer_start(m_acc_ct.timer_id, ACC_TIMER_PERIOD, NULL);
        if (err_code != NRF_SUCCESS) {
			printf("acc timer start fail\n");
            return err_code;
        }
		else
			printf("acc timer start sucess\n");

        m_acc_ct.timer_running = true;
    }

    return NRF_SUCCESS;
}

uint32_t acc_get_chan_data(uint16_t *p_buf)
{
	uint32_t err_code = NRF_SUCCESS;

	return err_code;
}

static void acc_work_timerout(void *p_context)
{
    /* uint8_t data[6]; */
    int16_t acc_value[3];
    /* char buff[20]; */
    //x
    acc_value[0] = LIS2DH12_Get_Chan_Data(LIS2DH12_ACC_CHAN_X);
    /* data[0] = (acc_value >> 8) & 0xFF; */
    /* data[1] = acc_value & 0xFF; */

    //y
    acc_value[1] = LIS2DH12_Get_Chan_Data(LIS2DH12_ACC_CHAN_Y);
    /* data[2] = (acc_value >> 8) & 0xFF; */
    /* data[3] = acc_value & 0xFF; */

    //z
    acc_value[2] = LIS2DH12_Get_Chan_Data(LIS2DH12_ACC_CHAN_Z);
    /* data[4] = (acc_value >> 8) & 0xFF; */
    /* data[5] = acc_value & 0xFF; */

	printf("acc value: X:0x%x, Y:0x%x, Z:0x%x\n", acc_value[0], acc_value[1], acc_value[2]);
}

static uint32_t acc_work_init(void)
{
	uint32_t err_code;

	m_acc_ct.timer_running = false;
	err_code = app_timer_create(&m_acc_ct.timer_id, APP_TIMER_MODE_REPEATED, acc_work_timerout);
	if(err_code != NRF_SUCCESS) {
		printf("create acc timer fail");
	}

	return err_code;
}

uint32_t acc_init(void)
{
	uint32_t err_code;

	LIS2DH12_InitStruct lis =
	  {
		 .MISO = ACC_PIN_MISO,
		 .MOSI = ACC_PIN_MOSI,
		 .CSN = ACC_PIN_CSN,
		 .SCK = ACC_PIN_SCK,
		 .INT1 = ACC_PIN_INT1,
		 .INT2 = ACC_PIN_INT2,
		 .FREQUENCY = LIS2DH12_FREQUENCY,
	  };

	LIS2DH12_Config(&lis);
	LIS2DH12_Set_Data_Rate(LIS2DH12_DATA_RAT_25HZ);

	m_acc_ct.acc_en = false;

	err_code = acc_work_init();

	return err_code;
}

