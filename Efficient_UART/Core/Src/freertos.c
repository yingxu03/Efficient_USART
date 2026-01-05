/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "queue.h"
#include "usart.h"
#include "FIFO.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern FIFO_BUFFER_CTRL uart_fifo_buffer;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void uart_analysis_task(void *argument);

QueueHandle_t Uart_Analysis_Queue_Handle;
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	Uart_Analysis_Queue_Handle = xQueueCreate(16, 4);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	BaseType_t result;
	
	result = xTaskCreate(uart_analysis_task, "uart_analysis_task", 256, NULL, osPriorityNormal4, NULL);
	configASSERT(result == pdPASS); 
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
uint8_t temp_data[100] = {0};// 临时数据缓冲区，用于存储从FIFO读取的数据
void uart_analysis_task(void *argument)
{
	// 使能UART5的空闲中断
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
	
	// 启动DMA接收，数据直接存入FIFO缓冲区
	HAL_UART_Receive_DMA(&huart5, uart_fifo_buffer.circular_buffer, FIFO_MAX_BUFFER);
	
	// 中断标志变量，用于接收队列消息
	uint32_t isr_flag = 0;
	
	// 临时缓冲区索引
	uint8_t i = 0;
	for(;;)
  {
	    // 阻塞等待队列消息（来自中断）
		xQueueReceive(Uart_Analysis_Queue_Handle, &isr_flag, portMAX_DELAY);
		
		while(FIFO_Buffer_is_Empty(&uart_fifo_buffer) != 1){ 
			// 循环读取直到FIFO为空
			
			Get_Byte_from_FIFO_Buffer(&uart_fifo_buffer, &temp_data[i]);
			// 从FIFO获取一个字节到临时缓冲区
			i = (i+1)%100;
			//下面可以放数据分析的代码
		}
  }
}
/* USER CODE END Application */

