
#include "led.h"
//#include <stm32f4xx.h>






void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;  //定义结构体，把需要配置的内容填写到结构体中

	//1.时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//2.配置为输出方式，输出高电平（熄灭）
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //	
	GPIO_Init(GPIOE, &GPIO_InitStruct);  //这个函数，让结构体的内容配置到寄存器中
	
	//3.输出高电平
	GPIO_SetBits(GPIOE, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);	
}




void led_set_status(ledn_t ledn,led_sta_t status)
{
	const uint16_t pin[] = {GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10,GPIO_Pin_8 ,GPIO_Pin_9,GPIO_Pin_10};
	
	
	if(ledn > LED3)
	{
		return ;
	}
	
	
	if(status == LEDOFF)
	{			
		GPIO_SetBits(GPIOE, pin[ledn]);   //设置这个引脚为高电平
	}
	else //if(status == LEDON)
	{
		GPIO_ResetBits(GPIOE, pin[ledn]); //设置这个引脚为低电平
	}
}


/*
	能够显示的就是0-7的二进制表示
	熄灭表示0，点亮表示1，led1是最低位
*/
void led_show_num(uint8_t num)
{
	num = ~num; //0,表示要熄灭所有的灯	
	
	GPIO_WriteBit(GPIOE, GPIO_Pin_8, (BitAction)(num&1));
	GPIO_WriteBit(GPIOE, GPIO_Pin_9, (BitAction)((num>>1) & 1));
	GPIO_WriteBit(GPIOE, GPIO_Pin_10, (BitAction)((num>>2) & 1));
}


