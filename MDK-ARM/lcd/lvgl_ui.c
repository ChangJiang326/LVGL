#include "lvgl_ui.h"
#include "../task/ADC_TASK.h"
#include <stdint.h>

#define SCREEN_W          1024
#define SCREEN_H          600
#define COLOR_BUTTON_W    290
#define COLOR_BUTTON_H    360
#define COLOR_BUTTON_Y    120
#define RED_BUTTON_X      40
#define BLUE_BUTTON_X     367
#define INPUT_BUTTON_X    694

#define GRID_COLS         3
#define GRID_ROWS         4
#define GRID_CELL_SIZE    120
#define GRID_GAP          20
#define GRID_W            (GRID_COLS * GRID_CELL_SIZE + (GRID_COLS - 1) * GRID_GAP)
#define GRID_H            (GRID_ROWS * GRID_CELL_SIZE + (GRID_ROWS - 1) * GRID_GAP)
#define GRID_X            260
#define GRID_Y            ((SCREEN_H - GRID_H) / 2)
#define STATUS_LABEL_Y    548
#define SIDE_BTN_X        760
#define SIDE_BTN_Y        98
#define SIDE_BTN_W        170
#define SIDE_BTN_H        72
#define SIDE_BTN_GAP      26
#define STATE_LABEL_X     110
#define STATE_LABEL_Y     72
#define STATE_LABEL_W     120
#define STATE_LABEL_H     36
#define STATE_LABEL_GAP   8
#define GRID_VALUE_MAX    8
#define GRID_R1_REQUIRED  3
#define GRID_R2_REQUIRED  4
#define GRID_FAKE_REQUIRED 1
#define INPUT_LABEL_COUNT 12
#define INPUT_KEY_COUNT   4
#define INPUT_KEY_BOX_W   96
#define INPUT_KEY_BOX_H   54
#define INPUT_KEY_BOX_Y   42
#define INPUT_KEY_BOX_X   372
#define INPUT_KEY_BOX_GAP 18
#define NAV_PAGE_HOME     1
#define NAV_PAGE_GRID     2
#define NAV_PAGE_INPUT    3

volatile uint8_t lvgl_ui_last_key = 0;
volatile uint8_t lvgl_ui_last_zone = 0;
volatile uint8_t lvgl_ui_selected_color = 0;
volatile uint8_t lvgl_ui_selected_square = 0;
volatile int8_t lvgl_ui_selected_mode = 1;
volatile uint8_t lvgl_ui_page = 0;
volatile uint8_t lvgl_ui_last_gesture = 0;
volatile uint32_t lvgl_ui_key_click_count = 0;
volatile uint32_t lvgl_ui_stage = 0;
volatile uint32_t lvgl_ui_sub_stage = 0;
volatile uint32_t lvgl_ui_create_count = 0;
volatile uint32_t lvgl_ui_event_count = 0;
volatile uint32_t lvgl_ui_color_event_count = 0;
volatile uint32_t lvgl_ui_grid_event_count = 0;
volatile uint32_t lvgl_ui_back_event_count = 0;
volatile uint32_t lvgl_ui_mode_event_count = 0;
volatile uint32_t lvgl_ui_confirm_event_count = 0;
volatile uint8_t lvgl_ui_r1_count = 0;
volatile uint8_t lvgl_ui_r2_count = 0;
volatile uint8_t lvgl_ui_fake_count = 0;
volatile uint8_t lvgl_ui_total_marked_count = 0;
volatile uint8_t lvgl_ui_confirm_ok = 0;
volatile uintptr_t lvgl_ui_last_obj = 0;
volatile int8_t lvgl_ui_square_values[13];
volatile int8_t lvgl_ui_zone_square_values[3][13];
volatile uint8_t lvgl_ui_zone_confirmed[3];

static void create_home_page(void);
static void create_grid_page(uint8_t color_id);
static void create_input_page(void);
static void schedule_page_change(uint8_t page, uint8_t color_id);
static void page_change_async_cb(void * user_data);
static void refresh_grid_square(uint8_t square_id);
static void refresh_state_label(uint8_t square_id);
static void refresh_all_state_labels(void);
static void refresh_mode_buttons(void);
static void update_grid_status(const char * suffix);
static void square_to_text(uint8_t value, char * text);

static lv_obj_t * home_screen;
static lv_obj_t * grid_screen;
static lv_obj_t * input_screen;
static lv_obj_t * grid_status_label;
static lv_obj_t * input_value_labels[INPUT_LABEL_COUNT];
static lv_obj_t * input_key_boxes[INPUT_KEY_COUNT];
static lv_timer_t * input_timer;
static lv_obj_t * grid_state_labels[13];
static lv_obj_t * grid_square_btns[13];
static lv_obj_t * mode_btn_r1;
static lv_obj_t * mode_btn_r2;
static lv_obj_t * mode_btn_fake;
static lv_obj_t * confirm_btn;
static volatile uint8_t pending_nav_page = 0;
static volatile uint8_t pending_nav_color = 0;
static volatile uint8_t pending_nav_active = 0;

static void schedule_page_change(uint8_t page, uint8_t color_id)
{
    pending_nav_page = page;
    pending_nav_color = color_id;

    if(pending_nav_active == 0U) {
        pending_nav_active = 1U;
        (void)lv_async_call(page_change_async_cb, NULL);
    }
}

static void page_change_async_cb(void * user_data)
{
    uint8_t page;
    uint8_t color_id;

    (void)user_data;
    page = pending_nav_page;
    color_id = pending_nav_color;
    pending_nav_active = 0U;

    if(page == NAV_PAGE_HOME) {
        create_home_page();
    } else if(page == NAV_PAGE_GRID) {
        create_grid_page(color_id);
    } else if(page == NAV_PAGE_INPUT) {
        create_input_page();
    }
}

static lv_color_t color_main(uint8_t color_id)
{
    if(color_id == 1U) return lv_color_hex(0xD92525);
    if(color_id == 2U) return lv_color_hex(0x1F66D1);
    return lv_color_hex(0x24A148);
}

static lv_color_t color_border(uint8_t color_id)
{
    if(color_id == 1U) return lv_color_hex(0x8F1212);
    if(color_id == 2U) return lv_color_hex(0x103D83);
    return lv_color_hex(0x12632A);
}

static lv_color_t color_page_bg(uint8_t color_id)
{
    (void)color_id;
    return lv_color_hex(0xF4F6F8);
}

static lv_color_t value_color(int8_t value)
{
    if(value == 1) return lv_color_hex(0xD92525);
    if(value == 2) return lv_color_hex(0x1F66D1);
    if(value == -1) return lv_color_hex(0xE8C22E);
    return lv_color_hex(0x24A148);
}

static lv_color_t value_border_color(int8_t value)
{
    if(value == 1) return lv_color_hex(0x8F1212);
    if(value == 2) return lv_color_hex(0x103D83);
    if(value == -1) return lv_color_hex(0x967400);
    return lv_color_hex(0x12632A);
}

static void apply_press_feedback(lv_obj_t * obj, lv_color_t bg)
{
    lv_obj_set_style_bg_color(obj, lv_color_mix(bg, lv_color_black(), 190), LV_STATE_PRESSED);
    lv_obj_set_style_border_color(obj, lv_color_white(), LV_STATE_PRESSED);
    lv_obj_set_style_translate_y(obj, 3, LV_STATE_PRESSED);
}

static void page_gesture_event_cb(lv_event_t * e)
{
    (void)e;
}

static void color_btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        uint32_t color_id = (uint32_t)(uintptr_t)lv_event_get_user_data(e);

        lvgl_ui_event_count++;
        lvgl_ui_color_event_count++;
        lvgl_ui_selected_color = (uint8_t)color_id;
        lvgl_ui_selected_square = 0;
        lvgl_ui_last_key = (uint8_t)color_id;
        lvgl_ui_last_zone = 1;
        lvgl_ui_key_click_count++;

        schedule_page_change(NAV_PAGE_GRID, (uint8_t)color_id);
    }
}

static void input_btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lvgl_ui_event_count++;
        lvgl_ui_last_key = 3;
        lvgl_ui_last_zone = 3;
        lvgl_ui_key_click_count++;
        schedule_page_change(NAV_PAGE_INPUT, 0);
    }
}

static void recalc_grid_counts(void)
{
    uint8_t i;

    lvgl_ui_r1_count = 0;
    lvgl_ui_r2_count = 0;
    lvgl_ui_fake_count = 0;

    for(i = 1; i <= 12U; i++) {
        if(lvgl_ui_square_values[i] == 1) lvgl_ui_r1_count++;
        else if(lvgl_ui_square_values[i] == 2) lvgl_ui_r2_count++;
        else if(lvgl_ui_square_values[i] == -1) lvgl_ui_fake_count++;
    }

    lvgl_ui_total_marked_count = lvgl_ui_r1_count + lvgl_ui_r2_count + lvgl_ui_fake_count;
}

static uint8_t grid_value_can_add(uint8_t square_id, int8_t value)
{
    if(lvgl_ui_square_values[square_id] == value) return 1U;
    if(value == 1 && lvgl_ui_r1_count >= GRID_R1_REQUIRED) return 0U;
    if(value == 2 && lvgl_ui_r2_count >= GRID_R2_REQUIRED) return 0U;
    if(value == -1 && lvgl_ui_fake_count >= GRID_FAKE_REQUIRED) return 0U;

    return 1U;
}

static void set_grid_value(uint8_t square_id, int8_t value)
{
    if(square_id == 0U || square_id > 12U) return;
    if((lvgl_ui_selected_color == 1U || lvgl_ui_selected_color == 2U) &&
       lvgl_ui_zone_confirmed[lvgl_ui_selected_color] != 0U) {
        update_grid_status("LOCKED");
        return;
    }

    if(lvgl_ui_square_values[square_id] == value) {
        lvgl_ui_square_values[square_id] = 0;
    } else if(value == -1 && square_id <= 3U) {
        update_grid_status("fake NO 1-3");
        return;
    } else if(grid_value_can_add(square_id, value) == 0U) {
        if(value == 1) update_grid_status("R1 MAX 3");
        else if(value == 2) update_grid_status("R2 MAX 4");
        else update_grid_status("fake MAX 1");
        return;
    } else if(lvgl_ui_square_values[square_id] != 0 || lvgl_ui_total_marked_count < GRID_VALUE_MAX) {
        lvgl_ui_square_values[square_id] = value;
    } else {
        update_grid_status("MAX 8");
        return;
    }

    if(lvgl_ui_selected_color == 1U || lvgl_ui_selected_color == 2U) {
        lvgl_ui_zone_square_values[lvgl_ui_selected_color][square_id] =
            lvgl_ui_square_values[square_id];
        lvgl_ui_zone_confirmed[lvgl_ui_selected_color] = 0U;
    }

    recalc_grid_counts();
    refresh_grid_square(square_id);
    refresh_state_label(square_id);
    update_grid_status(NULL);
}

static void grid_btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        uint32_t square_id = (uint32_t)(uintptr_t)lv_event_get_user_data(e);

        lvgl_ui_event_count++;
        lvgl_ui_grid_event_count++;
        lvgl_ui_selected_square = (uint8_t)square_id;
        lvgl_ui_last_key = (uint8_t)square_id;
        lvgl_ui_last_zone = (uint8_t)lvgl_ui_selected_mode;
        lvgl_ui_key_click_count++;

        set_grid_value((uint8_t)square_id, lvgl_ui_selected_mode);
    }
}

static void mode_btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        int32_t mode = (int32_t)(intptr_t)lv_event_get_user_data(e);

        lvgl_ui_event_count++;
        lvgl_ui_mode_event_count++;
        if((lvgl_ui_selected_color == 1U || lvgl_ui_selected_color == 2U) &&
           lvgl_ui_zone_confirmed[lvgl_ui_selected_color] != 0U) {
            update_grid_status("LOCKED");
            return;
        }
        lvgl_ui_selected_mode = (int8_t)mode;
        refresh_mode_buttons();
        update_grid_status(NULL);
    }
}

static void confirm_btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lvgl_ui_event_count++;
        lvgl_ui_confirm_event_count++;
        recalc_grid_counts();
        lvgl_ui_confirm_ok = (lvgl_ui_r1_count == GRID_R1_REQUIRED &&
                              lvgl_ui_r2_count == GRID_R2_REQUIRED &&
                              lvgl_ui_fake_count == GRID_FAKE_REQUIRED) ? 1U : 0U;
        if(lvgl_ui_selected_color == 1U || lvgl_ui_selected_color == 2U) {
            if(lvgl_ui_confirm_ok != 0U) {
                lvgl_ui_zone_confirmed[lvgl_ui_selected_color] = 1U;
            }
        }
        update_grid_status(lvgl_ui_confirm_ok ? "LOCKED" : "NEED 3/4/1");
    }
}

static void back_btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lvgl_ui_event_count++;
        lvgl_ui_back_event_count++;
        schedule_page_change(NAV_PAGE_HOME, 0);
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
    lv_obj_add_event_cb(btn, color_btn_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)color_id);
    lvgl_ui_sub_stage = 103U;

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

static void create_input_button(lv_obj_t * parent)
{
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_t * label;

    lvgl_ui_last_obj = (uintptr_t)btn;
    style_color_button(btn, 3);
    lv_obj_set_pos(btn, INPUT_BUTTON_X, COLOR_BUTTON_Y);
    lv_obj_add_event_cb(btn, input_btn_event_cb, LV_EVENT_CLICKED, NULL);

    label = lv_label_create(btn);
    lvgl_ui_last_obj = (uintptr_t)label;
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_label_set_text(label, "INPUT");
    lv_obj_center(label);
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
    lv_obj_t * btn = lv_btn_create(parent);

    (void)color_id;
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, GRID_CELL_SIZE, GRID_CELL_SIZE);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_style_shadow_width(btn, 0, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(btn, grid_btn_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)square_id);
    grid_square_btns[square_id] = btn;
    refresh_grid_square(square_id);

    lv_obj_t * label = lv_label_create(btn);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    square_to_text(square_id, label_text);
    lv_label_set_text(label, label_text);
    lv_obj_center(label);
}

static void refresh_grid_square(uint8_t square_id)
{
    lv_obj_t * btn;
    lv_color_t bg;

    if(square_id == 0U || square_id > 12U) return;

    btn = grid_square_btns[square_id];
    if(btn == NULL) return;

    bg = value_color(lvgl_ui_square_values[square_id]);
    lv_obj_set_style_radius(btn, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, bg, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 3, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, value_border_color(lvgl_ui_square_values[square_id]), LV_STATE_DEFAULT);
    apply_press_feedback(btn, bg);
}

static void grid_value_to_text(uint8_t square_id, char * text, uint32_t len)
{
    const char * value_text = "0";

    if(lvgl_ui_square_values[square_id] == 1) value_text = "R1";
    else if(lvgl_ui_square_values[square_id] == 2) value_text = "R2";
    else if(lvgl_ui_square_values[square_id] == -1) value_text = "fake";

    lv_snprintf(text, len, "%u:%s", square_id, value_text);
}

static void refresh_state_label(uint8_t square_id)
{
    char text[12];
    lv_obj_t * label;

    if(square_id == 0U || square_id > 12U) return;

    label = grid_state_labels[square_id];
    if(label == NULL) return;

    grid_value_to_text(square_id, text, sizeof(text));
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, value_border_color(lvgl_ui_square_values[square_id]), LV_STATE_DEFAULT);
}

static void refresh_all_state_labels(void)
{
    uint8_t i;

    for(i = 1; i <= 12U; i++) {
        refresh_state_label(i);
    }
}

static void create_state_labels(lv_obj_t * parent)
{
    uint8_t i;

    for(i = 1; i <= 12U; i++) {
        lv_obj_t * label = lv_label_create(parent);
        grid_state_labels[i] = label;
        lv_obj_remove_style_all(label);
        lv_obj_set_size(label, STATE_LABEL_W, STATE_LABEL_H);
        lv_obj_set_pos(label, STATE_LABEL_X, STATE_LABEL_Y + (i - 1U) * (STATE_LABEL_H + STATE_LABEL_GAP));
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, LV_STATE_DEFAULT);
    }

    refresh_all_state_labels();
}

static void style_side_button(lv_obj_t * btn, const char * text, int32_t x, int32_t y, int8_t value)
{
    lv_color_t bg = value == 0 ? lv_color_hex(0x30343A) : value_color(value);
    lv_obj_t * label;

    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, SIDE_BTN_W, SIDE_BTN_H);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_style_radius(btn, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, bg, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, lvgl_ui_selected_mode == value ? 7 : 3, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, lvgl_ui_selected_mode == value ? lv_color_black() : lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn, 0, LV_STATE_DEFAULT);
    apply_press_feedback(btn, bg);

    label = lv_label_create(btn);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_label_set_text(label, text);
    lv_obj_center(label);
}

static void create_side_buttons(lv_obj_t * parent)
{
    mode_btn_r1 = lv_btn_create(parent);
    style_side_button(mode_btn_r1, "R1", SIDE_BTN_X, SIDE_BTN_Y, 1);
    lv_obj_add_event_cb(mode_btn_r1, mode_btn_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)1);

    mode_btn_r2 = lv_btn_create(parent);
    style_side_button(mode_btn_r2, "R2", SIDE_BTN_X, SIDE_BTN_Y + (SIDE_BTN_H + SIDE_BTN_GAP), 2);
    lv_obj_add_event_cb(mode_btn_r2, mode_btn_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)2);

    mode_btn_fake = lv_btn_create(parent);
    style_side_button(mode_btn_fake, "fake", SIDE_BTN_X, SIDE_BTN_Y + 2 * (SIDE_BTN_H + SIDE_BTN_GAP), -1);
    lv_obj_add_event_cb(mode_btn_fake, mode_btn_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)-1);

    confirm_btn = lv_btn_create(parent);
    style_side_button(confirm_btn, "OK", SIDE_BTN_X, SIDE_BTN_Y + 3 * (SIDE_BTN_H + SIDE_BTN_GAP), 0);
    lv_obj_add_event_cb(confirm_btn, confirm_btn_event_cb, LV_EVENT_CLICKED, NULL);
}

static void refresh_mode_button(lv_obj_t * btn, int8_t value)
{
    if(btn == NULL) return;

    lv_obj_set_style_border_width(btn, lvgl_ui_selected_mode == value ? 7 : 3, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn, lvgl_ui_selected_mode == value ? lv_color_black() : lv_color_white(), LV_STATE_DEFAULT);
}

static void refresh_mode_buttons(void)
{
    refresh_mode_button(mode_btn_r1, 1);
    refresh_mode_button(mode_btn_r2, 2);
    refresh_mode_button(mode_btn_fake, -1);
}

static void update_grid_status(const char * suffix)
{
    char text[48];
    const char * mode = "R1";

    if(lvgl_ui_selected_mode == 2) mode = "R2";
    else if(lvgl_ui_selected_mode == -1) mode = "fake";

    if(suffix == NULL) {
        text[0] = '\0';
    } else {
        lv_snprintf(text, sizeof(text), "%s", suffix);
    }

    if(grid_status_label != NULL) {
        lv_label_set_text(grid_status_label, text);
    }
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
    lv_obj_add_event_cb(btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_label_set_text(label, "<");
    lv_obj_center(label);
}

static void set_input_label(uint8_t index, const char * text)
{
    if(index >= INPUT_LABEL_COUNT || input_value_labels[index] == NULL) return;

    lv_label_set_text(input_value_labels[index], text);
}

static void set_input_key_box(uint8_t index, uint8_t pressed)
{
    lv_obj_t * box;
    lv_color_t bg;
    lv_color_t border;

    if(index >= INPUT_KEY_COUNT) return;

    box = input_key_boxes[index];
    if(box == NULL) return;

    bg = pressed != 0U ? lv_color_hex(0x24A148) : lv_color_hex(0x30343A);
    border = pressed != 0U ? lv_color_hex(0x12632A) : lv_color_hex(0x7B8088);
    lv_obj_set_style_bg_color(box, bg, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(box, border, LV_STATE_DEFAULT);
}

static void input_timer_cb(lv_timer_t * timer)
{
    char text[32];
    uint8_t i;

    (void)timer;
    if(lvgl_ui_page != 3U) return;

    for(i = 0; i < INPUT_KEY_COUNT; i++) {
        set_input_key_box(i, key_state[i + 1U]);
    }

    lv_snprintf(text, sizeof(text), "X1 %d", X1);
    set_input_label(0, text);
    lv_snprintf(text, sizeof(text), "Y1 %d", Y1);
    set_input_label(1, text);
    lv_snprintf(text, sizeof(text), "X2 %d", X2);
    set_input_label(2, text);
    lv_snprintf(text, sizeof(text), "Y2 %d", Y2);
    set_input_label(3, text);
    lv_snprintf(text, sizeof(text), "KEY1 %u", key_state[1]);
    set_input_label(4, text);
    lv_snprintf(text, sizeof(text), "KEY2 %u", key_state[2]);
    set_input_label(5, text);
    lv_snprintf(text, sizeof(text), "KEY3 %u", key_state[3]);
    set_input_label(6, text);
    lv_snprintf(text, sizeof(text), "KEY4 %u", key_state[4]);
    set_input_label(7, text);
    lv_snprintf(text, sizeof(text), "Q1 %u", q_state[0]);
    set_input_label(8, text);
    lv_snprintf(text, sizeof(text), "Q2 %u", q_state[1]);
    set_input_label(9, text);
    lv_snprintf(text, sizeof(text), "B1 %u", BOGAN[0]);
    set_input_label(10, text);
    lv_snprintf(text, sizeof(text), "B2 %u", BOGAN[1]);
    set_input_label(11, text);
}

static void create_input_value_label(lv_obj_t * parent, uint8_t index, int32_t x, int32_t y)
{
    lv_obj_t * label = lv_label_create(parent);

    input_value_labels[index] = label;
    lv_obj_remove_style_all(label);
    lv_obj_set_size(label, 210, 46);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(0x20242A), LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, LV_STATE_DEFAULT);
    lv_label_set_text(label, "-");
}

static void create_input_key_box(lv_obj_t * parent, uint8_t index, int32_t x, int32_t y)
{
    char text[4];
    lv_obj_t * box = lv_obj_create(parent);
    lv_obj_t * label;

    input_key_boxes[index] = box;
    lv_obj_remove_style_all(box);
    lv_obj_set_size(box, INPUT_KEY_BOX_W, INPUT_KEY_BOX_H);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_style_radius(box, 6, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(box, 3, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(box, 0, LV_STATE_DEFAULT);

    label = lv_label_create(box);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_snprintf(text, sizeof(text), "K%u", (uint32_t)(index + 1U));
    lv_label_set_text(label, text);
    lv_obj_center(label);

    set_input_key_box(index, key_state[index + 1U]);
}

static void create_input_page(void)
{
    lv_obj_t * screen;
    uint8_t i;

    lvgl_ui_page = 3;
    lvgl_ui_selected_color = 0;
    lvgl_ui_selected_square = 0;

    if(input_screen == NULL) {
        input_screen = lv_obj_create(NULL);
    }

    screen = input_screen;
    lv_obj_clean(screen);
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xF4F6F8), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_STATE_DEFAULT);

    for(i = 0; i < INPUT_LABEL_COUNT; i++) {
        input_value_labels[i] = NULL;
    }
    for(i = 0; i < INPUT_KEY_COUNT; i++) {
        input_key_boxes[i] = NULL;
    }

    create_back_button(screen);

    lv_obj_t * title = lv_label_create(screen);
    lv_obj_remove_style_all(title);
    lv_obj_set_pos(title, 128, 42);
    lv_obj_set_style_text_color(title, lv_color_hex(0x20242A), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_label_set_text(title, "INPUT");

    for(i = 0; i < INPUT_KEY_COUNT; i++) {
        create_input_key_box(screen, i,
                             INPUT_KEY_BOX_X + i * (INPUT_KEY_BOX_W + INPUT_KEY_BOX_GAP),
                             INPUT_KEY_BOX_Y);
    }

    for(i = 0; i < 4U; i++) {
        create_input_value_label(screen, i, 130 + (i % 2U) * 270, 140 + (i / 2U) * 76);
    }

    for(i = 0; i < 4U; i++) {
        create_input_value_label(screen, (uint8_t)(4U + i), 130 + (i % 2U) * 270, 330 + (i / 2U) * 76);
    }

    create_input_value_label(screen, 8, 690, 140);
    create_input_value_label(screen, 9, 690, 216);
    create_input_value_label(screen, 10, 690, 330);
    create_input_value_label(screen, 11, 690, 406);

    input_timer_cb(NULL);
    if(input_timer == NULL) {
        input_timer = lv_timer_create(input_timer_cb, 100, NULL);
    } else {
        lv_timer_resume(input_timer);
    }

    lv_scr_load(screen);
    lv_indev_reset(NULL, NULL);
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
    create_input_button(screen);

    lvgl_ui_sub_stage = 23U;
    lv_scr_load(screen);
    lv_indev_reset(NULL, NULL);
    lvgl_ui_sub_stage = 24U;
    lvgl_ui_stage = 29U;
}

static void create_grid_page(uint8_t color_id)
{
    lv_obj_t * screen;
    uint8_t square_id = 1;
    uint8_t i;

    lvgl_ui_page = 2;
    lvgl_ui_selected_color = color_id;
    lvgl_ui_selected_mode = 1;
    lvgl_ui_confirm_ok = 0;
    for(i = 1; i <= 12U; i++) {
        lvgl_ui_square_values[i] = lvgl_ui_zone_square_values[color_id][i];
        grid_square_btns[i] = NULL;
        grid_state_labels[i] = NULL;
    }
    recalc_grid_counts();
    lvgl_ui_confirm_ok = lvgl_ui_zone_confirmed[color_id];

    if(grid_screen == NULL) {
        grid_screen = lv_obj_create(NULL);
    }

    screen = grid_screen;
    lv_obj_clean(screen);
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, SCREEN_W, SCREEN_H);
    lv_obj_set_style_bg_color(screen, color_page_bg(color_id), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_STATE_DEFAULT);

    create_back_button(screen);
    create_state_labels(screen);
    create_side_buttons(screen);
    grid_status_label = lv_label_create(screen);
    lv_obj_remove_style_all(grid_status_label);
    lv_obj_set_width(grid_status_label, SIDE_BTN_W);
    lv_obj_set_pos(grid_status_label, SIDE_BTN_X, STATUS_LABEL_Y);
    lv_obj_set_style_text_align(grid_status_label, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(grid_status_label, lv_color_hex(0x30343A), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(grid_status_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    update_grid_status(lvgl_ui_zone_confirmed[color_id] != 0U ? "LOCKED" : NULL);

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
    lv_indev_reset(NULL, NULL);
}

void lvgl_ui_create(void)
{
    lvgl_ui_stage = 1U;
    lvgl_ui_sub_stage = 1U;
    lvgl_ui_create_count++;
    home_screen = NULL;
    grid_screen = NULL;
    input_screen = NULL;
    input_timer = NULL;
    lvgl_ui_sub_stage = 2U;
    create_home_page();
    lvgl_ui_stage = 99U;
    lvgl_ui_sub_stage = 99U;
}

