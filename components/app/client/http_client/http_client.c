/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "http_client.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "ping/ping_sock.h"
#define MAX_HTTP_CONN 3
#define URL_SEVER_OFFSET 7
#define REQ_BUFF_SIZE 4096

typedef struct
{
    http_req_t req;
    size_t dataLen;
    uint8_t *buf;
} http_req_node_t;
struct http_req_ctrl
{
    uint8_t connNum; //链接数
    QueueHandle_t reqQueue;
};
static struct http_req_ctrl st_reqCtrl;
#define WEB_SERVER "192.168.101.109"
// #define WEB_SERVER "www.baidu.com"
#define WEB_PORT "8888"
#define WEB_PATH "/"
static const char *TAG = "http req:";
static void http_url_generate(uint8_t method, char *server, char *port, char *path, char *url, size_t urlLen)
{
    switch (method)
    {
    case 0:
        snprintf(url, urlLen, "GET %s HTTP/1.0\r\nHost: %s:%s\r\nUser-Agent: esp-idf/1.0 esp32\r\n\r\n", path, server, port);
        break;
    default:
        break;
    }
}
static void http_req_task(void *pvParameters)
{
    struct addrinfo hints =
    {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int socketHandle, err, readBytes;
    http_req_node_t reqNode;
    while (1)
    {
        xQueueReceive(st_reqCtrl.reqQueue, &reqNode, portMAX_DELAY);
        err = getaddrinfo(reqNode.req.server, reqNode.req.port, &hints, &res);
        if (err != 0 || res == NULL)
        {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            continue;
        }
        /* Code to print the resolved IP.
        Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));
        ESP_LOGI(TAG, "... allocating socket");
        socketHandle = socket(res->ai_family, res->ai_socktype, 0);
        if (socketHandle < 0)
        {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if (connect(socketHandle, res->ai_addr, res->ai_addrlen) != 0)
        {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(socketHandle);
            freeaddrinfo(res);
            continue;
        }
        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);
        char reqUrl[128];
        http_url_generate(reqNode.req.method, reqNode.req.server, reqNode.req.port, reqNode.req.path, reqUrl, sizeof(reqUrl));
        ESP_LOGI(TAG, "reqUrl=%s", reqUrl);
        if (write(socketHandle, reqUrl, strlen(reqUrl)) < 0)
        {
            ESP_LOGE(TAG, "... socket send failed");
            close(socketHandle);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");
        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                       sizeof(receiving_timeout)) < 0)
        {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(socketHandle);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");
        reqNode.buf = (uint8_t *)heap_caps_malloc(REQ_BUFF_SIZE, MALLOC_CAP_SPIRAM);
        bzero(reqNode.buf, REQ_BUFF_SIZE);
        /* Read HTTP response */
        readBytes = 0;
        size_t readLen;
        do
        {
            readLen = read(socketHandle, reqNode.buf + readBytes, REQ_BUFF_SIZE - 1);
            readBytes += readLen;
        }
        while (readLen > 0 && readBytes < REQ_BUFF_SIZE);
        if (readBytes <= 0)
        {
            ESP_LOGE(TAG, "Last read return=%d errno=%d.", readLen, errno);

        }
        close(socketHandle);
        //do handler
        if (reqNode.req.http_req_handler != NULL)
        {
            printf("==%s==\r\n", reqNode.buf);
            reqNode.req.http_req_handler(reqNode.req.ctx, reqNode.buf, readBytes);
        }
        heap_caps_free(reqNode.buf);
        bzero(&reqNode, sizeof(reqNode));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    printf("%ld bytes from %s icmp_seq=%d ttl=%d time=%ld ms\n",
           recv_len, ipaddr_ntoa((ip_addr_t *)&target_addr), seqno, ttl, elapsed_time);
}

static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    printf("From %s icmp_seq=%d timeout\n", ipaddr_ntoa((ip_addr_t *)&target_addr), seqno);
}

static void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
    uint32_t loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    if (IP_IS_V4(&target_addr))
    {
        printf("\n--- %s ping statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
    }
    else
    {
        printf("\n--- %s ping statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
    }
    printf("%ld packets transmitted, %ld received, %ld%% packet loss, time %ldms\n",
           transmitted, received, loss, total_time_ms);
    // delete the ping sessions, so that we clean up all resources and can create a new ping session
    // we don't have to call delete function in the callback, instead we can call delete function from other tasks
    esp_ping_delete_session(hdl);
}
int Start_Ping_ESP32(void)
{
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    // parse IP address
    struct sockaddr_in6 sock_addr6;
    ip_addr_t target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    struct addrinfo hint;
    struct addrinfo *res = NULL;
    memset(&hint, 0, sizeof(hint));
    /* convert ip4 string or hostname to ip4 or ip6 address */
    if (getaddrinfo(WEB_SERVER, NULL, &hint, &res) != 0)
    {
        printf("ping: unknown host %s\n", WEB_SERVER);
        return 1;
    }
    if (res->ai_family == AF_INET)
    {
        struct in_addr addr4 = ((struct sockaddr_in *)(res->ai_addr))->sin_addr;
        inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
    }
    else
    {
        struct in6_addr addr6 = ((struct sockaddr_in6 *)(res->ai_addr))->sin6_addr;
        inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &addr6);
    }
    freeaddrinfo(res);
    config.target_addr = target_addr;

    /* set callback functions */
    esp_ping_callbacks_t cbs =
    {
        .cb_args = NULL,
        .on_ping_success = cmd_ping_on_ping_success,
        .on_ping_timeout = cmd_ping_on_ping_timeout,
        .on_ping_end = cmd_ping_on_ping_end
    };
    esp_ping_handle_t ping;
    esp_ping_new_session(&config, &cbs, &ping);
    esp_ping_start(ping);
    return 0;
}
esp_err_t http_req_get(http_req_t *req)
{
    esp_err_t err = ESP_FAIL;
    if (st_reqCtrl.connNum < MAX_HTTP_CONN)
    {
        http_req_node_t reqNode;
        memcpy(&(reqNode.req), req, sizeof(http_req_t));
        xQueueSend(st_reqCtrl.reqQueue, &reqNode, pdMS_TO_TICKS(10));
        err = ERR_OK;
    }
    return err;
}
void http_req(void)
{
    //Start_Ping_ESP32();
    st_reqCtrl.reqQueue = xQueueCreate(MAX_HTTP_CONN, sizeof(http_req_node_t));
    xTaskCreate(&http_req_task, "http_client", 4096, NULL, 8, NULL);
}
