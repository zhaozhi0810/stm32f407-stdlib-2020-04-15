

#include "ADC1_0_gm.h"


/*
	光敏电阻电压的ADC转换

	ADC1 通道0
	
	引脚： GPA 0号引脚
*/


void adc1_gm_init(void)
{
	ADC_InitTypeDef ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//1.时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	
	//2.IO端口的配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;     //设置为模拟功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;   //开漏输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  //上下拉电阻不能开启
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  //	
	GPIO_Init(GPIOA, &GPIO_InitStruct);  //这个函数，让结构体的内容配置到寄存器中
		
	//3.ADC控制的配置
	//ADC公共的配置
	ADC_CommonStructInit(&ADC_CommonInitStruct);
	ADC_CommonInit(&ADC_CommonInitStruct);

	//ADC1的相关初始化	
	ADC_StructInit(&ADC_InitStruct);   //基本保持默认配置
	ADC_Init(ADC1, &ADC_InitStruct);

	//规则通道设置,第一次采集通道0
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1, ADC_SampleTime_480Cycles);
	
	//启动ADC
	ADC_Cmd(ADC1, ENABLE); 
	
}



//获得ADC的数据
uint16_t get_gm_val(void)
{
	//1.人为软件启动ADC转换
	ADC_SoftwareStartConv(ADC1);
	
	//2.等待转换结束 EOC
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	//3.把转换的结果读回来
	return ADC_GetConversionValue(ADC1);
	
}



