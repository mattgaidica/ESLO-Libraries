#ifndef LSM6DSOX_CCXXXX_H
#define LSM6DSOX_CCXXXX_H

#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>

uint8_t AXY_csPin;
uint8_t AXY_csPin2;
SPI_Handle spiAXY;

//typedef union {
//	int16_t i16bit[3];
//	uint8_t u8bit[6];
//} axis3bit16_t;
//
//typedef union {
//	int16_t i16bit;
//	uint8_t u8bit[2];
//} axis1bit16_t;
//
//typedef union {
//	int32_t i32bit[3];
//	uint8_t u8bit[12];
//} axis3bit32_t;

void AXY_Init(uint_least8_t _index, uint8_t _csPin, uint8_t _csPin2);

void AXY_Close(void);

int32_t write_reg(void *handle, uint8_t Reg, uint8_t *Bufp,
		uint16_t len);

int32_t read_reg(void *handle, uint8_t Reg, uint8_t *Bufp,
		uint16_t len);

#endif /* LSM6DSOX_CCXXXX_H */
