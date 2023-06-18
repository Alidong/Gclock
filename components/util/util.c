#include "util.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "client/ntp/ntp.h"
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
bool util_ntp_is_success()
{
    return ntp_sync_is_success();
}