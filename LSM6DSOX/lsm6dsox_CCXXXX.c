#include <lsm6dsox_reg.h>
#include <lsm6dsox_CCXXXX.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>

void AXY_Init(uint_least8_t _index, uint8_t _csPin, uint8_t _csPin2) {
	SPI_Params spiParams;
	AXY_csPin = _csPin;
	AXY_csPin2 = _csPin2;
	SPI_Params_init(&spiParams);
	spiParams.frameFormat = SPI_POL1_PHA1; // mode 3
	spiParams.bitRate = 1000000;
	spiAXY = SPI_open(_index, &spiParams);
}

void AXY_Close(void) {
	SPI_close(spiAXY);
}

int32_t write_reg(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len) {
	SPI_Transaction transaction;

	GPIO_write(AXY_csPin, GPIO_CFG_OUT_LOW);
	transaction.count = 0x01;
	transaction.rxBuf = NULL;
	transaction.txBuf = (void*) &Reg;
	SPI_transfer(handle, &transaction);

	transaction.count = len;
	transaction.rxBuf = NULL;
	transaction.txBuf = (void*) Bufp;
	SPI_transfer(handle, &transaction);

	GPIO_write(AXY_csPin, GPIO_CFG_OUT_HIGH);
	return 0x00;
}

int32_t read_reg(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len) {
	SPI_Transaction transaction;

	Reg = 0x80 | Reg; // set read bit
	GPIO_write(AXY_csPin, GPIO_CFG_OUT_LOW);
	GPIO_write(AXY_csPin2, GPIO_CFG_OUT_LOW);
	transaction.count = 0x01;
	transaction.rxBuf = NULL;
	transaction.txBuf = (void*) &Reg;
	SPI_transfer(handle, &transaction);

	transaction.count = len;
	transaction.rxBuf = Bufp;
	transaction.txBuf = NULL;
	SPI_transfer(handle, &transaction);

	GPIO_write(AXY_csPin, GPIO_CFG_OUT_HIGH);
	GPIO_write(AXY_csPin2, GPIO_CFG_OUT_HIGH);
	return 0x00;
}
