#include "lv_app_hal.h"
#ifndef USE_LV_SIMULATOR
#include "util/util.h"
#include "drivers/pal_dev.h"
#include "drivers/mic/mic_drv.h"
#include "drivers/wifi/wifi.h"
#include "drivers/bat/battery.h"
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
    return wifi_is_connected();
    #endif
}
bool lv_app_hal_ntp_is_ok()
{
    #ifdef USE_LV_SIMULATOR
    return true;
    #else
    return util_ntp_is_success();
    #endif
}
bool lv_app_hal_battery_is_charging()
{
    #ifdef USE_LV_SIMULATOR
    return false;
    #else
    return bat_is_in_charge();
    #endif
}
uint8_t lv_app_hal_battery_get_percent()
{
    #ifdef USE_LV_SIMULATOR
    return 36;
    #else
    return bat_get_percent();
    #endif
}
void lv_app_hal_aht10_get(float* Temp,float* Humi)
{
    #ifdef USE_LV_SIMULATOR
    *Temp=25.0;
    *Humi=50.0;
    #else
    aht10_data_t data;
    read(DRV->aht10Handle, &data, sizeof(aht10_data_t));
    *Temp=data.temp;
    *Humi=data.humi;
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
// static  float fftResultPink[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// // static const double fftResult[16] = {1.70,1.71,1.73,1.78,1.68,1.56,1.55,1.63,1.79,1.62,1.80,2.06,2.47,3.35,6.83,9.55};
// static float fftMax[128];
// static float fftMin[128];
// static float fftCal(float* fftSamples,uint16_t start,uint16_t end)
// {
//     float res=0;
//     for (size_t i = start; i <= end; i++)
//     {
//         res+=fftSamples[i];
//     }
//     return res/=(end-start+1);
// }
#define FFT_SMAPLES_NUM 64
uint8_t* lv_app_hal_fft(void)
{
    static uint8_t points[FFT_SMAPLES_NUM]={0};
    #ifdef USE_LV_SIMULATOR
    for (size_t i = 0; i < 1024; i++)
    {
       // points[i] = rand() % 255;
    }
    #else
    float fftSamples[FFT_SMAPLES_NUM];
    // float fftMax[FFT_SMAPLES_NUM];
    // float fftMin[FFT_SMAPLES_NUM];
    float max=180.f;
    float min=130.f;
    // for(size_t i=0;i < FFT_SMAPLES_NUM;i++)
    // {
    //     fftMax[i]=max;
    //     fftMin[i]=min;
    // }
    mic_fft(fftSamples,FFT_SMAPLES_NUM);
    uint8_t value=0;
    for (size_t i = 8; i < (FFT_SMAPLES_NUM-5);i++)
    {
        if (fftSamples[i] < min) {
            fftSamples[i]=min;
        }
        if (fftSamples[i] > max) {
            fftSamples[i]=max;
        }
        // if (fftMin[i] < min) {
        //     fftMin[i] = min;
        // }
        // if (fftMax[i] > max) {
        //     fftMax[i] = max;
        // }
        value=(uint8_t)(fftSamples[i]-min);
        points[i]=value*5;
    }
    #endif
    return points;
}