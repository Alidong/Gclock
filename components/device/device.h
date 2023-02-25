#ifndef _DEVICE_H
#define _DEVICE_H
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_err.h"
#include "esp_types.h"
#include <stdio.h>
#include <fcntl.h>

typedef struct 
{
    int lcdHandle;
    int buttonHandle;
    int aht10Handle;
}device_t;
#define STORAGE_PATH "/spiffs"
void device_init(void);
device_t* dev_get(void);
#define DEV (dev_get())
#endif