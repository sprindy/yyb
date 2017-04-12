#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "sensor_task.h"
#include "BMI160_ACC_GYRO_Calibate.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "flash_drv.h"



static BMI160_RETURN_FUNCTION_TYPE BMI160_X_Get_Axes_k6( SensorAxes_t_k6 *acceleration, struct bmi160_accel_t *accel);
static BMI160_RETURN_FUNCTION_TYPE BMI160_X_Get_Sensitivity_k6(float *sensitivity );
extern BMI160_RETURN_FUNCTION_TYPE  bmi160_test_accel_zdown(uint8_t *buffer);
extern BMI160_RETURN_FUNCTION_TYPE bmi160_ACC_fd_test(void);
extern int8_t usb_app_transmit_data(uint8_t *buf, uint8_t len);
extern int cmd_gsensor_check(char cmdsource);
extern int cmd_gyro_check(char cmdsource);

float xup_x = 0,xup_y = 0,xup_z = 0;//x up   x = -16384
float xdown_x = 0,xdown_y = 0,xdown_z = 0;//x down   x = 16384
float yup_x = 0,yup_y = 0,yup_z = 0;//y up   y= -16384
float ydown_x = 0,ydown_y = 0,ydown_z = 0;//y down   y= 16384
float zup_x = 0,zup_y = 0,zup_z = 0;//z up   z= -16384
float zdown_x = 0,zdown_y = 0,zdown_z = 0;//z down   z= 16384

//int mfg_init_flag = 0;
int bmi160_mfg_init(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode_stat_us8 = 0;
	if(1)
	{
		//Sensor_SW_Reset();
		gsensormfgInitial();
		bmi160_read_reg(0x0,&v_accel_power_mode_stat_us8, 1);
		if(v_accel_power_mode_stat_us8 !=0xd1)
		{
			gsensor_err("read chipid fail\n");
			return BMI160_ERROR;
		}
		 bmi160_delay_ms(2);
		com_rslt  = bmi160_set_intr_data_rdy(BMI160_INTR2_MAP_DATA_RDY,BMI160_DISABLE);//disable
		if(com_rslt !=BMI160_SUCCESS)
		{
			return BMI160_ERROR;
		}
	    /////////////////////////////////////////////////////////////////////////////////////////
	    bmi160_get_accel_power_mode_stat(&v_accel_power_mode_stat_us8);
	    bmi160_delay_ms(20);
	    if(v_accel_power_mode_stat_us8 != 1)
	    {
	        bmi160_accel_mode(ACCEL_MODE_NORMAL);
		 bmi160_delay_ms(200);
		 v_accel_power_mode_stat_us8 = 0;
		 bmi160_get_accel_power_mode_stat(&v_accel_power_mode_stat_us8);
		   if(v_accel_power_mode_stat_us8 != 1)
		  {
		  	return BMI160_ERROR;
		   }
	    }
	   ///////////////////////////////////////////////////////////////////////////////////////////
	 bmi160_delay_ms(5);
	 v_accel_power_mode_stat_us8 = 0;
	 bmi160_get_gyro_power_mode_stat(&v_accel_power_mode_stat_us8);
	  bmi160_delay_ms(20);
	 if(v_accel_power_mode_stat_us8 != 1)
	 {
	    	bmi160_gyro_mode(GYRO_MODE_NORMAL) ;
		bmi160_delay_ms(200);
		bmi160_get_gyro_power_mode_stat(&v_accel_power_mode_stat_us8);

		if(v_accel_power_mode_stat_us8 != 1)
	 	{
	 		return BMI160_ERROR;
		 }
	  }
	/////////////////////////////////////////////////////////////////////////////////////////////
	bmi160_delay_ms(5);
	if ( bmi160_set_accel_output_data_rate(BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ,BMI160_ACCEL_NORMAL_AVG4) != BMI160_SUCCESS )
  	{
    	 	return BMI160_ERROR;
  	}
	bmi160_delay_ms(5);
	if (bmi160_set_gyro_output_data_rate(BMI160_GYRO_OUTPUT_DATA_RATE_800HZ) !=BMI160_SUCCESS)
	{
		 return BMI160_ERROR;
	}
	bmi160_delay_ms(5);
	if(bmi160_set_accel_range(BMI160_ACCEL_RANGE_2G) != BMI160_SUCCESS )
	{
		return BMI160_ERROR;
	}

	bmi160_delay_ms(5);
	if ( bmi160_set_gyro_range(BMI160_GYRO_RANGE_2000_DEG_SEC) != BMI160_SUCCESS)
	{
	      return BMI160_ERROR;
	}
		//mfg_init_flag = 1;
	}
	return BMI160_SUCCESS;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_ACC_Calibate(unsigned char *buf)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_SUCCESS;
	 if (!strcmp((char const*)buf, "xup")) //k_acc  xup
	{
		if(bmi160_read_accel_xup() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "xdown"))//k_acc  x_down
	{
		if(bmi160_read_accel_xdown() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "yup"))//k_acc  yup
	{
		if(bmi160_read_accel_yup() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "ydown"))//k_acc  ydown
	{
		if(bmi160_read_accel_ydown() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "zup"))//k_acc  zup
	{
		if(bmi160_read_accel_zup() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
		}
	} else if (!strcmp((char const*)buf, "zdown"))//k_acc zdown
	{
		if(bmi160_read_accel_zdown() == 0)
		{
			usb_app_transmit_data("OK\n",3);
		}
		else
		{
			usb_app_transmit_data("fail\n",5);
			return BMI160_ERROR;
		}

		if(bmi160_accel_k6andtest() ==0)
		{
			usb_app_transmit_data("calibration pass\n",strlen("calibration pass\n"));
		}
		else
		{
			usb_app_transmit_data("calibration fail\n",strlen("calibration fail\n"));
		}

	} else if (!strcmp((char const*)buf, "dump"))//dump data
	{
		bmi160_dump_accdata();
	}else if (!strcmp((char const*)buf, "accheck"))//
	{
		cmd_gsensor_check(1);
	}else if (!strcmp((char const*)buf, "gyroheck"))//
	{
		cmd_gyro_check(1);
	}
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_Gyro_Calibate(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_gyro_power_mode = 0;
	int i = 0;
	int gyro_x_tmp = 0,gyro_y_tmp = 0,gyro_z_tmp = 0;
	struct bmi160_gyro_t gyro;
	BMI160_DataTypeDef gyro_calvalue = {0};
	if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
	{
		MFG_LOG_ERR(MFG_TAG_SENSOR,"Gyro Calibate fail!\n");
		return BMI160_ERROR;
	}
	com_rslt = bmi160_get_gyro_power_mode_stat(&v_gyro_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_gyro_power_mode == 1))
	{
		for(i = 0 ; i < 500 ; i++)
    	{
       	    com_rslt = bmi160_read_gyro_xyz(&gyro);
        	if (com_rslt != BMI160_SUCCESS)
        	{
          	  MFG_LOG_ERR(MFG_TAG_SENSOR,"Gyro Calibate fail!\n");
          	  return com_rslt;
       		}
        	gyro_x_tmp += gyro.x;
        	gyro_y_tmp += gyro.y;
        	gyro_z_tmp += gyro.z;
        	bmi160_delay_ms(2);/* bmi160_delay_ms in ms*/
    	}
	}
	else
	{
		 MFG_LOG_ERR(MFG_TAG_SENSOR,"Gyro Calibate fail!\n");
		 return com_rslt;
	}
	gyro_calvalue.gyro_x = gyro_x_tmp/i;
	gyro_calvalue.gyro_y = gyro_y_tmp/i;
	gyro_calvalue.gyro_z = gyro_z_tmp/i;

	saveGyroOffsetValue((void *)(&gyro_calvalue));
	MFG_LOG_INFO(MFG_TAG_SENSOR,"SaveBmi160Data:gyro_x = %d gyro_y = %d gyro_z = %d\n",
		gyro_calvalue.gyro_x,gyro_calvalue.gyro_y,gyro_calvalue.gyro_z);
	return com_rslt;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/*  * * * * * * * * * * * * * * * * k  6 * * * * * *  * * * * * * * * *////////////////////////////////////

double calculate_A( double src[][BMI160_MAX], int n )
{
    int i,j,k,x,y;
    double tmp[BMI160_MAX][BMI160_MAX], t;
    double result = 0.0;

    if( n == 1 )
    {
        return src[0][0];
    }

    for( i = 0; i < n; ++i )
    {
        for( j = 0; j < n - 1; ++j )
        {
            for( k = 0; k < n - 1; ++k )
            {
                x = j + 1;
                y = k >= i ? k + 1 : k;

                tmp[j][k] = src[x][y];
            }
        }

        t = calculate_A( tmp, n - 1 );

        if( i % 2 == 0 )
        {
            result += src[0][i] * t;
        }
        else
        {
            result -= src[0][i] * t;
        }
    }

    return result;
}
void calculate_A_adjoint( double src[BMI160_MAX][BMI160_MAX], double dst[BMI160_MAX][BMI160_MAX], int n )
{
    int i, j, k, t, x, y;
    double tmp[BMI160_MAX][BMI160_MAX];

    if( n == 1 )
    {
        dst[0][0] = 1;
        return;
    }

    for( i = 0; i < n; ++i )
    {
        for( j = 0; j < n; ++j )
        {
            for( k = 0; k < n - 1; ++k )
            {
                for( t = 0; t < n - 1; ++t )
                {
                    x = k >= i ? k + 1 : k ;
                    y = t >= j ? t + 1 : t;

                    tmp[k][t] = src[x][y];
                }
            }

            dst[j][i]  =  calculate_A( tmp, n - 1 );

            if( ( i + j ) % 2 == 1 )
            {
                dst[j][i] = -1*dst[j][i];
            }
        }
    }
}
int calculate_A_inverse( double src[BMI160_MAX][BMI160_MAX], double dst[BMI160_MAX][BMI160_MAX], int n )
{
    double A = calculate_A( src, n );
    double tmp[BMI160_MAX][BMI160_MAX];
    int i, j;

    if ( fabs( A - 0 ) <= BMI160_E )
    {
        MFG_LOG_ERR(MFG_TAG_SENSOR,"Acc Calibate fail!\n");
        return 0;
    }

    calculate_A_adjoint( src, tmp, n );

    for( i = 0; i < n; ++i )
    {
        for( j = 0; j < n; ++j )
        {
            dst[i][j] = (double)( tmp[i][j] / A );
        }
    }

    return 1;
}
void print_M( double M[][BMI160_MAX], int n )
{
    int i, j;

    for ( i = 0; i < n; ++i )
    {
        for ( j = 0; j < n; ++j )
        {
           MFG_LOG_INFO(MFG_TAG_SENSOR,"%lf ", M[i][j]);
        }

        MFG_LOG_INFO(MFG_TAG_SENSOR,"\n");
    }
}
int test_bmi160(void)
{
    double test[BMI160_MAX][BMI160_MAX], dst[BMI160_MAX][BMI160_MAX];
    int n = 3;
    int is_exist;

    /**
     *  1, 0, 0,
     *  0, 2, 0,
     *  0, 0, 5
     */
    memset( test, 0, sizeof( test ) );

    test[0][0] = 1.0;
	test[0][1] = 2.0;
	test[0][2] = 3.0;

	test[1][0] = 2.0;
    test[1][1] = 2.0;
    test[1][2] = 1.0;

	test[2][0] = 3.0;
    test[2][1] = 4.0;
    test[2][2] = 3.0;




    is_exist = calculate_A_inverse( test, dst, n );

    if ( is_exist )
    {
        print_M(dst, n);
    }
    else
    {
        MFG_LOG_ERR(MFG_TAG_SENSOR,"Acc Calibate fail!\n");
    }

    return 0;
}

static BMI160_RETURN_FUNCTION_TYPE BMI160_X_Get_Sensitivity_k6(float *sensitivity )
{

	  us8 fullScale;

	  /* Read actual full scale selection from sensor. */
	  if (bmi160_get_accel_range(&fullScale) != BMI160_SUCCESS)
	  {
		return BMI160_ERROR;
	  }

	  /* Store the sensitivity based on actual full scale. */
	  switch( fullScale )
	  {
		case BMI160_ACCEL_RANGE_2G:
		  *sensitivity = ( float )0.061;
		  break;
		case BMI160_ACCEL_RANGE_4G:
		  *sensitivity = ( float )0.122;
		  break;
		case BMI160_ACCEL_RANGE_8G:
		  *sensitivity = ( float )0.244;
		  break;
		case BMI160_ACCEL_RANGE_16G:
		  *sensitivity = ( float )0.488;
		  break;
		default:
		  *sensitivity = -1.0f;
		  return BMI160_ERROR;
	  }

	  return BMI160_SUCCESS;
}

static BMI160_RETURN_FUNCTION_TYPE BMI160_X_Get_Axes_k6( SensorAxes_t_k6 *acceleration, struct bmi160_accel_t *accel)
{

	  float sensitivity = 0;

	  /* Get BMI160 actual sensitivity. */
	  if ( BMI160_X_Get_Sensitivity_k6( &sensitivity ) == COMPONENT_ERROR )
	  {
		return BMI160_ERROR;
	  }
	  /* Read raw data from BMI160 output register. */
	  if (bmi160_read_accel_xyz(accel) != BMI160_SUCCESS)
	  {
		return BMI160_ERROR;
	  }



	  /* Calculate the data. */
	  acceleration->AXIS_X = ( (accel->x) * (sensitivity) );
	  acceleration->AXIS_Y = ( (accel->y) * (sensitivity) );
	  acceleration->AXIS_Z = ( (accel->z) * (sensitivity) );

	  return BMI160_SUCCESS;
}

BMI160_RETURN_FUNCTION_TYPE  bmi160_read_accel_xup(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0,j = 0;
	SensorAxes_t_k6 acceleration;
	Save_str_k6	accel_tmp;
	uint8_t buffer[128];

	xup_x = 0;
	xup_y = 0;
	xup_z = 0;
	if(bmi160_mfg_init() != BMI160_SUCCESS) //bmi160 init
	{
		MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_xup fail\n");
		return BMI160_ERROR;
	}
	if(VR_flash_erase(FOTA_LOG_START,0x2)!=0)
	{
		return BMI160_ERROR;
	}
	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
	    for(i = 0,j = 0; i < accel_cal_count ; i++)
        {
           j++;
           com_rslt = BMI160_X_Get_Axes_k6(&acceleration,&accel_tmp.accel[j-1]);
           if (com_rslt != BMI160_SUCCESS)
           {
             MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_xup fail\n");
             return com_rslt;
           }
		   if(j%ACC_WRITEFLASH_SIZE == 0)
		   {
			 j = 0;
			 accel_tmp.start_id = ACC_XUP_FLASHID;
			 accel_tmp.end_id = ACC_XUP_FLASHID;
			 //write flash
			 if(VR_flash_write(&accel_tmp,ACC_XUP_FLASHID + ((i+1)/ACC_WRITEFLASH_SIZE - 1)*1024 ,sizeof(Save_str_k6))!=0)
			 {
				return BMI160_ERROR;
			 }
		   }
           	   xup_x += acceleration.AXIS_X;
		   xup_y += acceleration.AXIS_Y;
		   xup_z += acceleration.AXIS_Z;
           bmi160_delay_ms(2);/* bmi160_delay_ms in ms*/
    	}
	}
	else
	{
		 MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_xup fail\n");
		return com_rslt;
	}
	xup_x = xup_x/i;
	xup_y = xup_y/i;
	xup_z = xup_z/i;
	if((-700 < xup_x) ||(xup_x < -1300) )
	{
		 memset(buffer,'\0',128);
		 sprintf( (char *)buffer,"bmi160 accel:xup_x = %.1f xup_y = %.1f xup_z = %.1f\n",xup_x,xup_y,xup_z); // C4996
		 usb_app_transmit_data(buffer,strlen((char const *)buffer));
		 printf("%s",buffer);
		return BMI160_ERROR;
	}

	MFG_LOG_INFO(MFG_TAG_SENSOR,"bmi160 accel:xup_x = %f xup_y = %f xup_z = %f\n",
		xup_x,xup_y,xup_z);
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_read_accel_xdown(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0,j = 0;
	SensorAxes_t_k6 acceleration;
	Save_str_k6	accel_tmp;
	uint8_t buffer[128];

	xdown_x = 0;
	xdown_y = 0;
	xdown_z = 0;
	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
	    for(i = 0,j = 0; i < accel_cal_count ; i++)
        {
           j++;
           com_rslt = BMI160_X_Get_Axes_k6(&acceleration,&accel_tmp.accel[j-1]);
           if (com_rslt != BMI160_SUCCESS)
           {
             MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_xdown fail\n");
             return com_rslt;
           }
		   if(j%ACC_WRITEFLASH_SIZE == 0)
		   {
			 j = 0;
			 accel_tmp.start_id = ACC_XDOWN_FLASHID;
			 accel_tmp.end_id = ACC_XDOWN_FLASHID;
			 //write flash
			 if(VR_flash_write(&accel_tmp,ACC_XDOWN_FLASHID + ((i+1)/ACC_WRITEFLASH_SIZE - 1)*1024 ,sizeof(Save_str_k6))!=0)
			 {
				return BMI160_ERROR;
			 }
		   }
           xdown_x += acceleration.AXIS_X;
		   xdown_y += acceleration.AXIS_Y;
		   xdown_z += acceleration.AXIS_Z;
           bmi160_delay_ms(2);/* bmi160_delay_ms in ms*/
    	}
	}
	else
	{
		MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_xdown fail\n");
		return com_rslt;
	}
	xdown_x = xdown_x/i;
	xdown_y = xdown_y/i;
	xdown_z = xdown_z/i;

	if((xdown_x < 700) ||(xdown_x >1300) )
	{
		 memset(buffer,'\0',128);
		 sprintf( (char *)buffer,"bmi160 accel:xdown_x = %.1f xdown_y = %.1f xdown_z = %.1f\n",xdown_x,xdown_y,xdown_z);
		 usb_app_transmit_data(buffer,strlen((char const *)buffer));
		 printf("%s",buffer);
		return BMI160_ERROR;
	}
	MFG_LOG_INFO(MFG_TAG_SENSOR,"bmi160 accel:xdown_x = %f xdown_y = %f xdown_z = %f\n",
		xdown_x,xdown_y,xdown_z);
	return com_rslt;
}

BMI160_RETURN_FUNCTION_TYPE  bmi160_read_accel_yup(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0,j = 0;
	SensorAxes_t_k6 acceleration;
	Save_str_k6	accel_tmp;
	uint8_t buffer[128];

	yup_x = 0;
	yup_y = 0;
	yup_z = 0;
	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
	    for(i = 0,j = 0; i < accel_cal_count ; i++)
        {
           j++;
           com_rslt = BMI160_X_Get_Axes_k6(&acceleration,&accel_tmp.accel[j-1]);
           if (com_rslt != BMI160_SUCCESS)
           {
             MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_yup fail\n");
             return com_rslt;
           }
		   if(j%ACC_WRITEFLASH_SIZE == 0)
		   {
			 j = 0;
			 accel_tmp.start_id = ACC_YUP_FLASHID;
			 accel_tmp.end_id = ACC_YUP_FLASHID;
			 //write flash
			 if(VR_flash_write(&accel_tmp,ACC_YUP_FLASHID + ((i+1)/ACC_WRITEFLASH_SIZE - 1)*1024 ,sizeof(Save_str_k6))!=0)
			 {
				return BMI160_ERROR;
			 }
		   }
           yup_x += acceleration.AXIS_X;
		   yup_y += acceleration.AXIS_Y;
		   yup_z += acceleration.AXIS_Z;
           bmi160_delay_ms(2);/* bmi160_delay_ms in ms*/
    	}
	}
	else
	{
		 MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_yup fail\n");
		return com_rslt;
	}
	yup_x = yup_x/i;
	yup_y = yup_y/i;
	yup_z = yup_z/i;

	if((-700 < yup_y) ||(yup_y < -1300) )
	{
		 memset(buffer,'\0',128);
		 sprintf( (char *)buffer,"bmi160 accel:yup_x = %.1f yup_y = %.1f yup_z = %.1f\n",yup_x,yup_y,yup_z);
		 usb_app_transmit_data(buffer,strlen((char const *)buffer));
		 printf("%s",buffer);
		return BMI160_ERROR;
	}
	MFG_LOG_INFO(MFG_TAG_SENSOR,"bmi160 accel:yup_x = %f yup_y = %f yup_z = %f\n",
		yup_x,yup_y,yup_z);
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_read_accel_ydown(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0,j = 0;
	SensorAxes_t_k6 acceleration;
	Save_str_k6	accel_tmp;
	uint8_t buffer[128];

	ydown_x = 0;
	ydown_y = 0;
	ydown_z = 0;
	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
	    for(i = 0,j =0; i < accel_cal_count ; i++)
        {
           j++;
           com_rslt = BMI160_X_Get_Axes_k6(&acceleration,&accel_tmp.accel[j-1]);
           if (com_rslt != BMI160_SUCCESS)
           {
             MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_ydown fail\n");
             return com_rslt;
           }
		   if(j%ACC_WRITEFLASH_SIZE == 0)
		   {
			 j = 0;
			 accel_tmp.start_id = ACC_YDOWN_FLASHID;
			 accel_tmp.end_id = ACC_YDOWN_FLASHID;
			 //write flash
			 if(VR_flash_write(&accel_tmp,ACC_YDOWN_FLASHID + ((i+1)/ACC_WRITEFLASH_SIZE - 1)*1024 ,sizeof(Save_str_k6))!=0)
			 {
				return BMI160_ERROR;
			 }
		   }
           ydown_x += acceleration.AXIS_X;
		   ydown_y += acceleration.AXIS_Y;
		   ydown_z += acceleration.AXIS_Z;
           bmi160_delay_ms(2);/* bmi160_delay_ms in ms*/
    	}
	}
	else
	{
		MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_ydown fail\n");
		return com_rslt;
	}
	ydown_x = ydown_x/i;
	ydown_y = ydown_y/i;
	ydown_z = ydown_z/i;

	if((ydown_y < 700) ||(ydown_y > 1300) )
	{
		 memset(buffer,'\0',128);
		 sprintf( (char *)buffer,"bmi160 accel:ydown_x = %.1f ydown_y = %.1f ydown_z = %.1f\n",ydown_x,ydown_y,ydown_z);
		 usb_app_transmit_data(buffer,strlen((char const *)buffer));
		 printf("%s",buffer);
		return BMI160_ERROR;
	}
	MFG_LOG_INFO(MFG_TAG_SENSOR,"bmi160 accel:ydown_x = %f ydown_y = %f ydown_z = %f\n",
		ydown_x,ydown_y,ydown_z);
	return com_rslt;
}

BMI160_RETURN_FUNCTION_TYPE  bmi160_read_accel_zup(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0,j = 0;
	SensorAxes_t_k6 acceleration;
	Save_str_k6	accel_tmp;
	uint8_t buffer[128];

	zup_x = 0;
	zup_y = 0;
	zup_z = 0;
	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
	    for(i = 0,j = 0; i < accel_cal_count ; i++)
        {
           j++;
           com_rslt = BMI160_X_Get_Axes_k6(&acceleration,&accel_tmp.accel[j-1]);
           if (com_rslt != BMI160_SUCCESS)
           {
             MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_zup fail\n");
             return com_rslt;
           }
		   if(j%ACC_WRITEFLASH_SIZE == 0)
		   {
			 j = 0;
			 accel_tmp.start_id = ACC_ZUP_FLASHID;
			 accel_tmp.end_id = ACC_ZUP_FLASHID;
			 //write flash
			 if(VR_flash_write(&accel_tmp,ACC_ZUP_FLASHID + ((i+1)/ACC_WRITEFLASH_SIZE - 1)*1024 ,sizeof(Save_str_k6))!=0)
			 {
				return BMI160_ERROR;
			 }
		   }
           zup_x += acceleration.AXIS_X;
		   zup_y += acceleration.AXIS_Y;
		   zup_z += acceleration.AXIS_Z;
           bmi160_delay_ms(2);/* bmi160_delay_ms in ms*/
    	}
	}
	else
	{
		 MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_zup fail\n");
		return com_rslt;
	}
	zup_x = zup_x/i;
	zup_y = zup_y/i;
	zup_z = zup_z/i;

	if((-700 < zup_z) ||(zup_z < -1300) )
	{
		 memset(buffer,'\0',128);
		 sprintf( (char *)buffer,"bmi160 accel:zup_x = %.1f zup_y = %.1f zup_z = %.1f\n",zup_x,zup_y,zup_z);
		 usb_app_transmit_data(buffer,strlen((char const *)buffer));
		 printf("%s",buffer);
		return BMI160_ERROR;
	}
	MFG_LOG_INFO(MFG_TAG_SENSOR,"bmi160 accel:zup_x = %f zup_y = %f zup_z = %f\n",
		zup_x,zup_y,zup_z);
	return com_rslt;
}
BMI160_RETURN_FUNCTION_TYPE bmi160_read_accel_zdown(void)
{
	BMI160_RETURN_FUNCTION_TYPE com_rslt = BMI160_ERROR;
	us8 v_accel_power_mode = 0;
	int i = 0,j = 0;
	SensorAxes_t_k6 acceleration;
	Save_str_k6	accel_tmp;
	uint8_t buffer[128];


	zdown_x = 0;
	zdown_y = 0;
	zdown_z = 0;
	com_rslt = bmi160_get_accel_power_mode_stat(&v_accel_power_mode);
	if((com_rslt == BMI160_SUCCESS) && (v_accel_power_mode == 1))
	{
	    for(i = 0 ; i < accel_cal_count ; i++)
        {
             j++;
           com_rslt = BMI160_X_Get_Axes_k6(&acceleration,&accel_tmp.accel[j-1]);
           if (com_rslt != BMI160_SUCCESS)
           {
             MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_zup fail\n");
             return com_rslt;
           }
		   if(j%ACC_WRITEFLASH_SIZE == 0)
		   {
			 j = 0;
			 accel_tmp.start_id = ACC_ZDOWN_FLASHID;
			 accel_tmp.end_id = ACC_ZDOWN_FLASHID;
			 //write flash
			 if(VR_flash_write(&accel_tmp,ACC_ZDOWN_FLASHID + ((i+1)/ACC_WRITEFLASH_SIZE - 1)*1024 ,sizeof(Save_str_k6))!=0)
			 {
				return BMI160_ERROR;
			 }
		   }
           zdown_x += acceleration.AXIS_X;
		   zdown_y += acceleration.AXIS_Y;
		   zdown_z += acceleration.AXIS_Z;
           bmi160_delay_ms(2);/* bmi160_delay_ms in ms*/
    	}
	}
	else
	{
		MFG_LOG_ERR(MFG_TAG_SENSOR,"bmi160 read accel_zdown fail\n");
		return com_rslt;
	}
	zdown_x = zdown_x/i;
	zdown_y = zdown_y/i;
	zdown_z = zdown_z/i;

	if(( zdown_z < 700) ||(zdown_z > 1300) )
	{
		 memset(buffer,'\0',128);
		 sprintf( (char *)buffer,"bmi160 accel:zdown_x = %.1f zdown_y = %.1f zdown_z = %.1f\n",zdown_x,zdown_y,zdown_z);
		 usb_app_transmit_data(buffer,strlen((char const *)buffer));
		 printf("%s",buffer);
		return BMI160_ERROR;
	}
	MFG_LOG_INFO(MFG_TAG_SENSOR,"bmi160 accel:zdown_x = %f zdown_y = %f zdown_z = %f\n",
		zdown_x,zdown_y,zdown_z);
	return 0;
}


BMI160_RETURN_FUNCTION_TYPE bmi160_accel_k6andtest(void)
{
	if(bmi160_ACC_Calibate_6() !=0)
	{
		usb_app_transmit_data("matrix fail\n",strlen("matrix fail\n"));
		return BMI160_ERROR;
	}

	//test
	gsensor_info("cal test\n");

	if(bmi160_ACC_fd_test() !=0)
	{
		return BMI160_ERROR;
	}

	return 0;
}

BMI160_RETURN_FUNCTION_TYPE bmi160_ACC_Calibate_6(void)
{
	BMI160_DataTypeDef acc_calvalue = {0};
	double test[BMI160_MAX][BMI160_MAX], dst[BMI160_MAX][BMI160_MAX];
	int is_exist = 0;
	int i = 0,j = 0;

	#if 0
	test[0][0] = (double)((xdown_x - xup_x)/2);// 1000mg  kxx
	test[0][1] = (double)((xdown_y - xup_y)/2);// kxy
	test[0][2] = (double)((xdown_z - xup_z)/2);// kxz
	acc_calvalue.accel_x= (xdown_x + xup_x)/2;

	test[1][0] = (double)((ydown_x - yup_x)/2);// kyx
	test[1][1] = (double)((ydown_y - yup_y)/2);// 1000mg  kyy
	test[1][2] = (double)((ydown_z - yup_z)/2);// kyz
	acc_calvalue.accel_y= (ydown_y + yup_y)/2;

	test[2][0] = (double)((zdown_x - zup_x)/2);// kzx
	test[2][1] = (double)((zdown_y - zup_y)/2);// kzy
	test[2][2] = (double)((zdown_z - zup_z)/2);// 1000mg  kzz
	acc_calvalue.accel_z= (zdown_z + zup_z)/2;
	#endif

	test[0][0] = (double)((xdown_x - xup_x)/2);// 1000mg  kxx
	test[1][0] = (double)((xdown_y - xup_y)/2);// kxy
	test[2][0] = (double)((xdown_z - xup_z)/2);// kxz
	acc_calvalue.accel_x= (xdown_x + xup_x)/2;

	test[0][1] = (double)((ydown_x - yup_x)/2);// kyx
	test[1][1] = (double)((ydown_y - yup_y)/2);// 1000mg  kyy
	test[2][1] = (double)((ydown_z - yup_z)/2);// kyz
	acc_calvalue.accel_y= (ydown_y + yup_y)/2;

	test[0][2] = (double)((zdown_x - zup_x)/2);// kzx
	test[1][2] = (double)((zdown_y - zup_y)/2);// kzy
	test[2][2] = (double)((zdown_z - zup_z)/2);// 1000mg  kzz
	acc_calvalue.accel_z= (zdown_z + zup_z)/2;

	is_exist = calculate_A_inverse( test, dst, 3 );
	for( i = 0; i < 3; ++i )
    {
        for( j = 0; j < 3; ++j )
        {
           dst[i][j] = 1000 * dst[i][j];
        }
    }
	if ( is_exist )
    {
        print_M(dst, 3);
    }
    else
    {
        MFG_LOG_ERR(MFG_TAG_SENSOR,"Acc Calibate fail!\n");
		return BMI160_ERROR;
    }
    for( i = 0; i < 3; ++i )
    {
        for( j = 0; j < 3; ++j )
        {
            acc_calvalue.accel_k_xyz[i][j] = dst[i][j];
        }
    }
	saveAccelOffsetValue((void *)(&acc_calvalue));
	MFG_LOG_INFO(MFG_TAG_SENSOR,"SaveBmi160Data accel:offsetx = %f offsety = %f offsetz =%f\n",
		acc_calvalue.accel_x,acc_calvalue.accel_y,acc_calvalue.accel_z);

	return 0;
}
