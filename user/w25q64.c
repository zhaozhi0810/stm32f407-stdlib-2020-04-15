

#include "w25q64.h"

/*
	PB2 是片选引脚

*/

void w25q64_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//0.时钟初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//1.spi接口初始化
	spi1_init();
	
	//2.片选		
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   //引脚频率比较高的时候，设置为pp模式
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	//2.0 这是总线上lora模块的片选引脚，初始化为输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOE, &GPIO_InitStruct);	
	
	//2.1 不片选，高电平
	GPIO_SetBits(GPIOB, GPIO_Pin_2);
	
	//2.2 不片选lora，高电平有效
	GPIO_SetBits(GPIOE, GPIO_Pin_1);
	
}




uint16_t w25q64_readID(void)
{
	uint16_t id = 0;
	
	//1.片选,低电平有效
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	
	//2.读ID
	spi1_write_read(CMD_W25Q_READ_ID);   //写
	spi1_write_read(0x00);
	spi1_write_read(0x00);
	spi1_write_read(0x00);
	
	id = spi1_write_read(0x00);    //返回值
	id <<= 8;
	id |= spi1_write_read(0x00);

	//1.取消片选,
	GPIO_SetBits(GPIOB, GPIO_Pin_2);
	
	return id;
}

//检查是不是忙
void w25q64_check_busy(void)
{
	uint8_t stat;

	do
	{
		//1.片选,低电平有效
		GPIO_ResetBits(GPIOB, GPIO_Pin_2);
		
		//2.读状态寄存器1
		spi1_write_read(CMD_W25Q_READ_SR1);
		
		stat = spi1_write_read(0);

		//3.取消片选		
		GPIO_SetBits(GPIOB, GPIO_Pin_2);
	}
	while(stat & 1);   //stat的最低位为1，表示芯片正在忙
	
}

static void w25q64_write_enable(void)
{	
	//1.片选,低电平有效
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	
	//2.发送写使能
	spi1_write_read(CMD_W25Q_WRITE_EN);
	
	//3.取消片选		
	GPIO_SetBits(GPIOB, GPIO_Pin_2);

	//4. 等待结束
	w25q64_check_busy();
	
}




void w25q64_erase_chip(void)
{
	//0.写使能
	w25q64_write_enable();
		
	//1.片选,低电平有效
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	
	//2.发送擦除指令（整片擦除）
	spi1_write_read(CMD_W25Q_ERASE_CHIP);
	
	//3.取消片选		
	GPIO_SetBits(GPIOB, GPIO_Pin_2);

	//4. 等待结束
	w25q64_check_busy();	
}

/*
	参数：
		addr 表示开始读取的地址
		buf 指向数据需要存储的空间的首地址
		len 表示需要读取数据的长度

	返回值是读到了多少个数据
*/
int32_t w25q64_read_data(uint32_t addr, uint8_t buf[],uint32_t len)
{
	uint32_t i;
	
	//1.片选,低电平有效
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	
	//2.发送擦除指令（整片擦除）
	spi1_write_read(CMD_W25Q_READ_DATA);
	spi1_write_read((addr>>16) & 0xff);
	spi1_write_read((addr>>8) & 0xff);
	spi1_write_read(addr & 0xff);
	
	for(i=0;i<len;i++)
	{
		buf[i] = spi1_write_read(0);
	}
	
	//3.取消片选		
	GPIO_SetBits(GPIOB, GPIO_Pin_2);
	
	
	return i;
}


/*
	页内写入数据，没有考虑超出页的情况，最多只能写入256个字节
	参数：
		addr 表示开始写入的地址
		buf 指向数据的首地址
		len 表示需要写入数据的长度

	返回值是写了多少个数据
*/
uint8_t w25q64_write_data(uint32_t addr, uint8_t buf[],uint8_t len)
{
	uint8_t i;
		
	//0.写使能
	w25q64_write_enable();
	
	//1.片选,低电平有效
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	
	//2.写入数据
	spi1_write_read(CMD_W25Q_WRITE_DATA);
	spi1_write_read((addr>>16) & 0xff);
	spi1_write_read((addr>>8) & 0xff);
	spi1_write_read(addr & 0xff);
	
	
	for(i=0;i<len;i++)
	{
		spi1_write_read(buf[i]);
	}
		
	//3.取消片选		
	GPIO_SetBits(GPIOB, GPIO_Pin_2);

	//4. 等待结束
	w25q64_check_busy();
	
	return i;
}
