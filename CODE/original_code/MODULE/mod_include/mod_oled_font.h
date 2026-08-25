#ifndef __OLED_FONT_H
#define __OLED_FONT_H
#include <stdint.h>

/*OLED字模库，宽8像素，高16像素*/
extern const uint8_t oled_font_8x16[][16];
/*OLED显示图像库*/
extern const uint8_t oled_img[];


typedef struct {
	char index[3];    //GB2312 一个汉字是两个字节,再加上'\0'
	uint8_t data[32];
}chinese_t;

extern uint16_t chinese_count;
/*OLED显示汉字*/
extern const chinese_t oled_chinese_font_16X16[];

#endif
