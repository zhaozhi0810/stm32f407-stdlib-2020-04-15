

#include "mysystick.h"
#include "i2c1.h"

#define SDA_PIN GPIO_Pin_7
#define SCL_PIN GPIO_Pin_6



/*
	i2c1 软件模拟时序实现
	
	初始化程序
	
	配置引脚的属性
	
	
	PB6  SCL OD输出
	PB7  SDA 有输入有输出，OD输出模式

*/
void i2c1_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct;  //定义结构体，把需要配置的内容填写到结构体中

	//1.时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		
	//2.输出高电平
	GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7 );	
	
	//3.配置为输出方式，输出高电平（熄灭）
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;   //开漏输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //	
	GPIO_Init(GPIOB, &GPIO_InitStruct);  //这个函数，让结构体的内容配置到寄存器中

}


void i2c1_start(void)
{
	//1.
	GPIO_SetBits(GPIOB, SDA_PIN | SCL_PIN );    //两个引脚都是高电平
	delay_us(2);
	
	//SCL为高电平的时候，SDA由高变低
	GPIO_ResetBits(GPIOB,  SDA_PIN );   //SDA变低电平
	delay_us(2);
	
	//？？？要不要把SCL拉低
	GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
}



void i2c1_stop(void)
{
	//SCL为低电平
	GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
	//SDA为低电平
	GPIO_ResetBits(GPIOB,  SDA_PIN );   //SDA变低电平
	
	delay_us(2);
	
	GPIO_SetBits(GPIOB, SCL_PIN );   //SCL为高
	delay_us(2);
	
	//SDA由低变为高电平 产生stop信号
	GPIO_SetBits(GPIOB, SDA_PIN );   //SDA为高
	delay_us(2);
	
	//stop之后，SDA和SCL都为高电平
}



//i2c 发送一个字节
//这个函数不检验有没有应答
void i2c1_put_byte_data(uint8_t dat)
{
	uint8_t i;
		
	for(i=0;i<8;i++)
	{
		//1.SCL为低
		GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
		delay_us(1);
		
		//2.在SCL为低电平的时候，可以在SDA上准备数据
		if((dat >> 7-i) & 1)  //高电平
		{
			GPIO_SetBits(GPIOB, SDA_PIN );   //SDA为高
		}
		else  //低电平
		{
			GPIO_ResetBits(GPIOB,  SDA_PIN );   //SDA变低电平
		}
		delay_us(1);
		
		//SCL为高电平，数据就发出去了
		GPIO_SetBits(GPIOB, SCL_PIN);   //SCL为高
		delay_us(4);
	}
	//这个函数退出的时候SCL是低电平
	GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
	delay_us(1);
}

//接收从设备的应答信号
//有应答，返回0
//无应答，返回1
uint8_t i2c1_get_ack(void)
{
	uint8_t ret = 0;
	//读取对方的应答	
//	GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
//	delay_us(1);

	//1.拉高SDA	主要的原因是OD输出模式
	GPIO_SetBits(GPIOB, SDA_PIN );   //SDA为高
	delay_us(1);
	
	GPIO_SetBits(GPIOB, SCL_PIN);   //SCL为高
	delay_us(2);
	
	//在SCL为高的时候 ，读取SDA的引脚电平
	if(GPIO_ReadInputDataBit(GPIOB, SDA_PIN ))
	{
		ret = 1;
	}
	//这个函数退出的时候SCL是低电平
	GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
	delay_us(1);
	
	return ret;
}


/*
	从 从设备 读取一个字节的数据

*/
uint8_t i2c1_get_byte_data(void)
{
	uint8_t i;
	uint8_t dat;

	//1.SCL为低
	GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
	delay_us(1);
	
	//2.拉高SDA	
	GPIO_SetBits(GPIOB, SDA_PIN );   //SDA为高
	delay_us(1);
	
	
	for(i=0;i<8;i++)
	{
		dat <<= 1;
		
		//3.拉高SCL	
		GPIO_SetBits(GPIOB, SCL_PIN );   //SCL为高
		delay_us(1);
		
		//4.SCL为高的时候，才是真正的数据
		if(GPIO_ReadInputDataBit(GPIOB, SDA_PIN ))
			dat |= 1;    //读到的是高电平，那么最低位置一
		
		//5.SCL为低,让24c02准备下一个数据
		GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
		delay_us(4);		
	}
	
	return dat;
}



void i2c1_send_ack(uint8_t ack)
{
	//1.SCL为低
	GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
	delay_us(1);
	
	if(ack == NO_ACK)  //发送非应答信号
	{
		GPIO_SetBits(GPIOB, SDA_PIN );   //SDA为高	
	}
	else
	{
		GPIO_ResetBits(GPIOB, SDA_PIN );   //SDA为低
	}
	delay_us(1);
	
	GPIO_SetBits(GPIOB,  SCL_PIN );   //SCL变高电平，（非）应答周期才发送出去
	delay_us(2);
	
	GPIO_ResetBits(GPIOB,  SCL_PIN );   //SCL变低电平
	delay_us(1);
	
}




