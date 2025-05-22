#ifndef KEY_KEY_H_
#define KEY_KEY_H_

#include "main.h"

#define KEY0 HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)
#define KEY1 HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define WK_UP HAL_GPIO_ReadPin(WK_UP_GPIO_Port, WK_UP_Pin)

#define KEY0_PRES 1
#define KEY1_PRES 2
#define WKUP_PRES 3

uint8_t key_scan(uint8_t mode);

#endif /* KEY_KEY_H_ */