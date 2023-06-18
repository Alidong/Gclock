#ifndef _NTP_H
#define _NTP_H
#include "stdbool.h"
void ntp_init(void);
bool ntp_sync_is_success(void);
#endif