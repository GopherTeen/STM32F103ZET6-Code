#include "tpad.h"

#include "stm32f1xx_hal.h"
#include "tim.h"

// 空载的时候(没有手按下),计数器需要的时间（这个值应该在每次开机的时候被初始化一次）
volatile uint16_t g_tpad_default_val = 0;

/**
 * @brief       复位TPAD
 *   @note      我们将TPAD按键看做是一个电容, 当手指按下/不按下时容值有变化
 *              该函数将GPIO设置成推挽输出, 然后输出0, 进行放电, 然后再设置
 *              GPIO为浮空输入, 等待外部大电阻慢慢充电
 * @param       无
 * @retval      无
 */
static void tpad_reset(void) {
    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = TPAD_Pin;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP; /* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(TPAD_GPIO_PORT, &gpio_init_struct);

    HAL_GPIO_WritePin(TPAD_GPIO_PORT, TPAD_Pin, GPIO_PIN_RESET); /* TPAD引脚输出0, 放电 */

    HAL_Delay(5); /* 等待5ms, 让电容放电完毕 */

    htim5.Instance->SR = 0; /* 清除标记 */
    htim5.Instance->CNT = 0; /* 归零 */

    gpio_init_struct.Pin = TPAD_Pin;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_NOPULL; /* 浮空 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(TPAD_GPIO_PORT, &gpio_init_struct);

    HAL_TIM_IC_Start(&htim5, TPAD_TIMX_CAP_CHY); /* 启动定时器捕获 */
}

/**
 * @brief       得到定时器捕获值
 *   @note      如果超时, 则直接返回定时器的计数值
 *              我们定义超时时间为: TPAD_ARR_MAX_VAL - 500
 * @param       无
 * @retval      捕获值/计数值（超时的情况下返回）
 */
uint16_t tpad_getVal(void) {
    tpad_reset();

    uint32_t flag = (TPAD_TIMX_CAP_CHY == TIM_CHANNEL_1) ? TIM_FLAG_CC1 : (TPAD_TIMX_CAP_CHY == TIM_CHANNEL_2) ? TIM_FLAG_CC2 : (TPAD_TIMX_CAP_CHY == TIM_CHANNEL_3) ? TIM_FLAG_CC3 : TIM_FLAG_CC4;

    while (__HAL_TIM_GET_FLAG(&htim5, flag) == RESET) { // 等待通道 CHY 捕获上升沿
        // char msg[] = "Hello\r\n";
        // HAL_UART_Transmit(&huart1, (uint8_t*) msg, sizeof(msg), HAL_MAX_DELAY);
        if (htim5.Instance->CNT > TPAD_ARR_MAX_VAL - 500) {
            return htim5.Instance->CNT; // 超时了,直接返回 CNT 的值
        }
    }

    return __HAL_TIM_GET_COMPARE(&htim5, TPAD_TIMX_CAP_CHY); // 返回捕获/比较值
}

/**
 * @brief       初始化触摸按键
 * @retval      1, 初始化成功; 0, 初始化失败;
 */
uint8_t tpad_init(void) {
    MX_TIM5_Init();

    uint16_t buf[10];
    uint16_t tmp;
    for (int i = 0; i < 10; i++) { // 连续读取 10 次
        buf[i] = tpad_getVal();
        HAL_Delay(10);
    }

    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (buf[i] > buf[j]) {
                tmp = buf[i];
                buf[i] = buf[j];
                buf[j] = tmp;
            }
        }
    }

    tmp = 0;
    for (int i = 2; i < 8; i++) { // 取中间的 6 个数据进行平均
        tmp += buf[i];
    }

    g_tpad_default_val = tmp / 6;
    // printf("g_tpad_default_val:%d\r\n", g_tpad_default_val); /* 调试的时候使用 */

    if (g_tpad_default_val > TPAD_ARR_MAX_VAL / 2) {
        return 0; /* 初始化遇到超过TPAD_ARR_MAX_VAL/2的数值,不正常! */
    }
    return 1;
}

/**
 * @brief       读取n次, 取最大值
 * @param       n       ：连续获取的次数
 * @retval      n次读数里面读到的最大读数值
 */
static uint16_t tpad_getMaxVal(uint8_t n) {
    uint16_t tmp = 0;
    uint16_t ans = 0;
    while (n--) {
        tmp = tpad_getVal();
        if (tmp > ans) {
            ans = tmp;
        }
    }
    return ans;
}

/**
 * @brief       扫描触摸按键
 * @param       mode    ：扫描模式
 *   @arg       0, 不支持连续触发(按下一次必须松开才能按下一次);
 *   @arg       1, 支持连续触发(可以一直按下)
 * @retval      0, 没有按下; 1, 有按下;
 */
uint8_t tpad_scan(uint8_t mode) {
    uint8_t res = 0;

    static uint8_t keyen = 0; // 0, 可以开始检测;  >0, 还不能开始检测
    uint8_t sample = 3; /* 默认采样次数为3次 */
    if (mode) {
        sample = 6; /* 支持连按的时候，设置采样次数为6次 */
        keyen = 0; /* 支持连按, 每次调用该函数都可以检测 */
    }

    // 大于 tpad_default_val + TPAD_GATE_VAL 有效
    if (tpad_getMaxVal(sample) > (g_tpad_default_val + TPAD_GATE_VAL)) {
        if (keyen == 0) {
            res = 1;
        }
        keyen = 3; // 至少要再过3次之后才能按键有效
    }
    if (keyen) {
        keyen--;
    }

    return res;
}
