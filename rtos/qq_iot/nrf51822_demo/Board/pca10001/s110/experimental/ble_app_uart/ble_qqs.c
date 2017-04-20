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
 */

#include "ble_qqs.h"
#include "nordic_common.h"
#include "ble_srv_common.h"
#include <string.h>

#include "fh_impl.h"

data_info g_send_data = {NULL, 0, 0};
//data_info g_recv_data = {NULL, 0, 0};
 
static uint8_t m_addl_adv_manuf_data[BLE_GAP_ADDR_LEN + 4];

static void get_mac_addr(uint8_t *p_mac_addr)
{
  uint32_t error_code;
  ble_gap_addr_t mac_addr_t;

  *p_mac_addr = 0x9A;
   p_mac_addr++;
  *p_mac_addr = 0x3B;
   p_mac_addr++;
  
  error_code = sd_ble_gap_address_get(&mac_addr_t);
	APP_ERROR_CHECK(error_code);

  memcpy(p_mac_addr, mac_addr_t.addr, 6);
}

static int ble_qqs_indicate_data_chunk(ble_qqs_t *p_qqs)
{
		ble_gatts_hvx_params_t hvx_params;
		uint16_t chunk_len = 0;
	
		chunk_len = g_send_data.len - g_send_data.offset;
  
		chunk_len = (chunk_len > BLE_QQS_MAX_DATA_LEN)?BLE_QQS_MAX_DATA_LEN:chunk_len;

		if (chunk_len == 0) {
			  //Free Top Layer Buffer
				g_send_data.data = NULL;
				g_send_data.len = 0;
				g_send_data.offset = 0;
				
        return -1;
		}
    
		memset(&hvx_params, 0, sizeof(hvx_params));
		hvx_params.handle = p_qqs->indicate_handles.value_handle;
    hvx_params.p_data = g_send_data.data + g_send_data.offset;
    hvx_params.p_len  = &chunk_len;
    hvx_params.type   = BLE_GATT_HVX_INDICATION;

		g_send_data.offset += chunk_len;
    
		return sd_ble_gatts_hvx(p_qqs->conn_handle, &hvx_params);
}

/**@brief     Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_nus     Nordic QQ Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_connect(ble_qqs_t * p_qqs, ble_evt_t * p_ble_evt)
{
    p_qqs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief     Function for handling the @ref BLE_GAP_EVT_DISCONNECTED event from the S110
 *            SoftDevice.
 *
 * @param[in] p_nus     Nordic QQ Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_disconnect(ble_qqs_t * p_qqs, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_qqs->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief     Function for handling the @ref BLE_GATTS_EVT_WRITE event from the S110 SoftDevice.
 *
 * @param[in] p_qqs     Nordic QQ Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_write(ble_qqs_t * p_qqs, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    if (
         (p_evt_write->handle == p_qqs->write_handles.value_handle)
      && (p_evt_write->len <= BLE_QQS_MAX_DATA_LEN) 
      && (p_qqs->data_handler != NULL)
        )
    {
        p_qqs->data_handler(p_qqs, p_evt_write->data, p_evt_write->len);
    }
    else
    {
        // Do Nothing. This event is not relevant to this service.
    }
}

/**@brief     Function for handling the @ref BLE_GATTS_EVT_WRITE event from the S110 SoftDevice.
 *
 * @param[in] p_qqs     Nordic QQ Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
void on_indicate_comfirm(ble_qqs_t *p_qqs, ble_evt_t * p_ble_evt)
{
		ble_qqs_indicate_data_chunk(p_qqs);
}


/**@brief       Function for adding Indicate characteristic.
 *
 * @param[in]   p_qqs        Nordic QQ Service structure.
 * @param[in]   
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_qqs_add_indicate_char(ble_qqs_t *p_qqs,uint8_t qqs_type)
{
  ble_gatts_char_md_t char_md;
  ble_gatts_attr_md_t cccd_md;
  ble_gatts_attr_t    attr_char_value;
  ble_uuid_t          char_uuid;
  ble_gatts_attr_md_t attr_md;
	
  char *data = "indicate char";
		
  memset(&cccd_md, 0, sizeof(cccd_md));
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
  cccd_md.vloc = BLE_GATTS_VLOC_STACK;
	
  memset(&char_md, 0, sizeof(char_md));
  char_md.char_props.indicate = 1;
  char_md.p_char_user_desc  = NULL;
  char_md.p_char_pf         = NULL;
  char_md.p_user_desc_md    = NULL;
  char_md.p_cccd_md         = NULL;//&cccd_md;
  char_md.p_sccd_md         = NULL;
	
  //BLE_UUID_BLE_ASSIGN(char_uuid, BLE_UUID_QQS_INDICATE_CHARACTERISTICS);
	char_uuid.type = qqs_type;
  char_uuid.uuid = BLE_UUID_QQS_INDICATE_CHARACTERISTICS;
	
  memset(&attr_md, 0, sizeof(attr_md));
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
  attr_md.rd_auth    = 0;
  attr_md.wr_auth    = 0;
  attr_md.vlen       = 1;
    
  memset(&attr_char_value, 0, sizeof(attr_char_value)); 
  attr_char_value.p_uuid       = &char_uuid;
  attr_char_value.p_attr_md    = &attr_md;
  attr_char_value.init_len     = strlen(data);
  attr_char_value.init_offs    = 0;
  attr_char_value.max_len      = BLE_QQS_MAX_DATA_LEN;
  attr_char_value.p_value      = (uint8_t *)data;
  
  return sd_ble_gatts_characteristic_add(p_qqs->service_handle,
                                         &char_md,
                                         &attr_char_value,
                                         &p_qqs->indicate_handles);
}

/**@brief       Function for adding Write characteristic.
 *
 * @param[in]   p_qqs        Nordic QQ Service structure.
 * @param[in]   
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t ble_qqs_add_write_char(ble_qqs_t *p_qqs,uint8_t qqs_type)
{
  ble_gatts_char_md_t char_md;
  ble_gatts_attr_t    attr_char_value;
  ble_uuid_t          char_uuid;
  ble_gatts_attr_md_t attr_md;
  char *data = "write char";
	
  memset(&char_md, 0, sizeof(char_md));
  char_md.char_props.write = 1;
  char_md.p_char_user_desc  = NULL;
  char_md.p_char_pf         = NULL;
  char_md.p_user_desc_md    = NULL;
  char_md.p_cccd_md         = NULL;
  char_md.p_sccd_md         = NULL;

  //BLE_UUID_BLE_ASSIGN(char_uuid, BLE_UUID_QQS_WRITE_CHARACTERISTICS);
	char_uuid.type = qqs_type;
  char_uuid.uuid = BLE_UUID_QQS_WRITE_CHARACTERISTICS;

  memset(&attr_md, 0, sizeof(attr_md));
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
  attr_md.rd_auth    = 0;
  attr_md.wr_auth    = 0;
  attr_md.vlen       = 1;
  
  memset(&attr_char_value, 0, sizeof(attr_char_value));
  
  attr_char_value.p_uuid       = &char_uuid;
  attr_char_value.p_attr_md    = &attr_md;
  attr_char_value.init_len     = strlen(data);
  attr_char_value.init_offs    = 0;
  attr_char_value.max_len      = BLE_QQS_MAX_DATA_LEN;
  attr_char_value.p_value      = (uint8_t *)data;
  
  return sd_ble_gatts_characteristic_add(p_qqs->service_handle,
                                         &char_md,
                                         &attr_char_value,
                                         &p_qqs->write_handles);
}

/**@brief       Function for adding Read characteristic.
 *
 * @param[in]   p_qqs        Nordic QQ Service structure.
 * @param[in]   
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_qqs_add_read_char(ble_qqs_t *p_qqs,uint8_t qqs_type)
{
  //m_addl_adv_manuf_data.
  get_mac_addr(m_addl_adv_manuf_data);
  
  ble_gatts_char_md_t char_md;
  ble_gatts_attr_t    attr_char_value;
  ble_uuid_t          char_uuid;
  ble_gatts_attr_md_t attr_md;

  memset(&char_md, 0, sizeof(char_md));
  char_md.char_props.read = 1;
  char_md.p_char_user_desc  = NULL;
  char_md.p_char_pf         = NULL;
  char_md.p_user_desc_md    = NULL;
  char_md.p_cccd_md         = NULL;
  char_md.p_sccd_md         = NULL;

  BLE_UUID_BLE_ASSIGN(char_uuid, BLE_UUID_QQS_READ_CHARACTERISTICS);
	char_uuid.type = qqs_type;
  char_uuid.uuid = BLE_UUID_QQS_READ_CHARACTERISTICS;

  memset(&attr_md, 0, sizeof(attr_md));
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
  attr_md.vloc       = BLE_GATTS_VLOC_STACK;
  attr_md.rd_auth    = 0;
  attr_md.wr_auth    = 0;
  attr_md.vlen       = 1;
  
  memset(&attr_char_value, 0, sizeof(attr_char_value));
  attr_char_value.p_uuid       = &char_uuid;
  attr_char_value.p_attr_md    = &attr_md;
  attr_char_value.init_len     = sizeof(m_addl_adv_manuf_data);
  attr_char_value.init_offs    = 0;
  attr_char_value.max_len      = BLE_QQS_MAX_DATA_LEN;
  attr_char_value.p_value      = m_addl_adv_manuf_data;
  
  return sd_ble_gatts_characteristic_add(p_qqs->service_handle,
                                         &char_md,
                                         &attr_char_value,
                                         &p_qqs->read_handles);
}

void ble_qqs_on_ble_evt(ble_qqs_t * p_qqs, ble_evt_t * p_ble_evt)
{
    if ((p_qqs == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_qqs, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_qqs, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_qqs, p_ble_evt);
            break;
        
        case BLE_GATTS_EVT_HVC:
						on_indicate_comfirm(p_qqs, p_ble_evt);
						break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_qqs_init(ble_qqs_t * p_qqs, const ble_qqs_init_t * p_qqs_init)
{
    uint32_t        err_code;
    ble_uuid_t      ble_qqs_uuid;
	ble_uuid128_t   qqs_base_uuid = BLE_UUID_QQS_BASE;
	uint8_t qqs_type;
 
    if ((p_qqs == NULL) || (p_qqs_init == NULL))
    {
        return NRF_ERROR_NULL;
    }
    
    // Initialize service structure.
    p_qqs->conn_handle              = BLE_CONN_HANDLE_INVALID;
    p_qqs->data_handler             = p_qqs_init->data_handler;
    
    /**@snippet [Adding proprietary Service to S110 SoftDevice] */

    // Add custom base UUID.
    BLE_UUID_BLE_ASSIGN(ble_qqs_uuid, BLE_UUID_QQS_SERVICE);
		err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_qqs_uuid, &p_qqs->service_handle);

    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    // Add RX Characteristic.
		
	  err_code = sd_ble_uuid_vs_add(&qqs_base_uuid, &qqs_type);
  if (err_code != NRF_SUCCESS)
  {
     return err_code;
  }

    err_code = ble_qqs_add_indicate_char(p_qqs,qqs_type);
		APP_ERROR_CHECK(err_code);
		
    err_code = ble_qqs_add_write_char(p_qqs,qqs_type);
		APP_ERROR_CHECK(err_code);
		
    err_code = ble_qqs_add_read_char(p_qqs,qqs_type);
		APP_ERROR_CHECK(err_code);
		
    return err_code;
}

//device sent data on the indicate characteristic
int ble_qqs_indicate_data(ble_qqs_t *p_qqs, uint8_t *data, int len)
{
  if ((data == NULL) || (len == 0)) {
      return 0;	
  }
  
  if ((g_send_data.len != 0) && (g_send_data.offset != g_send_data.len)) {
      return 0;
  }
  
  g_send_data.data = data;
  g_send_data.len = len;
  g_send_data.offset = 0;
  
  return (ble_qqs_indicate_data_chunk(p_qqs));
}


