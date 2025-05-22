#include "key.h"

uint8_t key_scan(uint8_t mode) {
    static uint8_t key_up = 1;
    uint8_t keyval = 0;
    if (mode) {
        key_up = 1;
    }
    if (key_up && (KEY0 == 0 || KEY1 == 0 || WK_UP == 1)) {
        HAL_Delay(10);
        key_up = 0;
        if (KEY0 == 0) {
            keyval = KEY0_PRES;
        }
        if (KEY1 == 0) {
            keyval = KEY1_PRES;
        }
        if (WK_UP == 1) {
            keyval = WKUP_PRES;
        }
    } else if (KEY0 == 1 && KEY1 == 1 && WK_UP == 0) {
        key_up = 1;
    }
    return keyval;
}