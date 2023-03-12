#include "button.h"
#include "board.h"
#include "../pal_driver.h"
#include "driver/gpio.h"
#include "../device.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
#include "esp_log.h"
#define BUTTON_LONG_PRESS_TICK pdMS_TO_TICKS(800)
#define BUTTON_SCAN_PERIOD pdMS_TO_TICKS(50)
static const char* TAG= "Button";
enum
{
    KEY_EVENT_NONE,
    KEY_EVENT_CLICK,
    KEY_EVENT_LONG_PRESS,
};
typedef struct 
{
  TimerHandle_t timer;
  uint8_t keyIOIndex[KEY_NUM_MAX];
  EventGroupHandle_t eventHandle;
  bool needClear;
}button_ctrl_t;
button_ctrl_t st_buttonCtrl;
static uint8_t button_event_parse(uint16_t pressTick)
{
  uint8_t keyEvent;
  if (pressTick>=BUTTON_LONG_PRESS_TICK)
  {
    keyEvent=KEY_EVENT_LONG_PRESS;
  }
  else if( pressTick!=0 && pressTick<BUTTON_LONG_PRESS_TICK)
  {
    keyEvent=KEY_EVENT_CLICK;
  }
  else
  {
    keyEvent=KEY_EVENT_NONE;
  }
  return keyEvent;
}
static void button_scan_timer( TimerHandle_t xTimer )   
{
  static uint32_t tick;
  static uint32_t pressTick[KEY_NUM_MAX];
  if (st_buttonCtrl.needClear)
  {
    tick+=BUTTON_SCAN_PERIOD;
    if (tick>=BUTTON_LONG_PRESS_TICK)
    {
      tick=0;
      st_buttonCtrl.needClear=false;
      xEventGroupClearBits(st_buttonCtrl.eventHandle,KEY_MASK_LONG_PRESS(KEY_1)|KEY_MASK_LONG_PRESS(KEY_2));
    }
  }
  for (size_t i = 0; i < KEY_NUM_MAX; i++)
  {
    if(gpio_get_level(st_buttonCtrl.keyIOIndex[i]))
    {
      switch (button_event_parse(pressTick[i]))
      {
      case KEY_EVENT_CLICK:
        xEventGroupSetBits(st_buttonCtrl.eventHandle,KEY_MASK_CLICK(i));
        break;
      case KEY_EVENT_LONG_PRESS:
        xEventGroupSetBits(st_buttonCtrl.eventHandle,KEY_MASK_LONG_PRESS(i));
        st_buttonCtrl.needClear=true;
        break;
      default:
        break;
      }
      pressTick[i]=0;
    }
    else
    {
      pressTick[i]+=BUTTON_SCAN_PERIOD;
    }
  }
}
//hardware
static esp_err_t button_init()
{
  //zero-initialize the config structure.
  gpio_config_t io_conf = {};
  //disable interrupt
  io_conf.intr_type = GPIO_INTR_DISABLE;
  //set as output mode
  io_conf.mode = GPIO_MODE_INPUT;
  //bit mask of the pins that you want to set,e.g.GPIO18/19
  io_conf.pin_bit_mask = (1ULL<<PIN_BUTTON_1)|(1ULL<<PIN_BUTTON_2);
  //disable pull-down mode
  io_conf.pull_down_en = 0;
  //disable pull-up mode
  io_conf.pull_up_en = true;
  //configure GPIO with the given settings
  gpio_config(&io_conf);
  return ESP_OK;
}
static esp_err_t button_read(void*buf,size_t size)
{
  uint32_t* button=(uint32_t*)buf;
  *button=(uint32_t)xEventGroupWaitBits(st_buttonCtrl.eventHandle,KEY_MASK_CLICK(KEY_1)|KEY_MASK_CLICK(KEY_2),true,false,0);
  return ESP_OK;
}
uint32_t button_wait_event(uint32_t keyEvnet,uint32_t tick)
{
  return (uint32_t)xEventGroupWaitBits(st_buttonCtrl.eventHandle,keyEvnet,true,true,tick);
}
esp_err_t dev_button_init(void) 
{
  esp_err_t res=ESP_FAIL;
  dev_cb_t devCB = {
      .init_dev=button_init,
      .attr_ctl_dev=NULL,
      .read_dev=button_read,
      .write_dev=NULL,
  };
  DEV->buttonHandle= dev_register("/dev/button",&devCB);
  if (DEV->buttonHandle)
  {
    st_buttonCtrl.keyIOIndex[KEY_1]=PIN_BUTTON_1;
    st_buttonCtrl.keyIOIndex[KEY_2]=PIN_BUTTON_2;
    st_buttonCtrl.eventHandle=xEventGroupCreate();
    st_buttonCtrl.timer=xTimerCreate("key_scan",BUTTON_SCAN_PERIOD,true,NULL,button_scan_timer);
    xTimerStart(st_buttonCtrl.timer,0);
    res=ESP_OK;
  }
  return res;
}




