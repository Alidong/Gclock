#include "device.h"
#include "bus/dev_bus.h"
#include "power/power.h"
#include "lcd/lcd.h"
#include "button/button.h"
#include "net/net.h"
#include "storage/dev_storage.h"
#include "aht10/aht10.h"
static const char *TAG = "device init";
static device_t dev;
void device_init(void)
{
    power_init();
    i2c_bus_init();
    i2c_scan_dev();
    dev_aht10_init();
    // dev_storage_init();
    // wifi_init();
    dev_button_init();
    lcd_init();
}
device_t* dev_get(void)
{
    return &dev;
}