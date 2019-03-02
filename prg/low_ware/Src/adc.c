/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
  ******************************************************************************
*/

#include "adc.h"

#include "err.h"
#include "gpio.h"

ADC_HandleTypeDef hadc_sensor;

void AdcSensorInit(void)
{
  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc_sensor.Instance = ADC1;
  hadc_sensor.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc_sensor.Init.ContinuousConvMode = DISABLE;
  hadc_sensor.Init.DiscontinuousConvMode = DISABLE;
  hadc_sensor.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc_sensor.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc_sensor.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc_sensor) != HAL_OK)
  {
    ErrorHandler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc_sensor, &sConfig) != HAL_OK)
  {
    ErrorHandler(__FILE__, __LINE__);
  }

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();
  
    GpioSensorAdcInit();
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();
  
    GpioSensorAdcDeinit();
  }
}

uint32_t AdcMoistureSensorRead(void)
{
	if(HAL_ADC_Start(&hadc_sensor) == HAL_OK) {
		if(HAL_ADC_PollForConversion(&hadc_sensor, ADC_MOISTURE_SENSOR_CONV_TIMEOUT_MS) == HAL_OK) {
			return HAL_ADC_GetValue(*hadc_sensor);
		}
	}

	return 0;
}
