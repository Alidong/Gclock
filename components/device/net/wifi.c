/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "net.h"
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

#define WIFI_CFG_NVS "WIFI_CONFIG"
/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "WIFI";

enum{
    WIFI_STATE_CLOSE,
    WIFI_STATE_OPEN,
    WIFI_STATE_DISCONNECT,
    WIFI_STATE_CONNECT,
    WIFI_STATE_SMARTCFG,
};
typedef struct 
{
    uint8_t state;
    wifi_config_t wifi_config;
    EventGroupHandle_t wifi_event_group;
    TaskHandle_t* task;
}wifi_ctl_t;
static wifi_ctl_t  wifi_ctrl;
static void smartconfig_task(void * parm);

//保存wifi配置参数结构体变量wifi_config到nvs------------------------------------------------
static void saveWifiConfig(wifi_config_t *wifi_config)
{
    nvs_handle_t handle;
    nvs_open(WIFI_CFG_NVS, NVS_READWRITE, &handle); //打开
    nvs_set_blob(handle, WIFI_CFG_NVS, wifi_config, sizeof(wifi_config_t));
    ESP_ERROR_CHECK(nvs_commit(handle)); //提交
    nvs_close(handle);                   //退出
}

//从nvs中读取wifi配置到给定的sta_config结构体变量
static void readWifiConfig(wifi_config_t *sta_config)
{
    nvs_handle_t handle;
    nvs_open(WIFI_CFG_NVS, NVS_READWRITE, &handle); //打开
    size_t len = sizeof(wifi_config_t);
    nvs_get_blob(handle, WIFI_CFG_NVS, sta_config, &len);
    nvs_close(handle); //退出

}
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    wifi_config_t *wifi_config=&wifi_ctrl.wifi_config;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG,"Open Wifi");
        wifi_ctrl.state=WIFI_STATE_OPEN;
        if (wifi_config->sta.ssid[0]!=0)
        {
            ESP_LOGI(TAG,"try to connect the wifi before");
            ESP_ERROR_CHECK( esp_wifi_disconnect() );
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, wifi_config) );
            esp_wifi_connect();
        }
        else
        {
            ESP_LOGW(TAG,"no wifi config");
            wifi_smart_cfg_start();
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(wifi_ctrl.wifi_event_group, CONNECTED_BIT);
        wifi_ctrl.state=WIFI_STATE_DISCONNECT;
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_ctrl.wifi_event_group, CONNECTED_BIT);
        wifi_ctrl.state=WIFI_STATE_CONNECT;
        ESP_LOGI(TAG, "SSID:%s", wifi_config->sta.ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", wifi_config->sta.password);
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config->sta.ssid, evt->ssid, sizeof(wifi_config->sta.ssid));
        memcpy(wifi_config->sta.password, evt->password, sizeof(wifi_config->sta.password));
        wifi_config->sta.bssid_set = evt->bssid_set;
        if (wifi_config->sta.bssid_set == true) {
            memcpy(wifi_config->sta.bssid, evt->bssid, sizeof(wifi_config->sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:");
            for (int i=0; i<33; i++) {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, wifi_config) );
        esp_wifi_connect();
        //保存wifi配网参数到nvs中
        saveWifiConfig(wifi_config);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(wifi_ctrl.wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

static void initialise_wifi(void)
{
    wifi_config_t *wifi_config=&wifi_ctrl.wifi_config;
    readWifiConfig(wifi_config);
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_ctrl.wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void smartconfig_task(void * parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
    while (1) {
        uxBits = xEventGroupWaitBits(wifi_ctrl.wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if(uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig done");
            // esp_smartconfig_stop();
            // wifi_ctrl.task=NULL;
            // vTaskDelete(NULL);
            wifi_smart_cfg_abort();
        }
    }
}
// Initialize NVS 
static void wifi_get_cfg()
{
    esp_err_t err ;
    nvs_handle_t handle;
    err = nvs_open(WIFI_CFG_NVS, NVS_READWRITE, &handle);
    if (err != ESP_OK) 
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        size_t len = sizeof(wifi_config_t);
        err=nvs_get_blob(handle, WIFI_CFG_NVS, &wifi_ctrl.wifi_config, &len);
        if(err!=ESP_OK)
        {

            printf("Error (%s) reading NVS handle!\n", esp_err_to_name(err));
            err=nvs_set_blob(handle, WIFI_CFG_NVS, &wifi_ctrl.wifi_config , sizeof(wifi_config_t));
        }
    }
    ESP_ERROR_CHECK( err );
}
esp_err_t wifi_smart_cfg_start(void)
{
    esp_err_t err=ESP_FAIL;
    if (wifi_ctrl.state>=WIFI_STATE_OPEN && wifi_ctrl.task==NULL)
    {
        xEventGroupClearBits(wifi_ctrl.wifi_event_group, CONNECTED_BIT|ESPTOUCH_DONE_BIT);
        xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, wifi_ctrl.task);
        wifi_ctrl.state=WIFI_STATE_SMARTCFG;
        err=ESP_OK;
    }
    return err;
}
esp_err_t wifi_smart_cfg_abort(void)
{
    if (wifi_ctrl.task!=NULL || wifi_ctrl.state==WIFI_STATE_SMARTCFG)
    {
        ESP_LOGI(TAG, "abort smart config");
        vTaskDelete(*wifi_ctrl.task);
        esp_smartconfig_stop();
        xEventGroupClearBits(wifi_ctrl.wifi_event_group, CONNECTED_BIT|ESPTOUCH_DONE_BIT);
        wifi_ctrl.task=NULL;
    }
    return ESP_OK;
}
bool wifi_is_connect()
{
    return (wifi_ctrl.state==WIFI_STATE_CONNECT);
}
void wifi_init(void)
{
    memset(&wifi_ctrl,0,sizeof(wifi_ctl_t));
    wifi_ctrl.state=WIFI_STATE_CLOSE;
    wifi_get_cfg();
    initialise_wifi();
}
