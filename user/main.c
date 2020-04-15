

#include <stm32f4xx.h>
#include "led.h"
#include "key.h"
#include "uart1.h"
#include "mysystick.h"
#include "dht11.h"
#include "24c02.h"
#include "ADC1_0_gm.h"
#include "w25q64.h"
#include "oled.h"

#include <stdio.h>
/*
	使用标准库的方法，点亮led1

*/


#if 0
void my_systeminit(void)
{
	//1.打开外部晶振电路
	RCC->CR |= 1<<16;     
	//2.等着外部晶振电路的稳定
	while(!(RCC->CR & (1<<17)));
	
	//3.PLL打开之前，我们配置一下PLL的参数
	RCC->PLLCFGR &= 0xff000000;    //低24位全部清零
	
	//n= 336,p = 2,m = 8,PLL时钟源选择HSE
	RCC->PLLCFGR |= 1<<22 | 336<<6 | 8;
	
	//4.开启PLL
	RCC->CR |= 1<<24; 
	while(!(RCC->CR & (1<<25)));  //等待PLL时钟稳定
	
	//5.APB2 不能超过84M APB1不能超过42M
	RCC->CFGR |= 4<<13 | 5<<10;
		
	//6.****调整flash的读取速度（cpu的频率加速，但是flash的速度没有cpu那么快，这时候系统系统就死机）
	FLASH->ACR = 7<<8 | 5;
	
	
	//7.把系统时钟切换到PLL时钟
	RCC->CFGR |= 2;
	
	while((RCC->CFGR & 0xc) != 0x8);
	
}
#endif








void delay(unsigned int n)
{
	uint32_t i;
	
	for(i=0;i<n;i++)
		;
}


void serial_cmd_handle(void)
{
	uint8_t i;
	uint8_t check_sum = 0; //校验和
	
	//校验和处理
	for(i=0;i<recv_buf[2]+2;i++)
	{
		check_sum += recv_buf[i];
	}
	
	printf("serial_cmd_handle\n");
	if(check_sum == recv_buf[recv_buf[2]+2] )  //检验和成功
	{
		printf("check ok\n");

		if((recv_buf[3] == 1) || (recv_buf[3] == 2) || (recv_buf[3] == 3) || (recv_buf[3] == 0xf))
		{
			if(recv_buf[3] == 0xf)
			{
				recv_buf[3] = 0;
			}
			led_set_status((ledn_t)recv_buf[3],(led_sta_t)recv_buf[4]);
		
			printf("len handle\n");
		}
	}
}






int main(void)
{		
	uint8_t key_val;
//	uint8_t temp_val;   //温度
//	uint8_t humi_val;   //湿度
	
	uint8_t buf[50];
	uint8_t i;

	
	//0.设置中断优先级分组，一旦设置之后，就不要修改了
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	
	my_systick_int_init(168);   //168表示现在的时钟频率

	
	//1.led初始化，熄灭
	led_init();
	
	//2.初始化按键
	key_init();
	
	//flash 初始化
	w25q64_init();
	
	//3.串口1的初始化,可以指定波特率,使用中断接收
  
	uart1_int_init(115200);


	//光敏电阻ADC初始化
	adc1_gm_init();

	//温湿度传感器初始化
	DHT11_init();

	//eeprom初始化
//	AT24c02_init();
	
	
	printf("w25q64 id = 0x%x\n",w25q64_readID());

	

//	for(i=0;i<50;i++)
//	{
//		buf[i] = i*2+20;
//		
//	}
//	w25q64_write_data(100, buf,50);


	w25q64_read_data(50, buf,50);
	for(i=0;i<50;i++)
	{
		printf("%x ",buf[i]);
		if(i%10 == 9)
		{
			printf("\n");
		}
	}
	printf("\n");
	
	
	
	w25q64_read_data(100, buf,50);
	for(i=0;i<50;i++)
	{
		printf("%d ",buf[i]);
		if(i%10 == 9)
		{
			printf("\n");
		}
	}
	printf("\n");
	

	printf("hello arm %d-%d-%d\n",2020,4,9);
	oled_init();

//	OLED_ShowChar(0,0,'c',16);
	OLED_ShowString(0,0,"2020-04-10",16);


	//按下按键，led2点亮，松开led2熄灭	
	while(1)
	{
		//是不是收到串口数据
		if(recv_flag)
		{
			recv_flag = 0;  //对于刚刚接受的数据，已经进行了处理
	
			serial_cmd_handle();		
		}
		
		if(key_flag)   //检测到按键了
		{
			key_val = key_flag;
			key_flag = 0;
			printf("key is %d\n",key_val);
		}
		
//		if(DHT11_read(&humi_val,&temp_val) == 0)
//		{
//			printf("humi = %d temp = %d\n",humi_val,temp_val);
//		}
//		else
//		{
//			printf("humi temp read error\n");
//		}
		
//		printf("guang min val = %d\n",get_gm_val());

		
		
		led_set_status(LED2,LEDON);	
		delay_ms(700);	
		led_set_status(LED2,LEDOFF);
		delay_ms(700);
	
	}
	
}

