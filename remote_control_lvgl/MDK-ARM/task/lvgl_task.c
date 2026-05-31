#include "cmsis_os.h"
#include "lcd.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl_ui.h"
#include "ctp.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

extern volatile unsigned char LCD_PWM_Reg85;
extern volatile unsigned char LCD_PWM_Reg86;
extern volatile unsigned char LCD_PWM_Reg88;
extern volatile unsigned char LCD_PWM_Reg89;
extern volatile unsigned char LCD_PWM_Reg8A;
extern volatile unsigned char LCD_PWM_Reg8B;
extern volatile unsigned char LCD_PWM_Reg8C;
extern volatile unsigned char LCD_PWM_Reg8D;
extern volatile unsigned char LCD_PWM_Reg8E;
extern volatile unsigned char LCD_PWM_Reg8F;
extern volatile unsigned char LCD_Reg10_Debug;
extern volatile unsigned char LCD_Reg12_Debug;
extern volatile unsigned char LCD_Reg13_Debug;
extern volatile unsigned char LCD_RegF3_Debug;
extern volatile unsigned char LCD_RegF4_Debug;

volatile unsigned int lvgl_task_stage = 0;
volatile unsigned int codex_build_magic = 0x20260529U;
volatile unsigned int codex_runtime_magic = 0;
volatile unsigned int lvgl_task_timer_enter_count = 0;
volatile unsigned int lvgl_task_timer_exit_count = 0;
volatile unsigned int lvgl_task_refr_enter_count = 0;
volatile unsigned int lvgl_task_refr_exit_count = 0;
volatile unsigned int lvgl_task_loop_count = 0;
volatile unsigned int lvgl_task_stack_watermark = 0;
volatile unsigned int lvgl_task_heap_free = 0;
volatile unsigned int lvgl_task_stack_before_timer = 0;
volatile unsigned int lvgl_task_stack_after_timer = 0;
volatile unsigned int lvgl_task_heap_before_timer = 0;
volatile unsigned int lvgl_task_heap_after_timer = 0;

static void LCD_WriteRegDirect(unsigned short reg, unsigned short value)
{
	LCD_CmdWrite(reg);
	LCD_DataWrite(value);
}

static void LCD_BacklightOnDirect(void)
{
	const unsigned short pwm_count = 6000U;

	LCD_WriteRegDirect(0xF3, 0x80);
	LCD_WriteRegDirect(0xF4, 0x80);
	LCD_WriteRegDirect(0x84, 0x00);
	LCD_WriteRegDirect(0x85, 0x0A);

	LCD_CmdWrite(0x8A);
	LCD_DataWrite(pwm_count & 0xFF);
	LCD_CmdWrite(0x8B);
	LCD_DataWrite((pwm_count >> 8) & 0xFF);
	LCD_CmdWrite(0x88);
	LCD_DataWrite(pwm_count & 0xFF);
	LCD_CmdWrite(0x89);
	LCD_DataWrite((pwm_count >> 8) & 0xFF);

	LCD_CmdWrite(0x8E);
	LCD_DataWrite(pwm_count & 0xFF);
	LCD_CmdWrite(0x8F);
	LCD_DataWrite((pwm_count >> 8) & 0xFF);
	LCD_CmdWrite(0x8C);
	LCD_DataWrite(pwm_count & 0xFF);
	LCD_CmdWrite(0x8D);
	LCD_DataWrite((pwm_count >> 8) & 0xFF);

	LCD_WriteRegDirect(0x86, 0x33);

	LCD_PWM_Reg85 = LCD_Reg_Read(0x85);
	LCD_PWM_Reg86 = LCD_Reg_Read(0x86);
	LCD_PWM_Reg88 = LCD_Reg_Read(0x88);
	LCD_PWM_Reg89 = LCD_Reg_Read(0x89);
	LCD_PWM_Reg8A = LCD_Reg_Read(0x8A);
	LCD_PWM_Reg8B = LCD_Reg_Read(0x8B);
	LCD_PWM_Reg8C = LCD_Reg_Read(0x8C);
	LCD_PWM_Reg8D = LCD_Reg_Read(0x8D);
	LCD_PWM_Reg8E = LCD_Reg_Read(0x8E);
	LCD_PWM_Reg8F = LCD_Reg_Read(0x8F);
	LCD_Reg10_Debug = LCD_Reg_Read(0x10);
	LCD_Reg12_Debug = LCD_Reg_Read(0x12);
	LCD_Reg13_Debug = LCD_Reg_Read(0x13);
	LCD_RegF3_Debug = LCD_Reg_Read(0xF3);
	LCD_RegF4_Debug = LCD_Reg_Read(0xF4);
}

static void LCD_ApplyPanelNormalMode(void)
{
	LCD_WriteRegDirect(0x02, 0x40);
	LCD_WriteRegDirect(0x03, 0x00);
	LCD_WriteRegDirect(0x5E, 0x01);
	LCD_WriteRegDirect(0x10, 0x04);
	LCD_WriteRegDirect(0x13, 0xC0);
	LCD_WriteRegDirect(0x12, 0xC0);
}

static void LCD_ApplyTiming(void)
{
	LCD_HorizontalWidth_VerticalHeight(lcdx, lcdy);
	LCD_Horizontal_Non_Display(LCD_HBPD);
	LCD_HSYNC_Start_Position(LCD_HFPD);
	LCD_HSYNC_Pulse_Width(LCD_HSPW);
	LCD_Vertical_Non_Display(LCD_VBPD);
	LCD_VSYNC_Start_Position(LCD_VFPD);
	LCD_VSYNC_Pulse_Width(LCD_VSPW);
	Display_ON();
	LCD_ApplyPanelNormalMode();
}

static void LCD_FillWhite(void)
{
	Foreground_color_16M(0xFFFFFFUL);
	Line_Start_XY(0, 0);
	Line_End_XY(lcdx - 1, lcdy - 1);
	Start_Square_Fill();
}

void lvgl_task(void const *argument)
{

	codex_runtime_magic = 0x13572468U;
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk;
	lvgl_task_stack_watermark = (unsigned int)uxTaskGetStackHighWaterMark(NULL);
	lvgl_task_heap_free = (unsigned int)xPortGetFreeHeapSize();
	lvgl_task_stage = 1;
	CTP_Prepare_Reset();
	lvgl_task_stage = 2;
 	LCD_Init();
	lvgl_task_stage = 3;
	Display_ON();
	LCD_BacklightOnDirect();
	Color_Bar_OFF();
	LCD_ApplyPanelNormalMode();
	LCD_ApplyTiming();
	LCD_BacklightOnDirect();
	lvgl_task_stage = 4;
	LCD_FillWhite();
	lvgl_task_stage = 5;
	lv_init();
	lvgl_task_stage = 6;
	lv_port_disp_init();
	lvgl_task_stage = 7;
	lvgl_ui_create();
	lvgl_task_stage = 8;
	lv_tick_inc(50);
	lvgl_task_stack_before_timer = (unsigned int)uxTaskGetStackHighWaterMark(NULL);
	lvgl_task_heap_before_timer = (unsigned int)xPortGetFreeHeapSize();
	lvgl_task_timer_enter_count++;
	lvgl_task_stage = 81;
	lv_timer_handler();
	lvgl_task_stack_after_timer = (unsigned int)uxTaskGetStackHighWaterMark(NULL);
	lvgl_task_heap_after_timer = (unsigned int)xPortGetFreeHeapSize();
	lvgl_task_timer_exit_count++;
	lvgl_task_stage = 82;
	lvgl_task_refr_enter_count++;
	lv_refr_now(NULL);
	lvgl_task_refr_exit_count++;
	lvgl_task_stage = 9;
	lv_port_indev_init();
	lvgl_task_stage = 10;

	for(;;)
	{
		lvgl_task_loop_count++;
		lvgl_task_stage = 11;
		lv_tick_inc(2);
		lvgl_task_stack_before_timer = (unsigned int)uxTaskGetStackHighWaterMark(NULL);
		lvgl_task_heap_before_timer = (unsigned int)xPortGetFreeHeapSize();
		lvgl_task_timer_enter_count++;
		lvgl_task_stage = 111;
		lv_timer_handler();
		lvgl_task_stack_after_timer = (unsigned int)uxTaskGetStackHighWaterMark(NULL);
		lvgl_task_heap_after_timer = (unsigned int)xPortGetFreeHeapSize();
		lvgl_task_timer_exit_count++;
		lvgl_task_stage = 112;
		osDelay(2);
	}
}
