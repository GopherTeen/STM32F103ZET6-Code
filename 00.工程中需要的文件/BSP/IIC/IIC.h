#ifndef __IIC_H__
#define __IIC_H__

#include <stdint.h>

/* 引脚定义 */
#define I2C_SCL_GPIO_PORT GPIOB
#define I2C_SCL_GPIO_PIN GPIO_PIN_6
#define I2C_SCL_GPIO_CLK_ENABLE()     \
    do {                              \
        __HAL_RCC_GPIOB_CLK_ENABLE(); \
    } while (0)

#define I2C_SDA_GPIO_PORT GPIOB
#define I2C_SDA_GPIO_PIN GPIO_PIN_7
#define I2C_SDA_GPIO_CLK_ENABLE()     \
    do {                              \
        __HAL_RCC_GPIOB_CLK_ENABLE(); \
    } while (0)

/* IO 操作 */
#define I2C_SCL(x)                                                                                                                                         \
    do {                                                                                                                                                   \
        x ? HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT, I2C_SCL_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT, I2C_SCL_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0)

#define I2C_SDA(x)                                                                                                                                         \
    do {                                                                                                                                                   \
        x ? HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN, GPIO_PIN_RESET); \
    } while (0)

#define I2C_READ_SDA HAL_GPIO_ReadPin(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN)

/* IIC 操作函数 */
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NAck(void);
uint8_t I2C_Wait_Ack(void);
void I2C_Send_byte(uint8_t txd);
uint8_t I2C_Read_byte(unsigned char ack);

#endif
