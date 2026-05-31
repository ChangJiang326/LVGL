#include "lvgl_ui.h"
#include <stdint.h>

#define SCREEN_W          1024
#define SCREEN_H          600
#define COLOR_BUTTON_W    440
#define COLOR_BUTTON_H    420
#define COLOR_BUTTON_Y    90
#define RED_BUTTON_X      52
#define BLUE_BUTTON_X     532

#define GRID_COLS         3
#define GRID_ROWS         4
#define GRID_CELL_SIZE    120
#define GRID_GAP          20
#define GRID_W            (GRID_COLS * GRID_CELL_SIZE + (GRID_COLS - 1) * GRID_GAP)
#define GRID_H            (GRID_ROWS * GRID_CELL_SIZE + (GRID_ROWS - 1) * GRID_GAP)
#define GRID_X            ((SCREEN_W - GRID_W) / 2)
#define GRID_Y            ((SCREEN_H - GRID_H) / 2)
#define LVGL_UI_DEBUG_NO_LABEL 1

volatile uint8_t lvgl_ui_last_key = 0;
volatile uint8_t lvgl_ui_last_zone = 0;
volatile uint8_t lvgl_ui_selected_color = 0;
volatile uint8_t lvgl_ui_selected_square = 0;
volatile uint8_t lvgl_ui_page = 0;
volatile uint8_t lvgl_ui_last_gesture = 0;
volatile uint32_t lvgl_ui_key_click_count = 0;
volatile uint32_t lvgl_ui_stage = 0;
volatile uint32_t lvgl_ui_sub_stage = 0;
volatile uint32_t lvgl_ui_create_count = 0;
volatile uintptr_t lvgl_ui_last_obj = 0;

static void create_home_page(void);
static void create_grid_page(uint8_t color_id);

static lv_obj_t * home_screen;
static lv_obj_t * grid_screen;

static lv_color_t color_main(uint8_t color_id)
{
    return color_id == 1U ? lv_color_hex(0xD92525) : lv_color_hex(0x1F66D1);
}

static lv_color_t color_border(uint8_t color_id)
{
    return color_id == 1U ? lv_color_hex(0x8F1212) : lv_color_hex(0x103D83);
}

static lv_color_t color_page_bg(uint8_t color_id)
{
    return color_id == 1U ? lv_color_hex(0xFFF0F0) : lv_color_hex(0xEEF5FF);
}

static void apply_press_feedback(lv_obj_t * obj, lv_color_t bg)
{
    lv_obj_set_style_bg_color(obj, lv_color_mix(bg, lv_color_black(), 190), LV_STATE_PRESSED);
    lv_obj_set_style_border_color(obj, lv_color_white(), LV_STATE_PRESSED);
    lv_obj_set_style_translate_y(obj, 3, LV_STATE_PRESSED);
}

static void page_gesture_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        lvgl_ui_last_gesture = (uint8_t)dir;

        if(lvgl_ui_page == 2U && dir == LV_DIR_RIGHT) {
            create_home_page();
        }
    }
}

static void color_btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_PRESSED) {
        uint32_t color_id = (uint32_t)(uintptr_t)lv_event_get_user_data(e);

        lvgl_ui_selected_color = (uint8_t)color_id;
        lvgl_ui_selected_square = 0;
        lvgl_ui_last_key = (uint8_t)color_id;
        lvgl_ui_last_zone = 1;
        lvgl_ui_key_click_count++;

        create_grid_page((uint8_t)color_id);
    }
}

static void grid_btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_PRESSED) {
        uint32_t square_id = (uint32_t)(uintptr_t)lv_event_get_user_data(e);

        lvgl_ui_selected_square = (uint8_t)square_id;
        lvgl_ui_last_key = (uint8_t)square_id;
        lvgl_ui_last_zone = lvgl_ui_selected_color;
        lvgl_ui_key_click_count++;
    }
}

static void back_btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_PRESSED) {
        create_home_page();
    }
}

static void style_color_button(lv_obj_t * btn, uint8_t color_id)
{
    lv_color_t bg = color_main(color_id);

    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, COLOR_BUTTON_W, COLOR_BUTTON_H);
    lv_obj_set_style_radius(btn, 8, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, bg, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 4, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, color_border(color_id), LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn, 0, LV_STATE_DEFAULT);
    apply_press_feedback(btn, bg);
}

static void create_color_button(lv_obj_t * parent, int32_t x, const char * text, uint8_t color_id)
{
    lvgl_ui_stage = 100U + color_id;
    lv_obj_t * btn = lv_btn_create(parent);
    lvgl_ui_last_obj = (uintptr_t)btn;
    lvgl_ui_sub_stage = 101U;
    style_color_button(btn, color_id);
    lvgl_ui_sub_stage = 102U;
    lv_obj_set_pos(btn, x, COLOR_BUTTON_Y);
    lv_obj_add_event_cb(btn, color_btn_event_cb, LV_EVENT_PRESSED, (void *)(uintptr_t)color_id);
    lvgl_ui_sub_stage = 103U;

    (void)text;
#if LVGL_UI_DEBUG_NO_LABEL == 0
    {
        lv_obj_t * label = lv_label_create(btn);
        lvgl_ui_last_obj = (uintptr_t)label;
        lvgl_ui_sub_stage = 104U;
        lv_obj_remove_style_all(label);
        lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
        lv_label_set_text(label, text);
        lv_obj_center(label);
        lvgl_ui_sub_stage = 105U;
    }
#else
    lvgl_ui_sub_stage = 105U;
#endif
}

static void square_to_text(uint8_t value, char * text)
{
    if(value < 10U) {
        text[0] = (char)('0' + value);
        text[1] = '\0';
    } else {
        text[0] = '1';
        text[1] = (char)('0' + value - 10U);
        text[2] = '\0';
    }
}

static void create_grid_square(lv_obj_t * parent, uint8_t color_id, uint8_t square_id,
                               int32_t x, int32_t y)
{
    char label_text[3];
    lv_color_t bg = color_main(color_id);
    lv_obj_t * btn = lv_btn_create(parent);

    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, GRID_CELL_SIZE, GRID_CELL_SIZE);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_style_radius(btn, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, bg, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 3, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, color_border(color_id), LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn, 0, LV_STATE_DEFAULT);
    apply_press_feedback(btn, bg);
    lv_obj_add_event_cb(btn, grid_btn_event_cb, LV_EVENT_PRESSED, (void *)(uintptr_t)square_id);

    lv_obj_t * label = lv_label_create(btn);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    square_to_text(square_id, label_text);
    lv_label_set_text(label, label_text);
    lv_obj_center(label);
}

static void create_back_button(lv_obj_t * parent)
{
    lv_obj_t * btn = lv_btn_create(parent);

    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 72, 56);
    lv_obj_set_pos(btn, 18, 18);
    lv_obj_set_style_radius(btn, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x30343A), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x111315), LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn, back_btn_event_cb, LV_EVENT_PRESSED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_label_set_text(label, "<");
    lv_obj_center(label);
}

static void create_home_page(void)
{
    lv_obj_t * screen;

    lvgl_ui_stage = 10U;
    lvgl_ui_page = 1;
    lvgl_ui_selected_color = 0;
    lvgl_ui_selected_square = 0;
    lvgl_ui_sub_stage = 11U;

    if(home_screen == NULL) {
        lvgl_ui_sub_stage = 12U;
        home_screen = lv_obj_create(NULL);
        lvgl_ui_last_obj = (uintptr_t)home_screen;
        lvgl_ui_sub_stage = 13U;
        lv_obj_add_event_cb(home_screen, page_gesture_event_cb, LV_EVENT_GESTURE, NULL);
        lvgl_ui_sub_stage = 14U;
    }

    screen = home_screen;
    lvgl_ui_sub_stage = 15U;
    lv_obj_clean(screen);
    lvgl_ui_sub_stage = 16U;
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xF4F6F8), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_STATE_DEFAULT);
    lvgl_ui_sub_stage = 17U;

    lvgl_ui_stage = 20U;
    create_color_button(screen, RED_BUTTON_X, "RED", 1);
    lvgl_ui_stage = 21U;
    create_color_button(screen, BLUE_BUTTON_X, "BLUE", 2);
    lvgl_ui_stage = 22U;

    lvgl_ui_sub_stage = 23U;
    lv_scr_load(screen);
    lvgl_ui_sub_stage = 24U;
    lvgl_ui_stage = 29U;
}

static void create_grid_page(uint8_t color_id)
{
    lv_obj_t * screen;
    uint8_t square_id = 1;

    lvgl_ui_page = 2;

    if(grid_screen == NULL) {
        grid_screen = lv_obj_create(NULL);
        lv_obj_add_event_cb(grid_screen, page_gesture_event_cb, LV_EVENT_GESTURE, NULL);
    }

    screen = grid_screen;
    lv_obj_clean(screen);
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
    lv_obj_set_style_bg_color(screen, color_page_bg(color_id), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_STATE_DEFAULT);

    create_back_button(screen);

    for(int row = 0; row < GRID_ROWS; row++) {
        for(int col = 0; col < GRID_COLS; col++) {
            int32_t x = GRID_X + col * (GRID_CELL_SIZE + GRID_GAP);
            int32_t y = GRID_Y + row * (GRID_CELL_SIZE + GRID_GAP);
            uint8_t display_id = square_id;

            if(color_id == 2U) {
                display_id = (uint8_t)(row * GRID_COLS + (GRID_COLS - col));
            }

            create_grid_square(screen, color_id, display_id, x, y);
            square_id++;
        }
    }

    lv_scr_load(screen);
}

void lvgl_ui_create(void)
{
    lvgl_ui_stage = 1U;
    lvgl_ui_sub_stage = 1U;
    lvgl_ui_create_count++;
    home_screen = NULL;
    grid_screen = NULL;
    lvgl_ui_sub_stage = 2U;
    create_home_page();
    lvgl_ui_stage = 99U;
    lvgl_ui_sub_stage = 99U;
}
