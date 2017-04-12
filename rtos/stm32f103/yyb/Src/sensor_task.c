/*
 ******************************************************************************
 * File Name          : sensor_task.c
 * Description        : Sensor task to get sensor data
 ******************************************************************************
 *
 * COPYRIGHT(c) 2016 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "sensor_task.h"
#include "gpio.h"

static void *BMI160_X_0_handle = NULL;
/* static void *BMI160_G_0_handle = NULL; */
extern SPI_HandleTypeDef hspi1;
/* extern pcbid_t pcb_id; */
osThreadId sensorTaskHandle;
BaseType_t xSensorTaskWoken = pdFALSE;

void StartSensorTask(void const * argument);
void sensorInitial(SENSOR_ID_t sensor_id);
void sensorEnable(SENSOR_ID_t sensor_id,int enable);
void sensorSetOdr(SENSOR_ID_t sensor_id, SensorOdr_t odr);

#ifdef CARDBOARD_FUSION
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct quat_t_ {
    double w;
    double x;
    double y;
    double z;
} quat_t ;

struct rawdata_buf {
    double x[5], y[5], z[5];
    double sum[3];
    size_t index;
};
void rawdata_buf_append(struct rawdata_buf* raw, double x, double y, double z)
{
    raw->sum[0] += -raw->x[raw->index] + x;
    raw->sum[1] += -raw->y[raw->index] + y;
    raw->sum[2] += -raw->z[raw->index] + z;
    raw->x[raw->index] = x;
    raw->y[raw->index] = y;
    raw->z[raw->index] = z;
    raw->index = (raw->index + 1) % 5;
}
void rawdata_buf_average(struct rawdata_buf* raw, double* x, double* y,
                         double* z)
{
    *x = raw->sum[0] / 5;
    *y = raw->sum[1] / 5;
    *z = raw->sum[2] / 5;
}

//#define SENSOR_TASK_DUMP_SAMPLE_RATE_LOG

#ifdef SENSOR_TASK_DUMP_SAMPLE_RATE_LOG
#define SENSOR_TASK_DUMP_DURING		configTICK_RATE_HZ
static osTimerDef_t dump_sample_rate_timer_def = { 0 };
static osTimerId dump_sample_rate_timer_id = { 0 };
static int count_imu = 0;
static long long TimeStamp_E = 0;

static void sensor_task_dump_sample_rate_log(void const *argument)
{
#ifdef PLUGIN
	printf("IMU Sample rate = %d (times/sec), Spent Fusion time %f ms\r\n",
				count_imu, (float)TimeStamp_E / count_imu);
	count_imu = 0;
	TimeStamp_E = 0;
#endif
}

static void sensor_task_dump_sample_rate_init()
{
	dump_sample_rate_timer_def.ptimer = sensor_task_dump_sample_rate_log;
	dump_sample_rate_timer_id = osTimerCreate(
		&dump_sample_rate_timer_def,
		osTimerPeriodic,
		NULL);
	osTimerStart(dump_sample_rate_timer_id, SENSOR_TASK_DUMP_DURING);
}
#endif /* SENSOR_TASK_DUMP_REPORT_RATE_LOG */

static double power(double base, int n)
{
    int i;
    double pow = 1.0;

    for (i = 1; i <= n; i++) {
        pow = pow * base;
    }

    return pow;
}
/*
static double QFormatIntegerToFloat(const int x,
                                    const int n)
{
    return (((double)x) / ((double)(1 << n)));
}
*/
static short FloatToQFormatInteger(const double x,
                                   const int m, const int n)
{
    return (((x) < (0.0)) ? ((int)(power(2, n) * (x) - 0.5)) : (min(32767,
            (int)((32767.0 / (power(2, m))) * (x) + 0.5))));
}
/*
static double clamp(double v)
{
    return v < 0 ? 0 : v;
}

static void matrixToQuat(double* R, quat_t* q)
{
    const double Hx = R[0];
    const double My = R[4];
    const double Az = R[8];

    q->x = sqrtf(clamp(Hx - My - Az + 1) * 0.25f);
    q->y = sqrtf(clamp(-Hx + My - Az + 1) * 0.25f);
    q->z = sqrtf(clamp(-Hx - My + Az + 1) * 0.25f);
    q->w = -sqrtf(clamp(Hx + My + Az + 1) * 0.25f);
    q->x = -copysignf(q->x, R[7] - R[5]);
    q->y = -copysignf(q->y, R[2] - R[6]);
    q->z = -copysignf(q->z, R[3] - R[1]);
    // guaranteed to be unit-quaternion
}

static void quatToMatrix(quat_t* q, double* R)
{
    double q0 = q->w; // TYPE q0(q.w);
    double q1 = q->x; // TYPE q1(q.x);
    double q2 = q->y; // TYPE q2(q.y);
    double q3 = q->z; // TYPE q3(q.z);
    double sq_q1 = 2 * q1 * q1;
    double sq_q2 = 2 * q2 * q2;
    double sq_q3 = 2 * q3 * q3;
    double q1_q2 = 2 * q1 * q2;
    double q3_q0 = 2 * q3 * q0;
    double q1_q3 = 2 * q1 * q3;
    double q2_q0 = 2 * q2 * q0;
    double q2_q3 = 2 * q2 * q3;
    double q1_q0 = 2 * q1 * q0;

    R[0] = 1 - sq_q2 - sq_q3;
    R[1] = q1_q2 - q3_q0;
    R[2] = q1_q3 + q2_q0;
    R[3] = q1_q2 + q3_q0;
    R[4] = 1 - sq_q1 - sq_q3;
    R[5] = q2_q3 - q1_q0;
    R[6] = q1_q3 - q2_q0;
    R[7] = q2_q3 + q1_q0;
    R[8] = 1 - sq_q1 - sq_q2;
}


static void QuaternionToEuler(float *data, float *q)
{

     quaternion[offset + 0] = x;
     quaternion[offset + 1] = y;
     quaternion[offset + 2] = z;
     quaternion[offset + 3] = w;


     // yaw:
     data[0] = (float)atan2(2 * (q[0] * q[1] + q[3] * q[2]),
                                 q[3] * q[3] + q[0] * q[0] - q[1] * q[1] - q[2] * q[2]);
     // pitch:
     data[1] = (float)asin(-2 * (q[0] * q[2] - q[3] * q[1]));

     // roll:
     data[2] = (float)atan2(2 * (q[1] * q[2] + q[3] * q[0]),
                                 q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2]);
}
*/
float convertRawAcceleration(float aRaw)
{
    // since we are using 2G range
    // -2g maps to a raw value of -32768
    // +2g maps to a raw value of 32767

    float a = (aRaw * 2.0) / 32768.0;
    return a;
}

float convertRawGyro(float gRaw)
{
    // since we are using 250 degrees/seconds range
    // -250 maps to a raw value of -32768
    // +250 maps to a raw value of 32767

    float g = (gRaw * 245.0) / 32768.0;
    return g;
}
#endif
/* extern void bmi160_postinit(void); */
extern unsigned int g_MFGModeFlag;
#if SENSOR_LATENCY_TEST
#define  DWT_CR                 *(volatile uint32_t *)0xE0001000
#define  DWT_CYCCNT             *(volatile uint32_t *)0xE0001004
#define  DEM_CR                 *(volatile uint32_t *)0xE000EDFC
#define  DBGMCU_CR              *(volatile uint32_t *)0xE0042004
#define  DEM_CR_TRCENA          (1 << 24)
#define  DWT_CR_CYCCNTENA       (1 <<  0)

uint32_t latency_data[SENSOR_LATENCY_TEST_COUNT][SENSOR_LATENCY_TEST_POINT] =
{0};
volatile uint32_t latency_index = 0;
uint32_t latency_lock = 0;


void sensor_latency_test_init(void)
{
    DEM_CR         |=  DEM_CR_TRCENA;
    DWT_CR         &= ~DWT_CR_CYCCNTENA;
    DWT_CYCCNT      = 0u;
    DWT_CR         |= DWT_CR_CYCCNTENA;
    latency_index = 0;
}

#endif
void sensorTaskInit(void)
{
	Sensor_SW_Reset();
	sensorInitial(ACC);
	/* sensorInitial(GYRO); */
	/* sensorInitial(MAG); */
	/* sensorSetOdr(MAG,ODR_HIGH); */
	/* sensorEnable(MAG,1); */
	sensorEnable(ACC,1);//range:2g
	/* sensorEnable(GYRO,1);//range:2000 */
	sensorSetOdr(ACC, ODR_MID_LOW);//output data rate:100hz
	/* sensorSetOdr(GYRO,ODR_MID_LOW);//output data rate:100hz */

	osThreadDef(sensorTask, StartSensorTask, osPriorityNormal, 0, 256);
	sensorTaskHandle = osThreadCreate(osThread(sensorTask), NULL);
	if(NULL == sensorTaskHandle)
		printf("********%s: sensorTask create fail**********\r\n", __func__);

	/* bmi160_postinit(); */
#ifdef SENSOR_TASK_DUMP_SAMPLE_RATE_LOG
	sensor_task_dump_sample_rate_init();
#endif /* SENSOR_TASK_DUMP_SAMPLE_RATE_LOG*/
#if SENSOR_LATENCY_TEST
    sensor_latency_test_init();
#endif
}
void sensorIsr(uint16_t GPIO_Pin)
{
	if(sensorTaskHandle) {
        #if SENSOR_LATENCY_TEST
        latency_data[latency_index][0] = DWT_CYCCNT;
        #endif
		xTaskNotifyFromISR(sensorTaskHandle, GPIO_Pin, eSetBits, &xSensorTaskWoken);
		portYIELD_FROM_ISR(xSensorTaskWoken);
	}
}
#if 0
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_8){
		printf("Ecompass interrupt enter\n");
		vTaskNotifyGiveFromISR(sensorTaskHandle, &xSensorTaskWoken);
		portYIELD_FROM_ISR(xSensorTaskWoken);
	}
}
#endif
#if 0
/*mfg initial sensor*/
void gsensormfgInitial(void)
{
	BSP_ACCELERO_Init( BMI160_X_0, &BMI160_X_0_handle);
	BSP_GYRO_Init( BMI160_G_0, &BMI160_G_0_handle );
}
#endif
/*initial sensor*/
void sensorInitial(SENSOR_ID_t sensor_id)
{
	switch (sensor_id)
	{
		case ACC :
			if(BSP_ACCELERO_Init( BMI160_X_0, &BMI160_X_0_handle) == COMPONENT_ERROR )
			{
				gsensor_err("G-Sensor init fail\n");
			}
			break;
#if 0
		case GYRO :
			if(BSP_GYRO_Init( BMI160_G_0, &BMI160_G_0_handle ) == COMPONENT_ERROR )
			{
				gsensor_err("Gyro Sensor init fail\n");
			}
			break;
#endif
		default :
			break;
	}
}
/*enable sensor*/
void sensorEnable(SENSOR_ID_t sensor_id,int enable)
{
	switch (sensor_id)
	{
		case ACC :
			  if(BSP_ACCELERO_Sensor_Enable( BMI160_X_0_handle ) == COMPONENT_ERROR )
              {
	             gsensor_err("G-Sensor enable fail\n");
              }
			break;
#if 0
		case GYRO :
			  if(BSP_GYRO_Sensor_Enable( BMI160_G_0_handle ) == COMPONENT_ERROR )
 			  {
				gsensor_err("Gyro Sensor enable fail\n");
  			  }
			break;
#endif
		default :
			break;
	}


}
/*set sensor ODR*/
void sensorSetOdr(SENSOR_ID_t sensor_id, SensorOdr_t odr)
{
	switch (sensor_id)
	{
		case ACC :
			BSP_ACCELERO_Set_ODR( BMI160_X_0_handle, odr );
			break;
#if 0
		case GYRO :
			BSP_GYRO_Set_ODR( BMI160_G_0_handle, odr );
			break;
#endif
		default :
			break;
	}

}

void sensorGetData(SENSOR_ID_t sensor_id, SensorAxes_t *sensor_data)
{
	/*set sensor ODR*/
	switch (sensor_id)
	{
		case ACC :
			BSP_ACCELERO_Get_Axes( BMI160_X_0_handle, sensor_data);
			break;
		default :
			break;
	}

}
void Sensor_SW_Reset(void)
{
    uint8_t setting[8] = {0};
	setting[0] = 0x7e & 0x7f;//register 0x7e mask 0x7f
	setting[1] = 0xB6;
	HAL_SPI_Transmit(&hspi1,setting,2, 1000);
}

/* StartSensorTask function */
void StartSensorTask(void const * argument)
{
    /* struct rawdata_buf raw_buf = { 0 }; */
    /* int count_m = 0; */
    /* bool flag_m = false; */
    /* double avgx, avgy, avgz; */

	SensorAxes_t acceleration;
	/* SensorAxes_gyro_t angular_velocity; */
	uint32_t ulNotificationValue;
	uint8_t DrdyFlag;
	printf("********%s**********\r\n", __func__);
	for(;;)
	{
		//ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
		xTaskNotifyWait(0,0xFFFFFFFF, &ulNotificationValue, portMAX_DELAY );
		xSensorTaskWoken = pdFALSE;
        #if SENSOR_LATENCY_TEST
        latency_data[latency_index][1] = DWT_CYCCNT;
        #endif
		if( ((ulNotificationValue & GPIO_PIN_BMI160_INT0) == GPIO_PIN_BMI160_INT0) ||
		    ((ulNotificationValue & GPIO_PIN_BMI160_INT1) == GPIO_PIN_BMI160_INT1) ) {
			bmi160_read_reg(BMI160_USER_STAT_ADDR,&DrdyFlag,1);
			if ((DrdyFlag & 0xC0) == 0xC0) {
				sensorGetData(ACC,&acceleration);
				/* BSP_GYRO_Get_Axes(BMI160_G_0_handle, &angular_velocity); */

				gsensor_info("Acc x = %d y = %d z = %d\n",
					acceleration.AXIS_X, acceleration.AXIS_Y, acceleration.AXIS_Z);
			#if 0
				gsensor_info("GYRO x = %f y = %f z = %f\n",
					angular_velocity.AXIS_X, angular_velocity.AXIS_Y, angular_velocity.AXIS_Z);
			#endif
			}
			#if 0
			if ((DrdyFlag & 0x20) == 0x20) {
				sensorGetData(MAG, &MAG_Value);
				BSP_MAGNETO_Get_Accuracy(MAGNETO_handle,&accuracy_m);
				mag_info("mag x = %d y = %d z = %d\n",
					MAG_Value.AXIS_X, MAG_Value.AXIS_Y, MAG_Value.AXIS_Z);
			}
			#endif
			#if SENSOR_LATENCY_TEST
            latency_data[latency_index][2] = DWT_CYCCNT;
            #endif
#if 0
			TimeStamp_G = HAL_GetTick();
            TimeStamp_B = TimeStamp_G; // just for build warning
			bmi160_get_sensor_time(&TimeStamp_B);
			TimeStamp_H = (float)TimeStamp_B * 0.039;
#endif
		}
#ifdef CARDBOARD_FUSION
        if(UNKNOWN == pcb_id){
            ax = -((float)acceleration.AXIS_Y * 9.8 / 1000.0);
            ay = ((float)acceleration.AXIS_X * 9.8 / 1000.0);
            az = ((float)acceleration.AXIS_Z * 9.8 / 1000.0);

            // Degree transfer to Rad
            gx = -(((float)angular_velocity.AXIS_Y)) * 3.14 / 180.0;
            gy = (((float)angular_velocity.AXIS_X)) * 3.14 / 180.0;
            gz = (((float)angular_velocity.AXIS_Z)) * 3.14 / 180.0;

            mx = ((float)MAG_Value.AXIS_X / 1000.0);
            my = ((float)MAG_Value.AXIS_Y / 1000.0);
            mz = ((float)MAG_Value.AXIS_Z / 1000.0);
        } else {
            ax = -((float)acceleration.AXIS_Y * 9.8 / 1000.0);
            ay = ((float)acceleration.AXIS_X * 9.8 / 1000.0);
            az = ((float)acceleration.AXIS_Z * 9.8 / 1000.0);

            // Degree transfer to Rad
            gx = -(((float)angular_velocity.AXIS_Y)) * 3.14 / 180.0;
            gy = (((float)angular_velocity.AXIS_X)) * 3.14 / 180.0;
            gz = (((float)angular_velocity.AXIS_Z)) * 3.14 / 180.0;

            mx = ((float)MAG_Value.AXIS_X / 1000.0);
            my = ((float)MAG_Value.AXIS_Y / 1000.0);
            mz = ((float)MAG_Value.AXIS_Z / 1000.0);
        }
            //printf("ACC: %f,%f,%f Gyro: %f,%f,%f TimeStamp: %f \n", ax, ay, az, gx, gy, gz, TimeStamp_H);
            //printf("%f,%f,%f accuracy_m %d \n", mx, my, mz, accuracy_m);

            /* rawdata_buf_append(&raw_buf, gx, gy, gz); */
            count_m++;
    #ifdef SENSOR_TASK_DUMP_SAMPLE_RATE_LOG
        TimeStamp_S = HAL_GetTick();
    #endif /* SENSOR_TASK_DUMP_SAMPLE_RATE_LOG */
            accelEvent.sensorType = 1;
            accelEvent.values[0] = ax;
            accelEvent.values[1] = ay;
            accelEvent.values[2] = az;
            accelEvent.timestamp = (long long)(TimeStamp_H * 1000000); //TimeStamp_G * 1000000;
            onSensorChanged(&accelEvent, headTracker);

            if(accuracy_m > 2) {
                MagEvent.sensorType = 2;
                MagEvent.values[0] = mx;
                MagEvent.values[1] = my;
                MagEvent.values[2] = mz;
                MagEvent.timestamp = TimeStamp_M * 1000000;
                onSensorChanged(&MagEvent, headTracker);
            }
#if 0
            if(count_m == 5 || flag_m) {
            flag_m = true;
            /* rawdata_buf_average(&raw_buf, &avgx, &avgy, &avgz); */
            gyroEvent.sensorType = 4;
            gyroEvent.values[0] = avgx;
            gyroEvent.values[1] = avgy;
            gyroEvent.values[2] = avgz;
            gyroEvent.timestamp = (long long)(TimeStamp_H * 1000000); //TimeStamp_G * 1000000;
            onSensorChanged(&gyroEvent, headTracker);
            }
#endif

    #ifdef PLUGIN
        imu_t imu_data;
        double R[9] = {0.0};
        double last_gyro[3] = {0.0};
        quat_t q;
        float headRotation[4];

        if(accuracy_m_temp != accuracy_m) {
          printf("accuracy_m %d \n", accuracy_m);
        }
        accuracy_m_temp = accuracy_m;

        getLastGyro(last_gyro, headTracker);
        getSensorFromWorld(R, headTracker);

        ble_getLastHeadView(getHeadView_l(headTransform), 16, 0, headTracker);
        getQuaternion(headRotation, 4, 0, headTransform);
        #ifdef SENSOR_TASK_DUMP_SAMPLE_RATE_LOG
        TimeStamp_E = (HAL_GetTick() - TimeStamp_S) + TimeStamp_E;
        count_imu++;
        #endif /* SENSOR_TASK_DUMP_SAMPLE_RATE_LOG */
/*
         printf("%f, %f, %f, %f\n",
                headRotation[3], headRotation[0], headRotation[1], headRotation[2]);
*/
        q.w = headRotation[3];
        q.x = headRotation[0];
        q.y = headRotation[1];
        q.z = headRotation[2];
/*
        QuaternionToEuler(eulerAngles, headRotation);
        printf("Yaw: %f; Pitch: %f;Roll: %f : %d\n",
                ((eulerAngles[0] * 180.0) / 3.14),
                ((eulerAngles[1] * 180.0) / 3.14),
                ((eulerAngles[2] * 180.0) / 3.14), accuracy_m);
*/
        // quaternion to Q14, -2?x?1.9999390
        imu_data.q_w = FloatToQFormatInteger(q.w , 1, 14);
        imu_data.q_x = FloatToQFormatInteger(q.x , 1, 14);
        imu_data.q_y = FloatToQFormatInteger(q.y , 1, 14);
        imu_data.q_z = FloatToQFormatInteger(q.z , 1, 14);

        // gyro to Q9, -64?x?63.9980469
        imu_data.g_x = FloatToQFormatInteger(last_gyro[0] , 6, 9);
        imu_data.g_y = FloatToQFormatInteger(last_gyro[1] , 6, 9);
        imu_data.g_z = FloatToQFormatInteger(last_gyro[2] , 6, 9);
        #if SENSOR_LATENCY_TEST
        latency_data[latency_index][3] = DWT_CYCCNT;
        if(!latency_lock){
            if(++latency_index >= SENSOR_LATENCY_TEST_COUNT){
                latency_index = 0;
            }
        }
        #endif

        SendImuDataToPhone((uint8_t *)&imu_data, sizeof(imu_t));

    #else
            getLastHeadView(getHeadView_l(headTransform), 16, 0, headTracker);
        #ifdef NORMAL
         float headRotation[4];
         getQuaternion(headRotation, 4, 0, headTransform);
         QuaternionToEuler(eulerAngles, headRotation);
            #if 0
         printf("%f;%f;%f;%f;0;0 \n",
                headRotation[3], headRotation[0], headRotation[1], headRotation[2]);
            #else
/*
         printf("%f;%f;%f;0;0;0 \n",
                (-(eulerAngles[0] * 180.0) / 3.14),
                (-(eulerAngles[1] * 180.0) / 3.14),
                ((eulerAngles[2] * 180.0) / 3.14));

         printf("Yaw: %f; Pitch: %f;Roll: %f : %d\n",
                ((eulerAngles[0] * 180.0) / 3.14),
                ((eulerAngles[1] * 180.0) / 3.14),
                ((eulerAngles[2] * 180.0) / 3.14),
                  accuracy_m);
*/
            #endif
        #else

        getEulerAngles(eulerAngles, 3, 0 , headTransform);

        printf("pitch %f :", (eulerAngles[0] * 180.0) / 3.14);
        printf("yaw %f :", (eulerAngles[1] * 180.0) / 3.14);
        printf("roll %f : %d \n", (eulerAngles[2] * 180.0) / 3.14, accuracy_m);
        #endif // NORMAL
    #endif // PLUGIN
#endif // CARDBOARD_FUSION
	}
}
