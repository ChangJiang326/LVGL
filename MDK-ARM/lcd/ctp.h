#ifndef __CTP_H
#define __CTP_H

#include "sys.h"

#define TOUCH_NC    0
#define TOUCH_DOWN  1
#define TOUCH_UP    2

#define GT_CMD_WR   0xBA
#define GT_CMD_RD   0xBB

#define GT_CTRL_REG  0x8040
#define GT_CFGS_REG  0x8047
#define GT_CHECK_REG 0x80FF
#define GT_PID_REG   0x8140
#define GT_GSTID_REG 0x814E
#define GT_TPINFO_REG 0x814F
#define GT_TP1_REG   0x8150
#define GT_TP2_REG   0x8158
#define GT_TP3_REG   0x8160
#define GT_TP4_REG   0x8168
#define GT_TP5_REG   0x8170

extern uchar Touch_State;
extern volatile u16 X0;
extern volatile u16 Y0;
extern volatile u16 X_S;
extern volatile u16 Y_S;
extern volatile uint8_t ctp_hw_i2c_ready;
extern volatile uint8_t ctp_hw_i2c_addr;
extern volatile uint8_t ctp_hw_i2c_proto;
extern volatile uint8_t ctp_hw_i2c_last_status;
extern volatile uint32_t ctp_hw_i2c_error;
extern volatile uint32_t ctp_hw_i2c_recover_count;
extern volatile uint32_t ctp_hw_i2c_read_fail_count;
extern volatile uint32_t ctp_hw_i2c_write_fail_count;
extern volatile uint8_t ctp_touch_point_count;
extern volatile uint8_t ctp_touch_data_ready;
extern volatile uint8_t ctp_touch_release_filter;
extern volatile uint16_t ctp_touch_raw_x;
extern volatile uint16_t ctp_touch_raw_y;
extern volatile uint32_t ctp_touch_sample_count;
extern volatile uint32_t ctp_touch_release_count;
extern volatile uint8_t ctp_id0;
extern volatile uint8_t ctp_id1;
extern volatile uint8_t ctp_id2;
extern volatile uint8_t ctp_id3;
extern volatile uint8_t ctp_gt_ctrl;
extern volatile uint8_t ctp_gt_cfg_x_l;
extern volatile uint8_t ctp_gt_cfg_x_h;
extern volatile uint8_t ctp_gt_cfg_y_l;
extern volatile uint8_t ctp_gt_cfg_y_h;
extern volatile uint8_t ctp_gt_raw_814e;
extern volatile uint8_t ctp_gt_raw_814f;
extern volatile uint8_t ctp_gt_raw_8150;

void CTP_Init(void);
char Readtouch_CTP(void);
char Readtouch_CTP_GT(void);
char Readtouch_CTP_FT(void);
void GT9147_RD_Reg(u16 reg, u8 *buf, u8 len);
u8 GT9147_WR_Reg(u16 reg, u8 *buf, u8 len);

#endif
