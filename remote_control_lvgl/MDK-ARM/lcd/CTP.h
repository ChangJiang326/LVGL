#ifndef __CTP_H
#define __CTP_H

#include "sys.h"

#define GTP_I2C_SDA_OPEN()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define GTP_I2C_SCL_OPEN()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define GTP_I2C_PORT_OPEN() do { GTP_I2C_SDA_OPEN(); GTP_I2C_SCL_OPEN(); } while(0)
#define GTP_I2C_INT_OPEN()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define GTP_I2C_RST_OPEN()  __HAL_RCC_GPIOE_CLK_ENABLE()

#define GTP_I2C_SDA_PORT    GPIOC
#define GTP_I2C_SDA_PIN     GPIO_PIN_9
#define GTP_I2C_SCL_PORT    GPIOA
#define GTP_I2C_SCL_PIN     GPIO_PIN_8

#define GTP_RST_GPIO_PORT   GPIOE
#define GTP_RST_GPIO_PIN    GPIO_PIN_1

#define GTP_INT_GPIO_PORT   INT_GPIO_Port
#define GTP_INT_GPIO_PIN    INT_Pin

#define CT_SDA_IN() do { \
    GPIO_InitTypeDef GPIO_Initure; \
    GPIO_Initure.Pin = GTP_I2C_SDA_PIN; \
    GPIO_Initure.Mode = GPIO_MODE_INPUT; \
    GPIO_Initure.Pull = GPIO_PULLUP; \
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; \
    HAL_GPIO_Init(GTP_I2C_SDA_PORT, &GPIO_Initure); \
} while (0)

#define CT_SDA_OUT() do { \
    GPIO_InitTypeDef GPIO_Initure; \
    GPIO_Initure.Pin = GTP_I2C_SDA_PIN; \
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD; \
    GPIO_Initure.Pull = GPIO_PULLUP; \
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; \
    HAL_GPIO_Init(GTP_I2C_SDA_PORT, &GPIO_Initure); \
} while (0)

#define GT_RST         PEout(1)
#define GT_INT         PBout(12)
#define CT_IIC_SCL     PAout(8)
#define CT_IIC_SDA     PCout(9)
#define CT_READ_SDA    PCin(9)

#define GT_CMD_WR      0XBA
#define GT_CMD_RD      0XBB

#define GT_CTRL_REG    0X8040
#define GT_CFGS_REG    0X8047
#define GT_CHECK_REG   0X80FF
#define GT_PID_REG     0X8140
#define GT_GSTID_REG   0X814E
#define GT_TP1_REG     0X8150
#define GT_TP2_REG     0X8158
#define GT_TP3_REG     0X8160
#define GT_TP4_REG     0X8168
#define GT_TP5_REG     0X8170

#define TP_PRES_DOWN   0x80
#define TP_CATH_PRES   0x40

#define TOUCH_NC       0
#define TOUCH_DOWN     1
#define TOUCH_UP       2

char Readtouch_CTP(void);
char Readtouch_CTP_GT(void);
char Readtouch_CTP_FT(void);
void CTP_Prepare_Reset(void);
void CTP_Init(void);

void GT9147_RD_Reg(u16 reg, u8 *buf, u8 len);
u8 GT9147_WR_Reg(u16 reg, u8 *buf, u8 len);

void CT_IIC_Init(void);
void CT_IIC_Start(void);
void CT_IIC_Stop(void);
void CT_IIC_Send_Byte(u8 txd);
u8 CT_IIC_Read_Byte(unsigned char ack);
u8 CT_IIC_Wait_Ack(void);
void CT_IIC_Ack(void);
void CT_IIC_NAck(void);

#endif
