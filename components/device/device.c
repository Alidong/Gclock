#include "device.h"
#include "bus/dev_bus.h"
#include "bat/battery.h"
#include "lcd/dev_lcd.h"
#include "button/button.h"
#include "storage/dev_storage.h"
#include "aht10/aht10.h"
#include "wifi/wifi.h"
static const char *TAG = "device init";
static device_t dev;
void device_init(void)
{
    bat_init();
    i2c_bus_init();
    i2c_scan_dev();
    dev_storage_init();
    dev_button_init();
    dev_lcd_init();
    dev_aht10_init();
    wifi_init();
}
device_t* dev_get(void)
{
    return &dev;
}