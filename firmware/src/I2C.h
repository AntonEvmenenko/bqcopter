#ifndef I2C_H
#define I2C_H

#include "stm32f10x_i2c.h"

void I2C_init(I2C_TypeDef* I2Cx, uint32_t I2C_ClockSpeed);
void I2C_write(uint8_t deviceAddress, uint8_t address, uint8_t val);
void I2C_read(uint8_t deviceAddress, uint8_t address, uint8_t* buffer, uint8_t buffer_size);

#endif
