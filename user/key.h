

#include <stm32f4xx.h>



void key_init(void);
uint8_t get_key_val(void);
void key_int_init(void);


extern uint8_t key_flag;   //非0表示检测到按键按下，0表示没有检测到按键

void key_scan(void);



