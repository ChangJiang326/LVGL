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
extern volatile uint8_t lvgl_ui_page;
extern volatile uint8_t lvgl_ui_last_gesture;
extern volatile uint32_t lvgl_ui_key_click_count;

void lvgl_ui_create(void);

#ifdef __cplusplus
}
#endif

#endif
