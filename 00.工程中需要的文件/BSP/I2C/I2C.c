#include "I2C.h"

#include <stdint.h>

#include "gpio.h"

void I2C_Init(void) {
    I2C_SCL_GPIO_CLK_ENABLE();
    I2C_SDA_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = I2C_SCL_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // 推挽
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C_SCL_GPIO_PORT, &GPIO_InitStruct);

    /* SDA引脚模式设置,开漏输出,上拉, 这样就不用再设置IO方向了, 开漏输出的时候(=1),也可以读取外部信号的高低电平 */
    GPIO_InitStruct.Pin = I2C_SDA_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // 开漏
    HAL_GPIO_Init(I2C_SDA_GPIO_PORT, &GPIO_InitStruct);

    I2C_Stop(); // 停止总线所有设备
}

static void I2C_Delay(void) {
    // 延时 2us
    for (uint32_t i = 0; i < 2; i++) {
        __NOP();
    }
}

void I2C_Start(void) {
    I2C_SDA(1);
    I2C_SCL(1);
    I2C_Delay();
    I2C_SDA(0);
    I2C_Delay();
    I2C_SCL(0);
    I2C_Delay();
}

void I2C_Stop(void) {
    I2C_SDA(0);
    I2C_Delay();
    I2C_SCL(1);
    I2C_Delay();
    I2C_SDA(1);
    I2C_Delay();
}

void I2C_Send_byte(uint8_t data) {
    for (int t = 0; t < 8; t++) {
        I2C_SDA((data & 0x80) >> 7); // 发送高位
        I2C_Delay();
        I2C_SCL(1); // 拉高时钟线
        I2C_Delay();
        I2C_SCL(0); // 拉低时钟线
        data <<= 1; // 左移一位
    }
    I2C_SDA(1); // 释放数据线
}

// ack=1时，发送ack; ack=0时，发送nack
uint8_t I2C_Read_byte(uint8_t ack) {
    uint8_t receive = 0;
    for (int i = 0; i < 8; i++) {
        receive <<= 1; // 左移一位
        I2C_SCL(1); // 拉高时钟线
        I2C_Delay();
        if (I2C_READ_SDA) {
            receive |= 0x01; // 读取数据线
        }
        I2C_SCL(0); // 拉低时钟线
        I2C_Delay();
    }
    if (ack) {
        I2C_Ack(); // 发送ACK
    } else {
        I2C_NAck(); // 发送NACK
    }
    return receive;
}

// 1，接收应答失败；0，接收应答成功
uint8_t I2C_Wait_Ack(void) {
    uint8_t time = 0;
    uint8_t rack = 0;
    I2C_SDA(1); // 主机释放数据线，外部器件可以拉低 SDA 线
    I2C_Delay();
    I2C_SCL(1); // 拉高时钟线
    I2C_Delay();
    while (I2C_READ_SDA) {
        time++;
        if (time > 250) {
            I2C_Stop();
            rack = 1;
            break; // 超时，接收应答失败
        }
    }
    I2C_SCL(0); // 拉低时钟线
    I2C_Delay();
    return rack;
}

void I2C_Ack(void) {
    I2C_SDA(0); // SCL 0 -> 1 时 SDA = 0,表示应答
    I2C_Delay();
    I2C_SCL(1); // 拉高时钟线
    I2C_Delay();
    I2C_SCL(0); // 拉低时钟线
    I2C_Delay();
    I2C_SDA(1); // 释放数据线
    I2C_Delay();
}

void I2C_NAck(void) {
    I2C_SDA(1); // SCL 0 -> 1 时 SDA = 1,表示不应答
    I2C_Delay();
    I2C_SCL(1); // 拉高时钟线
    I2C_Delay();
    I2C_SCL(0); // 拉低时钟线
    I2C_Delay();
}
