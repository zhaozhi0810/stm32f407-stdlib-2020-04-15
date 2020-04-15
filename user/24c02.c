


#include "24c02.h"
#include <stdio.h>

/*
	主要是读写24c02
	
	

*/

void AT24c02_init(void)
{
	i2c1_init();
}




/*
	从当前地址读一个字节
	不提供对外的接口
*/	
static uint8_t AT24c02_read_byte_cur(uint8_t *dat,uint8_t len)
{
	uint8_t i;
	
	//1.发送起始信号
	if(i2c1_start() != 0)
	{
		printf("i2c1_start() failed\n");
		return 254;
	}
	
	//2.发送设备地址
	if(i2c1_put_devaddr(SLAVER_ADDR | 1)!= 0)   //最低位是1，表示读操作
	{
		printf("i2c1_put_devaddr failed\n");
		return 253;
	}

		
	for(i=0;i<len;i++)
	{			
		//4.只有最后一个数据发送非应答
		if(i == len -1)
		{
			i2c1_send_ack(NO_ACK);
		}
		else
		{
			i2c1_send_ack(ACK);
		}
		
		//3.获得一个字节的数据
		dat[i] = i2c1_get_byte_data();
	}
	
	//.发送stop时序
	i2c1_stop();     //发送停止信号，总线就空闲了

	return 0;
}





/*
	24c02 单片机写多个数据
	参数：
		word_addr 空间地址，就是要把字节dat写到24c02的哪一个存储单元中去
		dat       实际数据的首地址，
		len       实际需要写入数据的个数
	返回值：
		0  ：     成功
		非0：     失败
*/
uint8_t AT24c02_write_byte(uint8_t word_addr,uint8_t *dat,uint8_t len)
{
	uint8_t i;


	//1.发送起始信号
	if(i2c1_start() != 0)
	{
		printf("write i2c1_start()  failed\n");
		return 254;
	}
	
	//2.发送设备地址
	if(i2c1_put_devaddr(SLAVER_ADDR )!= 0)   //最低位是0，表示写操作
	{
		printf("write i2c1_put_devaddr failed\n");
		return 253;
	}
	
	//4.发送空间地址
	i2c1_put_byte_data(word_addr);  
	
	//3.读24c02的ack
	if(i2c1_get_ack())  //如果没有应答，直接退出
	{
		i2c1_stop();     //发送停止信号，总线就空闲了
		return 2;
	}
	
	//len等于0的时候，我认为是随机读的一个有效操作
	if(len == 0)
	{
		return 5;
	}
	
	for(i=0;i<len;i++)
	{		
		//4.发送内容
		i2c1_put_byte_data(dat[i]); 
		
		//3.读24c02的ack
		if(i2c1_get_ack())  //如果没有应答，直接退出
		{
			i2c1_stop();     //发送停止信号，总线就空闲了
			return 3;
		}
	}
	
	//5.结束，结束总线的占用
	i2c1_stop();
	
	delay_ms(100);
	
	return 0;
}



/*
	随机读数据
	参数 ： word_addr 指定我要读取的位置
			dat 表示存储数据缓存的首地址
			len 表示需要读取数据的个数
	返回值： 0 表示成功
*/
uint8_t AT24c02_read_byte(uint8_t word_addr,uint8_t *dat,uint8_t len)
{
	AT24c02_write_byte(word_addr,(void*)0,0);
		
	AT24c02_read_byte_cur(dat,len);
	
	return 0;
}





/*
	24c02 是页编程

	起始地址如果是16的整数倍，那么最多可以写入有效的16个字节，多余的字节，就会环回
	
*/
#define PAGE_SIZE 16    //页字节数
#define PAGE_SHIFT 4    //



uint8_t AT24c02_write_data(uint8_t word_addr,uint8_t *dat,uint8_t len)
{
	uint8_t n;  //写入的次数
	uint8_t m;  //第一次写入的字节数
	
	if((word_addr & (PAGE_SIZE-1)) != 0)   //表示word_addr不能够被16整除
	{
		m = PAGE_SIZE - (word_addr& (PAGE_SIZE-1));   //第一次写入的字节数
		n = (len-m+PAGE_SIZE-1) >> PAGE_SHIFT;
	//	n += 1;
		AT24c02_write_byte(word_addr,dat,m);
		word_addr += m;
		dat += m;
		len -= m;
		
	}
	else //表示word_addr能够被16整除
	{
		n = (len+PAGE_SIZE-1)>>PAGE_SHIFT;
	}
	
	while(n>1)
	{
//		printf("word_addr = %d len = %d\n",word_addr,len);
		AT24c02_write_byte(word_addr,dat,PAGE_SIZE);
		word_addr += PAGE_SIZE;
		dat += PAGE_SIZE;
		len -= PAGE_SIZE;
		n--;
	}
	
	AT24c02_write_byte(word_addr,dat,len);
	
	return 0;
}


