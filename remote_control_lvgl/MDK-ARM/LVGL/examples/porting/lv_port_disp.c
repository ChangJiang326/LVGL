/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "lcd.h"
#include "stm32f4xx.h"
/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    lcdx
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    lcdy
#endif

#define DISP_BUF_LINES 1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * DISP_BUF_LINES] __attribute__((aligned(8)));
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * DISP_BUF_LINES);

//    /* Example for 2) */
//    static lv_disp_draw_buf_t draw_buf_dsc_2;
//    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
//    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

//    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
//                          MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
}

volatile bool disp_flush_enabled = true;
volatile uint32_t lv_port_disp_flush_count = 0;
volatile int32_t lv_port_disp_last_x1 = 0;
volatile int32_t lv_port_disp_last_y1 = 0;
volatile int32_t lv_port_disp_last_x2 = 0;
volatile int32_t lv_port_disp_last_y2 = 0;
volatile uint32_t lv_port_disp_last_px = 0;
volatile uint32_t lv_port_disp_flush_done_count = 0;
volatile uint32_t lv_port_disp_fifo_timeout_count = 0;
volatile uint32_t lv_port_disp_stage = 0;
volatile uint32_t lv_port_disp_loop_stage = 0;
volatile uint32_t lv_port_disp_write_i = 0;
volatile uint32_t lv_port_disp_break_i = 0;
volatile uint32_t lv_port_disp_first_color = 0;
volatile uint32_t lv_port_disp_last_color = 0;
volatile uint32_t lv_port_disp_last_status = 0;
volatile uint32_t lv_port_disp_wait_spin = 0;
volatile uint32_t lv_port_disp_pre_write_i = 0;
volatile uint32_t lv_port_disp_post_write_i = 0;
volatile uint32_t lv_port_disp_color_base_addr = 0;
volatile uint32_t lv_port_disp_color_cur_addr = 0;
volatile uint32_t lv_port_disp_color_addr_lowbits = 0;
volatile uint32_t lv_port_disp_fault_probe_before_load = 0;
volatile uint32_t lv_port_disp_fault_probe_after_load = 0;
volatile uint32_t lv_port_disp_fault_probe_before_write = 0;
volatile uint32_t lv_port_disp_fault_probe_after_write = 0;
volatile uint32_t lv_port_disp_skip_status_read = 0;
volatile uint32_t lv_port_disp_fifo_wait_every = 1;
volatile uint32_t lv_port_disp_psp_enter = 0;
volatile uint32_t lv_port_disp_msp_enter = 0;
volatile uint32_t lv_port_disp_psp_stage4 = 0;
volatile uint32_t lv_port_disp_msp_stage4 = 0;
volatile uint32_t lv_port_disp_psp_before_ready = 0;
volatile uint32_t lv_port_disp_msp_before_ready = 0;
volatile uint32_t lv_port_disp_write_gap_nop = 80;
volatile uint32_t lv_port_disp_write_gap_every = 1;
volatile uint32_t lv_port_disp_burst_gap_nop = 2048;
volatile uint32_t lv_port_disp_burst_gap_every = 64;
volatile uint32_t lv_port_disp_flush_end_gap_nop = 0;
volatile uint32_t lv_port_disp_skip_lcd_write = 0;
volatile uint32_t lv_port_disp_force_color_enable = 0;
volatile uint32_t lv_port_disp_force_color = 0xFFFF;
volatile uint32_t lv_port_disp_enter_count = 0;
volatile uint32_t lv_port_disp_exit_count = 0;
volatile uint32_t lv_port_disp_last_elapsed = 0;
volatile uint32_t lv_port_disp_max_elapsed = 0;
volatile uint32_t lv_port_disp_last_start_tick = 0;
volatile uint32_t lv_port_disp_heartbeat = 0;
volatile uint32_t lv_port_disp_last_abs_x = 0;
volatile uint32_t lv_port_disp_last_abs_y = 0;
volatile uint32_t lv_port_disp_last_line_progress = 0;
volatile uint32_t lv_port_disp_max_pixels_per_flush = 0;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint32_t flush_start = lv_tick_get();

    lv_port_disp_enter_count++;
    lv_port_disp_last_start_tick = flush_start;
    lv_port_disp_psp_enter = __get_PSP();
    lv_port_disp_msp_enter = __get_MSP();
    lv_port_disp_stage = 1;
    if(disp_flush_enabled) {
        int32_t w = area->x2 - area->x1 + 1;
        int32_t h = area->y2 - area->y1 + 1;
        int32_t px_cnt = w * h;

        if(w <= 0 || h <= 0 || px_cnt <= 0) {
            lv_port_disp_stage = 9;
            lv_disp_flush_ready(disp_drv);
            return;
        }

        lv_port_disp_flush_count++;
        lv_port_disp_last_x1 = area->x1;
        lv_port_disp_last_y1 = area->y1;
        lv_port_disp_last_x2 = area->x2;
        lv_port_disp_last_y2 = area->y2;
        lv_port_disp_last_px = (uint32_t)px_cnt;
        lv_port_disp_write_i = 0;
        lv_port_disp_pre_write_i = 0;
        lv_port_disp_post_write_i = 0;
        lv_port_disp_break_i = 0;
        lv_port_disp_color_base_addr = (uint32_t)color_p;
        lv_port_disp_color_cur_addr = (uint32_t)color_p;
        lv_port_disp_color_addr_lowbits = lv_port_disp_color_cur_addr & 0x7U;
        lv_port_disp_fault_probe_before_load = 0;
        lv_port_disp_fault_probe_after_load = 0;
        lv_port_disp_fault_probe_before_write = 0;
        lv_port_disp_fault_probe_after_write = 0;
        lv_port_disp_first_color = color_p->full;
        lv_port_disp_last_color = 0;

        /* Keep the LT758 memory-write transaction continuous. Reading status
         * between pixels changes the controller bus cycle and corrupts refreshes. */
        lv_port_disp_stage = 2;
        Graphic_Mode();
        Memory_XY_Mode();
        Memory_16bpp_Mode();
        lv_port_disp_stage = 3;
        Active_Window_XY((unsigned short)area->x1, (unsigned short)area->y1);
        Active_Window_WH((unsigned short)w, (unsigned short)h);
        Goto_Pixel_XY((unsigned short)area->x1, (unsigned short)area->y1);
        LCD_CmdWrite(0x04);

        lv_port_disp_stage = 4;
        lv_port_disp_psp_stage4 = __get_PSP();
        lv_port_disp_msp_stage4 = __get_MSP();
        for(int32_t i = 0; i < px_cnt; i++) {
            uint16_t px = lv_port_disp_force_color_enable ? (uint16_t)lv_port_disp_force_color : color_p->full;

            lv_port_disp_fault_probe_before_load = (uint32_t)i;
            lv_port_disp_fault_probe_after_load = (uint32_t)i;
            lv_port_disp_pre_write_i = (uint32_t)i;
            if(!lv_port_disp_skip_lcd_write) {
                lv_port_disp_loop_stage = 42;
                lv_port_disp_fault_probe_before_write = (uint32_t)i;
                LCD_DataWrite_Fast(px);
                lv_port_disp_fault_probe_after_write = (uint32_t)i;
            }
            color_p++;
        }
        lv_port_disp_write_i = (uint32_t)px_cnt;
        lv_port_disp_post_write_i = (uint32_t)px_cnt;
        lv_port_disp_last_color = color_p[-1].full;
        lv_port_disp_heartbeat++;
        lv_port_disp_stage = 5;

        lv_port_disp_stage = 6;
        Active_Window_WH(lcdx, lcdy);
        Active_Window_XY(0, 0);
    }

    lv_port_disp_psp_before_ready = __get_PSP();
    lv_port_disp_msp_before_ready = __get_MSP();
    lv_port_disp_stage = 7;
    lv_port_disp_flush_done_count++;
    lv_disp_flush_ready(disp_drv);
    lv_port_disp_stage = 8;
    lv_port_disp_last_elapsed = lv_tick_elaps(flush_start);
    if(lv_port_disp_last_elapsed > lv_port_disp_max_elapsed) {
        lv_port_disp_max_elapsed = lv_port_disp_last_elapsed;
    }
    lv_port_disp_exit_count++;
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
