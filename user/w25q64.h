

#include <stm32f4xx.h>
#include "spi1.h"


#define CMD_W25Q_READ_ID 0x90
#define CMD_W25Q_ERASE_CHIP 0x60
#define CMD_W25Q_READ_SR1 0x05       //¶Á×´Ì¬¼Ä´æÆ÷1
#define CMD_W25Q_READ_DATA 0x03 
#define CMD_W25Q_WRITE_DATA 0x02     //Ò³±à³Ì 
#define CMD_W25Q_WRITE_EN 0x06 





void w25q64_init(void);
uint16_t w25q64_readID(void);
void w25q64_erase_chip(void);
int32_t w25q64_read_data(uint32_t addr, uint8_t buf[],uint32_t len);
uint8_t w25q64_write_data(uint32_t addr, uint8_t buf[],uint8_t len);


