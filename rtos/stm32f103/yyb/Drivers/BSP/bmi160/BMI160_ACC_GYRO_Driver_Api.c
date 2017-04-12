#include "BMI160_ACC_GYRO_Driver_Api.h"
#include "BMI160_ACC_GYRO_Calibate.h"
#include <math.h>
#include "flash_drv.h"

#define MFG_LOG_ERR printf
#define MFG_LOG_INFO printf
#define FOTA_LOG_START  0
#define ACC_XUP_FLASHID 	FOTA_LOG_START
#define ACC_XDOWN_FLASHID	  (FOTA_LOG_START + 30720)
#define ACC_YUP_FLASHID		(FOTA_LOG_START + 61440)
#define ACC_YDOWN_FLASHID	 (FOTA_LOG_START + 92160)
#define ACC_ZUP_FLASHID		(FOTA_LOG_START + 122880)
#define ACC_ZDOWN_FLASHID		(FOTA_LOG_START + 153600)
//#define ACC_XDOWN_FLASHID	6
__weak int getBmi160OffsetValue(void *buff)
{
	return 0;
}

BMI160_DataTypeDef bmi160_offset = {0};
extern int abs(int i);
//static struct pwrmgr_notify_func_data Bmi160PmNotifyData = {0};

//extern int8_t usb_app_transmit_data(uint8_t *buf, uint8_t len);
/* int bmi160_notify_callback(uint32_t _notify_flag, uint32_t _state, void *data); */
void bmi160_postinit(void);
static DrvStatusTypeDef BMI160_X_Init( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_X_DeInit( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_X_Sensor_Enable( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_X_Sensor_Disable( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_X_Get_WhoAmI( DrvContextTypeDef *handle, uint8_t *who_am_i );
static DrvStatusTypeDef BMI160_X_Check_WhoAmI( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_X_Get_Axes( DrvContextTypeDef *handle, SensorAxes_t *acceleration );
static DrvStatusTypeDef BMI160_X_Get_AxesRaw( DrvContextTypeDef *handle, SensorAxesRaw_t *value );
static DrvStatusTypeDef BMI160_X_Get_Sensitivity( DrvContextTypeDef *handle, float *sensitivity );
static DrvStatusTypeDef BMI160_X_Get_ODR( DrvContextTypeDef *handle, float *odr );
static DrvStatusTypeDef BMI160_X_Set_ODR( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef BMI160_X_Set_ODR_Value( DrvContextTypeDef *handle, float odr );
static DrvStatusTypeDef BMI160_X_Get_FS( DrvContextTypeDef *handle, float *fullScale );
static DrvStatusTypeDef BMI160_X_Set_FS( DrvContextTypeDef *handle, SensorFs_t fs );
static DrvStatusTypeDef BMI160_X_Set_FS_Value( DrvContextTypeDef *handle, float fullScale );
static DrvStatusTypeDef BMI160_X_Get_Axes_Status( DrvContextTypeDef *handle, us8 *xyz_enabled );
static DrvStatusTypeDef BMI160_X_Set_Axes_Status( DrvContextTypeDef *handle, us8 *enable_xyz );

static DrvStatusTypeDef BMI160_G_Init( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_G_DeInit( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_G_Sensor_Enable( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_G_Sensor_Disable( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_G_Get_WhoAmI( DrvContextTypeDef *handle, uint8_t *who_am_i );
static DrvStatusTypeDef BMI160_G_Check_WhoAmI( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_G_Get_Axes( DrvContextTypeDef *handle, SensorAxes_gyro_t *angular_velocity );
static DrvStatusTypeDef BMI160_G_Get_AxesRaw( DrvContextTypeDef *handle, SensorAxesRaw_t *value );
static DrvStatusTypeDef BMI160_G_Get_Sensitivity( DrvContextTypeDef *handle, float *sensitivity );
static DrvStatusTypeDef BMI160_G_Get_ODR( DrvContextTypeDef *handle, float *odr );
static DrvStatusTypeDef BMI160_G_Set_ODR( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef BMI160_G_Set_ODR_Value( DrvContextTypeDef *handle, float odr );
static DrvStatusTypeDef BMI160_G_Get_FS( DrvContextTypeDef *handle, float *fullScale );
static DrvStatusTypeDef BMI160_G_Set_FS( DrvContextTypeDef *handle, SensorFs_t fullScale );
static DrvStatusTypeDef BMI160_G_Set_FS_Value( DrvContextTypeDef *handle, float fullScale );
static DrvStatusTypeDef BMI160_G_Get_Axes_Status( DrvContextTypeDef *handle, us8 *xyz_enabled );
static DrvStatusTypeDef BMI160_G_Set_Axes_Status( DrvContextTypeDef *handle, us8 *enable_xyz );


static DrvStatusTypeDef BMI160_Get_WhoAmI( DrvContextTypeDef *handle, uint8_t *who_am_i );
static DrvStatusTypeDef BMI160_Check_WhoAmI( DrvContextTypeDef *handle );
static DrvStatusTypeDef BMI160_X_Set_ODR_When_Enabled( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef BMI160_X_Set_ODR_When_Disabled( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef BMI160_X_Set_ODR_Value_When_Enabled( DrvContextTypeDef *handle, float odr );
static DrvStatusTypeDef BMI160_X_Set_ODR_Value_When_Disabled( DrvContextTypeDef *handle, float odr );
static DrvStatusTypeDef BMI160_G_Set_ODR_When_Enabled( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef BMI160_G_Set_ODR_When_Disabled( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef BMI160_G_Set_ODR_Value_When_Enabled( DrvContextTypeDef *handle, float odr );
static DrvStatusTypeDef BMI160_G_Set_ODR_Value_When_Disabled( DrvContextTypeDef *handle, float odr );




ACCELERO_Drv_t BMI160_X_Drv =
{
  BMI160_X_Init,
  BMI160_X_DeInit,
  BMI160_X_Sensor_Enable,
  BMI160_X_Sensor_Disable,
  BMI160_X_Get_WhoAmI,
  BMI160_X_Check_WhoAmI,
  BMI160_X_Get_Axes,
  BMI160_X_Get_AxesRaw,
  BMI160_X_Get_Sensitivity,
  BMI160_X_Get_ODR,
  BMI160_X_Set_ODR,
  BMI160_X_Set_ODR_Value,
  BMI160_X_Get_FS,
  BMI160_X_Set_FS,
  BMI160_X_Set_FS_Value,
  BMI160_X_Get_Axes_Status,
  BMI160_X_Set_Axes_Status
};
GYRO_Drv_t BMI160_G_Drv =
{
  BMI160_G_Init,
  BMI160_G_DeInit,
  BMI160_G_Sensor_Enable,
  BMI160_G_Sensor_Disable,
  BMI160_G_Get_WhoAmI,
  BMI160_G_Check_WhoAmI,
  BMI160_G_Get_Axes,
  BMI160_G_Get_AxesRaw,
  BMI160_G_Get_Sensitivity,
  BMI160_G_Get_ODR,
  BMI160_G_Set_ODR,
  BMI160_G_Set_ODR_Value,
  BMI160_G_Get_FS,
  BMI160_G_Set_FS,
  BMI160_G_Set_FS_Value,
  BMI160_G_Get_Axes_Status,
  BMI160_G_Set_Axes_Status,
};

BMI160_Combo_Data_t BMI160_Combo_Data[BMI160_SENSORS_MAX_NUM];


static DrvStatusTypeDef BMI160_X_Init( DrvContextTypeDef *handle )
{
  //us8 axes_status[] = {1,1,1};
  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
  BMI160_X_Data_t *pComponentData = ( BMI160_X_Data_t * )pData->pComponentData;

  if ( BMI160_Check_WhoAmI( handle ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Select default output data rate. */
  //pComponentData->Previous_ODR = BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ
  pComponentData->Previous_ODR = 800.0f;
  pComponentData->comboData->lastODR = 800.0f;

  /* Output data rate selection - power down. */
  if ( bmi160_set_accel_output_data_rate(BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ,BMI160_ACCEL_NORMAL_AVG4) != BMI160_SUCCESS )
  {
    return COMPONENT_ERROR;
  }
   bmi160_delay_ms(5);
  /* Full scale selection */
  if ( BMI160_X_Set_FS( handle, FS_LOW ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Enable axes
  if ( BMI160_X_Set_Axes_Status( handle, axes_status ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }*/

  handle->isInitialized = 1;

  return COMPONENT_OK;
}
static DrvStatusTypeDef BMI160_X_DeInit( DrvContextTypeDef *handle )
{
	  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
	  BMI160_X_Data_t *pComponentData = ( BMI160_X_Data_t * )pData->pComponentData;

	  if ( BMI160_Check_WhoAmI( handle ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }
	  bmi160_delay_ms(5);
	  /* Disable the component */
	  if ( BMI160_X_Sensor_Disable( handle ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }

	  /* Reset output data rate. */
	  pComponentData->Previous_ODR = 0.0f;

	  handle->isInitialized = 0;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Sensor_Enable( DrvContextTypeDef *handle )
{
	  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
	  BMI160_X_Data_t *pComponentData = ( BMI160_X_Data_t * )pData->pComponentData;
	  us8 v_accel_power_mode_stat_us8 = 0;

	  /* Check if the component is already enabled */
	  if ( handle->isEnabled == 1 )
	  {
		return COMPONENT_OK;
	  }

	  /* Output data rate selection */
	  if ( BMI160_X_Set_ODR_Value_When_Enabled( handle, pComponentData->Previous_ODR ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }
      /*enable accel fifo mode
	  bmi160_delay_ms(5);
      if(bmi160_set_fifo_accel_enable(0x01) != BMI160_SUCCESS)
 	  {
	    return COMPONENT_ERROR;
      }
	  bmi160_delay_ms(5);
      if(bmi160_set_fifo_down_accel(0x01) != BMI160_SUCCESS)
      {
	    return COMPONENT_ERROR;
      }
	  bmi160_delay_ms(5);
	  if(bmi160_set_fifo_header_enable(BMI160_ENABLE) != BMI160_SUCCESS)
	  {
	    return COMPONENT_ERROR;
      }*/
	  bmi160_delay_ms(5);
	  if ( bmi160_accel_mode(ACCEL_MODE_NORMAL) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }
	  bmi160_delay_ms(200);
	  if( bmi160_get_accel_power_mode_stat(&v_accel_power_mode_stat_us8) != BMI160_SUCCESS)
	  {
		 return COMPONENT_ERROR;
	  }

	  if(v_accel_power_mode_stat_us8 != 1)
	  {
		 return COMPONENT_ERROR;
	  }
	  pComponentData->comboData->isEnabled = 1;
	  handle->isEnabled = 1;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Sensor_Disable( DrvContextTypeDef *handle )
{
	  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
	  BMI160_X_Data_t *pComponentData = ( BMI160_X_Data_t * )pData->pComponentData;

	  /* Check if the component is already disabled */
	  if ( handle->isEnabled == 0 )
	  {
		return COMPONENT_OK;
	  }

	  /* Store actual output data rate. */
	  if ( BMI160_X_Get_ODR( handle, &( pComponentData->Previous_ODR ) ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }
	  bmi160_delay_ms(5);
	  /* Output data rate selection - power down.
	  if ( bmi160_set_accel_output_data_rate(BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ,BMI160_ACCEL_NORMAL_AVG4) != BMI160_SUCCESS )
	  {
		return COMPONENT_ERROR;
	  }*/

	  if(bmi160_accel_mode(ACCEL_SUSPEND) != BMI160_SUCCESS )
	  {
		return COMPONENT_ERROR;
	  }
	  pComponentData->comboData->isEnabled = 0;
	  pComponentData->comboData->lastODR = 800.0f;
	  handle->isEnabled = 0;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Get_WhoAmI( DrvContextTypeDef *handle, us8 *who_am_i )
{
    return BMI160_Get_WhoAmI( handle, who_am_i );
}
static DrvStatusTypeDef BMI160_X_Check_WhoAmI( DrvContextTypeDef *handle )
{
	return BMI160_Check_WhoAmI( handle );
}
static DrvStatusTypeDef BMI160_X_Get_Axes( DrvContextTypeDef *handle, SensorAxes_t *acceleration )
{

	  struct bmi160_accel_t accel = {0};
	  float sensitivity = 0;


	  /* Get BMI160 actual sensitivity. */
	  if ( BMI160_X_Get_Sensitivity( handle, &sensitivity ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }
	  /* Read raw data from BMI160 output register. */
	  if (bmi160_read_accel_xyz(&accel) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }




	  //printf("before acc:AXIS_X = %d  AXIS_Y = %d  AXIS_Z = %d\n",acceleration->AXIS_X,acceleration->AXIS_Y,acceleration->AXIS_Z);
	  if(bmi160_offset.isAccValid == 1)
	  {
		acceleration->AXIS_X = ( int32_ts )(bmi160_offset.accel_k_xyz[0][0]*((accel.x)*(sensitivity)-bmi160_offset.accel_x) + \
							   				bmi160_offset.accel_k_xyz[0][1]*((accel.y)*(sensitivity)-bmi160_offset.accel_y) + \
							   				bmi160_offset.accel_k_xyz[0][2]*((accel.z)*(sensitivity)-bmi160_offset.accel_z));

		acceleration->AXIS_Y = ( int32_ts )(bmi160_offset.accel_k_xyz[1][0]*((accel.x)*(sensitivity)-bmi160_offset.accel_x) + \
							  				bmi160_offset.accel_k_xyz[1][1]*((accel.y)*(sensitivity)-bmi160_offset.accel_y) + \
							  				bmi160_offset.accel_k_xyz[1][2]*((accel.z)*(sensitivity)-bmi160_offset.accel_z));

		acceleration->AXIS_Z = ( int32_ts )(bmi160_offset.accel_k_xyz[2][0]*((accel.x)*(sensitivity)-bmi160_offset.accel_x) + \
							   				bmi160_offset.accel_k_xyz[2][1]*((accel.y)*(sensitivity)-bmi160_offset.accel_y) + \
							   				bmi160_offset.accel_k_xyz[2][2]*((accel.z)*(sensitivity)-bmi160_offset.accel_z));
		//printf("after acc:AXIS_X = %d  AXIS_Y = %d  AXIS_Z = %d\n",acceleration->AXIS_X,acceleration->AXIS_Y,acceleration->AXIS_Z);
	  }
	  else
	  {
		 /* Calculate the data. */
	  	acceleration->AXIS_X = ( int32_ts )( (accel.x) * (sensitivity) );
	  	acceleration->AXIS_Y = ( int32_ts )( (accel.y) * (sensitivity) );
	 	acceleration->AXIS_Z = ( int32_ts )( (accel.z) * (sensitivity) );
	  }
	  return COMPONENT_OK;
}


static DrvStatusTypeDef BMI160_X_Get_AxesRaw( DrvContextTypeDef *handle, SensorAxesRaw_t *value )
{

	  struct bmi160_accel_t accel = {0};

	  /* Read raw data from BMI160 output register. */
	  if (bmi160_read_accel_xyz(&accel) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  /* Set the raw data. */
	  value->AXIS_X = accel.x;
	  value->AXIS_Y = accel.y;
	  value->AXIS_Z = accel.z;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Get_Sensitivity( DrvContextTypeDef *handle, float *sensitivity )
{

	  us8 fullScale;

	  /* Read actual full scale selection from sensor. */
	  if (bmi160_get_accel_range(&fullScale) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  /* Store the sensitivity based on actual full scale. */
	  switch( fullScale )
	  {
		case BMI160_ACCEL_RANGE_2G:
		  *sensitivity = ( float )BMI160_ACC_SENSITIVITY_FOR_FS_2G;
		  break;
		case BMI160_ACCEL_RANGE_4G:
		  *sensitivity = ( float )BMI160_ACC_SENSITIVITY_FOR_FS_4G;
		  break;
		case BMI160_ACCEL_RANGE_8G:
		  *sensitivity = ( float )BMI160_ACC_SENSITIVITY_FOR_FS_8G;
		  break;
		case BMI160_ACCEL_RANGE_16G:
		  *sensitivity = ( float )BMI160_ACC_SENSITIVITY_FOR_FS_16G;
		  break;
		default:
		  *sensitivity = -1.0f;
		  return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Get_ODR( DrvContextTypeDef *handle, float *odr )
{

	  us8 odr_low_level;
	  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
	  BMI160_X_Data_t *pComponentData = ( BMI160_X_Data_t * )pData->pComponentData;

	  /* Accelerometer ODR forced to be same like gyroscope ODR. */
	  if(pComponentData->comboData->isEnabled == 1)
	  {
		*odr = pComponentData->comboData->lastODR;
		return COMPONENT_OK;
	  }

	  if ( bmi160_get_accel_output_data_rate(&odr_low_level) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  switch( odr_low_level )
	  {
		case BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED:
		  *odr =   0.0f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_0_78HZ:
		  *odr =  0.78f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ:
		  *odr =  1.56f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_3_12HZ:
		  *odr = 3.12f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_6_25HZ:
		  *odr = 6.25f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ:
		  *odr = 12.5f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ:
		  *odr = 25.0f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ:
		  *odr = 50.0f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ:
		  *odr = 100.0f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ:
		  *odr = 200.0f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ:
		  *odr = 400.0f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ:
		  *odr = 800.0f;
		  break;
		case BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ:
		  *odr = 1600.0f;
		  break;
		default:
		  *odr =  0.0f;
		  return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_ODR( DrvContextTypeDef *handle, SensorOdr_t odr )
{

	  if(handle->isEnabled == 1)
	  {
		if(BMI160_X_Set_ODR_When_Enabled(handle, odr) == COMPONENT_ERROR)
		{
		  return COMPONENT_ERROR;
		}
	  }
	  else
	  {
		if(BMI160_X_Set_ODR_When_Disabled(handle, odr) == COMPONENT_ERROR)
		{
		  return COMPONENT_ERROR;
		}
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_ODR_Value( DrvContextTypeDef *handle, float odr )
{

	  if(handle->isEnabled == 1)
	  {
		if(BMI160_X_Set_ODR_Value_When_Enabled(handle, odr) == COMPONENT_ERROR)
		{
		  return COMPONENT_ERROR;
		}
	  }
	  else
	  {
		if(BMI160_X_Set_ODR_Value_When_Disabled(handle, odr) == COMPONENT_ERROR)
		{
		  return COMPONENT_ERROR;
		}
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Get_FS( DrvContextTypeDef *handle, float *fullScale )
{

	  us8 fs_low_level;

	  if ( bmi160_get_accel_range(&fs_low_level) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  switch( fs_low_level )
	  {
		case BMI160_ACCEL_RANGE_2G:
		  *fullScale =	2.0f;
		  break;
		case BMI160_ACCEL_RANGE_4G:
		  *fullScale =	4.0f;
		  break;
		case BMI160_ACCEL_RANGE_8G:
		  *fullScale =	8.0f;
		  break;
		case BMI160_ACCEL_RANGE_16G:
		  *fullScale = 16.0f;
		  break;
		default:
		  *fullScale = -1.0f;
		  return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_FS( DrvContextTypeDef *handle, SensorFs_t fs )
{
    us8 new_fs;
	switch( fs )
	{
		case FS_LOW:
		  new_fs = BMI160_ACCEL_RANGE_2G;
		  break;
		case FS_MID:
		  new_fs = BMI160_ACCEL_RANGE_4G;
		  break;
		case FS_HIGH:
		  new_fs = BMI160_ACCEL_RANGE_8G;
		  break;
		default:
		  return COMPONENT_ERROR;
	}
	if ( bmi160_set_accel_range(new_fs) != BMI160_SUCCESS)
	{
		return COMPONENT_ERROR;
	}
	return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_FS_Value( DrvContextTypeDef *handle, float fullScale )
{

	  us8 new_fs;

	  new_fs =   ( fullScale <= 2.0f ) ? BMI160_ACCEL_RANGE_2G
			   : ( fullScale <= 4.0f ) ? BMI160_ACCEL_RANGE_4G
			   : ( fullScale <= 8.0f ) ? BMI160_ACCEL_RANGE_8G
			   :						 BMI160_ACCEL_RANGE_16G;

	  if ( bmi160_set_accel_range(new_fs) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Get_Axes_Status( DrvContextTypeDef *handle, us8 *xyz_enabled )
{

	  us8 xyzStatus;

	  if ( bmi160_get_accel_power_mode_stat(&xyzStatus) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }


	  xyz_enabled[0] = ( xyzStatus == ACCEL_MODE_NORMAL ) ? 1 : 0;
	  xyz_enabled[1] = ( xyzStatus == ACCEL_MODE_NORMAL ) ? 1 : 0;
	  xyz_enabled[2] = ( xyzStatus == ACCEL_MODE_NORMAL ) ? 1 : 0;


	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_Axes_Status( DrvContextTypeDef *handle, us8 *enable_xyz )
{
      us8 xyzStatus;

	  if((enable_xyz[0]+enable_xyz[1]+enable_xyz[2]) == 0)
	  {
          xyzStatus = ACCEL_SUSPEND;
	  }
	  else
	  {
	      xyzStatus = ACCEL_MODE_NORMAL;
	  }
	  if (bmi160_accel_mode(xyzStatus) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}
static DrvStatusTypeDef BMI160_G_Init( DrvContextTypeDef *handle )
{

	  //us8 axes_status[] = { 1, 1, 1 };
	  GYRO_Data_t *pData = ( GYRO_Data_t * )handle->pData;
	  BMI160_G_Data_t *pComponentData = ( BMI160_G_Data_t * )pData->pComponentData;

	  if ( BMI160_Check_WhoAmI( handle ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }

	  /* Select default output data rate. */
	  pComponentData->Previous_ODR = 800.0f;
	  pComponentData->comboData->lastODR = 800.0f;

	  /* Output data rate selection - power down. */
	  if (bmi160_set_gyro_output_data_rate(BMI160_GYRO_OUTPUT_DATA_RATE_800HZ) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }
	  bmi160_delay_ms(5);
	  /* Full scale selection */
	  if ( BMI160_G_Set_FS( handle, FS_HIGH ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }

	  /* Enable axes
	  if ( BMI160_G_Set_Axes_Status( handle, axes_status ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }*/

	  handle->isInitialized = 1;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_DeInit( DrvContextTypeDef *handle )
{
	  GYRO_Data_t *pData = ( GYRO_Data_t * )handle->pData;
	  BMI160_G_Data_t *pComponentData = ( BMI160_G_Data_t * )pData->pComponentData;

	  if ( BMI160_Check_WhoAmI( handle ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }
	  bmi160_delay_ms(5);
	  /* Disable the component */
	  if ( BMI160_G_Sensor_Disable( handle ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }

	  /* Reset output data rate. */
	  pComponentData->Previous_ODR = 800.0f;

	  handle->isInitialized = 0;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Sensor_Enable( DrvContextTypeDef *handle )
{
	  GYRO_Data_t *pData = ( GYRO_Data_t * )handle->pData;
	  BMI160_G_Data_t *pComponentData = ( BMI160_G_Data_t * )pData->pComponentData;
	  us8 v_accel_power_mode_stat_us8 = 0;

	  /* Check if the component is already enabled */
	  if ( handle->isEnabled == 1 )
	  {
		return COMPONENT_OK;
	  }

	  /* Output data rate selection */
	  if ( BMI160_G_Set_ODR_Value_When_Enabled( handle, pComponentData->Previous_ODR ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }
	  bmi160_delay_ms(5);
	  /*enable  gyro fifo
	  if (bmi160_set_fifo_gyro_enable(0x01) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }
	  bmi160_delay_ms(5);
	  if (bmi160_set_fifo_down_gyro(0x01) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }
	   bmi160_delay_ms(5);
	  if(bmi160_set_fifo_header_enable(BMI160_ENABLE) != BMI160_SUCCESS)
	  {
	    return COMPONENT_ERROR;
      }*/
	  bmi160_delay_ms(5);
	  if ( bmi160_gyro_mode(GYRO_MODE_NORMAL) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }
	  bmi160_delay_ms(200);
	  if( bmi160_get_gyro_power_mode_stat(&v_accel_power_mode_stat_us8) != BMI160_SUCCESS)
	  {
		 return COMPONENT_ERROR;
	  }

	  if(v_accel_power_mode_stat_us8 != 1)
	  {
		 return COMPONENT_ERROR;
	  }
	  pComponentData->comboData->isEnabled = 1;
	  handle->isEnabled = 1;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Sensor_Disable( DrvContextTypeDef *handle )
{
	  GYRO_Data_t *pData = ( GYRO_Data_t * )handle->pData;
	  BMI160_G_Data_t *pComponentData = ( BMI160_G_Data_t * )pData->pComponentData;

	  /* Check if the component is already disabled */
	  if ( handle->isEnabled == 0 )
	  {
		return COMPONENT_OK;
	  }

	  /* Store actual output data rate.
	  if ( BMI160_G_Get_ODR( handle, &( pComponentData->Previous_ODR ) ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }*/
	  bmi160_delay_ms(5);
	  /* Output data rate selection - power down.
	  if ( bmi160_set_gyro_output_data_rate(BMI160_GYRO_OUTPUT_DATA_RATE_100HZ) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }*/
	  if ( bmi160_gyro_mode(GYRO_MODE_SUSPEND) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  pComponentData->comboData->isEnabled = 0;
	  pComponentData->comboData->lastODR = 800.0f;
	  handle->isEnabled = 0;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Get_WhoAmI( DrvContextTypeDef *handle, us8 *who_am_i )
{

	  return BMI160_Get_WhoAmI(handle, who_am_i);
}

static DrvStatusTypeDef BMI160_G_Check_WhoAmI( DrvContextTypeDef *handle )
{

	  return BMI160_Check_WhoAmI(handle);
}

static DrvStatusTypeDef BMI160_G_Get_Axes( DrvContextTypeDef *handle, SensorAxes_gyro_t *angular_velocity )
{

	  struct bmi160_gyro_t gyro = {0,0,0};
	  float sensitivity = 0;

	  /* Get BMI160 actual sensitivity. */
	  if ( BMI160_G_Get_Sensitivity( handle, &sensitivity ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }
	  if ( bmi160_read_gyro_xyz(&gyro) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }
	  /* Calculate the data. */
	  angular_velocity->AXIS_X = ( gyro.x * sensitivity );
	  angular_velocity->AXIS_Y = ( gyro.y * sensitivity );
	  angular_velocity->AXIS_Z = ( gyro.z * sensitivity );
	  //printf("before gyro:AXIS_X =%d AXIS_Y = %d AXIS_Z = %d\n",angular_velocity->AXIS_X,angular_velocity->AXIS_Y,angular_velocity->AXIS_Z);
	  if(bmi160_offset.isGyroValid == 1)
	  {
		angular_velocity->AXIS_X = angular_velocity->AXIS_X - ((bmi160_offset.gyro_x) *(sensitivity));
		angular_velocity->AXIS_Y = angular_velocity->AXIS_Y - ((bmi160_offset.gyro_y) *(sensitivity));
		angular_velocity->AXIS_Z = angular_velocity->AXIS_Z - ((bmi160_offset.gyro_z) *(sensitivity));
		//printf("after gyro:AXIS_X =%d AXIS_Y = %d AXIS_Z = %d\n",angular_velocity->AXIS_X,angular_velocity->AXIS_Y,angular_velocity->AXIS_Z);
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Get_AxesRaw( DrvContextTypeDef *handle, SensorAxesRaw_t *value )
{

	  struct bmi160_gyro_t gyro = {0,0,0};


	  if ( bmi160_read_gyro_xyz(&gyro) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  /* Set the raw data. */
	  value->AXIS_X = gyro.x;
	  value->AXIS_Y = gyro.y;
	  value->AXIS_Z = gyro.z;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Get_Sensitivity( DrvContextTypeDef *handle, float *sensitivity )
{

	  us8 fullScale;

	  /* Read actual full scale selection from sensor. */
	  if ( bmi160_get_gyro_range( &fullScale ) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  /* Store the sensitivity based on actual full scale. */
	  switch( fullScale )
	  {
		case BMI160_GYRO_RANGE_125_DEG_SEC:
		  *sensitivity = ( float )BMI160_GYRO_SENSITIVITY_FOR_FS_125DPS;
		  break;
		case BMI160_GYRO_RANGE_250_DEG_SEC:
		  *sensitivity = ( float )BMI160_GYRO_SENSITIVITY_FOR_FS_250DPS;
		  break;
		case BMI160_GYRO_RANGE_500_DEG_SEC:
		  *sensitivity = ( float )BMI160_GYRO_SENSITIVITY_FOR_FS_500DPS;
		  break;
		case BMI160_GYRO_RANGE_1000_DEG_SEC:
		  *sensitivity = ( float )BMI160_GYRO_SENSITIVITY_FOR_FS_1000DPS;
		  break;
		case BMI160_GYRO_RANGE_2000_DEG_SEC:
		  *sensitivity = ( float )BMI160_GYRO_SENSITIVITY_FOR_FS_2000DPS;
		  break;
		default:
		  *sensitivity = -1.0f;
		  return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Get_ODR( DrvContextTypeDef *handle, float *odr )
{

	  us8 odr_low_level;

	  if (bmi160_get_gyro_output_data_rate(&odr_low_level) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  switch( odr_low_level )
	  {
		case BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED:
		  *odr =   0.0f;
		  break;
		case BMI160_GYRO_OUTPUT_DATA_RATE_25HZ:
		  *odr =  25.0f;
		  break;
		case BMI160_GYRO_OUTPUT_DATA_RATE_50HZ:
		  *odr =  50.0f;
		  break;
		case BMI160_GYRO_OUTPUT_DATA_RATE_100HZ:
		  *odr = 100.0f;
		  break;
		case BMI160_GYRO_OUTPUT_DATA_RATE_200HZ:
		  *odr = 200.0f;
		  break;
		case BMI160_GYRO_OUTPUT_DATA_RATE_400HZ:
		  *odr = 400.0f;
		  break;
		case BMI160_GYRO_OUTPUT_DATA_RATE_800HZ:
		  *odr = 800.0f;
		  break;
		case BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ:
		  *odr = 1600.0f;
		  break;
		case BMI160_GYRO_OUTPUT_DATA_RATE_3200HZ:
		  *odr = 3200.0f;
		  break;
		default:
		  *odr =  -1.0f;
		  return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Set_ODR( DrvContextTypeDef *handle, SensorOdr_t odr )
{

	  if(handle->isEnabled == 1)
	  {
		if(BMI160_G_Set_ODR_When_Enabled(handle, odr) == COMPONENT_ERROR)
		{
		  return COMPONENT_ERROR;
		}
	  }
	  else
	  {
		if(BMI160_G_Set_ODR_When_Disabled(handle, odr) == COMPONENT_ERROR)
		{
		  return COMPONENT_ERROR;
		}
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Set_ODR_Value( DrvContextTypeDef *handle, float odr )
{

	  if(handle->isEnabled == 1)
	  {
		if(BMI160_G_Set_ODR_Value_When_Enabled(handle, odr) == COMPONENT_ERROR)
		{
		  return COMPONENT_ERROR;
		}
	  }
	  else
	  {
		if(BMI160_G_Set_ODR_Value_When_Disabled(handle, odr) == COMPONENT_ERROR)
		{
		  return COMPONENT_ERROR;
		}
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Get_FS( DrvContextTypeDef *handle, float *fullScale )
{

	  us8 fs_low_level;

	  if ( bmi160_get_gyro_range( &fs_low_level ) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  switch( fs_low_level )
	  {
		case BMI160_GYRO_RANGE_125_DEG_SEC:
		  *fullScale =	125.0f;
		  break;
		case BMI160_GYRO_RANGE_250_DEG_SEC:
		  *fullScale =	250.0f;
		  break;
		case BMI160_GYRO_RANGE_500_DEG_SEC:
		  *fullScale =  500.0f;
		  break;
		case BMI160_GYRO_RANGE_1000_DEG_SEC:
		  *fullScale =	1000.0f;
		  break;
		case BMI160_GYRO_RANGE_2000_DEG_SEC:
		  *fullScale =  2000.0f;
		  break;
		default:
		  *fullScale =	 -1.0f;
		  return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Set_FS( DrvContextTypeDef *handle, SensorFs_t fullScale )
{

	  us8 new_fs;

	  switch( fullScale )
	  {
		case FS_LOW:
		  new_fs = BMI160_GYRO_RANGE_125_DEG_SEC;
		  break;
		case FS_MID:
		  new_fs = BMI160_GYRO_RANGE_500_DEG_SEC;
		  break;
		case FS_HIGH:
		  new_fs = BMI160_GYRO_RANGE_2000_DEG_SEC;
		  break;
		default:
		  return COMPONENT_ERROR;
	  }

	  if ( bmi160_set_gyro_range(new_fs) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Set_FS_Value( DrvContextTypeDef *handle, float fullScale )
{

	  us8 new_fs;

	  new_fs =   ( fullScale <= 125.0f ) ? BMI160_GYRO_RANGE_125_DEG_SEC
			   : ( fullScale <= 250.0f ) ? BMI160_GYRO_RANGE_250_DEG_SEC
			   : ( fullScale <= 500.0f ) ? BMI160_GYRO_RANGE_500_DEG_SEC
			   : ( fullScale <=1000.0f ) ? BMI160_GYRO_RANGE_1000_DEG_SEC
			   :						   BMI160_GYRO_RANGE_2000_DEG_SEC;

	  if ( bmi160_set_gyro_range(new_fs) != BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Get_Axes_Status( DrvContextTypeDef *handle, us8 *xyz_enabled )
{

		  us8 xyzStatus;

		  if ( bmi160_get_gyro_power_mode_stat(&xyzStatus) != BMI160_SUCCESS)
		  {
			return COMPONENT_ERROR;
		  }


		  xyz_enabled[0] = ( xyzStatus == GYRO_MODE_NORMAL ) ? 1 : 0;
		  xyz_enabled[1] = ( xyzStatus == GYRO_MODE_NORMAL ) ? 1 : 0;
		  xyz_enabled[2] = ( xyzStatus == GYRO_MODE_NORMAL ) ? 1 : 0;


		  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Set_Axes_Status( DrvContextTypeDef *handle, us8 *enable_xyz )
{
   us8 xyzStatus;

   if((enable_xyz[0]+enable_xyz[1]+enable_xyz[2]) == 0)
   {
          xyzStatus = GYRO_MODE_SUSPEND;
   }
   else
   {
	      xyzStatus = GYRO_MODE_NORMAL;
   }
   if (bmi160_gyro_mode(xyzStatus) != BMI160_SUCCESS)
   {
		  return COMPONENT_ERROR;
   }

   return COMPONENT_OK;
}
static DrvStatusTypeDef BMI160_Get_WhoAmI( DrvContextTypeDef *handle, us8 *who_am_i )
{
	/* Read CHIP ID register */
	if ( bmi160_read_reg(BMI160_USER_CHIP_ID_ADDR, who_am_i, 1) != BMI160_SUCCESS )
	{
		return COMPONENT_ERROR;
	}
	else
		printf("%s: read bmi160 chip id:0x%x\r\n", __func__, *who_am_i);

	return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_Check_WhoAmI( DrvContextTypeDef *handle )
{

	  us8 who_am_i = 0x00;

	  if ( BMI160_Get_WhoAmI( handle, &who_am_i ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }
	  if ( who_am_i != handle->who_am_i )
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_ODR_When_Enabled( DrvContextTypeDef *handle, SensorOdr_t odr )
{

	  us8 new_odr;

	  switch( odr )
	  {
		case ODR_LOW:
		  new_odr = BMI160_ACCEL_OUTPUT_DATA_RATE_0_78HZ;
		  break;
		case ODR_MID_LOW:
		  new_odr = BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ;
		  break;
		case ODR_MID:
		  new_odr = BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ;
		  break;
		case ODR_MID_HIGH:
		  new_odr = BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ;
		  break;
		case ODR_HIGH:
		  new_odr = BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ;
		  break;
		default:
		  return COMPONENT_ERROR;
	  }

	  if ( bmi160_set_accel_output_data_rate(new_odr,BMI160_ACCEL_NORMAL_AVG4) != BMI160_SUCCESS )
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_ODR_When_Disabled( DrvContextTypeDef *handle, SensorOdr_t odr )
{

	  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
	  BMI160_X_Data_t *pComponentData = ( BMI160_X_Data_t * )pData->pComponentData;

	  switch( odr )
	  {
		case ODR_LOW:
		  pComponentData->Previous_ODR = 0.78f;
		  break;
		case ODR_MID_LOW:
		  pComponentData->Previous_ODR = 100.0f;
		  break;
		case ODR_MID:
		  pComponentData->Previous_ODR = 200.0f;
		  break;
		case ODR_MID_HIGH:
		  pComponentData->Previous_ODR = 400.0f;
		  break;
		case ODR_HIGH:
		  pComponentData->Previous_ODR = 800.0f;
		  break;
		default:
		  return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_ODR_Value_When_Enabled( DrvContextTypeDef *handle, float odr )
{

	  us8 new_odr;

	  new_odr =  ( odr <=	0.78f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_0_78HZ
				: ( odr <=  1.56f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ
				: ( odr <=  3.12f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_3_12HZ
				: ( odr <=  6.25f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_6_25HZ
				: ( odr <=  12.5f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ
				: ( odr <=  25.0f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ
				: ( odr <=  50.0f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ
				: ( odr <= 100.0f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ
				: ( odr <= 200.0f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ
				: ( odr <= 400.0f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ
				: ( odr <= 800.0f ) ? BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ
				:					  BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ;

	  if ( bmi160_set_accel_output_data_rate(new_odr,BMI160_ACCEL_NORMAL_AVG4) != BMI160_SUCCESS )
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_X_Set_ODR_Value_When_Disabled( DrvContextTypeDef *handle, float odr )
{

	  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
	  BMI160_X_Data_t *pComponentData = ( BMI160_X_Data_t * )pData->pComponentData;

	  pComponentData->Previous_ODR =  ( odr <=  0.78f ) ? 0.78f
									 : ( odr <=  1.56f ) ? 1.56f
									 : ( odr <=  3.12f ) ? 3.12f
									 : ( odr <=  6.25f ) ? 6.25f
									 : ( odr <=  12.5f ) ? 12.5f
									 : ( odr <=  25.0f ) ? 25.0f
									 : ( odr <=  50.0f ) ? 50.0f
									 : ( odr <= 100.0f ) ? 100.0f
									 : ( odr <= 200.0f ) ? 200.0f
									 : ( odr <= 400.0f ) ? 400.0f
									 : ( odr <= 800.0f ) ? 800.0f
									 :					   1600.0f;

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Set_ODR_When_Enabled( DrvContextTypeDef *handle, SensorOdr_t odr )
{

	  us8  new_odr;
	  GYRO_Data_t *pData = ( GYRO_Data_t * )handle->pData;
	  BMI160_G_Data_t *pComponentData = ( BMI160_G_Data_t * )pData->pComponentData;

	  switch( odr )
	  {
		case ODR_LOW:
		  new_odr = BMI160_GYRO_OUTPUT_DATA_RATE_50HZ;
		  break;
		case ODR_MID_LOW:
		  new_odr = BMI160_GYRO_OUTPUT_DATA_RATE_100HZ;
		  break;
		case ODR_MID:
		  new_odr = BMI160_GYRO_OUTPUT_DATA_RATE_200HZ;
		  break;
		case ODR_MID_HIGH:
		  new_odr = BMI160_GYRO_OUTPUT_DATA_RATE_400HZ;
		  break;
		case ODR_HIGH:
		  new_odr = BMI160_GYRO_OUTPUT_DATA_RATE_800HZ;
		  break;
		default:
		  return COMPONENT_ERROR;
	  }

	  if ( bmi160_set_gyro_output_data_rate(new_odr) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  if(BMI160_G_Get_ODR( handle, &pComponentData->comboData->lastODR ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}


static DrvStatusTypeDef BMI160_G_Set_ODR_When_Disabled( DrvContextTypeDef *handle, SensorOdr_t odr )
{

	  GYRO_Data_t *pData = ( GYRO_Data_t * )handle->pData;
	  BMI160_G_Data_t *pComponentData = ( BMI160_G_Data_t * )pData->pComponentData;

	  switch( odr )
	  {
		case ODR_LOW:
		  pComponentData->Previous_ODR = 50.0f;
		  pComponentData->comboData->lastODR = 50.0f;
		  break;
		case ODR_MID_LOW:
		  pComponentData->Previous_ODR = 100.0f;
		  pComponentData->comboData->lastODR = 100.0f;
		  break;
		case ODR_MID:
		  pComponentData->Previous_ODR = 200.0f;
		  pComponentData->comboData->lastODR = 200.0f;
		  break;
		case ODR_MID_HIGH:
		  pComponentData->Previous_ODR = 400.0f;
		  pComponentData->comboData->lastODR = 400.0f;
		  break;
		case ODR_HIGH:
		  pComponentData->Previous_ODR = 800.0f;
		  pComponentData->comboData->lastODR = 800.0f;
		  break;
		default:
		  return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Set_ODR_Value_When_Enabled( DrvContextTypeDef *handle, float odr )
{

	  us8 new_odr;
	  GYRO_Data_t *pData = ( GYRO_Data_t * )handle->pData;
	  BMI160_G_Data_t *pComponentData = ( BMI160_G_Data_t * )pData->pComponentData;

	  new_odr = ( odr <=  0.0f ) ? BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
				: ( odr <=	25.0f ) ? BMI160_GYRO_OUTPUT_DATA_RATE_25HZ
				: ( odr <= 50.0f ) ? BMI160_GYRO_OUTPUT_DATA_RATE_50HZ
				: ( odr <= 100.0f ) ? BMI160_GYRO_OUTPUT_DATA_RATE_100HZ
				: ( odr <= 200.0f ) ? BMI160_GYRO_OUTPUT_DATA_RATE_200HZ
				: ( odr <= 400.0f ) ? BMI160_GYRO_OUTPUT_DATA_RATE_400HZ
				: ( odr <= 800.0f ) ? BMI160_GYRO_OUTPUT_DATA_RATE_800HZ
				: ( odr <= 1600.0f ) ? BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ
				:					  BMI160_GYRO_OUTPUT_DATA_RATE_3200HZ;

	  if ( bmi160_set_gyro_output_data_rate(new_odr) !=BMI160_SUCCESS)
	  {
		return COMPONENT_ERROR;
	  }

	  if(BMI160_G_Get_ODR( handle, &pComponentData->comboData->lastODR ) == COMPONENT_ERROR )
	  {
		return COMPONENT_ERROR;
	  }

	  return COMPONENT_OK;
}

static DrvStatusTypeDef BMI160_G_Set_ODR_Value_When_Disabled( DrvContextTypeDef *handle, float odr )
{

	  GYRO_Data_t *pData = ( GYRO_Data_t * )handle->pData;
	  BMI160_G_Data_t *pComponentData = ( BMI160_G_Data_t * )pData->pComponentData;

	  pComponentData->Previous_ODR = ( odr <=  0.0f ) ? 0.0f
									 : ( odr <=  25.0f ) ? 25.0f
									 : ( odr <= 50.0f ) ? 50.0f
									 : ( odr <= 100.0f ) ? 100.0f
									 : ( odr <= 200.0f ) ? 200.0f
									 : ( odr <= 400.0f ) ? 400.0f
									 : ( odr <= 800.0f ) ? 800.0f
									 : ( odr <= 1600.0f ) ? 1600.0f
									 :					   3200.0f;

	  pComponentData->comboData->lastODR =  ( odr <=  0.0f ) ? 0.0f
									 			: ( odr <=  25.0f ) ? 25.0f
									 			: ( odr <= 50.0f ) ? 50.0f
									 			: ( odr <= 100.0f ) ? 100.0f
									 			: ( odr <= 200.0f ) ? 200.0f
									 			: ( odr <= 400.0f ) ? 400.0f
									 			: ( odr <= 800.0f ) ? 800.0f
									 			: ( odr <= 1600.0f ) ? 1600.0f
									 			:					   3200.0f;

	  return COMPONENT_OK;
}

char BMI160_INIT_SENSOR_API(void)
{
    char com_rslt = 0;
	bmi160_offset.isAccValid = 0;
	bmi160_offset.isGyroValid = 0;
	com_rslt  = bmi160_init_sensor();
	com_rslt += bmi160_init_gpio();
	//bmi160_postinit();
	com_rslt += getBmi160OffsetValue((void *)(&bmi160_offset));//read accel and gyro offset value
	com_rslt += bmi160_set_intr_data_rdy(BMI160_INTR2_MAP_DATA_RDY,BMI160_ENABLE);//data ready intr ---> GYRO_S_INT(PB0)
	//com_rslt += bmi160_init_fifo();
	//bmi160_delay_ms(20);
	//com_rslt += bmi160_set_intr_fifo_wm(BMI160_INTR1_MAP_DATA_RDY, BMI160_ENABLE);
	bmi160_delay_ms(20);
	return com_rslt;
}

BMI160_RETURN_FUNCTION_TYPE bmi160_init_fifo(void)
{
	char com_rslt = 0;
	us8  init_value = 0;
	com_rslt  = bmi160_write_reg(BMI160_USER_FIFO_CONFIG_0_ADDR,&init_value,1);
	bmi160_delay_ms(5);
	com_rslt += bmi160_write_reg(BMI160_USER_FIFO_CONFIG_1_ADDR,&init_value,1);
	bmi160_delay_ms(5);
	com_rslt += bmi160_write_reg(BMI160_USER_FIFO_DOWN_ADDR,&init_value,1);
	bmi160_delay_ms(5);
	com_rslt += bmi160_set_fifo_wm(0x5A);
	bmi160_delay_ms(5);
	com_rslt += bmi160_set_fifo_header_enable(BMI160_ENABLE);
	return com_rslt;

}

/*!
 *	@brief This function used for interrupt configuration
 *
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval 1 -> Error
 *
 *
 */
BMI160_RETURN_FUNCTION_TYPE bmi160_init_gpio(void)
{
	/* This variable used for provide the communication
	results*/
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 init_value = 0;
	struct bmi160_t *bmi160_temp_p;
        /* Configure the in/out control of interrupt*/
	bmi160_temp_p = bmi160_get_ptr();
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);
    init_value = 0xBB;
	com_rslt = bmi160_write_reg(BMI160_USER_INTR_OUT_CTRL_ADDR,&init_value,1);
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);
	init_value = 0x01;
	com_rslt += bmi160_write_reg(BMI160_USER_INTR_LATCH_ADDR,&init_value,1);
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);

	init_value = 0x00;//init disable
	com_rslt += bmi160_write_reg(0x50,&init_value,1);
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);
	init_value = 0x70;
	com_rslt += bmi160_write_reg(0x51,&init_value,1);
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);
	init_value = 0x00;
	com_rslt += bmi160_write_reg(0x52,&init_value,1);
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);

	init_value = 0x00;//map init 0x0(0x55-0x57)
	com_rslt += bmi160_write_reg(0x55,&init_value,1);
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);
	com_rslt += bmi160_write_reg(0x56,&init_value,1);
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);
	com_rslt += bmi160_write_reg(0x57,&init_value,1);
	bmi160_temp_p->delay_msec(BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY);

	return com_rslt;
}

/*
#define ACCEL_MODE_NORMAL	(0x11)
#define	ACCEL_LOWPOWER		(0X12)
#define	ACCEL_SUSPEND		(0X10)
*/

BMI160_RETURN_FUNCTION_TYPE bmi160_accel_mode(us8 accel_power_mode)
{
    BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
    com_rslt = bmi160_set_command_register(accel_power_mode);
    return com_rslt;
}
/*
#define GYRO_MODE_SUSPEND		(0x14)
#define GYRO_MODE_NORMAL		(0x15)
#define GYRO_MODE_FASTSTARTUP	(0x17)
*/
BMI160_RETURN_FUNCTION_TYPE bmi160_gyro_mode(us8 gyro_power_mode)
{
    BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
    com_rslt = bmi160_set_command_register(gyro_power_mode);
    return com_rslt;
}


BMI160_RETURN_FUNCTION_TYPE bmi160_fifo_flush(void)
{
    BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
    com_rslt = bmi160_set_command_register(0xB0);
	bmi160_delay_ms(3);
    return com_rslt;
}

BMI160_RETURN_FUNCTION_TYPE bmi160_int_reset(void)
{
    BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
    com_rslt = bmi160_set_command_register(0xB1);
    return com_rslt;
}

BMI160_RETURN_FUNCTION_TYPE bmi160_softreset(void)
{
    BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
    com_rslt = bmi160_set_command_register(0xB6);
    bmi160_delay_ms(2);
    BMI160_CS_LOW();
    bmi160_delay_ms(2);
    BMI160_CS_HIGH();
    bmi160_delay_ms(2);
    BMI160_CS_LOW();
    bmi160_delay_ms(2);
    return com_rslt;
}
void bmi160_get_accel_rawdata(_command_source source)
{
	SensorAxes_t acceleration;
	uint8_t buffer[64];
	int j = 0;

	getBmi160OffsetValue((void *)(&bmi160_offset));
	if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
	{
		/* if(source == CMD_SOURCE_USBCDC) */
			/* usb_app_transmit_data("fail\r\n",6); */
		/* else */
			MFG_LOG_ERR( "fail\n");
	}

	BMI160_X_Get_Axes(NULL, &acceleration );
	memset(buffer,0,64);
	 j = 0;
	 j  = sprintf( (char *)buffer,       "x=%dmg\t", acceleration.AXIS_X); // C4996
       j += sprintf( (char *)buffer + j, "y=%dmg\t", acceleration.AXIS_Y); // C4996
       j += sprintf( (char *)buffer + j, "z=%dmg\t", acceleration.AXIS_Z); // C4996
       j += sprintf( (char *)buffer + j, "\r\n"); // C4996

	  /* if(source == CMD_SOURCE_USBCDC) */
        	/* usb_app_transmit_data(buffer,strlen((char const*)buffer)); */
	  /* else */
	 	gsensor_info("accel.x = %dmg accel.y = %dmg accel.z = %dmg\n",
			acceleration.AXIS_X,acceleration.AXIS_Y,acceleration.AXIS_Z);
}
void bmi160_get_gyro_rawdata(_command_source source)
{
	SensorAxes_gyro_t angular_velocity;
	uint8_t buffer[64];
	int j = 0;

	getBmi160OffsetValue((void *)(&bmi160_offset));
	if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
	{
		/* if(source == CMD_SOURCE_USBCDC) */
			/* usb_app_transmit_data("fail\r\n",6); */
		/* else */
			MFG_LOG_ERR( "fail\n");
	}

	BMI160_G_Get_Axes( NULL, &angular_velocity );
	memset(buffer,0,64);
	 j = 0;
	 j  = sprintf( (char *)buffer,       "x=%.2fmdps\t", angular_velocity.AXIS_X); // C4996
       j += sprintf( (char *)buffer + j, "y=%.2fmdps\t", angular_velocity.AXIS_Y); // C4996
       j += sprintf( (char *)buffer + j, "z=%.2fmdps\t", angular_velocity.AXIS_Z); // C4996
	j += sprintf( (char *)buffer + j, "\r\n");

	 /* if(source == CMD_SOURCE_USBCDC) */
	 	/* usb_app_transmit_data(buffer,strlen((char const*)buffer)); */
	 /* else */
	 	gsensor_info("gyro.x = %.2fmdps gyro.y = %.2fmdps gyro.z = %.2fmdps\n",
			angular_velocity.AXIS_X,angular_velocity.AXIS_Y,angular_velocity.AXIS_Z);
}
void bmi160_get_kvalue(void)
{
	BMI160_DataTypeDef bmi160_offset_tmp = {0};
	getBmi160OffsetValue((void *)(&bmi160_offset_tmp));
	if((bmi160_offset_tmp.isAccValid == 1) && (bmi160_offset_tmp.isGyroValid == 1))
	{
	 gsensor_info("flash offset value:isAccValid=%d accel_x=%f accel_y=%f accel_y=%f isGyroValid=%d gyro_x =%d gyro_y=%d gyro_z=%d\n",
		bmi160_offset_tmp.isAccValid,bmi160_offset_tmp.accel_x,bmi160_offset_tmp.accel_y,bmi160_offset_tmp.accel_z,
		bmi160_offset_tmp.isGyroValid,bmi160_offset_tmp.gyro_x,bmi160_offset_tmp.gyro_y,bmi160_offset_tmp.gyro_z);
	}
	else
	{
	 gsensor_info("bmi160 has not calibrated/self-tseted.\n");
	}
}
int system_read_bmi160_chipid(void)
{
	struct bmi160_t *s_bmi160_tmp = NULL;
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	s_bmi160_tmp = bmi160_get_ptr();
	if(s_bmi160_tmp == NULL)
	{
		gsensor_err("can not get chip id!\n");
		return com_rslt;
	}
	if(s_bmi160_tmp->chip_id == 0xD1)
	{
		gsensor_info("bmi160:g-sensor chip_id = 0x%x\n",s_bmi160_tmp->chip_id);
	}
	else
	{
		gsensor_err("can not get chip id!\n");
	}

	return 0;
}
int test_sensor_data(void)
{
	//BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	struct bmi160_gyro_t gyro;
	struct bmi160_accel_t accel;
	int i = 0;
	int gyro_x = 0,gyro_y = 0,gyro_z = 0;
	int accel_x = 0,accel_y = 0,accel_z = 0;
	for(i=0;i<20;i++)
	{
		bmi160_read_gyro_xyz(&gyro);
		gyro_x += gyro.x;
		gyro_y += gyro.y;
		gyro_z += gyro.z;
		HAL_Delay(1);
		bmi160_read_accel_xyz(&accel);
		accel_x += accel.x;
		accel_y += accel.y;
		accel_z += accel.z;
		HAL_Delay(1);
	}
	if((gyro_x == 0) && (gyro_y == 0) && (gyro_z == 0))
	{
		gsensor_err("bmi160:can not get sensor data.\n");
		return -1;
	}
	if((accel_x == 0) && (accel_y == 0) && (accel_z == 0))
	{
		gsensor_err("bmi160:can not get sensor data.\n");
		return -1;
	}
	gsensor_info("bmi160:can get sensor data.\n");
	return 0;
}
/**************************************************************/
/*    					MFG command									  */
/*************************************************************/

BMI160_RETURN_FUNCTION_TYPE bmi160_selftest_mode(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	struct bmi160_t *s_bmi160_tmp = NULL;

	s_bmi160_tmp = bmi160_get_ptr();
	if(s_bmi160_tmp == NULL)
	{
		return com_rslt;
	}
	do
	{   if(s_bmi160_tmp->chip_id != 0xD1)
		{
			com_rslt = BMI160_ERROR;
			break;
		}
		/*Set the accel mode as Normal write in the register 0x7E*/
		com_rslt = bmi160_set_command_register(ACCEL_MODE_NORMAL);
		if (com_rslt != BMI160_SUCCESS)
			break ;
		s_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
		/*Set the gyro mode as Normal write in the register 0x7E*/
		com_rslt = bmi160_set_command_register(GYRO_MODE_NORMAL);
		if (com_rslt != BMI160_SUCCESS)
			break ;
		s_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
		/* Set the accel range as 8G */
		com_rslt = bmi160_set_accel_range(C_BMI160_EIGHT_U8X);
		if (com_rslt != BMI160_SUCCESS)
			 break ;
		s_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
		/* Set the accel bandwidth as OSRS4 */
		com_rslt = bmi160_set_accel_bw(BMI160_ACCEL_NORMAL_AVG4);
		if (com_rslt != BMI160_SUCCESS)
			break ;
		s_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
		/* Set the gryo bandwidth as OSR4 */
		com_rslt = bmi160_set_gyro_bw(BMI160_GYRO_NORMAL_MODE);
		if (com_rslt != BMI160_SUCCESS)
			break ;
		s_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
		/* set gyro data rate as 1600Hz*/
		com_rslt = bmi160_set_gyro_output_data_rate(BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ);
		if (com_rslt != BMI160_SUCCESS)
			break ;
		s_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
		 /* set accel data rate as 1600Hz*/
		com_rslt = bmi160_set_accel_output_data_rate(BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ,BMI160_ACCEL_NORMAL_AVG4);
		if (com_rslt != BMI160_SUCCESS)
			break ;
		s_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
	}while(0);

	return com_rslt ;
}

#define ACCGYROTESTCOUNT 350
#define GYROTESTBASE_VALUE 75 //DPS
BMI160_RETURN_FUNCTION_TYPE bmi160_Acc_selftest(_command_source source)
{
    BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
    float pAcc[3], nAcc[3], dAcc[3] ;
    struct bmi160_t *p_bmi160_tmp = NULL;
    SensorAxes_t pacc,nacc;
    //int i =0;
    //us8 tmp = 0x0;
    	printf("accel selftest start,please wait 8 second ....\n");
    bmi160_delay_ms(2000);
    if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
    {
	 	MFG_LOG_ERR("fail\n");
	 return com_rslt;
    }
    p_bmi160_tmp = bmi160_get_ptr();
    if(p_bmi160_tmp == NULL)
    {
		MFG_LOG_ERR("fail\n");
	return com_rslt;
    }
#if 0
    for(i =0; i < ACCGYROTESTCOUNT;i++)
    {
    	bmi160_delay_ms(2);
	com_rslt = BMI160_X_Get_Axes(NULL, &pacc);
       if (com_rslt != BMI160_SUCCESS)
       {
		MFG_LOG_ERR("fail\n");
		usb_app_transmit_data("fail\n",5);
        	return -1;
        }
		if(((-200)<pacc.AXIS_X) && (pacc.AXIS_X < 200) &&
		    ((-200)<pacc.AXIS_Y) && (pacc.AXIS_Y < 200) )
		{
			if(pacc.AXIS_Z<0)
			{
				if(((-1200) < pacc.AXIS_Z) &&  (pacc.AXIS_Z < (-800)))
				{}
				else
				{
					usb_app_transmit_data("fail\n",5);
					MFG_LOG_ERR("fail\n");
					return -1;
				}
			}
			else
			{
				if(((800) < pacc.AXIS_Z) &&  (pacc.AXIS_Z < (1200)))
				{}
				else
				{
					usb_app_transmit_data("fail\n",5);
					MFG_LOG_ERR("fail\n");
					return -1;
				}
			}
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		       MFG_LOG_ERR("fail\n");
			return -1;

		}

    }
#endif
    do
    {
        com_rslt = bmi160_selftest_mode();
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR("fail\n");
            break;
        }
       p_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
        /*
         *  Positive test
         */
        com_rslt = bmi160_set_accel_selftest_sign(C_BMI160_ONE_U8X);
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR("fail\n");
            break;
        }
        //Set high amp
        com_rslt = bmi160_set_accel_selftest_amp(C_BMI160_ONE_U8X);
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR("fail\n");
            break;
        }
        //Start selftest
        com_rslt = bmi160_set_accel_selftest_axis(C_BMI160_ONE_U8X);
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR("fail\n");
            break;
        }
        //Wait 60 msec for testing
        p_bmi160_tmp->delay_msec(C_BMI160_HUNDRED_U8X);
        //Get result
        com_rslt = BMI160_X_Get_Axes(NULL, &pacc);
        if (com_rslt != BMI160_SUCCESS)
        {
			MFG_LOG_ERR("fail\n");
            break;
        }
        else
        {
        	//MFG_LOG_DEBUG( "Positive : X:(%d), Y:(%d), Z:(%d)\n", pacc.AXIS_X, pacc.AXIS_Y, pacc.AXIS_Z);
        }
        /*
         *  Negative test
         */
        com_rslt = bmi160_set_accel_selftest_sign(C_BMI160_ZERO_U8X);
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR("fail\n");
            break;
        }
        //Set high amp
        com_rslt = bmi160_set_accel_selftest_amp(C_BMI160_ONE_U8X);
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR("fail\n");
            break;
        }
        //Start selftest
        com_rslt = bmi160_set_accel_selftest_axis(C_BMI160_ONE_U8X);
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR("fail\n");
            break;
        }
        //Wait 60 msec for testing
        p_bmi160_tmp->delay_msec(C_BMI160_HUNDRED_U8X);
        //Get result
        com_rslt = BMI160_X_Get_Axes(NULL, &nacc);
        if (com_rslt != BMI160_SUCCESS)
        {
            		MFG_LOG_ERR(" fail\n");
            break;
        }
        else
        {
            //MFG_LOG_DEBUG( "Positive : X:(%d), Y:(%d), Z:(%d)\n", nacc.AXIS_X, nacc.AXIS_Y, nacc.AXIS_Z);
        }
        //
        pAcc[0] = (float)pacc.AXIS_X;
		pAcc[1] = (float)pacc.AXIS_Y;
		pAcc[2] = (float)pacc.AXIS_Z;

		nAcc[0] = (float)nacc.AXIS_X;
		nAcc[1] = (float)nacc.AXIS_Y;
		nAcc[2] = (float)nacc.AXIS_Z;

        dAcc[0] = (float)fabs(pAcc[0] - nAcc[0]) ;
        dAcc[1] = (float)fabs(pAcc[1] - nAcc[1]) ;
        dAcc[2] = (float)fabs(pAcc[2] - nAcc[2]) ;


	 if(bmi160_softreset() !=BMI160_SUCCESS)
	 {
	    	MFG_LOG_ERR("fail\n");
	    break;

	 }
	  bmi160_delay_ms(3000);
	  if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
        {
	   	 MFG_LOG_ERR("fail\n");
	    break;
        }
        //xlog (LOG_TAG, BMI160_LOG_LEVEL_OFF, "Difference : X:(%3.3f), Y:(%3.3f), Z:(%3.3f)\n", dAcc[0], dAcc[1], dAcc[2]);
        if ( dAcc[0] >= 800 && dAcc[1] >= 800 && dAcc[2] >= 400 )
        {
        	saveAccSelfTestValue();//save result
        		MFG_LOG_PASS("pass\n");
			//MFG_LOG_INFO("ABS(X):(%7.3f) >= 7.84\n", dAcc[0]);
			//MFG_LOG_INFO("ABS(Y):(%7.3f) >= 7.84\n", dAcc[1]);
			//MFG_LOG_INFO("ABS(Z):(%7.3f) >= 3.92\n", dAcc[2]);
            com_rslt = BMI160_SUCCESS;
        }
        else
        {
            		MFG_LOG_ERR( "fail\n");
            //MFG_LOG_INFO("ABS(X):(%7.3f) >= 7.84\n", dAcc[0]);
			//MFG_LOG_INFO("ABS(Y):(%7.3f) >= 7.84\n", dAcc[1]);
			//MFG_LOG_INFO("ABS(Z):(%7.3f) >= 3.92\n", dAcc[2]);
            com_rslt = BMI160_ERROR ;
        }
    }while(0);

    return com_rslt ;
}


BMI160_RETURN_FUNCTION_TYPE bmi160_Gyro_selftest(_command_source source)
{
    BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
    us8 u_SelfTest = 0 ;
     //SensorAxes_gyro_t gyrodata_tmp;
	struct bmi160_t *p_bmi160_tmp = NULL;
	//int i =0 ;


	if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
	{
			MFG_LOG_ERR( "fail\n");
	}
	p_bmi160_tmp = bmi160_get_ptr();
	if(p_bmi160_tmp == NULL)
	{
			MFG_LOG_ERR( "fail\n");
		return com_rslt;
	}
#if 0
	for(i =0; i < ACCGYROTESTCOUNT;i++)
	{
		 bmi160_delay_ms(2);
		 com_rslt = BMI160_G_Get_Axes( NULL,&gyrodata_tmp );
		 if (com_rslt != BMI160_SUCCESS)
       	{
       		usb_app_transmit_data("fail\n",5);
			MFG_LOG_ERR("fail\n");
        		return BMI160_ERROR;
        	}
		  if(((-GYROTESTBASE_VALUE)<gyrodata_tmp.AXIS_X) &&(gyrodata_tmp.AXIS_X<GYROTESTBASE_VALUE) && \
		   	  ((-GYROTESTBASE_VALUE)<gyrodata_tmp.AXIS_Y) &&(gyrodata_tmp.AXIS_Y<GYROTESTBASE_VALUE) && \
		   	  ((-GYROTESTBASE_VALUE)<gyrodata_tmp.AXIS_Z) &&(gyrodata_tmp.AXIS_Z<GYROTESTBASE_VALUE)    )
		   {

		   }else
		   {
		   		usb_app_transmit_data("fail\n",5);
		   		MFG_LOG_ERR("fail\n");
				return BMI160_ERROR;
		   }
	}
#endif
    do
    {
    	com_rslt = bmi160_selftest_mode();
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR( "fail\n");
            break;
        }
        p_bmi160_tmp->delay_msec(C_BMI160_ONE_U8X);/* bmi160_delay_ms in ms*/
        com_rslt = bmi160_set_gyro_selftest_start(C_BMI160_ONE_U8X);
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR( "fail\n");
            break;
        }
        p_bmi160_tmp->delay_msec(C_BMI160_HUNDRED_U8X);/* bmi160_delay_ms in ms*/
        com_rslt = bmi160_get_gyro_selftest(&u_SelfTest);
        if (com_rslt != BMI160_SUCCESS)
        {
        		MFG_LOG_ERR( "fail\n");
            break;
        }
    }while(0);

    bmi160_delay_ms(10);
    if (u_SelfTest == C_BMI160_ONE_U8X)
    {
    	saveGyroSelfTestValue();
       	MFG_LOG_PASS( "pass\n");
       com_rslt = BMI160_SUCCESS ;
    }
    else
    {
       	MFG_LOG_ERR( "fail\n");
        com_rslt = BMI160_ERROR;
    }

    return com_rslt ;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_read_chipid(_command_source source)
{
	struct bmi160_t *s_bmi160_tmp = NULL;
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;

	if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
	{
			MFG_LOG_ERR( "can not get chip id!\n");
	}
	s_bmi160_tmp = bmi160_get_ptr();
	if(s_bmi160_tmp == NULL)
	{
			MFG_LOG_ERR( "can not get chip id!\n");
		return com_rslt;
	}
	if(s_bmi160_tmp->chip_id == 0xD1)
	{
			MFG_LOG_PASS("pass chip_id = 0x%x\n",s_bmi160_tmp->chip_id);
	}
	else
	{
			MFG_LOG_ERR( "can not get chip id!\n");
	}
	return 0;
}

int cmd_gsensor_check(char cmdsource)
{

	BMI160_DataTypeDef bmi160_offset_tmp = {0};
	uint32_t selftestvalue = 0;
	uint8_t buffer[128];
	getBmi160OffsetValue((void *)(&bmi160_offset_tmp));
	if(bmi160_offset_tmp.isAccValid == 1)
	{
		selftestvalue = getAccCalheaderValue();
		if(selftestvalue == 0x67676767)
		{
			memset(buffer,'\0',128);
			sprintf( (char *)buffer,"Acc [calibrated pass, x = %f, y = %f, z = %f]\n",
					bmi160_offset_tmp.accel_x, bmi160_offset_tmp.accel_y,bmi160_offset_tmp.accel_z); // C4996
			{
				MFG_LOG_INFO("%s", buffer);
			}
		}
		else
		{
			memset(buffer,'\0',128);
			sprintf( (char *)buffer,"Acc [calibrated fail]\r\n"); // C4996
			{
				MFG_LOG_INFO("%s", buffer);
			}
		}
	}
	else
	{
		memset(buffer,'\0',128);
		sprintf( (char *)buffer,"Acc [has not calibrated]\r\n"); // C4996
		{
			MFG_LOG_INFO("%s", buffer);
		}
	}
	selftestvalue = getAccSelfTestValue();
	if(selftestvalue == 0x67676767)
	{
		memset(buffer,'\0',128);
		sprintf( (char *)buffer,"Acc [self-tseted pass]\r\n"); // C4996
		{
			MFG_LOG_INFO("%s", buffer);
		}
	}
	else
	{
		memset(buffer,'\0',128);
		sprintf( (char *)buffer,"Acc [has not self-tseted]\r\n"); // C4996
		{
			MFG_LOG_INFO("%s", buffer);
		}
	}


    return 0 ;
}
int cmd_gyro_check(char cmdsource)
{

	BMI160_DataTypeDef bmi160_offset_tmp = {0};
	uint32_t selftestvalue = 0;
	uint8_t buffer[128];
	getBmi160OffsetValue((void *)(&bmi160_offset_tmp));
	if(bmi160_offset_tmp.isAccValid == 1)
	{
		memset(buffer,'\0',128);
		sprintf( (char *)buffer,"Gyro [calibrated pass, x = %d, y = %d, z = %d]\n",
					bmi160_offset_tmp.gyro_x, bmi160_offset_tmp.gyro_y,bmi160_offset_tmp.gyro_z); // C4996
		{
			MFG_LOG_INFO("%s", buffer);
		}
	}

	selftestvalue = getGyroSelfTestValue();
	if(selftestvalue == 0x67676767)
	{
		memset(buffer,'\0',128);
		sprintf( (char *)buffer,"Gyro [self-tseted pass]\r\n");// C4996
		{
			MFG_LOG_INFO("%s", buffer);
		}
	}
	else
	{
		memset(buffer,'\0',128);
		sprintf( (char *)buffer,"Gyro [has not self-tseted]\r\n");// C4996
		{
			MFG_LOG_INFO("%s", buffer);
		}
	}


    return 0 ;
}


#define accel_test_count 3000
#define GYRO_BASE_VALUE  5.5  //dps
#define ACCEL_BASE_VALUE 40  //mg
#define ACCEL_1000MG     1000 //mg

BMI160_RETURN_FUNCTION_TYPE  bmi160_test_gyro(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_gyro_power_mode = 0;
	int i = 0;
	SensorAxes_gyro_t gyro_tmp = {0};
	int gyro_flag  = 0;
	uint8_t buffer[128];

	gyro_flag  = 0;
	com_rslt =  bmi160_get_gyro_power_mode_stat(&v_gyro_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_gyro_power_mode == 1))
	{
		bmi160_offset.isGyroValid = 0;
		getBmi160OffsetValue((void *)(&bmi160_offset));
		if(bmi160_offset.isGyroValid != 1)
	 	{
			MFG_LOG_INFO("Gyro [has not  calibration]\r\n");
			return BMI160_ERROR;
	  	}
	    for(i = 0 ; i < accel_test_count ; i++)
	        {
	           if(BMI160_G_Get_Axes( NULL,&gyro_tmp) != 0)
	           {
					MFG_LOG_ERR("bmi160 gyro calibration fail\n");
					clearGyroValue();
					return BMI160_ERROR;
			   }
			   //2000 dps  +/- 5.5 dps
			   if(((-GYRO_BASE_VALUE)<gyro_tmp.AXIS_X) &&(gyro_tmp.AXIS_X<GYRO_BASE_VALUE) && \
			   	  ((-GYRO_BASE_VALUE)<gyro_tmp.AXIS_Y) &&(gyro_tmp.AXIS_Y<GYRO_BASE_VALUE) && \
			   	  ((-GYRO_BASE_VALUE)<gyro_tmp.AXIS_Z) &&(gyro_tmp.AXIS_Z<GYRO_BASE_VALUE)    )
			   {
					;//MFG_LOG_ERR("bmi160 gyro calibration success\n");
			   }else
			   {
			   		//MFG_LOG_ERR("bmi160 gyro calibration fail\n");
					//clearGyroValue();
					//return BMI160_ERROR;
					gyro_flag++;
					memset(buffer,'\0',128);
		 			sprintf( (char *)buffer,"x = %.3f dps\ty =%.3f dps\tz=%.3f dps\r\n", gyro_tmp.AXIS_X, gyro_tmp.AXIS_Y, gyro_tmp.AXIS_Z); // C4996
		 			printf("%s",buffer);
					break;
			   }
	    	}
	}
	else
	{
		MFG_LOG_ERR("bmi160 gyro calibration fail\n");
		clearGyroValue();
		return com_rslt;
	}
	if(gyro_flag != 0)
	{
		clearGyroValue();
		return BMI160_ERROR;
	}
	return com_rslt;

}
#if 0
BMI160_RETURN_FUNCTION_TYPE  bmi160_test_accel_xup(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0;
	SensorAxes_t accel_tmp = {0};

	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
		bmi160_offset.isAccValid = 0;
		getBmi160OffsetValue((void *)(&bmi160_offset));
		if(bmi160_offset.isAccValid != 1)
	 	{
			if(getAccSelfTestValue() == 0x67676767)
			{
				MFG_LOG_INFO("Acc [has not calibrated]\r\n");
			}
			else
			{
				MFG_LOG_INFO("Acc [has not self-tested]\r\n");
			}
			return BMI160_ERROR;
	  	}
	    for(i = 0 ; i < accel_test_count ; i++)
        {
           if(BMI160_X_Get_Axes( NULL,&accel_tmp) != 0)
           {
           		MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
		   if(((-(ACCEL_1000MG + ACCEL_BASE_VALUE))<accel_tmp.AXIS_X) && (accel_tmp.AXIS_X<(-(ACCEL_1000MG - ACCEL_BASE_VALUE))) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Y)) && (accel_tmp.AXIS_Y<ACCEL_BASE_VALUE) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Z)) && (accel_tmp.AXIS_Z<ACCEL_BASE_VALUE)    )
		   {
				//MFG_LOG_ERR("bmi160 accel calibration success\n");
		   }
		   else
		   {
				MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
    	}
	}
	else
	{
		MFG_LOG_ERR("bmi160 accel calibration fail\n");
		clearAccValue();
		return com_rslt;
	}
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE  bmi160_test_accel_xdown(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0;
	SensorAxes_t accel_tmp = {0};

	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
		bmi160_offset.isAccValid = 0;
		getBmi160OffsetValue((void *)(&bmi160_offset));
		if(bmi160_offset.isAccValid != 1)
	 	{
			if(getAccSelfTestValue() == 0x67676767)
			{
				MFG_LOG_INFO("Acc [has not calibrated]\r\n");
			}
			else
			{
				MFG_LOG_INFO("Acc [has not self-tested]\r\n");
			}
			return BMI160_ERROR;
	  	}
	    for(i = 0 ; i < accel_test_count ; i++)
        {
           if(BMI160_X_Get_Axes( NULL,&accel_tmp) != 0)
           {
           		MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
		   if((((ACCEL_1000MG - ACCEL_BASE_VALUE))<accel_tmp.AXIS_X) && (accel_tmp.AXIS_X<((ACCEL_1000MG + ACCEL_BASE_VALUE))) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Y)) && (accel_tmp.AXIS_Y<ACCEL_BASE_VALUE) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Z)) && (accel_tmp.AXIS_Z<ACCEL_BASE_VALUE)    )
		   {
				//MFG_LOG_ERR("bmi160 accel calibration success\n");
		   }
		   else
		   {
				MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
    	}
	}
	else
	{
		MFG_LOG_ERR("bmi160 accel calibration fail\n");
		clearAccValue();
		return com_rslt;
	}
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE  bmi160_test_accel_yup(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0;
	SensorAxes_t accel_tmp = {0};

	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
		bmi160_offset.isAccValid = 0;
		getBmi160OffsetValue((void *)(&bmi160_offset));
		if(bmi160_offset.isAccValid != 1)
	 	{
			if(getAccSelfTestValue() == 0x67676767)
			{
				MFG_LOG_INFO("Acc [has not calibrated]\r\n");
			}
			else
			{
				MFG_LOG_INFO("Acc [has not self-tested]\r\n");
			}
			return BMI160_ERROR;
	  	}
	    for(i = 0 ; i < accel_test_count ; i++)
        {
           if(BMI160_X_Get_Axes( NULL,&accel_tmp) != 0)
           {
           		MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
		   if(((-(ACCEL_1000MG + ACCEL_BASE_VALUE))<accel_tmp.AXIS_Y) && (accel_tmp.AXIS_Y<(-(ACCEL_1000MG - ACCEL_BASE_VALUE))) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_X)) && (accel_tmp.AXIS_X<ACCEL_BASE_VALUE) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Z)) && (accel_tmp.AXIS_Z<ACCEL_BASE_VALUE)    )
		   {
				//MFG_LOG_ERR("bmi160 accel calibration success\n");
		   }
		   else
		   {
				MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
    	}
	}
	else
	{
		MFG_LOG_ERR("bmi160 accel calibration fail\n");
		clearAccValue();
		return com_rslt;
	}
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE  bmi160_test_accel_ydown(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0;
	SensorAxes_t accel_tmp = {0};

	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
		bmi160_offset.isAccValid = 0;
		getBmi160OffsetValue((void *)(&bmi160_offset));
		if(bmi160_offset.isAccValid != 1)
	 	{
			if(getAccSelfTestValue() == 0x67676767)
			{
				MFG_LOG_INFO("Acc [has not calibrated]\r\n");
			}
			else
			{
				MFG_LOG_INFO("Acc [has not self-tested]\r\n");
			}
			return BMI160_ERROR;
	  	}
	    for(i = 0 ; i < accel_test_count ; i++)
        {
           if(BMI160_X_Get_Axes( NULL,&accel_tmp) != 0)
           {
           		MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
		   if((((ACCEL_1000MG - ACCEL_BASE_VALUE))<accel_tmp.AXIS_Y) && (accel_tmp.AXIS_Y<((ACCEL_1000MG + ACCEL_BASE_VALUE))) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_X)) && (accel_tmp.AXIS_X<ACCEL_BASE_VALUE) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Z)) && (accel_tmp.AXIS_Z<ACCEL_BASE_VALUE)    )
		   {
				//MFG_LOG_ERR("bmi160 accel calibration success\n");
		   }
		   else
		   {
				MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
    	}
	}
	else
	{
		MFG_LOG_ERR("bmi160 accel calibration fail\n");
		clearAccValue();
		return com_rslt;
	}
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE  bmi160_test_accel_zup(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0;
	SensorAxes_t accel_tmp = {0};

	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
		bmi160_offset.isAccValid = 0;
		getBmi160OffsetValue((void *)(&bmi160_offset));
		if(bmi160_offset.isAccValid != 1)
	 	{
			if(getAccSelfTestValue() == 0x67676767)
			{
				MFG_LOG_INFO("Acc [has not calibrated]\r\n");
			}
			else
			{
				MFG_LOG_INFO("Acc [has not self-tested]\r\n");
			}
			return BMI160_ERROR;
	  	}
	    for(i = 0 ; i < accel_test_count ; i++)
        {
           if(BMI160_X_Get_Axes( NULL,&accel_tmp) != 0)
           {
           		MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
		   if(((-(ACCEL_1000MG + ACCEL_BASE_VALUE))<accel_tmp.AXIS_Z) && (accel_tmp.AXIS_Z<(-(ACCEL_1000MG - ACCEL_BASE_VALUE))) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_X)) && (accel_tmp.AXIS_X<ACCEL_BASE_VALUE) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Y)) && (accel_tmp.AXIS_Y<ACCEL_BASE_VALUE)    )
		   {
				//MFG_LOG_ERR("bmi160 accel calibration success\n");
		   }
		   else
		   {
				MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
    	}
	}
	else
	{
		MFG_LOG_ERR("bmi160 accel calibration fail\n");
		clearAccValue();
		return com_rslt;
	}
	return com_rslt;
}
#endif
BMI160_RETURN_FUNCTION_TYPE  bmi160_test_accel_zdown(uint8_t *buffer)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0;
	SensorAxes_t accel_tmp = {0};
	int zdown_max = 0;

	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
		bmi160_offset.isAccValid = 0;
		getBmi160OffsetValue((void *)(&bmi160_offset));
		if(bmi160_offset.isAccValid != 1)
	 	{
			if(getAccSelfTestValue() == 0x67676767)
			{
				MFG_LOG_INFO("Acc [has not calibrated]\r\n");
			}
			else
			{
				MFG_LOG_INFO("Acc [has not self-tested]\r\n");
			}
			return BMI160_ERROR;
	  	}
	    for(i = 0 ; i < accel_test_count ; i++)
        {
           if(BMI160_X_Get_Axes( NULL,&accel_tmp) != 0)
           {
           		MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
		   zdown_max =( (zdown_max > abs(accel_tmp.AXIS_Z- 1000))?zdown_max:abs(accel_tmp.AXIS_Z- 1000));
		   if((((ACCEL_1000MG - ACCEL_BASE_VALUE))<accel_tmp.AXIS_Z) && (accel_tmp.AXIS_Z<((ACCEL_1000MG + ACCEL_BASE_VALUE))) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_X)) && (accel_tmp.AXIS_X<ACCEL_BASE_VALUE) && \
		   	  ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Y)) && (accel_tmp.AXIS_Y<ACCEL_BASE_VALUE)    )
		   {
				//MFG_LOG_ERR("bmi160 accel calibration success\n");
		   }
		   else
		   {
				MFG_LOG_ERR("bmi160 accel calibration fail\n");
				clearAccValue();
				return BMI160_ERROR;
		   }
    	}
	}
	else
	{
		MFG_LOG_ERR("bmi160 accel calibration fail\n");
		clearAccValue();
		return com_rslt;
	}
	sprintf( (char *)buffer,  "zdown_max=%.2fg\n", (float)zdown_max/1000 );
	return com_rslt;
}


int32_t BMI160_ACCEL_K6(uint8_t * buf, uint8_t len)
{
    if(buf == NULL)
    {
        return -1;
    }
	#if 0
	if (!strcmp((char const*)buf, "acc_get_rawdata"))
	{
		//bmi160_get_accel_rawdata();
	} else if (!strcmp((char const*)buf, "gyro_get_rawdata"))
	{
		//bmi160_get_gyro_rawdata();
	} else if (!strcmp((char const*)buf, "acc_selftest"))
	{
		//usb_app_transmit_data("accel selftest start,please wait 8 second ....\n",
							  //  strlen("accel selftest start,please wait 8 second ....\n"));
		//bmi160_Acc_selftest();
	} else if (!strcmp((char const*)buf, "gyro_selftest"))
	{
		//bmi160_Gyro_selftest();
	} else if (!strcmp((char const*)buf, "get_chipid"))
	{
		//bmi160_read_chipid();
	} else

	if (!strcmp((char const*)buf, "k_start"))
	{
		usb_app_transmit_data("OK\n",3);
	} else if (!strcmp((char const*)buf, "k_acc xup")) //k_acc  xup
	{
		if(bmi160_read_accel_xup() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "k_acc xdown"))//k_acc  x_down
	{
		if(bmi160_read_accel_xdown() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "k_acc yup"))//k_acc  yup
	{
		if(bmi160_read_accel_yup() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "k_acc ydown"))//k_acc  ydown
	{
		if(bmi160_read_accel_ydown() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "k_acc zup"))//k_acc  zup
	{
		if(bmi160_read_accel_zup() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "k_acc zdown"))//k_acc zdown
	{
		if(bmi160_read_accel_zdown() == 0)
		{

		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "k_gyro"))//k_gyro
	{
		if(bmi160_Gyro_Calibate() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	}

	else if (!strcmp((char const*)buf, "ktest_acc xup")) //test_acc  xup
	{
		if(bmi160_test_accel_xup() == 0)
		{
			usb_app_transmit_data("OK",2);
		}
		else
		{
			usb_app_transmit_data("fail",4);
		}
	}else if (!strcmp((char const*)buf, "ktest_acc xdown"))//test_acc  x_down
	{
		if(bmi160_test_accel_xdown() == 0)
		{
			usb_app_transmit_data("OK",2);
		}
		else
		{
			usb_app_transmit_data("fail",4);
		}
	} else if (!strcmp((char const*)buf, "ktest_acc yup"))//test_acc  yup
	{
		if(bmi160_test_accel_yup() == 0)
		{
			usb_app_transmit_data("OK",2);
		}
		else
		{
			usb_app_transmit_data("fail",4);
		}
	} else if (!strcmp((char const*)buf, "ktest_acc ydown"))//test_acc  ydown
	{
		if(bmi160_test_accel_ydown() == 0)
		{
			usb_app_transmit_data("OK",2);
		}
		else
		{
			usb_app_transmit_data("fail",4);
		}
	} else if (!strcmp((char const*)buf, "ktest_acc zup"))//test_acc  zup
	{
		if(bmi160_test_accel_zup() == 0)
		{
			usb_app_transmit_data("OK",2);
		}
		else
		{
			usb_app_transmit_data("fail",4);
		}
	} else if (!strcmp((char const*)buf, "ktest_acc zdown"))//test_acc zdown
	{
		if(bmi160_test_accel_zdown() == 0)
		{
			usb_app_transmit_data("OK",2);
		}
		else
		{
			usb_app_transmit_data("fail",4);
		}
	}

	else if (!strcmp((char const*)buf, "ktest_gyro"))//test_gyro
	{
		if(bmi160_test_gyro() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	}else
	{
			usb_app_transmit_data("input arguments ERROR!\n",22);
	}
	#endif
	return 0;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_ACC_fd_test(void)
{
	Save_str_k6	accel_k6;
	int i = 0,j = 0;
	SensorAxes_t accel_tmp = {0};
	BMI160_DataTypeDef bmi160_offset_tmp;
	float sensitivity = 0;
	int32_t x= 0,y = 0,z = 0;
	//int32_t xdown_xmax = 0,xdown_ymax = 0,xdown_ymax = 0;
	//int32_t yup_xmax = 0,yup_ymax = 0,yup_zmax = 0;
	//int32_t ydown_xmax = 0,ydown_ymax = 0,ydown_zmax = 0;
	//int32_t zup_xmax = 0,zup_ymax = 0,zup_zmax = 0;
	int32_t cal_flag = 0;

	int32_t offset_max[6][3];

	uint8_t buffer[128];

	bmi160_offset_tmp.isAccValid = 0;
	getBmi160OffsetValue((void *)(&bmi160_offset_tmp));
	if(bmi160_offset_tmp.isAccValid != 1)
	{
		MFG_LOG_INFO("Acc [has not calibrated]\r\n");
		return BMI160_ERROR;
	 }
	/* Get BMI160 actual sensitivity. */
	if ( BMI160_X_Get_Sensitivity( NULL, &sensitivity ) == COMPONENT_ERROR )
	{
		return BMI160_ERROR;
	}


	 cal_flag = 0;
	//test xup
	for(i = 0;i < 30;i++)
	{
		VR_flash_read(ACC_XUP_FLASHID + i*1024, &accel_k6, sizeof(Save_str_k6));
		if((accel_k6.start_id == ACC_XUP_FLASHID) && (accel_k6.end_id== ACC_XUP_FLASHID))
		{
			for(j =0;j<100;j++)
			{
				accel_tmp.AXIS_X = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[0][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Y = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[1][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Z = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[2][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				if(x < abs(accel_tmp.AXIS_X + 1000))
				{
					offset_max[0][0] = accel_tmp.AXIS_X;
					x = abs(accel_tmp.AXIS_X + 1000);
				}
				if(y < abs(accel_tmp.AXIS_Y))
				{
					offset_max[0][1] = accel_tmp.AXIS_Y;
					y = abs(accel_tmp.AXIS_Y);
				}
				if(z < abs(accel_tmp.AXIS_Z))
				{
					offset_max[0][2] = accel_tmp.AXIS_Z;
					z = abs(accel_tmp.AXIS_Z);
				}

				//printf("x = %d y = %d z = %d\n",accel_tmp.AXIS_X,accel_tmp.AXIS_Y,accel_tmp.AXIS_Z);
				if(((-(ACCEL_1000MG + ACCEL_BASE_VALUE))<accel_tmp.AXIS_X) && (accel_tmp.AXIS_X<(-(ACCEL_1000MG - ACCEL_BASE_VALUE))) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Y)) && (accel_tmp.AXIS_Y<ACCEL_BASE_VALUE) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Z)) && (accel_tmp.AXIS_Z<ACCEL_BASE_VALUE)    )
		   		{

		   		}
		   		else
		   		{
					cal_flag++;
		   		}
			}
		}
		else
		{
			/* usb_app_transmit_data("read flash fail\n",strlen("read flash fail\n")); */
			return BMI160_ERROR;
		}
	}

	x = 0;
	y = 0;
	z = 0;
	//test xdown
	for(i = 0;i < 30;i++)
	{
		VR_flash_read(ACC_XDOWN_FLASHID + i*1024, &accel_k6, sizeof(Save_str_k6));
		if((accel_k6.start_id == ACC_XDOWN_FLASHID) && (accel_k6.end_id== ACC_XDOWN_FLASHID))
		{
			for(j =0;j<100;j++)
			{
				accel_tmp.AXIS_X = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[0][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Y = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[1][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Z = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[2][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				if(x < abs(accel_tmp.AXIS_X - 1000))
				{
					offset_max[1][0] = accel_tmp.AXIS_X;
					x = abs(accel_tmp.AXIS_X - 1000);
				}
				if(y < abs(accel_tmp.AXIS_Y))
				{
					offset_max[1][1] = accel_tmp.AXIS_Y;
					y = abs(accel_tmp.AXIS_Y);
				}
				if(z < abs(accel_tmp.AXIS_Z))
				{
					offset_max[1][2] = accel_tmp.AXIS_Z;
					z = abs(accel_tmp.AXIS_Z);
				}

				//printf("x = %d y = %d z = %d\n",accel_tmp.AXIS_X,accel_tmp.AXIS_Y,accel_tmp.AXIS_Z);
				if((((ACCEL_1000MG - ACCEL_BASE_VALUE))<accel_tmp.AXIS_X) && (accel_tmp.AXIS_X<((ACCEL_1000MG + ACCEL_BASE_VALUE))) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Y)) && (accel_tmp.AXIS_Y<ACCEL_BASE_VALUE) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Z)) && (accel_tmp.AXIS_Z<ACCEL_BASE_VALUE)    )
		   		{

		  		}
		   		else
		   		{
					cal_flag++;
		   		}
			}
		}
		else
		{
			/* usb_app_transmit_data("read flash fail\n",strlen("read flash fail\n")); */
			return BMI160_ERROR;
		}
	}
	x = 0;
	y = 0;
	z = 0;
	//test yup
	for(i = 0;i < 30;i++)
	{
		VR_flash_read(ACC_YUP_FLASHID + i*1024, &accel_k6, sizeof(Save_str_k6));
		if((accel_k6.start_id == ACC_YUP_FLASHID) && (accel_k6.end_id== ACC_YUP_FLASHID))
		{
			for(j =0;j<100;j++)
			{
				accel_tmp.AXIS_X = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[0][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Y = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[1][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Z = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[2][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				if(x < abs(accel_tmp.AXIS_X ))
				{
					offset_max[2][0] = accel_tmp.AXIS_X;
					x = abs(accel_tmp.AXIS_X );
				}
				if(y < abs(accel_tmp.AXIS_Y + 1000))
				{
					offset_max[2][1] = accel_tmp.AXIS_Y;
					y = abs(accel_tmp.AXIS_Y +1000);
				}
				if(z < abs(accel_tmp.AXIS_Z))
				{
					offset_max[2][2] = accel_tmp.AXIS_Z;
					z = abs(accel_tmp.AXIS_Z);
				}
				//printf("x = %d y = %d z = %d\n",accel_tmp.AXIS_X,accel_tmp.AXIS_Y,accel_tmp.AXIS_Z);
				if(((-(ACCEL_1000MG + ACCEL_BASE_VALUE))<accel_tmp.AXIS_Y) && (accel_tmp.AXIS_Y<(-(ACCEL_1000MG - ACCEL_BASE_VALUE))) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_X)) && (accel_tmp.AXIS_X<ACCEL_BASE_VALUE) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Z)) && (accel_tmp.AXIS_Z<ACCEL_BASE_VALUE)    )
		   		{

		   		}
		   		else
		   		{
					cal_flag++;
		   		}
			}
		}
		else
		{
			/* usb_app_transmit_data("read flash fail\n",strlen("read flash fail\n")); */
			return BMI160_ERROR;
		}
	}
	//test ydown
	x = 0;
	y = 0;
	z = 0;
	for(i = 0;i < 30;i++)
	{
		VR_flash_read(ACC_YDOWN_FLASHID + i*1024, &accel_k6, sizeof(Save_str_k6));
		if((accel_k6.start_id == ACC_YDOWN_FLASHID) && (accel_k6.end_id== ACC_YDOWN_FLASHID))
		{
			for(j =0;j<100;j++)
			{
				accel_tmp.AXIS_X = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[0][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Y = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[1][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Z = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[2][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				if(x < abs(accel_tmp.AXIS_X ))
				{
					offset_max[3][0] = accel_tmp.AXIS_X;
					x = abs(accel_tmp.AXIS_X );
				}
				if(y < abs(accel_tmp.AXIS_Y - 1000))
				{
					offset_max[3][1] = accel_tmp.AXIS_Y;
					y = abs(accel_tmp.AXIS_Y -1000);
				}
				if(z < abs(accel_tmp.AXIS_Z))
				{
					offset_max[3][2] = accel_tmp.AXIS_Z;
					z = abs(accel_tmp.AXIS_Z);
				}
				if((((ACCEL_1000MG - ACCEL_BASE_VALUE))<accel_tmp.AXIS_Y) && (accel_tmp.AXIS_Y<((ACCEL_1000MG + ACCEL_BASE_VALUE))) && \
		   	 		 ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_X)) && (accel_tmp.AXIS_X<ACCEL_BASE_VALUE) && \
		   	  		 ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Z)) && (accel_tmp.AXIS_Z<ACCEL_BASE_VALUE)    )
		   		{

		  		}
		   		else
		   		{
					cal_flag++;
		   		}
			}
		}
		else
		{
			/* usb_app_transmit_data("read flash fail\n",strlen("read flash fail\n")); */
			return BMI160_ERROR;
		}
	}
	x = 0;
	y = 0;
	z = 0;
	//test zup
	for(i = 0;i < 30;i++)
	{
		VR_flash_read(ACC_ZUP_FLASHID + i*1024, &accel_k6, sizeof(Save_str_k6));
		if((accel_k6.start_id == ACC_ZUP_FLASHID) && (accel_k6.end_id== ACC_ZUP_FLASHID))
		{
			for(j =0;j<100;j++)
			{
				accel_tmp.AXIS_X = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[0][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Y = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[1][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Z = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[2][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				if(x < abs(accel_tmp.AXIS_X ))
				{
					offset_max[4][0] = accel_tmp.AXIS_X;
					x = abs(accel_tmp.AXIS_X );
				}
				if(y < abs(accel_tmp.AXIS_Y))
				{
					offset_max[4][1] = accel_tmp.AXIS_Y;
					y = abs(accel_tmp.AXIS_Y );
				}
				if(z < abs(accel_tmp.AXIS_Z + 1000))
				{
					offset_max[4][2] = accel_tmp.AXIS_Z;
					z = abs(accel_tmp.AXIS_Z +1000);
				}
				//printf("x = %d y = %d z = %d\n",accel_tmp.AXIS_X,accel_tmp.AXIS_Y,accel_tmp.AXIS_Z);
				if(((-(ACCEL_1000MG + ACCEL_BASE_VALUE))<accel_tmp.AXIS_Z) && (accel_tmp.AXIS_Z<(-(ACCEL_1000MG - ACCEL_BASE_VALUE))) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_X)) && (accel_tmp.AXIS_X<ACCEL_BASE_VALUE) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Y)) && (accel_tmp.AXIS_Y<ACCEL_BASE_VALUE)    )
		   		{

		   		}
		   		else
		   		{
					cal_flag++;
		   		}
			}
		}
		else
		{
			/* usb_app_transmit_data("read flash fail\n",strlen("read flash fail\n")); */
			return BMI160_ERROR;
		}
	}
	//test zdown
	x = 0;
	y = 0;
	z = 0;
	for(i = 0;i < 30;i++)
	{
		VR_flash_read(ACC_ZDOWN_FLASHID + i*1024, &accel_k6, sizeof(Save_str_k6));
		if((accel_k6.start_id == ACC_ZDOWN_FLASHID) && (accel_k6.end_id== ACC_ZDOWN_FLASHID))
		{
			for(j =0;j<100;j++)
			{
				accel_tmp.AXIS_X = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[0][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[0][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Y = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[1][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							  					bmi160_offset_tmp.accel_k_xyz[1][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				accel_tmp.AXIS_Z = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[2][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   					bmi160_offset_tmp.accel_k_xyz[2][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

				if(x < abs(accel_tmp.AXIS_X ))
				{
					offset_max[5][0] = accel_tmp.AXIS_X;
					x = abs(accel_tmp.AXIS_X );
				}
				if(y < abs(accel_tmp.AXIS_Y))
				{
					offset_max[5][1] = accel_tmp.AXIS_Y;
					y = abs(accel_tmp.AXIS_Y );
				}
				if(z < abs(accel_tmp.AXIS_Z - 1000))
				{
					offset_max[5][2] = accel_tmp.AXIS_Z;
					z = abs(accel_tmp.AXIS_Z -1000);
				}
				  if((((ACCEL_1000MG - ACCEL_BASE_VALUE))<accel_tmp.AXIS_Z) && (accel_tmp.AXIS_Z<((ACCEL_1000MG + ACCEL_BASE_VALUE))) && \
		   	  		((-ACCEL_BASE_VALUE<accel_tmp.AXIS_X)) && (accel_tmp.AXIS_X<ACCEL_BASE_VALUE) && \
		   	 		 ((-ACCEL_BASE_VALUE<accel_tmp.AXIS_Y)) && (accel_tmp.AXIS_Y<ACCEL_BASE_VALUE)    )
		   		{

		   		}
		   		else
		   		{
					cal_flag++;
		   		}
			}
		}
		else
		{
			/* usb_app_transmit_data("read flash fail\n",strlen("read flash fail\n")); */
			return BMI160_ERROR;
		}
	}


	if(cal_flag !=0)
	{
		 memset(buffer,'\0',128);
		 sprintf( (char *)buffer,"xup:   xmax =%dmg\tymax=%dmg\tzmax =%dmg\r\n", offset_max[0][0], offset_max[0][1], offset_max[0][2]); // C4996
		 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
		 printf("%s",buffer);
		 memset(buffer,'\0',128);

		 sprintf( (char *)buffer,"xdown: xmax =%dmg\tymax=%dmg\tzmax =%dmg\r\n", offset_max[1][0], offset_max[1][1], offset_max[1][2]); // C4996
		 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
		 printf("%s",buffer);
		 memset(buffer,'\0',128);

		 sprintf( (char *)buffer,"yup:   xmax =%dmg\tymax=%dmg\tzmax =%dmg\r\n", offset_max[2][0], offset_max[2][1], offset_max[2][2]); // C4996
		 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
		 printf("%s",buffer);
		 memset(buffer,'\0',128);

		 sprintf( (char *)buffer,"ydown: xmax =%dmg\tymax=%dmg\tzmax =%dmg\r\n", offset_max[3][0], offset_max[3][1], offset_max[3][2]); // C4996
		 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
		 printf("%s",buffer);
		 memset(buffer,'\0',128);

		 sprintf( (char *)buffer ,"zup:   xmax =%dmg\tymax=%dmg\tzmax =%dmg\r\n", offset_max[4][0], offset_max[4][1], offset_max[4][2]); // C4996
		 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
		 printf("%s",buffer);
		 memset(buffer,'\0',128);

		 sprintf( (char *)buffer,"zdown: xmax =%dmg\tymax=%dmg\tzmax =%dmg\r\n", offset_max[5][0], offset_max[5][1], offset_max[5][2]); // C4996
		 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
		 printf("%s",buffer);

	 	 saveChangeACCValue();
		return -1;
	 }
	return 0;
}

BMI160_RETURN_FUNCTION_TYPE bmi160_dump_accdata(void)
{
	Save_str_k6	accel_k6;
	int i = 0,j = 0;
	SensorAxes_t accel_tmp = {0};
	BMI160_DataTypeDef bmi160_offset_tmp;
	float sensitivity = 0;
	SensorAxes_t acc_rawdata;
	uint8_t  buffer[256];

	bmi160_offset_tmp.isAccValid = 0;
	getBmi160OffsetValue((void *)(&bmi160_offset_tmp));
	if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
	{
		MFG_LOG_ERR("bmi160 read accel_xup fail\n");
	}
	/* Get BMI160 actual sensitivity. */
	if ( BMI160_X_Get_Sensitivity( NULL, &sensitivity ) == COMPONENT_ERROR )
	{
		MFG_LOG_ERR("bmi160 read sensitivity fail\n");
	}


	///////////////////////////////////////////
	 memset(buffer,'\0',256);
	 j = 0;
	 j  = sprintf( (char *)buffer,    "k 6_axis offset: x = %f y = %fz = %f\n",   bmi160_offset_tmp.accel_x,
											  					   bmi160_offset_tmp.accel_y,
											  					   bmi160_offset_tmp.accel_z);

	 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
	 //////////////////////////////////////////
	memset(buffer,'\0',256);
	 j = 0;
	 j  = sprintf( (char *)buffer,   "%f\t%f\t%f\n",bmi160_offset_tmp.accel_k_xyz[0][0],
	 									    bmi160_offset_tmp.accel_k_xyz[0][1],
	 									    bmi160_offset_tmp.accel_k_xyz[0][2]) ;

	 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
	 //////////////////////////////////////////
	memset(buffer,'\0',256);
	 j = 0;
	 j  = sprintf( (char *)buffer,   "%f\t%f\t%f\n",bmi160_offset_tmp.accel_k_xyz[1][0],
	 									    bmi160_offset_tmp.accel_k_xyz[1][1],
	 									    bmi160_offset_tmp.accel_k_xyz[1][2]) ;
	 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
	 //////////////////////////////////////////
	memset(buffer,'\0',256);
	 j = 0;
	 j  = sprintf( (char *)buffer,   "%f\t%f\t%f\n",bmi160_offset_tmp.accel_k_xyz[2][0],
	 									    bmi160_offset_tmp.accel_k_xyz[2][1],
	 									    bmi160_offset_tmp.accel_k_xyz[2][2]) ;
	 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
	 //////////////////////////////////////////

	for(i = 0;i<180;i++)
	{
		VR_flash_read(ACC_XUP_FLASHID + i*1024, &accel_k6, sizeof(Save_str_k6));

		for(j =0;j<100;j++)
		{
			accel_tmp.AXIS_X = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[0][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   			bmi160_offset_tmp.accel_k_xyz[0][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   			bmi160_offset_tmp.accel_k_xyz[0][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

			accel_tmp.AXIS_Y = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[1][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							  			bmi160_offset_tmp.accel_k_xyz[1][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							  			bmi160_offset_tmp.accel_k_xyz[1][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

			accel_tmp.AXIS_Z = ( int32_ts )(bmi160_offset_tmp.accel_k_xyz[2][0]*((accel_k6.accel[j].x)*(sensitivity)-bmi160_offset_tmp.accel_x) + \
							   			bmi160_offset_tmp.accel_k_xyz[2][1]*((accel_k6.accel[j].y)*(sensitivity)-bmi160_offset_tmp.accel_y) + \
							   			bmi160_offset_tmp.accel_k_xyz[2][2]*((accel_k6.accel[j].z)*(sensitivity)-bmi160_offset_tmp.accel_z));

			acc_rawdata.AXIS_X = ( int32_ts )( (accel_k6.accel[j].x) * (sensitivity) );
	  		acc_rawdata.AXIS_Y = ( int32_ts )( (accel_k6.accel[j].y) * (sensitivity) );
	 		acc_rawdata.AXIS_Z = ( int32_ts )( (accel_k6.accel[j].z) * (sensitivity) );

			 //////////////////////////////////////////
			memset(buffer,'\0',256);
			sprintf( (char *)buffer,  "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",accel_tmp.AXIS_X,accel_tmp.AXIS_Y,accel_tmp.AXIS_Z,
											   			   acc_rawdata.AXIS_X,acc_rawdata.AXIS_Y,acc_rawdata.AXIS_Z,
											   			   accel_k6.accel[j].x,accel_k6.accel[j].y,accel_k6.accel[j].z);
			 /* usb_app_transmit_data(buffer,strlen((char const *)buffer)); */
	             //////////////////////////////////////////

		}
	}
	return 0;
}
#if 0
int bmi160_notify_callback(uint32_t _notify_flag, uint32_t _state, void *data)
{
    switch(_notify_flag){
        case PWRMGR_NOTIFY_STOP_STATE:
            if(_state == STOP_ENTER){
                //printf("%s _notify_flag:%#x, state:%d\n",__func__, _notify_flag, _state);
                if(bmi160_accel_mode(ACCEL_SUSPEND) != 1)
                {
                	HAL_Delay(2);
					bmi160_accel_mode(ACCEL_SUSPEND);
				}
				HAL_Delay(20);
				if(bmi160_gyro_mode(GYRO_MODE_SUSPEND) != 1)
				{
					HAL_Delay(2);
					bmi160_gyro_mode(GYRO_MODE_SUSPEND);
				}
				HAL_Delay(5);
            }
            else if(_state == STOP_LEAVE){
				if(bmi160_accel_mode(ACCEL_MODE_NORMAL) != 1)
                {
                	HAL_Delay(2);
					bmi160_accel_mode(ACCEL_MODE_NORMAL);
				}
				HAL_Delay(20);
				if(bmi160_gyro_mode(GYRO_MODE_NORMAL) != 1)
				{
					HAL_Delay(2);
					bmi160_gyro_mode(GYRO_MODE_NORMAL);
				}
				HAL_Delay(5);
            }
            else{
                gsensor_err("%s state error\n",__func__);
            }
            break;
        default:
            gsensor_info("bmi160 don't care this flag\n");
            break;
    }
    return 0;
}
void bmi160_postinit(void)
{
    Bmi160PmNotifyData.func_name = "bmi160_drv";
    Bmi160PmNotifyData.data = NULL;
    Bmi160PmNotifyData.callback= bmi160_notify_callback;
    Bmi160PmNotifyData.notify_flag = PWRMGR_NOTIFY_STOP_STATE;
    Bmi160PmNotifyData.func_level = PWRMGR_FUNC_DRIVER_LEVEL;
    PWRMGR_register_notify_func(&Bmi160PmNotifyData);

    gsensor_info("bmi160_postinit success!\n");

}
#endif
/*<<<BMI function for E-compass sensor start>>>*/
BMI160_RETURN_FUNCTION_TYPE bmi160_mag_init(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = E_BMI160_COMM_RES;
	us8 v_data_us8  = 0;
	v_data_us8  = 0x19;
	com_rslt  = bmi160_write_reg(0x7E,&v_data_us8,1);
	bmi160_delay_ms(100);

	v_data_us8  = 0x37;
	com_rslt += bmi160_write_reg(0x7E,&v_data_us8,1);
	bmi160_delay_ms(5);

	v_data_us8  = 0x9A;
	com_rslt += bmi160_write_reg(0x7E,&v_data_us8,1);
	bmi160_delay_ms(5);

	v_data_us8  = 0xC0;
	com_rslt += bmi160_write_reg(0x7E,&v_data_us8,1);
	bmi160_delay_ms(5);

	v_data_us8  = 0x90;
	com_rslt += bmi160_write_reg(0x7F,&v_data_us8,1);
	bmi160_delay_ms(5);

	v_data_us8  = 0x80;
	com_rslt += bmi160_write_reg(0x7F,&v_data_us8,1);
	bmi160_delay_ms(5);

	v_data_us8  = (0x0C << 1);//I2C ADDR
	com_rslt += bmi160_write_reg(0x4B,&v_data_us8,1);
	v_data_us8  = 0x20;
	com_rslt += bmi160_write_reg(0x6B,&v_data_us8,1);
	bmi160_delay_ms(5);

	v_data_us8  = 0X08;//
	com_rslt += bmi160_write_reg(0x44,&v_data_us8,1);

	v_data_us8  = 0x80 |0x02;
	com_rslt += bmi160_write_reg(0x4C,&v_data_us8, 1);

	return com_rslt;
}
static BMI160_RETURN_FUNCTION_TYPE bmi_is_mag_manual(us8 * enable)
{
	us8 v_data_us8;
	BMI160_RETURN_FUNCTION_TYPE com_rslt = E_BMI160_COMM_RES;
	com_rslt = bmi160_read_reg(0x4C,&v_data_us8, 1);
	if(v_data_us8 & 0x80)
		*enable = 1;
	else
		*enable = 0;
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_mag_write_reg(us8 v_addr_us8,us8 *v_data)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = E_BMI160_COMM_RES;
	us8 v_data_us8  = 0;
	com_rslt = bmi_is_mag_manual(&v_data_us8);
	if(!v_data_us8){
		com_rslt += bmi160_mag_manual_enable();
	}

	com_rslt += bmi160_write_reg(0x4F,v_data, 1);//data to write
	bmi160_delay_ms(2);
	com_rslt += bmi160_write_reg(0x4E,&v_addr_us8, 1);//address to write
	bmi160_delay_ms(2);

	com_rslt += bmi_is_mag_manual(&v_data_us8);
	if(v_data_us8){
		com_rslt += bmi160_mag_manual_disable(0x11,8);
	}
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_mag_read_reg(us8 v_addr_us8,us8 *v_data,us8 v_len_us8 )
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = E_BMI160_COMM_RES;
	us8 v_data_us8;


	if(v_len_us8 != 9)
	{
		com_rslt = bmi_is_mag_manual(&v_data_us8);
		if(!v_data_us8){
			com_rslt += bmi160_mag_manual_enable();
		}

		com_rslt += bmi160_write_reg(0x4D,&v_addr_us8, 1);
		bmi160_delay_ms(2);
		com_rslt += bmi160_read_reg(0x04,v_data, v_len_us8);//return reg value

		com_rslt += bmi_is_mag_manual(&v_data_us8);
		if(v_data_us8){
			com_rslt += bmi160_mag_manual_disable(0x11,8);
		}

	}else
	{
		com_rslt = bmi_is_mag_manual(&v_data_us8);
		if(v_data_us8){
			com_rslt += bmi160_mag_manual_disable(0x11,8);
		}

		*v_data = 0x01;
		com_rslt += bmi160_read_reg(0x04,v_data+1, v_len_us8-1);//return rawdata
	}
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE  bmi160_mag_manual_disable(us8 v_addr_us8,us8 v_len_us8 )
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = E_BMI160_COMM_RES;
	us8 v_data_us8  = 0;


	switch(v_len_us8)
	{
		case 1:
			v_data_us8  = 0x00;
			break;
		case 2:
			v_data_us8  = 0x01;
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			v_data_us8  = 0x02;
			break;
		case 7:
		case 8:
		case 9:
			v_data_us8  = 0x03;
			break;
		default:
			return 0;

	}
	com_rslt = bmi160_write_reg(0x4D,&v_addr_us8, 1);
	bmi160_delay_ms(2);
	com_rslt += bmi160_write_reg(0x4C,&v_data_us8, 1);
	bmi160_delay_ms(104);
	return com_rslt;
}

BMI160_RETURN_FUNCTION_TYPE bmi160_mag_manual_enable(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = E_BMI160_COMM_RES;
	us8 v_data_us8  = 0;

	v_data_us8  = 0x80|0x02;
	com_rslt = bmi160_write_reg(0x4C,&v_data_us8, 1);
	bmi160_delay_ms(104);
	return com_rslt;
}

/*
#define MAG_MODE_SUSPEND	(0x18)
#define MAG_MODE_NORMAL	(0x19)
#define MAG_MODE_LOWPOWER	(0x1A)
*/
BMI160_RETURN_FUNCTION_TYPE bmi160_mag_mode(us8 mag_power_mode)
{
    BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
    com_rslt = bmi160_set_command_register(mag_power_mode);
    return com_rslt;
}
/*>>>BMI function for E-compass sensor end<<<***/
