
//#include <stm32f4xx.h>
#include "hardiic.h"
#include "mysystick.h"
#include "oledfont.h"



#define OLED_DEV_ADDR 0x78

#define OLED_CMD  0	    //Ð´ÃüÁî
#define OLED_DATA 0x40	//Ð´Êý¾Ý




#define u8 unsigned char
#define u32 unsigned int




void oled_init(void);

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1);


