

#include "mysystick.h"
#include "key.h"


static uint16_t tick_total;   //重载的tick总数
static uint16_t tick_fact;    //每一个微秒的tick数


/*
	systick定时器初始化,systick中断
	systick 选择1ms中断，（在1s内，这个中断会中断我cpu1000次）
	参数 ： sysclock AHB时钟，单位为MHz
*/
void my_systick_int_init(uint8_t sysclock)
{
	//
	tick_total = sysclock*1000/8;
	tick_fact = (sysclock + 7) / 8;
	
	SysTick_Config(tick_total);
	
	//修改一下时钟源
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}


/*
	us 的延时时间798915us 
	//取一个整数，好计算

	us的延时不能超过70,0000us

*/
void delay_us(uint32_t dus)
{
	uint32_t now;
	int ticks;   //我需要等待的脉冲数
	uint32_t t;
	int k;
	
	if(dus == 0)
	{
		return;
	}	
		
	now = SysTick->VAL;    //我先把当前计数的数字记下来

	ticks = dus*tick_fact;   //这个变量的最大值 21亿，1亿us，大约100s
							 //我等待dus需要经过的ticks数
	while(ticks > 0)
	{
		t = SysTick->VAL;   //就是现在的计数
		k = now - t;    //k表示已经过去的ticks
		if(k>=0)
		{
			ticks -= k;			
		}
		else
		{
			ticks -= tick_total+k;  //因为k是负数，两个值加起来是一个小的整数，正好是已经过去的ticks
		}
		now = t;
	}		
}

static volatile uint32_t delay_count = 0;
/*
	延时函数
	单位是ms
*/
void delay_ms(uint32_t dms)
{
	delay_count = dms;
	
	while(delay_count);   //如果不为0，就继续等
}



/*

	定义好时间间隔：就是多少时间中断一次 1ms

	每过1ms，这个函数就会被调用一次

*/

static uint32_t count = 0;

void SysTick_Handler(void)
{
	count ++;
	if(count % 20 == 0)  //每过20ms，扫描一次按键
	{
		key_scan();  //执行时间越短越好
	}
	
		
	if(delay_count)
	{
		delay_count --;
	}
}


