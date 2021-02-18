#include <lsm303agr_reg.h>
#include <lsm303agr_CCXXXX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>

void AXY_Init(uint_least8_t _index) {
	I2C_Params i2cParams;
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_3400kHz; // !!try I2C_3400kHz
	axy_i2c = I2C_open(_index, &i2cParams);
}

void AXY_Close(void) {
	I2C_close(axy_i2c);
}

int32_t platform_i2c_write(void *handle, uint8_t Reg, uint8_t *Bufp,
		uint16_t len) {
	I2C_Transaction i2cTransaction;
	uint32_t i2c_add = (uint32_t) handle;
	uint8_t txBuffer[7]; // lsm303agr_reg.c never sends more than 6 bytes

	if (i2c_add == LSM303AGR_I2C_ADD_XL) {
		/* enable auto incremented in multiple read/write commands */
		Reg |= 0x80;
	}

	txBuffer[0] = Reg;
	memcpy(txBuffer + 1, Bufp, len);

	i2cTransaction.slaveAddress = (uint8_t) i2c_add; // ACC OR MAG
	i2cTransaction.writeBuf = txBuffer;
	i2cTransaction.writeCount = len + 1;
	i2cTransaction.readBuf = NULL;
	i2cTransaction.readCount = 0;
	I2C_transfer(axy_i2c, &i2cTransaction);

	return 0;
}

int32_t platform_i2c_read(void *handle, uint8_t Reg, uint8_t *Bufp,
		uint16_t len) {
	I2C_Transaction i2cTransaction;
	uint32_t i2c_add = (uint32_t) handle;

	if (i2c_add == LSM303AGR_I2C_ADD_XL) {
		/* enable auto incremented in multiple read/write commands */
		Reg |= 0x80;
	}

	i2cTransaction.slaveAddress = (uint8_t) i2c_add; // ACC OR MAG
	i2cTransaction.writeBuf = &Reg;
	i2cTransaction.writeCount = 1;
	i2cTransaction.readBuf = Bufp;
	i2cTransaction.readCount = len;
	I2C_transfer(axy_i2c, &i2cTransaction);

	return 0;
}
