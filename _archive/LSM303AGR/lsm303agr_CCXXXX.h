#ifndef LSM303AGR_CCXXXX_H
#define LSM303AGR_CCXXXX_H

#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>

I2C_Handle axy_i2c;

typedef union {
	int16_t i16bit[3];
	uint8_t u8bit[6];
} axis3bit16_t;

typedef union {
	int16_t i16bit;
	uint8_t u8bit[2];
} axis1bit16_t;

typedef union {
	int32_t i32bit[3];
	uint8_t u8bit[12];
} axis3bit32_t;

void AXY_Init(uint_least8_t _index);

void AXY_Close(void);

int32_t platform_i2c_write(void *handle, uint8_t Reg, uint8_t *Bufp,
		uint16_t len);

int32_t platform_i2c_read(void *handle, uint8_t Reg, uint8_t *Bufp,
		uint16_t len);

#endif /* LSM303AGR_CCXXXX_H */
