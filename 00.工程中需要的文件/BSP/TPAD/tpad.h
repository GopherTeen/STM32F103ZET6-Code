#ifndef TPAD_TPAD_H_
#define TPAD_TPAD_H_

#include <stdint.h>

#define TPAD_GPIO_PORT GPIOA
#define TPAD_Pin GPIO_PIN_1

#define TPAD_TIMX_CAP TIM5
#define TPAD_TIMX_CAP_CHY TIM_CHANNEL_2

/*
    触摸的门限值, 也就是必须大于 g_tpad_default_val + TPAD_GATE_VAL 才认为是有效触摸
    改大 TPAD_GATE_VAL, 可以降低灵敏度, 反之, 则可以提高灵敏度
    根据实际需求, 选择合适的 TPAD_GATE_VAL 即可
*/
#define TPAD_GATE_VAL 100
extern volatile uint16_t g_tpad_default_val; // 空载的时候(没有手按下),计数器需要的时间

// 最大的ARR值, 一般设置为定时器的ARR最大值
#define TPAD_ARR_MAX_VAL 0XFFFF

uint8_t tpad_init(void);
uint8_t tpad_scan(uint8_t mode); /* TPAD 扫描 函数 */

#endif /* TPAD_TPAD_H_ */
