
#include  <stm32f4xx.h>

#define NO_ACK 0 /*/表示发送不应答信号*/
#define ACK 1 /*表示发送应答信号 */



void i2c1_init(void);

void i2c1_start(void);

void i2c1_stop(void);

void i2c1_put_byte_data(uint8_t dat);

uint8_t i2c1_get_ack(void);


uint8_t i2c1_get_byte_data(void);

void i2c1_send_ack(uint8_t ack);

