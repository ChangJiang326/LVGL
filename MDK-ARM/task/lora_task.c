#include "lora_task.h"

#include "ADC_TASK.h"
#include "main.h"
#include "usart.h"
#include "cmsis_os.h"
#include <string.h>

/*
 * 最简单用法：
 *
 * 你只改 LORA_Task() 里面的 lora_data[]。
 * 想发什么，就直接往里面填什么：
 *
 *     int16_t lora_data[] = {
 *         X1,
 *         Y1,
 *         -1,
 *         -555,
 *         2918,
 *     };
 *
 * 不用手动拆高字节、低字节。
 * Lora_DmaSendInt16() 会自动把每个 int16_t 拆成 2 个字节发送。
 *
 * 发送格式固定为：
 *     0xAA + 数据 + 0x55
 *
 * 例如：
 *     int16_t lora_data[] = {-1, -555, 2918};
 *
 * 实际发送：
 *     AA FF FF FD D5 0B 66 55
 *
 * 接收端每 2 个字节合成一个 int16_t：
 *     int16_t v = (int16_t)((payload[i] << 8) | payload[i + 1]);
 */

volatile uint8_t lora_ready = 1U;
volatile uint16_t lora_tx_seq = 0U;
volatile uint32_t lora_tx_count = 0U;
volatile uint32_t lora_rx_count = 0U;
volatile uint32_t lora_error_count = 0U;
volatile uint32_t lora_aux_timeout_count = 0U;
volatile uint32_t lora_aux_low_tx_count = 0U;
volatile uint32_t lora_last_status = HAL_OK;
volatile uint8_t lora_last_rx_byte = 0U;
volatile uint8_t lora_last_frame_len = 0U;
volatile uint8_t lora_last_checksum = 0U;
volatile uint8_t lora_aux_pin = 0U;
volatile uint8_t lora_enable_tx = 1U;
volatile uint32_t lora_send_period_ms = LORA_SEND_PERIOD_MS;
volatile uint8_t lora_debug_tx_frame[LORA_TX_FRAME_LEN] = {0U};
volatile uint16_t lora_debug_tx_frame_len = 0U;
volatile int16_t lora_debug_values[LORA_TX_VALUE_MAX] = {0};
volatile uint8_t lora_debug_value_count = 0U;
volatile uint8_t lora_debug_payload_len = 0U;
volatile uint8_t lora_debug_manual_mode = 0U;
volatile uint8_t lora_debug_send_once = 0U;
volatile uint32_t lora_task_loop_count = 0U;
volatile uint32_t lora_send_call_count = 0U;
volatile uint32_t lora_dma_start_count = 0U;
volatile uint32_t lora_skip_disabled_count = 0U;
volatile uint32_t lora_skip_manual_count = 0U;
volatile uint32_t lora_uart_busy_count = 0U;
volatile uint32_t lora_dma_fail_count = 0U;
volatile uint32_t lora_last_uart_gstate = 0U;

static uint8_t lora_tx_buf[LORA_TX_FRAME_LEN];
static uint8_t lora_value_buf[LORA_TX_PAYLOAD_MAX];

uint8_t Lora_DmaSend(const uint8_t * data, uint8_t len)
{
    HAL_StatusTypeDef status;
    uint16_t frame_len;
    uint16_t i;

    if ((data == NULL) || (len == 0U) || (len > LORA_TX_PAYLOAD_MAX))
    {
        lora_last_status = HAL_ERROR;
        lora_error_count++;
        return 0U;
    }

    if (huart1.gState != HAL_UART_STATE_READY)
    {
        lora_ready = 0U;
        lora_last_status = HAL_BUSY;
        lora_last_uart_gstate = (uint32_t)huart1.gState;
        lora_uart_busy_count++;
        return 0U;
    }

    lora_tx_buf[0] = LORA_FRAME_HEAD;
    memcpy(&lora_tx_buf[1], data, len);
    lora_tx_buf[len + 1U] = LORA_FRAME_TAIL;
    frame_len = (uint16_t)(len + 2U);

    for (i = 0U; i < frame_len; i++)
    {
        lora_debug_tx_frame[i] = lora_tx_buf[i];
    }
    lora_debug_tx_frame_len = frame_len;
    lora_debug_payload_len = len;

    lora_dma_start_count++;
    status = HAL_UART_Transmit_DMA(&huart1, lora_tx_buf, frame_len);
    lora_last_status = status;
    lora_last_uart_gstate = (uint32_t)huart1.gState;

    if (status == HAL_OK)
    {
        lora_ready = 0U;
        lora_last_frame_len = (uint8_t)frame_len;
        lora_tx_count++;
        lora_tx_seq++;
        return 1U;
    }

    lora_ready = 0U;
    lora_error_count++;
    lora_dma_fail_count++;
    return 0U;
}

/*
 * 推荐使用这个函数。
 *
 * data[] 是 int16_t 数组，所以 lora_data[] 里面可以直接写：
 *     -1, -555, 2918, X1, Y1, X2, Y2
 *
 * count 是数组元素个数，用 LORA_ARRAY_COUNT(lora_data) 自动计算。
 */
uint8_t Lora_DmaSendInt16(const int16_t * data, uint8_t count)
{
    uint8_t i;

    if ((data == NULL) || (count == 0U) || (count > LORA_TX_VALUE_MAX))
    {
        lora_last_status = HAL_ERROR;
        lora_error_count++;
        return 0U;
    }

    for (i = 0U; i < count; i++)
    {
        uint16_t value = (uint16_t)data[i];

        lora_debug_values[i] = data[i];
        lora_value_buf[i * 2U] = (uint8_t)(value >> 8);
        lora_value_buf[i * 2U + 1U] = (uint8_t)value;
    }
    lora_debug_value_count = count;

    return Lora_DmaSend(lora_value_buf, (uint8_t)(count * 2U));
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart)
{
    if (huart->Instance == USART1)
    {
        lora_ready = 1U;
    }
}

void LORA_Task(void const * argument)
{
    (void)argument;

    for (;;)
    {
        /*
         * 无脑改这里。
         *
         * 每个数都按 int16_t 发送，范围是 -32768 ~ 32767。
         * 可以直接填变量，也可以直接填数字。
         */
        int16_t lora_data[] =
        {
            (int16_t)X1,
            (int16_t)Y1,
            (int16_t)X2,
            (int16_t)Y2,
            (int16_t)BOGAN[0],
            (int16_t)BOGAN[1],
            (int16_t)key_state[1],
            (int16_t)key_state[2],
            (int16_t)key_state[3],
            (int16_t)key_state[4],
            (int16_t)q_state[0],
            (int16_t)q_state[1],

            /* 测试例子：不需要就删掉。 */
            -1,
            -555,
            2918,
        };

        lora_task_loop_count++;

        if (lora_enable_tx == 0U)
        {
            lora_skip_disabled_count++;
        }
        else if ((lora_debug_manual_mode != 0U) && (lora_debug_send_once == 0U))
        {
            lora_skip_manual_count++;
        }
        else
        {
            lora_send_call_count++;
            (void)Lora_DmaSendInt16(lora_data, LORA_ARRAY_COUNT(lora_data));
            lora_debug_send_once = 0U;
        }

        osDelay(lora_send_period_ms);
    }
}
