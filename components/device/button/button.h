#ifndef _BUTTON_H
#define _BUTTON_H
#include "esp_err.h"
#include <stdint.h>
#include "stdbool.h"
#ifdef __cplusplus
extern "C" {
#endif
enum button_event_t
{
    KEY_EVENT_NONE,
    KEY_EVENT_PRESS,
    KEY_EVENT_LONG_PRESS,
    KEY_EVENT_DOUBLE_CLICK,
};
enum button_index_t
{
    KEY_1,
    KEY_2,
    KEY_NUM_MAX,
};
typedef struct 
{
    bool keyPressd;
    uint8_t keyEvent[KEY_NUM_MAX];
}key_event_t;
esp_err_t dev_button_init(void);
#ifdef __cplusplus
}
#endif
#endif
