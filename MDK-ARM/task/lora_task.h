#ifndef _USART_TASK_H
#define _USART_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#include <stdint.h>

/* ==================== 数据帧参数 ==================== */

/*
 * JustFloat数据帧：
 *
 * 8个float数据，共32字节；
 * 后面跟随4字节结束符：00 00 80 7F。
 */
#define CURVE_END_0 0x00U
#define CURVE_END_1 0x00U
#define CURVE_END_2 0x80U
#define CURVE_END_3 0x7FU

#define LORA_REMOTE_FLOAT_COUNT 8U

#define CURVE_TX_MAX_FLOATS \
    LORA_REMOTE_FLOAT_COUNT

#define LORA_FRAME_SIZE \
    (LORA_REMOTE_FLOAT_COUNT * 4U + 4U)

#define LORA_UART_TX_FRAME_SIZE LORA_FRAME_SIZE

#define LORA_RX_DMA_BUFFER_SIZE \
    (LORA_FRAME_SIZE * 2U)

/* ==================== 任务和发送参数 ==================== */

/* LoRa任务运行周期：2ms */
#define LORA_TASK_PERIOD_MS 2U

/* 使用USART1 DMA收发，波特率为115200。 */
#define LORA_USE_TX_DMA 1U
#define LORA_TX_TIMEOUT_MS 20U

/* 摇杆不变化时，100ms发送一次心跳 */
#define LORA_HEARTBEAT_MS 100U

/* 摇杆数值变化达到2时立即发送 */
#define LORA_JOYSTICK_CHANGE_THRESHOLD 2.0f

/*
 * 遥控器发送端设置为1。
 * 接收端必须设置为0。
 *
 * 两个LoRa模块不能同时持续发送，
 * 否则可能发生半双工无线碰撞。
 */
#define LORA_AUTO_SEND_ENABLE 1U

/* ==================== 遥控数据结构体 ==================== */

/*
 * 所有成员必须保持为float类型。
 *
 * 结构体一共有8个float：
 * 4个摇杆值 + 4个LVGL控制值。
 */
typedef struct
{
    /* X1、Y1、X2、Y2 */
    float joystick[4];

    /* 1表示红色，2表示蓝色 */
    float selected_color;

    /* R1选中网格数量 */
    float r1_count;

    /* R2选中网格数量 */
    float r2_count;

    /* FAKE选中网格数量 */
    float fake_count;

} LoraRemoteData_t;

/* ==================== 外部变量 ==================== */

/* 本机即将发送的遥控器数据 */
extern LoraRemoteData_t lora_tx_data;

/* 最新接收到的遥控器数据 */
extern volatile LoraRemoteData_t lora_rx_data;

/* 接收到新数据后置1 */
extern volatile uint8_t lora_rx_data_valid;

/* 最新接收到的float数组 */
extern volatile float
    lora_rx_float_array[CURVE_TX_MAX_FLOATS];

/* 最新接收到的float数量 */
extern volatile uint16_t lora_rx_float_count;

/* 发送统计 */
extern volatile uint32_t lora_tx_count;
extern volatile uint32_t lora_tx_complete_count;
extern volatile uint32_t lora_tx_busy_count;
extern volatile uint32_t lora_tx_error_count;
extern volatile uint32_t lora_tx_last_complete_tick;
extern volatile uint8_t lora_tx_ready;

extern volatile uint32_t lora_tx_change_count;
extern volatile uint32_t lora_tx_heartbeat_count;
extern volatile uint32_t lora_tx_noise_ignored_count;

/* LoRa任务循环次数 */
extern volatile uint32_t lora_task_loop_count;

/* 接收统计 */
extern volatile uint32_t lora_rx_frame_count;
extern volatile uint32_t lora_rx_event_count;
extern volatile uint32_t lora_rx_error_count;
extern volatile uint32_t lora_rx_length_error_count;
extern volatile uint32_t lora_rx_overflow_count;
extern volatile uint32_t lora_rx_dma_error_count;
extern volatile uint32_t lora_rx_resync_count;
extern volatile uint32_t lora_rx_value_error_count;
extern volatile uint32_t lora_rx_last_tick;

/* ==================== 函数声明 ==================== */

/**
 * @brief 使用当前摇杆和LVGL数值更新发送数据。
 */
void Lora_UpdateLocalData(void);

/**
 * @brief 发送一个float数组。
 *
 * @param data        float数组地址。
 * @param float_count float数据数量。
 *
 * @return 1表示成功启动发送，0表示失败或者串口忙。
 */
uint8_t Lora_SendFloatArray(
    const float *data,
    uint16_t float_count);

/**
 * @brief 发送一帧完整遥控器数据。
 *
 * @param data 遥控器数据结构体地址。
 *
 * @return 1表示成功启动发送，0表示失败或者串口忙。
 */
uint8_t Lora_SendRemoteData(
    const LoraRemoteData_t *data);

/**
 * @brief FreeRTOS LoRa任务。
 */
void LORA_Task(void const *argument);

/* ==================== 使用示例 ==================== */

/*
 * 一、发送端
 *
 * 保持：
 *
 * #define LORA_AUTO_SEND_ENABLE 1U
 *
 * LORA_Task会自动读取摇杆和LVGL数据并发送。
 */

/*
 * 二、接收端
 *
 * 修改为：
 *
 * #define LORA_AUTO_SEND_ENABLE 0U
 *
 * 然后在接收端任务中读取：
 *
 * if (lora_rx_data_valid != 0U)
 * {
 *     float remote_x1;
 *     float remote_y1;
 *     float remote_color;
 *
 *     remote_x1 = lora_rx_data.joystick[0];
 *     remote_y1 = lora_rx_data.joystick[1];
 *     remote_color = lora_rx_data.selected_color;
 *
 *     lora_rx_data_valid = 0U;
 * }
 */

/*
 * 三、手动发送数据
 *
 * LoraRemoteData_t data = {0};
 *
 * data.joystick[0] = 100.0f;
 * data.joystick[1] = 200.0f;
 * data.selected_color = 1.0f;
 * data.r1_count = 3.0f;
 *
 * Lora_SendRemoteData(&data);
 */

#ifdef __cplusplus
}
#endif

#endif
