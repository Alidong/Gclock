#include "dev_lcd.h"
#include "../pal.h"
#include "../driver.h"
#include "../pal_driver.h"
#include "lcd.h"

//hardware
static esp_err_t pal_lcd_init()
{
    lcd_init();
    return ESP_OK;
}
static esp_err_t dev_lcd_attr_ctrl(uint8_t cmd, uint32_t arg)
{
    switch (cmd)
    {
    case LCD_SET_LIGHT:
        lcd_set_light((uint8_t)arg);
        break;
    case LCD_GET_SIZE:
        dev_lcd_pix_t *lcd = (dev_lcd_pix_t *)arg;
        lcd->height = LCD_V_RES;
        lcd->width = LCD_H_RES;
        break;    
    default:
        break;
    }
    return ESP_OK;
}
static esp_err_t dev_lcd_write(const void *buf, size_t size)
{
    dev_lcd_flush_t *lcd = (dev_lcd_flush_t *)buf;
    lcd_flush(lcd->xStart, lcd->yStart, lcd->xEnd, lcd->yEnd, lcd->buf);
    return ESP_OK;
}
esp_err_t dev_lcd_init(void)
{
    esp_err_t res = ESP_FAIL;
    dev_cb_t devCB =
    {
        .init_dev = pal_lcd_init,
        .attr_ctl_dev = dev_lcd_attr_ctrl,
        .write_dev = dev_lcd_write,
    };
    DRV->lcdHandle = dev_register("/dev/lcd", &devCB);
    if (DRV->lcdHandle)
    {
        res = ESP_OK;
    }
    return res;
}