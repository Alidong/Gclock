#ifndef _HTTP_REQ_H
#define _HTTP_REQ_H
#include "esp_types.h"
typedef struct
{
    char server[32];
    char port[16];
    char path[32];
    uint8_t method;
    void *ctx;
    void(*http_req_handler)(void *ctx, uint8_t *data, size_t dataLen);
} http_req_t;
void http_req(void);
#endif