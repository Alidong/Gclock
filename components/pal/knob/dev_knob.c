#include "dev_knob.h"
#include "board.h"
#include "pal_driver.h"
#include "pal_dev.h"
#include "esp_log.h"
#include "iot_knob.h"
#include "iot_button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
static const char* TAG = "knob:";
typedef struct _knob_ctrl
{
    button_handle_t button;
    knob_handle_t encoder;
    EventGroupHandle_t event;
}knob_ctrt_t;
static knob_ctrt_t st_knob_ctrl;
static void button_press_cb(void *button_handle, void *usr_data);
static void encoder_cb(void *handle, void *usr_data);
static esp_err_t dev_knob_init(void)
{
    st_knob_ctrl.event= xEventGroupCreate();
    button_config_t cfg = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
        .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
        .gpio_button_config = {
            .gpio_num = PIN_KNOB_BUTTON ,
            .active_level = KNOB_BUTTON_ACTIVE_LEVEL,
        },
    };
    st_knob_ctrl.button=iot_button_create(&cfg);

    knob_config_t kcfg={
        .default_direction=0,
        .gpio_encoder_a=PIN_KNOB_ENCODER_A,
        .gpio_encoder_b=PIN_KNOB_ENCODER_B,
    };
    st_knob_ctrl.encoder = iot_knob_create(&kcfg);

    iot_button_register_cb(st_knob_ctrl.button, BUTTON_PRESS_DOWN, button_press_cb, (void*)BUTTON_PRESS_DOWN);
    iot_button_register_cb(st_knob_ctrl.button, BUTTON_PRESS_UP, button_press_cb, (void*)BUTTON_PRESS_UP);
    iot_button_register_cb(st_knob_ctrl.button, BUTTON_LONG_PRESS_START, button_press_cb, (void*)BUTTON_LONG_PRESS_START);
    iot_knob_register_cb(st_knob_ctrl.encoder, KNOB_LEFT, encoder_cb, (void*)KNOB_LEFT);
    iot_knob_register_cb(st_knob_ctrl.encoder, KNOB_RIGHT, encoder_cb, (void*)KNOB_RIGHT);
    return ESP_OK;
}
static void button_press_cb(void *button_handle, void *usr_data)
{
    button_event_t event=(button_event_t) usr_data;
    switch (event)
    {
    case BUTTON_PRESS_DOWN:
        ESP_LOGI(TAG,"press down");
        xEventGroupSetBits(st_knob_ctrl.event,KNOB_PRESS_MASK);
        break;
    case BUTTON_PRESS_UP:
        ESP_LOGI(TAG,"press up");
        xEventGroupClearBits(st_knob_ctrl.event,KNOB_PRESS_MASK|KNOB_LONG_PRESS_MASK);
        break;
    case BUTTON_LONG_PRESS_START:
        ESP_LOGI(TAG,"long press");
        xEventGroupSetBits(st_knob_ctrl.event,KNOB_LONG_PRESS_MASK);
        break;
    default:
        break;
    }
}
static void encoder_cb(void *handle, void *usr_data)
{
    knob_event_t event=(knob_event_t) usr_data;
    switch (event)
    {
    case KNOB_LEFT:
        ESP_LOGI(TAG,"left");
        xEventGroupSetBits(st_knob_ctrl.event,KNOB_LEFT_MASK);
        break;
    case KNOB_RIGHT:
        ESP_LOGI(TAG,"right");
        xEventGroupSetBits(st_knob_ctrl.event,KNOB_RIGHT_MASK);
        break;
    default:
        break;
    }
}
static esp_err_t dev_knob_read(void *buf, size_t size)
{
    uint32_t* event=(uint32_t*)buf;
    if (size!=sizeof(uint32_t))
    {
        return ESP_ERR_INVALID_ARG;
    }
    *event=xEventGroupClearBits(st_knob_ctrl.event,KNOB_LEFT_MASK|KNOB_RIGHT_MASK);
    // ESP_LOGI(TAG,"encoder=%d",iot_knob_get_count_value(st_knob_ctrl.encoder));
    return ESP_OK;
}
int pal_knob_init(void)
{
    dev_cb_t devCB =
    {
        .init_dev = dev_knob_init,
        .attr_ctl_dev = NULL,
        .read_dev = dev_knob_read,
        .write_dev = NULL,
    };
    devHandle knob = dev_register("/dev/knob", &devCB);
    return knob;
}