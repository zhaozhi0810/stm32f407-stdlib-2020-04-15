
#include <stm32f4xx.h>
#include <stdio.h>
//typedef void FILE ;


void uart1_init(uint32_t bandrate);
void uart1_int_init(uint32_t bandrate);


void uart1_putchar(uint8_t dat);
void uart1_putstr(uint8_t dat[]);
uint8_t	uart1_getchar(void);

int fputc(int ch, FILE *f);


extern uint8_t recv_buf[8];   //用来存储串口的数据
extern uint8_t recv_flag; //接收标志，为1时表示接收到了数据，为0的时候表示没有接收到数据




