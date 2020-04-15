

#include "mysystick.h"


/*
	systick定时器初始化
	仅仅用于定时
*/
void my_systick_init(void)
{
	SysTick->LOAD = 1000;   //重载值
	SysTick->VAL = 0;       //当前值
	SysTick->CTRL = 0;     //使用外部系统时钟，不使能定时器，不产生中断
}


/*
	us 的延时时间798915us 
	//取一个整数，好计算

	us的延时不能超过70,0000us

*/
void delay_us(uint32_t dus)
{
	if(dus == 0)
	{
		return;
	}	
	else if(dus > 700000)
	{
		return;
	}
		
	SysTick->LOAD = dus * 21 -1;
	SysTick->VAL = 0; 
	SysTick->CTRL |= 1;    //开启定时器
	
	while((SysTick->CTRL & 1<<16) == 0);  //等着定时器时间到
	
	SysTick->CTRL = 0;    //关闭定时器
	
}


/*
	注意超过700ms的处理，会有更大的误差

	能够延时的最大的ms 65536 ms

*/
void delay_ms(uint16_t dms)
{
	while(dms >= 700)
	{
		delay_us(700000);
		dms -= 700;
	}
		
	if(dms < 700)
	{
		delay_us(dms*1000);
	}
}



/*
	SysTick_Config(uint32_t ticks)



	定义好时间间隔：就是多少时间中断一次 1ms

	每过1ms，这个函数就会被调用一次

*/
void SysTick_Handler(void)
{

}


