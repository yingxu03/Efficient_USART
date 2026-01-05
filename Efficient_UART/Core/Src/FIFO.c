#include "FIFO.h"
#include "FreeRTOS.h"

uint8_t Create_FIFO_Buffer(FIFO_BUFFER_CTRL **p_fifo_buffer)// 创建FIFO缓冲区
{
	*p_fifo_buffer = (FIFO_BUFFER_CTRL *)pvPortMalloc(sizeof(FIFO_BUFFER_CTRL));// 分配内存给FIFO控制结构体
	
	if(*p_fifo_buffer == NULL){// 检查内存分配是否成功
		return 0;
	}
	
	memset(*p_fifo_buffer, 0, sizeof(FIFO_BUFFER_CTRL));// 初始化FIFO控制结构体为0
	
	return 1;
}

uint8_t FIFO_Buffer_is_Empty(FIFO_BUFFER_CTRL * p_buffer)// 检查FIFO是否为空
{
	if(p_buffer == NULL){
			return 0;
	}
	
	if(p_buffer->head == p_buffer->tail){// 头尾相等表示空
		return 1;
	}
	
	return 0;
}

uint8_t FIFO_Buffer_is_Full(FIFO_BUFFER_CTRL * p_buffer)// 检查FIFO是否已满
{
	if(p_buffer == NULL){
			return 0;
	}
	
	if((p_buffer->head - p_buffer->tail) == FIFO_MAX_BUFFER){// 头尾差等于最大缓冲区大小表示满
		return 1;
	}
	
	return 0;
}

uint8_t Insert_Byte_to_FIFO_Buffer(FIFO_BUFFER_CTRL * p_buffer, uint8_t byte)// 向FIFO插入单个字节
{
	if(p_buffer == NULL){
			return 0;
	}
	
	if(FIFO_Buffer_is_Full(p_buffer)){
		return 0;
	}
	
	p_buffer->circular_buffer[p_buffer->head%FIFO_MAX_BUFFER] = byte;// 在头位置插入字节（循环缓冲区）
	
	p_buffer->head++;
	
	return 1;
}

uint8_t Get_Byte_from_FIFO_Buffer(FIFO_BUFFER_CTRL * p_buffer, uint8_t *byte) // 从FIFO获取单个字节
{
	if(p_buffer == NULL){
			return 0;
	}
	
	if(FIFO_Buffer_is_Empty(p_buffer)){
		return 0;
	}
	
	*byte = p_buffer->circular_buffer[p_buffer->tail%FIFO_MAX_BUFFER]; // 从尾位置获取字节（循环缓冲区）
	
	p_buffer->tail++;
	
	return 1;
}

uint8_t Insert_Buff_to_FIFO_Buffer(FIFO_BUFFER_CTRL * p_buffer, uint8_t *buf, int32_t len)// 向FIFO插入数据缓冲区
{
	if(p_buffer == NULL){
			return 0;
	}
	
	int32_t unused;// 未使用空间变量
	unused = FIFO_MAX_BUFFER - (p_buffer->head - p_buffer->tail);// 计算剩余空间
	if(unused < len){// 检查剩余空间是否足够
		return 0;
	}
	
	for(uint32_t i=0; i<len; i++){// 循环插入每个字节
		Insert_Byte_to_FIFO_Buffer(p_buffer, buf[i]);
	}
	
	return 1;
}

uint8_t Get_Buff_from_FIFO_Buffer(FIFO_BUFFER_CTRL * p_buffer, uint8_t *buf, int32_t len) // 从FIFO获取数据缓冲区
{
	if(p_buffer == NULL){
			return 0;
	}
	
	int32_t used;// 已使用空间变量
	used = p_buffer->head - p_buffer->tail;// 计算已使用空间
	if(used < len){// 检查数据是否足够
		return 0;
	}
	
	for(uint32_t i=0; i<len; i++){// 循环获取每个字节
		Get_Byte_from_FIFO_Buffer(p_buffer, &buf[i]);
	}
	
	return 1;
}

uint32_t Get_FIFO_Buffer_Avail(FIFO_BUFFER_CTRL * p_buffer)// 获取FIFO可用空间大小
{
	return FIFO_MAX_BUFFER - (p_buffer->head - p_buffer->tail);
}

uint8_t Get_Head_Position(FIFO_BUFFER_CTRL * p_buffer, uint32_t *pos)// 获取头指针位置
{
	if(p_buffer == NULL){
			return 0;
	}
	
	*pos = p_buffer->head;
	return 1;
}

uint8_t Move_Head_Position(FIFO_BUFFER_CTRL * p_buffer, uint32_t len)// 移动头指针位置
{
	if(p_buffer == NULL){
			return 0;
	}
	
	p_buffer->head = p_buffer->head + len; 
	return 1;
}

