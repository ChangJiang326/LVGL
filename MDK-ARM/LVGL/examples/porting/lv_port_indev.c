/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "lvgl.h"
#include "ctp.h"
/*********************
 *      DEFINES
 *********************/
#define TOUCH_HOR_RES      1024
#define TOUCH_VER_RES      600
#define TOUCH_SWAP_XY      0
#define TOUCH_INVERT_X     0
#define TOUCH_INVERT_Y     0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

static void mouse_init(void);
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void encoder_handler(void);

static void button_init(void);
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
lv_indev_t * indev_button;
static lv_obj_t * touch_cursor;

volatile uint8_t lv_port_touch_pressed = 0;
volatile uint16_t lv_port_touch_raw_x = 0;
volatile uint16_t lv_port_touch_raw_y = 0;
volatile uint16_t lv_port_touch_x = 0;
volatile uint16_t lv_port_touch_y = 0;
volatile uint32_t lv_port_touch_read_count = 0;
volatile uint32_t lv_port_touch_press_count = 0;
volatile uint32_t lv_port_touch_release_count = 0;

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);

    touch_cursor = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(touch_cursor);
    lv_obj_set_size(touch_cursor, 18, 18);
    lv_obj_set_style_radius(touch_cursor, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(touch_cursor, lv_color_hex(0x00FF00), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(touch_cursor, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(touch_cursor, 2, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(touch_cursor, lv_color_black(), LV_STATE_DEFAULT);
    lv_obj_add_flag(touch_cursor, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(touch_cursor, LV_OBJ_FLAG_CLICKABLE);
    lv_indev_set_cursor(indev_touchpad, touch_cursor);

//    /*------------------
//     * Mouse
//     * -----------------*/

//    /*Initialize your mouse if you have*/
//    mouse_init();

//    /*Register a mouse input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_POINTER;
//    indev_drv.read_cb = mouse_read;
//    indev_mouse = lv_indev_drv_register(&indev_drv);

//    /*Set cursor. For simplicity set a HOME symbol now.*/
//    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
//    lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
//    lv_indev_set_cursor(indev_mouse, mouse_cursor);

//    /*------------------
//     * Keypad
//     * -----------------*/

//    /*Initialize your keypad or keyboard if you have*/
//    keypad_init();

//    /*Register a keypad input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
//    indev_drv.read_cb = keypad_read;
//    indev_keypad = lv_indev_drv_register(&indev_drv);

//    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
//     *add objects to the group with `lv_group_add_obj(group, obj)`
//     *and assign this input device to group to navigate in it:
//     *`lv_indev_set_group(indev_keypad, group);`*/

//    /*------------------
//     * Encoder
//     * -----------------*/

//    /*Initialize your encoder if you have*/
//    encoder_init();

//    /*Register a encoder input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_ENCODER;
//    indev_drv.read_cb = encoder_read;
//    indev_encoder = lv_indev_drv_register(&indev_drv);

//    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
//     *add objects to the group with `lv_group_add_obj(group, obj)`
//     *and assign this input device to group to navigate in it:
//     *`lv_indev_set_group(indev_encoder, group);`*/

//    /*------------------
//     * Button
//     * -----------------*/

//    /*Initialize your button if you have*/
//    button_init();

//    /*Register a button input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_BUTTON;
//    indev_drv.read_cb = button_read;
//    indev_button = lv_indev_drv_register(&indev_drv);

//    /*Assign buttons to points on the screen*/
//    static const lv_point_t btn_points[2] = {
//        {10, 10},   /*Button 0 -> x:10; y:10*/
//        {40, 100},  /*Button 1 -> x:40; y:100*/
//    };
//    lv_indev_set_button_points(indev_button, btn_points);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    CTP_Init();
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    bool pressed;

    (void)indev_drv;
    lv_port_touch_read_count++;

    /*Save the pressed coordinates and the state*/
    pressed = touchpad_is_pressed();
    if(pressed) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
        lv_port_touch_press_count++;
        if(touch_cursor != NULL) {
            lv_obj_clear_flag(touch_cursor, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else {
        lv_port_touch_pressed = 0;
        data->state = LV_INDEV_STATE_REL;
        lv_port_touch_release_count++;
        if(touch_cursor != NULL) {
            lv_obj_add_flag(touch_cursor, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;
}

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    return Readtouch_CTP() != 0;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    int32_t tx = (int32_t)X0;
    int32_t ty = (int32_t)Y0;

    lv_port_touch_raw_x = X0;
    lv_port_touch_raw_y = Y0;

#if TOUCH_SWAP_XY
    {
        int32_t tmp = tx;
        tx = ty;
        ty = tmp;
    }
#endif

#if TOUCH_INVERT_X
    tx = (TOUCH_HOR_RES - 1) - tx;
#endif

#if TOUCH_INVERT_Y
    ty = (TOUCH_VER_RES - 1) - ty;
#endif

    if(tx < 0) {
        tx = 0;
    } else if(tx >= TOUCH_HOR_RES) {
        tx = TOUCH_HOR_RES - 1;
    }

    if(ty < 0) {
        ty = 0;
    } else if(ty >= TOUCH_VER_RES) {
        ty = TOUCH_VER_RES - 1;
    }

    lv_port_touch_x = (uint16_t)tx;
    lv_port_touch_y = (uint16_t)ty;
    lv_port_touch_pressed = 1;

    (*x) = (lv_coord_t)tx;
    (*y) = (lv_coord_t)ty;
}

/*------------------
 * Mouse
 * -----------------*/

/*Initialize your mouse*/
static void mouse_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the mouse*/
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the mouse button is pressed or released*/
    if(mouse_is_pressed()) {
        data->state = LV_INDEV_STATE_PR;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }
}

/*Return true is the mouse button is pressed*/
static bool mouse_is_pressed(void)
{
    /*Your code comes here*/

    return false;
}

/*Get the x and y coordinates if the mouse is pressed*/
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}

/*------------------
 * Keypad
 * -----------------*/

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
            case 1:
                act_key = LV_KEY_NEXT;
                break;
            case 2:
                act_key = LV_KEY_PREV;
                break;
            case 3:
                act_key = LV_KEY_LEFT;
                break;
            case 4:
                act_key = LV_KEY_RIGHT;
                break;
            case 5:
                act_key = LV_KEY_ENTER;
                break;
        }

        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/

    return 0;
}

/*------------------
 * Encoder
 * -----------------*/

/*Initialize your keypad*/
static void encoder_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    data->enc_diff = encoder_diff;
    data->state = encoder_state;
}

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void)
{
    /*Your code comes here*/

    encoder_diff += 0;
    encoder_state = LV_INDEV_STATE_REL;
}

/*------------------
 * Button
 * -----------------*/

/*Initialize your buttons*/
static void button_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for(i = 0; i < 2; i++) {
        /*Return the pressed button's ID*/
        if(button_is_pressed(i)) {
            return i;
        }
    }

    /*No button pressed*/
    return -1;
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

    /*Your code comes here*/

    return false;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
