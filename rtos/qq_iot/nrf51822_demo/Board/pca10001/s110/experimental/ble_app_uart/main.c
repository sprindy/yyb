/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 *                                  Tencent  &    Honestar
 *
 *                                 http://www.tencent.com
 *                                 http://www.honestar.com
 *--------------File Info-------------------------------------------------------------------------------
 * File Name:           main.c
 * Last modified Date:  2015/01/23
 * Last Version:        V1.0
 * Description:         QQ Interface Communication
 *
 *------------------------------------------------------------------------------------------------------
 * Created By:          Alan Lan
 * Created date:        2015/01/23
 * Version:             V1.0
 * Descriptions:        Initial Version
 *                      alan.lan@honestar.com
 * 
 */

/** @file
 *
 * @defgroup ble_sdk_uart_over_ble_main main.c
 * @{
 * @ingroup  ble_sdk_app_nus_eval
 * @brief    UART over BLE application main file.
 *
 * This file contains the source code for a sample application that uses the Nordic UART service.
 * This application uses the @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf51_bitfields.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "app_button.h"
#include "app_gpiote.h"
#include "ble_nus.h"
#include "simple_uart.h"
#include "boards.h"
#include "ble_error_log.h"
#include "ble_debug_assert_handler.h"
#include "nrf_delay.h"

#include "pstorage.h"
#include "pstorage_platform.h"

#include "fh_impl.h"
#include "ff_inc.h"
#include "ff_syshdr.h"
#include "fd_engine.h"
#include "oc_dump.h"

#include "ble_qqs.h"

#define mBUTTON_1     17
#define mBUTTON_2     18
#define mBUTTON_3     19
#define mBUTTON_4     20

#ifdef QCHENG
#define ADV_CTRL_PIN        23
#define CS_EN               11
//UART RX --- P0.14
#endif

#define WAKEUP_BUTTON_PIN              mBUTTON_1                                    /**< Button used to wake up the application. */
#define ADVERTISING_LED_PIN_NO         mLED_1                                       /**< LED to indicate advertising state. */
#define CONNECTED_LED_PIN_NO           mLED_2                                       /**< LED to indicate connected state. */

#define DEVICE_NAME                    "QCheng"                                     /**< Name of device. Will be included in the advertising data. */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            8                                           /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         10                                          /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               160                                          /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               600                                          /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< slave latency. */
#define CONN_SUP_TIMEOUT                400                                         /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (20 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_GPIOTE_MAX_USERS            1
#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)    /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SEC_PARAM_TIMEOUT               30                                          /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define TX_POWER_LEVEL                    (-8)                                              /**< TX Power Level value. This will be set both in the TX Power service, in the advertising data, and also used to set the radio transmit power. */

#define START_STRING                    "System Start...\r\n"                       /**< The string that will be sent over the UART when the application starts. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static ble_gap_sec_params_t             m_sec_params;                               /**< Security requirements for this application. */
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
//static 
ble_qqs_t                        m_qqs;                                      /**< Structure to identify the Nordic QQ Service. */

static uint8_t s_fd_zone[256];
struct fd_engine_s *g_fd_engine;

static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
static uint8_t index = 0;
static void qqs_data_upload(void);
static void nus_data_upload(void);
static int g_seq = 0;
static void get_mac_addr(uint8_t *p_mac_addr)
{
  uint32_t error_code;
  ble_gap_addr_t mac_addr_t;
		
  error_code = sd_ble_gap_address_get(&mac_addr_t);
	APP_ERROR_CHECK(error_code);

  memcpy(p_mac_addr, mac_addr_t.addr, 6);  
}

void load_mac_str(char *pMacAddrStr)
{
  uint32_t error_code;
  ble_gap_addr_t mac_addr_t;
  uint8_t macaddr[6];
  
  error_code = sd_ble_gap_address_get(&mac_addr_t);
	APP_ERROR_CHECK(error_code);
  memcpy(macaddr, mac_addr_t.addr, 6);
  sprintf(pMacAddrStr, "%02X%02X%02X%02X%02X%02X", macaddr[5], macaddr[4], macaddr[3], macaddr[2], macaddr[1], macaddr[0]);
}

static void ft_text_msg(int prop_id){
  ff_property_t ffprop;
  int8_t ret;
  memset(&ffprop,0,sizeof(ffprop));
  if (g_fd_engine != NULL) {
    char json[] = "{\"text\":\"msg test\",\"value\":1}";

    ffprop.seq_no = g_seq++;
    ffprop.len = strlen(json);
    ffprop.prop_id = prop_id;
    ffprop.value = (uint8_t*)json;
    
    ret = fd_set_property(g_fd_engine, &ffprop);
    if (ret != 0) {
     printf("fd set prop error\r\n");
   }
 }
}

uint8_t s_hhb_default_buffer[8] = {0x10, 0x01, 0x00, 0x02, 0xD2, 0x00};
uint8_t s_hhb_buffer[8] = {0x10, 0x01, 0x00, 0x02, 0xD2, 0x00};

static void  ft_hhb_reset(void)
{
  ff_property_t ffprop;
  memset(&ffprop,0,sizeof(ffprop));
  int8_t ret;
  
  if (g_fd_engine != NULL) {
   ffprop.seq_no = 0x01;
   ffprop.len = 8;
   ffprop.prop_id = 100000096;
   ffprop.value = s_hhb_default_buffer;
   memcpy(s_hhb_buffer,s_hhb_default_buffer,8);
   
   if (ffprop.len == 0) return;
   
   ret = fd_set_property(g_fd_engine, &ffprop);
   if (ret != 0) {
     printf("fd set prop error\r\n");
   }
 }
}

static void  ft_hhb_inc(void)
{
  ff_property_t ffprop;
  memset(&ffprop,0,sizeof(ffprop));
  int8_t ret;

  if (g_fd_engine != NULL) {
    s_hhb_buffer[1]=s_hhb_buffer[1]+2;
    if(s_hhb_buffer[1]>0x38)
    {
      s_hhb_buffer[0]=s_hhb_buffer[0]+2;
    }

    s_hhb_buffer[3]=s_hhb_buffer[3]+9;
    if(s_hhb_buffer[3]>0xff)
    {
      s_hhb_buffer[2]=s_hhb_buffer[2]+1;
    }

    s_hhb_buffer[5]=s_hhb_buffer[5]+9;
    if(s_hhb_buffer[5]>0xff)
    {
      s_hhb_buffer[4]=s_hhb_buffer[4]+1;
    }
    ffprop.seq_no = 0x01;
    ffprop.len = 8;
    ffprop.prop_id = 100000096;
    ffprop.value = s_hhb_buffer;

    if (ffprop.len == 0) return;

    ret = fd_set_property(g_fd_engine, &ffprop);
    if (ret != 0) {
      printf("fd set prop error\r\n");
    }
  }
}


/**@brief     Error handler function, which is called when an error has occurred.
 *
 * @warning   This handler is an example only and does not fit a final product. You need to analyze
 *            how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}


/**@brief       Assert macro callback function.
 *
 * @details     This function will be called in case of an assert in the SoftDevice.
 *
 * @warning     This handler is an example only and does not fit a final product. You need to
 *              analyze how your product is supposed to react in case of Assert.
 * @warning     On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief   Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    nrf_gpio_cfg_output(ADVERTISING_LED_PIN_NO);
    nrf_gpio_cfg_output(CONNECTED_LED_PIN_NO);
    nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
  
    nrf_gpio_cfg_output(mLED_4);
    nrf_gpio_pin_set(mLED_4);
    nrf_gpio_cfg_output(mLED_3);
    nrf_gpio_pin_set(mLED_3);
}

/**@brief   Function for the LEDs contrl.
 *
 * @details contrl all LEDs used by this application.
 * 
 *          lednum: mLED_1/2/3/4
 *          ctrl: 0--led off; 1 -- led on; 2 -- led blink
 */
void leds_ctrl(uint8_t lednum, uint8_t ctrl)
{
  switch(ctrl)
  {
    case 0:
      nrf_gpio_pin_set(lednum);
      break;
    
    case 1:
      nrf_gpio_pin_clear(lednum);
      break;
    
    case 2:
      nrf_gpio_pin_clear(lednum);
      nrf_delay_ms(200);
      nrf_gpio_pin_set(lednum);
      break;
    
    default:
      break;
  }
  
}

/**@brief   Function for Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);
}


/**@brief   Function for the GAP initialization.
 *
 * @details This function will setup all the necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    
	char mac_str[13];
	load_mac_str(mac_str);
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) mac_str+6,
                                          6);
	/*
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
	*/
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
		
    err_code = sd_ble_gap_tx_power_set(TX_POWER_LEVEL);
    APP_ERROR_CHECK(err_code);
}


/**@brief   Function for the Advertising functionality initialization.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint8_t	 															m_addl_adv_manuf_data[BLE_GAP_ADDR_LEN + 2];
    int8_t        tx_power_level = TX_POWER_LEVEL;

    m_addl_adv_manuf_data[0] = 0x9A;
    m_addl_adv_manuf_data[1] = 0x3B;
    get_mac_addr(&m_addl_adv_manuf_data[2]);
    
    uint32_t      err_code;
    ble_advdata_t advdata;
		/**< LE General Discoverable Mode, BR/EDR not supported. */
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    ble_uuid_t adv_uuids[] =
    {
        {BLE_UUID_TX_POWER_SERVICE,        BLE_UUID_TYPE_BLE}, 
        {BLE_UUID_IMMEDIATE_ALERT_SERVICE, BLE_UUID_TYPE_BLE}, 
        {BLE_UUID_QQS_SERVICE, BLE_UUID_TYPE_BLE},
    };

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type               = BLE_ADVDATA_NO_NAME;
    advdata.include_appearance      = false;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
    advdata.p_tx_power_level        = &tx_power_level;
    advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;

		ble_advdata_manuf_data_t        manuf_data;
    manuf_data.company_identifier = COMPANY_IDENTIFIER;
    manuf_data.data.size          = sizeof(m_addl_adv_manuf_data);
    manuf_data.data.p_data        = m_addl_adv_manuf_data;
    advdata.p_manuf_specific_data = &manuf_data;
		
    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);
}

#if 0
static void core_gpregret_set(uint32_t v)
{
	sd_power_gpregret_set(v);
	sd_power_gpregret_clr(~v);
}
#endif

/**@brief    Function for handling the data from the Nordic UART Service.
 *
 * @details  This function will process the data received from the Nordic UART BLE Service and send
 *           it to the UART module.
 */
/**@snippet [Handling the data received over BLE] */
void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
    for (int i = 0; i < length; i++)
    {
        simple_uart_put(p_data[i]);
    }
    simple_uart_put('\n');
    
#if 0   
    uint8_t err_code;
    
    err_code = ble_nus_send_string(&m_nus, p_data, length);
    if (err_code != NRF_ERROR_INVALID_STATE)
    {
       APP_ERROR_CHECK(err_code);
    }
    
    if ((p_data[0] == 0xCD) && (p_data[1] == 0x05) && (p_data[2] == 0x00) && (p_data[3] == 0xDD)) {
      //NRF_POWER->GPREGRET = 0xDD;
      core_gpregret_set(0xDD);
	    NVIC_SystemReset();
    }
#endif
}
/**@snippet [Handling the data received over BLE] */

/**@brief    Function for handling the data from the Nordic QQ Service.
 *
 * @details  This function will process the data received from the Nordic QQ BLE Service and send
 *           it to the UART module.
 */
/**@snippet [Handling the data received over BLE] */
void qqs_data_handler(ble_qqs_t * p_qqs, uint8_t * p_data, uint16_t length)
{

    //when Device received data from BLE rx charactic
    //int8_t fd_received(struct fd_engine_s* engine, uint8_t* datum,uint16_t length);
  int8_t err_code;
	PRINTF("fd_received (%d)\r\n", length);
	DUMP(p_data, length);
  err_code = fd_received(g_fd_engine, p_data, length);
  //APP_ERROR_CHECK(err_code);

}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t         err_code;
    ble_nus_init_t   nus_init;
    ble_qqs_init_t   qqs_init;
  
    memset(&nus_init, 0, sizeof(nus_init));
    nus_init.data_handler = nus_data_handler;
      
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
  
    memset(&qqs_init, 0, sizeof(qqs_init));
    qqs_init.data_handler = qqs_data_handler;
  
    err_code = ble_qqs_init(&m_qqs, &qqs_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing security parameters.
 */
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;  
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}


/**@brief       Function for handling an event from the Connection Parameters Module.
 *
 * @details     This function will be called for all events in the Connection Parameters Module
 *              which are passed to the application.
 *
 * @note        All this function does is to disconnect. This could have been done by simply setting
 *              the disconnect_on_fail config parameter, but instead we use the event handler
 *              mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief       Function for handling errors from the Connection Parameters module.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;
    
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t             err_code;
    ble_gap_adv_params_t adv_params;
    
    // Start advertising
    memset(&adv_params, 0, sizeof(adv_params));
    
    adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr = NULL;
    adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    adv_params.interval    = APP_ADV_INTERVAL;
    adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = sd_ble_gap_adv_start(&adv_params);
    APP_ERROR_CHECK(err_code);

    nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);
}


/**@brief       Function for the Application's S110 SoftDevice event handler.
 *
 * @param[in]   p_ble_evt   S110 SoftDevice event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t                         err_code;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;
    int                              ret;
  
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
            nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            ret = fd_connect(g_fd_engine);
            if (ret == 0) {
               printf("fd connect ok\r\n");
            }
            
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;

            ret = fd_disconnet(g_fd_engine);
            if (ret == 0) {
               printf("fd disconnect ok\r\n");
            }
            
            advertising_start();

            break;
            
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
                                                   BLE_GAP_SEC_STATUS_SUCCESS, 
                                                   &m_sec_params);
            APP_ERROR_CHECK(err_code);
            break;
            
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
            break;
            
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            p_enc_info = &m_auth_status.periph_keys.enc_info;
            if (p_enc_info->div == p_ble_evt->evt.gap_evt.params.sec_info_request.div)
            {
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, p_enc_info, NULL);
                APP_ERROR_CHECK(err_code);
            }
            else
            {
                // No keys found for this device
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL);
                APP_ERROR_CHECK(err_code);
            }
            break;
#if 0
        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            { 
                nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);

                // Configure buttons with sense level low as wakeup source.
                nrf_gpio_cfg_sense_input(WAKEUP_BUTTON_PIN,
                                         BUTTON_PULL,
                                         NRF_GPIO_PIN_SENSE_LOW);
                
                // Go to system-off mode (this function will not return; wakeup will cause a reset)
                err_code = sd_power_system_off();    
                APP_ERROR_CHECK(err_code);
            }
            break;
#else
        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            { 
              if (m_conn_handle == BLE_CONN_HANDLE_INVALID) {
                advertising_start();
              }
            }
            break;
#endif

        default:
            // No implementation needed.
            break;
    }
}


/**@brief       Function for dispatching a S110 SoftDevice event to all modules with a S110
 *              SoftDevice event handler.
 *
 * @details     This function is called from the S110 SoftDevice event interrupt handler after a
 *              S110 SoftDevice event has been received.
 *
 * @param[in]   p_ble_evt   S110 SoftDevice event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
    ble_qqs_on_ble_evt(&m_qqs, p_ble_evt);
    on_ble_evt(p_ble_evt);
}

/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
}

/**@brief   Function for the S110 SoftDevice initialization.
 *
 * @details This function initializes the S110 SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    // Initialize SoftDevice.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, false);
    //SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, false);
    
    // Subscribe for BLE events.
    uint32_t err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
  
    // Register with the SoftDevice handler module for System events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling button events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    switch (pin_no)
    {
        case WAKEUP_BUTTON_PIN:
				{
          if (button_action == APP_BUTTON_RELEASE)
          {
            printf("wake button ...[qqs_data_upload]\r\n");
            qqs_data_upload();
          }
          else
          {
            index = 0;
          }
				}
					break;
        
#ifdef QCHENG        
        case ADV_CTRL_PIN:
        {
          if (button_action == APP_BUTTON_RELEASE)
          {
            printf("adv btn release...\r\n");
          }
          else
          {
            printf("adv btn push...\r\n");
          }
        }
          break;
        
        case CS_EN:
        {
          if (button_action == APP_BUTTON_RELEASE)
          {
            printf("bt enable...\r\n");
            //qqs_data_upload();
            nus_data_upload();
          }
          else
          {
            printf("bt disable...\r\n");
            index = 0;
          }
        }
          break;
#else 
        case mBUTTON_2:
        {
          if (button_action == APP_BUTTON_RELEASE)
          {
            printf("btn2 ...[ft_hhb_reset]\r\n");
						ft_hhb_reset();
          }
        }
          break;
        
        case mBUTTON_3:
        {
          if (button_action == APP_BUTTON_RELEASE)
          {
            printf("btn3 ...[ft_hhb_inc]\r\n");						
						ft_hhb_inc();
          }
        }
          break;
        
        case mBUTTON_4:
        {
          if (button_action == APP_BUTTON_RELEASE)
          {
						printf("btn4 ...[ft_text_msg]\r\n");
						if (g_seq %2 ) {
							ft_text_msg(100000095); // 预制消息
						} else {
							ft_text_msg(10000); 			// 动态消息
						}
          }
        }
          break;
#endif        
        default:
            APP_ERROR_HANDLER(pin_no);
            break;
    }
}


/**@brief  Function for configuring the buttons.
 */
static void buttons_init(void)
{
  static app_button_cfg_t buttons[] =
    {
		  {WAKEUP_BUTTON_PIN, false, BUTTON_PULL, button_event_handler},
#ifdef QCHENG      
      {ADV_CTRL_PIN, false, BUTTON_PULL, button_event_handler},
      {CS_EN, false, BUTTON_PULL, button_event_handler},
#else
      {mBUTTON_1, false, BUTTON_PULL, button_event_handler},
      {mBUTTON_2, false, BUTTON_PULL, button_event_handler},
      {mBUTTON_3, false, BUTTON_PULL, button_event_handler},
      {mBUTTON_4, false, BUTTON_PULL, button_event_handler},
#endif
    };
    
  APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
 
  APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, false);
   
  app_button_enable();
}


/**@brief  Function for placing the application in low power state while waiting for events.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


/**@brief  Function for initializing the UART module.
 */
static void uart_init(void)
{
    /**@snippet [UART Initialization] */
    simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
    
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
    
    NVIC_SetPriority(UART0_IRQn, APP_IRQ_PRIORITY_LOW);
    NVIC_EnableIRQ(UART0_IRQn);
    /**@snippet [UART Initialization] */
}


/**@brief   Function for handling UART interrupts.
 *
 * @details This function will receive a single character from the UART and append it to a string.
 *          The string will be be sent over BLE when the last character received was a 'new line'
 *          i.e '\n' (hex 0x0D) or if the string has reached a length of @ref NUS_MAX_DATA_LENGTH.
 */
void UART0_IRQHandler(void)
{
    /**@snippet [Handling the data received over UART] */

    data_array[index] = simple_uart_get();
    index++;
#if 0
    uint32_t err_code;
  
    if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN - 1)))
    {
        err_code = ble_nus_send_string(&m_nus, data_array, index + 1);
        if (err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_CHECK(err_code);
        }
        
        index = 0;
    }
#else
    if (index >= (BLE_NUS_MAX_DATA_LEN - 1)) {
        index = 0;
    }
#endif
    /**@snippet [Handling the data received over UART] */
}

/**@brief   Function for handling QQService data upload.
 *
 * @details This function will upload the data by QQ service
 */
static void  qqs_data_upload(void)
{
  ff_property_t ffprop;
  int8_t ret;
  
  //set for test
  index = 6;
  data_array[0] = 0x01;
  data_array[1] = 0x01;
  data_array[2] = 0x00;
  data_array[3] = 0x07;
  data_array[4] = 0x02;
  data_array[5] = 0x00;
  
  //set the property, direct copy the value to transport stream
  //int8_t fd_set_property(struct fd_engine_s* engine, ff_property_t* prop);
  if (g_fd_engine != NULL) {
     ffprop.seq_no = 0x01;
     ffprop.len = index;
     ffprop.prop_id = 100000081;
     ffprop.value = data_array;
     
     if (ffprop.len == 0) return;
    
     ret = fd_set_property(g_fd_engine, &ffprop);
     if (ret != 0) {
       printf("fd set prop error %d\r\n", ret);
    } else {
			 printf("fd set prop ok\r\n");
		}
  }
}


/**@brief   Function for handling Nordic UART Service data upload.
 *
 * @details This function will upload the data by Nordic UART service
 */
static void  nus_data_upload(void)
{
  uint32_t err_code;
  
  //if (index == 0)
    //return;
  //set for test
  index = 6;
  data_array[0] = 0x01;
  data_array[1] = 0x01;
  data_array[2] = 0x00;
  data_array[3] = 0x07;
  data_array[4] = 0x02;
  data_array[5] = 0x00;
  
  err_code = ble_nus_send_string(&m_nus, data_array, index);
  if (err_code != NRF_ERROR_INVALID_STATE)
  {
     APP_ERROR_CHECK(err_code);
  }
  printf("NUS upload\r\n");
}

/**@brief  Application main function.
 */
int main(void)
{
    uint32_t err_code;

    // Initialize
    leds_init();

		timers_init();
    buttons_init();
    uart_init();
    
    ble_stack_init();
    gap_params_init();
    
	  #if 0
    ble_gap_addr_t device_mac;
    device_mac.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;
    device_mac.addr[0]   = 0xFD;
	  device_mac.addr[1]   = 0xFC;
	  device_mac.addr[2]   = 0xC1;
	  device_mac.addr[3]   = 0x0C;
	  device_mac.addr[4]   = 0xAF;
	  device_mac.addr[5]   = 0xF2;

    err_code = sd_ble_gap_address_set(&device_mac);
    APP_ERROR_CHECK(err_code);
	  #endif
    
    services_init();
    advertising_init();
    conn_params_init();
    sec_params_init();
    
    simple_uart_putstring(START_STRING);
    
    advertising_start();
    
    err_code = pstorage_init();
    APP_ERROR_CHECK(err_code);
    
    fh_data_load();
    
		#if FT_SET_DEFAULT  
		//enable this code when first load
    fh_data_set_default();
    fh_data_store();
    #endif
    
    g_fd_engine = fd_init(s_fd_zone, 256);
    if (g_fd_engine != NULL) {
       printf("fd_init OK!\r\n");
    }
    
    // Enter main loop
    for (;;)
    {
        power_manage();
    }
}

/** 
 * @}
 */
