#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

static __inline u8 SPI2_ReadWriteByte(u8 data)
{
    (void)data;
    return 0xFFU;
}

#endif
