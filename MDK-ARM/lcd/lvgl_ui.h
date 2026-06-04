#ifndef LVGL_UI_H
#define LVGL_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

extern volatile uint8_t lvgl_ui_last_key;
extern volatile uint8_t lvgl_ui_last_zone;
extern volatile uint8_t lvgl_ui_selected_color;
extern volatile uint8_t lvgl_ui_selected_square;
extern volatile int8_t lvgl_ui_selected_mode;
extern volatile uint8_t lvgl_ui_page;
extern volatile uint8_t lvgl_ui_last_gesture;
extern volatile uint32_t lvgl_ui_key_click_count;
extern volatile uint32_t lvgl_ui_event_count;
extern volatile uint32_t lvgl_ui_color_event_count;
extern volatile uint32_t lvgl_ui_grid_event_count;
extern volatile uint32_t lvgl_ui_back_event_count;
extern volatile uint32_t lvgl_ui_mode_event_count;
extern volatile uint32_t lvgl_ui_confirm_event_count;
extern volatile uint8_t lvgl_ui_r1_count;
extern volatile uint8_t lvgl_ui_r2_count;
extern volatile uint8_t lvgl_ui_fake_count;
extern volatile uint8_t lvgl_ui_total_marked_count;
extern volatile uint8_t lvgl_ui_confirm_ok;
extern volatile int8_t lvgl_ui_square_values[13];

void lvgl_ui_create(void);

#ifdef __cplusplus
}
#endif

#endif
