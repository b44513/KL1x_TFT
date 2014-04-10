#ifndef __LCD_9341_H__
#define __LCD_9341_H__

#include "common.h"

#define LCD_XSIZE      320
#define LCD_YSIZE      240

#define WHITE          0xFFFF
#define BLACK          0x0000
#define GREY           0xF7DE
#define BLUE           0x001F
#define BLUE2          0x051F
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0

void lcd_init(void);
void lcd_io_init(void);
void lcd_reset(void);
void lcd_draw_pixel(uint16 x, uint16 y, uint16 color);
uint16 lcd_get_pixel(uint16 x, uint16 y);
void lcd_draw_h_line(uint16 x1, uint16 y, uint16 x2, uint16 color);
void lcd_draw_v_line(uint16 x, uint16 y1, uint16 y2, uint16 color);
void lcd_set_window(uint16 x1, uint16 y1, uint16 x2, uint16 y2);
void lcd_fill_window(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 color);
void lcd_fill_window_fast(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 color);
void lcd_backlight_ctrl (uint32 on_off);
void lcd_display_off(void);
void lcd_display_on(void);

#endif