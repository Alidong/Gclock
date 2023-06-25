#ifndef _PAL_H
#define _PAL_H
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_err.h"
#include "esp_types.h"
#include <stdio.h>
#include <fcntl.h>
#include "pal_driver.h"
#include "pal_dev.h"

typedef struct
{
    devHandle lcdHandle;
    devHandle knobHandle;
    devHandle aht10Handle;
    devHandle wifi;
    devHandle ble;
    devHandle mic;
    devHandle speaker;
    devHandle rgb;
    devHandle power;
} device_t;
void pal_init(void);
device_t *dev_get_handle(void);
#define DRV (dev_get_handle()) 
#endif