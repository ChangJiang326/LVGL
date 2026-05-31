
/**
 * @file lv_port_indev_templ.h
 *
 */

/*Copy this file as "lv_port_indev.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_INDEV_TEMPL_H
#define LV_PORT_INDEV_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_port_indev_init(void);

extern volatile uint8_t lv_port_touch_pressed;
extern volatile uint16_t lv_port_touch_raw_x;
extern volatile uint16_t lv_port_touch_raw_y;
extern volatile uint16_t lv_port_touch_x;
extern volatile uint16_t lv_port_touch_y;
extern volatile uint32_t lv_port_touch_read_count;
extern volatile uint32_t lv_port_touch_press_count;
extern volatile uint32_t lv_port_touch_release_count;

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_INDEV_TEMPL_H*/

#endif /*Disable/Enable content*/
