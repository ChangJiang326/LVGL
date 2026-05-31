#include "ctp.h"
#include "i2c.h"
#include <string.h>

#define CTP_HOR_RES 1024U
#define CTP_VER_RES 600U
#define CTP_I2C_TIMEOUT_MS 20U
#define CTP_PROTO_NONE 0U
#define CTP_PROTO_GT   1U
#define CTP_PROTO_FT   2U

uchar Touch_State = TOUCH_NC;
volatile u16 X0 = 0;
volatile u16 Y0 = 0;
volatile u16 X_S = 0;
volatile u16 Y_S = 0;
uint releasex = 0;
uint releasey = 0;

volatile uint8_t ctp_hw_i2c_ready = 0;
volatile uint8_t ctp_hw_i2c_addr = 0;
volatile uint8_t ctp_hw_i2c_proto = CTP_PROTO_NONE;
volatile uint8_t ctp_hw_i2c_last_status = 0;
volatile uint32_t ctp_hw_i2c_error = 0;
volatile uint32_t ctp_hw_i2c_recover_count = 0;
volatile uint32_t ctp_hw_i2c_read_fail_count = 0;
volatile uint32_t ctp_hw_i2c_write_fail_count = 0;
volatile uint8_t ctp_touch_point_count = 0;
volatile uint8_t ctp_touch_data_ready = 0;
volatile uint8_t ctp_touch_release_filter = 0;
volatile uint16_t ctp_touch_raw_x = 0;
volatile uint16_t ctp_touch_raw_y = 0;
volatile uint32_t ctp_touch_sample_count = 0;
volatile uint32_t ctp_touch_release_count = 0;
volatile uint8_t ctp_id0 = 0;
volatile uint8_t ctp_id1 = 0;
volatile uint8_t ctp_id2 = 0;
volatile uint8_t ctp_id3 = 0;
volatile uint8_t ctp_gt_ctrl = 0;
volatile uint8_t ctp_gt_cfg_x_l = 0;
volatile uint8_t ctp_gt_cfg_x_h = 0;
volatile uint8_t ctp_gt_cfg_y_l = 0;
volatile uint8_t ctp_gt_cfg_y_h = 0;
volatile uint8_t ctp_gt_raw_814e = 0;
volatile uint8_t ctp_gt_raw_814f = 0;
volatile uint8_t ctp_gt_raw_8150 = 0;

static uint16_t gt_i2c_addr = (0x5DU << 1);
static uint8_t gt_ready = 0;

static void ctp_hw_i2c_recover_bus(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    uint8_t i;

    ctp_hw_i2c_recover_count++;
    (void)HAL_I2C_DeInit(&hi2c3);

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Pin = CLK_Pin;
    HAL_GPIO_Init(CLK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DIN_Pin;
    HAL_GPIO_Init(DIN_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(DIN_GPIO_Port, DIN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
    HAL_Delay(1);

    for(i = 0; i < 9U; i++) {
        HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
        HAL_Delay(1);
    }

    HAL_GPIO_WritePin(DIN_GPIO_Port, DIN_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(DIN_GPIO_Port, DIN_Pin, GPIO_PIN_SET);
    HAL_Delay(1);

    MX_I2C3_Init();
}

static HAL_StatusTypeDef ctp_hw_i2c_mem_read(uint16_t dev_addr, uint16_t reg, uint8_t * buf, uint16_t len)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c3, dev_addr, reg, I2C_MEMADD_SIZE_16BIT, buf, len, CTP_I2C_TIMEOUT_MS);
    if(status != HAL_OK) {
        ctp_hw_i2c_error = HAL_I2C_GetError(&hi2c3);
        ctp_hw_i2c_read_fail_count++;
        ctp_hw_i2c_recover_bus();
    }

    return status;
}

static HAL_StatusTypeDef ctp_hw_i2c_reg8_read(uint16_t dev_addr, uint8_t reg, uint8_t * buf, uint16_t len)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c3, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, CTP_I2C_TIMEOUT_MS);
    if(status != HAL_OK) {
        ctp_hw_i2c_error = HAL_I2C_GetError(&hi2c3);
        ctp_hw_i2c_read_fail_count++;
        ctp_hw_i2c_recover_bus();
    }

    return status;
}

static HAL_StatusTypeDef ctp_hw_i2c_mem_write(uint16_t dev_addr, uint16_t reg, uint8_t * buf, uint16_t len)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c3, dev_addr, reg, I2C_MEMADD_SIZE_16BIT, buf, len, CTP_I2C_TIMEOUT_MS);
    if(status != HAL_OK) {
        ctp_hw_i2c_error = HAL_I2C_GetError(&hi2c3);
        ctp_hw_i2c_write_fail_count++;
        ctp_hw_i2c_recover_bus();
    }

    return status;
}

static HAL_StatusTypeDef ctp_hw_i2c_reg8_write(uint16_t dev_addr, uint8_t reg, uint8_t * buf, uint16_t len)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c3, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, CTP_I2C_TIMEOUT_MS);
    if(status != HAL_OK) {
        ctp_hw_i2c_error = HAL_I2C_GetError(&hi2c3);
        ctp_hw_i2c_write_fail_count++;
        ctp_hw_i2c_recover_bus();
    }

    return status;
}

static uint16_t clamp_u16(uint16_t value, uint16_t max)
{
    return value > max ? max : value;
}

void CTP_Init(void)
{
    static const uint16_t addr_list[] = { (0x5DU << 1), (0x14U << 1), (0x38U << 1), (0x15U << 1) };
    uint8_t i;
    uint8_t product_id[4] = {0};
    uint8_t ft_id[4] = {0};

    gt_ready = 0;
    ctp_hw_i2c_ready = 0;
    ctp_hw_i2c_addr = 0;
    ctp_hw_i2c_proto = CTP_PROTO_NONE;
    ctp_hw_i2c_last_status = 0;

    if(HAL_I2C_GetState(&hi2c3) == HAL_I2C_STATE_RESET) {
        MX_I2C3_Init();
    }

    if(__HAL_I2C_GET_FLAG(&hi2c3, I2C_FLAG_BUSY) != RESET) {
        ctp_hw_i2c_recover_bus();
    }

    for(i = 0; i < (uint8_t)(sizeof(addr_list) / sizeof(addr_list[0])); i++) {
        if(HAL_I2C_IsDeviceReady(&hi2c3, addr_list[i], 3, CTP_I2C_TIMEOUT_MS) == HAL_OK) {
            gt_i2c_addr = addr_list[i];
            gt_ready = 1;
            ctp_hw_i2c_ready = 1;
            ctp_hw_i2c_addr = (uint8_t)(addr_list[i] >> 1);
            ctp_hw_i2c_proto = CTP_PROTO_GT;
            break;
        }
    }

    if(gt_ready != 0U) {
        uint8_t clear = 0;
        uint8_t normal = 0;
        uint8_t cfg[4] = {0};
        if(ctp_hw_i2c_mem_read(gt_i2c_addr, GT_PID_REG, product_id, sizeof(product_id)) == HAL_OK) {
            ctp_id0 = product_id[0];
            ctp_id1 = product_id[1];
            ctp_id2 = product_id[2];
            ctp_id3 = product_id[3];
            ctp_hw_i2c_proto = CTP_PROTO_GT;
            (void)ctp_hw_i2c_mem_write(gt_i2c_addr, GT_CTRL_REG, &normal, 1);
            (void)ctp_hw_i2c_mem_read(gt_i2c_addr, GT_CTRL_REG, (uint8_t *)&ctp_gt_ctrl, 1);
            if(ctp_hw_i2c_mem_read(gt_i2c_addr, GT_CFGS_REG + 1U, cfg, sizeof(cfg)) == HAL_OK) {
                ctp_gt_cfg_x_l = cfg[0];
                ctp_gt_cfg_x_h = cfg[1];
                ctp_gt_cfg_y_l = cfg[2];
                ctp_gt_cfg_y_h = cfg[3];
            }
            (void)GT9147_WR_Reg(GT_GSTID_REG, &clear, 1);
        } else if(ctp_hw_i2c_reg8_read(gt_i2c_addr, 0xA3U, ft_id, sizeof(ft_id)) == HAL_OK ||
                  ctp_hw_i2c_reg8_read(gt_i2c_addr, 0xA8U, ft_id, 1) == HAL_OK) {
            ctp_hw_i2c_proto = CTP_PROTO_FT;
            ctp_id0 = ft_id[0];
            ctp_id1 = ft_id[1];
            ctp_id2 = ft_id[2];
            ctp_id3 = ft_id[3];
        }
    } else {
        ctp_hw_i2c_error = HAL_I2C_GetError(&hi2c3);
    }
}

u8 GT9147_WR_Reg(u16 reg, u8 *buf, u8 len)
{
    if(gt_ready == 0U) {
        return 1;
    }

    return ctp_hw_i2c_mem_write(gt_i2c_addr, reg, buf, len) == HAL_OK ? 0 : 1;
}

void GT9147_RD_Reg(u16 reg, u8 *buf, u8 len)
{
    if(gt_ready == 0U || ctp_hw_i2c_mem_read(gt_i2c_addr, reg, buf, len) != HAL_OK) {
        memset(buf, 0, len);
    }
}

static char readtouch_gt(void)
{
    uint8_t status = 0;
    uint8_t point[9] = {0};
    uint8_t clear = 0;
    uint8_t points;
    uint16_t x;
    uint16_t y;

    if(gt_ready == 0U) {
        CTP_Init();
        if(gt_ready == 0U) {
            Touch_State = TOUCH_NC;
            return 0;
        }
    }

    GT9147_RD_Reg(GT_GSTID_REG, &status, 1);
    ctp_hw_i2c_last_status = status;
    ctp_gt_raw_814e = status;
    (void)ctp_hw_i2c_mem_read(gt_i2c_addr, GT_TPINFO_REG, (uint8_t *)&ctp_gt_raw_814f, 1);
    (void)ctp_hw_i2c_mem_read(gt_i2c_addr, GT_TP1_REG, (uint8_t *)&ctp_gt_raw_8150, 1);
    points = status & 0x0FU;
    ctp_touch_point_count = points;
    ctp_touch_data_ready = (status & 0x80U) != 0U ? 1U : 0U;

    if((status & 0x80U) != 0U && points > 0U && points < 6U) {
        GT9147_RD_Reg(GT_TPINFO_REG, point, sizeof(point));

        x = (uint16_t)point[1] | ((uint16_t)point[2] << 8);
        y = (uint16_t)point[3] | ((uint16_t)point[4] << 8);
        ctp_touch_raw_x = x;
        ctp_touch_raw_y = y;

        if(x >= CTP_HOR_RES || y >= CTP_VER_RES) {
            x = (uint16_t)point[2] | ((uint16_t)point[3] << 8);
            y = (uint16_t)point[4] | ((uint16_t)point[5] << 8);
            ctp_touch_raw_x = x;
            ctp_touch_raw_y = y;
        }

        X0 = clamp_u16(x, CTP_HOR_RES - 1U);
        Y0 = clamp_u16(y, CTP_VER_RES - 1U);
        X_S = X0;
        Y_S = Y0;
        releasex = X0;
        releasey = Y0;
        Touch_State = TOUCH_DOWN;
        ctp_touch_release_filter = 0;
        ctp_touch_sample_count++;
        (void)GT9147_WR_Reg(GT_GSTID_REG, &clear, 1);
        return 1;
    }

    if((status & 0x80U) != 0U) {
        (void)GT9147_WR_Reg(GT_GSTID_REG, &clear, 1);
    }

    if(Touch_State == TOUCH_DOWN) {
        if(ctp_touch_release_filter < 2U) {
            ctp_touch_release_filter++;
            return 1;
        }

        Touch_State = TOUCH_UP;
        ctp_touch_release_count++;
        return 0;
    }

    Touch_State = TOUCH_NC;

    return 0;
}

static char readtouch_ft(void)
{
    uint8_t point[6] = {0};
    uint8_t points;
    uint16_t x;
    uint16_t y;

    if(ctp_hw_i2c_reg8_read(gt_i2c_addr, 0x02U, point, sizeof(point)) != HAL_OK) {
        Touch_State = TOUCH_NC;
        return 0;
    }

    points = point[0] & 0x0FU;
    ctp_hw_i2c_last_status = point[0];
    ctp_touch_point_count = points;
    ctp_touch_data_ready = points > 0U ? 1U : 0U;

    if(points > 0U && points < 6U) {
        x = (uint16_t)(((uint16_t)(point[1] & 0x0FU) << 8) | point[2]);
        y = (uint16_t)(((uint16_t)(point[3] & 0x0FU) << 8) | point[4]);
        ctp_touch_raw_x = x;
        ctp_touch_raw_y = y;

        X0 = clamp_u16(x, CTP_HOR_RES - 1U);
        Y0 = clamp_u16(y, CTP_VER_RES - 1U);
        X_S = X0;
        Y_S = Y0;
        releasex = X0;
        releasey = Y0;
        Touch_State = TOUCH_DOWN;
        ctp_touch_release_filter = 0;
        ctp_touch_sample_count++;
        return 1;
    }

    if(Touch_State == TOUCH_DOWN) {
        if(ctp_touch_release_filter < 2U) {
            ctp_touch_release_filter++;
            return 1;
        }

        Touch_State = TOUCH_UP;
        ctp_touch_release_count++;
        return 0;
    }

    Touch_State = TOUCH_NC;
    return 0;
}

char Readtouch_CTP_GT(void)
{
    if(gt_ready == 0U) {
        CTP_Init();
        if(gt_ready == 0U) {
            Touch_State = TOUCH_NC;
            return 0;
        }
    }

    if(ctp_hw_i2c_proto == CTP_PROTO_FT) {
        return readtouch_ft();
    }

    if(readtouch_gt() != 0) {
        return 1;
    }

    if(ctp_hw_i2c_addr == 0x14U || ctp_hw_i2c_addr == 0x38U || ctp_hw_i2c_addr == 0x15U) {
        if(readtouch_ft() != 0) {
            ctp_hw_i2c_proto = CTP_PROTO_FT;
            return 1;
        }
    }

    return 0;
}

char Readtouch_CTP_FT(void)
{
    return Readtouch_CTP_GT();
}

char Readtouch_CTP(void)
{
    return Readtouch_CTP_GT();
}
