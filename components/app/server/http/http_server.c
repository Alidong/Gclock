/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "http_server.h"
#include "service/http_service.h"
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "cJSON.h"
#include "wifi/wifi.h"
/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */
#define HTTP_SERVER_STACK 8*1024
#define HTTP_SERVER_PRIO 8
static const char *TAG = "http_server";
typedef struct
{
    bool running;
    httpd_handle_t server;
    TaskHandle_t task;
} http_server_ctrl_t;
static http_server_ctrl_t http_server_ctl;

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.uri_match_fn = httpd_uri_match_wildcard;
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        hello_service_init(server);
        file_server_init(server);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}

static void http_server(void *xTimer)
{
    while (1)
    {
        if (wifi_is_connected() && !http_server_ctl.running)
        {
            ESP_LOGI(TAG, "start");
            http_server_ctl.server = start_webserver();
            http_server_ctl.running = true;
        }
        else if (!wifi_is_connected() && http_server_ctl.running)
        {
            stop_webserver(http_server_ctl.server);
            http_server_ctl.server = NULL;
            http_server_ctl.running = false;
            ESP_LOGI(TAG, "stop");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
esp_err_t http_server_init(void)
{
    esp_err_t res = ESP_FAIL;
    memset(&http_server_ctl, 0, sizeof(http_server_ctl));
    /* Start the server for the first time */
    xTaskCreate(http_server, "http_server", HTTP_SERVER_STACK, NULL, HTTP_SERVER_PRIO, &http_server_ctl.task);
    if (http_server_ctl.task)
    {
        res = ESP_OK;
    }
    return res;
}
