#ifndef MIC_DRV_H
#define MIC_DRV_H
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
void mic_drv_init(void);
bool mic_fft(float* samples ,uint16_t len);
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif 