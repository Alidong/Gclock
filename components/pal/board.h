#ifndef _BOARD_H
#define _BOARD_H
#include "esp_err.h"
#include "esp_types.h"

#define NTP_SERVER1                  "pool.ntp.org"
#define NTP_SERVER2                  "time.nist.gov"
#define GMT_OFFSET_SEC               0
#define DAY_LIGHT_OFFSET_SEC         0
// if CUSTOM_TIMEZONE is not defined then TIMEZONE API used based on IP, check zones.h
// #define CUSTOM_TIMEZONE             "Europe/London"

/* Automatically update local time */
#define GET_TIMEZONE_API             "https://ipapi.co/timezone/"


#define EXAMPLE_PSRAM_DATA_ALIGNMENT 64

/*ESP32S3*/
/*power*/
#define PIN_POWER_ON                 46
#define PIN_BAT_VOLT                 4
/*SD card spi*/
#define PIN_SD_MOSI                 41
#define PIN_SD_MISO                 38
#define PIN_SD_CLK                  40
#define PIN_SD_CS                   39

/*LCD spi*/
#define PIN_LCD_BL                   15
#define PIN_LCD_RES                  9
#define PIN_LCD_CS                   10
#define PIN_LCD_DC                   5
#define PIN_LCD_CLK                  12
#define PIN_LCD_MOSI                 11
#define PIN_LCD_MISO                 13

/*knob*/
#define PIN_KNOB_ENCODER_A           2
#define PIN_KNOB_ENCODER_B           1
#define PIN_KNOB_BUTTON              0
#define KNOB_BUTTON_ACTIVE_LEVEL     0

/*Mic ES7210*/
#define PIN_MIC_INT                  3
#define PIN_MIC_SD                   14
#define PIN_MIC_LRCK                 21
#define PIN_MIC_SCLK                 47
#define PIN_MIC_MCLK                 48
/* External expansion */
/*i2s*/
#define I2S_PORT                     0
#define I2S_PIN_WS                   5
#define I2S_PIN_DA                   6
#define I2S_PIN_BCK                  7



/*i2c*/
#define I2C_SCL_PIN                    8
#define I2C_SDA_PIN                    18
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       100
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */
#endif