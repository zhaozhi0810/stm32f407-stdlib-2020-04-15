
#include "key.h"




/*
	KEY1、2、3 对应PE端口的4、5、6引脚
	KEY4 对应PC端口的13号引脚

	按下时，低电平
	松开时，高电平
*/

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//1.使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//2.初始化为输入模式（key1、2、3）
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	//key4的初始化
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
}




/*
	按键中断方式的初始化
	
*/
void key_int_init(void)
{
	EXTI_InitTypeDef  EXTI_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//中断模式的时候，按键的io端口依然配置为输入模式
	key_init();
	
	//syscfg的时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  //时钟使能
	
	
	//1.配置中断引脚 exti4 选择 PE4这个引脚
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);    //Pe4配置为外部中断4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource6);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
	
	
	
	//2.对外部中断4进行配置，使能，设置触发方式
	EXTI_InitStruct.EXTI_Line = EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line13;   //指定哪一个外部中断
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;  //使能还是禁止
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;     //中断还是事件
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  //触发方式，下降沿 rise fall
	EXTI_Init(&EXTI_InitStruct);
	
	//3.NVIC的初始化，优先级，中断允许
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;   //外部中断4
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;    //抢占优先级 0-3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;           //响应优先级 0-3
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStruct);
}





/*
	功能： 获取按键的状态
	注意：不支持多个按键同时按下的情况，
	参数 ： 无
	返回值： 0 表示没有按键按下
			 1 表示按键1被按下
			 2 表示按键2被按下
			 3 表示按键3被按下
			 4 表示按键4被按下
*/
uint8_t get_key_val(void)
{
	//按键按下之后是低电平，松开之后是高电平
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == Bit_RESET)
	{
		return 1;
	}	
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == Bit_RESET)
	{
		return 2;
	}
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) == Bit_RESET)
	{
		return 3;
	}
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_RESET)
	{
		return 4;
	}
	

	
	return 0;
	
}



/*
	使用定时器扫描按键 每20ms扫描一次
	两次扫描同一个按键值表示有按键按下
	否则就是抖动

*/

uint8_t key_flag = 0;   //非0表示检测到按键按下，0表示没有检测到按键

void key_scan(void)
{
	uint8_t key_val = 0;
	static uint8_t key_val_old = 0;  //保存上一次扫描的按键的值
	static uint8_t key_stat = 0;     //0表示没有检测到按键，1表示已经检测到按键，并且没有松开
	
	
	key_val = get_key_val();   //获得按键的值
	
	if(key_stat && key_val == key_val_old)
	{
		return;
	}
	else if(key_val != key_val_old)  //这个按键值与之前的按键值不同
	{
		key_stat = 0;
	}
	
	//有检测按键，并且这一次检测到的和上一次检测的是一样按键
	if(key_val != 0 && key_val == key_val_old)
	{
		key_flag = key_val;   //kay_flag的值表示哪一个按键被按下
		key_stat = 1;
	}
	else //可能的情况：1.没有检测到按键 2.两次检测的值不同
	{
		key_val_old = key_val;
	}
	
}















//中断处理函数
/*这个函数只有在中断的触发的时候被调用，不需要main或者其他函数调用
	是由硬件自己完成的跳转，不是显式去调用
	1.没有参数，没有返回值
	2.不能阻塞函数
	3.应该越快退出中断处理越好（当然需要满足一些功能处理）
*/

void EXTI4_IRQHandler(void)
{	
	//翻转一下led1的状态
	GPIO_ToggleBits(GPIOE, GPIO_Pin_8);
	
	//清除中断标志
	EXTI_ClearITPendingBit(EXTI_Line4);
}

/*
	这个函数可以处理5个外部中断，区分是哪一个中断
*/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5))  //为1的时候，表示触发了外部中断5
	{
		//翻转一下led1的状态
		GPIO_ToggleBits(GPIOE, GPIO_Pin_9);
		//清除中断标志
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	
	else if(EXTI_GetITStatus(EXTI_Line6))
	{
		//翻转一下led1的状态
		GPIO_ToggleBits(GPIOE, GPIO_Pin_10);
		//清除中断标志
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
		
}



void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line13))
	{
		//翻转一下led1的状态
		GPIO_ToggleBits(GPIOE, GPIO_Pin_8);
		
		//清除中断标志
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}

