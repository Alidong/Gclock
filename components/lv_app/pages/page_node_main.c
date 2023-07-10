#include "../pageManager/page_manager.h"
#include "pal.h"
#include"icon/icons.h"
#include "lvgl.h"
#include "hal/lv_app_hal.h"
#include "esp_log.h"
const char* TAG = "main node";
static page_node_t page1;
static lv_obj_t* gif=NULL;
static lv_timer_t* timerHandle;
static void scan_gif(const char* dirPath,char* gifPath)
{
    static uint16_t idx=1;
    struct dirent *entry;
    struct stat entry_stat;
    DIR *dir = opendir(dirPath);
    char filePath[64];
    /* Iterate over all files / folders and fetch their names and sizes */
    uint16_t count=0;
    while ((entry = readdir(dir)) != NULL) 
    {
        char* typeStr=&(entry->d_name[strlen(entry->d_name)-4]);
        if (entry->d_type!=DT_DIR && (strcmp(typeStr,".gif")==0))
        {
           snprintf(filePath,sizeof(filePath),"%s/%s",dirPath,entry->d_name);
           if (stat(filePath, &entry_stat) == -1) {
                ESP_LOGE(TAG, "Failed to stat file: %s",filePath);
                continue;
            }
            if (((entry_stat.st_size)/1024)>512)
            {
                continue;
            }
            ESP_LOGI(TAG, "Found %s : (%d.%dKB)", entry->d_name, entry_stat.st_size/1024, entry_stat.st_size%1024);
            count++;
            if (count==idx)
            {
                strcat(gifPath,filePath);
                idx++;
                break;
            }
        }
    }
    if ((entry = readdir(dir)) == NULL)
    {
        idx=1;
    }
    closedir(dir);
}
static void change_img(void)
{
    char gifPath[64];
    gifPath[0]=0;
    strcat(gifPath,"P:");
    scan_gif(EXTERNAL_DISK,&gifPath[2]);
    if (strlen(gifPath)>2)
    {
        ESP_LOGI(TAG, "gifPath %s ",gifPath);
        if (gif)
        {
            lv_obj_del(gif);
            gif=NULL;
        }
        gif=lv_gif_create(page1.obj);
        lv_gif_set_src(gif,gifPath);
        lv_obj_center(gif);
    }
}
static void timer_cb(lv_timer_t* timer)
{
    change_img();
}
static void onCreate(page_node_t *page)            // 页面创建
{
    //top bar
    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj,LV_PCT(100),LV_PCT(100));
    // lv_obj_set_style_bg_img_src(obj,&IMG_CLOCK,0);
    lv_obj_center(obj);
    timerHandle=lv_timer_create(timer_cb, 10 * 1000, NULL);
    page->obj=obj;
    change_img();
}

static void onRelease(page_node_t *page)            // 页面销毁
{
    lv_obj_del(page->obj);
    lv_timer_del(timerHandle);
    gif=NULL;
}

void page_main_init()
{
    page1.onCreate = onCreate;
    page1.onRelease = onRelease;
    lv_snprintf(page1.name,PAGE_NAME_LEN,"page1");
    pm_register_page(&page1);
}