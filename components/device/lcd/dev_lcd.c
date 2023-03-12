#include "dev_lcd.h"
#include "lcd.h"
#include "../device.h"
#include "../pal_driver.h"

//hardware
static esp_err_t pal_lcd_init()
{
    lcd_init();
    return ESP_OK;
}
static esp_err_t dev_lcd_attr_ctrl(uint8_t cmd,uint32_t arg)
{
    switch (cmd)
    {
    case LCD_SET_LIGHT:
        lcd_set_light(arg);
        break; 
    default:
        break;
    }
    return ESP_OK;
}
static esp_err_t dev_lcd_write(const void*buf,size_t size)
{
    dev_lcd_flush_t* lcd=(dev_lcd_flush_t*)buf;
    lcd_flush(lcd->xStart,lcd->yStart,lcd->xEnd,lcd->yEnd,lcd->buf);
    return ESP_OK;
}
static esp_err_t dev_lcd_read(void*buf,size_t size)
{
    dev_lcd_pix_t* lcd=(dev_lcd_pix_t*)buf;
    lcd->height=LCD_V_RES;
    lcd->width=LCD_H_RES;
    return ESP_OK;
}
esp_err_t dev_lcd_init(void) 
{
  esp_err_t res=ESP_FAIL;
  dev_cb_t devCB = {
      .init_dev=pal_lcd_init,
      .attr_ctl_dev=dev_lcd_attr_ctrl,
      .read_dev=dev_lcd_read,
      .write_dev=dev_lcd_write,
  };
  DEV->lcdHandle= dev_register("/dev/lcd",&devCB);
  if (DEV->lcdHandle)
  {
      res=ESP_OK;
  }
  return res;
}