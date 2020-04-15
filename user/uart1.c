

#include "uart1.h"


/*
	串口1的使用
	 115200，8N1
	
	对应的引脚
	GPA9，GPA10  （TX,RX）

*/
uint8_t recv_buf[8];   //用来存储串口的数据
uint8_t recv_flag = 0;  //接收标志，为1时表示接收到了数据，为0的时候表示没有接收到数据



void uart1_init(uint32_t bandrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	
	//1.时钟使能，（不使能，外部设备不能使用）
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//复用的功能很多，设置具体的复用功能
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	//2.配置IO端口的模式（复用功能）
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;	
	GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
		
	//3.初始化串口控制器
	USART_StructInit(& USART_InitStruct);  //初始化结构体
	USART_InitStruct.USART_BaudRate = bandrate;
	USART_Init(USART1, &USART_InitStruct);



	//4.开启串口控制器
	USART_Cmd(USART1, ENABLE);
}



void uart1_int_init(uint32_t bandrate)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	
	uart1_init(bandrate);   //初始化串口
		
	//1.串口的中断有好几个，我们只使能接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	//2.配置NVIC，允许串口1的中断，并且设置优先级
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
}




//这个函数只在这个文件使用，其他文件不能调用这个函数
static void uart1_putchar(uint8_t dat)
{
	//查询一下是不是发送寄存器为空
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));  //USART_GetFlagStatus返回值是0，表示发送缓存不为空
	
	USART_SendData(USART1, dat);
}


//串口发送字符串
void uart1_putstr(uint8_t dat[])
{	
	while(*dat)
	{
		uart1_putchar(*dat++);
	}
}





//需要使用printf,需要实现fputc这个函数
int fputc(int ch, FILE *f)
{
	uart1_putchar(ch);
	return 1;
}




//接收串口的数据
uint8_t	uart1_getchar(void)
{
	//查询是不是有数据 阻塞函数
	while(!USART_GetFlagStatus(USART1, USART_FLAG_RXNE)); //返回值是0，表示没有接收到数据，阻塞
	
	return USART_ReceiveData(USART1);  //把接收的数据读出来
	
	
}



//串口1的中断处理函数
//因为只开启了接收中断，只有接收到数据，才会引起中断
void USART1_IRQHandler(void)
{
	static uint8_t n = 0;
	
	//读取中断接收的数据到全局变量中	
	recv_buf[n++] = USART_ReceiveData(USART1);
		
	if(n>7)  //防止缓存溢出
	{
		n = 0;
	}
	
	if(n==1)  //已经接收到第一个数据
	{
		if(recv_buf[0] != 0x55)
		{
			n = 0;
		}
	}
	else if(n == 2) //已经接收到第二个数据
	{
		if(recv_buf[1] != 0xaa)
		{
			n = 0;
		}
	}
	else if(n > 3) //已经收到帧头，还有数据的长度
	{
		if(n == recv_buf[2]+3 )
		{
			n = 0;
			recv_flag = 1;	   //表示我接收到了一帧完整的数据
		}
	}
	
	//清中断标志，
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}



