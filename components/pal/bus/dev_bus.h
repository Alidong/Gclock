#ifndef _DEV_BUS_H
#define _DEV_BUS_H
#include "esp_err.h"
esp_err_t i2c_bus_init(void);
esp_err_t i2c_find_dev(uint8_t address);
void i2c_scan_dev(void);
esp_err_t i2c_bus_read_dev(uint8_t device_address, const uint8_t *cmd, size_t cmd_len, uint8_t *read_buffer, size_t read_size);
esp_err_t i2c_bus_write_dev(uint8_t device_address, const uint8_t *write_buffer, size_t write_size);
#endif