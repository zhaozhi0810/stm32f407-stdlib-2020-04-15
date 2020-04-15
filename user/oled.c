

#include "oled.h"
#include <stdio.h>

//#include "oledfont.h"


/*
	0.96 oled 控制  iic的方式
	ssd1306 控制器

*/

static uint8_t OLED_GRAM[8][128];  //显存在单片机的内容


//发送一个字节
//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
uint8_t OLED_WR_Data(u8 dat[],uint16_t len,u8 mode)
{
	uint16_t i;

	//1.发送起始信号
	if(i2c1_start() != 0)
	{
		printf("write i2c1_start()  failed\n");
		return 254;
	}

	//2.发送设备地址
	if(i2c1_put_devaddr(OLED_DEV_ADDR)!= 0)   //最低位是0，表示写操作
	{
		printf("write i2c1_put_devaddr failed\n");
		return 253;
	}

	//4.发送模式，mode可能是命令，也可能是数据
	i2c1_put_byte_data(mode);  
	
	//3.读ack
	if(i2c1_get_ack())  //如果没有应答，直接退出
	{
		printf("mode error \n");
		i2c1_stop();     //发送停止信号，总线就空闲了
		return 2;
	}

	for(i=0;i<len;i++)
	{			
		//4.发送内容
		i2c1_put_byte_data(dat[i]); 

		//3.读ack
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




//更新显存到OLED	
void OLED_Refresh(void)
{
	OLED_WR_Data((void*)OLED_GRAM,sizeof OLED_GRAM,OLED_DATA);

}


//清屏函数
void OLED_Clear(void)
{
	u8 i,n;

	for(i=0;i<8;i++)
	{
		for(n=0;n<128;n++)
		{
			 OLED_GRAM[i][n]=0x3;//清除所有数据
		}
	}
	OLED_Refresh();//更新显示
}





void oled_init(void)
{
	uint8_t cmd_arr[] = {0xAE,0x00,0x10,0x40,0x81,0xCF,0xA1,0xC8,0xA6,0xA8,
						0x3f,0xD3,0x00,0xd5,0x80,0xD9,0xF1,0xDA,0x12,0xDB,
						0x40,0x20,0x00,0x8D,0x14,0xA4,0xA6,0xAF};
	
	//i2c接口的初始化
	i2c1_init();
		
	OLED_WR_Data(cmd_arr,sizeof cmd_arr,OLED_CMD);

	OLED_Clear();
}






//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size:选择字体 12/16/24
//取模方式 逐列式
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1)
{
	u8 i,j,chr1;
	u8 y0=y/8;

	chr1=chr-' ';  //计算偏移后的值

	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			OLED_GRAM[y0+j][x+i] = asc2_1608[chr1][i+j*8];
		}
	}
	
}


//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1);

		x+=size1/2;
		if(x>128-size1/2)  //换行
		{
			x=0;
			y+=8;
		}
		chr++;
	}
	OLED_Refresh();//更新显示
}
