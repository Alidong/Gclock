#include "button.h"
#include "board.h"
#include "../pal_driver.h"
#include "driver/gpio.h"
#include "../device.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#define BUTTON_LONG_PRESS_TICK 30
#define BUTTON_SCAN_PERIOD pdMS_TO_TICKS(20)
typedef struct 
{
  TimerHandle_t timer;
  QueueHandle_t queueEvent;
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
    keyEvent=KEY_EVENT_PRESS;
  }
  else
  {
    keyEvent=KEY_EVENT_NONE;
  }
  return keyEvent;
}
static void button_scan_timer( TimerHandle_t xTimer )   
{
  static uint16_t pressTick[KEY_NUM_MAX];
  QueueHandle_t queueHandle=st_buttonCtrl.queueEvent;
  key_event_t keyEvent=
  {
    .key1Event=0,
    .key2Event=0,
    .keyRemain=0,
  };
  uint8_t event;
  if(gpio_get_level(PIN_BUTTON_1))
  {
    event=button_event_parse(pressTick[KEY_1]);
    if (event)
    {
      keyEvent.key1Event=event;
    }
    pressTick[KEY_1]=0;
  }
  else
  {
    pressTick[KEY_1]++;
  }
  if(gpio_get_level(PIN_BUTTON_2))
  {
    event=button_event_parse(pressTick[KEY_2]);
    if (event)
    {
      keyEvent.key2Event=event;
    }
    pressTick[KEY_2]=0;
  }
  else
  {
    pressTick[KEY_2]++;
  }
  if (keyEvent.key1Event || keyEvent.key2Event)
  {
    xQueueSend(queueHandle,&keyEvent,10);
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
  key_event_t* button=(key_event_t*)buf;
  QueueHandle_t queueHandle=st_buttonCtrl.queueEvent;
  memset(button,0,sizeof(key_event_t));
  xQueueReceive(queueHandle,button,0);
  return ESP_OK;
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
    st_buttonCtrl.queueEvent=xQueueCreate(2,sizeof(key_event_t));
    st_buttonCtrl.timer=xTimerCreate("key_scan",BUTTON_SCAN_PERIOD,true,NULL,button_scan_timer);
    xTimerStart(st_buttonCtrl.timer,0);
    res=ESP_OK;
  }
  return res;
}




