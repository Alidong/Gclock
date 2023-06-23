#include "aht10.h"
#include "board.h"
#include "../pal.h"
#include "pal_driver.h"
#include "../driver.h"
#include "../bus/dev_bus.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"


#define AHT10_ADDR 0x38
#define AHT10_INIT 0xE1
#define AHT10_MEASURE 0xAC
#define AHT10_RESET  0xBA
#define HUMIDITY(data)  (data/(1024.0*1024.0)*100.0)
#define TEMP(data)      ((data/(1024.0*1024.0))*200.0-50.0)
#define AHT10_PERIOD_MS  pdMS_TO_TICKS(1000*1)
const char* TAG="AHT10";
const uint8_t AHT10_CMD_RESTART[] = {0xBA};
const uint8_t AHT10_CMD_CALIBRATION[] = {0xE1, 0x08, 0x00};
const uint8_t AHT10_CMD_MEASURE[] = {0xAC, 0x33, 0x00};
typedef struct
{
    TimerHandle_t timer;
    aht10_data_t data;
    uint32_t tick;
} aht10_ctrl_t;
aht10_ctrl_t st_aht10Ctrl;
//hardware
static esp_err_t aht10_reset(void)
{
    //reset sensor
    i2c_bus_write_dev(AHT10_ADDR, AHT10_CMD_RESTART, sizeof(AHT10_CMD_RESTART));
    vTaskDelay(pdMS_TO_TICKS(30));
    return ESP_OK;
}
static esp_err_t aht10_init(void)
{
    if (i2c_find_dev(AHT10_ADDR) != ESP_OK)
    {
        ESP_LOGW(TAG,"can not find AHT10!\r\n");
        return ESP_FAIL;
    }
    aht10_reset();
    i2c_bus_write_dev(AHT10_ADDR, AHT10_CMD_CALIBRATION, sizeof(AHT10_CMD_CALIBRATION));
    vTaskDelay(pdMS_TO_TICKS(200));
    ESP_LOGI(TAG,"AHT10 init!\r\n");
    return ESP_OK;
}
static void aht10_get_data(aht10_data_t *data)
{
    memcpy(data, &st_aht10Ctrl.data, sizeof(aht10_data_t));
}
static esp_err_t aht10_trigger_measurement(aht10_data_t *result)
{
    esp_err_t res = ESP_FAIL;
    uint32_t hum = 0, temp = 0;
    uint8_t data[6];
    i2c_bus_read_dev(AHT10_ADDR, NULL, 0, data, 6);
    uint8_t status=data[0];
    if ((status & 0x68) == 0x08)
    {
        hum = (hum | data[1]) << 8;
        hum = (hum | data[2]) << 8;
        hum = (hum | data[3]);
        hum = hum >> 4;
        temp = (temp | (data[3]&0x0f)) << 8;
        temp = (temp | data[4]) << 8;
        temp = (temp | data[5]);
        // temp = temp & 0xFFFFF;
        result->temp = TEMP(temp);
        result->humi = HUMIDITY(hum);
        //printf("#AHT10 Temp=%ld Humi=%ld#\r\n",TEMP(temp),HUMIDITY(hum));
        //trigger a measurement
        i2c_bus_write_dev(AHT10_ADDR, AHT10_CMD_MEASURE, sizeof(AHT10_CMD_MEASURE));
        res = ESP_OK;
    }
    else
    {
        //aht10 re-calibration
        i2c_bus_write_dev(AHT10_ADDR, AHT10_CMD_CALIBRATION, sizeof(AHT10_CMD_CALIBRATION));
    }
    return res;
}

//vfs
static esp_err_t aht10_read(void *dst, size_t size)
{
    esp_err_t res = ESP_FAIL;
    aht10_data_t *data = (void *)dst;
    aht10_get_data(data);
    return res;
}
static esp_err_t aht10_fcntl(uint8_t cmd, uint32_t arg)
{
    switch (cmd)
    {
    case 0:
        //reset sensor
        i2c_bus_write_dev(AHT10_ADDR, AHT10_CMD_RESTART, sizeof(AHT10_CMD_RESTART));
        break;
    case 1:
        //stop read
        xTimerStop(st_aht10Ctrl.timer, 0);
        break;
    case 2:
        //exit stop mode
        xTimerStart(st_aht10Ctrl.timer, 0);
        break;
    default:
        break;
    }
    return ESP_OK;
}
static void aht10_timer(TimerHandle_t xTimer)
{
    aht10_data_t data;
    if (aht10_trigger_measurement(&data)==ESP_OK)
    {
        st_aht10Ctrl.data=data;
    }
    if ((st_aht10Ctrl.tick++)>50)
    {
        i2c_bus_write_dev(AHT10_ADDR, AHT10_CMD_CALIBRATION, sizeof(AHT10_CMD_CALIBRATION));
        st_aht10Ctrl.tick=0;
    }
}
esp_err_t dev_aht10_init(void)
{
    esp_err_t res = ESP_FAIL;
    dev_cb_t devCB =
    {
        .init_dev = aht10_init,
        .attr_ctl_dev = aht10_fcntl,
        .read_dev = aht10_read,
        .write_dev = NULL,
    };
    st_aht10Ctrl.timer = xTimerCreate("aht10_sacn", AHT10_PERIOD_MS, true, NULL, aht10_timer);
    DRV->aht10Handle = dev_register("/dev/aht10", &devCB);
    if (DRV->aht10Handle)
    {
        res = ESP_OK;
        xTimerStart(st_aht10Ctrl.timer, pdMS_TO_TICKS(100));
    }
    return res;
}