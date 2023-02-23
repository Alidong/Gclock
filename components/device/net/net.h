#ifndef _NET_H
#define _NET_H
#include "esp_types.h"
#include "esp_err.h"
void wifi_init(void);
esp_err_t wifi_smart_cfg_start(void);
esp_err_t wifi_smart_cfg_abort(void);
bool wifi_is_connect();
#endif