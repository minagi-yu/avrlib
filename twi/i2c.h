#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdint.h>

/* I2C Speed Mode */
#define I2C_SM 100000
#define I2C_FM 400000
#define I2C_FMPLUS 1000000

void i2c_init(int khz);
uint_fast8_t i2c_write(uint8_t addr, void *data, size_t len);
uint_fast8_t i2c_read(uint8_t addr, void *data, size_t len);

#endif