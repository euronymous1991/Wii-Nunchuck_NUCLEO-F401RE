/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c-lcd.h"
//#include "custom_chars.h"
#include "stdint.h"
#include "stm32_ds3231.h"
#include "stdio.h"
#include "stdint.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
_RTC rtc = {
    .Year = 21, .Month = 10, .Date = 25,
    .DaysOfWeek = MONDAY,
    .Hour = 13, .Min = 20, .Sec = 00
};

uint8_t regVal;
float rtcTemp;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//nunchaku structure declaration 
 typedef struct {
    uint8_t joy_x;
    uint8_t joy_y;
    uint8_t button_c;
    uint8_t button_z;
    uint16_t accel_x;
    uint16_t accel_y;
    uint16_t accel_z;
 }wiiNunchuck;
 uint8_t rxData[6],decData[6];
 uint8_t pData[2] = {0x40, 0x00};
 wiiNunchuck keyStatus;
 char aaa;
 short ax,ay,az=0;
 char flag=0;
 int counter=0;
 //The HAL library set by left shifting the 7bit address 
 #define WII_NUNCHUCK_DEV_ID ((0x52)<<1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
 static void send_request(I2C_HandleTypeDef *hi2c)
 {
    uint8_t pData[2] = {0x00, 0x00};
    HAL_I2C_Master_Transmit(hi2c, WII_NUNCHUCK_DEV_ID, pData, 1, 3000);
 }
 
  static uint8_t decode_byte(uint8_t x)
 {
    return ((x ^ 0x17) + 0x17);
 }
  void WiiNunchuck_init(I2C_HandleTypeDef *hi2c)
 {
    HAL_I2C_Master_Transmit(hi2c, WII_NUNCHUCK_DEV_ID, pData, 2, 3000);
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
char mystr[17];
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
char vieta;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	lcd_init ();
	lcd_clear ();
	HAL_Delay(2);
WiiNunchuck_init(&hi2c1);
  /* USER CODE END 2 */
  DS3231_Init(&hi2c1);
 // DS3231_SetTime(&rtc);
	
	  /* Configure Alarm1 */
  DS3231_ClearAlarm1();
  //DS3231_SetAlarm1(ALARM_MODE_ONCE_PER_SECOND, 0, 0, 0, 0);
  DS3231_SetAlarm1(ALARM_MODE_SEC_MATCHED, 0, 0, 0, 30);
	
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		if(counter >= 10000){
		counter=0;
		if(flag==1){
		flag=0;}else {flag=1;}
		}
		    int cnt=0;
		
		if(flag==0){
		    DS3231_GetTime(&rtc);
    DS3231_ReadTemperature(&rtcTemp);

    ReadRegister(DS3231_REG_STATUS, &regVal);
    if(regVal & DS3231_STA_A1F)
    {
      regVal &= ~DS3231_STA_A1F;
      WriteRegister(DS3231_REG_STATUS, regVal);
    }
	}
		else {		
   send_request(&hi2c1);
    HAL_Delay(2);

    //6Byte lead 
    HAL_I2C_Master_Receive(&hi2c1, WII_NUNCHUCK_DEV_ID|0x01 , rxData, 6, 3000);
 
		//Data composite 
    for(cnt=0; cnt<6; cnt++){
        decData[cnt] = decode_byte(rxData[cnt]);
    }
    //assignment to structure 

    keyStatus.joy_x = decData[0];
    keyStatus.joy_y = decData[1];
    keyStatus.button_c = (decData[5]>>1) & 0x01;
    keyStatus.button_z = (decData[5]>>0) & 0x01;
    keyStatus.accel_x = (((uint16_t)decData[2])<<2) | ((decData[5]>>2) & 0x03);
    keyStatus.accel_y = (((uint16_t)decData[3])<<2) | ((decData[5]>>4) & 0x03);
    keyStatus.accel_z = (((uint16_t)decData[4])<<2) | ((decData[5]>>6) & 0x03);
	}
		
	/*	if(flag == 0){
		LCDGotoXY(0,1);
	lcd_send_string("                ");
			flag=1;
			} */
	
			if(flag==0){
		sprintf(mystr,"%02d/%02d/%02d %1d flag%1d",rtc.Year,rtc.Month,rtc.Date,rtc.DaysOfWeek,flag);
		lcd_string_draw(mystr, 0, 1);
		sprintf(mystr,"%02d:%02d:%02d T:%2.2f",rtc.Hour,rtc.Min,rtc.Sec,rtcTemp);
		lcd_string_draw(mystr, 0, 0);
		    HAL_Delay(1000); 
			} else {
		sprintf(mystr,"X%03d  Y%03d  Z%03d",keyStatus.accel_x,keyStatus.accel_y,keyStatus.accel_z);
		lcd_string_draw(mystr, 0, 1);
				sprintf(mystr,"JX%03d JY%03d C%01dZ%01d",keyStatus.joy_x,keyStatus.joy_y,keyStatus.button_c,keyStatus.button_z);
		lcd_string_draw(mystr, 0, 0);   
			HAL_Delay(100); }
		
    /* USER CODE END WHILE */
		
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/