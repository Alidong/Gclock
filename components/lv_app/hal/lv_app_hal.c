#include "lv_app_hal.h"
#ifndef USE_LV_SIMULATOR
#include "util.h"
#include "pal.h"
#include <time.h>
#include <sys/time.h>
#else
#include <stdlib.h>
#endif

bool lv_app_hal_wifi_is_ok()
{
    #ifdef USE_LV_SIMULATOR
    return true;
    #else
    return true;
    #endif
}
bool lv_app_hal_ntp_is_ok()
{
    #ifdef USE_LV_SIMULATOR
    return true;
    #else
    return true;
    #endif
}
bool lv_app_hal_battery_is_charging()
{
    #ifdef USE_LV_SIMULATOR
    return false;
    #else
    return false;
    #endif
}
uint8_t lv_app_hal_battery_get_percent()
{
    #ifdef USE_LV_SIMULATOR
    return 36;
    #else
    return 36;
    #endif
}
void lv_app_hal_aht10_get(float* Temp,float* Humi)
{
    #ifdef USE_LV_SIMULATOR
    *Temp=25.0;
    *Humi=50.0;
    #else
    *Temp=25.0;
    *Humi=50.0;
    #endif
}
void lv_app_hal_get_local_time(lv_app_hal_local_time_t* ptime)
{
    #ifdef USE_LV_SIMULATOR
    static uint8_t sec=0;
    if (sec>=60)
    {
        sec=0;
    }
    ptime->mon=2;
    ptime->monDay=5;
    ptime->weekDay=6;
    ptime->hour=16;
    ptime->min=8;
    ptime->sec = sec;
    sec++;
    #else
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    ptime->mon=timeinfo.tm_mon+1;
    ptime->monDay=timeinfo.tm_mday;
    ptime->weekDay=timeinfo.tm_wday;
    ptime->hour=timeinfo.tm_hour;
    ptime->min=timeinfo.tm_min;
    ptime->sec=timeinfo.tm_sec;
    #endif
}