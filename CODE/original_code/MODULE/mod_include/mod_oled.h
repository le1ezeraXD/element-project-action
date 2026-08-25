#ifndef __MOD_OLED_H
#define __MOD_OLED_H
#include "sys.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

extern uint8_t oled_display_buffer[8][128];

void mod_oled_init(void);

void oled_update(void);
void oled_clear(void);
void oled_clear_area(uint8_t x, uint8_t y, uint8_t width, uint8_t hight);
void oled_reverse_area(uint8_t x, uint8_t y, uint8_t width, uint8_t hight);
void oled_on(void);
void oled_show_img(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, const uint8_t* img_data);
void oled_showchar(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, char char_data);
void oled_show_string(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, char* string);
void oled_show_chinese(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, char* chinese_lib);

/*draw*/
void oled_draw_point(uint8_t x, uint8_t y, uint8_t color);
uint8_t oled_get_point(uint8_t x, uint8_t y);
void oled_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
void oled_draw_square(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t isfill);

void oled_refresh_display(void);
void oled_clear_trail(void);
void oled_trail_init(void);
void map_touch_to_oled(int16_t touch_x, int16_t touch_y, uint8_t *oled_x, uint8_t *oled_y);

#if 0

// 触摸轨迹专用缓冲区（8页，每页128列）
extern uint8_t oled_buffer[8][128];
extern uint8_t is_oled_initialized;

// 保存上一个触摸点坐标
extern int16_t last_x;
extern int16_t last_y;

void set_light_pointer(uint8_t x, uint8_t y);
void oled_clear(void);
void oled_on(void);
void oled_showchar(uint8_t line, uint8_t column, char char_data);
void oled_show_string(uint8_t line, uint8_t column, char *string);
uint32_t oled_pow(uint32_t x, uint32_t y);
void oled_show_num(uint8_t line, uint8_t column, uint32_t number, uint8_t length);
void oled_show_signed_num(uint8_t line, uint8_t column, int32_t number, uint8_t length);
void oled_show_hex_num(uint8_t line, uint8_t column, uint32_t number, uint8_t length);
void oled_show_bin_num(uint8_t line, uint8_t column, uint32_t number, uint8_t length);


void oled_show_img(uint8_t page, uint8_t column, uint8_t width, uint8_t hight, const uint8_t* img_data);
void oled_show_chinese(uint8_t page, uint8_t column, uint8_t width, uint8_t hight, char* chinese_lib);
void oled_show_chinese(uint8_t page, uint8_t column, char* chinese_lib);

void oled_draw_point(uint8_t x, uint8_t y, uint8_t color);
void oled_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
void oled_refresh_display(void);
void oled_clear_trail(void);
void oled_trail_init(void);
void map_touch_to_oled(int16_t touch_x, int16_t touch_y, uint8_t *oled_x, uint8_t *oled_y);
#endif


#endif



