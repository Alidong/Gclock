#ifndef _BATTERY_H
#define _BATTERY_H
#include "stdint.h"
#include "esp_types.h"
void bat_init(void);
void bat_pin_toggle(void);
uint8_t bat_get_percent();
bool bat_is_in_charge();
#endif