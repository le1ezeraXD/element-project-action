#include "sdk_menu.h"
#include "mod_oled.h"
#include "sys_encoder.h"
#include "drv_systick.h"

//菜单项类型
typedef enum{
	menu_type_normal,   	//普通菜单，有子菜单
	menu_type_func,			//功能项，执行函数
	menu_type_num_edit,		//数值类型
	menu_type_toggle		//开关类型
}menu_item_type;

//菜单项结构体(双向链表)
typedef struct menu_item{
    char* text;                 //显示文本     
	menu_item_type type;         
    void (*func)(void);         //功能函数指针
    struct menu_item* parent;   //父菜单指针
    struct menu_item* child;   	//子菜单链表头
    struct menu_item* next;   	//同级下一个
    struct menu_item* prev;  	//同级上一个

	//数据字段
	int32_t value;
	int32_t min;
	int32_t max;
	int32_t step;
}menu_list;


//菜单控制结构体
typedef struct {
    menu_list* current;         //当前选中的菜单项
    // menu_list* head;            //菜单链表头指针
	menu_list* root;            //根菜单  头指针，直接指向第一个实际数据节点
    uint8_t position;           //当前选中位置
    uint8_t item_count;         //菜单项总数
	uint8_t is_in_submenu;      //是否在子菜单中
	uint8_t is_editing;			//是否在编辑模式
	uint8_t offset;				//当前显示的第一项在菜单中索引
}menu_control;

menu_control menu;
// static const uint8_t DISPLAY_ROWS = 3;  //屏幕显示3行
static const uint8_t DISPLAY_ROWS = 4;  //屏幕显示4行
static const uint8_t scroll_width = 4;  //滚动条宽度
static const uint8_t menu_start_y = 0;  //菜单开始y坐标
static const uint8_t menu_end_y = 63;  	//菜单结束y坐标


//初始化菜单
void menu_init(void);
//执行当前功能
void menu_enter(void);
//显示菜单
void menu_display(void);


void func_test1(void) {
    LOG_DEBUG("1");
    oled_show_string(8 * 2, 0, 8, 16, "func1");
}

void func_test2(void) {
    LOG_DEBUG("2");
    oled_show_string(8 * 2, 16, 8, 16, "func2");
}

void func_test3(void) {
    LOG_DEBUG("3");
    oled_show_string(8 * 2, 32, 8, 16, "func3");
}

void func_test4(void) {
    LOG_DEBUG("4");
    oled_show_string(8 * 2, 48, 8, 16, "func4");
}

void func_time(void) {
	oled_clear();
	oled_show_string(8 * 2, 0, 8, 16, "time set");
}

void func_date(void) {
	oled_clear();
	oled_show_string(8 * 2, 0, 8, 16, "date set");
}

void func_info(void) {
	oled_clear();
	oled_show_string(8 * 2, 0, 8, 16, "zyj_watch");
}

void func_Back(void) {
    // 空函数，用于返回
}

/**
  * @brief 创建菜单项
  * @param text：显示文本
  * @param type：决定是普通结点还是功能节点（区别在于按键按下普通节点进入子菜单，功能节点执行回调函数）
  * @param func：回调功能函数
  * @retval 节点
  */
menu_list* create_item(const char* text, menu_item_type type, void (*func)(void)) {
    menu_list* item = (menu_list *)malloc(sizeof(menu_list));
    if (item == NULL) {
        LOG_ERROR("create_node fail!!!");
        return NULL;
    }
    item->text = (char *)malloc(strlen(text) + 1);   //strlen 计数有效字符 不包括'\0' 所以申请内存要加一个字节
    if (item->text) {
        strcpy(item->text, text);
    }
    else {
		free(item);  // 先释放item
        return NULL;
    }

	item->type = type;
    item->func = func;
	item->child = NULL;
	item->parent = NULL;
	item->prev = NULL;
    item->next = NULL;

	//初始化数据字段
	item->value = 0;
	item->min = 0;
	item->max = 100;
	item->step = 1;

    return item;
}


//添加子菜单项
void add_child_item(menu_list* parent, menu_list* child) {
	if(parent == NULL || child == NULL) {
		return;
	}
	child->parent = parent;

	if (parent->child == NULL) {
		parent->child = child;
	} else {
		menu_list* temp = parent->child;  //这里的parent->child是该项的子节点的头指针
		while (temp->next != NULL) { 
			temp = temp->next;
		}
		temp->next = child;
		child->prev = temp;
	}
}

//计数菜单项(获取链表长度)
uint8_t get_item_count(menu_list* head) {
	int length = 0;
	menu_list* temp = head;

	while (temp != NULL) {
		length++;
		temp = temp->next;
	}
	return length;
}


//根据位置获取菜单项(取值)
menu_list* get_elem_item(menu_list* head, int pos) {
	//获取线性表L中的某个数据元素的内容，通过变量value返回
    if (head == NULL) {
        LOG_DEBUG("头指针为空!");
        return NULL;
    }

	menu_list* temp = head;  //指向第一个节点
	uint16_t count  = 0;

	while (temp != NULL && count < pos) { 
		//向后扫描 直到p指向第i个元素 或者 p为空
		temp = temp->next;
		count++;
	}

	if (temp == NULL) {
		LOG_DEBUG("位置 %d 超出链表长度!", pos);
		return NULL;
	}

	return temp;
}

//添加菜单项目到链表尾部
void add_item_tail(menu_list** head, menu_list* new_item) {
    if (*head == NULL) {
        *head = new_item;
    }else {
        menu_list* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next; 
        }
        temp->next = new_item;
    }
}


//设置数值范围
void set_num_range(menu_list* item, int32_t min, int32_t max, int32_t step, int32_t init_value) {
	if (item && item->type == menu_type_num_edit) {
		item->min = min;
		item->max = max;
		item->step = step;
		item->value = init_value;
	}
}

//显示滚动条
void menu_draw_scroll_bar(uint8_t total_items, uint8_t visible_items, uint8_t position) {

	//先清空最右侧滚动条显示区域（右侧4个像素 127 - 4 + 1 = 124   4个像素：124 125 126 127）
	oled_clear_area(127 - scroll_width + 1, menu_start_y, scroll_width, menu_end_y);

	//绘制滚动条背景边框
	oled_draw_square(127 - scroll_width, menu_start_y, 127, menu_end_y, 0);

	//计算项数占整个屏幕的长度  就是计算把滚动条分成多少份  这里也就计算出每移动一次的step以及滑块的大小
	uint8_t thumb_height = 64 / total_items;
	if (thumb_height < 4) {
		thumb_height = 4;   //最小高度
	}

	//计算滑块位置
	uint8_t thumb_y = position * thumb_height;

	//限制滑块位置
    if (thumb_y + thumb_height > 64) {
        thumb_y = 64 - thumb_height;
    }

	//绘制滑块
	oled_draw_square(127 - scroll_width + 1, thumb_y, 127, thumb_y + thumb_height, 1);

	#if 0
	// 滑块高度_与可见行数占总数的比例成比例
	uint8_t thumb_hight = visible_items * 64 / total_items;
	if (thumb_hight < 4) {
		thumb_hight = 4;   //最小高度
	}

	// 计算滑块位置
	uint8_t thumb_y = 0;
	if (total_items < 1) {
		thumb_y = menu_start_y;
	} else {
		uint16_t max_position = total_items - 1;
		thumb_y = menu_start_y + (position * (menu_end_y - menu_start_y - thumb_hight)) / max_position;
	}

	// 绘制滑块
	oled_reverse_area(127 - scroll_width + 1, thumb_y, scroll_width - 1, thumb_hight);
	#endif
}

//菜单显示函数
void menu_display(void) {
	oled_clear();

	#if 0
	//显示标题
	if (menu.is_in_submenu) {
		oled_show_string(0, 0, 8, 16, "sub_menu");
	} else {
		oled_show_string(0, 0, 8, 16, "main_menu");
	}
	#endif

    //显示菜单项
	//获取当前菜单显示的头指针
    // menu_list* menu_head = menu.is_in_submenu && menu.current && menu.current->parent 
	// 						? menu.current->parent->child : menu.root;  //menu.current->parent->child 这个指的是当前在子目录中，然后要先拿到父菜单，然后取父菜单的首个子项
	
    menu_list* menu_head = NULL;
    if (menu.is_in_submenu && menu.current && menu.current->parent) {
        menu_head = menu.current->parent->child;
    } else {
        menu_head = menu.root;
    }

	// menu_list* item = menu_head;

	//计算滚动偏移量（自动滚动逻辑）
	//滚动显示是在显示时从offset开始取连续的4个菜单项
	//屏幕只能显示DISPLAY_ROWS  4项菜单  
	//如果总菜单项 < 4 不需要滚动显示  直接显示所有项
	//如果menu.item_count > 4 需要滚动显示

	//如果当前选中项在屏幕显示区域的上方(这里的menu.position由编码器决定)
	if (menu.position < menu.offset) {
		menu.offset = menu.position;  //向上滚动，让选中项显示在第一行
	} else if (menu.position >= menu.offset + DISPLAY_ROWS) {  //如果当前选中项在屏幕显示的下方
		menu.offset = menu.position - DISPLAY_ROWS + 1;		   //向下滚动，让当前选中项显示在屏幕的最下方
	}

	for (uint8_t i = 0; i < DISPLAY_ROWS; i++) {
		uint8_t item_index = menu.offset + i;  //从 menu.offset 处开始显示
		if (item_index >= menu.item_count) {
			break;
		}

		//更新第一行的显示
		/*
		position = 4
		检查：4 >= 0 + 4? 是的
		offset = 4 - 4 + 1 = 1
		显示：1, 2, 3, 4  ← 自动向下滚动一行
		*/
		menu_list* item = get_elem_item(menu_head, item_index);
		if(item == NULL) {
			break;
		}

		uint8_t y = i * 16;

		//检查是否是当前选中项
		if (item_index == menu.position) {
			oled_show_string(0, y, 8, 16, ">");
			//显示文本
        	oled_show_string(8, y, 8, 16, item->text);
			// oled_reverse_area(8, y, 128 - 8 - 8, 16); //整行反色

			if (!menu.is_editing || (menu.is_editing && item->type != menu_type_num_edit && item->type != menu_type_toggle)) {
				oled_reverse_area(8, y, strlen(item->text) * 8, 16);  //字符长度反色
			}
			//绘制滑块_简易
			// oled_draw_square(127 - scroll_width + 1, y, 127, y + 16, 1);
		} else {
            oled_show_string(0, y, 8, 16, "*");
			oled_show_string(8, y, 8, 16, item->text);
        }
		// item = item->next;

		//显示不同类型的内容
		switch (item->type) {
			case menu_type_toggle: {
				char toggle_text[8] = {0};
				sprintf(toggle_text, "[%s]", item->value ? "ON" : "OFF");
				// oled_show_string(strlen(item->text) * 8 + 16, y, 8, 16, toggle_text);
				oled_show_string(77, y, 8, 16, toggle_text);
				if (menu.is_editing && item_index == menu.position) {
					// oled_reverse_area(strlen(item->text) * 8 + 16, y, strlen(toggle_text) * 8, 16);
					oled_reverse_area(77, y, strlen(toggle_text) * 8, 16);
				}
				break;
			}
			case menu_type_num_edit:{
				char num_buf[8] = {0};
				sprintf(num_buf, "[%d]", item->value);
				// oled_show_string(strlen(item->text) * 8 + 16, y, 8, 16, num_buf);
				oled_show_string(77, y, 8, 16, num_buf);
				if (menu.is_editing && item_index == menu.position) {
					// oled_reverse_area(strlen(item->text) * 8 + 16, y, strlen(num_buf) * 8, 16);
					oled_reverse_area(77, y, strlen(num_buf) * 8, 16);
				}
				break;
			}
			default :
			 	//普通菜单或功能菜单不需要额外显示
				break;
		}
	}
	
	//绘制滑块&滚动条
	menu_draw_scroll_bar(menu.item_count, DISPLAY_ROWS, menu.position);
}


//返回上级菜单
void menu_back(void) {
	if (menu.is_in_submenu && menu.current && menu.current->parent) {
		//返回到父级菜单
		menu.is_in_submenu = 0;
		menu.is_editing = 0;
		menu.current = menu.current->parent;
		menu.position = 0;
		menu.offset = 0;
		menu.item_count = get_item_count(menu.root);
		menu_display();
	}
}

//进入子菜单
void menu_enter(void) {

	if(menu.current == NULL){
		return;
	}

	#if 1
	switch (menu.current->type) {
		case menu_type_normal:
			//进入子菜单
			menu.is_in_submenu = 1;
			menu.is_editing = 0;
			menu.current = menu.current->child;
			menu.position = 0;
			menu.offset = 0;
			//menu.item_count = get_item_count(menu.current->parent->child);  //这里的menu.current已经是二级菜单了，所以先要拿到上级菜单，然后再用child（二级菜单的头开始计数）
			menu.item_count = get_item_count(menu.current); 
			menu_display();
			break;

		case menu_type_num_edit:
		case menu_type_toggle:
			menu.is_editing = !menu.is_editing;
			menu_display();
			break;

		case menu_type_func:
			if (menu.current->func == func_Back) {
				menu_back();
			} else {
				menu.current->func();
			}
			break;
	}
	#endif

	#if 0
	if (menu.current->type == menu_type_normal && menu.current->child != NULL) {
		//进入子菜单
		menu.is_in_submenu = 1;
		menu.current = menu.current->child;
		menu.position = 0;
		//menu.item_count = get_item_count(menu.current->parent->child);  //这里的menu.current已经是二级菜单了，所以先要拿到上级菜单，然后再用child（二级菜单的头开始计数）
		menu.item_count = get_item_count(menu.current); 
		menu_display();

	} else if (menu.current->func != NULL) {
		if (menu.current->func == func_Back) {
			menu_back();
		} else {
			menu.current->func();
		}
	}
	#endif
}

void menu_encoder_process(void) {

	static int32_t last_diff = 0;
	static int32_t accumulator = 0;

    int32_t diff = sys_get_encoder_diff(TIM3, &g_encoder_dev);

	if (menu.is_editing) {
		//编辑模式
		if (menu.current) {
			switch (menu.current->type) 
				case menu_type_toggle: {
					//开关项 累计超过阈值切换
					accumulator += diff;
					if (accumulator >= 4 || accumulator <= -4) {
						//累计4步切换
						menu.current->value = !menu.current->value;
						accumulator = 0;
						menu_display();
					}
					break;

				case menu_type_num_edit: 
					//数值编辑模式
					accumulator += diff;
					if (accumulator >= 4 || accumulator <= -4) {
						int32_t change = (accumulator > 0) ? -menu.current->step : menu.current->step;
						menu.current->value += change; 
						//限制范围
						if (menu.current->value < menu.current->min) {
							menu.current->value = menu.current->min;
						} else if (menu.current->value > menu.current->max) {
							menu.current->value = menu.current->max;
						}
						accumulator = 0;
						menu_display();
					}
					break;
			}
		}
	} else {
		//浏览模式
		// 合并微小变化
		// static int32_t accumulator = 0;
		accumulator += diff;
		
		// 检测方向变化
		if ((last_diff > 0 && diff < 0) || (last_diff < 0 && diff > 0)) {
			accumulator = 0;  // 方向改变时重置
		}
		last_diff = diff;
		
		// 设置步进阈值
		const int32_t STEP_SIZE = 4;
		
		if (accumulator >= STEP_SIZE) {
			// 顺时针旋转 - 向上移动
			menu_list* menu_head = menu.is_in_submenu && menu.current && menu.current->parent 
								? menu.current->parent->child : menu.root;   //这里只是为获取头指针，是为了下面根据位置获取节点做铺垫
			
			if(menu.position > 0){
				menu.position--;
			}
			
			menu.current = get_elem_item(menu_head, menu.position);  //这里才根据position做出选择
			menu_display();
			accumulator = 0;
			
		} else if (accumulator <= -STEP_SIZE) {
			// 逆时针旋转 - 向下移动
			menu_list* menu_head = menu.is_in_submenu && menu.current && menu.current->parent 
								? menu.current->parent->child : menu.root;
			
			if (menu.position < menu.item_count - 1) {  //进了子菜单会由enter函数更新menu.item_count
				menu.position++;
			}
			
			menu.current = get_elem_item(menu_head, menu.position);
			menu_display();
			accumulator = 0;
		}
	}
    
	#if 0
    // 合并微小变化
    static int32_t accumulator = 0;
    accumulator += diff;
    
    // 检测方向变化
    if ((last_diff > 0 && diff < 0) || (last_diff < 0 && diff > 0)) {
        accumulator = 0;  // 方向改变时重置
    }
    last_diff = diff;
    
    // 设置步进阈值
    const int32_t STEP_SIZE = 4;
    
    if (accumulator >= STEP_SIZE) {
        // 顺时针旋转 - 向上移动
        menu_list* menu_head = menu.is_in_submenu && menu.current && menu.current->parent 
                               ? menu.current->parent->child : menu.root;   //这里只是为获取头指针，是为了下面根据位置获取节点做铺垫
        
        if(menu.position > 0){
            menu.position--;
        }
        
        menu.current = get_elem_item(menu_head, menu.position);  //这里才根据position做出选择
        menu_display();
        accumulator = 0;
        
    } else if (accumulator <= -STEP_SIZE) {
        // 逆时针旋转 - 向下移动
        menu_list* menu_head = menu.is_in_submenu && menu.current && menu.current->parent 
                               ? menu.current->parent->child : menu.root;
        
        if (menu.position < menu.item_count - 1) {  //进了子菜单会由enter函数更新menu.item_count
            menu.position++;
        }
        
        menu.current = get_elem_item(menu_head, menu.position);
        menu_display();
        accumulator = 0;
    }
	#endif

	#if 0
    int32_t diff = sys_get_encoder_diff(TIM3, &g_encoder_dev);
    
    if (diff == 0) {
        return;
    }

	//获取当前菜单显示的头指针
    menu_list* menu_head = menu.is_in_submenu ? menu.current->parent->child : menu.root; 

    // 根据差值移动光标
    if (diff > 0) {
        // 顺时针旋转 - 向上移动
        if(menu.position > 0){
            menu.position--;
        }
    } else {
        // 逆时针旋转 - 向下移动
        if (menu.position < menu.item_count - 1) {
            menu.position++;
        }
    }

	menu.current = get_elem_item(menu_head, menu.position);
    //显示菜单
    menu_display();
	#endif
}



//初始化菜单
void menu_init(void) {

	//创建根菜单项
	menu_list* main1 =	create_item("1.time_set", menu_type_normal, NULL);
	menu_list* main2 =	create_item("2.date_set", menu_type_normal, NULL);
	menu_list* main3 =	create_item("3.device_info", menu_type_func, func_info);
	menu_list* main4 =  create_item("4.brightness", menu_type_normal, NULL);
	menu_list* main5 =  create_item("5.volume ", menu_type_normal, NULL);
	menu_list* main6 =  create_item("6.rfw ", menu_type_normal, NULL);
	menu_list* main7 =  create_item("7.test ", menu_type_normal, NULL);

	//创建时间设置子菜单
	menu_list* time1 =	create_item("1.set_hour", menu_type_func, func_time);
	menu_list* time2 =	create_item("2.set_min", menu_type_func, func_time);
	menu_list* time3 =	create_item("3.back", menu_type_func, func_Back);

	//创建系统设置子菜单
	menu_list* sys1 =	create_item("1.set_year", menu_type_func, func_date);
	menu_list* sys2 =	create_item("2.set_month ", menu_type_func, func_date);
	menu_list* sys3 =	create_item("3.back", menu_type_func, func_Back);

	//创建音量编辑子菜单
	menu_list* volume_switch = create_item("switch", menu_type_toggle, NULL);
	menu_list* volume_edit = create_item("set_size", menu_type_num_edit, NULL);
	menu_list* volume_back = create_item("back", menu_type_func, func_Back);

	//设置数值范围
	set_num_range(volume_edit, 0, 100, 1, 0);

	//构建菜单树
	//主菜单链表
	/*这里没有头节点，main1是首元节点  root是头指针， 这里创建的是不带头节点的双向链表*/
	main1->next = main2;
	main2->prev = main1;
	main2->next = main3;
	main3->prev = main2;
	main3->next = main4;
	main4->prev = main3;
	main4->next = main5;
	main5->prev = main4;
	main5->next = main6;
	main6->prev = main5;
	main6->next = main7;
	main7->prev = main6;

	//时间菜单子链表
	add_child_item(main1, time1);
	add_child_item(main1, time2);
	add_child_item(main1, time3);
	time1->next = time2;
	time2->prev = time1;
	time2->next = time3;
	time3->prev = time2;

	//系统子菜单链表
	add_child_item(main2, sys1);
	add_child_item(main2, sys2);
	add_child_item(main2, sys3);
	sys1->next = sys2;
	sys2->prev = sys1;
	sys2->next = sys3;
	sys3->prev = sys2;

	//音量编辑子菜单
	add_child_item(main5, volume_switch);
	add_child_item(main5, volume_edit);
	add_child_item(main5, volume_back);
	volume_switch->next = volume_edit;
	volume_edit->prev = volume_switch;
	volume_edit->next = volume_back;
	volume_back->prev = volume_edit;

	//初始化菜单状态
	menu.root = main1;   //头指针指向main1
	menu.current = main1;
	menu.position = 0;
	menu.item_count = get_item_count(menu.root);
	menu.offset = 0;
	menu.is_in_submenu = 0;
	menu.is_editing = 0;

    //显示菜单
    menu_display();
}






