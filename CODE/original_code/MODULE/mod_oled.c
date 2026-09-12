#include "mod_oled.h"
#include "mod_oled_Font.h"
#include "sys_i2c.h"
#include "param.h"

/**
	******************************************************************************
	* @file    mod_oled.c
	* @author  zyj
	* @version V0.0.1
	* @date    19-10-2025
	* @brief   oled
	******************************************************************************
	*	驱动代码: oled
	* ic: SSD1306  SH1106
	*     串行模式下不支持数据读取  只可以写入
	******************************************************************************
	*/
	
//OLED 从机写入地址
#define OLED_ADDR_WD  	0x78

//OLED 控制字
#define OLED_W_DATA     0x40     //写数据
#define OLED_W_COMMAND  0X00     //写命令

// OLED屏幕分辨率定义（0.96寸/1.3寸通常为128x64）
#define OLED_WIDTH  128
#define OLED_HEIGHT 64


// 定义OLED初始化命令序列
uint8_t oled_init_cmd[] = {
#ifdef SSD1306
    0xAE,  						// 关闭显示
    0xA8, 0x3F,  			// 设置多路复用率
    0xD3, 0x00,  			// 设置显示偏移
    0x40,  						// 设置显示起始行
    0xA1,  						// 设置段重映射
    0xC8,  						// 设置COM输出扫描方向
    0xDA, 0x12,  			// 设置COM引脚外设配置
    0x81, 0x7F,  			// 设置对比度控制
    0xD9, 0xF1,  			// 设置预充电周期
    0xDB, 0x30,  			// 设置VCOMH取消选择级别
    0xA4,  						// 全屏显示开启
    0xA6,  						// 设置正常显示
    0xD5, 0x80,  			// 设置振荡器频率
    0x8D, 0x14,  			// 开启电荷泵
    0xAF   						// 打开显示
#else
		0xAE, /*display off*/ 
		0x02, /*set lower column address*/ 
		0x10, /*set higher column address*/ 
		0x40, /*set display start line*/ 
		0xB0, /*set page address*/
		0x81, /*contract control*/ 0xcf, /*128*/ 
		0xA1, /*set segment remap*/ 
		0xA6, /*normal / reverse*/ 
		0xA8, /*multiplex ratio*/ 
		0x3F, /*duty = 1/64*/ 
		0xad, /*set charge pump enable*/ 
		0x8b, /* 0x8B 内供 VCC */ 
		0x33, /*0X30---0X33 set VPP 9V */ 
		0xC8, /*Com scan direction*/ 
		0xD3, /*set display offset*/ 
		0x00, /* 0x20 */ 
		0xD5, /*set osc division*/ 
		0x80, 
		0xD9, /*set pre-charge period*/ 
		0x1f, /*0x22*/ 
		0xDA, /*set COM pins*/ 
		0x12, 
		0xdb, /*set vcomh*/ 
		0x40,
		0xAF, /*display ON*/
#endif
};
	
//创建OLED设备
i2c_device oled_dev = {
	.dev_addr_w = OLED_ADDR_WD,							//OLED地址
	.reg_addr 	= OLED_W_COMMAND,  					//此处设置OLED_W_COMMAND是命令寄存器地址
	.pdata		= oled_init_cmd,					  //指向初始化命令数组
	.data_len   = sizeof(oled_init_cmd)  	  //初始化命令数组长度
};

// 用于临时存储命令和数据的缓冲区
uint8_t oled_temp_buffer[128];  // 最大128字节，足够存储一行的数据

//用于显示的oled显示缓存 -- 对应屏幕点阵
uint8_t oled_display_buffer[8][128];

/**
  * @brief  oled写入命令 单字节
  * @param  cmd 命令字节
  * @retval None
  */
void oled_write_command(uint8_t cmd) {
    i2c_device temp_dev = {
        .dev_addr_w = OLED_ADDR_WD,
        .reg_addr   = OLED_W_COMMAND,
        .pdata      = &cmd,
        .data_len 	= 1
    };
    sys_i2c_write(I2C2, &temp_dev);
}

/**
  * @brief  oled写入数据 单字节
  * @param  data 数据字节
  * @retval None
  */
void oled_write_data(uint8_t* data, uint8_t length) {
    i2c_device temp_dev = {
        .dev_addr_w = OLED_ADDR_WD,
        .reg_addr 	= OLED_W_DATA,
        .pdata 			= data,
        .data_len 	= length
    };
    sys_i2c_write(I2C2, &temp_dev);
}


/** 一次性发送所有初始化命令
  * @brief  oled-ssd1306 ic初始化
  * @param  None
  * @retval None
  */
void mod_oled_init(void) {
	
	uint8_t ret;
	ret = sys_i2c_write(I2C2, &oled_dev);
	if(ret){
		LOG_ERROR("OLED initialization failed");
		return;
	}
	
	delay_ms(100);
	oled_clear();
	
	LOG_DEBUG("OLED initialized successfully");
}


/**
  * @brief  OLED设置光标位置
  * @param  page 页地址，范围：0~7 (对应8页)
  * @param  column 列地址，范围：0~127
  * @retval None
  */
void oled_set_cursor(uint8_t page, uint8_t column) {
	
    /*如果使用此程序驱动1.3寸的OLED显示屏，则需要解除此注释*/
    /*因为1.3寸的OLED驱动芯片（SH1106）有132列*/
    /*屏幕的起始列接在了第2列，而不是第0列*/
    /*所以需要将column加2，才能正常显示*/
    column += 2;
	
    oled_write_command(0xB0 | page);        						// 设置页地址
    oled_write_command(0x00 | (column & 0x0F));     		// 设置列地址低4位
    oled_write_command(0x10 | ((column >> 4) & 0x0F)); 	// 设置列地址高4位
}


void oled_update(void) {
    for (uint8_t i = 0; i < 8; i++) {
      oled_set_cursor(i, 0);
      oled_write_data(oled_display_buffer[i], 128);
    }
}

/**
	* @brief  清屏函数
	* @param  None
	* @retval None
	*/
void oled_clear(void) {
	// 初始化清屏缓冲区为0x00
  memset(oled_display_buffer, 0x00, sizeof(oled_display_buffer));
	// for (uint8_t i = 0; i < 8; i++) {
  //     for (uint8_t j = 0; j < 128; j++) {
  //         oled_display_buffer[i][j] = 0x00;
  //     }
	// }
}

/**
	* @brief  清屏部分区域函数  
	* @param  None
	* @retval None
	*/
void oled_clear_area(uint8_t x, uint8_t y, uint8_t width, uint8_t hight) {
	for (uint8_t j = y; j < y + hight; j++) {
		for (uint8_t i = x; i < x + width; i++) {
			oled_display_buffer[j / 8][i] &= ~(0x01 << (j % 8));
		}
	}
}
/**
	* @brief  屏幕全亮  一次发送一行数据
	* @param  None
	* @retval None
	*/
void oled_on(void) {
	// 初始化全亮缓冲区为0xFF
	memset(oled_display_buffer, 0xFF, sizeof(oled_display_buffer));
}

/**
  * @brief  将oled显存数组部分取反
  * @param  x 起始行位置，范围：0 - 127
  * @param  y 起始列位置，范围：0 - 63
  * @param  width 图片宽度 
  * @param  hight 图片高度  
  * @retval None
  */
void oled_reverse_area(uint8_t x, uint8_t y, uint8_t width, uint8_t hight) {
	for (int16_t j = y; j < y + hight; j++) {  //遍历page
		for (int16_t i = x; i < x + width; i++) {  //遍历column 
			if (i >= 0 && i <= 127 && j >= 0 && j <= 63) {  //超出屏幕的内容不显示
				oled_display_buffer[j / 8][i] ^= 0x01 << (j % 8);   //对显存数据进行取反
			}
			
		}
	}
}

/**
  * @brief  oled任意位置显示图像
  * @param  x 起始行位置，范围：0 - 127
  * @param  y 起始列位置，范围：0 - 63
  * @param  width 图片宽度 
  * @param  hight 图片高度  
  * @retval None
  */
void oled_show_img(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, const uint8_t* img_data) {

	oled_clear_area(x, y, width, hight);
	//这里需要y/8 并向上取整
	uint8_t page_num = (hight - 1) / 8 + 1;
 	uint8_t start_page = y / 8;

	for (uint8_t j = 0; j < page_num; j++) {
		//y坐标要先算在哪个page
		for (uint8_t i = 0; i < width; i++) {
      //第一页（当前位置所在页）
			oled_display_buffer[start_page + j][x + i] |= img_data[i + width * j] << (y % 8);

			//第二页（只有y不是8的整数倍时才需要处理）
			if (y % 8 != 0) {
				uint8_t next_page = start_page + 1 + j;
				if (next_page < 8) { //确保不超过OLED页数
					oled_display_buffer[next_page][x + i] |= img_data[i + width * j] >> (8 - (y % 8));  //0对8取余为0
				}
			}
		}
	}
}

/**
  * @brief  oled显示一个字符
  * @param  X 指定图像左上角的横坐标，屏幕区域：0~127
  * @param  Y 指定图像左上角的纵坐标，屏幕区域：0~63
  * @param  width 字符宽度  默认8
  * @param  hight 字符高度  默认16
  * @param  char_data 要显示的一个字符，范围：ASCII可见字符
  * @retval None
  */
void oled_showchar(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, char char_data) {      	
	// 计算字符在字模库中的索引
	uint8_t char_index = char_data - ' ';  //字模库从空格开始

  	// oled_display_buffer[x][y] = oled_font_8x16[char_index][i];
  	oled_show_img(x, y, width, hight, oled_font_8x16[char_index]);
}

/**
  * @brief  oled显示字符串
  * @param  X 指定图像左上角的横坐标，屏幕区域：0~127
  * @param  Y 指定图像左上角的纵坐标，屏幕区域：0~63
  * @param  width 字符宽度  默认8
  * @param  hight 字符高度  默认16
  * @param  string 要显示的字符串，范围：ASCII可见字符
  * @retval None
  */
void oled_show_string(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, char* string) {
    uint8_t i;
    for (i = 0; string[i] != '\0'; i++) {
        oled_showchar(x + i * width, y, width, hight, string[i]);
    }
}

/**
  * @brief  oled任意位置显示汉字
  * @param  x 起始行位置，范围：0 - 127
  * @param  y 起始列位置，范围：0 - 63
  * @param  width 字符宽度  默认16
  * @param  hight 字符高度  默认16
  * @retval None
  */
void oled_show_chinese(uint8_t x, uint8_t y, uint8_t width, uint8_t hight, char* chinese_lib) {

  char single_chinese[3] = { 0 };
  uint8_t p_chinese = 0;
  uint8_t p_index = 0;

  for (uint8_t i = 0; chinese_lib[i] != '\0'; i++) {
    single_chinese[p_chinese] = chinese_lib[i];
    p_chinese++;

    if (p_chinese >= 2) {
      p_chinese = 0;
      for (p_index = 0; strcmp(oled_chinese_font_16X16[p_index].index , "") != 0; p_index++) {
          if (strcmp(oled_chinese_font_16X16[p_index].index, single_chinese) == 0) {
              break;
          }
      }

      oled_show_img(x + ((i+1) / 2 - 1) * 16, y, width, hight, oled_chinese_font_16X16[p_index].data);
    }
  }
}


/**
  * @brief  在OLED缓冲区的指定位置画点
  * @param  x X坐标，0~127
  * @param  y Y坐标，0~63
  * @param  color 颜色：1-点亮，0-熄灭
  * @retval None
  */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
    
    uint8_t page = y / 8;
    uint8_t bit_pos = y % 8;
    
    if (color) {
        oled_display_buffer[page][x] |= (1 << bit_pos);
    } else {
        oled_display_buffer[page][x] &= ~(1 << bit_pos);
    }
}

/**
  * @brief  检测点是否点亮
  * @param  x X坐标，0~127
  * @param  y Y坐标，0~63
  * @param  color 颜色：1-点亮，0-熄灭
  * @retval None
  */
uint8_t oled_get_point(uint8_t x, uint8_t y) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return 0;
    
    uint8_t page = y / 8;
    uint8_t bit_pos = y % 8;

	if (oled_display_buffer[page][x] & (0x01 << bit_pos)) {
		return 1;
	}
    return 0;
}


/**
  * @brief  在OLED缓冲区的两点之间画线
  * @param  x0 起点X坐标
  * @param  y0 起点Y坐标
  * @param  x1 终点X坐标
  * @param  y1 终点Y坐标
  * @param  color 颜色：1-点亮，0-熄灭
  * @retval None
  */
void oled_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    
    while (1) {
        oled_draw_point(x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int16_t e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
  * @brief  绘制一个方块
  * @param  x0 起点X坐标
  * @param  y0 起点Y坐标
  * @param  x1 终点X坐标
  * @param  y1 终点Y坐标
  * @param  isfill 是否填充矩形
  * @retval None
  */
void oled_draw_square(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t isfill) {
    
    if (isfill) {
		//填充
		for (uint8_t i = x0; i < x1; i++) {
			for (uint8_t j = y0; j < y1; j++) {
				oled_draw_point(i, j, 1);
			}
		}
       
    } else {
        //不填充
		//上下两条线
		oled_draw_line(x0, y0, x1, y0, 1);
		oled_draw_line(x0, y1, x1, y1, 1);
		//左右两条线
		oled_draw_line(x0, y0, x0, y1, 1);
		oled_draw_line(x1, y0, x1, y1, 1);
    }
}



#if 0
/*	
******************************************************************************
*
*	触控转oled屏幕显示函数
*
******************************************************************************
*/

// 触摸轨迹专用缓冲区（8页，每页128列）
uint8_t oled_buffer[8][128] = {0};
uint8_t is_oled_initialized = 0;

// 保存上一个触摸点坐标
int16_t last_x = -1;
int16_t last_y = -1;

/**
  * @brief  在OLED缓冲区的指定位置画点
  * @param  x X坐标，0~127
  * @param  y Y坐标，0~63
  * @param  color 颜色：1-点亮，0-熄灭
  * @retval None
  */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
    
    uint8_t page = y / 8;
    uint8_t bit_pos = y % 8;
    
    if (color) {
        oled_buffer[page][x] |= (1 << bit_pos);
    } else {
        oled_buffer[page][x] &= ~(1 << bit_pos);
    }
}

/**
  * @brief  在OLED缓冲区的两点之间画线
  * @param  x0 起点X坐标
  * @param  y0 起点Y坐标
  * @param  x1 终点X坐标
  * @param  y1 终点Y坐标
  * @param  color 颜色：1-点亮，0-熄灭
  * @retval None
  */
void oled_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    
    while (1) {
        oled_draw_point(x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int16_t e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
  * @brief  刷新OLED显示（将缓冲区内容发送到OLED）
  * @param  None
  * @retval None
  */
void oled_refresh_display(void) {
    uint8_t i;
    uint8_t ret;
    
    for(i = 0; i < 8; i++) {
        oled_set_cursor(i, 0);
        
        i2c_device data_dev = {
            .dev_addr_w = OLED_ADDR_WD,
            .reg_addr   = OLED_W_DATA,
            .pdata      = oled_buffer[i],  // 使用对应页的缓冲区
            .data_len   = 128              // 一次发送一整行
        };
        
        ret = sys_i2c_write(I2C2, &data_dev);
        if(ret) {
            LOG_ERROR("oled_refresh_display fail at page %d", i);
        }
    }
}

/**
  * @brief  清空触摸轨迹缓冲区
  * @param  None
  * @retval None
  */
void oled_clear_trail(void) {
    memset(oled_buffer, 0, sizeof(oled_buffer));
    oled_refresh_display();
	LOG_DEBUG("Trail cleared after 3 seconds");
}

/**
  * @brief  初始化触摸轨迹显示
  * @param  None
  * @retval None
  */
void oled_trail_init(void) {
    memset(oled_buffer, 0, sizeof(oled_buffer));
    is_oled_initialized = 1;
    
    oled_clear();  //清屏
}



/**
  * @brief  将触摸坐标映射到OLED坐标
  * @param  touch_x 触摸X坐标
  * @param  touch_y 触摸Y坐标
  * @param  oled_x 输出的OLED X坐标
  * @param  oled_y 输出的OLED Y坐标
  * @retval None
  */
void map_touch_to_oled(int16_t touch_x, int16_t touch_y, uint8_t *oled_x, uint8_t *oled_y) {
    // 根据触摸板实际分辨率调整
    #define TOUCH_MAX_X 300
    #define TOUCH_MAX_Y 300
    
    // 将触摸坐标映射到OLED分辨率
    *oled_x = (touch_x * OLED_WIDTH) / TOUCH_MAX_X;
    *oled_y = (touch_y * OLED_HEIGHT) / TOUCH_MAX_Y;
    
    // 确保坐标在有效范围内
    if (*oled_x >= OLED_WIDTH) *oled_x = OLED_WIDTH - 1;
    if (*oled_y >= OLED_HEIGHT) *oled_y = OLED_HEIGHT - 1;
}

#endif


#if 0


/**
	* @brief  清屏函数  一次发送一行数据
	* @param  None
	* @retval None
	*/

void oled_clear(void) {
	
	uint8_t i;
	
	// 初始化清屏缓冲区为0x00
	memset(oled_temp_buffer, 0x00, sizeof(oled_temp_buffer));
	
	for(i = 0; i < 8; i++) {
		oled_set_cursor(i, 0);
    oled_write_data(oled_temp_buffer, 128);
	}
}


/**
	* @brief  屏幕全亮  一次发送一行数据
	* @param  None
	* @retval None
	*/
void oled_on(void) {
	PROJ_FUNC_ENTER;
	uint8_t i;
	uint8_t ret;
	
	// 初始化全亮缓冲区为0xFF
	memset(oled_temp_buffer, 0xFF, sizeof(oled_temp_buffer));
	
	for(i = 0; i < 8; i++) {
			oled_set_cursor(i, 0);
      oled_write_data(oled_temp_buffer, 128);
			if(ret){
				LOG_ERROR("oled_on_fail");
			}
	}
	PROJ_FUNC_EXIT;
}

/**
  * @brief  oled显示一个字符
  * @param  line 行位置，范围：1~4
  * @param  column 列位置，范围：1~16
  * @param  char_data 要显示的一个字符，范围：ASCII可见字符
  * @retval None
  */
void oled_showchar(uint8_t line, uint8_t column, char char_data) {      	
	PROJ_FUNC_ENTER;
	// 计算字符在字模库中的索引
	uint8_t char_index = char_data - ' ';  //字模库从空格开始
	
	// 设置光标位置在上半部分
	oled_set_cursor((line - 1) * 2, (column - 1) * 8);
	
	// 创建数据写入设备实例
	i2c_device data_dev_upper = {
			.dev_addr_w = OLED_ADDR_WD,
			.reg_addr   = OLED_W_DATA,
			.pdata 			= (uint8_t *)oled_font_8x16[char_index],  // 前8字节
			.data_len 	= 8  // 一次发送8字节
	};
	sys_i2c_write(I2C2, &data_dev_upper);
	
	// 设置光标位置在下半部分
	oled_set_cursor((line - 1) * 2 + 1, (column - 1) * 8);
	
	// 发送下半部分数据（后8字节）
	i2c_device data_dev_lower = {
			.dev_addr_w = OLED_ADDR_WD,
			.reg_addr 	= OLED_W_DATA,
			.pdata 			= (uint8_t *)oled_font_8x16[char_index] + 8,   // 后8字节
			.data_len 	= 8
	};
	
	// 发送下半部分数据
	sys_i2c_write(I2C2, &data_dev_lower);
	
	PROJ_FUNC_EXIT;
}


/**
  * @brief  oled显示字符串
  * @param  line 起始行位置，范围：1~4
  * @param  column 起始列位置，范围：1~16
  * @param  string 要显示的字符串，范围：ASCII可见字符
  * @retval None
  */
void oled_show_string(uint8_t line, uint8_t column, char* string) {
    PROJ_FUNC_ENTER;
    
    uint8_t i;
    for (i = 0; string[i] != '\0'; i++) {
        oled_showchar(line, column + i, string[i]);
    }
    
    PROJ_FUNC_EXIT;
}


void oled_show_chinese(uint8_t page, uint8_t column, uint8_t width, uint8_t hight, char* chinese_lib) {

  char index[3] = { 0 };
  uint8_t char_count = strlen(chinese_lib); 
  
  for (uint8_t i = 0; i < char_count / 2; i++) {
    memcpy(index, chinese_lib + i * 2, 2);
    for(uint8_t j = 0; j < chinese_count; j++){
      if (strcmp(index, oled_chinese_font_16X16[j].index) == 0) {
          oled_show_img(page, column + i * width, width, hight, oled_chinese_font_16X16[j].data);
          break;
      }
      else{
          oled_show_img(page, column + i * width, width, hight, oled_chinese_font_16X16[chinese_count - 1].data);
      }
    }
  }
}


/**
  * @brief  oled显示字符串
  * @param  line 起始行位置，范围：1~4
  * @param  column 起始列位置，范围：1~16
  * @param  string 要显示的字符串，范围：ASCII可见字符
  * @retval None
  */
void oled_show_string(uint8_t line, uint8_t column, char* string) {
    PROJ_FUNC_ENTER;
    
    uint8_t i;
    for (i = 0; string[i] != '\0'; i++) {
        oled_showchar(line, column + i, string[i]);
    }
    
    PROJ_FUNC_EXIT;
}

/**
  * @brief  oled显示图像
  * @param  page 起始行位置，范围：0 - 7
  * @param  column 起始列位置，范围：0 - 127
  * @param  width 图片宽度 
  * @param  hight 图片高度  8个为一个page
  * @retval None
  */
void oled_show_img(uint8_t page, uint8_t column, uint8_t width, uint8_t hight, const uint8_t* img_data) {
	uint8_t page_num = hight / 8;
	for (uint8_t i = 0; i < page_num; i++) {
	  oled_set_cursor((page + i) , column);
	  oled_write_data((uint8_t *)img_data + i * width, width);
	}
}

void oled_show_chinese(uint8_t page, uint8_t column, char* chinese_lib) {

  char single_chinese[3] = { 0 };
  uint8_t p_chinese = 0;
  uint8_t p_index = 0;

  for (uint8_t i = 0; chinese_lib[i] != '\0'; i++) {
    single_chinese[p_chinese] = chinese_lib[i];
    p_chinese++;

    if (p_chinese >= 2) {
      p_chinese = 0;
      for (p_index = 0; strcmp(oled_chinese_font_16X16[p_index].index , "") != 0; p_index++) {
          if (strcmp(oled_chinese_font_16X16[p_index].index, single_chinese) == 0) {
              break;
          }
      }

      oled_show_img(page, column + ((i+1) / 2 - 1) * 16, 16, 16, oled_chinese_font_16X16[p_index].data);
    }
  }
}
#endif

#if 0
void oled_show_chinese(uint8_t page, uint8_t column, uint8_t width, uint8_t hight, char* chinese_lib) {

  char index[3] = { 0 };
  uint8_t char_count = strlen(chinese_lib); 
  
  for (uint8_t i = 0; i < char_count / 2; i++) {
    memcpy(index, chinese_lib + i * 2, 2);
    for(uint8_t j = 0; j < chinese_count; j++){
      if (strcmp(index, oled_chinese_font_16X16[j].index) == 0) {
          oled_show_img(page, column + i * width, width, hight, oled_chinese_font_16X16[j].data);
          break;
      }
      else{
          oled_show_img(page, column + i * width, width, hight, oled_chinese_font_16X16[chinese_count - 1].data);
      }
    }
  }
}
#endif



// /**
//   * @brief  oled次方函数
//   * @param  x 底数
//   * @param  y 指数
//   * @retval 返回值等于x的y次方
//   */
// uint32_t oled_pow(uint32_t x, uint32_t y) {
//     PROJ_FUNC_ENTER;
    
//     uint32_t result = 1;
//     while (y--) {
//         result *= x;
//     }
    
//     PROJ_FUNC_EXIT;
//     return result;
// }

// /**
//   * @brief  oled显示数字（十进制，正数）
//   * @param  line 起始行位置，范围：1~4
//   * @param  column 起始列位置，范围：1~16
//   * @param  number 要显示的数字，范围：0~4294967295
//   * @param  length 要显示数字的长度，范围：1~10
//   * @retval None
//   */
// void oled_show_num(uint8_t line, uint8_t column, uint32_t number, uint8_t length) {
//     PROJ_FUNC_ENTER;
    
//     uint8_t i;
//     for (i = 0; i < length; i++) {
//         oled_showchar(line, column + i, number / oled_pow(10, length - i - 1) % 10 + '0');
//     }
    
//     PROJ_FUNC_EXIT;
// }

// /**
//   * @brief  oled显示数字（十进制，带符号数）
//   * @param  line 起始行位置，范围：1~4
//   * @param  column 起始列位置，范围：1~16
//   * @param  number 要显示的数字，范围：-2147483648~2147483647
//   * @param  length 要显示数字的长度，范围：1~10
//   * @retval None
//   */
// void oled_show_signed_num(uint8_t line, uint8_t column, int32_t number, uint8_t length) {
//     PROJ_FUNC_ENTER;
    
//     uint8_t i;
//     uint32_t number_temp;
    
//     if (number >= 0) {
//         oled_showchar(line, column, '+');
//         number_temp = number;
//     } else {
//         oled_showchar(line, column, '-');
//         number_temp = -number;
//     }
    
//     for (i = 0; i < length; i++) {
//         oled_showchar(line, column + i + 1, number_temp / oled_pow(10, length - i - 1) % 10 + '0');
//     }
    
//     PROJ_FUNC_EXIT;
// }

// /**
//   * @brief  oled显示数字（十六进制，正数）
//   * @param  line 起始行位置，范围：1~4
//   * @param  column 起始列位置，范围：1~16
//   * @param  number 要显示的数字，范围：0~0xFFFFFFFF
//   * @param  length 要显示数字的长度，范围：1~8
//   * @retval None
//   */
// void oled_show_hex_num(uint8_t line, uint8_t column, uint32_t number, uint8_t length) {
//     PROJ_FUNC_ENTER;
    
//     uint8_t i, single_number;
//     for (i = 0; i < length; i++) {
//         single_number = number / oled_pow(16, length - i - 1) % 16;
//         if (single_number < 10) {
//             oled_showchar(line, column + i, single_number + '0');
//         } else {
//             oled_showchar(line, column + i, single_number - 10 + 'A');
//         }
//     }
    
//     PROJ_FUNC_EXIT;
// }

// /**
//   * @brief  oled显示数字（二进制，正数）
//   * @param  line 起始行位置，范围：1~4
//   * @param  column 起始列位置，范围：1~16
//   * @param  number 要显示的数字，范围：0~1111 1111 1111 1111
//   * @param  length 要显示数字的长度，范围：1~16
//   * @retval None
//   */
// void oled_show_bin_num(uint8_t line, uint8_t column, uint32_t number, uint8_t length) {
//     PROJ_FUNC_ENTER;
    
//     uint8_t i;
//     for (i = 0; i < length; i++) {
//         oled_showchar(line, column + i, number / oled_pow(2, length - i - 1) % 2 + '0');
//     }
    
//     PROJ_FUNC_EXIT;
// }






