/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
    HAL_Delay(200);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
    HAL_Delay(100);



    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);
    HAL_Delay(100);



    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
}
void SSD_SPI_Write(uint8_t *buf,int length){
    HAL_SPI_Transmit(&hspi1,buf,length,HAL_TIMEOUT);
}
void SSD_SPI_Read(int length, uint8_t *r){
    HAL_SPI_Receive(&hspi1,r,length,HAL_TIMEOUT);
}
void SSD_SPI_ReadWrite(uint8_t *buf, int length, uint16_t *r){
    volatile uint16_t bufr[10];
    HAL_SPI_TransmitReceive(&hspi1, buf, (uint8_t *) bufr, length + 2, HAL_TIMEOUT);
    volatile uint8_t byter0=0|bufr[length]>>1;
    volatile uint8_t byter1=0|((bufr[length]&0b000000001)<<7)|(bufr[length+1]>>2);
    r[0]=byter0;
    r[1]=byter1;
}
uint16_t SSD_SPI_ReadReg(uint8_t reg){
    uint16_t tbuf[2];
    tbuf[0]=reg;
    tbuf[1]=0xfa;
    uint16_t rbuf[2];
    SSD_SPI_ReadWrite((uint8_t*)tbuf,2,rbuf);
    return rbuf[0]|rbuf[1]<<8;

}

void SSD_SPI_WriteReg(uint8_t reg,uint16_t data,int len){
    uint16_t buf[3]={reg,1<<8|(data&0xff),1<<8|((data>>8)&0xff)};
    SSD_SPI_Write(buf,len+1);
}
void SSD_MIPI_WriteShortGeneric(uint8_t reg,uint16_t data,int len){
    SSD_SPI_WriteReg(0xb7,0x0302,2);
    SSD_SPI_WriteReg(0xb8,0x0000,2);

    SSD_SPI_WriteReg(0xbc,len,2);
    SSD_SPI_WriteReg(0xbf,reg|(data<<8),2);
}
void SSD_MIPI_WriteShortDCS(uint8_t reg,uint16_t data,int len){
    SSD_SPI_WriteReg(0xb7,0x0050,2);
    SSD_SPI_WriteReg(0xb8,0x0000,2);

    SSD_SPI_WriteReg(0xbc,len,2);
    SSD_SPI_WriteReg(0xbf,reg|(data<<8),2);
}
void SSD_MIPI_WriteLongGeneric(uint8_t reg,uint16_t* data,int len){
    SSD_SPI_WriteReg(0xb7,0x0302,2);
    SSD_SPI_WriteReg(0xb8,0x0000,2);

    SSD_SPI_WriteReg(0xbc,len,2);
    if(len>1)SSD_SPI_WriteReg(0xbf,reg|(data[0]<<8),2);
    if(len>2)SSD_SPI_WriteReg(0xbf,data[1]|(data[2]<<8),2);
}
void SSD_MIPI_WriteLongDCS(uint8_t reg,uint16_t* data,int len){
    SSD_SPI_WriteReg(0xb7,0x0050,2);
    SSD_SPI_WriteReg(0xb8,0x0000,2);

    SSD_SPI_WriteReg(0xbc,len,2);
    if(len>1)SSD_SPI_WriteReg(0xbf,reg|(data[0]<<8),2);
    if(len>2)SSD_SPI_WriteReg(0xbf,data[1]|(data[2]<<8),2);
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
   // HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
   // HAL_Delay(100);
    uint16_t buf[4];
    SSD_Reset();
    HAL_Delay(300);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//      volatile uint16_t id;
//      id=SSD_SPI_ReadReg(0xb0);
//      id;
//      continue;
      SSD_SPI_WriteReg(0xee,0x0600,2);
      //SSD_SPI_WriteReg(0xb7,0x0300,2);
      SSD_SPI_WriteReg(0xb4,0x0438,2);
      SSD_SPI_WriteReg(0xb5,0x0780,2);
      SSD_SPI_WriteReg(0xde,0x0003,2);
      SSD_SPI_WriteReg(0xb9,0x0000,2);
      SSD_SPI_WriteReg(0xba,0x8028,2);
      SSD_SPI_WriteReg(0xb9,0x0001,2);
      SSD_SPI_WriteReg(0xb8,0x0000,2);

      //-------------------------------------
      SSD_MIPI_WriteShortGeneric(0xb0,0x04,2);
      SSD_MIPI_WriteShortGeneric(0x00,0x00,1);
      SSD_MIPI_WriteShortGeneric(0x00,0x00,1);
      SSD_MIPI_WriteShortGeneric(0xd6,0x01,2);
      uint16_t ld[3]={0x0f,0xff,0};
     // SSD_MIPI_WriteLongDCS(0x51,ld,3);
      SSD_MIPI_WriteShortDCS(0x53,0x04,2);
      SSD_MIPI_WriteShortGeneric(0x29,0x00,1);
      SSD_MIPI_WriteShortGeneric(0x11,0x00,1);

//
      SSD_SPI_WriteReg(0xb6,0xc003,2);
      SSD_SPI_WriteReg(0xee,0x0600,2);
      SSD_SPI_WriteReg(0xde,0x0003,2);
      SSD_SPI_WriteReg(0xb7,0x024b,2);

      //------------------------------------
      volatile uint16_t r,r1,r2,r3,r4;
      SSD_SPI_WriteReg(0xb7,0x0382,2);
      SSD_SPI_WriteReg(0xc1,0x000a,2);
      SSD_SPI_WriteReg(0xc0,0x0001,2);

      SSD_SPI_WriteReg(0xbc,0x0001,2);
      SSD_SPI_WriteReg(0xbf,0x00b0,2);


      r=SSD_SPI_ReadReg(0xb0);
      r1= SSD_SPI_ReadReg(0xc2);
      r2= SSD_SPI_ReadReg(0xff);

      SSD_SPI_WriteReg(0xb7,0x03c2,2);
      SSD_SPI_WriteReg(0xc1,0x000a,2);
      SSD_SPI_WriteReg(0xc0,0x0001,2);

      SSD_SPI_WriteReg(0xbc,0x0001,2);
      SSD_SPI_WriteReg(0xbf,0x0053,2);

      r3= SSD_SPI_ReadReg(0xc2);
      r4= SSD_SPI_ReadReg(0xff);
      r;r1;r2;r3;r4;


        //break;
      //------------------------------------
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
