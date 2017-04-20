/****************************************Copyright (c)**************************************************
**                                  Tencent  &    Honestar
**
**                                 http://www.tencent.com
**                                 http://www.honestar.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:           fh_impl.c
** Last modified Date:  2015/1/22
** Last Version:        V1.0
** Description:         fh_* function implementation  in txble SDK
**
**------------------------------------------------------------------------------------------------------
** Created By:          Alan Lan
** Created date:        2015/1/22
** Version:             V1.0
** Descriptions:        Initial Version
**
**------------------------------------------------------------------------------------------------------
** Modified by:         Alick Wang
** Modified date:
** Version:
** Description:
**
********************************************************************************************************/

#include "fh_impl.h"
#include "oc_dump.h"

/*********************************************************************************************************
** Global variable
*********************************************************************************************************/
static pstorage_handle_t      m_storage_handle;                                     /**< Persistent storage handle for blocks requested by the module. */
QQSecInfo qqSecurityInfo;

extern ble_qqs_t           m_qqs;
extern struct fd_engine_s *g_fd_engine;

extern void load_mac_str(char *pMacAddrStr);

/*********************************************************************************************************
** Config variable, you should implement 
*********************************************************************************************************/

int16_t fh_get_license_length(void)
{
  return qqSecurityInfo.licenselen;
}

void fh_load_license(uint8_t* zone)
{
  PRINTF("fh_load_license\r\n");
  memcpy(zone, qqSecurityInfo.license, qqSecurityInfo.licenselen);
  DUMP(zone, qqSecurityInfo.licenselen);
}

uint32_t fh_get_pid(void)
{
  PRINTF("fh_get_pid = %d\r\n", qqSecurityInfo.pid);
  return qqSecurityInfo.pid;
}

void fh_load_auth_key(uint8_t* zone)
{
  PRINTF("fh_load_auth_key\r\n");
  memcpy(zone, qqSecurityInfo.authkey, AUTH_KEY_LEN);
  DUMP(zone, AUTH_KEY_LEN);
}

//mac string has 12 chars, format as AABBCCDDEEFF, upper case
void fh_load_mac(uint8_t* zone)
{
  load_mac_str(zone);
  PRINTF("fh_load_mac = %s\r\n", zone);
}

void fh_load_puk(uint8_t* zone)
{
  PRINTF("fh_load_puk\r\n");
  memcpy(zone, qqSecurityInfo.pukstr, PUK_STR_LEN);
  DUMP(zone, PUK_STR_LEN);
}

//if din exist ,return 0, which means OK; or else, return FF_FUNC_RET_DATA_NULL
void fh_load_din(uint8_t* zone)
{
  PRINTF("fh_load_din\r\n");
  memcpy(zone, qqSecurityInfo.din, DIN_LEN);
  DUMP(zone, DIN_LEN);
}


/*********************************************************************************************************
** Interface in txble, implement by hardware
*********************************************************************************************************/

//save the din(8 bytes) , when the device relauch, the token would be exist
void fh_save_din(uint8_t* din)
{
  PRINTF("fh_save_din...\r\n");
  DUMP(din, DIN_LEN);
	if (memcmp(din, "\x5\x5\x5\x5\x5\x5\x5\x5", DIN_LEN) == 0) {
		PRINTF("firefly, not save");
		return;
	}
  memcpy(qqSecurityInfo.din, din, DIN_LEN);
  fh_data_store();
}


//this should implement by hardware bussiness
int8_t fh_send(uint8_t* datum, uint16_t length)
{
  int err_code;
  
  PRINTF("fh_send (%d)\r\n", length);
  DUMP(datum, length);
	
  err_code = ble_qqs_indicate_data(&m_qqs, datum, length);
  if (err_code != NRF_ERROR_INVALID_STATE)
  {
    APP_ERROR_CHECK(err_code);
  }
  fd_flush(g_fd_engine);
  return 0;
}

int8_t fh_disconnect(void)
{
  PRINTF("fh_disconnect\r\n");
  return 0;
}

//if could not interface for human, just set it NULL
//when User agree to bind,for example ,click twice;the call bind
int8_t fh_on_wait_bind(int8_t bind(struct fd_engine_s* engine))
{
  PRINTF("fh_on_wait_bind\r\n");
  return 0;
}

void fh_on_binded(int8_t result)
{
  PRINTF("fh_on_binded\r\n");
}

void fh_on_login(int8_t result, uint32_t timestamp)
{
  PRINTF("fh_on_login\r\n");
}

//app get property value from device
int8_t fh_get_property(ff_property_t* prop)
{
  PRINTF("fh_get_property\r\n");
  leds_ctrl(mLED_4, 2);
  return 0;
}

//app set property value to device
int8_t fh_set_property(ff_property_t* prop)
{
  PRINTF("fh_set_property prop_id=%d seq = %hd (%d)\r\n", prop->prop_id, prop->seq_no, prop->len);
	DUMP(prop->value, prop->len);
  leds_ctrl(mLED_3, 2);
	
	uint32_t val;
  memcpy(&val,prop->value,prop->len);
	
  return 0;
}

//app post result to device
int8_t fh_post_result(ff_result_t* result)
{
	PRINTF("fh_post_result\r\n");
  leds_ctrl(mLED_4, 2);
  return 0;
}


/*********************************************************************************************************
** internal implement,  it is not defined by TXBLE
*********************************************************************************************************/

void fh_data_set_default(void)
{
	PRINTF("fh_data_set_default \r\n");
  uint8_t license[] = FT_LICENSE;
  uint8_t authkey[16] = FT_DEFAULT_AUTH_KEY;
  uint8_t pukstr[25]  = FT_DEFAULT_PUK;
	
  memcpy(qqSecurityInfo.license, license, FT_LICENSE_LEN);
  qqSecurityInfo.licenselen = FT_LICENSE_LEN;
  qqSecurityInfo.pid = FT_DEFAULT_PID;
  memcpy(qqSecurityInfo.authkey, authkey, AUTH_KEY_LEN);
  memcpy(qqSecurityInfo.pukstr, pukstr, PUK_STR_LEN);
}

// Event Notification Handler.
static void fh_pstorage_cb_handler(pstorage_handle_t * p_handle,
                                   uint8_t             op_code,
                                   uint32_t            result,
                                   uint8_t           * p_data,
                                   uint32_t            data_len)
{
    switch(op_code)
    {
        case PSTORAGE_LOAD_OP_CODE:
           if (result == NRF_SUCCESS)
           {
               // Store operation successful.
           }
           else
           {
               // Store operation failed.
           }
           // Source memory can now be reused or freed.
           break;       
        case PSTORAGE_UPDATE_OP_CODE:
           if (result == NRF_SUCCESS)
           {
               // Update operation successful.
           }
           else
           {
               // Update operation failed.
           }
           break;
       case PSTORAGE_CLEAR_OP_CODE:
           if (result == NRF_SUCCESS)
           {
               // Clear operation successful.
           }
           else
           {
               // Clear operation failed.
           }
           break;
    }
}

void fh_data_load()
{
	PRINTF("fh_data_load\r\n");
  uint32_t err_code;
  pstorage_module_param_t param;
  pstorage_handle_t       block_handle;
  
  //All context with respect to a particular device is stored contiguously.
  param.block_size  = FDATA_BLOCK_SIZE;
  param.block_count = FDATA_BLOCK_COUNT;
  param.cb          = fh_pstorage_cb_handler;
  
  err_code = pstorage_register(&param, &m_storage_handle);
  if (err_code == NRF_SUCCESS)
  {
 		err_code = pstorage_block_identifier_get(&m_storage_handle, 0, &block_handle);
		//Issue read request if you successfully get the block identifier.
		if (err_code == NRF_SUCCESS)
		{
			 err_code = pstorage_load((uint8_t *)&qqSecurityInfo,	&block_handle, FDATA_BLOCK_SIZE, 0);
			 if (err_code == NRF_SUCCESS)
			 {
				 return;
			 }
		 }
		 //err_code = pstorage_clear(&m_storage_handle, (param.block_size * param.block_count));
   }
	 PRINTF("fh_data_load failed\r\n");
}

void fh_data_store(void)
{
  uint32_t err_code;
  pstorage_handle_t       block_handle;
  PRINTF("fh_data_store\r\n");
  err_code = pstorage_block_identifier_get(&m_storage_handle, 0, &block_handle);
  //Issue read request if you successfully get the block identifier.
  if (err_code == NRF_SUCCESS)
  {
    err_code = pstorage_update(&block_handle, (uint8_t *)&qqSecurityInfo, FDATA_BLOCK_SIZE, 0);
    if (err_code == NRF_SUCCESS)
    {
			PRINTF("Success store qqSecurityInfo\r\r");
			DUMP((uint8_t *)&qqSecurityInfo, FDATA_BLOCK_SIZE);
      return;
    }
  }
	PRINTF("fh_data_store failed = %d\r\n", err_code);
}

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/
