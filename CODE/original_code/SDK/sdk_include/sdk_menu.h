#ifndef __SDK_MENU_H
#define __SDK_MENU_H
#include "sys.h"


void menu_init(void);
void menu_display(void);
void menu_encoder_process(void);

void menu_enter(void);
void menu_back(void);

void test_encoder_simple(void);


// menu_list* create_item(const char* text, menu_item_type type, void (*func)(void));
// void add_child_item(menu_list* parent, menu_list* child);
// uint8_t get_item_count(menu_list* head);
// menu_list* get_elem_item(menu_list* head, int pos);
// void add_item_tail(menu_list** head, menu_list* new_item);
// void set_num_range(menu_list* item, int32_t min, int32_t max, int32_t step, int32_t init_value);
// void menu_draw_scroll_bar(uint8_t total_items, uint8_t visible_items, uint8_t position);

#endif


