#ifndef __SYS_H
#define __SYS_H

#include "main.h"
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint8_t uchar;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint32_t ulong;
typedef volatile uint8_t vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;

#define BITBAND(addr, bitnum) ((addr & 0xF0000000U) + 0x02000000U + ((addr & 0x000FFFFFU) << 5) + ((bitnum) << 2))
#define MEM_ADDR(addr)        (*((volatile unsigned long *)(addr)))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND((addr), (bitnum)))

#define GPIOA_ODR_ADDR (GPIOA_BASE + 0x14U)
#define GPIOB_ODR_ADDR (GPIOB_BASE + 0x14U)
#define GPIOC_ODR_ADDR (GPIOC_BASE + 0x14U)
#define GPIOD_ODR_ADDR (GPIOD_BASE + 0x14U)
#define GPIOE_ODR_ADDR (GPIOE_BASE + 0x14U)
#define GPIOF_ODR_ADDR (GPIOF_BASE + 0x14U)

#define GPIOA_IDR_ADDR (GPIOA_BASE + 0x10U)
#define GPIOB_IDR_ADDR (GPIOB_BASE + 0x10U)
#define GPIOC_IDR_ADDR (GPIOC_BASE + 0x10U)
#define GPIOD_IDR_ADDR (GPIOD_BASE + 0x10U)
#define GPIOE_IDR_ADDR (GPIOE_BASE + 0x10U)
#define GPIOF_IDR_ADDR (GPIOF_BASE + 0x10U)

#define PAout(n) BIT_ADDR(GPIOA_ODR_ADDR, (n))
#define PBout(n) BIT_ADDR(GPIOB_ODR_ADDR, (n))
#define PCout(n) BIT_ADDR(GPIOC_ODR_ADDR, (n))
#define PDout(n) BIT_ADDR(GPIOD_ODR_ADDR, (n))
#define PEout(n) BIT_ADDR(GPIOE_ODR_ADDR, (n))
#define PFout(n) BIT_ADDR(GPIOF_ODR_ADDR, (n))

#define PAin(n) BIT_ADDR(GPIOA_IDR_ADDR, (n))
#define PBin(n) BIT_ADDR(GPIOB_IDR_ADDR, (n))
#define PCin(n) BIT_ADDR(GPIOC_IDR_ADDR, (n))
#define PDin(n) BIT_ADDR(GPIOD_IDR_ADDR, (n))
#define PEin(n) BIT_ADDR(GPIOE_IDR_ADDR, (n))
#define PFin(n) BIT_ADDR(GPIOF_IDR_ADDR, (n))

#endif
