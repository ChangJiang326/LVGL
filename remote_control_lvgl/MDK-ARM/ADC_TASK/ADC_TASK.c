#include "ADC_TASK.h"
#include "FreeRTOS.h"
#include "adc.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"
#include <stdlib.h>

#define KEY_NUM 4
#define KEY_DEBOUNCE_CNT 10
#define JOYSTICK_DEADZONE_ADC 20
#define LEVER_LOW_ON_ADC 1200
#define LEVER_LOW_OFF_ADC 1500
#define LEVER_HIGH_ON_ADC 2900
#define LEVER_HIGH_OFF_ADC 2600
#define ADC_DMA_IRQ_DEBUG_DISABLE 1

volatile uint8_t key_raw[KEY_NUM + 1] = {0};
volatile uint8_t key_state[KEY_NUM + 1] = {0};
volatile uint8_t key_cnt[KEY_NUM + 1] = {0};
volatile uint8_t q_raw[Q_NUM] = {0};
volatile uint8_t q_state[Q_NUM] = {0};
volatile uint8_t q_cnt[Q_NUM] = {0};

#define BOGAN_NUM LEVER_CH_NUM
volatile uint8_t BOGAN[BOGAN_NUM] = {0};
volatile uint16_t lever_adc[LEVER_CH_NUM] = {0};
volatile int8_t lever_state[LEVER_CH_NUM] = {0};
volatile uint32_t ADC_DMA_IRQ_Disabled_Debug = 0;
volatile uint32_t ADC_DMA_Start_Status_Debug = 0;
volatile uint32_t ADC_Task_Stage_Debug = 0;

typedef struct
{
    uint16_t low_on;
    uint16_t low_off;
    uint16_t high_on;
    uint16_t high_off;
} Lever_Threshold_t;

static const Lever_Threshold_t lever_threshold[LEVER_CH_NUM] =
    {
        {LEVER_LOW_ON_ADC, LEVER_LOW_OFF_ADC, LEVER_HIGH_ON_ADC, LEVER_HIGH_OFF_ADC},
        {LEVER_LOW_ON_ADC, LEVER_LOW_OFF_ADC, 3600, 3400},
};

/* --------------------映射值-------------------- */
float chushu = 1.0f;
volatile int X1 = 0;
volatile int Y1 = 0;
volatile int X2 = 0;
volatile int Y2 = 0;

typedef struct
{
    int16_t min; // 最左/最上
    int16_t mid; // 中间
    int16_t max; // 最右 最下
} Joystick_Calib_t;

static Joystick_Calib_t js_calib[JOYSTICK_CH_NUM] =
    {
        {3, 2155, 4095},   // X1: PA0 / ADC1_IN0
        {4095, 2013, 4},   // Y1: PA1 / ADC1_IN1
        {266, 2007, 3689}, // X2: PA2 / ADC1_IN2
        {3795, 2109, 356}, // Y2: PA3 / ADC1_IN3
};

/* -------------------- ???? -------------------- */

/* 原始值 */
volatile uint16_t revADC1DMA[ADC_NUM] = {0};

/* 最终摇杆值?*/
volatile int gui_adc[JOYSTICK_CH_NUM] = {0};

/* GUI均值过滤历史 */
static int gui_adc_history[JOYSTICK_CH_NUM][GUI_ADC_FILTER_SIZE] = {0};
static uint8_t gui_adc_filter_index = 0;

/* 摇杆 处理 */
typedef struct
{
    uint16_t filter[JOYSTICK_CH_NUM][ADC_MEDIAN_FILTER_SIZE]; // ??????
    int16_t raw[JOYSTICK_CH_NUM];                             // ????????ADC?
} ADC_Joystick_t;

static ADC_Joystick_t adc_js = {0};

/* --------------------内部函数-------------------- */

static void sort_u16(uint16_t *buf, uint8_t len)
{
    uint16_t temp;
    for (uint8_t i = 0; i < len - 1; i++)
    {
        for (uint8_t j = 0; j < len - 1 - i; j++)
        {
            if (buf[j] > buf[j + 1])
            {
                temp = buf[j];
                buf[j] = buf[j + 1];
                buf[j + 1] = temp;
            }
        }
    }
}
// 整数限幅
static int clamp_int(int val, int min_val, int max_val)
{
    if (val < min_val)
        return min_val;
    if (val > max_val)
        return max_val;
    return val;
}

/* 4个摇杆中值过滤 */
static void ADC_Joystick_UpdateMedian(int16_t *ch_filter)
{
    static uint8_t filter_index = 0;

    /* 1. 存入新采样 */
    for (int i = 0; i < JOYSTICK_CH_NUM; i++)
    {
        adc_js.filter[i][filter_index] = revADC1DMA[i];
    }

    /* 2. 环形索引 */
    filter_index++;
    if (filter_index >= ADC_MEDIAN_FILTER_SIZE)
    {
        filter_index = 0;
    }

    /* 3. 对每个通道排序取两个平均 */
    for (int ch = 0; ch < JOYSTICK_CH_NUM; ch++)
    {
        uint16_t temp_buf[ADC_MEDIAN_FILTER_SIZE];

        for (int k = 0; k < ADC_MEDIAN_FILTER_SIZE; k++)
        {
            temp_buf[k] = adc_js.filter[ch][k];
        }

        sort_u16(temp_buf, ADC_MEDIAN_FILTER_SIZE);

        ch_filter[ch] = (temp_buf[4] + temp_buf[5]) / 2;
    }
}

/* GUI平滑过滤 */
static void ADC_Joystick_UpdateAverage(void)
{
    for (int i = 0; i < JOYSTICK_CH_NUM; i++)
    {
        gui_adc_history[i][gui_adc_filter_index] = gui_adc[i];
    }

    gui_adc_filter_index++;
    if (gui_adc_filter_index >= GUI_ADC_FILTER_SIZE)
    {
        gui_adc_filter_index = 0;
    }

    for (int ch = 0; ch < JOYSTICK_CH_NUM; ch++)
    {
        int sum = 0;
        for (int j = 0; j < GUI_ADC_FILTER_SIZE; j++)
        {
            sum += gui_adc_history[ch][j];
        }
        gui_adc[ch] = sum / GUI_ADC_FILTER_SIZE;
    }
}
// x轴映射函数
static int8_t ADC_Lever_UpdateState(uint16_t raw, int8_t old_state, Lever_Threshold_t threshold)
{
    if (old_state < 0)
    {
        return (raw > threshold.low_off) ? 0 : -1;
    }

    if (old_state > 0)
    {
        return (raw < threshold.high_off) ? 0 : 1;
    }

    if (raw < threshold.low_on)
    {
        return -1;
    }

    if (raw > threshold.high_on)
    {
        return 1;
    }

    return 0;
}

static void ADC_Lever_Update(void)
{
    for (int i = 0; i < LEVER_CH_NUM; i++)
    {
        lever_adc[i] = revADC1DMA[JOYSTICK_CH_NUM + i];
        lever_state[i] = ADC_Lever_UpdateState(lever_adc[i], lever_state[i], lever_threshold[i]);

        BOGAN[i] = (uint8_t)(lever_state[i] + 1);
    }
}

static int Joystick_Map_X_Signed(int raw, Joystick_Calib_t calib, int deadzone)
{
    int out;

    /* ???? */
    if ((raw > (calib.mid - deadzone)) && (raw < (calib.mid + deadzone)))
    {
        return 0;
    }

    /* min -> mid : -500 -> 0 */
    if ((calib.min >= calib.mid && raw <= calib.min && raw >= calib.mid) ||
        (calib.min <= calib.mid && raw >= calib.min && raw <= calib.mid))
    {
        out = (raw - calib.min) * 500 / (calib.mid - calib.min) - 500;
        return clamp_int(out, -500, 0);
    }

    /* mid -> max : 0 -> +500 */
    if ((calib.max >= calib.mid && raw >= calib.mid && raw <= calib.max) ||
        (calib.max <= calib.mid && raw <= calib.mid && raw >= calib.max))
    {
        out = (raw - calib.mid) * 500 / (calib.max - calib.mid);
        return clamp_int(out, 0, 500);
    }

    /* ?????? */
    if ((raw - calib.min) * (raw - calib.min) < (raw - calib.max) * (raw - calib.max))
    {
        return -500;
    }
    else
    {
        return 500;
    }
}
// y轴映射函数
static int Joystick_Map_Y_Signed(int raw, Joystick_Calib_t calib, int deadzone)
{
    int out;

    /* ???? */
    if ((raw > (calib.mid - deadzone)) && (raw < (calib.mid + deadzone)))
    {
        return 0;
    }

    /* min(?) -> mid : +500 -> 0 */
    if ((calib.min >= calib.mid && raw <= calib.min && raw >= calib.mid) ||
        (calib.min <= calib.mid && raw >= calib.min && raw <= calib.mid))
    {
        out = 500 - (raw - calib.min) * 500 / (calib.mid - calib.min);
        return clamp_int(out, 0, 500);
    }

    /* mid -> max(?) : 0 -> -500 */
    if ((calib.max >= calib.mid && raw >= calib.mid && raw <= calib.max) ||
        (calib.max <= calib.mid && raw <= calib.mid && raw >= calib.max))
    {
        out = -(raw - calib.mid) * 500 / (calib.max - calib.mid);
        return clamp_int(out, -500, 0);
    }

    /* ?????? */
    if ((raw - calib.min) * (raw - calib.min) < (raw - calib.max) * (raw - calib.max))
    {
        return 500;
    }
    else
    {
        return -500;
    }
}

static int Joystick_Map3Point(int raw, Joystick_Calib_t calib, int deadzone)
{
    int out;

    /* ???? */
    if ((raw > (calib.mid - deadzone)) && (raw < (calib.mid + deadzone)))
    {
        return 500;
    }

    /* ???:min -> mid  ??? 0 -> 500 */
    if ((calib.min >= calib.mid && raw <= calib.min && raw >= calib.mid) ||
        (calib.min <= calib.mid && raw >= calib.min && raw <= calib.mid))
    {
        out = (raw - calib.min) * 500 / (calib.mid - calib.min);
        return clamp_int(out, 0, 500);
    }

    /* ???:mid -> max  ??? 500 -> 1000 */
    if ((calib.max >= calib.mid && raw >= calib.mid && raw <= calib.max) ||
        (calib.max <= calib.mid && raw <= calib.mid && raw >= calib.max))
    {
        out = 500 + (raw - calib.mid) * 500 / (calib.max - calib.mid);
        return clamp_int(out, 500, 1000);
    }
    /* ????,?????????? */
    if (abs(raw - calib.min) < abs(raw - calib.max))
    {
        return 0;
    }
    else
    {
        return 1000;
    }
}

/* -------------------- ??? -------------------- */
static void Keys_Update(void)
{
    // 读取按键
    key_raw[1] = !HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
    key_raw[2] = !HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
    key_raw[3] = !HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin);
    key_raw[4] = !HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin);

    // 消抖
    for (int i = 1; i <= KEY_NUM; i++)
    {
        if (key_raw[i] != key_state[i])
        {
            if (key_cnt[i] < KEY_DEBOUNCE_CNT)
                key_cnt[i]++;
            else
            {
                key_cnt[i] = 0;
                key_state[i] = key_raw[i]; // 按键按下为1，没按下为0
            }
        }
        else
        {
            key_cnt[i] = 0;
        }
    }
}

static void Q_Update(void)
{
    q_raw[0] = !HAL_GPIO_ReadPin(Q1_GPIO_Port, Q1_Pin);
    q_raw[1] = !HAL_GPIO_ReadPin(Q2_GPIO_Port, Q2_Pin);

    for (int i = 0; i < Q_NUM; i++)
    {
        if (q_raw[i] != q_state[i])
        {
            if (q_cnt[i] < KEY_DEBOUNCE_CNT)
                q_cnt[i]++;
            else
            {
                q_cnt[i] = 0;
                q_state[i] = q_raw[i];
            }
        }
        else
        {
            q_cnt[i] = 0;
        }
    }
}

void ADC_Task(void const *argument)
{
    static const uint32_t revLength = ADC_NUM;
    int16_t ch_filter[JOYSTICK_CH_NUM] = {0};

    ADC_Task_Stage_Debug = 1;
    ADC_DMA_Start_Status_Debug = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)revADC1DMA, revLength);
    if (ADC_DMA_Start_Status_Debug != HAL_OK)
    {
        Error_Handler();
    }
    ADC_Task_Stage_Debug = 2;

#if ADC_DMA_IRQ_DEBUG_DISABLE
    HAL_NVIC_DisableIRQ(DMA2_Stream0_IRQn);
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
    ADC_DMA_IRQ_Disabled_Debug = 1;
#endif
    ADC_Task_Stage_Debug = 3;

    for (;;)
    {
        ADC_Joystick_UpdateMedian(ch_filter);

        for (int i = 0; i < JOYSTICK_CH_NUM; i++)
        {
            adc_js.raw[i] = ch_filter[i];
        }

        gui_adc[0] = Joystick_Map_X_Signed(adc_js.raw[0], js_calib[0], JOYSTICK_DEADZONE_ADC); // X1
        gui_adc[1] = Joystick_Map_Y_Signed(adc_js.raw[1], js_calib[1], JOYSTICK_DEADZONE_ADC); // Y1
        gui_adc[2] = Joystick_Map_X_Signed(adc_js.raw[2], js_calib[2], JOYSTICK_DEADZONE_ADC); // X2
        gui_adc[3] = Joystick_Map_Y_Signed(adc_js.raw[3], js_calib[3], JOYSTICK_DEADZONE_ADC); // Y2

        ADC_Joystick_UpdateAverage();
        ADC_Lever_Update();

        X1 = gui_adc[0] + 52;
        Y1 = gui_adc[1] - 20;
        X2 = gui_adc[2] + 9;
        Y2 = gui_adc[3];

        Keys_Update();
        Q_Update();
        osDelay(2);
    }
}
