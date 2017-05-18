#include "stdio.h"
#include "pca20006.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "lis3dh.h"

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
#if ENABLE_ACC_TIMER
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
#endif

    return NRF_SUCCESS;
}

uint32_t acc_get_chan_data(uint16_t *p_buf)
{
	uint32_t err_code = NRF_SUCCESS;

	return err_code;
}

static void acc_work_timerout(void *p_context)
{
#if ENABLE_ACC_TIMER
    int16_t acc_value[3];
	AxesRaw_t buff;
	LIS3DH_GetAccAxesRaw(&buff);

    //x
    acc_value[0] = buff.AXIS_X;

    //y
    acc_value[1] = buff.AXIS_Y;

    //z
    acc_value[2] = buff.AXIS_Z;

	printf("acc value: X:%s%d, Y:%s%d, Z:%s%d\n", acc_value[0]>0 ? " ":"-", acc_value[0], acc_value[2]>0 ? " ":"-", acc_value[1], acc_value[2]>0 ? " ":"-", acc_value[2]);

#endif
}

static uint32_t acc_work_init(void)
{
	uint32_t err_code;

#if ENABLE_ACC_TIMER
	m_acc_ct.timer_running = false;
	err_code = app_timer_create(&m_acc_ct.timer_id, APP_TIMER_MODE_REPEATED, acc_work_timerout);
	if(err_code != NRF_SUCCESS) {
		printf("create acc timer fail");
	}

#endif
	return err_code;
}

uint32_t acc_init(void)
{
	uint32_t err_code;
	m_acc_ct.acc_en = false;

	//config nrf51822 spi interface
	nrf_gpio_cfg_output(ACC_PIN_CSN);
	nrf_gpio_cfg_output(ACC_PIN_SCK);
	nrf_gpio_cfg_output(ACC_PIN_MOSI);
	nrf_gpio_cfg_output(ACC_PIN_INT1);
	nrf_gpio_cfg_output(ACC_PIN_INT2);
	nrf_gpio_cfg_input(ACC_PIN_MISO, NRF_GPIO_PIN_NOPULL);

	/* init spi memory */
	SPI_Mems_Init();

	LIS3DH_SetFullScale(LIS3DH_FULLSCALE_2);
	LIS3DH_SetODR(LIS3DH_ODR_50Hz);
	//set PowerMode
	LIS3DH_SetMode(LIS3DH_NORMAL);
	uint8_t val = 0;
	LIS3DH_GetWHO_AM_I(&val);
	printf("%s who am i:0x%x\n", __func__, val);
	if(val == 0x33) {
		m_acc_ct.acc_en = true;
	}

	err_code = acc_work_init();

	return err_code;
}

