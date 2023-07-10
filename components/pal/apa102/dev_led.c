#include "dev_led.h"
#include "apa102.h"
#include "board.h"
#include "../pal_dev.h"
#include "../pal_driver.h"
#include "fcntl.h"
typedef struct _led_ctrl_
{
    apa102_t pin;
    uint8_t num;
    uint8_t idx;
    uint32_t color;
}led_ctrl_t;
static led_ctrl_t led;
//hardware
static esp_err_t dev_led_init()
{
    led.num=7;
    led.pin.clockPin=PIN_LED_CLK;
    led.pin.dataPin=PIN_LED_DO;
    led.color=0x01891EF4;
    apa102_init(&(led.pin));
    return ESP_OK;
}
static esp_err_t dev_led_attr_ctrl(uint8_t cmd, uint32_t arg)
{
    switch (cmd)
    {
    case LED_SET_COLOR:
        led.color = arg;
        break;
    case LED_SCROLL_RIGHT:
        {
            if(led.idx==0)
            {
                led.idx=led.num-1;
            }
            else
            {
                led.idx--;
            }
            uint32_t* pColor = (uint32_t*)malloc(sizeof(uint32_t)*led.num);
            for (uint8_t i = 0; i < led.num; i++)
            {
                if (i==led.idx)
                {
                    pColor[i]=led.color;
                }
                else
                {
                    pColor[i]=0;
                }
            }
            apa102_send_colorfram(&led.pin,led.num,pColor);
            free(pColor);
        }
        break;
    case LED_SCROLL_LEFT:
        {
            if(led.idx==(led.num-1))
            {
                led.idx=0;
            }
            else
            {
                led.idx++;
            }
            uint32_t* pColor = (uint32_t*)malloc(sizeof(uint32_t)*led.num);
            for (uint8_t i = 0; i < led.num; i++)
            {
                if (i==led.idx)
                {
                    pColor[i]=led.color;
                }
                else
                {
                    pColor[i]=0;
                }
            }
            apa102_send_colorfram(&led.pin,led.num,pColor);
            free(pColor);
            break; 
        }       
    default:
        break;
    }
    return ESP_OK;
}

int pal_led_init(void)
{
    dev_cb_t devCB =
    {
        .init_dev = dev_led_init,
        .attr_ctl_dev = dev_led_attr_ctrl,
    };
    devHandle led = dev_register("led", &devCB);
    return led;
}