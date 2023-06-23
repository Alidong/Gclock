#ifndef _LCD_H
#define _LCD_H
#include <esp_types.h>
/* LCD CONFIG */

// ST7789 Parameters not documented in ESP IDF esp_lcd_panel_commands
#define ST7789_RAMCTRL          0xB0      // RAM control
#define ST7789_PORCTRL          0xB2      // Porch control
#define ST7789_GCTRL            0xB7      // Gate control
#define ST7789_VCOMS            0xBB      // VCOMS setting
#define ST7789_LCMCTRL          0xC0      // LCM control
#define ST7789_IDSET            0xC1      // ID setting
#define ST7789_VDVVRHEN         0xC2      // VDV and VRH command enable
#define ST7789_VRHS             0xC3      // VRH set
#define ST7789_VDVSET           0xC4      // VDV setting
#define ST7789_VCMOFSET         0xC5      // VCOMS offset set
#define ST7789_FRCTR2           0xC6      // FR Control 2
#define ST7789_PWCTRL1          0xD0      // Power control 1
#define ST7789_VAPVANEN         0xD2      // Enable VAP/VAN signal output
#define ST7789_CMD2EN           0xDF      // Command 2 enable
#define ST7789_PVGAMCTRL        0xE0      // Positive voltage gamma control
#define ST7789_NVGAMCTRL        0xE1      // Negative voltage gamma control
#define ST7789_DGMLUTR          0xE2      // Digital gamma look-up table for red
#define ST7789_DGMLUTB          0xE3      // Digital gamma look-up table for blue
#define ST7789_GATECTRL         0xE4      // Gate control
#define ST7789_SPI2EN           0xE7      // SPI2 enable
#define ST7789_PWCTRL2          0xE8      // Power control 2
#define ST7789_EQCTRL           0xE9      // Equalize time control
#define ST7789_PROMCTRL         0xEC      // Program control
#define ST7789_PROMEN           0xFA      // Program mode enable
#define ST7789_NVMSET           0xFC      // NVM setting
#define ST7789_PROMACT          0xFE      // Program action

#define LCD_PIXEL_CLOCK_HZ   (24 * 1000 * 1000)
// The pixel number in horizontal and vertical
#define LCD_H_RES            320
#define LCD_V_RES            170
#define LCD_PIX_SIZE            (LCD_H_RES * LCD_V_RES)

typedef void(*lcd_flush_done_cb_t)(void* ctx);
// The pixel number in horizontal and vertical
void lcd_init(void);
void lcd_flush(int x_start, int y_start, int x_end, int y_end, const void *buf);
void lcd_set_light(uint8_t lighteness);
void lcd_register_done_cb(void* ctx,lcd_flush_done_cb_t callback);
#endif