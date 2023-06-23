#ifndef _PAL_DRV_H
#define _PAL_DRV_H
#include "esp_err.h"
#include "esp_types.h"
#include <stdio.h>
typedef struct
{
    esp_err_t(*init_dev)(void);
    esp_err_t(*read_dev)(void *buf, size_t size);
    esp_err_t(*write_dev)(const void *buf, size_t size);
    esp_err_t(*attr_ctl_dev)(uint8_t cmd, uint32_t arg);
} dev_cb_t;
typedef int devHandle;
devHandle dev_register(const char *path, dev_cb_t *cb);
#endif