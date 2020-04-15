
#include "spi1.h"


/*
	硬件SPI 初始化
	
	软件片选
	
	spi的3个硬件接口是GPIOB 3，4，5号引脚
		
*/

void spi1_init(void)
{
	static uint8_t ready = 0;	   //非0表示已经初始化完成
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	if(ready == 0)
	{
		//1.时钟初始化
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
			
		//2.io复用功能配置
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;  //推挽模式，通信的频率达到更高
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;   //注意一下输出频率	
		GPIO_Init(GPIOB, &GPIO_InitStruct);	
		
		//2.1具体的复用功能
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
		
		//SPI控制器初始化
		SPI_StructInit(&SPI_InitStruct);
		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;  //主机模式
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;     //不使用硬件片选信号
		SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
		SPI_Init(SPI1, &SPI_InitStruct);
		
		SPI_Cmd(SPI1, ENABLE);
		
		ready = 1;   //已经完成初始化工作，下一次再调用这个函数，就不再初始化了
	}
}


uint8_t spi1_write_read(uint8_t dat)
{
	//是不是空闲，如果有数据就等待
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, dat);
	
	//是不是接收完了？没有就等待
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);	
}


