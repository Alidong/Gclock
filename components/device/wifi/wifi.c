/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "wifi.h"
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

#define WIFI_CFG_NVS "wifiConfig"
/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP */
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
    wifi_config_t wifiConfig;
    esp_netif_ip_info_t ip;
    EventGroupHandle_t wifi_event_group;
    TaskHandle_t* task;
    TimerHandle_t timerHandle;
}wifi_ctl_t;
static wifi_ctl_t  s_wifiCtrl;
static void smartconfig_task(void * parm);
static esp_err_t wifi_smart_cfg_start(void);
static esp_err_t wifi_smart_cfg_abort(void);
//保存wifi配置参数结构体变量wifiConfig到nvs
static void saveWifiConfig(wifi_config_t *wifiConfig)
{
    nvs_handle_t handle;
    nvs_open(WIFI_CFG_NVS, NVS_READWRITE, &handle); //打开
    nvs_set_blob(handle, WIFI_CFG_NVS, wifiConfig, sizeof(wifi_config_t));
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
    wifi_config_t *wifiConfig=&s_wifiCtrl.wifiConfig;
    if(event_base==WIFI_EVENT)
    {
        if (event_id==WIFI_EVENT_STA_START)
        {
            ESP_LOGI(TAG,"Open Wifi");
            s_wifiCtrl.state=WIFI_STATE_OPEN;
            if (wifiConfig->sta.ssid[0]!=0)
            {
                ESP_LOGI(TAG,"try to connect the wifi before");
                ESP_ERROR_CHECK( esp_wifi_disconnect() );
                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, wifiConfig) );
                esp_wifi_connect();
                //if wifi no connected, smartconfig will be actived in 10s.
                // xTimerReset(s_wifiCtrl.timerHandle,0);
                // xTimerStart(s_wifiCtrl.timerHandle,0);
            }
            else
            {
                ESP_LOGW(TAG,"no wifi config");
                wifi_smart_cfg_start();
            }
        }
        else if(event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            //retry to connect
            ESP_LOGW(TAG,"retry to connect");
            esp_wifi_connect();
            xEventGroupClearBits(s_wifiCtrl.wifi_event_group, CONNECTED_BIT);
            s_wifiCtrl.state=WIFI_STATE_DISCONNECT;
            //if wifi no connected, smartconfig will be actived in 10s.
            // if (!xTimerIsTimerActive(s_wifiCtrl.timerHandle))
            // {
            //     xTimerStart(s_wifiCtrl.timerHandle,0);
            // }
        }
    }
    else if(event_base==IP_EVENT)
    {
        if(event_id == IP_EVENT_STA_GOT_IP)
        {
            // xTimerReset(s_wifiCtrl.timerHandle,0);
            // xTimerStop(s_wifiCtrl.timerHandle,0);
            xEventGroupSetBits(s_wifiCtrl.wifi_event_group, CONNECTED_BIT);
            s_wifiCtrl.state=WIFI_STATE_CONNECT;
            ESP_LOGI(TAG, "SSID:%s", wifiConfig->sta.ssid);
            ESP_LOGI(TAG, "PASSWORD:%s", wifiConfig->sta.password);
            ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
            ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            s_wifiCtrl.ip=event->ip_info;
        }
    }
    else if(event_base == SC_EVENT)
    {
        if(event_id == SC_EVENT_SCAN_DONE)
        {
            ESP_LOGI(TAG, "Scan done");
        }
        else if(event_id == SC_EVENT_FOUND_CHANNEL)
        {
            ESP_LOGI(TAG, "Found channel");
        }
        else if (event_id == SC_EVENT_GOT_SSID_PSWD)
        {
            ESP_LOGI(TAG, "Got SSID and password");
            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
            uint8_t ssid[33] = { 0 };
            uint8_t password[65] = { 0 };
            uint8_t rvd_data[33] = { 0 };

            bzero(wifiConfig, sizeof(wifi_config_t));
            memcpy(wifiConfig->sta.ssid, evt->ssid, sizeof(wifiConfig->sta.ssid));
            memcpy(wifiConfig->sta.password, evt->password, sizeof(wifiConfig->sta.password));
            wifiConfig->sta.bssid_set = evt->bssid_set;
            if (wifiConfig->sta.bssid_set == true) {
                memcpy(wifiConfig->sta.bssid, evt->bssid, sizeof(wifiConfig->sta.bssid));
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
            ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, wifiConfig) );
            esp_wifi_connect();
            //保存wifi配网参数到nvs中
            saveWifiConfig(wifiConfig);
        }
        else if (event_id == SC_EVENT_SEND_ACK_DONE)
        {
            xEventGroupSetBits(s_wifiCtrl.wifi_event_group, ESPTOUCH_DONE_BIT);
        }
    }
}

static void initialise_wifi(void)
{
    wifi_config_t *wifiConfig=&s_wifiCtrl.wifiConfig;
    readWifiConfig(wifiConfig);
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifiCtrl.wifi_event_group = xEventGroupCreate();
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
        uxBits = xEventGroupWaitBits(s_wifiCtrl.wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if(uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
            wifi_smart_cfg_abort();
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig done");
        }  
    }
}
static void auto_smart_config( TimerHandle_t xTimer )
{
    ESP_LOGI(TAG,"auto smart config");
    wifi_smart_cfg_start();
}
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
        err=nvs_get_blob(handle, WIFI_CFG_NVS, &s_wifiCtrl.wifiConfig, &len);
        if(err!=ESP_OK)
        {

            printf("Error (%s) reading NVS handle!\n", esp_err_to_name(err));
            err=nvs_set_blob(handle, WIFI_CFG_NVS, &s_wifiCtrl.wifiConfig , sizeof(wifi_config_t));
        }
    }
    ESP_ERROR_CHECK( err );
}
static esp_err_t wifi_smart_cfg_start(void)
{
    esp_err_t err=ESP_FAIL;
    if (s_wifiCtrl.state>=WIFI_STATE_OPEN && s_wifiCtrl.task==NULL)
    {
        xEventGroupClearBits(s_wifiCtrl.wifi_event_group, CONNECTED_BIT|ESPTOUCH_DONE_BIT);
        xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, s_wifiCtrl.task);
        s_wifiCtrl.state=WIFI_STATE_SMARTCFG;
        err=ESP_OK;
    }
    return err;
}
static esp_err_t wifi_smart_cfg_abort(void)
{
    if (s_wifiCtrl.task!=NULL || s_wifiCtrl.state==WIFI_STATE_SMARTCFG)
    {
        ESP_LOGI(TAG, "abort smart config");
        vTaskDelete(*s_wifiCtrl.task);
        esp_smartconfig_stop();
        xEventGroupClearBits(s_wifiCtrl.wifi_event_group, CONNECTED_BIT|ESPTOUCH_DONE_BIT);
        s_wifiCtrl.task=NULL;
    }
    return ESP_OK;
}
bool wifi_is_connected()
{
    return (s_wifiCtrl.state==WIFI_STATE_CONNECT);
}
void wifi_init(void)
{
    bzero(&s_wifiCtrl,sizeof(wifi_ctl_t));
    s_wifiCtrl.state=WIFI_STATE_CLOSE;
    wifi_get_cfg();
    initialise_wifi();
    // s_wifiCtrl.timerHandle=xTimerCreate("wifiReconn",pdMS_TO_TICKS(15*1000),false,NULL,auto_smart_config);
}
