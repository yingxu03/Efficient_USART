#ifndef _FIFO_H
#define _FIFO_H

#include "main.h"

#define  FIFO_MAX_BUFFER          16

typedef struct
{
	uint8_t circular_buffer[FIFO_MAX_BUFFER];
	volatile uint32_t head;
	volatile uint32_t tail;
}FIFO_BUFFER_CTRL;

uint8_t Create_FIFO_Buffer(FIFO_BUFFER_CTRL ** p_fifo_buffer);

uint8_t FIFO_Buffer_is_Empty(FIFO_BUFFER_CTRL * p_buffer);
uint8_t FIFO_Buffer_is_Full(FIFO_BUFFER_CTRL * p_buffer);

uint8_t Insert_Byte_to_FIFO_Buffer(FIFO_BUFFER_CTRL * p_buffer, uint8_t byte);
uint8_t Get_Byte_from_FIFO_Buffer(FIFO_BUFFER_CTRL * p_buffer, uint8_t *byte);

uint8_t Insert_Buff_to_FIFO_Buffer(FIFO_BUFFER_CTRL * p_buffer, uint8_t *buf, int32_t len);
uint8_t Get_Buff_from_FIFO_Buffer(FIFO_BUFFER_CTRL * p_buffer, uint8_t *buf, int32_t len);

uint32_t Get_FIFO_Buffer_Avail(FIFO_BUFFER_CTRL * p_buffer);

uint8_t Get_Head_Position(FIFO_BUFFER_CTRL * p_buffer, uint32_t *pos);
uint8_t Move_Head_Position(FIFO_BUFFER_CTRL * p_buffer, uint32_t len);

#endif
