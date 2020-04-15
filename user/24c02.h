

#include  <stm32f4xx.h>
#include "hardiic.h"



#define SLAVER_ADDR 0xa0 //高7位表示有效设备地址，最低位表示读写
#define NO_ACK 0         //表示发送不应答信号
#define ACK 1            //表示发送应答信号


void AT24c02_init(void);


//连续多页的写操作
uint8_t AT24c02_write_data(uint8_t word_addr,uint8_t *dat,uint8_t len);

//页内写操作
uint8_t AT24c02_write_byte(uint8_t word_addr,uint8_t *dat,uint8_t len);


uint8_t AT24c02_read_byte(uint8_t word_addr,uint8_t *dat,uint8_t len);

