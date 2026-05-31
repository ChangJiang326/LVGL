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
#include "gui.h"
#include "delay.h"

volatile uint32_t lv_port_disp_stage = 0;
volatile uint32_t lv_port_flush_stage = 0;
volatile uint32_t lv_port_flush_substage = 0;
volatile uint32_t lv_port_flush_count = 0;
volatile uint32_t lv_port_flush_last_w = 0;
volatile uint32_t lv_port_flush_last_h = 0;
volatile uint32_t lv_port_flush_pixels = 0;
volatile uint32_t lv_port_flush_last_color = 0;
volatile uint32_t lv_port_flush_status = 0;
volatile uint32_t lv_port_flush_fifo_timeout = 0;
volatile uint32_t lv_port_flush_x = 0;
volatile uint32_t lv_port_flush_y = 0;
volatile uint32_t lv_port_disp_clear_stage = 0;
volatile uint32_t lv_port_disp_clear_pixels = 0;

#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    1024
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    600
#endif

static bool lcd_wait_mem_wr_fifo_not_full(void)
{
    uint32_t retry = 100000U;
    uint8_t status;

    do {
        status = LCD_StatusRead();
        lv_port_flush_status = status;
        if((status & 0x80U) == 0U) {
            return true;
        }
    } while(--retry != 0U);

    lv_port_flush_fifo_timeout++;
    return false;
}

static void lcd_config_lvgl_canvas(void)
{
    Memory_Select_SDRAM();
    Canvas_Image_Start_address(Layer0);
    Canvas_image_width(MY_DISP_HOR_RES);
    Main_Image_Start_Address(Layer0);
    Main_Image_Width(MY_DISP_HOR_RES);
    Main_Window_Start_XY(0, 0);
    Active_Window_XY(0, 0);
    Active_Window_WH(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    Select_Write_Data_Position();
    Memory_XY_Mode();
    Memory_16bpp_Mode();
    Select_Main_Window_16bpp();
    RGB_16b_16bpp();
    MemWrite_Left_Right_Top_Down();
}

static void lcd_clear_lvgl_canvas(uint16_t color)
{
    uint32_t i;
    uint32_t total = (uint32_t)MY_DISP_HOR_RES * (uint32_t)MY_DISP_VER_RES;

    lv_port_disp_clear_stage = 1;
    lv_port_disp_clear_pixels = 0;
    lcd_config_lvgl_canvas();
    Goto_Pixel_XY(0, 0);
    LCD_CmdWrite(0x04);

    for(i = 0; i < total; i++) {
        if(lcd_wait_mem_wr_fifo_not_full()) {
            LCD_DataWrite_FSMC(color);
            lv_port_disp_clear_pixels++;
        } else {
            break;
        }
    }

    lv_port_disp_clear_stage = 2;
}
/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    1024
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    600
#endif

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
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    /* Example for 2) */
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
    lv_port_disp_stage = 1;
    LCD_Init();
    lv_port_disp_stage = 2;
    Display_OFF();
    lcd_clear_lvgl_canvas(0x0000U);
    lv_port_disp_stage = 3;
    Display_ON();
    lv_port_disp_stage = 4;
    Set_Backlight(100);
    lv_port_disp_stage = 5;
}

volatile bool disp_flush_enabled = true;

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
    lv_port_flush_stage = 1;
    lv_port_flush_count++;

    if(disp_flush_enabled) {
        int32_t x1 = area->x1 < 0 ? 0 : area->x1;
        int32_t y1 = area->y1 < 0 ? 0 : area->y1;
        int32_t x2 = area->x2 >= MY_DISP_HOR_RES ? MY_DISP_HOR_RES - 1 : area->x2;
        int32_t y2 = area->y2 >= MY_DISP_VER_RES ? MY_DISP_VER_RES - 1 : area->y2;

        if(x1 <= x2 && y1 <= y2) {
            int32_t x;
            int32_t y;
            int32_t area_w = area->x2 - area->x1 + 1;
            int32_t flush_w = x2 - x1 + 1;
            int32_t flush_h = y2 - y1 + 1;

            lv_port_flush_last_w = (uint32_t)flush_w;
            lv_port_flush_last_h = (uint32_t)flush_h;

            lv_port_flush_stage = 3;
            lv_port_flush_pixels = 0;
            lcd_config_lvgl_canvas();
            color_p += (y1 - area->y1) * area_w + (x1 - area->x1);
            for(y = y1; y <= y2; y++) {
                lv_color_t * row_p = color_p;

                lv_port_flush_stage = 2;
                lv_port_flush_y = (uint32_t)y;
                Active_Window_XY((uint)x1, (uint)y);
                Active_Window_WH((uint)flush_w, 1);
                Goto_Pixel_XY((uint)x1, (uint)y);
                LCD_CmdWrite(0x04);

                lv_port_flush_stage = 3;
                for(x = x1; x <= x2; x++) {
                    lv_port_flush_x = (uint32_t)x;
                    lv_port_flush_last_color = row_p->full;
                    lv_port_flush_substage = 1;
                    if((lv_port_flush_pixels & 0x0FU) == 0U) {
                        delay_us(2);
                    }
                    lv_port_flush_substage = 2;
                    if(lcd_wait_mem_wr_fifo_not_full()) {
                        LCD_DataWrite_FSMC(row_p->full);
                    } else {
                        break;
                    }
                    lv_port_flush_substage = 3;
                    lv_port_flush_pixels++;
                    row_p++;
                }

                color_p += area_w;
            }

            lv_port_flush_stage = 4;
            Active_Window_XY(0, 0);
            Active_Window_WH(MY_DISP_HOR_RES, MY_DISP_VER_RES);
        }
    }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
    lv_port_flush_stage = 5;
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
