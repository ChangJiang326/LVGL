#ifndef __DELAY_H
#define __DELAY_H

#include "main.h"

static __inline void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

static __inline void delay_us(uint32_t us)
{
    uint32_t cycles = (SystemCoreClock / 1000000U / 5U) * us;

    while(cycles-- != 0U) {
        __NOP();
    }
}

#endif
