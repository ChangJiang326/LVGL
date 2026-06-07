#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl_ui.h"
#include <stdint.h>

volatile uint32_t lvgl_task_loop_count = 0;
volatile uint32_t lvgl_task_handler_count = 0;
volatile uint32_t lvgl_mem_free_size = 0;
volatile uint32_t lvgl_mem_free_biggest_size = 0;
volatile uint32_t lvgl_mem_max_used = 0;
volatile uint8_t lvgl_mem_used_pct = 0;
volatile uint32_t lvgl_task_stack_free = 0;
volatile uint32_t lvgl_task_stack_free_min = 0xFFFFFFFFU;

void lvgl_task(void const * argument)
{
  /* USER CODE BEGIN lvgl_task */
  (void)argument;

	lv_init();
  lv_port_disp_init();
	 lv_port_indev_init();
  lvgl_ui_create();

  for(;;)
  {
    lvgl_task_loop_count++;
    lv_tick_inc(1);
    lv_timer_handler();
    lvgl_task_handler_count++;
    lvgl_task_stack_free = uxTaskGetStackHighWaterMark(NULL);
    if(lvgl_task_stack_free < lvgl_task_stack_free_min) {
      lvgl_task_stack_free_min = lvgl_task_stack_free;
    }
    if((lvgl_task_loop_count % 100U) == 0U) {
      lv_mem_monitor_t mon;
      lv_mem_monitor(&mon);
      lvgl_mem_free_size = mon.free_size;
      lvgl_mem_free_biggest_size = mon.free_biggest_size;
      lvgl_mem_max_used = mon.max_used;
      lvgl_mem_used_pct = mon.used_pct;
    }
    osDelay(1);
  }
  /* USER CODE END lvgl_task */
}

uint32_t __clz(uint32_t value)
{
  uint32_t count = 0;
  uint32_t mask = 0x80000000U;

  if(value == 0U) {
    return 32U;
  }

  while((value & mask) == 0U) {
    count++;
    mask >>= 1;
  }

  return count;
}

void EXTI_Init(void)
{
}
