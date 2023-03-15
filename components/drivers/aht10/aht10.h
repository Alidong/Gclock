#ifndef _AHT10_H
#define _AHT10_H
#include "esp_err.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
    int32_t humi;
    int32_t temp;
} aht10_data_t;
esp_err_t dev_aht10_init(void);
#ifdef __cplusplus
}
#endif
#endif
