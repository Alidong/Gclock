#include "dev_bus.h"
#include "../board.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
static const char *TAG = "i2c bus";
static SemaphoreHandle_t mutex;
/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}
void i2c_scan_dev(void)
{
    uint8_t address;
    xSemaphoreTake(mutex,portMAX_DELAY);
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            address = i + j;
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
            i2c_master_stop(cmd);
            esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 50);
            i2c_cmd_link_delete(cmd);
            if (ret == ESP_OK) {
                printf("%02x ", address);
            } else if (ret == ESP_ERR_TIMEOUT) {
                printf("UU ");
            } else {
                printf("-- ");
            }
        }
        printf("\r\n");
    }
    xSemaphoreGive(mutex);
}
esp_err_t i2c_find_dev(uint8_t address)
{
    xSemaphoreTake(mutex,portMAX_DELAY);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret==ESP_OK)
    {
        ESP_LOGI(TAG,"address %d found device!",address);
    } else {
        ESP_LOGE(TAG,"address %d not found! (%d)",address,ret);
    }
    xSemaphoreGive(mutex);
    return ret;
}
esp_err_t i2c_bus_read_dev(uint8_t device_address,const uint8_t* cmd, size_t cmd_len,uint8_t* read_buffer, size_t read_size)
{
    esp_err_t res;
    xSemaphoreTake(mutex,portMAX_DELAY);
    if (cmd_len)
    {
        res=i2c_master_write_read_device(I2C_MASTER_NUM,device_address,cmd,cmd_len,read_buffer,read_size,pdMS_TO_TICKS(100));
    }
    else
    {
        res = i2c_master_read_from_device(I2C_MASTER_NUM,device_address,read_buffer,  read_size,pdMS_TO_TICKS(100));
    }
    xSemaphoreGive(mutex);
    return res;
}
esp_err_t i2c_bus_write_dev(uint8_t device_address,const uint8_t* write_buffer, size_t write_size)
{
    esp_err_t res;
    xSemaphoreTake(mutex,portMAX_DELAY);
    res=i2c_master_write_to_device(I2C_MASTER_NUM,device_address,write_buffer,write_size,pdMS_TO_TICKS(100));
    xSemaphoreGive(mutex);
    return res; 
}

esp_err_t i2c_bus_init(void)
{
    esp_err_t err=ESP_FAIL;
    mutex=xSemaphoreCreateMutex();
    err=i2c_master_init();
    if (err!=ESP_OK)
    {
        ESP_LOGE(TAG,"i2c-0 init faild! (%d)",err);
    }
    return err;
}    