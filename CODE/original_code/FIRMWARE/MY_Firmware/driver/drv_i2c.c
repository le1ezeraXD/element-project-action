#include "drv_i2c.h"

/**
	******************************************************************************
	* @file    drv_i2c.c
	* @author  zyj
	* @version V0.0.1
	* @date    15-10-2025
	* @brief   i2c
	******************************************************************************
	*
	*	硬件IIC  
	*	同步  多主从  串行总线标准
	*	两线  SDA  SCL 
	*	通信方式:  半双工通信  同步串行通信   多主从架构
	*	速度: 标准模式100kb/s  快速模式400kb/s
	*	      高速模式3.4Mb/s  超快速模式5Mb/s
	*	寻址方式: 7位地址模式   10位地址模式
	******************************************************************************
	*/
	
/** I2C1  PB6-SCL   PB7-SDA
  * @brief  硬件I2C1初始化
  * @param  None
  * @retval None
  */
void drv_i2c1_init(void) {
	
#if MY_DRIVER
	//使能时钟 
	RCC->APB2ENR |= (1<<3);    //GPIOB
	RCC->APB1ENR |= (1<<21);   //IIC2
	RCC->APB2ENR |= (1<<0);    //AFIO
	
	//设置GPIO	PB6 PB7  复用开漏模式 SCL-输出  SDA-先设置为输出
	//PB6 (SCL) 和 PB7 (SDA) 设置为复用开漏
	GPIOB->CRL &= ~(0xFFUL << 24);    // 清除 PB6/PB7 配置
	GPIOB->CRL |= 0xFFUL<<24;   	  // PB6: AF Open-Drain, PB7: AF Open-Drain
	
	//配置输入时钟 设置为快速模式400kHz  
	I2C1->CR2 |= 0x24;        //设置输入时钟为36MHz  PCLK1
	I2C1->CR1 &= ~(1<<0);     //PE位为0 CCR寄存器只有在关闭I2C时(PE=0)才能设置
	
	I2C1->TRISE = 0x0C;  //上升时间是300ns
	
	//配置为快速模式  设置SCL频率
	I2C1->CCR |= (1<<15);
	
	//设置快速模式占空比
	I2C1->CCR &= ~(1<<14);

	I2C1->CCR |= 0x1E;  //设置分频系数为30 = 0x1E

	//使能I2C2
	I2C1->CR1 |= (1<<0); //PE位为0  <<  >> 运算符优先级大于按位与以及赋值运算符
	
	//默认使能应答  必须在使能之后再使能应答
	I2C1->CR1 |= (1<<10);
	
	//应答地址，选择7位，从机模式下才有效 | 自身地址，从机模式下才有效
	I2C1->OAR1 = 0x4000;
	
	//再次使能I2C2
	I2C1->CR1 |= (1<<0); //PE位为0  <<  >> 运算符优先级大于按位与以及赋值运算符
	
#else

	I2C_InitTypeDef   I2C_InitStruct;
    GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1、使能时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); //IIC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);   //GPIOB | AFIO
	
	//2、设置GPIO	PB6 PB7  复用开漏模式 SCL-输出  SDA-先设置为输出
	//PB6 (SCL) 和 PB7 (SDA) 设置为复用开漏
	GPIO_InitStruct.GPIO_Pin = I2C1_SCL_PIN | I2C1_SDA_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_PORT, &GPIO_InitStruct);
	
	//3、I2C初始化
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;                                //模式，选择为I2C模式
	I2C_InitStruct.I2C_ClockSpeed = 400000;								   //时钟速度，选择为400KHz
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;						   //时钟占空比，选择Tlow/Thigh = 2
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;							   //应答，选择使能	
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //应答地址，选择7位，从机模式下才有效											 
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;       						   //自身地址，从机模式下才有效
	I2C_Init(I2C1, &I2C_InitStruct);
	
	//使能I2C1
	I2C_Cmd(I2C1, ENABLE);

#endif
}


/** I2C2  PB10-SCL   PB11-SDA
  * @brief  硬件I2C2初始化
  * @param  None
  * @retval None
  */
void drv_i2c2_init(void) {
	
#if MY_DRIVER
	//使能时钟 
	RCC->APB2ENR |= (1<<3);    //GPIOB
	RCC->APB1ENR |= (1<<22);   //IIC2
	RCC->APB2ENR |= (1<<0);    //AFIO
	
	//PB10 (SCL) 和 PB11 (SDA) 设置为复用开漏
	GPIOB->CRH &= ~(0xFFUL<<8);   // 清除 PB10/PB11 配置
	GPIOB->CRH |= 0xFFUL<<8;   	  // PB10: AF Open-Drain, PB11: AF Open-Drain
	
	//配置输入时钟 设置为快速模式400kHz  
	I2C2->CR2 |= 0x24;  
	I2C2->CR1 &= ~(1<<0);   
	
	I2C2->TRISE = 0x0C;  //上升时间是300ns
	
	//配置为快速模式  设置SCL频率
	I2C2->CCR |= (1<<15);
	
	//设置快速模式占空比  设置为标准快速模式 - Tlow/Thigh = 2
	I2C2->CCR &= ~(1<<14);

	I2C2->CCR |= 0x1E;  //设置分频系数为30 = 0x1E

	//使能I2C2
	I2C2->CR1 |= (1<<0); //PE位为0  <<  >> 运算符优先级大于按位与以及赋值运算符
	
	//默认使能应答  必须在使能之后再使能应答
	I2C2->CR1 |= (1<<10);
	
	//应答地址，选择7位，从机模式下才有效 | 自身地址，从机模式下才有效
	I2C2->OAR1 = 0x4000;
	
	//再次使能I2C2
	I2C2->CR1 |= (1<<0); //PE位为0  <<  >> 运算符优先级大于按位与以及赋值运算符
	
#else

	I2C_InitTypeDef   I2C_InitStruct;
    GPIO_InitTypeDef  GPIO_InitStruct;
	
	//1、使能时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); //IIC2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);   //GPIOB | AFIO
	
	//2、设置GPIO	PB10 PB11  复用开漏模式 SCL-输出  SDA-先设置为输出
	//PB10 (SCL) 和 PB11 (SDA) 设置为复用开漏
	GPIO_InitStruct.GPIO_Pin = I2C2_SCL_PIN | I2C2_SDA_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C2_PORT, &GPIO_InitStruct);
	
	//3、I2C初始化
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;                               //模式，选择为I2C模式
	I2C_InitStruct.I2C_ClockSpeed = 400000;								  //时钟速度，选择为400KHz
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;						  //时钟占空比，选择Tlow/Thigh = 2
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;							  //应答，选择使能	
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//应答地址，选择7位，从机模式下才有效											 
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;       						  //自身地址，从机模式下才有效
	I2C_Init(I2C2, &I2C_InitStruct);
	
	//使能I2C2
	I2C_Cmd(I2C2, ENABLE);

#endif
}


/**
  * @brief  iic等待标志位 - 寄存器版本
  * @param  I2Cx: I2C外设
  * @param  flag: 要等待的标志位
  * @retval 0:成功 1:超时
  */
uint8_t i2c_wait_flag(I2C_TypeDef* I2Cx, uint32_t flag) {
	
	uint32_t timeout = 10000;										//给定超时计数时间
	
	while(!(I2Cx->SR1 & flag)) {									//循环等待指定事件
		timeout--;													//等待时，计数值自减
		if(timeout == 0){											//自减到0后，等待超时
			/*超时的错误处理代码，可以添加到此处*/
			return 1;												//返回超时错误
		}
	}
	return 0;														//成功返回
}

/**
  * @brief  iic等待事件 - 库函数版本
  * @param  同I2C_CheckEvent
  * @retval 0:成功 1:超时
  */
uint8_t i2c_wait_event(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT) {
	
	uint32_t timeout = 10000;															//给定超时计数时间
																	
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS) {								//循环等待指定事件
		timeout --;																		//等待时，计数值自减
		if (timeout == 0)	{															//自减到0后，等待超时
			
			/*超时的错误处理代码，可以添加到此处*/
			return 1;																	//返回超时错误
		}
	}
	return 0;																			//成功返回
}


/**
  * @brief  IIC起始条件
  * @param  None
  * @retval None
  */ 
void i2c_start(I2C_TypeDef* I2Cx) {
#if MY_DRIVER
	
	//start
	I2Cx->CR1 |= (1<<8);
	//等待硬件把SB位置位  --  EV5
	if(i2c_wait_flag(I2Cx, 0x01)) {}
	
#else
	
	//start
	I2C_GenerateSTART(I2Cx, ENABLE);
	//等待硬件把SB位置位  --  EV5
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {}
	
#endif
}


/**
  * @brief  IIC停止条件
  * @param  None
  * @retval None
  */
void i2c_stop(I2C_TypeDef* I2Cx) {
#if MY_DRIVER
	
	//stop
	I2Cx->CR1 |= (1<<9);
	
#else
	
	I2C_GenerateSTOP(I2Cx, ENABLE);
	
#endif
}


/*	
	******************************************************************************
	*
	* 硬件I2C读写操作1
	* 从机没有指定地址读取或写入 & 没有命令和数据区分
	*
	******************************************************************************
	*/


/** 
  * @brief  IIC硬件连续写多个数据 - 非存储类芯片  无需指定地址 or 寄存器读取
  * @param  device_addr_w: 	设备地址-写指令   
  *         pdata: 			要写入的数据缓冲区首地址
  *         len: 			要写入的数据长度
	* @retval 成功返回:0    失败返回:1
  */
uint8_t i2c_master_transmit(I2C_TypeDef* I2Cx, uint8_t device_addr, uint8_t *pdata, uint16_t len){
	
	uint16_t i = 0;
	
	//发送起始条件  --  写数据
	i2c_start(I2Cx);
	
	//发送设备地址  
	I2C_Send7bitAddress(I2Cx, device_addr, I2C_Direction_Transmitter);
	
	//判断ADDR是否为1  若是1 代表地址发送成功
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {//判断ADDR位是否被置位  类似于等待应答的操作
		return 1;
	}
	
	for(i = 0; i < len; i++) { 
		//写入数据
		I2C_SendData(I2Cx, pdata[i]);
		
		//等待数据发送完成  TXE
		if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
				i2c_stop(I2Cx);  // 出错时发送停止条件
				return 1;
		}
	}
	
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		return 1;
	}
	
	//发送停止条件
	i2c_stop(I2Cx);

	
	//等待芯片内部写入完成
	delay_us(2);  			// 根据具体芯片调整延时
	return 0;   			//写入数据成功

}

/** 
  * @brief  IIC硬件连续读多个数据 - - 非存储类芯片  无需指定地址 or 寄存器读取
  * @param  device_addr_r: 设备地址-读指令
  *         pdata: 		   数据存储缓冲区首地址
  *         len: 		   要读取的数据长度
  * @retval 成功返回0，失败返回1
  */
uint8_t i2c_read_master_receive(I2C_TypeDef* I2Cx, uint8_t device_addr_r, uint8_t *pdata, uint16_t len) {
	
	uint16_t i = 0;
	
	//发送起始条件
	i2c_start(I2Cx);
	
	//发送设备地址 - 读数据
	I2C_Send7bitAddress(I2Cx, device_addr_r, I2C_Direction_Receiver);
	
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
		return 1;  //失败返回 1
	}
	
	//连续读取多个字节
	for(i = 0; i < len; i++) {
		if(i == len - 1) {
			I2C_AcknowledgeConfig(I2Cx, DISABLE);
			i2c_stop(I2Cx);  // 发送停止条件
		}
		
		if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
//            I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复ACK
			return 1;
		}
			
		//读取数据
		pdata[i] = I2C_ReceiveData(I2Cx);
	}
	
	//恢复应答使能
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	delay_us(2);
	return 0;  // 成功返回0
}
	



/*	
	******************************************************************************
	*
	* 硬件I2C读写操作2
	* 存储类芯片 & 带有内部寄存器的芯片 & 需要发送命令的芯片
	*
	******************************************************************************
	*/

/**
  * @brief  IIC硬件写单个字节 - 存储类芯片
  * @param  device_addr: 设备地址-写指令  
			addr:		 设备存储地址
			data:        写入的数据
  * @retval 0:成功  1:失败
  */
uint8_t i2c_write_Byte(I2C_TypeDef* I2Cx, uint8_t device_addr, uint8_t addr, uint8_t data) {
#if MY_DRIVER
	
	//发送起始条件  --  写数据
	i2c_start(I2Cx);
	
	//发送设备地址  
	I2Cx->DR = device_addr;
	
	//判断ADDR是否为1  若是1 代表地址发送成功   --  等待EV6  
	if(i2c_wait_flag(I2Cx, 0x02)) {  //判断ADDR位是否被置位  类似于等待应答的操作
		return 1;
	}
	(void)I2Cx->SR2;
	
	//发送设备存储地址
	I2Cx->DR = addr;
	if(i2c_wait_flag(I2Cx, 0x80)) {
		return 1;
	}
	
	//在指定位置写入数据
	I2Cx->DR = data;
	if(i2c_wait_flag(I2Cx, 0x04)) {
		return 1;
	}
	
	//stop
	i2c_stop(I2Cx);
	
	return 0;     //写入数据成功
	
#else

	//发送起始条件  --  写数据
	i2c_start(I2Cx);
	
	//发送设备地址  
	I2C_Send7bitAddress(I2Cx, device_addr, I2C_Direction_Transmitter);
	
	//判断ADDR是否为1  若是1 代表地址发送成功
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {//判断ADDR位是否被置位  类似于等待应答的操作
		return 1;
	}
	
	//发送设备存储地址
	I2C_SendData(I2Cx, addr);
	
	//判断移位寄存器
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
		return 1;
	}
	
	//在指定位置写入数据
	I2C_SendData(I2Cx, data);
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		return 1;
	}
	
	//stop
	i2c_stop(I2Cx);

	return 0;   //写入数据成功
	
#endif
}
	
/**
  * @brief  IIC硬件读单个字节 - 存储类芯片
  * @param  device_addr_w: 		设备地址-写指令   
			device_addr_r: 		设备地址-读指令
			addr: 				设备存储地址
			*data:				数据存储缓冲区首地址
  * @retval 传输出错返回1  成功返回0
  */
uint8_t i2c_read_Byte(I2C_TypeDef* I2Cx, uint8_t device_addr_w, uint8_t device_addr_r, uint8_t addr, uint8_t *data) {
	
#if MY_DRIVER
	
	//发送起始条件
	i2c_start(I2Cx);
	
	//发送设备地址    --  写数据
	I2Cx->DR = device_addr_w;
	
	//判断ADDR是否为1  若是1 代表地址发送成功
	if(i2c_wait_flag(I2Cx, 0x02)) { //判断ADDR位是否被置位  类似于等待应答的操作
		return 1;
	}
	(void)I2Cx->SR2;
	
	//发送设备存储地址 
	I2Cx->DR = addr;
	if(i2c_wait_flag(I2Cx, 0x80)) {
		return 1;
	}
	
	//再次发起起始条件 
	i2c_start(I2Cx);
	
	//发送设备地址    --  读数据 
	I2Cx->DR = device_addr_r;
	
	//判断ADDR是否为1  若是1 代表地址发送成功
	if(i2c_wait_flag(I2Cx, 0x02)) { //判断ADDR位是否被置位  类似于等待应答的操作
		return 1;
	}
	(void)I2Cx->SR2;
	
	I2Cx->CR1 &= ~(1<<10);
	
	i2c_stop(I2Cx);
	
	if(i2c_wait_flag(I2Cx, 0x40)) { //等待移位寄存器把第一个数据放进DR寄存器中
		return 1;
	}
	
	//读取DR会清除掉RXNE位
	*data = I2Cx->DR;
	
	//恢复应答使能（避免影响后续通信）
	I2Cx->CR1 |= (1 << 10);  // ACK=1（应答）
	
	return *data;
	
#else
	
	//发送起始条件
	i2c_start(I2Cx);
	
	//发送设备地址
	I2C_Send7bitAddress(I2Cx, device_addr_w, I2C_Direction_Transmitter);
	
	//判断ADDR是否为1  若是1 代表地址发送成功
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {//判断ADDR位是否被置位  类似于等待应答的操作
		return 1;
	}
	
	//发送设备存储地址 
	I2C_SendData(I2Cx, addr);
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
		return 1;
	}
	
	//再次发起起始条件 
	i2c_start(I2Cx);
	
	//发送设备地址
	I2C_Send7bitAddress(I2Cx, device_addr_r, I2C_Direction_Receiver);
	
	//判断ADDR是否为1  若是1 代表地址发送成功
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {//判断ADDR位是否被置位  类似于等待应答的操作
		return 1;
	}
	
	I2C_AcknowledgeConfig(I2Cx, DISABLE);

	i2c_stop(I2Cx);
	
	//读取数据之前先判断RXNE位 如果是1  表明DR寄存器有数据
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {  //等待移位寄存器把第一个数据放进DR寄存器中
		return 1;
	}
	
	//读取DR会清除掉RXNE位
	*data = I2C_ReceiveData(I2Cx);
	
	//恢复应答使能（避免影响后续通信）
	I2C_AcknowledgeConfig(I2Cx, ENABLE);  // ACK=1（应答）
	
	return *data;

#endif
}



/*	
	******************************************************************************
	*
	*	可读单个字节 & 读连续多个字节
	*
	******************************************************************************
	*/


/** 
  * @brief  IIC硬件连续写多个数据 - 存储类芯片
  * @param  device_addr_w: 	设备地址-写指令   
  *         addr: 			设备存储起始地址
  *         pdata: 			要写入的数据缓冲区首地址
  *         len: 			要写入的数据长度
	* @retval 成功返回:0  失败返回:1
  */
uint8_t i2c_write_multibyte(I2C_TypeDef* I2Cx, uint8_t device_addr_w, uint16_t addr, uint8_t *pdata, uint16_t len) {
	
	uint8_t i = 0;
	
#if MY_DRIVER
	
	//发送起始条件
	i2c_start(I2Cx);
	
	//发送设备地址 - 写数据
	I2Cx->DR = device_addr_w;
	
	//等待EV6 - 地址发送成功
	if(i2c_wait_flag(I2Cx, 0x02)) {
			return 1;
	}
	(void)I2Cx->SR2;
	
	//发送设备存储起始地址
	I2Cx->DR = addr;
	if(i2c_wait_flag(I2Cx, 0x80)) {
			return 1;
	}
	
	//连续写入多个字节
	for(i = 0; i < len; i++) {
			//发送数据字节
			I2Cx->DR = pdata[i];
			
			if(i2c_wait_flag(I2Cx, 0x80)) {
					i2c_stop(I2Cx);  // 出错时发送停止条件
					return 1;
			}
	}
	
	//等待EV8_2--BTF
	if(i2c_wait_flag(I2Cx, 0x04)) {
		return 1;
	}
	
	//发送停止条件
	i2c_stop(I2Cx);
	
#else
	
	//发送起始条件
	i2c_start(I2Cx);
	
	//发送设备地址 - 写数据
	I2C_Send7bitAddress(I2Cx, device_addr_w, I2C_Direction_Transmitter);
	
	//地址发送成功
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
			return 1;
	}
	
	//发送设备存储起始地址
	I2C_SendData(I2Cx, addr);
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
			return 1;
	}
	
	//连续写入多个字节
	for(i = 0; i < len; i++) {
			//发送数据字节
			I2C_SendData(I2Cx, pdata[i]);
			
			if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
					i2c_stop(I2Cx);  // 出错时发送停止条件
					return 1;
			}
	}
	
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		return 1;
	}
	
	//发送停止条件
	i2c_stop(I2Cx);
	
#endif

	//等待存储芯片内部写入完成
	delay_us(2);  // 根据具体芯片调整延时
	return 0;  	  // 成功返回0
}


/** 
  * @brief  IIC硬件连续读多个数据 - 存储类芯片
  * @param  device_addr_w: 设备地址-写指令   
  *         device_addr_r: 设备地址-读指令
  *         addr: 		   设备存储起始地址
  *         pdata: 		   数据存储缓冲区首地址
  *         len: 		   要读取的数据长度
  * @retval 成功返回0，失败返回1
  */
uint8_t i2c_read_multibyte(I2C_TypeDef* I2Cx, uint8_t device_addr_w, uint8_t device_addr_r, uint16_t addr, uint8_t *pdata, uint16_t len) {
	
	uint8_t i = 0;
	
#if MY_DRIVER
	
	//发送起始条件
	i2c_start(I2Cx);
	
	//发送设备地址 - 写数据
	I2Cx->DR = device_addr_w;
	
	//等待EV6 - 地址发送成功
	if(i2c_wait_flag(I2Cx, 0x02)) {
			return 1;
	}
	(void)I2Cx->SR2;
	
	//发送设备存储起始地址
	I2Cx->DR = addr;
	if(i2c_wait_flag(I2Cx, 0x80)) {
			return 1;
	}
	if(i2c_wait_flag(I2Cx, 0x04)) {
		return 1;
	}
	
	i2c_start(I2Cx);
	
	//发送设备地址 - 读数据
	I2Cx->DR = device_addr_r;
	
	if(i2c_wait_flag(I2Cx, 0x02)) {
			return 1;
	}
	(void)I2Cx->SR2;
	
	//连续读取多个字节
	for(i = 0; i < len; i++) {
			if(i == len - 1) {
				I2Cx->CR1 &= ~(1 << 10);  // ACK失能
				i2c_stop(I2Cx);           // 发送停止条件
			}
			
			if(i2c_wait_flag(I2Cx, 0x40)) {
//              I2Cx->CR1 |= (1 << 10);   // 恢复ACK
				return 1;
			}
			
			//读取数据
			pdata[i] = I2Cx->DR;	
	}
	
	//恢复应答使能
	I2Cx->CR1 |= (1 << 10);
	
#else
	
	//发送起始条件
	i2c_start(I2Cx);
	
	//发送设备地址 - 写数据
	I2C_Send7bitAddress(I2Cx, device_addr_w, I2C_Direction_Transmitter);
	
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
			return 1;
	}
	
	I2C_SendData(I2Cx, addr);
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
			return 1;
	}
	
	i2c_start(I2Cx);
	
	//发送设备地址 - 读数据
	I2C_Send7bitAddress(I2Cx, device_addr_r, I2C_Direction_Receiver);
	
	if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
			return 1;
	}
	
	//连续读取多个字节
	for(i = 0; i < len; i++) {
			if(i == len - 1) {
					I2C_AcknowledgeConfig(I2Cx, DISABLE);
					i2c_stop(I2Cx);  // 发送停止条件
			}
			
			if(i2c_wait_event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
//            I2C_AcknowledgeConfig(I2Cx, ENABLE);  // 恢复ACK
				return 1;
			}
			
			//读取数据
			pdata[i] = I2C_ReceiveData(I2Cx);
	}
	
	//恢复应答使能
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	
#endif

	delay_us(2);
	return 0;  // 成功返回0
}

