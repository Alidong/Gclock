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
#include "string.h"
#define DEV_NAME_LEN 12
typedef struct
{
    SemaphoreHandle_t mutexLock;
    dev_cb_t devCB;
    char name[DEV_NAME_LEN];
} dev_obj_t;
typedef struct 
{
    const char* mount_point;
    uint8_t num;
    dev_obj_t* devList;
}pal_driver_ctrl_t;

static pal_driver_ctrl_t st_dev_ctrl;;
//vfs
static int dev_open(void *ctx, const char *path, int flags, int mode)
{
    int idx=-1;
    pal_driver_ctrl_t* ctrl=(pal_driver_ctrl_t*)ctx;
    path++;
    for (uint8_t i = 0; i < (ctrl->num); i++)
    {
        const char* name=ctrl->devList[i].name;
        if (strcmp(path,name)==0)
        {
            dev_obj_t *dev = &(ctrl->devList[i]);
            if (dev->devCB.init_dev)
            {
                xSemaphoreTake(dev->mutexLock, portMAX_DELAY);
                dev->devCB.init_dev();
                xSemaphoreGive(dev->mutexLock);
                idx=i;
            }
            break;
        } 
    }
    return idx;
}
static ssize_t dev_write(void *ctx, int fd, const void *data, size_t size)
{
    esp_err_t res = ESP_FAIL;
    if (fd<0 || fd>0xFF)
    {
        return res;
    }
    pal_driver_ctrl_t* ctrl=(pal_driver_ctrl_t*)ctx;
    dev_obj_t *dev = &(ctrl->devList[fd]);
    if (dev->devCB.write_dev)
    {
        xSemaphoreTake(dev->mutexLock, portMAX_DELAY);
        res = dev->devCB.write_dev(data, size);
        xSemaphoreGive(dev->mutexLock);
    }
    return res;
}
static ssize_t dev_read(void *ctx, int fd, void *dst, size_t size)
{
    esp_err_t res = ESP_FAIL;
    if (fd<0 || fd>0xFF)
    {
        return res;
    }
    pal_driver_ctrl_t* ctrl=(pal_driver_ctrl_t*)ctx;
    dev_obj_t *dev = &(ctrl->devList[fd]);
    if (dev->devCB.read_dev)
    {
        xSemaphoreTake(dev->mutexLock, portMAX_DELAY);
        res = dev->devCB.read_dev(dst, size);
        xSemaphoreGive(dev->mutexLock);
    }
    return res;
}
static int dev_fcntl(void *ctx, int fd, int cmd, int arg)
{
    esp_err_t res = ESP_FAIL;
    if (fd<0 || fd>0xFF)
    {
        return res;
    }
    pal_driver_ctrl_t* ctrl=(pal_driver_ctrl_t*)ctx;
    dev_obj_t *dev = &(ctrl->devList[fd]);
    if (dev->devCB.attr_ctl_dev)
    {
        xSemaphoreTake(dev->mutexLock, portMAX_DELAY);
        res = dev->devCB.attr_ctl_dev(cmd, arg);
        xSemaphoreGive(dev->mutexLock);
    }
    return res;
}
devHandle dev_register(const char *name, dev_cb_t *cb)
{
    pal_driver_ctrl_t* ctrl= &st_dev_ctrl;
    ctrl->num++;
    ctrl->devList=realloc(ctrl->devList,(ctrl->num)*sizeof(dev_obj_t));

    uint8_t idx=ctrl->num-1;
    dev_obj_t* dev=&ctrl->devList[idx];
    snprintf(dev->name,DEV_NAME_LEN,"%s",name);

    dev->mutexLock=xSemaphoreCreateMutex();
    xSemaphoreGive(dev->mutexLock);
    memcpy(&(dev->devCB), cb, sizeof(dev_cb_t));

    char path[32];
    snprintf(path,sizeof(path),"%s/%s",ctrl->mount_point,name);
    int devHandle = ESP_FAIL;
    devHandle = open(path, 0, 0);
    return devHandle;
}
void pal_devfs_mount(const char *path)
{
    esp_vfs_t dev_vfs =
    {
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .open_p = &dev_open,
        .read_p = &dev_read,
        .write_p = &dev_write,
        .fcntl_p = &dev_fcntl,
    };
    st_dev_ctrl.num=0;
    st_dev_ctrl.devList=NULL;
    st_dev_ctrl.mount_point=path;
    ESP_ERROR_CHECK(esp_vfs_register(path, &dev_vfs, &st_dev_ctrl));
}