#include "lcd.h"
#include "../board.h"
#include <string.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "hal/spi_types.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define LCD_MODULE_CMD_1
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (PIN_LCD_BL) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY_MAX           (1024)
#define LEDC_FREQUENCY          (10000) // Frequency in Hertz. Set frequency at 10 kHz
static const char *TAG = "lcd:";
// Commands for the LCD panel on init
typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;
typedef struct _lcd_driver
{
    esp_lcd_panel_handle_t   panelHandle;
    esp_lcd_panel_io_handle_t ioHandle;
    void* ctx;
    lcd_flush_done_cb_t cb;
}lcd_driver_t;
static lcd_driver_t st_lcd_ctrl;

static void ledc_init(void);
void lcd_init(void) {

    ledc_init();
    ESP_LOGI(TAG, "Backlight off");
    ESP_LOGI(TAG, "Init SPI bus");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    spi_bus_config_t buscfg = {
        .sclk_io_num = 12,
        .mosi_io_num = 11,
        .miso_io_num = -1,
        .quadwp_io_num = -1, // Quad SPI LCD driver is not yet supported
        .quadhd_io_num = -1, // Quad SPI LCD driver is not yet supported
        .max_transfer_sz = 320 * 80 * sizeof(uint16_t), // transfer 80 lines of pixels (assume pixel is RGB565) at most in one SPI transaction,
        .flags = SPICOMMON_BUSFLAG_GPIO_PINS
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO)); // Enable the DMA feature

    ESP_LOGI(TAG, "Init panel io");
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = 13,
        .cs_gpio_num = 10,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 4,
        .trans_queue_depth = 2,
        .flags.dc_low_on_data = 0,
        .flags.lsb_first = 0,
        .flags.sio_mode = 1,
        .flags.cs_high_active = 0,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
    };

// Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));
    esp_lcd_panel_handle_t panel_handle = NULL;
    ESP_LOGI(TAG, "Attach panel");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_LCD_RES,
        .flags.reset_active_high = 0,
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = 16,
    };

    // Create LCD panel handle for ST7789, with the SPI IO device handle
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    ESP_LOGI(TAG, "Init lcd");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(100));

    // Command sequence from https://github.com/Xinyuan-LilyGO/T-Embed/blob/main/example/tft/tft.ino#L12
    const lcd_cmd_t lcd_st7789v[] = {
        {ST7789_PORCTRL, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
        {ST7789_GCTRL, {0X75}, 1},
        {ST7789_VCOMS, {0X28}, 1},
        {ST7789_LCMCTRL, {0X2C}, 1},
        {ST7789_VDVVRHEN, {0X01}, 1},
        {ST7789_VRHS, {0X1F}, 1},
        {ST7789_FRCTR2, {0X13}, 1},
        {ST7789_PWCTRL1, {0XA7}, 1},
        {ST7789_PWCTRL1, {0XA4, 0XA1}, 2},
        {0xD6, {0XA1}, 1},
        {ST7789_PVGAMCTRL, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
        {ST7789_NVGAMCTRL, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
        {0x21,{0},0}
        // CAS and RAS are set by the ESP_lcd driver
    };

    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, lcd_st7789v[i].cmd, lcd_st7789v[i].data, lcd_st7789v[i].len & 0x7f));
        if (lcd_st7789v[i].len & 0x80) {
            vTaskDelay(pdMS_TO_TICKS(120));
        }
    }

    ESP_LOGI(TAG, "Enable lcd");
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    esp_lcd_panel_set_gap(panel_handle, 0, 35); // Some offset from the start of the line to where the display actually is
    // Swap coordinates around to match the display
    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, false, true);

    ESP_LOGI(TAG, "Backlight on");
    lcd_set_light(100);
    st_lcd_ctrl.panelHandle = panel_handle;
    st_lcd_ctrl.ioHandle = io_handle;
}

static void ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer =
    {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel =
    {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PIN_LCD_BL,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ledc_fade_func_install(0);
}
void lcd_set_light(uint8_t lighteness)
{
    //0-100
    if (lighteness >= 100)
    {
        lighteness = 100;
    }
    uint16_t ledcDutySet = lighteness * (LEDC_DUTY_MAX - 1) / 100;
    ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, ledcDutySet, 500);
    ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
}
static bool lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    if (st_lcd_ctrl.cb)
    {
        st_lcd_ctrl.cb(user_ctx);
    }
    return false;
}
void lcd_register_done_cb(void* ctx,lcd_flush_done_cb_t callback)
{
    st_lcd_ctrl.cb=callback;
    esp_lcd_panel_io_callbacks_t cbs={
        .on_color_trans_done=lcd_flush_ready,
    };
    esp_lcd_panel_io_register_event_callbacks(st_lcd_ctrl.ioHandle,&cbs,ctx);
}
void lcd_flush(int x_start, int y_start, int x_end, int y_end, const void *buf)
{
    // pass the draw buffer to the driver
    esp_lcd_panel_draw_bitmap(st_lcd_ctrl.panelHandle, x_start, y_start, x_end + 1, y_end + 1, buf);
}
