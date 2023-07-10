#include "dev_lcd.h"
#include "lcd.h"
#include "../pal_dev.h"
#include "../pal_driver.h"


//hardware
static esp_err_t dev_lcd_init()
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
    case LCD_SET_CB:
        dev_lcd_flush_ready_cb_t* CB=(dev_lcd_flush_ready_cb_t*)arg;
        lcd_register_done_cb(CB->ctx,CB->cb);
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
int pal_lcd_init(void)
{
    dev_cb_t devCB =
    {
        .init_dev = dev_lcd_init,
        .attr_ctl_dev = dev_lcd_attr_ctrl,
        .write_dev = dev_lcd_write,
    };
    devHandle lcdHandle = dev_register("lcd", &devCB);
    return lcdHandle;
}