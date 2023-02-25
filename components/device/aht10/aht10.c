#include "aht10.h"
#include "board.h"
#include "../device.h"
#include "pal_driver.h"
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
#define HUMIDITY(data)  (uint32_t)(data*1000/1024/1024)
#define TEMP(data)      (uint32_t)(data*2000/1024/1024-50)
#define AHT10_PERIOD_MS  pdMS_TO_TICKS(1000)
const uint8_t AHT10_CMD_RESTART[]={0xBA}; 
const uint8_t AHT10_CMD_INITT[]={0xAC,0x08,0x00}; 
const uint8_t AHT10_CMD_MEASURE[]={0xAC,0x33,0x00};  
typedef struct 
{
    SemaphoreHandle_t lock;
    TimerHandle_t timer;
    aht10_data_t data;
    bool newData;
}aht10_ctrl_t;
aht10_ctrl_t st_aht10Ctrl;
//hardware
static esp_err_t aht10_init(void)
{
    if(i2c_find_dev(AHT10_ADDR)!=ESP_OK)
    {
        printf("dev: can not find AHT10!\r\n");
        return ESP_FAIL;
    }
    //reset sensor
    i2c_bus_write_dev(AHT10_ADDR,AHT10_CMD_RESTART,sizeof(AHT10_CMD_RESTART));
    vTaskDelay(pdMS_TO_TICKS(30));
    i2c_bus_write_dev(AHT10_ADDR,AHT10_CMD_INITT,sizeof(AHT10_CMD_INITT));
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("dev:AHT10 init!\r\n");
    return ESP_OK;
}
static void aht10_set_data(aht10_data_t* data)
{
    xSemaphoreTake(st_aht10Ctrl.lock,portMAX_DELAY);
    memcpy(&st_aht10Ctrl.data,data,sizeof(aht10_data_t));
    xSemaphoreGive(st_aht10Ctrl.lock);
}
static void aht10_get_data(aht10_data_t* data)
{
    xSemaphoreTake(st_aht10Ctrl.lock,portMAX_DELAY);
    memcpy(data,&st_aht10Ctrl.data,sizeof(aht10_data_t));
    xSemaphoreGive(st_aht10Ctrl.lock);
}
static esp_err_t aht10_trigger_measurement(aht10_data_t* result)
{
    esp_err_t res=ESP_FAIL;
    uint8_t status;
    uint32_t hum=0,temp=0;
    i2c_bus_read_dev(AHT10_ADDR,NULL,0,&status,1);
    if ((status&0x68)==0x08)
    {
        uint8_t data[6];
        i2c_bus_read_dev(AHT10_ADDR,NULL,0,data,6);
        hum = (hum|data[1])<<8;
        hum = (hum|data[2])<<8;
        hum = (hum|data[3]);
        hum =hum >>4;
        temp = (temp|data[3])<<8;
        temp = (temp|data[4])<<8;
        temp = (temp|data[5]);
        temp = temp & 0xFFFFF;
        //trigger a measurement
        i2c_bus_write_dev(AHT10_ADDR,AHT10_CMD_MEASURE,sizeof(AHT10_CMD_MEASURE));
        result->temp=TEMP(temp);
        result->humi=HUMIDITY(hum);
        //printf("#AHT10 Temp=%ld Humi=%ld#\r\n",TEMP(temp),HUMIDITY(hum));
        st_aht10Ctrl.newData=true;
        res=ESP_OK;
    }
    return res;
}

//vfs
static esp_err_t aht10_read(void * dst, size_t size)
{
    esp_err_t res=ESP_FAIL;
    aht10_data_t* data=(void *)dst;
    aht10_get_data(data);
    st_aht10Ctrl.newData=false;
    return res;
}
static esp_err_t aht10_fcntl(uint8_t cmd, uint32_t arg)
{
    switch (cmd)
    {
    case 0:
        //reset sensor
        i2c_bus_write_dev(AHT10_ADDR,AHT10_CMD_RESTART,sizeof(AHT10_CMD_RESTART));
        break;
    case 1:
        //stop read
        xTimerStop(st_aht10Ctrl.timer,0);
        break;
    case 2:
        //exit stop mode
       xTimerStart(st_aht10Ctrl.timer,0);
        break;        
    default:
        break;
    }
    return ESP_OK;
}
static void aht10_timer( TimerHandle_t xTimer )   
{
    aht10_data_t data;
    aht10_trigger_measurement(&data);
    aht10_set_data(&data);
} 
esp_err_t dev_aht10_init(void)
{
    esp_err_t res=ESP_FAIL;
    dev_cb_t devCB = {
        .init_dev=aht10_init,
        .attr_ctl_dev=aht10_fcntl,
        .read_dev=aht10_read,
        .write_dev=NULL,
    };
    st_aht10Ctrl.lock=xSemaphoreCreateBinary();
    st_aht10Ctrl.timer=xTimerCreate("aht10_sacn",AHT10_PERIOD_MS,true,NULL,aht10_timer);
    DEV->aht10Handle= dev_register("/dev/aht10",&devCB);
    if (DEV->aht10Handle)
    {
        res=ESP_OK;
        xTimerStart(st_aht10Ctrl.timer,pdMS_TO_TICKS(100));
        xSemaphoreGive(st_aht10Ctrl.lock);
    }
    return res;
}