

#include "hardiic.h"

/*
	PB6  SCL 
	PB7  SDA 
*/

//i2c1的硬件初始化
void i2c1_init(void)
{
	static i2c1_ready = 0;	
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	if(i2c1_ready == 0)
	{	
		//1.时钟使能
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		
		//2.gpio的功能配置，复用功能
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;   //开漏输出模式
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //	
		GPIO_Init(GPIOB, &GPIO_InitStruct);  //这个函数，让结构体的内容配置到寄存器中
		
		//2.1 指定复用为iic的引脚
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
		
		
		//3.iic控制器的初始化
		I2C_StructInit(&I2C_InitStruct);     //应答是非应答
		I2C_InitStruct.I2C_ClockSpeed = 400000;   //最大不能超过400k
		I2C_Init(I2C1, &I2C_InitStruct);
		
		I2C_Cmd(I2C1, ENABLE);
	
		i2c1_ready = 1; //表示已经初始化了，下次调用这个函数就不再初始化
	}
}



//产生起始信号
//返回值 0 --》 成功
//		非0 --》失败
int8_t i2c1_start(void)
{
	uint16_t times = 0;
	
	I2C_GenerateSTART(I2C1, ENABLE);
	
	//检查ev5这个事件
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)!= SUCCESS)
	{
		times ++;
		if(times > 2000)
		{
			return -1;
		}
	}
	return 0;
}



//产生停止信号
void i2c1_stop(void)
{
	I2C_GenerateSTOP(I2C1, ENABLE);
}



//发送从设备地址
int8_t i2c1_put_devaddr(uint8_t addr)
{
	uint16_t times = 0;
	uint32_t event = I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED;   //发送器模式
	
	I2C_Send7bitAddress(I2C1, addr, addr&1);
	
	if(addr&1)   //接收模式
	{
		event = I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED;
	}
	
	/*
	    @arg I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: EV6     
        @arg I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: EV6
	*/
	
	//检查ev6
	while (I2C_CheckEvent(I2C1, event)!= SUCCESS)
	{
		times ++;
		if(times > 2000)
		{
			return -1;
		}
	}
	return 0;
}


//发送数据
void i2c1_put_byte_data(uint8_t dat)
{
	I2C_SendData(I2C1, dat);

	

}



uint8_t i2c1_get_ack(void)
{
	uint16_t times = 0;

	//检查ev8_2
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)!= SUCCESS)
	{
		times ++;
		if(times > 800)
		{
			return 254;
		}
	}
	return 0;
}


uint8_t i2c1_get_byte_data(void)
{
	uint8_t dat;
	
	uint16_t times = 0;

	//检查ev7
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)!= SUCCESS)
	{
		times ++;
		if(times > 2000)
		{
			return 254;
		}
	}
	
	dat = I2C_ReceiveData(I2C1);
	
	return dat;
}

void i2c1_send_ack(uint8_t ack)
{
	if(ack == NO_ACK)  //发送非应答信号
	{
		I2C_AcknowledgeConfig(I2C1, DISABLE);
	}
	else
	{
		I2C_AcknowledgeConfig(I2C1, ENABLE);
	}

}


