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
} device_t;
void drv_init(void);
device_t *drv_get_handle(void);
#define DRV (drv_get_handle()) 
#endif