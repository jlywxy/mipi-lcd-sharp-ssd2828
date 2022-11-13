/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SSD_Reset(){
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
    HAL_Delay(100);

    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
}
void SSD_SPI_Write(uint8_t *buf,int length){
    HAL_SPI_Transmit(&hspi1,buf,length,HAL_TIMEOUT);
}
void SSD_SPI_Read(int length, uint8_t *r){
    HAL_SPI_Receive(&hspi1,r,length,HAL_TIMEOUT);
}
void SSD_SPI_ReadWrite(uint8_t *buf, int length, uint16_t *r){
    uint16_t bufr[length];
    HAL_SPI_TransmitReceive(&hspi1,buf,bufr,length+2,HAL_TIMEOUT);
    volatile uint8_t byter0=0|bufr[length]>>1;
    volatile uint8_t byter1=0|((bufr[length]&0b000000001)<<7)|(bufr[length+1]>>2);
    volatile uint8_t byter2=0|((bufr[length+1]&0b000000011)<<6)|(bufr[length+2]>>3);
    r[0]=byter0;
    r[1]=byter1;
    r[2]=byter2;
}
uint16_t SSD_SPI_ReadReg(uint8_t reg){
    uint16_t tbuf[2];
    tbuf[0]=reg;
    tbuf[1]=0xfa;
    uint16_t rbuf[2];
    SSD_SPI_ReadWrite((uint8_t*)tbuf,2,(uint8_t*)rbuf);
    return rbuf[0]|rbuf[1]<<8;

}

void SSD_SPI_WriteReg(uint8_t reg,uint16_t data){
    uint16_t buf[3]={reg,1<<8|(data&0xff),1<<8|((data>>8)&0xff)};
    SSD_SPI_Write(buf,3);
}
void SSD_WriteMIPI(uint8_t reg,uint8_t data0,uint8_t data1,int length){
    SSD_SPI_WriteReg(0xbc,length);
    uint16_t tbuf[4];
    tbuf[0]=0x0bf, tbuf[1]=1<<8|reg; tbuf[2]=1<<8|data0; tbuf[3]=1<<8|data1;
    SSD_SPI_Write(&tbuf,length+1);
}
uint16_t SSD_ReadMIPI(uint8_t reg){
    SSD_SPI_WriteReg(0xb7,0x0382);
    SSD_SPI_WriteReg(0xbb,0x0004);
    SSD_SPI_WriteReg(0xc1,0x000a);
    SSD_SPI_WriteReg(0xc0,0x0100);
    SSD_SPI_WriteReg(0xbc,0x0051);
    SSD_SPI_WriteReg(0xbf,0x00c6);


    uint16_t tbuf[2];uint16_t rbuf[2];
    tbuf[0]=0xd4;tbuf[1]=0x1fa;tbuf[2]=0x100;
    SSD_SPI_Write(&tbuf,3);
    tbuf[0]=0xc6,tbuf[1]=0xfa;
    SSD_SPI_ReadWrite(&tbuf,2,&rbuf);
    return rbuf[0]|rbuf[1]<<8;
}
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
    HAL_Delay(1000);
    uint16_t buf[4];
    SSD_Reset();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

      //SSD_Reset();

      volatile uint16_t result=SSD_SPI_ReadReg(0xb0);
      result;

      SSD_SPI_WriteReg(0xb9,0x0001);
      SSD_SPI_WriteReg(0xb7,0x0300);

      SSD_WriteMIPI(0xb0,0x04,0,2);
      SSD_WriteMIPI(0x00,0,0,1);
      SSD_WriteMIPI(0x00,0,0,1);
      SSD_WriteMIPI(0xd6,0x01,0,2);
      SSD_WriteMIPI(0x51,0x0f,0xff,3);
      SSD_WriteMIPI(0x53,0x04,0,2);
      SSD_WriteMIPI(0x29,0,0,1);
      SSD_WriteMIPI(0x11,0,0,1);

      SSD_SPI_WriteReg(0xb7,0x0309);
      SSD_SPI_WriteReg(0xee,0x0600);
      volatile uint16_t r=SSD_ReadMIPI(0x00);
      r=0|r;

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
