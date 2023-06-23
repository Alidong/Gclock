#include "pal.h"
#include "bus/dev_bus.h"
#include "bat/battery.h"
#include "lcd/dev_lcd.h"
#include "knob/dev_knob.h"
#include "storage/dev_storage.h"
#include "wifi/wifi.h"
static const char *TAG = "device init";
static device_t dev;
void drv_init(void)
{
    bat_init();
    i2c_bus_init();
    i2c_scan_dev();
    dev_storage_init();
    //dev_button_init();
    dev.lcdHandle= pal_lcd_init();
    dev.knobHandle= pal_knob_init();
    //dev_aht10_init();
    // wifi_init();
    //mic_drv_init();
}
device_t *drv_get_handle(void)
{
    return &dev;
}