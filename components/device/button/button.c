#include "board.h"
#include "freertos/FreeRTOS.h" 
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include "driver/gpio.h"
#include "../device.h"
#include "power/power.h"
#include "esp_err.h"
#include "esp_log.h"
static SemaphoreHandle_t mutex;
//hardware
static void button_init()
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
}

//vfs
static int button_open(const char * path, int flags, int mode)
{
  button_init();
  return ESP_OK;
}
static ssize_t button_read(int fd, void * dst, size_t size)
{
  xSemaphoreTake(mutex,portMAX_DELAY);
  uint8_t* button=(uint8_t*)dst;
  *button=0;
  if(!gpio_get_level(PIN_BUTTON_1))
  {
    (*button)|=0x01;
  }
  if(!gpio_get_level(PIN_BUTTON_2))
  {
    (*button)|=0x02;
  }
  xSemaphoreGive(mutex);
  return size;
}
esp_err_t dev_button_init(void) 
{
  esp_err_t res=ESP_FAIL;
  esp_vfs_t touch_vfs = {
    .flags = ESP_VFS_FLAG_DEFAULT,
    .open = &button_open,
    .read = &button_read,
  };
  mutex=xSemaphoreCreateMutex();
  ESP_ERROR_CHECK(esp_vfs_register("/dev/button", &touch_vfs, NULL));
  DEV->buttonHandle= open("/dev/button",0,0);
  if (DEV->buttonHandle)
  {
    res=ESP_OK;
  }
  return res;
}




