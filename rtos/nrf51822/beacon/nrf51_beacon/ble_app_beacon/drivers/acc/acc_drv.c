#include "stdio.h"
#include "nrf51.h"
#include "pca20006.h"
#include "nrf_gpio.h"
#include "app_gpiote.h"
#include "app_timer.h"
#include "lis3dh.h"
#include "display.h"
#include "log.h"

typedef struct
{
    app_timer_id_t       timer_id;
    bool                 timer_running;
    bool                 initialized;
	bool				 acc_en;
}acc_context_t;

static acc_context_t m_acc_ct = {0};
static app_gpiote_user_id_t           m_gpiote_user_id;            /**< GPIOTE user id for buttons module. */
//static pin_transition_t               m_pin_transition;            /**< pin transaction direction. */

#if ENABLE_ACC_TIMER
uint32_t acc_timer_start(void)
{
    if (!m_acc_ct.timer_running) {
        uint32_t err_code;

        err_code = app_timer_start(m_acc_ct.timer_id, ACC_TIMER_PERIOD, NULL);
		APP_ERROR_CHECK(err_code);

        if (err_code != NRF_SUCCESS) {
			log_d("[ACC] acc timer start fail\n");
            return err_code;
        }
		else {
			log_d("[ACC] acc timer start sucess\n");
		}

        m_acc_ct.timer_running = true;
    }

    return NRF_SUCCESS;
}
#endif

#if ENABLE_ACC_TIMER
static void acc_work_timerout(void *p_context)
{
    int16_t acc_value[3];
	AxesRaw_t buff;
	LIS3DH_GetAccAxesRaw(&buff);
	uint8_t pin = nrf_gpio_pin_read(ACC_PIN_INT1);
	uint8_t val = 0;
	/* LIS3DH_GetInt1Src(&val); */
	/* printf("INT1 src:0x%x\n", val); */

    //x
    acc_value[0] = buff.AXIS_X;

    //y
    acc_value[1] = buff.AXIS_Y;

    //z
    acc_value[2] = buff.AXIS_Z;

	/* printf("acc value: X:%s%6d, Y:%s%6d, Z:%s%6d\n", acc_value[0]>0 ? " ":"-", acc_value[0], acc_value[1]>0 ? " ":"-", acc_value[1], acc_value[2]>0 ? " ":"-", acc_value[2]); */
	/* printf("X:%6d, Y:%6d, Z:%6d\n", acc_value[0], acc_value[1], acc_value[2]); */
	/* printf("X:%6d, Y:%6d, Z:%6d  0x%x\n", acc_value[0], acc_value[1], acc_value[2], val); */
	log_d("[ACC] X:%6d, Y:%6d, Z:%6d  0x%2x %s\n", acc_value[0], acc_value[1], acc_value[2], val, pin ? "H":"L");

}

static uint32_t acc_timer_init(void)
{
	uint32_t err_code;

	m_acc_ct.timer_running = false;
	err_code = app_timer_create(&m_acc_ct.timer_id, APP_TIMER_MODE_REPEATED, acc_work_timerout);
	APP_ERROR_CHECK(err_code);
	if(err_code != NRF_SUCCESS) {
		log_d("[ACC] create acc timer fail");
	}
	else {
		log_d("[ACC] create acc timer success");
	}

	return err_code;
}
#endif

/* failing edge will work */
/**@brief Function for handling the GPIOTE event.
 *
 * @details Saves the current status of the button pins, and starts a timer. If the timer is already
 *          running, it will be restarted.
 *
 * @param[in]  event_pins_low_to_high   Mask telling which pin(s) had a low to high transition.
 * @param[in]  event_pins_high_to_low   Mask telling which pin(s) had a high to low transition.
 */
static void acc_gpiote_event_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
    uint32_t err_code;
    STATIC_ASSERT(sizeof(void *) == sizeof(uint32_t));

/* sleep: low to high */
/* wakeup: high to low */
/* https://www.ecnmag.com/article/2013/04/one-accelerometer-interrupt-pin-both-wakeup-and-non-motion-detection */
	if ((event_pins_high_to_low & (1 << ACC_PIN_INT1)) != 0) {
		/* display_timer_stop(); */
	}
	if ((event_pins_low_to_high & (1 << ACC_PIN_INT1)) != 0)
	{
		/* display_timer_start(); */

		uint8_t val = 0;
		LIS3DH_GetInt1Src(&val);
#if 0
		if(val & 0x20)
			printf("0x%2x Z++++\n", val);
		if(val & 0x10)
			printf("0x%2x Z----\n", val);
#endif
#if ACC_INT_USE_Y
		if(val & 0x08) {
#if 0
			int16_t acc_value[3];
			AxesRaw_t buff;
			LIS3DH_GetAccAxesRaw(&buff);
			acc_value[1] = buff.AXIS_Y;
			if(acc_value[1] > 0) {
				display_change_direction(true);
				printf("0x%2x %6d Y++++\n", val, acc_value[1]);
			}
			else {
				display_change_direction(false);
				printf("0x%2x %6d Y----\n", val, acc_value[1]);
			}
#else
			display_change_direction(true);
			log_d("[ACC] 0x%2x Y++++\n", val);
#endif
		}
		if(val & 0x04) {
			display_change_direction(false);
			log_d("[ACC] 0x%2x Y----\n", val);
		}
#endif
#if ACC_INT_USE_X
		if(val & 0x02)
			printf("0x%2x X++++\n", val);
		if(val & 0x01)
			printf("0x%2x X----\n", val);
#endif
	}
}

static uint32_t acc_gpiote_init(void)
{
	uint32_t err_code;
	// Configure pins.
	uint32_t pins_transition_mask = 0;
	// Build GPIOTE user registration masks.
	pins_transition_mask |= (1 << ACC_PIN_INT1);
	pins_transition_mask |= (1 << ACC_PIN_INT2);
	// Register module as a GPIOTE user.
	err_code = app_gpiote_user_register(&m_gpiote_user_id,
										pins_transition_mask,
										pins_transition_mask,
										acc_gpiote_event_handler);
	APP_ERROR_CHECK(err_code);
	if (err_code != NRF_SUCCESS) {
		return err_code;
	}
	else {
		log_d("[ACC] %s success.\n", __func__);
	}

	return app_gpiote_user_enable(m_gpiote_user_id);
}

uint32_t acc_init(void)
{
	uint32_t err_code;
	m_acc_ct.acc_en = false;

	//config nrf51822 spi interface
	nrf_gpio_cfg_output(ACC_PIN_CSN);
	nrf_gpio_cfg_output(ACC_PIN_SCK);
	nrf_gpio_cfg_output(ACC_PIN_MOSI);
	nrf_gpio_cfg_input(ACC_PIN_INT1, NRF_GPIO_PIN_NOPULL);
	/* nrf_gpio_cfg_input(ACC_PIN_INT1, NRF_GPIO_PIN_PULLUP); */
	nrf_gpio_cfg_input(ACC_PIN_INT2, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(ACC_PIN_MISO, NRF_GPIO_PIN_NOPULL);

	acc_gpiote_init();

	/* init spi memory */
	SPI_Mems_Init();

	uint8_t val = 0;
	LIS3DH_GetWHO_AM_I(&val);
	log_d("[ACC] %s who am i:0x%x\n", __func__, val);
	if(val == 0x33) {
		m_acc_ct.acc_en = true;
	}
#if 1
	/* LIS3DH_WriteReg(LIS3DH_CTRL_REG1, 0x27); //10Hz */
	LIS3DH_WriteReg(LIS3DH_CTRL_REG1, 0x47); //50Hz
	uint8_t data = 0;
	LIS3DH_ReadReg(LIS3DH_CTRL_REG1, &data);
	log_d("[ACC] %s write reg CTRL_REG1:%s\n", __func__, data==0x47 ? "success":"fail");
	LIS3DH_WriteReg(LIS3DH_CTRL_REG2, 0x01);
	LIS3DH_WriteReg(LIS3DH_CTRL_REG3, 0x40);
	/* LIS3DH_WriteReg(LIS3DH_CTRL_REG4, 0x88); //2g */
	LIS3DH_WriteReg(LIS3DH_CTRL_REG4, 0x98); //4g
	LIS3DH_WriteReg(LIS3DH_CTRL_REG5, 0x00);

	/* LIS3DH_WriteReg(LIS3DH_INT1_THS, 0x08); */
	/* LIS3DH_WriteReg(LIS3DH_INT1_THS, 0x18); */
	LIS3DH_WriteReg(LIS3DH_INT1_THS, 400);
	LIS3DH_WriteReg(LIS3DH_INT1_DURATION, 0x2);
	/* LIS3DH_WriteReg(LIS3DH_INT1_CFG, 0x95); //all /and interrupt */
	/* LIS3DH_WriteReg(LIS3DH_INT1_CFG, 0x85); //no Z /and interrupt */
	/* LIS3DH_WriteReg(LIS3DH_INT1_CFG, 0x84); //Y only /and interrupt */
	/* LIS3DH_WriteReg(LIS3DH_INT1_CFG, 0x4c); //Y only /move ment interrupt */
	LIS3DH_WriteReg(LIS3DH_INT1_CFG, 0x7f); //all /move ment interrupt
#else
	LIS3DH_HPFAOI1Enable(MEMS_ENABLE);
	LIS3DH_SetInt1Pin(LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE);
	/* LIS3DH_FULLSCALE_2  2g, 16mg/LSb */
	/* LIS3DH_FULLSCALE_4  4g, 32mg/LSb */
	/* LIS3DH_FULLSCALE_8  8g, 62mg/LSb */
	/* LIS3DH_FULLSCALE_16 16g 186mg/LSb */
	LIS3DH_SetFullScale(LIS3DH_FULLSCALE_4);
	LIS3DH_SetODR(LIS3DH_ODR_100Hz);
	//set PowerMode
	LIS3DH_SetMode(LIS3DH_NORMAL);
	LIS3DH_Int1LatchEnable(1 << LIS3DH_LIR_INT1);
	status_t response = MEMS_SUCCESS;
	//set Interrupt Threshold = xxmg/LSb * num 32*8=256mg
	response = LIS3DH_SetInt1Threshold(32);
	/* Sets Interrupt 1 Duration t = num*(1/100Hz) */
	response &= LIS3DH_SetInt1Duration(4);

	//set Interrupt configuration (all enabled)
	response &= LIS3DH_SetIntConfiguration(
		LIS3DH_INT1_YHIE_ENABLE | LIS3DH_INT1_YLIE_ENABLE |
		LIS3DH_INT1_ZHIE_ENABLE | LIS3DH_INT1_ZLIE_ENABLE |
		LIS3DH_INT1_XHIE_ENABLE | LIS3DH_INT1_XLIE_ENABLE
									   );
	if(response==MEMS_SUCCESS){
		printf("SET_INT_CONF_OK \n");
	}
	//set Interrupt Mode
	/* response = LIS3DH_SetIntMode(LIS3DH_INT_MODE_6D_POSITION); */
	response = LIS3DH_SetIntMode(LIS3DH_INT_MODE_6D_MOVEMENT);
	if(response==MEMS_SUCCESS){
		printf("SET_INT_MODE  \n");
	}
#endif

#if ENABLE_ACC_TIMER
	err_code = acc_timer_init();
	if(err_code == NRF_SUCCESS) {
		 err_code = acc_timer_start();
	}
#endif

	return err_code;
}

