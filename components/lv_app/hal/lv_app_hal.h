#ifndef LV_APP_HAL_H
#define LV_APP_HAL_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>
typedef struct 
{
    uint16_t year;
    uint8_t mon;
    uint8_t monDay;
    uint8_t weekDay;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
}lv_app_hal_local_time_t;

bool lv_app_hal_wifi_is_ok();
bool lv_app_hal_ntp_is_ok();
bool lv_app_hal_battery_is_charging();
uint8_t lv_app_hal_battery_get_percent();
void lv_app_hal_aht10_get(float* Temp,float* Humi);
void lv_app_hal_get_local_time(lv_app_hal_local_time_t* ptime);
uint8_t* lv_app_hal_fft(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
