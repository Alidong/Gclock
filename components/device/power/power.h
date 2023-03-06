#ifndef _POWER_H
#define _POWER_H
#include "stdint.h"
#include "esp_types.h"
void power_init(void);
void power_pin_toggle(void);
uint8_t power_get_bat_percent();
bool power_is_in_charge();
#endif