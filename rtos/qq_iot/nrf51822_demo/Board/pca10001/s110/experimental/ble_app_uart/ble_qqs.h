/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
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

/**@file
 *
 * @defgroup ble_sdk_srv_nus Nordic UART Service - Experimental
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Nordic QQ Service implementation.
 *
 * @details  The Nordic QQ Service is a simple GATT based service with a TX and RX
 *           characteristics. Data received from the peer will be passed to the application and the
 *           data received from the application of this service will be sent to the peer as Handle
 *           Value Notifications. This module is intended to demonstrate how custom GATT based
 *           service and characteristics can be implemented using the S110 SoftDevice. This service
 *           is used by the application to send and receive ASCII text strings to and from the
 *           peer.
 *
 * @note     The application must propagate S110 SoftDevice events to the Nordic UART Service module
 *           by calling the ble_nus_on_ble_evt() function from the @ref ble_stack_handler callback.
 */

#ifndef BLE_QQS_H__
#define BLE_QQS_H__

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>


#define BLE_UUID_QQS_SERVICE                   0xFEBA                       /**< The UUID of the Nordic QQ Service. */
#define BLE_UUID_QQS_WRITE_CHARACTERISTICS     0xFEC7                       /**< The UUID of the Write Characteristic. */
#define BLE_UUID_QQS_INDICATE_CHARACTERISTICS  0xFEC8                       /**< The UUID of the Indicate Characteristic. */
#define BLE_UUID_QQS_READ_CHARACTERISTICS      0xFEC9                       /**< The UUID of the Read Characteristic. */
#define BLE_UUID_QQS_BASE {0x03, 0x0c, 0x7d, 0xe0, 0x0c, 0x7e, 0xc0, 0x99,0xf1, 0xf1, 0xba, 0xfe, 0x00, 0x00, 0x00, 0x00};

#define BLE_QQS_MAX_DATA_LEN            (GATT_MTU_SIZE_DEFAULT - 3)  /**< Maximum length of data (in bytes) that can be transmitted by the Nordic UART service module to the peer. */

// Forward declaration of the ble_qqs_t type. 
typedef struct ble_qqs_s ble_qqs_t;

/**@brief Nordic QQ Service event handler type. */
typedef void (*ble_qqs_data_handler_t) (ble_qqs_t * p_qqs, uint8_t * data, uint16_t length);

/**@brief   Nordic QQ Service init structure.
 *
 * @details This structure contains the initialization information for the service. The application
 *          needs to fill this structure and pass it to the service using the @ref ble_qqs_init
 *          function.
 */
typedef struct
{
    ble_qqs_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */
} ble_qqs_init_t;

/**@brief   Nordic QQ Service structure.
 *
 * @details This structure contains status information related to the service.
 */
typedef struct ble_qqs_s
{
    uint16_t                 service_handle;          /**< Handle of Nordic UART Service (as provided by the S110 SoftDevice). */
    ble_gatts_char_handles_t indicate_handles;
    ble_gatts_char_handles_t write_handles;           /**< Handles related to the TX characteristic. (as provided by the S110 SoftDevice)*/
    ble_gatts_char_handles_t read_handles;            /**< Handles related to the RX characteristic. (as provided by the S110 SoftDevice)*/
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the S110 SoftDevice). This will be BLE_CONN_HANDLE_INVALID if not in a connection. */
    ble_qqs_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */

} ble_qqs_t;

typedef struct _DATA_INFO
{
  uint8_t *data;
  int len;
  int offset;
} data_info;

/**@brief       Function for initializing the Nordic QQ Service.
 *
 * @param[out]  p_qqs       Nordic QQ Service structure. This structure will have to be supplied
 *                          by the application. It will be initialized by this function and will
 *                          later be used to identify this particular service instance.
 * @param[in]   p_qqs_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 *              This function returns NRF_ERROR_NULL if either of the pointers p_qqs or p_qqs_init
 *              is NULL.
 */
uint32_t ble_qqs_init(ble_qqs_t * p_qqs, const ble_qqs_init_t * p_qqs_init);

/**@brief       Nordic QQ Service BLE event handler.
 *
 * @details     The Nordic QQ service expects the application to call this function each time an
 *              event is received from the S110 SoftDevice. This function processes the event if it
 *              is relevant for it and calls the Nordic QQ Service event handler of the
 *              application if necessary.
 *
 * @param[in]   p_qqs      Nordic QQ Service structure.
 * @param[in]   p_ble_evt  Event received from the S110 SoftDevice.
 */
void ble_qqs_on_ble_evt(ble_qqs_t * p_qqs, ble_evt_t * p_ble_evt);

/**@brief       Function for sending data to the peer.
 *
 * @details     This function will send the input data as a Indicate characteristic notification to the
 *              peer.
  *
 * @param[in]   p_qqs          Pointer to the Nordic QQ Service structure.
 * @param[in]   data           data to be sent.
 * @param[in]   length         Length of string.
 *
 * @return      NRF_SUCCESS if the QQ Service has successfully requested the S110 SoftDevice to
 *              send the notification. Otherwise an error code.
 *              This function returns NRF_ERROR_INVALID_STATE if the device is not connected to a
 *              peer or if the notification of the RX characteristic was not enabled by the peer.
 *              It returns NRF_ERROR_NULL if the pointer p_nus is NULL.
 */
int ble_qqs_indicate_data(ble_qqs_t *p_qqs, uint8_t *data, int len);

#endif // BLE_QQS_H__

/** @} */
