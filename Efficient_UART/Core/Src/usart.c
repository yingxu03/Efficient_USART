/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "FIFO.h"

FIFO_BUFFER_CTRL uart_fifo_buffer;

extern QueueHandle_t Uart_Analysis_Queue_Handle;// 外部声明UART分析队列句柄
/* USER CODE END 0 */

UART_HandleTypeDef huart5;
DMA_HandleTypeDef hdma_uart5_rx;// UART5 RX DMA句柄

/* UART5 init function */
void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;                  // 设置UART5实例
  huart5.Init.BaudRate = 2000000;           // 设置波特率为2Mbps
  huart5.Init.WordLength = UART_WORDLENGTH_8B;  // 设置数据位为8位
  huart5.Init.StopBits = UART_STOPBITS_1;   // 设置停止位为1位
  huart5.Init.Parity = UART_PARITY_NONE;    // 设置无奇偶校验
  huart5.Init.Mode = UART_MODE_TX_RX;       // 设置为收发模式
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;  // 设置无硬件流控制
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;  // 设置16倍过采样
  if (HAL_UART_Init(&huart5) != HAL_OK)     // 初始化UART5
  {
    Error_Handler();                        // 初始化失败调用错误处理
  }
  /* USER CODE BEGIN UART5_Init 2 */
	
  /* USER CODE END UART5_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspInit 0 */

  /* USER CODE END UART5_MspInit 0 */
    /* UART5 clock enable */
    __HAL_RCC_UART5_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**UART5 GPIO Configuration
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5; // 复用功能为UART5
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* UART5 DMA Init */
    /* UART5_RX Init */
    hdma_uart5_rx.Instance = DMA1_Stream0;
    hdma_uart5_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_uart5_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;// 外设到内存方向
    hdma_uart5_rx.Init.PeriphInc = DMA_PINC_DISABLE;// 外设地址不递增
    hdma_uart5_rx.Init.MemInc = DMA_MINC_ENABLE;// 内存地址递增
    hdma_uart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;// 外设数据对齐为字节
    hdma_uart5_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;// 内存数据对齐为字节
    hdma_uart5_rx.Init.Mode = DMA_CIRCULAR;// 循环模式
    hdma_uart5_rx.Init.Priority = DMA_PRIORITY_LOW;// 低优先级
    hdma_uart5_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;// 禁用FIFO模式

    if (HAL_DMA_Init(&hdma_uart5_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_uart5_rx);// 链接DMA到UART

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(UART5_IRQn, 5, 0); // 设置UART5中断优先级为5
    HAL_NVIC_EnableIRQ(UART5_IRQn); // 使能UART5中断
  /* USER CODE BEGIN UART5_MspInit 1 */
	
  /* USER CODE END UART5_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspDeInit 0 */

  /* USER CODE END UART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART5_CLK_DISABLE();

    /**UART5 GPIO Configuration
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* UART5 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* UART5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspDeInit 1 */

  /* USER CODE END UART5_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int fputc(int ch, FILE *f)
{ 	
	while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
	UART5->DR = (unsigned char) ch;      
	return ch;
}

void HAL_UART_RxIDleCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART5){
		static uint32_t idle_flag = 0x01;// 空闲中断标志
		if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))// 检测空闲中断标志
		{
			__HAL_UART_CLEAR_IDLEFLAG(huart);// 清除空闲中断标志
			//1. 获取当前head位置
			uint32_t cur_head_pos = 0;
			Get_Head_Position(&uart_fifo_buffer, &cur_head_pos);// 获取FIFO头位置
			cur_head_pos = cur_head_pos%FIFO_MAX_BUFFER;// 计算实际位置
			//2. 计算head应该在的位置
			uint32_t need_head_pos = FIFO_MAX_BUFFER - __HAL_DMA_GET_COUNTER(&hdma_uart5_rx);
			//3. 计算head应该移动的长度
			uint32_t move_head_len = (need_head_pos >= cur_head_pos)?\
			                         (need_head_pos - cur_head_pos):\
			                         (need_head_pos + FIFO_MAX_BUFFER - cur_head_pos);
			//4. 移动head位置
			Move_Head_Position(&uart_fifo_buffer, move_head_len);
			//5. 唤醒数据分析任务
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			xQueueSendFromISR(Uart_Analysis_Queue_Handle, &idle_flag, &xHigherPriorityTaskWoken);// 发送消息到队列
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);// 如果有更高优先级任务则进行任务切换
		}
	}
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART5){
		static uint32_t half_flag = 0x02;// 半传输完成标志
		//1. 获取当前head位置
		uint32_t cur_head_pos = 0;
		Get_Head_Position(&uart_fifo_buffer, &cur_head_pos);
		cur_head_pos = cur_head_pos%FIFO_MAX_BUFFER;
		//2. 计算head应该在的位置
		uint32_t need_head_pos = FIFO_MAX_BUFFER/2;// 半缓冲区位置
		//3. 计算head应该移动的长度
		uint32_t move_head_len = (need_head_pos >= cur_head_pos)?\
														 (need_head_pos - cur_head_pos):\
														 (need_head_pos + FIFO_MAX_BUFFER - cur_head_pos);
		//4. 移动head位置
		Move_Head_Position(&uart_fifo_buffer, move_head_len);
		//5. 唤醒数据分析任务
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendFromISR(Uart_Analysis_Queue_Handle, &half_flag, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART5){
		static uint32_t cplt_flag = 0x03;
		//1. 获取当前head位置
		uint32_t cur_head_pos = 0;
		Get_Head_Position(&uart_fifo_buffer, &cur_head_pos);
		cur_head_pos = cur_head_pos%FIFO_MAX_BUFFER;
		//2. 计算head应该在的位置
		uint32_t need_head_pos = FIFO_MAX_BUFFER;
		//3. 计算head应该移动的长度
		uint32_t move_head_len = (need_head_pos >= cur_head_pos)?\
														 (need_head_pos - cur_head_pos):\
														 (need_head_pos + FIFO_MAX_BUFFER - cur_head_pos);
		//4. 移动head位置
		Move_Head_Position(&uart_fifo_buffer, move_head_len);
		//5. 唤醒数据分析任务
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendFromISR(Uart_Analysis_Queue_Handle, &cplt_flag, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
/* USER CODE END 1 */
