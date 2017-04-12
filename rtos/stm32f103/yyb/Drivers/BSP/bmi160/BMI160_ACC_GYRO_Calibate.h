/* user defined code to be added here ... */
#ifndef __BMI160_Calibate_H__
#define __BMI160_Calibate_H__
#include "BMI160_ACC_GYRO_Driver.h"
#include "flash_define.h"


#define BMI160_MAX 3
#define BMI160_E 0.000000001
#define ACC_READ_XUP "xup"
#define ACC_READ_XDOWN "xdown"
#define ACC_READ_YUP "yup"
#define ACC_READ_YDOWN "ydown"
#define ACC_READ_ZUP "zup"
#define ACC_READ_ZDOWN "zdown"


typedef struct
{
	char isAccValid;// 1 is valid
	char isGyroValid;// 1 is valid
	int gyro_x;
	int gyro_y;
	int gyro_z;

	float accel_x;
	float accel_y;
	float accel_z;
	float accel_k_xyz[BMI160_MAX][BMI160_MAX];
} BMI160_DataTypeDef;
typedef struct
{
  float AXIS_X;
  float AXIS_Y;
  float AXIS_Z;
} SensorAxes_t_k6;
#define accel_cal_count 3000
#define accel_cal_count 3000
#define FOTA_LOG_START  0
#define ACC_XUP_FLASHID 	FOTA_LOG_START
#define ACC_XDOWN_FLASHID	  (FOTA_LOG_START + 30720)
#define ACC_YUP_FLASHID		(FOTA_LOG_START + 61440)
#define ACC_YDOWN_FLASHID	 (FOTA_LOG_START + 92160)
#define ACC_ZUP_FLASHID		(FOTA_LOG_START + 122880)
#define ACC_ZDOWN_FLASHID		(FOTA_LOG_START + 153600)
//#define ACC_XDOWN_FLASHID	6

#define ACC_WRITEFLASH_SIZE 100
typedef struct
{
	int start_id;
	struct bmi160_accel_t accel[ACC_WRITEFLASH_SIZE];
	int end_id;
}Save_str_k6;
BMI160_RETURN_FUNCTION_TYPE bmi160_ACC_Calibate(unsigned char *buf);
BMI160_RETURN_FUNCTION_TYPE bmi160_Gyro_Calibate(void);
int bmi160_mfg_init(void);
double calculate_A( double src[][BMI160_MAX], int n );
void calculate_A_adjoint( double src[BMI160_MAX][BMI160_MAX], double dst[BMI160_MAX][BMI160_MAX], int n );
int calculate_A_inverse( double src[BMI160_MAX][BMI160_MAX], double dst[BMI160_MAX][BMI160_MAX], int n );
void print_M( double M[][BMI160_MAX], int n );
int test_bmi160(void);
BMI160_RETURN_FUNCTION_TYPE bmi160_ACC_Calibate_6(void);
BMI160_RETURN_FUNCTION_TYPE  bmi160_read_accel_xup(void);
BMI160_RETURN_FUNCTION_TYPE bmi160_read_accel_xdown(void);
BMI160_RETURN_FUNCTION_TYPE  bmi160_read_accel_yup(void);
BMI160_RETURN_FUNCTION_TYPE bmi160_read_accel_ydown(void);
BMI160_RETURN_FUNCTION_TYPE  bmi160_read_accel_zup(void);
BMI160_RETURN_FUNCTION_TYPE bmi160_read_accel_zdown(void);
BMI160_RETURN_FUNCTION_TYPE bmi160_accel_k6andtest(void);

#endif
