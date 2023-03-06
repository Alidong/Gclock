#include "power.h"
#include "../pal_driver.h"
#include "board.h"
#include "driver/gpio.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#define GET_UNIT(x)         ((x>>3) & 0x1)
#define ADC_READ_LEN        64
#define ADC_CONV_MODE       ADC_CONV_SINGLE_UNIT_1
#define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define ADC_CHANNEL         ADC_CHANNEL_3
#define ADC_DIGI_MAX        4095
#define BAT_MAX_MV          4200 //4200mV=4.2V
#define BAT_MIN_MV          3200 //3200mV=3.2V
#define BAT_LEVEL_FILTER    10   //10 times
#define ADC_SCAN_PERIOD     pdMS_TO_TICKS(100)
#define volt2percent(volt)        (uint16_t)(((volt)-BAT_MIN_MV)*100/(BAT_MAX_MV-BAT_MIN_MV))
static const char* TAG="power:";
#pragma pack(4)
typedef struct 
{
    adc_continuous_handle_t adcHandle;
    TimerHandle_t timer;
    uint8_t adcCache[ADC_READ_LEN];
    uint16_t batVolt;       //mV
    uint8_t powerLevel;     //0-100%
    uint16_t loopTimes;
    bool updateADC;
}power_ctrl_t;
#pragma pack()
static power_ctrl_t st_powerCtrl;
static void bat_adc_scan(TimerHandle_t xtimer) ;
static bool adc_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    st_powerCtrl.updateADC=true;
    return true;
}
static void power_bat_adc_init(void)
{
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = ADC_READ_LEN*4+128,
        .conv_frame_size = ADC_READ_LEN,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &st_powerCtrl.adcHandle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = SOC_ADC_SAMPLE_FREQ_THRES_LOW,
        .conv_mode = ADC_CONV_MODE,
        .format = ADC_OUTPUT_TYPE,
    };
    adc_digi_pattern_config_t adc_pattern = {0};
    dig_cfg.pattern_num = 1;
    uint8_t unit = ADC_UNIT_1;
    uint8_t ch = ADC_CHANNEL;
    adc_pattern.atten = ADC_ATTEN_DB_11;
    adc_pattern.channel = ch;
    adc_pattern.unit = unit;
    adc_pattern.bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
    ESP_LOGI(TAG, "adc_pattern.atten is :%x", adc_pattern.atten);
    ESP_LOGI(TAG, "adc_pattern.channel is :%x", adc_pattern.channel);
    ESP_LOGI(TAG, "adc_pattern.unit is :%x", adc_pattern.unit);
    dig_cfg.adc_pattern = &adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(st_powerCtrl.adcHandle, &dig_cfg));
    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = adc_conv_done_cb,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(st_powerCtrl.adcHandle, &cbs, NULL));
    ESP_ERROR_CHECK(adc_continuous_start(st_powerCtrl.adcHandle));
    st_powerCtrl.updateADC=true;
    st_powerCtrl.timer= xTimerCreate("BAT_Volt",ADC_SCAN_PERIOD,true,NULL,bat_adc_scan);
    xTimerStart(st_powerCtrl.timer,100);
}
static void power_pin_init(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL<<PIN_POWER_ON);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = true;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_set_level(PIN_POWER_ON, 1);
}
void power_pin_toggle(void)
{
    int level=!gpio_get_level(PIN_POWER_ON);
    gpio_set_level(PIN_POWER_ON, level);
}
static void bat_adc_scan(TimerHandle_t xtimer)
{
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint32_t volt=0;
    static bool firstLoop=true;
    static uint16_t lastNewLevel=0;
    if (st_powerCtrl.updateADC)
    {
        ret = adc_continuous_read(st_powerCtrl.adcHandle, (uint8_t*)st_powerCtrl.adcCache, ADC_READ_LEN, &ret_num, 0);
        st_powerCtrl.updateADC=false;
        if (ret == ESP_OK) 
        {
            //ESP_LOGD(TAG, "ret is %x, ret_num is %"PRIu32, ret, ret_num);
            for (int i = 0; i < ret_num; i+=SOC_ADC_DIGI_RESULT_BYTES) {
                adc_digi_output_data_t *p = (void*)&st_powerCtrl.adcCache[i];
                //ESP_LOGD(TAG, "Unit: %d,_Channel: %d, Value: %x", p->type2.unit+1, p->type2.channel, p->type2.data);
                volt+=p->type2.data;
            }
            volt=volt/(ret_num/SOC_ADC_DIGI_RESULT_BYTES);
            st_powerCtrl.batVolt=(volt*3300*2/ADC_DIGI_MAX);
            if (firstLoop)
            {
                st_powerCtrl.powerLevel=st_powerCtrl.batVolt*100/BAT_MAX_MV;
                firstLoop=false;
            }
            else
            {
                uint16_t newLevel=volt2percent(st_powerCtrl.batVolt);
                if (newLevel!=lastNewLevel)
                {
                    st_powerCtrl.loopTimes=0;
                }
                else
                {
                    st_powerCtrl.loopTimes++; 
                    if (st_powerCtrl.loopTimes>=BAT_LEVEL_FILTER)
                    {
                        st_powerCtrl.powerLevel=newLevel;
                    }
                }
                lastNewLevel=newLevel;
            }
            ESP_LOGD(TAG, "%dmV", st_powerCtrl.batVolt);
        } else
        {
            ESP_LOGW(TAG, "adc wait...");
        }
    }
}
void power_init(void)
{
    power_pin_init();
    power_bat_adc_init();
}
uint8_t power_get_bat_percent()
{
    return st_powerCtrl.powerLevel>=100? 100:st_powerCtrl.powerLevel;
}
bool power_is_in_charge()
{
    return st_powerCtrl.powerLevel>=100? true:false;
}