
#include "lora_task.h"

#include "ADC_TASK.h"
#include "cmsis_os.h"
#include "lvgl_ui.h"
#include "usart.h"

#include <string.h>

#define LORA_TX_BUFFER_SIZE LORA_UART_TX_FRAME_SIZE

/* ==================== 全局变量 ==================== */

LoraRemoteData_t lora_tx_data = {0};

volatile LoraRemoteData_t lora_rx_data = {0};
volatile uint8_t lora_rx_data_valid = 0U;

volatile float lora_rx_float_array[CURVE_TX_MAX_FLOATS] = {0.0f};
volatile uint16_t lora_rx_float_count = 0U;

/* 发送调试变量 */
volatile uint32_t lora_tx_count = 0U;
volatile uint32_t lora_tx_complete_count = 0U;
volatile uint32_t lora_tx_busy_count = 0U;
volatile uint32_t lora_tx_error_count = 0U;
volatile uint32_t lora_tx_last_complete_tick = 0U;
volatile uint8_t lora_tx_ready = 1U;

volatile uint32_t lora_tx_change_count = 0U;
volatile uint32_t lora_tx_heartbeat_count = 0U;
volatile uint32_t lora_tx_noise_ignored_count = 0U;

/* 任务调试变量 */
volatile uint32_t lora_task_loop_count = 0U;

/* 接收调试变量 */
volatile uint32_t lora_rx_frame_count = 0U;
volatile uint32_t lora_rx_event_count = 0U;
volatile uint32_t lora_rx_error_count = 0U;
volatile uint32_t lora_rx_length_error_count = 0U;
volatile uint32_t lora_rx_overflow_count = 0U;
volatile uint32_t lora_rx_dma_error_count = 0U;
volatile uint32_t lora_rx_resync_count = 0U;
volatile uint32_t lora_rx_value_error_count = 0U;
volatile uint32_t lora_rx_last_tick = 0U;

/* ==================== 静态变量 ==================== */

static uint8_t lora_tx_buffer[LORA_TX_BUFFER_SIZE];
static uint8_t lora_rx_dma_buffer[LORA_RX_DMA_BUFFER_SIZE];
static uint8_t lora_rx_frame_buffer[LORA_FRAME_SIZE];

static uint16_t lora_rx_frame_length = 0U;
static uint16_t lora_rx_dma_position = 0U;

static volatile uint8_t lora_rx_restart_pending = 0U;

static LoraRemoteData_t lora_last_sent_data = {0};
static uint32_t lora_last_send_tick = 0U;
static uint8_t lora_last_sent_valid = 0U;

/* ==================== 内部函数 ==================== */

/**
 * @brief 判断当前遥控器数据相对于上一帧是否发生有效变化。
 */
static uint8_t Lora_DataChanged(
    const LoraRemoteData_t *current,
    const LoraRemoteData_t *previous)
{
    uint8_t i;

    for (i = 0U; i < 4U; i++)
    {
        float difference =
            current->joystick[i] - previous->joystick[i];

        if ((difference >= LORA_JOYSTICK_CHANGE_THRESHOLD) ||
            (difference <= -LORA_JOYSTICK_CHANGE_THRESHOLD))
        {
            return 1U;
        }
    }

    if ((current->selected_color != previous->selected_color) ||
        (current->r1_count != previous->r1_count) ||
        (current->r2_count != previous->r2_count) ||
        (current->fake_count != previous->fake_count))
    {
        return 1U;
    }

    return 0U;
}

/**
 * @brief 判断USART1当前是否可以启动DMA发送。
 */
static uint8_t Lora_CanTransmit(void)
{
    if ((lora_tx_ready == 0U) ||
        (huart1.gState != HAL_UART_STATE_READY))
    {
        lora_tx_busy_count++;
        return 0U;
    }

    return 1U;
}

/**
 * @brief 启动USART1 DMA发送。
 */
static uint8_t Lora_StartTransmit(uint16_t length)
{
    lora_tx_ready = 0U;

    if (HAL_UART_Transmit_DMA(
            &huart1,
            lora_tx_buffer,
            length) != HAL_OK)
    {
        lora_tx_ready = 1U;
        lora_tx_error_count++;

        return 0U;
    }

    lora_tx_count++;

    return 1U;
}

/**
 * @brief 更新本机遥控器数据。
 */
void Lora_UpdateLocalData(void)
{
    int joystick[JOYSTICK_CH_NUM];

    ADC_GetJoystickSnapshot(joystick);

    lora_tx_data.joystick[0] = (float)joystick[0];
    lora_tx_data.joystick[1] = (float)joystick[1];
    lora_tx_data.joystick[2] = (float)joystick[2];
    lora_tx_data.joystick[3] = (float)joystick[3];

    lora_tx_data.selected_color =
        (float)lvgl_ui_selected_color;

    lora_tx_data.r1_count =
        (float)lvgl_ui_r1_count;

    lora_tx_data.r2_count =
        (float)lvgl_ui_r2_count;

    lora_tx_data.fake_count =
        (float)lvgl_ui_fake_count;
}

/**
 * @brief 发送一个float数组。
 *
 * 数据格式：
 * float数据 + 00 00 80 7F结束符。
 */
uint8_t Lora_SendFloatArray(
    const float *data,
    uint16_t float_count)
{
    uint16_t payload_length;

    if ((data == NULL) ||
        (float_count == 0U) ||
        (float_count > CURVE_TX_MAX_FLOATS))
    {
        return 0U;
    }

    if (Lora_CanTransmit() == 0U)
    {
        return 0U;
    }

    payload_length =
        (uint16_t)(float_count * sizeof(float));

    memcpy(
        lora_tx_buffer,
        data,
        payload_length);

    lora_tx_buffer[payload_length + 0U] = CURVE_END_0;
    lora_tx_buffer[payload_length + 1U] = CURVE_END_1;
    lora_tx_buffer[payload_length + 2U] = CURVE_END_2;
    lora_tx_buffer[payload_length + 3U] = CURVE_END_3;

    return Lora_StartTransmit(
        (uint16_t)(payload_length + 4U));
}

/**
 * @brief 发送一帧遥控器数据。
 */
uint8_t Lora_SendRemoteData(
    const LoraRemoteData_t *data)
{
    return Lora_SendFloatArray(
        (const float *)data,
        LORA_REMOTE_FLOAT_COUNT);
}

/**
 * @brief 校验并保存一帧接收到的数据。
 */
static uint8_t Lora_SaveReceivedFrame(void)
{
    LoraRemoteData_t decoded;

    uint16_t payload_length =
        (uint16_t)(lora_rx_frame_length - 4U);

    uint16_t float_count =
        (uint16_t)(payload_length / sizeof(float));

    uint16_t i;

    if ((lora_rx_frame_length != LORA_FRAME_SIZE) ||
        (payload_length !=
         (LORA_REMOTE_FLOAT_COUNT * sizeof(float))))
    {
        lora_rx_length_error_count++;
        lora_rx_error_count++;

        return 0U;
    }

    memcpy(
        &decoded,
        lora_rx_frame_buffer,
        sizeof(decoded));

    /* 检查四个摇杆数值范围 */
    for (i = 0U; i < 4U; i++)
    {
        if ((decoded.joystick[i] < -600.0f) ||
            (decoded.joystick[i] > 600.0f))
        {
            lora_rx_value_error_count++;
            lora_rx_error_count++;

            return 0U;
        }
    }

    /* 检查LVGL相关数据范围 */
    if ((decoded.selected_color < 0.0f) ||
        (decoded.selected_color > 2.0f) ||
        (decoded.r1_count < 0.0f) ||
        (decoded.r1_count > 12.0f) ||
        (decoded.r2_count < 0.0f) ||
        (decoded.r2_count > 12.0f) ||
        (decoded.fake_count < 0.0f) ||
        (decoded.fake_count > 12.0f))
    {
        lora_rx_value_error_count++;
        lora_rx_error_count++;

        return 0U;
    }

    memcpy(
        (void *)lora_rx_float_array,
        &decoded,
        sizeof(decoded));

    memcpy(
        (void *)&lora_rx_data,
        &decoded,
        sizeof(decoded));

    lora_rx_float_count = float_count;
    lora_rx_frame_count++;
    lora_rx_last_tick = HAL_GetTick();
    lora_rx_data_valid = 1U;

    return 1U;
}

/**
 * @brief 解析USART1接收到的字节流。
 */
static void Lora_ParseReceivedBytes(
    const uint8_t *data,
    uint16_t length)
{
    uint16_t i;

    for (i = 0U; i < length; i++)
    {
        lora_rx_frame_buffer[lora_rx_frame_length] =
            data[i];

        lora_rx_frame_length++;

        if (lora_rx_frame_length == LORA_FRAME_SIZE)
        {
            if ((lora_rx_frame_buffer[
                     LORA_FRAME_SIZE - 4U] == CURVE_END_0) &&
                (lora_rx_frame_buffer[
                     LORA_FRAME_SIZE - 3U] == CURVE_END_1) &&
                (lora_rx_frame_buffer[
                     LORA_FRAME_SIZE - 2U] == CURVE_END_2) &&
                (lora_rx_frame_buffer[
                     LORA_FRAME_SIZE - 1U] == CURVE_END_3) &&
                (Lora_SaveReceivedFrame() != 0U))
            {
                lora_rx_frame_length = 0U;
            }
            else
            {
                /*
                 * 当前36字节不是有效数据帧。
                 * 删除第一个字节，继续寻找下一帧。
                 */
                memmove(
                    lora_rx_frame_buffer,
                    &lora_rx_frame_buffer[1],
                    LORA_FRAME_SIZE - 1U);

                lora_rx_frame_length =
                    LORA_FRAME_SIZE - 1U;

                lora_rx_resync_count++;
            }
        }
    }
}

/**
 * @brief 启动USART1空闲中断DMA接收。
 */
static void Lora_StartReceiveDma(void)
{
    lora_rx_dma_position = 0U;

    if (HAL_UARTEx_ReceiveToIdle_DMA(
            &huart1,
            lora_rx_dma_buffer,
            sizeof(lora_rx_dma_buffer)) == HAL_OK)
    {
        /*
         * 72字节循环DMA缓存。
         * 一帧LoRa数据为36字节。
         */
    }
    else
    {
        lora_rx_dma_error_count++;
        lora_rx_error_count++;
    }
}

/* ==================== HAL回调函数 ==================== */

/**
 * @brief USART空闲中断或者DMA接收事件回调。
 */
void HAL_UARTEx_RxEventCallback(
    UART_HandleTypeDef *huart,
    uint16_t size)
{
    if (huart->Instance == USART1)
    {
        lora_rx_event_count++;

        if (size > sizeof(lora_rx_dma_buffer))
        {
            size = sizeof(lora_rx_dma_buffer);

            lora_rx_overflow_count++;
            lora_rx_error_count++;
        }

        /*
         * DMA当前位置没有发生回绕。
         */
        if (size > lora_rx_dma_position)
        {
            Lora_ParseReceivedBytes(
                &lora_rx_dma_buffer[lora_rx_dma_position],
                (uint16_t)(
                    size - lora_rx_dma_position));
        }
        /*
         * DMA当前位置发生回绕。
         */
        else if (size < lora_rx_dma_position)
        {
            Lora_ParseReceivedBytes(
                &lora_rx_dma_buffer[lora_rx_dma_position],
                (uint16_t)(
                    sizeof(lora_rx_dma_buffer) -
                    lora_rx_dma_position));

            if (size > 0U)
            {
                Lora_ParseReceivedBytes(
                    lora_rx_dma_buffer,
                    size);
            }
        }

        if (size == sizeof(lora_rx_dma_buffer))
        {
            lora_rx_dma_position = 0U;
        }
        else
        {
            lora_rx_dma_position = size;
        }
    }
}

/**
 * @brief USART错误回调。
 */
void HAL_UART_ErrorCallback(
    UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        lora_rx_dma_error_count++;
        lora_rx_error_count++;
        lora_rx_restart_pending = 1U;
    }
}

/**
 * @brief USART DMA发送完成回调。
 */
void HAL_UART_TxCpltCallback(
    UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        lora_tx_complete_count++;
        lora_tx_last_complete_tick = HAL_GetTick();
        lora_tx_ready = 1U;
    }
}

/* ==================== FreeRTOS任务 ==================== */

/**
 * @brief LoRa发送和接收任务。
 */
void LORA_Task(void const *argument)
{
    (void)argument;

    Lora_StartReceiveDma();

    for (;;)
    {
        lora_task_loop_count++;

        /*
         * 防止发送完成回调偶尔没有及时更新状态。
         */
        if ((lora_tx_ready == 0U) &&
            (huart1.gState == HAL_UART_STATE_READY))
        {
            lora_tx_ready = 1U;
        }

        /*
         * USART接收出现错误后重新启动DMA。
         */
        if ((lora_rx_restart_pending != 0U) &&
            (huart1.RxState == HAL_UART_STATE_READY))
        {
            lora_rx_restart_pending = 0U;
            Lora_StartReceiveDma();
        }

        /*
         * 更新摇杆和LVGL控制数据。
         */
        Lora_UpdateLocalData();

#if LORA_AUTO_SEND_ENABLE

        {
            uint8_t changed;
            uint8_t heartbeat;

            /*
             * 第一次必须发送。
             * 后续数据变化超过阈值时立即发送。
             */
            changed =
                (lora_last_sent_valid == 0U) ||
                (Lora_DataChanged(
                     &lora_tx_data,
                     &lora_last_sent_data) != 0U);

            /*
             * 数据不变化时，每100ms发送一次心跳。
             */
            heartbeat =
                ((uint32_t)(
                     HAL_GetTick() -
                     lora_last_send_tick) >=
                 LORA_HEARTBEAT_MS);

            /*
             * 数据发生微小变化，但是没有达到变化阈值。
             */
            if ((lora_last_sent_valid != 0U) &&
                (changed == 0U) &&
                (heartbeat == 0U) &&
                (memcmp(
                     &lora_tx_data,
                     &lora_last_sent_data,
                     sizeof(lora_tx_data)) != 0))
            {
                lora_tx_noise_ignored_count++;
            }

            /*
             * 摇杆发生有效变化或者到达心跳时间时发送。
             */
            if ((changed != 0U) ||
                (heartbeat != 0U))
            {
                if (Lora_SendRemoteData(
                        &lora_tx_data) != 0U)
                {
                    memcpy(
                        &lora_last_sent_data,
                        &lora_tx_data,
                        sizeof(lora_tx_data));

                    lora_last_sent_valid = 1U;
                    lora_last_send_tick = HAL_GetTick();

                    if (changed != 0U)
                    {
                        lora_tx_change_count++;
                    }
                    else
                    {
                        lora_tx_heartbeat_count++;
                    }
                }
            }
        }

#endif

        osDelay(LORA_TASK_PERIOD_MS);
    }
}
