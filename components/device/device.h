#ifndef _DEVICE_H
#define _DEVICE_H
#include "esp_err.h"
#include "esp_types.h"
#include <stdio.h>

// typedef struct 
// {
//     esp_err_t(*init_dev)(void);
//     esp_err_t(*read_dev)(void*buf,size_t size);
//     esp_err_t(*write_dev)(void*buf,size_t size);
//     esp_err_t(*ctrl_dev)(uint8_t cmd,uint32_t arg);
// }device_obj_t;

typedef struct 
{
    int32_t humi;
    int32_t temp;
}aht10_data_t;

typedef struct 
{
    int lcdHandle;
    int buttonHandle;
    int aht10Handle;
}device_t;

void device_init(void);
device_t* dev_get(void);
#define DEV (dev_get())
#endif