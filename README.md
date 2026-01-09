# UART DMA FIFO with FreeRTOS on STM32F4

这是一个基于STM32F4系列微控制器的UART DMA接收项目，结合了FreeRTOS实时操作系统和循环FIFO缓冲区机制，实现了高效的数据接收和处理。

## 项目概述

本项目演示了如何使用STM32F4的DMA（直接存储器访问）功能配合UART空闲中断，在FreeRTOS环境下实现高效的数据接收。通过FIFO缓冲区管理数据流，避免了数据丢失并提高了系统响应速度。

## 主要特性

- ✅ **UART DMA接收**：使用DMA进行UART数据接收，减轻CPU负担
- ✅ **空闲中断检测**：检测UART总线空闲状态，及时处理接收到的数据
- ✅ **循环FIFO缓冲区**：实现数据缓冲，防止数据溢出
- ✅ **FreeRTOS集成**：使用RTOS进行任务管理和调度
- ✅ **中断驱动队列**：通过队列在中断和任务间传递消息
- ✅ **高波特率支持**：支持2Mbps的高波特率通信

## 硬件要求

- STM32F4系列开发板（本项目基于STM32F407）
- UART5接口（PC12-TX, PD2-RX）
- 外部串口设备

## 软件架构

### 主要文件说明

1. **dma.c** - DMA控制器初始化
   - 使能DMA1时钟
   - 配置DMA1 Stream0中断优先级

2. **FIFO.c** - 循环FIFO缓冲区管理
   - 提供FIFO创建、插入、读取等操作
   - 支持字节和缓冲区级别的操作
   - 线程安全的缓冲区管理

3. **freertos.c** - FreeRTOS任务管理
   - 初始化FreeRTOS对象
   - 创建UART数据分析任务
   - 配置任务间通信队列

4. **usart.c** - UART和DMA配置
   - UART5初始化（2Mbps，8N1）
   - DMA配置（循环模式，外设到内存）
   - 空闲中断、半传输完成、传输完成回调函数

5. **stm32f4xx_it.c** - 中断服务程序
   - UART5全局中断处理
   - DMA1 Stream0中断处理
   - TIM4中断处理（系统时钟）

6. **main.c** - 主程序入口
   - 系统时钟配置
   - 外设初始化
   - FreeRTOS内核启动

### 关键组件

#### FIFO缓冲区 (`FIFO_BUFFER_CTRL`)
```c
typedef struct {
    uint8_t circular_buffer[FIFO_MAX_BUFFER];  // 循环缓冲区
    uint32_t head;  // 写指针
    uint32_t tail;  // 读指针
} FIFO_BUFFER_CTRL;
```

#### 数据流处理流程
1. **DMA接收**：UART数据通过DMA直接存入FIFO的循环缓冲区
2. **中断触发**：空闲中断、半传输完成、传输完成中断触发回调
3. **指针更新**：根据DMA计数器更新FIFO的head指针
4. **任务唤醒**：通过队列通知数据分析任务
5. **数据处理**：任务从FIFO读取数据进行分析

## 配置参数

### UART配置
- **波特率**: 2,000,000 bps
- **数据位**: 8位
- **停止位**: 1位
- **校验位**: 无
- **流控制**: 无

### DMA配置
- **模式**: 循环模式
- **方向**: 外设到内存
- **数据对齐**: 字节对齐
- **优先级**: 低优先级
- **FIFO模式**: 禁用

### FreeRTOS配置
- **队列大小**: 16个消息
- **UART任务栈大小**: 256字
- **默认任务栈大小**: 512字
- **任务优先级**: 普通优先级

## 使用方法

### 1. 硬件连接
- 连接外部串口设备的TX到STM32的PD2（UART5_RX）
- 连接外部串口设备的RX到STM32的PC12（UART5_TX）

### 2. 编译和下载
- 使用STM32CubeIDE或Keil MDK打开项目
- 配置正确的目标MCU型号
- 编译并下载到开发板

### 3. 数据发送
- 从外部设备向STM32发送数据
- 数据将自动通过DMA接收并存入FIFO
- 数据分析任务将处理接收到的数据

### 4. 自定义处理
在`freertos.c`的`uart_analysis_task`函数中添加数据处理代码：
```c
while(FIFO_Buffer_is_Empty(&uart_fifo_buffer) != 1) {
    Get_Byte_from_FIFO_Buffer(&uart_fifo_buffer, &temp_data[i]);
    i = (i+1)%100;
    // 在这里添加自定义数据处理代码
}
```

## API函数

### FIFO操作函数
- `Create_FIFO_Buffer()` - 创建FIFO缓冲区
- `Insert_Byte_to_FIFO_Buffer()` - 插入单个字节
- `Get_Byte_from_FIFO_Buffer()` - 获取单个字节
- `Insert_Buff_to_FIFO_Buffer()` - 插入数据缓冲区
- `Get_Buff_from_FIFO_Buffer()` - 获取数据缓冲区
- `FIFO_Buffer_is_Empty()` - 检查FIFO是否为空
- `FIFO_Buffer_is_Full()` - 检查FIFO是否已满

### 回调函数
- `HAL_UART_RxIDleCallback()` - 空闲中断回调
- `HAL_UART_RxHalfCpltCallback()` - 半传输完成回调
- `HAL_UART_RxCpltCallback()` - 传输完成回调

## 注意事项

1. **FIFO大小**：确保`FIFO_MAX_BUFFER`大小足够处理预期的数据流量
2. **中断优先级**：合理配置UART和DMA中断优先级，避免数据丢失
3. **数据溢出**：监控FIFO状态，防止缓冲区溢出
4. **任务优先级**：确保数据分析任务及时响应队列消息
5. **电源管理**：在高波特率下注意电源稳定性和信号完整性

## 性能优化建议

1. **增大FIFO缓冲区**：如果数据量较大，可以增加`FIFO_MAX_BUFFER`大小
2. **调整任务优先级**：根据实际需求调整数据分析任务的优先级
3. **使用DMA双缓冲**：考虑实现DMA双缓冲机制进一步提高效率
4. **优化数据处理算法**：在数据分析任务中使用高效的数据处理算法

## 故障排除

### 常见问题
1. **数据丢失**：检查DMA配置和FIFO大小
2. **中断不触发**：确认中断使能和优先级设置
3. **任务不响应**：检查队列创建和任务优先级
4. **通信错误**：验证波特率设置和硬件连接

### 调试方法
1. 使用LED或串口打印调试信息
2. 检查DMA传输计数器
3. 监控FIFO状态（空/满）
4. 使用逻辑分析仪检查UART信号

## 许可证

本项目基于STMicroelectronics的许可证，详情见各文件头部的版权声明。

## 贡献

欢迎提交Issue和Pull Request来改进本项目。
