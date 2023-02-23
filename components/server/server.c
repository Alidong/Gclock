#include "server.h"
#include "./http/http_server.h"
esp_err_t server_init(void)
{
    http_server_init();
    return ESP_OK;
}