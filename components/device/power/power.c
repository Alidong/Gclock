#include "power.h"
#include "driver/gpio.h"
#include "board.h"
void power_init(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL<<PIN_POWER_ON);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = true;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_set_level(PIN_POWER_ON, 1);
}
void power_toggle(void)
{
    int level=!gpio_get_level(PIN_POWER_ON);
    gpio_set_level(PIN_POWER_ON, level);
}