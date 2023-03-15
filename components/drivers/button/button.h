#ifndef _BUTTON_H
#define _BUTTON_H
#include "esp_err.h"
#include <stdint.h>
#include "stdbool.h"
#ifdef __cplusplus
extern "C" {
#endif
enum button_index_t
{
    KEY_1,
    KEY_2,
    KEY_NUM_MAX,
};
#define KEY_MASK_CLICK(keyID)         (0x01<<(keyID*2))
#define KEY_MASK_LONG_PRESS(keyID)    (0x02<<(keyID*2))
esp_err_t dev_button_init(void);
uint32_t button_wait_event(uint32_t keyEvnet, uint32_t tick);
#ifdef __cplusplus
}
#endif
#endif
