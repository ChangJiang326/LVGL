#ifndef __ADC_TASK_H
#define __ADC_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "cmsis_os.h"

#define ADC_NUM 6

/* 摇杆数 */
#define JOYSTICK_CH_NUM 4

#define LEVER_CH_NUM 2

#define Q_NUM 2

/* 中值过滤波窗口长度 */
#define ADC_MEDIAN_FILTER_SIZE 5

/* GUI平滑过滤窗口长度 */
#define GUI_ADC_FILTER_SIZE 2

/*
   摇杆值:-100 ~ 100 */
extern volatile int gui_adc[JOYSTICK_CH_NUM];
extern volatile int X1;
extern volatile int Y1;
extern volatile int X2;
extern volatile int Y2;
extern volatile uint32_t joystick_snapshot_sequence;
extern volatile uint16_t lever_adc[LEVER_CH_NUM];
extern volatile int8_t lever_state[LEVER_CH_NUM];
extern volatile uint8_t BOGAN[LEVER_CH_NUM];
extern volatile uint8_t key_state[5];
extern volatile uint8_t q_state[Q_NUM];

/*
 * 一次性读取同一个采样周期的X1、Y1、X2、Y2。
 * 避免ADC任务更新到一半时，LoRa读取到混合数据。
 */
void ADC_GetJoystickSnapshot(int joystick[JOYSTICK_CH_NUM]);

/* 任务函数 */
void ADC_Task(void const *argument);



#ifdef __cplusplus
}
#endif

#endif
