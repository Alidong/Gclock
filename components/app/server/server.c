#include "server.h"
#include "http_server/http_server.h"
esp_err_t server_init(void)
{
    http_server_init();
    return ESP_OK;
}