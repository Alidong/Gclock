#ifndef _WIFI_H
#define _WIFI_H
#include "esp_types.h"
#include "esp_err.h"
void wifi_init(void);
bool wifi_is_connected();
#endif