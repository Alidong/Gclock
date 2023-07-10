#include "dev_speaker.h"
#include "../board.h"
#include "../pal_dev.h"
#include "../pal_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_check.h"
#define EXAMPLE_BUFF_SIZE               2048

static i2s_chan_handle_t                tx_chan;        // I2S tx channel handler
static void i2s_example_write_task(void *args)
{
    uint8_t *w_buf = (uint8_t *)calloc(1, EXAMPLE_BUFF_SIZE);
    assert(w_buf); // Check if w_buf allocation success

    /* Assign w_buf */
    for (int i = 0; i < EXAMPLE_BUFF_SIZE; i += 8) {
        w_buf[i]     = 0x12;
        w_buf[i + 1] = 0x34;
        w_buf[i + 2] = 0x56;
        w_buf[i + 3] = 0x78;
        w_buf[i + 4] = 0x9A;
        w_buf[i + 5] = 0xBC;
        w_buf[i + 6] = 0xDE;
        w_buf[i + 7] = 0xF0;
    }

    size_t w_bytes = 0;
    while (1) {
        /* Write i2s data */
        if (i2s_channel_write(tx_chan, w_buf, EXAMPLE_BUFF_SIZE, &w_bytes, 1000) == ESP_OK) {
            printf("Write Task: i2s write %d bytes\n", w_bytes);
        } else {
            printf("Write Task: i2s write failed\n");
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    free(w_buf);
    vTaskDelete(NULL);
}
static void i2s_init(void)
{
    /* Setp 1: Determine the I2S channel configuration and allocate two channels one by one
     * The default configuration can be generated by the helper macro,
     * it only requires the I2S controller id and I2S role
     * The tx and rx channels here are registered on different I2S controller,
     * only ESP32-C3, ESP32-S3 and ESP32-H2 allow to register two separate tx & rx channels on a same controller */
     i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, NULL, &tx_chan));

    /* Step 2: Setting the configurations of standard mode and initialize each channels one by one
     * The slot configuration and clock configuration can be generated by the macros
     * These two helper macros is defined in 'i2s_std.h' which can only be used in STD mode.
     * They can help to specify the slot and clock configurations for initialization or re-configuring */
    i2s_std_config_t tx_std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,    // some codecs may require mclk signal, this example doesn't need it
            .bclk = I2S_PIN_BCK,
            .ws   = I2S_PIN_WS,
            .dout = I2S_PIN_DA,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &tx_std_cfg));
    /* Step 3: Enable the tx and rx channels before writing or reading data */
    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    /* Step 4: Create writing and reading task */
    xTaskCreate(i2s_example_write_task, "i2s_example_write_task", 4096, NULL, 5, NULL);
}
void pal_speaker_init(void)
{
    i2s_init();
}