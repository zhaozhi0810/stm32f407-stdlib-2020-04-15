


#include "dht11.h"
#include "mysystick.h" 

/*
	DHT11 连接在单片机的GPA3 引脚上
	并且有上拉电阻
	
*/


void DHT11_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//1.使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

		//把引脚设置为高电平
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	
	//2.初始化为输出模式
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;   //开漏输出模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;	//电路本身有上拉电阻，我就不设置啦
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}





//从DHT11读取数据位，每次调用读取8个位，并返回
static uint8_t DHT11_read_bitdat(void)
{
	uint8_t i;
	uint8_t dat;
	
	
	for(i=0;i<8;i++)
	{	
		dat <<= 1;   //左移一位
		
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)) ; //等待引脚变为低电平
		
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0);  // 等待引脚变为高电平
		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3))   //测量高电平的时间
		{
			delay_us(30);
		}
		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3))  //30us之后还是高电平，DHT11传递给我的数据就是数据位1
			dat |= 1;   //有条件修改最低位
				
	}
	
	return dat;
}






/*
	通过参数把温度和湿度返回
	返回值：
           0，表示成功读取到温度和湿度
		   其他值，表示读取失败
*/
uint8_t DHT11_read(uint8_t * humi,uint8_t *temp)
{
	uint8_t retry = 0;
	uint8_t i;
	uint8_t buf[5];
	
	
	//1.由单片发送起始信号
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	delay_ms(20);   //拉低20ms
	//转为高电平
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	delay_us(30);   //拉高30us
	
		
	//等到DHT11的响应和数据
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3))  //等待引脚变为低电平
	{
		delay_us(2);
		retry++;		
		if(retry > 10)
		{	
			return 1;      //出错
		}
	}
	retry = 0;
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0)  //等待引脚由低电平变为高电平
	{
		delay_us(10);
		retry++;		
		if(retry > 10)
		{	
			return 1;      //出错
		}
	}

	//读取数据
	for(i=0;i<5;i++)
	{
		buf[i] = DHT11_read_bitdat();
	}
	
	
	if(buf[0]+buf[1]+buf[2]+buf[3] == buf[4]) //检验成功
	{
		*humi = buf[0];
		*temp = buf[2];
		
		return 0;   //成功
	}
	
	return 1;   //错误
}




