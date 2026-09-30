#include "u8g2.h"
#include "sys_i2c.h"
#include "drv_systick.h"
#include "param.h"
#include <string.h>

/**
  ******************************************************************************
  * @file    mod_u8g2_port.c
  * @author  thc
  * @brief   u8g2 lib port - i2c byte/gpio callback for SSD1306/SH1106 (128x64)
  ******************************************************************************
  * u8g2:      8bit graphics lib
  * ic:        SSD1306 / SH1106
  * bus:       I2C2,  write addr 0x78
  ******************************************************************************
  */   

/* OLED i2c 8bit write address */
#define OLED_ADDR_WD  0x78

/* u8g2 handle */
u8g2_t u8g2;

/* buffer to accumulate one transfer:
 * [0]  : control byte (0x00 command / 0x40 data)
 * [1..]: payload
 * 128 + 1 is enough for one tile row of a 128px wide display */
static uint8_t u8x8_tx_buf[128 + 1];
static uint8_t u8x8_tx_cnt = 0;

/**
  * @brief  u8g2 byte communication callback (I2C, one transfer = ctrl byte + data)
  * @param  u8x8     : u8x8 handle
  * @param  msg      : u8x8 message id
  * @param  arg_int  : message argument (length etc.)
  * @param  arg_ptr  : message argument (data pointer etc.)
  * @retval 1 success, 0 fail
  */
uint8_t u8x8_byte_stm32_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	switch (msg) {
		case U8X8_MSG_BYTE_INIT:
			/* I2C bus already initialized by mod_oled_init(), do nothing here */
			u8x8_tx_cnt = 0;
			break;

		case U8X8_MSG_BYTE_START_TRANSFER:
			/* start of a new transfer, clear accumulate buffer */
			u8x8_tx_cnt = 0;
			break;

		case U8X8_MSG_BYTE_SEND:
			/* u8x8 sends data in chunks, accumulate them first */
			if (u8x8_tx_cnt + arg_int <= sizeof(u8x8_tx_buf)) {
				memcpy(&u8x8_tx_buf[u8x8_tx_cnt], arg_ptr, arg_int);
				u8x8_tx_cnt += arg_int;
			}
			break;

		case U8X8_MSG_BYTE_END_TRANSFER:
		{
			/* one transfer = ctrl byte + payload, send via i2c in one go */
			if (u8x8_tx_cnt >= 1) {
				i2c_device dev = {
					.dev_addr_w = OLED_ADDR_WD,
					.reg_addr   = u8x8_tx_buf[0],      /* 0x00 command / 0x40 data */
					.pdata      = &u8x8_tx_buf[1],     /* payload after control byte */
					.data_len   = (uint16_t)(u8x8_tx_cnt - 1),
				};
				sys_i2c_write(I2C2, &dev);
			}
			u8x8_tx_cnt = 0;
			break;
		}

		default:
			return 0;
	}
	return 1;
}

/**
  * @brief  u8g2 gpio & delay callback (only delay is used with hardware i2c)
  * @param  u8x8     : u8x8 handle
  * @param  msg      : u8x8 message id
  * @param  arg_int  : message argument
  * @param  arg_ptr  : message argument
  * @retval 1 success, 0 fail
  */
uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	switch (msg) {
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			/* nothing to init, gpio for the oled is handled elsewhere */
			break;

		case U8X8_MSG_DELAY_MILLI:
			delay_ms(arg_int);
			break;

		case U8X8_MSG_DELAY_10MICRO:
			delay_us((uint32_t)arg_int * 10);
			break;

		case U8X8_MSG_DELAY_100NANO:
			/* not needed for this port */
            __NOP();
			break;

		default:
			/* other gpio messages (reset pin, etc.) are unused here */
			return 0;
	}
	return 1;
}

void draw_cat(void);
void draw_dog(void);
// void dog_normal(void);
// void dog_hello(void);

void oled_show_cat(void) {
	u8g2_ClearBuffer(&u8g2);
    delay_ms(1000);
	draw_cat();
	u8g2_SendBuffer(&u8g2);
}

void oled_show_dog(void) {
	u8g2_ClearBuffer(&u8g2);
    delay_ms(1000);
	draw_dog();
	u8g2_SendBuffer(&u8g2);
}

/**
  * @brief  用基本图形拼出一只小猫脸, 画进 u8g2 缓冲
  * @param  None
  * @retval None
  * @note   调用前需先 u8g2_ClearBuffer(), 调用后 u8g2_SendBuffer()
  */
void draw_cat(void) {
    /* 脸: 圆心(64,42) 半径22 的大圆 */
    u8g2_DrawCircle(&u8g2, 64, 42, 22, U8G2_DRAW_ALL);

    /* 两只耳朵: 三角形 */
    u8g2_DrawTriangle(&u8g2, 44, 30, 50, 8, 66, 26);    /* 左耳 */
    u8g2_DrawTriangle(&u8g2, 62, 26, 78, 8, 84, 30);    /* 右耳 */

    /* 眼睛: 实心小圆 */
    u8g2_DrawDisc(&u8g2, 55, 38, 3, U8G2_DRAW_ALL);     /* 左眼 */
    u8g2_DrawDisc(&u8g2, 73, 38, 3, U8G2_DRAW_ALL);     /* 右眼 */

    /* 鼻子: 一个小点 */
    u8g2_DrawDisc(&u8g2, 64, 46, 2, U8G2_DRAW_ALL);

    /* 嘴巴: 倒 V 两条线 */
    u8g2_DrawLine(&u8g2, 64, 48, 58, 54);
    u8g2_DrawLine(&u8g2, 64, 48, 70, 54);

        /* 胡须: 左右各两根 */
    u8g2_DrawLine(&u8g2, 40, 44, 20, 40);
    u8g2_DrawLine(&u8g2, 40, 48, 20, 50);
    u8g2_DrawLine(&u8g2, 88, 44, 108, 40);
    u8g2_DrawLine(&u8g2, 88, 48, 108, 50);
}

/**
  * @brief  用基本图形拼出一只小狗脸, 画进 u8g2 缓冲
  * @param  None
  * @retval None
  * @note   调用前需先 u8g2_ClearBuffer(), 调用后 u8g2_SendBuffer()
  */
void draw_dog(void) {
    /* 头: 圆心(64,40) 半径22 的大圆 */
    u8g2_DrawCircle(&u8g2, 64, 40, 22, U8G2_DRAW_ALL);

    /* 两只垂耳: 左右各一个圆角框 */
    u8g2_DrawRFrame(&u8g2, 34, 30, 12, 34, 5);    /* 左耳 */
    u8g2_DrawRFrame(&u8g2, 82, 30, 12, 34, 5);    /* 右耳 */

    /* 眼睛: 实心小圆 */
    u8g2_DrawDisc(&u8g2, 55, 36, 3, U8G2_DRAW_ALL);   /* 左眼 */
    u8g2_DrawDisc(&u8g2, 73, 36, 3, U8G2_DRAW_ALL);   /* 右眼 */

    /* 鼻子: 稍大的实心圆 */
    u8g2_DrawDisc(&u8g2, 64, 46, 4, U8G2_DRAW_ALL);

    /* 嘴巴: 鼻头往下两条线 */
    u8g2_DrawLine(&u8g2, 64, 50, 58, 56);
    u8g2_DrawLine(&u8g2, 64, 50, 70, 56);
    u8g2_DrawLine(&u8g2, 58, 56, 64, 58);
    u8g2_DrawLine(&u8g2, 70, 56, 64, 58);

    /* 舌头: 中间一小点 */
    u8g2_DrawDisc(&u8g2, 64, 58, 2, U8G2_DRAW_ALL);
}

static int isqrt(int n) {
    if (n <= 0) return 0;
    int x = n;
    int y = (x + 1) / 2;
    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }
    return x;
}

void drawFilledEllipse(u8g2_t *u8g2, int cx, int cy, int xa, int yb) {//cx cy 椭圆中心 xa:水平半轴 yb:垂直轴
    int b2 = yb * yb;
    for (int y = -yb; y <= yb; y++) {
        int dx = (xa * isqrt(b2 - y * y) + yb / 2) / yb; // 四舍五入
        u8g2_DrawHLine(u8g2, cx - dx, cy + y, 2 * dx + 1);
    }
}

void dog_normal(void) {
	u8g2_ClearBuffer(&u8g2);
    /* 左眼睛： */
    u8g2_DrawBox(&u8g2, 12, 12, 40, 10);

    /* 右眼睛： */
    u8g2_DrawBox(&u8g2, 76, 12, 40, 10);
   
	/* 嘴巴 */
	u8g2_DrawLine(&u8g2, 42, 37, 42, 47);
    u8g2_DrawLine(&u8g2, 42, 47, 72, 47);
    u8g2_DrawLine(&u8g2, 72, 47, 79, 52);
    u8g2_DrawLine(&u8g2, 79, 52, 86, 47);
    u8g2_DrawLine(&u8g2, 86, 47, 86, 37);
	u8g2_DrawLine(&u8g2, 86, 37, 42, 37);

	u8g2_SendBuffer(&u8g2);
}

void dog_hello(void) {
	u8g2_ClearBuffer(&u8g2);
	drawFilledEllipse(&u8g2, 34, 32, 15, 20);
	drawFilledEllipse(&u8g2, 94, 32, 15, 20);
	u8g2_SendBuffer(&u8g2);
}

void dog_forward(void) {
	u8g2_ClearBuffer(&u8g2);
	//眼睛
	u8g2_DrawTriangle(&u8g2, 10, 20, 20, 10, 60, 30);
	u8g2_DrawTriangle(&u8g2, 118, 20, 108, 10, 68, 30);
	// 嘴巴
	u8g2_DrawLine(&u8g2, 64, 40, 69, 45);
	u8g2_DrawLine(&u8g2, 69, 45, 74, 42);
	u8g2_DrawLine(&u8g2, 74, 42, 74, 52);
	u8g2_DrawLine(&u8g2, 74, 52, 54, 52);
	u8g2_DrawLine(&u8g2, 54, 52, 54, 42);
	u8g2_DrawLine(&u8g2, 54, 42, 59, 45);
	u8g2_DrawLine(&u8g2, 59, 45, 64, 40);
	u8g2_SendBuffer(&u8g2);
}

void dog_wag_tail(void) {
	// 中心 (64, 32)，外半径 30，内半径 20
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);                       // 前景色，画白点
	u8g2_DrawDisc(&u8g2, 32, 22, 18, U8G2_DRAW_ALL);   // 外圆实心
	u8g2_SetDrawColor(&u8g2, 0);                       // 背景色，擦除
	u8g2_DrawDisc(&u8g2, 32, 22, 13, U8G2_DRAW_ALL);   // 内圆挖空
	u8g2_SetDrawColor(&u8g2, 1);                       // 恢复前景色
	u8g2_DrawDisc(&u8g2, 96, 22, 18, U8G2_DRAW_ALL);   // 外圆实心
	u8g2_SetDrawColor(&u8g2, 0);                       // 背景色，擦除
	u8g2_DrawDisc(&u8g2, 96, 22, 13, U8G2_DRAW_ALL);   // 内圆挖空
	u8g2_SetDrawColor(&u8g2, 1);                       // 恢复前景色

	u8g2_DrawLine(&u8g2, 58, 18, 58, 26);
	u8g2_DrawLine(&u8g2, 64, 20, 64, 26);
	u8g2_DrawLine(&u8g2, 70, 18, 70, 26);

	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_DrawCircle(&u8g2, 58, 50, 6, U8G2_DRAW_ALL); // 空心整圆
	u8g2_DrawCircle(&u8g2, 70, 50, 6, U8G2_DRAW_ALL); // 空心整圆
	u8g2_SetDrawColor(&u8g2, 0);
	u8g2_DrawBox(&u8g2, 58 - 6, 50 - 6, 32, 6);      
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_SendBuffer(&u8g2);

}
/**
  * @brief  init u8g2 for ssd1306 128x64 i2c
  * @param  None
  * @retval None
  * @note   if the 1.3" panel (SH1106) shows a 2 column offset, please use:
  *         u8g2_Setup_sh1106_i2c_128x64_noname_f(...)
  */
void mod_u8g2_init(void) {
	/* 1.3" oled uses SH1106 controller (132-col GRAM, visible 128),
	 * which handles the 2-column offset internally. SSD1306 setup here
	 * would leave a stray vertical line at the right edge. */
	u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_stm32_i2c, u8x8_gpio_and_delay_stm32);
	u8x8_SetI2CAddress(&u8g2.u8x8, OLED_ADDR_WD);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);   /* 0 = display on */

	u8g2_ClearBuffer(&u8g2);
    delay_ms(1000);
	dog_forward();
	u8g2_SendBuffer(&u8g2);
}