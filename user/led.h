
#include <stm32f4xx.h>

enum LED_NUM {LEDALL=0,LED1,LED2,LED3};
typedef enum LED_NUM ledn_t;
enum LED_STATUTS   {LEDOFF=0,LEDON};
typedef enum LED_STATUTS led_sta_t;



void led_init(void);
void led_set_status(ledn_t ledn,led_sta_t status);

void led_show_num(uint8_t num);






