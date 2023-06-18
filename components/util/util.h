#ifndef _UTIL_H
#define _UTIL_H
#include "stdbool.h"
inline void enter_critical(void);
inline void exit_critical(void);
bool util_ntp_is_success();
#endif