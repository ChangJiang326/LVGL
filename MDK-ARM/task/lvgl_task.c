#include "cmsis_os.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl_ui.h"
#include <stdint.h>


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
    lv_tick_inc(1);
    lv_timer_handler();
    osDelay(1);
  }
  /* USER CODE END lvgl_task */
}

uint32_t __get_IPSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, ipsr" : "=r" (result));
  return result;
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
