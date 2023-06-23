#include "board.h"
#include "pal_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "fcntl.h"
typedef struct
{
    SemaphoreHandle_t mutexLock;
    dev_cb_t devCB;
} dev_obj_t;
//vfs
static int dev_open(void *ctx, const char *path, int flags, int mode)
{
    esp_err_t res = ESP_FAIL;
    dev_obj_t *dev = (dev_obj_t *)ctx;
    xSemaphoreTake(dev->mutexLock, portMAX_DELAY);
    res = dev->devCB.init_dev();
    xSemaphoreGive(dev->mutexLock);
    return res;
}
static ssize_t dev_write(void *p, int fd, const void *data, size_t size)
{
    esp_err_t res = ESP_FAIL;
    dev_obj_t *dev = (dev_obj_t *)p;
    xSemaphoreTake(dev->mutexLock, portMAX_DELAY);
    res = dev->devCB.write_dev(data, size);
    xSemaphoreGive(dev->mutexLock);
    return res;
}
static ssize_t dev_read(void *ctx, int fd, void *dst, size_t size)
{
    esp_err_t res = ESP_FAIL;
    dev_obj_t *dev = (dev_obj_t *)ctx;
    xSemaphoreTake(dev->mutexLock, portMAX_DELAY);
    res = dev->devCB.read_dev(dst, size);
    xSemaphoreGive(dev->mutexLock);
    return res;
}
static int dev_fcntl(void *ctx, int fd, int cmd, int arg)
{
    esp_err_t res = ESP_FAIL;
    dev_obj_t *dev = (dev_obj_t *)ctx;
    xSemaphoreTake(dev->mutexLock, portMAX_DELAY);
    res = dev->devCB.attr_ctl_dev(cmd, arg);
    xSemaphoreGive(dev->mutexLock);
    return res;
}
devHandle dev_register(const char *path, dev_cb_t *cb)
{
    dev_obj_t *dev = (dev_obj_t *)malloc(sizeof(dev_obj_t));
    int devHandle = ESP_FAIL;
    esp_vfs_t dev_vfs =
    {
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .open_p = &dev_open,
        .read_p = &dev_read,
        .write_p = &dev_write,
        .fcntl_p = &dev_fcntl,
    };
    dev->mutexLock = xSemaphoreCreateMutex();
    memcpy(&(dev->devCB), cb, sizeof(dev_cb_t));
    ESP_ERROR_CHECK(esp_vfs_register(path, &dev_vfs, dev));
    devHandle = open(path, 0, 0);
    if (devHandle)
    {
        xSemaphoreGive(dev->mutexLock);
    }
    return devHandle;
}