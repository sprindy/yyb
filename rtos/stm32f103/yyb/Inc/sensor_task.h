#ifndef __SENSOR_TASK_H
#define __SENSOR_TASK_H

#ifdef __cplusplus
extern "C" {
#endif



/* Includes ------------------------------------------------------------------*/
#include "BMI160_ACC_GYRO_Driver_Api.h"
#include "x_nucleo_iks01a1_accelero.h"

#define SENSOR_LATENCY_TEST     0
#define SENSOR_LATENCY_TEST_COUNT     200
#define SENSOR_LATENCY_TEST_POINT     4



/* Exported types ------------------------------------------------------------*/
typedef enum
{
	ACC = 0,     /* Acceleration sensor  */
	GYRO,        /* Gyroscope sensor. */
	MAG		/* Magnetic sensor. */
} SENSOR_ID_t;

//extern osThreadId sensorTaskHandle;
//extern BaseType_t xSensorTaskWoken;

/* Exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void sensorTaskInit(void);
void sensorIsr(uint16_t GPIO_Pin);
void Sensor_SW_Reset(void);
void gsensormfgInitial(void);
#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_TASK_H */
