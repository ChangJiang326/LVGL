#ifndef LORA_TASK_H
#define LORA_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define LORA_TX_FRAME_LEN      256U
#define LORA_TX_PAYLOAD_MAX    (LORA_TX_FRAME_LEN - 2U)
#define LORA_FRAME_HEAD        0xAAU
#define LORA_FRAME_TAIL        0x55U
#define LORA_TX_VALUE_MAX      (LORA_TX_PAYLOAD_MAX / 2U)
#define LORA_ARRAY_COUNT(arr)  ((uint8_t)(sizeof(arr) / sizeof((arr)[0])))
#define LORA_SEND_PERIOD_MS    1000U

extern volatile uint32_t lora_send_period_ms;
extern volatile uint8_t lora_debug_tx_frame[LORA_TX_FRAME_LEN];
extern volatile uint16_t lora_debug_tx_frame_len;
extern volatile int16_t lora_debug_values[LORA_TX_VALUE_MAX];
extern volatile uint8_t lora_debug_value_count;
extern volatile uint8_t lora_debug_payload_len;
extern volatile uint8_t lora_debug_manual_mode;
extern volatile uint8_t lora_debug_send_once;
extern volatile uint32_t lora_task_loop_count;
extern volatile uint32_t lora_send_call_count;
extern volatile uint32_t lora_dma_start_count;
extern volatile uint32_t lora_skip_disabled_count;
extern volatile uint32_t lora_skip_manual_count;
extern volatile uint32_t lora_uart_busy_count;
extern volatile uint32_t lora_dma_fail_count;
extern volatile uint32_t lora_last_uart_gstate;

uint8_t Lora_DmaSend(const uint8_t *data, uint8_t len);
uint8_t Lora_DmaSendInt16(const int16_t *data, uint8_t count);
void LORA_Task(void const *argument);

#ifdef __cplusplus
}
#endif

#endif
