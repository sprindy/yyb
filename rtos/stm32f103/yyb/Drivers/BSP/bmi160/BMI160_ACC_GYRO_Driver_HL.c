#include "BMI160_ACC_GYRO_Driver_HL.h"
#include "gpio.h"

extern SPI_HandleTypeDef hspi1;
uint32_t Spi2Timeout = BMI160_SPI1_TIMEOUT_MAX;

static void BMI160_IO_Init(void);
static void SPI1_MspInit(SPI_HandleTypeDef *hspi);
static void SPI1_Init(void);
static void SPI1_Error(void);


static void  BMI160_IO_Init(void)
{
  bmi160_spi_cs();
  SPI1_Init();
}

static void SPI1_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance==SPI1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  }
}
/**
  * @brief  Initializes SPI HAL.
  * @param  None
  * @retval None
  */
static void SPI1_Init(void)
{
  if(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_RESET)
  {
    /* SPI Config */
    hspi1.Instance = BMI160_SPI1;
      /* SPI baudrate is set to 12,5 MHz maximum (PCLK2/SPI_BaudRatePrescaler = 100/8 = 12,5 MHz) 
       to verify these constraints:
          - ST7735 LCD SPI interface max baudrate is 15MHz for write and 6.66MHz for read
            Since the provided driver doesn't use read capability from LCD, only constraint 
            on write baudrate is considered.
          - SD card SPI interface max baudrate is 25MHz for write/read
          - PCLK2 max frequency is 100 MHz 
       */ 
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
    hspi1.Init.Mode = SPI_MODE_MASTER;

    SPI1_MspInit(&hspi1);
    HAL_SPI_Init(&hspi1);
  }
}

 void bmi160_spi_cs(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  /* BMI160_CS_GPIO Periph clock enable */
  BMI160_CS_GPIO_CLK_ENABLE();
  /* Configure BMI160_CS_PIN pin: BMI160 CS pin */
  GPIO_InitStruct.Pin = GPIO_PIN_BMI160_CS;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIO_PORT_BMI160_CS, &GPIO_InitStruct);
}

/**
  * @brief  SPI error treatment function.
  * @param  None
  * @retval None
  */
static void SPI1_Error (void)
{
  /* De-initialize the SPI communication BUS */
  HAL_SPI_DeInit(&hspi1);
  
  /* Re-Initiaize the SPI communication BUS */
  BMI160_IO_Init();
}


#ifdef INCLUDE_BMI160API
#define MASK_DATA1	0xFF
#define MASK_DATA2	0x80
#define MASK_DATA3	0x7F
/*!
 *	@brief : The function is used as SPI bus read
 *	@return : Status of the SPI read
 *	@param dev_addr : The device address of the sensor
 *	@param reg_addr : Address of the first register,
 *	will data is going to be read
 *	@param reg_data : This data read from the sensor,
 *	which is hold in an array
 *	@param cnt : The no of byte of data to be read
 */
int8_ts bmi160_spi_bus_read(us32 dev_addr, us8 reg_addr, us8 *reg_data, us8 cnt)
{
	int ierror = 0;
	#ifdef INCLUDE_BMI160API

	uint8_t array[8] = {MASK_DATA1};
	uint8_t array_pTx[8] = {MASK_DATA1};
	uint8_t stringpos;
	HAL_StatusTypeDef status = HAL_OK;
	cnt++;

	/*	For the SPI mode only 7 bits of register addresses are used.
	The MSB of register address is declared the bit what functionality it is
	read/write (read as 1/write as 0)*/
	array_pTx[0] = reg_addr|MASK_DATA2;
	BMI160_CS_LOW();
	status = HAL_SPI_TransmitReceive(&hspi1, array_pTx,array, (uint16_t)cnt, Spi2Timeout);
	BMI160_CS_HIGH();
	/* Check the communication status */
    if(status != HAL_OK)
	{
	   /* Execute user timeout callback */
	   SPI1_Error();
	   return -1;
	}
	for (stringpos = 0; stringpos < cnt; stringpos++)
		*(reg_data + stringpos) = array[stringpos+ 1];
	#endif
	return (char)ierror;
} 
/*!
 *	@brief : The function is used as SPI bus write
 *	@return : Status of the SPI write
 *	@param dev_addr : The device address of the sensor
 *	@param reg_addr : Address of the first register,
 *	will data is going to be written
 *	@param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	@param cnt : The no of byte of data to be write
 */
int8_ts bmi160_spi_bus_write(us32 dev_addr, us8 reg_addr,us8 *reg_data, us8 cnt)
{
	int ierror = 0;
	#ifdef INCLUDE_BMI160API

	uint8_t array[2];

	HAL_StatusTypeDef status = HAL_OK;

	array[0] = (reg_addr) & MASK_DATA3;
	array[1] = *(reg_data);

   BMI160_CS_LOW();
   status = HAL_SPI_Transmit(&hspi1,array, 2, Spi2Timeout);
   BMI160_CS_HIGH();
   /* Check the communication status */
   if(status != HAL_OK)
   {
     /* Execute user timeout callback */
     SPI1_Error();
	 return -1;
   }
   #endif
   return (char)ierror;
}
int8_ts burst_read(us32 dev_addr, us8 reg_addr,us8 *reg_data, us32 cnt)
{
	int ierror = 0;
	#ifdef INCLUDE_BMI160API
	
	uint8_t array[8] = {MASK_DATA1};
	uint8_t array_pTx[8] = {MASK_DATA1};
	uint16_t stringpos;
	HAL_StatusTypeDef status = HAL_OK;
	cnt++;
		
		/*	For the SPI mode only 7 bits of register addresses are used.
		The MSB of register address is declared the bit what functionality it is
		read/write (read as 1/write as 0)*/
	array_pTx[0] = reg_addr|MASK_DATA2;
	BMI160_CS_LOW();
	status = HAL_SPI_TransmitReceive(&hspi1, array_pTx,array, (uint16_t)cnt, Spi2Timeout);
	BMI160_CS_HIGH();
		/* Check the communication status */
	if(status != HAL_OK)
	{
		   /* Execute user timeout callback */
		SPI1_Error();
		return -1;
	}
	for (stringpos = 0; stringpos < cnt; stringpos++)
	{
		*(reg_data + stringpos) = array[stringpos+ 1];
	}
	#endif
		return (char)ierror;
}

#endif

/*!
 *	@brief This function is an example for delay
 *	@param msek: delay in milli seconds
 *	@return : communication result
 */
void bmi160_delay_ms(uint32_t msek)
{
 /* user delay*/
 HAL_Delay(msek);
}
