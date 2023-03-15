#include "util.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
static portMUX_TYPE schedulerLocker = portMUX_INITIALIZER_UNLOCKED;
inline void enter_critical(void)
{
    portENTER_CRITICAL(&schedulerLocker);
}
inline void exit_critical(void)
{
    portEXIT_CRITICAL(&schedulerLocker);
}