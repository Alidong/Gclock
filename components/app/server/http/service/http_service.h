#ifndef _HTTP_SERVICE_H
#define _HTTP_SERVICE_H
#include "esp_types.h"
#include "esp_err.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>

#define FILESERVICE_DOWNLOAD_FILE  "/storage/"
#define FILESERVICE_UPLOAD_FILE    "/upload/storage/"
#define FILESERVICE_DELETE_FILE    "/delete/storage/"

esp_err_t hello_service_init(httpd_handle_t* server);
esp_err_t file_server_init(httpd_handle_t* server);
#endif